
#ifndef CM_H
#define CM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**==========================================================================
@FILE_DOC

              C A L L   M A N A G E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for
  clients to interface with the Call Manager module.

Copyright (c) 2002 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/cm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/09   rm      Adding new feature defintions to have consistent value in 
                   the RPC files  
06/10/09   em      Updated meta-comments in cm_dbm_payload_copy_cb_f_type
05/27/09   aj      Adding support for updating clients with cdma local time
05/22/09   aj      Adding support for passing otasp status to clients
05/20/09   ks      Provide new API for timing advance and current
                   CDMA position info
05/06/09   aj      Added CM_PH_CMD_ERR_NO_SUBSCRIPTION_S
01/26/09   fj      Added FEATURE_MM_SUPERSET for data structures.
04/29/09   rk      Correcting cm_util APIs IN param metacomments
04/09/09   aj      Added support for IRAT measurements
04/03/09   rk      Correcting RPC meta comments.
03/31/09   mh      Added support for prl match indicator in ss info
03/26/09   ks      Added support for blocking MO/MT voice call on Data Card
03/23/09   mh      Added support for prl preferred setting in ph info
03/20/09   ks      Removing support for CM_API_DELAY_SETUPRES as 
                   FEATURE_CM_DELAY_SETUPRES functionality no more valid.
03/09/09   rm      Reverting the fix -
                   Adding FEATURE_MM_SUPERSET for some features
02/17/09   mh      Obsoleted avoid_time in cm_ph_cmd_avoid_sys()
02/13/09   np      Added HPCD Rev C
02/13/09   mh      Added support for default roaming indicator in ss info
02/12/09   mh      Added support for no SR after MAP for IS-707B
02/04/09   fj      Add cm_daylt_savings_e_type instead of include sd.h
01/26/09   fj      Move APIs from cmutil.h to here
01/23/09   rm      Adding FEATURE_MM_SUPERSET for some features
01/13/09   ks      Added support for CM_API_SPARE_BITS_IN_NW_NAMES
11/28/08   am      Fixing compilation warning due to RPC file.
12/08/08   fj      Added mode_capability and band_capability in cm_ph_info_s_type
                   removed function cm_get_mode_band_capability()
12/10/08   rn      Defined feature CM_API_CC_MOD_PARAM
12/10/08   rn      FDN checking option in call control
12/08/08   sv      Adding support for OTASP SPC Error
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
10/22/08   fj      Added HTORPC meta comments for cm_client_init() to support
                   the automatic deregistration on behalf of exited CM client.
10/23/08   jd      Added Doxygen Comments
10/21/08   ks      Update default threshold value for sir and pathloss and
                   add masks for RSSI event
10/16/08   aj      Add activation code for BC 7 D band
10/21/08   rn      metacomment correction
10/16/08   aj      Add macro CM_API_OTASP_BC18_19_SUPPORT
10/06/08   np      Added HPCD Rev B.
10/02/08   aj      Add OTASP activation support for BC 18,19
09/27/08   cl      Support active channels for QMI
09/18/08   aj   Add support for UMA-GAN feature
09/04/08   sv      Added WPS API
07/24/08   rn      Send regn reject ind to clients
07/28/08   rk      Added error code to inform CM blocked mode pref change.
07/18/08   ks      Added new info for Current Operator's Cell ID and 
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
06/26/08   ak      Fix for compiler warnings
06/16/08   rk      Adding FEATURE_MM_SUPERSET to remaining feature flags
06/11/08   fj      Added remoted API cm_get_mode_band_capability
05/23/08   aj      Adding metacomments for cm_mm_call_cmd_orig_exten
                   cm_mm_call_cmd_handover_req,cm_ph_cmd_wakeup_from_standby2,
                   cm_ph_cmd_update_cdma_capability,cm_ss_cmd_country_selected
05/22/08   rk      Added CM_CALL_EVENT_MPTY_CALL_IND in cm_call_event_e enum.
05/22/08   vk/sv   added roam only preference
05/06/08   ak      BCMCS 2.0 changes
04/10/08   rn      Added metacomment for CM_SS_MOBILITY_MGMT_MASK and others
03/13/08   ks      Define CM_API_SIGNAL_STRENGTH_INDICATION
03/10/08   rn      Added support for indicating Limited_regional to clients
01/28/07   jqi/aj  Added FEATURE_FULL_TIME_SHDR_3 support.
02/15/08   ks      Adding SIR, Pathloss Implementation
02/11/08   rn      Alpha buffer Size being increased to support SIM related
                   GCF test cases.
02/06/08   rn      Reverted the changes done for remoting cm_nc_reg_with_call_control
02/04/08   rn      Remoted CM API cm_nc_reg_with_call_control
01/25/08   rn      Added metacomments to cm_call_comp_cap_cb_f_type etc
01/24/08   jqi     Removed the metacomments for cm_mm_get_call_comp_cap.
01/24/08   ks      Display of UE signal strength indication
01/21/08   rn      Added metacomments for cm_mm_get_call_comp_cap
01/03/08   ka      Adding handset based plus code dialing feature
12/20/07   ic      Added missing meta comment to cm_m511_mode_settings_f_type
12/16/07   ks      Added cm_ph_cmd_signal_strength_delta
12/07/07   ra      Added support for IMS VCC in HOMER files
12/04/07   ic      Corrected typo in comment for CM_API_CLIENT_TYPE_PBM
11/26/07   sv      Removing Feature FEATURE_GW_CNAP_CODING_SCHEME
11/22/07   sg      Adding is_network_initiated field to cm_gw_cs_call_info_s_type.
10/03/07   rn      Added support for UE initiated dormancy
11/05/07   sg/sv   Renaming FEATURE_CM_TEMP_NAS_DEP_TBD to
                   FEATURE_GW_CNAP_CODING_SCHEME
10/29/07   rn      Corrected meta comment for cm_enc_alpha_s_type
10/15/07   sk      Fixed meta comments for cm_call_substate_type
09/25/07   dm      Added support for FEATURE_MM_SUPERSET
09/17/07   ic      Fixed location of meta comments for cm_alpha_s_type
                   Corrected format of comments for cm_enc_alpha_s_type
09/17/07   rn      Added structure cm_enc_alpha_s_type to include coding_scheme
                   and number of bytes info for CNAP string.
                   Added CM_MAX_ALPHA_TAG_BYTES for CNAP
                   Added field enc_alpha in struct cm_mm_call_info_s
09/14/07   dm      Added support for FEATURE_MM_SUPERSET
08/27/07   rm      Added new error type for WMS-CM-NAS flow. Fix for CR 121142
08/17/07   ka/dm   New field to report HS call in progress
08/08/07   cl      Add OTASP Support for ASW Band
07/25/07   sk      Added lots of Doxygen comments.
07/16/07   sk      Featurized CM_API_WLAN_ADHOC under FEATURE_WLAN_ADHOC.
06/27/07   cl      Add new CM API to expose cmwcall_get_user_data() to RPC
06/07/07   rp      Adding support for EGPRS indication
06/06/07   pk/cl   Added support for LN status notification from HDR
05/15/07   rp      Adding gw_ecio
04/30/07   jqi     Added EMPA support for MT DBM over HDR.
04/23/07   jqi     DOS feature rework for dual processor arch.
04/16/07   rp      Removing FEATURE_MMGPS
04/10/07   rp      Adding fields for SMS domain selection
04/03/07   sk      Added CM_PH_CMD_CLIENT_RELEASE for internal use.
03/29/07   rp      Adding RPC metacomments for 3rd part MOLR
03/27/07   rp      Adding  CM_NAS_SMS_NO_RESPONSE_FROM_NETWORK
03/22/07   ic      Added several missing ) in #define CM_BAND_PREF_*
03/09/07   sk      RAM usage reduction.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
                   Fixed the meta comments.
02/23/07   ic      In cm_call_substate_u added to meta comments handling for
                   CM_CALL_STATE_ORIG
02/17/07   rp      Added FEATURE_WMS_CM_NAS_INTERFACES and related datastructures
02/09/07   pk      Added checks for CDMA subscriptions for GSM builds, for PD
                   sessions
02/02/07   pk      Added CM_IP_CALL_END_EMERG_ORIG
01/22/07   rp      Support for copying result from cc to call_info_ptr
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
01/08/07   ic      Removed FEATURE_GSTK from cm_gw_cs_orig_params_s_type
12/28/06   ka      Adding persistent enum to acq order, band, roam, hyb pref
                   and srv domain enums.
12/19/06   ka      Adding new enum for UCS2 Data coding scheme.
12/14/06   ic      FEATURE_HDR_EMPA tune up, restoring original fields in
                   cm_dbm_info_s_type and cm_dbm_data_block_s_type
11/21/06   ka      Adding rab_id_present field to cm_gw_cs_call_info_s_type.
11/20/06   ic      Mainlined FEATURE_HDR_EMPA
11/17/06   jqi     Added support for HDR active protocol revision information.
                   Added CDMA connect Oder Ack support.
11/16/06   ic      Added support for FEATURE_HDR_EMPA
                   Clients to check for CM_API_FEATURE_HDR_EMPA compile flag
                   before using this functionality
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
11/07/06   sk      Memory reduction effort.
11/06/06   hn/ic   Under FEATURE_HTORPC_METACOMMENTS added meta comments for
                   cm_ph_cmd_subscription_changed & cm_wlan_pref_init
10/05/06   ic      Under FEATURE_HTORPC_METACOMMENTS added meta comments for
                   cm_m51_mode_reg()
10/05/06   ic      Added CM_CLIENT_TYPE_OEMNET
09/29/06   rp/ka   Added code to initialize all the fields in sups object.
09/22/06   rp      Added support for BC15, BC16
09/13/06   rp      Added extra field to supps_info_s_type to pass unparsed L3
                   data from MN.
09/06/06   ic      Unnecessary comments clean up
08/29/06   sk      Added WLAN Adhoc support.
08/28/06   ka      Adding pdp header comp and data comp enums.
08/25/08   ka      Adding support for delaying setup response to let QTV
                   PAUSE streaming session.
08/16/06   ka/rp   Adding advice of charge info (aoc_info) to PH event struc.
08/11/06   ka      Adding metacomments for cm_mm_client_call_info_list_reg()
08/08/06   sk      Added CM_MODE_PREF_ANY_BUT_HDR_WLAN.
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
07/07/06   pk      Added cmtask_add_cmd_to_hold_q, and supporting data
                   structures, fields and functions. Added
                   CM_PD_SESSION_STATUS_DENY_CMD_ON_HOLD
07/07/06   pk      Added CM_SS_ORIG_THR_DS_SIP_FAILURE to support DCTM 2.0
07/06/06   ka/ic   Featurization fixes for PS call control
07/04/06   ka      Adding Call control Support for PS calls.
06/08/06   ic      Modified meta-comments for cm_bcmcs_cmd_flow_request()
06/05/06   ka/ic   CM clients expect number buffer to hold "+" along with
                   maximum dialed digits.
                   Increased CM_MAX_CALLED_INFO_CHARS by 1 to achieve this.
05/02/06   sk      Added RPC metacomments for cm_mm_call_cmd_modify_res().
04/17/06   ka      Adding support for HSDPA and DTM indications.
                   Metacomments for cm_change_xxx functions in cm.h#228
04/19/06   pk      Added meta comments for 7200
04/06/06   sk      Included CM_SRV_DOMAIN_PREF_NONE as a valid service domain
04/06/06   sk      Added the following fields to cm_mm_ss_info_s_type:
                   - hybr_hdr_bcmcs_srv_status
                   - ss_main_bcmcs_srv_status
04/01/06   ka      Changes include emerg serv category during orig, aborting
                   manual search, returning current band with srv ind and
                   increasing dialed string to 80 chars.
03/24/06   ka      Adding VideoTelephony fallback to voice feature for UMTS.
03/19/06   jqi     Added CM_API_BAND_PREF_NO_CHANGE_ANY_MAX_UPDATE .
03/14/06   pk      Added support for high PSIST
03/13/06   ka      Adding metacomments to cm_end_params_u.
03/13/06   sk      Added two new client types:
                   - CM_CLIENT_TYPE_GSDI
                   - CM_CLIENT_TYPE_BREW_APPS
03/10/06   pk      Added CM_SS_MCC_MASK and CM_SS_IMSI_11_12_MASK
03/09/06   pk      Added support for Power Collapse
03/05/06   jqi     Merged the WCDMA BC3,4,8,9 support.
02/17/06   ka      Adding support for videoshare
02/10/05   ka      Adding new substates to call incoming state.
02/02/06   pk      Added CM_API_CLIENT_TYPE_PBM
01/09/06   ic      Undid CM API changes from lint clean up for cm_num_init()
                   and cm_num_copy()
01/09/06   ic      Under FEATURE_HTORPC_METACOMMENTS, merged in meta comments
                   from //depot/asic/msm7500/orphans/00.04/services/cm/cm.h#7
                   (delta from 6 to 7)
                   Changed declaration of cm_dbm_payload_copy_cb_f_type so it
                   can be remoted for RPC
01/06/06   ka      Adding Release 5 changes including emerg call list,
                   traffic flow template for PDP modify and protocol config
                   options for some of the GW PS call reports.
01/06/06   ic      Lint cleanup.
                   Under FEATURE_HTORPC_METACOMMENTS, added meta comments for
                   cm_end_params_u_type
                   CASE CM_CALL_MODE_INFO_IP cm_end_params_u.ip_cause
01/05/06   pk      Added numbering to all the enums
12/13/05   ic      Fixed compiler error "..\..\services\cm\cmcall.c",
                   Error: C3028E: <arg 7 to 'cm_nc_call_control_processing'>:
                   implicit cast of pointer to non-equal pointer
11/30/05   ka      Adding minimum_qos to pdp modify request.
12/03/05   ic      Added CM_CALL_INCOM_SUBST_ANSWERED
                   to cm_call_incom_subst_e_type
11/30/05   ka      Adding new network selection mode pref for HPLMN search.
11/23/05   ka      Modification to allow hold/active for ip calls.
11/18/05   pk      Added support for BC 5 and BC 11 sub blocks I,J,K
11/18/05   ic      Added CM_CLIENT_TYPE_OEMTELEPHONE, clients to check for
                   compile flag CM_API_CLIENT_TYPE_OEMTELEPHONE
11/07/05   ic      Added CM_CLIENT_TYPE_MFLO, clients to check for compile
                   flag CM_API_CLIENT_TYPE_MFLO
10/27/05   pk      Removed featurization from cm_call_evetn_e_type and
                   cm_ss_event_e_type
                   Added CM_SS_EVENT_ORIG_THR_TBL_UPDATE and
                   cm_ss_orig_thr_info_s_type.
                   Added throttle table information and uptime to
                   cm_mm_ss_info_s_type
10/20/05   pk      Added CM_MODE_PREF_INTERSECT_OR_FORCE and
                   CM_SRV_TYPE_NON_AUTOMATIC
10/20/05   pk      Added support for CM_CALL_END_ACCESS_BLOCK_ALL
10/18/05   pk      Added CM_PD_SESSION_STATUS_HOLD_FORCE_MODE to
                   cm_pd_session_status_e_type
10/14/05   ic      Removed extra comma from cm_nas_error_e_type
09/15/05   pk      Added flags CM_API_PD_SESSION, CM_API_CLIENT_TYPE_DS_BCMCS
09/13/05   pk      Added CM_CLIENT_TYPE_DS_BCMCS client type
09/12/05   pk      Added support for GPS session, added session type
                   CM_PD_SESSION_TYPE_WITH_GPS_NORMAL
                   CM_PD_SESSION_TYPE_WITH_GPS_DMOD
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/23/05   ic      FEATURE_HTORPC_METACOMMENTS changes - removed declaration
                   of cm_call_id_is_allocated() as BREW / BT no longer use it
                   (CRs 71154 and 63846)
08/19/05   sk      Completely removed is_jcdma_emergency flag.
08/15/05   pk      Added cm_ph_state_e_type, this indicates the current state
                   of the phone. Currently we support Normal and emergency
                   call back mode. Added ph_state field to
                   cm_ph_info_s_type
07/29/05   dk      Added a call failure reason when call is on hold because
                   of other activity in progress.
07/25/05   sk      Added function prototypes for:
                   - cm_util_ussd_pack()
                   - cm_util_ussd_unpack()
                   - cm_util_ascii_to_gsm_alphabet()
                   - cm_util_gsm_alphabet_to_ascii()
                   Added comments to old API indicating that the new one
                   should be used instead.
07/21/05   sk      Removed internal use of is_jcdma_emergency flag.
07/15/05   pk      Added CM_API_ORIG_THR_PKT_ZONE feature, to differentiate
                   between the old call throttle API without packet_zone
                   support and the new call throttle API with packet_zone
                   support
07/11/05   ic      Externalized cm_util_pack() / cm_util_unpack()
07/11/05   pk      Modified cm_ss_cmd_orig_thr_update_tbl() API, added
                   packet_zone as one more parameter.
07/08/05   ic      Report highest layer call origination failure as part of
                   call events
07/08/05   ic      Added misc utility conversion functions (moved from
                   cmutil.h)
06/30/05   pk      Removed feature flags from
                   CM_PH_EVENT_TERMINATE_GET_NETWORKS
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/23/05   ic      Added ULL suffix to CM_BAND_PREF_ANY (compiler warning fix)
06/07/05   ic      Added meta comments for cm_get_cur_hybr_status()
06/06/05   pk      Added CM_CALL_END_ORIG_THR to call end reason enum
                   Added CM_SS_ORIG_THR_STATUS_MASK
                   Added CM_SS_HDR_SYS_ID_MASK
                   Added CM_SS_ORIG_THR_CMD_UPDATE_TABLE and
                   CM_SS_ORIG_THR_CMD_CALL_STATUS to cm_ss_cmd_e_type
                   Added cm_ss_orig_thr_reasons_e_type
                   Added ss_thr_status and hdr_sys_id to cm_mm_ss_info_s_type
                   The above changes are required to implement the Data
                   Throttle Manager API's
06/02/05   ic      Mainlined FEATURE_36_DIGIT_DIAL_STRING
05/25/05   ic      Formatting and comment changes to cm_ph_cmd_err_e_type
                   Removed references to FEATURE_GPSONE_DBM as it was already
                   mainlined
05/19/05   ka      Adding support to inform clients about call ciphering.
05/12/05   ic      Added meta comments under FEATURE_HTORPC_METACOMMENTS
                   Removed declaration of cmph_is_ph_in_emerg_cb_mode()
05/11/05   ic      Externalized cm_ph_net_list_copy()
04/13/05   ic      Put RPC comments under FEATURE_HTORPC_METACOMMENTS
04/11/05   ka      Restoring #define CM_API_WLAN and differentiating between
                   CM_API_IP_CALL and CM_API_WLAN
04/07/05   ic      Added #define CM_API_IS_JCDMA_EMERGENCY
04/07/05   ic      Added is_jcdma_emergency to cm_cdma_call_info_s_type so
                   it's sent to clients with call events
04/06/05   ka      Change in #define name
03/31/05   ic      Corrected comment for CM_CALL_EVENT_END_REQ
03/24/05   ic      Removed unnecessary featurization from cm_call_cmd_e_type
03/21/05   dk      Added CM_API_BAND_PREF_64_BIT define type.
03/17/05   ic      Fixed typo in cm_ussd_dcs_e_type, added CM_WAKEUP_MAX,
                   fixed typo in cm_nas_error_e_type
03/16/05   ic      Include cmwmsg.h in cm.h directly instead of indirectly
                   (through sys_gw.h) as cm.h uses MAXNO_FORWARD_TO_NUMBER
                   that is defined in cmwmsg.h
03/14/05   ka      Adding call end status enums and new answer struct for ip
                   call.
03/09/05   pk      Added CM_PH_EVENT_DDTM_STATUS event to indicate the change
                   of DDTM status.
03/09/05   dk      Added comments for band_pref.
03/07/05   ic      Removed unnecessary comas
03/04/05   ic      Merged in DataOverSignaling (DOS) support
02/17/05   pk      Added cm_is_sys_mode_allowed() to verify the compatiblity
                   of the mode_pref, band_pref and sys_mode
02/17/05   ic      Removed extra , after CM_ALS_LINE_SWITCHING_MAX
02/17/05   ic      Added new set of Meta-comments
02/14/05   pk      Added CM_CALL_EVENT_LINE_CTRL
02/09/10   dk      Expanded cm_band_pref to 64 bit.
02/03/05   ic      Added support for FEATURE_UMTS_BMC
02/02/05   ka      commented CM_BAND_PREF_WLAN_2400_ETS
02/01/05   dk      Added WLAN Support.
01/17/05   dk      Added support to read unpacked User PLMN Lists.
01/13/05   ka      Correcting signature of  cm_call_event_user_data_f_type
01/12/05   ka      Adding macros and enums for ussd data coding scheme
01/10/05   sj      Added new call end reasons.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
01/04/05   sj      Corrected base lat/long info in BS status response.
12/29/04   dk      Added VideoTelephony call interface.
12/19/04   ic      Replaced CM_CALL_CONV_SUBSTATE_MAX by CM_CALL_CONV_SUBST_MAX
12/05/04   ic      Added cur_ddtm_status to cm_ph_info_s_type
12/03/04   ws      Support for Remote WMS.
12/02/04   dk      Added call_subst in cm_call_state_info_s_type.
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
                   Added CM_CALL_CMD_ERR_USER_DATA_LENGTH_P error code to
                   cm_call_cmd_err_e_type
11/19/04   ka      Adding support for user to user signaling
11/18/04   ka      Adding support for MT PDP feature
11/16/04   sj      Changed comments for avoid type names.
10/20/04   sj      Include is_colloc in CM SS event.
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/14/04   ic      Call direction clean up - removed direction field from
                   cm_gw_cs_call_info_s_type
09/13/04   ka      Added new sups cmd error for call control reject.
09/10/04   jqi     Any systerm preference change or operation mode change
                   should terminate a pending get network request.
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
08/30/04   ic      Fixed ADS 1.2 compiler error "line 4146: Error: C2839E: junk
                   at end of #endif line - ignored" (digit 2 following comment)
08/27/04   jqi     Added ps dormant optimization.
08/26/04   sbs     Added externalAddress and nameString IEs to MOLR_Invoke /
                   LocationNotification SS messages
08/23/04   ws      Updated RPC Meta Comments.
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/19/04   dk      Changed CM_CALL_END_HDR_HOLD_DBM_IN_PROG to
                   CM_CALL_END_HOLD_DBM_IN_PROG.
08/18/04   ka      Adding direction as a parameter to call object
08/17/04   ic      Undoing P4 change list #121638
08/12/04   ws      Remove cm20 support.
07/28/04   ka      Added support for secondary PDPcontext
07/27/04   ws      Call Control support for SUPS.
07/21/04   sj      Added support for umts to 1x handover.
07/15/04   sj      Added new end_status = no_full_srv.
07/14/04   ic      Added extern declaration to cm_ph_cmd_line_switching() and
                   cm_ph_cmd_selected_line()
07/14/04   ws      Add requesting_client_id to event info structs.
07/12/04   ic      Added support for FEATURE_ALS
07/08/04   ws      Add RPC meta comments.
07/01/04   sj      Added new call end status for redirection or handoff.
06/30/04   ws      Include callback for cm_pd_cmd_reva_mo_msg()
06/25/04   dk      Added CM_CALL_CMD_ERR_CONNECTION_ID_OUT_L error code.
06/21/04   ic      Corrected comment in cm_ss_event_e_type
06/21/04   ank     Mainlined FEATURE_GPSONE_DBM
06/16/04   ic      Added CM_CALL_END_HDR_RELEASED_BY_CM used when HDR call is
                   released by CM to let voice call through.
                   Removed CM_SMS_CMD_ERR_NOT_ALLOWED_IN_HDR_S as it became
                   obsolete
06/03/04   ws      Add support for redirecting number.
06/02/04   ws      Added camera as a client type.
06/01/04   npr     Added IP Header and Data compression requirements to GW PS
                   call origination and information structures for GPRS SNDCP.
05/28/04   ic      Defined CM_SS_P_REV_IN_USE_SVD_SUPPORTED and
                   CM_SS_P_REV_IN_USE_SVD_NOT_SUPPORTED
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/30/04   ws      Added enums for num type and num plan.
04/29/04   ic      cm_mode_pref_e_type clean up.
                   Removed CM_MODE_PREF_ANY_BUT_HDR_AND_AMPS as it's identical
                   to CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY.
                   Added CM_MODE_PREF_CURRENT_LESS_AMPS
04/19/04   ic      Added support for HDR Ec/Io & Io reporting.
                   HDR Ec/Io & Io will be reported with CM_SS_EVENT_RSSI &
                   CM_SS_EVENT_HDR_RSSI events.
04/15/04   ws      Added CM_MODE_PREF_CDMA_AMPS_HDR_ONLY.
04/13/04   ic      Featurization changes related to 6500 5.0 CUST files being
                   used
04/08/04   ws      Added CM_MODE_PREF_CDMA_HDR_ONLY.
03/29/04   ic      Added support for FEATURE_36_DIGIT_DIAL_STRING
03/17/04   ic      Fixes to comment indentation in file header.
03/17/04   sj      Updated the DDTM api with new parameters.
03/10/04   ic      FEATURE_BCMCS - removed parameter check error
                   CM_BCMCS_CMD_ERR_INVALID_PORT_NUMBER_P
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/12/04   ic      Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/06/04   ws      Initial Jaguar/Mainline merge.
01/20/04   ka      Added support for Muliple PDP and call info list api
01/13/04   ws      Added new end status when data calls are ended for e911 calls.
01/08/04   sbs     Added missing semicolon after cm_sups_cmd_lcs_molr_completed
                   function prototype
11/20/03   ic      Added support for:
                   CM Commands:
                   - CM_PD_CMD_FIX_START
                   - CM_PD_CMD_FIX_END
                   CM Events:
                   - CM_PD_EVENT_FIX_START
                   - CM_PD_EVENT_FIX_END
                   Log events:
                   - EVENT_CM_BLOCK_HDR_ORIG_DURING_GPS
                   - EVENT_CM_ALLOW_HDR_ORIG_DURING_GPS
11/19/03   sbs     Added LCS MOLR Completed API function.
11/14/03   ws      Added support for is_jcdma_emergency
10/27/03   ws      Removed uim_pref_slot_selection support.
10/23/03   ws      jqi's changes for band preference enum.
10/20/03   ws      Dual Slot Support.
                   Fixed misspelled available with several enums.
                   Added CM_SS_EVENT_SRV_NEW for FEATURE_GPSONE_BS_INFO.
10/07/03   ic      Added CM_CALL_CMD_ERR_MODE_PREF_P to cm_call_cmd_err_e_type.
                   Added CM_CALL_END_INTERNAL_ERROR to cm_call_end_e_type.
                   Added CM_MODE_PREF_ANY_BUT_HDR_AND_AMPS and
                   CM_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS to cm_mode_pref_e_type.
09/30/03   vt      Modified to support common cm.h across all targets.
09/22/03   AT      Moved AOC functionality to this file.
09/12/03   prk     Added CM_PH_EVENT_PS_ATTACH_FAILED event to cm_ph_event_e_type.
09/23/03   ws      Added cm_get_current_acq_sys_mode
09/22/03   vt      Added bluetooth client id.
09/12/03   vt      Moved hybrid instance parametrs from cdma structure to
                   common for serving system.
09/05/03   ic      Added CM_CALL_CMD_ERR_FAVORED_MODE_PREF_FOR_ORIGINATION to
                   cm_call_cmd_err_e_type enumeration.
                   Changes to cm_mode_pref_e_type enumeration - un-did changes
                   from rev#7 so that new enums are added at the end to not
                   break compatibility with test tools.
09/16/03   vt      Added preferred slot handling.
09/16/03   ws      Add support for uz_id and uz_name in ss
08/27/03   ic      Added CM_SRV_TYPE_WCDMA and CM_SRV_TYPE_GSM to
                   cm_srv_type_e_type.
                   Put drs_bit field in cm_cdma_orig_params_s_type under
                   FEATURE_IS2000 to keep consistent with other structures.
                   Added CM_MODE_PREF_GSM_WCDMA_ONLY,
                   CM_MODE_PREF_ANY_BUT_HDR_AND_AMPS and CM_MODE_PREF_ALL_BUT_HDR
                   to cm_mode_pref_e_type.
08/22/03   vt      Mainlined packet state handling.
08/21/03   ws      Support for MM silent redial and automatic mode.
08/14/03   vt      Added DDTM.
08/13/03   prk     Added minimum_qos field to PS call info structures.
08/01/03   az      Added support to return overflow size for MO SDB.
07/17/03   jqi     Added more band pref enum types.
07/16/03   ic      Added CM_CLIENT_TYPE_BREW to cm_client_type_e_type
07/09/03   prk     Added CM_CALL_CMD_ERR_INVALID_SIM_STATE_S to
                   cm_call_cmd_err_e_type.
07/08/03   sbs/lt  Added support for Location Services related SS messages
07/08/03   vt      Added sys mode to call info structure.
07/02/03   vt      Updated HDR related service indicators.
06/30/03   vt      Added a new call end status.
06/19/03   ws      Changed band pref bitmask to 16 bit enum
06/04/03   wli     Added RTRE control item for SIM access.
05/28/03   vt      Added more service types. Defined roam & band as bitmasks.
05/15/03   vt      Added a function to return current service for PS call.
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
05/02/03   prk     Changed CM_MAX_SUBADDRESS_DIGITS from 20 to 21 to be
                   compliant with GSM specs.
04/29/03   sun     Added cc_reject to cm_gw_cs_call_info_s_type
04/28/03   AT      Lint cleanup.
04/21/03   vt      Added handling of SIM access for RTRE.
04/14/03   prk     Removed references to FEATURE_SUPS and FEATURE_PLMN.
04/14/03   ws      Added BSID changes.
04/05/03   ws      Modified #def for flash type emergency to sups type emergency
03/11/03   ATT     Added support for call control via clients.
03/11/03   ws      Changed CALL_ID_MAX to 8 except when CM20 Interface is on
02/28/03   vt      Added the interface to block/unblock HDR data call
                   origination.
02/28/03   AT      Added to error reason for call control rejecting call.
02/27/03   prk     Added support for CM_CLIENT_TYPE_GSTK.
02/27/02   cs      Added MT/MO indication and transaction ID in CS GSM/WCDMA
                   call info for USAT support.
                   Added cm_mm_call_cmd_ps_data_available API function.
                   Added ps_data_suspend field in cm_mm_ss_info_s_type to indicate
                   that the service has been suspended. Added CM_SS_PS_DATA_SUSPEND_MASK.
02/25/03   RI      Added CM_SRV_DOMAIN_PREF_PS_ATTACH and
                   CM_SRV_DOMAIN_PREF_PS_DETACH in cm_srv_domain_pref_e_type.
02/25/03   vt      Set some enums to fixed values for event logging.
02/19/03   ATM     Added DIAG client
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/10/03   at      Added support for CAT_APP
02/06/03   vt      Removed answer_duration field from cm_ph_info_s_type.
02/06/03   ri      Added support for concurrent services.
                   Modified cm_mm_call_cmd_end().
02/03/03   jqi     Added support for band class 6.
01/21/03   ic      Added answer_duration field to cm_ph_info_s_type structure
01/21/03   ws      Updated copyright information for 2003
01/21/03   ws      Added #define for mapping PKT_DATA to PS_DATA for call types.
12/17/02   vt      Defined fixed numbers to several enums for event logging.
12/11/02   vt      Added more comments.
11/27/02   vt      Added comments.
11/22/02   PRK     Added support to process user preferred PLMN list.
11/19/02   vt      Added changing RTRE configuration.
11/14/02   vt      Merged changes from GW mainline.
                   [AT]Added additional support for No funds in AOC.
                   [YR]Added test control type for Test Control test.
11/13/02   ic      More changes to monitor object interface - added
                   CM_MONIT_CMD_CLIENT_ACT command
11/13/02   ic      Changes to monitor object interface.
10/31/02   vt      Removed handling of NDSS origination failure.
                   Mainlined feature SD20.
10/30/02   vt      Undid - NDSS origination failure change.
10/28/02   vt      Removed handling of NDSS origination failure.
10/23/02   vt      Moved M* menu functions into utility section.
10/22/02   ic      Added M51 mode settings object
10/05/02   vt      Added forbidden system to ss mask.
09/30/02   RI      Added CM_CALL_EVENT_ORIG_FWD_STATUS,
                   CM_CALL_EVENT_CALL_FORWARDED,
                   CM_CALL_EVENT_CALL_BEING_FORWARDED,
                   CM_CALL_EVENT_INCOM_FWD_CALL for Call Forwarding SS.
                   Added ss_info field in cm_gw_cs_call_info_s_type.
09/11/02   ws      Added initial cm20 backward compatibility.
09/11/02   RI      Added redirect_party_number field in cm_gw_cs_call_info_s_type
09/05/02   vt      Added comments to indicate what FEATURE's must be
                   defined to access certain functions.
08/30/02   vt      Removed many ifdef FEATURE's & replaced byte with uint8
08/15/02   RI      Modified the max. forwarded to number size.
08/12/02   RI      Support for ECT and CD features ( GSM/WCDMA).
08/08/02   AT      Adding in support for CUG and CNAP events.
08/07/02   vt      Set call objects to 1 if SVD is undefined.
08/06/02   HQ      Added FEATURE_CDSMS.
07/29/02   RI      Added call_progress,cause_of_no_cli fields to
07/24/02   vt      Added handling for wakeup from deep sleep(standby)
07/22/02   PRK     Added cm_ph_cmd_get_networks and cm_ph_cmd_terminate_get_
                   networks.  Added CM_PH_CMD_ERR_GET_NETWORKS_S to
                   cm_ph_cmd_err_e_type.
06/28/02   vt      Added dummy structures for cm_rab_reestab_params_s_type and
                   cm_setup_res_params_s_type.
06/11/02   RI      Added CM_CALL_CMD_ERR_CALL_ORIG_FAIL_S in
                   cm_call_cmd_err_e_type.
05/31/02   RI      Added sups_type and sups_params fields in
                   cm_gw_cs_call_info_s_type.
05/04/02   vt      Updated to make it backward compatible with CM2.0
04/25/02   RI      Added uss_data_type field to cm_sups_info_s_type.
04/24/02   RI      Added CM_CALL_EVENT_RAB_REESTAB_FAIL and
                   renamed CM_CALL_EVENT_RAB_ESTAB_IND to CM_CALL_EVENT_RAB_REESTAB_IND
                   in cm_call_event_e_type.
04/22/02   RI      Added ATCOP as CM client in cm_client_type_e_type.
03/11/02   RI      Added CM_CALL_CMD_ERR_NULL_PARAM_P,
                   CM_CALL_CMD_ERR_NO_CALL_ID_S
                   CM_SUPS_CMD_ERR_NULL_PARAM_P
                   Added ss_success field in cm_sups_info_s_type.
                   Deleted cm_sups_cmd_abort().
02/15/02   VT      Initial multimode version
===========================================================================*/


/****************************************************************************

  Organization of the file:
  -------------------------
    The file is divided into multiple sections. Each section describes a
    call manager object. You can jump from one sections to the other by
    searching for / followed by 3 *'s. The order of objects defined is as
    follows:

        Common section
        Call Object
        Phone Object
        Serving System Object
        Inband Object
        SMS Object
        Position Determination Object
        Supplementary Services Object (GSM/WCDMA)
        DBM Object
        BCMCS Object
        MBMS object 
        Client Object
        Utility functions

    Each section is further subdivided to represent different types of info.
    You can jump from one sub-sections to the other by searching for /
    followed by 2 *'s. The data in each section is organized in the
    following order.

        #defines
        enums
        macros
        datatypes
        functions

  Abbreviations Used:
  -------------------
    GW  - GSM/WCDMA
    DBM - Data Burst Message

****************************************************************************/



/****************************************************************************

Common to all Call Manager Objects

****************************************************************************/

/** @defgroup COMMON Common definitions
** @{
*/

/**------------------------------------------------------------------------
** Defines that can be checked by CM clients to detect support for a feature.
** Added for client code to co-exist between different versions of CM.
**---------------------------------------------------------------------------
*/

/** A new error type for WMS-CM-NAS interaction.
** Again CM acts like a pipe only.
*/
#define CM_API_NAS_NO_ERROR_CP_ACK

/** Add new activation code */
#define CM_API_OTASP_ACTIVATE_CODE

/** Add new activation code support for BC 18, 19
** and remove activation code for BC 7 D band. This macro
** enables backward compatibility from UI side
*/
#define CM_API_OTASP_BC18_19_SUPPORT

/** Support for FEATURE_HDR_EMPA
*/
#define CM_API_FEATURE_HDR_EMPA

#if (defined(FEATURE_CM_TECH_SPEC_SRV_IND) || \
      defined(FEATURE_MM_SUPERSET))

/** Define the CM api flag for new service indicators
*/
#define CM_TECH_SPEC_SRV_IND
#endif

/** Client type GSDI. */
#define CM_API_CLIENT_TYPE_GSDI

/** Client type BREW_APPS. */
#define CM_API_CLIENT_TYPE_BREW_APPS

/** Cleint type PBM
*/
#define CM_API_CLIENT_TYPE_PBM

/** Added several new enums
*/
#define CM_API_CM_DBM_CMD_ERR_PAYLOAD_NULL_PTR_P
#define CM_API_CM_DBM_CMD_ERR_PAYLOAD_SIZE_P
#define CM_API_CM_PD_CMD_ERR_PAYLOAD_SIZE_P
#define CM_API_CM_PD_CMD_ERR_PAYLOAD_NULL_PTR_P
#define CM_API_CM_PH_CMD_ERR_DDTM_SRV_OPTIONS_LIST_P

/** Client type CM_CLIENT_TYPE_OEMTELEPHONE
*/
#define CM_API_CLIENT_TYPE_OEMTELEPHONE

/** Client type CM_CLIENT_TYPE_MFLO
*/
#define CM_API_CLIENT_TYPE_MFLO

/** Client type DS_BCMCS
*/
#define CM_API_CLIENT_TYPE_DS_BCMCS

/** API change for the new PD session functionality
*/

#define CM_API_PD_SESSION

/** Integration of the BCMCS DS to CM to HDRMC path
*/
#define CM_API_BCMCS_DS_CM_HDRMC_PATH_INTEGRATION

/** New USSD pack/unpack functions.
*/
#define CM_API_UTIL_USSD_FUNCTIONS

/** To differentiate between the old call throttle API without packet_zone
** support and the new call throttle API with packet_zone support
*/
#define CM_API_ORIG_THR_PKT_ZONE


/** Report highest layer call origination failure as part of call events
*/
#define CM_API_HIGHEST_LAYER_CALL_ORIG_FAIL

/** Externalized misc utility functions from cmutil.c
*/
#define CM_API_MISC_UTIL_FUNCTIONS

/** Origination throttle support
*/
#define CM_API_ORIG_THR

/**
** Indicates that client needs to call on cm_ph_net_list_copy()
** to get more detailed CMPH information
** If this flag is not defined, client needs to call on net_list_copy_func
** provided as part of CMPH event info structure
*/
#define CM_API_STATIC_FUNC_FOR_EXTRA_EVENT_INFO

/** Secondary PDP support in CM can be detected by checking for this define
*/
#define CM_API_SEC_PDP

/** Support for User to user data signaling can be detected by
** checking for this define
*/
#define CM_API_USER_USER_DATA

/** Support for PRL preferences in CM can be detected by checking for this
** define.
*/
#define CM_API_SYS_SEL_PRL_PREF

/** Support for WLAN in CM can be detected by checking for this
** define.
*/
#define CM_API_WLAN

/** Support for Remote WMS.  This would require CM to handle certain NAS events
** and commands for WMS.
*/
#define CM_API_REMOTE_WMS


/** Support for VideoTelephony Call support.
*/
#define CM_API_VT

/** Support for USSD data coding scheme enums and macros
*/
#define CM_API_USSD_DCS

/** Support for 64 Bit Band Preferences.
*/
#define CM_API_BAND_PREF_64_BIT

/** Support for IP call api in CM can be detected by checking for this
** define.
*/
#define CM_API_IP_CALL

/** Support for GW ciphering indication
*/
#define CM_CIPHERING_IND_SUPP

/** Support for Call Completion to busy subscriber
*/
#define CM_API_CCBS

/** Support for IP call cause values
*/
#define CM_API_IP_CAUSE

/** Support for HPLMN search request
*/
#define CM_API_INITIATE_HPLMN_SRCH

/** Changes to PDP modify API request
*/
#define CM_API_CHG_PDP_MOD

/** Support for reporting emergency number list
*/
#define CM_API_EMERGENCY_NUM_LIST

/** Supporting additional incoming substates
*/
#define CM_API_ADD_INCOM_SUBST

/** Support for Video Share
*/
#define CM_API_VIDEOSHARE

/** Support for WCDMA900 band class (BC3,4,8,9)
*/
#define CM_API_WCDMA900

/** Support for change CM_BAND_PREF_NO_CHANGE to 0x40000000
**                    CM_BAND_PREF_ANY to 0xFFFFFFFFBFFFFFFFULL
**                    CM_BAND_PREF_MAX tp CM_BAND_PREF_ANY + 1
*/
#define CM_API_BAND_PREF_NO_CHANGE_ANY_MAX_UPDATE

/** Support for Advice of Charge info
*/
#define CM_API_AOC_INFO

/** Support for sending unparsed L3 data from MN to clients
*/
#define CM_SUPS_UNPARSED_L3_DATA


/** Api's and enums for VT fallback to Voice
** feature
*/
#define CM_API_VT_FALLBACK_TO_VOICE

/** Support for emergency service category in
** call orig and call control
*/
#define CM_API_ECATEG_IN_NUM_STRUCT

/** Support to inform client on the success/failure
** of PLMN list request
*/
#define CM_API_AVAIL_NET_LIST_CNF

/** Supports SS events for cell supported services
*/
#define CM_API_CELL_SUPP_SERVICES

/** Defines support for GW PS call control
*/
#define CM_API_GW_PS_CC

/** Support for ip call ss success/failure.
*/
#define CM_API_IPCALL_SS_INFO

/** Support for WLAN ADHOC networks
*/
#ifdef FEATURE_WLAN_ADHOC
#error code not present
#endif

/** Support for new incoming call substate when
** call is waiting on timer before
** sending setup response
*/

/** Support for WLAN WPS procedure.
*/
#define CM_API_WLAN_WPS

/** Support for enumeration on PDP header
** and data compression
*/
#define CM_API_PDP_COMPR

/** Support for BC 12, 14,15 and 16
*/
#define CM_API_BAND_CLASS_12_14_15_16


/** Support for the new SUPS default values for ss_code, ss_reference, etc
*/
#define CM_API_SUPS_DEFAULT_VALUES


/** Check for this compile flag before trying to use CM_CLIENT_TYPE_OEMNET
*/
#define CM_API_CM_CLIENT_TYPE_OEMNET

/** Support for the HDR active protocol revision
*/
#define CM_API_HDR_ACTIVE_PROT

/** Check for this compile flag before trying to use
** CM_CALL_EVENT_CONNECT_ORDER_ACK
*/
#define CM_API_CM_CALL_EVENT_CONNECT_ORDER_ACK

/** Support for rab_id for
** circuit switched calls
*/
#define CM_API_CS_RABID

/** Addition of enumeration for UCS2
*/
#define CM_API_USSD_UCS2_DCS

/** Supporting Persistent implementations for
** acq_order, band_pref, roam_pref, hybrid_pref and srv_domain_pref
*/
#define CM_API_PERSISTENT_PREF

/** Support for gw_ecio
*/
#define CM_API_GW_ECIO

/** Check for this flag before using cm_result_from_cc_s_type
*/
#define CM_API_RESULT_FROM_CC

/** Adding new enum for NAS-WMS interaction
*/
#define CM_API_NAS_CELL_CHANGE_TYPE

/** Adding support for FEATURE_SMS_OVER_IP
*/
#define CM_API_SMS_OVER_IP

/** Adding new CM API function cm_get_user_data that exposes
** cmwcall_get_user_data
*/
#define CM_API_GET_USER_DATA_FUNCTION

/** Adding enw field to report HS call status
*/
#define CM_API_CELL_SUPP_SERVICES_HS_CALL

/* Support for handset based plus dial
*/
#define CM_API_PLUS_DIALING

/* Adding new field rssi2, rscp, and ecio_for_display
** to report signal strength indication
*/
#define CM_API_SIGNAL_STRENGTH_INDICATION

/* Support for MBMS
*/
#define CM_API_MBMS

/* Adding a new field to report OTASP SPC Error
*/
#define CM_API_OTASP_SPC_ERR


/* Support for FDN call control parameters in call orig API
*/
#define CM_API_CC_MOD_PARAM

/* Added spare_bits field in the mm_information to indicate 
** no. of spare bits in end octet of n/w names(full/short)
*/
#define CM_API_SPARE_BITS_IN_NW_NAMES

/*---------------------------------------------------------------------------
** Includes
**---------------------------------------------------------------------------
*/

#include "customer.h"   /* Customer configuration file */
#include "comdef.h"     /* Definition for basic types and macros */
#include "qw.h"         /* FEATURE_JCDMA - Quad word service header file */
#include "sys.h"        /* system wide common types */
#include "sys_gw.h"     /* GSM/WCDMA specific types */
#include "net.h"        /* NET definitions */
#include "cmwmsg.h"     /* Redefines lower layer types to CM types     */


/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/
  /** CM signal for NV synchronous read by UI in CM thread context */
#define CM_UI_NV_SIG        0x0080
  /* CM signal for NV synchronous read by UI in CM thread context */

  /** Used in cm_sups_unparsed_l3_data_s_type to define the max size of the
  ** L3 message
  ** Clients need to check for CM_SUPS_UNPARSED_L3_DATA before
  ** using this macro.
  */
#define CM_MAX_SUPS_PARAMETER_LENGTH         260

   /**
  ** Alternate Line Service feature default settings (FEATURE_ALS)
  */
#define CM_ALS_DEFAULT_CPHS_SETTING           CM_CPHS_PROC_NOT_ALLOWED

   /**
  ** Alternate Line Service feature default settings (FEATURE_ALS)
  */
#define CM_ALS_DEFAULT_ALS_SETTING            CM_ALS_PROC_NOT_ALLOWED

   /**
  ** Alternate Line Service feature default settings (FEATURE_ALS)
  */
#define CM_ALS_DEFAULT_LINE_SWITCHING         CM_ALS_LINE_SWITCHING_ALLOWED

   /**
  ** Alternate Line Service feature default settings (FEATURE_ALS)
  */
#define CM_ALS_DEFAULT_LINE                   CM_ALS_LINE_01

   /**
  ** No change constant for ph cmd signal delta API
  */
#define CM_SIG_STRENGTH_DELTA_NO_CHANGE        ((uint8)-1)

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/** Enumeration of lines supported (FEATURE_ALS)
*/
typedef enum cm_als_line_e {

  CM_ALS_LINE_NONE = -1,
    /**< @internal */

  CM_ALS_LINE_01,
    /**< Line #1 */

  CM_ALS_LINE_02,
    /**< Line #2 */

  CM_ALS_LINE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_als_line_e_type;

/** Enumeration of line switching status (FEATURE_ALS)
*/
typedef enum cm_als_line_switching_e {

  CM_ALS_LINE_SWITCHING_NONE = -1,
    /**< @internal */

  CM_ALS_LINE_SWITCHING_NOT_ALLOWED,
    /**< User is not allowed to switch between lines */

  CM_ALS_LINE_SWITCHING_ALLOWED,
    /**< User is allowed to switch between lines */

  CM_ALS_LINE_SWITCHING_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_als_line_switching_e_type;

/** Enumeration of CPHS procedures status (FEATURE_ALS)
*/
typedef enum cm_als_cphs_proc_e {

  CM_CPHS_PROC_NONE = -1,
    /**< @internal */

  CM_CPHS_PROC_NOT_ALLOWED,
    /**< CPHS procedures are not supported */

  CM_CPHS_PROC_ALLOWED,
    /**< CPHS procedures are supported */

  CM_ALS_CPHS_PROC_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_cphs_proc_e_type;

/** Enumeration of ALS procedures status (FEATURE_ALS)
*/
typedef enum cm_als_proc_e {

  CM_ALS_PROC_NONE = -1,
    /**< @internal */

  CM_ALS_PROC_NOT_ALLOWED,
    /**< ALS procedures are not supported */

  CM_ALS_PROC_ALLOWED,
    /**< ALS procedures are supported */

  CM_ALS_PROC_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_als_proc_e_type;

/** This enum stores the various status codes that the MC can send to the
** CM regarding MO DBM messages.
*/
typedef enum cm_dbm_mo_status_e {

  CM_DBM_MO_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_DBM_MO_OK,
    /**< SMS msg has been completed successfully */

  CM_DBM_MO_HOLD_ORIG_RETRY_TIMEOUT,
    /**< HOLD ORIG retry timeout*/

  CM_DBM_MO_HOLD_ORIG,
    /**< MC can not process MO SMS because HOLD ORIG is TRUE*/

  CM_DBM_MO_NO_SRV,
    /**< can not send SMS msg  - no service */

  /* 4 */
  CM_DBM_MO_ABORT,
    /**< MC or CM aborts the SMS request */

  CM_DBM_MO_NOT_ALLOWED_IN_AMPS,
    /**< can not sent SMS msg in analog mode */

  CM_DBM_MO_NOT_ALLOWED_IN_HDR,
    /**< can not send data burst msg while HDR call is up */

  CM_DBM_MO_L2_ACK_FAILURE,
    /**< not receiving L2 ack from base station */

  /* 8 */
  CM_DBM_MO_L3_ACK_FAILURE,
    /**< not receiving L3 ack from base station */

  CM_DBM_MO_OUT_OF_RESOURCES,
    /**< e.g. out of memory buffer */

  CM_DBM_MO_ACCESS_TOO_LARGE,
    /**< message is too large to be sent over access */

  CM_DBM_MO_DTC_TOO_LARGE,
    /**< message is too large to be sent over DTC */

  /* 12 */
  CM_DBM_MO_OTHER,
  /**< any status response other than above */

  CM_DBM_MO_ACCT_BLOCK,
    /**< Access Control based on Call Type(service option)  is blocked */

  CM_DBM_MO_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_dbm_mo_status_e_type;

/** This enum should be used for call originations to fill in cm_num_s_type.
*/
typedef enum cm_num_type_e {

    CM_NUM_TYPE_NONE = -1,                /**< @internal */

    CM_NUM_TYPE_UNKNOWN = 0,              /**< Unknown number type */

    CM_NUM_TYPE_INTERNATIONAL = 1,        /**< International number */

    CM_NUM_TYPE_NATIONAL = 2,             /**< National number */

    CM_NUM_TYPE_NETWORK_SPECIFIC = 3,     /**< Network Specific number */

    CM_NUM_TYPE_SUBSCRIBER = 4,           /**< Subscriber number */

    CM_NUM_TYPE_ABBREVIATED = 6,          /**< Abbreviated number */

    CM_NUM_TYPE_RESERVED_EXTENSION = 7,   /**< Reserved for extension */

    CM_NUM_TYPE_MAX                       /**< @internal */

} cm_num_type_e_type;

typedef enum cm_num_plan_e {

    CM_NUM_PLAN_NONE = -1,          /**< @internal */

    CM_NUM_PLAN_UNKNOWN = 0,        /**< Unknown number plan */

    CM_NUM_PLAN_ISDN = 1,           /**< ISDN/Telephony numbering plan
                                    **   (CCITT E.164 & CCITT E.163) */

    CM_NUM_PLAN_DATA = 3,           /**< Data numbering plan (CCITT X.121) */

    CM_NUM_PLAN_TELEX = 4,          /**< Telex numbering plan (CCITT F.69) */

    CM_NUM_PLAN_NATIONAL = 8,       /**< National numbering plan */

    CM_NUM_PLAN_PRIVATE = 9,        /**< Private numbering plan */

    CM_NUM_PLAN_RESERVED_CTS = 11,  /**< Reserved for CTS */

    CM_NUM_PLAN_RESERVED_EXTENSION = 15, /**< Reserved for extension */

    CM_NUM_PLAN_MAX                 /**< @internal */

} cm_num_plan_e_type;

/** Response to recall indication from network
** Enumeration can be used only if CM_API_CCBS is defined
*/
typedef enum cm_recall_rsp_e {

    CM_RECALL_RSP_CCBS_NONE = -1,

    CM_RECALL_RSP_CCBS_ACCEPT,       /**< Accept the recall */

    CM_RECALL_RSP_CCBS_REJECT,       /**< Reject the recall */

    CM_RECALL_RSP_CCBS_HOLD_ACCEPT,  /**< Hold active calls & accept recall */

    CM_RECALL_RSP_CCBS_REL_ACCEPT,   /**< Rel active calls & accept recall */

    CM_RECALL_RSP_CCBS_MAX

} cm_ccbs_recall_rsp_e_type;

/**  Responses that can be received in 5_send_cnf and recall indication
** Enumeration can be used only if CM_API_CCBS is defined
*/
typedef enum cm_ccbs_cnf_e {

    CM_CCBS_CNF_NONE       = -1,

    CM_CCBS_CNF_RET_RES,   /**< CCBS specific information present */

    CM_CCBS_CNF_RET_ERR,   /**< SS error */

    CM_CCBS_CNF_REJ,       /**< SS request rejected */

    CM_CCBS_CNF_RECALL_IND, /**< recall indicaton for CCBS call */

    CM_CCBS_CNF_MAX

} cm_ccbs_cnf_e_type;


/** Enums for the power collapse functionality
** enum to indicate which processor the clients are executing
*/

typedef enum cm_processor_e {

  CM_PROCESSOR_NONE   = -1,

  CM_PROCESSOR_MODEM,  /**< The client is executing on the modem processor */

  CM_PROCESSOR_APPS_1, /**< The client is executing on the APPS processor  */

  CM_PROCESSOR_MAX
} cm_processor_e_type;


/** Enum to indicate the event notification preferences while in power
** collapse
*/

typedef enum cm_pwr_collapse_notify_e {

  CM_PWR_COLLAPSE_NOTIFY_NULL = -1,

  CM_PWR_COLLAPSE_NOTIFY_NONE,   /**< Do not notify anything in powersave */

  CM_PWR_COLLAPSE_NOTIFY_NORMAL, /**< Notify all the events in power save */

  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT, /**< Notify the default "important
                                            ** events" as defined by CM
                                            */

  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM, /**< Notify the "important events"
                                           ** as defined by clients
                                           */
  CM_PWR_COLLAPSE_NOTIFY_MAX

} cm_pwr_collapse_notify_e_type;


/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

typedef int   cm_client_id_type;
  /**< Client ID type to differentiate CM client objects from one another */

typedef uint8 cm_call_id_type;
  /**< Data type for call id */

typedef uint8 cm_npi_type;
  /**<
  ** Data type for number presentation indicator.
  ** Values are defined by CM_PRESENTATION_XXX.
  */

typedef uint8 cm_si_type;
  /**<
  ** Data type for screening indicator.
  ** Values are defined by CM_USER_PROVIDED_XXX.
  */

typedef uint8 cm_ccbs_indx_type;
  /**<
  ** Data type for ccbs index
  ** Can be used only if CM_API_CCBS is defined
  */

typedef uint64 cm_call_type_mask_type;
  /**<
  ** Data type to carry bit masks of call types.
  ** CM_CALL_TYPE_x_MASK are the defines that
  ** go with this type.
  **
  ** Clients needs to check for CM_API_VIDEOSHARE
  ** before using this type
  */

typedef uint32 cm_emerg_srv_categ_type;
  /**< Data type for emergency service category */

/** @}
*  endgroup COMMON Common definitions
*/

/****************************************************************************

Call Object

****************************************************************************/

/** @defgroup CALL Call related definitions
**  @{
*/

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

#define MAX_DISPLAY_TEXT_LEN       255

#define CM_CALL_CMD_PARAM_DEFAULT_VALUE      0
  /**< Init value for all call command parameter buffer */

#define CM_CALL_EVENT_DEFAULT_FIELD_VALUE    CM_CALL_CMD_PARAM_DEFAULT_VALUE
  /**< Initial init value for all call event fields */


/** Define number of simulteneous called allowed:
** - For CDMA only build, 8 for SVD, 1 otherwise
** - For GSM/WCDMA or Multimode, 8 always.
*/
#define CM_CALL_ID_MAX              ( 6 + SYS_MAX_PDP_CONTEXTS )
  /**< Maximum number of simulteneous calls allowed.
  ** CDMA: voice/test voice, data/test data, SMS, GPS, OTASP, OTAPA
  ** GW: 6 voice + N ps */

#define CM_CALL_ID_INTERFACE_MAX    15
  /* The value 15 is derived same as CM_CALL_ID_MAX, assuming that
  ** FEATURE_MULTIPLE_PRIMARY_PDP and FEATURE_SECONDARY_PDP are
  ** defined. In case value of SYS_MAX_PRIMARY_PDP_CONTEXTS or 
  ** SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY changes, this value 
  ** needs to be updated accordingly.
  */

#define CM_CALL_ID_INVALID          ((cm_call_id_type) 0xFF)
  /**< Invalid call id */

#define CM_CALL_ID_UNASSIGNED       ((cm_call_id_type) 0xFE)
  /**< Call id unknown at this time */

#define CM_CALL_ID_ALL_CALLS        ((cm_call_id_type) 0xFD)
  /**< Applies to all calls. Call id is not necessary */

#define CM_CALL_ID_NONE             ((cm_call_id_type) 0xFC)
  /**< @internal No call id */

#define CM_MAX_DIALED_DIGITS  36
  /**<
  ** Max length of dialed digit string.
  ** This should be same as NV_MAX_DIAL_DIGITS
  ** This is hardcoded here to
  ** eliminate dependency on nv.h for remote processors.
  */

/*
** Subaddress values
*/

#define CM_MAX_SUBADDRESS_DIGITS   21
  /**< Max length of subaddress digit string */

#define CM_SUBADDRESS_TYPE_NSAP     0
  /**< Subaddress type - NSAP (CCITT X.213 / ISO 8348 AD2) */

#define CM_SUBADDRESS_TYPE_USER     1
  /**< Subaddress type - User specified */

#define CM_SUBADDRESS_EO_IND_EVEN   0
  /**< Even number of address signals */

#define CM_SUBADDRESS_EO_IND_ODD    1
  /**< Odd number of address signals  */


#define CM_MAX_CALLER_ID_CHARS      32
  /**< Max length of Caller ID string */

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_MM_SUPERSET)) 
#define CM_MAX_CALLED_INFO_CHARS    81
  /**< 3GPP spec 24.008 (Rel 5) section 10.5.4.7 specifices
  ** called party BCD number to 40 BCD digits  (80 bytes).
  ** UI can supply CM with "+" in the begining (1  byte).
  ** for international numbers
  ** CAUTION: x increase in value of INFO_CHARS increases
  ** call obj by close to 8x times.
  */
#else
#define CM_MAX_CALLED_INFO_CHARS    65
#endif

#define CM_MAX_CONNECTED_NUM_CHARS  64
  /**< Max length of Connected num string. CM_MAX_DATA_DIGITS */

#define CM_MAX_NUMBER_CHARS         CM_MAX_CALLED_INFO_CHARS
  /**< Max length of CM number string
  ** This is the Maximum of CM_MAX_DIALED_DIGITS, CM_MAX_CALLER_ID_CHARS
  ** CM_MAX_CALLED_INFO_CHARS and CM_MAX_CALLED_INFO_CHARS */

#define CM_MAX_ALPHA_LEN_FOR_CALL_ORIG      90
  /*< Max number of characters for alpha tag on call origination.
  ** Size being increased to support SIM related GCF test cases
  ** where a string upto size 90 can be required to be displayed
  ** on the UE.
  */

#define CM_MAX_ALPHA_LEN_FOR_DISPLAY_INFO   90
  /*< CM_MAX_DATA_DIGITS
  ** Same reason as for CM_MAX_ALPHA_LEN_FOR_CALL_ORIG. Call events
  ** deliver alpha string to UI and so they need to carry as much
  ** as origination can give.
  */

#define CM_MAX_ALPHA_TAG_CHARS      CM_MAX_ALPHA_LEN_FOR_DISPLAY_INFO
  /**< Max length of CM alpha tag string.
  ** This is the maximum of CM_MAX_ALPHA_LEN_FOR_CALL_ORIG and
  ** CM_MAX_ALPHA_LEN_FOR_DISPLAY_INFO */

#define CM_MAX_ALPHA_TAG_BYTES      (CM_MAX_ALPHA_TAG_CHARS)
  /*< Max length of CM alpha tag string in bytes.
  ** This is taken to be same as CM_MAX_ALPHA_TAG_CHARS
  ** assuming that it is such a coding scheme
  */

#define CM_ORIG_QOS_LEN_MAX         31
  /**< Max lenght of Quality of Service buffer.
  ** Should be same as CAI_ORIG_QOS_LEN_MAX */

/* The following constants are used for GSM/WCDMA.
*/

#define CM_EXTENSION_BIT           0x80
  /**< Indicates octet 3a of the BCD number IE
      (calling party/called party/redirecting party information element)
  */

/** Presentation Indicator */
#define CM_PRESENTATION_IND_MASK   0x60
  /**< Presentation indicator  (octet 3a) */

/** Bits 7-6 for presentation indicator */
#define CM_PRESENTATION_ALLOWED    0x00
  /**< Presentation allowed */

#define CM_PRESENTATION_RESTRICTED 0x01
  /**< Presentation restricted */

#define CM_NUMBER_NOT_AVAILABLE    0x02
  /**< Number not available due to interworking */

/** Screening Indicator */
#define CM_SCREENING_IND_MASK       0x03
  /**< Screening indicator (octet 3a) */

/** Bits 2-1 */
#define CM_USER_PROVIDED_NOT_SCRND  0
  /**< User-provided, not screened */

#define CM_USER_PROVIDED_VER_PASSED 0x01
  /**< User-provided, verified and passed */

#define CM_USER_PROVIDED_VER_FAILED 0x02
  /**< User-provided, verified and failed */

#define CM_NETWORK_PROVIDED         0x03
  /**< Network provided */

/* type of number
*/
#define CM_TON_MASK                 0x70
/**< type of number  (octet 3) */

/** Bits 7-5 */
#define CM_TON_UNKNOWN              (0x00 << 4)
  /**< Unknown */

#define CM_TON_INTERNATIONAL        (0x01 << 4)
  /**< International number */

#define CM_TON_NATIONAL             (0x02 << 4)
  /**< National number */

#define CM_TON_NETWORK_SPECIFIC     (0x03 << 4)
  /**< Network specific number */

#define CM_TON_DEDICATED_ACCESS     (0x04 << 4)
  /**< Dedicated access, short code */

#define CM_TON_RESERVED_5           (0x05 << 4)
  /**< Reserved */

#define CM_TON_RESERVED_6           (0x06 << 4)
  /**< Reserved */

#define CM_TON_RESERVED_EXTENSION   (0x07 << 4)
  /**< Reserved for extension */

/* Numbering Plan Identification
*/
#define CM_NUM_PLAN_MASK            0x0F
  /**< Numbering Plan Identification (octet 3) */

#define CM_NPI_UNKNOWN              0x00
  /**< Unknown */

#define CM_NPI_ISDN                 0x01
  /**< ISDN/telephony numbering plan */

#define CM_NPI_DATA                 0x03
  /**< Data numbering plan */

#define CM_NPI_TELEX                0x04
  /**< Telex numbering plan */

#define CM_NPI_NATIONAL             0x08
  /**< National numbering plan */

#define CM_NPI_PRIVATE              0x09
  /**< Private numbering plan */

#define CM_NPI_RESERVED_CTS         0x0b
  /**< Reserved for CTS */

#define CM_NPI_RESERVED_EXTENSION   0x0f
  /**< Reserved for extension */

#define CM_MAX_CCP_LEN              14

#define CM_MAX_SEARCH_LIST_LENGTH   10
  /**< Max length of search list */

#define CM_RP_USER_DATA_TPDU_LENGTH 232
  /**< Max lenght of RP user data */

#define CM_NAS_MN_TPDU_SIZE         164
  /**< Max size of MN TPDU size */

#define CM_NAS_CB_DATA_PAGE_SIZE    88

#define CM_SMS_ADDRESS_MAX          32

#define CM_SMS_MAX_LEN              255

#define CM_USSD_ALPHABET_MASK       0x0F
  /*< MASK used to decide on USSD Data coding scheme *
  ** Clients need to check for CM_API_USSD_DCS before
  ** using this macro
  */

#define CM_CCBS_INDX_INVALID       ((uint8) 0xFF)
  /**< Invalid ccbs indx
  ** can be used only if CM_API_CCBS is defined
  */


#define CM_MAX_CCBS_REQ             5  /* see 24.093 fig 4.5.3 NOTE */
  /**< Max total of CCBS indexes possible
  ** Should be the same as MAX_CCBS_FEATURE_LIST_SIZE in mn_cm_exp.h
  ** Not using "#define CM_MAX_CCBS_REQ MAX_CCBS_FEATURE_LIST_SIZE"
  ** because then it has to be featurized under GSM.
  ** can be used only if CM_API_CCBS is defined
  */

#define CM_MAX_DO_NEIGHBOR_CNT      16 
/**< The max number of DO neighbors of an LTE systems 
** that is sent for pilot strength measurments to CM
*/

#define CM_MIN_DO_NEIGHBOR_CNT      1 
/**< The min number of DO neighbors of an LTE systems 
** that is sent for pilot strength measurments to CM
*/

#define CM_MAX_DO_SRCH_WIN          15 
/**< The maximum search window size allowed by HDR searcher 
*/

#define CM_MAX_EMERGENCY_NUM_COUNT  16

#define CM_MIN_EMERGENCY_NUM_COUNT 1

 /**< Clients need to check for CM_API_EMERGENCY_NUM_LIST before
 **   using
 **
 ** Should be the same as CM_MAX_EMERGENCY_NO_COUNT in reg_cm.h.
 **
 ** Emergency list IE is max of 50 bytes. One byte of IEI and one byte of
 ** List Length leaves with max of 48 bytes. Each individual emergency number
 ** will have one byte of length indicator, one byte of type indicator
 ** (if number is Hospital or Police etc.) and at least one byte of BCD
 ** digits for the emergency number. This means that each individual emergency
 ** number will take at least 3 bytes. Therefore, max number of emergency
 ** numbers that can be sent by network in this IE is 48/3 = 16.
 */


#define CM_CALLED_PARTY_BCD_WITH_BCD_LEN  (CM_CALLED_PARTY_BCD_NO_LENGTH + 1)
  /**< To be used when passing bcd buffer to
  **  cm_util_number_to_bcd() or cm_util_bcd_to_ascii(). Function
  **  puts BCD length in the same array and so need that extra
  **  byte.
  **
  ** Clients needs to check for CM_API_ECATEG_IN_NUM_STRUCT before
  ** using this field.
  */

/**
** Type definition of a buffer that contains a number
*/
typedef uint8 cm_num_buf_type[CM_MAX_NUMBER_CHARS];

/*-------------------------------------------------------------------------*/
/*
** Clients needs to check for CM_API_VIDEOSHARE
** before using following #defines
*/

#define CM_IPAPP_HOST_SIZE                  20
  /**< Size of host name generating ip warning
  **
  ** CAUTION: x increase in HOST_SIZE increases
  ** cm_cmd_s_type by CM_CALL_ID_MAX * x times.
  ** end_params field
  */

#define CM_IPAPP_TEXT_SIZE                  20
  /**< Size of warning text generatedn from ipapp
  **
  ** CAUTION: x increase in TEXT_SIZE increases
  ** cm_cmd_s_type by CM_CALL_ID_MAX * x times.
  ** end_params field
  */

#define CM_MAX_OCTETS_L3_MESSAGE           251  /**< max size of L3 message
                                                ** as defined in 04.06 sect
                                                ** 5.8.5
                                                */

/**< Bit masks of fundamental call types. Combination of these can be generated
** by using ADD_CALL_TYPE_MASK
*/

#define CM_CALL_TYPE_NONE_MASK    ((cm_call_type_mask_type)0)
  /**< Bit mask for call type none */

#define CM_CALL_TYPE_VOICE_MASK   (cm_call_type_mask_type) SYS_BM_64BIT(CM_CALL_TYPE_VOICE)
  /**< Bit mask for voice call type */

#define CM_CALL_TYPE_VT_MASK      (cm_call_type_mask_type) SYS_BM_64BIT(CM_CALL_TYPE_VT)
  /**< Bit mask for VT call type */

#define CM_CALL_TYPE_VS_MASK      (cm_call_type_mask_type) SYS_BM_64BIT(CM_CALL_TYPE_VS)
  /**< Bit mask for VS call type */

#define CM_CALL_TYPE_SMS_MASK     (cm_call_type_mask_type) SYS_BM_64BIT(CM_CALL_TYPE_SMS)
  /**< Bit mask for SMS call type */

#define ADD_CALL_TYPE_MASK(CALL_TYPE1,CALL_TYPE2)  (cm_call_type_mask_type) (((uint64)CALL_TYPE1) | ((uint64)CALL_TYPE2))
  /**< Utility to create combination of call type capabilites */

/*-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/** Enumeration of call commands client are able to send to CM.
*/
typedef enum cm_call_cmd_e {

  CM_CALL_CMD_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_CALL_CMD_ORIG,
    /**< Originate a call */

  CM_CALL_CMD_ANSWER,
    /**< Answer an incoming call */

  CM_CALL_CMD_END,
    /**< End a call in orig, incom or conv state */

  CM_CALL_CMD_SUPS,
    /**< Send a call related supplementary service command
    ** eg: In CDMA, this is a FLASH command with information
    **     In WCDMA/GSM this may be a 1 SEND command to
    **     release the active call and accept the waiting call
    */

  /* 4 */
  CM_CALL_CMD_PRIVACY_PREF,
    /**< Change privacy pref for CDMA*/

  CM_CALL_CMD_INFO_GET,
    /**< Get a sanpshot of the call information in the CM */

  CM_CALL_CMD_INFO_LIST_GET,
    /**< Get a snapshot of call information for all calls in the CM */

  CM_CALL_CMD_SETUP_RES,
    /**< Response to a setup message for WCDMA/GSM/IP Voice App*/

  /* 8 */
  CM_CALL_CMD_PDP_ACT_RES,
    /**< Response to a PDP activation request for WCDMA/GSM PS*/

  CM_CALL_CMD_PDP_MODIFY,
    /**< cmd to modify a PDP for WCDMA/GSM PS */

  CM_CALL_CMD_RAB_REESTAB,
    /**< cmd to reetablish a RAB for WCDMA/GSM PS*/

  CM_CALL_CMD_PS_DATA_AVAILABLE,
    /**< cmd to inform protocol stack that data is available */
    /* for transmission                                    */

  /* 12 */
  CM_CALL_CMD_CC_COMPLETE_REORIG,
    /**< cmd to inform CM that call control is complete */
    /**< @internal */

  CM_CALL_CMD_UMTS_CDMA_HANDOVER_REQ,
    /**< Request to handoff UMTS call to 1x. */

  CM_CALL_CMD_UMTS_CDMA_HANDOVER_CNF,
    /**< Status confirmation for UMTS->1x handover. */

  CM_CALL_CMD_RECALL_RSP,
    /**<
    ** Client response to recall indication from network.
    */

  /* 16 */
  CM_CALL_CMD_MO_MODIFY_REQ,
    /**<
    ** Client request to modify call type
    */

  CM_CALL_CMD_MODIFY_RES,
    /**<
    ** Client response to modify request from network
    */

  CM_CALL_CMD_HANDOVER_REQ,
    /**<
     ** Generic handoff request
    */

  CM_CALL_CMD_PS_SIG_REL,
    /**<
    ** Command to request CM to release PS signaling
    */

  CM_CALL_CMD_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_call_cmd_e_type;


/** Enumeration of call command errors returned by call command
** callback functions.
*/
typedef enum cm_call_cmd_err_e {

  CM_CALL_CMD_ERR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_CALL_CMD_ERR_NOERR,
    /**< No errors found */

  /* Error codes resulting from clients feeding a call command
  ** function with bad or inappropriate parameters.
  */

  CM_CALL_CMD_ERR_CLIENT_ID_P,
    /**< invalid client ID - null or not initialized*/

  CM_CALL_CMD_ERR_CALL_ID_P,
    /**< invalid call ID in the command */

  CM_CALL_CMD_ERR_NUM_CALL_IDS_P,
    /**< Number of call Ids specified exceeds the maximum
    ** number of call IDs allocated
    */

  /* 4 */
  CM_CALL_CMD_ERR_CALL_TYPE_P,
    /**< Bad call type parameter */

  CM_CALL_CMD_ERR_SRV_TYPE_P,
    /**< Bad service type parameter */

  CM_CALL_CMD_ERR_SRV_OPT_P,
    /**< Bad service option parameter. CDMA only */

  CM_CALL_CMD_ERR_MODE_PREF_P,
    /**< Requested call type cannot be made in current mode preference */

  /* 8 */
  CM_CALL_CMD_ERR_NO_NUM_P,
    /**< Expecting a number to be provided via the number parameter */

  CM_CALL_CMD_ERR_NULL_NUM_P,
    /**< Number length is > 0, but number buffer points to NULL */

  CM_CALL_CMD_ERR_NUM_P,
    /**< Invalid digits found in number buffer */

  CM_CALL_CMD_ERR_NUM_LEN_P,
    /**< Out of range value found in number length field */

  /* 12 */
  CM_CALL_CMD_ERR_NULL_ALPHA_P,
    /**< Alpha length is > 0, but alpha buffer points to NULL */

  CM_CALL_CMD_ERR_ALPHA_LEN_P,                                     /* 10 */
    /* Out of range value found in alpha length field */

  CM_CALL_CMD_ERR_MISMATCH_P,
    /**< Call type and parameter type do not match. GW only */

  CM_CALL_CMD_ERR_NULL_PARAM_P,
    /**< NULL pointer paramter in the API */

  /* 16 */
  CM_CALL_CMD_ERR_ACTIVATE_CODE_P,
    /**< Invalid OTASP activation code. CDMA only */

  CM_CALL_CMD_ERR_CALL_SUPS_P,
    /**< Invalid call related supplementary service parameter */

  CM_CALL_CMD_ERR_QOS_LEN_P,
    /**< Length of QOS parameters exceed maximum size */

  CM_CALL_CMD_ERR_LAST_ACT_DATA_NET_P,
    /**< Invalid last active data network */

  /* 20 */
  CM_CALL_CMD_ERR_INVALID_PRIMARY_CALL_ID_P,
  /**< OBSOLETE with definition of CM_API_SEC_PDP */
  /* The primary PDP context's call Id is invalid.
  ** Sent during a secondary PDP context origination and
  ** the specified primary PDP context is not valid
  */


  CM_CALL_CMD_ERR_INVALID_GROUP_ID_P,
    /**< Clients check CM_API_SEC_PDP for SEC PDP support.
    ** Sent during a secondary PDP context origination if
    ** the specified group id is not valid
    */

  CM_CALL_CMD_ERR_PROFILE_NUMBER_IN_USE_P,
    /**< The profile is already in use by another
    ** PDP context
    */

  CM_CALL_CMD_ERR_UMTS_CDMA_HANDOVER_REQ_P,
    /**< Error in parameter for UMTS ->1x handover cnf.
    */

  /* 24 */
  CM_CALL_CMD_ERR_UMTS_CDMA_HANDOVER_CNF_P,
    /**< Error in parameter for UMTS ->1x handover cnf.
    */

  /* Error codes resulting from clients trying to instruct
  ** CM to perform inappropriate actions relative to the current
  ** state of the call and/or phone.
  */

  CM_CALL_CMD_ERR_OFFLINE_S,
    /**< Phone is offline */

  CM_CALL_CMD_ERR_CDMA_LOCK_S,
    /**< Phone is CDMA locked */

  CM_CALL_CMD_ERR_FLASH_S,
    /**< Client tried to send a flash while a call that not supporting
    ** flashes is in progress. CDMA only */

  /* 28 */
  CM_CALL_CMD_ERR_ORIG_RES_S,
    /**< Dialed number is not allowed under the current origination
    ** restriction */

  CM_CALL_CMD_ERR_NO_FUNDS_S,
    /**< Dialed number is not allowed because phone is out of funds. GW only */

  CM_CALL_CMD_ERR_EMERGENCY_MODE_S,
    /**< Client tried to originate a NONE EMERGENCY call while EMERGENCY mode
    ** preference is in effect. CDMA only */

  CM_CALL_CMD_ERR_IN_USE_S,
    /**< Can't perform the command in question while phone is in-use */

  /* 32 */
  CM_CALL_CMD_ERR_SRV_TYPE_S,
    /**< Client originated with service type that is currently not
    ** supported by the phone/PRL */

  CM_CALL_CMD_ERR_CALL_TYPE_S,
    /**< The command in question is invalid for the call(s) in progress.
    ** e.g. Client answered with inappropriate call type relative to
    ** to the type of the incoming call, etc. */

  CM_CALL_CMD_ERR_CALL_STATE_S,
    /**< The command in question is invalid in the current call state -
    ** e.g. answering a call that was already answered,
    ** ending a call that was already ended, etc. */

  CM_CALL_CMD_ERR_SRV_STATE_S,
    /**< The command in question is invalid in the current Phone serving state-
    ** e.g answering a call but serving state is no service */

  /* 36 */
  CM_CALL_CMD_ERR_NO_SO_S,
    /**< no valid service found for origination with AUTOMATIC
    ** or CDMA_AUTOMATIC srv type. CDMA only  */

  CM_CALL_CMD_ERR_ALERT_ANS_BYPASS_S,
    /**< Client is not allowed to answer the incoming call. CDMA only
    ** Valid for FEATURE_IS95B_ALERT_ANS_BYPASS */

  CM_CALL_CMD_ERR_UIM_NOT_PRESENT_S,
    /**< UIM not present. Valid for FEATURE_UIM_RUIM */

  CM_CALL_CMD_ERR_BAD_PRIVACY_PREF_S,
    /**< Invalid privacy preference. CDMA only */

  /* 40 */
  CM_CALL_CMD_ERR_NO_CALL_ID_S,
    /**< No call IDs can be allocated */

  CM_CALL_CMD_ERR_CALL_ORIG_FAIL_S,
    /**< Call origination failed in the lower layer */

  CM_CALL_CMD_ERR_INVALID_MODE_S,
    /**< Recieved the command in a mode different from the phone's
    ** operational mode
    */

  CM_CALL_CMD_ERR_INVALID_SIM_STATE_S,
    /**< Call origination failed because the SIM was marked
    ** by the network as invalid for the circuit and/or
    ** packet service domain.
    */

  /* 44 */
  CM_CALL_CMD_ERR_MAX_PRIMARY_PDP_CONTEXTS_S,
    /**< The maximum number of primary PDP contexts have been activated */

  CM_CALL_CMD_ERR_MAX_SEC_PDP_CONTEXT_PER_PRI_S,
    /**< The maximum number of secondary PDP contexts
    ** has been reached for this primary PDP context
    */

  CM_CALL_CMD_ERR_NO_COLLOC_HDR,
    /**< There is no clocated HDR */

  /* Error codes resulting from operational limitation
  ** of CM.
  */

  CM_CALL_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command */

  /* 48 */
  CM_CALL_CMD_ERR_MC_COM_L,
    /**< Communication problems with MC - e.g. MC buffer shortage */

  CM_CALL_CMD_ERR_OTHER,
    /**< A problem other than the above was found */

  CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED,
    /**< Call control has rejected the command. */

  /* 52 */
  CM_CALL_CMD_ERR_FAVORED_MODE_PREF_FOR_ORIGINATION,
    /**< CM was not able to determine favored mode preference for origination
       for given call type, service type and current mode preference */

  CM_CALL_CMD_ERR_CONNECTION_ID_OUT_L,
    /**< CM was not able to get a connection Id from MN or SM */

  CM_CALL_CMD_ERR_SUPS_SRV_ON_PROHIBITED_LINE_S,
    /**<
    ** FEATURE_ALS
    ** Supplementary services are not allowed
    ** for calls that are on prohibited line.
    ** Prohibited line is one that is different than currently
    ** selected line when line switching is not allowed.
    */

  CM_CALL_CMD_ERR_USER_DATA_LENGTH_P,
    /**<
    ** FEATURE_UUS
    ** Invalid user data length parameter
    */

  /* 56 */
  CM_CALL_CMD_ERR_NON_NEGOT_BEARER_P,
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE definition
    ** before using this enum
    **
    ** When call gets connected bearer capabilities that both parties
    ** can support are negotiatied and stored in call object. MO Call
    ** modification request gets rejected by CM if bearer for new type
    ** requested has not been neogitiated during call set up. GSM WCDMA only.
    */

  CM_CALL_CMD_ERR_INVALID_BC_P,
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE definition
    ** before using this enum
    **
    ** Set up response to MT call setup indication should carry a valid
    ** BC in BC1 if there is a valid BC in BC2. GSM WCDMA only.
    **
    */

  CM_CALL_CMD_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_call_cmd_err_e_type;


/** Enumeration of call events.
** Clients are able to register with CM to be notified of any subsets
** of these events.
*/
typedef enum cm_call_event_e {

  CM_CALL_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_CALL_EVENT_ORIG,
    /**< phone originated a call */

  CM_CALL_EVENT_ANSWER,
    /**< Incoming call was answered */

  CM_CALL_EVENT_END_REQ,
    /**< Started call end process */

  CM_CALL_EVENT_END,
    /**< Originated/incoming call was ended */

  CM_CALL_EVENT_SUPS,
    /**< phone sent Flash / Flash with Info to BS */

  /* 5 */
  CM_CALL_EVENT_INCOM,
    /**< phone received an incoming call */

  CM_CALL_EVENT_CONNECT,
    /**< Originated/incoming call was connected */

  CM_CALL_EVENT_SRV_OPT,
    /**< Service option changed while in a call. CDMA only */

  CM_CALL_EVENT_PRIVACY,
    /**< Privacy mode changed while in a call. CDMA only */

  CM_CALL_EVENT_PRIVACY_PREF,
    /**< Privacy mode preference changed */

  /* 10 */
  CM_CALL_EVENT_CALLER_ID,
    /**< Caller ID info was received from BS. CDMA only */

  CM_CALL_EVENT_ABRV_ALERT,
    /**< CMDA/AMPS abbreviated alert. CDMA only */

  CM_CALL_EVENT_ABRV_REORDER,
    /**< AMPS abbreviated reorder. CDMA only */

  CM_CALL_EVENT_ABRV_INTERCEPT,
    /**< AMPS abbreviated intercept. CDMA only */

  CM_CALL_EVENT_SIGNAL,
    /**< signal info was received from BS */

  /* 15 */
  CM_CALL_EVENT_DISPLAY,
    /**< display info was received from BS. CDMA only */

  CM_CALL_EVENT_CALLED_PARTY,
    /**< called party info was received from BS. CDMA only */

  CM_CALL_EVENT_CONNECTED_NUM,
    /**< connected number info was received from BS */

  CM_CALL_EVENT_INFO,
    /**< call information, this event only sent to the client
    ** that request this information through cm_call_cmd_get_call_info */

  CM_CALL_EVENT_EXT_DISP,
    /**< extended display was received from BS. CDMA only */

  /* 20 */
  CM_CALL_EVENT_NDSS_START,
    /**< Start NDSS redirection. CDMA only */

  CM_CALL_EVENT_NDSS_CONNECT,
    /**< call was re-connected due to NDSS. CDMA only */

  /* The following are for FEATURE_JCDMA
  */

  CM_CALL_EVENT_EXT_BRST_INTL,
    /**< Extended Burst Type - International */

  CM_CALL_EVENT_NSS_CLIR_REC,
    /**< National Supplementary services - CLIR */

  CM_CALL_EVENT_NSS_REL_REC,
    /**< National Supplementary services - Release */

  /* 25 */
  CM_CALL_EVENT_NSS_AUD_CTRL,
    /**< National Supplementary services - Audio Control */

  CM_CALL_EVENT_L2ACK_CALL_HOLD,
    /**< Call Hold */

  /* The following are for WCDMA/GSM
  ** Except CM_CALL_EVENT_SETUP_RES which is also used by IP APP
  */

  CM_CALL_EVENT_SETUP_IND,
    /**< phone received a setup ind message from BS*/

  CM_CALL_EVENT_SETUP_RES,
    /**< A setup response was sent, used also for IP call */

  CM_CALL_EVENT_CALL_CONF,
    /**< The call origination has been accepted*/

  /* The following are for WCDMA/GSM  PS Data
  */

  /* 30 */
  CM_CALL_EVENT_PDP_ACTIVATE_IND,
    /**< phone received Incoming PDP call  */

  CM_CALL_EVENT_PDP_ACTIVATE_RES,
    /**< A response to incoming PDP was sent */

  CM_CALL_EVENT_PDP_MODIFY_REQ,
    /**< PDP modify request was sent */

  CM_CALL_EVENT_PDP_MODIFY_IND,
    /**< phone received a PDP modify indication from BS*/

  CM_CALL_EVENT_PDP_MODIFY_REJ,
    /**< phone received a PDP modify rejection from BS*/

  /* 35 */
  CM_CALL_EVENT_PDP_MODIFY_CONF,
    /**< phone received a PDP modify confirmation from BS*/

  CM_CALL_EVENT_RAB_REL_IND,
    /**< phone received a RAB release indication from BS*/

  CM_CALL_EVENT_RAB_REESTAB_IND,
    /**< phone received a RAB reestablish indication from BS*/

  CM_CALL_EVENT_RAB_REESTAB_REQ,
    /**< RAB reestablish request was sent */

  CM_CALL_EVENT_RAB_REESTAB_CONF,
    /**< phone received a RAB reestablish confirm from BS*/

  /* 40 */
  CM_CALL_EVENT_RAB_REESTAB_REJ,
    /**< phone received a RAB reestablish reject from BS*/

  CM_CALL_EVENT_RAB_REESTAB_FAIL,
    /**< RAB reestablishment failed */

  CM_CALL_EVENT_PS_DATA_AVAILABLE,
    /**< PS data available reqeust was sent */

  CM_CALL_EVENT_MNG_CALLS_CONF,
    /**< confirmation for mpty calls */

  CM_CALL_EVENT_CALL_BARRED,
    /**< call barred notification */

  /* 45 */
  CM_CALL_EVENT_CALL_IS_WAITING,
    /**< call is in waiting notification */

  CM_CALL_EVENT_CALL_ON_HOLD,
    /**< call on hold notification */

  CM_CALL_EVENT_CALL_RETRIEVED,
    /**< call retrieved notification */

  CM_CALL_EVENT_ORIG_FWD_STATUS,
    /**< Originated call may be forwarded notification to
       forwarding subscriber
    */
  CM_CALL_EVENT_CALL_FORWARDED,
    /**< call forwarded notification to forwarding subscriber*/

  /* 50 */
  CM_CALL_EVENT_CALL_BEING_FORWARDED,
    /**< call being forwarded notification to calling subsciber */

  CM_CALL_EVENT_INCOM_FWD_CALL,
    /**< Incoming forwarded call notification to
       forwarded-to-subscriber
    */
  CM_CALL_EVENT_CALL_RESTRICTED,
    /**< call restricted notification */

  CM_CALL_EVENT_CUG_INFO_RECEIVED,
    /**< call forward CUG notification */

  CM_CALL_EVENT_CNAP_INFO_RECEIVED,
    /**< caller name info notification */

  /* 55 */
  CM_CALL_EVENT_EMERGENCY_FLASHED,
    /**< A voice call was converted into emergency call */

  CM_CALL_EVENT_PROGRESS_INFO_IND,
    /**< call origination progress indication */

  CM_CALL_EVENT_CALL_DEFLECTION,
    /**< Call deflection notification */

  CM_CALL_EVENT_TRANSFERRED_CALL,
    /**< Call transfer notification */

  CM_CALL_EVENT_EXIT_TC,
    /**< A traffic channel has been torn down. CDMA only */

  /* 60 */
  CM_CALL_EVENT_REDIRECTING_NUMBER,
    /**< Redirecting Number Info Record Event. CDMA only */

  CM_CALL_EVENT_PDP_PROMOTE_IND,
    /**< OBSOLETE with definition of CM_API_SEC_PDP */
    /* Clients check CM_API_SEC_PDP for SEC PDP support
    ** Secondary context being promoted to primary
    */

  CM_CALL_EVENT_UMTS_CDMA_HANDOVER_START,
    /**< An event to initiate the UMTS to 1X handoff */

  CM_CALL_EVENT_UMTS_CDMA_HANDOVER_END,
    /**< An event to indicate the UMTS to 1X handoff is completed.*/

  CM_CALL_EVENT_SECONDARY_MSM,
    /**< Invoke event call back even if the event is from secondary MSM.
    ** CM will not generate any events with this, but is used by
    ** thin client CM layer to call client call backs for events from
    ** secondary msm.
    */

  /* 65 */
  CM_CALL_EVENT_ORIG_MOD_TO_SS,
    /**< An Origination command was modified to SS due to Call Control */

  CM_CALL_EVENT_USER_DATA_IND,
    /**< Clients need to check for CM_API_USER_USER_DATA before
    ** using this event. Indicates user data from remote side
    */

  CM_CALL_EVENT_USER_DATA_CONG_IND,
    /**< Clients need to check for CM_API_USER_USER_DATA before
    ** using this event. Indication from lower layers to stop
    ** sending user data
    */

  CM_CALL_EVENT_MODIFY_IND,
    /**< This event is applicable when CM_API_VT is defined.
    ** Network requested modification of VT to Voice and vice-versa.
    */

  CM_CALL_EVENT_MODIFY_REQ,
    /**< This event is applicable when CM_API_VT is defined.
    ** Request to do modification of VT to Voice and vice-versa is sent.
    */

  /* 70 */
  CM_CALL_EVENT_LINE_CTRL,
    /**< This event is sent when CM_LINE_CTRL_F report is received from
    ** the lower layers {1X only}.
    */

  CM_CALL_EVENT_CCBS_ALLOWED,
    /**< Clients need to check for CM_API_CCBS before
    ** using this event. Informs that CCBS is allowed
    ** on this call.
    */

  CM_CALL_EVENT_ACT_CCBS_CNF,
    /**< Clients need to check for CM_API_CCBS before
    ** using this event.
    ** Sent after CM_5_SEND_CNF is received from network
    */

  CM_CALL_EVENT_CCBS_RECALL_IND,
    /**< Clients need to check for CM_API_CCBS before
    ** using this event.
    ** Sent in response to CM_RECALL_IND from Network
    */

  CM_CALL_EVENT_CCBS_RECALL_RSP,
    /**< Clients need to check for CM_API_CCBS before
    ** using this event.
    ** Sent after receiving recall response from client
    */


  /* 75 */
  CM_CALL_EVENT_CALL_ORIG_THR,
    /**< Call origination throttled
    */

  CM_CALL_EVENT_VS_AVAIL,
    /**< Clients should check for CM_API_VIDEOSHARE before using
    ** this event.
    ** Videoshare call is possible for this voice call
    */

  CM_CALL_EVENT_VS_NOT_AVAIL,
    /**< Clients should check for CM_API_VIDEOSHARE before using
    ** this event.
    ** Videoshare call is not possible for this voice call
    */

  CM_CALL_EVENT_MODIFY_COMPLETE_CONF,
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE before
    ** using this event.
    ** Event sent after in-call modification confirmation has been
    ** received by CM.
    */

  CM_CALL_EVENT_MODIFY_RES,
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE before
    ** using this event.
    ** Event sent to indicate client's response to MT in-call modification
    ** indication.
    */

  /* 80 */
  CM_CALL_EVENT_CONNECT_ORDER_ACK,
    /**< Clients need to  check for CM_API_CONNECT_ORDER_ACK before
    ** using this event.
    ** Event sent to indicate client's response to user answered the call.
    */

  CM_CALL_EVENT_TUNNEL_MSG,
    /**< Got a tunneled message for the call
    */

  CM_CALL_EVENT_END_VOIP_CALL,
    /**< Event to end the voip call
    */

  CM_CALL_EVENT_VOIP_CALL_END_CNF,
    /**< Event to confirm voip call end
    */
  /* 84 */
  CM_CALL_EVENT_PS_SIG_REL_REQ,
   /**< Started PS Signaling release process
   */

  /* 85 */
  CM_CALL_EVENT_PS_SIG_REL_CNF,
   /**< Event to notify PS Signaling release confirmation
   */

  CM_CALL_EVENT_HANDOFF_CALL_ORIG,
    /**<  HOM originated call
    */

    /* 87 */
  CM_CALL_EVENT_HANDOFF_CALL_CONNECT,
    /**<  HOM originated / incoming call was connected
    */

  CM_CALL_EVENT_HANDOFF_CALL_END,
    /**<  HOM originated / incoming call was ended
    */

  CM_CALL_EVENT_HANDOFF_MANUAL_CALL_ORIG,
    /**<  Manually originated / incoming call.
    */

    /* 90 */
  CM_CALL_EVENT_MPTY_CALL_IND,
    /**<  multiparty call indication.
    */

  CM_CALL_EVENT_OTASP_STATUS,
    /**<  OTASP status indication
    */

   /* 92 */
  CM_CALL_EVENT_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_call_event_e_type;

typedef enum cm_redirecting_reason_e {

  CM_REDIRECTING_REASON_NONE = -1,
    /**< @internal */

  CM_REDIRECTING_REASON_UNKNOWN = 0,
    /**< Unknown */

  CM_REDIRECTING_REASON_CALL_FORWARDING_BUSY = 1,
    /**< Call forwarding busy or called DTE busy */

  CM_REDIRECTING_REASON_CALL_FORWARDING_NO_REPLY = 2,
    /**< Call forwarding no reply (circuit-mode only) */

  CM_REDIRECTING_REASON_CALLED_DTE_OUT_OF_ORDER = 9,
    /**< Called DTE out of order (packet-mode only) */

  CM_REDIRECTING_REASON_CALL_FORWARDING_BY_THE_CALLED_DTE = 10,
    /**< Call forwarding by the called DTE (packet-mode only) */

  CM_REDIRECTING_REASON_CALL_FORWARDING_UNCONDITIONAL = 15,
    /**< Call forwarding unconditional or Systematic call redirection */

  CM_REDIRECTING_REASON_RESERVED,
    /**< Reserved */

  CM_REDIRECTING_REASON_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

}cm_redirecting_reason_e_type;

/* mapping the old band name to the new enumeration */
#define CM_ABAND      CM_ACTIVATE_CODE_BC0_ABAND
#define CM_BBAND      CM_ACTIVATE_CODE_BC0_BBAND
#define CM_ABLOCK     CM_ACTIVATE_CODE_BC1_ABLOCK
#define CM_BBLOCK     CM_ACTIVATE_CODE_BC1_BBLOCK
#define CM_CBLOCK     CM_ACTIVATE_CODE_BC1_CBLOCK
#define CM_DBLOCK     CM_ACTIVATE_CODE_BC1_DBLOCK
#define CM_EBLOCK     CM_ACTIVATE_CODE_BC1_EBLOCK
#define CM_FBLOCK     CM_ACTIVATE_CODE_BC1_FBLOCK

/**< OTASP activation code. For CDMA only.
*/
typedef enum cm_activate_code_e {
  CM_ACTIVATE_CODE_BC0_ABAND = 0,
  /**< Band Class 0, A band */

  CM_ACTIVATE_CODE_BC0_BBAND = 1,
  /**< Band Class 0, B band */

  CM_ACTIVATE_CODE_BC1_ABLOCK = 2,
  /**< Band Class 1, A block */

  CM_ACTIVATE_CODE_BC1_BBLOCK = 3,
  /**< Band Class 1, B block */

  CM_ACTIVATE_CODE_BC1_CBLOCK = 4,
  /**< Band Class 1, C block */

  CM_ACTIVATE_CODE_BC1_DBLOCK = 5,
  /**< Band Class 1, D block */

  CM_ACTIVATE_CODE_BC1_EBLOCK = 6,
  /**< Band Class 1, E block */

  CM_ACTIVATE_CODE_BC1_FBLOCK = 7,
  /**< Band Class 1, F block */

  /* Band Class 2 and 3 are not supportd */

  CM_ACTIVATE_CODE_BC4_ABAND = 12,
  /**< Band Class 4, A Band */

  CM_ACTIVATE_CODE_BC4_BBAND = 13,
  /**< Band Class 4, B Band */

  CM_ACTIVATE_CODE_BC4_CBAND = 14,
  /**< Band Class 4, C Band */

  CM_ACTIVATE_CODE_BC5_ABAND = 15,
  /**< Band Class 5, A Band */

  CM_ACTIVATE_CODE_BC5_BBAND = 16,
  /**< Band Class 5, B Band */

  CM_ACTIVATE_CODE_BC5_CBAND = 17,
  /**< Band Class 5, C Band */

  CM_ACTIVATE_CODE_BC5_DBAND = 18,
  /**< Band Class 5, D Band */

  CM_ACTIVATE_CODE_BC5_EBAND = 19,
  /**< Band Class 5, E Band */

  CM_ACTIVATE_CODE_BC5_FBAND = 20,
  /**< Band Class 5, F Band */

  CM_ACTIVATE_CODE_BC5_GBAND = 21,
  /**< Band Class 5, G Band */

  CM_ACTIVATE_CODE_BC5_HBAND = 22,
  /**< Band Class 5, H Band */

  CM_ACTIVATE_CODE_BC6 = 23,
  /**< Band Class 6 */

  /* Band Class 7, A Band and B Band are not supported */

  CM_ACTIVATE_CODE_BC7_CBAND = 24,
  /**< Band Class 7, C Band */

  /* Kept for backward compatibility issues for 1H07
  ** Needs to be removed once UI picks up BC 18, 19 support
  */
  CM_ACTIVATE_CODE_BC7_DBAND = 27,
  /**< Band Class 7, D Band */

  CM_ACTIVATE_CODE_BC19_ABAND = 25,
  /**< Band Class 19, A Band */

  CM_ACTIVATE_CODE_BC19_BBAND = 26,
  /**< Band Class 19, B Band */

  CM_ACTIVATE_CODE_BC19_CBAND = 27,
  /**< Band Class 19, C Band */

  CM_ACTIVATE_CODE_BC8 = 28,
  /**< Band Class 8 */

  CM_ACTIVATE_CODE_BC9 = 29,
  /**< Band Class 9 */

  CM_ACTIVATE_CODE_BC10_ABAND = 30,
  /**< Band Class 10, A Band */

  CM_ACTIVATE_CODE_BC10_BBAND = 31,
  /**< Band Clas 10, B Band */

  CM_ACTIVATE_CODE_BC10_CBAND = 32,
  /**< Band Class 10, C Band */

  CM_ACTIVATE_CODE_BC10_DBAND = 33,
  /**< Band Class 10, D Band */

  CM_ACTIVATE_CODE_BC10_EBAND = 34,
  /**< Band Class 10, E Band */

  CM_ACTIVATE_CODE_BC11_ABAND = 35,
  /**< Band Class 11, A Band */

  CM_ACTIVATE_CODE_BC11_BBAND = 36,
  /**< Band Class 11, B Band */

  CM_ACTIVATE_CODE_BC11_CBAND = 37,
  /**< Band Class 11, C Band */

  CM_ACTIVATE_CODE_BC11_DBAND = 38,

  CM_ACTIVATE_CODE_BC11_EBAND = 39,

  CM_ACTIVATE_CODE_BC11_FBAND = 40,
  /**< Band Class 11, F Band */

  CM_ACTIVATE_CODE_BC12_ABAND = 41,
  /**< Band Class 12, A Band */

  CM_ACTIVATE_CODE_BC12_BBAND = 42,
  /**< Band Class 12, B Band */

  /* Band Class 14, A Block to F Block has the same activation code
  ** and the same frequency as Band Class 1.
  ** So we'll use Band Class 1 for it.
  */

  CM_ACTIVATE_CODE_BC14_GBLOCK = 57,
  /**< Band Class 14, G Block */

  CM_ACTIVATE_CODE_BC15_ABAND = 58,
  /**< Band Class 15, A Band */

  CM_ACTIVATE_CODE_BC15_BBAND = 59,
  /**< Band Class 15, B Band */

  CM_ACTIVATE_CODE_BC15_CBAND = 60,
  /**< Band Class 15, C Band */

  CM_ACTIVATE_CODE_BC15_DBAND = 61,
  /**< Band Class 15, D Band */

  CM_ACTIVATE_CODE_BC15_EBAND = 62,
  /**< Band Class 15, E Band */

  CM_ACTIVATE_CODE_BC15_FBAND = 63,
  /**< Band Class 15, F Band */

  CM_ACTIVATE_CODE_BC16_ABAND = 64,
  /**< Band Class 16, A Band */

  CM_ACTIVATE_CODE_BC16_BBAND = 65,
  /**< Band Class 16, B Band */

  CM_ACTIVATE_CODE_BC16_CBAND = 66,
  /**< Band Class 16, C Band */

  CM_ACTIVATE_CODE_BC16_DBAND = 67,
  /**< Band Class 16, D Band */

  CM_ACTIVATE_CODE_BC18_DBAND = 68,
  /**< Band Class 18, D Band */

  CM_ACTIVATE_CODE_BC18_PSB_BAND = 69,
  /**< Band Class 18, PSB Band */

  /* Activation code 70 - 99 is reserved */

  CM_PREFERRED  = 100,
  /**< Used Internally
  ** PREFFERRED is currently mapped to BC1 A Block.
  ** This is only used for non-standard otasp call
  */

  CM_OTASP_ACT_CODE_NONE = 101
  /**< None */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_activate_code_e_type;

/** Enumeration of OTASP states. While an OTASP call is in
**  progress, CP will inform CM of the current state via this
**  enumeration
*/
typedef enum cm_otasp_status_e {

  CM_OTASP_STATUS_NONE = -1,
    /**< @internal */

  CM_OTASP_STATUS_SPL_UNLOCKED,
    /**< SPL unlocked. Only for user initiated OTASP.*/

  CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED,
    /**< SPC retries exceeded. Only for user initiated OTASP.*/

  CM_OTASP_STATUS_AKEY_EXCHANGED,
    /**< A-key exchanged. Only for user initiated OTASP.*/

  /* 3 */
  CM_OTASP_STATUS_SSD_UPDATED,
    /**< SSD updated. For both user initiated OTASP and 
    **   network initiated OTASP(OTAPA)
    */

  CM_OTASP_STATUS_NAM_DOWNLOADED,
    /**< NAM downloaded. Only for user initiated OTASP. */

  CM_OTASP_STATUS_MDN_DOWNLOADED,
    /**< MDN downloaded. Only for user initiated OTASP. */

  CM_OTASP_STATUS_IMSI_DOWNLOADED,
    /**< IMSI downloaded. Only for user initiated OTASP.*/

  /* 7 */
  CM_OTASP_STATUS_PRL_DOWNLOADED,
    /**< PRL downloaded. Only for user initiated OTASP.*/

  CM_OTASP_STATUS_COMMITTED,
    /**< Commit successful. Only for user initiated OTASP. */

  CM_OTASP_STATUS_OTAPA_STARTED,
    /**< OTAPA started . Only for network initiated OTASP(OTAPA) */

  CM_OTASP_STATUS_OTAPA_STOPPED,
    /**< OTAPA stopped .Only for network initiated OTASP(OTAPA) */

  /* 11 */
  CM_OTASP_STATUS_OTAPA_ABORTED,
    /**< OTAPA aborted. Only for network initiated OTASP(OTAPA) */

  CM_OTASP_STATUS_MAX
    /**< @internal */

} cm_otasp_status_e_type;

/** Enumeration of call states. At all times a CM call must be in one
** of the states below.
*/
typedef enum cm_call_state_e {

  CM_CALL_STATE_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_CALL_STATE_IDLE,
    /**< Call is in idle state - i.e. no call */

  CM_CALL_STATE_ORIG,
    /**< Call is in origination state */

  CM_CALL_STATE_INCOM,
    /**< Call is in alerting state */

  CM_CALL_STATE_CONV,
    /**< Call is in conversation state */

  /* 4 */
  CM_CALL_STATE_CC_IN_PROGRESS,
    /**< Call is originating but waiting for call control to complete */

  CM_CALL_STATE_RECALL_RSP_PEND,
    /**< Call is waiting for recall response from client
    */

  CM_CALL_STATE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_call_state_e_type;


/** Enumeration of CM call types.
** When originating or answering a call, clients need to specify
** one of these call types.
**
** For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_call_type_e {

  CM_CALL_TYPE_NONE=-1,
    /**< @internal */

  CM_CALL_TYPE_VOICE=0,
    /**< Voice call type.
    **
    ** NOTE! Answering with this type is only successful if the
    ** incoming call is of type VOICE */

  CM_CALL_TYPE_CS_DATA=1,
    /**< CS Data call type - (for modem, fax, etc. calls).
    **
    ** NOTE! Answering with this call is only successful if the
    ** incoming call is of type DATA or VOICE (with AMPS service) */

  CM_CALL_TYPE_PS_DATA=2,
    /**< Packet Switched Data call type */

  CM_CALL_TYPE_SMS=3,
    /**< SMS call type (SMS SO-6 and SO-14). CDMA only
    **
    ** NOTE! Answering with this call type is only successful
    ** if incoming call is of type SMS */

  CM_CALL_TYPE_PD=4,
    /**< Position Determination call type (SO-35 and 36). CDMA only
    ** This call type used to exchange IS-801 messages.
    */

  CM_CALL_TYPE_TEST=5,
    /**< Test call type (for Markov, Loopback, etc.). CDMA only
    **
    ** NOTE! Answering with this call type is only successful
    ** if incoming call is of type TEST */

  CM_CALL_TYPE_OTAPA=6,
    /**< OTAPA call type (OTAPA SO-18 and SO-19). CDMA only
    **
    ** NOTE! Answering with this call type is only successful
    ** if incoming call is of type OTAPA */

  /* Call types that are automatically assigned by call manager when
  ** detecting an origination dial-string that matches one of the
  ** special numbers that are stored in the phone-book.
  **
  ** NOTE! This call types should not be used to originate
  ** or answer calls.
  */

  CM_CALL_TYPE_STD_OTASP=7,
    /**< Standard OTASP call type. CDMA only
    **
    ** This call type is automatically assigned by call manager when
    ** detecting an origination dial-string that matches one of the
    ** standard OTASP numbers that are stored in the phone-book.
    **
    ** NOTE! This call type should not be used to originate
    ** or answer calls */

  CM_CALL_TYPE_NON_STD_OTASP=8,
    /**< None standard OTASP call type. CDMA only
    **
    ** This call type is automatically assigned by call manager when
    ** detecting an origination dial-string that matches one of the none
    ** standard OTASP numbers that are stored in the phone-book.
    **
    ** NOTE! This call type should not be used to originate
    ** or answer calls */

  CM_CALL_TYPE_EMERGENCY=9,
    /**< Emergency call type.
    **
    ** This call type is automatically assigned by call manager when
    ** detecting an origination dial-string that matches one of the
    ** EMERGENCY numbers that are stored in the phone-book.
    **
    ** This call type can be used by Clients also to originate calls.
    **
    ** NOTE! If call manager assigned this call type, the mode
    ** preference is forced to EMERGENCY before origination. If at the
    ** end of the call the phone is not in EMERGENCY callback mode,
    ** the mode is automatically restored to its original selection.
    ** Else the EMERGENCY mode stays in effect until changed by a client
    ** or power-cycle */

  CM_CALL_TYPE_SUPS=10,
    /**< A supplementary services command is in progress.
    ** The call type here is to facilatate call control where a sups
    ** command can be turned into a voce call or vice versa.
    */

  CM_CALL_TYPE_VT=11,
    /**< Video Telephony Call Type. Currently applicable for GSM/WCDMA only.
    */

  CM_CALL_TYPE_VT_LOOPBACK,
    /**< Video Telephony Loopback Call Type.
    ** Currently applicable for GSM/WCDMA only.
    */

  CM_CALL_TYPE_VS,
    /**< Clients should check for CM_API_VIDEOSHARE before using
    ** this call type
    **
    ** Used for Video Share calls. Differs from VT in the sense that voice
    ** does not have to be synced with video and generally it is transmitted
    ** only ine one direction from user A to B.
    */

  CM_CALL_TYPE_PS_DATA_IS707B,
    /* IS-707B Addendum Call Type. CDMA only.
    ** Used to suppress 1x registraion and send an orig(SO-33, DRS=0) msg.
    */

  CM_CALL_TYPE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_call_type_e_type;

/**
** Call processing layer failure (1X)
** Call processing layer where the call failed during origination.
** CM_CALL_ORIG_ERR_LAYER_OTHER is lower layer failure than
** CM_CALL_ORIG_ERR_LAYER_L2 and CM_CALL_ORIG_ERR_LAYER_L2 is
** lower layer failure than CM_CALL_ORIG_ERR_LAYER_L3
*/
typedef enum {
  CM_CALL_ORIG_ERR_LAYER_NONE = -1,                      /**< CM internal  */
  CM_CALL_ORIG_ERR_LAYER_OTHER,    /**< Internal failure or rejected by BS */
  CM_CALL_ORIG_ERR_LAYER_L2,                             /**< L2 failure   */
  CM_CALL_ORIG_ERR_LAYER_L3,                             /**< L3 failure   */
  CM_CALL_ORIG_ERR_LAYER_MAX                             /**< CM internal  */
} cm_call_orig_fail_layer_e_type;

/** Enumeration of service types.
**
** When originating a call, clients need to specify
** one of these service types.
**
** For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_srv_type_e {

  CM_SRV_TYPE_NONE = -1,
    /**< @internal */

  CM_SRV_TYPE_AMPS=0,
    /**< Call is to be connected over analog service.
    **
    ** Note that the client does NOT need to specify service option when
    ** originating with this call type.
    **
    ** NOTE! When originating with this service type, the mode preference
    ** might be forced to ANALOG ONLY before origination and returned to
    ** its original setting at the end of the call */

  CM_SRV_TYPE_HDR=1,
    /**< Call is to be connected over HDR service.
    ** Valid for FEATURE_HDR */

  CM_SRV_TYPE_CDMA_SPECIFIC=2,
    /**< Call is to be connected over CDMA service with specific client
    ** designated CDMA service option.
    **
    ** Note that the client need to specify service option when
    ** originating with this call type.
    **
    ** NOTE! When originating with this service type, the mode preference
    ** might be forced to DIGITAL ONLY before origination and returned to
    ** its original setting at the end of the call */

  CM_SRV_TYPE_CDMA_AUTOMATIC=3,
    /**< Call is to be connected over CDMA service where CM automatically
    ** originate with the best available service option for the specified
    ** call type.
    **
    ** Note that the client does NOT need to specify service option when
    ** originating with this call type */

  CM_SRV_TYPE_AUTOMATIC=4,
    /**< Call is to be connected over available service - AMPS or CDMA,
    ** or GSM or WCDMA or HDR.
    **
    ** Note that the client does NOT need to specify service option when
    ** originating with this srv type except for the packet data call*/

  CM_SRV_TYPE_CDMA_HDR=5,
    /**< Call is to be connected over available service - CDMA or HDR.
    **
    ** Note that the client need to specify service option when
    ** originating with this srv type for CDMA. */

  CM_SRV_TYPE_GSM_WCDMA=6,
    /**< Call is to be connected over available service - GSM or WCDMA */

  CM_SRV_TYPE_WCDMA=7,
    /**< Call is to be connected over WCDMA service */

  CM_SRV_TYPE_GSM=8,
    /**< Call is to be connected over GSM service */

  CM_SRV_TYPE_WLAN=9,
    /**< Call is to be connected over WLAN service */

  CM_SRV_TYPE_NON_AUTOMATIC=10,
    /**< Service type is non automatic */
    /**< @internal */

  CM_SRV_TYPE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_srv_type_e_type;


/** Enumeration of call end statuses
** - i.e. the reason for ending a call.
**
** For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_call_end_e {

  CM_CALL_END_NONE                         = -1,
    /**< @internal */

  /* Common
  */
  CM_CALL_END_OFFLINE                      = 0,
    /**< phone is offline */

  /* CDMA
  */
  CM_CALL_END_CDMA_LOCK                    = 20,
    /**< phone is CDMA locked until power cycle. CDMA only */

  CM_CALL_END_NO_SRV                       = 21,
    /**< phone has no service. */
    /* Backwards compatibility -- NO_CDMA_SRV & NO_GW_SRV mapped to this */

  CM_CALL_END_FADE                         = 22,
    /**< Call has ended abnormally. CDMA only */

  CM_CALL_END_INTERCEPT                    = 23,
    /**< received intercept from BS - orig only. CDMA only */

  CM_CALL_END_REORDER                      = 24,
    /**< received reorder from BS - orig only. CDMA only */

  CM_CALL_END_REL_NORMAL                   = 25,
    /**< received release from BS - no reason given. */

  CM_CALL_END_REL_SO_REJ                   = 26,
    /**< received release from BS - SO reject. CDMA only */

  CM_CALL_END_INCOM_CALL                   = 27,
    /**< received incoming call from BS */

  CM_CALL_END_ALERT_STOP                   = 28,
    /**< received alert stop from BS - incom only. CDMA only */

  CM_CALL_END_CLIENT_END                   = 29,
    /**< client ended the call */

  CM_CALL_END_ACTIVATION                   = 30,
    /**< received end activation - OTASP call only. CDMA only */

  CM_CALL_END_MC_ABORT                     = 31,
    /**< MC aborts the origination/conversation. CDMA only */

  CM_CALL_END_MAX_ACCESS_PROBE             = 32,
    /**< Max access probes transmitted. CDMA only */

  CM_CALL_END_PSIST_NG                     = 33,
    /**< Persistence test failure. FEATURE_JCDMA only. CDMA only */

  CM_CALL_END_UIM_NOT_PRESENT              = 34,
    /**< RUIM is not present */

  CM_CALL_END_ACC_IN_PROG                  = 35,
    /**< Access attempt already in progress. */

  CM_CALL_END_ACC_FAIL                     = 36,
    /**< Access failure for reason other than the above */

  CM_CALL_END_RETRY_ORDER                  = 37,
    /**< Received retry order - orig only. IS-2000. CDMA only */

  CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS      = 38,
    /**< Concurrent servive is not supported by base station */

  CM_CALL_END_NO_RESPONSE_FROM_BS          = 39,
    /**< No response recived from base station */

  CM_CALL_END_REJECTED_BY_BS               = 40,
    /**< Call rejected by the base station. CDMA only */

  CM_CALL_END_INCOMPATIBLE                 = 41,
    /**< Concurrent services requested were not compatible. CDMA only */

  CM_CALL_END_ACCESS_BLOCK                 = 42,
    /**< Access is blocked by base station. CDMA only */

  CM_CALL_END_ALREADY_IN_TC                = 43,
    /**< Corresponds to CM_CALL_ORIG_ERR_ALREADY_IN_TC */

  CM_CALL_END_EMERGENCY_FLASHED            = 44,
    /**< Call is ended due to Emergency call is flashed over this call. CDMA only */

  CM_CALL_END_USER_CALL_ORIG_DURING_GPS    = 45,
    /**< Used if CM is ending a GPS call in favor of a user call */

  CM_CALL_END_USER_CALL_ORIG_DURING_SMS    = 46,
    /**< Used if CM is ending a SMS call in favor of a user call */

  CM_CALL_END_USER_CALL_ORIG_DURING_DATA   = 47,
    /**< Used if CM is ending a DATA call in favor of an emergency call */

  CM_CALL_END_REDIR_OR_HANDOFF             = 48,
    /**< Call rejected because of redirection or handoff */

  CM_CALL_END_ACCESS_BLOCK_ALL             = 49,
    /**< Access is blocked by base station for all mobiles. KDDI specific. CDMA only */

  CM_CALL_END_OTASP_SPC_ERR                = 50,
    /**< To Support OTASP SPC Error indication */

  CM_CALL_END_IS707B_MAX_ACC               = 51,
    /**< Max Access Probes for IS-707B call. CDMA only */

  /* GSM/WCDMA
  */
  CM_CALL_END_LL_CAUSE                     = 100,
    /**< rxd a reason from lower layer,look in cc_cause. WCDMA/GSM only*/

  CM_CALL_END_CONF_FAILED,
    /**< call orig request failed. WCDMA/GSM only */

  CM_CALL_END_INCOM_REJ,
    /**< client rejected the incoming call. WCDMA/GSM only */

  CM_CALL_END_SETUP_REJ,
    /**< client rejected the setup_ind. WCDMA/GSM only */

  /* 104 */
  CM_CALL_END_NETWORK_END,
    /**< network ended the call,look in cc_cause. WCDMA/GSM only*/

  CM_CALL_END_NO_FUNDS,
    /**< GSM/WCDMA only */

  CM_CALL_END_NO_GW_SRV,
    /**< GWM/WCDMA only.  Phone has no service */

  CM_CALL_END_NO_CDMA_SRV,
    /**< 1x only.  Phone has no service */

  /* 108 */
  CM_CALL_END_NO_FULL_SRV,
    /**< Full srv unavailable. */

  CM_CALL_END_MAX_PS_CALLS,
    /**< Indicates resources not available to handle
    ** new MO/MT PS call
    */

  /* HDR
  */
  CM_CALL_END_CD_GEN_OR_BUSY               = 150,
    /**< abort connection setup due to the reception of a ConnectionDeny msg
    ** with deny code = general or network busy */

  CM_CALL_END_CD_BILL_OR_AUTH              = 151,
    /**< abort connection setup due to the reception of a ConnectionDeny msg
    ** with deny code = billing or authentication failure */

  CM_CALL_END_CHG_HDR                      = 152,
    /**< change HDR system due to redirection or PRL not preferred */

  CM_CALL_END_EXIT_HDR                     = 153,
    /**< exit HDR  due to redirection or PRL not preferred */

  CM_CALL_END_HDR_NO_SESSION               = 154,
    /**< no HDR session  */

  CM_CALL_END_CM_COLLOC_ACQ_FAIL           = 155,
    /**< only for internal CM use -Fail to acquire collocated HDR for
    ** origination*/

  CM_CALL_END_HDR_ORIG_DURING_GPS_FIX      = 156,
    /**< Used if CM is ending a HDR call orig in favor of GPS fix */

  CM_CALL_END_HDR_CS_TIMEOUT               = 157,
    /**< connection setup timeout*/

  CM_CALL_END_HDR_RELEASED_BY_CM,
    /**< CM released HDR call so 1X call can continue */

  CM_CALL_END_HOLD_DBM_IN_PROG,
    /**< CM will hold the HDR origination to allow 1X SMS to end */

  /* 160 */
  CM_CALL_END_OTASP_COMMIT_IN_PROG,

  CM_CALL_END_NO_HYBR_HDR_SRV,
    /**< Mobile has no Hybrid HDR service. */

  CM_CALL_END_HDR_NO_LOCK_GRANTED,
    /**< Call ended because HDR didn't get the RF Lock. */

  CM_CALL_END_HOLD_OTHER_IN_PROG,
    /**< CM will hold the the current call to allow other call to end */

  /* Following Call Release reason are specific to VideoTelephony Call.
  */

  CM_CALL_END_VIDEO_CONN_LOST              = 301,
    /**< modem released the call after the modem had been connected */

  CM_CALL_END_VIDEO_SETUP_FAILURE          = 302,
    /**< call setup failed while trying to setup the modem */

  CM_CALL_END_VIDEO_PROTOCOL_CLOSED        = 303,
    /**< video protocol closed after the video protocol setup was done */

  CM_CALL_END_VIDEO_PROTOCOL_SETUP_FAILURE = 304,
    /**< video protocol setup failed */

  CM_CALL_END_INTERNAL_ERROR,
    /**< CM internal error other than any of the above */

  /* WLAN
  */

  CM_CALL_END_NO_WLAN_SRV                  = 200,

  CM_CALL_END_VOIP_FAIL_WLAN               = 201,
    /**< VOIP failed on WLAN */

  CM_CALL_END_IP_FAIL                      = 202,
    /**< To be used only when CM_API_IP_CALL is defined
    ** Call origination on IP Failed
    */

  CM_CALL_END_IP_RETRY                     = 203,
    /**< To be used only when CM_API_IP_CALL is defined
    ** Call needs to be retried on IP
    */

  CM_IP_CALL_END_EMERG_ORIG                = 204,
    /**< To be used only when CM_API_IP_CALL is defined
    ** Call ended due to Emerg origination
    */

  CM_CALL_END_ORIG_THR                     = 401,
    /**< Origination throttled */

  CM_CALL_END_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_call_end_e_type;


/** Enumeration of call mode info types
*/
typedef enum cm_call_mode_info_e {

  CM_CALL_MODE_INFO_NONE,
    /**< @internal */

  CM_CALL_MODE_INFO_CDMA,
    /**< Call info is CDMA */

  CM_CALL_MODE_INFO_GW_CS,
    /**< Call info is GSM/WCDMA circuit switched */

  CM_CALL_MODE_INFO_GW_PS,
    /**< Call info is GSM/WCDMA packet switched */

  CM_CALL_MODE_INFO_IP,
    /**< To be used only if CM_API_IP_CALL is defined
    ** Call info is WLAN
    */

  CM_CALL_MODE_INFO_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_call_mode_info_e_type;


/** Enumeration of digit modes.
*/
typedef enum cm_digit_mode_e {

  CM_DIGIT_MODE_NONE=-1,
    /**< @internal */

  CM_DIGIT_MODE_4BIT_DTMF,
    /**< 4-bit DTMF */

  CM_DIGIT_MODE_8BIT_ASCII,
    /**< 8-bit ASCII */

  CM_DIGIT_MODE_SIP_URI,
    /**< Clients should check for CM_API_VIDEOSHARE before using
    ** this enum
    ** Indicates that buf in
    ** cm_num_s_type carries SIP URI.
    */

  CM_DIGIT_MODE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_digit_mode_e_type;


/** Privacy mode during a call. CDMA only.
*/
typedef enum cm_privacy_pref_e {

  CM_PRIVACY_PREF_NONE = -1,
    /**< @internal */

  CM_PRIVACY_PREF_STANDARD,
    /**< Standard privacy */

  CM_PRIVACY_PREF_ENHANCED,
    /**< Enhanced privacy */

  CM_PRIVACY_PREF_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_privacy_pref_e_type;


/** signal type (for GSM/WCDMA)*/
typedef enum cm_gw_signal_e
{
  CM_SIGNAL_NONE,
  CM_SIGNAL_REMOTE_ALERT,
  CM_SIGNAL_NETWORK_ALERT,
  CM_SIGNAL_MAX
} cm_gw_signal_e_type;

/** signal/tone  information from the network (for GSM/WCDMA)
*/
typedef enum cm_gw_tone_e {

  CM_GW_TONE_NONE = -1,
    /**< @internal */

  /* 0 */
  CM_GW_DIAL_TONE_ON,
    /**< dial tone */

  CM_GW_RING_BACK_TONE_ON,
    /**< ring back tone */

  CM_GW_INTERCEPT_TONE_ON,
    /**< intercept tone */

  CM_GW_NW_CONGESTION_TONE_ON,
    /**< network congestion */

  /* 4 */
  CM_GW_BUSY_TONE_ON,
    /**< busy tone */

  CM_GW_CONFIRM_TONE_ON,
    /**< call confirmed tone */

  CM_GW_ANSWER_TONE_ON,
    /**< answer tone */

  CM_GW_CALL_WAITING_TONE_ON,
    /**< call waiting tone */

  /* 8 */
  CM_GW_OFF_HOOK_WARNING_TONE_ON,
    /**< off hook warning tone */

  CM_GW_TONES_OFF = 0x3F,
    /**< no tones */

  CM_GW_ALERTING_TONE_OFF = 0x4F,
    /**< alerting tone off */

  CM_GW_TONE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_gw_tone_e_type;


/** Retry delay type ( FEATURE_IS2000 )
*/
typedef enum cm_retry_type_e {

  CM_RETRY_TYPE_NONE = -1,
    /**< No retry order */

  CM_RETRY_TYPE_CLEAR_ALL,
    /**< Clear all retry order */

  CM_RETRY_TYPE_ORIGINATION,
    /**< Origination retry order */

  CM_RETRY_TYPE_RESOURCE_REQ,
    /**< Resource request retry order */

  CM_RETRY_TYPE_SUPP_CH_REQ,
    /**< Supplemental channel request retry order */

  CM_RETRY_TYPE_RESERVED4,
    /**< Reserved */

  CM_RETRY_TYPE_RESERVED5,
    /**< Reserved */

  CM_RETRY_TYPE_RESERVED6,
    /**< Reserved */

  CM_RETRY_TYPE_RESERVED7,
    /**< Reserved */

  CM_RETRY_TYPE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_retry_type_e_type;


/** Number classification return status
*/
typedef enum cm_nc_status_e {

  CM_NC_ERR_NONE=-1,
    /**< @internal */

  CM_NC_OK,
    /**< No error, request was completed successfully */

  CM_NC_ERR_NULL_CALL_TYPE,
    /**< Null function pointer for resolve call type callback function */

  CM_NC_ERR_NULL_CALL_ALLOWED,
    /**< Null function pointer for call allowed callback function */

  CM_NC_ERR_NULL_CC_PROCESSING_FUNC,
    /**< Null function pointer for call control processing function */

  CM_NC_ERR_NULL_CC_PREFERENCES_FUNC,
    /**< Null function pointer for call control preferences function */

  CM_NC_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_nc_status_e_type;

/** Enumeration of supplementary system commands client are able to send to CM.
*/
typedef enum cm_sups_cmd_e {

  CM_SUPS_CMD_NONE = -1,
    /**< @internal */

  /* 0 */
  CM_SUPS_CMD_REGISTER,
    /**< Register Supplementary Service Request */

  CM_SUPS_CMD_ERASE,
    /**< Erase Supplementary Service Request */

  CM_SUPS_CMD_ACTIVATE,
    /**< Activate Supplementary Service Request */

  CM_SUPS_CMD_DEACTIVATE,
    /**< Deactivate Supplementary Service Request */

  /* 4 */
  CM_SUPS_CMD_INTERROGATE,
    /**< Interrogate Supplementary Service Request */

  CM_SUPS_CMD_PROCESS_USS,
    /**< Process Unstructured Supplementary Service */

  CM_SUPS_CMD_REG_PASSWORD,
    /**< Register Password Supplementary Service Request */

  CM_SUPS_CMD_ABORT,
    /**< Abort Supplementary Service Request */

  /* 8 */
  CM_SUPS_CMD_RELEASE,
    /**< Release Supplementary Service Request */

  CM_SUPS_CMD_GET_PASSWORD_RES,
    /**< Get Password Response */

  CM_SUPS_CMD_USS_NOTIFY_RES,
    /**< Unstructured Supplementary Service Notify Response */

  CM_SUPS_CMD_USS_RES,
    /**< Unstructured Supplementary Service Response */


  CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES,
    /**< Location Services Location Notification Supp Serv Response */

  CM_SUPS_CMD_LCS_MOLR,
    /**< Location Services LCS MOLR Request */

  CM_SUPS_CMD_LCS_MOLR_COMPLETED,
    /**< Location Services LCS MOLR Completed */



  CM_SUPS_CMD_ERASE_CC_ENTRY,
    /**< Request to deactivate one or all pending ccbs requests at
    ** the network. Does not have a CM API to call this directly.
    ** Gets mapped from cm_sups_cmd_deactivate ()
    */

  CM_SUPS_CMD_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_sups_cmd_e_type;


/** Enumeration of call command supplementary service type
*/
typedef enum cm_call_sups_type_e {

  CM_CALL_SUPS_TYPE_NONE = -1,
    /**< @internal */

  CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB,
    /**< 0 SEND - Release all held calls or
    **          set User Determined User Busy (UDUB)
    **          for a waiting call. */

  CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL,
    /**< 1 SEND - Release all active calls and
    **          accept other call (held or waiting) */

  CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL,
    /**< 1 X SEND - Release a specific call X */

  CM_CALL_SUPS_TYPE_HOLD_CALL,
    /**< 2 SEND - Place all active calls on hold and
    **          accept another call (held or waiting) */

  CM_CALL_SUPS_TYPE_SELECT_CALL_X,
    /**< 2 X SEND - Place all active calls on hold except X
    **            with which communication is still established. */

  CM_CALL_SUPS_TYPE_MULTIPARTY_CALL,
    /**< 3 SEND - Add a held call to the conversation,
    **          Multiparty call */

  CM_CALL_SUPS_TYPE_CALL_DEFLECTION,
    /**< 4 * DN SEND - Deflect an incoming call */

  CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER,
    /**< 4 SEND - Explicit call transfer */

  CM_CALL_SUPS_TYPE_CALL_FORWARD,
    /**< Call forward */

  CM_CALL_SUPS_TYPE_EMERGENCY_CALL,
    /**< CDMA - indicates emergency call */

  CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD,
    /**< CDMA - Activate answer hold */

  CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD,
    /**< CDMA - Deactivate answer hold */

  CM_CALL_SUPS_TYPE_SIMPLE_FLASH,
    /**< CDMA - Send it as regular flash */

  CM_CALL_SUPS_TYPE_ACT_CCBS_REQ,
    /**<
    ** 5 SEND - Activates the Completion of Calls to Busy Subscriber Request
    ** GW only. Response to CCBS_ALLOWED indication from network
    */

  CM_CALL_SUPS_TYPE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_call_sups_type_e_type;


/**
** Enumeration of error codes used by cm_num_validate.
*/
typedef enum cm_num_err_e {

  CM_NUM_ERR_NONE = -1,
    /**< @internal */

  CM_NUM_ERR_NOERR,
    /**< No errors found */

  CM_NUM_ERR_NULL_PTR,
    /**< Null pointer */

  CM_NUM_ERR_INVALID_NUM_LENGTH,
    /**< Invalid number length */

  CM_NUM_ERR_INVALID_NUM_TYPE,
    /**< Invalid number type */

  CM_NUM_ERR_INVALID_NUM_PLAN,
    /**< Invalid number plan */

  CM_NUM_ERR_INVALID_PI,
    /**< Invalid presentation indicator */

  CM_NUM_ERR_INVALID_SI,
    /**< Invalid screening indicator */

  CM_NUM_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_num_err_e_type;

/** Enumeration of call direction.
*/
typedef enum cm_call_direction_e {
  CM_CALL_DIRECTION_NONE,
    /**< Undefined Direction */

  CM_CALL_DIRECTION_MO,
    /**< Mobile Originated Call */

  CM_CALL_DIRECTION_MT,
    /**< Mobile Terminated Call */

  CM_CALL_DIRECTION_MAX
  /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_call_direction_e_type;

/** Enumeration of source of subscription information.
*/
typedef enum cm_cc_subscription_source_e {
  CM_CC_SUBSCRIPTION_SOURCE_NONE,
    /**< No Subscription information */

  CM_CC_SUBSCRIPTION_SOURCE_SIM_1,
    /**< Subscription information is from SIM 1 */

  CM_CC_SUBSCRIPTION_SOURCE_SIM_2,
    /**< Subscription information is from SIM 2 */

  CM_CC_SUBSCRIPTION_SOURCE_NV,
    /**< Subscription information is from NV */

  CM_CC_SUBSCRIPTION_SOURCE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif  /* FEATURE_RPC */

} cm_cc_subscription_source_e_type;

/** Enumeration of Call control mode preference.
*/
typedef enum cm_cc_mode_pref_e {
  CM_CC_MODE_PREF_NONE       = 0,
    /**< Reject the call */

  CM_CC_MODE_PREF_ANY,
    /**< Use any mode that is available */

  CM_CC_MODE_PREF_CDMA_ONLY,
    /**< Use only cdma to originate call */

  CM_CC_MODE_PREF_GW_ONLY,
    /**< Use only gw to originate call */

  CM_CC_MODE_PREF_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif  /* FEATURE_RPC */

} cm_cc_mode_pref_e_type;

/** Enumeration of Call control status.
*/
typedef enum cm_cc_status_e {
  CM_CC_STATUS_NONE = -1,
    /**< @internal */

  /* 0 */
  CM_CC_STATUS_SUCCESS,
    /**< Call Control Processing Successful */

  CM_CC_STATUS_ERROR,
    /**< Call Control Processing Error */

  CM_CC_STATUS_DUPLICATE_REGISTRATION,
    /**< Call Control Duplicate Registration */

  CM_CC_STATUS_CLIENT_NOT_REGISTERED,
    /**< Call Control Client Not Registered */

  /* 4 */
  CM_CC_STATUS_PROFILE_VALUE_NOT_ALLOWED,
    /**< Call Control Status Profile Value Is Not Allowed */

  CM_CC_STATUS_PROFILE_ALREADY_SENT,
    /**< Call Control Profile Already Sent */

  CM_CC_STATUS_MEMORY_ERROR,
    /**< Call Control Memory Error */

  CM_CC_STATUS_CLIENT_SPACE_FULL,
    /**< Call Control Client Space Full */

  /* 8 */
  CM_CC_STATUS_BAD_PARAM,
    /**< Call Control Status Bad Param */

  CM_CC_STATUS_NULL_INPUT_PARAM,
    /**< Call Control Null Input Param */

  CM_CC_STATUS_PARAM_EXCEED_BOUNDARY,
    /**< Call Control Param Exceeded the Boundary */

  CM_CC_STATUS_INVALID_LENGTH,
    /**< Call Control Status Invalid Length */

  /* 12 */
  CM_CC_STATUS_EXTRA_PARAM_MISSING,
    /**< Call Control Extra Param Missing */

  CM_CC_STATUS_EXTRA_PARAM_NOT_REQUIRED,
    /**< Call Control Extra Param Not Required */

  CM_CC_STATUS_UNSUPPORTED_COMMAND,
    /**< Call Control Unsupported Command */

  CM_CC_STATUS_INVALID_COMMAND,
    /**< Call Control Invalid Command */

  /* 16 */
  CM_CC_STATUS_INSUFFICIENT_INFO,
    /**< Call Control Insufficient Info */

  CM_CC_STATUS_FILE_ACCESS_FAIL,
    /**< Call Control Status File Access Fail */

  CM_CC_STATUS_NOT_INIT,
    /**< Call Control Status Not Init */

  CM_CC_STATUS_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif   /* FEATURE_RPC */

} cm_cc_status_e_type;


/** Enumeration of Call control result.
*/
typedef enum cm_cc_result_e {

  CM_CC_RESULT_NONE = -1,
    /**< @internal */

  /* 0 */
  CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS,
    /**< Call Allowed, Call control did not make any modifications */

  CM_CC_RESULT_NOT_ALLOWED,
    /**< Call Not Allowed */

  CM_CC_RESULT_ALLOWED_BUT_MODIFIED,
    /**< Call Allowed, But there were modifications */

  CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_VOICE,
    /**< Call Allowed, Call type was changed to voice */

  /* 4 */
  CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_SS,
    /**< Call Allowed, Call type was chanegd to SS */

  CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD,
    /**< Call Allowed, Call type was changed to USSD */

  CM_CC_RESULT_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif   /* FEATURE_RPC */

} cm_cc_result_e_type;


typedef enum cm_call_modify_e{

  CM_CALL_MODIFY_NONE  = -1,
    /**< @internal */

  CM_CALL_MODIFY_NO_CHANGE = 0,
    /**< No Modification for this call. */

  CM_CALL_MODIFY_VOCIE = 1,
    /**< Modify the call to Voice Only */

  CM_CALL_MODIFY_VT    = 2,
    /**< Modify the call to VT */

  CM_CALL_MODIFY_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif   /* FEATURE_RPC */

}cm_call_modify_e_type;

/** Enumeration of Data coding schemes used by USSD
*/
/* Clients need to check for CM_API_USSD_DCS before
** using this enumeration
*/
typedef enum cm_ussd_dcs_e {

  CM_USSD_DCS_NONE = -1,
    /**< @internal */

  CM_USSD_DCS_7_BIT = 0x00,
    /**< 7 bit Data encoding scheme used for ussd */

  CM_USSD_DCS_8_BIT = 0x04,
    /**< 8 bit Data encoding scheme used for ussd */

  CM_USSD_DCS_UCS2  = 0x08,
    /**< Universal multi-octet character set encoding
    ** Clients need to check for CM_API_USSD_DCS
    ** and CM_API_USSD_UCS2_DCS
    */

  CM_USSD_DCS_UNSPECIFIED = 0x0F,
    /**< Data encoding scheme unspecified */

  CM_USSD_DCS_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif   /* FEATURE_RPC */

} cm_ussd_dcs_e_type;

/* Enumeration for values carried by handset based plus dial */
typedef enum cm_hs_based_plus_dial_e {

  CM_HS_BASED_PLUS_DIAL_NONE = -1,
    /**< @internal */

  CM_HS_BASED_PLUS_DIAL_DISABLED = 0,
    /**< Handset Based Plus Code dial disabled */

  CM_HS_BASED_PLUS_DIAL_ENABLED = 1,
    /**< Handset Based Plus Code enabled */

  CM_HS_BASED_PLUS_DIAL_ENABLED_ACCEPT_NETW_SOLN = 2,
    /**< Plus dial enabled. If country table
  ** entry shows network based solution then
    ** network based will be used. For future use.
  */

  CM_HS_BASED_PLUS_DIAL_ENABLED_IGNORE_NETW_SOLN = 3,
    /**< Plus dial enabled. Even if country table entry
  ** shows network based solution is avail, CM will
     ** still use handset based solution. For future use
  */

  CM_HS_BASED_PLUS_DIAL_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif   /* FEATURE_RPC */

} cm_hs_based_plus_dial_e_type;


/** Progress states for
** CM_CALL_EVENT_PROGRESS_INFO_IND.
**
** Clients needs to check for CM_API_IPCALL_SS_INFO
** before using this type
*/
typedef enum cm_call_prog_info_e {

  CM_CALL_PROG_INFO_NONE = -1,
    /**< @internal */

  CM_CALL_PROG_INFO_REMOTE_ALERT,
    /**< Indicates that remote party is being alerted
    ** for MO call
    */

  CM_CALL_PROG_INFO_MAX
    /**< @internal */

} cm_call_prog_info_e_type;


/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** Structure holds warning information
** sent by IP app during registration or ending
** of an ip call.
** Clients needs to check for CM_API_VIDEOSHARE
** before using this type.
*/
typedef struct cm_ipapp_warn_info_s {

  boolean   present;
    /**< TRUE   - Warning is present
    ** FALSE  - Warning is not present
    */

  int   cause;
    /**< Warning code - SIP RFC 3261, sect 20.43
    */

  char  hostname[CM_IPAPP_HOST_SIZE];
    /**< Null terminated host name that
    ** generates the warning
    */

  char  description[CM_IPAPP_TEXT_SIZE];
    /**< Null terminated Warning text.
    */

} cm_ipapp_warn_info_s_type;


/** Structure to store ccbs indexes
** Clients need to check for CM_API_CCBS before using
** this structure
*/
typedef struct {

    cm_ccbs_indx_type              ccbs_index;
      /**< ccbs index value stored
      */

    cm_called_party_bcd_no_s_type  called_party;
      /**< called party number for the corresp call
      */

} cm_ccbs_indx_store_s_type;


/* Type for ccbs information returned to clients
** Clients need to check for CM_API_CCBS before using
*/
typedef struct cm_ccbs_store_info_s {

    int                         cm_ccbs_store_info_len;
      /**< Total entries present in ccbs store
      */

    cm_ccbs_indx_store_s_type   ccbs_indx_store[CM_MAX_CCBS_REQ];
      /**< Carries ccbs index and called number
      */
} cm_ccbs_store_info_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_ccbs_store_info_s.ccbs_indx_store VARRAY CM_MAX_CCBS_REQ
    LENGTH cm_ccbs_store_info_s.cm_ccbs_store_info_len */
#endif /**< FEATURE_HTORPC_METACOMMENTS */

/** Type for CM signal types.
*/
typedef struct cm_signal_s {

  boolean     is_signal_info_avail;
    /**< Indicate whether signal information is available.
    **
    ** NOTE! if this variable is FALSE, the values found in other
    ** fields of this structure as NO meaning */

  uint8       signal_type;
    /**< Signal type - Tone Signal, ISDN Alerting or IS-54B Alerting */

  uint8       alert_pitch;
    /**< Alert pitch - Medium, High or Low */

  uint8       signal;
    /**< Indicate which Tone signals, ISDN Alerting or IS_54B Alerting */

} cm_signal_s_type;


typedef struct cm_subaddress_s
{
  boolean   extension_bit;
    /**< The extension bit */

  uint8     subadd_type;
    /**< Type of subaddress - NSAP/USER */

  uint8     odd_even_ind;
    /**< The indicator of even/odd bits - EVEN/ODD */

  uint8     num_char;
    /**< Number of characters in subaddress array */

  uint8     chari[CM_MAX_SUBADDRESS_DIGITS];
    /**< Array of subaddress digits */

} cm_subaddress_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_subaddress_s.chari VARRAY CM_MAX_SUBADDRESS_DIGITS
    LENGTH cm_subaddress_s.num_char */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/** Type for CM number.
** Able to store a dialed number or a caller ID number.
**
** NOTE! In the case where the digit_mode parameter is set to
** CM_DIGIT_MODE_4BIT_DTMF the number buffer may only contain
** 0-9, "#", or "*" characters.
**
** In the case where the digit_mode parameter is set to
** CM_DIGIT_MODE_8BIT_ASCII the number buffer may contain any
** 8-bit ASCII characters.
**
** Digital calls include CDMA and GSM/WCDMA but not AMPS.
**
** number_type should be of type cm_num_type_e_type.
** number_plan should be of type cm_num_plan_e_type.
*/
typedef struct cm_num_s {

  uint8                    buf[CM_MAX_NUMBER_CHARS];
    /**< Number buffer */

  uint8                    len;
    /**< Length of number stored in num_buf */

  cm_digit_mode_e_type     digit_mode;
    /**< Origination digit mode */

  uint8                    number_type;
    /**< Number type */

  uint8                    number_plan;
    /**< Numbering Plan */

  cm_npi_type              pi;
    /**< Presentation Indicator */

  cm_si_type               si;
    /**< Screening Indicator */

  cm_subaddress_s_type     subaddr;
    /**< Subaddress */

  cm_emerg_srv_categ_type  emerg_srv_categ;
    /**< Clients needs to check for CM_API_ECATEG_IN_NUM_STRUCT before
    ** using this field.
    **
    ** Applies only when call type is EMERGENCY.
    ** Holds service category bits that are or'ed
    ** These bits come from CM_EMERG_NUM_TYPE_xx.
    ** Example - CM_EMERG_NUM_TYPE_POLICE_MASK
    */

} cm_num_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_num_s.buf VARRAY CM_MAX_NUMBER_CHARS LENGTH cm_num_s.len */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/** Type for Redirecting Numbers.
*/
typedef struct cm_redirecting_num_s {

  cm_num_s_type                num;
    /**< Number that is redirecting */

  cm_redirecting_reason_e_type redirecting_reason;
    /**< Optional param -- redirecting reason
    ** REDIRECTING_REASON_UNKNOWN if no included.
    */

}cm_redirecting_num_s_type;

/** Type for CM alpha.
** Able to store alpha tag text.
*/
typedef struct cm_alpha_s {

  uint8    buf[CM_MAX_ALPHA_TAG_CHARS];
    /**< Alpha buffer */

  uint8    len;
    /**< Length of alpha tag stored in alpha_buf */

} cm_alpha_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_alpha_s.buf VARRAY CM_MAX_ALPHA_TAG_CHARS LENGTH cm_alpha_s.len */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* Type to hold the country code */
typedef uint16 cm_country_code_type;

/* Type to hold sid */
typedef uint16 cm_sid_type;


/* Number type for conversion from plus to digits or digits to plus */
typedef struct cm_num_details_for_conversion_s {

  cm_hs_based_plus_dial_e_type  hs_based_plus_dial_setting;
     /* NV flag regarding handset based given by CM_SS_EVENT_SRV_CHANGED */

  cm_country_code_type          home_mobile_country_code;
    /* Home mcc as given by CM_SS_EVENT_SRV_CHANGED */

  cm_country_code_type          current_mobile_country_code;
    /* Current mcc as given by CM_SS_EVENT_SRV_CHANGED */

  cm_call_type_e_type           call_type;
    /* Call type this number is associated with. Voice, SMS */

  cm_num_s_type                 num;
     /* CM number type carrying the number for conversion*/

} cm_num_details_for_conversion_s_type;


/* Type for CM alpha with coding scheme.
** Able to store
** - Encoded CNAP string
** - Number of characters in the decoded string
** - Number of bytes in the encoded data
** - Coding scheme used
*/
typedef struct cm_enc_alpha_s {

  byte     buf[CM_MAX_ALPHA_TAG_BYTES];
    /* Alpha buffer */

  uint8    num_characters;
    /* Length of alpha tag stored in alpha_buf */

  uint8    num_bytes;
    /* Length of alpha tag stored in alpha_buf */

  uint8    coding_scheme;
    /* Coding scheme of data */

} cm_enc_alpha_s_type;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_enc_alpha_s.buf VARRAY CM_MAX_ALPHA_TAG_BYTES LENGTH cm_enc_alpha_s.num_bytes */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/** Extended Burst Type International message - FEATURE_T53
*/
typedef struct cm_ext_brst_intl_s {

  uint16   mcc;
   /**< Mobile Country Code (fixed '0101010011')*/

  uint8    db_subtype;
   /**< Data Burst subtype (fixed '000001') */

  uint8    chg_ind;
   /**< Charge Indication */

  uint8    sub_unit;
   /**< Unit call time (1/10 second) */

  uint8    unit;
   /**< Unit call time (second) */

} cm_ext_brst_intl_s_type;


/** National Supplementary services - CLIR - FEATURE_T53
*/
typedef struct cm_nss_clir_rec_s {

  uint8   cause;
    /**< Reason code */

} cm_nss_clir_rec_s_type;


/** National Supplementary services - Audio Control - FEATURE_T53
*/
typedef struct cm_nss_aud_ctrl_s {

  uint8   up_link;
    /**< Audio path from the MS to the BS */

  uint8   down_link;
    /**< Audio path from the BS to the MS */

} cm_nss_aud_ctrl_s_type;


/** Retry order info
*/
typedef struct cm_retry_order_s {

  cm_retry_type_e_type retry_type;
    /**< Retry delay type */

  uint16               srv_opt;
    /**< Service option   */

  boolean              infinite_delay;
    /**< True if infinite delay, it should always be FALSE for origination */

  uint32               retry_delay_in_ms;
    /**< Only meaningful if infinite_delay is FALSE */

} cm_retry_order_s_type;


/** Type for holding optional supplementary service parameters
*/
typedef struct cm_call_sups_params_s {

  cm_call_id_type   call_id;
    /**< Optional call ID of the call */

  cm_num_s_type     sups_number;
    /**< Optional User supplied number */

  uint8             cause;
    /* Optional cause value for GSM/WCDMA N/Nx SEND command */

} cm_call_sups_params_s_type;


/** Redirected call information */
typedef struct cm_route_dest_number_s
{
  boolean                           present;
  routing_number_option_s_type      type;
  cm_called_party_bcd_no_s_type     number[MAXNO_FORWARD_TO_NUMBER];
  cm_called_party_subaddress_s_type address;

} cm_route_dest_number_s_type;


/** Explicit Call Tranfer indicator */
typedef struct cm_ect_indicator_s
{
  boolean                         present;
  ect_call_state_s_type           callState;
  cm_route_dest_number_s_type     routeDestNumber;

} cm_ect_indicator_s_type;

/** Call Complete on Busy Subscriber indication
** Clients need to check for CM_API_CCBS before using
*/
typedef struct cm_ccbs_indicator_s
{
  boolean            present;
    /**< TRUE if this structure is valid */

  cm_ccbs_cnf_e_type ccbs_cnf;
    /**< CM_CCBS_CNF_RET_RES - Check ccbs_indx_present flag and then ccbs_indx
    ** CM_CCBS_CNF_RET_ERR and CM_CCBS_CNF_REJ - Look inside ss_error within
    ** cm_gw_cs_call_info_s_type.
    */

  boolean            ccbs_indx_present;
    /**< TRUE if ccbs indx is present
    */

  cm_ccbs_indx_type  ccbs_indx;
    /**< Actual ccbs indx
    */

} cm_ccbs_ret_res_s_type;

typedef struct cm_call_ss_info_s
{
  ie_ss_status_s_type        ss_status;
  ie_ss_code_s_type          ss_code;
  ie_ss_notification_s_type  ss_notification;
}cm_call_ss_info_s_type;

/** Forward declaration of cm_mm_call_info_s_type for
** use by event_user_data function
*/
typedef struct cm_mm_call_info_s cm_mm_call_info_s_type;


/** Used by clients to query for user data
** signaling messages after receiving call
** event related to user data.
** Clients need to check for CM_API_USER_USER_DATA before
** using.
*/
typedef struct {
     cm_user_data_s_type      mt_user_data;
       /**< Data from remote user in case
       ** of CM_CALL_EVENT_USER_DATA_IND
       */

     boolean                     is_receiver_busy;
       /**< Used for CM_CALL_EVENT_USER_DATA_CONG_IND
       */

     ie_cm_cc_cause_s_type       cm_cc_cause;
       /**< Used for CM_CALL_EVENT_USER_DATA_CONG_IND
       */
} cm_call_event_user_data_s_type;

/** Type for function to be called by clients to retrieve
** information regarding user to user data.
** Clients need to check for CM_API_USER_USER_DATA before
** using.
*/
typedef void (cm_call_event_user_data_f_type) (

    cm_call_event_user_data_s_type  *call_event_user_data,
      /**< Structure holding user data corresponding to events */

    const cm_mm_call_info_s_type    *call_info_ptr
        /**< pointer to call info pointer sent to the clients */
);


/** Circuit switched Call information returned with each event
*/
typedef struct cm_gw_cs_call_info_s {

  uint8                               transaction_id;
    /**< Call transaction ID */

  boolean                             accept;
    /**< Accept the call */

  boolean                             success;
    /**< Confirmation message from the lower layer is successful */


  cm_repeat_indicator_s_type          bc_repeat_indicator;
    /**< Bearer capability repeat indicator - included only if
    ** bearer capability 1 and bearer capability 2 are included */

  cm_bearer_capability_s_type         bearer_capability_1;
    /**< Bearer Capability 1 */

  cm_bearer_capability_s_type         bearer_capability_2;
    /**< Bearer Capability 2*/

  cm_repeat_indicator_s_type          llc_repeat_indicator;
    /**< LLC repeat indicator - included only if BC repeat indicator
    ** and low layer compatibility 1 are included */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_1;
    /**< Low Layer Compatibility 1 - included in the Network -> UE
    ** direction if the calling user specifies a  low layer
    ** compatibility */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_2;
    /**< Low Layer Compatibility 2 - included only if LLC repeat indicator
    ** is included */

  cm_repeat_indicator_s_type          hlc_repeat_indicator;
    /**< HLC repeat indicator - included only if BC repeat indicator
    ** and high layer compatibility 1 are included */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_1;
    /**< High Layer compatibility 1 - included in the Network -> UE
    ** direction if the calling user specifies a  high layer
    ** compatibility
    */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_2;
    /**< High Layer compatibility 2 - included only if HLC repeat
    ** indicator is included */

  cm_called_party_bcd_no_s_type       called_party_bcd_number;
    /**< Called party BCD number */

  cm_called_party_subaddress_s_type   called_party_subaddress;
    /**< Called party subaddress */

  cm_calling_party_bcd_no_s_type      calling_party_bcd_number;
    /**< Calling party BCD number */

  cm_calling_party_subaddress_s_type  calling_party_subaddress;
    /**< Calling party subaddress */

  cm_num_s_type                       redirect_party_number;
    /**< Redirecting party number */

  cm_forward_cug_info_s_type          forward_cug_info;
    /**< Closed user Group information */

  uint32                              rab_id;
    /**< Radio Access Bearer ID
    ** Valid only if rab_id_present is TRUE
    **
    ** Clients need to check for CM_API_CS_RABID before
    ** using this field.
    */

  uint8                               cm_clir;
    /**< CLIR information - CLIR suppression / invocation etc. */

  ie_cm_cc_cause_s_type               cc_cause;
    /**< Call control cause information - eg: */

  ie_cm_cc_reject_s_type              cc_reject;

  ie_cm_mo_call_details_s_type        call_details;
    /**<  Bearer capability information for mobile originated
    **  calls */

  ie_cm_ss_error_s_type               ss_error;
    /**< Supplementary service error */

  boolean                             call_ss_success;
    /**< Flag to indicate whether multiple call management
    ** is successful or not */

  active_calls_list_s_type            active_calls_list;
    /**< Active call ID list from multiple calls management
    ** confirmation */

  cm_connected_no_s_type              connected_num;
    /**< Connected number */

  cm_connected_subaddress_s_type      connected_subaddress;
    /**< Connected subaddress */

  cm_call_sups_type_e_type            sups_type;
     /**< Supplementary services type */

  cm_call_sups_params_s_type          sups_params;
    /**< supplementary service specific data */

  uint8                               call_progress;
    /**< call origination progress information */

  cm_cause_of_no_cli_s_type           cause_of_no_cli;
    /**< cause for no CLI in mobile terminated call */

  cm_ect_indicator_s_type             ect_info;
    /**< explicit call transfer information */

  cm_call_ss_info_s_type              ss_info;
    /**< call related SS information */


  #if (defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM) || \
       defined(FEATURE_MM_SUPERSET))
  sys_is2000_msg_s_type               umts_cdma_ho_info;
    /**< UMTS to 1X handoff information.
    */
  #endif

 #if (defined(FEATURE_UUS) || defined(FEATURE_MM_SUPERSET))
  cm_call_event_user_data_f_type     *user_data_func;
  #endif

  boolean                             is_user_data_present;
    /**< Can be used by clients to check if user_data_func
    ** can be called to return valid values.
    */

  cm_ccbs_ret_res_s_type             ccbs_ret_res;
    /**< Clients need to check for CM_API_CCBS before
    ** using this structure. Carries CCBS specific
    ** parameters for the clients
    */

  cm_ccbs_recall_rsp_e_type          ccbs_recall_rsp;
    /**< Clients need to check for CM_API_CCBS before
    ** using this structure. Carries recall response
    ** sent by the client for Recall indication
    */

  cm_call_type_e_type                new_call_type_req;
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
    ** using this field.
    ** New call type that is requested by in-call modification api
    ** cm_mm_call_cmd_modify_req()
    */

  boolean                            incall_modif_err_status;
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
    ** using this field.
    ** Error status returned by MN as a result of in-call modification
    ** request
    ** TRUE  - Error in processing in-call modification req.
    ** FALSE - No error in processing in-call modification req.
    */

  boolean                             is_call_mod_possible;
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
    ** using this field.
    **
    ** Used by UI clients to display icon that can be chosen by user to
    ** perform call modifications.
    **
    ** TRUE   - Call can be modified. Voice to VT and VT to Voice.
    ** FALSE  - Call cannot be modified.
    */

  boolean                             gw_cs_modify_res;
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
    ** using this field.
    ** Client response to modification request
    ** TRUE   - Modification got accepted by client
    ** FALSE  - Modification got rejected by client
    */

  boolean                             rab_id_present;
    /**< TRUE   - rab_id carries valid value
    ** FALSE  - rab_id does not carry valid value
    **
    ** Clients need to check for CM_API_CS_RABID before
    ** using this field.
    */

  boolean                             is_network_initiated;
    /* Clients need to check for CM_API_VT_FALLBACK_TO_VOICE before
    ** using this field.
    **
    ** Used by UI clients to tailor messages/prompts indicating whether
    ** network or remote-user is responsible for this event.
    **
    ** TRUE   - Network initiated event
    ** FALSE  - Remote-user initiated event
    */

} cm_gw_cs_call_info_s_type;


/** Packet switched Call information returned with each event
*/
typedef struct cm_gw_ps_call_info_s {

  nsapi_T                         nsapi;
    /**< NSAPI information */

  qos_T                           qos;
    /**< Quality of Service information */

  qos_T                           minimum_qos;
    /**< The minimum quality of service allowed for the
    ** PDP context */

  pdp_address_T                   pdp_addr;
    /**< Packet data protocol address */

  protocol_cfg_options_T          config_options;
    /**< Protocol Configuration Options */

  apn_T                           apn_name;
    /**< Access point  name - a label or a full
    ** qualified domain name */

  sys_pdp_header_comp_e_type      h_comp;
    /**< IP Header Compression Required
    **
    ** Clients need to check for CM_API_PDP_COMPR before
    ** using this enum
    */

  sys_pdp_data_comp_e_type        d_comp;
    /**< Data Compression Required
    **
    ** Clients need to check for CM_API_PDP_COMPR before
    ** using this enum
    */

  ti_T                            tear_down_ind;
    /**< Tear down indicator - indicates whether
    ** only the PDP context associated with this specific
    ** TI or all active PDP contexts sharing the same PDP
    ** address associated with this specific TI shall be deactivated. */

  cause_enum_type_T               pdp_cause_type;
    /**< PDP cause type */

  pdp_cause_T                     cause;
    /**< PDP cause */

  rabm_rabid_T                    rabid;
    /**< RAB Id */

  rabmcm_cause_T                  rab_cause;
    /**< RAB cause */

  cm_call_id_type                 primary_call_id;
    /**< Call Id of the primary PDP context.
       This field is only used if this is
       a secondary PDP context origination
    */

  uint16                            profile_number;
    /**< The PDP profile number used for this
    ** PDP context
    */

  #if (defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM) || \
       defined(FEATURE_MM_SUPERSET))
  sys_is2000_msg_s_type               umts_cdma_ho_info;
    /**< UMTS to 1X handoff information.
    */
  #endif

  byte                            pdp_group_id;
   /**< Clients check CM_API_SEC_PDP for SEC PDP support
   ** Group id of the primary context
   ** that is associated with the
   ** secondary PDP context origination
   */

 uint16                            assoc_pri_profile_num;
   /**< Clients check CM_API_SEC_PDP for SEC PDP support
   ** For Primary context, holds own profile number.
   ** For Seconadry context, profile of associated
   ** primary.
   */

 boolean                         is_net_alloc_qos_diff_from_req;
   /**< Clients check CM_API_SEC_PDP for SEC PDP support
   ** Valid for only Sec PDP context
   ** Sec PDP origination's requested qos and
   ** minimum_qos decide this value
   ** FALSE - Nw has allocated qos in between requested and minimum_qos
   ** TRUE  - Nw has allocated different qos
   */

 cm_ps_sig_rel_status_T          ps_sig_rel_status;
   /* Status of PS Signaling release request that was
   ** initiated by client
   */

} cm_gw_ps_call_info_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_gw_ps_call_info_s.cause DISC cm_gw_ps_call_info_s.pdp_cause_type */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/* GAN status information structure. 
*/
typedef struct cm_gw_gan_info_s{

    sys_gw_gan_state_e_type                       gan_state;
        /**< gan state - init or shutdown  */

    sys_gw_gan_conn_status_e_type                 gan_conn_status;
        /**< GAN connection status */

    sys_gw_gan_sig_chan_status_e_type             gan_sig_chan_status;
        /**< signal channel status */

    sys_gw_gan_mode_display_s_type                gan_mode_display;
         /**< mode display - indicates if 
         **   WLAN or GERAN is the active RAT
         */

    sys_gw_gan_error_s_type                       gan_error;
        /**< gan error information 
        **   category and codepoint
         */

}cm_gw_gan_info_s_type;


/** CDMA call information returned with each event
** Applicable for targets that support CDMA
*/
typedef struct cm_cdma_call_info_s {

  uint16                          srv_opt;
    /**< Service option as defined in CAI.H file
    **
    ** NOTE! The service option takes on the CM_SO_ANALOG
    ** value (defined in this file) when the call is connected
    ** (or to be connected) over analog service */

  cm_privacy_pref_e_type          privacy_pref;
    /**< NOTE: This is the user preference. It is different from is_privacy
    ** which indicates the actual status of privacy during the call. */

  boolean                         is_privacy;
    /**< Indicate whether privacy mode is on */

  boolean                         is_otasp_commit;
    /**< Indicate whether OTASP/OTAPA activation is completed */

  cm_otasp_status_e_type          otasp_status;
    /**< Indicate status of OTASP call */

  boolean                         is_last_cdma_info_rec;
    /**< this field indicates whether this is the last
    ** CDMA info record or not */

  cm_call_sups_type_e_type        sups_type;
      /**< Supplementary services (flash) type */

  cm_call_id_type                 emergency_flash_call_id;
      /**< The call used in sending emergency flash */

  boolean                         emergency_flash_over_incoming_call;
      /**< whether a previous emergency request was flashed on this
      incoming call or not */

  boolean                         qos_parms_incl;
    /**< Qos Parameters presense indicator. TRUE - Present; FALSE - Not */

  uint8                           qos_parms_len;
    /**< Length of QOS parameter list */

  uint8                           qos_parms[CM_ORIG_QOS_LEN_MAX];
    /**< QOS parameters list */

  uint8 end_reason;
    /**< the reason for ending a call, it's different from end_status to keep
    ** the old interface unchanged while adding more IS2000 specific reasons */

  cm_retry_order_s_type           retry_order;
    /**< retry order info */

  uint8                           ext_disp_type;
    /**< display type for extended disaply info record - FEATURE_IS95B_EXT_DISP */

  boolean                         alert_ans_bypass;
    /**< Indicate whether the incoming call need to be answered by the clients
    ** FEATURE_IS95B_ALERT_ANS_BYPASS */

  boolean                         ndss_in_progress;
    /**< MC is perfoming NDSS redirection - FEATURE_IS95B_NDSS */

  cm_ext_brst_intl_s_type         ext_brst_intl_msg;
    /**< Contains charge rate information - FEATURE_T53 */

  cm_nss_clir_rec_s_type          nss_clir_rec;
    /**< National Supplementary services - CLIR - FEATURE_T53 */

  cm_nss_aud_ctrl_s_type          nss_aud_ctrl_rec;
    /**< National Supplementary services - Audio Control - FEATURE_T53 */

  sys_umts_cdma_handover_e_type   umts_cdma_ho_status;

  sys_ho_status_e_type            ho_status;

  /**< Line control information
  */

  boolean                              line_ctrl_polarity_included;
    /**< TRUE if polarity info included
    */

  boolean                              line_ctrl_toggle;
    /**< TRUE = toggle polarity
    */

  boolean                              line_ctrl_reverse;
    /**< TRUE = reverse polarity
    ** FALSE = normal polarity
    */

  byte                                 line_ctrl_power_denial;
    /**< Power denial time
    */

  cm_call_orig_fail_layer_e_type       orig_fail_layer;
    /**< Highest layer where the call failed during origination
    ** This field is adjusted after every silent redial failure during
    ** origination.
    ** CM_CALL_ORIG_ERR_LAYER_OTHER is lower layer failure than
    ** CM_CALL_ORIG_ERR_LAYER_L2 and CM_CALL_ORIG_ERR_LAYER_L2 is
    ** lower layer failure than CM_CALL_ORIG_ERR_LAYER_L3
    */

} cm_cdma_call_info_s_type;


/** IP call information returned with IP call related
** events
**
** Clients needs to check for CM_API_IPCALL_SS_INFO
** before using this type
*/
typedef struct cm_ip_call_info_s {

  cm_call_sups_type_e_type      sups_type;
     /**< Indicates the supp service req for which
     ** call_ss_success is being generated.
     ** Relevant for CM_CALL_EVENT_MNG_CALLS_CONF
     */

  boolean                       call_ss_success;
    /**< Flag to indicate if sups service request on ip call
    ** succeded or not for CM_CALL_EVENT_MNG_CALLS_CONF.
    */

  cm_call_prog_info_e_type      call_prog_info;
    /**< Gives the specific progress states for
    ** CM_CALL_EVENT_PROGRESS_INFO_IND
    */
} cm_ip_call_info_s_type;


typedef union cm_call_mode_info_u {

  cm_gw_cs_call_info_s_type     gw_cs_call;
      /**< CS GSM or WCDMA specific info */

  cm_gw_ps_call_info_s_type     gw_ps_call;
      /**< PS GSM or WCDMA specific info */

  cm_cdma_call_info_s_type      cdma_call;
      /**< CDMA specific info */

  cm_ip_call_info_s_type        ip_call;
      /**< IP specific info
      **
      ** Clients needs to check for CM_API_IPCALL_SS_INFO
      ** before using this type
      */

} cm_call_mode_info_u_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CASE CM_CALL_MODE_INFO_NONE  cm_call_mode_info_u.void */
/*~ CASE CM_CALL_MODE_INFO_GW_CS cm_call_mode_info_u.gw_cs_call */
/*~ CASE CM_CALL_MODE_INFO_GW_PS cm_call_mode_info_u.gw_ps_call */
/*~ CASE CM_CALL_MODE_INFO_CDMA  cm_call_mode_info_u.cdma_call */
/*~ CASE CM_CALL_MODE_INFO_IP  cm_call_mode_info_u.ip_call */
#endif

typedef struct cm_call_mode_info_s {

  cm_call_mode_info_e_type  info_type;
    /**< Which mode information is present */

  cm_call_mode_info_u_type  info;
    /**< Mode information */

} cm_call_mode_info_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_call_mode_info_s.info DISC cm_call_mode_info_s.info_type */
#endif

/* The following data structures are primarily for passing information
** through the functions. They are not used anywhere else.
*/

/** Origination parameters for GSM/WCDMA circuit switched call
   set by the clients.
*/
typedef struct cm_gw_cs_orig_params_s {

  cm_repeat_indicator_s_type          bc_repeat_indicator;
    /**< Bearer capability repeat indicator - included only if
       bearer capability 1 and bearer capability 2 are included
    */

  cm_bearer_capability_s_type         bearer_capability_1;
    /**< Bearer Capability 1 */

  cm_bearer_capability_s_type         bearer_capability_2;
    /**< Bearer Capability 2*/

  cm_repeat_indicator_s_type          llc_repeat_indicator;
    /**< LLC repeat indicator - included only if BC repeat indicator
       and low layer compatibility 1 are included
    */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_1;
    /**< Low Layer Compatibility 1 - included in the Network -> UE
       direction if the calling user specifies a  low layer
       compatibility
    */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_2;
    /**< Low Layer Compatibility 2 - included only if LLC repeat indicator
       is included
    */

  cm_repeat_indicator_s_type          hlc_repeat_indicator;
    /**< HLC repeat indicator - included only if BC repeat indicator
       and high layer compatibility 1 are included
    */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_1;
    /**< High Layer compatibility 1 - included in the Network -> UE
       direction if the calling user specifies a  high layer
       compatibility
    */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_2;
    /**< High Layer compatibility 2 - included only if HLC repeat
       indicator is included
    */

  cm_called_party_bcd_no_s_type       called_party_bcd_number;
    /**< Called party BCD number */

  cm_called_party_subaddress_s_type   called_party_subaddress;
    /**< Called party subaddress */

  cm_calling_party_subaddress_s_type  calling_party_subaddress;
    /**< Calling party subaddress */

  cm_forward_cug_info_s_type          forward_cug_info;
    /**< Closed user Group information */

  uint8                               cm_clir;
    /**< CLIR information - CLIR suppression / invocation etc. */

  boolean                             call_control_completed;
    /**< Calling task has applied call control aready */

  #if defined (FEATURE_UUS) || defined(FEATURE_MM_SUPERSET)
  cm_user_data_s_type                 mo_user_data;
    /**< Clients need to check for CM_API_USER_USER_DATA before
    ** using this field. user to user data request structure
    */
  #endif

} cm_gw_cs_orig_params_s_type;


/** Origination parameters for GSM/WCDMA packet switched call
*/
typedef struct cm_gw_ps_orig_params_s {

  boolean                accept;
  /**< Applicable for only MT calls
  ** True: MT call gets accepted
  ** False: MT call is rejected
  ** accept being made first parameter to align with other call answer
  ** structures
  */

  boolean                primary_pdp;
  /**< True: Primary
     False: Secondary
  */

  qos_T                  qos;
    /**< Quality of Service */

  qos_T                  minimum_qos;
    /**< The minimum quality of service allowed for the
       PDP context
    */

  pdp_address_T          pdp_addr;
    /**< Packet data protocol address */

  apn_T                  apn_name;
    /**< Access point  name - a label or a full
       qualified domain name
    */

  protocol_cfg_options_T config_options;
    /**< Protocol configuration options */

  sys_pdp_header_comp_e_type h_comp;
    /**< IP Header Compression Required
    **
    ** Clients need to check for CM_API_PDP_COMPR before
    ** using this enum
    */

  sys_pdp_data_comp_e_type   d_comp;
    /**< Data Compression Required
    **
    ** Clients need to check for CM_API_PDP_COMPR before
    ** using this enum
    */

  tft_type_T             tft;
    /**< Traffic flow template */

  nsapi_T                primary_nsapi;
    /**< Primary NSAPI */

  cm_call_id_type        primary_call_id;
    /**< Call Id of the primary PDP context.
       This field is only used if this is
       a secondary PDP context origination
    */

  uint16                   profile_number;
    /**< The PDP profile number used for this
    ** PDP context
    */

  byte                   pdp_group_id;
    /**< Clients check CM_API_SEC_PDP for SEC PDP support
    ** Group id of the primary context
    ** that is associated with the
    ** secondary PDP context origination
    */

  uint16                   assoc_pri_profile_num;
   /**< Clients check CM_API_SEC_PDP for SEC PDP support
   ** Valid for only Sec PDP context
   ** Sec PDP origination contains requested qos and
   ** minimum_qos.
   ** FALSE - Nw has allocated qos in between requested and minimum_qos
   ** TRUE  - Nw has allocated different qos
   */


} cm_gw_ps_orig_params_s_type;

/** Clients need to check for CM_API_GW_PS_CC before
** using this structure.
**
** Call Control parameters for GSM/WCDMA packet switched call
** are in 31.111-660 sect 7.3.1.6. This spec refers to
** 24.008 section 9.5.1.
**
** Structure carries length of PDP Activationr equest
** and bytes in L3 PDP Activation message format
*/
typedef struct cm_gw_ps_cc_params_s {

  word length;
    /**< Length of pdp activation byte */

  byte pdp_act_mesg[CM_MAX_OCTETS_L3_MESSAGE];
    /**< Buffer carrying pdp act parameters,
    ** MAX size is [CM_MAX_OCTETS_L3_MESSAGE]
    */

  apn_T apn_name;
    /**< apn_name is required by UI to compare with ACL in SIM, for
    ** accepting or rejecting PS call.
    */

} cm_gw_ps_cc_params_s_type;


/** Answer parameters for GSM/WCDMA circuit switched call
** set by the clients.
*/
typedef struct cm_gw_answer_params_s {

  boolean                         accept;
    /**< Flag indicating whether the call is accepted or not
    */

  cm_call_type_e_type             call_type;
    /**< Call type */

  cm_connected_subaddress_s_type  connected_subaddress;
    /**< Connected suaddress */

} cm_gw_answer_params_s_type;

/** To be used only if CM_API_IP_CALL is defined
** Answer parameters for WLAN voice call
** set by the clients.
*/
typedef struct cm_ip_answer_params_s {

  boolean                         accept;
    /**< Flag indicating whether the call is accepted or not
    */

  cm_call_type_e_type             call_type;
    /**< Call type */

} cm_ip_answer_params_s_type;


/** Call End parameters for GSM/WCDMA circuit switched call
** set by the clients.
*/
typedef struct cm_gw_cs_end_params_s {

  uint8                      reserved;
    /**< empty for now */

} cm_gw_cs_end_params_s_type;


/** Call End parameters for GSM/WCDMA packet switched call
** set by the clients.
*/
typedef struct cm_gw_ps_end_params_s {

  nsapi_T                    nsapi;
    /**< NSAPI */

  ti_T                       tear_down_ind;
    /**< Tear down indicator - indicates whether
       only the PDP context associated with this specific
       TI or all active PDP contexts sharing the same PDP
       address associated with this specific TI shall be deactivated.
    */

  sm_status_T                cause;
    /**< Cause for ending the call */

} cm_gw_ps_end_params_s_type;


/** Setup res parameters for GSM/WCDMA circuit switched call
** and VoIP calls set by the clients.
*/
typedef struct cm_setup_res_params_s {

  boolean                             accept;
    /**< Flag indicating whether the call is accepted or not */

  /**< Following fields only used for responding to GSM/WCDMA
  ** set up requests
  */
  ie_cm_cc_cause_s_type               cc_cause;
    /**< Call control cause information */

  cm_repeat_indicator_s_type          bc_repeat_indicator;
    /**< Bearer capability repeat indicator - included only if
       bearer capability 1 and bearer capability 2 are included
    */

  cm_bearer_capability_s_type         bearer_capability_1;
    /**< Bearer Capability 1 */

  cm_bearer_capability_s_type         bearer_capability_2;
    /**< Bearer Capability 2*/

  cm_repeat_indicator_s_type          llc_repeat_indicator;
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
    ** using this field.
    **
    ** LLC repeat indicator - included only if BC repeat indicator
    ** and low layer compatibility 1 are included */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_1;
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
    ** using this field.
    **
    ** Low Layer Compatibility 1 - included in the Network -> UE
    ** direction if the calling user specifies a  low layer
    ** compatibility */

  cm_low_layer_compatibility_s_type   low_layer_compatibility_2;
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
    ** using this field.
    **
    ** Low Layer Compatibility 2 - included only if LLC repeat indicator
    ** is included */

  cm_repeat_indicator_s_type          hlc_repeat_indicator;
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
    ** using this field.
    **
    ** HLC repeat indicator - included only if BC repeat indicator
    ** and high layer compatibility 1 are included */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_1;
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
    ** using this field.
    **
    ** High Layer compatibility 1 - included in the Network -> UE
    ** direction if the calling user specifies a  high layer
    ** compatibility
    */

  cm_high_layer_compatibility_s_type  high_layer_compatibility_2;
    /**< Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
    ** using this field.
    **
    ** High Layer compatibility 2 - included only if HLC repeat
    ** indicator is included */

} cm_setup_res_params_s_type;


/** PDP activation response parameters for GSM/WCDMA packet
 *  switched call
*/
typedef struct cm_pdp_act_res_params_s {

  boolean                accept;
    /**< Flag indicating whether the call is accepted or not */

  sm_status_T            cause;
    /**< Cause information for the Session Manager */

} cm_pdp_act_res_params_s_type;


/** PDP modify parameters for GSM/WCDMA packet switched call
*/
typedef struct cm_pdp_modify_params_s {

  nsapi_T                nsapi;
    /**< NSAPI */

  qos_T                  qos;
    /**< Quality of Service information */

  tft_type_T             tft;
    /**< Traffic flow template */

  qos_T                  minimum_qos;
    /**<  Clients need to check for CM_API_CHG_PDP_MOD before
    **  using this macro
    **  Minimum quality of service required for the
    **  PDP context
    */

} cm_pdp_modify_params_s_type;


/** RAB Reestablish request parameters for GSM/WCDMA packet
 *  switched call
*/
typedef struct cm_rab_reestab_params {

  rabm_rabid_T           rabid;
    /**< RAB Id */

} cm_rab_reestab_params_s_type;


/** CDMA call origination parameter structure
** Applicable for FEATURE_CDMA_800 or FEATURE_CDMA_1900.
*/
typedef struct cm_cdma_orig_params_s {

  uint16                    srv_opt;
    /**< Service options as defined in CAI.H file */

  cm_activate_code_e_type   activate_code;
    /**< OTASP activation code */

  boolean                   drs_bit;
    /**< Data ready set is ON/OFF */

  boolean                   sr_id_included;
    /**< Service Identifier is included or not */

  uint8                     sr_id;
    /**< Service Identifier */

  boolean                   qos_parms_incl;
    /**< Quality of Service parameters included or not */

  uint8                     qos_parms_len;
    /**< Length of Quality of Service parameter list */

  uint8                     qos_parms[CM_ORIG_QOS_LEN_MAX];
    /**< Quality of Service parameter list */

  sys_sys_mode_e_type       last_act_data_net;
    /**< last active data network - FEATURE_HDR_HANDOFF */

} cm_cdma_orig_params_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_cdma_orig_params_s.qos_parms VARRAY CM_ORIG_QOS_LEN_MAX
    LENGTH cm_cdma_orig_params_s.qos_parms_len */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/** CDMA call answer parameter structure
** Applicable for FEATURE_CDMA_800 or FEATURE_CDMA_1900.
*/
typedef struct cm_cdma_answer_params_s {

  cm_call_type_e_type        call_type;
    /**< Call type */

} cm_cdma_answer_params_s_type;


/** CDMA call end parameter structure
** Applicable for FEATURE_CDMA_800 or FEATURE_CDMA_1900.
*/
typedef struct cm_cdma_end_params_s {

  boolean      end_reason_included;
    /**< End reason included or not */

  uint8        end_reason;
    /**< End reson */

} cm_cdma_end_params_s_type;

/** IP cause values for call hold/retrieve and call end
*/
typedef struct cm_ip_cause_s {

  uint32        cause;
    /**< ip cause */

} cm_ip_cause_s_type;


/** Type for CM alpha.
** Able to store alpha tag text.
*/
typedef struct cm_orig_alpha_s {

  uint8    buf[CM_MAX_ALPHA_LEN_FOR_CALL_ORIG];
    /**< Alpha buffer */

  uint8    len;
    /**< Length of alpha tag stored in alpha_buf */

} cm_orig_alpha_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_orig_alpha_s.buf VARRAY CM_MAX_ALPHA_LEN_FOR_CALL_ORIG
    LENGTH cm_orig_alpha_s.len */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/** Union for all mode answer parameters
*/
typedef union cm_ans_params_u {

  cm_gw_answer_params_s_type             gw_cs_ans;
    /**< GW CS answer parameters */

  cm_gw_ps_orig_params_s_type            gw_ps_ans;
    /**< GW PS answer parameters. Parameters required
    ** are exactly the same as reqd for ps call
    ** origination. So reusing call orig structure.
    */

  cm_ip_answer_params_s_type           ip_ans;
    /**< To be used only if CM_API_IP_CALL is defined
    ** Parameters to answer a MT IP voice
    */

  cm_cdma_answer_params_s_type           cdma_ans;
    /**< CDMA answer parameters */

} cm_ans_params_u_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CASE CM_CALL_MODE_INFO_NONE  cm_ans_params_u.void */
/*~ CASE CM_CALL_MODE_INFO_GW_CS cm_ans_params_u.gw_cs_ans */
/*~ CASE CM_CALL_MODE_INFO_CDMA  cm_ans_params_u.cdma_ans */
/*~ CASE CM_CALL_MODE_INFO_GW_PS cm_ans_params_u.gw_ps_ans */
/*~ CASE CM_CALL_MODE_INFO_IP    cm_ans_params_u.ip_ans */
#endif

/** Call answer parameters depending on the mode - CDMA, GSM,
 *  WCDMA etc.
*/
typedef struct cm_ans_params_s {

  cm_call_mode_info_e_type  info_type;
    /**< mode type */

  cm_ans_params_u_type      ans_params;
    /**< union - for mode specific parameters */

} cm_ans_params_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_ans_params_s.ans_params DISC cm_ans_params_s.info_type */
#endif


/** cm_ip_end_params_s_type is part of cm_end_params_u_type which is
** used by clients to call call_cmd_end api with end parameters.
**
** Here the structure is used to report information with call end
** event.
**
** !!Need to put a field based on this structure to clients.!!
** Clients needs to check for CM_API_VIDEOSHARE
** before using this type
*/
typedef struct cm_ip_end_params_s {

  int                         cause;
    /**< Reason for ending the call
    */

  cm_ipapp_warn_info_s_type   warn_info;
    /**< Warning information given by ipapp
    ** RFC 3261 sect 20.43
    */

  char                        reason[CM_IPAPP_TEXT_SIZE];
    /**< Null terminated reason string
    ** RFC 3326
    ** Does not mention any specific reason string lengths
    */

} cm_ip_end_params_s_type;


/** Union of all mode call end parameters
*/
typedef union cm_end_params_u {

  cm_gw_cs_end_params_s_type           gw_cs_end;
    /**< GSM/WCDMA circuit switched call end parameters */

  cm_gw_ps_end_params_s_type           gw_ps_end;
    /**< GSM/WCDMA packet switched call end parameters */

  cm_cdma_end_params_s_type            cdma_end;
    /**< CDMA call end parameters */

  cm_ip_cause_s_type                   ip_cause;
    /**< Clients need to check for CM_API_IP_CAUSE before using this
    ** field.
    ** ip call cause values for call hold/retrieve and call end
    */

    cm_ip_end_params_s_type              ip_end;
    /**< Clients should check for CM_API_VIDEOSHARE before using
    ** this field.
    */

} cm_end_params_u_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARTIAL cm_end_params_u */
/*~ CASE CM_CALL_MODE_INFO_NONE  cm_end_params_u.void */
/*~ CASE CM_CALL_MODE_INFO_GW_CS cm_end_params_u.gw_cs_end */
/*~ CASE CM_CALL_MODE_INFO_GW_PS cm_end_params_u.gw_ps_end */
/*~ CASE CM_CALL_MODE_INFO_CDMA  cm_end_params_u.cdma_end */
/*~ CASE CM_CALL_MODE_INFO_IP    cm_end_params_u.ip_cause */
#endif

/** Call end parameters depending on the mode - CDMA, GSM, WCDMA etc.
*/
typedef struct cm_end_params_s {

  cm_call_id_type           call_id;
    /**< Call ID to end */

  cm_call_mode_info_e_type  info_type;
    /**< Mode type */

  cm_end_params_u_type      end_params;
    /**< mode specific end parameters */

} cm_end_params_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_end_params_s.end_params DISC cm_end_params_s.info_type */
#endif

/** Enumeration for call substates in idle state.
*/
typedef enum cm_call_idle_substate_e {

    CM_CALL_IDLE_SUBST_NONE = -1, /**< @internal */

    CM_CALL_IDLE_SUBST_ENDWAIT,   /* Waiting for END ACK */

    CM_CALL_IDLE_SUBST_MAX        /**< @internal */

} cm_call_idle_subst_e_type;

/** Enumeration for call substates in incoming state.
*/
typedef enum cm_call_incom_substate_e {

    CM_CALL_INCOM_SUBST_NONE = -1, /**< @internal */

    /* 0 */
    CM_CALL_INCOM_SUBST_REGULAR,   /**< All the other SO incoming calls */

    CM_CALL_INCOM_SUBST_OTAPA,     /**< OTAPA SO 18/19 incoming call */

    CM_CALL_INCOM_SUBST_SETUP,     /**< Processing Incoming Setup(GSM/WCDMA)
                                   ** Setup is pending for client to respond
                                   **
                                   ** Clients need to check for
                                   ** CM_API_ADD_INCOM_SUBST before using
                                   */

    CM_CALL_INCOM_SUBST_SETUP_PROCESSED, /**< Client response sent to
                                         ** protocol layer (GSM/WCDMA)
                                         ** Clients need to check for
                                         ** CM_API_ADD_INCOM_SUBST bef using
                                         */

    CM_CALL_INCOM_SUBST_ALERTING,   /**< Client being alerted on incoming call
                                    ** (GSM/WCDMA)
                                    ** Clients need to check for
                                    **  CM_API_ADD_INCOM_SUBST bef using
                                    */

    CM_CALL_INCOM_SUBST_ANSWERED,   /**< Answer from client sent to
                                    ** Protocol layer (GSM/WCDMA)
                                    ** Clients need to check for
                                    ** CM_API_ADD_INCOM_SUBST bef using
                                    */

     CM_CALL_INCOM_SUBST_MAX        /**< @internal */

} cm_call_incom_subst_e_type;


/** Enumeration for call substates in conversation state.
*/
typedef enum cm_call_conv_substate_e {

    CM_CALL_CONV_SUBST_NONE = -1,  /**< @internal */

    /** 0 */
    CM_CALL_CONV_SUBST_REGULAR,    /**< All the other calls */

    CM_CALL_CONV_SUBST_COMMIT,
                                   /**< OTASP or busy OTAPA call. */
           /**< Commit is in progress */

    CM_CALL_CONV_SUBST_PC_OTAPA_REGULAR,
                                   /**< SO 18/19 OTAPA call */
           /**< Without commit (either commit */
           /**< never happens or commit finishes */

    CM_CALL_CONV_SUBST_PC_OTAPA_COMMIT,
                                   /**< SO 18/19 OTAPA call */
           /**< Commit is in progress. */

    /* 4 */
    CM_CALL_CONV_CALL_ACTIVE,
          /**< the call is in conversation state and is active */

    CM_CALL_CONV_CALL_ON_HOLD,
          /**< the call is in conversation state, but is put on hold */

    CM_CALL_CONV_SUBST_MAX      /**< @internal */

} cm_call_conv_subst_e_type;

/** Type for call substate.
*/
typedef union cm_call_substate_u {


    cm_call_idle_subst_e_type idle;

    cm_call_incom_subst_e_type incom;

    cm_call_conv_subst_e_type  conv;

} cm_call_substate_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CASE CM_CALL_STATE_IDLE      cm_call_substate_u.idle */
/*~ CASE CM_CALL_STATE_INCOM     cm_call_substate_u.incom */
/*~ CASE CM_CALL_STATE_CONV      cm_call_substate_u.conv */
/*~ DEFAULT                      cm_call_substate_u.void */
#endif


/** Defines the state information for a particular call object.
*/
typedef struct cm_call_state_info_s {

  cm_call_id_type                call_id;
    /**< Call ID */

  cm_call_type_e_type            call_type;
    /**< Call type - VOICE, DATA, OTASP, etc */

  cm_call_state_e_type           call_state;
    /**< Call state - idle, orig, incom, conv */

  sys_sys_mode_e_type            sys_mode;
    /**< The mode in which the call was originated */

  cm_call_direction_e_type       call_direction;
    /**< Indicates whether the call was mobile originated
    ** or mobile terminated
    */

  dword                          call_connect_time;
    /**< The time the call was connected.  The units is
    ** seconds since the phone has powered on (clock
    ** uptime).
    **/

  cm_alpha_s_type                alpha;
    /**< Alpha tag to be displayed along with dialed address
    ** of an originated call.
    */

  /*
  ** The following fields are only valid if
  ** call_type is CM_CALL_TYPE_PS_DATA.
  */
  boolean                        primary_pdp;
    /**< True  = Primary PDP context
       False = Secondary PDP context
    */

  uint16                           profile_number;
    /**< The number/Id of the PDP profile used
       for this PDP context
    */

  cm_call_id_type                primary_call_id;
    /**< Call ID of the primary PDP context */

  uint16                           assoc_pri_profile_num;
    /**< Clients check CM_API_SEC_PDP for SEC PDP support
    ** Valid for only Sec PDP context
    ** Sec PDP origination contains requested qos and
    ** minimum_qos.
    ** FALSE - Nw has allocated qos in between requested and minimum_qos
    ** TRUE  - Nw has allocated different qos
    */

  boolean                        is_net_alloc_qos_diff_from_req;
    /**< Clients check CM_API_SEC_PDP for SEC PDP support
    ** Valid for only Sec PDP context
    ** Sec PDP origination's requested qos and
    ** minimum_qos decide this value
    ** FALSE - Nw has allocated qos in between requested and minimum_qos
    ** TRUE  - Nw has allocated different qos
    */

  byte                           pdp_group_id;
     /**< Clients check CM_API_SEC_PDP for SEC PDP support
     ** Valid for only Sec context.
     ** Group id of primary context particular
     ** sec is associated with
     */

  cm_call_substate_type          call_subst;
     /**< Call substates - idle, conv, incom
     */
} cm_call_state_info_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_call_state_info_s.call_subst DISC cm_call_state_info_s.call_state */
#endif

/** The state information for all call objects
*/
typedef struct cm_call_state_info_list_s {

  byte                      number_of_active_calls;
  /**< The number of calls whose state is not idle */

  cm_call_state_info_s_type info[CM_CALL_ID_INTERFACE_MAX];
  /**< The state info for all call objects */
} cm_call_state_info_list_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_call_state_info_list_s.info VARRAY CM_CALL_ID_INTERFACE_MAX
    LENGTH cm_call_state_info_list_s.number_of_active_calls */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/** Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
** using this field.
** Structure for holding call compatibility
** and capability details.
*/
typedef struct cm_call_comp_cap_info_s {


  cm_bearer_capability_s_type         bearer_capability;
    /**< Bearer Capability */


  cm_low_layer_compatibility_s_type   low_layer_compatibility;
    /**< Low Layer Compatibility 1 - included in the Network -> UE
    ** direction if the calling user specifies a  low layer
    ** compatibility */


  cm_high_layer_compatibility_s_type  high_layer_compatibility;
    /**< High Layer compatibility 1 - included in the Network -> UE
    ** direction if the calling user specifies a  high layer
    ** compatibility
    */

} cm_call_comp_cap_info_s_type;

/**
** Structure to hold alpha and num returned by call control
** Check for CM_API_RESULT_FROM_CC flag before using cm_result_from_cc_s_type

*/
typedef struct cm_result_from_cc_s
{
  cm_cc_result_e_type                call_control_result;
    /**< Indicates whether there were modifications */

  cm_num_s_type                        num;
    /**< Number buffer:
    ** CM_CALL_EVENT_ORIG      - contains dialed digits string.
    ** CM_CALL_EVENT_FLASH     - contains dialed digits string.
    ** CM_CALL_EVENT_INCOM     - contains caller ID string.
    ** CM_CALL_EVENT_CALLER_ID - contains caller ID string.
    */
  cm_alpha_s_type                      alpha;
    /**< Alpha tag returned by call control
    */


} cm_result_from_cc_s_type;


/** Type to hold call state information.
** When notified of call events, clients are given
** a reference to such a structure.
*/
struct cm_mm_call_info_s {

  cm_client_id_type                    call_client_id;
    /**< Call client - originating/answering client */

  cm_call_id_type                      call_id;
    /**< call ID of the call */

  cm_call_state_e_type                 call_state;
    /**< Call state - idle, orig, incom, conv */

  cm_call_type_e_type                  call_type;
    /**< Call type - VOICE, DATA, OTASP, etc */

  cm_num_s_type                        num;
    /**< Number buffer:
    ** CM_CALL_EVENT_ORIG      - contains dialed digits string.
    ** CM_CALL_EVENT_FLASH     - contains dialed digits string.
    ** CM_CALL_EVENT_INCOM     - contains caller ID string.
    ** CM_CALL_EVENT_CALLER_ID - contains caller ID string.
    */

  #ifdef FEATURE_IP_CALL
  cm_num_s_type                        waiting_num;
  #endif /* FEATURE_IP_CALL */

  cm_redirecting_num_s_type            redirecting_number;
    /**< Number buffer:
    ** CM_CALL_EVENT_REDIRECTING_NUMBER           - contains redirecting num.
    */

  cm_alpha_s_type                      alpha;
    /**< Alpha tag to be displayed along with dialed address
    ** of an originated call.
    **
    ** NOTE! this is not a phone book name match, but rather
    ** an indication for the UI to display some text for an
    ** outgoing call
    **
    ** We probably need to change this one to a UI text component to
    ** support I18N
    **
    Alpha buffer:
    CM_CALL_EVENT_ORIG
    CM_CALL_EVENT_DISPLAY
    CM_CALL_EVENT_EXT_DISP
    */

  cm_enc_alpha_s_type                    enc_alpha;
    /* Alpha tag to be displayed along with dialed address
    ** of an originated call. This has coding information that
    ** may be later used by UI
    */

  cm_signal_s_type                     signal;
    /**< Alert signal information to be played for the incoming call
    */

  cm_call_end_e_type                   end_status;
    /**< Call end status (i.e. reason for ending a call in origination,
    **incoming or conversation state)
    */

  uint8                                num_call_ids_ended;
    /**< number of calls ended
    */

  cm_end_params_s_type                 end_params;
    /**< call end information
    */

  sys_sys_mode_e_type                  sys_mode;
    /**< Indicates the system on which the call is made
    */

  dword                                call_connect_time;
    /**< The time the call was connected.  The units is seconds since
    ** the phone has powered on (clock uptime).
    **/

  dword                                call_end_time;
    /**< The time the call was ended.  The units is seconds since
    ** the phone has powered on (clock uptime).
    **/

  uint32                               bytes_received;
    /**< The number of bytes received during the call (PS data call only).
    ** This field is populated after the call has ended.
    */

  uint32                               bytes_transmitted;
    /**< The number of bytes transmitted during the call (PS data call only).
    ** This field is populated after the call has ended.
    */
  cm_call_mode_info_s_type             mode_info;
    /**< System specific info - CDMA, GSM/WCDMA-CS, GSM/WCDMA-PS
    */

  cm_als_line_e_type                   line;
    /**< What line is being used for this call (FEATURE_ALS)
    */

  cm_client_id_type                    requesting_client_id;
    /**< Requesting client
    */

  #if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
       defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM) || \
       defined(FEATURE_MM_SUPERSET))
  sys_msm_id_e_type                    msm_id;
    /**< Identifies the MSM which generated this event.
    */
  #endif

  cm_call_direction_e_type             direction;
    /**< Holds direction of the current call.
    ** It can be Mobile originated or Mobile
    ** Terminated
    */

  cm_call_modify_e_type                modify;
    /**< Indicates the modified call type. It indiactes network's
    ** desired modify action when CM sends CM_CALL_EVENT_MODIFY.
    ** In all other cases, it will represent modified call type
    ** being used. Please note that CALL_TYPE is not changed as
    ** a result of MODIFY.
    */

  cm_result_from_cc_s_type             result_from_cc;
    /**< Holds alpha and num returned by call control
    ** Check for CM_API_RESULT_FROM_CC before using this field.
    */
};

typedef struct cm_cap_config_s {

    uint8                        length;

    uint8                        ccp[CM_MAX_CCP_LEN];

} cm_cap_config_s_type;


/** Type for call compatibility and capability info callback function.
**
** Clients need to check for CM_API_VT_FALLBACK_TO_VOICE  before
** using this type definition.
**
** Function registered along with call type to retrieve compatibility
** capability.
*/
typedef void (cm_call_comp_cap_cb_f_type) (

  cm_call_comp_cap_info_s_type    *call_comp_cap_info_ptr
    /**< Pointer to call compatibility and capability info */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cm_call_comp_cap_cb_f_type */
/*~ PARAM call_comp_cap_info_ptr POINTER */
#endif


/** Type to hold client callbacks for retrieving call compatibilty and
** capability information
*/
typedef struct cm_call_comp_cap_cb_s {

  cm_call_type_e_type           call_type;
    /**< Call type for cb function */

  cm_call_comp_cap_cb_f_type    *call_comp_cap_func_ptr;
    /**< Call compatibility and capability info callback */

} cm_call_comp_cap_cb_s_type;

/**  Call Control generic parameters for a call
**
** Structure carries FDN checking information
*/
typedef struct cm_cc_generic_params_s{

  boolean     is_fdn_to_check;
    /**< This can be checked by clients to decide whether FDN checking has
    ** has to be done or not
    */

} cm_cc_generic_params_s_type;


/** Type to hold CDMA position information.
*/
typedef struct cm_cdma_position_info_s {

  uint16                                            sid;       
    /**< System Identification */

  uint16                                            nid;  
    /**< Network Identification */

  uint16                                            base_id;
    /**< Base Station Identification */

  uint16                                            refpn;            
    /**< reference PN */

  uint32                                            base_lat;      
    /**< Base Station Latitude */

  uint32                                            base_long;    
    /**< Base Station Longitude */

}cm_cdma_position_info_s_type;


/*---------------------------------------------------------------------------
** Macros
**-------------------------------------------------------------------------*/

/** Macro to decide whether a given call type is an OTASP
** (standard or none standard ).
*/
#define CM_CALL_TYPE_IS_OTASP( call_type )  \
              ((call_type) == CM_CALL_TYPE_STD_OTASP    || \
                                (call_type) == CM_CALL_TYPE_NON_STD_OTASP   )

#define CM_CALL_TYPE_IS_OTAPA( call_type )  \
              ((call_type) == CM_CALL_TYPE_OTAPA)

/** Macro to return the length of a CM number.
*/
#define CM_NUM_LEN( num_ptr )  ( (num_ptr)->len )


/** Macro to return the Number type of a call manager number
*/
#define CM_NUM_NUMBER_TYPE( num_ptr )  ( (num_ptr)->number_type )


/** Macro to return the Numbering Plan of a CM number
*/
#define CM_NUM_NUMBER_PLAN( num_ptr )  ( (num_ptr)->number_plan )


/** Macro to return the Presentation Indicator of a CM number
*/
#define CM_NUM_PI( num_ptr )  ( (num_ptr)->pi )


/** Macro to return the Screening Indicator of a CM number
*/
#define CM_NUM_SI( num_ptr )  ( (num_ptr)->si )


/** Macro to return the Origination digit mode of a CM number
*/
#define CM_NUM_DIGIT_MODE( num_ptr )  ( (num_ptr)->digit_mode )


/*---------------------------------------------------------------------------
** Functions
**-------------------------------------------------------------------------*/

/* CM command functions
**
** The CM command functions defines the Client->CM interface.
**
** Each command function takes a pointer to a client provided callback
** function, a client provided void pointer and the command's required set
** of fields.
**
** If the command function find one or more of the parameters to contain
** invalid values ( e.g. not one of the enum values for an enum parameter),
** it calls on the client's callback function IMMEDIATELY with the
** appropriate command error code. NOTE that in this case the callback
** function is actually being called from the client context - NOT CM.
**
** If all parameters found to be valid, the command function queues a
** command to CM. When CM gets the command it checks whether the command
** is valid relative to the current state of the phone/call.
**
** In case the command is not valid (e.g. ending a call that was already
** ended), CM calls on the client callback function with the appropriate
** command error code. NOTE that in this case the callback function is
** indeed being called from the CM context.
**
** If the command is successful, CM calls on the client callback function
** with an error code that indicates successful command. In addition CM
** notified registered clients of the corresponding CM event (e.g. if a
** client successfully ends a call, CM notify clients that are registered
** for the call-end-event of this event).
*/

/** Type for call command callback functions.
**
** A client that wants to be notified of the result of a  call command
** needs to supply a pointer to a function of this type as the first
** parameter to a call-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, CM context simultaneously.
*/
typedef void (cm_call_cmd_cb_f_type) (

  void                      *data_block_ptr,
    /**< Pointer to client provided data block.
    **
    ** NOTE! Depending on the client implementation of this function
    ** the data block pointed by this pointer might be accessed from
    ** client, as well as, CM context simultaneously */

  cm_call_cmd_e_type        call_cmd,
    /**< Indicate which command is this error status for */

  cm_call_cmd_err_e_type    call_cmd_err
    /* Call command error code, indicating whether command
    ** is rejected and the reason */

);



/**==========================================================================

@FUNCTION cm_mm_call_cmd_orig

@DESCRIPTION
  Command CM to originate a call. The client ID should be from a registered CM client.
  The activate_code field is only used for OTASP calls
  (i.e., the call type is CM_CALL_TYPE_STD_OTASP or CM_CALL_TYPE_NON_STD_OTASP).
  For nonOTASP calls, this field should be set to CM_OTASP_ACT_CODE_NONE.
  For called_number; If the number->digit_mode is set to CM_DIGIT_MODE_4BIT_DTMF, the number->buf
  may contain only 0-9, "#", or "*" characters and If the number->digit_mode is set to
  CM_DIGIT_MODE_8BIT_ASCII, the number buffer may contain any 8-bit ASCII characters.

  If the call type is voice, then Call Manager will call the application-registered resolve call type
  callback function to determine if the dial number is one of the special numbers for OTASP or EMERGENCY.
  If it is a special number, then the resolve call type function will modify the original call type accordingly.
  If no application registered a resolve call type callback function with Call Manager, then the call type will not be changed.
  For all call types, Call Manager will call the application-registered call-allowed callback function to
  determine if the dialed number is allowed. If no application registered a call-allowed callback function with
  Call Manager, then the call origination for all dialed numbers is always allowed.
  For targets supporting dual slots, the application-registered resolve call type and call-allowed callback
  functions will be called as described above if there is no call-control processing callback function registered.
  If there is a call-control processing callback function registered, Call Manager will call the call-control
  processing callback type function instead. This call-control processing callback type function will perform call type
  resolutions, call allowed checks, and also perform any call control that is required. In the case of dual slots in
  automatic mode, conflicts may arise during call control. Call Manager will call an application-registered call-control
  preferences callback type function to resolve any such conflicts.

  Note that the client supplied callback function, cmd_cb_func, will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided, command-callback function, cmd_cb_func. If the
  command request is processed without errors, CM_CALL_CMD_ERR_NOERR is passed back to the client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  - TRUE - request sent
  - FALSE - No buffer to send request
  Call_id will be returned in call_id_ptr.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_orig(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /**< Client callback function */

  void                        *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Originating client */

  cm_call_type_e_type         call_type,
    /**< Call type - voice, data, etc. */

  cm_srv_type_e_type          srv_type,
    /**< Service type - automatic, etc. */

  const cm_num_s_type         *calling_number,
    /**< Number of mobile that is originating the call */

  const cm_num_s_type         *called_number,
    /**< Number dialed - phone that is being called */

  const cm_orig_alpha_s_type  *alpha_buf,
    /**< Alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /**< Pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /**< Pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /**< Pointer to GW PS mode specific data */

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_orig */
/*~ PARAM calling_number POINTER */
/*~ PARAM called_number POINTER */
/*~ PARAM alpha_buf POINTER */
/*~ PARAM cdma_orig_params_ptr POINTER */
/*~ PARAM gw_cs_orig_params_ptr POINTER */
/*~ PARAM gw_ps_orig_params_ptr POINTER */
/*~ PARAM OUT return_call_id_ptr POINTER */
#endif


/**==========================================================================

@FUNCTION cm_mm_call_cmd_orig_exten

@DESCRIPTION
  Command CM to originate a handover call.
  The client ID should be from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  - TRUE - request sent
  - FALSE - No buffer to send request
  Switch_in Call_id will be returned in switch_in_call_id_ptr.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_orig_exten(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /**< Client callback function */

  void                        *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Originating client */

  cm_call_type_e_type         call_type,
    /**< Call type - voice, data, etc. */

  cm_srv_type_e_type          srv_type,
    /**< Service type - automatic, etc. */

  const cm_num_s_type         *calling_number,
    /**< Number of mobile that is originating the call */

  const cm_num_s_type         *called_number,
    /**< Number dialed - phone that is being called */

  const cm_orig_alpha_s_type  *alpha_buf,
    /**< Alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /**< Pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /**< Pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /**< Pointer to GW PS mode specific data */

  cm_call_id_type             switch_out_call_id,
    /**< Switch out call id */

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_orig_exten */
/*~ PARAM calling_number POINTER */
/*~ PARAM called_number POINTER */
/*~ PARAM alpha_buf POINTER */
/*~ PARAM cdma_orig_params_ptr POINTER */
/*~ PARAM gw_cs_orig_params_ptr POINTER */
/*~ PARAM gw_ps_orig_params_ptr POINTER */
/*~ PARAM OUT return_call_id_ptr POINTER */
#endif

/**==========================================================================
@FUNCTION cm_mm_call_cmd_orig_exten2

@DESCRIPTION
  Command CM to originate a call with call-control generic parameters.
  Clients need to check if CM_API_CC_MOD_PARAM is defined before calling this
  API

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  - TRUE - request sent
  - FALSE - No buffer to send request
  Switch_in Call_id will be returned in switch_in_call_id_ptr.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_orig_exten2(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /**< Client callback function */

  void                        *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Originating client */

  cm_call_type_e_type         call_type,
    /**< Call type - voice, data, etc. */

  cm_srv_type_e_type          srv_type,
    /**< Service type - automatic, etc. */

  const cm_num_s_type         *calling_number,
    /**< Number of mobile that is originating the call */

  const cm_num_s_type         *called_number,
    /**< Number dialed - phone that is being called */

  const cm_orig_alpha_s_type  *alpha_buf,
    /**< Alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /**< Pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /**< Pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /**< Pointer to GW PS mode specific data */

  cm_call_id_type             switch_out_call_id,
    /**< Switch out call id */

  const cm_cc_generic_params_s_type   *cc_generic_params_ptr,
    /**< Call-control generic parameters. Default val = TRUE */

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_orig_exten */
/*~ PARAM calling_number POINTER */
/*~ PARAM called_number POINTER */
/*~ PARAM alpha_buf POINTER */
/*~ PARAM cdma_orig_params_ptr POINTER */
/*~ PARAM gw_cs_orig_params_ptr POINTER */
/*~ PARAM gw_ps_orig_params_ptr POINTER */
/*~ PARAM cc_generic_params_ptr POINTER */
/*~ PARAM OUT return_call_id_ptr POINTER */
#endif

/**===========================================================================

@FUNCTION cm_mm_call_cmd_answer

@DESCRIPTION
  Command CM to answer an incoming call. The call id passed has range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_answer(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< answering client */

  cm_call_id_type        call_id,
    /**< call ID of the call */

  const cm_ans_params_s_type   *ans_params_ptr
    /**< pointer to mode specific data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_answer */
/*~ PARAM ans_params_ptr POINTER */
#endif


/**===========================================================================

@FUNCTION cm_mm_call_cmd_end

@DESCRIPTION
  Command CM to end a call - in origination, incoming or conversation state.
  The call id passed has range from 0 to CM_CALL_ID_MAX. The client ID should be from a registered
  CM client. Multiple calls can be ended, where num_call_ids holds number of calls.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed
  without errors, CM_CALL_CMD_ERR_NOERR is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_end(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< ending client */

  uint8                  num_call_ids,
    /**< number of call IDs sent */

  const cm_end_params_s_type   *end_params_ptr
    /**< pointer to mode specific data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_end */
/*~ PARAM end_params_ptr VARRAY LENGTH num_call_ids */
#endif

#if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
     defined(FEATURE_MM_SUPERSET))

/**===========================================================================

@FUNCTION cm_mm_call_cmd_end2

@DESCRIPTION
  Command CM to end a call - in origination, incoming or conversation state.
  The call id passed has range from 0 to CM_CALL_ID_MAX. The client ID should be from a registered
  CM client. Multiple calls can be ended, where num_call_ids holds number of calls.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed
  without errors, CM_CALL_CMD_ERR_NOERR is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_end2(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< ending client */

  uint8                  num_call_ids,
    /**< number of call IDs sent */

  cm_end_params_s_type   *end_params_ptr,
    /**< pointer to mode specific data */

  sys_msm_id_e_type      msm_id
    /**< MSM id to which this command is directed.
    */

);
#endif



/**===========================================================================

@FUNCTION cm_mm_call_cmd_sups

@DESCRIPTION
  Command CM to send a supplementary service request to the BS.
  All GW SUPS come with valid call id. The call id passed has range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_sups(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< sending client */

  cm_call_sups_type_e_type    sups_type,
    /**< Type of supplementary service */

  const cm_call_sups_params_s_type  *sups_params_ptr
    /**< Pointer to supplementary service specific data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_sups */
/*~ PARAM sups_params_ptr POINTER */
#endif


/**===========================================================================

@FUNCTION cm_mm_call_cmd_privacy_pref

@DESCRIPTION
  Command CM to change the privacy pref.
  The client ID should be from a registered CM client. Since the privacy preference applies to
  all the calls, this function does not need a call ID parameter.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_CDMA_800 || FEATURE_CDMA_1900 must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_mm_call_cmd_privacy_pref(

  cm_call_cmd_cb_f_type   cmd_cb_func,
    /**< client callback function */

  void                    *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type       client_id,
    /**< ending client */

  cm_call_id_type         call_id,
    /**< call ID of the call */

  cm_privacy_pref_e_type  privacy_pref
    /**< privacy pref setting */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_privacy_pref */
#endif


/**===========================================================================

@FUNCTION cm_mm_call_cmd_get_call_info

@DESCRIPTION
  Get a sanpshot of the current call information in the CM.The call id passed has
  range from 0 to CM_CALL_ID_MAX. The client ID should be from a registered CM client.
  The client needs to register for CM_CALL_EVENT_INFO to receive response to this command.
  The CM_CALL_EVENT_INFO is sent only to the client that issues this command.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_get_call_info(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< requesting client ID */

  cm_call_id_type        call_id
    /**< call ID of the call */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_get_call_info */
#endif



/**===========================================================================

@FUNCTION cm_mm_call_cmd_get_call_info_list

@DESCRIPTION
  Get a sanpshot of the state of all call objects maintained by CM.  This
  is a synchronous function. The client ID should be from a registered CM client.
  The client needs to register for CM_CALL_EVENT_INFO to receive response to this command.
  The CM_CALL_EVENT_INFO is sent only to the client that issues this command.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_get_call_info_list(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id
    /**< requesting client ID */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_get_call_info_list */
#endif
/**===========================================================================

@FUNCTION cm_mm_call_comp_cap_cb_reg

@DESCRIPTION
  Clients call this function to register call backs that can be used to
  receive capability and compatibility information for a call type.
  The client ID should be from a registered CM client.

  BC(Bearer Capability), LLC(Low Level Capability) and HLC(High Level Capability)
  values are differnet for  call types. For mixed mode call
  origination, a voice call needs to carry BC, LLC, HLC details for a VT(Video telephony)
  call. CM provides a storage for these values so that clients can query
  for other call type's details. Example - UI can query bearer details for
  VT call in order to originate a mixed mode call.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

  CM_API_VT_FALLBACK_TO_VOICE should be defined

@RETURNS
  TRUE   - Callback could be registered succeddfully
  FALSE  - Callback could not be registered successfully.

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern boolean cm_mm_call_comp_cap_cb_reg(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< requesting client ID */

  cm_call_type_e_type    call_type,
    /**< Call type for which call back has to be set */

  cm_call_comp_cap_cb_f_type *comp_cap
    /**< Callback that retrieves compatibility and capability for the
    ** call type.
    */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_comp_cap_cb_reg */
/*~ PARAM comp_cap POINTER */
#endif

/**===========================================================================

@FUNCTION cm_mm_get_call_comp_cap

@DESCRIPTION
  Return the call compatibility and capability for given call type.
  The client ID should be from a registered CM client.


@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

  CM_API_VT_FALLBACK_TO_VOICE should be defined.

@RETURNS
  TRUE   - Call compatibiliy and capability being returned.
  FALSE  - Call compatibiliy and capability could not be returned.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_get_call_comp_cap(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< requesting client ID */

  cm_call_type_e_type    call_type,
    /**< Call type */

  cm_call_comp_cap_info_s_type   *call_comp_cap_info_ptr
    /**< Pointer used to return compatibility and capability for call type */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_get_call_comp_cap */
/*~ PARAM call_comp_cap_info_ptr POINTER */
#endif


/**===========================================================================

@FUNCTION cm_mm_call_cmd_setup_res

@DESCRIPTION
  Command CM to send a setup message response for an incoming call.
  The call id passed has range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client.
  Set up response to MN requires just BC  but CM also copies LLC(Low Level Capability) and
  HLC(High Level Capability) so that this information can be used for in-call modification
  after call gets connected. Example - For VT setup DS sends setup res
  as voice. LLC for VT needs to be stored in llc2 so that it can be
  used later for in-call modification to VT.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  FEATURE_GSM || FEATURE_WCDMA must be defined.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.


@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_setup_res(

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /**< client callback function */

  void                          *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type             client_id,
    /**< answering client */

  cm_call_id_type               call_id,
    /**< call ID of the call */

  const cm_setup_res_params_s_type   *setup_params_ptr
    /**< pointer to mode specific data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_setup_res */
/*~ PARAM setup_params_ptr POINTER */
#endif


/**===========================================================================

@FUNCTION cm_mm_call_cmd_pdp_act_res

@DESCRIPTION
  Response to an Incoming PDP activation request.
  The call id passed has range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.


@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_pdp_act_res(

  cm_call_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type              client_id,
    /**< answering client */

  cm_call_id_type                call_id,
    /**< call ID of the call */

  const cm_pdp_act_res_params_s_type   *act_res_params_ptr
    /**< pointer to mode specific data */

);



/**===========================================================================

@FUNCTION cm_mm_call_cmd_pdp_modify

@DESCRIPTION
  Command CM to request a PDP modify command.
  This function commands CM to change the privacy preference during a call.
  The call id passed has range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_pdp_modify(

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /**< client callback function */

  void                          *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type             client_id,
    /**< answering client */

  cm_call_id_type               call_id,
    /**< call ID of the call */

  const cm_pdp_modify_params_s_type   *modify_params_ptr
    /**< pointer to command data */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_pdp_modify */
/*~ PARAM modify_params_ptr POINTER */
#endif


/**===========================================================================

@FUNCTION cm_mm_call_cmd_rab_reestablish

@DESCRIPTION
  Command CM to send a RAB reestablish request.
  The call id passed has range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_rab_reestablish(

  cm_call_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type              client_id,
    /**< answering client */

  cm_call_id_type                call_id,
    /**< call ID of the call */

  const cm_rab_reestab_params_s_type   *reestab_params_ptr
    /**< pointer to mode specific data */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_rab_reestablish */
/*~ PARAM reestab_params_ptr POINTER */
#endif

/*===========================================================================

@FUNCTION cm_mm_call_cmd_ps_data_available

@DESCRIPTION
  Inform CM that uplink data is available for transmission.
  The client ID should be from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_ps_data_available(

  cm_call_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type              client_id
    /**< requesting client ID */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_ps_data_available */
#endif


/**===========================================================================

@FUNCTION cm_mm_call_cmd_ps_sig_release

@DESCRIPTION
  Request CM to release PS signaling.
  The client ID should be from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.


@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

@SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.
@x
===========================================================================*/
extern boolean cm_mm_call_cmd_ps_sig_release(

  cm_call_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type              client_id
    /**< requesting client ID */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_ps_sig_release */
#endif

/*===========================================================================

@FUNCTION cm_mm_call_cmd_umts_cdma_handover_req

@DESCRIPTION
  Command CM to complete the handoff from UMTS -> 1x.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_mm_call_cmd_umts_cdma_handover_req
(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< originating client */

  cm_call_type_e_type    call_type,
    /**< call type - voice, data, etc. */

  cm_call_state_e_type   call_state,
    /**< Call state - orig, incoming or conv */

  const sys_is2000_msg_s_type  *ho_msg_ptr,
    /**< Pointer to CDMA message from UTRAN */

  cm_call_id_type        *return_call_id_ptr
    /**< call ID will be allocated and returned */

);

/**===========================================================================

@FUNCTION cm_mm_call_cmd_handover_req

@DESCRIPTION
  Command CM to complete the handoff between different RAT.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided, command-callback
  function, cmd_cb_func. If the command request is processed without errors, CM_CALL_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

@SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_mm_call_cmd_handover_req
(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< originating client */

  cm_call_type_e_type    call_type,
    /**< call type - voice, data, etc. */

  cm_call_state_e_type   call_state,
    /**< Call state - orig, incoming or conv */

  cm_call_id_type        *return_call_id_ptr
    /**< call ID will be allocated and returned */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_handover_req*/
/*~ PARAM OUT return_call_id_ptr POINTER */
#endif

/**===========================================================================

@FUNCTION cm_mm_call_cmd_umts_cdma_handover_cnf

@DESCRIPTION
  Command to CM to inform about the handover status.
  The call id passed has range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed without errors,
  CM_CALL_CMD_ERR_NOERR is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request
  Call_id will be returned in call_id_ptr.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_umts_cdma_handover_cnf(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< originating client */

  cm_call_id_type             call_id,
    /**< call ID of the call */

  sys_umts_cdma_handover_e_type  status
    /**< Handover status */

);

/** @}
*  endgroup CALL Call related definitions
*/

/****************************************************************************

DATA SESSION TYPES & FUNCTIONS

****************************************************************************/

/** @defgroup DSTF Data Session Types & Functions
**                DATA SESSION TYPES & FUNCTIONS
** This block group all the functionality that is associated with
** data session callback functions.
**
** When call manager ends a data call, it consults the data session callback
** functions for the following information:
**
*    1. Total number of bytes received and transmitted during the data call.
*
@{
*/

typedef enum cm_data_session_status_e {

  CM_DATA_SESSION_CB_ERR_NONE=-1,
    /**< @internal */

  CM_DATA_SESSION_CB_OK,
    /**< No error, request was completed successfully */

  CM_DATA_SESSION_ERR_NULL_DATA_SESSION_TOTAL_BYTE_CTRS_FUNC,
    /**< Null function provided for checking if a request is time-constraint */

  CM_DATA_SESSION_ERR_DATA_SESSION_TOTAL_BYTE_CTRS_FUNC_PREV_REG,
    /**< Get Daa Seesion Total Byte Ctr function already registered */

  CM_DATA_SESSION_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_data_session_status_e_type;

/** Type for get data session byte counters callback functions.
**
** This callback function is provided to CM so CM can obtain the
** number of total number of bytes received and transmitted during
** a data call.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, CM context simultaneously.
*/
typedef boolean (cm_get_data_session_total_byte_ctrs_cb_f_type) (

    cm_call_id_type                call_id,
    /**< call ID of the call */

    uint32                        *bytes_received,
    /**< Number of bytes received during the call */

    uint32                        *bytes_transmitted
    /**< Number of bytes transmitted during the call */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cm_get_data_session_total_byte_ctrs_cb_f_type
    ONERROR return FALSE */
/*~ PARAM OUT bytes_received POINTER */
/*~ PARAM OUT bytes_transmitted POINTER */
#endif


/**===========================================================================

@FUNCTION cm_data_session_reg

@DESCRIPTION
  Register data session callback functions with CM

  When call manager ends a data call, it consults the data session callback
  functions for the following information:

     1. Total number of bytes received and transmitted during the data call.

@DEPENDENCIES
  CM must have already been initialized with cm_init_before_task_start().

@RETURNS
  Status of registration

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern cm_data_session_status_e_type cm_data_session_reg(

  cm_get_data_session_total_byte_ctrs_cb_f_type
                                *data_session_total_byte_ctrs_cb_func_ptr
    /**< Pointer to data session total byte counters callback function */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_data_session_reg */
#endif


/** Register Supplementary Service Parameters.
*/
typedef struct cm_reg_ss_params_s {

  uint8                                ss_code;
    /**< SS code */

  uint8                                ss_ref;
    /**< SS reference */

  basic_service_s_type                 basic_service;
    /**< Basic service group information */

  cm_called_party_bcd_no_s_type        fwd_to_num;
    /**< Forwarded to number */

  cm_called_party_subaddress_s_type    fwd_to_subaddr;
    /**< Forwarded to subaddress */

  ie_no_reply_condition_timer_s_type   nr_timer;
    /**< No reply condition timer */

} cm_reg_ss_params_s_type;


/** Erase Supplementary Service Request
*/
typedef struct cm_erase_ss_params_s {

  uint8                     ss_ref;
    /**< SS reference */

  uint8                     ss_code;
    /**< SS code */

  basic_service_s_type      basic_service;
    /**< Basic service group information */

} cm_erase_ss_params_s_type;


/** Activate Supplementary Service Parameters
*/
typedef struct cm_act_ss_params_s {

  uint8                     ss_ref;
    /**< SS reference */

  uint8                     ss_code;
    /**< SS code */

  basic_service_s_type      basic_service;
    /**< Basic service group information */

} cm_act_ss_params_s_type;

/** Deactivate Supplementary Service Parameters
*/
typedef struct cm_deact_ss_params_s {

  uint8                     ss_ref;
    /**< SS reference */

  uint8                     ss_code;
    /**< SS code */

  basic_service_s_type      basic_service;
    /**< Basic service group information */


  cm_ccbs_indx_type         ccbs_indx;
    /**< Clients should check for CM_API_CCBS
    ** to use this field
    ** Field used only for ss_code == CCBS
    **
    ** ccbs_indx = CM_CCBS_INDX_INVALID, results
    **   deact of all pending ccbs requests.
    **
    ** ccbs_indx = valid, results in one particular
    **   ccbs getting deactivated.
  */

} cm_deact_ss_params_s_type;


/** Interrogate Supplementary Service Parameters
*/
typedef struct
{
  uint8                     ss_ref;
    /**< SS reference */

  uint8                     ss_code;
    /**< SS code */

  basic_service_s_type      basic_service;
    /**< Basic service group information */

  cm_ccbs_indx_type         ccbs_indx;
    /**< Clients should check for CM_API_CCBS
    ** to use this field
    ** Field used only for ss_code == CCBS
    **
    ** ccbs_indx = CM_CCBS_INDX_INVALID, results
    **   deact of all pending ccbs requests.
    **
    ** ccbs_indx = valid, results in one particular
    **   ccbs getting deactivated.
    */

} cm_interr_ss_params_s_type;


/** Register Password Parameters.
*/
typedef struct cm_reg_pwd_params_s {

  ie_ss_code_s_type       ss_code;
    /**< SS code */

  uint8                   ss_ref;
    /**< SS reference */

  ie_ss_password_s_type   ss_old_pwd;
    /**< Old password */

  ie_ss_password_s_type   ss_new_pwd;
    /**< New password */

  ie_ss_password_s_type   ss_new_pwd_again;
    /**< Confirmation of new password */

} cm_reg_pwd_params_s_type;

/** Process Unstructured SS Parameters
*/
typedef  struct cm_uss_params_s {

  uint8                  ss_ref;
    /**< SS reference */

  uss_data_s_type        uss_data;
    /**< Process unstructured SS request data */

} cm_uss_params_s_type;


/** Release Supplementary Service Parameters.
*/
typedef struct cm_release_ss_params_s {

  uint8                ss_ref;
    /**< SS reference */

  uint8                ss_oper;
    /**< SS operation to be released */

} cm_release_ss_params_s_type;


/** @}
*  endgroup DSTF Data Session Types & Functions
*/

/****************************************************************************

Number Classification Types & Functions

****************************************************************************/

/** @defgroup NCTF Number Classification Types & Functions
**                NUMBER CLASSIFICATION TYPES & FUNCTIONS
** This block group all the functionality that is associated with
** number-classification interfacing.
**
** When call manager is commanded to originate a call, it consults the
** number classification of the following information:
**
** 1. Whether the originating number matches one of the special
**    numbers that are stored in the number-classification (i.e. EMERGENCY, OTASP, etc.)
**    If a match is found, CM modify the originating call type accordingly.
**
** 2. After deciding on the originating call type, CM consults the
**    number-classification whether the originating number is allowed by the
**    origination restriction currently in effect. If originating number
**    is not allowed, CM blocks the outgoing call and notifies clients of
**    origination failure.
**
** Note that CM expects a UI like client to register a number-classification callback
** functions with CM during initialization time.
**
** Until such callback functions are registered with CM, CM uses a default
* hard-coded number-classification to decide on the call type of special numbers.


@{
*/

/** Type for number-classification callback function to determine whether an originating
** number matches one of the special number-classification number, such as EMERGENCY or
** OTASP. If a match is found, this function should indicate the originating
** call type via the call_type_ptr and if applicable the OTASP activation
** code via the activate_code_ptr.
*/
typedef void ( cm_nc_resolve_call_type_f_type ) (

  const cm_num_s_type      *orig_num_ptr,
    /**< originating number */

  cm_call_type_e_type      *call_type_ptr,
    /**< Associated call type */

  cm_activate_code_e_type  *activate_code_ptr
    /* Associated OTASP activation code (for OTASP call types) */

);


/** Type for number-classification callback function to determine whether an originating
** number is allowed by the origination restriction currently in effect. If
** originating number is allowed this function should return TRUE, otherwise
** this function should return FALSE.
*/
typedef boolean ( cm_nc_is_call_allowed_f_type ) (

  const cm_num_s_type      *orig_num_ptr
    /**< originating number */

);


/** Type for number-classification callback function to insert or delete 184 or 186
** number to original dial number strings as pre-pending dialing.
*/
typedef void ( cm_nc_pre_pending_dial_f_type ) (

  cm_num_s_type                  *orig_num_ptr,
    /**< originating number */

  const cm_call_type_e_type      *call_type_ptr
    /**< Associated call type */

);

/** Type to hold mode specific information returned by call control
*/
typedef struct cm_orig_call_control_s {

  cm_cc_result_e_type                call_control_result;
    /**< Indicates whether there were modifications */

  cm_alpha_s_type                    alpha;
    /**< alpha tag */

  cm_num_s_type                      num;
    /**< modified number */

  cm_activate_code_e_type            activate_code;
    /**< CDMA Only - associated OTASP activation code */

  cm_cap_config_s_type               ccp1;
    /**< capability configuration */

  cm_cap_config_s_type               ccp2;
    /**< capability configuration */

  cm_sups_cmd_e_type                 sups_cmd;
    /**< Indicates what sups command the dialed digits is
    ** is modified to.  Must be filled in if the call_control_result
    ** indicates a change to SS or USSD.
    */

  union
  {
    cm_erase_ss_params_s_type        erase_ss_params;
    cm_reg_ss_params_s_type          reg_ss_params;
    cm_act_ss_params_s_type          activate_ss_params;
    cm_deact_ss_params_s_type        deactivate_ss_params;
    cm_interr_ss_params_s_type       interrogate_ss_params;
    cm_uss_params_s_type             uss_params;
    cm_reg_pwd_params_s_type         reg_pwd_params;
    cm_release_ss_params_s_type      release_ss_params;
  } sups_params;
    /**< One of thes fields must be filled in if the call_control_result
    ** indicates a change to SS or USSD.
    */

  cm_gw_ps_cc_params_s_type          gw_ps_cc_params;
    /**< Clients need to check for CM_API_GW_PS_CC before
    ** using this structure.
    **
    ** Call control parameters for GW PS call
    */


} cm_orig_call_control_s_type;

/** Type for callback function to be called by a client when all
*  call control processing is complete.
*/
typedef void (cm_nc_call_control_complete_cb_f_type) (

  cm_call_id_type                         call_id,
    /**< call id */

  cm_cc_status_e_type                     status,
    /**< status of command */

  cm_call_type_e_type                     call_type,
    /**< call type */

  const cm_orig_call_control_s_type       *cdma_call_control_info_ptr,
    /**< pointer to an info struct - NULL if not allowed on cdma */

  const cm_orig_call_control_s_type       *gw_call_control_info_ptr,
    /**< pointer to an info struct - NULL if not allowed on gw */

  void                                    *user_data
   /**< data passed from original call */

);

/** Type for call control function to be registered by a client.  CM will call this
** function during an origination to resolve call types, check whether the call is
** allowed and do call control.
*/
typedef boolean (cm_nc_call_control_processing_f_type) (

  cm_call_id_type                       call_id,
    /**< call id */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /**< where CM is getting it's cdma subscription information */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /**< where CM is getting it's gw subscription information */

  cm_call_type_e_type                   call_type,
    /**< call type */

  cm_num_s_type                         *orig_num_ptr,
    /**< orginating number */

  void                                  *user_data,
    /**< Clients should not modify - Data to be passed back in cb function */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /**< function to be called when processing is complete */

  cm_gw_ps_cc_params_s_type             *gw_ps_cc_params
    /**< Clients need to check for CM_API_GW_PS_CC before
    ** using this structure.
    **
    ** Call control parameters for Packet data call
    */
);


/** Type for call control preferences function to be registered by a client.  CM will
** call this function after call control is complete to mediate between which modes
** to use in the case where the call is allowed on both modes.
*/
typedef cm_cc_mode_pref_e_type (cm_nc_call_control_preferences_f_type) (

  cm_orig_call_control_s_type            *cdma_call_control_info_ptr,
    /**< cdma information for the call */

  cm_orig_call_control_s_type            *gw_call_control_info_ptr,
    /**< gw information for the call */

  cm_num_s_type                          *orig_num_ptr,
    /**< Original number information before call control */

  cm_call_type_e_type                    call_type,
    /**< The type of call returned by call control */

  sys_sys_mode_e_type                    current_mode
    /* mode, if any, that is acquired right now */

);

/** Type for call control function to be registered by a client.  CM will call this
** function during an origination to resolve call types, check whether the call is
*  allowed and do call control, with or without FDN checking.
*/
typedef boolean (cm_nc_call_control_processing_mod_f_type) (

  cm_call_id_type                       call_id,
    /**< call id */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /**< where CM is getting it's cdma subscription information */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /**< where CM is getting it's gw subscription information */

  cm_call_type_e_type                   call_type,
    /**< call type */

  cm_num_s_type                         *orig_num_ptr,
    /**< orginating number */

  void                                  *user_data,
    /**< Clients should not modify - Data to be passed back in cb function */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /**< function to be called when processing is complete */

  cm_gw_ps_cc_params_s_type             *gw_ps_cc_params,
    /**< Clients need to check for CM_API_GW_PS_CC before
    ** using this structure.
    **
    ** Call control parameters for Packet data call
    */

  const cm_cc_generic_params_s_type   *cc_generic_params
    /**<
    ** Call control generic parameters for call
    */

);

/**===========================================================================

@FUNCTION cm_nc_reg

@DESCRIPTION
  Register number-classification callback functions with CM

  When call manager is commanded to originate a call, it consults the
  number-classification callback functions of the following information:

  1. Whether the originating number matches one of the special
     numbers that are stored in the number-classification (i.e. EMERGENCY, OTASP, etc.)
     If a match is found, CM modify the originating call type accordingly.

  2. After deciding on the originating call type, CM consults the
     number-classification whether the originating number is allowed by the
     origination restriction currently in effect. If originating number
     is not allowed, CM blocks the outgoing call and notifies clients of
     origination failure.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern cm_nc_status_e_type cm_nc_reg(

  cm_nc_resolve_call_type_f_type     *resolve_call_type_func,
    /**< pointer number-classification function resolving call types */

  cm_nc_is_call_allowed_f_type       *cm_nc_is_call_allowed_func,
    /**< pointer to a number-classification deciding whether a specific call type
    ** is allowed for origination */

  cm_nc_pre_pending_dial_f_type      *pre_pending_dial_func
    /**< pointer pre-pending function resolving origination # */

);


/**===========================================================================

@FUNCTION cm_nc_reg_with_call_control

@DESCRIPTION
  Register number-classification callback functions with CM

  When call manager is commanded to originate a call, it consults the
  number-classification callback function for the following information:

  1. Whether the originating number matches one of the special
     numbers that are stored in the number-classification (i.e. EMERGENCY, OTASP, etc.)
     If a match is found, CM modify the originating call type accordingly.

  2. Whether the originating number is allowed on the sims present.

  3. And if the calls are allowed and call control is required, perform any necessary
     modifications.


@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern cm_nc_status_e_type cm_nc_reg_with_call_control(

  cm_nc_call_control_processing_f_type    *cm_nc_call_control_processing_func,
    /**< ptr to function that will resolve call type, check whether the call is
       allowed, and do any call control modifications if necessary and for
       JCDMA any pre pending dial information.
    */

  cm_nc_call_control_preferences_f_type   *cm_nc_call_control_preferences_func
    /**< ptr to function that will resolve which mode to originate on when the
       call is allowed on both slots and/or the modifications are conflicting.
    */

);

/**===========================================================================

@FUNCTION cm_nc_reg_with_call_control_mod

@DESCRIPTION
  Modified Register number-classification callback functions with CM with
  option to not check for FDN

  When call manager is commanded to originate a call, it consults the
  number-classification callback function for the following information:

  1. Whether the originating number matches one of the special
     numbers that are stored in the number-classification (i.e. EMERGENCY, OTASP, etc.)
     If a match is found, CM modify the originating call type accordingly.

  2. Whether the originating number is allowed on the sims present.

  3. And if the calls are allowed and call control is required, perform any necessary
     modifications.


@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern cm_nc_status_e_type cm_nc_reg_with_call_control_mod(

  cm_nc_call_control_processing_mod_f_type    *cm_nc_call_control_processing_func_mod,
    /**< ptr to function that will resolve call type, check whether the call is
       allowed, and do any call control modifications if necessary and for
       JCDMA any pre pending dial information.
    */

  cm_nc_call_control_preferences_f_type   *cm_nc_call_control_preferences_func
    /**< ptr to function that will resolve which mode to originate on when the
       call is allowed on both slots and/or the modifications are conflicting.
    */

);


/** @}
* endgroup NCTF Number Classification Types & Functions
*/


/****************************************************************************

Phone Object

****************************************************************************/

/** @defgroup PHONE Phone Related Definitions
** @{
*/


/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

/* Invalid user zone id
*/
#define CM_INVALID_UZID         (-1)

/* Default avoid time for avoid system command from UI == 100 hours
*/
#define CM_DFLT_AVOID_SYS_TIME  (360000)


/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/** Enumeration of phone commands client are able to send to CM.
*/
typedef enum cm_ph_cmd_e {

  CM_PH_CMD_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PH_CMD_OPRT_MODE,
    /**< Change the operating mode */

  CM_PH_CMD_SYS_SEL_PREF,
    /**< Change the system selection pref - SD20 */

  CM_PH_CMD_ANSWER_VOICE,
    /**< Change the answer-voice-as-data setting. CDMA Only */

  CM_PH_CMD_NAM_SEL,
    /**< Change the NAM selection. CDMA Only */

  /* 4 */
  CM_PH_CMD_CLIENT_ACT,
    /**< A CM client activated itself. Internal Command. No client
    ** can invoke this command */

  CM_PH_CMD_INFO_GET,
    /**< Get a sanpshot of the phone information in the CM */

  CM_PH_CMD_RSSI_DELTA,
    /**< Set RSSI change delta threshold for RSSI reporting */

  CM_PH_CMD_SUBSCRIPTION_AVAILABLE,
    /**< Subscription info available for use */

  /* 8 */
  CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE,
    /**< Subscription info not available for use */

  CM_PH_CMD_SUBSCRIPTION_CHANGED,
    /**< Subscription info changed for use */

  CM_PH_CMD_GET_NETWORKS,
    /**< Get the preferred or available networks. GSM/WCDMA Only */

  CM_PH_CMD_TERMINATE_GET_NETWORKS,
    /**< Terminate the request to get the preferred
    ** or available networks. GSM/WCDMA Only */

  /* 12 */
  CM_PH_CMD_SET_PREFERRED_NETWORKS,
    /**< Client made a request to save
    ** the preferred network list to SIM/USIM. GSM/WCDMA Only. */

  CM_PH_CMD_PACKET_STATE,
    /**< Set Rm packet state. CDMA Only */

  CM_PH_CMD_VOC_ACTIVITY,
    /**< Voice activity. CDMA Only - FEATURE_JCDMA*/

  CM_PH_CMD_AVOID_SYS,
    /**< Command to avoid a particular system. CDMA Only */

  /* 16 */
  CM_PH_CMD_WAKEUP_FROM_STANDBY,
    /**< Command to wake up lower layers from deep sleep (standby) */

  CM_PH_CMD_CHANGE_RTRE_CONFIG,
    /**< Command to change RTRE configuration. CDMA Only */

  CM_PH_CMD_DDTM_PREF,
    /**< data dedicated transmission mode preference */

  CM_PH_CMD_RESET_ACM,
    /**< Reset the ACM to 0. This also resets the ability for CM to send
     * the FUNDS_LOW message.  */

  /* 20 */
  CM_PH_CMD_SET_ACMMAX,
    /**< Set the ACMmax to the value given. This command could cause
     * the FUNDS_LOW message to be resent if the new value would cause
     * calls to end within an estimated 30 seconds.  */

  CM_PH_CMD_LINE_SWITCHING,
    /**<
    ** Allow or prevent the user from switching lines
    ** (i.e changing selected line).
    ** Sent to CM by UI client
    ** FEATURE_ALS
    */

  CM_PH_CMD_SELECTED_LINE,
    /**<
    ** Set selected line,
    ** Sent to CM by UI client
    ** FEATURE_ALS
    */

  CM_PH_CMD_ALS_SIM_INFO_AVAIL,
    /**<
    ** All ALS info is available.
    ** Sent to CM by ALS custom when all ALS related items
    ** have been read from the SIM.
    ** FEATURE_ALS
    */

  /* 24 */
  CM_PH_CMD_LINE_SWITCHING_SAVED,
    /**<
    ** Sent to CM by ALS custom when line switching
    ** setting has been written to the SIM
    ** Update setting in CM and notify clients.
    ** FEATURE_ALS
    */

  CM_PH_CMD_SELECTED_LINE_SAVED,
    /**<
    ** Sent to CM by ALS custom when selected line
    ** has been written to the SIM
    ** Update setting in CM and notify clients.
    ** FEATURE_ALS
    */

  CM_PH_CMD_UPDATE_CDMA_CAPABILITY,
    /**< Update the CDMA capability information.
    */

  CM_PH_CMD_CLIENT_RELEASE,
    /**< @internal
    ** Release the client object associated
    ** with the deregistering client.
    */

  /* 28 */
  CM_PH_CMD_SIGNAL_STRENGTH_DELTA,
    /**<
    ** Set RSSI/ECIO/IO change delta threshold for Signal Strength reporting
    */

  /* 29 */
  CM_PH_CMD_WPS_START,
    /**< Command to get WLAN security credentials using WPS procedure.
    */

  /* 30 */
  CM_PH_CMD_WPS_ABORT,
    /**< Command to abort the WPS procedure.
    */

  CM_PH_CMD_MEAS_MODE,
    /**< Command to put the MS in measurement mode.
    ** Currently applicable only for HDR . 
    ** In this mode, HDR disables network registration.
    */

   CM_PH_CMD_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_ph_cmd_e_type;


/** Enumeration of phone command errors returned by phone command
** callback functions.
*/
typedef enum cm_ph_cmd_err_e {

  CM_PH_CMD_ERR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PH_CMD_ERR_NOERR,
    /**< No errors found */

  CM_PH_CMD_ERR_ANSWER_VOICE_F,
    /**< Answer voice feature is not defined */

  /* Error codes resulting from clients feeding a phone command
  ** function with bad or inappropriate parameters.
  */

  CM_PH_CMD_ERR_CLIENT_ID_P,
    /**< Invalid client ID; null or not initialized */

  CM_PH_CMD_ERR_OPRT_MODE_P,
    /**< Bad operating mode parameter */

  /* 4 */
  CM_PH_CMD_ERR_MODE_PREF_P,
    /**< Bad mode preference parameter */

  CM_PH_CMD_ERR_PREF_TERM_P,
    /**< Bad mode term parameter */

  CM_PH_CMD_ERR_SYS_PREF_P,
    /**< Bad system preference parameter - obsolete in SD 2.0 */

  CM_PH_CMD_ERR_BAND_PREF_P,
    /**< Bad band preference parameter */

  /* 8 */
  CM_PH_CMD_ERR_ROAM_PREF_P,
    /**< Bad roam preference parameter */

  CM_PH_CMD_ERR_ANSWER_VOICE_P,
    /**< Bad answer voice parameter. CDMA Only */

  CM_PH_CMD_ERR_ANSWER_DURATION_P,
    /**< Bad answer duration parameter. When indicating a temp. setting,
    ** such as CM_ANSWER_VOICE_AS_FAX_ONCE or DB_VOICE_AS_MODEM_ONCE
    ** a non-zero duration must be indicated, as well. CDMA Only */

  CM_PH_CMD_ERR_NAM_P,
    /**< Bad NAM parameter. CDMA Only */

  /* 12 */
  CM_PH_CMD_ERR_PACKET_STATE_P,
    /**< Bad packet state parameter. CDMA Only */

  CM_PH_CMD_ERR_HYBR_PREF_P,
   /**< Hybrid preference */

  CM_PH_CMD_ERR_SERVICE_DOMAIN_P,
    /**< Invaild service domain parameter. GSM/WCDMA Only */

  CM_PH_CMD_ERR_ACQ_ORDER_PREF_P,
    /**< Invalid acquisition order preference parameter. GSM/WCDMA Only */

  /* 16 */
  CM_PH_CMD_ERR_NETWORK_LIST_TYPE_P,
    /**< Invalid network list type parameter. GSM/WCDMA Only */

  CM_PH_CMD_ERR_NETWORK_SEL_MODE_P,
    /**< Invalid network selection mode parameter. GSM/WCDMA Only */

  CM_PH_CMD_ERR_SYSTEM_MODE_P,
    /**< Invalid system mode parameter. GSM/WCDMA Only */

  CM_PH_CMD_ERR_PLMN_P,
    /**< Invalid PLMN identifier parameter. GSM/WCDMA Only */

  /* 20 */
  CM_PH_CMD_ERR_RTRE_CONFIG_P,
    /**< Invalid RTRE configuration parameter. CDMA Only */

  CM_PH_CMD_ERR_CDMA_CAPABILITY_P,
    /**< Invalid CDMA capability parameter error. */

  CM_PH_CMD_ERR_PRL_PREF_P,
    /**< Bad prl preference parameter */

  CM_PH_CMD_ERR_WLAN_PREF_P,
    /**< Bad WLAN preference parameter */
    /* Clients check CM_API_WLAN for WLAN support */

  /* Error codes resulting from clients trying to instruct
  ** CM to perform inappropriate actions relative to the current
  ** state of the phone.
  */

  /* 24 */
  CM_PH_CMD_ERR_IN_USE_S,
    /**< Phone is in-use (i.e. in a call) */

  CM_PH_CMD_ERR_ONLINE_S,
    /**< Phone is offline */

  CM_PH_CMD_ERR_OPRT_MODE_S,
    /**< Invalid command or operating mode switch relatively to the current
    ** operating mode - e.g. OFFLINE -> ONLINE */

  CM_PH_CMD_ERR_MODE_PREF_S,
    /**< Invalid mode preference selection relatively to what we
    ** currently have in the PRL - e.g. ANALOG_ONLY when there are no
    ** analog entries in the PRL */

  /* 28 */
  CM_PH_CMD_ERR_SYS_PREF_S,
    /**< Invalid system preference selection for this target
    ** - e.g. A_ONLY for a none 800 MHz target, etc. - obsolete in SD20*/

  CM_PH_CMD_ERR_SYS_SEL_PREF_S,
    /**< Invalid system selection preference for this target/PRL
    ** - e.g. band-class 1 for a PCS only target
    **
    ** Also returned when CM_NETWORK_SEL_MODE_PREF_HPLMN_SRCH is requested other
    ** than in Automatic mode */

  CM_PH_CMD_ERR_HYBR_PREF_S,
    /**< FEATURE_HDR - Invalid command or hybrid preference selection */

  CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S,
    /**< Invalid command relative to the current value of subscription
    ** available - e.g. transitioning from subscription available to
    ** subscription available is invalid or issuing a command when
    ** the subscription information is not available.  */

  /* 32 */
  CM_PH_CMD_ERR_GET_NETWORKS_S,
    /**< A terminate get networks request was made and
    ** a request for available network list was not previously made.
    **
    ** OR
    **
    ** A get network request was made while a previous get network
    ** request for different technology was still pending.
    **/

  /* Error codes resulting from operational limitation
  ** of CM.
  */

  CM_PH_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command */

  CM_PH_CMD_ERR_MC_COM_L,
    /**< MC communication problems */

  /* Error codes related to "Avoid system" command.
  */

  CM_PH_CMD_ERR_AVOID_SYS_P,
      /**< Atleast one of the parameters in "Avoid system"
      ** command from phone client was invalid. CDMA Only
      */

  /* 36 */
  CM_PH_CMD_ERR_SYS_NOT_IN_UZ,
      /**< Indicates that the MS is not in user zone, when
      ** "Avoid system" on user zone was processed. CDMA Only
      */

  CM_PH_CMD_ERR_OTHER,
    /**< A problem other than the above was found */

  CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  CM_PH_CMD_ERR_CURRENT_NAM_IS_NOT_RUIM_NAM_S,
    /**< Current NAM is not RUIM NAM. CDMA Only */

  /* 40 */
  CM_PH_CMD_ERR_USER_PREFERRED_NETWORK_LIST_ACCESS,
    /**< An error occurred accessing the user preferred network list.
    ** GSM/WCDMA Only. */

  CM_PH_CMD_ERR_DDTM_PREF_P,
    /**< Bad DDTM pref parameter
    */

  CM_PH_CMD_ERR_DDTM_NUM_SRV_OPTIONS_P,
    /**< Bad number of service options.
    */

  CM_PH_CMD_ERR_LINE_SWITCHING_P,
    /**<
    ** Invalid line switching parameter
    ** FEATURE_ALS
    */

  /* 44 */
  CM_PH_CMD_ERR_LINE_SWITCHING_S,
    /**<
    ** Client requested that selected line be changed
    ** but line switching is not allowed
    ** FEATURE_ALS
    */

  CM_PH_CMD_ERR_SELECTED_LINE_P,
    /**<
    ** Invalid selected line parameter
    ** FEATURE_ALS
    */

  CM_PH_CMD_ERR_ALS_NOT_SUPPORTED_S,
    /**<
    ** ALS is not supported and therefore ALS
    ** cmd cannot be processed
    ** FEATURE_ALS
    */

  CM_PH_CMD_ERR_ALS_SIM_WRITE_FAILED_S,
    /**<
    ** SIM write failed
    ** FEATURE_ALS
    */

  CM_PH_CMD_ERR_AOC_NOT_READY_S,
     /**< Values related to advice of charge cannot be
     ** read or written at this time.
     **
     ** Clients need to check for CM_API_AOC_INFO before
     ** using this enum.
     */

  /* 49 */
  CM_PH_CMD_ERR_NET_TYPE_P,
    /**< Invalid Network type requested in
    ** cm_ph_cmd_get_networks API.
    ** Clients check CM_API_WLAN for WLAN support */

  CM_PH_CMD_ERR_DDTM_SRV_OPTIONS_LIST_P,
    /**< Bad service options pointer
    */

  CM_PH_CMD_ERR_INVALID_HYBR_PREF_MODE_PREF,
    /**< Invalid Hybr pref and mode pref combination
    */

  /* 53 */
  CM_PH_CMD_ERR_MODE_PREF_CHANGE_NOT_ALLOWED_S,
    /**< mode pref change is not allowed 
    */

  CM_PH_CMD_ERR_INVALID_WPS_MODE,
    /**< Invalid WPS mode used in WPS start command.
    */

  CM_PH_CMD_ERR_INVALID_WPS_CONFIG_METHOD,
    /**< Invalid WPS config method used in WPS start command.
    */

  CM_PH_CMD_ERR_INVALID_WPS_DEV_PASSWD,
    /**< Invalid WPS device password used in WPS start command.
    */

  /* 57 */
  CM_PH_CMD_ERR_NO_WPS_IN_PROGRESS,
    /**< Client request to abort WPS even when it is not started.
    */

  CM_PH_CMD_ERR_WPS_ALREADY_IN_PROGRESS,
    /**< Client send another WPS Start request to abort WPS when 
    ** the previous request has not been completed yet.
    */

  CM_PH_CMD_ERR_NO_SUBSCRIPTION_S,
    /**< Phone command cannot be processed because subscription  
    ** is not available. WLAN only 
    */

  CM_PH_CMD_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_ph_cmd_err_e_type;


/** Enumeration of phone events.
** Clients are able to register with CM to be notified of any subsets
** of these phone events.
*/
typedef enum cm_ph_event_e {

  CM_PH_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PH_EVENT_OPRT_MODE,
    /**< Operating mode was changed */

  CM_PH_EVENT_TEST_CONTROL_TYPE,
    /**< Operating mode type needs to be updated*/

  CM_PH_EVENT_SYS_SEL_PREF,
    /**< System Selection pref was changed */

  CM_PH_EVENT_ANSWER_VOICE,
    /**< Answer voice as data was changed. CDMA Only. */

  /* 4 */
  CM_PH_EVENT_NAM_SEL,
    /**< NAM selection was changed. CDMA Only.  */

  CM_PH_EVENT_CURR_NAM,
    /**< Current NAM was changed. CDMA Only.  */

  CM_PH_EVENT_IN_USE_STATE,
    /**< In use state was changed */

  CM_PH_EVENT_CDMA_LOCK_MODE,
    /**< CDMA lock mode was changed. CDMA Only.  */

  /* 8 */
  CM_PH_EVENT_UZ_CHANGED,
    /**< User zone selection was changed. CDMA Only.  */

  CM_PH_EVENT_MAINTREQ,
    /**< CDMA maintenance required command. CDMA Only.  */

  CM_PH_EVENT_STANDBY_SLEEP,
    /**< Entering powerdown sleep mode */

  CM_PH_EVENT_STANDBY_WAKE,
    /**< Existing powerdown sleep mode */

  /* 12 */
  CM_PH_EVENT_INFO,
    /**< Phone information, this event only sent to the client
    ** that request this information through cm_ph_cmd_get_ph_info */

  CM_PH_EVENT_PACKET_STATE,
    /**< Packet state was changed. CDMA Only. */

  CM_PH_EVENT_INFO_AVAIL,
    /**< Phone information is now available. Automatically generated
    ** after a client does activation */

  CM_PH_EVENT_SUBSCRIPTION_AVAILABLE,
    /**< A client is informing CM that the subscription info is available */

  /* 16 */
  CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE,
    /**< A client is informing CM that the subscription info is not available */

  CM_PH_EVENT_SUBSCRIPTION_CHANGED,
    /**< A client is informing CM that the subscription info is changed */

  CM_PH_EVENT_AVAILABLE_NETWORKS_CONF,
    /**< The requested available networks are available. GSM/WCDMA/WLAN Only. */

  CM_PH_EVENT_PREFERRED_NETWORKS_CONF,
    /**< The requested preferred networks are available. GSM/WCDMA Only */

  /* 20 */
  CM_PH_EVENT_FUNDS_LOW,
    /**< Funds running low. GSM/WCDMA Only. */

  CM_PH_EVENT_WAKEUP_FROM_STANDBY,
    /**< Command is sent to lower layers to wake up from deep sleep(standby) */

  CM_PH_EVENT_NVRUIM_CONFIG_CHANGED,
    /**< RTRE configuration changed. CDMA Only */

  CM_PH_EVENT_PREFERRED_NETWORKS_SET,
    /**<
    ** The requested preferred networks were
    ** written to the (U)SIM. GSM/WCDMA Only
    */

  /* 24 */
  CM_PH_EVENT_DDTM_PREF,
    /**< DDTM pref change  */

  CM_PH_EVENT_PS_ATTACH_FAILED,
    /**<
    ** A client request for Packet Service attach failed.  Sent if the
    ** cm_ph_cmd_sys_sel_pref() API is called with the srv_domain_pref
    ** parameter set to CM_SRV_DOMAIN_PREF_PS_ATTACH and the PS attach
    ** fails.
    */
  CM_PH_EVENT_RESET_ACM_COMPLETED,
    /**< As requested the ACM has been reset on the (U)SIM. GSM/WCDMA Only */

  CM_PH_EVENT_SET_ACMMAX_COMPLETED,
    /**< The requested value was written to the (U)SIM. GSM/WCDMA Only */

  /* 28 */
  CM_PH_EVENT_CDMA_CAPABILITY_UPDATED,
    /**< A event to indicate that the CDMA capability was updated. */

  CM_PH_EVENT_LINE_SWITCHING,
    /**<
    ** Line switching setting has been updated in CM
    ** as per client's request
    ** FEATURE_ALS
    */

   CM_PH_EVENT_SELECTED_LINE,
    /**<
    ** Selected line setting has been updated in CM
    ** as per client's request
    ** FEATURE_ALS
    */

  CM_PH_EVENT_SECONDARY_MSM,
    /**< Invoke event call back even if the event is from secondary MSM.
    ** CM will not generate any events with this, but is used by
    ** thin client CM layer to call client call backs for events from
    ** secondary msm.
    */

  /* 32 */
  CM_PH_EVENT_TERMINATE_GET_NETWORKS,
    /**< Indicate that the current get network request is terminated.
    */

  CM_PH_EVENT_DDTM_STATUS,
    /**< update the DDTM status.
    ** DDTM status = ON if either
    **    (DDTM_PREF is ON)  AND (GPS fix is in progress)
    **         OR
    **    (DDTM_PREF is ON) AND ( In hybrid HDR call and no MO SMS and no MO DBM)
    ** Event triggered if the DDTM status changes
    */

  CM_PH_EVENT_CCBS_STORE_INFO_CHANGED,
    /**< Clients need to check for CM_API_CCBS before
    ** using this event.
    ** Event generated when an entry is added to or removed from
    ** ccbs_store_info of phone event structure
    */

  CM_PH_EVENT_WPS_COMPLETED,
    /**< Event generated when the WPS procedure has been completed.
    ** Completed does not necessarily mean any credential was obtained.
    ** Clients need to check for CM_API_WLAN_WPS.
    */

    /* 36 */
  CM_PH_EVENT_WPS_ABORTED,
    /**< Event generated when the WPS procedure is aborted by a client.
    ** Clients need to check for CM_API_WLAN_WPS.
    */

  CM_PH_EVENT_WPS_START,
    /**< Event generated when the WPS security credentials are requested by the clients
    ** Clients need to check for CM_API_WLAN_WPS.
    */

  CM_PH_EVENT_MEAS_MODE,
    /**< Event generated when the measurement mode change is requested by a 
    ** client
    */

  CM_PH_EVENT_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_ph_event_e_type;


/** The current state of the phone.
*/
typedef enum cm_ph_state_e {

  CM_PH_STATE_NONE = -1,
    /**< @internal */

  CM_PH_STATE_NORMAL,
    /**< The phone is in Normal operation */

  CM_PH_STATE_EMERG_CB,
    /**< The phone is in Emergency call back mode */

  CM_PH_STATE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */
} cm_ph_state_e_type;

/** Enumeration of mode preferences.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_mode_pref_e {

  CM_MODE_PREF_NONE=-1,
    /**< @internal */

  CM_MODE_PREF_AMPS_ONLY=0,
    /**< = NV_MODE_ANALOG_ONLY, Service is limited to analog only */

  CM_MODE_PREF_DIGITAL_ONLY=1,
    /**< = NV_MODE_DIGITAL_ONLY, Service is limited to digital
    ** (CDMA, HDR, GW) only */

  CM_MODE_PREF_AUTOMATIC=2,
    /**< = NV_MODE_AUTOMATIC, Determine the mode automatically */

  CM_MODE_PREF_EMERGENCY=3,
    /**< = NV_MODE_EMERGENCY, Emergency mode
    **
    **  It's intended to be used internally by CM.
    **  Client is not allowed to use it as parameter to change system
    **  selection preference command.
    **
     */

  /* For compatibility with QPST, do not change values or
  ** order. We start with NV_MODE_CELL_CDMA_ONLY+1 (i.e. 9).
  */
  CM_MODE_PREF_CDMA_ONLY = 9,
    /**<
    ** Service is limited to CDMA only
    */

  CM_MODE_PREF_HDR_ONLY=10,
    /**<
    ** Service is limited to HDR only
    */

  CM_MODE_PREF_CDMA_AMPS_ONLY=11,
    /**<
    ** Service is limited to CDMA and AMPS only
    */

  CM_MODE_PREF_GPS_ONLY=12,
    /**<
    ** Service is limited to GPS only
    */

  /*
  ** The following enums are the radio access technologies for
  ** WCDMA and GSM.
  */
  CM_MODE_PREF_GSM_ONLY=13,
    /**<
    ** Service is limited to GSM only
    */

  CM_MODE_PREF_WCDMA_ONLY=14,
    /**<
    ** Service is limited to WCMDA only
    */

  /*
  ** Others
  */
  CM_MODE_PREF_PERSISTENT=15,
    /**<
    ** Return to persistent mode preference.
    ** Will be mapped to sth else by CM,
    ** ph_ptr->mode_pref is never set to this value
    */

  CM_MODE_PREF_NO_CHANGE=16,
    /**<
    ** Do not change the mode preference.
    ** Will be mapped to sth else by CM,
    ** ph_ptr->mode_pref is never set to this value
    */

  /*
  ** Even though logically the below belog with
  ** enums listed above, they were added at the end
  ** to not break compatibility with test tools
  */

  CM_MODE_PREF_ANY_BUT_HDR=17,
    /**<
    ** Use any service but HDR
    */

  CM_MODE_PREF_CURRENT_LESS_HDR=18,
    /**<
    ** Remove HDR component from current mode preference.
    ** Will be mapped to sth else by CM,
    ** ph_ptr->mode_pref is never set to this value
    */

  CM_MODE_PREF_GSM_WCDMA_ONLY=19,
    /**<
    ** Service is limited to GSM and WCDMA only
    */

  CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY=20,
    /**< Acquire digital, non-HDR mode systems only
    ** (CDMA or GSM or WCDMA )
    */

  CM_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS=21,
    /**<
    ** Remove HDR and AMPS component from current mode preference.
    ** Will be mapped to sth else by CM,
    ** ph_ptr->mode_pref is never set to this value
    */

  CM_MODE_PREF_CDMA_HDR_ONLY=22,
    /**<
    ** Acquire CDMA or HDR systems only
    */

  CM_MODE_PREF_CDMA_AMPS_HDR_ONLY=23,
    /**<
    ** Acquire CDMA or AMPS or HDR systems only
    */

  CM_MODE_PREF_CURRENT_LESS_AMPS=24,
    /**<
    ** Remove AMPS component from current mode preference.
    ** Will be mapped to sth else by CM,
    ** ph_ptr->mode_pref is never set to this value
    */
  /* Clients check CM_API_WLAN for WLAN support for following
  ** preferences.
  */
  CM_MODE_PREF_WLAN_ONLY=25,
    /**<
    ** Acquire WLAN systems only
    */

  CM_MODE_PREF_CDMA_WLAN=26,
    /**<
    ** Acquire CDMA and WLAN systems only
    */

  CM_MODE_PREF_HDR_WLAN=27,
    /**<
    ** Acquire HDR and WLAN systems only
    */

  CM_MODE_PREF_CDMA_HDR_WLAN=28,
    /**<
    ** Acquire CDMA, HDR and WLAN systems only
    */

  CM_MODE_PREF_GSM_WLAN=29,
    /**<
    ** Acquire GSM and WLAN systems only
    */

  CM_MODE_PREF_WCDMA_WLAN=30,
    /**<
    ** Acquire WCDMA and WLAN systems only
    */

  CM_MODE_PREF_GW_WLAN=31,
    /**<
    ** Acquire GSM/WCDMA and WLAN systems only
    */

  CM_MODE_PREF_CURRENT_PLUS_WLAN=32,
    /**<
    ** Acquire WLAN systems in addition to current system
    */

  CM_MODE_PREF_CURRENT_LESS_WLAN=33,
    /**<
    ** Remove WLAN systems from the current system
    */

  CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY = 34,
    /**<
    ** Acquire CDMA, AMPS, HDR and WLAN systems.
    */

  CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY = 35,
    /**<
    ** Acquire CDMA, AMPS and WLAN systems.
    */

  CM_MODE_PREF_INTERSECT_OR_FORCE = 36,
    /**< Acquire the mode that is common (intersection of) the current mode and
    ** the requested mode, if the intersection is NULL, then Force the
    ** requested preferences
    */

  CM_MODE_PREF_ANY_BUT_HDR_WLAN = 37,
    /**<
    ** Use any service except HDR and WLAN
    */

    /**< @internal */

  CM_MODE_PREF_MAX
    /* FOR INTERNAL USE OF CM ONLY!
    */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_mode_pref_e_type;


/** Enumeration of GSM/WCDMA acquisition order
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_gw_acq_order_pref_e {

  CM_GW_ACQ_ORDER_PREF_NONE = -1,
    /**< @internal */

  CM_GW_ACQ_ORDER_PREF_AUTOMATIC=0,
    /**< Determine mode automatically from the PRL order */

  CM_GW_ACQ_ORDER_PREF_GSM_WCDMA=1,
    /**< Acqisition order is first GSM followed by WCDMA */

  CM_GW_ACQ_ORDER_PREF_WCDMA_GSM=2,
    /**< Acqisition order is first WCDMA followed by GSM */

  CM_GW_ACQ_ORDER_PREF_NO_CHANGE=3,
    /**< Do not change GSM/WCDMA acquisition order */

  CM_GW_ACQ_ORDER_PREF_PERSISTENT=4,
    /**< Return to acq order preference given by NV.
    **
    ** Clients need to check for CM_API_PERSISTENT_PREF
    ** before using this
    */

  CM_GW_ACQ_ORDER_PREF_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_gw_acq_order_pref_e_type;


/** Enumeration of mode terms - i.e designate the mode span.
*/
typedef enum cm_pref_term_e {

  CM_PREF_TERM_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PREF_TERM_PERMANENT,
    /**< Permanent mode change */

  CM_PREF_TERM_PWR_CYCLE,
    /**< Until power cycle */

  CM_PREF_TERM_1_CALL,
    /**< Until end of next call or power-cycle */

  CM_PREF_TERM_1_CALL_OR_TIME,
    /**< Until end of next call, specified timeout or power-cycle */

  /* 4 */
  CM_PREF_TERM_CM_1_CALL,
  CM_PREF_TERM_CM_1_CALL_PLUS,
  CM_PREF_TERM_CM_MO_SMS,
    /**< Mode terms that are assigned automatically by call manager
    ** when mode is forced on the fly during a call origination or
    ** mobile originated SMS.
    **
    ** CM unforce the mode back to it original settings at end of
    ** call or some short time after end of call/message transmission.
    **
    ** NOTE! These mode term should NOT be used by clients
    ** when changing the mode preference */

  CM_PREF_TERM_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_pref_term_e_type;


/** Enumeration of band preferences.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
/* The following values need to be consistant with sd_ss_band_pref_e_type and
** sys_band_mask_e_type. Any enum added here need to be updated in above two
** places.
*/

/* CM Band prefernece is a mask of various bands. Each Bit represents
** a particluar band. Bit location of each band is specified in the
** sys.h in sys_sband_e_type enumeration.
**
** Please Note that certain bits are reserved as evident in bit numbering
** of sys_sband_e_type.
**
** Band_PREF ANY is a special value. We reserve Bit number 31 as a special
** Bit. If this Bit is set, then subsequent number forming out of it
** carry a special meaning, that is reserved for future. For example,
** CM_BAND_PREF_NO_CHANGE is right after CM_BAND_PREF_ANY. This has the
** effect of setting the 31st bit to 1 and takes a special meaning.
**
** If we were to add more magic valued like this, they all be after
** BAND_PREF_NO_CHANGE and in all these case, Bit 31 will be set and
** would signify a special value. Since it is 31 bit space afterwards,
** we don't anticipate to outgrow this.
**
**
*/

/** 64 Bit cm_band_pref is to be used if CM_API_BAND_PREF_64_BIT is defined.
*/

typedef uint64 cm_band_pref_e_type;

#define  CM_BAND_PREF_NONE   (cm_band_pref_e_type)0
    /**< @internal */

#define  CM_BAND_PREF_BC0_A  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC0_A))
    /**< Acquire band class 0, A-Side systems only */

#define  CM_BAND_PREF_BC0_B  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC0_B))
    /**< Acquire band class 0, B-Side systems only */

#define  CM_BAND_PREF_BC0    ((cm_band_pref_e_type)(SYS_BM_64BIT(SYS_SBAND_BC0_A)|SYS_BM_64BIT(SYS_SBAND_BC0_B)))
    /**< Acquire band class 0 systems only */

#define  CM_BAND_PREF_BC1    ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC1))
    /**< Acquire band class 1 systems only */

#define  CM_BAND_PREF_BC3    ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC3))
    /**< Acquire band class 3 systems only */

#define  CM_BAND_PREF_BC4    ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC4))
    /**< Acquire band class 4 systems only */

#define  CM_BAND_PREF_BC5    ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC5))
    /**< Acquire band class 5 systems only */

#define  CM_BAND_PREF_GSM_DCS_1800  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_GSM_DCS_1800))
    /**< Acquire GSM DCS systems only */

#define  CM_BAND_PREF_GSM_EGSM_900  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_GSM_EGSM_900))
    /**< Acquire GSM Extended GSM 900 systems only */

#define  CM_BAND_PREF_GSM_PGSM_900  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_GSM_PGSM_900))
    /**< Acquire GSM Primary GSM 900 systems only */

#define  CM_BAND_PREF_BC6  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC6))
    /**< Acquire band class 6 systems only */

#define  CM_BAND_PREF_BC7  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC7))
    /**< Acquire band class 7 systems only */

#define  CM_BAND_PREF_BC8 ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC8))
    /**< Acquire band class 8 systems only */

#define  CM_BAND_PREF_BC9  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC9))
    /**< Acquire band class 9 systems only */

#define  CM_BAND_PREF_BC10  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC10))
    /**< Acquire band class 10 systems only */

#define  CM_BAND_PREF_BC11  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC11))
    /**< Acquire band class 11 systems only */

/** Clients should check for CM_API_BAND_CLASS_12_14_15_16
** before using this macro
*/
#define  CM_BAND_PREF_BC12  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC12))
    /**< Acquire band class 12 systems only */

/** Clients should check for CM_API_BAND_CLASS_12_14_15_16
** before using this macro
*/
#define  CM_BAND_PREF_BC14  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC14))
    /**< Acquire band class 14 systems only */

/** Clients should check for CM_API_BAND_CLASS_12_14_15_16
** before using this macro
*/
#define  CM_BAND_PREF_BC15  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC15))
    /**< Acquire band class 15 systems only */

/** Clients should check for CM_API_BAND_CLASS_12_14_15_16
** before using this macro
*/
#define  CM_BAND_PREF_BC16  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC16))
    /**< Acquire band class 16 systems only */

#define  CM_BAND_PREF_BC17  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC17))
    /**< Acquire band class 17 systems only */

#define  CM_BAND_PREF_BC18  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC18))
    /**< Acquire band class 18 systems only */

#define  CM_BAND_PREF_BC19  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_BC19))
    /**< Acquire band class 19 systems only */

#define  CM_BAND_PREF_GSM_450  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_GSM_450 ))
    /**< Acquire GSM 450 systems only */

#define  CM_BAND_PREF_GSM_480  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_GSM_480 ))
    /**< Acquire GSM 480 systems only */

#define  CM_BAND_PREF_GSM_750  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_GSM_750 ))
    /**< Acquire GSM 750 systems only */

#define  CM_BAND_PREF_GSM_850  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_GSM_850 ))
    /**< Acquire GSM 850 systems only */

#define  CM_BAND_PREF_GSM_RGSM_900  ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_GSM_RGSM_900))
    /**< Acquire GSM Railway GSM 900 systems only */

#define  CM_BAND_PREF_GSM_PCS_1900  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_GSM_PCS_1900 ))
    /**< Acquire GSM PCS systems only */

#define  CM_BAND_PREF_WCDMA_I_IMT_2000  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_I_IMT_2000 ))
    /**< Acquire WCDMA I IMT 2000 systems only */

#define  CM_BAND_PREF_WCDMA_II_PCS_1900  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_II_PCS_1900 ))
    /**< Acquire WCDMA II PCS systems only */

#define  CM_BAND_PREF_WCDMA_III_1700  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_III_1700 ))
    /**< Acquire WCDMA III 1700 systems only */

#define  CM_BAND_PREF_WCDMA_IV_1700  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_IV_1700 ))
    /**< Acquire WCDMA IV 1700 systems only */

#define  CM_BAND_PREF_WCDMA_V_850  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_V_850 ))
    /**< Acquire WCDMA V 850 systems only */

#define  CM_BAND_PREF_WCDMA_VI_800  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_VI_800 ))
    /**< Acquire WCDMA VI 800 systems only */

/** Clients need to check for CM_API_WCDMA900 for the following three band
** classes.
** - CM_BAND_PREF_WCDMA_VII_2600
** - CM_BAND_PREF_WCDMA_VIII_900
** - CM_BAND_PREF_WCDMA_IX_1700
*/
#define CM_BAND_PREF_WCDMA_VII_2600      ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_VII_2600 ))
    /**< Acquire WCDMA EUROPE 2600 systems only */

#define CM_BAND_PREF_WCDMA_VIII_900      ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_VIII_900 ))
    /**< Acquire WCDMA EUROPE & CHINA 900 systems only */

#define CM_BAND_PREF_WCDMA_IX_1700      ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WCDMA_IX_1700 ))
    /**< Acquire WCDMA JAPAN 1700 systems only */

  /**< Clients check CM_API_WLAN for WLAN support for WLAN bands.*/
#define  CM_BAND_PREF_WLAN_2400_US  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_US_2400))
    /**< Acqurie WLAN in US 2.4 GHz */

#define  CM_BAND_PREF_WLAN_2400_JP  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_JAPAN_2400))
    /**< Acqurie WLAN in JP 2.4 GHz */

#define  CM_BAND_PREF_WLAN_2400_ETSI ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_EUROPE_2400))
    /**< Acqurie WLAN in Europe 2.4 GHz */

#define CM_BAND_PREF_WLAN_2400_SP    ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_SPAIN_2400))
    /**< Acqurie WLAN in SPAIN 2.4 GHz */

#define CM_BAND_PREF_WLAN_2400_FR    ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_FRANCE_2400))
    /**< Acqurie WLAN in France 2.4 GHz */

#define  CM_BAND_PREF_WLAN_5000_US  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_US_5000))
    /**< Acqurie WLAN in US 5 GHz */

#define  CM_BAND_PREF_WLAN_5000_JP  ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_JAPAN_5000))
    /**< Acqurie WLAN in JP 5 GHz */

#define  CM_BAND_PREF_WLAN_5000_ETSI ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_EUROPE_5000))
    /**< Acqurie WLAN in Europe 5 GHz */

#define CM_BAND_PREF_WLAN_5000_SP    ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_SPAIN_5000))
    /**< Acqurie WLAN in SPAIN 5 GHz */

#define CM_BAND_PREF_WLAN_5000_FR    ((cm_band_pref_e_type)SYS_BM_64BIT( SYS_SBAND_WLAN_FRANCE_5000))
    /**< Acqurie WLAN in France 5 GHz */

/** When CM_API_BAND_PREF_NO_CHANGE_ANY_MAX_UPDATE  is defined, revise the
** CM_BAND_PREF_NO_CHANGE back to the value when band pref is still 32 bit
** for backward compatible purpose. The bit 30 is reserved for NO_CHANGE.
*/
#define  CM_BAND_PREF_NO_CHANGE   (cm_band_pref_e_type)(0x40000000)
    /**< Do not change the band preference */

/** When CM_API_BAND_PREF_NO_CHANGE_ANY_MAX_UPDATE is defined, updated the
** CM_BAND_PREF_ANY to align with CM_BAND_PREF_NO_CHANGE.
*/
#define  CM_BAND_PREF_ANY   (cm_band_pref_e_type)0xFFFFFFFFBFFFFFFFULL
    /**< Acquire any band class systems
    ** ULL designates it as "unsigned long long" to get rid of compiler warnings
    ** "literal treated as unsigned long long"
    */

/** When CM_API_BAND_PREF_NO_CHANGE_ANY_MAX_UPDATE is defined, updated the
** CM_BAND_PREF_MAX to align with CM_BAND_PREF_NO_CHANGE.
*/
#define  CM_BAND_PREF_MAX       (cm_band_pref_e_type)(CM_BAND_PREF_ANY + 1)
    /**< @internal */

  //#ifdef FEATURE_RPC
  //, CM_BAND_PREF_BIG=0x10000000
    /* To force enum to 32 bit for windows NT */
  //#endif /* FEATURE_RPC */

#define  CM_BAND_PREF_PERSISTENT        ((cm_band_pref_e_type)SYS_BM_64BIT(SYS_SBAND_PERSISTENT))
    /**< Return to the value given by NV
    ** Clients need to check for CM_API_PERSISTENT_PREF
    ** before using this
    */



/* Cellular and PCS band preference to band-class preference mapping.
**
** Note that IS-683-A PRL only provide support for one cellular band and one
** PCS band. Therefore we define a Cellular and PCS band preferences and map
** them to different band-classes preferences depending on the market, as
** follows:
**
** Cellular band preference is mapped to band-class 3 preference for the
** Japanese market or band-class 0 preference for any other market.
**
** PCS band preference is mapped to band-class 4 preference for the Korean
** market, band-class 5 preference for Band Class 5 market or band-class 1
** preference for any other market.
*/

  #define CM_BAND_PREF_CELL_A  CM_BAND_PREF_BC0_A  /* US Cell A */
  #define CM_BAND_PREF_CELL_B  CM_BAND_PREF_BC0_B  /* US Cell B */
  #define CM_BAND_PREF_CELL    CM_BAND_PREF_BC0    /* US Cell A/B */

  #define CM_BAND_PREF_PCS   CM_BAND_PREF_BC1  /* US PCS */




/** Enumeration of system selection roam preferences.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_roam_pref_e {

  CM_ROAM_PREF_NONE=0,
    /**< @internal */

  CM_ROAM_PREF_HOME=SYS_BM(SYS_ROAM_STATUS_OFF),
    /**< Acquire only systems for which the roaming indication is off
    ** SD_SS_ROAM_PREF_HOME */

  CM_ROAM_PREF_ROAM_ONLY=SYS_BM(SYS_ROAM_STATUS_ON),
    /**<Acquire a system as long as its roaming indication is not off*/

  CM_ROAM_PREF_AFFIL=SYS_BM(SYS_ROAM_STATUS_OFF)|SYS_BM(SYS_ROAM_STATUS_ON),
    /**< Acquire only systems for which the roaming indication is off or
    ** solid on - i.e. not flashing. CDMA Only
    ** SD_SS_ROAM_PREF_AFFIL */

  CM_ROAM_PREF_ANY=0xFF,
    /**< Acquire systems regardless of their roaming indication
    ** SD_SS_ROAM_PREF_ANY */

  CM_ROAM_PREF_NO_CHANGE,
    /**< Do not change the roam preference */

  CM_ROAM_PREF_PERSISTENT,
    /**< Return to roam preference as given by NV
    **
    ** Clients need to check for CM_API_PERSISTENT_PREF
    ** before using this
    */

  CM_ROAM_PREF_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_roam_pref_e_type;


/** Enumeration of prl preferences.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_prl_pref_e {

  CM_PRL_PREF_NONE=0,
    /**< @internal */

  CM_PRL_PREF_AVAIL_BC0_A=SYS_BM(SYS_PRL_AVAIL_BC0_A),
    /**< Acquire available system only on the B side.
    */

  CM_PRL_PREF_AVAIL_BC0_B=SYS_BM(SYS_PRL_AVAIL_BC0_B),
    /**< Acquire available system only on the B side.
    */

  CM_PRL_PREF_ANY=0x3FFF,
    /**< Acquire any available systems.
    */

  CM_PRL_PREF_NO_CHANGE,
    /**< Do not change the prl preference */

  CM_PRL_PREF_PERSISTENT,
    /**< Return to persistent preference setting */

  CM_PRL_PREF_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_prl_pref_e_type;



/** Enumeration of system selection WLAN Technology  preferences.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_wlan_tech_pref_e {

  CM_WLAN_TECH_PREF_NONE=0,
    /**< @internal */

  CM_WLAN_TECH_PREF_A       =          SYS_BM(SYS_TECH_WLAN_80211A),
    /**< Acquire 802.11a only systems */

  CM_WLAN_TECH_PREF_B       =          SYS_BM(SYS_TECH_WLAN_80211B),
    /**< Acquire 802.11b only systems */

  CM_WLAN_TECH_PREF_G_ONLY  =          SYS_BM(SYS_TECH_WLAN_80211G),
    /**< Acquire 802.11g only systems */

  CM_WLAN_TECH_PREF_G       =          SYS_BM(SYS_TECH_WLAN_80211B) | \
                                       SYS_BM(SYS_TECH_WLAN_80211G),
    /**< Acquire Any 802.11g systems */

  CM_WLAN_TECH_PREF_ANY     =          0x3FFFFFFF,
    /**< Acquire Any 802.11 systems */

  CM_WLAN_TECH_PREF_NO_CHANGE,
    /**< Do not change the WLAN Technology preference */

  CM_WLAN_TECH_PREF_MAX
    /**< @internal */

} cm_wlan_tech_pref_e_type;

/** Enumeration of system selection WLAN Technology  preferences.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_wlan_bss_type_pref_e {

  CM_WLAN_BSS_TYPE_PREF_NONE=0,
    /**< @internal */

  CM_WLAN_BSS_TYPE_PREF_ADHOC,
    /**< Acquire Adhoc systems only */

  CM_WLAN_BSS_TYPE_PREF_INFRA,
    /**< Acquire Infra systems only */

  CM_WLAN_BSS_TYPE_PREF_ANY,
    /**< Acquire Any systems */

  CM_WLAN_BSS_TYPE_PREF_NO_CHANGE,
    /**< Do not change the WLAN BSS type preference */

  CM_WLAN_BSS_TYPE_PREF_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_wlan_bss_type_pref_e_type;

/** FEATURE_HDR - Enumeration of system selection hybrid
 *  preferences.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_hybr_pref_e {

  CM_HYBR_PREF_NONE        = -1,
    /**< @internal */

  CM_HYBR_PREF_OFF=0,
    /**< hybrid operation is not allowed - SD_SS_HYBR_PREF_NONE */

  CM_HYBR_PREF_ON=1,
    /**< hybrid operation is allowed - SD_SS_HYBR_PREF_CDMA_HDR */

  CM_HYBR_PREF_CDMA__HDR = CM_HYBR_PREF_ON,
    /**< CDMA + HDR hybrid operation */

  CM_HYBR_PREF_NO_CHANGE=2,
    /**< Do not change the hybr preference */

  CM_HYBR_PREF_PERSISTENT=3,
    /**< Return to value given in NV
    **
    ** Clients need to check for CM_API_PERSISTENT_PREF
    ** before using this
    **
    ** Bumping  pref_presistent to 3 to preserve enum value
    ** from 02.00.47.00 branch. Diag clients depend on
    ** CM_HYBR_PREF_PERSISTENT to hold same value across
    ** targets.
    */

  CM_HYBR_PREF_CDMA__HDR_WCDMA,
    /**< CDMA and (HDR or WCDMA) whichever is available */

  CM_HYBR_PREF_CDMA__WCDMA,
    /**< CDMA and WCDMA hybrid operation */

  CM_HYBR_PREF_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_hybr_pref_e_type;


/** FEATURE_DDTM_CNTL - Enumeration of DDTM preferences.
*/
typedef enum cm_ddtm_pref_e {

  CM_DDTM_PREF_NONE        = -1,
    /**< @internal */

  CM_DDTM_PREF_OFF,
    /**< disable data dedicated transmission mode */

  CM_DDTM_PREF_ON,
    /**< enable data dedicated transmission mode */

  CM_DDTM_PREF_NO_CHANGE,
    /**< Do not change the DDTM preference */

  CM_DDTM_PREF_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_ddtm_pref_e_type;


/** Enum of action types to Add, replace, delete etc the user specified SOs
** in the service option list maintained by CM for DDTM.
*/
typedef enum cm_ddtm_pref_act_e {

  CM_DDTM_SO_LIST_ACT_NONE        = -1,
    /**< @internal */

  CM_DDTM_SO_LIST_ACT_ADD,
    /**< Add the specified SOs to the current DDTM SO list
    ** maintained by CM.
    */

  CM_DDTM_SO_LIST_ACT_REPLACE,
     /**< Replace the current DDTM SO list maintained by CM with one
     ** specified.
     */

  CM_DDTM_SO_LIST_ACT_DELETE,
     /**< Delete the user specified SOs from the DDTM SO list maintained
     ** by CM.
     */

  CM_DDTM_SO_LIST_ACT_NO_CHANGE,
     /**< No change in the DDTM SO list.
     */

  CM_DDTM_SO_LIST_ACT_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */
} cm_ddtm_so_list_act_e_type;



/** Enumeration to indicate the type of system to be avoided
*/
typedef enum {

  CM_SS_AVOID_SYS_NONE        = -1,
    /**< @internal */

  CM_SS_AVOID_SYS_SRV_IF_UZ,
    /**< Avoid idle system only if MS has currently
    ** selected user zone.
    */

  CM_SS_AVOID_SYS_SRV,
    /**< Avoid Idle system.
    */

  CM_SS_AVOID_SYS_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_ss_avoid_sys_e_type;


/** Enumeration of answer voice as data settings. CDMA Only.
*/
typedef enum cm_answer_voice_e {

  CM_ANSWER_VOICE_NONE=-1,
    /**< @internal */

  CM_ANSWER_VOICE_AS_VOICE,
    /**< = DB_VOICE_AS_DATA_NEVER, Answer voice as voice */

  CM_ANSWER_VOICE_AS_FAX_ONCE,
    /**< = DB_VOICE_AS_FAX_ONCE, Answer voice as fax once (for the next
    ** 10 minutes) */

  CM_ANSWER_VOICE_AS_FAX_ALWAYS,
     /**< = DB_VOICE_AS_FAX_ALWAYS, Answer voice as fax always */

  CM_ANSWER_VOICE_AS_MODEM_ONCE,
    /**< = DB_VOICE_AS_MODEM_ONCE, Answer voice as modem once (for the
    ** next 10 minutes) */

  CM_ANSWER_VOICE_AS_MODEM_ALWAYS,
    /**< = DB_VOICE_AS_MODEM_ALWAYS, Answer voice as modem always */

  CM_ANSWER_VOICE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_answer_voice_e_type;


/** Enumeration of NAM selections
*/
typedef enum cm_nam_e {

  CM_NAM_NONE=-1,
    /**< @internal */

  CM_NAM_1,
    /**< NAM 1*/

  CM_NAM_2,
    /**< NAM 2 - for targets with more than 1 NAM */

  CM_NAM_3,
    /**< NAM 3 - for targets with more than 2 NAM's */

  CM_NAM_4,
    /**< NAM 4 - for targets with more than 3 NAM's */

  CM_NAM_AUTO,
    /**< Auto NAM - for targets with more than 1 NAM */

  CM_NAM_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_nam_e_type;


/** Enumeration of phone CDMA lock modes.
*/
typedef enum cm_cdma_lock_mode_e {

  CM_CDMA_LOCK_MODE_NONE=-1,
    /**< @internal */

  CM_CDMA_LOCK_MODE_OFF,
    /**< phone is NOT CDMA locked */

  CM_CDMA_LOCK_MODE_ON,
    /**< phone is CDMA locked until power-cycle */

  CM_CDMA_LOCK_MODE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_cdma_lock_mode_e_type;


/** Enumeration of phone service states
*/
typedef enum cm_packet_state_e {

  CM_PACKET_STATE_NONE=-1,
    /**< @internal */

  CM_PACKET_STATE_NULL,
    /**< packet state is null */

  CM_PACKET_STATE_CONNECT,
    /**< packet state is connect */

  CM_PACKET_STATE_DORMANT,
    /**< packet state is dormant */

  CM_PACKET_STATE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_packet_state_e_type;


/**
** Enumeration that defines the service domain being requested by
** the client (the preferred service domain).
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_srv_domain_pref_e {

  CM_SRV_DOMAIN_PREF_NONE=-1,
    /**< No service domain is requested. */

  CM_SRV_DOMAIN_PREF_CS_ONLY=0,
    /**< Prefer Circuit Switched Only */

  CM_SRV_DOMAIN_PREF_PS_ONLY=1,
    /**< Prefer Packet Switched Only */

  CM_SRV_DOMAIN_PREF_CS_PS=2,
    /**< Prefer Circuit and Packet Switched */

  CM_SRV_DOMAIN_PREF_ANY=3,
    /**< Any domain will do. No preference */

  CM_SRV_DOMAIN_PREF_NO_CHANGE=4,
    /**< To be used by clients who do not want to change the service domain */

  CM_SRV_DOMAIN_PREF_PS_ATTACH=5,
    /**<  PS attach on demand.
        Note: This value of service domain preference is not saved in NV  */

  CM_SRV_DOMAIN_PREF_PS_DETACH=6,
    /**< PS detach on demand
       Note: This value of service domain preference is not saved in NV   */


  CM_SRV_DOMAIN_PREF_PERSISTENT=7,
    /**< Return to value stored in NV
    **
    ** Clients need to check for CM_API_PERSISTENT_PREF
    ** before using this
    */

  CM_SRV_DOMAIN_PREF_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_srv_domain_pref_e_type;


/**
** Enumeration that defines whether the network should be selected
** automatically, manually or for limited service. GSM/WCDMA or WLAN
** Only.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cm_network_sel_mode_pref_e {

  CM_NETWORK_SEL_MODE_PREF_NONE=-1,
    /**< @internal */

  CM_NETWORK_SEL_MODE_PREF_AUTOMATIC=0,
    /**< Automatic network selection */

  CM_NETWORK_SEL_MODE_PREF_MANUAL=1,
    /**< Manual network selection */

  CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV=2,
    /**< Select a network for limited service
    ** Not applicable for WLAN!!
    */

  CM_NETWORK_SEL_MODE_PREF_NO_CHANGE=3,
    /**< No change */

  CM_NETWORK_SEL_MODE_PREF_PERSISTENT=4,
   /**< Return to persistent preference.
   ** Not applicable for WLAN!!
   */

  CM_NETWORK_SEL_MODE_PREF_HPLMN_SRCH=5,
   /**< Clients need to check for define CM_API_INITIATE_HPLMN_SRCH before
   ** using this enum.
   **
   **
   ** Search and Camp on HPLMN .
   ** When requested using cm_ph_cmd_sys_sel_pref()
   ** pref_duration and plmn_ptr are ignored
   ** term_pref = CM_PREF_TERM_PWR_CYCLE
   ** other parameters should carry NO_CHANGE
   */


  CM_NETWORK_SEL_MODE_PREF_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_network_sel_mode_pref_e_type;


/**
** Enumeration that defines the type of network list to be requested.  The
** requested list will either contain the preferred networks or all available
** networks.
*/
typedef enum cm_network_list_type_e {

  CM_NETWORK_LIST_NONE=-1,
    /**< @internal */

  CM_NETWORK_LIST_AVAILABLE,
    /**< The available network list. */

  CM_NETWORK_LIST_USER_PREFERRED,
    /**< The preferred network list from SIM/USIM
    ** This request returnes the list of PLMNs that
    ** are valid only. List does not contain any
    ** holes even if there are holes in SIM/USIM file.
    */

  CM_NETWORK_LIST_USER_PREFERRED_UNPACKED,
    /**< The preferred network list from SIM/USIM. This would
    ** return the list the way it is stored in the SIM/USIM.
    ** If there are holes between 2 records, it would be there
    ** too.
    */

  CM_NETWORK_LIST_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_network_list_type_e_type;


typedef enum cm_subscription_status_e {

  CM_SUBSCRIPTION_STATUS_NONE=-1,
    /**< @internal */

  CM_SUBSCRIPTION_STATUS_CHANGE,
    /**< Subscription for the specified mode changed */

  CM_SUBSCRIPTION_STATUS_NO_CHANGE,
    /**< Subscription for the specified mode has not changed */

  CM_SUBSCRIPTION_STATUS_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_subscription_status_e_type;


typedef enum cm_rtre_config_e {

  CM_RTRE_CONFIG_NONE,
    /**< @internal */

  CM_RTRE_CONFIG_RUIM_ONLY,
    /**< RTRE configuration: use R-UIM only */

  CM_RTRE_CONFIG_NV_ONLY,
    /**< RTRE configuration: use NV only    */

  CM_RTRE_CONFIG_RUIM_OR_DROP_BACK,
    /**< RTRE use R-UIM if available or NV  */

  CM_RTRE_CONFIG_SIM_ACCESS,
    /**< RTRE use SIM */

  CM_RTRE_CONFIG__MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_rtre_config_e_type;


typedef enum cm_rtre_control_e {

  CM_RTRE_CONTROL_NONE,
    /**< @internal */

  CM_RTRE_CONTROL_RUIM,
    /**< RTRE control: use R-UIM */

  CM_RTRE_CONTROL_NV,
    /**< RTRE control: use NV */

  CM_RTRE_CONTROL_SIM
    /**< RTRE control: use SIM */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_rtre_control_e_type;


typedef enum cm_test_control_type_e {

  CM_TEST_CONTROL_TYPE_NONE=-1,
    /**< @internal */

  CM_TEST_CONTROL_TYPE_NO_TEST,
    /**< Init value when no testing is going on */

  CM_TEST_CONTROL_TYPE_LB_MODE1,
    /**< LOOPBACK MODE1 */

  CM_TEST_CONTROL_TYPE_LB_MODE2,
    /**< LOOPBACK MODE2 */

  CM_TEST_CONTROL_TYPE_LB_MODE1_WITH_DUMMY_SIGNALLING,
    /**< LOOPBACK MODE1 with dummy signalling */

  CM_TEST_CONTROL_TYPE_LB_MODE2_WITH_DUMMY_SIGNALLING,
    /**< LOOPBACK MODE2 with dummy signalling */

  CM_TEST_CONTROL_TYPE_MAX
    /**< @internal */

} cm_test_control_type_e_type;

/** Gives clients advice of charge information.
**
** Clients need to check for CM_API_AOC_INFO before
** using this field.
*/

typedef struct cm_ph_aoc_info_s {

  boolean  aoc_ready;
    /**< Tells if the AOC subsystem ready */

  unsigned long ACM;
    /**< Accumulated Call Meter.
    ** aoc_ready should be TRUE for this to be valid
    */

  unsigned long ACMmax;
    /**< Max allowed Accumulated Call Meter.
    ** aoc_ready should be TRUE for this to be valid
    */

} cm_ph_aoc_info_s_type;

/** Clients check CM_API_WLAN for WLAN support for following
*/

/*
** Enumeration that defines the type of Wakeup Action required.
*/
typedef enum cm_wakeup_e {

  CM_WAKEUP_NONE=-1,
    /**< @internal */

  CM_WAKEUP_MAIN   = SYS_BM(0),
    /**< Wakeup Main service. It could be CDMA, HDR, GSM, WCDMA or GW
    ** depending on the mode_pref and hybr_pref.
    */

  CM_WAKEUP_HDR    = SYS_BM(1),
    /**< Wakeup HDR service. It is valid only if hybr_pref is on */

  CM_WAKEUP_WLAN   = SYS_BM(2),
    /**< Wakeup WLAN Service */

  CM_WAKEUP_ALL    =  CM_WAKEUP_MAIN | \
                      CM_WAKEUP_HDR  | \
                      CM_WAKEUP_WLAN,
    /**< Wakeup All services */

  CM_WAKEUP_MAX
  /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_wakeup_e_type;

/**
** Enumeration that defines the WLAN scan preference
*/
typedef enum cm_wlan_scan_pref_e {

  CM_WLAN_SCAN_PREF_NONE =-1,
    /**< @internal */

  CM_WLAN_SCAN_PREF_ACTIVE   = SYS_WLAN_SCAN_PREF_ACTIVE,
    /**< ACTIVE WLAN scan.
    */

  CM_WLAN_SCAN_PREF_PASSIVE  = SYS_WLAN_SCAN_PREF_PASSIVE,
    /**< Passive WLAN Scan.
    */

  CM_WLAN_SCAN_PREF_AUTO     = SYS_WLAN_SCAN_PREF_AUTO,
    /**< CM to decide Active/Passive scan automatically.
    */

  CM_WLAN_SCAN_PREF_NO_CHANGE,

  CM_WLAN_SCAN_PREF_MAX

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_wlan_scan_pref_e_type;

/**---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** Type to hold net lists phone information.
** When notified of phone events about network lists, clients are given
** a reference to such a structure.
*/
typedef struct cm_ph_net_lists_info_s {

  sys_user_pref_plmn_list_s_type   gw_user_pref_networks;
    /**< The user preferred network list. GSM/WCDMA only */

  sys_detailed_plmn_list_s_type    gw_available_networks;
    /**< Available networks. GSM/WCDMA only */

  sys_wlan_bss_info_list_s_type    wlan_bss_list;
    /**< Available WLAN BSS List */

} cm_ph_net_lists_info_s_type;

typedef struct cm_wlan_pref_s {

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref;
    /**< WLAN selection is automatic or manual */

  cm_wlan_scan_pref_e_type          scan_mode;
  /**< Active/Passive scan mode */

  cm_wlan_tech_pref_e_type          tech_pref;
   /**< Specify 802.11 a/b/g etc. here. */

  cm_wlan_bss_type_pref_e_type      bss_type_pref;
   /**< Specify Adhoc/Infra/Any for Automatic mode. */

  /* Following information is required for MANUAL mode of WLAN
  ** selection.
  */

  sys_wlan_bss_e_type               bss_type;
   /**< BSS TYpe : AD-HOC, ADHOC_START or INFRA for Manual mode */

  sys_wlan_bssid_type               bss_id;
    /**< If WLAN network selection is set to manual, this specifies BSS */
    /**< if BSS Id is unknown, then set bss_id to 0 */

  sys_wlan_ssid_s_type              ssid;
    /**< If WLAN network selection is set to manual, this specifies SSID */
    /**< if SSId is unknown, then set ssid length to 0 */

  sys_chan_type                     chan;

   /**< Channel is specified as a bit mask. Please see definition in net.h
   ** for BIT Pattern of Channel as defined in
   ** net_chan_wlan_2400_e_type  or
   ** net_chan_wlan_5000_e_type
   */

   /**< If WLAN network selection is set to manual, this specifies Channel */
   /**< If WLAN channel is unkown, then set channel to 0*/

  sys_band_mask_type                band;
   /**< If WLAN network selection is set to manual, this specifies WLAN Band */
   /**< If WLAN channel is unkown, then set channel to SYS_BAND_MASK_WLAN_ANY*/

  sys_profile_id_type               profile_id;
   /**< Profile ID to be used with this manual system. */

  net_profile_s_type                profile;
  /**< Profile to be used with this manual system */


}cm_wlan_pref_s_type;



/** Type to hold phone information.
** When notified of phone events, clients are given
** a reference to such a structure.
*/
typedef struct cm_ph_info_s {

  boolean                           is_in_use;
    /**< Indicate whether phone is currently in use */

  sys_oprt_mode_e_type              oprt_mode;
    /**< Indicate current operating mode */

  cm_test_control_type_e_type       test_control_type;
    /**< Indicate current test control type */

  cm_cdma_lock_mode_e_type          cdma_lock_mode;
    /**< Indicate whether phone is CDMA locked until power cycle */

  cm_answer_voice_e_type            answer_voice;
    /**< Indicate whether phone should answer incoming
    ** voice calls as voice, fax or modem. CDMA only */

  cm_mode_pref_e_type               mode_pref;
    /**< Indicate current mode preference */

  cm_pref_term_e_type               pref_term;
    /**< Indicate the current preference term - one-call, power-cycle, etc.*/

  cm_gw_acq_order_pref_e_type       acq_order_pref;
    /**< The acquisition order preferred by the client. GSM/WCDMA only. */

  cm_srv_domain_pref_e_type         srv_domain_pref;
    /**< The service domain preferred by the client.
    ** This field is set after a call to cm_ph_cmd_sys_sel_pref.
    ** GSM/WCDMA only. */

  cm_band_pref_e_type               band_pref;
    /**< Indicate current band preference */

  cm_roam_pref_e_type               roam_pref;
    /**< Indicate current roam preference */

  cm_nam_e_type                     nam_sel;
    /**< Indicate the NAM selection. CDMA only */

  cm_nam_e_type                     curr_nam;
    /**< Indicate the current NAM -
    ** needed when NAM selection is set to AUTO-NAM. CDMA only */

  cm_packet_state_e_type            packet_state;
    /**< Indicate current packet state. CDMA only */

  cm_hybr_pref_e_type               hybr_pref;
    /**< Indicate hybrid preference */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref;
    /**< The network selection mode selected by the client. GSM/WCDMA only */

  sys_plmn_id_s_type                plmn;
    /**< The identifier of the PLMN to be selected manually. GSM/WCDMA only */

  sys_wlan_bss_info_s_type          wlan_bss_info;
    /**< The identifier of the BSS to be selected manually. WLAN only */

  sys_user_pref_plmn_list_s_type    user_pref_networks;
    /**< The user preferred network list. GSM/WCDMA only */

  sys_detailed_plmn_list_s_type     available_networks;
    /**< Available networks. GSM/WCDMA only */

  sys_wlan_bss_info_list_s_type     wlan_bss_list;
    /**< Available WLAN BSS List */
    /**< Clients check CM_API_WLAN for WLAN support*/


  boolean                           is_cdma_subscription_available;
    /**< Indicate whether the cdma subscription/provisioning information
    ** is available for consumption.                              */

  boolean                           is_gw_subscription_available;
    /**< Indicate whether the GSM/WCDMA subscription/provisioning information
    ** is available for consumption.                              */

  boolean                           is_wlan_subscription_available;
    /**< Indicate whether the WLAN subscription/provisioning information
    ** is available for consumption.                              */
    /**< Clients check CM_API_WLAN for WLAN support*/

  cm_rtre_config_e_type             rtre_config;
    /**< Current RTRE configuration - RUIM, NV, RUIM fallback NV. CDMA only */

  cm_rtre_control_e_type            rtre_control;
    /**< Current RTRE control - RUIM or NV. CDMA only */

  cm_srv_domain_pref_e_type         orig_srv_domain_pref;
    /**< service domain preference for PS attach/detach commands.
       This is different from the phone service domain preference
    */

  cm_ddtm_pref_e_type               ddtm_pref;
    /**< Data Dedicated Tranmission mode (DDTM) preference.
    */

  boolean                           cur_ddtm_status;
    /**< ddtm status */

  sys_ddtm_act_mask_e_type          ddtm_act_mask;
    /**< DDTM action mask settings.
    */

  sys_ddtm_act_mask_e_type          orig_ddtm_act_mask;
    /**< DDTM action mask settings.
    */

  uint16                            ddtm_num_srv_opt;
    /**< Number of service options specified in below list.
    */

  sys_srv_opt_type             ddtm_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
    /**< List of service options whose pages have to be ignored provided
    ** ddtm_pref & ddtm_act_mask allows it.
    */

  cm_cphs_proc_e_type               cphs_allowed;
    /**<
    ** Whether Common PCN Handset Specifications procedures are allowed
    ** FEATURE_ALS
    */

  cm_als_proc_e_type                als_allowed;
    /**<
    ** Whether ALS procedures are allowed.
    ** FEATURE_ALS
    */

  cm_als_line_e_type                line;
    /**<
    ** Currently selected line (all MO voice calls will use it)
    ** FEATURE_ALS
    */

  cm_als_line_switching_e_type      line_switching;
    /**<
    ** Whether the user is allowed to switch between lines
    ** FEATURE_ALS
    */

  cm_client_id_type                 requesting_client_id;
    /**< Requesting client */


  #if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
       defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM) || \
       defined(FEATURE_MM_SUPERSET)) 
  sys_is2000_msg_s_type             cdma_capability;
    /**< CDMA capability in the form of IS2000 Status Message.
    */

  sys_msm_id_e_type                 msm_id;
    /**< Identifies the MSM which generated this event.
    */
  #endif

  boolean                           is_net_lists_present;
    /**< This flag indicates if network lists info is available
    ** for clients.
    */

  cm_wlan_pref_s_type               wlan_pref;
    /**< WLAN preferences
    */

  cm_ccbs_store_info_s_type         ccbs_store_info;
    /**< Clients need to check for CM_API_CCBS before
    ** using this event.
    ** Holds ccbs information for clients
    */

  cm_ph_state_e_type                ph_state;
    /**< Indicates the current operating mode of the phone
    ** Used to indicate that the phone is in emergency call back mode
    */

  cm_ph_aoc_info_s_type             aoc_info;
    /**< Gives clients advice of charge information.
    **
    ** Clients need to check for CM_API_AOC_INFO before
    ** using this field.
    */

  sys_plmn_list_status_e_type       available_networks_list_cnf;
    /**< Clients need to check for CM_API_AVAIL_NET_LIST_CNF
    ** before using this field
    **
    ** Gives the status of plmn list request (success/abort).
    */

  sys_wlan_wps_result_s_type        wlan_wps_result;
    /** WPS procedure result.
    **
    ** Clients need to check for CM_API_WLAN_WPS before using this field.
    */

  sys_sys_mode_mask_e_type          mode_capability;
    /**< Indicate mode capability */

  sys_band_mask_e_type              band_capability;
    /**< Indicate band capability */

  cm_prl_pref_e_type                prl_pref;
    /**< Indicate current prl preferred setting */

  sys_meas_mode_e_type              meas_mode;
    /**< Indicate measurement mode - enable/disable HDR access*/

} cm_ph_info_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_ph_info_s.ddtm_srv_opt_list VARRAY SYS_DDTM_MAX_SO_LIST_SIZE
    LENGTH cm_ph_info_s.ddtm_num_srv_opt */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/** Type for phone command callback functions.
**
** A client that wants to be notified of the result of a
** phone-command-function needs to supply a pointer to a function
** of this type as the first parameter to a phone-command-function.
*/
typedef void (cm_ph_cmd_cb_f_type) (

  void                    *data_block_ptr,
    /**< Pointer to client provided data block */

  cm_ph_cmd_e_type        ph_cmd,
    /**< Indicate which phone command is this error status for */

  cm_ph_cmd_err_e_type    ph_cmd_err
    /**< Phone command error code, indicating whether command
    ** is rejected and the reason */

);



/**===========================================================================

@FUNCTION cm_ph_cmd_oprt_mode

@DESCRIPTION
  Command CM to put phone in a specified operating mode, such as offline,
  online, LPM, etc.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_oprt_mode(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< Requesting client */

  sys_oprt_mode_e_type   oprt_mode
    /**< Switch to this operating mode */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_oprt_mode */
#endif


/**===========================================================================

@FUNCTION cm_ph_cmd_meas_mode

@DESCRIPTION
  Command CM to put phone in or out of measurement mode. Currently valid 
  only for HDR .Measurement mode disables all HDR call processing and puts
  the modem into a special mode that allows monitoring HDR air interface
  without creating a HDR session or using the existing HDR session 

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event - CM_PH_EVENT_MEAS_MODE.

@x
===========================================================================*/
extern boolean cm_ph_cmd_meas_mode(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< Requesting client */

  sys_meas_mode_e_type   meas_mode
    /**< Switch to this measurement mode */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_meas_mode */
#endif



#if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
     defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM) || \
     defined(FEATURE_MM_SUPERSET))

/**===========================================================================

@FUNCTION cm_ph_cmd_oprt_mode2

@DESCRIPTION
  Command CM to put phone in a specified operating mode, such as offline,
  online, LPM, etc with msm_id information.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_oprt_mode2(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< Requesting client */

  sys_oprt_mode_e_type   oprt_mode,
    /**< Switch to this operating mode */

  sys_msm_id_e_type      msm_id
    /**< MSM to which this command has to be sent.
    */
);
#endif



/**===========================================================================

@FUNCTION cm_ph_cmd_sys_sel_pref

@DESCRIPTION
  Command CM to change the phone's system selection preference (i.e., the
  mode, band and roam preference).

  The preferred modes that can be used for this function are defined in "cm_mode_pref_e_type".
  The preference terms that can be used in this function are defined in "cm_pref_term_e_type".
  The values for the other preferences are defined in "cm_hybr_pref_e_type" and "cm_srv_domain_pref_e_type".
  If a particular system selection preference is not to be changed, then the
  appropriate NO_CHANGE enumerated value should be passed to this function

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_sys_sel_pref(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /**< Client callback function */

  void                              *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /**< Requesting client */

  cm_mode_pref_e_type               mode_pref,
    /**< Switch to this mode preference */

  cm_pref_term_e_type               pref_term,
    /**< Switch the preference for this term */

  dword                             pref_duration,
    /**< Specify the duration in seconds for temporary term */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /**< gw acquisition order preference */

  cm_band_pref_e_type               band_pref,
    /**< Switch to this band preference */

  cm_prl_pref_e_type                prl_pref,
    /**< Switch to this prl preference */

  cm_roam_pref_e_type               roam_pref,
    /**< Switch to this roam preference */

  cm_hybr_pref_e_type               hybr_pref,
    /**< Switch to this hybrid preference */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /**< Switch to this service domain (e.g. Circuit Switch)
    ** Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
    ** domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /**< Defines whether the network should be selected automatically
    ** or manually */

  const sys_plmn_id_s_type                *plmn_ptr,
    /**< If network selection is set to manual, this specifies plmn id */

  const cm_wlan_pref_s_type               *wlan_pref_ptr
    /**< WLAN Prefereces.
    ** Clients check CM_API_WLAN for WLAN support */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_sys_sel_pref */
/*~ PARAM plmn_ptr POINTER */
/*~ PARAM wlan_pref_ptr POINTER */
#endif


/**===========================================================================

@FUNCTION cm_ph_cmd_ddtm_pref

@DESCRIPTION
  Command CM to change the phone's data dedicated transmission mode (DDTM)
  preference to a specified selection, such as ON or OFF etc. On successful execution of this command,
  an event is returned to the client indicating current DDTM preference. Posssible values for the DDTM
  preference parameter are defined in "cm_ddtm_pref_e_type"

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_ph_cmd_ddtm_pref(

  cm_ph_cmd_cb_f_type        cmd_cb_func,
    /**< client callback function.
    */

  void                       *data_block_ptr,
    /**< pointer to client callback data block.
    */

  cm_client_id_type          client_id,
    /**< Requesting client.
    */

  cm_ddtm_pref_e_type        ddtm_pref,
    /**< Switch to this DDTM preference.
    */

  sys_ddtm_act_mask_e_type   ddtm_act,
    /**< When DDTM is enabled, this field specifies what combined
    ** DDTM actions should takes place to enhance HDR data thru'put.
    ** This is bit-mapped enum, with each bit specifying action.
    */

  cm_ddtm_so_list_act_e_type so_list_act,
    /**< Specifies what to do ( add, replace, delete ) with the user
    ** specified SO list in the following parameters.
    */

  uint16                     num_srv_opt,
    /**< Number of SOs specified in list,
    ** MAX = SYS_DDTM_MAX_SO_LIST_SIZE
    */

  const sys_srv_opt_type           srv_opt_list[]
    /** List of SOs for which 1x pages have to ignored while in
    ** HDR connected state. if srv_opt_list[] contains CAI_SO_NULL
    ** & ddtm_pref is ON, ignore all pages.
    */


);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_ddtm_pref */
/*~ PARAM srv_opt_list VARRAY LENGTH num_srv_opt */
#endif


/**===========================================================================

@FUNCTION cm_ph_cmd_avoid_sys

@DESCRIPTION
  Command CM to avoid the system defined in "avoid_type".
  "avoid_time" is no longer used.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  CDMA Only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_avoid_sys(

  cm_ph_cmd_cb_f_type     cmd_cb_func,
    /**< client callback function */

  void                    *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type       client_id,
    /**< Requesting client */

  cm_ss_avoid_sys_e_type  avoid_type,
    /**< type of the system to avoid */

  dword                   avoid_time
    /**< OBSOLETE - avoid time in seconds */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_avoid_sys */
#endif


/**===========================================================================

@FUNCTION cm_ph_cmd_answer_voice

@DESCRIPTION
  Command CM to change the phone's answer-voice-as setting to a specified
  selection as defined in "cm_answer_voice_e_type", such as answer-voice-as-data, answer-voice-as-voice, etc.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  CDMA Only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_CDMA_800 || FEATURE_CDMA_1900 must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_answer_voice(

  cm_ph_cmd_cb_f_type        cmd_cb_func,
    /**< client callback function */

  void                       *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type          client_id,
    /**< Requesting client */

  cm_answer_voice_e_type     answer_voice,
    /**< Indicate whether phone should answer incoming
    ** voice calls as voice, fax or modem */

  dword                      answer_duration
    /**< Indicate seconds duration for temp. setting, such as
    ** CM_ANSWER_VOICE_AS_FAX_ONCE or DB_VOICE_AS_MODEM_ONCE */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_answer_voice */
#endif


/**===========================================================================

@FUNCTION cm_ph_cmd_nam_sel

@DESCRIPTION
  Command CM to change the phone's NAM selection to NAM-1, NAM-2, etc.

  NOTE! NV_MAX_NAMS defines the maximum number of NAMs supported in the phone
  This command fails when trying to change the NAM to illegal selection. E.g. changing
  to NAM-2 for a 1 NAM target.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  CDMA Only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_CDMA_800 || FEATURE_CDMA_1900 must be defined.

@RETURNS
  none

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_nam_sel(

  cm_ph_cmd_cb_f_type        cmd_cb_func,
    /**< client callback function */

  void                       *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type          client_id,
    /**< Requesting client */

  cm_nam_e_type              nam_sel
    /**< Change NAM selection to this NAM */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_nam_sel */
#endif


/**===========================================================================

@FUNCTION cm_ph_cmd_get_ph_info

@DESCRIPTION
  Get a sanpshot of the current phone information in the CM. This function submits a request for
  a snapshot of phone object. The client needs to register for CM_PH_EVENT_INFO in order to receive the response
  to this command. CM_PH_EVENT_INFO is sent only to the client that issues this command.

 Special considerations
 The client can call this function only after it has received CM_PH_EVENT_INFO_AVAIL notification from CM


@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_ph_cmd_get_ph_info(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id
    /**< requesting client ID */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_get_ph_info */
#endif




/**===========================================================================

@FUNCTION cm_ph_cmd_rssi_delta_threshold

@DESCRIPTION
  This function commands CM to set the RSSI delta threshold to the
  specified value. The default delta threshold is 5 dBm. When RSSI change
  exceeds the delta threshold, CM sends CM_SS_EVENT_RSSI to the registered client.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_ph_cmd_rssi_delta_threshold(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< requesting client ID */

  uint8                  delta
    /**< RSSI change delta threshold for RSSI reporting */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_rssi_delta_threshold */
#endif

/*===========================================================================

@FUNCTION cm_ph_cmd_signal_strength_delta

@DESCRIPTION
  This function commands CM to set the RSSI/ECIO/IO delta threshold to the
  specified value.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_ph_cmd_signal_strength_delta(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client,
    /**< requesting client ID */

  uint8                  rssi_delta,
    /**< RSSI change delta threshold for Signal Strength reporting */

  uint8                  ecio_delta,
    /**< ECIO change delta threshold for Signal Strength  reporting */

  uint8                  io_delta
    /**< IO change delta threshold for Signal Strength  reporting */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_signal_strength_delta */
#endif


/*===========================================================================

@FUNCTION cm_ph_cmd_signal_strength_delta2

@DESCRIPTION
  This function commands CM to set the RSSI/ECIO/IO/SIR/PATHLOSS
  delta threshold to the specified value.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_ph_cmd_signal_strength_delta2(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client,
    /**< requesting client ID */

  uint8                  rssi_delta,
    /**< RSSI change delta threshold for Signal Strength reporting */

  uint8                  ecio_delta,
    /**< ECIO change delta threshold for Signal Strength  reporting */

  uint8                  io_delta,
    /**< IO change delta threshold for Signal Strength  reporting */

  uint8                  sir_delta,
    /**< SIR change delta threshold for Signal Strength  reporting */

  uint8                  pathloss_delta
    /**< PATHLOSS change delta threshold for Signal Strength  reporting */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_signal_strength_delta2 */
#endif
/**===========================================================================

@FUNCTION cm_ph_cmd_subscription_available

@DESCRIPTION
  This function informs CM that the subscription/provisioning information
  has been verified and available for consumption. The result of this command may
  be the start of full service acquisition.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If the subscription is currently available, the request will be
  ignored and a CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned.

  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_subscription_available(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gw_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   wlan_status
    /**< Subscription status of WLAN system.
    ** Clients check CM_API_WLAN for WLAN support */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_subscription_available */
#endif

#if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
     defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM) || \
     defined(FEATURE_MM_SUPERSET))

/**===========================================================================

@FUNCTION cm_ph_cmd_subscription_available2

@DESCRIPTION
  This function informs CM that the subscription/provisioning information
  has been verified and available for consumption. With MSM_ID

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If the subscription is currently available, the request will be
  ignored and a CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned.

  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_subscription_available2(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gw_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   wlan_status,
    /**< Subscription status of WLAN system.
    ** Clients check CM_API_WLAN for WLAN support */

  sys_msm_id_e_type               msm_id
    /**< MSM Id to which this command has to be sent.
    */

);
#endif



/**===========================================================================

@FUNCTION cm_ph_cmd_subscription_not_available

@DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is not available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT

  If the subscription is currently not available, the request will be
  ignored and a CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned.

  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_subscription_not_available(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gw_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   wlan_status
    /**< Subscription status of WLAN system.
    ** Clients check CM_API_WLAN for WLAN support */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_subscription_not_available */
#endif



/**===========================================================================

@FUNCTION cm_ph_cmd_subscription_changed

@DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is changed.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT

  If the subscription is currently not available, the request will be
  ignored and a CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned.

  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_subscription_changed(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gw_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   wlan_status
    /**< Subscription status of WLAN system.
    ** Clients check CM_API_WLAN for WLAN support */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_subscription_changed */
#endif

/**===========================================================================

@FUNCTION cm_ph_cmd_get_networks

@DESCRIPTION
  Command CM to retrieve either a list of preferred networks or a list of
  all available networks.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

  GSM/WCDMA Only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

@SIDE_EFFECT
  If command is successful, CM clients will receive the list of networks
  via the CM_PH_EVENT_AVAILABLE_NETWORKS_CONF or CM_PH_EVENT_PREFERRED_-
  NETWORKS_CONF event.
@x
===========================================================================*/
extern boolean cm_ph_cmd_get_networks(

  cm_ph_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type            client_id,
    /**< Requesting client */

  cm_network_list_type_e_type  list_type,
    /**< Indicates whether to retrieve the preferred or available networks */

  cm_mode_pref_e_type          network_type

    /**< Clients check CM_API_WLAN for WLAN support */

    /**< Indicates the desired system such as WLAN or GW.
    ** Only WLAN or GW is supported, one at a time can be
    ** requested only.
    */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_get_networks */
#endif


/**===========================================================================

@FUNCTION cm_ph_cmd_terminate_get_networks

@DESCRIPTION
  Command CM to terminate retrieval operation of list of all available
  networks.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

  GSM/WCDMA Only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

@SIDE_EFFECT
  If command is successful, the previous command to retrieve the list will be
  terminated.

@x
===========================================================================*/
extern boolean cm_ph_cmd_terminate_get_networks(

  cm_ph_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type            client_id
    /**< Requesting client */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_terminate_get_networks */
#endif


/*===========================================================================

@FUNCTION cm_ph_cmd_wps_start

@DESCRIPTION
  Command CM to get WLAN security credentials using WPS.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

  WLAN only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  - TRUE  - command was successfully sent to CM
  - FALSE - command was not successfully sent to CM

@SIDE_EFFECT
  If command is successful, CM clients will receive a list of WLAN
  credentials via the CM_PH_EVENT_WPS_COMPLETED event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_wps_start(

        cm_ph_cmd_cb_f_type              cmd_cb_func,
          /**< Client callback function.
          */

        void                            *data_block_ptr,
          /**< Pointer to client callback data block.
          */

        cm_client_id_type                client_id,
          /**< Requesting client.
          */

        sys_wlan_wps_start_param_s_type  const *wps_start_param
          /**< Info required to start WPS procedure.
          */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_wps_start */
/*~ PARAM wps_start_param POINTER */
#endif



/*===========================================================================

@FUNCTION cm_ph_cmd_wps_abort

@DESCRIPTION
  Command CM to terminate WPS procedure.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

  WLAN Only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

@SIDE_EFFECT
  If command is successful, the previous command to retrieve the WLAN
  credentials using WPS will be aborted.

@x
===========================================================================*/
extern boolean cm_ph_cmd_wps_abort(

  cm_ph_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type            client_id
    /**< Requesting client */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_wps_abort */
#endif



/*===========================================================================

@FUNCTION cm_ph_cmd_set_preferred_networks

@DESCRIPTION
  Command CM to save the preferred networks to the SIM/USIM. The preferred networks structure is
  defined is as follows:

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

  GSM/WCDMA Only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern boolean cm_ph_cmd_set_preferred_networks(

  cm_ph_cmd_cb_f_type              cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                client_id,
    /**< Requesting client */

  const sys_user_pref_plmn_list_s_type  *preferred_networks_ptr
    /**< The preferred networks to be saved */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_set_preferred_networks */
/*~ PARAM preferred_networks_ptr POINTER */
#endif


/**===========================================================================

@FUNCTION cm_ph_cmd_packet_state

@DESCRIPTION
  Command CM to change Rm packet state in packet mode.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  CDMA Only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_ph_cmd_packet_state(

  cm_ph_cmd_cb_f_type         cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Requesting client */

  cm_packet_state_e_type      packet_state
    /**< Change Rm packet state to this state */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_packet_state */
#endif


/**===========================================================================

@FUNCTION cm_ph_cmd_voc_activity

@DESCRIPTION
  Command CM to activate voice activity test for current or next voice call.
  On call termination, the voice activity test will be disabled.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  CDMA Only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_JCDMA must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_voc_activity(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< Requesting client */

  boolean                enable,
    /**< enable/disable voice activity test */

  uint16                 voc_1,
    /**< # of full rate */

  uint16                 voc_2,
    /**< # of 1/2 rate */

  uint16                 voc_8
    /**< # of 1/8 rate */

);



/**===========================================================================

@FUNCTION cm_ph_cmd_wakeup_from_standby

@DESCRIPTION
  Command CM to inform lower layers to wake up from deep sleep (standby mode)

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_wakeup_from_standby(

  cm_ph_cmd_cb_f_type        cmd_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type          client_id
    /**< Requesting client */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_wakeup_from_standby */
#endif

/**===========================================================================

@FUNCTION cm_ph_cmd_wakeup_from_standby2

@DESCRIPTION

  Clients check CM_API_WLAN for WLAN support

  Command CM to inform lower layers to wake up from deep sleep (standby mode)

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_wakeup_from_standby2(

  cm_ph_cmd_cb_f_type        cmd_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type          client_id,
    /**< Requesting client */

  cm_wakeup_e_type           service
    /**< Service to be waken up */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_wakeup_from_standby2 */
#endif



/**===========================================================================

@FUNCTION cm_ph_cmd_change_rtre_config

@DESCRIPTION
  Command CM to change RTRE configuration. On successful execution of this command, an event
  is returned to the client indicating the current RTRE configuration as well as RTRE control. Possible values for the RTRE
  config are listed in  "cm_rtre_config_e_type". Possible values for the RTRE control are defined in "cm_rtre_control_e_type"

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  CDMA Only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_UIM_RUN_TIME_ENABLE must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_change_rtre_config(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< Requesting client */

  cm_rtre_config_e_type  rtre_config
    /**< new RTRE configuration */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_change_rtre_config */
#endif


/**===========================================================================

@FUNCTION cm_ph_cmd_aoc_reset_acm

@DESCRIPTION
  This command will reset the ACM to 0 on a system supporting ACM/ACMAX.

Special considerations
You must ensure that PIN2 (CHV2) has been unlocked on the UIM before issuing this command or it will fail


@DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, The ACM value will be set to 0.

@x
===========================================================================*/
extern boolean cm_ph_cmd_aoc_reset_acm(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id
    /**< Requesting client */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_aoc_reset_acm */
#endif

/**===========================================================================

@FUNCTION cm_ph_cmd_aoc_set_acmmax

@DESCRIPTION
  This command will set the ACMMax value in the SIM to the requested value.

Special considerations
You must ensure that PIN2 (CHV2) has been unlocked on the UIM before issuing this command or it will fail


@DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA must be defined.


@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  None.

@x
===========================================================================*/
extern boolean cm_ph_cmd_aoc_set_acmmax(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< Requesting client */

  uint32                 new_max
    /**< new max value for ACMMAX. */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_aoc_set_acmmax */
#endif

/**===========================================================================

@FUNCTION cm_ph_cmd_aoc_get_ccm

@DESCRIPTION
  This function returns the current value of the Current Call Meter.

@DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  None.

@SIDE_EFFECT
  None
@x
===========================================================================*/
extern unsigned long cm_ph_cmd_aoc_get_ccm(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_aoc_get_ccm */
#endif



/**===========================================================================

@FUNCTION cm_ph_cmd_aoc_get_acm

@DESCRIPTION
  This function returns the current value of the Accumulated Call Meter.

@DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  None.

@SIDE_EFFECT
  None
@x
===========================================================================*/
extern unsigned long cm_ph_cmd_aoc_get_acm(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_aoc_get_acm */
#endif


/**===========================================================================

@FUNCTION cm_ph_cmd_aoc_get_acmmax

@DESCRIPTION
  This function returns the current value of the Accumulated Call Meter Max.

@DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  None.

@SIDE_EFFECT
  None
@x
===========================================================================*/
extern unsigned long cm_ph_cmd_aoc_get_acmmax(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_aoc_get_acmmax */
#endif

/**===========================================================================

@FUNCTION cm_ph_cmd_aoc_get_puct

@DESCRIPTION
  This function returns the current value of the AOC Price Unit Conversion value.

@DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  None.

@SIDE_EFFECT
  None
@x
===========================================================================*/
extern unsigned long cm_ph_cmd_aoc_get_puct(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_aoc_get_puct */
#endif

/**===========================================================================
@FUNCTION cm_ph_cmd_line_switching

@DESCRIPTION

  Queue CM_PH_CMD_LINE_SWITCHING command to CM.

@DEPENDENCIES
  FEATURE_ALS and FEATURE_GSM have to be defined.

@RETURNS

  TRUE
  Command was queued successfully to CM

  FALSE
  No CM buffer available to queue the command

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern boolean cm_ph_cmd_line_switching(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /**< client cmd callback function */

  void                                *data_block_ptr,
    /**< pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /**< commanding client's id */

  cm_als_line_switching_e_type         status
    /**< whether the user is allowed to switch between lines */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_line_switching */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================
@FUNCTION cm_ph_cmd_selected_line

@DESCRIPTION

  Queue CM_PH_CMD_SELECTED_LINE command to CM.

@DEPENDENCIES
  FEATURE_ALS and FEATURE_GSM have to be defined.

@RETURNS

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern boolean cm_ph_cmd_selected_line(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /**< client cmd callback function */

  void                                *data_block_ptr,
    /**< pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /**< commanding client's id */

  cm_als_line_e_type                   selected_line
    /**< selected line */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_selected_line */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_ph_cmd_update_cdma_capability

@DESCRIPTION
  Command CM to update the CDMA2000 capability information in form of
  IS2000 Status message.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_update_cdma_capability(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /**< Client callback function */

  void                              *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /**< Requesting client */

  const sys_is2000_msg_s_type             *is2000_msg_ptr
    /**< Pointer to the IS2000 message */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_cmd_update_cdma_capability */
/*~ PARAM is2000_msg_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/** @}
*  endgroup PHONE Phone Related Definitions
*/



/****************************************************************************

Serving System Object

****************************************************************************/

/** @defgroup SSO Serving System Object
** @{
*/

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/
/* apitrim -include */

/* Default values for serving system stored information.
** For CDMA only.
*/
#define CM_SS_SID_NONE                         ( (uint16) 0 )
  /**< Indicates an undefined SID */

#define CM_SS_NID_NONE                         ( (uint16) 0 )
  /**< Indicates an undefined NID */

#define CM_SS_REG_ZONE_NONE                    ( (uint16) 0xFFFF )
  /**< Indicates an undefined registration zone */

#define CM_SS_PACKET_ZONE_NONE                 ( (uint8) 0 )
  /**< Indicates an undefined packet zone */

#define CM_SS_BS_P_REV_NONE                    ( (uint8) 0 )
  /**< Indicates an undefined base station p rev */

#define CM_SS_P_REV_IN_USE_NONE                ( (uint8) 0 )
  /**< Indicates an undefined p rev */

/**< Bit masks indicating which fields are changed
** in cm_mm_ss_info_s_type structure.
*/
#define CM_SS_SID_MASK                          ( SYS_BM_64BIT(0) )
  /**< SID */

#define CM_SS_NID_MASK                          ( SYS_BM_64BIT(1)  )
  /**< NID */

#define CM_SS_REG_ZONE_MASK                     ( SYS_BM_64BIT(2)  )
  /**< Registration Zone */

#define CM_SS_PACKET_ZONE_MASK                  ( SYS_BM_64BIT(3)  )
  /**< Packet Zone */

#define CM_SS_BS_P_REV_MASK                     ( SYS_BM_64BIT(4)  )
  /**< Basestation P-Rev */

#define CM_SS_P_REV_IN_USE_MASK                 ( SYS_BM_64BIT(5)  )
  /**< P-Rev in use */

#define CM_SS_CCS_SUPPORTED_MASK                ( SYS_BM_64BIT(6)  )
  /**< Concurrent services supported? */

#define CM_SS_SRV_STATUS_MASK                   ( SYS_BM_64BIT(7)  )
  /**< Serving Status - No, limited, full */

#define CM_SS_CDMA_SRV_STATUS_MASK              CM_SS_SRV_STATUS_MASK

#define CM_SS_SRV_DOMAIN_MASK                   ( SYS_BM_64BIT(8)  )
  /**< Service Domain */

#define CM_SS_CDMA_SRV_DOMAIN_MASK              CM_SS_SRV_DOMAIN_MASK

#define CM_SS_SRV_CAPABILITY_MASK               ( SYS_BM_64BIT(9)  )
  /**< System/s service capability */

#define CM_SS_CDMA_SRV_CAPABILITY_MASK          CM_SS_SRV_CAPABILITY_MASK

#define CM_SS_SYS_MODE_MASK                     ( SYS_BM_64BIT(10) )
  /**< System mode */

#define CM_SS_ROAM_STATUS_MASK                  ( SYS_BM_64BIT(11) )
  /**< Roaming status */

#define CM_SS_CDMA_ROAM_STATUS_MASK             CM_SS_ROAM_STATUS_MASK

#define CM_SS_SYS_ID_MASK                       ( SYS_BM_64BIT(12) )
  /**< System ID */

#define CM_SS_CDMA_SYS_ID_MASK                  CM_SS_SYS_ID_MASK

#define CM_SS_SRV_IND_MASK                      ( SYS_BM_64BIT(13) )
  /**< Service indicators: This mask indicates a change of service occurred.
  ** It is set even if the service is the same as the previous service. */

#define CM_SS_MOBILITY_MGMT_MASK                ( SYS_BM_64BIT(14) )
  /**< Mobility Management */

#define CM_SS_IDLE_DIGITAL_MODE_MASK            ( SYS_BM_64BIT(15) )
  /**< HDR: Idle digital mode */

#define CM_SS_SIM_STATE_MASK                    ( SYS_BM_64BIT(16) )
  /**< SIM card status */

#define CM_SS_PLMN_FORBIDDEN_MASK               ( SYS_BM_64BIT(17) )
#define CM_SS_SYS_FORBIDDEN_MASK                CM_SS_PLMN_FORBIDDEN_MASK
  /**< Forbidden PLMN/System */

#define CM_SS_PS_DATA_SUSPEND_MASK              ( SYS_BM_64BIT(18) )
  /**< PS Data Suspend MASK */

#define CM_SS_UZ_CHANGED_MASK                   ( SYS_BM_64BIT(19) )
  /**< UZ values changed */

#define CM_SS_HDR_SRV_STATUS_MASK               ( SYS_BM_64BIT(20) )
  /**< HDR Serving Status - No, limited, full */

#define CM_SS_HDR_ROAM_STATUS_MASK              ( SYS_BM_64BIT(21) )
  /**< HDR Roaming status */

#define CM_SS_MAIN_BCMCS_SRV_SUPPORTED_MASK     ( SYS_BM_64BIT(22) )
  /**< Whether 1X supports BCMCS service */

#define CM_SS_MAIN_BCMCS_SRV_STATUS_MASK        ( SYS_BM_64BIT(23) )
  /**< BCMCS service status on 1X */

#define CM_SS_HYBR_HDR_BCMCS_SRV_SUPPORTED_MASK ( SYS_BM_64BIT(24) )
  /**< Whether HDR supports BCMCS service */

#define CM_SS_HYBR_HDR_BCMCS_SRV_STATUS_MASK    ( SYS_BM_64BIT(25) )
  /**< BCMCS service status on HDR */

#define CM_SS_BASE_STATION_PARMS_CHGD_MASK      ( SYS_BM_64BIT(26) )
  /**< Base station Parameters changed */

/**< Clients check CM_API_WLAN for WLAN support for following*/
#define CM_SS_WLAN_SRV_STATUS_MASK              ( SYS_BM_64BIT(27) )
  /**< WLAN Serving Status - No, full */

#define CM_SS_WLAN_BSS_INFO_MASK                ( SYS_BM_64BIT(28) )
  /**< WLAN BSS Info is change is indicated */

#define CM_SS_ORIG_THR_STATUS_MASK              ( SYS_BM_64BIT(29) )
  /**< Origination throttle status changed */

#define CM_SS_HDR_SYS_ID_MASK                   ( SYS_BM_64BIT(30) )
  /**< HDR System ID */

#define CM_SS_MCC_MASK                          ( SYS_BM_64BIT(31) )
  /**< MCC changed */

#define CM_SS_IMSI_11_12_MASK                   ( SYS_BM_64BIT(32) )
  /**< IMSI changed */

#define CM_SS_PSIST_CHGD_MASK                   ( SYS_BM_64BIT(33) )
  /**< Psist information changed */

#define CM_SS_ACTIVE_BAND_MASK                  ( SYS_BM_64BIT(34) )
  /**< Clients needs to check for CM_API_ECATEG_IN_NUM_STRUCT before
  ** using this field.
  **
  ** Active band information
  */

#define CM_SS_GW_SRV_STATUS_MASK                ( SYS_BM_64BIT(35) )
  /**< GW Serving Status - No, limited, full */

#define CM_SS_GW_ROAM_STATUS_MASK               ( SYS_BM_64BIT(36) )
  /**< GW Roaming status */

#define CM_SS_GW_SRV_DOMAIN_MASK                ( SYS_BM_64BIT(37) )
  /**< Srv domain on GW changed */

#define CM_SS_GW_SRV_CAPABILITY_MASK            ( SYS_BM_64BIT(38) )
  /**< Srv capability on GW changed */

#define CM_SS_GW_SYS_MODE_MASK                  ( SYS_BM_64BIT(39) )
  /**< Sys mode on GW changed */

#define CM_SS_GW_ACTIVE_BAND_MASK               ( SYS_BM_64BIT(40) )
  /**< Clients needs to check for CM_API_ECATEG_IN_NUM_STRUCT before
  ** using this field.
  **
  ** Active band information
  */

#define CM_SS_GW_SYS_ID_MASK                    ( SYS_BM_64BIT(41) )
  /**< System ID */

#define CM_SS_GW_SYS_FORBIDDEN_MASK             ( SYS_BM_64BIT(42) )


#define CM_SS_SIMULTANEOUS_MODE_MASK            ( SYS_BM_64BIT(43) )
  /**< Sumultaneous mode changed */

#define CM_SS_HDR_ACTIVE_PROT_MASK              ( SYS_BM_64BIT(44) )
  /**< HDR active protocol is changed.
  ** Need to define CM_API_HDR_ACTIVE_PROT to use this field.
  */
#define CM_SS_WLAN_LINK_QUAL_MASK               ( SYS_BM_64BIT(45) )
  /**< WLAN LINK QUALITY has changed in the WLAN_STATS ss event.
  */

#define CM_SS_WLAN_TX_RX_STATS_MASK             ( SYS_BM_64BIT(46) )
  /**< WLAN TX RX stats have changed in the WLAN STATS ss event.
  */

#define CM_SS_TRUE_SRV_STATUS_MASK              ( SYS_BM_64BIT(47)  )
  /* True Serving Status - No, limited, limited_regional */

#define CM_SS_TRUE_GW_SRV_STATUS_MASK           ( SYS_BM_64BIT(48) )
  /* GW Serving Status - No, limited, limited_regional */

#define CM_SS_CELL_INFO_MASK                    ( SYS_BM_64BIT(49) )
  /**< CELL_INFO: change in cell_id and neighbor cell_id
  */

#define CM_SS_GAN_STATE_MASK                    ( SYS_BM_64BIT(50) )
  /**< UMA GAN client state
  **  init and shutdown indication
  */

#define CM_SS_GAN_CONN_STATUS_MASK              ( SYS_BM_64BIT(51) )
  /**< UMA GAN connection status
  */

#define CM_SS_GAN_SIG_CHAN_STATUS_MASK          ( SYS_BM_64BIT(52) )
  /**< UMA GAN signal channel status
  */

#define CM_SS_GAN_MODE_DISPLAY_MASK             ( SYS_BM_64BIT(53) )
  /**< UMA GAN indication whether WLAN 
  **  or GERAN becomes the active RAT 
  */

#define CM_SS_GAN_ERROR_PROMPT_MASK             ( SYS_BM_64BIT(54) )
  /**< UMA GAN error category
  */

#define CM_SS_ACTIVE_CHANNEL_MASK               ( SYS_BM_64BIT(55) )
  /**< Active channel information
  */

#define CM_SS_HYBRID_ACTIVE_BAND_MASK              ( SYS_BM_64BIT(56) )
  /**< Active HYBRID channel information
  */

#define CM_SS_HYBRID_ACTIVE_CHANNEL_MASK           ( SYS_BM_64BIT(57) )
  /**< Active HYBRID channel information
  */

#define CM_SS_DEF_ROAM_IND_MASK                    ( SYS_BM_64BIT(58) )
  /** Default roaming indicator from PRL
  */

#define CM_SS_SYS_PRL_MATCH_MASK                   ( SYS_BM_64BIT(59) )
  /** PRL Match info changed
  */

#define CM_SS_CDMA_TIME_CHGD_MASK                  ( SYS_BM_64BIT(60) )
  /** CDMA/HDR time related info changed
  */

/**< Clients need to check for CM_API_EMERGENCY_NUM_LIST before using.
** Masks for Emergency number types
*/
#define CM_EMERG_NUM_TYPE_NONE                        ( (uint32) 0 )
   /**< Clients need to use this type when no
   ** category is defined for emergency call
   */

#define CM_EMERG_NUM_TYPE_POLICE_MASK                 ( BM(0) )
   /**< Police */

#define CM_EMERG_NUM_TYPE_AMBULANCE_MASK              ( BM(1) )
   /**< Ambulance */

#define CM_EMERG_NUM_TYPE_FIRE_BRIGADE_MASK           ( BM(2) )
   /**< Fire Brigade */

#define CM_EMERG_NUM_TYPE_MARINE_GUARD_MASK           ( BM(3) )
   /**< Marine Guard */

#define CM_EMERG_NUM_TYPE_MOUNTAIN_RESCUE_MASK        ( BM(4) )
   /**< Mountain Rescue */

#define CM_EMERG_NUM_TYPE_MAX                         ( BM(5) )
   /**< For bounds checking, CM INTERNAL USE ONLY */

/* apitrim -end-include */

#define CM_COUNTRY_CODE_LIST_SIZE                      5
   /* List of countries presented for conflict resolution */

#define CM_INVALID_MOBILE_COUNTRY_CODE                (cm_country_code_type)(-1)
   /* Country code not pointing to any country
   ** Mirrors SD_MCC_NONE value
   **
   **
   */

#define CM_MAX_MOBILE_COUNTRY_CODE_VAL                1000
   /* Mobile country code cannot exceed this value.
   ** Mirrors SD_MCC_MAX value
   */


/**< Bit masks indicating which signal_strength fields are changed
** in cm_mm_ss_info_s_type structure used in RSSI event.
*/
#define CM_SS_RSSI_MASK                          ( SYS_BM_64BIT(0) )
  /**< RSSI */

#define CM_SS_RSCP_MASK                          ( SYS_BM_64BIT(1) )
  /**< RSCP */

#define CM_SS_ECIO_MASK                          ( SYS_BM_64BIT(2) )
  /**< ECIO */

#define CM_SS_IO_MASK                            ( SYS_BM_64BIT(3) )
  /**< IO */

#define CM_SS_SINR_MASK                          ( SYS_BM_64BIT(4) )
  /**< SINR */

#define CM_SS_SIR_MASK                           ( SYS_BM_64BIT(5) )
  /**< SIR */

#define CM_SS_PATHLOSS_MASK                      ( SYS_BM_64BIT(6) )
  /**< PATHLOSS */


/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/** Enumeration of serving system commands client are able to send to CM.
*/
typedef enum cm_ss_cmd_e {

  CM_SS_CMD_NONE=-1,
    /**< @internal */

  CM_SS_CMD_INFO_GET,
    /**< Get serving system info */

  CM_SS_CMD_PS_DATA_AVAIL,
    /**< Get PS data available request from DS */

  CM_SS_CMD_ORIG_THR_UPDATE_TBL,
    /**< Update the throttle table */

  CM_SS_CMD_COUNTRY_SELECTED,
    /* Update current country as sel by user */

  CM_SS_CMD_GET_MEASUREMENT,
    /* Obtain system measurements */

  CM_SS_CMD_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_ss_cmd_e_type;


/** Enumeration of serving system command errors returned by call command
** callback functions.
*/
typedef enum cm_ss_cmd_err_e {

  CM_SS_CMD_ERR_NONE=-1,
    /**< @internal */

  CM_SS_CMD_ERR_NOERR,
    /**< No errors found */

  /* Error codes resulting from clients feeding a serving system command
  ** function with bad or inappropriate parameters.
  */

  CM_SS_CMD_ERR_CLIENT_ID_P,
    /**< Bad client parameter */

  /* Error codes resulting from operational limitation
  ** of CM.
  */

  CM_SS_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command */

  CM_SS_CMD_ERR_OTHER,
    /**< any other errors not listed above */

  /* Error codes resulting from clients trying to instruct
  ** CM to perform inappropriate actions relative to the current
  ** state of the call and/or phone.
  */

  CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  CM_SS_CMD_ERR_OFFLINE_S,
    /**< Phone is offline */

  CM_SS_CMD_ERR_SRV_STATE_S,
    /**< Serving status is wrong */

  CM_SS_CMD_ERR_PS_DATA_SUSPEND_S,
    /**< PS data suspend state is not correct. */

  CM_SS_CMD_ERR_INCORRECT_MODE_S,
    /**< Phone is not in correct mode to honor 
    ** the request */

  CM_SS_CMD_ERR_BAD_PARM_P,
    /**< One of the parameters of the command 
    ** is in error */

  CM_SS_CMD_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_ss_cmd_err_e_type;


/** Enumeration of call origination throttling reasons.
*/

typedef enum cm_ss_orig_thr_reasons_e {

  CM_SS_ORIG_THR_REASON_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_SS_ORIG_THR_DS_MIP_UP_SUCCESS,
    /**< MIP Success */

  CM_SS_ORIG_THR_DS_MIP_PDSN_FAILURE,
    /**< PDSN, PPP setup  failure */

  CM_SS_ORIG_THR_DS_MIP_SOL_TIMEOUT,
    /**< MIP Solicitation Timeout */

  CM_SS_ORIG_THR_DS_MIP_RRQ_TIMEOUT,
    /**< MIP RRQ Timeout */

  /* 4 */
  CM_SS_ORIG_THR_DS_MIP_FA_FAILURE,
    /**< MIP FA Failure */

  CM_SS_ORIG_THR_DS_MIP_HA_FAILURE,
    /**< MIP HA Failure */

  CM_SS_ORIG_THR_DS_MIP_FAILURE,
    /**< MIP fail due to any other network reason */

  CM_SS_ORIG_THR_DS_PPP_UP_SUCCESS,
    /**< PPP Success */

  /* 8 */
  CM_SS_ORIG_THR_DS_PPP_AUTH_FAILURE,
    /**< PPP Authorization failure */

  CM_SS_ORIG_THR_DS_PPP_LCP_TIMEOUT,
    /**< PPP LCP Timeout */

  CM_SS_ORIG_THR_DS_PPP_NCP_TIMEOUT,
    /**< PPP IPCP Timeout */

  CM_SS_ORIG_THR_DS_PPP_FAILURE,
    /**< PPP fail due to any other network reason */

  /* 12 */
  CM_SS_ORIG_THR_CM_INTERCEPT,
    /**< Received Intercept from BS */

  CM_SS_ORIG_THR_CM_REL_SO_REJ,
    /**< SO Reject */

  CM_SS_ORIG_THR_CM_CD_GEN_OR_BUSY,
    /**< Connection Deny with deny code busy or general */

  CM_SS_ORIG_THR_CM_CD_BILL_OR_AUTH,
    /**< Billing or auth failure */

  /* 16 */
  CM_SS_ORIG_THR_DS_SIP_FAILURE,
    /**< SIP Failure */

  CM_SS_ORIG_THR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */
} cm_ss_orig_thr_reasons_e_type;


/** Enumeration of ipapp registration states
** Clients needs to check for CM_API_VIDEOSHARE
** before using this type
*/
typedef enum {

  CM_IPAPP_REG_STATUS_NONE=-1,
    /**< @internal */

  CM_IPAPP_REG_STATUS_REGISTERED,
    /**< IP app has successfully registered with network.
    */

  CM_IPAPP_REG_STATUS_DEREGISTERED,
    /**< IP app has deregistered from network.
    */

  CM_IPAPP_REG_STATUS_FAILURE,
    /**< IP app has failed to register
    */

  CM_IPAPP_REG_STATUS_MAX
    /**< @internal */

} cm_ipapp_reg_status_e_type;


/** Enumeration of serving system events.
** Clients are able to register with CM to be notified of any subsets
** of these serving system events.
*/
typedef enum cm_ss_event_e {

  CM_SS_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_SS_EVENT_SRV_CHANGED,
    /**< Serving system information changed */

  CM_SS_EVENT_RSSI,
    /**< RSSI changed */

  CM_SS_EVENT_INFO,
    /**< Serving system information, this event only sent to the client
    ** that request this information through cm_ss_cmd_get_ss_info() */

  /* Registration results - FEATURE_JCDMA only
  */
  CM_SS_EVENT_REG_SUCCESS,
    /**< Registration Success */

  CM_SS_EVENT_REG_FAILURE,
    /**< Registration Failure */

  /* 5 */
  CM_SS_EVENT_HDR_RSSI,
    /**< RSSI changed */

  CM_SS_EVENT_WLAN_RSSI,
    /**< WLAN RSSI */
    /* Clients check CM_API_WLAN for WLAN support*/

  CM_SS_EVENT_SRV_NEW,
    /**< Serving system information changed */

  CM_SS_EVENT_SECONDARY_MSM,
    /**< Invoke event call back even if the event is from secondary MSM.
    ** CM will not generate any events with this, but is used by
    ** thin client CM layer to call client call backs for events from
    ** secondary msm.
    */

  CM_SS_EVENT_PS_DATA_AVAIL,
    /**< Received PS data available request */

  /* 10 */
  CM_SS_EVENT_PS_DATA_FAIL,
    /**< Indicate fail to resume PS data */

  CM_SS_EVENT_PS_DATA_SUCCESS,
    /**< Indicate success in resuming PS data */

  CM_SS_EVENT_WLAN_STATS,
    /**< Indicate updates in WLAN Stats.*/

  CM_SS_EVENT_ORIG_THR_TBL_UPDATE,
    /**< Indicates updates to the Call origination throttle table */


  CM_SS_EVENT_EMERG_NUM_LIST,
    /**< CM_API_EMERGENCY_NUM_LIST needs to be checked before using
    ** this enum
    **
    ** Indicates that list of emergency numbers are available from the
    ** network. emerg_num_list in cm_mm_ss_info_s_type carries the numbers
    */

  /* 15 */
  CM_SS_EVENT_IPAPP_REG_STATUS,
    /**< Clients should check for CM_API_VIDEOSHARE before using
    ** this field.
    ** Indicates registration status of an IP app
    */

  CM_SS_EVENT_GW_RSSI,
    /**< RSSI for GW systems
    */

  CM_SS_EVENT_HDR_LN_STATUS,
    /* Location notification status event */

  CM_SS_EVENT_SELECT_COUNTRY,
    /* CM_API_PLUS_DIALING needs to be checked before using
    ** this enum.
    **
    ** Event sent to prompt user to select a country among given
    ** list. country_list field in cm_mm_ss_info_s_type will carry
    ** the list of countries.
    **
    ** UI should also present an option called "OTHER COUNTRY" to the user,
    ** which can be selected if user does not find any of the listed
    ** countries applicable. country_code can be set to CM_INVALID_MOBILE_COUNTRY_CODE
    ** when cm_ss_cmd_country_selected() is called.
    */


  CM_SS_EVENT_COUNTRY_SELECTED,
    /* CM_API_PLUS_DIALING needs to be checked before using
    ** this enum.
    **
    ** Event sent to inform clients of user selected country.
    ** current_country_code in cm_mm_ss_info_s_type will carry
    ** the current country.
    */
  
  /* 20 */
  CM_SS_EVENT_REG_REJECT,
    /* Event to sent to indicate Registration failure.
    ** Feature GSM or WCDMA only
    */

  CM_SS_EVENT_MEAS_RESPONSE,
    /* Event providing details regarding measurement response
    ** Sent in response to cmd CM_SS_CMD_GET_MEASUREMENT
    ** Functionality controlled by FEATURE_CM_SS_MEAS
    */

  CM_SS_EVENT_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_ss_event_e_type;



/** Enumeration of serving system measurement response status
*/
typedef enum cm_ss_meas_status_e{

  CM_SS_MEAS_STATUS_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_SS_MEAS_STATUS_OK,
    /**< Measurement was successful  */

  CM_SS_MEAS_STATUS_NO_SRV,
    /**< Measurement failed, no service */

  CM_SS_MEAS_STATUS_BAR_PARM,
    /**< Meas req parameters incorrect */

  /* 3 */
  CM_SS_MEAS_STATUS_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_ss_meas_status_e_type;


/** Enumeration of avoid system type.
*/
typedef enum cm_avoid_sys_e_type {

  CM_AVOID_SYS_SRV_IF_UZ, /* =  SD_SS_AVOID_SYS_SRV_IF_UZ,*/
    /**< Avoid serving system only if MS has currently selected
    ** user zone */

  CM_AVOID_SYS_SRV,       /* =  SD_SS_AVOID_SYS_SRV, */
    /**< Avoid serving system */

  CM_AVOID_SYS_MAX       /* =  SD_SS_AVOID_SYS_MAX */
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */


} cm_avoid_sys_e_type;


/** Enumeration of serving system origination throttle action
*/
typedef enum cm_ss_orig_thr_act_e{

  CM_SS_ORIG_THR_ACT_NONE = -1,
    /**< No action should be taken */

  CM_SS_ORIG_THR_RESET_TBL,
    /**< Reset the orig throttle table */

  CM_SS_ORIG_THR_UPDATE_SS,
    /**< Update a particular orig throttle entry */

  CM_SS_ORIG_THR_UPDATE_CURR_SS,
    /**< Update the current orig throttle entry */

  CM_SS_ORIG_THR_ACT_MAX
    /**< @internal */

} cm_ss_orig_thr_act_e_type;


typedef enum cm_psist_e{

  CM_PSIST_NONE = -1,
    /**< @internal */

  CM_PSIST_LOW,
    /**< Low psist */

  CM_PSIST_HIGH,
    /**< HIgh Psist */

  CM_PSIST_UNKNOWN,
    /**< Unknown psist */

  CM_PSIST_MAX
    /**< @internal */
} cm_psist_e_type;


/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** CM_API_EMERGENCY_NUM_LIST needs to be checked before using this struct.
**
** Structure to hold emergency number and type
*/
typedef struct cm_emerg_num_s {

  uint32                    num_type;
    /**< Type of emergency number. Carries Bits with
    ** positions that are defined by CM_EMERG_NUM_TYPE_*
    */

  cm_num_s_type             num;
    /**< Digits in the number */

} cm_emerg_num_s_type;



/** CM_API_EMERGENCY_NUM_LIST needs to be checked before using this struct.
**
** Structure to hold list of emergency numbers
*/
typedef struct cm_emerg_num_list_s {

  int                       num_list_len;
    /**< Count of emergency numbers present */

  cm_emerg_num_s_type       num_list [CM_MAX_EMERGENCY_NUM_COUNT];
    /**< Emergency number type and digits */

} cm_emerg_num_list_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_emerg_num_list_s.num_list VARRAY CM_MAX_EMERGENCY_NUM_COUNT
    LENGTH cm_emerg_num_list_s.num_list_len */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/** CM_API_CELL_SUPP_SERVICES needs to be checked before using this struct.
**
** Structure to hold information about services in current cell
*/
typedef struct cm_cell_srv_ind_s {

  sys_hs_ind_e_type         hs_ind;
    /**< High speed service indication (Over WCDMA) */

  sys_dtm_support_e_type    dtm_supp;
    /**< Dual Transfer mode indication (Over GSM) */

  sys_egprs_support_e_type  egprs_supp;
    /**< EGPRS indication  */

  sys_hs_ind_e_type         hs_call_status;
    /**< Clients need to check for CM_API_CELL_SUPP_SERVICES
  ** and CM_API_CELL_SUPP_SERVICES_HS_CALL  before
    ** using this structure.
  */

    /**< Reusing HS enums to indicate call status
  ** on HS.
  **
  ** SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL - No calls in progress on HS.
  **
  ** SYS_HS_IND_HSDPA_SUPP_CELL  - Call in progress on HSDPA.
  **
  ** SYS_HS_IND_HSUPA_SUPP_CELL  - Call in progress on HSUPA
  */

} cm_cell_srv_ind_s_type;

/** Structure for registration reject information
*/
typedef struct cm_reg_reject_info_s {

  sys_srv_domain_e_type        reject_srv_domain;
    /**<   Srv domain on which registration is rejected */

  byte                         reject_cause;
    /**<  Registration Reject Cause */

} cm_reg_reject_info_s_type;

/**
** Type to hold serving system information for GSM or WCDMA modes.
*/
typedef struct cm_ss_gw_info_s {

  uint8                                   bit_err_rate;
    /**< Bit Error Rate / RX QUAL in GSM */

  sys_sim_state_e_type                    sim_state;
    /**< Current state of the SIM.
    ** Mask used is CM_SS_SIM_STATE_MASK.
    */

  boolean                                 plmn_forbidden;
    /**< Indicates whether the PLMN is forbidden.
    ** Mask used is CM_SS_PLMN_FORBIDDEN_MASK.
    */

  sys_mm_information_s_type                mm_information;
    /**< Mobility management information, i.e. network name, date,
    ** time and time zone of registered network */

} cm_ss_gw_info_s_type;


/**
** Type to hold serving system information for CDMA modes.
** Applicable for FEATURE_CDMA_800 or FEATURE_CDMA_1900.
*/
typedef struct cm_ss_cdma_info_s {

  sys_band_class_e_type        band_class;
    /**< cellular or PCS */

  sys_blksys_e_type            block_or_system;
    /**< PCS frequency block: A to F Cellular System A or B*/

  uint16                       reg_zone;
    /**< 12 bit registration zone */

  uint16                       packet_zone;
    /**< 8  bit packet zone, 0xffff means PACKET ZONE NONE.
    ** Mask used is CM_SS_PACKET_ZONE_MASK.
    */

  uint8                        bs_p_rev;
    /**< 8  bit base station p_rev.
    ** Mask used is CM_SS_BS_P_REV_MASK.
    */

  uint8                        p_rev_in_use;
    /**< 8  bit p_rev in use the lessor of BS p_rev and mobile p_rev.
    ** Mask used is CM_SS_P_REV_IN_USE_MASK.
    */

  boolean                      is_registered;
    /**< is mobile registered on the current system */

  boolean                      ccs_supported;
    /**< is concurrent services supported.
    */

  int32                        uz_id;
     /**< User zone Identification of the selected user zone,
     ** == CM_INVALID_UZID, if no user zone selected. FEATURE_USERZONE.
     ** Mask used is CM_SS_UZ_CHANGED_MASK.
     */

  sys_puzl_uz_name_s_type      uz_name;
     /**< User zone name of the selected user zone.
     ** Valid only if uz_id != CM_INVALID_UZID. FEATURE_USERZONE.
     ** Mask used is CM_SS_UZ_CHANGED_MASK.
     */

  sys_srch_win_type            srch_win_n;
    /**< Neighbour set window search size.
    ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
    */

  sys_base_lat_type            base_lat;
    /**< Base station latitude[0.25sec and already sign extended].
    ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
    */

  sys_base_long_type           base_long;
    /**< Base station latitude[0.25sec and already sign extended].
    ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
    */

  sys_base_id_type             base_id;
    /**< Base station Identification number.
    ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
    */

  sys_time_info_s_type         time_info;
    /**< Time information obtained from the CDMA/HDR 
    ** OTA messages. Mask used is CM_SS_CDMA_TIME_CHGD_MASK.
    ** If time_info.sys_mode is SYS_SYS_MODE_NO_SRV, it means that
    ** currently there is no valid time info available.
    */

} cm_ss_cdma_info_s_type;


/** SS information
*/
typedef union cm_ss_info_u {

  cm_ss_gw_info_s_type     gw_info;
    /**< GSM/WCDMA SS information */

  cm_ss_cdma_info_s_type   cdma_info;
    /**< CDMA SS information */

} cm_ss_info_u_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ IF (_DISC_ == SYS_SYS_MODE_GSM || _DISC_ == SYS_SYS_MODE_WCDMA)
       cm_ss_info_u.gw_info */
/*~ DEFAULT cm_ss_info_u.cdma_info */
#endif


/** HDR specific Measurement response info
*/
typedef struct cm_ss_hdr_resp_s{

  sys_pilot_pn_type        ref_pilot_pn;
    /**< Reference pilot PN
    */

  byte                     ref_pilot_strength;
    /**< Reference pilot PN strength.
    */

  sys_channel_type         ref_pilot_chan;
    /**< Reference pilot band class and channel num
    */

  byte                     num_pilots; 
    /**< Number of pilots for which measurement is returned
    */

  sys_pilot_info_s_type    pilot_info [ CM_MAX_DO_NEIGHBOR_CNT ];
    /**< Pilot measurements
    */

  cm_ss_meas_status_e_type  meas_status;
    /**< The status of the meas request. Can convey
    ** information about failure scenarios
    */

}cm_ss_hdr_resp_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_ss_hdr_resp_s.pilot_info VARRAY CM_MAX_DO_NEIGHBOR_CNT
    LENGTH cm_ss_hdr_resp_s.num_pilots */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/** Union of Technology specific Measurement response info
*/
typedef union cm_ss_meas_resp_info_u {

  cm_ss_hdr_resp_s_type   hdr_resp;
    /**< HDR sys information 
    */

} cm_ss_meas_resp_info_u_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ IF (_DISC_ == SYS_SYS_MODE_HDR ) cm_ss_meas_resp_info_u.hdr_resp */
/*~ DEFAULT cm_ss_meas_resp_info_u.void */
#endif

/** Response to measurement request
*/
typedef struct cm_ss_meas_resp_s {

  sys_sys_mode_e_type            sys_mode;
    /**< The mode of the system for which measurement
    ** response is being given */

  cm_ss_meas_resp_info_u_type    info;
    /**< The technology specific info that is 
    ** conveyed by the underlying protocol  */

}cm_ss_meas_resp_s_type; 
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_ss_meas_resp_s.info DISC cm_ss_meas_resp_s.sys_mode */
#endif

/** HDR Measurement request info
*/
typedef struct cm_ss_hdr_info_s{

  boolean                  srch_win_size_inc;
    /**< indicates if search window size is included or not 
    */

  byte                     srch_win_size;
    /**< search window size . 
    ** valid only if srch_win_size_inc = TRUE
    */

  sys_channel_type         channel;
    /**< HDR channel info
    */

  byte                     neighbor_count;
    /**< number of neighbours for which meas is requested
    */

  sys_pilot_pn_type        neighbor_pilot_pn [ CM_MAX_DO_NEIGHBOR_CNT ];
    /**< pilot PNs of neighbours for which meas is requested
    */
}cm_ss_hdr_info_s_type;    
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_ss_hdr_info_s.neighbor_pilot_pn VARRAY CM_MAX_DO_NEIGHBOR_CNT
    LENGTH cm_ss_hdr_info_s.neighbor_count */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/** Tech specific Measurement request info
*/
typedef union cm_ss_meas_request_info_u {

  cm_ss_hdr_info_s_type   hdr_info;
    /**< HDR sys information
     */

} cm_ss_meas_request_info_u_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ IF (_DISC_ == SYS_SYS_MODE_HDR ) cm_ss_meas_request_info_u.hdr_info */
/*~ DEFAULT cm_ss_meas_request_info_u.void */
#endif

/** Measurement request info
*/
typedef struct cm_ss_meas_req_s {

  sys_sys_mode_e_type            sys_mode;
    /**< The mode of the system for which measurement
    ** is requested  */

  cm_ss_meas_request_info_u_type info;
    /**< The technology specific info needed to
    ** make the measurement  */

}cm_ss_meas_req_s_type;    
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_ss_meas_req_s.info DISC cm_ss_meas_req_s.sys_mode */
#endif


/* Structure to hold throttling information
*/
typedef struct cm_ss_orig_thr_info_type {

  boolean                       ss_thr_status;
    /**< The throttle status of the current ss
    */

  dword                         thr_uptime;
    /**< The throttling time period
    */

  unsigned int                  idx;
    /**< The index in the throttle_time table
    ** Used internally by CM to keep track of the next throttle time
    ** increment
    */

  cm_ss_orig_thr_reasons_e_type call_status;
    /**< The call end reason
    */

} cm_ss_orig_thr_info_s_type;

/* Structure to hold list of country
** codes for user to be prompted with
*/
typedef struct cm_ss_country_list_s {

  boolean                present; /* !!!! CHECK USAGE !!!! */
    /* present flag if this structure
    ** is populated
    */

  cm_sid_type            sid;
    /* Sid that is in conflict with
    ** countries
    */

  uint8                  country_code_len;
    /* Length of country_code_list[]
    */

  cm_country_code_type   country_code_list [CM_COUNTRY_CODE_LIST_SIZE];
    /* Array of mobile country codes that conflict
    ** with given sid
    */

} cm_ss_country_list_s_type;



/** Information returned by ipapp on regis/deregis.
** Clients needs to check for CM_API_VIDEOSHARE
** before using this type
*/
typedef struct cm_ipapp_info_s {

  cm_ipapp_reg_status_e_type reg_status;
    /**< IP app registration status
    */

  int                        cause;
    /**< Cause given when app registration fails or deregisters
    */

  cm_ipapp_warn_info_s_type  warn_info;
    /**< Warning information given by ipapp
    */

  cm_call_type_mask_type     call_type_mask;
    /**< Call tpyes that ipapp can service
    */

  sys_sys_mode_mask_e_type   sys_mode;
    /**< System modes app can support for calls
    ** Can be used only if CM_API_SMS_OVER_IP is defined.
    */

} cm_ipapp_info_s_type;

/**
** Type to hold serving system information.
** When notified of serving system events, clients are given
** a reference to such a structure.
*/
typedef struct cm_mm_ss_info_s {

  uint64                     changed_fields;
    /**< Bit mask indicating which of the following serving system fields
    ** were changed since last notification.  Refer to the #defines
    ** (CM_SS_*_MASK) above. */

  uint64                     signal_strength_changed_fields;
    /**< Bit mask indicating which of the following signal strength fields
     ** were changed since last notification. Refer to the #defines
     ** (CM_SS_*_MASK) above. */

  sys_srv_status_e_type      srv_status;
    /**< The service status (SERVICE/NO SERVICE) of the system.
    ** Mask used is CM_SS_SRV_STATUS_MASK.
    */

  sys_srv_status_e_type      true_srv_status;
    /* The true service status (LIMITED/LIMITED_REGIONAL) of the system.
    ** Mask used is CM_SS_TRUE_SRV_STATUS_MASK.
    */

  sys_srv_domain_e_type      srv_domain;
    /**< The service domain registered on the system. /
    ** Mask used is CM_SS_SRV_DOMAIN_MASK.
    */

  sys_srv_domain_e_type      srv_capability;
    /**< The system's service capability.
    ** Mask used is CM_SS_SRV_CAPABILITY_MASK.
    */

  sys_sys_mode_e_type        sys_mode;
    /**< The system's mode.
    ** Mask used is CM_SS_SYS_MODE_MASK.
    */

  sys_roam_status_e_type     roam_status;
    /**< Indicate current roaming status.
    ** Mask used is CM_SS_ROAM_STATUS_MASK.
    */

  sys_sys_id_s_type          sys_id;
    /**< The ID of the selected system.
    ** Mask used is CM_SS_SYS_ID_MASK.
    */

  boolean                    is_sys_prl_match;
    /**< Indicates whether the System is in PRL
    ** Mask used is CM_SS_SYS_PRL_MATCH_MASK.
    */

  boolean                    is_sys_forbidden;
    /**< Indicates whether the System is forbidden.
    ** Mask used is CM_SS_SYS_FORBIDDEN_MASK.
    */

  boolean                   is_colloc;
    /**< Indicates whether the system is collocated.
    ** Mask used is CM_SS_SYS_ID_MASK.
    */

  uint16                     rssi;
    /**< RSSI in positive dBm i.e. 75 means -75dBm. The range for
    ** AMPS:      -110 to -89 dBm
    ** 800 CDMA:  -105 to -90 dBm
    ** 1900 CDMA: -108 to -93 dBm
    ** GSM/WCDMA: -105 to -60 dBm
    */

  int16                      ecio;
  int                        io;
  byte                       sinr;
    /**< ECIO in negative 0.5 dBm i.e. 63 means -31.5dBm.
    ** Io field is valid only when in HDR mode only operation
    ** SINR range: from 0 to 8, Level 8 represents highest SINR
    ** SINR applicable only when sys_mode = HDR.
    */

  int16                      pathloss;
    /**< Pathloss is applicable only in Connected mode (otherwise 0xFF)
    */

  int16                      sir;
    /**< SIR applicable and valid (otherwise 0xFF) only in WCDMA
    ** connected mode when DPCCH is active.
    */

  int16                      rssi2;
    /**< The -ve RSSI value, coming from RRC layer
    */

  int16                      rscp;
    /**< The range of RSCP would be from -28 to -121 dBm
    */

  boolean                    ecio_for_display;
    /**< TRUE if UE is in Data call
    */

  cm_psist_e_type            psist;
    /**< The psist of the system
    */

  boolean                    ps_data_suspend;
    /**< Indicates that the current PS data service has been suspended.
    ** Mask used is CM_SS_PS_DATA_SUSPEND_MASK.
    */

  boolean                    hdr_hybrid;
    /**< system is hybrid or not. FEATURE_HDR_HYBRID */

  sys_srv_status_e_type      hdr_srv_status;
     /**< Indicate current service state - FEATURE_HDR_HYBRID.
     ** Mask used is CM_SS_HDR_SRV_STATUS_MASK.
     */

  sys_roam_status_e_type     hdr_roam_status;
     /**< Indicate current hybrid hdr roaming status - FEATURE_HDR_HYBRID.
     ** Mask used is CM_SS_HDR_ROAM_STATUS_MASK.
     */

  uint16                     hdr_rssi;
    /**< HDR RSSI in positive dBm i.e. 75 means -75dBm - FEATURE_HDR_HYBRID
    ** The range is -105 to -90 dBm
    */

  int16                      hdr_ecio;
  int                        hdr_io;
  byte                       hdr_sinr;
    /**<
    ** HDR Ec/Io and Io values (used when mobile is in
    ** hybrid operation).
    ** Ec/Io range: from 5 ( -31.5 dB ) to 512 ( 0 dB )
    ** Io    range: from -106 dBm to -21 dBm
    ** SINR range: from 0 to 8, Level 8 represents highest SINR
    ** ( FEATURE_HDR )
    */

  sys_active_prot_e_type     hdr_active_prot;
    /**< The HDR active protocol revision information.
    ** Mask used is CM_SS_HDR_ACTIVE_PROT_MASK
    ** Need to define CM_API_HDR_ACTIVE_PROT before access this field.
    */
  sys_sys_mode_e_type        prev_idle_digital_mode;
    /**< previous idle digital mode. FEATURE_HDR_HANDOFF.
    ** Mask used is CM_SS_IDLE_DIGITAL_MODE_MASK.
    */

  sys_sys_mode_e_type        cur_idle_digital_mode;
    /**< current idle digital mode. FEATURE_HDR_HANDOFF.
    ** Mask used is CM_SS_IDLE_DIGITAL_MODE_MASK.
    */

  cm_ss_info_u_type          mode_info;
    /**< Mode specific serving system information */

  /* FEATURE_BCMCS
  ** These fields specify whether lower layer
  ** supports broadcast/multicast as per
  ** High Rate Broadcast-Multicast Packet data Air
  ** Interface Specification C.S0054 and
  ** BCMCS Framework Draft Document 0.1.3 XP0019
  */

  boolean                    hybr_hdr_bcmcs_srv_supported;
    /**< Specify whether HDR is configured to support BCMCS service.
    ** Mask used is CM_SS_HYBR_HDR_BCMCS_SRV_SUPPORTED_MASK.
    */

  sys_bcmcs_srv_status_e_type hybr_hdr_bcmcs_srv_status;
    /**< Specify whether BCMCS service is available on HDR.
    ** Mask used is CM_SS_HYBR_HDR_BCMCS_SRV_STATUS_MASK.
    */

  boolean                    ss_main_bcmcs_srv_supported;
    /**< Specify whether MAIN is configured to support BCMCS service.
    ** Mask used is CM_SS_MAIN_BCMCS_SRV_SUPPORTED_MASK.
    */

  sys_bcmcs_srv_status_e_type ss_main_bcmcs_srv_status;
    /**< Specify whether BCMCS service is available on MAIN.
    ** Mask used is CM_SS_MAIN_BCMCS_SRV_STATUS_MASK.
    */

  cm_client_id_type          requesting_client_id;
    /**< Requesting client */

  #if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
       defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM) || \
       defined(FEATURE_MM_SUPERSET))
  sys_msm_id_e_type          msm_id;
     /**< Identifies the MSM which generated this event.
     */
  #endif

  /** Clients check CM_API_WLAN for WLAN support for following
  **
  ** wlan_rssi, wlan_srv_status, wlan_bss_info, wlan_stats
  */
  uint16                     wlan_rssi;

  sys_srv_status_e_type      wlan_srv_status;
     /**< Indicate current WLAN service state
     ** Mask used is CM_SS_WLAN_SRV_STATUS_MASK.
     */

  sys_wlan_bss_info_s_type   wlan_bss_info;
    /**< 802.11 (WLAN) System Information
    */

  sys_wlan_stats_s_type      wlan_stats;
    /**< WLAN stats such as Current Transmit Rate, TX/RX bytes.
    */

  sys_srv_domain_e_type      cipher_domain;
    /**< Can be used only if CM_CIPHERING_IND_SUPP is defined.
    ** Ciphering on CS or PS domain or both.
    */

  cm_ss_orig_thr_info_s_type main_thr_status;
    /**< The DCTM status, indicates the current throttle status of the MAIN
    ** system
    */

  cm_ss_orig_thr_info_s_type hdr_thr_status;
    /**< The DCTM status, indicates the current throttle status of the HDR
    ** system
    */

  sys_sys_id_s_type          hdr_sys_id;
    /**< The ID of the HDR sub system.
    ** Mask used is CM_SS_HDR_SYS_ID_MASK.
    */

  dword                      curr_uptime;
    /**< The current uptime
    */

  cm_emerg_num_list_s_type   emerg_num_list;
    /**< Clients need to check for CM_API_EMERGENCY_NUM_LIST before
    ** using this structure.
    **
    ** Emergency number list returned from the network
    */


  cm_ipapp_info_s_type       ipapp_info;
    /**< Clients should check for CM_API_VIDEOSHARE before using
    ** this structure type
    **
    ** Registration information of the app
    */

  sys_band_class_e_type       active_band;
    /**< Clients need to check for CM_API_EMERGENCY_NUM_LIST before
    ** using this structure.
    **
    */

  sys_channel_num_type        active_channel;
    /**< Report active channel of the current serving system.
    */

  sys_band_class_e_type       hybrid_active_band;
    /**< Report active band of hybrid system 
    */

  sys_channel_num_type        hybrid_active_channel;
    /**< Report active channel of the current serving system.
    */

  cm_cell_srv_ind_s_type      cell_srv_ind;
    /**< Clients need to check for CM_API_CELL_SUPP_SERVICES before
    ** using this structure.
    **
    ** Structure to hold information about services in current cell.
    ** Information tied to CM_SS_SRV_IND_MASK.
    */

  #if (defined(FEATURE_CM_TECH_SPEC_SRV_IND) || \
       defined(FEATURE_MM_SUPERSET))

  sys_band_class_e_type      gw_active_band;
    /**< Clients need to check for CM_API_EMERGENCY_NUM_LIST before
    ** using this structure.
    **
    */

  sys_srv_status_e_type      gw_srv_status;
    /* The service status (SERVICE/NO SERVICE) of the system.
    ** Mask used is CM_SS_GW_SRV_STATUS_MASK.
    */

  sys_srv_status_e_type      true_gw_srv_status;
    /* The true service status (LIMITED/LIMITED_REGIONAL) of the system.
    ** Mask used is CM_SS_TRUE_GW_SRV_STATUS_MASK.
    */

  sys_srv_domain_e_type      gw_srv_domain;
    /**< The service domain registered on the system. /
    ** Mask used is CM_SS_SRV_DOMAIN_MASK.
    */

  sys_srv_domain_e_type      gw_srv_capability;
    /**< The system's service capability.
    ** Mask used is CM_SS_SRV_CAPABILITY_MASK.
    */

  sys_sys_mode_e_type        gw_sys_mode;
    /**< The system's mode.
    ** Mask used is CM_SS_SYS_MODE_MASK.
    */

  sys_roam_status_e_type     gw_roam_status;
    /**< Indicate current roaming status.
    ** Mask used is CM_SS_ROAM_STATUS_MASK.
    */

  sys_sys_id_s_type          gw_sys_id;
    /**< The ID of the selected system.
    ** Mask used is CM_SS_SYS_ID_MASK.
    */

  boolean                    gw_is_sys_forbidden;
    /**< Indicates whether the System is forbidden.
    ** Mask used is CM_SS_SYS_FORBIDDEN_MASK.
    */


  uint16                     gw_rssi;
    /**< RSSI in positive dBm i.e. 75 means -75dBm. The range for
    ** AMPS:      -110 to -89 dBm
    ** 800 CDMA:  -105 to -90 dBm
    ** 1900 CDMA: -108 to -93 dBm
    ** GSM/WCDMA: -105 to -60 dBm
    */


  int                        gw_io;
  byte                       gw_sinr;
    /**< ECIO in negative 0.5 dBm i.e. 63 means -31.5dBm.
    ** Io field is valid only when in HDR mode only operation
    ** SINR range: from 0 to 8, Level 8 represents highest SINR
    ** SINR applicable only when sys_mode = HDR.
    */

  int16                      gw_pathloss;
    /**< Pathloss is applicable only in Connected mode (otherwise 0xFF)
    */

  int16                      gw_sir;
    /**< SIR applicable and valid (otherwise 0xFF) only in WCDMA
    ** connected mode when DPCCH is active.
    */

  int16                      gw_rssi2;
    /**< The -ve RSSI value, coming from RRC layer
    */

  int16                      gw_rscp;
    /**< Signal strength in RSSI, change for WCDMA only  (otherwise 0xFF)
    */

  boolean                    gw_ecio_for_display;
    /**< TRUE if UE is in a data call, else FALSE
    */

  sys_cell_info_s_type       gw_cell_info;
    /**< The CELL_INFO of the selected system.
    **  Mask used is CM_SS_CELL_INFO_MASK
    */

  #endif

  #if (defined(FEATURE_HYBR_GW) || \
       defined(FEATURE_MM_SUPERSET))
  sys_simultaneous_mode_e_type simultaneous_mode;
    /**< Simultaneous, used to differentiate between WX and non-WX modes
    ** CM_SS_SRV_IND_MASK?
    */
  #endif

  int16                      gw_ecio;
    /**< ECIO for WCDMA only
    ** Ec/Io is measured in -0.5 dB.
    ** What WCDMA L1 sends to RRC is in 0.5dB (positive value)
    ** For GSM it is defaulted to CMSS_ECIO_VALUE_INVALID_SIGNAL
    ** Clients should check for CM_API_GW_ECIO to use this field
    */


  sys_cell_info_s_type      cell_info;
    /**< The CELL_INFO of the selected system.
    **  Mask used is CM_SS_CELL_INFO_MASK
    */


  sys_hdr_location_notification_status_e_type hdr_ln_status;
    /**< HDR LN status
    */

  cm_ss_country_list_s_type   country_list;
    /* Clients need to check for CM_API_PLUS_DIALING before
    ** using this structure.
    **
    ** Structure populated when sending CM_SS_EVENT_SELECT_COUNTRY
    */

  /* cm_plus_code*/

  cm_country_code_type        current_mcc;
    /* Clients need to check for CM_API_PLUS_DIALING before
    ** using this structure. Associated with CM_SS_SRV_STATUS_MASK
    **
    ** Carries country code of current serving country.
    */


  cm_country_code_type        home_mcc;
    /* Clients need to check for CM_API_PLUS_DIALING before
    ** using this structure. Associated with CM_SS_SRV_STATUS_MASK
    **
    ** Carries home country code.
    */


  cm_hs_based_plus_dial_e_type  plus_dial_setting;
    /* Clients need to check for CM_API_PLUS_DIALING before
    ** using this structure. Associated with CM_SS_SRV_STATUS_MASK
    **
    ** Carries NV setting corresp to plus dial
    */


  cm_reg_reject_info_s_type reg_reject_info;
    /**< Registration reject information
    */

  cm_gw_gan_info_s_type gan_info;
    /**< GAN controller information  
    */

  sys_roam_status_e_type     def_roam_ind;
    /**< Default roaming indicator from PRL
    */

  cm_ss_meas_resp_s_type    meas_resp;
    /**< Indicate response to measurement request made 
    **  by connection manager. 
    */

} cm_mm_ss_info_s_type;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_mm_ss_info_s.mode_info DISC cm_mm_ss_info_s.sys_mode */
#endif


/** Orig throttle SS information
*/
typedef union cm_ss_orig_thr_ss_info_u {

  cm_ss_cdma_info_s_type   cdma_info;
    /**< CDMA SS information */

} cm_ss_orig_thr_ss_info_u_type;


/** Type to hold the origination call status information
*/
typedef struct cm_ss_orig_throttle_call_status_s
{
  cm_call_type_e_type                 call_type;
    /**< Type of the call, e.g PS_DATA
    */

  sys_sys_id_s_type                   sys_id;
    /**< The sys id of the system, SID,NID
    */

  cm_ss_orig_thr_ss_info_u_type       mode_info;
    /**< Mode specific serving system information
    */

  cm_ss_orig_thr_reasons_e_type       call_status;
    /**< The call end reason
    */
} cm_ss_orig_thr_call_status_s_type;


/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/


/** Type for serving system command callback functions.
**
** A client that wants to be notified of the result of a
** serving system-command-function needs to supply a pointer to a function
** of this type as the first parameter to a serving system-command-function.
*/
typedef void (cm_ss_cmd_cb_f_type) (

  void                    *data_block_ptr,
    /**< Pointer to client provided data block */

  cm_ss_cmd_e_type        ss_cmd,
    /**< Indicate which serving system command is this error status for */

  cm_ss_cmd_err_e_type    ss_cmd_err
    /**< Serving system command error code, indicating whether command
    ** is rejected and the reason */

);



/**===========================================================================

@FUNCTION cm_ss_cmd_get_ss_info

@DESCRIPTION
  Get a sanpshot of the current serving system information in the CM
  The client needs to register for CM_SS_EVENT_INFO in order to receive
  the response to this command. CM_SS_EVENT_INFO is sent only to the client that
  issues this command.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request


@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_ss_cmd_get_ss_info(

  cm_ss_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                 *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type    client_id
    /**< requesting client ID */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ss_cmd_get_ss_info */
#endif


/**===========================================================================
@FUNCTION cm_ss_cmd_get_measurement

@DESCRIPTION
  Get measurements results for systems specified in the payload of this
  command. The client needs to register for CM_SS_EVENT_MEAS_RESPONSE in 
  order to receive the response to this command. This event is sent to all
  clients that register for this event.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_ss_cmd_get_measurement(

  cm_ss_cmd_cb_f_type      cmd_cb_func,
    /**< client callback function */

  void                     *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type        client_id,
    /**< requesting client ID */

  cm_ss_meas_req_s_type    meas_req
    /**< meas request information */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ss_cmd_get_measurement */
#endif

/**===========================================================================

@FUNCTION cm_mm_ss_cmd_ps_data_available

@DESCRIPTION
  Inform CM that data is available for transmission.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_ss_cmd_ps_data_available(

  cm_ss_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type              client_id
    /**< requesting client ID */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_ss_cmd_ps_data_available */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

@FUNCTION cm_ss_cmd_orig_thr_update_tbl

@DESCRIPTION
  This API queues a command to CM to update the origination throttle table.
  This function is only for CDMA and HDR. Call origination is throttled with the
  help of this throttle table which this function updates.

@DEPENDENCIES
  cmss_init() should have been called previously.

@RETURNS
  True if a command was queued
  False otherwise

@SIDE_EFFECT
  None.
@x
===========================================================================*/

boolean cm_ss_cmd_orig_thr_update_tbl
(
  cm_ss_cmd_cb_f_type               *ss_cmd_cb_func,
    /**< Pointer to the call back function
    */

  void                              *data_block_ptr,
    /**< Pointer to user data
    */

  cm_client_id_type                  client,
    /**< requesting client ID
    */

  cm_ss_orig_thr_act_e_type          orig_thr_act,
    /**< The action to be taken
    */

  cm_ss_orig_thr_reasons_e_type      call_status,
    /**< The call end reason
    */

  cm_call_type_e_type                call_type,
    /**< Type of the call, e.g PS_DATA
    */

  sys_sys_mode_e_type                mode,
    /**< The mode on which the call was connected
    */

  const sys_sys_id_s_type           *sys_id,
    /**< The sys id of the system, SID,NID
    ** This field is only used if
    ** orig_thr_act = CM_SS_ORIG_THR_UPDATE_TBL_ENTRY
    */

  sys_band_class_e_type              band_class,
    /**< cellular or PCS
    ** This field is only used if
    ** orig_thr_act = CM_SS_ORIG_THR_UPDATE_TBL_ENTRY
    */

  uint16                             packet_zone
    /**< The packet zone ID
    ** This field is only used if
    ** orig_thr_act = CM_SS_ORIG_THR_UPDATE_SS
    */


);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ss_cmd_orig_thr_update_tbl */
/*~ PARAM ss_cmd_cb_func POINTER */
/*~ PARAM sys_id POINTER */
#endif

/** @}
*  endgroup SSO Serving System Object
*/

/*===========================================================================

FUNCTION cm_ss_cmd_country_selected

DESCRIPTION
  Carries the country code selected by user in response to
  CM_SS_EVENT_SELECT_COUNTRY.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern boolean cm_ss_cmd_country_selected (

  cm_ss_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  cm_client_id_type            client_id,
    /* requesting client ID */

  cm_sid_type                  sid,
    /* sid for which country is being resolved */

  cm_country_code_type         country_code
    /* code of country that has been selected */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ss_cmd_country_selected */
#endif

/****************************************************************************

Inband Object

****************************************************************************/

/** @defgroup IO Inband Object
** @{
*/

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/


#define CM_INBAND_MAX_FWD_DIALED_DIGITS 64
  /* Max length of forward DTMF digit string */

#ifdef FEATURE_JCDMA
  #define CM_INBAND_MAX_REV_DIALED_DIGITS 255  /* value of CAI_SBDTMF_MAX */
  #define CM_INBAND_MAX_DIALED_DIGITS CM_INBAND_MAX_REV_DIALED_DIGITS
#else /* FEATURE_JCDMA */
  #define CM_INBAND_MAX_REV_DIALED_DIGITS 32
  #define CM_INBAND_MAX_DIALED_DIGITS CM_INBAND_MAX_FWD_DIALED_DIGITS
#endif /* #else FEATURE_JCDMA */
  /* Max length of reverse DTMF digit string */


/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/


/** Enumeration of inband commands client are able to send to CM.
*/
typedef enum cm_inband_cmd_e {

  CM_INBAND_CMD_NONE=-1,
    /**< @internal */

  CM_INBAND_CMD_BURST_DTMF,
    /**< request to send  a burst DTMF */

  CM_INBAND_CMD_START_CONT_DTMF,
    /**< request to start a continuous DTMF tone */

  CM_INBAND_CMD_STOP_CONT_DTMF,
    /**< request to stop  a continuous DTMF tone */

  CM_INBAND_CMD_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_inband_cmd_e_type;


/** Enumeration of inband command errors returned by call command
** callback functions.
*/
typedef enum cm_inband_cmd_err_e {

  CM_INBAND_CMD_ERR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_INBAND_CMD_ERR_NOERR,
    /**< No errors found */

  /* parameter related error
  */

  CM_INBAND_CMD_ERR_CLIENT_ID_P,
    /**< invalid client ID */

  CM_INBAND_CMD_ERR_ON_LEN_P,
    /**< invalid DTMF pulse width */

  CM_INBAND_CMD_ERR_OFF_LEN_P,
    /**< invalid DTMF inter-digit interval */

  /* 4 */
  CM_INBAND_CMD_ERR_NULL_BUF_P,
    /**< null digit buffer */

  CM_INBAND_CMD_ERR_DIGIT_P,
    /**< invalid digits */

  CM_INBAND_CMD_ERR_CNT_P,
    /**< the value of cnt exceeds the maximum allowed value */

  CM_INBAND_CMD_ERR_CALL_ID_P,
    /**< invalid call ID */

  /* Error codes resulting from clients trying to instruct
  ** CM to perform inappropriate actions relative to the current
  ** state of the phone.
  */

  /* 8 */
  CM_INBAND_CMD_ERR_BAD_STATE_S,
    /**< invalid state for inband request */

  CM_INBAND_CMD_ERR_OFFLINE_S,
    /**< Phone is offline */

  /* Error codes resulting from operational limitation
  ** of CM.
  */

  CM_INBAND_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command */

  /* other error
  */

  CM_INBAND_CMD_ERR_OTHER,
    /**< A problem other than the above was found */

  /* 12 */
  CM_INBAND_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  CM_INBAND_CMD_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_inband_cmd_err_e_type;


/** Enumeration of inband events.
** Clients are able to register with CM to be notified of any subsets
** of these inband events.
*/
typedef enum cm_inband_event_e {

  CM_INBAND_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_INBAND_EVENT_REV_BURST_DTMF,
    /**< send a CDMA-burst DTMF */

  CM_INBAND_EVENT_REV_START_CONT_DTMF,
    /**< start a continuous DTMF tone */

  CM_INBAND_EVENT_REV_START_CONT_DTMF_CONF,
    /**< WCDMA/GSM only: confirm whether a continuous DTMF tone is played */

  CM_INBAND_EVENT_REV_STOP_CONT_DTMF,
    /**< stop a continuous DTMF tone */

  /* 4 */
  CM_INBAND_EVENT_REV_STOP_CONT_DTMF_CONF,
    /**< WCDMA/GSM only: confirm a continuous DTMF tone is stopped */

  CM_INBAND_EVENT_FWD_BURST_DTMF,
    /**< received a CDMA-burst DTMF message */

  CM_INBAND_EVENT_FWD_START_CONT_DTMF,
    /**< received a start-continuous DTMF tone order */

  CM_INBAND_EVENT_FWD_STOP_CONT_DTMF,
    /**< received a stop-continuous DTMF tone order */

  /* 8 */
  CM_INBAND_EVENT_REV_BURST_DTMF_ACK,
    /**< FEATURE_T53 - received L2 Ack for a CDMA-burst DTMF message */

  CM_INBAND_EVENT_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_inband_event_e_type;


/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** Type to hold inband information.
** When notified of inband events, clients are given
** a reference to such a structure.
*/
typedef struct cm_inband_info_s {

  cm_client_id_type       client_id;
    /**< request client ID for reverse DTMF requests */

  cm_call_id_type         call_id;
    /**< call ID */

  boolean                 success;
  /**< whether a continuous DTMF is played (GSM/WCDMA)*/

  uint8                   on_length;
    /**< DTMF pulse width */

  uint8                   off_length;
    /**< DTMF inter-digit interval */

  uint8                   cnt;
    /**< digit count */

  uint8                   digits[CM_INBAND_MAX_DIALED_DIGITS];
    /**< DTMF digit buffer */

  uint16                  duration;
    /**< duration for continuous DTMF; in milliseconds
    ** for reverse stop continuous DTMF event only */

  cm_client_id_type       requesting_client_id;
    /**< Requesting client */

} cm_inband_info_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_inband_info_s.digits VARRAY CM_INBAND_MAX_DIALED_DIGITS
    LENGTH cm_inband_info_s.cnt */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/


/** Type for inband command callback functions.
**
** A client that wants to be notified of the result of a
** inband-command-function needs to supply a pointer to a function
** of this type as the first parameter to a inband-command-function.
*/
typedef void (cm_inband_cmd_cb_f_type) (

  void                        *data_block_ptr,
    /**< Pointer to client provided data block */

  cm_inband_cmd_e_type        inband_cmd,
    /**< Indicate which inband command is this error status for */

  cm_inband_cmd_err_e_type    inband_cmd_err
    /* Inband command error code, indicating whether command
    ** is rejected and the reason */

);



/**===========================================================================

@FUNCTION cm_mm_inband_cmd_burst_dtmf

@DESCRIPTION
  Command CM to send a burst DTMF

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_CDMA_800 || FEATURE_CDMA_1900 must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_inband_cmd_burst_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Requesting client */

  cm_call_id_type             call_id,
    /**< call ID of the call */

  uint8                       on_length,
    /**< DTMF pulse width */

  uint8                       off_length,
    /**< DTMF inter-digit interval */

  uint8                       cnt,
    /**< DTMF digit count */

  const uint8                 *dtmf_digit_ptr
    /**< Pointer to the DTMF digit buffer */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_inband_cmd_burst_dtmf */
/*~ PARAM dtmf_digit_ptr VARRAY LENGTH cnt */
#endif


/**===========================================================================

@FUNCTION cm_mm_inband_cmd_start_cont_dtmf

@DESCRIPTION
  Command CM to start a continuous DTMF

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  CM must have already been initialized with

  cm_init_before_task_start() and cm_init_after_task_start()
@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_inband_cmd_start_cont_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Requesting client */

  cm_call_id_type             call_id,
    /**< call ID of the call */

  uint8                       digit
    /**< DTMF digit */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_inband_cmd_start_cont_dtmf */
#endif


/**===========================================================================

@FUNCTION cm_mm_inband_cmd_stop_cont_dtmf

@DESCRIPTION
  Command CM to stop a continuous DTMF

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_inband_cmd_stop_cont_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Requesting client */

  cm_call_id_type             call_id,
    /**< call ID of the call */

  uint16                      duration
    /**< DTMF duration */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_inband_cmd_stop_cont_dtmf */
#endif


/** @}
*  endgroup IO Inband Object
*/


/****************************************************************************

Short Messaging Service Object
** For internal use (uasms) only **

****************************************************************************/

/** @defgroup SMSO Short Messaging Service Object
** @{
*/


/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

typedef enum cm_sms_cmd_e {

  CM_SMS_CMD_NONE=-1,
    /**< @internal */

  CM_SMS_CMD_MO_MSG,
    /**< FEATURE_CDSMS - MO SMS */

  CM_SMS_CMD_AWI_ACK,
    /**< FEATURE_CDSMS - Analog Alert with info ack */

  CM_SMS_CMD_BC_ENABLE,
    /**< FEATURE_BROADCAST_SMS */

  CM_SMS_CMD_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_sms_cmd_e_type;

/** Enumeration of type of NAS command
** Clients should check for CM_API_REMOTE_WMS
** before using this enum.
*/
typedef enum cm_nas_cmd_e {

  CM_NAS_CMD_NONE=-1,
    /**< @internal */

  CM_NAS_CMD_REQ,
    /**< FEATURE_GSM || FEATURE_WCDMA */

  CM_NAS_CMD_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_nas_cmd_e_type;


/** Enumeration of SMS command errors returned by SMS command
** callback functions.
*/
typedef enum cm_sms_cmd_err_e {

  CM_SMS_CMD_ERR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_SMS_CMD_ERR_NOERR,
    /**< FEATURE_CDSMS - No errors found */

  /* Error codes resulting from clients feeding a serving system command
  ** function with bad or inappropriate parameters.
  */

  CM_SMS_CMD_ERR_CLIENT_ID_P,
    /**< FEATURE_CDSMS - Bad client parameter */

  CM_SMS_CMD_ERR_NOT_IDLE_S,
    /**< FEATURE_CDSMS - Can not accept SMS origination while SMS state
    ** is not IDLE */

  CM_SMS_CMD_ERR_OFFLINE_S,
    /**< FEATURE_CDSMS - Phone is offline */

  /* 4 */
  CM_SMS_CMD_ERR_CDMA_LOCK_S,
    /**< FEATURE_CDSMS - Phone is CDMA locked */

  CM_SMS_CMD_ERR_CALL_ORIG_S,
    /**< FEATURE_CDSMS - Call origination is in progress */

  CM_SMS_CMD_ERR_EMERGENCY_MODE_S,
    /**< FEATURE_CDSMS - phone is in EMERGENCY callback mode only EMERGENCY
    ** call is allowed  */

  CM_SMS_CMD_ERR_NO_SRV_S,
    /**< FEATURE_CDSMS - Client tried to send MO SMS but there is no service */

  /* 8 */
  CM_SMS_CMD_ERR_NOT_ALLOWED_IN_AMPS_S,
    /**< FEATURE_CDSMS - Client tried to send MO SMS but the srv state
    ** is analog */

  CM_SMS_CMD_ERR_NOT_ANALOG_S,
    /**< FEATURE_CDSMS - Client tried to send AWI ACK but the srv state
    ** is not analog */

  /* Error codes resulting from operational limitation of CM.
  */

  CM_SMS_CMD_ERR_NO_BUF_L,
    /**< FEATURE_CDSMS - No available CM command buffers to queue the
    ** command */

  CM_SMS_CMD_ERR_OTHER,
    /**< FEATURE_CDSMS - any other errors not listed above */

  /* 12 */
  CM_SMS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  CM_SMS_CMD_ERR_DDTM,
    /**< SMS blocked due to high psist on 1X and DO call in progress */

  CM_SMS_CMD_ERR_MODE_PREF,
    /**< Wrong mode preference over which SMS is sent
    ** Can be used only if CM_API_SMS_OVER_IP is defined
    */

  CM_SMS_CMD_ERR_SIP,
    /**< Wrong SIP status when SMS is sent over SIP.
    ** Can be used only if CM_API_SMS_OVER_IP is defined
    */

  CM_SMS_CMD_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_sms_cmd_err_e_type;

/** Enumeration of NAS command errors returned by NAS command
** callback functions.
** Clients need to check for CM_API_REMOTE_WMS before using
** this enum.
*/
typedef enum cm_nas_cmd_err_e {

  CM_NAS_CMD_ERR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_CMD_ERR_NOERR,

  /* Error codes resulting from clients feeding a serving system command
  ** function with bad or inappropriate parameters.
  */

  CM_NAS_CMD_ERR_CLIENT_ID_P,
    /**< FEATURE_CDSMS - Bad client parameter */

  CM_NAS_CMD_ERR_NULL_PARAM_P,
    /**< Null pointer parameter */

  /* Error codes resulting from operational limitation of CM.
  */

  CM_NAS_CMD_ERR_NO_BUF_L,

  /* 4 */
  CM_NAS_CMD_ERR_OTHER,

  CM_NAS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  CM_NAS_CMD_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_nas_cmd_err_e_type;

/** Enumeration of SMS events.
** Clients are able to register with CM to be notified of any subsets
** of these events.
*/
typedef enum cm_sms_event_e {

  CM_SMS_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_SMS_EVENT_MO_MSG,
    /**< FEATURE_CDSMS */

  CM_SMS_EVENT_AWI_ACK,
    /**< FEATURE_CDSMS */

  CM_SMS_EVENT_MO_STATUS,
    /**< FEATURE_CDSMS */

  CM_SMS_EVENT_MT_MSG,
    /**< FEATURE_CDSMS */

  /* 4 */
  CM_SMS_EVENT_BC_ENABLED,
    /**< FEATURE_BROADCAST_SMS */

  CM_SMS_EVENT_MSG_WAITING,
    /**< FEATURE_CDSMS */

  CM_SMS_EVENT_N_VOICE,
    /**< FEATURE_CDSMS */

  CM_SMS_EVENT_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_sms_event_e_type;


/** Now the possible statuses, as returned by MC. Since SMS uses
**  DBMs, these must map to the DBM enum, defined above. FEATURE_CDSMS only.
*/
typedef enum cm_sms_mo_status_e {

  CM_SMS_MO_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_SMS_MO_OK=CM_DBM_MO_OK,
    /**< SMS msg has been completed successfully */

  CM_SMS_MO_HOLD_ORIG_RETRY_TIMEOUT=CM_DBM_MO_HOLD_ORIG_RETRY_TIMEOUT,
    /**< HOLD ORIG retry timeout*/

  CM_SMS_MO_HOLD_ORIG=CM_DBM_MO_HOLD_ORIG,
    /**< MC can not process MO SMS because HOLD ORIG is TRUE*/

  CM_SMS_MO_NO_SRV=CM_DBM_MO_NO_SRV,
    /**< can not send SMS msg  - no service */

  /* 4 */
  CM_SMS_MO_ABORT=CM_DBM_MO_ABORT,
    /**< MC or CM aborts the SMS request */

  CM_SMS_MO_NOT_ALLOWED_IN_AMPS=CM_DBM_MO_NOT_ALLOWED_IN_AMPS,
    /**< can not sent SMS msg in analog mode */

  CM_SMS_MO_NOT_ALLOWED_IN_HDR=CM_DBM_MO_NOT_ALLOWED_IN_HDR,
    /**< can not sent SMS msg while a HDR call is up*/

  CM_SMS_MO_L2_ACK_FAILURE=CM_DBM_MO_L2_ACK_FAILURE,
    /**< not receiving L2 ack from base station */

  /* 8 */
  CM_SMS_MO_OUT_OF_RESOURCES=CM_DBM_MO_OUT_OF_RESOURCES,
    /**< e.g. out of memory buffer */

  CM_SMS_MO_ACCESS_TOO_LARGE=CM_DBM_MO_ACCESS_TOO_LARGE,
    /**< message is too large to be sent over access */

  CM_SMS_MO_DTC_TOO_LARGE=CM_DBM_MO_DTC_TOO_LARGE,
    /**< message is too large to be sent over DTC */

  CM_SMS_MO_ACCT_BLOCK = CM_DBM_MO_ACCT_BLOCK,
    /**< Access Control based on Call Type(service option)  is blocked */

  /* 12 */
  CM_SMS_MO_SIP_PERMANENT_FAILURE,
    /* The permanent SIP transport failure for MO SMS.
    ** i.e. refer to 4xx 5xx 6xx SIP error codes.
    ** Can be used only if CM_API_SMS_OVER_IP is defined
    */

  CM_SMS_MO_SIP_TEMPORARY_FAILURE,
    /* The temporary SIP transport failure for MO SMS.
    ** i.e. SIP layer time out.
    ** Can be used only if CM_API_SMS_OVER_IP is defined
    */

  CM_SMS_MO_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_sms_mo_status_e_type;


/** Enumeration of SMS states. At all times a CM SMS must be in one
** of the states below.
*/
typedef enum cm_sms_state_e {

  CM_SMS_STATE_NONE=-1,
    /**< @internal */

  CM_SMS_STATE_IDLE,
    /**< SMS is in idle state */

  CM_SMS_STATE_HOLD_ORIG_RETRY,
    /**< SMS is checking Hold Orig status periodically to determine
    ** whether MO SMS can be sent to MC */

  CM_SMS_STATE_WAIT_FOR_STATUS,
    /**< SMS is waiting for MO SMS status from MC */

  CM_SMS_STATE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_sms_state_e_type;

/** Enum of cell change types from NAS to WMS
** Clients should check for CM_API_NAS_CELL_CHANGE_TYPE
** before using this enum.
*/
typedef enum cm_nas_cell_change_type_e
{
  CM_NAS_CELL_CHANGE_NONE=-1,
    /**< FOR INTERNAL CM USE ONLY! */

  CM_NAS_CELL_CHANGE_NO_SERVICE,
   /**< No service */

  CM_NAS_CELL_CHANGE_SAME_CELL,
    /**< Same cell. No change */

  CM_NAS_CELL_CHANGE_CELL_CHANGE,
    /**< Cell has changed */

  CM_NAS_CELL_CHANGE_LAC_CHANGE,
    /**< LAC has changed */

  CM_NAS_CELL_CHANGE_PLMN_CHANGE,
    /**< PLMN has changed */

  CM_NAS_CELL_CHANGE_MAX
    /**< FOR INTERNAL CM USE ONLY! */

} cm_nas_cell_change_type_e_type;

/** Enumeration of NAS events that a client can register for.
** Clients need to check for CM_API_REMOTE_WMS before using
** this enum.
*/
typedef enum cm_nas_event_e
{

  CM_NAS_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_EVENT_SM_DATA_IND,
    /**< Data Indication from SM */

  CM_NAS_EVENT_SM_REPORT_IND,
    /**< Report Indication from SM */

  CM_NAS_EVENT_CB_DATA_IND,
    /**< Data Indication from CB */

  CM_NAS_EVENT_CB_CELL_CHANGE_IND,
    /**< Cell Change Indication from CB */

  /* 4 */
  CM_NAS_EVENT_CB_DUPLICATION_DETECTION_IND,
    /**< Duplicate Detection Indication from CB */

  CM_NAS_EVENT_SM_LINK_CONTROL_IND,
    /**< Link Control Indication from SM */

  CM_NAS_EVENT_MAX
   /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_nas_event_e_type;

/** Enumeration of NAS MN Link Control modes
*/
typedef enum cm_nas_mn_link_control_mode_e
{

  CM_NAS_MN_LINK_CONTROL_MODE_NONE=-1,
    /**< @internal */

  CM_NAS_MN_LINK_CONTROL_MODE_DISABLED,
    /**< MN Link control disabled */

  CM_NAS_MN_LINK_CONTROL_MODE_ENABLED_ONE,
    /**< MN Link control one enabled */

  CM_NAS_MN_LINK_CONTROL_MODE_ENABLED_ALL,
    /**< MN Link control all enabled */

  CM_NAS_MN_LINK_CONTROL_MODE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

}  cm_nas_mn_link_control_mode_e_type;

/** Enumeration of NAS Errors
*/
typedef enum cm_nas_error_e
{

  CM_NAS_ERROR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_NO_ERROR_RP_ACK,
    /**< No Error RP Ack */

  CM_NAS_NO_ERROR_ABORT_SM_COMPLETE,
    /**< No Error on Abort */

  CM_NAS_NO_ERROR_SMMA_ABORT_SENT,
    /**< No Error Abort Sent */

  CM_NAS_SMR_ERROR_NO_SM_RESOURCES,
    /**< Error No SM Resourcees */

  /* 4 */
  CM_NAS_SMR_ERROR_TR1M_TIMEOUT,
    /**< Error TR1M timeout */

  CM_NAS_SMR_ERROR_TR2M_TIMEOUT,
    /**< Error TR2M timeout */

  CM_NAS_SMS_RP_ERROR,
    /**< Error SMS RP */

  CM_NAS_SMS_CP_ERROR,
    /**< Error SMS CP */

  /* 8 */
  CM_NAS_SMS_LL_ERROR,
    /**< Error SMS LL */

  CM_NAS_SMS_NO_RESPONSE_FROM_NETWORK,
    /**< No response from Network */

  CM_NAS_NO_ERROR_CP_ACK,
    /**< ACK from NAS to WMS */

  CM_NAS_SMS_LL_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_nas_error_e_type;

/** Enumeration of NAS MN Link Control Status
*/
typedef enum cm_nas_mn_link_control_status_e
{

  CM_NAS_MN_LINK_CONTROL_STATUS_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_MN_LINK_CONTROL_STATUS_INACTIVE,
    /**< MN Link Control Inactive */

  CM_NAS_MN_LINK_CONTROL_STATUS_ACTIVE,
    /**< MN Link Control Active */

  CM_NAS_MN_LINK_CONTROL_STATUS_TIMEOUT,
    /**< MN Link Control Timed Out */

  CM_NAS_MN_LINK_CONTROL_STATUS_CONNECTION_RELEASED,
    /**< MN Link Control Released */

  /* 4 */
  CM_NAS_MN_LINK_CONTROL_STATUS_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_nas_mn_link_control_status_e_type;


/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** Structure of SM Data Indication Information returned from a
*  NAS event. Clients need to check for CM_API_REMOTE_WMS before
*  using this struct.
*/
typedef struct cm_nas_sm_rl_data_ind_s
{

  uint8                               connection_id;
    /**< Connection Id */

  cm_called_party_bcd_no_s_type       RP_OA;
    /**< RP OA */

  uint8                               tpdu_length;
    /**< Length of tpdu buffer */

  uint8                               tpdu[ CM_RP_USER_DATA_TPDU_LENGTH ];
    /**< tpdu */

} cm_nas_sm_rl_data_ind_s_type;

/** Structure of SM RL report indication returned from a NAS event.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_sm_rl_report_ind_s
{

  uint16                               short_message_identifier;
    /**< Short Message Identifier */

  cm_nas_error_e_type                  error_status;
    /**< Error Status */

  uint8                                cause_value;
    /**< Cause Value */

  boolean                              diagnostics_present;
    /**< Diagnostics present */

  uint8                                diagnostics;
    /**< Diagnostics */

  uint8                                tpdu_length;
    /**< Length of tpdu buffer */

  uint8                                tpdu[ CM_RP_USER_DATA_TPDU_LENGTH ];
    /**< tpdu */

  boolean                              permanent_error;
    /**< Indicates any access class restriction or sim invalidated information*/

  sys_srv_domain_e_type                sms_srv_domain;
    /**< Domain in which SMS is tried */


} cm_nas_sm_rl_report_ind_s_type;

/** Structure of SM RL Link Control report returned from a NAS event.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_sm_rl_link_control_report_s
{

  cm_nas_mn_link_control_mode_e_type                  mode;
    /**< Link Control Mode */

  cm_nas_mn_link_control_status_e_type                status;
    /**< Link Control Status */

} cm_nas_sm_rl_link_control_report_s_type;

/** Structure of CB data indication returned from a NAS event.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_cb_data_ind_s
{

  uint8                               page_length;
    /**< Page Length */

  uint8                               cb_page[ CM_NAS_CB_DATA_PAGE_SIZE ];
    /**< CB page */

} cm_nas_cb_data_ind_s_type;

/** Structure of CB duplication detection returned from a NAS event.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_cb_duplication_detection_ind_s
{

  uint16                            message_id;
    /**< Message Identifier */

  uint16                            message_code;
    /**< Message Code */

  uint8                             gs_indicator;
    /**< Geographic scope */

  uint8                             update_number;
    /**< Update Number */

  uint8                             page_number;
    /**< Page Number */

  uint8                             number_of_pages;
    /**< Total number of pages */

  uint8                             data_coding_scheme;
    /**< Data coding Scheme */


} cm_nas_cb_duplication_detection_ind_s_type;

/** Analog alert with info ack message
*/
typedef struct cm_sms_awi_ack_s {

  boolean                 bf;
    /**< begin/final flag */

  uint8                   seq_no;
    /**< sequence number */

  uint8                   status;
    /**< status */

  uint8                   error_class;
    /**< error class */

} cm_sms_awi_ack_s_type;

/** Structure of information returned from a NAS event.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_info_s {

  cm_client_id_type                          nas_client_id;
    /**< Client Id */

  cm_nas_sm_rl_data_ind_s_type               rl_data_ind;
    /**< SM Data Indication Information */

  cm_nas_sm_rl_report_ind_s_type             rl_report_ind;
    /**< SM Report Indication Information */

  cm_nas_sm_rl_link_control_report_s_type    rl_link_control_report_ind;
    /**< SM Link Control Report Information */

  cm_nas_cb_data_ind_s_type                  cb_data_ind;
    /**< CB Data Indication Information */

  cm_nas_cb_duplication_detection_ind_s_type cb_duplicate_ind;
    /**< CB Duplication Detection Information */

  cm_client_id_type                          requesting_client_id;
    /**< Requesting client */

  cm_nas_cell_change_type_e_type             cell_change_type;
  /**< Cell Change Type
  ** Clients should check for CM_API_NAS_CELL_CHANGE_TYPE before using this
  ** field.
  */

} cm_nas_info_s_type;


/** Type to hold sms information.
** When notified of sms events, clients are given
** a reference to such a structure.
** FEATURE_CDSMS is removed in the data structure
** when CM_API_SMS_OVER_IP is defined.
*/
typedef struct cm_sms_info_s {

  cm_client_id_type       sms_client_id;
    /**< requesting client - MO_SMS/AWI_ACK */

  cm_sms_state_e_type     sms_state;
    /**< SMS state for MO SMS */

  cm_sms_awi_ack_s_type   awi_ack;
    /**< field for awi ack */

  uint8                   mt_sms_format;
    /**< message format for MT SMS */

  uint16                  length;
    /**< length for SMS buffer - used by MT SMS only*/

  uint8                   *data;
    /**< pointer to MT SMS buffer */

  sys_sys_mode_e_type     sys_mode;
    /**< system mode over which SMS is received.
    ** can not used when CM_API_SMS_OVER_IP is defined.
    */

  boolean                 l2_ack_requested;
    /**< l2 ack required or not - used by MO SMS*/

  cm_sms_mo_status_e_type mo_status;
    /**< status for MO SMS */

  boolean                 bc_enable;
    /**< Enable broadcast SMS - FEATURE_BROADCAST_SMS */

  cm_client_id_type       requesting_client_id;
    /**< Requesting client */

} cm_sms_info_s_type;
// TODO - callbacks must copy data array instead of pointer
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_sms_info_s.data VARRAY LENGTH cm_sms_info_s.length */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/** Type to describe a CB message id range.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_cb_message_id_value {

  uint16                          from;
    /**< from message id */

  uint16                          to;
    /**< to message id */

} cm_nas_cb_message_id_value_type;

/** Type to hold a CB search list request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_cb_search_list_req {

  byte                             search_list_length;
    /**< length of the search list */

  cm_nas_cb_message_id_value_type  search_list[ CM_MAX_SEARCH_LIST_LENGTH ];
    /**< pointer to the search list */

  #ifdef FEATURE_UMTS_BMC
#error code not present
  #endif /* #ifdef FEATURE_UMTS_BMC */

} cm_nas_cb_search_list_req_type;

/** Type to hold a CB delete message request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_cb_delete_msg_req {

  uint16              message_id;
    /**< message id */

  uint16              message_code;
    /**< message code */

  uint8               page_number;
    /**< page number */

} cm_nas_cb_delete_msg_req_type;


/** Enumeration of MM Bearer Selections.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef enum cm_nas_mm_bearer_selection_e
{
  CM_NAS_MM_BEARER_SELECTION_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_MM_BEARER_SELECTION_CS_PREFERRED,
    /**< Circuit Switch Preferred */

  CM_NAS_MM_BEARER_SELECTION_PS_PREFERRED,
    /**< Packete Switch Preferred */

  CM_NAS_MM_BEARER_SELECTION_CS_ONLY,
    /**< Circuit Switch Only */

  CM_NAS_MM_BEARER_SELECTION_PS_ONLY,
    /**< Packet Switch Only */

  /* 4 */
  CM_NAS_MM_BEARER_SELECTION_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_nas_mm_bearer_selection_e_type;

/** Type to hold a MN RL Memory Available Request
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_mn_rl_memory_available_req
{

  uint16       short_message_identifier;
    /**< Short Message identifier */

  boolean      abort;
    /**< Whether to abort or not */

} cm_nas_mn_rl_memory_available_req_type;

/** Type to hold a MN RL Report Request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_mn_rl_report_req
{

  uint8         connection_id;
    /**< Connection Id */

  boolean       sm_rl_success;
    /**< Status */

  uint8         cause_value;
    /**< Cause Value */

  boolean       diagnostics_present;
    /**< Diagnostics Present */

  uint8         diagnostics;
    /**< Diagnostics */

  uint8         tpdu_length;
    /**< Length of TPDU */

  uint8         tpdu[ CM_NAS_MN_TPDU_SIZE ];
    /**< TPDU */

} cm_nas_mn_rl_report_req_type;

/** Type to hold a MN RL Data Request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_mn_rl_data_req
{

  uint8                            short_message_identifier;
    /**< Short Message Identifier */

  cm_called_party_bcd_no_s_type    RP_DA;
    /**< RP DA */

  uint8                            tpdu_length;
    /**< Length of TPDU */

  uint8                            tpdu[ CM_NAS_MN_TPDU_SIZE ];
    /**< TPDU */

} cm_nas_mn_rl_data_req_type;

/** Type to hold a MN RL Link Control Request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_mn_rl_link_control_req
{

  uint8                               timeout_in_seconds;
    /**< Timeout */

  cm_nas_mn_link_control_mode_e_type  mode;
    /**< Mode of Link Control */

} cm_nas_mn_rl_link_control_req_type;

/** Enumeration of NAS Request Types.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef enum cm_nas_req_e
{

  CM_NAS_REQ_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_REQ_MN_RL_REPORT,
    /**< MN RL Report */

  CM_NAS_REQ_MN_RL_DATA,
    /**< MN RL Data */

  CM_NAS_REQ_MN_RL_MEMORY_AVAILABLE,
    /**< MN Memory Available Req */

  CM_NAS_REQ_MN_RL_LINK_CONTROL,
    /**< MN Link Control Req */

  /* 4 */
  CM_NAS_REQ_MM_BEARER_SELECTION,
    /**< MM Bearer Selection Req */

  CM_NAS_REQ_CB_SEARCH_LIST,
    /**< CB Search List Req */

  CM_NAS_REQ_CB_DELETE_MESSAGE,
    /**< CB Delete message Req */

  CM_NAS_REQ_CB_DELETE_ALL_MESSAGES,
    /* CB Delete all messages Req */

  CM_NAS_REQ_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_nas_req_e_type;

/** Type to hold the NAS request parameters.
** Clients need to check for CM_API_REMOTE_WMS before using this
** union.
*/
typedef union cm_nas_req_param_u
{
  cm_nas_mn_rl_report_req_type               rl_report_req;
    /**< MN Report Req */

  cm_nas_mn_rl_data_req_type                 rl_data_req;
    /**< MN Data Req */

  cm_nas_mn_rl_memory_available_req_type     memory_available_req;
    /**< MN Memory Available Req */

  cm_nas_mn_rl_link_control_req_type         rl_link_control_req;
    /**< MN Link Control Req */

  cm_nas_mm_bearer_selection_e_type          bearer_selection_req;
    /**< MM Bearer Selection Req */

  cm_nas_cb_search_list_req_type             search_list_req;
    /**< CB Search List Req */

  cm_nas_cb_delete_msg_req_type              delete_msg_req;
    /**< CB Delete Msg Req */

} cm_nas_req_param_u_type;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CASE CM_NAS_REQ_NONE  cm_nas_req_param_u.void */
/*~ CASE CM_NAS_REQ_MN_RL_REPORT cm_nas_req_param_u.rl_report_req */
/*~ CASE CM_NAS_REQ_MN_RL_DATA cm_nas_req_param_u.rl_data_req */
/*~ CASE CM_NAS_REQ_MN_RL_MEMORY_AVAILABLE  cm_nas_req_param_u.memory_available_req */
/*~ CASE CM_NAS_REQ_MN_RL_LINK_CONTROL cm_nas_req_param_u.rl_link_control_req */
/*~ CASE CM_NAS_REQ_MM_BEARER_SELECTION cm_nas_req_param_u.bearer_selection_req */
/*~ CASE CM_NAS_REQ_CB_SEARCH_LIST cm_nas_req_param_u.search_list_req */
/*~ CASE CM_NAS_REQ_CB_DELETE_MESSAGE cm_nas_req_param_u.delete_msg_req */
#endif


/** Type to hold a NAS Request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_req
{

  cm_nas_req_e_type                   req;
    /**< Type of Req */

  cm_nas_req_param_u_type             param;
    /**< Parameters according to type of req */

} cm_nas_req_type;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_nas_req.param DISC cm_nas_req.req */
#endif

/** Type for TS data formats
** Clients need to check for CM_API_REMOTE_WMS before using this
** enum.
*/
typedef enum cm_format_e
{
  CM_SMS_FORMAT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_SMS_FORMAT_CDMA,
    /**< IS-95 */

  CM_SMS_FORMAT_ANALOG_CLI,
    /**< IS-91 */

  CM_SMS_FORMAT_ANALOG_VOICE_MAIL,
    /**< IS-91 */

  CM_SMS_FORMAT_ANALOG_SMS,
    /**< IS-91 */

  /* 4 */
  CM_SMS_FORMAT_ANALOG_AWISMS,
    /**< IS-95 Alert With Information SMS */

  CM_SMS_FORMAT_MWI,
    /**< Message Waiting Indication as voice mail */

  CM_SMS_FORMAT_GW_PP,
    /**< GW Point-to-Point SMS */

  CM_SMS_FORMAT_GW_CB,
    /**< GW CB SMS */

  CM_SMS_FORMAT_MAX
   /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_format_e_type;

/** Type for Number Mode
** Clients need to check for CM_API_REMOTE_WMS before using this
** enum.
*/
typedef enum cm_num_mode_e
{

  CM_NUM_MODE_NONE = -1,
    /**< @internal */

  CM_NUM_MODE_NONE_DATA_NETWORK,
    /**< No Data Network */

  CM_NUM_MODE_DATA_NETWORK,
    /**< Data Network */

  CM_NUM_MODE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif

} cm_num_mode_e_type;

/** Type for SMS address
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_address_s
{

  cm_digit_mode_e_type          digit_mode;
    /**< Indicates 4-bit or 8-bit
    */

  cm_num_mode_e_type            number_mode;
    /**< Used in CDMA only: only meaningful when digit_mode is 8-bit
    */

  cm_num_type_e_type            number_type;
    /**< In case of CDMA address, this is used only when digit_mode is 8-bit.
    ** To specify an international address for CDMA, use the following:
    **   digit_mode  = 8-bit
    **   number_mode = NONE_DATA_NETWORK
    **   number_type = INTERNATIONAL
    **   number_plan = TELEPHONY
    **   number_of_digits = number of digits
    **   digits = ASCII digits, e.g. '1', '2', '3', '4' and '5'.
    */

  cm_num_plan_e_type            number_plan;
    /**< In case of CDMA address, this is used only when digit_mode is 8-bit.
    */

  uint8                         number_of_digits;

  uint8                         digits[ CM_SMS_ADDRESS_MAX ];
    /**< Each byte in this array represents a 4-bit or 8-bit digit of
    ** address data.
    */

} cm_address_s_type;

/** Type for Over-the-air (raw) message structure
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_OTA_message
{

  cm_format_e_type                format;

  uint16                          data_len;

  uint8                           data[ CM_SMS_MAX_LEN ];

} cm_OTA_message_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM data VARRAY LENGTH data_len */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/** Type for sending SMS
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_sms_msg
{

  cm_OTA_message_type             ota;
    /**< The raw SMS data to be sent */

  cm_address_s_type               address;
    /**< SMS destination addr for the auth. */

} cm_sms_msg_type;


/** Type for received SMS
** Clients need to check for CM_API_MT_SMS before using this
** struct.
** Can be used only if CM_API_SMS_OVER_IP is defined
*/
typedef struct cm_mt_sms_msg
{

  cm_OTA_message_type             sms;
    /**< The raw SMS data to be received
    */

  sys_sys_mode_e_type             sys_mode;
    /**< The sys mode over which the sms is received.
    */

} cm_mt_sms_msg_type;


/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/


/** Type for SMS command callback functions.
**
** A client that wants to be notified of the result of a
** sms-command-function needs to supply a pointer to a function
** of this type as the first parameter to a inband-command-function.
*/
typedef void (cm_sms_cmd_cb_f_type) (

  void                        *data_block_ptr,
    /**< Pointer to client provided data block */

  cm_sms_cmd_e_type           sms_cmd,
    /**< Indicate which sms command is this error status for */

  cm_sms_cmd_err_e_type       sms_cmd_err
    /* SMS command error code, indicating whether command
    ** is rejected and the reason */

);

/** Type for NAS command callback functions.
**
** A client that wants to be notified of the result of a
** nas-command-function needs to supply a pointer to a function
** of this type as the first parameter to a inband-command-function.
*/
typedef void (cm_nas_cmd_cb_f_type) (

  void                        *data_block_ptr,
    /**< Pointer to client provided data block */

  cm_nas_cmd_e_type           nas_cmd,
    /**< Indicate which sms command is this error status for */

  cm_nas_cmd_err_e_type       nas_cmd_err
    /* NAS command error code, indicating whether command
    ** is rejected and the reason */
);



/**===========================================================================

@FUNCTION cm_sms_cmd_mo_msg

@DESCRIPTION

     Send a mobile originated SMS message

     Applicable for FEATURE_UASMS

@DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()
  FEATURE_CDSMS || FEATURE_UASMS must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request


@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_sms_cmd_mo_msg(

  cm_sms_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                    *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type       client_id,
    /**< requesting client ID */

  boolean                 l2_ack_requested,
    /**< l2 ack is required or not */

  const uint8             *data
    /**< pointer to SMS buffer */

);
/* TODO~ FUNCTION cm_sms_cmd_mo_msg */


/* Can only be used when CM_API_SMS_OVER_IP is defined.
*/
/**===========================================================================

@FUNCTION cm_sms_cmd_mo_msg2

@DESCRIPTION

     Send a mobile originated SMS message

@DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURN VALUE

  TRUE - request sent, FALSE - No buffer to send request

@SIDE EFFECTS

  none

===========================================================================*/
extern boolean cm_sms_cmd_mo_msg2(

    cm_sms_cmd_cb_f_type    cmd_cb_func,
        /**< client callback function */

    void                    *data_block_ptr,
        /**< pointer to client callback data block */

    cm_client_id_type       client,
        /**< requesting client ID */

    boolean                 l2_ack_requested,
        /**< l2 ack is required or not */

    const cm_sms_msg_type   *data,
        /**< pointer to SMS buffer */

    cm_mode_pref_e_type     mode_pref
        /**< The mode over which the SMS is sent */

);/* cm_sms_cmd_mo_msg2 */


/**===========================================================================

@FUNCTION cm_sms_cmd_awi_ack

@DESCRIPTION

  Send analog alert with info ack message to base station

  Applicable for FEATURE_UASMS

@DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()
  FEATURE_CDSMS || FEATURE_UASMS must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request


@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_sms_cmd_awi_ack(

  cm_sms_cmd_cb_f_type   cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< requesting client ID */

  uint8                  seq_no,
    /**< sequence number */

  boolean                bf,
    /**< begin/final flag */

  uint8                  error_class,
    /**< error class */

  uint8                  status
    /**< ack status */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sms_cmd_awi_ack */
#endif


/**===========================================================================

@FUNCTION cm_sms_cmd_bc_enable

@DESCRIPTION

  Enable/disable broadcast SMS

  Applicable for FEATURE_BROADCAST_SMS

@DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()
  FEATURE_BROADCAST_SMS && FEATURE_CDSMS must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request


@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_sms_cmd_bc_enable(

  cm_sms_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                  *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type     client_id,
    /**< requesting client ID */

  boolean               bc_enable
    /**< enable broadcast message */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sms_cmd_bc_enable */
#endif

/**===========================================================================

@FUNCTION cm_nas_cmd_req

@DESCRIPTION
  Send a command to NAS.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_nas_cmd_req(

  cm_nas_cmd_cb_f_type                      cmd_cb_func,
    /**< client callback function */

  void                                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                         client_id,
    /**< requesting client ID */

  cm_nas_req_type                           nas_req
    /**< request to send to NAS and parameters */

);


/** @}
*  endgroup SMSO Short Messaging Service Object
*/


/****************************************************************************

Position Determination Object
** For internal use (uapdms) only **

****************************************************************************/


/** @defgroup PDO Position Determination Object
** @{
*/


/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/** Possible PD commands
*/
typedef enum cm_pd_cmd_e {

  CM_PD_CMD_NONE = -1,
    /**< @internal */

  /* 0 */
  CM_PD_CMD_SEND_MSG,
    /**< Send data burst message */

  CM_PD_CMD_SESSION_START,
    /**< Start Session Command */

  CM_PD_CMD_SESSION_END,
    /**< End Session Command */

  CM_PD_CMD_FIX_START,
    /**< Block HDR data call origination to start GPS search */

  /* 4 */
  CM_PD_CMD_FIX_END,
    /**< Unblock HDR data call origination */

  CM_PD_CMD_SEND_REVA,
    /**< This command to be used to send Release A message */

  CM_PD_CMD_SESSION_DORMANT,
    /**< The dormancy status of the session changed */

  CM_PD_CMD_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_pd_cmd_e_type;


/** Possible PD errors
*/
typedef enum cm_pd_cmd_err_e {

  CM_PD_CMD_ERR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PD_CMD_ERR_NOERR,
    /**< No errors found */

  /* Error codes resulting from clients feeding a serving system command
  ** function with bad or inappropriate parameters.
  */

  CM_PD_CMD_ERR_CLIENT_ID_P,
    /**< Bad client parameter */

  CM_PD_CMD_ERR_NOT_IDLE_S,
    /**< Can not accept SMS origination while SMS state is not IDLE */

  CM_PD_CMD_ERR_OFFLINE_S,
    /**< Phone is offline */

  /* 4 */
  CM_PD_CMD_ERR_CDMA_LOCK_S,
    /**< Phone is CDMA locked */

  CM_PD_CMD_ERR_CALL_ORIG_S,
    /**< Call origination is in progress */

  CM_PD_CMD_ERR_EMERGENCY_MODE_S,
    /**<  phone is in EMERGENCY callback mode only EMERGENCY call is allowed  */

  CM_PD_CMD_ERR_NO_SRV_S,
    /**< Client tried to send MO SMS but there is no service */

  /* 8 */
  CM_PD_CMD_ERR_NOT_ALLOWED_IN_AMPS_S,
    /**< Client tried to send but the srv state is analog */

  CM_PD_CMD_ERR_NOT_ALLOWED_IN_HDR_S,
    /**< Client tried to send but an HDR call is up */

  /* Error codes resulting from operational limitation
  ** of CM.
  */

  CM_PD_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command */

  CM_PD_CMD_ERR_PHONE_BUSY,
    /**< Phone is already in use */

  /* 12 */
  CM_PD_CMD_ERR_SESSION_TYPE_INVALID,
    /**< Session type is invalid */

  CM_PD_CMD_ERR_OTHER,
    /**< any other errors not listed above */

  CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  CM_PD_CMD_ERR_PAYLOAD_SIZE_P,
    /**< Invalid size of message payload */

  CM_PD_CMD_ERR_PAYLOAD_NULL_PTR_P,
    /**< NULL pointer passed as message payload */

  CM_PD_CMD_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_pd_cmd_err_e_type;


/** Possible PD events
*/
typedef enum cm_pd_event_e {

  CM_PD_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PD_EVENT_SESSION_START,
    /**< A PD session start event */

  CM_PD_EVENT_SESSION_GRANTED,
    /**< A PD session granted event */

  CM_PD_EVENT_REVA_MSG,
    /**<A PD event with RevA message*/

  CM_PD_EVENT_SESSION_END,
    /**< A PD session end event */

  /* 4 */
  CM_PD_EVENT_FIX_START,
    /**< Blocked HDR data call origination to start GPS search */

  CM_PD_EVENT_FIX_END,
    /**< Unblocked HDR data call origination */

  CM_PD_EVENT_SESSION_DENIED,
    /**< PD Session denied */

  CM_PD_EVENT_SESSION_DORMANT_CHGD,
    /**< The session dormancy changed */

  /* 8 */
  CM_PD_EVENT_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_pd_event_e_type;


/** status related typedefs. Since PD uses DBM, these statuses
*  must map to the DBM statuses.
*/
typedef enum cm_pd_mo_status_e {

  CM_PD_MO_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PD_MO_OK=CM_DBM_MO_OK,
    /**< PD msg has been completed successfully */

  CM_PD_MO_HOLD_ORIG_RETRY_TIMEOUT=CM_DBM_MO_HOLD_ORIG_RETRY_TIMEOUT,
    /**< HOLD ORIG retry timeout*/

  CM_PD_MO_HOLD_ORIG=CM_DBM_MO_HOLD_ORIG,
    /**< MC can not process MO MS because HOLD ORIG is TRUE*/

  CM_PD_MO_NO_SRV=CM_DBM_MO_NO_SRV,
    /**< can not send msg  - no service */

  /* 4 */
  CM_PD_MO_ABORT=CM_DBM_MO_ABORT,
    /**< MC or CM aborts the request */

  CM_PD_MO_NOT_ALLOWED_IN_AMPS=CM_DBM_MO_NOT_ALLOWED_IN_AMPS,
    /**< can not send msg in analog mode */

  CM_PD_MO_NOT_ALLOWED_IN_HDR=CM_DBM_MO_NOT_ALLOWED_IN_HDR,
    /**< can not send data burst msg while HDR call is up */

  CM_PD_MO_L2_ACK_FAILURE=CM_DBM_MO_L2_ACK_FAILURE,
    /**< not receiving L2 ack from base station */

  /* 8 */
  CM_PD_MO_OUT_OF_RESOURCES=CM_DBM_MO_OUT_OF_RESOURCES,
    /**< e.g. out of memory buffer */

  CM_PD_MO_ACCESS_TOO_LARGE=CM_DBM_MO_ACCESS_TOO_LARGE,
    /**< message is too large to be sent over access */

  CM_PD_MO_DTC_TOO_LARGE=CM_DBM_MO_DTC_TOO_LARGE,
    /**< message is too large to be sent over DTC */

  CM_PD_MO_ACCT_BLOCK,
    /**< Access Control based on Call Type(service option)  is blocked */

  CM_PD_MO_MAX
  /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_pd_mo_status_e_type;


/** PD session status typedef
** FEATURE_GPSONE_MSBASED
*/
typedef enum cm_pd_session_status_e {

  CM_PD_SESSION_STATUS_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PD_SESSION_STATUS_END,
    /**< PD session is ended normally*/

  CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
    /**< PD session abort because Phone is not in Idle:
    **  +. Incoming call
    **  +. Outgoing call
    **  +. etc.
    */

  CM_PD_SESSION_STATUS_ABORT_ACQ_FAIL,
    /**< PD session abort because Acquisition is failed */

  CM_PD_SESSION_STATUS_ABORT_SYSTEM_LOST,
    /**< PD session abort because system is lost */

  /* 4 */
  CM_PD_SESSION_STATUS_ABORT_GPS_MODE_EXIT,
    /**< PD session abort because of GPS mode exit */

  CM_PD_SESSION_STATUS_DENY_GSM_CALL,
    /**< PD session deny because Phone is in GSM call
    */

  CM_PD_SESSION_STATUS_DENY_HDR_CALL,
    /**< PD session deny because Phone is in HDR call
    */

  CM_PD_SESSION_STATUS_DENY_GSM_ORIG,
    /**< PD session deny because Phone is in GSM call Orig
    */

  /* 8 */
  CM_PD_SESSION_STATUS_GRANTED,
    /**< Session can be granted */

  CM_PD_SESSION_STATUS_HOLD_FORCE_MODE,
    /**< Wait 1 seconds for the force mode to work */

  CM_PD_SESSION_STATUS_DENY_CMD_ON_HOLD,
    /**< There is a pending command */

  CM_PD_SESSION_STATUS_DENY_NO_CDMA_SUBSC,
    /**< PD session deny because Phone doesn't have CDMA subscription */


  CM_PD_SESSION_STATUS_MAX
    /**< @internal */

} cm_pd_session_status_e_type;


/** PD sessions
** FEATURE_GPSONE_MSBASED
*/
typedef enum cm_pd_session_type_e {

  CM_PD_SESSION_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PD_SESSION_TYPE_WITH_SYS_TIME,

  CM_PD_SESSION_TYPE_WITH_GPS_MODE,

  CM_PD_SESSION_TYPE_WITH_GPS_NORMAL,
    /**< Normal GPS session, typically
    ** this is of short duration
    */

  CM_PD_SESSION_TYPE_WITH_GPS_DMOD,
    /**< GPS DMOD session, this might
    ** Take upto 6 minutes
    */

  /* 4 */
  CM_PD_SESSION_TYPE_SIMULTANEOUS_DMOD_NORMAL,
    /**< Simultaneous DMOD and Normal sessions
    */

  CM_PD_SESSION_MAX
    /**< @internal */

} cm_pd_session_type_e_type;

/** PD session dormant type
*/

typedef enum cm_pd_session_dormant_e {

  CM_PD_SESSION_DORMANT_NONE=-1,
    /**< @internal */

  CM_PD_SESSION_ACTIVE_TO_DORMANT,
    /**< Session going from active to dormant
    */

  CM_PD_SESSION_DORMANT_TO_ACTIVE,
    /**< Session going from Dormant to Active
    */

  CM_PD_SESSION_DORMANT_MAX
    /**< @internal */

}cm_pd_session_dormant_e_type;


typedef enum cm_pd_status_e {

  CM_PD_ERR_NONE=-1,
    /**< @internal */

  CM_PD_OK,
    /**< No error, request was completed successfully */

  CM_PD_ERR_NULL_CHECK_REQ_FUNC,
    /**< Null function provided for checking if a request is time-constraint */

  CM_PD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  CM_PD_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_pd_status_e_type;



/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

#define CM_BSSR_MAX_PILOTS          0xF /* 0xF pilots per Rec*/

typedef struct
{
  word    pilot_pn;
  word    base_id;
  word    sid;
  word    nid;
  boolean           base_lat_long_incl;
  sys_base_lat_type
                    base_lat;
  sys_base_long_type
                    base_long;
} cm_pd_pi_rec_type;

/**for now the assumption is that num_records will be 1*/
typedef struct {
    uint16            num_pilots;
    cm_pd_pi_rec_type pi_rec[CM_BSSR_MAX_PILOTS]; /*record per pilot_pn*/
} cm_pd_bss_rsp_info_s_type;




typedef struct cm_pd_info_s {

  cm_pd_session_status_e_type   end_status;
    /**< Session status. FEATURE_GPSONE_MSBASED only */

  cm_pd_session_type_e_type     session_type;
    /**< Session type. FEATURE_GPSONE_MSBASED only */

  boolean                       gps_fix_status;
  /**<---------------------------------
    Following field used for receiving
    Pilot info in BSStatus Request message.
  *------------------------------ */
  cm_pd_bss_rsp_info_s_type pilot_info;
  /**< this contains pilot info received in BSStatusRsp msg*/
    /**< HDR data call origination status. TRUE - blocked; FALSE - Unblocked */

  cm_client_id_type             requesting_client_id;
    /**< Requesting client */

  cm_pd_session_dormant_e_type  dormant_status;
    /**< The dormancy status */

} cm_pd_info_s_type;


/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/


/** Type for PD command callback functions.
**
** A client that wants to be notified of the result of a
** pd-command-function needs to supply a pointer to a function
** of this type.
*/
typedef void (cm_pd_cmd_cb_f_type) (

  void                       *data_block_ptr,
    /**< Pointer to client provided data block */

  cm_pd_cmd_e_type           pd_cmd,
    /**< Indicate which pd command is this error status for */

  cm_pd_cmd_err_e_type       pd_cmd_err
    /**< Reason for rejection */

);


/**===========================================================================

@FUNCTION cm_pd_cmd_session_start

@DESCRIPTION
  Send a start session command to CM. Places a request on the CM queue.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()
  FEATURE_GPSONE_MSBASED must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_pd_cmd_session_start(

  cm_pd_cmd_cb_f_type         cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< requesting client ID */

  cm_pd_session_type_e_type   session_type

);

/**===========================================================================

@FUNCTION cm_pd_cmd_session_dorm_update

@DESCRIPTION
  Send a session dormancy command to CM. Places a request on the CM queue.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
boolean cm_pd_cmd_session_dorm_update(

  cm_pd_cmd_cb_f_type          cmd_cb_func,
      /**< client callback function */

  void                        *data_block_ptr,
      /**< pointer to client callback data block */

  cm_client_id_type            client,
      /**< requesting client ID */

  cm_pd_session_type_e_type    session_type,
      /**< type of session */

  cm_pd_session_dormant_e_type session_dormant
      /**< The dormant status of the session */

);

/**===========================================================================

@FUNCTION cm_pd_cmd_session_end

@DESCRIPTION
  Send a end session command to CM. Places a request on the CM queue.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()
  FEATURE_GPSONE_MSBASED must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_pd_cmd_session_end
(
  cm_pd_cmd_cb_f_type       cmd_cb_func,
    /**< client callback function */

  void                     *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< requesting client ID */

  cm_pd_session_type_e_type session_type
    /**< type of session */

);

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

/** For FEATURE_GPSONE_NEW_L2_ACK only
*/
typedef boolean ( cm_pd_resp_is_within_t2m_f_type ) (

  uint8      *payload,
    /**< msg buffer pointer */

  uint32     numBytes
    /**< msg length */

);



/**===========================================================================

@FUNCTION cm_pd_reg

@DESCRIPTION

@DEPENDENCIES
  FEATURE_GPSONE_NEW_L2_ACK must be defined.

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern cm_pd_status_e_type cm_pd_reg (

  cm_pd_resp_is_within_t2m_f_type   *cm_pd_resp_is_within_t2m_func
    /**< pointer position determination function  types */

);

/** @}
*  endgroup PDO Position Determination Object
*/

/****************************************************************************

Supplementary Services Object

****************************************************************************/

/** @defgroup SSO Supplementary Services Object
** @{
*/

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

/**
** Client should check for CM_API_SUPS_DEFAULT_VALUES before using this macro
*/
#define CM_SS_REF_UNASSIGNED  ((byte) 0x00)
  /* Default value of ss_ref in cm_sups_info_s */

/**
** Client should check for CM_API_SUPS_DEFAULT_VALUES before using this macro
*/
#define CM_SS_CODE_UNASSIGNED  ((byte) 0x00)
  /**< Default value of ss_code in cm_sups_info_s */

/**
** Client should check for CM_API_SUPS_DEFAULT_VALUES before using this macro
*/
#define CM_SS_OPERATION_UNASSIGNED  ((byte) 0x00)
  /**< Default value of ss_operation in cm_sups_info_s
  ** Set equal to NULL_SS_OPERATION in MN
  */

/**
** Client should check for CM_API_SUPS_DEFAULT_VALUES before using this macro
*/
#define CM_SS_INVOKE_ID_UNASSIGNED  ((byte) 0x00)
  /**< Default value of invoke_id in cm_sups_info_s */


#define CM_USS_DATA_MAX CM_USS_PHASE2_TYPE_MAX
  /**< Enum cm_uss_data_e */

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/



/** Enumeration of sups command errors returned by command
** callback functions.
*/
typedef enum cm_sups_cmd_err_e {

  CM_SUPS_CMD_ERR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_SUPS_CMD_ERR_NOERR,
    /**< No errors found */

  /* Error codes resulting from clients providing a sups command
  ** function with bad or invalid parameters.
  */

  CM_SUPS_CMD_ERR_CLIENT_ID_P,
    /**< invalid client ID - null or not initialized*/

  CM_SUPS_CMD_ERR_NULL_PARAM_P,
    /**< Null pointer parameter */

  /* Error codes resulting from clients trying to instruct
  ** CM to perform inappropriate actions relative to the current
  ** state of the call and/or phone.
  */

  CM_SUPS_CMD_ERR_OFFLINE_S,
    /**< Phone is offline */

  /* 4 */
  CM_SUPS_CMD_ERR_SRV_STATE_S,
    /**< The command in question is invalid in the current Phone serving state -
    ** e.g answering a call but serving state is no service */

  /* Error codes resulting from operational limitation
  ** of CM.
  */

  CM_SUPS_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command */

  CM_SUPS_CMD_ERR_OTHER,
    /**< A problem other than the above was found */

  CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  /* 8 */
  CM_SUPS_CMD_ERR_CALL_CONTROL_FAILURE,
    /**< Unable to complete call control */

  CM_SUPS_CMD_ERR_CALL_CONTROL_REJECTED,
    /**< Call control rejected sups request */

  CM_SUPS_CMD_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_sups_cmd_err_e_type;


/** Enumeration of supplementary services events.
** Clients are able to register with CM to be notified of any subsets
** of these supplementary services events.
*/
typedef enum cm_sups_event_e {

  CM_SUPS_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_SUPS_EVENT_REGISTER,
    /**< Register Supplementary Service */

  CM_SUPS_EVENT_REGISTER_CONF,
    /**< Register Supplementary Service Confirmation */

  CM_SUPS_EVENT_ERASE,
    /**< Erase Supplementary Service Request */

  CM_SUPS_EVENT_ERASE_CONF,
    /**< Erase Supplementary Service Confirmation */

  /* 4 */
  CM_SUPS_EVENT_ACTIVATE,
    /**< Activate Supplementary Service Request */

  CM_SUPS_EVENT_ACTIVATE_CONF,
    /**< Activate Supplementary Service Confirmation */

  CM_SUPS_EVENT_DEACTIVATE,
    /**< Deactivate Supplementary Service Request */

  CM_SUPS_EVENT_DEACTIVATE_CONF,
    /**< Deactivate Supplementary Service Confirmation */

  /* 8 */
  CM_SUPS_EVENT_INTERROGATE,
    /**< Interrogate Supplementary Service Request */

  CM_SUPS_EVENT_INTERROGATE_CONF,
    /**< Interrogate Supplementary Service Confirmation */

  CM_SUPS_EVENT_REG_PASSWORD,
    /**< Register Password Supplementary Service Request*/

  CM_SUPS_EVENT_REG_PASSWORD_CONF,
    /**< Register Password Supplementary Service Confirmation */

  /* 12 */
  CM_SUPS_EVENT_PROCESS_USS,
    /**< Process Unstructured Supplementary Service Request */

  CM_SUPS_EVENT_PROCESS_USS_CONF,
    /**< Process Unstructured Supplementary Service Confirmation */

  CM_SUPS_EVENT_RELEASE,
    /**< Release Supplementary Service Request */

  CM_SUPS_EVENT_ABORT,
    /**< Abort Supplementary Service Request */

  /* 16 */
  CM_SUPS_EVENT_FWD_CHECK_IND,
    /**< Forward Check Supplementary Service Indication */

  CM_SUPS_EVENT_USS_NOTIFY_IND,
    /**< Unstructured Supplementary Service Notify Indication */

  CM_SUPS_EVENT_USS_NOTIFY_RES,
    /**< Unstructured Supplementary Service Notify Response */

  CM_SUPS_EVENT_USS_IND,
    /**< Unstructured Supplementary Service Indication */

  /* 20 */
  CM_SUPS_EVENT_USS_RES,
    /**< Unstructured Supplementary Service Response */

  CM_SUPS_EVENT_RELEASE_USS_IND,
    /**< Release Unstructured Supplementary Service Indication */

  CM_SUPS_EVENT_GET_PASSWORD_IND,
    /**< Get Password Supplementary Service Indication */

  CM_SUPS_EVENT_GET_PASSWORD_RES,
    /**< Get Password Supplementary Service Response */


  /* 24 */
  CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_IND,
    /**< LCS Location Notification Indication */

  CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_RES,
    /**< LCS Location Notification Response */

  CM_SUPS_EVENT_LCS_MOLR,
    /**< LCS MOLR Request */

  CM_SUPS_EVENT_LCS_MOLR_COMPLETED,
    /**< LCS MOLR Completed */

  /* 28 */
  CM_SUPS_EVENT_LCS_MOLR_CONF,
    /**< LCS MOLR Confirmation */


  CM_SUPS_EVENT_SS_MOD_TO_ORIG,
    /**< Sups command was modified to origination due to call control */

  CM_SUPS_EVENT_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_sups_event_e_type;


/* Response to Unstructured SS Indication from the
   network.
*/
typedef enum cm_uss_res_e {

  CM_USS_RES_SS_NONE=-1,
    /**< @internal */

  CM_USS_RES_SS_DATA,
    /**< USS response with data */

  CM_USS_RES_SS_ERROR,
    /**< USS response indicating SS error */

  CM_USS_RES_SS_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_uss_res_e_type;


typedef enum cm_uss_data_e
{

  CM_USS_UNKNOWN_TYPE = -1,
    /**< @internal */

  CM_USS_PHASE1_TYPE,
    /**< Phase 1 USS data */

  CM_USS_PHASE2_TYPE,
    /**< Phase 2 USS data */

  CM_USS_DATA_NONE,
    /**< USS data with no type */

  CM_USS_PHASE2_TYPE_MAX
  /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */


} cm_uss_data_e_type;



/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/**
** Unparsed SUPS L3 message passed from MN.
** Clients need to check for CM_SUPS_UNPARSED_L3_DATA before
** using this field.
*/
typedef struct cm_sups_unparsed_l3_data_s
{
   boolean                         present;
     /**< To check if size and data are valid */

   uint16                            size;
     /**< Actual size of the array data */

   byte                            data[CM_MAX_SUPS_PARAMETER_LENGTH];
     /**< array to hold the L3 raw data from MN */

} cm_sups_unparsed_l3_data_s_type;


/** Type to hold supplementary services information.
** When notified of supplementary services events, clients are given
** a reference to such a structure.
*/
typedef struct cm_sups_info_s {
  cm_client_id_type                   sups_client_id;
    /**< client */

  boolean                             ss_success;

  uint8                               ss_ref;
    /**< SS reference */

  uint8                               ss_code;
    /**< SS code */

  uint8                               ss_operation;
    /**< SS operation */

  uint8                               invoke_id;
    /**< Invocation Id */

  basic_service_s_type                basic_service;
    /**< Basic Service Group information */

  cm_called_party_bcd_no_s_type       forwarded_to_number;
    /**< Forwarded to number */

  cm_called_party_subaddress_s_type   forwarded_to_subaddress;
    /**< Forwarded to subaddress */

  ie_no_reply_condition_timer_s_type  nr_timer;
    /**< No reply condition timer */

  ie_ss_password_s_type               ss_password;
    /**< SS password */

  ie_ss_password_s_type               ss_new_password;
    /**< New password */

  ie_ss_password_s_type               ss_new_password_again;
    /**< Confirmation of new password */

  ie_ss_code_s_type                   code;
    /**< SS code */

  ss_password_value                   guidance_info;
    /**< Password guidance information */

  cm_ss_conf_e_type                   conf_type;
    /**< SS confirmation type */

  ie_cm_ss_error_s_type               ss_error;
    /**< SS error */

  ie_forwarding_info_s_type           forwarding_info;
    /**< Call forwarding Information */

  ie_call_barring_info_s_type         call_barring_info;
    /**< Call barring information */

  ie_cug_info_s_type                  cug_info;
    /**< Closed User Group information */

  ie_cm_cc_cause_s_type               cc_cause;
    /**< Call control cause */

  ie_ss_status_s_type                 ss_status;
    /**< SS status */

  basic_service_group_list_s_type     bsg_list;
    /**< Basic Service Group list information */

  ie_forwarding_feature_list_s_type   fwd_feature_list;
    /**< Forwarding Feature list information */

  cli_restriction_info_s_type         cli_restriction;
    /**< CLI restriction information */

  cm_uss_data_e_type                  uss_data_type;
    /**< Type of USS message - phase1, phase2 */

  uss_data_s_type                     uss_data;
    /**< Unstructured SS data */

  ie_cm_ss_data_s_type                ss_data;
    /**< SS data */


  location_notification_s_type    location_notification;
    /**< LCS Location Notification Data */

  lcs_molr_res_s_type                     lcs_molr_res;
    /**< LCS MOLR Response */



  cm_client_id_type                   requesting_client_id;
    /**< Requesting client */


  cm_ccbs_indx_type                   ccbs_indx;
    /**< Clients should check for CM_API_CCBS
    ** to use this field
    **
    ** ccbs_indx = CM_CCBS_INDX_INVALID results
    **   deact of all pending ccbs requests.
    **
    ** ccbs_indx = valid value results in one particular
    **   ccbs getting deactivated.
    */

  #if defined(FEATURE_CCBS) || defined(FEATURE_MM_SUPERSET)
  ccbs_info_s_type                    ccbs_info;
    /**< Clients should check for CM_API_CCBS
    ** to use this field.
    ** Carries list of ccbs indices returned as a result of
    ** interrogation. Note that ccbs_Index will contain call
    ** ids.
    */
  #endif

  cm_sups_unparsed_l3_data_s_type      sups_unparsed_l3_data;
    /**<
    **  Unparsed L3 message from MN.
    ** Clients need to check for CM_SUPS_UNPARSED_L3_DATA before
    ** using this field.
    */

} cm_sups_info_s_type;


/** Get Password Response Parameters
*/
typedef struct cm_get_pwd_res_params_s {

  uint8                      invoke_id;
    /**< Invocation Id */

  ie_ss_password_s_type      ss_pwd;
    /**< SS password - one of old, new or confirmation of
                     new password as requested by the
                     network
    */

} cm_get_pwd_res_params_s_type;



/** LCS Location Notification Response Parameters
*/
typedef struct cm_lcs_location_notification_res_params_s {

  uint8                        invoke_id;
    /**< Invocation ID */

  ie_ss_location_notification_res_s_type  location_notification_res;
    /**< Enumerated Response */

} cm_lcs_location_notification_res_params_s_type;

/** LCS MOLR SS Parameters
*/
typedef  struct cm_lcs_molr_ss_params_s {

  uint8                  ss_ref;
    /**< SS reference */

  cm_molr_type_e_type             molr_type;
    /**< MOLR Type */

  location_method_s_type       location_method;
    /**< Location Method */

  lcs_qos_s_type               lcs_qos;
    /**< QOS Type */

  lcs_client_external_id_s_type external_id;
    /**< LCS Client External ID */

  mlc_number_s_type            mlc_number;
    /**< MLC Number */

  gps_assistance_data_s_type   gps_assistance_data;
    /**< GPS Assistance Data */

  supported_gad_shapes_s_type  supported_gad_shapes;
    /**< Supported GAD Shapes */

} cm_lcs_molr_params_s_type;

/** LCS MOLR Completed SS Parameters
*/
typedef struct cm_lcs_molr_completed_params_s {

  uint8  ss_ref;
    /**< SS Reference */

} cm_lcs_molr_completed_params_s_type;




/** Unstructured SS Notify Response Parameters
*/
typedef struct cm_uss_notify_res_params_s {

  ie_cm_ss_error_s_type        ss_error;
    /**< USS Notify response SS error */

  uint8                        invoke_id;
    /**< Invocation Id */

} cm_uss_notify_res_params_s_type;


/** Unstructured SS Response Parameters
*/
typedef struct cm_uss_res_params_s {

  uint8                           invoke_id;
    /**< Invocation Id */

  uss_res_data_s_type             uss_res_data;
    /**< USS response data */

} cm_uss_res_params_s_type;


/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/** Type for SUPS command callback functions.
**
** A client that wants to be notified of the result of a
** sups-command-function needs to supply a pointer to a function
** of this type as the first parameter to a sups-command-function.
*/
typedef void (cm_sups_cmd_cb_f_type) (

  void                        *data_block_ptr,
    /**< Pointer to client provided data block */

  cm_sups_cmd_e_type           sups_cmd,
    /**< Indicate which sups command is this error status for */

  cm_sups_cmd_err_e_type       sups_cmd_err
    /**< SUPS command error code, indicating whether command
    ** is rejected and the reason */

);



/**===========================================================================

@FUNCTION cm_sups_cmd_register

@DESCRIPTION
  Register SS command to CM. This function allows the client to send
  a Register Supplementary Service request to the CM. This is a common
  function that can be used to register different Supplementary Services.
  The client ID should be from a registered CM client.


  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_register(

  cm_sups_cmd_cb_f_type     sups_cmd_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< originating client */

  const cm_reg_ss_params_s_type   *reg_cmd_params
    /**< pointer to register SS command data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_register */
/*~ PARAM reg_cmd_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_sups_cmd_register_cc

@DESCRIPTION
  Register SS command to CM with call control.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_register_cc(

  cm_sups_cmd_cb_f_type     sups_cmd_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< originating client */

  const cm_reg_ss_params_s_type   *reg_cmd_params,
    /**< pointer to register SS command data */

  const cm_num_s_type             *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                   is_call_control_processing_complete
    /**< Indicates whether call control has already been coompleted */

);



/**===========================================================================

@FUNCTION cm_sups_cmd_erase

@DESCRIPTION
  EraseSS command to CM.
  This function enables the client to send an Erase Supplementary Service
  request to the CM. This is a common function that can be used to erase
  a previous registration of a Supplementary Service. The client ID should
  be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_erase(

  cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< originating client */

  const cm_erase_ss_params_s_type   *erase_cmd_params
    /**< pointer to erase SS command data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_erase */
/*~ PARAM erase_cmd_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_sups_cmd_erase_cc

@DESCRIPTION
  EraseSS command to CM with call control.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_erase_cc(

  cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< originating client */

  const cm_erase_ss_params_s_type   *erase_cmd_params,
    /**< pointer to erase SS command data */

  const cm_num_s_type               *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                     is_call_control_processing_complete
   /**< Indicates whether call control has already been completed */

);



/**===========================================================================

@FUNCTION cm_sups_cmd_activate

@DESCRIPTION
  Send an Activate Supplementary Service request.
  This function enables the client to send an Activate Supplementary
  Service request to the CM. This is a common function that can be used
  to activate different Supplementary Services. The client ID should be
  from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_activate(

  cm_sups_cmd_cb_f_type    sups_cmd_cb_func,
    /**< client callback function */

  void                     *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type        client_id,
    /**< originating client */

  const cm_act_ss_params_s_type  *act_cmd_params
    /**< pointer to activate SS command data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_activate */
/*~ PARAM act_cmd_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_sups_cmd_activate_cc

@DESCRIPTION
  EraseSS command to CM with call control.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_activate_cc(

  cm_sups_cmd_cb_f_type    sups_cmd_cb_func,
    /**< client callback function */

  void                     *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type        client_id,
    /**< originating client */

  const cm_act_ss_params_s_type  *act_cmd_params,
    /**< pointer to activate SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  boolean                  is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);



/**===========================================================================

@FUNCTION cm_sups_cmd_deactivate

@DESCRIPTION
  DeactivateSS command to CM.
  This function enables the client to send an Deactivate Supplementary Service
  request to the CM. This is a common function that can be used to deactivate
  different Supplementary Services. The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_deactivate(

  cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< originating client */

  const cm_deact_ss_params_s_type   *deact_cmd_params
    /**< pointer to erase SS command data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_deactivate */
/*~ PARAM deact_cmd_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_sups_cmd_deactivate_cc

@DESCRIPTION
  DeactivateSS command to CM with call control.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_deactivate_cc(

  cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< originating client */

  const cm_deact_ss_params_s_type   *deact_cmd_params,
    /**< pointer to erase SS command data */

  const cm_num_s_type               *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                     is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);


/**===========================================================================

@FUNCTION cm_sups_cmd_interrogate

@DESCRIPTION
  InterrogateSS command to CM.
  This function enables the client to send an Interrogate Supplementary Service request
  to the CM. The status of a Supplementary Service can be checked using this command.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_interrogate(

  cm_sups_cmd_cb_f_type        sups_cmd_cb_func,
    /**< client callback function */

  void                         *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type            client_id,
    /**< originating client */

  const cm_interr_ss_params_s_type   *interr_cmd_params
    /**< pointer to interrogate SS command data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_interrogate */
/*~ PARAM interr_cmd_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_sups_cmd_interrogate_cc

@DESCRIPTION
  InterrogateSS command to CM with call control.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_interrogate_cc(

  cm_sups_cmd_cb_f_type        sups_cmd_cb_func,
    /**< client callback function */

  void                         *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type            client_id,
    /**< originating client */

  const cm_interr_ss_params_s_type   *interr_cmd_params,
    /**< pointer to interrogate SS command data */

  const cm_num_s_type                *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                      is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);


/**===========================================================================

@FUNCTION cm_sups_cmd_process_uss

@DESCRIPTION
  Process Unstructured SS command to CM. Send a mobile-initiated Unstructured
  Supplementary Service Data request. This function enables the client to send
  a Process Unstructured Supplementary Service request to the CM. This function
  is used to originate an Unstructured SS Data operation by the mobile station.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_process_uss(

  cm_sups_cmd_cb_f_type    sups_cmd_cb_func,
    /**< client callback function */

  void                     *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type        client_id,
    /**< originating client */

  const cm_uss_params_s_type     *process_uss_cmd_params
    /**< pointer to Process Unstructured SS command data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_process_uss */
/*~ PARAM process_uss_cmd_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_sups_cmd_process_uss_cc

@DESCRIPTION
  Process Unstructured SS command to CM with call control.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_process_uss_cc(

  cm_sups_cmd_cb_f_type    sups_cmd_cb_func,
    /**< client callback function */

  void                     *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type        client_id,
    /**< originating client */

  const cm_uss_params_s_type     *process_uss_cmd_params,
    /**< pointer to Process Unstructured SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                  is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);



/**===========================================================================

@FUNCTION cm_sups_cmd_reg_password

@DESCRIPTION
  Register Password SS command to CM.This function enables the client to send a
  Register Password Supplementary Service request to the CM. This function is
  called to register a password for a Supplementary Service (for example, register
  a password for call restriction service) or to set a new password (overwrite the
  existing password with a new password). The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_reg_password(

  cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
    /**< client callback function */

  void                       *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type          client_id,
    /**< originating client */

  const cm_reg_pwd_params_s_type   *reg_pwd_cmd_params
    /**< pointer to register password SS command data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_reg_password */
/*~ PARAM reg_pwd_cmd_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_sups_cmd_reg_password_cc

@DESCRIPTION
  Register Password SS command to CM with call control.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_reg_password_cc(

  cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
    /**< client callback function */

  void                       *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type          client_id,
    /**< originating client */

  const cm_reg_pwd_params_s_type   *reg_pwd_cmd_params,
    /**< pointer to register password SS command data */

  const cm_num_s_type              *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                    is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);



/**===========================================================================

@FUNCTION cm_sups_cmd_release

@DESCRIPTION
  Release SS command to CM. This function enables the client to send
  a Release Supplementary Service request to the CM. The release request
  is sent to end a Get Password operation or a mobile-originated Unstructured
  Supplementary Service Data operation. The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_release(

  cm_sups_cmd_cb_f_type          sups_cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type              client_id,
    /**< originating client */

  const cm_release_ss_params_s_type    *release_cmd_params
    /**< pointer to release SS command data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_release */
/*~ PARAM release_cmd_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_sups_cmd_release_cc

@DESCRIPTION
  Release SS command to CM with call control.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_release_cc(

  cm_sups_cmd_cb_f_type          sups_cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type              client_id,
    /**< originating client */

  const cm_release_ss_params_s_type    *release_cmd_params,
    /**< pointer to release SS command data */

  const cm_num_s_type                  *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                        is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);



/**===========================================================================

@FUNCTION cm_sups_cmd_get_password_res

@DESCRIPTION
  Send a response to a Get Password Indication.
  This function is called by the client to send a Get Password Response to the CM.
  This response is sent when the mobile station receives a Get Password Indication
  from the network. The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_get_password_res(

  cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
    /**< client callback function */

  void                              *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                 client_id,
    /**< originating client */

  const cm_get_pwd_res_params_s_type      *get_pwd_res_params
    /**< pointer to Get Password response data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_get_password_res */
/*~ PARAM get_pwd_res_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/**===========================================================================

@FUNCTION cm_sups_cmd_lcs_molr

@DESCRIPTION
  LCS MOLR SS command to CM.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_sups_cmd_lcs_molr
(

        cm_sups_cmd_cb_f_type            sups_cmd_cb_func,
            /**< client callback function */

        void                            *data_block_ptr,
            /**< pointer to client callback data block */

        cm_client_id_type                client,
            /**< originating client */

        const cm_lcs_molr_params_s_type *lcs_molr_cmd_params
            /**< pointer to lcs-MOLR SS command data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_lcs_molr */
/*~ PARAM lcs_molr_cmd_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_sups_cmd_lcs_location_notification_res

@DESCRIPTION
  Send an LCS location notification response to the network.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_sups_cmd_lcs_location_notification_res(

        cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
            /**< client callback function */

        void                               *data_block_ptr,
            /**< pointer to client callback data block */

        cm_client_id_type                  client,
            /**< originating client */

  const cm_lcs_location_notification_res_params_s_type       *lcs_location_notification_res_params
            /**< pointer to location notification response data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  cm_sups_cmd_lcs_location_notification_res */
/*~ PARAM lcs_location_notification_res_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_sups_cmd_lcs_molr_completed

@DESCRIPTION
  LCS MOLR SS Completed command to CM.
  The client ID should be from a registered CM client.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_sups_cmd_lcs_molr_completed(

        cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
            /**< client callback function */

        void                       *data_block_ptr,
            /**< pointer to client callback data block */

        cm_client_id_type           client,
            /**< originating client */

 const cm_lcs_molr_completed_params_s_type    *lcs_molr_completed_cmd_params
            /**< pointer to lcs-MOLR Completed SS command data */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION  cm_sups_cmd_lcs_molr_completed */
/*~ PARAM lcs_molr_completed_cmd_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */





/**===========================================================================

@FUNCTION cm_sups_cmd_uss_notify_res

@DESCRIPTION
  Unstructured SS Notify response command to CM.
  This function is called by the client to send an Unstructured Supplementary Service
  Notify Response to the CM. This response message is sent by the mobile station when
  an Unstructured Supplementary Service Notification received from the network has an error.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_uss_notify_res(

  cm_sups_cmd_cb_f_type               sups_cmd_cb_func,
    /**< client callback function */

  void                                *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                   client_id,
    /**< originating client */

  const cm_uss_notify_res_params_s_type     *uss_notify_res_params
    /**< pointer to Unstructured SS Notify response data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_uss_notify_res */
/*~ PARAM uss_notify_res_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/**===========================================================================

@FUNCTION cm_sups_cmd_uss_res

@DESCRIPTION
  Unstructured SS response command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_uss_res(

  cm_sups_cmd_cb_f_type         sups_cmd_cb_func,
    /**< client callback function */

  void                          *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type             client_id,
    /**< originating client */

  const cm_uss_res_params_s_type      *uss_res_params
    /**< pointer to Unstructured SS response data */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_sups_cmd_uss_res */
/*~ PARAM uss_res_params POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/** @}
*  endgroup SSO Supplementary Services Object
*/

/****************************************************************************

Data Burst Message Object
** For internal use (uapdms) only **

****************************************************************************/

/** @defgroup DBM Data Burst Message Object
** @{
*/

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

#define CM_DBM_MAX_PAYLOAD_SIZE 255 /* Data Burst Message max payload size */

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

typedef enum
{
  CM_DBM_NONE = -1,

  /**< CM will try to send the DBM on either access or enhanced access channel
   *  or traffic (if traffic channel is available). If it fails to successfully
   *  do that, then it will originate a call and send the DBM on traffic.
  */
  CM_DBM_TRANSMIT_AUTOMATIC,


  /**< CM will try to send the DBM on enhanced access channel or traffic (if
   *  traffic channel is available). If it fails to successfully do that or
   *  enhanced access channel is not available, then it will notify back a
   *  failure event to the interested clients.
  */
  CM_DBM_TRANSMIT_ENH_ACCESS_OR_TRAFFIC,


  /**< CM will try to send the DBM on access channel or traffic (if traffic
   *  channel is available). If it fails to successfully do that, then it
   *  will notify back a failure event to the interested clients.
  */
  CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE,


  CM_DBM_MAX

} cm_dbm_trans_pref_e_type;


/*---------------------------------
  Possible DBM events
 *------------------------------ */

typedef enum cm_dbm_event_e {
    CM_DBM_EVENT_NONE = -1,    /**< @internal */

    CM_DBM_EVENT_MO_STATUS,
    CM_DBM_EVENT_MT_MSG,

    CM_DBM_EVENT_MAX         /**< @internal */
} cm_dbm_event_e_type;

/*---------------------------------
  Possible DBM classes
 *------------------------------ */

typedef enum cm_dbm_class_e {
    CM_DBM_CLASS_NONE=-1,    /**< @internal */

    CM_DBM_CLASS_PD,         /* Class for Position Determination */
    CM_DBM_CLASS_SDB,        /* Class for Short Data Burst */

    CM_DBM_CLASS_MAX         /**< @internal */
} cm_dbm_class_e_type;


/*---------------------------------
  Possible DBM commands
 *------------------------------ */

typedef enum cm_dbm_cmd_e {
    CM_DBM_CMD_NONE = -1,

    CM_DBM_CMD_SEND_MSG,

    CM_DBM_CMD_MAX
} cm_dbm_cmd_e_type;


/*---------------------------------
  Possible DBM errors
 *------------------------------ */

typedef enum cm_dbm_cmd_err_e {

    CM_DBM_CMD_ERR_NONE=-1,
        /**< @internal */

    /* 0 */
    CM_DBM_CMD_ERR_NOERR,
        /**< No errors found */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Error codes resulting from clients feeding a serving system command
    ** function with bad or inappropriate parameters.
    */

    CM_DBM_CMD_ERR_CLIENT_ID_P,
        /**< Bad client parameter */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    CM_DBM_CMD_ERR_NOT_IDLE_S,
     /**< Can not accept SMS origination while SMS state is not IDLE */

    CM_DBM_CMD_ERR_OFFLINE_S,
        /**< Phone is offline */

    /* 4 */
    CM_DBM_CMD_ERR_CDMA_LOCK_S,
        /**< Phone is CDMA locked */

    CM_DBM_CMD_ERR_CALL_ORIG_S,
        /**< Call origination is in progress */

    CM_DBM_CMD_ERR_E911_MODE_S,
        /**<  phone is in E911 callback mode only E911 call is allowed  */

    CM_DBM_CMD_ERR_NO_SRV_S,
        /**< Client tried to send MO SMS but there is no service */

    /* 8 */
    CM_DBM_CMD_ERR_NOT_ALLOWED_IN_AMPS_S,
        /**< Client tried to send but the srv state is analog */

    /* Error codes resulting from operational limitation
    ** of CM.
    */

    CM_DBM_CMD_ERR_NO_BUF_L,
        /**< No available CM command buffers to queue the command */


    CM_DBM_CMD_ERR_OTHER,
        /**< any other errors not listed above */

    CM_DBM_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
      /**< The function is unavailable due to relavent feature being undefined */

    /* 12 */
    CM_DBM_CMD_ERR_PAYLOAD_NULL_PTR_P,
      /**< Bad payload pointer */

    CM_DBM_CMD_ERR_PAYLOAD_SIZE_P,
    /**< Bad payload size parameter */

    CM_DBM_CMD_ERR_DDTM,
      /**< 1X in high psist and DO call in progress */

    CM_DBM_CMD_ERR_MAX
        /**< @internal */

} cm_dbm_cmd_err_e_type;

typedef enum cm_dbm_source_e
{

  CM_DBM_FROM_1X,
  CM_DBM_FROM_HDR

} cm_dbm_source_e_type;

/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

typedef struct cm_dbm_cmd_info_s {

  uint8                    payload[CM_DBM_MAX_PAYLOAD_SIZE];
  uint16                   num_bytes;

  cm_dbm_trans_pref_e_type trans_pref;
      /**< Transmission preference whether to send on
         EACH, ACH or set up a call etc..
      */

  uint8                    rlp_flow;
      /**< RLP Flow only used in HDR DOS msgs to identify RLP
      */

  uint8                    route;
      /**< Route indication.
      */

} cm_dbm_cmd_info_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_dbm_cmd_info_s.payload VARRAY CM_DBM_MAX_PAYLOAD_SIZE
    LENGTH cm_dbm_cmd_info_s.num_bytes */
#endif /* FEATURE_HTORPC_METACOMMENTS */


 typedef struct
{

  uint8                   burst_type;
    /**< Burst type whether a PD or SDB e.g. CAI_POSITION_DET etc. */

  cm_dbm_source_e_type    burst_source;
    /**< Whether the DBM came over 1X or HDR. */

  /**<---------------------------------
    Following 2 fields used for MO
    message status event.
   *------------------------------ */

  void                   *user_data;
    /**< The user data tag associated with a MO message */

  cm_dbm_mo_status_e_type mo_status;
    /**< The status of a MO message */

  word                    burst_overflow_size;
    /**< Number of bytes the message exceeds its maximum allowed limit */

  /**<---------------------------------
    Following 2 fields used for receiving
    messages over CDMA
   *------------------------------ */

  uint8                  *payload;
    /**< Pointer to data for received message */

  uint8                   num_bytes;
    /**< Number of bytes in the payload */

  /**<---------------------------------
    Following 2 fields used for receiving
    MT DOS messages over HDR
   *------------------------------ */

  void                   *dos_payload;
    /**< Pointer to DOS payload from HDR passed as a DISM item
    */

  uint32                 dos_num_bytes;
    /**< Number of bytes in the dos payload
    */

  cm_client_id_type      requesting_client_id;
    /**< Requesting client
    */

  uint8                  rlp_flow;
    /**< Corresponding Link Flow number for the DoS packet
    ** (FEATURE_HDR_EMPA)
    */

  byte                   route;
    /**< Route on which the DoS packet should go
    ** (FEATURE_HDR_EMPA)
    */


} cm_dbm_info_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
 /*~ FIELD cm_dbm_info_s_type.payload VARRAY LENGTH cm_dbm_info_s_type.num_bytes */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

typedef void (cm_dbm_cmd_cb_f_type) (

    void                        * data_block_ptr,
        /**< Pointer to client provided data block */

    cm_dbm_cmd_e_type           dbm_cmd,
        /**< Indicate which DBM command is this error status for */

    cm_dbm_cmd_err_e_type       dbm_cmd_err
        /**< Reason for rejection */
);


/** This callback function is used for copying the payload from
 *  Client's space to CM's space. Not all client are required to
 *  provide an implementation for this. If this is NULL CM does
 *  a memcopy itself. If this not NULL, this function is called
 *  to copy the payload into CM's space. This was provided for
 *  DS to avoid multiple copying of long payload.
*/
typedef int8 (cm_dbm_payload_copy_cb_f_type) (

    void                        * payload,
        /**<  Pointer to client provided payload
         *  <input parameter>
        */

    byte                        * dbm_payload,
        /**<  Pointer to memory buffer to store the payload data
         *  <output parameter>
        */
    int                         * num_bytes
        /**<  Number of bytes to copy
        */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cm_dbm_payload_copy_cb_f_type
    ONERROR return 0 */
/*~ PARAM OUT dbm_payload VARRAY CM_DBM_MAX_PAYLOAD_SIZE LENGTH *num_bytes */
/*~ PARAM num_bytes POINTER */
#endif

/*===========================================================================

@FUNCTION cm_dbm_cmd_mo_msg

@DESCRIPTION
  Send a data burst message. Places a request on the CM queue.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_dbm_cmd_mo_msg(
  uint8                              burst_type,
      /**< Burst type whether a PD ro SDB e.g. CAI_POSITION_DET etc */

  cm_dbm_cmd_cb_f_type               cmd_cb_func,
      /**< client callback function */

  void                               *data_block_ptr,
      /**< pointer to client callback data block */

  cm_client_id_type                  client_id,
      /**< requesting client ID */

  void *                             payload_ptr,
      /**< pointer to start of message */

  uint16                             num_bytes,
      /**< Number of bytes in message */

  cm_dbm_payload_copy_cb_f_type      *payload_copy_func,
      /**< Pointer to callback function to copy payload from client's
       * memory space to CM's memory space
      */

    uint8                   l2_ack_wait_time,
        /**< Maximum number of seconds to wait for receiving L2 ack */

    uint8                   l3_ack_wait_time,
        /**< Maximum number of seconds to wait for receiving L3 ack
         * before sending the pref sys change command
        */

    cm_dbm_trans_pref_e_type  trans_pref
        /* Transmission preference like REACH, ACH etc.. */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_dbm_cmd_mo_msg */
/*~ PARAM payload_copy_func POINTER */
#endif


/**===========================================================================

@FUNCTION cm_dbm_cmd_mo_msg2

@DESCRIPTION
  Send a data burst message. Places a request on the CM queue.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

@SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_dbm_cmd_mo_msg2 (
  uint8                              burst_type,
      /**< Burst type whether a PD ro SDB e.g. CAI_POSITION_DET etc */

  cm_dbm_cmd_cb_f_type               cmd_cb_func,
      /**< client callback function */

  void                               *data_block_ptr,
      /**< pointer to client callback data block */

  cm_client_id_type                  client_id,
      /**< requesting client ID */

  const cm_dbm_cmd_info_s_type       *dbm_cmd_info_ptr,
      /**< DBM information pointer */

  uint8                              l2_ack_wait_time,
      /**< Maximum number of seconds to wait for receiving L2 ack */

  uint8                              l3_ack_wait_time
      /**< Maximum number of seconds to wait for receiving L3 ack
       * before sending the pref sys change command
      */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_dbm_cmd_mo_msg2 */
/*~ PARAM dbm_cmd_info_ptr POINTER */
#endif

/** @}
*    endgroup DBM Data Burst Message Object
*/


/****************************************************************************

BCMCS Object

This object implements CM part of "High Rate BroadCast-MultiCast packet
data air interface Specification C.S0054" and "BCMCS Framework Draft Document
0.1.3 XP0019"

****************************************************************************/

/** @defgroup BO BCMCS Object
** @{
*/

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/** Enumeration of BCMCS commands that clients are able to send to CM.
*/
typedef enum cm_bcmcs_cmd_e {

  CM_BCMCS_CMD_NONE = -1,
    /**< @internal */

  CM_BCMCS_CMD_FLOW_REQUEST_F,
    /**< Request for flow registration */

  CM_BCMCS_CMD_REG_REQUEST_F,
    /**< BCMCS Registration Request */

  CM_BCMCS_CMD_BOM_CACHING_SETUP_F,
    /**< BCMCS strict caching setup */

  CM_BCMCS_CMD_REG_HANDOFF_F,
    /**< Enable event driven registrations on handoff */
  
  CM_BCMCS_CMD_FLOW_DEACT_REQUEST_F,
    /* Request for flow deactivation */

  CM_BCMCS_CMD_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_bcmcs_cmd_e_type;


/** Enumeration of BCMCS command errors returned by
** BCMCS command callback functions.
*/
typedef enum cm_bcmcs_cmd_err_e {

  CM_BCMCS_CMD_ERR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_BCMCS_CMD_ERR_NOERR,
    /**< No errors found */

  /*
  ** Error codes resulting from clients feeding a command
  ** function with bad or inappropriate parameters.
  */

  CM_BCMCS_CMD_ERR_INVALID_NUMBER_OF_FLOW_IDS_P,
    /**< Number of flow ids is < 0 or > SYS_BCMCS_MAX_FLOWS or
       number of flow ids is 0 while pointer to flow id list is not NULL.
       Returned the moment client tries to queue a BCMCS command to CM */

  CM_BCMCS_CMD_ERR_FLOW_IDS_NULL_PTR_P,
     /**< Null pointer given as parameter instead of a pointer
        to a list of flow ids when number of flow ids is not 0.
        Returned the moment client tries to queue a BCMCS command to CM */

  /**<- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Error codes resulting from CM being in wrong state
     for the command to be processed */

  CM_BCMCS_CMD_ERR_BCMCS_SRV_NOT_AVAILABLE_S,
    /**< Returned in response to CM_BCMCS_CMD_FLOW_REQUEST_F
       when no BCMCS service is available at any of
       the acquired systems
       NOTE: Client will be returned this error only after
             CM_BCMCS_CMD_FLOW_REQUEST_F was processed by CM.
             This is because BCMCS service may have became
             available / not available between when
             CM_BCMCS_CMD_FLOW_REQUEST_F is queued and processed by CM */

  /* Error codes resulting from operational limitation
  ** of CM.
  */

  /* 4 */
  CM_BCMCS_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command */

  CM_BCMCS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to FEATURE_BCMCS being undefined */

  CM_BCMCS_CMD_ERR_OTHER,
    /**< Error other than any of the above */

  CM_BCMCS_CMD_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_bcmcs_cmd_err_e_type;


/** Enumeration of BCMCS events.
** Clients are able to register with CM to be notified of any subsets
** of these events.
*/
typedef enum cm_bcmcs_event_e {

  CM_BCMCS_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_BCMCS_EVENT_FLOW_REQUEST_SUCCESS,
    /**< CM_BCMCS_CMD_FLOW_REQUEST_F succeeded
       Sent to client when CM received CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F
       or CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F report from HDR protocol.
       Event payload will indicate flow status for each flow that
       got registered / deregistered. */

  CM_BCMCS_EVENT_FLOW_REQUEST_FAILURE,
    /**< CM_BCMCS_CMD_FLOW_REQUEST_F failed
       Sent to client when CM failed to deliver flow registration
       request to HDR (currently it only happens when BCMCS service is
       permanently lost).
       Event payload will indicate flow status for each flow. */

  CM_BCMCS_EVENT_FLOW_STATUS_CHANGE,
    /**< HDR flow status change
       Sent to client when CM received CM_HDR_BCMCS_FLOW_STATUS_F
       report from HDR protocol.
       Event payload will indicate flow status for each flow */

  CM_BCMCS_EVENT_BROADCAST_RESET,
    /**< Broadcast reset
       Sent to client when CM received CM_HDR_BCMCS_BROADCAST_RESET_F report
       from HDR protocol.
       Event payload will indicate flow status for each flow */

  /* 4 */
  CM_BCMCS_EVENT_REGISTRATION_STATUS,
    /**<
    ** HDR registration status update Sent to client when CM
    ** received a CM_HDR_BCMCS_REG_STATUS_F report from the HDR
    ** protocol. Event payload will indicate flow registration
    ** status for each flow that was registered
    */

  CM_BCMCS_EVENT_FLOW_DEACT_REQUEST_SUCCESS,
     /* Sent to client when MBMS deact cnf is received 
     */

  CM_BCMCS_EVENT_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_bcmcs_event_e_type;



/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** Type to hold BCMCS information.
** When notified of BCMCS events, clients are given
** a reference to such a structure.
*/
typedef struct cm_bcmcs_info_s {

  sys_sys_mode_e_type             protocol;
    /**< Indicate what protocol this event comes from */

  uint8                           num_of_flows;
    /**< Number of entries in flow_status array below */

  sys_bcmcs_flow_status_element_s_type  flow_status_array[ SYS_BCMCS_MAX_FLOWS ];
    /**< Array of <IP>:<port number> addresses for which flows should be requested */

  cm_client_id_type               requesting_client_id;
    /**< Requesting client */

  sys_bcmcs_reg_status_element_s_type reg_status_array[ SYS_BCMCS_MAX_FLOWS ];
  /**< List of all the flows and registration statuses */

} cm_bcmcs_info_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_bcmcs_info_s.flow_status_array VARRAY SYS_BCMCS_MAX_FLOWS
    LENGTH cm_bcmcs_info_s.num_of_flows */
/*~ FIELD cm_bcmcs_info_s.reg_status_array VARRAY SYS_BCMCS_MAX_FLOWS
    LENGTH cm_bcmcs_info_s.num_of_flows */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*---------------------------------------------------------------------------
** Macros
**-------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
** Functions
**-------------------------------------------------------------------------*/

/* CM command functions
**
** The CM command functions defines the Client->CM interface.
**
** Each command function takes a pointer to a client provided callback
** function, a client provided void pointer and the command's required set
** of fields.
**
** If the command function find one or more of the parameters to contain
** invalid values ( e.g. not one of the enum values for an enum parameter),
** it calls on the client's callback function IMMEDIATELY with the
** appropriate command error code. NOTE that in this case the callback
** function is actually being called from the client context - NOT CM.
**
** If all parameters found to be valid, the command function queues a
** command to CM. When CM gets the command it checks whether the command
** is valid relative to the current state of the system.
**
** In case the command is not valid, CM calls on the client callback
** function with the appropriate command error code. NOTE that in this
** case the callback function is indeed being called from the CM context.
**
** If the command is successful, CM calls on the client callback function
** with an error code that indicates successful command. In addition CM
** notifies registered clients of the corresponding CM event.
*/

/* Type for BCMCS command callback functions.
**
** A client that wants to be notified of the result of a BCMCS command
** needs to supply a pointer to a function of this type as the first
** parameter to a BCMCS command function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, CM context simultaneously.
*/
typedef void (cm_bcmcs_cmd_cb_f_type) (

  void                      *data_block_ptr,
    /**< Pointer to client provided data block.
    **
    ** NOTE! Depending on the client implementation of this function
    ** the data block pointed by this pointer might be accessed from
    ** client, as well as, CM context simultaneously */

  cm_bcmcs_cmd_e_type        bcmcs_cmd,
    /**< Indicate which command is this error status for */

  cm_bcmcs_cmd_err_e_type    bcmcs_cmd_err
    /* Call command error code, indicating whether command
    ** is rejected and the reason */

);

/**===========================================================================
@FUNCTION cm_bcmcs_cmd_flow_request

@DESCRIPTION

  Queue CM_BCMCS_CMD_FLOW_REQUEST_F command to CM.
  This command is used to register BCMCS data flows with
  lower layer.

  If num_of_flows > 0 and flow_ids_array_ptr is not NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F will result in
  "update flow registration" command sent to the lower layer.

  If num_of_flows = 0 and flow_ids_array_ptr is NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F will result in
  "discontinue broadcast" command sent to the lower layer.


@DEPENDENCIES
  None

@RETURNS

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

@SIDE_EFFECT
  None

@x
===========================================================================*/
boolean cm_bcmcs_cmd_flow_request(

  cm_bcmcs_cmd_cb_f_type   cmd_cb_func,
    /**< client callback function */

  void                 *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type     client_id,
    /**< originating client */

  uint8                 num_of_flows,
    /**< Number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type* flow_ids_array_ptr
    /**< Array of <IP>:<port number> addresses for which flows should be requested */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_bcmcs_cmd_flow_request */
/*~ PARAM flow_ids_array_ptr VARRAY LENGTH num_of_flows */
#endif

boolean cm_bcmcs_cmd_flow_request2(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< originating client */

  uint8                           num_of_flows,
    /**< number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr,
    /**< array of IP:port number addresses for which flows should be requested */

  const sys_bcmcs_reg_setup_e_type *flow_reg_setup_array_ptr
    /**< whether non-required registration should be allowed for each flow */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_bcmcs_cmd_flow_request2 */
/*~ PARAM flow_ids_array_ptr VARRAY LENGTH num_of_flows */
/*~ PARAM flow_reg_setup_array_ptr VARRAY LENGTH num_of_flows */
#endif


boolean cm_bcmcs_cmd_registration_request(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< originating client */

  uint8                           num_of_flows,
    /**< number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr
    /**< array of IP:port number addresses for which flows should be registered */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_bcmcs_cmd_registration_request */
/*~ PARAM flow_ids_array_ptr VARRAY LENGTH num_of_flows */
#endif

boolean cm_bcmcs_cmd_registration_handoff(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< originating client */

  uint8                           num_of_flows,
    /**< number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr
    /**< array of IP:port number addresses for which flows should be registered */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_bcmcs_cmd_registration_handoff */
/*~ PARAM flow_ids_array_ptr VARRAY LENGTH num_of_flows */
#endif

boolean cm_bcmcs_cmd_bom_cache_setup(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< originating client */

  uint8                           stale_BOM_factor,
    /**< multiplier for staleness of BOM entry */

  sys_bcmcs_bom_caching_e_type    enable_strict_caching
    /**< enable / disable strict BOM caching */
);

/*===========================================================================
@FUNCTION cm_mbms_cmd_flow_deact_request

@DESCRIPTION
  This command is used to deact MBMS data flow requests with
  lower layers.

  
@DEPENDENCIES
  None

@RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

@SIDE EFFECTS
  None

@x

===========================================================================*/
boolean cm_mbms_cmd_flow_deact_request(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< originating client */

  uint8                           num_of_flows,
    /**< number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr
    /**< array of IP:port number addresses for which flows should be requested */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mbms_cmd_flow_deact_request */
/*~ PARAM flow_ids_array_ptr VARRAY LENGTH num_of_flows */
#endif

/** @}
*  endgroup BO BCMCS Object
*/


/****************************************************************************

MBMS Object

This object implements CM part of "Multicast Broadcast Multimedia Services"


****************************************************************************/

/** @defgroup MBMS Multicast Broadcast Multimedia Service Object
* @{
*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/
/* Enumeration of MBMS commands that clients are able to send to CM.
*/
typedef enum cm_mbms_cmd_e {

  CM_MBMS_CMD_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY! */

  CM_MBMS_CMD_GENERIC,
    /**< Generic MBMS command */


  CM_MBMS_CMD_MAX
    /**< FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_mbms_cmd_e_type;

/** Enumeration for MBMS command errors to be sent to clients when
** command errors are detected by CM.
** Clients should check for CM_API_MBMS before using this enum
*/
typedef enum cm_mbms_cmd_err_e {

  CM_MBMS_CMD_ERR_NONE=-1,
    /**< FOR INTERNAL USE OF CM ONLY! */

  /* 0 */
  CM_MBMS_CMD_ERR_NOERR,
    /**< No errors found */

  CM_MBMS_CMD_ERR_INVALID_COMMAND,
    /**< Command requested by client in mbms_generic_cmd is invalid */

  CM_MBMS_CMD_ERR_NO_BUF_L,
    /**< No available CM command buffers to queue the command */

  CM_MBMS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to FEATURE_MODEM_MBMS being undefined */

  CM_MBMS_CMD_ERR_MBMS_SRV_UNAVAILABLE,
    /**< MBMS service unavailable */

  /* 5 */ 
  CM_MBMS_CMD_ERR_OTHER,
    /**< Error other than any of the above */

  CM_MBMS_CMD_ERR_MAX
    /**< FOR INTERNAL USE OF CM ONLY! */


} cm_mbms_cmd_err_e_type;

/** Enumeration of MBMS events.
** Clients are able to register with CM to be notified of any subsets
** of these events.
** Clients should check for CM_API_MBMS before using this enum
*/
typedef enum cm_mbms_event_e {

  CM_MBMS_EVENT_NONE=-1,
    /**< FOR INTERNAL USE OF CM ONLY! */

  /* 0 */
  CM_MBMS_EVENT_GENERIC_CMD,
    /**< CM_MBMS_EVENT_GENERIC_CMD will be sent to registered clients
    ** whenever a MBMS command is issued to CM
    */

  CM_MBMS_EVENT_GENERIC_RPT,
    /**< CM_MBMS_EVENT_GENERIC_RPT will be sent to registered clients
    ** whenever lower layers send a MBMS report to CM.
    */


  CM_MBMS_EVENT_MAX
    /**< FOR INTERNAL USE OF CM ONLY! */


  #ifdef FEATURE_RPC
#error code not present
  #endif /**< FEATURE_RPC */

} cm_mbms_event_e_type;

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/** Type to hold MBMS information.
** When notified of MBMS events, clients are given
** a reference to such a structure.
** Clients should check for CM_API_MBMS before using this struct
*/

/** Struct to hold MBMS commands sent to the lower layers
*/
typedef struct cm_mbms_generic_cmd_s
{
   /**< MBMS command being sent by the client
   */
   sys_gw_mbms_req_e_type                          cmd_type;
   union {
     sys_gw_mbms_activate_req_s_type               activate_req;
     sys_gw_mbms_deactivate_req_s_type             deactivate_req;
     sys_gw_mbms_session_complete_ind_s_type       session_complete_ind;
     sys_gw_mbms_abort_req_s_type                  abort_req;
     sys_gw_mbms_suspend_req_s_type                suspend_req;
     sys_gw_mbms_resume_req_s_type                 resume_req;
     sys_gw_mbms_avail_srv_list_req_s_type         avail_srv_list_req;
     sys_gw_mbms_capability_req_s_type             capability_req;
     sys_gw_mbms_set_attr_req_s_type               set_attr;                    
    
   }cmd;   
} cm_mbms_generic_cmd_s_type ;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_mbms_generic_cmd_s.cmd DISC cm_mbms_generic_cmd_s.cmd_type */
/*~ CASE SYS_GW_MBMS_REQ_ACTIVATE  cm_mbms_generic_cmd_s.cmd.activate_req */
/*~ CASE SYS_GW_MBMS_REQ_DEACTIVATE  cm_mbms_generic_cmd_s.cmd.deactivate_req */
/*~ CASE SYS_GW_MBMS_REQ_SESSION_COMPLETE_IND  cm_mbms_generic_cmd_s.cmd.session_complete_ind */
/*~ CASE SYS_GW_MBMS_REQ_ABORT  cm_mbms_generic_cmd_s.cmd.abort_req */
/*~ CASE SYS_GW_MBMS_REQ_SUSPEND  cm_mbms_generic_cmd_s.cmd.suspend_req */
/*~ CASE SYS_GW_MBMS_REQ_RESUME  cm_mbms_generic_cmd_s.cmd.resume_req */
/*~ CASE SYS_GW_MBMS_REQ_AVAIL_SERVICES_LIST  cm_mbms_generic_cmd_s.cmd.avail_srv_list_req */
/*~ CASE SYS_GW_MBMS_REQ_GET_CAPABILITY cm_mbms_generic_cmd_s.cmd.capability_req */
/*~ CASE SYS_GW_MBMS_REQ_SET_ATTR  cm_mbms_generic_cmd_s.cmd.set_attr */
#endif

/**< Struct to hold MBMS report parameters sent by lower layers
*/
typedef struct cm_mbms_generic_rpt_s
{
  /**< MBMS report type */
  sys_gw_mbms_resp_e_type   rpt_type;
  union{
    sys_gw_mbms_resp_activate_cnf_s_type            activate_cnf;
    sys_gw_mbms_resp_deactivate_cnf_s_type          deactivate_cnf;
    sys_gw_mbms_resp_service_ind_s_type             service_ind;
    sys_gw_mbms_resp_capability_ind_s_type          capability_ind;
    sys_gw_mbms_resp_context_activated_ind_s_type   context_activated_ind;
    sys_gw_mbms_resp_context_deactivated_ind_s_type context_deactivated_ind;
    sys_gw_mbms_resp_abort_cnf_s_type               abort_cnf;
    sys_gw_mbms_resp_suspend_cnf_s_type             suspend_cnf;
    sys_gw_mbms_resp_resume_cnf_s_type              resume_cnf;
    sys_gw_mbms_resp_avail_srv_list_cnf_s_type      avail_srv_list_cnf;
    sys_gw_mbms_resp_ll_ready_ind_s_type            ll_ready_ind;
    sys_gw_mbms_resp_set_attr_s_type                set_attr;                    

  }rpt;
} cm_mbms_generic_rpt_s_type;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_mbms_generic_rpt_s.rpt DISC cm_mbms_generic_rpt_s.rpt_type */
/*~ CASE SYS_GW_MBMS_RESP_ACTIVATE_CNF  cm_mbms_generic_rpt_s.rpt.activate_cnf */
/*~ CASE SYS_GW_MBMS_RESP_DEACTIVATE_CNF  cm_mbms_generic_rpt_s.rpt.deactivate_cnf */
/*~ CASE SYS_GW_MBMS_RESP_SERVICE_IND  cm_mbms_generic_rpt_s.rpt.service_ind */
/*~ CASE SYS_GW_MBMS_RESP_CAPABILITY_IND  cm_mbms_generic_rpt_s.rpt.capability_ind */
/*~ CASE SYS_GW_MBMS_RESP_CONTEXT_ACTIVATED_IND cm_mbms_generic_rpt_s.rpt.context_activated_ind */
/*~ CASE SYS_GW_MBMS_RESP_CONTEXT_DEACTIVATED_IND  cm_mbms_generic_rpt_s.rpt.context_deactivated_ind */
/*~ CASE SYS_GW_MBMS_RESP_ABORT_CNF  cm_mbms_generic_rpt_s.rpt.abort_cnf */
/*~ CASE SYS_GW_MBMS_RESP_SUSPEND_CNF  cm_mbms_generic_rpt_s.rpt.suspend_cnf */
/*~ CASE SYS_GW_MBMS_RESP_RESUME_CNF  cm_mbms_generic_rpt_s.rpt.resume_cnf */
/*~ CASE SYS_GW_MBMS_RESP_AVAIL_SERVICES_LIST_CNF  cm_mbms_generic_rpt_s.rpt.avail_srv_list_cnf */
/*~ CASE SYS_GW_MBMS_RESP_LL_READY_IND  cm_mbms_generic_rpt_s.rpt.ll_ready_ind */
/*~ CASE SYS_GW_MBMS_RESP_SET_ATTR  cm_mbms_generic_rpt_s.rpt.set_attr */
#endif

typedef struct cm_mbms_info_s {

  cm_mbms_generic_rpt_s_type     mbms_generic_rpt;
    /**< MBMS generic report structure */

    cm_mbms_generic_cmd_s_type     mbms_generic_cmd;
    /**< MBMS generic command structure */

    cm_client_id_type               requesting_client_id;
    /**< Requesting client */

} cm_mbms_info_s_type;

/**--------------------------------------------------------------------------
** Macros
** ------------------------------------------------------------------------*/


/**--------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------*/

/* CM command functions
**
** The CM command functions defines the Client->CM interface.
**
** Each command function takes a pointer to a client provided callback
** function, a client provided void pointer and the command's required set
** of fields.
**
** If the command function find one or more of the parameters to contain
** invalid values ( e.g. not one of the enum values for an enum parameter),
** it calls on the client's callback function IMMEDIATELY with the
** appropriate command error code. NOTE that in this case the callback
** function is actually being called from the client context - NOT CM.
**
** If all parameters found to be valid, the command function queues a
** command to CM. When CM gets the command it checks whether the command
** is valid relative to the current state of the system.
**
** In case the command is not valid, CM calls on the client callback
** function with the appropriate command error code. NOTE that in this
** case the callback function is indeed being called from the CM context.
**
** If the command is successful, CM calls on the client callback function
** with an error code that indicates successful command. In addition CM
** notifies registered clients of the corresponding CM event.
*/

/* Type for  MBMS command callback functions 
**
** A client that wants to be notified of the result of a MBMS command
** needs to supply a pointer to a function of this type as the first
** parameter to a MBMS command function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, CM context simultaneously.
** Clients should check for CM_API_MBMS before using this function
*/

typedef void (cm_mbms_cmd_cb_f_type) (

  void                      *data_block_ptr,
    /**< Pointer to client provided data block.
    **
    ** NOTE! Depending on the client implementation of this function
    ** the data block pointed by this pointer might be accessed from
    ** client, as well as, CM context simultaneously */

  cm_mbms_cmd_e_type        mbms_cmd,
    /**< Indicate which MBMS request is this error status for */

  cm_mbms_cmd_err_e_type     mbms_cmd_err
    /**< Call command error code, indicating whether command
    ** is rejected and the reason */

);


/*===========================================================================

@FUNCTION cm_mbms_cmd_generic

@DESCRIPTION
  API to be used by MBMS Apps to send commands to MBMS Service Manager
  Clients should fill in the appropriate MBMS request type and also
  provide the corresponding parameters in the mbms_generic_req struct.
  Clients should check for CM_API_MBMS before using this function

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.


@RETURN VALUE
  TRUE   - Cmd has been queued to CM
  FALSE  - Cmd did not get queued to CM

@SIDE EFFECTS
  None

@x
===========================================================================*/
boolean cm_mbms_cmd_generic (

  cm_mbms_cmd_cb_f_type                cmd_cb_func,
    /**< client callback function */

  void                                 *data_block_ptr,
    /**< pointer to client callback data block */

  const cm_client_id_type              client_id,
    /**< originating client */

  const cm_mbms_generic_cmd_s_type    *mbms_generic_cmd
    /**< Structure to hold MBMS commands and their parameters */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mbms_cmd_generic */
/*~ PARAM mbms_generic_cmd POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/** @}
*  endgroup MBMS Multicast Broadcast Multimedia Service Object
*/

/****************************************************************************

Client Object

****************************************************************************/

/** @defgroup CO Client Object
** @{
*/

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

/* Anonymous client ID to be used by clients that are not interested
** in being notified of CM events and therefore do not allocate a
** an object of type cm_client_s_type in their own program space.
*/
#define CM_CLIENT_ID_ANONYMOUS  (cm_client_id_type) (-1)


/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/*
** Clients are pre-defined. Also, floating types are provided for clients that
** use and release CM as needed.
*/

typedef enum cm_client_type_e {

  CM_CLIENT_TYPE_NONE = -1,
    /**< @internal */

  /* 0 */
  CM_CLIENT_TYPE_UI,
    /**< Reserved for UI running on the same processor  */

  CM_CLIENT_TYPE_SMS,
    /**< Reserved for SMS running on the same processor  */

  CM_CLIENT_TYPE_DS,
    /**< Reserved for DATA SERVICES running on the same processor  */

  CM_CLIENT_TYPE_PD,
    /**< Reserved for position determination client on the same uP */

  /* 4 */
  CM_CLIENT_TYPE_ATCOP,
    /**< Reserved for DS ATCOP client on the same procesor */

  CM_CLIENT_TYPE_CAT_APP,
    /**< Reserved for USAT/CAT APP client on the same procesor */

  CM_CLIENT_TYPE_GSTK,
    /**< Reserved for GSTK client on the same procesor */

  CM_CLIENT_TYPE_FLOATING1,
    /**< Floating client 1 running on same/remote processor. */

  /* 8 */
  CM_CLIENT_TYPE_FLOATING2,
    /**< Floating client 2 running on same/remote processor. */

  CM_CLIENT_TYPE_FLOATING3,
    /**< Floating client 3 running on same/remote processor. */

  CM_CLIENT_TYPE_FLOATING4,
    /**< Floating client 4 running on same/remote processor. */

  CM_CLIENT_TYPE_DIAG,
    /**< Client used for diagnostic logging running on the same processor. */

  /* 12 */
  CM_CLIENT_TYPE_BREW,
    /**< Reserved for ICM */

  CM_CLIENT_TYPE_BLUETOOTH,
    /**< Reserved for bluetooth application */

  CM_CLIENT_TYPE_HDR_AN,
    /**< Reserved for HDR AN Authentication client on the same procesor */

  CM_CLIENT_TYPE_CAMERA,
    /**< Reserved for Camera */

  /* 16 */
  CM_CLIENT_TYPE_CMHM,
    /**< Reserved for Handover Manager */

  CM_CLIENT_TYPE_RM_NETWORK_INTERFACE,
    /**< Reserved for RM Interface Object*/

  CM_CLIENT_TYPE_DS_BCMCS,
    /**< Reserved for DS BCMCS client */

  CM_CLIENT_TYPE_MFLO,
    /**< Reserved for MediaFLO */

  /* 20 */
  CM_CLIENT_TYPE_OEMTELEPHONE,
    /**< Reserved for OEMTelephone */

  /* 21 */
  CM_CLIENT_TYPE_PBM,
    /**< Phone Book Manager, Check for CM_API_EMERGENCY_NUM_LIST before using
    ** this enum
    */

  /* 22 */
  CM_CLIENT_TYPE_END_LEGACY,
    /**< FOR INTERNAL USE OF CM ONLY!
    ** All new client types should be added AFTER this.
    */

  CM_CLIENT_TYPE_GSDI,
  /**< Reserved for GSDI */

  CM_CLIENT_TYPE_BREW_APPS,
  /**< Reserved for GSDI */

  CM_CLIENT_TYPE_OEMNET,
    /**< Reserved for OEMNet */

  CM_CLIENT_TYPE_HOMER,
   /**< Reserved for WLAN homer */

  CM_CLIENT_TYPE_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_client_type_e_type;


/** Enumeration of client API return status
*/
typedef enum cm_client_status_e {

  CM_CLIENT_ERR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_CLIENT_OK,
    /**< No error, request was completed successfully */

  CM_CLIENT_ERR_CLIENT_ID_PTR,
    /**< Invalid client ID pointer*/

  CM_CLIENT_ERR_CLIENT_ID,
    /**< Invalid client ID */

  CM_CLIENT_ERR_EVENT_REG_TYPE,
    /**< Invalid event registration type */

  /* 4 */
  CM_CLIENT_ERR_FROM_EVENT,
    /**< Invalid from event */

  CM_CLIENT_ERR_TO_EVENT,
    /**< Invalid to event */

  CM_CLIENT_ERR_CLIENT_INIT_FAIL,
    /**< Unable to initialize the client */

  CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  /* 8 */
  CM_CLIENT_ERR_PWR_COLLAPSE_TYPE,
    /**< Invalid power collapse type */

  CM_CLIENT_ERR_PROC_ID,
    /**< Invalid processor id */

  CM_CLIENT_ERR_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_client_status_e_type;


/** Enumeration of client event registration type
*/
typedef enum cm_client_event_reg_e {

  CM_CLIENT_EVENT_NONE=-1,
    /**< @internal */

  CM_CLIENT_EVENT_REG,
    /**< event registration */

  CM_CLIENT_EVENT_DEREG,
    /**< event de-registration */

  CM_CLIENT_EVENT_MAX
    /**< @internal */

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_client_event_reg_e_type;


/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/* SIP Response codes from RFC 3261
*/
typedef enum
{

  CM_IPAPP_CAUSE_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY!
    */

  CM_IPAPP_CAUSE_NORMAL = 0,
    /**< Indicates user requested call end.
    ** Cause also used when operation completes
    ** successfully.
    */

  CM_IPAPP_CAUSE_GENERAL_FAILURE = 1,
    /**< Used by CM and app to map end causes
    ** not listed in this enumeration.
    */

  /* Call end cause due to redirectionx */
  CM_IPAPP_CAUSE_MULTIPLE_CHOICES  = 300,
    /**< Request resolved in several choices, each with
    ** its own specific location. User can select from
    ** these to reidrect request
    */

  CM_IPAPP_CAUSE_MOVED_PERMANENTLY = 301,
    /**< User is no longer at the requested address and
   ** client should retry at new address given
   */


  CM_IPAPP_CAUSE_MOVED_TEMPORARILY = 302,
    /**< Requesting client should retry request at
    ** new address given. Expires header field gives
    ** the expiration time
    */


  CM_IPAPP_CAUSE_USE_PROXY = 305,
    /**< Requested resource needs to be accessed
    ** through a proxy given by the contact field
    */


  CM_IPAPP_CAUSE_ALTERNATE_SERVICE = 380,
    /**< Call was not successful, but alternate services
    ** are possible
    */


  /* Call end due to Request Failure 4xx */

  CM_IPAPP_CAUSE_BAD_REQUEST  = 400,
    /**< Malformed syntax in request
    */

  CM_IPAPP_CAUSE_UNAUTHORIZED = 401,
    /**< Request requires user
    ** authentication
    */

  CM_IPAPP_CAUSE_PAYMENT_REQUIRED = 402,
    /**< Future use
    */

  CM_IPAPP_CAUSE_FORBIDDEN        = 403,
    /**< Server refusing to fulfill request
    ** Request should not be repeated,
    */

  CM_IPAPP_CAUSE_NOT_FOUND        = 404,
    /**< User does not exist at domain
    ** specified or Domain in requested
    ** URI does not match domains handled by
    ** recipient of the request
    */

  CM_IPAPP_CAUSE_METHOD_NOT_ALLOWED = 405,
    /**< Method requested in address line
    ** not allowed for address identified
    ** by request-URI
    */

  CM_IPAPP_CAUSE_NOT_ACCEPTABLE     = 406,
    /**< Resource identified by request can
    ** only generate resp with content
    ** not acceptable
    */

  CM_IPAPP_CAUSE_PROXY_AUTHENTICATION_REQUIRED = 407,
    /**< Client must first authenticate with proxy
    */

  CM_IPAPP_CAUSE_REQUEST_TIMEOUT = 408,
    /**< Server could not respond within
    ** suitable amount of time
    */

  CM_IPAPP_CAUSE_GONE = 410,
    /**< Requested resource no longer available
    ** at server and no forwarding address
    */

  CM_IPAPP_CAUSE_REQUEST_ENTITY_TOO_LARGE = 413,
    /**< Request entity body is larger than what
    ** server is willing to process
    */

  CM_IPAPP_CAUSE_REQUEST_URI_TOO_LARGE    = 414,
    /**< Server refusing to service since request-URI
    ** is longer than server willing to interpret.
    */

  CM_IPAPP_CAUSE_UNSUPPORTED_MEDIA_TYPE   = 415,
    /**< Message body of request not in format
    ** supported by server for requested method
    */

  CM_IPAPP_CAUSE_UNSUPPORTED_URI_SCHEME   = 416,
    /**< Unsupported URI scheme
    */

  CM_IPAPP_CAUSE_BAD_EXTENSION            = 420,
    /**< Server did not understand protocol exten
    ** specified in proxy-required or require
    ** header field.
    */

  CM_IPAPP_CAUSE_EXTENSION_REQUIRED       = 421,
    /**< Extension to process a request is not
    ** listed in supported header field in
    ** request
    */

  CM_IPAPP_CAUSE_INTERVAL_TOO_BRIEF       = 422,
    /**< Expiration time of resource refreshed by
    ** request is too short
    */

  CM_IPAPP_CAUSE_TEMPORARILY_UNAVAILABLE  = 480,
    /**< End system contacted but callee is currently
    ** unavailable
    */

  CM_IPAPP_CAUSE_CALL_OR_TRANS_DOES_NOT_EXIST = 481,
    /**< Request received by UAS does not match
    ** any existing dialog or transaction
    */

  CM_IPAPP_CAUSE_LOOP_DETECTED              = 482,
    /**< Server detected a loop
    */

  CM_IPAPP_CAUSE_TOO_MANY_HOPS              = 483,
    /**< Request received has Max-Forwards header
    ** field at 0
    */

  CM_IPAPP_CAUSE_ADDRESS_INCOMPLETE         = 484,
    /**< Request had incomplete URI
    */

  CM_IPAPP_CAUSE_AMBIGUOUS                  = 485,
    /**< Request URI was ambiguous
    */

  CM_IPAPP_CAUSE_BUSY_HERE                  = 486,
    /**< Callee is not willing/unable to take
    ** additional calls at this end system
    */


  CM_IPAPP_CAUSE_REQUEST_TERMINATED         = 487,
    /**< Request terminated by a BYE/Cancel
    */

  CM_IPAPP_CAUSE_NOT_ACCEPTABLE_HERE        = 488,
    /**< Resource requested by request-URI not
    ** acceptable
    */

  CM_IPAPP_CAUSE_REQUEST_PENDING            = 491,
    /**< Request was received by UAS that had
    ** a pending request within same dialog
    */

  CM_IPAPP_CAUSE_UNDECIPHERABLE             = 493,
    /**< Request has an encrypted MIME body for which
    ** the recipient does not possess appropriate
    ** decryption key
    */


  /* Call end due to Server Failure 5xx */
  CM_IPAPP_CAUSE_SERVER_INTERNAL_ERROR      = 500,
    /**< Server hit an unexpected condition that
    ** prevented it from fulfilling the request
    */

  CM_IPAPP_CAUSE_NOT_IMPLEMENTED            = 501,
    /**< Server does not support functionality to
    ** fulfill request
    */

  CM_IPAPP_CAUSE_BAD_GATEWAY                = 502,
    /**< Server received invalid response from
    ** downstream gateway
    */

  CM_IPAPP_CAUSE_SERVER_UNAVAILABLE         = 503,
    /**< Server is temporarily unable to process the
    ** request due to overloading or maintanence
    */

  CM_IPAPP_CAUSE_SERVER_TIME_OUT            = 504,
    /**< Server did not receive timely response from
    ** external server it accessed
    */

  CM_IPAPP_CAUSE_VERSION_NOT_SUPPORTED      = 505,
    /**< Server does not support SIP protocol version
    ** used in the request
    */

  CM_IPAPP_CAUSE_MESSAGE_TOO_LARGE          = 513,
    /**< Server unable to process request since message
    ** length exceeded capabilities
    */

  /* Call end due to Global Failure 6xx */
  CM_IPAPP_CAUSE_BUSY_EVERYWHERE            = 600,
    /**< Callee's end system was contacted but callee
    ** is busy and does not wish to take call
    */

  CM_IPAPP_CAUSE_DECLINE                    = 603,
    /**< Callee's end system was contacted but user
    ** explicitly does not wish to or cannot
    ** participate.
    */

  CM_IPAPP_CAUSE_DOES_NOT_EXIST_ANYWHERE    = 604,
    /**< Server has information that user indicated in
    ** request-URI does not exist anywhere
    */

  CM_IPAPP_CAUSE_SESS_DESCR_NOT_ACCEPTABLE  = 605,
    /**< User's agent contacted but some aspects of
    ** session description not acceptable
    */

  CM_IPAPP_CAUSE_MAX = 0x7FFFFFFF
    /**< FOR INTERNAL USE OF CM ONLY!
    */

} cm_ipapp_cause_e_type;

/* Number conversion request types */
typedef enum cmutil_num_conv_req_e {

  CMUTIL_NUM_CONV_REQ_NONE = -1,
    /**< FOR INTERNAL USE OF CM ONLY! */

  CMUTIL_NUM_CONV_REQ_NO_REQ, /* !!!! WHY !!! */
    /**< Request to perform no conversion at all */

  CMUTIL_NUM_CONV_REQ_PLUS_TO_DIGITS,
    /**< Request to convert from plus to digits */

  CMUTIL_NUM_CONV_REQ_DIGITS_TO_PLUS,
    /**< Request to convert from digits to plus */

  CMUTIL_NUM_CONV_REQ_MAX
    /**< FOR INTERNAL USE OF CM ONLY! */

} cmutil_num_conv_req_e_type;



/* Result of resolving current country status */
typedef enum  {

  CMUTIL_CURRENT_COUNTRY_NONE = -1,
    /**< Bounds checking */

  CMUTIL_CURRENT_COUNTRY_RESOLVED,
    /**< Current country could be identified */

  CMUTIL_CURRENT_COUNTRY_CONFLICT,
    /**< Current country resolution ends
    ** up with 2 or more countries
    */

  CMUTIL_CURRENT_COUNTRY_FAILED,
    /**< Current country could be not identified */

  CMUTIL_CURRENT_COUNTRY_MAX
    /**< Bounds checking */

}cmutil_current_country_e_type;


/* Result of outgoing number conversion with plus */
typedef enum {

  CMUTIL_CONVER_RESULT_NONE = -1,
    /**< Bounds checking */

  CMUTIL_CONVER_RESULT_NUM_UNCHANGED = 0,
    /**< Number is unchanged */

  CMUTIL_CONVER_RESULT_NUM_MODIFIED,
    /**< Number has been modified */

  CMUTIL_CONVER_RESULT_NUM_INPUT_ERR,
    /**< Error in number given as Input */

  CMUNTIL_CONVER_RESULT_MAX
    /**< Bounds checking */

} cmutil_conver_result_e_type;

/* Result of country info request */
typedef enum {

  CMUTIL_COUNTRY_INFO_RESULT_NONE = -1,
    /**< Bounds checking */

  CMUTIL_COUNTRY_INFO_RESULT_FOUND = 0,
    /**< mcc is found in the coutry table */

  CMUTIL_COUNTRY_INFO_RESULT_NOT_FOUND = 1,
    /**< mcc is not found in country table*/

  CMUTIL_COUNTRY_INFO_RESULT_MCC_ERR = 2,
    /**< Error in mcc */

  CMUTIL_COUNTRY_INFO_RESULT_MAX
    /**< Bounds checking */

} cmutil_country_info_result_e_type;


/**< Length of international dialed digit */
#define  CMUTIL_IDD_DIGIT_LEN 7

/**< Length of national dialed digit */
#define CMUTIL_NDD_DIGIT_LEN    7

/**< Length of SPC */
#define CMUTIL_SPC_DIGIT_LEN    7

/**< Max allowed duplicate entries */
#define CMUTIL_MAX_ALLOWED_DUP_ENTRY  10


/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/
/* Structure to hold STATIC table
*/
struct country_specific_record_s
{
   uint16 sid1; 
   /**< Starting of Sid range inclusive of sid1*/

   uint16 sid2;
   /**< end of Sid range inclusive of sid2 */

   uint16 country_code;   
   /**< cc carries  country code directly, Array to accommodate a leading 0*/
                    /**< 0 can be neglected for now, used for some zones */

   char inter_dialed_digits[CMUTIL_IDD_DIGIT_LEN]; 
   /**< idd digits start after last F */

   char special_prefix_code[CMUTIL_SPC_DIGIT_LEN];
   /**< spc digits start after last F */

   char national_direct_dial[CMUTIL_NDD_DIGIT_LEN]; 
   /**< national direct dial */

   boolean is_network_soln; 
   /**< network solution */

   uint16  mobile_country_code;   
   /**< Mobile country code */
   
};

struct multi_country_specific_record_s
{
    uint8 entry_number;
    struct country_specific_record_s 
           country_entry[CMUTIL_MAX_ALLOWED_DUP_ENTRY];
};
typedef struct country_specific_record_s 
               cmutil_country_specific_record_s ;
typedef struct multi_country_specific_record_s 
               cmutil_multi_country_specific_record_s ;


/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/* Types for CM event callback functions.
**
** A client that want to be notified of CM events needs to register
** with the appropriate type of callback function with CM.
**
** CM calls on the client's callback functions when events to which the
** client is registered for occur. Each client is responsible for the
** implementation of its callback functions. Note that since callback
** functions are being called from CM context they are expected to
** only queue commands to their corresponding client and return. No extra
** processing is allowed and in particular no rex_wait() and alike.
**
** NOTE! callback functions are not allowed to change the content
** of the info structure which they are given a pointer.
**
** NOTE! clients are not allowed to access the info structure after
** the their callback functions return.
*/


/** Type for call event callback function.
**
** A client that want to be notified of call events needs to register
** with such a function with CM.
*/
typedef void (cm_mm_call_event_f_type) (

  cm_call_event_e_type         call_event,
    /***< Indicate call event */

  const cm_mm_call_info_s_type *call_info_ptr
    /***< A pointer to a call state information structure */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM call_info_ptr POINTER */
#endif

/** Type for phone event callback function.
**
** A client that want to be notified of phone events needs to register
** with such a function with CM. See "cm_client_ph_reg" for information on registration
*/
typedef void (cm_ph_event_f_type) (

  cm_ph_event_e_type           ph_event,
    /**< Indicate phone event */

  const cm_ph_info_s_type      *ph_info_ptr
    /**< A pointer to a phone state information structure */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM ph_info_ptr POINTER */
#endif

/** Type for BCMCS data event callback function.
**
** A client that wants to be notified of BCMCS data events needs to register
** such a function with CM.
*/
typedef void (cm_bcmcs_event_f_type) (

  cm_bcmcs_event_e_type            bcmcs_event,
    /**< Indicate BCMCS data event
    */

  const cm_bcmcs_info_s_type      *bcmcs_info_ptr
    /**< A pointer to a BCMCS data information structure
    */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM bcmcs_info_ptr POINTER */
#endif

/* Type for MBMS event callback function.
**
** A client that wants to be notified of MBMS data events needs to register
** such a function with CM.
** Clients should check for CM_API_MBMS before using this function
*/
typedef void (cm_mbms_event_f_type) (

  cm_mbms_event_e_type            mbms_event,
    /* Indicate MBMS data event
    */

  const cm_mbms_info_s_type      *mbms_info_ptr
    /* A pointer to a BCMCS data information structure
    */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM mbms_info_ptr POINTER */
#endif

/** Type for serving system event callback function.
**
** A client that want to be notified of serving system events needs to
** register with such a function with CM. See "cm_client_ss_reg" for information on registering for SS events with CM.
*/
typedef void (cm_mm_ss_event_f_type) (

  cm_ss_event_e_type           ss_event,
    /**< Indicate serving system event */

  const cm_mm_ss_info_s_type   *ss_info_ptr
    /**< A pointer to a serving system information structure */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM ss_info_ptr POINTER */
#endif

/** Type for inband event callback function.
**
** A client that want to be notified of inband events needs to
** register with such a function with CM.
*/
typedef void (cm_inband_event_f_type) (

  cm_inband_event_e_type           inband_event,
    /**< Indicate inband event */

  const cm_inband_info_s_type      *inband_info_ptr
    /**< A pointer to a inband information structure */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM inband_info_ptr POINTER */
#endif

/** Type for SMS event callback function.
**
** A client that want to be notified of inband events needs to
** register with such a function with CM.
*/
typedef void (cm_sms_event_f_type) (

  cm_sms_event_e_type           sms_event,
    /**< Indicate sms event */

  const cm_sms_info_s_type      *sms_info_ptr
    /**< A pointer to a sms information structure */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM sms_info_ptr POINTER */
#endif

/** Type for NAS event callback function.
**
** A client that want to be notified of NAS events needs to
** register with such a function with CM.
*/
typedef void (cm_nas_event_f_type) (

  cm_nas_event_e_type           nas_event,
    /**< Indicate nas event */

  const cm_nas_info_s_type      *nas_info_ptr
    /**< A pointer to a nas information structure */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM nas_info_ptr POINTER */
#endif

/** Type for PD event callback function.
**
** A client that want to be notified of PD events needs to
** register with such a function with CM.
*/
typedef void (cm_pd_event_f_type) (

  cm_pd_event_e_type           pd_event,
    /**< Indicate pd event */

  const cm_pd_info_s_type      *pd_info_ptr
    /**< A pointer to a pd information structure */

);


/** Type for DBM event callback function.
**
** A client that want to be notified of DBM events needs to
** register with such a function with CM.
*/
typedef void (cm_dbm_event_f_type) (

    cm_dbm_event_e_type           dbm_event,
        /**< Indicate pd event */

    const cm_dbm_info_s_type      *dbm_info_ptr
        /**< A pointer to a pd information structure */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM dbm_info_ptr POINTER */
#endif

/** Type for SUPS event callback function.
**
** A client that want to be notified of SUPS events needs to
** register with such a function with CM.
*/
typedef void (cm_sups_event_f_type) (

  cm_sups_event_e_type                 sups_event,
    /**< Indicate sups event */

  const cm_sups_info_s_type            *sups_info_ptr
    /**< A pointer to a sups information struct */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM sups_info_ptr POINTER */
#endif

/** Type for call info list callback function.
**
** A client that wants to be called with call information list needs to
** register such a function with CM.
*/
typedef void (cm_call_info_list_f_type) (

  const cm_call_state_info_list_s_type    *cm_call_info_list_ptr
    /**< Pointer to call info list struct */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM cm_call_info_list_ptr POINTER */
#endif


/** Type for call command error callback function.
**
** A client that want to be notified of OTHER clients call command errors
** needs to register with such a function with CM.
*/
typedef void (cm_mm_call_cmd_err_f_type) (

  cm_call_id_type              call_id,
    /**< call ID */

  cm_client_id_type            client_id,
    /**< Indicate which client caused the error */

  cm_call_cmd_e_type           cmd,
    /**< Indicate for which call command this error is reported */

  cm_call_cmd_err_e_type       cmd_err
    /**< Indicate nature of error */

);


/** Type for phone command error callback function.
**
** A client that want to be notified of OTHER clients phone command errors
** needs to register with such a function with CM. Refer to "cm_client_ph_reg"for
** information on registering this callback function
*/
typedef void (cm_ph_cmd_err_f_type) (

  cm_client_id_type       client_id,
    /**< Indicate which client caused the error */

  cm_ph_cmd_e_type        cmd,
    /**< Indicate for which phone command this error is reported */

  cm_ph_cmd_err_e_type    cmd_err
    /**< Indicate nature of error */

);

/** Type for BCMCS data command error callback function.
**
** A client that want to be notified of OTHER clients'
** BCMCS data command errors needs to register
** such a function with CM.
*/
typedef void (cm_bcmcs_cmd_err_f_type) (

  cm_client_id_type          client_id,
    /**< Indicate which client caused the error
    */

  cm_bcmcs_cmd_e_type        cmd,
    /**< Indicate for which BCMCS data command this error is reported
    */

  cm_bcmcs_cmd_err_e_type    cmd_err
    /**< Indicate nature of error
    */

);

/* Type for MBMS data command error callback function.
**
** A client that wants to be notified of OTHER clients'
** MBMS data command errors needs to register
** such a function with CM.
** Clients should check for CM_API_MBMS before using this function
*/
typedef void (cm_mbms_cmd_err_f_type) (

  cm_client_id_type          client_id,
    /* Indicate which client caused the error
    */

  cm_mbms_cmd_e_type        cmd,
    /* Indicate for which MBMS command this error is reported
    */

  cm_mbms_cmd_err_e_type    cmd_err
    /* Indicate nature of error
    */

);


/** Type for serving system command error callback function.
**
** A client that want to be notified of OTHER clients serving system command errors
** needs to register with such a function with CM. See "cm_client_ss_reg" for information on registering with CM.
*/
typedef void (cm_ss_cmd_err_f_type) (

  cm_client_id_type       client_id,
    /**< Indicate which client caused the error */

  cm_ss_cmd_e_type        cmd,
    /**< Indicate for which serving system command this error is reported */

  cm_ss_cmd_err_e_type    cmd_err
    /**< Indicate nature of error */

);


/** Type for inband command error callback function.
**
** A client that want to be notified of OTHER clients inband command errors
** needs to register with such a function with CM.
*/
typedef void (cm_mm_inband_cmd_err_f_type) (

  cm_call_id_type             call_id,
    /**< call ID */

  cm_client_id_type           client_id,
    /**< Indicate which client caused the error */

  cm_inband_cmd_e_type        cmd,
    /**< Indicate for which inband command this error is reported */

  cm_inband_cmd_err_e_type    cmd_err
    /**< Indicate nature of error */

);


/** Type for SMS command error callback function.
**
** A client that want to be notified of OTHER clients sms command errors
** needs to register with such a function with CM.
*/
typedef void (cm_sms_cmd_err_f_type) (

  cm_client_id_type           client_id,
    /**< Indicate which client caused the error */

  cm_sms_cmd_e_type           cmd,
    /**< Indicate for which sms command this error is reported */

  cm_sms_cmd_err_e_type       cmd_err
    /**< Indicate nature of error */

);


/*/8 Type for PD command error callback function.
**
** A client that want to be notified of OTHER clients pd command errors
** needs to register with such a function with CM.
*/
typedef void (cm_pd_cmd_err_f_type) (

  cm_client_id_type          client_id,
    /**< Indicate which client caused the error */

  cm_pd_cmd_e_type           cmd,
    /**< Indicate for which sms command this error is reported */

  cm_pd_cmd_err_e_type       cmd_err
    /**< Indicate nature of error */

);


/** Type for DBM command error callback function.
**
** A client that want to be notified of OTHER clients dbm command errors
** needs to register with such a function with CM.
*/
typedef void (cm_dbm_cmd_err_f_type) (

    cm_client_id_type           client,
        /**< Indicate which client caused the error */

    cm_dbm_cmd_e_type           cmd,
        /**< Indicate for which sms command this error is reported */

    cm_dbm_cmd_err_e_type       cmd_err
        /**< Indicate nature of error */

);

/** Type for SUPS command error callback function.
**
** A client that want to be notified of OTHER clients SUPS command errors
** needs to register with such a function with CM.
*/

typedef void (cm_sups_cmd_err_f_type) (

  cm_client_id_type            client_id,
    /**< Indicate which client caused the error */

  cm_sups_cmd_e_type           cmd,
    /**< Indicate for which sups command this error is reported */

  cm_sups_cmd_err_e_type       cmd_err
    /**< Indicate nature of error */

);

/** Type for NAS command error callback function.
**
** A client that want to be notified of OTHER clients NAS command errors
** needs to register with such a function with CM.
*/

typedef void (cm_nas_cmd_err_f_type) (

  cm_client_id_type            client_id,
    /**< Indicate which client caused the error */

  cm_nas_cmd_e_type           cmd,
    /**< Indicate for which sups command this error is reported */

  cm_nas_cmd_err_e_type       cmd_err
    /**< Indicate nature of error */

);



/**===========================================================================

@FUNCTION cm_client_init

@DESCRIPTION
  Initializing a client object. After initialization, a client is assigned a unique client ID.

  This function must be called exactly once on a client object before
  it is being used, in any way, place, or form.

  Note that in order to get notified of CM events a client still needs
  to do the following:
  1. Register its callback functions and specific events with each functional group
  2. Activate its client object with cm_client_act()

@DEPENDENCIES
  none

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern cm_client_status_e_type cm_client_init(

  cm_client_type_e_type           client_type,
    /**< Type of client */
  cm_client_id_type               *client_id_ptr
    /**< Client ID pointer*/

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_client_init 
    RELEASE_FUNC cm_client_release( *client_id_ptr )
    ONERROR return CM_CLIENT_ERR_CLIENT_INIT_FAIL */
/*~ PARAM OUT client_id_ptr POINTER */
#endif

/**===========================================================================

@FUNCTION cm_client_release

@DESCRIPTION
  Release a client object.

  This function should be called if a client doesn't need CM anymore.
  After this call, the client can no longer perform any CM functions.
  This call will release the client object. That object can be reused for
  another client. If the client needs to use CM again, it should call
  cm_client_init function to establish connection.

@DEPENDENCIES
  none

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern cm_client_status_e_type cm_client_release(

  cm_client_id_type               client_id
    /**< Client ID pointer*/

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_client_release */
#endif


/**===========================================================================

@FUNCTION cm_mm_client_call_reg

@DESCRIPTION
  This function allows a client to register the following call-related
  callbacks and events with CM:

   1. Callback function to receive registered call events
   2. Register and de-register call events
   3. Callback function to receive other clients' call-command errors.

   If a client registers a callback function to receive call event
   notification, it also needs to register one or more call events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of call events, a client needs to call on this API twice- once for each
   range of call events.

  If a client registers a callback function to receive other clients'
  call-command errors, it will receive notifications on all possible
  call-command errors caused by other clients. A client cannot register to
  receive only a subset of call-command errors.

  The NULL function pointer can also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' call-command errors, it can set
  call_cmd_err_func = NULL. However, if the call_event_func is set to NULL,
  all registered call events are also de-registered and the event_reg_type,
  from_call_event, and to_call_event parameters are ignored.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_call_reg(

  cm_client_id_type           client_id,
    /**< Requesting client */

  cm_mm_call_event_f_type     *call_event_func,
    /**< Pointer to a callback function to notify the client of call
    ** events */

  cm_client_event_reg_e_type  event_reg_type,
    /**< register or deregister */

  cm_call_event_e_type        from_call_event,
    /**< register from this event (including) */

  cm_call_event_e_type        to_call_event,
    /**< to this event (including) */

  cm_mm_call_cmd_err_f_type   *call_cmd_err_func
    /**< pointer to a callback function to notify the client of call
    ** command errors of OTHER clients */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_call_reg */
#endif


/**===========================================================================

@FUNCTION cm_mm_client_call_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_call_event and to_call_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of call events, a client needs to call on this API twice- once for each
  range of call events. In case of conflicting pwr_collapse_type, the last
  one will be honored


@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_call_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_mm_call_event_f_type      *call_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Registration type */

    cm_call_event_e_type          from_call_event,
        /**< register from this event (including) */

    cm_call_event_e_type          to_call_event
        /* to this event (including) */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_call_pwr_collapse_reg */
#endif



/*===========================================================================

@FUNCTION cm_client_ph_reg

@DESCRIPTION
  This function allows a client to register the following phone-related
  callbacks and events with CM:

   1. Callback function to receive registered phone events
   2. Register and de-register phone events
   3. Callback function to receive other clients' phone-command errors.

   If a client registers a callback function to receive phone event
   notification, it also needs to register one or more phone events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of call events, a client needs to call on this API twice- once for each
   range of call events.

  If a client registers a callback function to receive other clients'
  phone-command errors, it will receive notifications on all possible
  phone-command errors caused by other clients. A client cannot register to
  receive only a subset of phone-command errors.

  The NULL function pointer can also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' phone-command errors, it can set
  ph_cmd_err_func = NULL. However, if the ph_event_func is set to NULL,
  all registered phone events are also de-registered and the event_reg_type,
  from_ph_event, and to_ph_event parameters are ignored.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_client_ph_reg(

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_ph_event_f_type             *ph_event_func,
    /**< Pointer to a callback function to notify the client of phone
    ** events */

  cm_client_event_reg_e_type      event_reg_type,
    /**< register or deregister */

  cm_ph_event_e_type              from_ph_event,
    /**< register from this event (including) */

  cm_ph_event_e_type              to_ph_event,
    /**< to this event (including) */

  cm_ph_cmd_err_f_type           *ph_cmd_err_func
    /**< pointer to a callback function to notify the client of phone
    ** command errors of OTHER clients */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_client_ph_reg */
#endif

/**===========================================================================

@FUNCTION cm_mm_client_ph_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_ph_event and to_ph_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of ph events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored


@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_ph_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_ph_event_f_type           *ph_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Registration type */

    cm_ph_event_e_type            from_ph_event,
        /**< register from this event (including) */

    cm_ph_event_e_type            to_ph_event
        /* to this event (including) */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_ph_pwr_collapse_reg */
#endif


/**===========================================================================

@FUNCTION cm_mm_client_ss_reg

@DESCRIPTION
  This function allows a client to register the following serving system-related
  callbacks and events with CM:

   1. Callback function to receive registered serving system events
   2. Register and de-register serving system events
   3. Callback function to receive other clients' serving system-command errors.

   If a client registers a callback function to receive serving system event
   notification, it also needs to register one or more serving system events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of call events, a client needs to call on this API twice- once for each
   range of call events.

  If a client registers a callback function to receive other clients'
  serving system-command errors, it will receive notifications on all possible
  serving system-command errors caused by other clients. A client cannot register to
  receive only a subset of serving system-command errors.

  The NULL function pointer can also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' serving system-command errors, it can set
  ss_cmd_err_func = NULL. However, if the ss_event_func is set to NULL,
  all registered serving system events are also de-registered and the event_reg_type,
  from_ss_event, and to_ss_event parameters are ignored.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_ss_reg(

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_mm_ss_event_f_type          *ss_event_func,
    /**< Pointer to a callback function to notify the client of serving system
    ** events */

  cm_client_event_reg_e_type      event_reg_type,
    /**< register or deregister */

  cm_ss_event_e_type              from_ss_event,
    /**< register from this event (including) */

  cm_ss_event_e_type              to_ss_event,
    /**< to this event (including) */

  cm_ss_cmd_err_f_type           *ss_cmd_err_func
    /**< pointer to a callback function to notify the client of serving system
    ** command errors of OTHER clients */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_ss_reg */
#endif

/**===========================================================================

@FUNCTION cm_mm_client_ss_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_ss_event and to_ss_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of ss events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored


@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_ss_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_mm_ss_event_f_type        *ss_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Registration type */

    cm_ss_event_e_type            from_ss_event,
        /**< register from this event (including) */

    cm_ss_event_e_type            to_ss_event
        /* to this event (including) */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_ss_pwr_collapse_reg */
#endif

/**===========================================================================

@FUNCTION cm_client_inband_reg

@DESCRIPTION
  This function allows a client to register the following inband-related
  callbacks and events with CM:

   1. Callback function to receive registered inband events
   2. Register and de-register inband events
   3. Callback function to receive other clients' inband-command errors.

   If a client registers a callback function to receive inband event
   notification, it also needs to register one or more inband events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of inband events, a client needs to call on this API twice- once for each
   range of inband events.

  If a client registers a callback function to receive other clients'
  inband-command errors, it will receive notifications on all possible
  inband-command errors caused by other clients. A client cannot register to
  receive only a subset of inband-command errors.

  The NULL function pointer can also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' inband-command errors, it can set
  inband_cmd_err_func = NULL. However, if the inband_event_func is set to NULL,
  all registered inband events are also de-registered and the event_reg_type,
  from_inbnad_event, and to_inband_event parameters are ignored.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_inband_reg(

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_inband_event_f_type          *inband_event_func,
    /**< Pointer to a callback function to notify the client of inband
    ** events */

  cm_client_event_reg_e_type      event_reg_type,
    /**< register or deregister */

  cm_inband_event_e_type          from_inband_event,
    /**< register from this event (including) */

  cm_inband_event_e_type          to_inband_event,
    /**< to this event (including) */

  cm_mm_inband_cmd_err_f_type     *inband_cmd_err_func
    /**< pointer to a callback function to notify the client of inband
    ** command errors of OTHER clients */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_inband_reg */
#endif

/**===========================================================================

@FUNCTION cm_mm_client_inband_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_inband_event and to_inband_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of inband events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored


@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_inband_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_inband_event_f_type       *inband_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Registration type */

    cm_inband_event_e_type        from_inband_event,
        /**< register from this event (including) */

    cm_inband_event_e_type        to_inband_event
        /* to this event (including) */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_inband_pwr_collapse_reg */
#endif

/**===========================================================================

@FUNCTION cm_client_sms_reg

@DESCRIPTION
  This function allows a client to register the following sms-related
  callbacks and events with CM:

   1. Callback function to receive registered sms events
   2. Register and de-register sms events
   3. Callback function to receive other clients' sms-command errors.

   If a client registers a callback function to receive inband event
   notification, it also needs to register one or more inband events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of sms events, a client needs to call on this API twice- once for each
   range of sms events.

  If a client registers a callback function to receive other clients'
  sms-command errors, it will receive notifications on all possible
  sms-command errors caused by other clients. A client cannot register to
  receive only a subset of sms-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' sms-command errors, it can set
  sms_cmd_err_func = NULL. However, if the sms_event_func is set to NULL,
  all registered sms events are also de-registered and the event_reg_type,
  from_sms_event, and to_sms_event parameters are ignored.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().
  FEATURE_CDSMS must be defined.

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_client_sms_reg(

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_sms_event_f_type            *sms_event_func,
    /**< Pointer to a callback function to notify the client of sms
    ** events */

  cm_client_event_reg_e_type      event_reg_type,
    /**< register or deregister */

  cm_sms_event_e_type             from_sms_event,
    /**< register from this event (including) */

  cm_sms_event_e_type             to_sms_event,
    /**< to this event (including) */

  cm_sms_cmd_err_f_type          *sms_cmd_err_func
    /**< pointer to a callback function to notify the client of sms
    ** command errors of OTHER clients */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_client_sms_reg */
#endif

/**===========================================================================

@FUNCTION cm_mm_client_sms_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_sms_event and to_sms_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of sms events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored


@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_sms_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_sms_event_f_type          *sms_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Registration type */

    cm_sms_event_e_type           from_sms_event,
        /**< register from this event (including) */

    cm_sms_event_e_type           to_sms_event
        /* to this event (including) */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_sms_pwr_collapse_reg */
#endif

/**===========================================================================

@FUNCTION cm_mm_client_call_info_list_reg

@DESCRIPTION
  clients call this function to register a callback for
  receiving call information list from CM. CM api has to be
  seperately called to receive the information through callback.

@DEPENDENCIES
  cm_client_init must have been called previously.

@RETURNS
  results of the registration.

@SIDE_EFFECT
  None

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_call_info_list_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_call_info_list_f_type     *call_info_list_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_client_event_reg_e_type    event_reg_type,

    cm_mm_call_cmd_err_f_type    *call_info_list_cmd_err_func
      /**< pointer to a callback function to notify the client of call
      ** command errors of OTHER clients */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_call_info_list_reg */
#endif




/**===========================================================================

@FUNCTION cm_client_pd_reg

@DESCRIPTION
  This function is to be used by CM clients interested in registering for
  Position Determination call manager events.

@DEPENDENCIES
  cm_client_init must have been called previously.

@RETURNS
  results of the registration.

@SIDE_EFFECT
  None

@x
===========================================================================*/

cm_client_status_e_type cm_client_pd_reg(

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_pd_event_f_type             *pd_event_func,
    /**< Pointer to a callback function to notify the client of sms
    ** events */

  cm_client_event_reg_e_type      event_reg_type,
    /**< Is this a registration or de-reg */

  cm_pd_event_e_type              from_pd_event,
    /**< register from this event (including) */

  cm_pd_event_e_type              to_pd_event,
    /**< to this event (including) */


  cm_pd_cmd_err_f_type           *pd_cmd_err_func
    /**< pointer to a callback function to notify the client of sms
    ** command errors of OTHER clients */

);


/**===========================================================================

@FUNCTION cm_mm_client_pd_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_pd_event and to_pd_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of pd events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_pd_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_pd_event_f_type           *pd_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Is this a registration or de-reg */

    cm_pd_event_e_type            from_pd_event,
        /**< register from this event (including) */

    cm_pd_event_e_type            to_pd_event
          /* to this event (including) */
);

/**===========================================================================

@FUNCTION cm_client_dbm_event_reg

@DESCRIPTION
  This function is to be used by CM clients interested in registering for
  DBM call manager events.

@DEPENDENCIES
  cm_client_init must have been called previously.

@RETURNS
  results of the registration.

@SIDE_EFFECT
  None

@x
===========================================================================*/

cm_client_status_e_type cm_client_dbm_event_reg(

    cm_client_id_type               client,
        /**< Requesting client */

    cm_dbm_event_f_type             *dbm_event_func,
        /**< Pointer to a callback function to notify the client of dbm
        ** events */

    cm_client_event_reg_e_type      event_reg_type,
        /**< Is this a registration or de-reg */

    cm_dbm_event_e_type             from_dbm_event,
        /**< register from this event (including) */

    cm_dbm_event_e_type             to_dbm_event,
        /**< to this event (including) */


    cm_dbm_cmd_err_f_type           *dbm_cmd_err_func
      /**< pointer to a callback function to notify the client of dbm
      ** command errors of OTHER clients */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_client_dbm_event_reg */
/*~ PARAM dbm_event_func POINTER */
/*~ PARAM dbm_cmd_err_func POINTER */
#endif


/**===========================================================================

@FUNCTION cm_client_dbm_class_reg

@DESCRIPTION
  This function is to be used by CM clients interested in registering for
  DBM classes like PD, SDB etc..

@DEPENDENCIES
  cm_client_init must have been called previously.

@RETURNS
  results of the registration.

@SIDE_EFFECT
  None

@x
===========================================================================*/

cm_client_status_e_type cm_client_dbm_class_reg(

    cm_client_id_type               client,
        /**< Requesting client */

    cm_client_event_reg_e_type      event_reg_type,
        /**< Is this a registration or de-reg */

    cm_dbm_class_e_type             from_dbm_class,
        /**< register from this class (including) */

    cm_dbm_class_e_type             to_dbm_class
        /**< to this class (including) */

);


#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_client_dbm_class_reg */
#endif

/**===========================================================================

@FUNCTION cm_mm_client_dbm_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_dbm_event and to_dbm_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of dbm events, a client needs to call on this API twice- once for each
  range of dbm events. In case of conflicting pwr_collapse_type, the last
  one will be honored

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_dbm_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_dbm_event_f_type          *dbm_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Is this a registration or de-reg */

    cm_dbm_event_e_type           from_dbm_event,
        /**< register from this event (including) */

    cm_dbm_event_e_type           to_dbm_event
          /** to this event (including) */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_dbm_pwr_collapse_reg */
#endif


/**===========================================================================

@FUNCTION cm_client_sups_reg

@DESCRIPTION
  This function allows a client to register the following serving system-related
  callbacks and events with CM:

   1. Callback function to receive registered sups events
   2. Register and de-register sups events
   3. Callback function to receive other clients' sups command errors.

   If a client registers a callback function to receive sups event
   notification, it also needs to register one or more sups events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of sups events, a client needs to call this API twice- once for each
   range of sups events.

  If a client registers a callback function to receive other clients'
  sup command errors, it will receive notifications on all possible
  sups command errors caused by other clients. A client cannot register to
  receive only a subset of sups command errors.

  The NULL function pointer can also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' sups command errors, it can set
  sups_cmd_err_func = NULL. However, if the sups_event_func is set to NULL,
  all registered sups events are also de-registered and the event_reg_type,
  from_event, and to_event parameters are ignored.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_client_sups_reg(

  cm_client_id_type             client_id,
    /**< Requesting client */

  cm_sups_event_f_type          *sups_event_func,
    /**< Pointer to a callback function to notify the client of sups
    ** events */

  cm_client_event_reg_e_type    event_reg_type,
    /**< register or deregister */

  cm_sups_event_e_type          from_event,
    /**< register from this event (including) */

  cm_sups_event_e_type          to_event,
    /**< to this event (including) */

  cm_sups_cmd_err_f_type        *sups_cmd_err_func
    /**< pointer to a callback function to notify the client of sups
    ** command errors of OTHER clients */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_client_sups_reg */
#endif

/**===========================================================================

@FUNCTION cm_mm_client_sups_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_sups_event and to_sups_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of sups events, a client needs to call on this API twice- once for each
  range of sups events. In case of conflicting pwr_collapse_type, the last
  one will be honored

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_sups_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_sups_event_f_type         *sups_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Is this a registration or de-reg */

    cm_sups_event_e_type          from_sups_event,
        /**< register from this event (including) */

    cm_sups_event_e_type          to_sups_event
          /* to this event (including) */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_sups_pwr_collapse_reg */
#endif

/**===========================================================================
@FUNCTION cm_client_bcmcs_reg

@DESCRIPTION

  This function allows a client to register the following BCMCS
  callbacks and events with CM:

   1. Callback function to receive registered BCMCS events
   2. Register for and de-register from receiving BCMCS events
   3. Callback function to receive other clients' BCMCS command errors.

   If a client registers a callback function to receive BCMCS event
   notification, it also needs to register one or more BCMCS events
   it is interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of BCMCS events, a client needs to call on this API twice -
   once for each range of BCMCS events.

   If a client registers a callback function to receive other clients'
   BCMCS command errors, it will receive notifications on all possible
   BCMCS command errors caused by other clients. A client cannot
   register to receive only a subset of BCMCS command errors.

   The NULL function pointer can also be used for the callback function
   if a client is not interested in receiving that callback.
   For example, if a client is not interested in
   receiving other clients' BCMCS command errors, it can set
   bcmcs_cmd_err_func = NULL. However, if the bcmcs_event_func is set to NULL,
   all registered BCMCS events are also de-registered and the
   event_reg_type, from_bcmcs_event, and to_bcmcs_event parameters are ignored.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURNS
  Request status

@SIDE_EFFECT
  None

@x
===========================================================================*/
cm_client_status_e_type cm_client_bcmcs_reg(

    cm_client_id_type              client,
        /**< client id
        */

    cm_bcmcs_event_f_type         *bcmcs_event_func,
        /**< Pointer to a callback function to notify the client
        ** of BCMCS events
        */

    cm_client_event_reg_e_type     event_reg_type,
        /**< Registering or deregistering?
        */

    cm_bcmcs_event_e_type          from_bcmcs_event,
        /**< register from this event (including)
        */

    cm_bcmcs_event_e_type          to_bcmcs_event,
          /**< to this event (including)
          */

    cm_bcmcs_cmd_err_f_type        *bcmcs_cmd_err_func
      /**< pointer to a callback function to notify the client of BCMCS
      ** command errors of OTHER clients
      */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_client_bcmcs_reg */
/*~ PARAM bcmcs_event_func POINTER */
/*~ PARAM bcmcs_cmd_err_func POINTER */
#endif


/**===========================================================================

@FUNCTION cm_mm_client_bcmcs_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_bcmcs_event and to_bcmcs_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of bcmcs events, a client needs to call on this API twice- once for each
  range of bcmcs events. In case of conflicting pwr_collapse_type, the last
  one will be honored

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_bcmcs_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_bcmcs_event_f_type        *bcmcs_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Is this a registration or de-reg */

    cm_bcmcs_event_e_type         from_bcmcs_event,
        /**< register from this event (including) */

    cm_bcmcs_event_e_type         to_bcmcs_event
        /* to this event (including) */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_client_bcmcs_pwr_collapse_reg */
#endif


/**===========================================================================
@FUNCTION cm_client_mbms_reg

@DESCRIPTION

  This function allows a client to register the following MBMS
  callbacks and events with CM:

   1. Callback function to receive registered MBMS events
   2. Register for and de-register from receiving MBMS events
   3. Callback function to receive other clients' MBMS command errors.

   If a client registers a callback function to receive MBMS event
   notification, it also needs to register one or more MBMS events
   it is interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of BCMCS events, a client needs to call on this API twice -
   once for each range of MBMS events.

   If a client registers a callback function to receive other clients'
   MBMS command errors, it will receive notifications on all possible
   MBMS command errors caused by other clients. A client cannot
   register to receive only a subset of MBMS command errors.
   The NULL function pointer can also be used for the callback function
   if a client is not interested in receiving that callback.
   For example, if a client is not interested in
   receiving other clients' MBMS command errors, it can set
   mbms_cmd_err_func = NULL. However, if the mbms_event_func is set to NULL,
   all registered MBMS events are also de-registered and the
   event_reg_type, from_mbms_event, and to_mbms_event parameters are ignored.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURN VALUE
  Request status

@SIDE EFFECTS
  None

@x
===========================================================================*/
cm_client_status_e_type cm_client_mbms_reg(

    cm_client_id_type              client,
        /* client id
        */
    
    cm_mbms_event_f_type         *mbms_event_func,
        /* Pointer to a callback function to notify the client
        ** of MBMS events
        */
    
    cm_client_event_reg_e_type    event_reg_type,
        /* Registering or deregistering?
        */

    cm_mbms_event_e_type          from_mbms_event,
        /* register from this event (including)
        */

    cm_mbms_event_e_type          to_mbms_event,
          /* to this event (including)
          */

    cm_mbms_cmd_err_f_type        *mbms_cmd_err_func
      /* pointer to a callback function to notify the client of MBMS
      ** command errors of OTHER clients
      */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_client_mbms_reg */
/*~ PARAM mbms_event_func POINTER */
/*~ PARAM mbms_cmd_err_func POINTER */
#endif



/**===========================================================================
@FUNCTION cm_client_nas_reg

@DESCRIPTION

  This function allows a client to register the following NAS
  callbacks and events with CM:

   1. Callback function to receive registered NAS events
   2. Register for and de-register from receiving NAS events
   3. Callback function to receive other clients' NAS command errors.

   If a client registers a callback function to receive NAS event
   notification, it also needs to register one or more NAS events
   it is interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of NAS events, a client needs to call on this API twice -
   once for each range of BCMCS events.

   If a client registers a callback function to receive other clients'
   NAS command errors, it will receive notifications on all possible
   NAS command errors caused by other clients. A client cannot
   register to receive only a subset of NAS command errors.
   The NULL function pointer can also be used for the callback function
   if a client is not interested in receiving that callback.
   For example, if a client is not interested in
   receiving other clients' NAS command errors, it can set
   nas_cmd_err_func = NULL. However, if the nas_event_func is set to NULL,
   all registered NAS events are also de-registered and the
   event_reg_type, from_nas_event, and to_nas_event parameters are ignored.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURNS
  Request status

@SIDE_EFFECT
  None

@x
===========================================================================*/
cm_client_status_e_type cm_client_nas_reg(

    cm_client_id_type             client,
        /**< client id
        */

    cm_nas_event_f_type          *nas_event_func,
        /**< Pointer to a callback function to notify the client
        ** of NAS events
        */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Registering or deregistering?
        */

    cm_nas_event_e_type           from_nas_event,
        /**< register from this event (including)
        */

    cm_nas_event_e_type           to_nas_event,
          /**< to this event (including)
          */

    cm_nas_cmd_err_f_type        *nas_cmd_err_func
      /**< pointer to a callback function to notify the client of NAS
      ** command errors of OTHER clients
      */
);



/**===========================================================================

@FUNCTION cm_mm_client_nas_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_nas_event and to_nas_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of nas events, a client needs to call on this API twice- once for each
  range of nas events. In case of conflicting pwr_collapse_type, the last
  one will be honored

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_nas_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_nas_event_f_type          *nas_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Is this a registration or de-reg */

    cm_nas_event_e_type           from_nas_event,
        /**< register from this event (including) */

    cm_nas_event_e_type           to_nas_event
          /* to this event (including) */
);

/**===========================================================================

@FUNCTION cm_client_act

@DESCRIPTION
  Activate the client object, so that the client will be notified
  of call manager events (for which it is registered). Client-registered callback
  functions include the event callback function and other clients' command error
  callback functions. If the client is already activated, this request has no effect

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURNS
   request status

@SIDE_EFFECT
  Add the client object to the client list.

@x
===========================================================================*/
extern cm_client_status_e_type cm_client_act(

  cm_client_id_type               client_id
    /**< Requesting client */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_client_act */
#endif


/**===========================================================================

@FUNCTION cm_client_deact

@DESCRIPTION
  Deactivate a client object, so that the client will NOT be notified
  of call manager events (for which it is registered). Client-registered callback
  functions include the event callback function and other clients' command error
  callback functions. If the client is already disabled, this request has no effect.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURNS
  request status

@SIDE_EFFECT
  Remove the client object from the client list.

@x
===========================================================================*/
extern cm_client_status_e_type cm_client_deact(

  cm_client_id_type               client_id
    /**< Requesting client */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_client_deact */
#endif

/**===========================================================================

@FUNCTION cmclient_get_client_type

@DESCRIPTION
  Returns client id that corresponds to a client object. The client ID associated with
  any event is given by that CM event. This API allows a client to request that CM tell
  it what type of client is associated with a client_id.

@DEPENDENCIES
  None

@RETURNS
  client id

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern cm_client_type_e_type cmclient_get_client_type(

    cm_client_id_type client_id
    /**< Client object pointer */
);


/** @}
*  endgroup CO Client Object
*/

/****************************************************************************

Utility Functions

****************************************************************************/


/** @defgroup UF Utility Functions
** @{
*/


/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

#define CM_EXTDISP_MAX_SUBREC_LEN  20
  /* from ANSI T1.610 Annex D */


/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/* Extended Display subrecord - FEATURE_IS95B_EXT_DISP
*/
typedef struct {

  uint8 display_tag;
    /**< Indicator of the display info type */

  uint8 display_len;
    /**< The display length */

  uint8 chari[CM_EXTDISP_MAX_SUBREC_LEN];
    /**< display characters */

} cm_ext_disp_subrecord;


/** Extended Display subrecord iterator - FEATURE_IS95B_EXT_DISP
*/
typedef struct {

  cm_alpha_s_type*  info_ptr;
    /*~ FIELD cm_ext_disp_itr_type.info_ptr POINTER */
    /**< pointer to info being iterated */

  uint8 curr_offset;
    /**< byte offset of start of current record */

} cm_ext_disp_itr_type;
/*~ TYPE struct cm_ext_disp_itr_type */


/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/*                 MISC CONVERSION FUNCTIONS
*/

/**===========================================================================

@FUNCTION: cm_util_def_alphabet_to_ascii

@DESCRIPTION
  USE cm_util_gsm_alphabet_to_ascii INSTEAD. This function has bugs and
  exists for backward compatibility only.

  Convert the default alphabet to ASCII.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_def_alphabet_to_ascii(

    const byte    *default_alphabet_string,

    byte          num_chars,

    byte          *ascii_string
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_def_alphabet_to_ascii */
/*~ PARAM IN default_alphabet_string VARRAY LENGTH num_chars */
/*~ PARAM OUT ascii_string STRING MAX_DISPLAY_TEXT_LEN */
#endif

/**===========================================================================

@FUNCTION: cm_util_number_to_bcd

@DESCRIPTION
  Convert the phone number from ASCII to BCD

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_number_to_bcd(
    const cm_num_s_type *number,

    byte                *bcd_number
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_number_to_bcd */
/*~ PARAM number POINTER */
/*~ PARAM OUT bcd_number VARRAY CM_CALLED_PARTY_BCD_WITH_BCD_LEN 
    LENGTH (bcd_number[0]+1) */
#endif

/**===========================================================================

@FUNCTION: cm_util_bcd_to_ascii

@DESCRIPTION
  Convert the phone number from BCD to ASCII.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_bcd_to_ascii(
    const byte    *bcd_number,

    byte          *ascii_number
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_bcd_to_ascii */
/*~ PARAM IN bcd_number VARRAY CM_CALLED_PARTY_BCD_WITH_BCD_LEN 
    LENGTH (bcd_number[0]+1) */
/*~ PARAM OUT ascii_number STRING CM_MAX_NUMBER_CHARS */
#endif

/**===========================================================================

@FUNCTION: cm_util_ascii_to_def_alphabet

@DESCRIPTION
  USE cm_util_ascii_to_gsm_alphabet INSTEAD. This function has bugs and
  exists for backward compatibility only.

  Convert the ASCII to default alphabet.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_ascii_to_def_alphabet(
    byte          *default_alphabet_string,

    byte          num_chars,

    const byte   *ascii_string
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_ascii_to_def_alphabet */
/*~ PARAM OUT default_alphabet_string VARRAY MAX_DISPLAY_TEXT_LEN 
    LENGTH ((num_chars * 7)%8)?(((num_chars * 7)/8)+1):((num_chars * 7)/8) */
/*~ PARAM IN ascii_string VARRAY LENGTH num_chars */
#endif


/**===========================================================================

@FUNCTION: cm_util_pack

@DESCRIPTION
  USE cm_util_ussd_pack INSTEAD. This function has bugs and exists for
  backward compatibility only.

  Pack the bits in a byte.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_pack(
  byte         *coded_data,

  byte         num_chars,

  const byte  *string_to_convert
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_pack */
/*~ PARAM OUT coded_data VARRAY ((num_chars * 7)%8) ? (((num_chars * 7)/8)+1):((num_chars * 7)/8)
    LENGTH ((num_chars * 7)%8)?(((num_chars * 7)/8)+1):((num_chars * 7)/8) */
/*~ PARAM IN string_to_convert VARRAY LENGTH num_chars */
#endif


/**===========================================================================

@FUNCTION: cm_util_unpack

@DESCRIPTION
  USE cm_util_ussd_unpack INSTEAD. This function has bugs and exists for
  backward compatibility only.

  Unpack the bits into bytes.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_unpack(const byte *user_data, byte num_chars, byte *message_string);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_unpack */
/*~ PARAM IN user_data VARRAY LENGTH num_chars */
/*~ PARAM OUT message_string VARRAY ((num_chars * 8) % 7)?(((num_chars * 8)/7)+1):((num_chars * 8)/7)
    LENGTH ((num_chars * 8)%7)?(((num_chars * 8)/7)+1):((num_chars * 8)/7) */
#endif


/**===========================================================================

@FUNCTION: cm_util_ussd_pack

@DESCRIPTION
  Pack 7-bit GSM characters into bytes (8-bits)

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern byte cm_util_ussd_pack(

    byte *packed_data,

    const byte *str,

    byte num_chars
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_ussd_pack */
/*~ PARAM OUT packed_data VARRAY ((num_chars * 7)%8)?(((num_chars * 7)/8)+1):((num_chars * 7)/8)
    LENGTH ((num_chars * 7)%8)?(((num_chars * 7)/8)+1):((num_chars * 7)/8) */
/*~ PARAM IN str VARRAY LENGTH num_chars */
#endif

/**===========================================================================

@FUNCTION: cm_util_ussd_unpack

@DESCRIPTION
  Unpack the bytes (8-bit) into 7-bit GSM characters

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern byte cm_util_ussd_unpack(

    byte *str,

    const byte *packed_data,

    byte num_bytes
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_ussd_unpack */
/*~ PARAM OUT str VARRAY ((num_bytes * 8) % 7)?(((num_bytes * 8) / 7)+1):((num_bytes * 8)/7)
    LENGTH ((num_bytes * 8) % 7)?(((num_bytes * 8) / 7)+1):((num_bytes * 8)/7) */
/*~ PARAM IN packed_data VARRAY LENGTH num_bytes */
#endif

/**===========================================================================

@FUNCTION: cm_util_ascii_to_gsm_alphabet

@DESCRIPTION
  Convert the ASCII string to GSM default alphabets string and packs it
  into bytes.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern byte cm_util_ascii_to_gsm_alphabet(

    byte          *gsm_alphabet_string,

    const byte          *ascii_string,

    byte          num_chars
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_ascii_to_gsm_alphabet */
/*~ PARAM OUT gsm_alphabet_string VARRAY MAX_DISPLAY_TEXT_LEN 
    LENGTH ((num_chars * 7) % 8)?(((num_chars * 7) / 8) + 1):((num_chars * 7) / 8) */
/*~ PARAM IN ascii_string VARRAY LENGTH num_chars */
#endif

/**===========================================================================

@FUNCTION: cm_util_gsm_alphabet_to_ascii

@DESCRIPTION
  Unpacks bytes of data into 7-bit GSM default alphabet string and then
  converts it to an ASCII string.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern byte cm_util_gsm_alphabet_to_ascii(

    byte    *ascii_string,

    const byte    *gsm_alphabet_string,

    byte    num_bytes
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_gsm_alphabet_to_ascii */
/*~ PARAM OUT ascii_string STRING MAX_DISPLAY_TEXT_LEN */
/*~ PARAM IN gsm_alphabet_string VARRAY LENGTH num_bytes */
#endif



/**                M51 MODE SETTING TYPES & FUNCTIONS
** This block group all the functionality that is associated with
** M51 UI / DS interfacing.
**
** It is not desired that clients communicate with one another.
** Therefore CM provides API interface to both UI and DS to allow the user
** to change M51 settings.
**
** When the user selects M51 mode from UI menu, UI calls the following
** functions in CM:
** boolean cm_change_m511_mode(sys_jcdma_m511_type new_mode);
** boolean cm_change_m512_mode(sys_jcdma_m512_type new_mode);
** boolean cm_change_m513_mode(sys_jcdma_m513_type new_mode);
**
** In turn, CM looks at the M51 mode object and calls appropriate handler
** function that had been registered with it by DS.
**
** Note that CM expects DS to register M51 mode setting functions
** with CM during initialization time and those are expected to be
** synchronous functions.
**
** Until such callback functions are registered with CM, CM uses a default
** handlers that return FALSE to all M51 mode change requests.
**
*/


/* Types for M51 mode change functions.
*/
typedef boolean ( cm_m511_mode_settings_f_type ) (

  sys_jcdma_m511_e_type      new_mode

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cm_m511_mode_settings_f_type
  ONERROR return FALSE */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef boolean ( cm_m512_mode_settings_f_type ) (

  sys_jcdma_m512_e_type      new_mode

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cm_m512_mode_settings_f_type
  ONERROR return FALSE */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef boolean ( cm_m513_mode_settings_f_type ) (

  sys_jcdma_m513_e_type      new_mode

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cm_m513_mode_settings_f_type
  ONERROR return FALSE */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_change_m511_mode

@DESCRIPTION
  Request DS to change M511 mode via calling appropriate handler function
  in M51 mode object.
  Return TRUE if mode change succeeded and FALSE otherwise.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

@RETURNS
  TRUE/FALSE depending on whether mode change suceeded

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern boolean cm_change_m511_mode(sys_jcdma_m511_e_type new_mode);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_change_m511_mode */
#endif

/**===========================================================================

@FUNCTION cm_change_m512_mode

@DESCRIPTION
  Request DS to change M512 mode via calling appropriate handler function
  in M51 mode object.
  Return TRUE if mode change succeeded and FALSE otherwise.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

@RETURNS
  TRUE/FALSE depending on whether mode change suceeded

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern boolean cm_change_m512_mode(sys_jcdma_m512_e_type new_mode);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_change_m512_mode */
#endif


/**===========================================================================

@FUNCTION cm_change_m513_mode

@DESCRIPTION
  Request DS to change M513 mode via calling appropriate handler function
  in M51 mode object.
  Return TRUE if mode change succeeded and FALSE otherwise.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

@RETURNS
  TRUE/FALSE depending on whether mode change suceeded

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern boolean cm_change_m513_mode(sys_jcdma_m513_e_type new_mode);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_change_m513_mode */
#endif

/**===========================================================================
@FUNCTION cm_m51_mode_reg

@DESCRIPTION
  Register M51 mode change callback functions with CM.
  When cm_change_m511/512/513_mode() functions are called by UI, CM looks at
  M51 mode object and calls appropriate handler that had been registered
  for required mode change.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

@RETURNS
  TRUE

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern boolean cm_m51_mode_reg
(

  cm_m511_mode_settings_f_type     *change_m511_mode_func,
      /**< pointer to a DS function that changes M511 mode */

  cm_m512_mode_settings_f_type     *change_m512_mode_func,
      /**< pointer to a DS function that changes M512 mode */

  cm_m513_mode_settings_f_type     *change_m513_mode_func
      /**< pointer to a DS function that changes M513 mode */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_m51_mode_reg */
#endif



/**===========================================================================

@FUNCTION cm_is_valid_mode_band_pref

@DESCRIPTION
  Check whether a specified combination of mode and band preference is
  supported by the current target and the PRL that is associated with the
  currently selected NAM.

@DEPENDENCIES
  None

@RETURNS
  TRUE if the specified combination of mode and band preference is supported.
  FALSE otherwise.

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern boolean cm_is_valid_mode_band_pref(

  cm_mode_pref_e_type    mode_pref,
    /**< Mode preference to check */

  cm_band_pref_e_type    band_pref
    /**< Band preference to check */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_is_valid_mode_band_pref */
#endif



/**===========================================================================

@FUNCTION cm_is_valid_mode_pref

@DESCRIPTION
  Check whether a given mode preference is valid.

@DEPENDENCIES
  none

@RETURNS
  TRUE if mode preference is valid, FALSE otherwise.

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_is_valid_mode_pref(

  cm_mode_pref_e_type    mode_pref
    /**< mode preference to check */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_is_valid_mode_pref */
#endif


/**===========================================================================

@FUNCTION cm_is_sys_mode_digital

@DESCRIPTION
  This function determines whether the given system mode is a digital\
  mode.

@DEPENDENCIES
  none

@RETURNS
  TRUE if the system mode is digital, FALSE otherwise.

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_is_sys_mode_digital(

  sys_sys_mode_e_type    sys_mode
    /**< system mode to check */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_is_sys_mode_digital */
#endif


/**===========================================================================

@FUNCTION cm_get_pref_srv_type_for_packet_orig

@DESCRIPTION
  This function returns service type on which the next packet data call is
  likely to be originated on.

@DEPENDENCIES
  none

@RETURNS
  system mode.

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern sys_sys_mode_e_type cm_get_pref_srv_type_for_packet_orig(

  void

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_get_pref_srv_type_for_packet_orig */
#endif


/**===========================================================================

@FUNCTION cm_get_cur_nam

@DESCRIPTION
  Returns the current NAM.

@DEPENDENCIES
  Phone object must be initialized.

@RETURNS
  The current NAM. If phone was not yet initialized, return CM_NAM_NONE.

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern cm_nam_e_type  cm_get_curr_nam(

  void

);



/**===========================================================================

@FUNCTION CM_GET_CUR_HYBR_STATUS

@DESCRIPTION
  This function get the current  hybrid status

  Applicable for FEATURE_HDR_HYBRID.

@DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

@RETURNS
  TRUE- hybrid operation is enabled

@SIDE_EFFECT
  none

@x
===========================================================================*/
boolean cm_get_cur_hybr_status(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_get_cur_hybr_status */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/**===========================================================================

@FUNCTION cm_num_init

@DESCRIPTION
  Initialize a CM number object.

  This function must be called on a CM number object before it being used,
  in any way, place, or form.

@DEPENDENCIES

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_num_init(

  cm_num_s_type *num_ptr
    /**< The number to be initialized */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_num_init */
/*~ PARAM OUT num_ptr POINTER */
#endif


/**===========================================================================

@FUNCTION cm_num_fill

@DESCRIPTION
  Initialize a CM number object and fill it with a number.

@DEPENDENCIES

@RETURNS
  none

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern void cm_num_fill(

    cm_num_s_type          *num_ptr,
        /**< Pointer to a CM number to be initialized */
    const uint8            *num_buf,
        /**< Dialed address buffer.
        **
        ** NOTE! In the case where the digit_mode parameter is set to
        ** CM_DIGIT_MODE_4BIT_DTMF the number buffer may only contain
        ** 0-9, "#", or "*" characters.
        **
        ** In the case where the digit_mode parameter is set to
        ** CM_DIGIT_MODE_8BIT_ASCII the number buffer may contain any
        ** 8-bit ASCII characters */

    uint8                  num_len,
        /**< dialed address length */

    cm_digit_mode_e_type   digit_mode
        /**< digit mode */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_num_fill */
/*~ PARAM OUT num_ptr POINTER */
/*~ PARAM num_buf VARRAY LENGTH num_len */
#endif


/**===========================================================================

@FUNCTION cm_num_get

@DESCRIPTION
  Copy a CM number into a specified byte buffer.

@DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

@RETURNS
  The numbers digits actually copied.

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern uint8 cm_num_get(

  const cm_num_s_type    *cm_num_ptr,
    /**< Pointer to a CM number to copy from */

  uint8*                  num_buf,
    /**< Pointer to a byte buffer to copy CM number into */

  uint8                   max_len
    /**< Size of byte buffer */

);



/**===========================================================================

@FUNCTION cm_num_cmp

@DESCRIPTION
  Compare a CM number byte with a byte buffer number over a specified
  length.

@DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

@RETURNS
  TRUE if both numbers match over the specified compare length,
  FALSE otherwise.

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_num_cmp(

  const cm_num_s_type    *cm_num_ptr,
    /**< Pointer to a CM number */

  const uint8*           num_buf,
    /**< Pointer to a byte buffer number */

  unsigned int           cmp_len
    /**< Length to perform comparison over */

);



/**===========================================================================

@FUNCTION cm_num_cmp_exact

@DESCRIPTION
  Compare a CM number byte with a byte buffer for an exact match -
  i.e. length and pattern.

@DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

@RETURNS
  TRUE if both numbers match exactly, FALSE otherwise.

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_num_cmp_exact(

  const cm_num_s_type    *cm_num_ptr,
    /**< Pointer to a CM number */

  const uint8*                  num_buf,
    /**< Pointer to a byte buffer number */

  unsigned int           num_len
    /**< Length off byte buffer number */

);



/**===========================================================================

@FUNCTION cm_num_copy

@DESCRIPTION
  Copy a byte buffer number into a specified CM number.

@DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
void cm_num_copy(

    cm_num_s_type    *cm_num_ptr,
        /**< Pointer to a CM number */

    const byte*      num_buf,
        /**< Pointer to a number to be copied into CM number */

    uint8            num_len
        /**< Length of number to be copied */
);



/**===========================================================================

@FUNCTION cm_num_ep_msg_copy

@DESCRIPTION
  Copy an IS91 Extended Protocol Short Message into CM number.

@DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

@RETURNS
  none

@SIDE_EFFECT
  none

@x
===========================================================================*/
void cm_num_ep_msg_copy(

    cm_num_s_type    *cm_num_ptr,
        /**< Pointer to a CM number */

    const byte*      num_buf,
        /**< Pointer to a number to be copied into CM number */

    uint8            num_len
        /**< Length of number to be copied */
);



/**===========================================================================

@FUNCTION cm_num_ep_cli_copy

@DESCRIPTION
  Copy IS91 Extended Protocol Caller Line ID into a CM number.

@DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

@RETURNS
  none

@SIDE_EFFECT
  none

@x
===========================================================================*/
void cm_num_ep_cli_copy(

    cm_num_s_type    *cm_num_ptr,
        /**< Pointer to a CM number */

    const byte*      num_buf,
        /**< Pointer to a number to be copied into CM number */

    uint8            num_len
        /**< Length of number to be copied */
);

#if( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))


/**===========================================================================

@FUNCTION cm_num_attrib_copy

@DESCRIPTION
  Set the attributes of a CM number to a specified set of attributes.

@DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

@RETURNS
  none

@SIDE_EFFECT
  none

@x
===========================================================================*/
void cm_num_attrib_copy(

    cm_num_s_type           *num_ptr,
        /**< Pointer to a CM number */

    uint8                   number_type,
        /**< Number type */

    uint8                   number_plan,
        /**< Numbering Plan */

    uint8                   pi,
        /**< Presentation Indicator */

    uint8                   si,
        /**< Screening Indicator */

    cm_digit_mode_e_type    digit_mode
        /**< Origination digit mode */
);
#endif // #if( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))



/**===========================================================================

@FUNCTION cm_num_validate

@DESCRIPTION
  This function validates a number.

@DEPENDENCIES
  The number must have already been initialized with cm_num_init().

@RETURNS
  CM_NUM_ERR_NOERR is returned if the number is valid.  Otherwise, an
  appropriate error code is returned.

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_num_err_e_type cm_num_validate(

    const cm_num_s_type    *num_ptr
        /**< Pointer to a CM number to be asserted */
);



/**===========================================================================

@FUNCTION CM_EXT_DISP_ITR_RESET

@DESCRIPTION
  Resets the iterator that walks through the extended display subrecords
  which exist in a packed format in the alpha field of the call event info.

  Applicable for FEATURE_IS95B_EXT_DISP.

@DEPENDENCIES
  None.

@RETURNS
  None.

@SIDE_EFFECT
  Nothing.

@x
===========================================================================*/
extern void cm_ext_disp_itr_reset(

  cm_ext_disp_itr_type  *itr_ptr,
    /**< ptr to iterator */

  cm_alpha_s_type       *info_ptr
    /**< ptr to packed info structure */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ext_disp_itr_reset */
/*~ PARAM OUT itr_ptr POINTER */
/*~ PARAM info_ptr POINTER */
#endif



/**===========================================================================

@FUNCTION CM_EXT_DISP_ITR_NEXT

@DESCRIPTION
  Increments the iterator that walks through the extended display subrecords
  which exist in a packed format in the alpha field of the call event info.

  If the end was already reached, the iterator is unchanged.

  Applicable for FEATURE_IS95B_EXT_DISP.

@DEPENDENCIES
  Iterator is initialized using cm_ext_disp_itr_reset

@RETURNS
  none.

@SIDE_EFFECT
  Nothing.

@x
===========================================================================*/
extern void cm_ext_disp_itr_next(

  cm_ext_disp_itr_type *itr_ptr
    /**< pointer to iterator */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ext_disp_itr_next */
/*~ PARAM OUT itr_ptr POINTER */
#endif



/**===========================================================================

@FUNCTION CM_EXT_DISP_ITR_MORE

@DESCRIPTION
  Tests the iterator to see if it points to the end of the records.

  Applicable for FEATURE_IS95B_EXT_DISP.

@DEPENDENCIES
  Iterator is initialized using cm_ext_disp_itr_reset

@RETURNS
  FALSE if the iterator points past the end of the record, TRUE otherwise.

@SIDE_EFFECT
  Nothing.

@x
===========================================================================*/
extern boolean cm_ext_disp_itr_more(

  const cm_ext_disp_itr_type *itr_ptr
    /**< pointer to iterator */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ext_disp_itr_more */
/*~ PARAM itr_ptr POINTER */
#endif



/**===========================================================================

@FUNCTION CM_EXT_DISP_ITR_GET

@DESCRIPTION
  The contents of the current record pointed to by the iterator are
  copied into the supplied cm_ext_disp_subrecord structure.

  Applicable for FEATURE_IS95B_EXT_DISP.

@DEPENDENCIES
  Iterator is initialized using cm_ext_disp_itr_reset

@RETURNS
  none.

@SIDE_EFFECT
  Nothing.

@x
===========================================================================*/
extern void cm_ext_disp_itr_get(

  const cm_ext_disp_itr_type *itr_ptr,
    /**< pointer to iterator */

  cm_ext_disp_subrecord *subrec_ptr
    /**< area to copy next record to */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ext_disp_itr_get */
/*~ PARAM itr_ptr POINTER */
/*~ PARAM OUT subrec_ptr POINTER */
#endif


/**===========================================================================

@FUNCTION cm_get_current_acq_sys_mode

@DESCRIPTION
  This function returns the current system mode that is acquired.

@DEPENDENCIES
  none

@RETURNS
  system mode.

@SIDE_EFFECT
  none

@x
===========================================================================*/
sys_sys_mode_e_type cm_get_current_acq_sys_mode( void );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_get_current_acq_sys_mode */
#endif


/**=========================================================================*/

#define cm_flash_type_e_type     cm_call_sups_type_e_type
#define CM_FLASH_TYPE_EMERGENCY  CM_CALL_SUPS_TYPE_EMERGENCY_CALL
#define CM_CALL_TYPE_DATA        CM_CALL_TYPE_PS_DATA
#define CM_CALL_TYPE_PKT_DATA    CM_CALL_TYPE_PS_DATA

/**=========================================================================*/

#if ((defined(FEATURE_HDR_HYBRID) && defined(FEATURE_GPSONE)) || \
      defined(FEATURE_MM_SUPERSET))
/**===========================================================================

@FUNCTION cm_pd_cmd_fix_start

@DESCRIPTION
  Send a start fix command to CM. Places a request on the CM queue.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_pd_cmd_fix_start(

    cm_pd_cmd_cb_f_type   cmd_cb_func,
        /**< client callback function */

    void                  *data_block_ptr,
        /**< pointer to client callback data block */

    cm_client_id_type     client
        /**< requesting client ID */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_pd_cmd_fix_start */
#endif

/**===========================================================================

@FUNCTION cm_pd_cmd_fix_end

@DESCRIPTION
  Send a end fix command to CM. Places a request on the CM queue.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_pd_cmd_fix_end(

    cm_pd_cmd_cb_f_type   cmd_cb_func,
        /**< client callback function */

    void                  *data_block_ptr,
        /**< pointer to client callback data block */

    cm_client_id_type     client
        /**< requesting client ID */

    );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_pd_cmd_fix_end */
#endif
#endif /* FEATURE_HDR_HYBRID && FEATURE_GPSONE */

/**===========================================================================

@FUNCTION cm_init_wlan_pref

@DESCRIPTION
  Initialize WLAN preferences.

@DEPENDENCIES
  Clients check CM_API_WLAN for WLAN support.
  cm_num_init()

@RETURNS
  none

@SIDE_EFFECT
  none

@x
===========================================================================*/
void cm_wlan_pref_init(

    cm_wlan_pref_s_type  *wlan_pref
        /**< Pointer to a WLAN preference structure */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_wlan_pref_init */
/*~ PARAM OUT wlan_pref POINTER */
#endif


/**===========================================================================

@FUNCTION cm_is_sys_mode_allowed

@DESCRIPTION

  The function determines if the given sys_mode is included / conforms to
  given band and mode pref.

  For example, if the system mode is  SYS_SYS_MODE_GSM and  mode band
  preference is CM_MODE_PREF_GSM_WCDMA_ONLY / CM_BAND_PREF_GSM_850  the
  function returns TRUE but given SYS_SYS_MODE_GW it returns FALSE because
  CM_BAND_PREF_GSM_850 does not have WCDMA component.

@DEPENDENCIES
  None

@RETURNS
  TRUE  - If Valid
  FALSE - Otherwise

@SIDE_EFFECT
  none

@x
===========================================================================*/
boolean cm_is_sys_mode_allowed(

    cm_mode_pref_e_type   mode_pref,
      /**< The mode preference requested
      */

    cm_band_pref_e_type   band_pref,
      /**< The band preference requested
      */

    sys_sys_mode_e_type   sys_mode
      /**< The sys mode requested
      */
  );

/**===========================================================================

@FUNCTION cm_mm_call_cmd_recall_rsp

@DESCRIPTION
  This function carries response from client for a recall indication from
  network.The call id passed has range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  ((FEATURE_WCDMA || FEATURE_GSM) && FEATURE_CCBS) must be defined.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_recall_rsp (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /**< client callback function */

  void                          *data_block_ptr,
    /**< pointer to client callback data block */

  cm_call_id_type               call_id,
    /**< call ID of the call */

  cm_client_id_type             client_id,
    /**< commanding client's id */

  cm_ccbs_recall_rsp_e_type     ccbs_recall_rsp
    /**< client response to recall indication */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_recall_rsp */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_mm_call_cmd_modify_req

@DESCRIPTION
  Api used to request in-call (connected state) modification of call type.
  Call needs to be connected for this request to be processed.
  The call id passed has range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  ((FEATURE_WCDMA || FEATURE_GSM) && CM_API_VT_FALLBACK_TO_VOICE) must be
  defined.

@RETURNS
  TRUE   - Cmd has been queued to CM
  FALSE  - Cmd did not get queued to CM

@SIDE_EFFECT
  None

@x
===========================================================================*/
boolean cm_mm_call_cmd_modify_req (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /**< client callback function */

  void                          *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type             client_id,
    /**< commanding client's id */

  cm_call_id_type               call_id,
    /**< call ID of the call */

  cm_call_type_e_type           new_call_type_req
    /**< New call type requested for call modification */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_modify_req */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION cm_mm_call_cmd_modify_res

@DESCRIPTION
  Response to network initiated in-call modification request.
  Call needs to be connected for this request to be processed.
  The call id passed has range from 0 to CM_CALL_ID_MAX.
  The client ID should be from a registered CM client.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  ((FEATURE_WCDMA || FEATURE_GSM) && CM_API_VT_FALLBACK_TO_VOICE) must be
  defined.

@RETURNS
  TRUE   - Cmd got queued to CM
  FALSE  - Cmd did not get queued to CM

@SIDE_EFFECT
  None

@x
===========================================================================*/
boolean cm_mm_call_cmd_modify_res (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /**< client callback function */

  void                          *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type             client_id,
    /**< commanding client's id */

  cm_call_id_type               call_id,
    /**< call ID of the call */

  boolean                       gw_cs_modify_res,
    /**< Client response to modification request
    ** TRUE   - Modification is accepted by client
    ** FALSE  - Modification gets rejected by client
    */

  ie_cm_cc_cause_s_type         *cc_cause_ptr
    /**< Specific cause value when gw_cs_modify_res is FALSE
    ** Ignored when gw_cs_modify_res is TRUE.
    */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_mm_call_cmd_modify_res */
/*~ PARAM cc_cause_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/**===========================================================================

@FUNCTION cm_get_user_data

@DESCRIPTION
  Call back client uses to get details regarding user data.

@DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

@RETURN VALUE
  none

@SIDE EFFECTS
  none

===========================================================================*/
extern void cm_get_user_data
(
  const cm_mm_call_info_s_type      *call_info_ptr,
    /**< pointer to call info pointer sent to the clients */

  cm_call_event_user_data_s_type    *call_event_user_data
    /**< pointer to a client before for user data details */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_get_user_data       */
/*~ PARAM call_info_ptr POINTER             */
/*~ PARAM OUT call_event_user_data POINTER  */
#endif /* FEATURE_HTORPC_METACOMMENTS       */

/**===========================================================================

@FUNCTION cm_ph_net_list_copy

@DESCRIPTION
  This function copies networks lists from PH event info to the client when
  requested bv client.

@DEPENDENCIES
  None

@RETURNS

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_ph_net_list_copy
(
  cm_ph_net_lists_info_s_type   *info_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_ph_net_list_copy */
/*~ PARAM OUT info_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*========================================================================

FUNCTION CM_UNLOCK_HDR_RESOURCES

DESCRIPTION

  This function unconditionally sends DO an advise unlock notification

Limitation:

  The API is restricted to be used by 1XCP module when
  FEATURE_FULL_TIME_SHDR_3 & FEATURE_HDR_HYBRID are defined

  It is not recommended to be used by any other clients.

DEPENDENCIES
  The HDRMC tasks must have been started up already.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
extern void cm_unlock_hdr_resources(void);

/*===========================================================================
                          CMUTIL FUNCTION PROTOTYPES
===========================================================================*/

/*===========================================================================

@FUNCTION: cm_util_subaddr_to_ascii

@DESCRIPTION
  Convert the subaddress to ASCII.

@DEPENDENCIES
  None.

@RETURN VALUE
  None

@SIDE EFFECTS
  None

===========================================================================*/
extern void cm_util_subaddr_to_ascii(
    byte   *subaddress,
    byte   *ascii_subaddress,
    byte   length);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_subaddr_to_ascii */
/*~ PARAM IN subaddress VARRAY LENGTH length  */
/*~ PARAM OUT ascii_subaddress STRING CM_MAX_SUBADDRESS_DIGITS */
#endif


/*===========================================================================

@FUNCTION: cm_util_default_cdma_orig_params

@DESCRIPTION
  Return a cm_cdma_orig_params_s_type  of default orig parameters

@DEPENDENCIES
  None.

@RETURN VALUE
  None

@SIDE EFFECTS
  None

===========================================================================*/
extern void cm_util_default_cdma_orig_params(
  cm_cdma_orig_params_s_type         *cdma_orig_param_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_default_cdma_orig_params */
/*~ PARAM OUT cdma_orig_param_ptr POINTER */
#endif

/*===========================================================================

@FUNCTION: cm_util_default_gw_orig_params

@DESCRIPTION
  Return a cm_cdma_orig_params_s_type  of default orig parameters

@DEPENDENCIES
  None.

@RETURN VALUE
  None

@SIDE EFFECTS
  None

===========================================================================*/
extern void cm_util_default_gw_orig_params(
  cm_gw_cs_orig_params_s_type         *gw_orig_param_ptr
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_default_gw_orig_params */
/*~ PARAM OUT gw_orig_param_ptr POINTER */
#endif



/*===========================================================================

@FUNCTION: cm_util_set_default_voice_bearer_cap

@DESCRIPTION
  This function sets the default bearer capability for a  Voice call.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None

===========================================================================*/
extern void cm_util_set_default_voice_bearer_cap(

  cm_bearer_capability_s_type    *bc
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_set_default_voice_bearer_cap */
/*~ PARAM OUT bc POINTER */
#endif

/*===========================================================================

@FUNCTION CM_UTIL_ENCODE_DTMF

@DESCRIPTION
  Populates the dtmf equivalent of given ascii value.

@DEPENDENCIES
  None

@RETURN VALUE
  TRUE  -  dtmf_val contains the converted value.
  FALSE -  dtmf_val contains 0 (represented by CMUTIL_DTMF_FOR_DIAL_0).

@SIDE EFFECTS
  None

===========================================================================*/
extern boolean cm_util_encode_dtmf
(
  uint8 ascii_val,
    /* Ascii value */

  uint8 *dtmf_val_ptr
    /* dtmf value */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_encode_dtmf */
/*~ PARAM OUT dtmf_val_ptr POINTER */
#endif

/*===========================================================================

@FUNCTION CM_UTIL_DTMF_TO_ASCII

@DESCRIPTION
  Populates the ascii equivalent of given DTMF value.

@DEPENDENCIES
  None

@RETURN VALUE
  TRUE  -  ascii_val contains the converted value.
  FALSE -  sscii_val contains 0 (represented by CMUTIL_DTMF_FOR_DIAL_0).

@SIDE EFFECTS
  None

===========================================================================*/
extern boolean cm_util_dtmf_to_ascii
(

  uint8 dtmf_val,
    /* Ascii value */

  uint8 *ascii_val_ptr
    /* ascii value */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_dtmf_to_ascii */
/*~ PARAM OUT ascii_val_ptr POINTER */
#endif

/*===========================================================================

@FUNCTION: cm_util_perform_num_conversion

@DESCRIPTION
  This function performs the following conversions

   - For CONV_REQ_PLUS_TO_DIGITS replaces starting "+" of number with
     digits.

   - For CONV_REQ_DIGITS_TO_PLUS replaces specific length of starting
     digits with "+".


@DEPENDENCIES
  None.

@RETURN VALUE
  converted_num

@SIDE EFFECTS
  None

===========================================================================*/
extern cmutil_conver_result_e_type cm_util_perform_num_conversion(

  cmutil_num_conv_req_e_type               num_conv_req,
    /* Convert from plus to digits or digits to plus */

  const cm_num_details_for_conversion_s_type      *num_details_for_conv,
    /* Holds actual number on which conversion has
    ** to be performed
    */

  cm_num_s_type                             *converted_num
    /* Converted number as a result of operation
    */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_perform_num_conversion */
/*~ PARAM num_details_for_conv POINTER */
/*~ PARAM OUT converted_num POINTER */
#endif

/*===========================================================================

@FUNCTION: cm_util_init_num_conversion_params

@DESCRIPTION
  Initializes parameters that will be used in num conversion function

@DEPENDENCIES
  None.

@RETURN VALUE
  Initialized parameters

@SIDE EFFECTS
  None
===========================================================================*/
extern void cm_util_init_num_conversion_params(

  cmutil_num_conv_req_e_type               *num_conv_req_ptr,
    /* Convert from plus to digits or digits to plus */

  cm_num_details_for_conversion_s_type      *num_details_for_conv
    /* Holds actual number on which conversion has
    ** to be performed
    */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_init_num_conversion_params */
/*~ PARAM OUT num_conv_req_ptr POINTER */
/*~ PARAM OUT num_details_for_conv POINTER */
#endif


/*===========================================================================

@FUNCTION: cmutil_populate_country_list

@DESCRIPTION
  API returns list of countries that match given sid, ltm offset and daylight
  saving settings.

@DEPENDENCIES
  None.

@RETURN VALUE
  countries_with_sid_ltm_cnft contains list of countries.

@SIDE EFFECTS
  None
===========================================================================*/
extern void cmutil_populate_country_list (

  uint16 curr_sid,
    /* sid of a system */

  int8 curr_ltm,
    /* Ltm offset of system */

  sys_daylt_savings_e_type curr_daylt,
    /* Day light savings information */

  cm_ss_country_list_s_type *countries_with_sid_ltm_cnft
    /*  Countries conflicting on given sid and ltm */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmutil_populate_country_list */
/*~ PARAM OUT countries_with_sid_ltm_cnft POINTER */
#endif
/*===========================================================================

@FUNCTION: cmutil_identify_current_country

@DESCRIPTION
  API returns list of countries that match given sid, ltm offset and daylight
  saving settings.

@DEPENDENCIES
  None.

@RETURN VALUE
  countries_with_sid_ltm_cnft contains list of countries.

@SIDE EFFECTS
  None
===========================================================================*/
extern cmutil_current_country_e_type cmutil_identify_current_country (

  uint16 curr_sid,
  /* sid of a system */

  int8 curr_ltm,
  /* Ltm offset of system */

  sys_daylt_savings_e_type curr_daylt,
    /* daylight savings infomration */

  uint16 *curr_mcc
    /* MCC of country with given information */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmutil_identify_current_country */
/*~ PARAM OUT curr_mcc POINTER */
#endif

/*===========================================================================
@FUNCTION: cmutil_replace_plus_with_digits

@DESCRIPTION
  Given number details that are required for  conversion from plus to digits,
  converted_num_ptr returns the converted number.

@DEPENDENCIES
  None.

@RETURN VALUE
  countries_with_sid_ltm_cnft contains list of countries.

@SIDE EFFECTS
  None
===========================================================================*/
extern cmutil_conver_result_e_type cmutil_replace_plus_with_digits (

  const cm_num_details_for_conversion_s_type  *num_details_for_conv_ptr,
    /* Details required to convert number into plus digits */

  cm_num_s_type                               *converted_num_ptr
    /* Number with digits converted from plus to digits */

); /* cmutil_replace_plus_with_digits */
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmutil_replace_plus_with_digits */
/*~ PARAM num_details_for_conv_ptr POINTER */
/*~ PARAM OUT converted_num_ptr POINTER */
#endif

/*===========================================================================
@FUNCTION: cmutil_replace_digits_with_plus

@DESCRIPTION
  Given number details that are required for  conversion from digits to plus,
  converted_num_ptr returns the converted number.

@DEPENDENCIES
  None.

@RETURN VALUE
  countries_with_sid_ltm_cnft contains list of countries.

@SIDE EFFECTS
  None
===========================================================================*/
cmutil_conver_result_e_type cmutil_replace_digits_with_plus (

  const cm_num_details_for_conversion_s_type  *num_details_for_conv_ptr,
    /* Details required to convert number for plus digits */

  cm_num_s_type                               *converted_num_ptr
    /* Number with digits converted from digits to plus */

); /* cmutil_replace_digits_with_plus */
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmutil_replace_digits_with_plus */
/*~ PARAM num_details_for_conv_ptr POINTER */
/*~ PARAM OUT converted_num_ptr POINTER */
#endif

/*===========================================================================

@FUNCTION: cm_util_country_info_for_dialing

@DESCRIPTION
  provide country information for plus coding dialing but actully  assisted dialing.  

@DEPENDENCIES
  None.

@RETURN VALUE
 
  country_info_ptr carries country information corresponding to mcc 


@SIDE EFFECTS
  None
===========================================================================*/
cmutil_country_info_result_e_type cm_util_country_info_for_dialing(

  cm_country_code_type req_mcc,
    /* Requested MCC, input is invalid mcc, we try to provide serving mcc  */ 

  cmutil_multi_country_specific_record_s *country_info_ptr
    /* Country network information corresponding MCC*/ 

); /*cm_util_country_info_for_dialing*/
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_util_country_info_for_dialing */
/*~ PARAM OUT country_info_ptr POINTER */
#endif

/*===========================================================================

@FUNCTION: cm_get_timing_advance  

@DESCRIPTION
  To get timing_advance information

@DEPENDENCIES
  None.

@RETURN VALUE
   TRUE :   Success
   FALSE:   Failure

@SIDE EFFECTS
  None
===========================================================================*/
extern boolean cm_get_timing_advance(

  uint32 *timing_advance_ptr 
    /* To get timing_advance information */ 

); /*cm_get_timing_advance*/
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_get_timing_advance */
/*~ PARAM OUT timing_advance_ptr POINTER */
#endif


/*===========================================================================

@FUNCTION: cm_get_cdma_position_info   

@DESCRIPTION
  To get UE current CDMA Cell position information  

@DEPENDENCIES
  None.

@RETURN VALUE
   TRUE :  Success
   FALSE:  Failure

@SIDE EFFECTS
  None
===========================================================================*/
extern boolean cm_get_cdma_position_info(

  cm_cdma_position_info_s_type   *position_info_ptr
   /* pointer to cdma position info sent to client */

); /*cm_get_cdma_position_info*/
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cm_get_cdma_position_info */
/*~ PARAM OUT position_info_ptr POINTER */
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CM_H */

/** @}
*  endgroup UF Utility Functions
*/
