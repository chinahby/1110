
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   M O D U L E

GENERAL DESCRIPTION
  This module makes up the Call Manager of the subscriber station software.

  The Call Manager provides out of bound call control and event notification
  services to its clients.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1998 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/src/cm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/09   aj      Add check to reject call if srv type is invalid
05/20/09   ks      Provide new API to get timing_advance and current 
                   CDMA position info through CM
05/18/09   em      Corrected debug message
04/09/09   aj      Added support for IRAT measurements
03/05/09   rk      Fixing cm_get_pref_srv_type_for_packet_orig to return WLAN
             when phone mode preference is set to WLAN_ONLY
02/17/09   mh      Obsoleted avoid_time in cm_ph_cmd_avoid_sys()
02/12/09   mh      Added support for no SR after MAP for IS-707B
01/12/09   rm      Adding log messages for SUPS error case
12/08/08   fj      Removed function cm_get_mode_band_capability()
12/21/08   rn      Feature renamed for FDN
12/21/08   rn      FDN checking option in call control
12/03/08   ak      Corrected Feature name to FEATURE_HDR_BCMCS_DYNAMIC_MODEL.
12/03/08   ak      Featurized REG_HANDOFF and BOM_CACHE for QCHAT
11/26/08   jd      Fixing Lint Errors
11/14/08   jd      Fixing Lint Errors
10/21/08   ks      Update default threshold value for sir and pathloss and
                   add masks for RSSI event
10/15/08   rn      Corrected max line chars
09/15/08   np      Added HPCD Rev B.
09/15/08   cl      Add initialization before subscription commands are sent.
                   This allows the service domain to be sent to UI correctly.
09/04/08   dm/sv   Added WPS API for WLAN
09/05/08   sv      Changed Featurization to remove compiler warnings.
08/04/08   rk      Fixed the compilation issue when FEATURE_GPSONE not defined.
07/28/08   rm      Fixing compiler warnings
07/01/08   rm      Mainlining of MBMS changes
06/11/08   fj      Added remoted API cm_get_mode_band_capability
06/12/08   sv      Resolve Klocwork defects
05/28/08   ks      Updating the associated primary profile number when 
                   secondary PDP is promoted to Primary
05/06/08   ak      BCMCS 2.0 changes
04/29/08   ks      Reverting the changes of Updating the associated primary 
                   profile number when secondary PDP is promoted to Primary
04/07/08   cl      Fix cm_get_pref_srv_type_for_packet_orig() to return 
                   correct mode
03/31/08   ks      Updating the associated primary profile number 
                   when secondary PDP is promoted to Primary
03/22/08   ic      Replace custmmode.h with modem_mmode_cust.h
                   Replace FEATURE_MMODE_CUSTMMODE_H with FEATURE_CUSTMMODE_H   
03/08/08   ic      Corrected Lint comments
03/08/08   ic      Replace FEATURE_MM_CUSTMM_H with FEATURE_MMODE_CUSTMMODE_H
02/29/08   aj      Removed memory leak from cm_ph_cmd_ddtm_pref() 
02/28/07   jqi/aj  Added FEATURE_FULL_TIME_SHDR_3 support.
01/03/08   ka      Adding handset based plus code dialing feature.
12/16/07   ks      Added cm_ph_cmd_signal_strength_delta
12/08/07   jqi     CM shall pass the dail string when user decide to forward 
                   the incoming Call.
12/07/07   ra      Added support for IMS VCC in HOMER files 
11/20/07   vs      Lint Fix for CR#130667 - length of the dailed digitsis 
                   wrong in cmdiag
10/03/07   rn      Added cm_mm_call_cmd_ps_sig_release
09/25/07   dm      Added support for FEATURE_MM_SUPERSET
08/17/07   ic      Added compile time checks for custmmode.h and obsoleted
                   custcm.h
08/17/07   ka/dm   Lint fix
08/07/07   ic      Lint clean up
07/09/07   cl      Move cm_ext_disp_itr_* from cm.c to cmutil.c to resolve
                   link errors. Merge 482706
06/27/07   cl      Add new CM API to expose cmwcall_get_user_data() to RPC
04/23/07   jqi     DOS feature rework for dual processor arch.
04/15/07   rp      Removing FEATURE_MMGPS
03/09/07   sk      RAM usage reduction.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/26/07   ka      MSG_HIGH for oprt_mode request.
02/23/07   ic      Zero out memory before initializing fields of wlan_pref_ptr
                   in cm_wlan_pref_init()
02/18/07   rp      Replaced FEATURE_APIONE with FEATURE_WMS_CM_NAS_INTERFACES
02/22/07   pk      C-GPS changes
12/05/06   ic      Added check for FEATURE_CM_CUSTCM_H to make sure the right
                   custcm.h file is pulled in by target builds
11/16/06   pk      Lint Fixes
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
10/04/06   ka      Removing garbage characters that were causing compilation.
                   failure.
09/29/06   rp/ka   Added code to initialize all the fields in sups object.
09/14/06   ka      Adding call control for secondary PS calls.
08/29/06   sk      Added WLAN Adhoc support.
08/28/06   ka      Adding pdp header comp and data comp enums.
07/19/06   ic      Lint fixes
07/07/06   pk      Added cmtask_add_cmd_to_hold_q, and supporting data
                   structures, fields and functions
07/06/06   ka/ic   Featurization fixes for PS call control
07/04/06   ka      Adding Call control Support for PS calls.
07/03/06   ic      Cleaned up leftovers from mainlining FEATURE_SUPS
06/08/06   ic      Fixed RPC featurization problem.
                   Moved featurization into inside of cm_is_valid_mode_pref()
                   to avoid having to featurize cm.h
06/05/06   ka/ic   Moving buffer length to copy size check, before memcpy
                   in cm_num_get.
05/03/06   pk      Lint Fixes
04/13/06   ka      Initializing gw ps info in call cmd structure.
04/01/06   ka      Changes include emerg serv category during orig, aborting
                   manual search and returning current band with srv ind.
03/24/06   ka      Adding VideoTelephony fallback to voice feature for UMTS.
01/30/06   sk      Added debug info to help catch CM cmd buffer memory leaks.
01/26/06   ka      Adding NULL check to dialed_digits argument passed to
                   sups cmd.
01/18/06   ic      In cm_num_fill() initialize number even if number lenght
                   is 0
01/12/06   ic      Trimmed to minimum the list of lint errors that were
                   suppressed for va_list, va_start(), va_arg(), va_end()
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/09/06   ic      Undid CM API changes from lint clean up for cm_num_init()
                   and cm_num_copy()
01/06/06   ic      Lint cleanup
12/13/05   ic      Corrected comments in cm_bcmcs_cmd_flow_request()
12/13/05   ic      Fixed compiler warnings "different const qualifiers"
12/11/05   ic      Corrected Lint suppression comment so it does not turn on
                   613 warning
12/07/05   ka      Adding minimum_qos to cm_mm_call_cmd_pdp_modify()
11/23/05   ka      Modification to allow hold/active for ip calls.
10/21/05   sk      Removed all unnecessary checks on and modification of
                   digit_mode.
09/26/05   sk      Changed back cm_num_fill() to consider a, b and c as ASCII
                   characters.
09/12/05   pk      Added GPS session related API's
09/01/05   sk      Modified cm_num_fill to accept a, b and c as DTMF chars.
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/19/05   sk      Completely removed is_jcdma_emergency flag.
07/21/05   sk      Removed internal use of is_jcdma_emergency flag.
07/15/05   ic      Fixed lint warning 'symbol cm_cmd_alloc_pd_init() not used'
07/11/05   pk      Modified cm_ss_cmd_orig_thr_update_tbl() API, added
                   packet_zone as one more parameter.
07/06/05   ka      Initializing subscription related params of cmd ptr.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/10/05   ic      Lint clean up
06/09/05   ic      In cm_mm_call_cmd_sups() added copying of call_id into
                   common cmd_info_ptr (missed in previous check in)
06/09/05   ic      Changes to cm_mm_call_cmd_sups()
                   - added checks for SUPS params
                   - clean up to make it more readable
                   Removed all varieties of EJECT comments
06/09/05   dk      Fixed cm_get_pref_srv_type_for_packet_orig to return
                   correct SYS_MODE.
06/06/05   pk      Added cm_ss_cmd_orig_thr_update_tbl(). This API queues a
                   command to CM to update the origination throttle table
05/11/05   ic      Externalized cm_ph_net_list_copy()
04/08/05   pk      Changed cmph_get_cur_hybr_status(void), to
                   cmpref_is_hybr_status_on(). Included header file cmpref.h
04/07/05   ic      In cm_mm_call_cmd_orig() test is_jcdma_emergency field and
                   use default if not set correctly
03/14/05   ka      Adding IP call support for answer and set up response
                   api's.
02/17/05   pk      Added cm_is_sys_mode_allowed() to verify the
                   compatiblity of the mode_pref, band_pref and sys_mode
02/17/05   ic      Print parameters to cm_ph_cmd_change_rtre_config()
02/15/05   pk      Corrected alignment
02/09/05   ic      Lint cleanup
02/01/05   dk      Added WLAN Support.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/07/04   ws      Moved cm_num_init () forward in file.
12/03/04   ws      Add API for remote WMS.
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/19/04   ka      Adding support for user to user signaling
11/18/04   ka      Adding support for MT PDP feature
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/11/04   ic      Lint cleanup
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
08/27/04   jqi     Added ps dormant optimization.
08/26/04   sbs     Added externalAddress and nameString IEs to MOLR_Invoke /
                   LocationNotification SS messages
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/18/04   ka      Call direction now is common to system modes.
                   Setting call direction using a function.
08/17/04   ic      Undoing P4 change list #121638
08/12/04   ws      Remove cm20 support.
08/02/04   ic      Lint clean up - cm.c now free of Lint issues in all CUTE
                   configurations
07/28/04   ka      Added support for secondary PDP context
07/27/04   ws      Call Control support for SUPS.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Fixed ADS 1.2 compiler warning 'unsigned comparison with 0
                   is always true' by not using INRANGE() macro (casting its
                   parameters to int32 does the trick on RVCT 2.1 compiler but
                   not on ADS 1.2)
07/21/04   sj      Added support for umts to 1x handover.
07/14/04   ws      Fixed GSTK dependencies.
07/12/04   ic      Added FEATURE_ALS support.
                   Removed assertion from cm_cmd_alloc_ph_init() to be able to
                   test in CUTE that CM returns appropriate error when memory
                   allocation fails.
                   In cm_cmd_s_type replaced client field with client_id to make
                   it consistent with the rest of CM objects.
                   Removed code placed under FEATURE_HDR_TEMP.
                   In cm_m51_mode_reg() corrected formatting of if statements.
07/02/04   sj      Fixed lint errors.
06/30/04   ws      Updated cm_pd_cmd_reva_mo_msg to take cmd_cb_func
06/22/04   ic      Fixed obvious RVCT 2.1 compiler warnings
06/08/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/04/04   ka      Fixed Lint errors
06/01/04   ws      Fix num type and num plan validation routines.
05/10/04   ka      Fixed Lint errors.
04/30/04   ws      num type and num plan modifications.
04/29/04   ic      Removing outdated comments.
04/28/04   ka      Fixed Lint errors.
04/13/04   ic      Featurization changes related to 6500 5.0 CUST files being
                   used
04/01/04   ws      Updated with favored_mode mode independent.
03/23/04   ws      Merged from 6300.
03/15/04   ka      Fixed Lint errors
03/10/04   ic      FEATURE_BCMCS - removing throwing of an error when
                   CM_BCMCS_CMD_FLOW_REQUEST_F is queued with port number
                   equal to 0
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/09/04   ic      Changes so the code compiles for unit test.
02/06/04   ws      Initial Jaguar/Mainline merge.
01/20/04   ka      Added support for Multiple PDP and get call info api
01/19/04   ws      Modified handling of '+' character in cm_num_fill
01/15/04   ws      Fixed problem with cmd err in handling voice as data cmds.
01/13/04   ka      Merged changes from MSM6200
01/07/03   ws      Added handling for second slot for dual slot operations.
12/15/03   tml     Added revision history comment
12/15/03   tml     Fixed call control
12/12/03   jqi     CR38261-China Unicom test.
11/19/03   sbs     Added LCS MOLR Completed API function.
11/14/03   ws      Added support for is_jcdma_emergency.
11/07/03   ws      Fix GW/CDMA featurization problem with otasp_act code.
11/03/03   ws      Fix LTK compile errors.
10/31/03   ws      Include otasp act code/call type in cc processing.
10/27/03   ws      Removed uim pref slot selection support.
10/20/03   ws      Dual Slot Support.
                   Fixed misspelling with unavailable in several enums
                   Added F3 msgs to print out what ps orig acq mode is returned.
10/08/03   ws      Added support for checking NV for ps data calls.
10/07/03   vt      Prohibited more mode_pref's on sys_sel_pref command.
10/06/03   vt      Modified to support common cm.h across all targets.
10/01/03   ws      Mainlined DS_VOICE_AS_DATA
09/29/03   ws      Removed the workaround for Diag call end.
09/23/03   ws      Added support for getting currently acquired system.
09/16/03   vt      Added preferred slot handling.
08/29/03   ws      Added support for FEATURE_CM_PACKET_ORIG_CDMA_ONLY
08/26/03   ws      Removed RelA featurization for flash type.
08/22/03   vt      Mainlined packet state handling.
08/13/03   prk     Added processing of minimum_qos in PS data call origination.
07/08/03   sbs/lt  Added support for Location Services related SS messages
06/16/03   ws      Added check in memcpy qos_parms for overflow
05/28/03   vt      Added checks for new service types for origination.
05/20/03   ws      Modified cm_mm_call_cmd_end to return TRUE at the end.
05/15/03   vt      Added a function to return current service for PS call.
                   Fixed some compiler warnings.
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
04/25/03   ws      JCDMA support for call hold
04/28/03   AT      Lint cleanup.
04/14/03   prk     Mainlined FEATURE_SUPS.
04/14/03   ws      Added BSID changes
03/21/03   hxw     Changed FEATURE_GPSONE_OOS to FEATURE_GPSONE_MSBASED.
03/20/03   ws      Modified call_end routine so that if the client is diag & Rel0,
                   the call id will be modified to the first active call found.
03/11/03   ATT     Added support for call control performed by clients.
02/27/03   cs      Added support for PS data continuity.
02/27/03   ws      Added check for calling number being null in cmd orig
                   Removed reference to end_reason_included.
02/18/03   vt      Fixed problems to build for HDR.
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/11/03   prk     Modified cm_mm_call_cmd_sups() to assign the mode info type
                   and call id to the command info structure.  Modified
                   cm_num_init() to correctly initialize the number plan
                   and type for certain targets.
02/07/03   prk     Modified cm_mm_call_cmd_sups() to set cmd info type to
                   CM_CALL_MODE_INFO_GW_CS.
02/04/03   ri      Added support for concurrent services.
                   - modified cm_mm_call_cmd_end().
01/21/03   ic      In cm_cmd_alloc_ph_init() initialized
                   cmd_info_ptr->answer_voice to CM_ANSWER_VOICE_NONE
01/21/03   ws      Updated copyright information for 2003
11/22/02   PRK     Added support for accessing user preferred PLMN list.
11/20/02   vt      Fixed a bug where call id can't be allocated in orig call.
11/19/02   vt      Added changing RTRE configuration.
                   Fixed a compiler warning.
11/15/02   vt      Fixed a compile problem for release A.
                   Properly featurized the packet state.
11/14/02   vt      Merged changes from GW mainline.
                   [ws]Continued merge in position determination changes
                   [ws]Merged in position determination changes
                   [RI}Modified cm_mm_call_cmd_end() for GSM/WCDMA modes.
11/13/02   ic      More changes to monitor object interface - added
                   CM_MONIT_CMD_CLIENT_ACT command
11/13/02   ic      Changes to monitor object interface.
10/31/02   vt      Initialized the number fields to 0.
                   Mainlined feature SD20.
10/30/02   vt      Fixed a compile error.
10/22/02   ic      Added M51 mode object.
                   Fixed compile errors (a field changed from client to
                   client_id).
10/22/02   vt      Fixed a bug in determining minimum of alpha length.
10/22/02   vt      Fixed problems with sups_cmd for 1x.
10/05/02   vt      Removed setting of mode_info for call orig command.
08/30/02   vt      Made several functions return error status if proper
                   features are not defined. Moved cm_aoc_init function
                   prototype to cmwaoc.h
08/12/02   RI      Support for ECT and CD features (GSM/WCDMA).
08/06/02   HQ      Added FEATURE_CDSMS.
07/24/02   vt      Added handling for wakeup from deep sleep(standby)
07/23/02   PRK     Added cm_ph_cmd_get_networks and cm_ph_cmd_terminate_get_
                   networks.
07/11/02   PRK     Included cmll.h.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/03/02   ws      Added a check for NULL data_block_ptr in cm_call_cmd_orig
06/13/02   ws      Changed reference of cmmc.h to cmxll.h
06/11/02   RI      Merged the allocation of call IDs in different
                   modes in cm_mm_call_cmd_orig().
05/17/02   RI      Modified cm_sups_cmd_process_uss to initialze ss_operation
                   field.  Modified processing of called_number and calling_
                   number parameters for call orig command.
04/05/02   hxw     Featurize cmpd_init() with FEATURE_GPSONE_DBM.
04/02/02   hxw     Modified FEATURE_GPSONE_MSBASED.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
03/05/02   az      Changed the signal name for NV writes.
02/06/02   sh      Added hybrid 3.0 support
01/25/02   sj      Added support for tiered services - user zone support.
05/07/02   PRK     Included cmxpd.h instead of cmpd.h.
                   Included cmxsms.h instead of cmsms.h
05/05/02   vt      Fixed compile problem with new mm cm.h
03/11/02   RI      Added CM_CALL_CMD_ERR_NULL_PARAM_P,
                   CM_CALL_CMD_ERR_NO_CALL_ID_S
                   CM_SUPS_CMD_ERR_NULL_PARAM_P
                   Added ss_success field in cm_sups_info_s_type.
                   Deleted cm_sups_cmd_abort().
02/13/02   ic      Changed the type of call_id variable from byte/uint8 etc
                   to cm_call_id_type as it should have been to begin with.
                   Compile errors fixes related to the fact that call_id field
                   was removed from cm_inband_cmd_s_type (not needed, we use
                   call_id field in cm_inband_cmd__info_s_type instead)
02/07/02   ic      Fixes related to the fact that some structures' fields
                   are now compiled under FEATURE_IS2000_REL_A.
                   cm_nc_is_flash_emergency_f_type *cm_nc_is_flash_emergency_func
                   field was removed from cm_nc_s_type structure.
02/06/02   PRK     Added CM_OPRT_MODE_TEST to cm_oprt_mode_e_type.
02/04/02   RI      Modifications based on mn_cm_exp.h for Supplementary
                   Services.
                   - removed mn_cm_exp_t.h
                   - modified SUPS cmd err definitions
                   - modified cm_sups_con_s_type.
01/23/02   YR      Added CM_SIGNAL_NETWORK_ALERT in cm_signal_e_type.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the
                   changes between those two were merged with CCS CM
01/14/02   ic      Fixes related to running with FEATURE_IS2000_REL_A
                   undefined
01/14/02   PRK     Updated PLMN selection/reselection types/enums due to
                   lower layer software interface changes.
                   Removed featurization of the service_domain_pref parameter
                   in function ui_set_ph_mode_pref.
                   Featurized future PLMN selection/reselection code with
                   FEATURE_NEW_PLMN.
                   Added service_status field to cm_selected_network_-
                   attributes_s_type.
                   Added CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S to
                   cm_ph_cmd_err_e_type.
                   Changed stop/start full service to subscription not/available.
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32
                   (CM VU in N*2165)
                   that we needed to merge with and changes between those two
                   VUs were merged onto CCS CM tip.
11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
11/21/01   RI      Modified the function decaration of cm_find_call_id().
                   Deleted pdp_cause_T defintion.
11/17/01   sh      Added 1xhdr data session handoff support
11/06/01   RI      Modified cm_sups_common_conf_s_type
11/05/01   sh      Used new CM_CALL_TYPE_PKT_DATA for pkt data call orig
11/01/01   sh      Removed is_force_colloc from the call cmd info
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/23/01   VT/IC   Baseline for CCS
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                   Added support for PLMN selection/reselection
                   (FEATURE_PLMN).
                   Re-added Serving System group and moved srv_state,
                   roam_status, rssi from phone group to serving system group
                   (FEATURE_NEWSS).
10/17/01   AT      Added new events for AOC.
10/15/01   RI      Added support for Supplementary Services ( GSM/WCDMA ).
10/03/01   VT      Added determining whether flash number is emergency or not.
09/14/01   HD      Reversed the order of get_call_id,cleanup in ph_info_s.
08/23/01   HD      NAS header file reorg changes+ Inband and RSSI reporting
                   for WCDMA.
08/21/01   ic      Main-lined FEATURE_NEWCM
08/01/01   pj      Added call to cmpd_timer_proc.
07/17/01   HD      Added support for PS Data calls.
06/06/01   dna     copy SO for HDR call_cmd_orig_with_drs, too.
04/27/01   HD      Added changes for WCDMA/GSM mode.
04/13/01   SH      Added Preliminary HDR support
03/27/01   RC      Fixed cm_num related compiler errors when CM_ASSERT is
                   turned on.
03/20/01   HQ      Allow NULL pointer for number and alpha buffers in API
                   calls.
02/26/01   sko     Change CM monitor implementation from event to polling.
02/06/01   RC      Added support for System Determination 2.0.
01/29/01   HQ      Properly feature-ized NEWCM, UASMS and GPSONE_DBM.
01/23/01   ych     Merged T53 and JCDMA features.
12/28/00   fc      Added support for retry order.
12/15/00   HQ      Added CM_CALL_CMD_PRIVACY_PREF, CM_CALL_EVENT_PRIVACY_PREF
                   and CM_CALL_CMD_ERR_BAD_PRIVACY_PREF.
10/06/00   HD      Added support to pass the DRS bit setting to CP.
09/15/00   PJ      Added UAPDMS task. Added support for PD commands.
                   (FEATURE_GPSONE_DBM).
08/23/00   vt      Added new CM interface changes.
07/17/00   jq      Added cm_call_cmd_end_with_reason()
03/20/00   RY      Removed CM_CMD_TYPE_PH from cm_hold_cmd_q_check()
03/16/00   VT      Added BROADCAST SMS support
03/13/00   RY      Added OTAPA support
01/11/00   VT      Added cm_is_valid_mode_pref().
12/18/99   CS      Added cm_kick_dog function.
09/14/99   SH      Removed cm_clk_init
09/07/99   SH      Added uasms_init_cm
08/31/99   SH      Modified codes to featurize OTASP
08/31/99   SH      Modified codes to featurize UASMS
08/25/99   SH      Added cmcall_offline
08/17/99   SH      Used new sound API when new sound feature is defined
08/09/99   SH      Added SMS object
07/07/99   SH      Added CNAP feature
06/30/99   SH      Added activate_code for cm_call_cmd_orig
06/28/99   SH      Modified cm_nc_init
06/21/99   SH      Added processing for TASK_STOP_SIG and TASK_OFFLINE_SIG
05/31/99   SH      Added CM 2.0 features
04/09/99   jkl     Modified for new Sound API
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
12/08/98   RC      Changes per second week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Customer configuration file
*/
#include "customer.h"

/* This is to make sure all targets pull in custmmode.h
*/
#ifndef FEATURE_CUSTMMODE_H
#error  Need to pull in custmmode.h!!!
#endif

/* This is to make sure nobody uses obsoleted custcm.h
*/
#if (defined CUSTCM_H) || (defined FEATURE_CM_CUSTCM_H)
#error custcm.h is obsolete, use custmmode.h instead !!!
#endif

#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"       /* Internal interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */
#include "queue.h"     /* Interface to queue services */
#include <string.h>    /* Interface to string library */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */
#include "rex.h"       /* Interface to rex services */
#include "cmph.h"      /* Interface to CM phone object */
#include "cmll.h"      /* Interface to cm_call_id_allocate/deallocate */

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#include "cai.h"       /* Over the air messages definitions */
#include "mc.h"        /* Interface to MC Task */
#include "cmxcall.h"   /* Interface to CM CDMA call object */
#include "cmxll.h"     /* MC to CM interface*/
#include "db.h"        /* Interface to Database services */
#include "nv.h"        /* Interface to NV services */
#include "clk.h"       /* Interface to clock services */
#include "task.h"      /* Interface to task services */
#include "cmnv.h"      /* Interface to CM NV services */
#include "cmss.h"      /* Interface to CM serving system object */
#include "cminband.h"  /* Interface to CM inband object */
#include "mccdma.h"    /* Interface to current base station */
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

#include "cmsms.h"     /* Interface to cmsms.c*/

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

#ifdef FEATURE_DBM
#include "cmxdbm.h"     /* Interface to Data Burst Messages */
#endif

#ifdef FEATURE_GPSONE
#error code not present
#endif  /* FEATURE_GPSONE */

#include "cmtask.h"

#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */
#include "cmpref.h"    /* CM Preference interface */

#if (defined(FEATURE_GSM) && defined(FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE) )
#error code not present
#endif  /* defined(FEATURE_GSM) && defined(FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE) */

#define CHINA_UNICOM_MCC             460
#define CHINA_UNICOM_MNC              01
#define CHINA_MOBILE_MCC             460
#define CHINA_MOBILE_MNC              00



/*---------------------------------------------------------------------------
      Client registered callbacks for retrieving bearer capability,
      High level compatibility and low level compatibility for specific
      call types.
---------------------------------------------------------------------------*/

#if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
#endif /* (defined (FEATURE_GSM) || defined (FEATURE_WCDMA)) */


/*===========================================================================
=============================================================================
============================ CM COMMAND FUNCTIONS ===========================
=============================================================================
===========================================================================*/

#if (defined (FEATURE_BCMCS) || defined (FEATURE_MODEM_MBMS))
/*===========================================================================
FUNCTION cm_cmd_alloc_bcmcs_init

DESCRIPTION
  Allocate a BCMCS command buffer and initialize its BCMCS command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  None

===========================================================================*/
static cm_cmd_s_type *cm_cmd_alloc_bcmcs_init( void )
{
  cm_cmd_s_type            *cmd_ptr       = cm_cmd_alloc();
  cm_bcmcs_cmd_s_type      *bcmcs_cmd_ptr;
  cm_bcmcs_cmd_info_u_type *cmd_info_ptr  = NULL;

  CM_FUNC_START( "cm_cmd_alloc_bcmcs_init()",0,0,0 );
  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** If command allocation is successful, initialize call command fields.
  */
  if( cmd_ptr != NULL ) /*lint !e774 */
  {
    /*
    ** 1. Initialize command fields
    */
    cmd_ptr->cmd_type = CM_CMD_TYPE_BCMCS;

    /*
    ** 2. Initialize BCMCS command fields
    */
    bcmcs_cmd_ptr      = BCMCS_CMD_PTR( cmd_ptr );
    memset( bcmcs_cmd_ptr, 0, sizeof(*bcmcs_cmd_ptr) );
    bcmcs_cmd_ptr->cmd_cb_func               = NULL;
    bcmcs_cmd_ptr->data_block_ptr            = NULL;
    bcmcs_cmd_ptr->client_id                 = CM_CLIENT_ID_ANONYMOUS;
    bcmcs_cmd_ptr->client_ptr                = CM_CLIENT_PTR_NONE;
    bcmcs_cmd_ptr->cmd                       = CM_BCMCS_CMD_NONE;

    /*
    ** 3. Initialize BCMCS command information fields.
    */
    cmd_info_ptr              = CMD_INFO_PTR( bcmcs_cmd_ptr );
    cmd_info_ptr->flow_req.num_of_flows = 0;
    memset( cmd_info_ptr->flow_req.flow_addr_array ,
            0,
            SYS_BCMCS_MAX_FLOWS * sizeof( sys_bcmcs_flow_addr_s_type ) );

  } /*   if( cmd_ptr != NULL ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, command allocation failed!
  */
  else
  {
    CM_ERR( "cmd shortage",0,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return cmd_ptr;
} /* cm_cmd_alloc_bcmcs_init() */
#endif /* (FEATURE_BCMCS || FEATURE_MBMS) */


#if defined (FEATURE_MODEM_MBMS)
#error code not present
#endif /* #if defined (FEATURE_MODEM_MBMS) */ 

#if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
#error code not present
#endif /* FEATURE_GPSONE_MSBASED || FEATURE_GPSONE */

/*===========================================================================

FUNCTION cm_pd_cmd_fix_start

DESCRIPTION
  Send a start session command to CM. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(714, cm_pd_cmd_fix_start) */
/*lint -esym(765, cm_pd_cmd_fix_start)
** cm_pd_cmd_fix_start() not referenced
** It's not called in some configurations and can't turn it back on at the
** end of the function as Lint will then continue complaining about it
*/
boolean cm_pd_cmd_fix_start(

  cm_pd_cmd_cb_f_type   cmd_cb_func,
      /* client callback function */

  void                  *data_block_ptr,
      /* pointer to client callback data block */

  cm_client_id_type     client_id
      /* requesting client ID */
)
/*lint -esym(715,client_id)*/
{
    #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE)) && defined (FEATURE_HDR_HYBRID)
    cm_cmd_s_type              *cmd_ptr;
    cm_pd_cmd_s_type           *pd_cmd_ptr;

    cmd_ptr = cm_cmd_alloc_pd_init();

    if( cmd_ptr == NULL )
    {
      if( cmd_cb_func != NULL )
      {
        cmd_cb_func(data_block_ptr, CM_PD_CMD_FIX_START, CM_PD_CMD_ERR_NO_BUF_L);
      }

      return FALSE;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */
    else
    {
      /* Get a reference to command header
      ** and a reference to command information.
      */
      pd_cmd_ptr     = PD_CMD_PTR( cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      pd_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      pd_cmd_ptr->data_block_ptr  = data_block_ptr;
      pd_cmd_ptr->client_id       = client_id;
      pd_cmd_ptr->cmd             = CM_PD_CMD_FIX_START;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
      cm_cmd_queue( cmd_ptr );

      return TRUE;
    }
    #else
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PD_CMD_FIX_START,
        CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    }
    return FALSE;
    #endif /* ((FEATURE_GPSONE_MSBASED || FEATURE_GPSONE) && FEATURE_HDR_HYBRID)*/
}
/*lint +esym(715,client_id)*/

/*===========================================================================

FUNCTION cm_pd_cmd_fix_end

DESCRIPTION
  Send a end session command to CM. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(714, cm_pd_cmd_fix_end) */
/*lint -esym(765, cm_pd_cmd_fix_end)
** cm_pd_cmd_fix_end() not referenced
** It's not called in some configurations and can't turn it back on at the
** end of the function as Lint will then continue complaining about it
*/
boolean cm_pd_cmd_fix_end(

  cm_pd_cmd_cb_f_type   cmd_cb_func,
      /* client callback function */

  void                  *data_block_ptr,
      /* pointer to client callback data block */

  cm_client_id_type     client_id
      /* requesting client ID */

)
/*lint -esym(715,client_id)*/
{
    #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE)) && defined (FEATURE_HDR_HYBRID)

    cm_cmd_s_type              *cmd_ptr;
    cm_pd_cmd_s_type           *pd_cmd_ptr;

    cmd_ptr = cm_cmd_alloc_pd_init();

    if( cmd_ptr == NULL )
    {
      if( cmd_cb_func != NULL )
      {
        cmd_cb_func(data_block_ptr, CM_PD_CMD_FIX_END, CM_PD_CMD_ERR_NO_BUF_L);
      }

      return FALSE;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */
    else
    {
      /* Get a reference to command header
      ** and a reference to command information.
      */
      pd_cmd_ptr     = PD_CMD_PTR( cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      pd_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      pd_cmd_ptr->data_block_ptr  = data_block_ptr;
      pd_cmd_ptr->client_id       = client_id;
      pd_cmd_ptr->cmd             = CM_PD_CMD_FIX_END;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
      cm_cmd_queue( cmd_ptr );

      return TRUE;
    }
    #else
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PD_CMD_FIX_END,
        CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    }
    return FALSE;
    #endif /*((FEATURE_GPSONE_MSBASED || FEATURE_GPSONE) && FEATURE_HDR_HYBRID)*/
}
/*lint +esym(715,client_id)*/


/*===========================================================================

FUNCTION cm_num_init

DESCRIPTION
  Initialize a CM number object.

  This function must be called on a CM number object before it being used,
  in any way, place, or form.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_num_init(

    cm_num_s_type    *num_ptr
        /* Pointer to a CM number to be initialized */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( num_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(num_ptr == NULL) /*lint !e774 */
  {
    CM_ERR("num_ptr is NULL",0,0,0);
    return;
  }

  /*
  ** Set all fields to 0
  */
  num_ptr->len           = 0;
  num_ptr->number_type   = (uint8) CM_NUM_TYPE_UNKNOWN;
  num_ptr->number_plan   = (uint8) CM_NUM_PLAN_UNKNOWN;
  num_ptr->pi            = 0;
  num_ptr->si            = 0;
  num_ptr->digit_mode    = CM_DIGIT_MODE_4BIT_DTMF;

  /*
  ** Subaddress fields: num_char 0 indicates that subaddress is not present.
  */
  num_ptr->subaddr.num_char      = 0;
  num_ptr->subaddr.extension_bit = 0;
  num_ptr->subaddr.odd_even_ind  = CM_SUBADDRESS_EO_IND_EVEN;
  num_ptr->subaddr.subadd_type   = CM_SUBADDRESS_TYPE_NSAP;


  num_ptr->emerg_srv_categ       = CM_EMERG_NUM_TYPE_NONE;

  return;

} /* cm_num_init() */

#ifdef FEATURE_UUS
#error code not present
#endif /* FEATURE_UUS */

/*===========================================================================

FUNCTION cm_cmd_alloc_call_init

DESCRIPTION
  Allocate a CM command buffer and initialize its call command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_cmd_s_type *cm_cmd_alloc_call_init( void )
{
  cm_cmd_s_type                *cmd_ptr       = cm_cmd_alloc();
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type *cmd_info_ptr;

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cm_call_cmd_info_cdma_s_type      *cmd_info_cdma_ptr = NULL;
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_cmd_alloc_call_init()",0,0,0 );
  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** If command allocation is successful, initialize call command fields.
  */
  if( cmd_ptr != NULL ) /*lint !e774 */
  {
    /*
    ** 1. Initialize command fields
    */
    cmd_ptr->cmd_type = CM_CMD_TYPE_CALL;

    /*
    ** 2. Initialize to call command fields
    */
    call_cmd_ptr      = CALL_CMD_PTR( cmd_ptr );
    memset( call_cmd_ptr, CM_CALL_CMD_PARAM_DEFAULT_VALUE,
      sizeof(*call_cmd_ptr) );  /* first initialize the entire buffer to 0 */
    call_cmd_ptr->cmd_cb_func               = NULL;
    call_cmd_ptr->data_block_ptr            = NULL;
    call_cmd_ptr->client_id                 = CM_CLIENT_ID_ANONYMOUS;
    call_cmd_ptr->client_ptr                = CM_CLIENT_PTR_NONE;
    call_cmd_ptr->cmd                       = CM_CALL_CMD_NONE;

    /*
    ** 3. Initialize call command information fields.
    */
    cmd_info_ptr      = CALL_CMD_INFO_PTR( call_cmd_ptr );
    cmd_info_ptr->cmd_err                   = CM_CALL_CMD_ERR_NOERR;
    cmd_info_ptr->call_id                   = CM_CALL_ID_INVALID;
    cmd_info_ptr->call_type                 = CM_CALL_TYPE_NONE;
    cmd_info_ptr->srv_type                  = CM_SRV_TYPE_NONE;
    cm_num_init( &cmd_info_ptr->num );
    cmd_info_ptr->alpha.len                 = CM_ALPHA_LEN_NONE;
    cmd_info_ptr->num_end_call_ids          = 0;
    for ( i = 0; i < CM_CALL_ID_MAX; i++ )
    {
      memset( &cmd_info_ptr->end_params[i], 0,  sizeof( cm_end_params_s_type) );
      cmd_info_ptr->end_params[i].call_id   = CM_CALL_ID_INVALID;
    }
    cmd_info_ptr->end_status                = CM_CALL_END_NONE;
    cmd_info_ptr->info_type                 = CM_CALL_MODE_INFO_NONE;
    cmd_info_ptr->favored_mode_pref         = CM_MODE_PREF_NONE;
    cmd_info_ptr->favored_hybr_pref         = CM_HYBR_PREF_NONE;

    cmd_info_ptr->is_cdma_info_present      = FALSE;

    cmd_info_ptr->is_gsm_wcdma_cs_info_present = FALSE;
    cmd_info_ptr->is_gsm_wcdma_ps_info_present = FALSE;
    cmd_info_ptr->sups_cmd_type                = CM_CALL_SUPS_TYPE_NONE;
    cmd_info_ptr->cc_generic_params.is_fdn_to_check = TRUE;


    /*
    ** 4. Initialize call command mode-specific information fields.
    */
    #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
    #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */



    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    cmd_info_cdma_ptr = CALL_CMD_MODE_INFO_PTR( call_cmd_ptr );
    cmd_info_cdma_ptr->srv_opt              = CM_SRV_OPT_NONE;
    cmd_info_cdma_ptr->otasp_act_code       = CM_OTASP_ACT_CODE_NONE;
    #ifdef FEATURE_IS2000
    cmd_info_cdma_ptr->drs_bit              = TRUE;
    #endif /* FEATURE_IS2000 */
    cmd_info_cdma_ptr->privacy_pref         = CM_PRIVACY_PREF_NONE;
    #ifdef FEATURE_HDR_HANDOFF
#error code not present
    #endif /* FEATURE_HDR_HANDOFF */
    #ifdef FEATURE_IS2000_REL_A
    cmd_info_cdma_ptr->sr_id_included       = FALSE;
    cmd_info_cdma_ptr->qos_parms_incl       = FALSE;
    cmd_info_cdma_ptr->qos_parms_len        = 0;
    #endif /* FEATURE_IS2000_REL_A */
    cmd_info_cdma_ptr->flash_type           = CM_CALL_SUPS_TYPE_NONE;
    #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
    #endif

  } /*   if( cmd_ptr != NULL ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, command allocation failed!
  */
  else
  {
    CM_ERR( "cmd shortage",0,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return cmd_ptr;

} /* cm_cmd_alloc_call_init() */




/*===========================================================================

FUNCTION cm_cmd_alloc_ph_init

DESCRIPTION
  Allocate a CM command buffer and initialize its phone command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_cmd_s_type *cm_cmd_alloc_ph_init( void )
{
  cm_cmd_s_type          *cmd_ptr       = cm_cmd_alloc();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_cmd_alloc_ph_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the phone command fields.
  */
  if( cmd_ptr != NULL )
  {
    cm_cmd_ph_init(cmd_ptr);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, command allocation failed!
  */
  else
  {
    CM_ERR( "cmd shortage",0,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return cmd_ptr;

}


/*===========================================================================

FUNCTION cm_cmd_ph_init

DESCRIPTION
  Initialize the command buffer with its phone command fields

DEPENDENCIES
  The command buffer has been allocated.

RETURN VALUE
  .

SIDE EFFECTS
  none

===========================================================================*/
void cm_cmd_ph_init( cm_cmd_s_type * cmd_ptr)
{
  cm_ph_cmd_s_type       *ph_cmd_ptr    = PH_CMD_PTR( cmd_ptr );
  cm_ph_cmd_info_s_type  *cmd_info_ptr  = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_cmd_ph_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Initialize command type field.
    */
    cmd_ptr->cmd_type                       = CM_CMD_TYPE_PH;


    /* Initialize command header fields.
    */
    ph_cmd_ptr->cmd_cb_func                 = NULL;
    ph_cmd_ptr->data_block_ptr              = NULL;
    ph_cmd_ptr->client_id                   = CM_CLIENT_ID_ANONYMOUS;
    ph_cmd_ptr->client_ptr                  = CM_CLIENT_PTR_NONE;
    ph_cmd_ptr->cmd                         = CM_PH_CMD_NONE;

    /* Initialize the hold command field
    */

    cmd_info_ptr->is_command_on_hold        = FALSE;
    cmd_info_ptr->hold_cmd_uptime           = 0;
    cmd_info_ptr->hold_cmd_reason           = CM_HOLD_CMD_REAS_NONE;

    /* Initialize command information fields.
    */
    cmd_info_ptr->cmd_err                   = CM_PH_CMD_ERR_NOERR;
    cmd_info_ptr->mode_pref                 = CM_MODE_PREF_NONE;
    cmd_info_ptr->pref_term                 = CM_PREF_TERM_NONE;
    cmd_info_ptr->srv_domain_pref           = CM_SRV_DOMAIN_PREF_NONE;
    cmd_info_ptr->network_sel_mode_pref     = CM_NETWORK_SEL_MODE_PREF_NONE;
    cmd_info_ptr->acq_order_pref            = CM_GW_ACQ_ORDER_PREF_NONE;

    sys_plmn_undefine_plmn_id( &cmd_info_ptr->plmn );

    cmd_info_ptr->band_pref                 = CM_BAND_PREF_NONE;
    cmd_info_ptr->prl_pref                  = CM_PRL_PREF_NONE;
    cmd_info_ptr->roam_pref                 = CM_ROAM_PREF_NONE;

    cmd_info_ptr->oprt_mode                 = SYS_OPRT_MODE_NONE;
    cmd_info_ptr->nam_sel                   = CM_NAM_NONE;
    cmd_info_ptr->packet_state              = CM_PACKET_STATE_NONE;

    #ifdef FEATURE_HDR_HYBRID
#error code not present
    #endif /* FEATURE_HDR_HYBRID */

    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
    #endif /* FEATURE_WCDMA, FEATURE_GSM */

    /* Initialize command information fields.
    */

    /* Initialize the signal deltas
    */
    cmd_info_ptr->rssi_delta                = CM_SIG_STRENGTH_DELTA_NO_CHANGE;
    cmd_info_ptr->ecio_delta                = CM_SIG_STRENGTH_DELTA_NO_CHANGE;
    cmd_info_ptr->io_delta                  = CM_SIG_STRENGTH_DELTA_NO_CHANGE;
    cmd_info_ptr->sir_delta                 = CM_SIG_STRENGTH_DELTA_NO_CHANGE;
    cmd_info_ptr->pathloss_delta            = CM_SIG_STRENGTH_DELTA_NO_CHANGE;

    cmd_info_ptr->answer_voice              = CM_ANSWER_VOICE_NONE;

    /* RTRE fields
    */
    cmd_info_ptr->rtre_config               = CM_RTRE_CONFIG_NONE;
    cmd_info_ptr->rtre_control              = CM_RTRE_CONTROL_NONE;

    #ifdef FEATURE_DDTM_CNTL
    cmd_info_ptr->ddtm_pref                 = CM_DDTM_PREF_NONE;
    cmd_info_ptr->ddtm_act_mask             = SYS_DDTM_ACT_MASK_EMPTY;
    cmd_info_ptr->ddtm_so_list_act          = CM_DDTM_SO_LIST_ACT_NONE;
    cmd_info_ptr->ddtm_num_srv_opt          = 0;
    #endif

    cmd_info_ptr->cphs_allowed     = CM_CPHS_PROC_NONE;
    cmd_info_ptr->als_allowed      = CM_ALS_PROC_NONE;
    cmd_info_ptr->line             = CM_ALS_LINE_NONE;
    cmd_info_ptr->line_switching   = CM_ALS_LINE_SWITCHING_NONE;

    /* Pref slot fields
    */
    cmd_info_ptr->uim_status = FALSE;

    #if defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
        defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM)
#error code not present
    #endif

    cmd_info_ptr->network_type = CM_MODE_PREF_NONE;

    cmd_info_ptr->wlan_pref.tech_pref = CM_WLAN_TECH_PREF_NONE;

    cmd_info_ptr->wlan_pref.scan_mode = CM_WLAN_SCAN_PREF_NONE;

    cmd_info_ptr->wlan_pref.bss_type_pref = CM_WLAN_BSS_TYPE_PREF_NONE;

    cmd_info_ptr->wlan_pref.network_sel_mode_pref =
                                        CM_NETWORK_SEL_MODE_PREF_NONE;

    cmd_info_ptr->wlan_pref.bss_type = SYS_WLAN_BSS_TYPE_NONE;

    cmd_info_ptr->wakeup_service = CM_WAKEUP_NONE;

    /*
    ** cm_ph_cmd_subscription parameters
    */
    cmd_info_ptr->cdma_sub_avail_status     = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->cdma_sub_changed_status   = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->cdma_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_NONE;


    cmd_info_ptr->gw_sub_avail_status       = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->gw_sub_changed_status     = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->gw_sub_not_avail_status   = CM_SUBSCRIPTION_STATUS_NONE;


    cmd_info_ptr->wlan_sub_avail_status     = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->wlan_sub_changed_status   = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->wlan_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_NONE;

  }


/*===========================================================================

FUNCTION cm_cmd_alloc_ss_init

DESCRIPTION
  Allocate a CM command buffer and initialize its serving system command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_cmd_s_type *cm_cmd_alloc_ss_init( void )
{
  cm_cmd_s_type             *cmd_ptr        = cm_cmd_alloc();
  cm_ss_cmd_s_type          *ss_cmd_ptr     = SS_CMD_PTR( cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_cmd_alloc_ss_init()",0,0,0 );
  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the phone command fields.
  */
  if( cmd_ptr != NULL ) /*lint !e774 */
  {

    /* Initialize command type field.
    */
    cmd_ptr->cmd_type = CM_CMD_TYPE_SS;


    /* Initialize command header fields.
    */
    ss_cmd_ptr->cmd_cb_func     = NULL;
    ss_cmd_ptr->data_block_ptr  = NULL;
    ss_cmd_ptr->client_id       = CM_CLIENT_ID_ANONYMOUS;
    ss_cmd_ptr->client_ptr      = CM_CLIENT_PTR_NONE;
    ss_cmd_ptr->cmd             = CM_SS_CMD_NONE;


  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, command allocation failed!
  */
  else
  {
    CM_ERR( "cmd shortage",0,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return cmd_ptr;

}



/*===========================================================================

FUNCTION cm_cmd_alloc_inband_init

DESCRIPTION
  Allocate a CM command buffer and initialize its inbnad command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_cmd_s_type *cm_cmd_alloc_inband_init( void )
{
  cm_cmd_s_type             *cmd_ptr        = cm_cmd_alloc();
  cm_inband_cmd_s_type      *inband_cmd_ptr = INBAND_CMD_PTR( cmd_ptr );
  cm_inband_cmd_info_s_type *cmd_info_ptr  = CMD_INFO_PTR( inband_cmd_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_cmd_alloc_inband_init()",0,0,0 );
  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the inband command fields.
  */
  if( cmd_ptr != NULL ) /*lint !e774 */
  {

    /* Initialize command type field.
    */
    cmd_ptr->cmd_type = CM_CMD_TYPE_INBAND;


    /* Initialize command header fields.
    */
    inband_cmd_ptr->cmd_cb_func     = NULL;
    inband_cmd_ptr->data_block_ptr  = NULL;
    inband_cmd_ptr->client_id       = CM_CLIENT_ID_ANONYMOUS;
    inband_cmd_ptr->client_ptr      = CM_CLIENT_PTR_NONE;
    inband_cmd_ptr->cmd             = CM_INBAND_CMD_NONE;

    /* Initialize command information fields.
    */
    cmd_info_ptr->cmd_err           = CM_INBAND_CMD_ERR_NOERR;



  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, command allocation failed!
  */
  else
  {
    CM_ERR( "cmd shortage",0,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return cmd_ptr;

}

/*===========================================================================

FUNCTION cm_cmd_alloc_sms_init

DESCRIPTION
  Allocate a CM command buffer and initialize its SMS command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_cmd_s_type *cm_cmd_alloc_sms_init( void )
{
  cm_cmd_s_type             *cmd_ptr        = cm_cmd_alloc();
  cm_sms_cmd_s_type         *sms_cmd_ptr    = SMS_CMD_PTR( cmd_ptr );
  cm_sms_cmd_info_s_type    *cmd_info_ptr   = CMD_INFO_PTR( sms_cmd_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_cmd_alloc_sms_init()",0,0,0 );
  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the sms command fields.
  */
  if( cmd_ptr != NULL ) /*lint !e774 */
  {

    /* Initialize command type field.
    */
    cmd_ptr->cmd_type = CM_CMD_TYPE_SMS;


    /* Initialize command header fields.
    */
    sms_cmd_ptr->cmd_cb_func     = NULL;
    sms_cmd_ptr->data_block_ptr  = NULL;
    sms_cmd_ptr->client_id       = CM_CLIENT_ID_ANONYMOUS;
    sms_cmd_ptr->client_ptr      = CM_CLIENT_PTR_NONE;
    sms_cmd_ptr->cmd             = CM_SMS_CMD_NONE;

    /* Initialize command information fields.
    */
    cmd_info_ptr->cmd_err        = CM_SMS_CMD_ERR_NOERR;



  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, command allocation failed!
  */
  else
  {
    CM_ERR( "cmd shortage",0,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return cmd_ptr;

}


#ifdef FEATURE_DBM
/*===========================================================================

FUNCTION cm_cmd_alloc_dbm_init

DESCRIPTION
  Obtains a CM command buffer from the free queue and initializes it for
  a DBM command.

DEPENDENCIES
  CM must be initialized.

RETURN VALUE
  Pointer to the buffer, NULL otherwise.

SIDE EFFECTS
  None

===========================================================================*/

static cm_cmd_s_type* cm_cmd_alloc_dbm_init( void )
{
  cm_cmd_s_type            *cmd_ptr        = cm_cmd_alloc();
  cm_dbm_cmd_s_type         *dbm_cmd_ptr   = DBM_CMD_PTR( cmd_ptr );
  cm_dbm_cmd_info_s_type    *cmd_info_ptr  = CMD_INFO_PTR( dbm_cmd_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_cmd_alloc_dbm_init()",0,0,0 );
  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the dbm command fields.
  */
  if( cmd_ptr != NULL ) /*lint !e774 */
  {

    /*---------------------------------
      Initialize command type
     *------------------------------ */

    cmd_ptr->cmd_type = CM_CMD_TYPE_DBM;

    /*---------------------------------
      Now the header.
     *------------------------------ */

    dbm_cmd_ptr->cmd_cb_func      = NULL;
    dbm_cmd_ptr->data_block_ptr   = NULL;
    dbm_cmd_ptr->client_id        = CM_CLIENT_ID_ANONYMOUS;
    dbm_cmd_ptr->client_ptr       = CM_CLIENT_PTR_NONE;
    dbm_cmd_ptr->cmd              = CM_DBM_CMD_NONE;
    dbm_cmd_ptr->cmd_err          = CM_DBM_CMD_ERR_NOERR;
    dbm_cmd_ptr->l2_ack_wait_time = 0;

    cmd_info_ptr->num_bytes        = 0;
    cmd_info_ptr->trans_pref      = CM_DBM_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, command allocation failed!
  */
  else
  {
    dbm_cmd_ptr->cmd_err         = CM_DBM_CMD_ERR_NO_BUF_L;
    CM_ERR( "cmd shortage",0,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return cmd_ptr;

}
#endif   /* FEATURE_DBM */

#if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
#endif /* (defined (FEATURE_GSM) || defined (FEATURE_WCDMA)) */


/*===========================================================================

FUNCTION cm_dbm_cmd_mo_msg

DESCRIPTION
  Send a data burst message. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_dbm_cmd_mo_msg (
    uint8                              burst_type,
        /* Burst type whether a PD ro SDB e.g. CAI_POSITION_DET etc */

    cm_dbm_cmd_cb_f_type               cmd_cb_func,
        /* client callback function */

    void                               *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type                  client_id,
        /* requesting client ID */

    void                              *payload_ptr,
        /* pointer to start of message */

    uint16                             num_bytes,
        /* Number of bytes in message */

    cm_dbm_payload_copy_cb_f_type      *payload_copy_func,
        /* Pointer to callback function to copy payload from client's
         * memory space to CM's memory space
        */

    uint8                              l2_ack_wait_time,
        /* Maximum number of seconds to wait for receiving L2 ack */

    uint8                              l3_ack_wait_time,
        /* Maximum number of seconds to wait for receiving L3 ack
         * before sending the pref sys change command
        */

    cm_dbm_trans_pref_e_type           trans_pref
        /* Transmission preference like REACH, ACH etc.. */

)
/*lint -esym(818, payload_ptr)
** Can't be const, would have to change cm_dbm_payload_copy_cb_f_type
** and that causes compiler errors in other subsystems
*/
/*lint -esym(715,trans_pref) */
/*lint -esym(715,payload_ptr) */
/*lint -esym(715,payload_copy_func) */
/*lint -esym(715,client_id) */
/*lint -esym(715,burst_type) */
/*lint -esym(715,l2_ack_wait_time) */
/*lint -esym(715,l3_ack_wait_time) */
/*lint -esym(715,num_bytes) */
{
  #ifdef FEATURE_DBM
  cm_cmd_s_type              *cmd_ptr;
  cm_dbm_cmd_s_type          *dbm_cmd_ptr;
  cm_dbm_cmd_info_s_type     *cmd_info_ptr;

  uint16 counter;
  int temp_num_bytes = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------PD CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_dbm_cmd_mo_msg()",0,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_dbm_init();

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func(data_block_ptr, CM_DBM_CMD_SEND_MSG, CM_DBM_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* command buffer allocation is successful
  ** copy command information into command structure.
  */

  /* Get a reference to command header
  ** and a reference to command information.
  */
  dbm_cmd_ptr     = DBM_CMD_PTR( cmd_ptr );
  cmd_info_ptr    = CMD_INFO_PTR( dbm_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */

  dbm_cmd_ptr->burst_type         = burst_type;
  dbm_cmd_ptr->cmd_cb_func        = cmd_cb_func;
  dbm_cmd_ptr->data_block_ptr     = data_block_ptr;
  dbm_cmd_ptr->client_id          = client_id;
  dbm_cmd_ptr->cmd                = CM_DBM_CMD_SEND_MSG;
  dbm_cmd_ptr->l2_ack_wait_time   = l2_ack_wait_time;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->trans_pref     = trans_pref;
  cmd_info_ptr->num_bytes      = num_bytes;

  /* Check if the num_bytes is <= CM_DBM_MAX_PAYLOAD_SIZE
  ** If no, than a failure will eventually be reported back
  ** to the interested clients. Hence no need to copy the payload.
  ** Reject the command is num_bytes > 0 but payload is NULL
  */

  if(!INRANGE(num_bytes, 1, CM_DBM_MAX_PAYLOAD_SIZE ))
  {
    dbm_cmd_ptr->cmd_err = CM_DBM_CMD_ERR_PAYLOAD_SIZE_P;
  }
  else
  if(payload_ptr == NULL)
  {
    dbm_cmd_ptr->cmd_err = CM_DBM_CMD_ERR_PAYLOAD_NULL_PTR_P;
  }
  else
  {
    /*  The supplied payload is less than or equal to maximum
    **  size supported
    */
    temp_num_bytes = MIN(num_bytes, sizeof(cmd_info_ptr->payload));
    if (payload_copy_func != NULL)
    {
      /*  Use callback function provided to get the payload
      */
      if (payload_copy_func(&payload_ptr,
                            cmd_info_ptr->payload,
                            &temp_num_bytes))
      {
        /*  Some error occured during copying
        */
        dbm_cmd_ptr->cmd_err = CM_DBM_CMD_ERR_OTHER;
      }

    }
    else
    {
      for (counter = 0; counter < temp_num_bytes; counter++ )
      {
        cmd_info_ptr->payload[counter] = *((uint8 *)payload_ptr + counter);
      }
    }
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( cmd_ptr );

  return TRUE;
  #else /* FEATURE_DBM */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_DBM_CMD_SEND_MSG,
      CM_DBM_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of FEATURE_DBM */
}
/*lint +esym(715,trans_pref) */
/*lint +esym(715,payload_ptr) */
/*lint +esym(715,payload_copy_func) */
/*lint +esym(715,client_id) */
/*lint +esym(715,burst_type) */
/*lint +esym(715,l2_ack_wait_time) */
/*lint +esym(715,l3_ack_wait_time) */
/*lint +esym(715,num_bytes) */
/*lint +esym(818, payload_ptr) */



/*===========================================================================

FUNCTION cm_dbm_cmd_mo_msg2

DESCRIPTION
  Send a data burst message. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_dbm_cmd_mo_msg2 (
    uint8                              burst_type,
        /* Burst type whether a PD ro SDB e.g. CAI_POSITION_DET etc */

    cm_dbm_cmd_cb_f_type               cmd_cb_func,
        /* client callback function */

    void                              *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type                  client_id,
        /* requesting client ID */

    const cm_dbm_cmd_info_s_type       *dbm_cmd_info_ptr,
        /* DBM information pointer */

    uint8                              l2_ack_wait_time,
        /* Maximum number of seconds to wait for receiving L2 ack */

    uint8                              l3_ack_wait_time
        /* Maximum number of seconds to wait for receiving L3 ack
         * before sending the pref sys change command
        */
)
/*lint -esym(715,burst_type) */
/*lint -esym(715,data_block_ptr) */
/*lint -esym(715,client_id) */
/*lint -esym(715,dbm_cmd_info_ptr */
/*lint -esym(715,payload_copy_func) */
/*lint -esym(715,l2_ack_wait_time) */
/*lint -esym(715,l3_ack_wait_time) */
{
  #ifdef FEATURE_DBM
  cm_cmd_s_type              *cmd_ptr;
  cm_dbm_cmd_s_type          *dbm_cmd_ptr;
  cm_dbm_cmd_info_s_type     *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------PD CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_dbm_cmd_mo_msg2()",0,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_dbm_init();

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func(data_block_ptr, CM_DBM_CMD_SEND_MSG, CM_DBM_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( dbm_cmd_info_ptr == NULL )
  {

    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_DBM_CMD_SEND_MSG, CM_DBM_CMD_ERR_PAYLOAD_NULL_PTR_P );
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the num_bytes is <= CM_DBM_MAX_PAYLOAD_SIZE
  ** If no, than a failure will eventually be reported back
  ** to the interested clients. Hence no need to copy the payload.
  ** Reject the command is num_bytes > 0 but payload is NULL
  */

  if(!INRANGE(dbm_cmd_info_ptr->num_bytes, 1, CM_DBM_MAX_PAYLOAD_SIZE ))
  {
     if ( cmd_cb_func != NULL )
     {
       cmd_cb_func( data_block_ptr,
                    CM_DBM_CMD_SEND_MSG, CM_DBM_CMD_ERR_PAYLOAD_SIZE_P);
     }

     return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* command buffer allocation is successful
  ** copy command information into command structure.
  */

  /* Get a reference to command header
  ** and a reference to command information.
  */
  dbm_cmd_ptr     = DBM_CMD_PTR( cmd_ptr );
  cmd_info_ptr    = CMD_INFO_PTR( dbm_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */

  dbm_cmd_ptr->burst_type         = burst_type;
  dbm_cmd_ptr->cmd_cb_func        = cmd_cb_func;
  dbm_cmd_ptr->data_block_ptr     = data_block_ptr;
  dbm_cmd_ptr->client_id          = client_id;
  dbm_cmd_ptr->cmd                = CM_DBM_CMD_SEND_MSG;
  dbm_cmd_ptr->l2_ack_wait_time   = l2_ack_wait_time;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  memcpy(cmd_info_ptr, dbm_cmd_info_ptr, sizeof(cm_dbm_cmd_info_s_type));


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( cmd_ptr );

  return TRUE;

  #else /* FEATURE_DBM */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_DBM_CMD_SEND_MSG,
      CM_DBM_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;

  #endif /* else of FEATURE_DBM */
}
/*lint +esym(715,burst_type) */
/*lint +esym(715,data_block_ptr) */
/*lint +esym(715,client_id) */
/*lint +esym(715,dbm_cmd_info_ptr */
/*lint +esym(715,payload_copy_func) */
/*lint +esym(715,l2_ack_wait_time) */
/*lint +esym(715,l3_ack_wait_time) */



/*===========================================================================

FUNCTION cm_pd_cmd_session_start

DESCRIPTION
  Send a start session command to CM. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_pd_cmd_session_start(

  cm_pd_cmd_cb_f_type        cmd_cb_func,
      /* client callback function */

  void                      *data_block_ptr,
      /* pointer to client callback data block */

  cm_client_id_type          client,
      /* requesting client ID */

  cm_pd_session_type_e_type  session_type
      /* Type of session */

    )
/*lint -esym(715, client) */
/*lint -esym(715, session_type) */
{
  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE) )
#error code not present
  #else /* FEATURE_GPSONE_MSBASED */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PD_CMD_SESSION_START,
      CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of FEATURE_GPSONE_MSBASED */
}
/*lint +esym(715, client) */
/*lint +esym(715, session_type) */

/*===========================================================================

FUNCTION cm_pd_cmd_session_end

DESCRIPTION
  Send a end session command to CM. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_pd_cmd_session_end(

  cm_pd_cmd_cb_f_type        cmd_cb_func,
      /* client callback function */

  void                      *data_block_ptr,
      /* pointer to client callback data block */

  cm_client_id_type          client,
      /* requesting client ID */

  cm_pd_session_type_e_type  session_type
      /* type of session */

)
/*lint -esym(715, client) */
{
  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
#error code not present
  #else /* FEATURE_GPSONE_MSBASED */

  SYS_ARG_NOT_USED(client);
  SYS_ARG_NOT_USED(session_type);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PD_CMD_SESSION_END,
      CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of FEATURE_GPSONE_MSBASED */
}
/*lint +esym(715, client) */

/*===========================================================================

FUNCTION cm_pd_cmd_session_dorm_update

DESCRIPTION
  Send a session dormancy command to CM. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_pd_cmd_session_dorm_update(

  cm_pd_cmd_cb_f_type          cmd_cb_func,
      /* client callback function */

  void                        *data_block_ptr,
      /* pointer to client callback data block */

  cm_client_id_type            client,
      /* requesting client ID */

  cm_pd_session_type_e_type    session_type,
      /* type of session */

  cm_pd_session_dormant_e_type session_dormant
      /* The dormant status of the session */

)
{
  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
#error code not present
  #else /* FEATURE_GPSONE_MSBASED */
  SYS_ARG_NOT_USED(client);
  SYS_ARG_NOT_USED(session_type);
  SYS_ARG_NOT_USED(session_dormant);
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PD_CMD_SESSION_DORMANT,
      CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of FEATURE_GPSONE_MSBASED */
}

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */


/*===========================================================================

FUNCTION cm_mm_call_cmd_orig

DESCRIPTION
  Command CM to originate a call.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_orig
(

  cm_call_cmd_cb_f_type              cmd_cb_func,
    /* client callback function */

  void                              *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                  client_id,
    /* originating client */

  cm_call_type_e_type                call_type,
    /* call type - voice, data, etc. */

  cm_srv_type_e_type                 srv_type,
    /* service type - automatic, etc. */

  const cm_num_s_type               *calling_number_ptr,
    /* Number of mobile that is originating the call  */

  const cm_num_s_type               *called_number_ptr,
    /* Number dialed - phone that is being called */

  const cm_orig_alpha_s_type        *alpha_buf,
    /* alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /* pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /* pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /* pointer to GW PS mode specific data */

  cm_call_id_type                   *return_call_id_ptr
    /* call ID will be allocated and returned */

)
/*lint -esym(715, cdma_orig_params_ptr) */
/*lint -esym(715, gw_cs_orig_params_ptr) */
/*lint -esym(715, gw_ps_orig_params_ptr) */
{
    return (cm_mm_call_cmd_orig_exten(
                                    cmd_cb_func,
                                    /* client callback function */

                                    data_block_ptr,
                                    /* pointer to client callback data block */

                                    client_id,
                                    /* originating client */

                                    call_type,
                                    /* call type - voice, data, etc. */

                                    srv_type,
                                    /* service type - automatic, etc. */

                                    calling_number_ptr,
                                    /* Number of mobile that is originating 
                                     * the call  
                                     */

                                    called_number_ptr,
                                    /* Number dialed - phone that is being 
                                    * called 
                                    */

                                    alpha_buf,
                                    /* alpha tag to be displayed for the 
                                    * origination 
                                    */

                                    cdma_orig_params_ptr,
                                    /* pointer to cdma mode specific data */

                                    gw_cs_orig_params_ptr,
                                    /* pointer to GW CS mode specific data */

                                    gw_ps_orig_params_ptr,
                                    /* pointer to GW PS mode specific data */

                                    CM_CALL_ID_INVALID,
                                    /* Switch out call id*/

                                    return_call_id_ptr
                                    /* call ID will be allocated and returned */

                                    ));

 }


/**==========================================================================

@FUNCTION cm_mm_call_cmd_orig_exten

@DESCRIPTION
  Command CM to originate a handover call.

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
boolean cm_mm_call_cmd_orig_exten(

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
    /**< Switch_out_call_id */

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned */

)
/*lint -esym(715, cdma_orig_params_ptr) */
/*lint -esym(715, gw_cs_orig_params_ptr) */
/*lint -esym(715, gw_ps_orig_params_ptr) */
{
    cm_cc_generic_params_s_type   cc_generic_params;
    cc_generic_params.is_fdn_to_check = TRUE;

    return (cm_mm_call_cmd_orig_exten2(
                                    cmd_cb_func,
                                    /* client callback function */

                                    data_block_ptr,
                                    /* pointer to client callback data block */

                                    client_id,
                                    /* originating client */

                                    call_type,
                                    /* call type - voice, data, etc. */

                                    srv_type,
                                    /* service type - automatic, etc. */

                                    calling_number,
                                    /* Number of mobile that is originating
                                     * the call
                                     */

                                    called_number,
                                    /* Number dialed - phone that is being
                                    * called
                                    */

                                    alpha_buf,
                                    /* alpha tag to be displayed for the
                                    * origination
                                    */

                                    cdma_orig_params_ptr,
                                    /* pointer to cdma mode specific data */

                                    gw_cs_orig_params_ptr,
                                    /* pointer to GW CS mode specific data */

                                    gw_ps_orig_params_ptr,
                                    /* pointer to GW PS mode specific data */

                                    switch_out_call_id,
                                    /* Switch out call id*/

                                    &cc_generic_params,

                                    return_call_id_ptr
                                    /* call ID will be allocated and returned */

                                    ));

 }

/**==========================================================================

@FUNCTION cm_mm_call_cmd_orig_exten2

@DESCRIPTION
  Command CM to originate a handover call.

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
boolean cm_mm_call_cmd_orig_exten2(

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
    /**< Switch_out_call_id */

  const cm_cc_generic_params_s_type   *cc_generic_params_ptr,
    /**< Call-control generic parameters */

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned */

)
{
  cm_cmd_s_type                *cmd_ptr;
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_num_err_e_type             num_err;
  cm_call_cmd_info_s_type      *cmd_info_ptr;
  cm_call_cmd_err_e_type        cmd_err = CM_CALL_CMD_ERR_NOERR;
  cmcall_s_type                *call_ptr = NULL;

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cmph_s_type                  *ph_ptr  = cmph_ptr ();
  #endif

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cm_call_cmd_info_cdma_s_type   *cmd_info_cdma_ptr;
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "---------------------CALL CMD---------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_call_cmd_orig()",0,0,0 );
  CM_MSG_LOW( "--------------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Allocate and initialize call command buffer
  */
  cmd_ptr = cm_cmd_alloc_call_init();

  /** If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_CALL_CMD_ORIG,
                   CM_CALL_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }

  /** Command buffer allocation is successful
  */

  /**
  ** Copy command header fields.
  */
  call_cmd_ptr = CALL_CMD_PTR( cmd_ptr );
  call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  call_cmd_ptr->data_block_ptr  = data_block_ptr;
  call_cmd_ptr->client_id       = client_id;
  call_cmd_ptr->cmd             = CM_CALL_CMD_ORIG;

  /**
  ** Check for any function parameter errors
  */
  num_err = cm_num_validate( called_number);
  if ( num_err != CM_NUM_ERR_NOERR )
  {
    switch ( num_err )
    {
      case CM_NUM_ERR_NULL_PTR :
        cmd_err = CM_CALL_CMD_ERR_NULL_NUM_P;
        break;

      case CM_NUM_ERR_INVALID_NUM_LENGTH :
        cmd_err = CM_CALL_CMD_ERR_NUM_LEN_P;
        break;

      default :
        cmd_err = CM_CALL_CMD_ERR_OTHER;
        break;
    } /**< switch ( num_err ) */
  }
  else
  if( return_call_id_ptr== NULL)
  {
    cmd_err = CM_CALL_CMD_ERR_NULL_PARAM_P;
  }
  else if( alpha_buf == NULL )
  {
    cmd_err = CM_CALL_CMD_ERR_NULL_ALPHA_P;
  }
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  #ifdef FEATURE_IS2000_REL_A
  else if ( cdma_orig_params_ptr != NULL &&
            cdma_orig_params_ptr->qos_parms_incl &&
            cdma_orig_params_ptr->qos_parms_len > CM_ORIG_QOS_LEN_MAX )
  {

    cmd_err = CM_CALL_CMD_ERR_QOS_LEN_P;
  }
  #endif /* FEATURE_IS2000_REL_A */
  else if ( !BETWEEN(srv_type, CM_SRV_TYPE_NONE, CM_SRV_TYPE_MAX))
  {
    cmd_err = CM_CALL_CMD_ERR_SRV_TYPE_P;
  }
  else if ( srv_type == CM_SRV_TYPE_CDMA_HDR && cdma_orig_params_ptr == NULL )
  {
    cmd_err = CM_CALL_CMD_ERR_MISMATCH_P;
  }
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif /* if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  /**
  ** If no function parameter errors, allocate call id.
  */
  else
  { 
    if ( (*return_call_id_ptr = cm_call_id_allocate()) == CM_CALL_ID_INVALID )
    {
      cmd_err = CM_CALL_CMD_ERR_NO_CALL_ID_S;
    }
    else
    {
      call_ptr = cmcall_ptr (*return_call_id_ptr);

      if(call_ptr == NULL)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
      }
      else
      {
        cmcall_set_call_direction (call_ptr, CM_CALL_DIRECTION_MO);
      }
    }
  }
  if(cc_generic_params_ptr == NULL)
  {
     CM_MSG_HIGH("CC Gen params ptr NULL", 0, 0, 0);
     cmd_err = CM_CALL_CMD_ERR_MISMATCH_P;
  }

  /**
  ** If no parameter errors found, pack cmd info and queue it for CM,
  ** otherwise just queue it with command error
  */
  cmd_info_ptr          = CMD_INFO_PTR( call_cmd_ptr );
  cmd_info_ptr->cmd_err = cmd_err;

  if ( cmd_err == CM_CALL_CMD_ERR_NOERR )
  {
    /* Set the flag for IS-707B call and change the call type back to
    ** CM_CALL_TYPE_PS_DATA for normal operation
    */
    if (call_type == CM_CALL_TYPE_PS_DATA_IS707B) 
    {
      call_type = CM_CALL_TYPE_PS_DATA;
      call_ptr->is_707b_ps_data = TRUE;
    }

    /**<
    ** Copy command information fields.
    */
    cmd_info_ptr->call_id                   = *return_call_id_ptr;
    cmd_info_ptr->switch_out_call_id        = switch_out_call_id;

    cmd_info_ptr->call_type                 = call_type;
    cmd_info_ptr->srv_type                  = srv_type;
    cmd_info_ptr->num                       = *called_number;
    if ( calling_number == NULL )
    {
       cmd_info_ptr->calling_num.len        = 0;
    }
    else
    {
       cmd_info_ptr->calling_num            = *calling_number;
    }

    cmd_info_ptr->alpha.len  = MIN(alpha_buf->len,
                                   sizeof(cmd_info_ptr->alpha.buf) );
    /*lint -save -e668 */
    memcpy( cmd_info_ptr->alpha.buf,alpha_buf,cmd_info_ptr->alpha.len );
    /*lint -restore */

    CM_MSG_HIGH ("Emerg serv categ given = %d",
                  called_number->emerg_srv_categ, 0, 0);

    CM_MSG_MED ("FDN check to do = %d",
                  cc_generic_params_ptr->is_fdn_to_check, 0, 0);
    cmd_info_ptr->cc_generic_params.is_fdn_to_check = cc_generic_params_ptr->is_fdn_to_check;

    /**
    ** Copy mode-specific command information fields.
    */
    #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
    #endif /* FEATURE_WCDMA, FEATURE_GSM */

    #if ( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) )
    if ( cdma_orig_params_ptr != NULL )
    {
      cmd_info_ptr->is_cdma_info_present = TRUE;

      cmd_info_cdma_ptr = CALL_CMD_MODE_INFO_PTR( call_cmd_ptr );

      #ifdef FEATURE_HDR_HANDOFF
#error code not present
      #endif   /* FEATURE_HDR_HANDOFF */

      /** Only copy the service option parameter,
      ** if service type is CDMA specific or CDMA_HDR.
      */
      if( srv_type == CM_SRV_TYPE_CDMA_SPECIFIC ||
          srv_type == CM_SRV_TYPE_CDMA_HDR )
      {
        cmd_info_cdma_ptr->srv_opt = cdma_orig_params_ptr->srv_opt;
      } /**< if( srv_type == CM_SRV_TYPE_CDMA_SPECIFIC ) */

      #ifdef FEATURE_HDR
#error code not present
      #endif  /* FEATURE_HDR */

      cmd_info_ptr->favored_hybr_pref = ph_ptr->hybr_pref;
      cmd_info_cdma_ptr->otasp_act_code =
      cdma_orig_params_ptr->activate_code;

      #ifdef FEATURE_IS2000

      /** Only data calls are allowed to set DRS bit in the
      ** origination command
      */
      if(( call_type == CM_CALL_TYPE_PS_DATA ) ||
         ( call_type == CM_CALL_TYPE_CS_DATA ))
      {
        cmd_info_cdma_ptr->drs_bit = cdma_orig_params_ptr->drs_bit;
      }
      else
      {
        /**< For all other call types DRS bit defaults to TRUE.
        ** If it's not set to TRUE in the origination command,
        ** whine about it, set it to TRUE, allow the origination
        ** to go through
        */
        if( cdma_orig_params_ptr->drs_bit == FALSE )
        {
          CM_ERR("Illegal DRS bit value for call type %d",call_type,0,0);
        }
        cmd_info_cdma_ptr->drs_bit = TRUE;
      }
      #endif /* FEATURE_IS2000 */

      #ifdef FEATURE_IS2000_REL_A
      cmd_info_cdma_ptr->sr_id_included = cdma_orig_params_ptr->sr_id_included;
      cmd_info_cdma_ptr->sr_id          = cdma_orig_params_ptr->sr_id;
      cmd_info_cdma_ptr->qos_parms_incl = cdma_orig_params_ptr->qos_parms_incl;
      if ( cmd_info_cdma_ptr->qos_parms_incl )
      {
        cmd_info_cdma_ptr->qos_parms_len = cdma_orig_params_ptr->qos_parms_len;
        memcpy( cmd_info_cdma_ptr->qos_parms,
                cdma_orig_params_ptr->qos_parms,
                (size_t) MIN(cmd_info_cdma_ptr->qos_parms_len,
                             sizeof(cmd_info_cdma_ptr->qos_parms)) );
      } /**< if ( cmd_info_ptr->qos_params_incl ) */
      #endif /* FEATURE_IS2000_REL_A */

    } /* if ( cdma_orig_params_ptr != NULL ) */
    #endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
  } /* if (cmd_err == CM_CALL_CMD_ERR_NOERR) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Queue the command to CM.
  */
  cm_cmd_queue( cmd_ptr );
  return TRUE;
} /* cm_mm_call_cmd_orig_exten2() */
/*lint +esym(715, cdma_orig_params_ptr) */
/*lint +esym(715, gw_cs_orig_params_ptr) */
/*lint +esym(715, gw_ps_orig_params_ptr) */


/*===========================================================================

FUNCTION cm_mm_call_cmd_answer

DESCRIPTION
  Command CM to answer an incoming call.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_answer(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* answering client */

  cm_call_id_type        call_id,
    /* call ID of the call */

  const cm_ans_params_s_type   *ans_params_ptr
    /* pointer to mode specific data */

)
{

  cm_cmd_s_type                *cmd_ptr;
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type *cmd_info_ptr;

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif /* if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "---------------------CALL CMD---------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_call_cmd_answer()",0,0,0 );
  CM_MSG_LOW( "--------------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( ans_params_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      MSG_ERROR("ERROR: NULL ans_params ptr",0,0,0);
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_ANSWER, CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  } /* if( ans_params_ptr == NULL ) */

  cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if ( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_ANSWER,
          CM_CALL_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /* Copy command header fields. */
    call_cmd_ptr      = CALL_CMD_PTR( cmd_ptr );
    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_ANSWER;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields. */
    cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );
    cmd_info_ptr->call_id         = call_id;
    cmd_info_ptr->info_type       = ans_params_ptr->info_type;

    switch( ans_params_ptr->info_type )
    {
    #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
    #endif /* if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    case CM_CALL_MODE_INFO_CDMA:
      cmd_info_ptr->call_type = ans_params_ptr->ans_params.cdma_ans.call_type;
      break;
    #endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

    case CM_CALL_MODE_INFO_IP:
      /* In the future it can be a CM_CALL_TYPE_VT too
      */
      cmd_info_ptr->call_type      = ans_params_ptr->ans_params.ip_ans.call_type;
      cmd_info_ptr->ip_call.accept = ans_params_ptr->ans_params.ip_ans.accept;
      break;

    default:
      CM_ERR( "Invalid info type %d",ans_params_ptr->info_type,0,0 );
      cmd_info_ptr->cmd_err = CM_CALL_CMD_ERR_OTHER;
      break;
    } /* switch( ans_params_ptr->info_type ) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  } /* else of if ( cmd_ptr == NULL ) */

} /* cm_mm_call_cmd_answer() */



/*===========================================================================

FUNCTION cm_mm_call_cmd_setup_res

DESCRIPTION
  Command CM to send a setup message response.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_setup_res(

  cm_call_cmd_cb_f_type               cmd_cb_func,
    /* client callback function */

  void                               *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                   client_id,
    /* answering client */

  cm_call_id_type                     call_id,
    /* call ID of the call */

  const cm_setup_res_params_s_type   *setup_params_ptr
    /* pointer to mode specific data */

)
/*lint -esym(715,setup_params_ptr) */
/*lint -esym(715,call_id) */
/*lint -esym(715,client_id) */
{

  cm_cmd_s_type                     *cmd_ptr;
  cm_call_cmd_s_type                *call_cmd_ptr;
  cm_call_cmd_info_s_type           *cmd_info_ptr;
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "---------------------CALL CMD---------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_call_cmd_setup_res()",0,0,0 );
  CM_MSG_LOW( "--------------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( setup_params_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_SETUP_RES, CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }

  cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func(data_block_ptr, CM_CALL_CMD_SETUP_RES,
          CM_CALL_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Else (command buffer allocation is successful),
   ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    call_cmd_ptr       = CALL_CMD_PTR( cmd_ptr );
    cmd_info_ptr       = CMD_INFO_PTR( call_cmd_ptr );



    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_SETUP_RES;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->call_id                  = call_id;


    if (setup_params_ptr->accept == FALSE)
    {
      cmd_info_ptr->end_status             = CM_CALL_END_SETUP_REJ;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if defined(FEATURE_IP_CALL)
    /* Copy accept to ip accept flag
    */
    cmd_info_ptr->ip_call.accept           = setup_params_ptr->accept;
    #endif


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
    cm_cmd_queue( cmd_ptr );
    return TRUE;

  }
}
/*lint +esym(715,setup_params_ptr) */
/*lint +esym(715,call_id) */
/*lint +esym(715,client_id) */


/*===========================================================================

FUNCTION cm_mm_call_cmd_pdp_act_res

DESCRIPTION
  Response to an Incoming PDP activation request.(GSM/WCDMA PS calls)

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_pdp_act_res(

  cm_call_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type              client_id,
    /* answering client */

  cm_call_id_type                call_id,
    /* call ID of the call */

  const cm_pdp_act_res_params_s_type   *act_res_params_ptr
    /* pointer to mode specific data */

)
/*lint -esym(715,act_res_params_ptr) */
/*lint -esym(715,call_id) */
/*lint -esym(715,client_id) */
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDP_ACT_RES,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
}
/*lint +esym(715,act_res_params_ptr) */
/*lint +esym(715,call_id) */
/*lint +esym(715,client_id) */


/*===========================================================================

FUNCTION cm_mm_call_cmd_rab_reestablish

DESCRIPTION
  Command CM to send a RAB reestablish request.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_rab_reestablish(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* answering client */

  cm_call_id_type        call_id,
    /* call ID of the call */

  const cm_rab_reestab_params_s_type   *reestab_params_ptr
    /* pointer to mode specific data */

)
/*lint -esym(715,call_id) */
/*lint -esym(715,client_id) */
/*lint -esym(715,reestab_params_ptr) */
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_RAB_REESTAB,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
}
/*lint +esym(715,call_id) */
/*lint +esym(715,client_id) */
/*lint +esym(715,reestab_params_ptr) */



/*===========================================================================

FUNCTION cm_mm_call_cmd_pdp_modify

DESCRIPTION
  Command CM to request a PDP modify command.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_pdp_modify(

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* answering client */

  cm_call_id_type               call_id,
    /* call ID of the call */

  const cm_pdp_modify_params_s_type   *modify_params_ptr
    /* pointer to command data */

)
/*lint -esym(715,modify_params_ptr) */
/*lint -esym(715,client_id) */
/*lint -esym(715,call_id) */
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDP_MODIFY,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
}
/*lint +esym(715,modify_params_ptr) */
/*lint +esym(715,client_id) */
/*lint +esym(715,call_id) */


/*===========================================================================

FUNCTION cm_mm_ss_cmd_ps_data_available

DESCRIPTION
  Inform CM that data is available for transmission.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_ss_cmd_ps_data_available(

  cm_ss_cmd_cb_f_type            cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type              client_id
    /* requesting client ID */

)
/*lint -esym(715,client_id) */
{
  #if ( (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && \
         defined(FEATURE_PS_DORMANT_PWR_SAVE) )
#error code not present
  #else
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_SS_CMD_PS_DATA_AVAIL,
                 CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    /* The function is unavailable due to relavent feature being undefined */
  }
  return FALSE;
  #endif /* else of ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) &&
                     defined(FEATURE_PS_DORMANT_PWR_SAVE)) */
}
/*lint +esym(715,client_id) */

/*===========================================================================

FUNCTION cm_mm_call_cmd_ps_data_available

DESCRIPTION
  Inform CM that data is available for transmission.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_ps_data_available(

  cm_call_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type              client_id
    /* requesting client ID */

)
/*lint -esym(715,client_id) */
{
  #if ( (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && \
        !defined(FEATURE_PS_DORMANT_PWR_SAVE) )
#error code not present
  #else /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_RAB_REESTAB,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) &&
                    !defined(FEATURE_PS_DORMANT_PWR_SAVE)) */
}
/*lint +esym(715,client_id) */


/*===========================================================================

FUNCTION cm_mm_call_cmd_ps_sig_release

DESCRIPTION
  Request CM to release PS signaling.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_ps_sig_release(

  cm_call_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type              client_id
    /* requesting client ID */

)
/*lint -esym(715,client_id) */
{
  #if((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && \
       defined(FEATURE_UE_INITIATED_DORMANCY))
#error code not present
  #else /* #if((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && \
        ** defined(FEATURE_UE_INITIATED_DORMANCY))
        */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_CALL_CMD_PS_SIG_REL,
                 CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    /* The function is unavailable due to relavent feature being undefined */
  }
  return FALSE;

  #endif /* end of (#if((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && \
         ** defined(FEATURE_UE_INITIATED_DORMANCY))
         */
}
/*lint +esym(715,client_id) */

/*===========================================================================

FUNCTION cm_mm_call_cmd_end

DESCRIPTION
  Command CM to end a call - in origination, incoming or conversation state.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_end(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* ending client */

  uint8                  num_call_ids,
    /* number of call IDs sent */

  const cm_end_params_s_type   *end_params_ptr
    /* pointer to mode specific data */

)
{
  int                          i;
  cm_cmd_s_type                *cmd_ptr;
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------------------------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_call_cmd_end()",0,0,0 );
  CM_MSG_LOW( "--------------------------------------------------",0,0,0 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_END, CM_CALL_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    call_cmd_ptr      = CALL_CMD_PTR( cmd_ptr );
    cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    call_cmd_ptr->cmd_cb_func      = cmd_cb_func;
    call_cmd_ptr->data_block_ptr   = data_block_ptr;
    call_cmd_ptr->client_id        = client_id;
    call_cmd_ptr->cmd              = CM_CALL_CMD_END;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->end_status       = CM_CALL_END_CLIENT_END;

    if ( num_call_ids > CM_CALL_ID_MAX )
    {
      cmd_info_ptr->cmd_err          = CM_CALL_CMD_ERR_NUM_CALL_IDS_P;
      cmd_info_ptr->num_end_call_ids = CM_CALL_ID_MAX;
      CM_ERR("Invalid num_call_ids: %d",num_call_ids,0,0);
    }
    else if ( num_call_ids < 1 )
    {
      cmd_info_ptr->cmd_err          = CM_CALL_CMD_ERR_NUM_CALL_IDS_P;
      cmd_info_ptr->num_end_call_ids = 0;
      CM_ERR("Invalid num_call_ids: %d",num_call_ids,0,0);
    }
    else
    {
      cmd_info_ptr->num_end_call_ids = num_call_ids;
    }

    if ( end_params_ptr == NULL )
    {
      cmd_info_ptr->cmd_err          = CM_CALL_CMD_ERR_NULL_PARAM_P;
      CM_ERR("Null end params ptr",0,0,0);
    }

    if ( cmd_info_ptr->cmd_err == CM_CALL_CMD_ERR_NOERR )
    {
      for ( i = 0; i < cmd_info_ptr->num_end_call_ids; i++ )
      {
        if ( end_params_ptr != NULL )
        {

          memcpy ( &cmd_info_ptr->end_params[i],
                   &end_params_ptr[i],
                   sizeof( cm_end_params_s_type ) );
        }
      }

    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM. */
    cm_cmd_queue( cmd_ptr );
    return TRUE;
  }


} /* cm_mm_call_cmd_end */


#if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
     defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM))
#error code not present
#endif



/*===========================================================================

FUNCTION cm_mm_call_cmd_privacy_pref

DESCRIPTION
  Command CM to change the privacy preference.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_privacy_pref
(
  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* ending client */

  cm_call_id_type        call_id,
    /* call ID of the call */

  cm_privacy_pref_e_type  privacy_pref
    /* privacy pref setting */
)
/*lint -esym(715, privacy_pref) */
/*lint -esym(715, call_id) */
/*lint -esym(715, client_id) */
{
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cm_cmd_s_type                *cmd_ptr;
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;
  cm_call_cmd_info_cdma_s_type *cmd_mode_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "---------------------CALL CMD---------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_call_cmd_privacy_pref()",0,0,0 );
  CM_MSG_LOW( "--------------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* allocate and initialize a Call command buffer */

  cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_CALL_CMD_PRIVACY_PREF,
                   CM_CALL_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /*
    ** and a reference to command information.
    */
    call_cmd_ptr       = CALL_CMD_PTR( cmd_ptr );
    cmd_info_ptr       = CMD_INFO_PTR( call_cmd_ptr );
    cmd_mode_info_ptr  = CALL_CMD_MODE_INFO_PTR( call_cmd_ptr );
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    call_cmd_ptr->cmd_cb_func       = cmd_cb_func;
    call_cmd_ptr->data_block_ptr    = data_block_ptr;
    call_cmd_ptr->client_id         = client_id;
    call_cmd_ptr->cmd               = CM_CALL_CMD_PRIVACY_PREF;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->call_id           = call_id;
    cmd_mode_info_ptr->privacy_pref = privacy_pref;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );
    return TRUE;
  }
  #else /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_PRIVACY_PREF,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

} /* cm_mm_call_cmd_privacy_pref() */
/*lint +esym(715, privacy_pref) */
/*lint +esym(715, call_id) */
/*lint +esym(715, client_id) */



/*===========================================================================

FUNCTION cm_mm_call_cmd_sups

DESCRIPTION
  Command CM to send a call related supplementary service.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_sups(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client_id,
    /* sending client */

  cm_call_sups_type_e_type    sups_type,
    /* Type of supplementary service */

  const cm_call_sups_params_s_type  *sups_params_ptr
    /* Pointer to supplementary service specific data */

)
{

  cm_cmd_s_type                  *cmd_ptr;
  cm_call_cmd_s_type             *call_cmd_ptr;
  cm_call_cmd_info_s_type        *cmd_info_ptr;

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #endif /* #if defined FEATURE_GSM || defined (FEATURE_WCDMA) */

  cm_call_cmd_err_e_type         cmd_err = CM_CALL_CMD_ERR_NOERR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH( "cm_mm_call_cmd_sups()",0,0,0 );
  CM_MSG_HIGH( "client_id = %d, sups_type = %d", client_id, sups_type, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* All SUPS commands need to provide parameters
  */
  if( sups_params_ptr == NULL )
  {
    cmd_err = CM_CALL_CMD_ERR_NULL_NUM_P;
  }
  else
  {
    /* SUPS number needs to be at least initialized
    */
    if(cm_num_validate(&sups_params_ptr->sups_number) != CM_NUM_ERR_NOERR)
    {
      cmd_err = CM_CALL_CMD_ERR_NO_NUM_P;
    }

    /* SUPS call commands need to provide valid call id,
    ** ie. call id of an existing call
    */
    if(cmcall_ptr(sups_params_ptr->call_id) == NULL)
    {
      cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
    }
  }

  /* Reject the command right here if errors found
  */
  if(cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    if(cmd_cb_func != NULL)
    {
      cmd_cb_func( data_block_ptr,CM_CALL_CMD_SUPS,cmd_err);
    }
    CM_MSG_HIGH ("SUPS command error - %d", cmd_err, 0 , 0);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command is not rejected, try allocating a CM command buffer.
  */
  cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
  if(cmd_ptr == NULL)
  {
    if(cmd_cb_func != NULL)
    {
      cmd_cb_func( data_block_ptr,CM_CALL_CMD_SUPS,CM_CALL_CMD_ERR_NO_BUF_L);
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, command buffer is allocated successfully.
  */

  /* Copy command header fields.
  */
  call_cmd_ptr                  = CALL_CMD_PTR( cmd_ptr );
  call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  call_cmd_ptr->data_block_ptr  = data_block_ptr;
  call_cmd_ptr->client_id       = client_id;
  call_cmd_ptr->cmd             = CM_CALL_CMD_SUPS;

  /*
  ** Copy command information fields.
  */
  cmd_info_ptr                  = CMD_INFO_PTR( call_cmd_ptr );
  cmd_info_ptr->call_id         = sups_params_ptr->call_id;

  /* Copy sups type
  */
  cmd_info_ptr->sups_cmd_type   = sups_type;

  /* info_type for processing this command is learnt from the call object
  ** pertaining to this call_id
  */

  #if defined FEATURE_GSM || defined (FEATURE_WCDMA)
#error code not present
  #endif /* #if defined FEATURE_GSM || defined (FEATURE_WCDMA) */

  #if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900))
  /*
  ** Copy command information fields - 1X specific
  */
  if (( sups_type == CM_CALL_SUPS_TYPE_EMERGENCY_CALL) ||
      ( sups_type == CM_CALL_SUPS_TYPE_SIMPLE_FLASH) ||
      ( sups_type == CM_CALL_SUPS_TYPE_CALL_FORWARD )
      #ifdef FEATURE_T53
      || ( sups_type == CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD )
      || ( sups_type == CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD)
      #endif  /* FEATURE_T53 */
  )
  {
      /*
      ** Copy command information fields.
      */
      cmd_info_ptr->cdma_info.flash_type = sups_type;
      cm_num_copy( &cmd_info_ptr->num,
                          sups_params_ptr->sups_number.buf,
                          sups_params_ptr->sups_number.len);

  } /* if (( sups_type == CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL) || */
  #endif /* (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -restore */

  /* Queue the command to CM.
  */
  cm_cmd_queue( cmd_ptr );

  return TRUE;
} /* cm_mm_call_cmd_sups() */



/*===========================================================================

FUNCTION cm_mm_call_cmd_get_call_info

DESCRIPTION
  Get a sanpshot of the current call information in the CM

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request


SIDE EFFECTS
  none

===========================================================================*/
boolean cm_mm_call_cmd_get_call_info(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* requesting client ID */

  cm_call_id_type        call_id
    /* call ID of the call */

)
{

    cm_cmd_s_type                  *cmd_ptr;
    cm_call_cmd_s_type             *call_cmd_ptr;
    cm_call_cmd_info_s_type   *cmd_info_ptr;

    /*----------------------------------------------------------------------*/


    CM_MSG_LOW( "--------------------------------------------------",0,0,0 );
    CM_MSG_LOW( "cm_mm_call_cmd_get_call_info()",0,0,0 );
    CM_MSG_LOW( "--------------------------------------------------",0,0,0 );

    /*----------------------------------------------------------------------*/

    cmd_ptr = cm_cmd_alloc_call_init();

    /* If command buffer allocation failed,
    ** call the command callback and return FALSE
    */

    if( cmd_ptr == NULL )
    {
      if( cmd_cb_func != NULL )
      {
        cmd_cb_func(data_block_ptr, CM_CALL_CMD_INFO_GET,
            CM_CALL_CMD_ERR_NO_BUF_L);
      }

      return FALSE;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Else (command buffer allocation is successful),
      ** copy command information into command structure.
      */
    else
    {
      /* Get a reference to command header
      */
      call_cmd_ptr = CALL_CMD_PTR( cmd_ptr );
      cmd_info_ptr = CMD_INFO_PTR( call_cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      call_cmd_ptr->data_block_ptr  = data_block_ptr;
      call_cmd_ptr->client_id       = client_id;
      call_cmd_ptr->cmd             = CM_CALL_CMD_INFO_GET;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command information fields.
      */
      cmd_info_ptr->call_id         = call_id;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
      cm_cmd_queue( cmd_ptr );

      return TRUE;
    }
}




/*===========================================================================

FUNCTION cm_mm_call_cmd_get_call_info_list

DESCRIPTION
  Get a sanpshot of the state of all call objects maintained by CM.  This
  is a synchronous function.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_get_call_info_list(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id
    /* requesting client ID */

)
{

  cm_cmd_s_type                  *cmd_ptr;
  cm_call_cmd_s_type             *call_cmd_ptr;

  /*----------------------------------------------------------------------*/


  CM_MSG_LOW( "--------------------------------------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_call_cmd_get_call_info_list()",0,0,0 );
  CM_MSG_LOW( "--------------------------------------------------",0,0,0 );

  /*----------------------------------------------------------------------*/

  cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func(data_block_ptr, CM_CALL_CMD_INFO_LIST_GET,
          CM_CALL_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */
  else
  {
    /* Get a reference to command header
    */
    call_cmd_ptr = CALL_CMD_PTR( cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_INFO_LIST_GET;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  }

} /* cm_mm_call_cmd_get_call_info_list */




/*===========================================================================

FUNCTION cm_mm_call_cmd_umts_cdma_handover_req

DESCRIPTION
  Command CM to complete the handoff from UMTS -> 1x.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_umts_cdma_handover_req
(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* originating client */

  cm_call_type_e_type    call_type,
    /* call type - voice, data, etc. */

  cm_call_state_e_type   call_state,
    /* Call state - orig, incoming or conv */

  const sys_is2000_msg_s_type  *ho_msg_ptr,
    /* Pointer to CDMA message from UTRAN */

  cm_call_id_type        *return_call_id_ptr
    /* call ID will be allocated and returned */

)
/*lint -esym(715, ho_msg_ptr) */
/*lint -esym(715, call_state) */
/*lint -esym(715, call_type) */
{
  cm_cmd_s_type                *cmd_ptr;
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;
  cm_call_cmd_err_e_type       cmd_err = CM_CALL_CMD_ERR_NOERR;
  cmcall_s_type                *call_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "---------------------CALL CMD---------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_call_cmd_umts_cdma_handover_req()",0,0,0 );
  CM_MSG_LOW( "--------------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* allocate and initialize a Call command buffer */
  cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_CALL_CMD_UMTS_CDMA_HANDOVER_REQ,
                   CM_CALL_CMD_ERR_NO_BUF_L
                 );
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information into command structure.
  */
  call_cmd_ptr = CALL_CMD_PTR( cmd_ptr );
  call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  call_cmd_ptr->data_block_ptr  = data_block_ptr;
  call_cmd_ptr->client_id       = client_id;
  call_cmd_ptr->cmd             = CM_CALL_CMD_UMTS_CDMA_HANDOVER_REQ;

  /* Allocate call id.
  */
  if(return_call_id_ptr == NULL)
  {
    cmd_err = CM_CALL_CMD_ERR_NULL_PARAM_P;
  }
  else
  if ((*return_call_id_ptr = cm_call_id_allocate()) == CM_CALL_ID_INVALID )
  {
    cmd_err = CM_CALL_CMD_ERR_NO_CALL_ID_S;
  }
  else
  {
    call_ptr = cmcall_ptr (*return_call_id_ptr);

    if(call_ptr == NULL)
    {
      cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
    }
    else
    {
      cmcall_set_call_direction (call_ptr, CM_CALL_DIRECTION_MO);
    }
  }

  /* No parameter errors found. Now pack cmd info and queue it for CM
  */
  cmd_info_ptr = CMD_INFO_PTR( call_cmd_ptr );
  if (cmd_err == CM_CALL_CMD_ERR_NOERR)
  {
    /*
    ** Copy command information fields.
    */
    cmd_info_ptr->call_id                   = *return_call_id_ptr;
    cmd_info_ptr->call_type                 = call_type;
    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
    #endif

  } /* if (cmd_err == CM_CALL_CMD_ERR_NOERR) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( cmd_ptr );
  return TRUE;

} /* cm_mm_call_cmd_umts_cdma_handover_req() */
/*lint +esym(715, ho_msg_ptr) */
/*lint +esym(715, call_state) */
/*lint +esym(715, call_type) */

/**===========================================================================

@FUNCTION cm_mm_call_cmd_handover_req

@DESCRIPTION
  Command CM to complete the handoff between different RAT.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

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

)
/*lint -esym(715, ho_msg_ptr) */
/*lint -esym(715, call_state) */
/*lint -esym(715, call_type) */
/*lint -esym(715, return_call_id_ptr) */
/*lint -esym(715, cmd_cb_func) */
/*lint -esym(818, return_call_id_ptr) */
/*lint -esym(715, data_block_ptr) */
/*lint -esym(818, data_block_ptr)*/
/*lint -esym(715, client_id) */
{
  #ifdef FEATURE_IMS_VCC
  cm_cmd_s_type                *cmd_ptr;
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;
  cm_call_cmd_err_e_type        cmd_err = CM_CALL_CMD_ERR_NOERR;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "---------------------CALL CMD---------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_call_cmd_umts_cdma_handover_req()",0,0,0 );
  CM_MSG_LOW( "--------------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** allocate and initialize a Call command buffer */
  cmd_ptr = cm_cmd_alloc_call_init();

  /** If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_CALL_CMD_HANDOVER_REQ,
                   CM_CALL_CMD_ERR_NO_BUF_L
                 );
    }

    return FALSE;
  }

  /** Copy command information into command structure.
  */
  call_cmd_ptr = CALL_CMD_PTR( cmd_ptr );
  call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  call_cmd_ptr->data_block_ptr  = data_block_ptr;
  call_cmd_ptr->client_id       = client_id;
  call_cmd_ptr->cmd             = CM_CALL_CMD_HANDOVER_REQ;

  /** Allocate call id.
  */
  if( return_call_id_ptr == NULL )
  {
    cmd_err = CM_CALL_CMD_ERR_NULL_PARAM_P; 

  }
  else if( (*return_call_id_ptr = cm_call_id_allocate()) == CM_CALL_ID_INVALID )
  {
    cmd_err = CM_CALL_CMD_ERR_NO_CALL_ID_S;
  }
  else
  {
    cmcall_set_call_direction (cmcall_ptr (*return_call_id_ptr),
                               CM_CALL_DIRECTION_MO);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 
  /** No parameter errors found. Now pack cmd info and queue it for CM
  */
  cmd_info_ptr = CMD_INFO_PTR( call_cmd_ptr );
  cmd_info_ptr->cmd_err = cmd_err;

  if ( cmd_err == CM_CALL_CMD_ERR_NOERR )
  {
    cmd_info_ptr->call_id = *return_call_id_ptr;
    cmd_info_ptr->call_type = call_type;
  }
 
  cm_cmd_queue( cmd_ptr );
  return TRUE;

  #else
  /* Feature FEATURE_IMS_VCC is undefined so we return FALSE */
  return FALSE;
  #endif

} 
/*lint +esym(715, ho_msg_ptr) */ 
/*lint +esym(715, call_state) */
/*lint +esym(715, call_type) */
/*lint +esym(715, return_call_id_ptr) */
/*lint +esym(715, cmd_cb_func) */
/*lint +esym(715, data_block_ptr) */
/*lint +esym(715, client_id) */
/*lint +esym(818, return_call_id_ptr) */
/*lint +esym(818, data_block_ptr)*/


/*===========================================================================

FUNCTION cm_mm_call_cmd_umts_cdma_handover_cnf

DESCRIPTION
  Command to CM to inform about the handover status.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request
  Call_id will be returned in call_id_ptr.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_umts_cdma_handover_cnf(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client_id,
    /* originating client */

  cm_call_id_type             call_id,
    /* call ID of the call */

  sys_umts_cdma_handover_e_type  status
    /* Handover status */

)
/*lint -esym(715,status)*/
{

  cm_cmd_s_type                *cmd_ptr;
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "---------------------CALL CMD---------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_call_cmd_umts_cdma_handover_cnf()",0,0,0 );
  CM_MSG_LOW( "--------------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate call cmd info.
  */
  cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
  if ( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_UMTS_CDMA_HANDOVER_CNF,
          CM_CALL_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information into command structure.
  */
  call_cmd_ptr      = CALL_CMD_PTR( cmd_ptr );
  call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  call_cmd_ptr->data_block_ptr  = data_block_ptr;
  call_cmd_ptr->client_id       = client_id;
  call_cmd_ptr->cmd             = CM_CALL_CMD_UMTS_CDMA_HANDOVER_CNF;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields. */
  cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );
  cmd_info_ptr->call_id         = call_id;
  #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the commands to CM.
  */
  cm_cmd_queue( cmd_ptr );

  return TRUE;

} /* cm_mm_call_cmd_umts_cdma_handover_cnf() */
/*lint +esym(715,status)*/


/*===========================================================================

FUNCTION cm_data_session_reg

DESCRIPTION
  Register data session callback functions with CM

  When call manager ends a data call, it consults the data session callback
  functions for the following information:

     1. Total number of bytes received and transmitted during the data call.

DEPENDENCIES
  CM must have already been initialized with cm_init_before_task_start().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_data_session_status_e_type cm_data_session_reg(

  cm_get_data_session_total_byte_ctrs_cb_f_type
                              *data_session_total_byte_ctrs_cb_func_ptr
    /* Pointer to data session total byte counters callback function */

)
{
  return cmcall_data_session_reg( data_session_total_byte_ctrs_cb_func_ptr );

}  /* cm_data_session_reg */



/*===========================================================================

FUNCTION cm_ph_cmd_oprt_mode

DESCRIPTION
  Command CM to put phone in a specified operating mode, such as offline,
  online, LPM, etc.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_oprt_mode(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  sys_oprt_mode_e_type    oprt_mode
    /* Switch to this operating mode */

)
{

  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------PH CMD--------------------",0,0,0 );
  CM_MSG_HIGH( "cm_ph_cmd_oprt_mode(), mode=%d",oprt_mode,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_OPRT_MODE, CM_PH_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_OPRT_MODE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->oprt_mode  = oprt_mode;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  }
}



#if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
     defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM))
#error code not present
#endif


/*===========================================================================
FUNCTION cm_ph_cmd_meas_mode

DESCRIPTION
  Command CM to put phone in or out of measurement mode. Currently valid only for
  HDR .Measurement mode puts the modem into a special mode that allows monitoring 
  HDR air interface without creating a HDR session or using the existing HDR session 

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  TRUE - request sent, FALSE - No buffer to send request

SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event - CM_PH_EVENT_MEAS_MODE.

===========================================================================*/
boolean cm_ph_cmd_meas_mode(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  sys_meas_mode_e_type   meas_mode
    /* Switch to this operating mode */

)
{
  #ifdef FEATURE_CM_SS_MEAS
#error code not present
  #else /* FEATURE_CM_SS_MEAS */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_MEAS_MODE,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client_id );
  SYS_ARG_NOT_USED( meas_mode);
  return FALSE;
  #endif /* FEATURE_CM_SS_MEAS */
}



/*===========================================================================

FUNCTION cm_ph_cmd_avoid_sys

DESCRIPTION
  Command CM to avoid the system defined in "avoid_type".
  "avoid_time" is no longer used.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  CDMA Only.
  
DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_avoid_sys(

        cm_ph_cmd_cb_f_type    cmd_cb_func,
            /* client callback function */

        void                   *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type      client,
            /* Requesting client */

        cm_ss_avoid_sys_e_type avoid_type,
            /* type of the system to avoid */

        dword                  avoid_time
            /* OBSOLETE - avoid time in seconds */

)
{

  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------PH CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_ph_cmd_avoid_sys(), avoid_type=%d", avoid_type,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_AVOID_SYS, CM_PH_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_AVOID_SYS;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->avoid_type  = avoid_type;
    cmd_info_ptr->avoid_time  = avoid_time;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  }
}



/*===========================================================================

FUNCTION cm_ph_cmd_sys_sel_pref

DESCRIPTION
  Command CM to change the phone's system selection preference (i.e., the
  mode, band and roam preference).

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_sys_sel_pref(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                              *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  cm_mode_pref_e_type               mode_pref,
    /* Switch to this mode preference */

  cm_pref_term_e_type               term_pref,
    /* Switch the preference for this term */

  dword                             pref_duration,
    /* Specify the duration in seconds for temporary term */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /* gw acquisition order preference */

  cm_band_pref_e_type               band_pref,
    /* Switch to this band preference */

  cm_prl_pref_e_type                prl_pref,
    /* Switch to this prl preference */

  cm_roam_pref_e_type               roam_pref,
    /* Switch to this roam preference */

  cm_hybr_pref_e_type               hybr_pref,
    /* Switch to this hybrid preference */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /* Switch to this service domain (e.g. Circuit Switch)
    ** Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
    ** domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /* Defines whether the network should be selected automatically
    ** or manually */

  const sys_plmn_id_s_type                *plmn_ptr,
    /* If network selection is set to manual, this specifies plmn id */

  const cm_wlan_pref_s_type               *wlan_pref_ptr
    /* If WLAN network selection is set to manual, this specifies SSID,
    ** channle, BSSId etc.
    */

)
{
  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "---------------------PH CMD---------------------",0,0,0 );
  CM_MSG_LOW( "cm_ph_cmd_ss_pref()",0,0,0 );
  CM_MSG_LOW( "-------------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_SYS_SEL_PREF, CM_PH_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {

    /* Variables for printing PLMN.
    */
    boolean plmn_id_is_undefined;
    boolean mnc_includes_pcs_digit;
    uint32  mcc;
    uint32  mnc;

    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr          = data_block_ptr;
    ph_cmd_ptr->client_id               = client_id;
    ph_cmd_ptr->cmd                     = CM_PH_CMD_SYS_SEL_PREF;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->mode_pref             = mode_pref;
    cmd_info_ptr->pref_term             = term_pref;
    cmd_info_ptr->pref_duration         = pref_duration;
    cmd_info_ptr->acq_order_pref        = acq_order_pref;
    cmd_info_ptr->band_pref             = band_pref;
    cmd_info_ptr->prl_pref              = prl_pref;
    cmd_info_ptr->roam_pref             = roam_pref;
    cmd_info_ptr->hybr_pref             = hybr_pref;
    cmd_info_ptr->srv_domain_pref       = srv_domain_pref;
    cmd_info_ptr->network_sel_mode_pref = network_sel_mode_pref;

    if ( plmn_ptr != NULL )
    {
      cmd_info_ptr->plmn                = *plmn_ptr;
    }
    else
    {
      sys_plmn_undefine_plmn_id( &cmd_info_ptr->plmn );
    }

    if ( wlan_pref_ptr != NULL )
    {
      cmd_info_ptr->wlan_pref               = *wlan_pref_ptr;
      cmd_info_ptr->is_wlan_pref_included   = TRUE;
    }
    else
    {
      cmd_info_ptr->is_wlan_pref_included   = FALSE;

    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Print debug messages for params.
    */
    CM_MSG_HIGH("sys_sel_pref(),mode_pref %ld,pref_term %d,acq_order_pref %d",
                mode_pref, term_pref, acq_order_pref
               );
    CM_MSG_HIGH("sys_sel_pref(),band_pref %ld,roam_pref %d,hybr_pref %d",
                band_pref, roam_pref, hybr_pref
               );
    CM_MSG_HIGH("sys_sel_pref(),domain_pref %ld,net_sel_mode_pref %d prl_pref %d",
                srv_domain_pref, network_sel_mode_pref, prl_pref
               );
    if ( wlan_pref_ptr != NULL )
    {
      CM_MSG_HIGH("sys_sel_pref(), WLAN net_sel %d, tech 0x%x, bss_pref %d",
                  wlan_pref_ptr->network_sel_mode_pref, wlan_pref_ptr->tech_pref,
                  wlan_pref_ptr->bss_type_pref );
      CM_MSG_HIGH("sys_sel_pref(), WLAN scan %d",
                  wlan_pref_ptr->scan_mode,0,0 );
    }


    sys_plmn_get_mcc_mnc
    (
      cmd_info_ptr->plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );

    CM_MSG_HIGH("PLMN - undefined %d pcs_digit %d",
                plmn_id_is_undefined,
                mnc_includes_pcs_digit,
                0
               );
    CM_MSG_HIGH("PLMN(%d-%d)", mcc, mnc, 0);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  }
}



/*===========================================================================

FUNCTION cm_ph_cmd_ddtm_pref

DESCRIPTION
  Command CM to change the phone's data dedicated transmission mode preference
  to a specified selection, such as ON or OFF etc.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_ddtm_pref(

  cm_ph_cmd_cb_f_type        cmd_cb_func,
    /* client callback function.
    */

  void                       *data_block_ptr,
    /* pointer to client callback data block.
    */

  cm_client_id_type           client_id,
    /* Requesting client.
    */


  cm_ddtm_pref_e_type        ddtm_pref,
    /* Switch to this DDTM preference.
    */

  sys_ddtm_act_mask_e_type   ddtm_act,
    /* When DDTM is enabled, this field specifies what combined
    ** DDTM actions should takes place to enhance HDR data thru'put.
    ** This is bit-mapped enum, with each bit specifying action.
    */

  cm_ddtm_so_list_act_e_type so_list_act,
    /* Specifies what to do ( add, replace, delete ) with the user
    ** specified SO list in the following parameters.
    */

  uint16                     num_srv_opt,
    /* Number of SOs specified in list,
    ** MAX = SYS_DDTM_MAX_SO_LIST_SIZE
    */

  const sys_srv_opt_type     srv_opt_list[]
    /* List of SOs for which 1x pages have to ignored while in
    ** HDR connected state. if srv_opt_list[] contains CAI_SO_NULL
    ** & ddtm_pref is ON & ddtm_act includes ignore_so_pages,
    ** ignore all pages.
    */

)
/*lint -esym(715, ddtm_pref) */
/*lint -esym(715, ddtm_act) */
/*lint -esym(715, so_list_act) */
/*lint -esym(715, num_srv_opt) */
/*lint -esym(715, client_id) */
/*lint -esym(715, srv_opt_list) */
{
  #ifdef FEATURE_DDTM_CNTL
  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;
  int i;
  cm_ph_cmd_err_e_type     cmd_err = CM_PH_CMD_ERR_NOERR;

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    cmd_err = CM_PH_CMD_ERR_NO_BUF_L;
  }
  else
  if(srv_opt_list == NULL)
  {
    cmd_err = CM_PH_CMD_ERR_DDTM_SRV_OPTIONS_LIST_P;
  }

  if(cmd_err != CM_PH_CMD_ERR_NOERR)
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_DDTM_PREF, cmd_err);
    }
    /* Free the memory */
    if ( cmd_ptr != NULL )
    {
      cm_cmd_dealloc(cmd_ptr); 
    } 
    return FALSE;

  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to command header
  ** and a reference to command information.
  */
  ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr  = data_block_ptr;
  ph_cmd_ptr->client_id       = client_id;
  ph_cmd_ptr->cmd             = CM_PH_CMD_DDTM_PREF;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->ddtm_pref  = ddtm_pref;
  cmd_info_ptr->ddtm_act_mask = ddtm_act;
  cmd_info_ptr->ddtm_so_list_act = so_list_act;
  cmd_info_ptr->ddtm_num_srv_opt = num_srv_opt;
  for ( i = 0; i < num_srv_opt && i < SYS_DDTM_MAX_SO_LIST_SIZE; i++  )
  {
    cmd_info_ptr->ddtm_srv_opt_list[i] = srv_opt_list[i];
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( cmd_ptr );

  return TRUE;

  #else /* FEATURE_DDTM_CNTL */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_DDTM_PREF,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* FEATURE_DDTM_CNTL */

} /* cm_ph_cmd_ddtm_pref() */
/*lint +esym(715, ddtm_pref) */
/*lint +esym(715, ddtm_act) */
/*lint +esym(715, so_list_act) */
/*lint +esym(715, num_srv_opt) */
/*lint +esym(715, client_id) */
/*lint +esym(715, srv_opt_list) */



/*===========================================================================

FUNCTION cm_ph_cmd_answer_voice

DESCRIPTION
  Command CM to change the phone's answer-voice-as setting to a specified
  selection, such as answer-voice-as-data, answer-voice-as-voice, etc.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_answer_voice(

        cm_ph_cmd_cb_f_type        cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type          client,
            /* Requesting client */

        cm_answer_voice_e_type     answer_voice,
            /* Indicate whether phone should answer incoming
            ** voice calls as voice, fax or modem */

        dword                      answer_duration
            /* Indicate seconds duration for temp. setting, such as
            ** CM_ANSWER_VOICE_AS_FAX_ONCE or DB_VOICE_AS_MODEM_ONCE */
)
/*lint -esym(715, client) */
/*lint -esym(715, answer_voice) */
/*lint -esym(715, answer_duration) */
{
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "---------------------PH CMD---------------------",0,0,0 );
  CM_MSG_LOW( "cm_ph_cmd_answer_voice()",0,0,0 );
  CM_MSG_LOW( "-------------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_ANSWER_VOICE, CM_PH_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Else (command buffer allocation is successful),
   ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_ANSWER_VOICE;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command information fields.
      */
      cmd_info_ptr->answer_voice     = answer_voice;
      cmd_info_ptr->answer_duration  = answer_duration;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  }
  #else /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_ANSWER_VOICE,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

}
/*lint +esym(715, client) */
/*lint +esym(715, answer_voice) */
/*lint +esym(715, answer_duration) */



/*===========================================================================

FUNCTION cm_ph_cmd_nam_sel

DESCRIPTION
  Command CM to change the phone's NAM selection to NAM-1, NAM-2, etc.

  NOTE! This command fails when trying to change the NAM to illegal
  selection. E.g. changing to NAM-2 for a 1 NAM target.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_nam_sel(

        cm_ph_cmd_cb_f_type        cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client,
            /* Requesting client */

        cm_nam_e_type               nam_sel
            /* Change NAM selection to this NAM */

)
/*lint -esym(715, client) */
/*lint -esym(715, nam_sel) */
{
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) // Gemsea Modify
  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "---------------------PH CMD---------------------",0,0,0 );
  CM_MSG_LOW( "cm_ph_cmd_nam_sel()",0,0,0 );
  CM_MSG_LOW( "-------------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_NAM_SEL, CM_PH_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    ph_cmd_ptr = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_NAM_SEL;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->nam_sel       = nam_sel;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );
    return TRUE;

  }
  #else /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_NAM_SEL,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
} /* cm_ph_cmd_nam_sel() */
/*lint +esym(715, client) */
/*lint +esym(715, nam_sel) */




/*===========================================================================

FUNCTION cm_ph_cmd_get_ph_info

DESCRIPTION
  Get a sanpshot of the current phone information in the CM

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_cmd_get_ph_info(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

  void                   *data_block_ptr,
        /* pointer to client callback data block */

  cm_client_id_type      client
        /* requesting client ID */

)
{

  cm_cmd_s_type             *cmd_ptr;
  cm_ph_cmd_s_type          *ph_cmd_ptr;


  CM_MSG_LOW( "--------------------PH CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_ph_cmd_get_ph_info()",0,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );


  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_INFO_GET, CM_PH_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */
  else
  {
    /* Get a reference to command header
    */
    ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_INFO_GET;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;

  }

}


/*===========================================================================

FUNCTION cm_ph_cmd_rssi_delta_threshold

DESCRIPTION
  Set the RSSI delta value.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
   TRUE - request sent, FALSE - No buffer to send request
   
SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_cmd_rssi_delta_threshold(

    cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                   *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type      client,
        /* requesting client ID */

    uint8                  delta
        /* RSSI change delta threshold for RSSI reporting */
)
{

  /* Call the new api and pass on the delta for rssi
  */
  return( cm_ph_cmd_signal_strength_delta
          ( 
            cmd_cb_func, 
            data_block_ptr, 
            client, 
            delta, 
            CM_SIG_STRENGTH_DELTA_NO_CHANGE,
            CM_SIG_STRENGTH_DELTA_NO_CHANGE
           )
        );
}


/*===========================================================================

FUNCTION cm_ph_cmd_signal_strength_delta

DESCRIPTION
  Set the RSSI/ECIO/IO delta value.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
   TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_cmd_signal_strength_delta(

    cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                   *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type      client,
        /* requesting client ID */

    uint8                  rssi_delta,
        /* RSSI change delta threshold for Signal Strength reporting */

    uint8                  ecio_delta,
        /* ECIO change delta threshold for Signal Strength  reporting */

    uint8                  io_delta
        /* IO change delta threshold for Signal Strength  reporting */
)
{
  /* Call the new api and pass on the delta for rssi,ecio,io
  */
  return( cm_ph_cmd_signal_strength_delta2
          (
            cmd_cb_func,
            data_block_ptr,
            client,
            rssi_delta,
            ecio_delta,
            io_delta,
            CM_SIG_STRENGTH_DELTA_NO_CHANGE,
            CM_SIG_STRENGTH_DELTA_NO_CHANGE
           )
        );

} /* cm_ph_cmd_signal_strength_delta */



/*===========================================================================

FUNCTION cm_ph_cmd_signal_strength_delta2

DESCRIPTION
  Set the RSSI/ECIO/IO/SIR/PATHLOSS delta value.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
   TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_cmd_signal_strength_delta2(

    cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                   *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type      client,
        /* requesting client ID */

    uint8                  rssi_delta,
        /* RSSI change delta threshold for Signal Strength reporting */

    uint8                  ecio_delta,
        /* ECIO change delta threshold for Signal Strength  reporting */

    uint8                  io_delta,
        /* IO change delta threshold for Signal Strength  reporting */

    uint8                  sir_delta,
        /* SIR change delta threshold for Signal Strength  reporting */

    uint8                  pathloss_delta
        /* PATHLOSS change delta threshold for Signal Strength  reporting */
)
{
  cm_cmd_s_type             *cmd_ptr;
  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;

  CM_MSG_HIGH( "--------------------PH CMD--------------------",0,0,0 );
  CM_MSG_HIGH( "cm_ph_cmd_signal_strength_delta2(), rssi_delta=%d,\
               ecio_delta=%d, io_delta=%d ", 
               rssi_delta, ecio_delta, io_delta );
  CM_MSG_HIGH( "sir_delta=%d,pathloss_delta=%d",
               sir_delta, pathloss_delta, 0 );
  CM_MSG_HIGH( "----------------------------------------------",0,0,0 );

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, 
                   CM_PH_CMD_SIGNAL_STRENGTH_DELTA, 
                   CM_PH_CMD_ERR_NO_BUF_L );
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to command header
  ** and a reference to command information.
  */
  ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr  = data_block_ptr;
  ph_cmd_ptr->client_id       = client;
  ph_cmd_ptr->cmd             = CM_PH_CMD_SIGNAL_STRENGTH_DELTA;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->rssi_delta    = rssi_delta;
  cmd_info_ptr->ecio_delta    = ecio_delta;
  cmd_info_ptr->io_delta      = io_delta;
  cmd_info_ptr->sir_delta      = sir_delta;
  cmd_info_ptr->pathloss_delta = pathloss_delta;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( cmd_ptr );

  return TRUE;

} /* cm_ph_cmd_signal_strength_delta2 */


/*===========================================================================

FUNCTION cm_ph_cmd_packet_state

DESCRIPTION
  Command CM to change Rm packet state in packet mode.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_packet_state(

        cm_ph_cmd_cb_f_type        cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client_id,
            /* Requesting client */

        cm_packet_state_e_type      packet_state
            /* Change Rm packet state to this state */

)
{
  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "---------------------PH CMD---------------------",0,0,0 );
  CM_MSG_LOW( "cm_ph_cmd_packet_state()",0,0,0 );
  CM_MSG_LOW( "-------------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_PACKET_STATE, CM_PH_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    ph_cmd_ptr = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_PACKET_STATE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->packet_state  = packet_state;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );
    return TRUE;

  }
}



/*===========================================================================

FUNCTION cm_ph_cmd_voc_activity

DESCRIPTION
  Command CM to activate voice activity test for current or next voice call.
  On call termination, the voice activity test will be disabled.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_voc_activity(

        cm_ph_cmd_cb_f_type    cmd_cb_func,
            /* client callback function */

        void                   *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type      client,
            /* Requesting client */

        boolean                enable,
            /* enable/disable voice activity test */

        word                   voc_1,
            /* # of full rate */

        word                   voc_2,
            /* # of 1/2 rate */

        word                   voc_8
            /* # of 1/8 rate */

)
/*lint -esym(715, client) */
/*lint -esym(715, enable) */
/*lint -esym(715, voc_1) */
/*lint -esym(715, voc_2) */
/*lint -esym(715, voc_8) */
{
  #ifdef FEATURE_JCDMA
  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------PH CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_ph_cmd_voc_activity",0,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_VOC_ACTIVITY, CM_PH_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id          = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_VOC_ACTIVITY;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->voc_act_enable  = enable;
    cmd_info_ptr->voc_act_voc_1   = voc_1;
    cmd_info_ptr->voc_act_voc_2   = voc_2;
    cmd_info_ptr->voc_act_voc_8   = voc_8;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  }
  #else /* FEATURE_JCDMA */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_VOC_ACTIVITY,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of FEATURE_JCDMA */
}
/*lint +esym(715, client) */
/*lint +esym(715, enable) */
/*lint +esym(715, voc_1) */
/*lint +esym(715, voc_2) */
/*lint +esym(715, voc_8) */


/*===========================================================================

FUNCTION cm_ph_cmd_subscription_available

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  has been verified and available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_available(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /* client callback function */

  void                            *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /* Subscription status of CDMA system */

  cm_subscription_status_e_type   gw_status,
    /* Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   wlan_status
    /* Subscription status of WLAN system */

)
{

  cm_cmd_s_type             *cmd_ptr;
  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;
  boolean                    status;

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_PH_CMD_SUBSCRIPTION_AVAILABLE,
                   CM_PH_CMD_ERR_NO_BUF_L );
    }

    status = FALSE;
  }  /* if ( cmd_ptr == NULL ) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func        = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr     = data_block_ptr;
    ph_cmd_ptr->client_id          = client_id;
    ph_cmd_ptr->cmd                = CM_PH_CMD_SUBSCRIPTION_AVAILABLE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->cdma_sub_avail_status = cdma_status;
    cmd_info_ptr->gw_sub_avail_status   = gw_status;
    cmd_info_ptr->wlan_sub_avail_status = wlan_status;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    status = TRUE;
  } /* else if ( cmd_ptr == NULL ) */

  return status;

} /* cm_ph_cmd_subscription_available */



/*===========================================================================

FUNCTION cm_ph_cmd_subscription_not_available

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is not available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_not_available(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /* client callback function */

  void                            *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /* Subscription status of CDMA system */

  cm_subscription_status_e_type   gw_status,
    /* Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   wlan_status
    /* Subscription status of WLAN system */

)
{

  cm_cmd_s_type             *cmd_ptr;
  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;
  boolean                    status;

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE,
                   CM_PH_CMD_ERR_NO_BUF_L );
    }

    status = FALSE;
  }  /* if ( cmd_ptr == NULL ) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func        = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr     = data_block_ptr;
    ph_cmd_ptr->client_id          = client_id;
    ph_cmd_ptr->cmd                = CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->cdma_sub_not_avail_status = cdma_status;
    cmd_info_ptr->gw_sub_not_avail_status   = gw_status;
    cmd_info_ptr->wlan_sub_not_avail_status = wlan_status;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    status = TRUE;
  } /* else if ( cmd_ptr == NULL ) */

  return status;

} /* cm_ph_cmd_subscription_not_available */



/*===========================================================================

FUNCTION cm_ph_cmd_subscription_changed

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is changed.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS

  If the subscription is currently not available, the request will be
  ignored and a CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned.

  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_changed(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /* client callback function */

  void                            *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /* Subscription status of CDMA system */

  cm_subscription_status_e_type   gw_status,
    /* Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   wlan_status
    /* Subscription status of WLAN system */
)
{
  cm_cmd_s_type             *cmd_ptr;
  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;
  boolean                    status;

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_PH_CMD_SUBSCRIPTION_CHANGED,
                   CM_PH_CMD_ERR_NO_BUF_L );
    }

    status = FALSE;
  }  /* if ( cmd_ptr == NULL ) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func        = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr     = data_block_ptr;
    ph_cmd_ptr->client_id          = client_id;
    ph_cmd_ptr->cmd                = CM_PH_CMD_SUBSCRIPTION_CHANGED;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->cdma_sub_changed_status = cdma_status;
    cmd_info_ptr->gw_sub_changed_status   = gw_status;
    cmd_info_ptr->wlan_sub_changed_status = wlan_status;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    status = TRUE;
  } /* else if ( cmd_ptr == NULL ) */

  return status;

} /* cm_ph_cmd_subscription_changed */



/*===========================================================================

FUNCTION cm_ph_cmd_get_networks

DESCRIPTION
  Command CM to retrieve either a list of preferred networks or a list of
  all available networks.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
  If command is successful, CM clients will receive the list of networks
  via the CM_PH_EVENT_AVAILABLE_NETWORKS_CONF or CM_PH_EVENT_PREFERRED_-
  NETWORKS_CONF event.
===========================================================================*/
boolean cm_ph_cmd_get_networks(

        cm_ph_cmd_cb_f_type          cmd_cb_func,
            /* client callback function                                 */

        void                        *data_block_ptr,
            /* pointer to client callback data block                    */

        cm_client_id_type            client_id,
            /* Requesting client                                        */

        cm_network_list_type_e_type  list_type,
            /* Indicates whether to retrieve the preferred or available */
            /* networks                                                 */

        cm_mode_pref_e_type          network_type
            /* Indicates the desired network such as WLAN or GW.
            ** Only WLAN or GW is supported, one at a time can be
            ** requested only.
            */
)
/*lint -esym(715,list_type) */
/*lint -esym(715,client_id) */
{

  cm_cmd_s_type             *cmd_ptr;
  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;
  boolean                    status;

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_GET_NETWORKS, CM_PH_CMD_ERR_NO_BUF_L);
    }

    status = FALSE;
  } /* if ( cmd_ptr == NULL ) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func         = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr      = data_block_ptr;
    ph_cmd_ptr->client_id           = client_id;
    ph_cmd_ptr->cmd                 = CM_PH_CMD_GET_NETWORKS;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->network_list_type = list_type;

    cmd_info_ptr->network_type      = network_type;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    status = TRUE;
  } /* else if ( cmd_ptr == NULL ) */

  return status;

} /* cm_ph_cmd_get_networks */
/*lint +esym(715,list_type) */
/*lint +esym(715,client_id) */


/*===========================================================================

FUNCTION cm_ph_cmd_terminate_get_networks

DESCRIPTION
  Command CM to terminate retrieval operation of list of all available
  networks.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
  If command is successful, the previous command to retrieve the list will be
  terminated.

===========================================================================*/
boolean cm_ph_cmd_terminate_get_networks(

  cm_ph_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type            client_id
    /* Requesting client */
)
/*lint -esym(715,client_id) */
{

  cm_cmd_s_type             *cmd_ptr;
  cm_ph_cmd_s_type          *ph_cmd_ptr;
  boolean                    status;


  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_PH_CMD_TERMINATE_GET_NETWORKS,
                   CM_PH_CMD_ERR_NO_BUF_L);
    }

    status = FALSE;
  } /* if ( cmd_ptr == NULL ) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */
  else
  {
    /* Get a reference to command header
    */
    ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func         = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr      = data_block_ptr;
    ph_cmd_ptr->client_id           = client_id;
    ph_cmd_ptr->cmd                 = CM_PH_CMD_TERMINATE_GET_NETWORKS;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    status = TRUE;
  } /* else if ( cmd_ptr == NULL ) */

  return status;

} /* cm_ph_cmd_terminate_get_networks */
/*lint +esym(715,client_id) */

/*=====================================================================================

FUNCTION cm_ph_cmd_wps_start

DESCRIPTION
  Command CM to get WLAN security credentials using WPS.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

  WLAN only.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  - TRUE  - command was successfully sent to CM
  - FALSE - command was not successfully sent to CM

SIDE_EFFECT
  If command is successful, CM clients will receive a list of WLAN
  credentials via the CM_PH_EVENT_WPS_COMPLETED event.
===========================================================================*/

boolean cm_ph_cmd_wps_start(

        cm_ph_cmd_cb_f_type                         cmd_cb_func,
          /**< Client callback function.
          */

        void                                       *data_block_ptr,
          /**< Pointer to client callback data block.
          */

        cm_client_id_type                           client_id,
          /**< Requesting client.
          */

        sys_wlan_wps_start_param_s_type  const     *wps_start_param
          /**< Info required to start WPS procedure.
          */
)
/*lint -esym(715,list_type) */
/*lint -esym(715,client_id) */
{

  #ifdef FEATURE_WLAN_WPS
#error code not present
  #else
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_WPS_START,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client_id );
  SYS_ARG_NOT_USED( wps_start_param );
  return FALSE;
  #endif
} /* cm_ph_cmd_wps_start */

/*===========================================================================

FUNCTION cm_ph_cmd_wps_abort

DESCRIPTION
  Command CM to terminate WPS procedure.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

  WLAN Only.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE_EFFECT
  If command is successful, the previous command to retrieve the WLAN
  credentials using WPS will be aborted.

===========================================================================*/
boolean cm_ph_cmd_wps_abort(

  cm_ph_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type            client_id
    /**< Requesting client */
)
{
  #ifdef FEATURE_WLAN_WPS
#error code not present
  #else
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_WPS_ABORT,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client_id );
  return FALSE;
  #endif
} /* cm_ph_cmd_wps_abort */
/*===========================================================================

FUNCTION cm_ph_cmd_set_preferred_networks

DESCRIPTION
  Command CM to save the preferred networks to the SIM/USIM.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_set_preferred_networks(

        cm_ph_cmd_cb_f_type             cmd_cb_func,
            /* client callback function              */

        void                            *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type               client_id,
            /* Requesting client                     */

        const sys_user_pref_plmn_list_s_type  *preferred_networks_ptr
            /* The preferred networks to be saved    */
)
/*lint -esym(715,client_id) */
/*lint -esym(715,preferred_networks_ptr) */
{
  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_TERMINATE_GET_NETWORKS,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */

} /* cm_ph_cmd_save_preferred_networks */
/*lint +esym(715,client_id) */
/*lint +esym(715,preferred_networks_ptr) */


/*===========================================================================

FUNCTION cm_ph_cmd_wakeup_from_standby

DESCRIPTION
  Command CM to inform lower layers to wake up from deep sleep (standby mode)

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_wakeup_from_standby(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id
    /* Requesting client */
)
{
  return cm_ph_cmd_wakeup_from_standby2(cmd_cb_func,
                                        data_block_ptr,
                                        client_id,
                                        CM_WAKEUP_ALL);
}



/*===========================================================================

FUNCTION cm_ph_cmd_wakeup_from_standby2

DESCRIPTION
  Command CM to inform lower layers to wake up from deep sleep (standby mode)

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_wakeup_from_standby2(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  cm_wakeup_e_type       service
    /* Service to be waken up */
)
{

  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------PH CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_ph_cmd_wakeup_from_standby()",0,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_WAKEUP_FROM_STANDBY,
        CM_PH_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_WAKEUP_FROM_STANDBY;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */

    cmd_info_ptr->wakeup_service = service;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  }
}



/*===========================================================================

FUNCTION cm_ph_cmd_change_rtre_config

DESCRIPTION
  Command CM to change RTRE configuration

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_change_rtre_config(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  cm_rtre_config_e_type  rtre_config

)
/*lint -esym(715, client_id) */
/*lint -esym(715, rtre_config) */
{
  #if ( defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) )
  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------PH CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_ph_cmd_change_rtre_config()",0,0,0 );
  CM_MSG_LOW( "client_id   - %d",client_id,0,0 );
  CM_MSG_LOW( "rtre_config - %d",rtre_config,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_CHANGE_RTRE_CONFIG,
        CM_PH_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr = PH_CMD_PTR( cmd_ptr );
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_CHANGE_RTRE_CONFIG;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->rtre_config   = rtre_config;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  }

  #else /* defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_CHANGE_RTRE_CONFIG,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) */

} /* cm_ph_cmd_change_rtre_config() */
/*lint +esym(715, client_id) */
/*lint +esym(715, rtre_config) */


/*===========================================================================

FUNCTION cm_ph_cmd_client_act

DESCRIPTION
  Inform CM that a client was activated so that CM can inform the client
  when the phone information become available.

  This is mostly important during powerup, where a client needs to get
  the phone information in order to make powerup decisions.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
void cm_ph_cmd_client_act(

        cm_ph_cmd_cb_f_type        cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

  cm_client_id_type           client
            /* Requesting client */

)
{

  cm_cmd_s_type           *cmd_ptr;
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_err_e_type    cmd_err = CM_PH_CMD_ERR_NOERR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command is not rejected, try allocating a CM command buffer.
  */
  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** indicate nature of error.
  */
  if( cmd_ptr == NULL )
  {
    /* Command buffer allocation failed.
    */
    cmd_err = CM_PH_CMD_ERR_NO_BUF_L;
    CM_DBG_ERR_FATAL( "CM_PH_CMD_ERR_NO_BUF_L",0,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr = PH_CMD_PTR( cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_CLIENT_ACT;

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether any errors were found.
  */
  if( cmd_err != CM_PH_CMD_ERR_NOERR )
  {
    CM_ERR( "=CM= CM_PH_CMD_CLIENT_ACT, cm_err=%d", cmd_err, 0, 0);

    /* If client passed us a callback function, call on it now,
    ** informing the client of the command error.
    **
    ** NOTE that in case of NO errors we are not notifying the client
    ** now since errors might still occur once we actually try
    ** to process this command later.
    */
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_CLIENT_ACT, cmd_err);
    }
  }

}



/*===========================================================================

FUNCTION cm_ph_cmd_aoc_reset_acm

DESCRIPTION
  This function sends a command to the CM task to reset the ACM to 0.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

boolean cm_ph_cmd_aoc_reset_acm(
  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id
    /* Requesting client */
)
/*lint -esym(715,client_id) */
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#else
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_RESET_ACM,
        CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
#endif
}
/*lint +esym(715,client_id) */


/*===========================================================================

FUNCTION cm_ph_cmd_aoc_set_acmmax

DESCRIPTION
  This function sends a message to CM to set the value of the maximum
  value the Accumulated Call Meter can achieve before calls are ended.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_ph_cmd_aoc_set_acmmax(
  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  uint32                 new_max
)
/*lint -esym(715,client_id) */
/*lint -esym(715,new_max) */
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#else
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_SET_ACMMAX,
          CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    }
    return FALSE;
#endif
}
/*lint +esym(715,client_id) */
/*lint +esym(715,new_max) */


/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_ccm

DESCRIPTION
  This function returns the current value of the Current Call Meter.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The CCM value from the current or last call.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_ccm(void)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#else
  return 0;
#endif
}



/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_acm

DESCRIPTION
  This function returns the current value of the Accumulated Call Meter.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The ACM value from the SIM.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_acm(void)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#else
  return 0;
#endif
}



/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_acmmax

DESCRIPTION
  This function returns the current value of the Accumulated Call Meter Max.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The ACMMax value from the SIM.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_acmmax(void)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#else
  return 0;
#endif
}


/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_puct

DESCRIPTION
  This function returns the current value of the Price Unit Currency Conversion

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The puct value from the SIM.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_puct(void)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#else
  return 0;
#endif
}



/*===========================================================================

FUNCTION cm_ph_cmd_update_cdma_capability

DESCRIPTION
  Command CM to update the CDMA2000 capability information in form of
  IS2000 Status message.This is used by the UE to advertise the CDMA
  capability to UTRAN using RRC connection setup complete or UE
  Capability Information messages.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_update_cdma_capability(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                              *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  const sys_is2000_msg_s_type             *is2000_msg_ptr
    /* Pointer to the IS2000 message */

)
/*lint -esym(715, is2000_msg_ptr)*/
{

  cm_cmd_s_type             *cmd_ptr;
  cm_ph_cmd_s_type          *ph_cmd_ptr;

  cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_PH_CMD_UPDATE_CDMA_CAPABILITY,
                   CM_PH_CMD_ERR_NO_BUF_L);
    }


    return FALSE;

  } /* if ( cmd_ptr == NULL ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information into command structure.
  */
  ph_cmd_ptr   = PH_CMD_PTR( cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func         = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr      = data_block_ptr;
  ph_cmd_ptr->client_id           = client_id;
  ph_cmd_ptr->cmd                 = CM_PH_CMD_UPDATE_CDMA_CAPABILITY;


  #if defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
      defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM)
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( cmd_ptr );

  return TRUE;

} /* cm_ph_cmd_update_cdma_capability() */
/*lint +esym(715, is2000_msg_ptr)*/



/*===========================================================================

FUNCTION cm_ss_cmd_get_ss_info

DESCRIPTION
  Get a sanpshot of the current serving system information in the CM

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request


SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ss_cmd_get_ss_info(

    cm_ss_cmd_cb_f_type     cmd_cb_func,
        /* client callback function */

    void                    *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type       client
        /* requesting client ID */
)
{

    cm_cmd_s_type             *cmd_ptr;
    cm_ss_cmd_s_type          *ss_cmd_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------SS CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_ss_cmd_get_ss_info",0,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    cmd_ptr = cm_cmd_alloc_ss_init();

    /* If command buffer allocation failed,
    ** call the command callback and return FALSE
    */

    if( cmd_ptr == NULL )
    {
      if( cmd_cb_func != NULL )
      {
        cmd_cb_func( data_block_ptr, CM_SS_CMD_INFO_GET, CM_SS_CMD_ERR_NO_BUF_L);
      }

      return FALSE;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Else (command buffer allocation is successful),
      ** copy command information into command structure.
      */
    else
    {
      /* Get a reference to command header
      */
      ss_cmd_ptr = SS_CMD_PTR( cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      ss_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      ss_cmd_ptr->data_block_ptr  = data_block_ptr;
      ss_cmd_ptr->client_id       = client;
      ss_cmd_ptr->cmd             = CM_SS_CMD_INFO_GET;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
      cm_cmd_queue( cmd_ptr );

      return TRUE;
    }
}


/*===========================================================================

FUNCTION cm_ss_cmd_get_measurement

DESCRIPTION
  Get measurements results for systems specified in the payload of this
  command. The client needs to register for CM_SS_EVENT_MEAS_RESPONSE in 
  order to receive the response to this command. This event is sent to all
  clients that register for this event.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ss_cmd_get_measurement(

    cm_ss_cmd_cb_f_type     cmd_cb_func,
        /* client callback function */

    void                    *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type       client,
        /* requesting client ID */

    cm_ss_meas_req_s_type    meas_req
        /* meas request information */

)
/*lint -esym(715,meas_req)*/
{
  #ifdef FEATURE_CM_SS_MEAS
#error code not present
  #else /* FEATURE_CM_SS_MEAS */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_SS_CMD_GET_MEASUREMENT,
      CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client );

  return FALSE;
  #endif /* FEATURE_CM_SS_MEAS */
}
/*lint +esym(715,meas_req)*/

/*===========================================================================

FUNCTION cm_ss_cmd_orig_thr_update_tbl

DESCRIPTION
  This API queues a command to CM to update the origination throttle table

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  True if a command was queued
  False otherwise

SIDE EFFECTS
  None.
===========================================================================*/

boolean cm_ss_cmd_orig_thr_update_tbl
(

  cm_ss_cmd_cb_f_type               *ss_cmd_cb_func,
    /* Pointer to the call back function */

  void                              *data_block_ptr,
    /* Pointer to user data */

  cm_client_id_type                  client,
    /* requesting client ID */

  cm_ss_orig_thr_act_e_type          orig_thr_act,
    /* The action to be taken */

  cm_ss_orig_thr_reasons_e_type      call_status,
    /* The call end reason */

  cm_call_type_e_type                call_type,
    /* Type of the call, e.g PS_DATA */

  sys_sys_mode_e_type                mode,
    /* The mode on which the call was connected
    ** This field is only used if
    ** orig_thr_act = CM_SS_ORIG_THR_UPDATE_CURR_SS
    */

  const sys_sys_id_s_type                 *sys_id,
    /* The sys id of the system, SID,NID
    ** This field is only used if
    ** orig_thr_act = CM_SS_ORIG_THR_UPDATE_SS
    */

  sys_band_class_e_type              band_class,
    /* cellular or PCS
    ** This field is only used if
    ** orig_thr_act = CM_SS_ORIG_THR_UPDATE_SS
    */

  uint16                             packet_zone
    /* The packet zone ID
    ** This field is only used if
    ** orig_thr_act = CM_SS_ORIG_THR_UPDATE_SS
    */

)
{
  SYS_ARG_NOT_USED( orig_thr_act );
  SYS_ARG_NOT_USED( call_status );
  SYS_ARG_NOT_USED( call_type );
  SYS_ARG_NOT_USED( mode );
  SYS_ARG_NOT_USED( band_class );
  SYS_ARG_NOT_USED( client );
  SYS_ARG_NOT_USED( sys_id );
  SYS_ARG_NOT_USED( packet_zone );

  #ifdef FEATURE_CALL_THROTTLE
  {

    cm_cmd_s_type             *cmd_ptr;
    cm_ss_cmd_s_type          *ss_cmd_ptr;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    if( orig_thr_act == CM_SS_ORIG_THR_UPDATE_SS )
    {
      CM_ASSERT( sys_id != NULL );
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_MSG_LOW( "--------------------SS CMD--------------------",0,0,0 );

    CM_MSG_HIGH( "cm_ss_cmd_orig_thr_update_tbl \
                  Thr. act %d call status %d mode %d", orig_thr_act,
                                                       call_status,
                                                       mode );

    CM_MSG_LOW( "----------------------------------------------",0,0,0 );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    cmd_ptr = cm_cmd_alloc_ss_init();

    /* If command buffer allocation failed,
    ** call the command callback and return FALSE
    */

    if( cmd_ptr == NULL )
    {
      if( ss_cmd_cb_func != NULL )
      {
        ss_cmd_cb_func( data_block_ptr, CM_SS_CMD_ORIG_THR_UPDATE_TBL, \
                     CM_SS_CMD_ERR_NO_BUF_L);
      }

      return FALSE;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */

    /* Get a reference to command header
    */
    ss_cmd_ptr = SS_CMD_PTR( cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Initialize the command header fields
    */

    ss_cmd_ptr->info.call_info.sys_id.id_type = SYS_SYS_ID_TYPE_UNDEFINED;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */

    ss_cmd_ptr->cmd_cb_func                = ss_cmd_cb_func;
    ss_cmd_ptr->data_block_ptr             = data_block_ptr;
    ss_cmd_ptr->client_id                  = client;
    ss_cmd_ptr->info.orig_thr_act          = orig_thr_act;
    ss_cmd_ptr->cmd                        = CM_SS_CMD_ORIG_THR_UPDATE_TBL;
    ss_cmd_ptr->info.call_info.call_status = call_status;
    ss_cmd_ptr->info.call_info.call_type   = call_type;
    ss_cmd_ptr->info.mode                  = mode;
    ss_cmd_ptr->info.call_info.mode_info.cdma_info.band_class  = band_class;
    ss_cmd_ptr->info.call_info.mode_info.cdma_info.packet_zone = packet_zone;

    if( sys_id != NULL )
    {
      ss_cmd_ptr->info.call_info.sys_id    = *sys_id;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */

    cm_cmd_queue( cmd_ptr );
    return TRUE;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #else
  if( ss_cmd_cb_func != NULL )
  {
    ss_cmd_cb_func( data_block_ptr, CM_SS_CMD_ORIG_THR_UPDATE_TBL,
                CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif

}/* cm_ss_cmd_orig_thr_update_tbl */


/*===========================================================================

FUNCTION cm_ss_cmd_country_selected

DESCRIPTION
  Carries the country code selected by user in response to
  CM_SS_EVENT_SELECT_COUNTRY.

  country_code is either from the list presented to the user or
  CM_INVALID_MOBILE_COUNTRY_CODE if user does not find his/her
  country listed.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ss_cmd_country_selected (

  cm_ss_cmd_cb_f_type          ss_cmd_cb_func,
    /* client callback function */

  cm_client_id_type            client_id,
    /* requesting client ID */

  cm_sid_type                  sid,
    /* sid for which country is being resolved */

  cm_country_code_type         country_code
    /* code of country that has been selected */
)
{


  cm_cmd_s_type             *cmd_ptr;
  cm_ss_cmd_s_type          *ss_cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH( "--------------------SS CMD--------------------",0,0,0 );

  CM_MSG_HIGH( "cm_ss_cmd_country_selected ", 0, 0, 0);

  CM_MSG_HIGH( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate input */
  if ( (country_code > CM_MAX_MOBILE_COUNTRY_CODE_VAL)
       &&
     (country_code != CM_INVALID_MOBILE_COUNTRY_CODE)
     )
  {
    CM_ERR ("Country code selected in error %d", country_code, 0, 0);

    if( ss_cmd_cb_func != NULL )
    {
      ss_cmd_cb_func( NULL, CM_SS_CMD_COUNTRY_SELECTED, \
                      CM_SS_CMD_ERR_OTHER );
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_ss_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( ss_cmd_cb_func != NULL )
    {
      ss_cmd_cb_func( NULL, CM_SS_CMD_COUNTRY_SELECTED, \
                   CM_SS_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */

  /* Get a reference to command header
  */
  ss_cmd_ptr = SS_CMD_PTR( cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */

  ss_cmd_ptr->cmd_cb_func                = ss_cmd_cb_func;
  ss_cmd_ptr->client_id                  = client_id;
  ss_cmd_ptr->cmd                        = CM_SS_CMD_COUNTRY_SELECTED;

  ss_cmd_ptr->info.selected_country      = country_code;
  ss_cmd_ptr->info.sid                   = sid;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */

  cm_cmd_queue( cmd_ptr );
  return TRUE;

}/* cm_ss_cmd_country_selected */



/*===========================================================================

FUNCTION cm_mm_inband_cmd_burst_dtmf

DESCRIPTION
  Command CM to send a burst DTMF

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_inband_cmd_burst_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client_id,
    /* Requesting client */

  cm_call_id_type             call_id,
    /* call ID of the call */

  uint8                       on_length,
    /* DTMF pulse width */

  uint8                       off_length,
    /* DTMF inter-digit interval */

  uint8                       cnt,
    /* DTMF digit count */

  const uint8                 *dtmf_digit_ptr
    /* Pointer to the DTMF digit buffer */

)
/*lint -esym(715, cnt) */
/*lint -esym(715, on_length) */
/*lint -esym(715, off_length) */
/*lint -esym(715, call_id) */
/*lint -esym(715, dtmf_digit_ptr) */
/*lint -esym(715, client_id) */
{
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cm_cmd_s_type                 *cmd_ptr;
  cm_inband_cmd_s_type          *inband_cmd_ptr;
  cm_inband_cmd_info_s_type     *cmd_info_ptr;
  cm_inband_cmd_err_e_type      cmd_err=CM_INBAND_CMD_ERR_NOERR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------Inband CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_inband_cmd_burst_dtmf()",0,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_ptr = cm_cmd_alloc_inband_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_INBAND_CMD_BURST_DTMF,
          CM_INBAND_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {

      /* Get a reference to command header
      ** and a reference to command information.
      */
      inband_cmd_ptr  = INBAND_CMD_PTR( cmd_ptr );
      cmd_info_ptr    = CMD_INFO_PTR( inband_cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      inband_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      inband_cmd_ptr->data_block_ptr  = data_block_ptr;
      inband_cmd_ptr->client_id       = client_id;
      inband_cmd_ptr->cmd             = CM_INBAND_CMD_BURST_DTMF;
      cmd_info_ptr->call_id           = call_id;

      /* check null buffer pointer and maximum length related errors */

      /*lint -save -e685 Relational operator '>' always evaluates to 'false'
      ** because cnt is uint8 and CM_INBAND_MAX_REV_DIALED_DIGITS is 255 for JCDMA
      ** so cnt can't be bigger than that on JCDMA builds but it can on others
      */
      if( cnt > CM_INBAND_MAX_REV_DIALED_DIGITS )
      /*lint -restore */
      {
        cmd_err = CM_INBAND_CMD_ERR_CNT_P;
      }
      else if( dtmf_digit_ptr == NULL )
      {
        cmd_err = CM_INBAND_CMD_ERR_NULL_BUF_P;
      }


      if (cmd_err == CM_INBAND_CMD_ERR_NOERR)
      {

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /*
        ** 668: possibly passing null ptr to function
        ** 669: possible data overrun
        ** Here we know dtmf_digit_ptr is not NULL (see check for
        ** CM_INBAND_CMD_ERR_NULL_BUF_P above) so turn off Lint warning 668
        ** Also we know here that cnt <= CM_INBAND_MAX_REV_DIALED_DIGITS
        ** so turn off Lint warning 669
        */
        /*lint -save -e668 -e669 )*/

        /* Copy command information fields.
        */
        cmd_info_ptr->on_length     = on_length;
        cmd_info_ptr->off_length    = off_length;
        cmd_info_ptr->cnt           = cnt;
        memcpy(cmd_info_ptr->digits, dtmf_digit_ptr, cnt);
        /*lint -restore */
      }
      else
      {
        cmd_info_ptr->cmd_err       = cmd_err;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
      cm_cmd_queue( cmd_ptr );

      return TRUE;
  }
  #else /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_INBAND_CMD_BURST_DTMF,
      CM_INBAND_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
}
/*lint +esym(715, cnt) */
/*lint +esym(715, on_length) */
/*lint +esym(715, off_length) */
/*lint +esym(715, call_id) */
/*lint +esym(715, dtmf_digit_ptr) */
/*lint +esym(715, client_id) */


/*===========================================================================

FUNCTION cm_mm_inband_cmd_start_cont_dtmf

DESCRIPTION
  Command CM to start a contonuous DTMF

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_inband_cmd_start_cont_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client_id,
    /* Requesting client */

  cm_call_id_type        call_id,
    /* call ID of the call */

  uint8                       digit
    /* DTMF digit */

)
{

  cm_cmd_s_type                 *cmd_ptr;
  cm_inband_cmd_s_type          *inband_cmd_ptr;
  cm_inband_cmd_info_s_type     *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------Inband CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_inband_cmd_start_cont_dtmf()",0,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_inband_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_INBAND_CMD_START_CONT_DTMF,
          CM_INBAND_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    inband_cmd_ptr  = INBAND_CMD_PTR( cmd_ptr );
    cmd_info_ptr    = CMD_INFO_PTR( inband_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    inband_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    inband_cmd_ptr->data_block_ptr  = data_block_ptr;
    inband_cmd_ptr->client_id       = client_id;
    inband_cmd_ptr->cmd             = CM_INBAND_CMD_START_CONT_DTMF;
    cmd_info_ptr->call_id           = call_id;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->cnt           = 1;
    cmd_info_ptr->digits[0]     = digit;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  }
}


/*===========================================================================

FUNCTION cm_mm_inband_cmd_stop_cont_dtmf

DESCRIPTION
  Command CM to stop a continuous DTMF

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_inband_cmd_stop_cont_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client_id,
    /* Requesting client */

  cm_call_id_type        call_id,
    /* call ID of the call */

  uint16                      duration
    /* DTMF duration */

)
{

  cm_cmd_s_type                 *cmd_ptr;
  cm_inband_cmd_s_type          *inband_cmd_ptr;
  cm_inband_cmd_info_s_type     *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------Inband CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_mm_inbnad_cmd_stop_cont_dtmf()",0,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_inband_init();

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_INBAND_CMD_STOP_CONT_DTMF,
          CM_INBAND_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    inband_cmd_ptr  = INBAND_CMD_PTR( cmd_ptr );
    cmd_info_ptr    = CMD_INFO_PTR( inband_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    inband_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    inband_cmd_ptr->data_block_ptr  = data_block_ptr;
    inband_cmd_ptr->client_id       = client_id;
    inband_cmd_ptr->cmd             = CM_INBAND_CMD_STOP_CONT_DTMF;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->call_id         = call_id;
    cmd_info_ptr->duration        = duration;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  }
}



/*===========================================================================

FUNCTION cm_sms_cmd_mo_msg2

DESCRIPTION

     Send a mobile originated SMS message

DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE

  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS

  none

===========================================================================*/
boolean cm_sms_cmd_mo_msg2(

    cm_sms_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                    *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type       client,
        /* requesting client ID */

    boolean                 l2_ack_requested,
        /* l2 ack is required or not */

    const cm_sms_msg_type   *data,
        /* pointer to SMS buffer */

    cm_mode_pref_e_type     mode_pref
        /* The mode over which the SMS is sent */

)
{

  cm_cmd_s_type              *cmd_ptr;
  cm_sms_cmd_s_type          *sms_cmd_ptr;
  cm_sms_cmd_info_s_type     *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW( "--------------------SMS CMD--------------------",0,0,0 );
  CM_MSG_LOW( "cm_sms_cmd_mo_msg()",0,0,0 );
  CM_MSG_LOW( "----------------------------------------------",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = cm_cmd_alloc_sms_init();

  if( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_SMS_CMD_MO_MSG, CM_SMS_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */
  else
  {
    /* Get a reference to command header
    ** and a reference to command information.
    */
    sms_cmd_ptr     = SMS_CMD_PTR( cmd_ptr );
    cmd_info_ptr    = CMD_INFO_PTR( sms_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    sms_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    sms_cmd_ptr->data_block_ptr  = data_block_ptr;
    sms_cmd_ptr->client_id       = client;
    sms_cmd_ptr->cmd             = CM_SMS_CMD_MO_MSG;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */

    cmd_info_ptr->l2_ack_requested  = l2_ack_requested;

    /* Set the pointer to a static buffer to holding the MO SMS.
    */
    cmd_info_ptr->data = cmsms_get_sms_data_buffer();

    /* Copy the SMS data buffer to the field.
    */
    cmsms_sms_data_buffer_fill( data );

    /* The mode pref over which the SMS is sent.
    */
    cmd_info_ptr->mode_pref = mode_pref;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Queue the command to CM.
    */
    cm_cmd_queue( cmd_ptr );

    return TRUE;
  }
}

/*===========================================================================

FUNCTION cm_sms_cmd_mo_msg

DESCRIPTION

     Send a mobile originated SMS message

DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE

  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS

  none

===========================================================================*/
boolean cm_sms_cmd_mo_msg(

    cm_sms_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                    *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type       client,
        /* requesting client ID */

    boolean                 l2_ack_requested,
        /* l2 ack is required or not */

    const uint8             *data
        /* pointer to SMS buffer */


)
{
  /*lint -save -e826 */
  return( cm_sms_cmd_mo_msg2( cmd_cb_func,
                              data_block_ptr,
                              client,
                              l2_ack_requested,
                              (cm_sms_msg_type *)data,
                              CM_MODE_PREF_CDMA_ONLY) );
  /*lint -restore */
} /*cm_sms_cmd_mo_msg*/




/*===========================================================================

FUNCTION cm_sms_cmd_awi_ack

DESCRIPTION

  Send analog alert with info ack message to base station

DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request


SIDE EFFECTS
  none

===========================================================================*/
boolean cm_sms_cmd_awi_ack(

    cm_sms_cmd_cb_f_type  cmd_cb_func,
        /* client callback function */

    void                 *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type       client,
        /* requesting client ID */

    byte                   seq_no,
        /* sequence number */

    boolean                bf,
        /* begin/final flag */

    byte                   error_class,
        /* error class */

    byte                   status
        /* ack status */

)
/*lint -esym(715, client) */
/*lint -esym(715, seq_no) */
/*lint -esym(715, bf) */
/*lint -esym(715, error_class) */
/*lint -esym(715, status) */
{
    #ifdef FEATURE_CDSMS
    cm_cmd_s_type              *cmd_ptr;
    cm_sms_cmd_s_type          *sms_cmd_ptr;
    cm_sms_cmd_info_s_type     *cmd_info_ptr;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_MSG_LOW( "--------------------SMS CMD--------------------",0,0,0 );
    CM_MSG_LOW( "cm_sms_cmd_awi_ack()",0,0,0 );
    CM_MSG_LOW( "----------------------------------------------",0,0,0 );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    cmd_ptr = cm_cmd_alloc_sms_init();

    if( cmd_ptr == NULL )
    {
      if( cmd_cb_func != NULL )
      {
        cmd_cb_func( data_block_ptr, CM_SMS_CMD_AWI_ACK, CM_SMS_CMD_ERR_NO_BUF_L);
      }

      return FALSE;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */
    else
    {
      /* Get a reference to command header
      ** and a reference to command information.
      */
      sms_cmd_ptr     = SMS_CMD_PTR( cmd_ptr );
      cmd_info_ptr    = CMD_INFO_PTR( sms_cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      sms_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      sms_cmd_ptr->data_block_ptr  = data_block_ptr;
      sms_cmd_ptr->client_id       = client;
      sms_cmd_ptr->cmd             = CM_SMS_CMD_AWI_ACK;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command information fields.
      */

      cmd_info_ptr->awi_ack.seq_no      = seq_no;
      cmd_info_ptr->awi_ack.bf          = bf;
      cmd_info_ptr->awi_ack.status      = status;
      cmd_info_ptr->awi_ack.error_class = error_class;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
      cm_cmd_queue( cmd_ptr );

      return TRUE;
    }

    #else /* FEATURE_CDSMS */
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_SMS_CMD_MO_MSG,
        CM_SMS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    }
    return FALSE;
    #endif /* FEATURE_CDSMS */
}
/*lint +esym(715, client) */
/*lint +esym(715, seq_no) */
/*lint +esym(715, bf) */
/*lint +esym(715, error_class) */
/*lint +esym(715, status) */


/*===========================================================================

FUNCTION cm_sms_cmd_bc_enable

DESCRIPTION

  Enable/disbale broadcast SMS

DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request


SIDE EFFECTS
  none

===========================================================================*/
boolean cm_sms_cmd_bc_enable(

    cm_sms_cmd_cb_f_type  cmd_cb_func,
        /* client callback function */

    void                  *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type     client,
        /* requesting client ID */

    boolean               bc_enable
        /* enable broadcast message */

)
/*lint -esym(715, client) */
/*lint -esym(715, bc_enable) */
{
    #if ( defined(FEATURE_BROADCAST_SMS) && defined(FEATURE_CDSMS) )
    cm_cmd_s_type              *cmd_ptr;
    cm_sms_cmd_s_type          *sms_cmd_ptr;
    cm_sms_cmd_info_s_type     *cmd_info_ptr;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_MSG_LOW( "--------------------SMS CMD--------------------",0,0,0 );
    CM_MSG_LOW( "cm_sms_cmd_bc_enable()",0,0,0 );
    CM_MSG_LOW( "----------------------------------------------",0,0,0 );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    cmd_ptr = cm_cmd_alloc_sms_init();

    if( cmd_ptr == NULL )
    {
      if( cmd_cb_func != NULL )
      {
        cmd_cb_func( data_block_ptr, CM_SMS_CMD_BC_ENABLE, CM_SMS_CMD_ERR_NO_BUF_L);
      }

      return FALSE;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else (command buffer allocation is successful),
    ** copy command information into command structure.
    */
    else
    {
      /* Get a reference to command header
      ** and a reference to command information.
      */
      sms_cmd_ptr     = SMS_CMD_PTR( cmd_ptr );
      cmd_info_ptr    = CMD_INFO_PTR( sms_cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      sms_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      sms_cmd_ptr->data_block_ptr  = data_block_ptr;
      sms_cmd_ptr->client_id       = client;
      sms_cmd_ptr->cmd             = CM_SMS_CMD_BC_ENABLE;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command information fields.
      */

      cmd_info_ptr->bc_enable = bc_enable;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
      cm_cmd_queue( cmd_ptr );

      return TRUE;
    }

    #else /* ( defined(FEATURE_BROADCAST_SMS) && defined(FEATURE_CDSMS) ) */
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_SMS_CMD_MO_MSG,
        CM_SMS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    }
    return FALSE;
    #endif /* else of ( defined(FEATURE_BROADCAST_SMS) && defined(FEATURE_CDSMS) ) */
}
/*lint +esym(715, client) */
/*lint +esym(715, bc_enable) */

#if (defined FEATURE_GSM) || (defined FEATURE_WCDMA)
#error code not present
#endif   /* FEATURE_GSM || FEATURE_WCDMA */

/*===========================================================================

FUNCTION cm_nas_cmd_req

DESCRIPTION
  Send a command to NAS.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_nas_cmd_req(

  cm_nas_cmd_cb_f_type                      cmd_cb_func,
    /* client callback function */

  void                                     *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                         client_id,
    /* requesting client ID */

  cm_nas_req_type                           nas_req

)
/*lint -esym(715,nas_req) */
/*lint -esym(715,client_id) */
{
  #if (defined (FEATURE_WMS_CM_NAS_INTERFACES)) && (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #else /* (FEATURE_WCDMA || FEATURE_GSM) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_NAS_CMD_REQ,
      CM_NAS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /*(FEATURE_WCDMA || FEATURE_GSM) */
}
/*lint +esym(715,nas_req) */
/*lint +esym(715,client_id) */


/*===========================================================================

FUNCTION cm_is_valid_mode_band_pref

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
boolean cm_is_valid_mode_band_pref(

    cm_mode_pref_e_type    mode_pref,
        /* Mode preference to check */

    cm_band_pref_e_type    band_pref
        /* Band preference to check */
)
{
  return cmph_is_valid_mode_band_pref( mode_pref, band_pref );
}



/*===========================================================================
FUNCTION  cm_is_valid_mode_pref

DESCRIPTION
  Check whether a given mode preference is valid.

DEPENDENCIES
  None

RETURNS
  boolean: TRUE if mode is valid, FALSE if not.

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_is_valid_mode_pref(
  cm_mode_pref_e_type mode
)
/*lint -esym(715,mode) */
{
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  return cmph_is_valid_mode_pref( mode );
  #else
  return FALSE;
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
}
/*lint +esym(715,mode) */


/*===========================================================================

FUNCTION CM_GET_CUR_HYBR_STATUS

DESCRIPTION
  This function gets the current hybrid status

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE - hybrid operation is enabled, FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_get_cur_hybr_status(void)
{
  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #else
  return FALSE;
  #endif
}


/*===========================================================================

FUNCTION cm_is_sys_mode_digital

DESCRIPTION
  This function determines whether the given system mode is a digital\
  mode.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if the system mode is digital, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_is_sys_mode_digital(

  sys_sys_mode_e_type    sys_mode
    /* system mode to check */

)
{
  return (( sys_mode == SYS_SYS_MODE_CDMA  ) ||
          ( sys_mode == SYS_SYS_MODE_GSM   ) ||
          ( sys_mode == SYS_SYS_MODE_HDR   ) ||
          ( sys_mode == SYS_SYS_MODE_WCDMA ) ||
          ( sys_mode == SYS_SYS_MODE_GPS   )    );

} /* cm_is_sys_mode_digital */



/*===========================================================================

FUNCTION cm_sups_cmd_register

DESCRIPTION
  RegisterSS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_register(

        cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type          client,
            /* originating client */

        const cm_reg_ss_params_s_type    *reg_cmd_params
            /* pointer to register SS command data */

)
{
  return cm_sups_cmd_register_cc( sups_cmd_cb_func,
                                  data_block_ptr,
                                  client,
                                  reg_cmd_params,
                                  NULL,
                                  TRUE );
}

/*===========================================================================

FUNCTION cm_sups_cmd_register_cc

DESCRIPTION
  RegisterSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_register_cc(

  cm_sups_cmd_cb_f_type     sups_cmd_cb_func,
    /* client callback function */

  void                      *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type         client,
    /* originating client */

  const cm_reg_ss_params_s_type   *reg_cmd_params,
    /* pointer to register SS command data */

  const cm_num_s_type             *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                   is_call_control_processing_complete
    /* Indicates whether call control has already been coompleted */

)
/*lint -esym(715,client)*/
/*lint -esym(715,reg_cmd_params)*/
/*lint -esym(715,dialed_digits)*/
/*lint -esym(715,is_call_control_processing_complete)*/
{

  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_REGISTER,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */

}  /* cm_sups_cmd_register_cc */
/*lint +esym(715,dialed_digits)*/
/*lint +esym(715,is_call_control_processing_complete)*/
/*lint +esym(715,client)*/
/*lint +esym(715,reg_cmd_params)*/



/*===========================================================================

FUNCTION cm_sups_cmd_erase

DESCRIPTION
  EraseSS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_erase(

        cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
            /* client callback function */

        void                        *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client,
            /* originating client */

        const cm_erase_ss_params_s_type   *erase_cmd_params
            /* pointer to erase SS command data */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( erase_cmd_params != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -save -e668 */
  return cm_sups_cmd_erase_cc( sups_cmd_cb_func,
                               data_block_ptr,
                               client,
                               erase_cmd_params,
                               NULL,
                               TRUE );
  /*lint -restore */

}

/*===========================================================================

FUNCTION cm_sups_cmd_erase_cc

DESCRIPTION
  EraseSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_erase_cc(

  cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client,
    /* originating client */

  const cm_erase_ss_params_s_type   *erase_cmd_params,
    /* pointer to erase SS command data */

  const cm_num_s_type               *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                     is_call_control_processing_complete
   /* Indicates whether call control has already been completed */

)
/*lint -esym(715,client)*/
/*lint -esym(715,erase_cmd_params)*/
/*lint -esym(715,dialed_digits)*/
/*lint -esym(715,is_call_control_processing_complete)*/
{

  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_ERASE,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */

}  /* cm_sups_cmd_erase_cc */
/*lint +esym(715,client)*/
/*lint +esym(715,dialed_digits)*/
/*lint +esym(715,is_call_control_processing_complete)*/
/*lint +esym(715,erase_cmd_params)*/



/*===========================================================================

FUNCTION cm_sups_cmd_activate

DESCRIPTION
  EraseSS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_activate(

        cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
            /* client callback function */

        void                        *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client,
            /* originating client */

        const cm_act_ss_params_s_type     *act_cmd_params
            /* pointer to activate SS command data */

)
{
  return cm_sups_cmd_activate_cc( sups_cmd_cb_func,
                                  data_block_ptr,
                                  client,
                                  act_cmd_params,
                                  NULL,
                                  TRUE );

}

/*===========================================================================

FUNCTION cm_sups_cmd_activate_cc

DESCRIPTION
  EraseSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_activate_cc(

  cm_sups_cmd_cb_f_type    sups_cmd_cb_func,
    /* client callback function */

  void                     *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type        client,
    /* originating client */

  const cm_act_ss_params_s_type  *act_cmd_params,
    /* pointer to activate SS command data */

  const cm_num_s_type            *dialed_digits,
    /* pointer to a cm_num_s_type of the digitis dialed */

  boolean                  is_call_control_processing_complete
    /* Indicates whether call control has already been completed */

)
/*lint -esym(715,client)*/
/*lint -esym(715,act_cmd_params)*/
/*lint -esym(715,dialed_digits)*/
/*lint -esym(715,is_call_control_processing_complete)*/
{
  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_ACTIVATE,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */

} /* cm_sups_cmd_activate_cc */
/*lint +esym(715,dialed_digits)*/
/*lint +esym(715,is_call_control_processing_complete)*/
/*lint +esym(715,act_cmd_params)*/
/*lint +esym(715,client)*/


/*===========================================================================

FUNCTION cm_sups_cmd_deactivate

DESCRIPTION
  DeactivateSS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_deactivate(

        cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client,
            /* originating client */

        const cm_deact_ss_params_s_type   *deact_cmd_params
            /* pointer to erase SS command data */

)
{
  return cm_sups_cmd_deactivate_cc( sups_cmd_cb_func,
                                    data_block_ptr,
                                    client,
                                    deact_cmd_params,
                                    NULL,
                                    TRUE );
}

/*===========================================================================

FUNCTION cm_sups_cmd_deactivate_cc

DESCRIPTION
  DeactivateSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_deactivate_cc(

  cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client,
    /* originating client */

  const cm_deact_ss_params_s_type   *deact_cmd_params,
    /* pointer to erase SS command data */

  const cm_num_s_type               *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                     is_call_control_processing_complete
    /* Indicates whether call control has already been completed */

)
/*lint -esym(715,deact_cmd_params)*/
/*lint -esym(715,client)*/
/*lint -esym(715,dialed_digits)*/
/*lint -esym(715,is_call_control_processing_complete)*/
{
  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_DEACTIVATE,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */

}  /* cm_sups_cmd_deactivate */
/*lint +esym(715,dialed_digits)*/
/*lint +esym(715,is_call_control_processing_complete)*/
/*lint +esym(715,client)*/
/*lint +esym(715,deact_cmd_params)*/



/*===========================================================================

FUNCTION cm_sups_cmd_interrogate

DESCRIPTION
  InterrogateSS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_interrogate(

        cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
            /* client callback function */

        void                        *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type            client,
            /* originating client */

        const cm_interr_ss_params_s_type   *interr_cmd_params
            /* pointer to interrogate SS command data */

)
{
  return cm_sups_cmd_interrogate_cc( sups_cmd_cb_func,
                                     data_block_ptr,
                                     client,
                                     interr_cmd_params,
                                     NULL,
                                     TRUE );
}

/*===========================================================================

FUNCTION cm_sups_cmd_interrogate_cc

DESCRIPTION
  InterrogateSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_interrogate_cc(

  cm_sups_cmd_cb_f_type        sups_cmd_cb_func,
    /* client callback function */

  void                         *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type            client,
    /* originating client */

  const cm_interr_ss_params_s_type   *interr_cmd_params,
    /* pointer to interrogate SS command data */

  const cm_num_s_type                *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                      is_call_control_processing_complete
    /* Indicates whether call control has already been completed */

)
/*lint -esym(715,client) */
/*lint -esym(715,interr_cmd_params) */
/*lint -esym(715,dialed_digits) */
/*lint -esym(715,is_call_control_processing_complete) */
{

  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_INTERROGATE,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */

}  /* cm_sups_cmd_interrogate_cc */
/*lint +esym(715,interr_cmd_params) */
/*lint +esym(715,dialed_digits) */
/*lint +esym(715,client) */
/*lint +esym(715,is_call_control_processing_complete) */



/*===========================================================================

FUNCTION cm_sups_cmd_reg_password

DESCRIPTION
  Register Password SS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_reg_password(

        cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client,
            /* originating client */

        const cm_reg_pwd_params_s_type    *reg_pwd_cmd_params
            /* pointer to register password SS command data */

)
{
  return cm_sups_cmd_reg_password_cc( sups_cmd_cb_func,
                                      data_block_ptr,
                                      client,
                                      reg_pwd_cmd_params,
                                      NULL,
                                      TRUE );
}

/*===========================================================================

FUNCTION cm_sups_cmd_reg_password_cc

DESCRIPTION
  Register Password SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_reg_password_cc(

  cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
    /* client callback function */

  void                       *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type          client,
    /* originating client */

  const cm_reg_pwd_params_s_type   *reg_pwd_cmd_params,
    /* pointer to register password SS command data */

  const cm_num_s_type              *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                    is_call_control_processing_complete
    /* Indicates whether call control has already been completed */

)
/*lint -esym(715,dialed_digits)*/
/*lint -esym(715,is_call_control_processing_complete)*/
/*lint -esym(715,client)*/
/*lint -esym(715,reg_pwd_cmd_params)*/
{
  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_REG_PASSWORD,

      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */

}  /* cm_sups_cmd_reg_password_cc */
/*lint +esym(715,dialed_digits)*/
/*lint +esym(715,is_call_control_processing_complete)*/
/*lint +esym(715,client)*/
/*lint +esym(715,reg_pwd_cmd_params)*/



/*===========================================================================

FUNCTION cm_sups_cmd_process_uss

DESCRIPTION
  Process Unstructured SS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_process_uss(

        cm_sups_cmd_cb_f_type    sups_cmd_cb_func,
            /* client callback function */

        void                     *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type        client,
            /* originating client */

        const cm_uss_params_s_type    *process_uss_cmd_params
            /* pointer to Process Unstructured SS command data */

)
{
  return cm_sups_cmd_process_uss_cc( sups_cmd_cb_func,
                                     data_block_ptr,
                                     client,
                                     process_uss_cmd_params,
                                     NULL,
                                     TRUE );
}

/*===========================================================================

FUNCTION cm_sups_cmd_process_uss_cc

DESCRIPTION
  Process Unstructured SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_process_uss_cc(

  cm_sups_cmd_cb_f_type    sups_cmd_cb_func,
    /* client callback function */

  void                     *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type        client,
    /* originating client */

  const cm_uss_params_s_type     *process_uss_cmd_params,
    /* pointer to Process Unstructured SS command data */

  const cm_num_s_type            *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                  is_call_control_processing_complete
    /* Indicates whether call control has already been completed */

)
/*lint -esym(715,dialed_digits)*/
/*lint -esym(715,process_uss_cmd_params)*/
/*lint -esym(715,is_call_control_processing_complete)*/
/*lint -esym(715,client)*/
{
  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_PROCESS_USS,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */

}  /* cm_sups_cmd_process_uss_cc */
/*lint +esym(715,dialed_digits)*/
/*lint +esym(715,is_call_control_processing_complete)*/
/*lint +esym(715,process_uss_cmd_params)*/
/*lint +esym(715,client)*/


/*===========================================================================

FUNCTION cm_sups_cmd_release

DESCRIPTION
  Release SS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_release(

        cm_sups_cmd_cb_f_type         sups_cmd_cb_func,
            /* client callback function */

        void                          *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type             client,
            /* originating client */

        const cm_release_ss_params_s_type    *release_cmd_params
            /* pointer to release SS command data */

)
{
  return cm_sups_cmd_release_cc( sups_cmd_cb_func,
                                 data_block_ptr,
                                 client,
                                 release_cmd_params,
                                 NULL,
                                 TRUE );
}

/*===========================================================================

FUNCTION cm_sups_cmd_release_cc

DESCRIPTION
  Release SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_release_cc(

  cm_sups_cmd_cb_f_type         sups_cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client,
    /* originating client */

  const cm_release_ss_params_s_type    *release_cmd_params,
    /* pointer to release SS command data */

  const cm_num_s_type              *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                    is_call_control_processing_complete
    /* Indicates whether call control has already been completed */


)
/*lint -esym(715,dialed_digits)*/
/*lint -esym(715,is_call_control_processing_complete)*/
/*lint -esym(715,release_cmd_params)*/
/*lint -esym(715,client)*/
{
  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_RELEASE,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */

}  /* cm_sups_cmd_release_cc */
/*lint +esym(715,dialed_digits)*/
/*lint +esym(715,is_call_control_processing_complete)*/
/*lint +esym(715,release_cmd_params)*/
/*lint +esym(715,client)*/


/*===========================================================================

FUNCTION cm_sups_cmd_uss_res

DESCRIPTION
  Unstructured SS response command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_uss_res(

        cm_sups_cmd_cb_f_type          sups_cmd_cb_func,
            /* client callback function */

        void                           *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type              client,
            /* originating client */

        const cm_uss_res_params_s_type       *uss_res_params
            /* pointer to Unstructured SS response data */

)
/*lint -esym(715,uss_res_params)*/
/*lint -esym(715,client)*/
{
  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_USS_RES,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
}
/*lint +esym(715,uss_res_params)*/
/*lint +esym(715,client)*/



/*===========================================================================

FUNCTION cm_sups_cmd_uss_notify_res

DESCRIPTION
  Unstructured SS Notify response command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_uss_notify_res(

        cm_sups_cmd_cb_f_type                  sups_cmd_cb_func,
            /* client callback function */

        void                                   *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                      client,
            /* originating client */

        const cm_uss_notify_res_params_s_type        *uss_notify_res_params
            /* pointer to Unstructured SS Notify response data */

)
/*lint -esym(715,uss_notify_res_params)*/
/*lint -esym(715,client)*/
{
  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_USS_NOTIFY_RES,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
}
/*lint +esym(715,uss_notify_res_params)*/
/*lint +esym(715,client)*/


/*===========================================================================

FUNCTION cm_sups_cmd_get_password_res

DESCRIPTION
  Get Password response command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_get_password_res(

        cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
            /* client callback function */

        void                               *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                  client,
            /* originating client */

        const cm_get_pwd_res_params_s_type       *get_pwd_res_params
            /* pointer to Get Password response data */

)
/*lint -esym(715,get_pwd_res_params)*/
/*lint -esym(715,client)*/
{
  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_GET_PASSWORD_RES,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */
}
/*lint +esym(715,get_pwd_res_params)*/
/*lint +esym(715,client)*/



/*===========================================================================

FUNCTION cm_sups_cmd_lcs_molr

DESCRIPTION
  LCS MOLR SS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_lcs_molr(

        cm_sups_cmd_cb_f_type            sups_cmd_cb_func,
            /* client callback function */

        void                            *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                client,
            /* originating client */

        const cm_lcs_molr_params_s_type *lcs_molr_cmd_params
            /* pointer to lcs-MOLR SS command data */

)
{
  /*lint -esym(715, client)*/
  /*lint -esym(715, lcs_molr_cmd_params)*/
  #if ( (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) )
#error code not present
  #else /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_LCS_MOLR,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ((defined(FEATURE_WCDMA) || defined(FEATURE_GSM))) */
}
 /*lint +esym(715, client)*/
 /*lint +esym(715, lcs_molr_cmd_params)*/

/*===========================================================================

FUNCTION cm_sups_cmd_lcs_molr_completed

DESCRIPTION
  LCS MOLR SS Completed command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_lcs_molr_completed(

        cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
           /* pointer to client callback data block */

        cm_client_id_type           client,
           /* originating client */

        const cm_lcs_molr_completed_params_s_type  *lcs_molr_completed_cmd_params
          /* pointer to lcs-MOLR Completed SS command data */

)
{
  /*lint -esym(715, client)*/
  /*lint -esym(715, lcs_molr_completed_cmd_params)*/

  #if ( (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) )
#error code not present
  #else /* ((defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_LCS_MOLR_COMPLETED,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */
}

  /*lint +esym(715, client)*/
  /*lint +esym(715, lcs_molr_completed_cmd_params)*/

/*===========================================================================

FUNCTION cm_sups_cmd_lcs_location_notification_res

DESCRIPTION
  Send an LCS location notification response to the network.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_lcs_location_notification_res(

        cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
            /* client callback function */

        void                               *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                  client,
            /* originating client */

        const cm_lcs_location_notification_res_params_s_type       *lcs_location_notification_res_params
            /* pointer to location notification response data */

)
{


  /*lint -esym(715, client)*/
  /*lint -esym(715, lcs_location_notification_res_params)*/

  #if ( (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) )
#error code not present
  #else /* ((defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ((defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */
}
/*lint +esym(715, client)*/
/*lint +esym(715, lcs_location_notification_res_params)*/

/*===========================================================================
FUNCTION cm_bcmcs_cmd_flow_request

DESCRIPTION

  Queue CM_BCMCS_CMD_FLOW_REQUEST_F command to CM.
  This command is used to register BCMCS data flows with
  lower layer.

  If num_of_flows > 0 and flow_ids_array_ptr is not NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F will result in
  "update flow registration" command sent to the lower layer.

  If num_of_flows = 0 and flow_ids_array_ptr is NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F will result in
  "discontinue broadcast" command sent to the lower layer.


DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_bcmcs_cmd_flow_request(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           num_of_flows,
    /* number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr
    /* array of IP:port number addresses for which flows should be requested */
)
{
  static sys_bcmcs_reg_setup_e_type 
            flow_reg_setup_array_ptr[SYS_BCMCS_MAX_FLOWS];


  memset(flow_reg_setup_array_ptr , 0 , 
             SYS_BCMCS_MAX_FLOWS * sizeof(sys_bcmcs_reg_setup_e_type));

  if(num_of_flows > 0)
  {
    return(cm_bcmcs_cmd_flow_request2(cmd_cb_func,
                                      data_block_ptr,
                                      client_id,
                                      num_of_flows,
                                      flow_ids_array_ptr,
                                      flow_reg_setup_array_ptr));
  }
  else
  {
    return(cm_bcmcs_cmd_flow_request2(cmd_cb_func,
                                      data_block_ptr,
                                      client_id,
                                      num_of_flows,
                                      flow_ids_array_ptr,
                                      NULL));
  }
}

/*===========================================================================
FUNCTION cm_bcmcs_cmd_flow_request2

DESCRIPTION

  Queue CM_BCMCS_CMD_FLOW_REQUEST_F command to CM.
  This command is used to register BCMCS data flows with
  lower layer.

  If num_of_flows > 0 and flow_ids_array_ptr is not NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F will result in
  "update flow registration" command sent to the lower layer.

  If num_of_flows = 0 and flow_ids_array_ptr is NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F will result in
  "discontinue broadcast" command sent to the lower layer.


DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_bcmcs_cmd_flow_request2(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           num_of_flows,
    /* number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr,
    /* array of IP:port number addresses for which flows should be requested */

  const sys_bcmcs_reg_setup_e_type *flow_reg_setup_array_ptr
    /* whether non-required registration should be allowed for each flow */

)
/*lint -esym(715, num_of_flows)*/
/*lint -esym(715, flow_ids_array_ptr)*/
/*lint -esym(715, flow_reg_setup_array_ptr)*/
/*lint -esym(715, client_id)*/
{

  #if (defined (FEATURE_BCMCS) || defined (FEATURE_MODEM_MBMS))

  cm_cmd_s_type                   *cmd_ptr       = NULL;
  cm_bcmcs_cmd_s_type             *bcmcs_cmd_ptr;
  cm_bcmcs_cmd_info_u_type        *cmd_info_ptr  = NULL;

  CM_ASSERT( BETWEEN(client_id,CM_CLIENT_TYPE_NONE,CM_CLIENT_TYPE_MAX));

  /*
  ** Check for any function parameter errors
  */

  /* Check for valid number of flow ids */
  if( !(num_of_flows <= SYS_BCMCS_MAX_FLOWS) ||
      ((num_of_flows == 0) && (flow_ids_array_ptr != NULL) )||
      ((num_of_flows == 0) && (flow_reg_setup_array_ptr != NULL) ))
  {
    if( cmd_cb_func != NULL )
    {
        cmd_cb_func( data_block_ptr,
                     CM_BCMCS_CMD_FLOW_REQUEST_F,
                     CM_BCMCS_CMD_ERR_INVALID_NUMBER_OF_FLOW_IDS_P);
    }
    return FALSE;
  }

  /* Now that num_of_flows parameter is OK, check pointer
     to flow id list */
  if(((flow_ids_array_ptr == NULL) && ( num_of_flows != 0 ))||
     ((flow_reg_setup_array_ptr == NULL) && ( num_of_flows != 0 )))
  {
    if( cmd_cb_func != NULL )
    {

      cmd_cb_func( data_block_ptr,
                   CM_BCMCS_CMD_FLOW_REQUEST_F,
                   CM_BCMCS_CMD_ERR_FLOW_IDS_NULL_PTR_P);
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Now that all function parameters are correct,
     get command buffer to queue CM_BCMCS_CMD_FLOW_REQUEST_F */
  cmd_ptr = cm_cmd_alloc_bcmcs_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
  if ( cmd_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_BCMCS_CMD_FLOW_REQUEST_F,
                   CM_BCMCS_CMD_ERR_NO_BUF_L);
    }

    return FALSE;
  }
  
  /* Else (command buffer allocation is successful),
  ** copy command information into command structure.
  */

  /* Copy command header fields. */
  bcmcs_cmd_ptr                  = BCMCS_CMD_PTR( cmd_ptr );
  bcmcs_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  bcmcs_cmd_ptr->data_block_ptr  = data_block_ptr;
  bcmcs_cmd_ptr->client_id       = client_id;
  bcmcs_cmd_ptr->cmd             = CM_BCMCS_CMD_FLOW_REQUEST_F;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields. */
  cmd_info_ptr              = CMD_INFO_PTR( bcmcs_cmd_ptr );

  cmd_info_ptr->flow_req.num_of_flows = num_of_flows;

  if (num_of_flows > ARR_SIZE(cmd_info_ptr->flow_req.flow_addr_array))
  {
    CM_ERR ("num of flows exceeds flow_addr_array size", 0, 0,0);

    /* Limiting num_of_flows to what array allows
    */
    num_of_flows = ARR_SIZE(cmd_info_ptr->flow_req.flow_addr_array);
  }

  CM_MSG_HIGH ("Flow service requested = %d", 
    cmd_info_ptr->flow_req.flow_addr_array[0].flow_service, 0, 0);

    /*lint -e{418,831,668} Passing null pointer to memcpy */
  memcpy(cmd_info_ptr->flow_req.flow_addr_array ,
         flow_ids_array_ptr,
         num_of_flows * sizeof(sys_bcmcs_flow_addr_s_type));

  #ifdef FEATURE_BCMCS_20
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( cmd_ptr );

  return TRUE;

  #else /* FEATURE_BCMCS || MBMS not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_BCMCS_CMD_FLOW_REQUEST_F,
                 CM_BCMCS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of defined FEATURE_BCMCS */

}
/*lint +esym(715, num_of_flows)*/
/*lint +esym(715, flow_ids_array_ptr)*/
/*lint +esym(715, flow_reg_setup_array_ptr)*/
/*lint +esym(715, client_id)*/

/*===========================================================================
FUNCTION cm_bcmcs_cmd_registration_request

DESCRIPTION

  Queue CM_BCMCS_CMD_REG_REQUEST_F command to CM.
  This command is used to register BCMCS data flows with
  lower layer.


DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

SIDE EFFECTS
  None

===========================================================================*/

boolean cm_bcmcs_cmd_registration_request(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           num_of_flows,
    /* number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr
    /* array of IP:port number addresses for which flows should be registered */
)
/*lint -esym(715, num_of_flows)*/
/*lint -esym(715, flow_ids_array_ptr)*/
/*lint -esym(715, client_id)*/
{

  #ifdef FEATURE_BCMCS_20
#error code not present
  #else /* FEATURE_BCMCS_20 not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_BCMCS_CMD_REG_REQUEST_F,
                 CM_BCMCS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;

  #endif /* else of defined FEATURE_BCMCS_20 */

}
/*lint +esym(715, num_of_flows)*/
/*lint +esym(715, flow_ids_array_ptr)*/
/*lint +esym(715, client_id)*/


/*===========================================================================
FUNCTION cm_bcmcs_cmd_registration_handoff

DESCRIPTION

  Queue CM_BCMCS_CMD_REG_HANDOFF_F command to CM.
  This command is used to enable event driven registrations on handoff


DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or invalid parameters 
  passed to the function.

SIDE EFFECTS
  None

===========================================================================*/

boolean cm_bcmcs_cmd_registration_handoff(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           num_of_flows,
    /* number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr
    /* array of IP:port number addresses for which flows should be registered */
)
/*lint -esym(715, num_of_flows)*/
/*lint -esym(715, flow_ids_array_ptr)*/
/*lint -esym(715, client_id)*/
{

  #if defined( FEATURE_BCMCS_20 ) && defined( FEATURE_HDR_BCMCS_DYNAMIC_MODEL )
#error code not present
  #else /* FEATURE_BCMCS_20 not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_BCMCS_CMD_REG_HANDOFF_F,
                 CM_BCMCS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;

  #endif /* else of defined FEATURE_BCMCS_20 */

}
/*lint +esym(715, num_of_flows)*/
/*lint +esym(715, flow_ids_array_ptr)*/
/*lint +esym(715, client_id)*/

/*===========================================================================
FUNCTION cm_bcmcs_cmd_bom_cache_setup

DESCRIPTION

  Queue CM_BCMCS_CMD_BOM_CACHING_SETUP_F command to CM.
  This command is used to enable / disable strict BOM caching

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_bcmcs_cmd_bom_cache_setup(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           stale_BOM_factor,
    /* multiplier for staleness of BOM entry */

  sys_bcmcs_bom_caching_e_type    enable_strict_caching
    /* enable / disable strict BOM caching */
)
/*lint -esym(715, client_id)*/
/*lint -esym(715, stale_BOM_factor)*/
/*lint -esym(715, enable_strict_caching)*/
{

  #if defined( FEATURE_BCMCS_20 ) && defined( FEATURE_HDR_BCMCS_DYNAMIC_MODEL )
#error code not present
  #else /* FEATURE_BCMCS_20 not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_BCMCS_CMD_FLOW_REQUEST_F,
                 CM_BCMCS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of defined FEATURE_BCMCS_20 */

}
/*lint +esym(715, client_id)*/
/*lint +esym(715, stale_BOM_factor)*/
/*lint +esym(715, enable_strict_caching)*/

/*===========================================================================
FUNCTION cm_mbms_cmd_flow_deact_request

DESCRIPTION
  This command is used to deact MBMS data flow requests with
  lower layers.

  
DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

SIDE EFFECTS
  None
                     
===========================================================================*/
boolean cm_mbms_cmd_flow_deact_request(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           num_of_flows,
    /* number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr
    /* array of IP:port number addresses for which flows should be requested */

)
/*lint -esym(715,cmd_cb_func)*/
/*lint -esym(715,data_block_ptr)*/
/*lint -esym(715,client_id)*/
/*lint -esym(715,num_of_flows)*/
/*lint -esym(715,flow_ids_array_ptr)*/
/*lint -esym(818, data_block_ptr)*/
{  
  #if defined (FEATURE_MODEM_MBMS)
#error code not present
  #else
  CM_ERR( "FEATURE_MODEM_MBMS is not defined ",0,0,0 );
  return FALSE;
  #endif
  
} /* cm_mbms_cmd_flow_deact_request () */
/*lint +esym(715,cmd_cb_func)*/
/*lint +esym(715,data_block_ptr)*/
/*lint +esym(715,client_id)*/
/*lint +esym(715,num_of_flows)*/
/*lint +esym(715,flow_ids_array_ptr)*/
/*lint +esym(818, data_block_ptr)*/

/*===========================================================================
FUNCTION cm_ph_cmd_line_switching

DESCRIPTION

  Queue CM_PH_CMD_LINE_SWITCHING command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_line_switching(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  cm_als_line_switching_e_type         line_switching
    /* whether the user is allowed to switch between lines */

)
/*lint -esym(715, line_switching)*/
/*lint -esym(715, client_id)*/
{

  #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
  #else /* FEATURE_ALS not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_PH_CMD_LINE_SWITCHING,
                 CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of defined FEATURE_ALS */

}
/*lint +esym(715, line_switching)*/
/*lint +esym(715, client_id)*/

/*===========================================================================
FUNCTION cm_ph_cmd_selected_line

DESCRIPTION

  Queue CM_PH_CMD_SELECTED_LINE command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_selected_line(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  cm_als_line_e_type                   selected_line
    /* selected line */

)
/*lint -esym(715,client_id)*/
/*lint -esym(715,selected_line)*/
{

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
  #else /* FEATURE_ALS not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_PH_CMD_SELECTED_LINE,
                 CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of defined FEATURE_ALS */

}
/*lint +esym(715,client_id)*/
/*lint +esym(715,selected_line)*/


/*===========================================================================

FUNCTION cm_mm_call_cmd_recall_rsp

DESCRIPTION
  This function carries response from client for a recall indication from
  network.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mm_call_cmd_recall_rsp (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_call_id_type               call_id,
    /* call ID of the call */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_ccbs_recall_rsp_e_type     ccbs_recall_rsp
    /* client response to recall indication */
)
{

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  /* Lint suppressions
  */
  SYS_ARG_NOT_USED (ccbs_recall_rsp);
  SYS_ARG_NOT_USED (client_id);
  SYS_ARG_NOT_USED (call_id);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_RECALL_RSP,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

} /* cm_mm_call_cmd_recall_rsp () */


/*===========================================================================

FUNCTION cm_mm_call_cmd_modify_req

DESCRIPTION
  Sends a request for in-call (connected state) modification of the call type.
  Call needs to be connected for this request to be processed.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  ((FEATURE_WCDMA || FEATURE_GSM) && CM_API_VT_FALLBACK_TO_VOICE) must be
  defined.

RETURN VALUE
  TRUE   - Cmd has been queued to CM
  FALSE  - Cmd did not get queued to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mm_call_cmd_modify_req (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_call_id_type               call_id,
    /* call ID of the call */

  cm_call_type_e_type           new_call_type_req
    /* New call type requested for call modification */
)
{
  #if ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK))
#error code not present
  #else /* ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(call_id);
  SYS_ARG_NOT_USED(new_call_type_req);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_MO_MODIFY_REQ,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

} /* cm_mm_call_cmd_modify_req () */

/*===========================================================================

FUNCTION cm_mm_call_cmd_modify_res

DESCRIPTION
  Response to network initiated in-call modification request.
  Call needs to be connected for this request to be processed.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  ((FEATURE_WCDMA || FEATURE_GSM) && CM_API_VT_FALLBACK_TO_VOICE) must be
  defined.

RETURN VALUE
  TRUE   - Cmd got queued to CM
  FALSE  - Cmd did not get queued to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mm_call_cmd_modify_res (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_call_id_type               call_id,
    /* call ID of the call */

  boolean                       gw_cs_modify_res,
    /* Client response to modification request
    ** TRUE   - Modification is accepted by client
    ** FALSE  - Modification gets rejected by client
    */

  ie_cm_cc_cause_s_type         *cc_cause_ptr
    /* Specific cause value when gw_cs_modify_res is FALSE
    ** Ignored when gw_cs_modify_res is TRUE.
    */

)
/*lint -esym(818, cc_cause_ptr)*/
{
  #if ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK))
#error code not present
  #else /* ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

  SYS_ARG_NOT_USED(call_id);
  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(gw_cs_modify_res);
  SYS_ARG_NOT_USED(cc_cause_ptr);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_MODIFY_RES,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

} /* cm_mm_call_cmd_modify_res */
/*lint +esym(818, cc_cause_ptr)*/


/*===========================================================================

FUNCTION cm_mm_call_comp_cap_cb_reg

DESCRIPTION
  Clients call this function to register call backs that can be used to
  receive capability and compatibility information for a call type.

  BC, LLC and HLC values are differnet for  call types. For mixed mode call
  origination, a voice call needs to carry BC, LLC, HLC details for a VT
  call. CM provides a storage for these values so that clients can query
  for other call type's details. Example - UI can query bearer details for
  VT call in order to originate a mixed mode call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE   - Callback could be registered successfully
  FALSE  - Callback could not be registered successfully.

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mm_call_comp_cap_cb_reg(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* requesting client ID */

  cm_call_type_e_type    call_type,
    /* Call type for which call back has to be set */

  cm_call_comp_cap_cb_f_type *comp_cap_cb_func_ptr
    /* Callback that retrieves compatibility and capability for the
    ** call type.
    */
)
/*lint -esym(715,cmd_cb_func)*/
/*lint -esym(818,data_block_ptr)*/
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(call_type);
  SYS_ARG_NOT_USED(comp_cap_cb_func_ptr);
  SYS_ARG_NOT_USED(data_block_ptr);

  #endif /* else of (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  return FALSE;

} /* cm_mm_call_comp_cap_cb_reg */
/*lint +esym(715,cmd_cb_func)*/
/*lint +esym(818,data_block_ptr)*/

/*===========================================================================

FUNCTION cm_mm_get_call_comp_cap

DESCRIPTION
  Return the call compatibility and capability for given call type.


DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE   - Call compatibiliy and capability being returned.
  FALSE  - Call compatibiliy and capability could not be returned.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_get_call_comp_cap(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* requesting client ID */

  cm_call_type_e_type    call_type,
    /* Call type */

  cm_call_comp_cap_info_s_type   *call_comp_cap_info_ptr
    /* Pointer used to return compatibility and capability for call type */

)
/*lint -esym(715,cmd_cb_func)*/
/*lint -esym(818,data_block_ptr)*/
/*lint -esym(818,call_comp_cap_info_ptr)*/
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(call_type);
  SYS_ARG_NOT_USED(call_comp_cap_info_ptr);
  SYS_ARG_NOT_USED(data_block_ptr);

  #endif /* else of (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  return FALSE;

} /* cm_mm_get_call_comp_cap () */
/*lint +esym(715,cmd_cb_func)*/
/*lint +esym(818,data_block_ptr)*/
/*lint +esym(818,call_comp_cap_info_ptr)*/


#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */



/*===========================================================================

FUNCTION cm_pd_reg

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
cm_pd_status_e_type cm_pd_reg(

    cm_pd_resp_is_within_t2m_f_type     *cm_pd_resp_is_within_t2m_func
        /* pointer position determination function  types */
)
/*lint -esym(715, cm_pd_resp_is_within_t2m_func)*/
{
  #ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
  #else /* FEATURE_GPSONE_NEW_L2_ACK */
  return  CM_PD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* else of FEATURE_GPSONE_NEW_L2_ACK */
}
/*lint +esym(715, cm_pd_resp_is_within_t2m_func)*/


/*===========================================================================
=============================================================================
================ NUMBER-CLASSFICATION INTERFACE BLOCK =======================
=============================================================================
===========================================================================*/


/* This block group all the functionality that is associated with
** number-classfication interfacing.
**
** When call manager is commanded to originate a call, it consults the
** number-classfication of the following information:
**
** 1. Whether the dialed number matches one of the special
**    numbers that are stored in the number-classfication (i.e. E911, OTASP, etc.)
**    If a match is found, CM modify the originating call type accordingly.
**
** 2. After deciding on the originating call type, CM consults the
**    number-classfication whether the dialed number is allowed by the
**    origination restriction currently in effect. If the dialed number
**    is NOT allowed, CM blocks the outgoing call and notifies clients of
**    origination failure.
**
** Note that CM expects a UI like client to register a number-classfication callback
** functions with CM during initialization time.
**
** Until such callback functions are registered with CM, CM uses a default
** hard-coded number-classfication to decide on the call type of special numbers.
*/


/* Type to hold number classification interface related information
*/
typedef struct cm_nc_s {

    cm_nc_resolve_call_type_f_type     *resolve_call_type_func;
        /* pointer to a number-classfication function resolving call types */

    cm_nc_is_call_allowed_f_type       *cm_nc_is_call_allowed_func;
        /* pointer to a number-classfication function deciding whether a
           specific call type is allowed for origination */

#ifdef FEATURE_JCDMA
    cm_nc_pre_pending_dial_f_type      *pre_pending_dial_func;
        /* pointer pre-pending function resolving origination # */
#endif /* FEATURE_JCDMA */

    cm_nc_call_control_processing_f_type
                                        *cm_nc_call_control_processing_func;
        /* pointer to a call control processing function */

    cm_nc_call_control_preferences_f_type
                                        *cm_nc_call_control_preferences_func;
        /* pointer to a call control preferences function */

    cm_nc_call_control_processing_mod_f_type
                                    *cm_nc_call_control_processing_func_mod;
        /* pointer to a call control processing function with addnl params */

} cm_nc_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* CM number-classfication interface object.
*/
static cm_nc_s_type    cm_nc;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return a pointer to CM interface object
**
** NOTE! This macro should only be used within the
** number-classfication object scope.
*/
#define CM_NC_PTR() ( &cm_nc )





/*===========================================================================

FUNCTION cm_nc_init

DESCRIPTION
  Initialize the CM number-classficationinterface object.

  This function must be called before the CM number-classfication interface object
  is being used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_nc_init( void )
{

  cm_nc_s_type    *nc_ptr = CM_NC_PTR();
      /* point at phone-book interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_nc_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize callback function pointers to NULL
  */
  nc_ptr->resolve_call_type_func              = NULL;
  nc_ptr->cm_nc_is_call_allowed_func          = NULL;
  nc_ptr->cm_nc_call_control_processing_func  = NULL;
  nc_ptr->cm_nc_call_control_preferences_func = NULL;
#ifdef FEATURE_JCDMA
  nc_ptr->pre_pending_dial_func               = NULL;
#endif /* FEATURE_JCDMA */
  nc_ptr->cm_nc_call_control_processing_func_mod  = NULL;
}




/*===========================================================================

FUNCTION cm_nc_reg

DESCRIPTION
  Register number-classfication callback functions with CM

  When call manager is commanded to originate a call, it consults the
  number-classfication callback functions of the following information:

  1. Whether the dialed number matches one of the special
     numbers that are stored in the number-classfication (i.e. E911, OTASP, etc.)
     If a match is found, CM modify the originating call type accordingly.

  2. After deciding on the originating call type, CM consults the
     number-classfication whether the dialed number is allowed by the
     origination restriction currently in effect. If dialed number
     is not allowed, CM blocks the outgoing call and notifies clients of
     origination failure.

DEPENDENCIES
  number-classfication interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_nc_status_e_type cm_nc_reg(

    cm_nc_resolve_call_type_f_type     *resolve_call_type_func,
        /* pointer phone-book function resolving call types */

    cm_nc_is_call_allowed_f_type        *nc_is_call_allowed_func,
        /* pointer to a phone-book deciding whether a specific call type
        ** is allowed for origination */

    cm_nc_pre_pending_dial_f_type      *pre_pending_dial_func
        /* pointer pre-pending function resolving origination # */
)
/*lint -esym(715, pre_pending_dial_func)*/
{

  cm_nc_s_type    *nc_ptr = CM_NC_PTR();
      /* point at number-classfication interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(resolve_call_type_func == NULL)
  {
    return  CM_NC_ERR_NULL_CALL_TYPE;
  }
  else if (nc_is_call_allowed_func == NULL)
  {
    return  CM_NC_ERR_NULL_CALL_ALLOWED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nc_ptr->resolve_call_type_func      = resolve_call_type_func;
  nc_ptr->cm_nc_is_call_allowed_func  = nc_is_call_allowed_func;
#ifdef FEATURE_JCDMA
  nc_ptr->pre_pending_dial_func       = pre_pending_dial_func;
#endif /* FEATURE_JCDMA */

  return  CM_NC_OK;
}
/*lint +esym(715, pre_pending_dial_func)*/


/*===========================================================================

FUNCTION cm_nc_reg_with_call_control

DESCRIPTION
  Register number-classification callback functions with CM

  When call manager is commanded to originate a call, it consults the
  number-classification callback function for the following information:

  1. Whether the originating number matches one of the special
     numbers that are stored in the number-classification (i.e. EMERGENCY, OTASP, etc.)
     If a match is found, CM modify the originating call type accordingly.

  2. Whether the originating number is allowed on the sims present.

  3. And if the calls are allowed and call control is required, perform any necessary
     modifications.


DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_nc_status_e_type cm_nc_reg_with_call_control(

  cm_nc_call_control_processing_f_type    *cm_nc_call_control_processing_func,
    /* ptr to function that will resolve call type, check whether the call is
       allowed, and do any call control modifications if necessary and for
       JCDMA any pre pending dial information.
    */

  cm_nc_call_control_preferences_f_type   *cm_nc_call_control_preferences_func
    /* ptr to function that will resolve which mode to originate on when the
       call is allowed on both slots and/or the modifications are conflicting.
    */

)
{

  cm_nc_s_type                       *nc_ptr = CM_NC_PTR();
    /* point at number-classification interface object */

  /*-----------------------------------------------------------------------------*/

  if (cm_nc_call_control_processing_func == NULL)
  {
    return CM_NC_ERR_NULL_CC_PROCESSING_FUNC;
  }

  if (cm_nc_call_control_preferences_func == NULL)
  {
    return CM_NC_ERR_NULL_CC_PREFERENCES_FUNC;
  }

  /*-----------------------------------------------------------------------------*/
  nc_ptr->cm_nc_call_control_processing_func = cm_nc_call_control_processing_func;
  nc_ptr->cm_nc_call_control_preferences_func = cm_nc_call_control_preferences_func;

  return CM_NC_OK;
}

/*===========================================================================

FUNCTION cm_nc_reg_with_call_control_mod

DESCRIPTION
  Register number-classification callback functions with CM

  When call manager is commanded to originate a call, it consults the
  number-classification callback function for the following information:

  1. Whether the originating number matches one of the special
     numbers that are stored in the number-classification (i.e. EMERGENCY, OTASP, etc.)
     If a match is found, CM modify the originating call type accordingly.

  2. Whether the originating number is allowed on the sims present.

  3. And if the calls are allowed and call control is required, perform any necessary
     modifications.


DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_nc_status_e_type cm_nc_reg_with_call_control_mod(

  cm_nc_call_control_processing_mod_f_type
                                    *cm_nc_call_control_processing_func_mod,
    /* ptr to function that will resolve call type, check whether the call is
       allowed, and do any call control modifications if necessary and for
       JCDMA any pre pending dial information.
    */

  cm_nc_call_control_preferences_f_type
                                        *cm_nc_call_control_preferences_func
    /* ptr to function that will resolve which mode to originate on when the
       call is allowed on both slots and/or the modifications are conflicting.
    */

)
{

  cm_nc_s_type                       *nc_ptr = CM_NC_PTR();
    /* point at number-classification interface object */

  /*-----------------------------------------------------------------------------*/

  if (cm_nc_call_control_processing_func_mod == NULL)
  {
    return CM_NC_ERR_NULL_CC_PROCESSING_FUNC;
  }

  if (cm_nc_call_control_preferences_func == NULL)
  {
    return CM_NC_ERR_NULL_CC_PREFERENCES_FUNC;
  }

  /*-----------------------------------------------------------------------------*/
  nc_ptr->cm_nc_call_control_processing_func_mod = cm_nc_call_control_processing_func_mod;
  nc_ptr->cm_nc_call_control_preferences_func = cm_nc_call_control_preferences_func;

  return CM_NC_OK;
}

/* This is used only by cmwcall/cmxcall, must be moved */


/*===========================================================================

FUNCTION cm_nc_resolve_call_type

DESCRIPTION
  Resolve originating call type.

  This functions consults the phone-book to whether the dialed number
  matches one of the special numbers that are stored in the phone-book
  (i.e. E911, OTASP, etc.). If a match is found, it modifies the originating
  call type accordingly.

DEPENDENCIES
  Phone-book interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  none

SIDE EFFECTS
  Modifies the originating call type (pointed by activate_code_ptr) and or
  the activation code (pointed by activate_code_ptr).

===========================================================================*/
void cm_nc_resolve_call_type(

    const cm_num_s_type            *orig_num_ptr,
        /* dialed number */

    cm_call_type_e_type      *call_type_ptr,
        /* Associated call type */

    cm_activate_code_e_type    *activate_code_ptr
        /* Associated OTASP activation code (for OTASP call types) */
)
{
  cm_nc_s_type    *nc_ptr = CM_NC_PTR();
      /* point at number-classfication interface object */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(orig_num_ptr      != NULL);
  CM_ASSERT(call_type_ptr     != NULL);
  CM_ASSERT(activate_code_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (nc_ptr->resolve_call_type_func == NULL )
  {
     return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is not a voice call, return now.
  */
  if( *call_type_ptr != CM_CALL_TYPE_VOICE )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Consult the phone-book callback function of the type
  ** of the dialed number.
  */
  nc_ptr->resolve_call_type_func( orig_num_ptr,
                                  call_type_ptr,
                                  activate_code_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If FEATURE_OTASP is NOT defined, make sure that the callback
  ** did not set the call type to OTASP. If it did, default the call
  ** type back to VOICE.
  */
  #ifndef FEATURE_OTASP

  if( CM_CALL_TYPE_IS_OTASP(*call_type_ptr) )
  {
    CM_ERR( "FEATURE_OTASP is NOT defined",0,0,0 );
    *call_type_ptr = CM_CALL_TYPE_VOICE;
  }

  #endif /* FEATURE_OTASP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If FEATURE_E911 is NOT defined, make sure that the phone-book
  ** did not set the call type to E911. If it did, default the call
  ** type back to VOICE.
  */
  #if !defined(FEATURE_E911) && !defined(FEATURE_WCDMA) && !defined(FEATURE_GSM)
  if( *call_type_ptr == CM_CALL_TYPE_EMERGENCY )
  {
    CM_ERR( "FEATURE_E911 is NOT defined",0,0,0 );
    *call_type_ptr = CM_CALL_TYPE_VOICE;
  }
  #endif  /* ! FEATURE_E911 && ! FEATURE_WCDMA && ! FEATURE_GSM */
} /* cm_nc_resolve_call_type() */


/* This is used only by cmxcall/cmwcall (future, when SUPS will be merged) */

/*===========================================================================

FUNCTION cm_nc_is_call_allowed_func

DESCRIPTION
  Decide whether a dialed number is allowed.

  This functions consults the number-classfication to whether the dialed number
  is allowed by the origination restriction currently in effect.

DEPENDENCIES
  number-classfication interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  TRUE of dialed number is allowed for origination, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_nc_is_call_allowed_func(

    const cm_num_s_type            *orig_num_ptr
        /* pointer to dialed number */
)
{

  cm_nc_s_type    *nc_ptr = CM_NC_PTR();
      /* point at phone-book interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (nc_ptr->cm_nc_is_call_allowed_func == NULL )
  {
    /* all call allowed */
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Consult the number-classfication callback function to whether the originating
  ** number is allowed.
  */
  return nc_ptr->cm_nc_is_call_allowed_func( orig_num_ptr );

}


/*===========================================================================

FUNCTION cm_nc_call_control_processing

DESCRIPTION
  This function will perform call control processing on an originating number.

DEPENDENCIES
  none

RETURN VALUE
  FALSE if CM cmd buffer allocation fails, TRUE otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_nc_call_control_processing(

  cm_call_id_type                       call_id,
    /* call id */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /* where CM is getting it's gw subscription information */

  cm_call_type_e_type                   call_type,
    /* call type */

  cm_num_s_type                         *orig_num_ptr,
    /* orginating number */

  void                                  *user_data,
    /* Clients should not modify - Data to be passed back in cb function */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /* function to be called when processing is complete */

  cm_gw_ps_cc_params_s_type             *gw_ps_cc_params_ptr,

  const cm_cc_generic_params_s_type     *cc_generic_params_ptr
    /* Call-control generic parameters */

)
{
  cm_nc_s_type                      *nc_ptr = CM_NC_PTR();
  cm_cmd_s_type                     *cmd_ptr = (cm_cmd_s_type*) user_data;
  cm_orig_call_control_s_type       cdma_call_control_info;
  cm_orig_call_control_s_type       gw_call_control_info;
  cm_call_type_e_type               new_call_type = call_type;
  cm_activate_code_e_type           otasp_act_code;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(orig_num_ptr                     != NULL);
  CM_ASSERT(user_data                        != NULL);
  CM_ASSERT(cm_call_control_complete_cb_func != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL);

  /* Mark the command buffer for reuse since it will be requeued by the
  ** registered number classification functions.
  */
  cmd_ptr->is_reused = TRUE;

 /*-----------------------------------------------------------------------------------*/

  #ifdef FEATURE_GSTK_FDN_CC_SUPPORT

  CM_MSG_MED ("FEATURE_GSTK_FDN_CC_SUPPORT CC", 0, 0, 0);

  /* If there is a registered number classification function, go ahead and just use
  ** the registered one.
  */
  if ( nc_ptr->cm_nc_call_control_processing_func_mod != NULL )
  {
    CM_MSG_MED ("Calling regd CC fn", 0, 0, 0);

    cmd_ptr->passed_for_cc = 1;

    return nc_ptr->cm_nc_call_control_processing_func_mod
    (
      call_id,
      cdma_subscription_source,
      gw_subscription_source,
      call_type,
      orig_num_ptr,
      cmd_ptr,
      cm_call_control_complete_cb_func,
      gw_ps_cc_params_ptr,
      cc_generic_params_ptr
    );

  }

  #else
  SYS_ARG_NOT_USED(cc_generic_params_ptr);

  /* If there is a registered number classification function, go ahead and just use
  ** the registered one.
  */
  if ( nc_ptr->cm_nc_call_control_processing_func != NULL )
  {

    CM_MSG_MED ("Calling regd CC fn", 0, 0, 0);

    cmd_ptr->passed_for_cc = 1;

    return nc_ptr->cm_nc_call_control_processing_func
    (
      call_id,
      cdma_subscription_source,
      gw_subscription_source,
      call_type,
      orig_num_ptr,
      cmd_ptr,
      cm_call_control_complete_cb_func,
      gw_ps_cc_params_ptr
    );

  }

  #endif /* #ifdef FEATURE_GSTK_FDN_CC_SUPPORT */

  CM_MSG_MED ("No regd CC fn", 0, 0, 0);

  /* If the call control is for a call type of SS.
  ** There are no default operations for this call type.
  ** Just pass it through as unmodified.
  */
  if (call_type == CM_CALL_TYPE_SUPS)
  {
    gw_call_control_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
    cm_call_control_complete_cb_func(
      CM_CALL_ID_UNASSIGNED,
      CM_CC_STATUS_SUCCESS,
      CM_CALL_TYPE_SUPS,
      NULL,
      &gw_call_control_info,
      cmd_ptr
    );
    return TRUE;
  }

  /* Since there is no functions registered ....
  */

  /* First, resolve the call type */
  cm_kick_dog ();
  cm_nc_resolve_call_type(orig_num_ptr,
                          &new_call_type,
                          &otasp_act_code);
  cm_kick_dog ();

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  if ((new_call_type == CM_CALL_TYPE_OTAPA) ||
      (new_call_type == CM_CALL_TYPE_STD_OTASP) ||
      (new_call_type == CM_CALL_TYPE_NON_STD_OTASP))
  {
    cdma_call_control_info.activate_code = otasp_act_code;
    cmd_ptr->cmd.call.info.cdma_info.otasp_act_code = otasp_act_code;
  }
  #endif  /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

  cmd_ptr->cmd.call.info.call_type = new_call_type;

  /*
  ** If he call is of type emergency, the call is allowed
  ** and there is no need to continue any further.  Just return.
  */
  if (new_call_type == CM_CALL_TYPE_EMERGENCY)
  {
    cdma_call_control_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
    gw_call_control_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

    #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
    #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */


    cm_call_control_complete_cb_func(
      call_id,
      CM_CC_STATUS_SUCCESS,
      new_call_type,
      &cdma_call_control_info,
      &gw_call_control_info,
      cmd_ptr
    );
    return TRUE;
  }
  else if (new_call_type == CM_CALL_TYPE_PS_DATA)
  {
    /* Handles when there is no call control function registered with CM
    */

    CM_MSG_HIGH ("NO CC Registered, allowing PS as is", 0, 0, 0);

    if (call_type != CM_CALL_TYPE_PS_DATA)
    {
      CM_ASSERT (call_type == CM_CALL_TYPE_PS_DATA);
      CM_ERR ("Unexpected modification to PS call", 0, 0, 0);
    }

    cdma_call_control_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
    gw_call_control_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

    /* Copy packet call control parameters as given by cmd */
    gw_call_control_info.gw_ps_cc_params = *gw_ps_cc_params_ptr;

  }

  /* Now check whether the call is allowed */
  if (!cm_nc_is_call_allowed_func(orig_num_ptr))
  {
    cdma_call_control_info.call_control_result = CM_CC_RESULT_NOT_ALLOWED;
    gw_call_control_info.call_control_result = CM_CC_RESULT_NOT_ALLOWED;

    cm_call_control_complete_cb_func(
      call_id,
      CM_CC_STATUS_SUCCESS,
      new_call_type,
      &cdma_call_control_info,
      &gw_call_control_info,
      cmd_ptr
    );
    return TRUE;
  }

#if defined( FEATURE_GSTK ) && (defined( FEATURE_GSM ) || defined( FEATURE_WCDMA ))
#error code not present
#endif   /* FEATURE_GST && ( FEATURE_GSM || FEATURE_WCDMA ) */
  {
    if (cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      cdma_call_control_info.call_control_result = CM_CC_RESULT_NOT_ALLOWED;
    }
    else
    {
      cdma_call_control_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
    }
    if (gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      gw_call_control_info.call_control_result = CM_CC_RESULT_NOT_ALLOWED;
    }
    else
    {
      gw_call_control_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
    }

    cm_call_control_complete_cb_func(
      call_id,
      CM_CC_STATUS_SUCCESS,
      new_call_type,
      &cdma_call_control_info,
      &gw_call_control_info,
      cmd_ptr
    );
  }

  return TRUE;
}


/*===========================================================================

FUNCTION cm_nc_call_control_preferences

DESCRIPTION
  This function will decide which mode to originate on after call control
  is complete.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_cc_mode_pref_e_type cm_nc_call_control_preferences(

  cm_orig_call_control_s_type            *cdma_call_control_info_ptr,
    /* cdma information for the call */

  cm_orig_call_control_s_type            *gw_call_control_info_ptr,
    /* gw information for the call */

  cm_num_s_type                          *orig_num_ptr,
    /* Original number information before call control */

  cm_call_type_e_type                    call_type,
    /* The type of call returned by call control */

  sys_sys_mode_e_type                    current_mode
    /* mode, if any, that is acquired right now */
)
{
  cm_nc_s_type                      *nc_ptr = CM_NC_PTR();



  CM_ASSERT( nc_ptr != NULL );




  if ( nc_ptr->cm_nc_call_control_preferences_func != NULL )
  {
     return nc_ptr->cm_nc_call_control_preferences_func
     (
        cdma_call_control_info_ptr,
        gw_call_control_info_ptr,
        orig_num_ptr,
        call_type,
        current_mode
     );
  }


  /* Provide some sort of default behavior
  */

  if ((cdma_call_control_info_ptr == NULL) &&
        (gw_call_control_info_ptr == NULL))
  {
      return CM_CC_MODE_PREF_NONE;
  }
  if (cdma_call_control_info_ptr == NULL)
  {
      return CM_CC_MODE_PREF_GW_ONLY;
  }
  if (gw_call_control_info_ptr == NULL)
  {
      return CM_CC_MODE_PREF_CDMA_ONLY;
  }

  if ((cdma_call_control_info_ptr->call_control_result == CM_CC_RESULT_NOT_ALLOWED) &&
        (gw_call_control_info_ptr->call_control_result == CM_CC_RESULT_NOT_ALLOWED))
  {
      return CM_CC_MODE_PREF_NONE;
  }

  if (cdma_call_control_info_ptr->call_control_result == CM_CC_RESULT_NOT_ALLOWED)
  {
      return CM_CC_MODE_PREF_GW_ONLY;
  }
  if (gw_call_control_info_ptr->call_control_result == CM_CC_RESULT_NOT_ALLOWED)
  {
      return CM_CC_MODE_PREF_CDMA_ONLY;
  }

  return CM_CC_MODE_PREF_ANY;


} /* cm_nc_call_control_preferences */


#ifdef FEATURE_JCDMA
/*===========================================================================

FUNCTION cm_nc_pre_pending_dial_func

DESCRIPTION
  Resolve originating dial numbers for pre-pending.

  This functions insert or delete the dialed number.
  (i.e. 184, 186, etc.)

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  Modifies the originating dial numbers.

===========================================================================*/
void cm_nc_pre_pending_dial_func(

    cm_num_s_type                  *orig_num_ptr,
        /* dialed number */

    const cm_call_type_e_type      *call_type_ptr
        /* Associated call type */

)
{

  cm_nc_s_type    *nc_ptr = CM_NC_PTR();
      /* point at number-classfication interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( nc_ptr != NULL );
  if (nc_ptr->pre_pending_dial_func == NULL )
  {
     return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Consult the pre-pending dial callback function of
  ** the dialed number.
  */
  nc_ptr->pre_pending_dial_func( orig_num_ptr,
                                 call_type_ptr );

}
#endif /* FEATURE_JCDMA */



/*===========================================================================
=============================================================================
================ M51 MODE SETTINGS INTERFACE BLOCK ==========================
=============================================================================
===========================================================================*/

/* Type to hold information related to M51 mode settings interface
*/
typedef struct cm_m51_mode_settings_s {

    cm_m511_mode_settings_f_type     *change_m511_mode_func;
        /* pointer to a DS function that changes M511 mode */

    cm_m512_mode_settings_f_type     *change_m512_mode_func;
        /* pointer to a DS function that changes M512 mode */

    cm_m513_mode_settings_f_type     *change_m513_mode_func;
      /* pointer to a DS function that changes M513 mode */


} cm_m51_mode_settings_s_type;

/* CM M51 mode settings object
*/
static cm_m51_mode_settings_s_type cm_m51_mode;

/* Macro to return a pointer to M51 interface object
**
** NOTE! This macro should only be used within the
** M51 object scope.
*/
#define CM_M51_MODE_PTR() ( &cm_m51_mode )

/*===========================================================================
FUNCTION default_change_511_mode_function

DESCRIPTION

  Default handler for m511 mode change request.
  It is supposed to be replaced by DS with appropriate handler functions
  via a call to cm_m51_mode_reg()

DEPENDENCIES

RETURN VALUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean default_change_511_mode_function(sys_jcdma_m511_e_type new_mode)
/*lint -esym(715, new_mode) */
{
  CM_FUNC_START( "default_change_511_mode_function()",0,0,0 );
  return FALSE;
}
/*lint +esym(715, new_mode) */

/*===========================================================================
FUNCTION default_change_512_mode_function

DESCRIPTION

  Default handler for m512 mode change request.
  It is supposed to be replaced by DS with appropriate handler functions
  via a call to cm_m51_mode_reg()

DEPENDENCIES

RETURN VALUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean default_change_512_mode_function(sys_jcdma_m512_e_type new_mode)
/*lint -esym(715, new_mode) */
{
  CM_FUNC_START( "default_change_512_mode_function()",0,0,0 );
  return FALSE;
}
/*lint +esym(715, new_mode) */

/*===========================================================================
FUNCTION default_change_513_mode_function

DESCRIPTION

  Default handler for m513 mode change request.
  It is supposed to be replaced by DS with appropriate handler functions
  via a call to cm_m51_mode_reg()

DEPENDENCIES

RETURN VALUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean default_change_513_mode_function(sys_jcdma_m513_e_type new_mode)
/*lint -esym(715, new_mode)*/
{
  CM_FUNC_START( "default_change_513_mode_function()",0,0,0 );
  return FALSE;
}
/*lint +esym(715, new_mode)*/

/*===========================================================================
FUNCTION cm_m51_mode_init

DESCRIPTION
  Initialize M51 mode settings object.

  Use default handlers provided by CM for mode changes handlers.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_m51_mode_init( void )
{

  cm_m51_mode_settings_s_type    *m51_mode_ptr = CM_M51_MODE_PTR();
      /* point at M51 mode settings */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cm_m51_mode_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize callback function pointers to default CM handlers
  */
  m51_mode_ptr->change_m511_mode_func = default_change_511_mode_function;
  m51_mode_ptr->change_m512_mode_func = default_change_512_mode_function;
  m51_mode_ptr->change_m513_mode_func = default_change_513_mode_function;

}

/*===========================================================================
FUNCTION cm_m51_mode_reg

DESCRIPTION
  Register or de-register with CM functions used to change M51 mode.

  To de-register, provide NULL pointer as argument.
  In this case CM will substitute a default handler for M51 mode change.

DEPENDENCIES

RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_m51_mode_reg(

  cm_m511_mode_settings_f_type     *change_m511_mode_func,
      /* pointer to a DS function that changes M511 mode */

  cm_m512_mode_settings_f_type     *change_m512_mode_func,
      /* pointer to a DS function that changes M512 mode */

  cm_m513_mode_settings_f_type     *change_m513_mode_func
    /* pointer to a DS function that changes M513 mode */
)
/*lint -esym(715, change_m511_mode_func ) */
/*lint -esym(715, change_m512_mode_func ) */
/*lint -esym(715, change_m513_mode_func ) */
{
#ifdef FEATURE_JCDMA_1X
  cm_m51_mode_settings_s_type    *m51_mode_ptr = CM_M51_MODE_PTR();
      /* point at M51 mode settings */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( m51_mode_ptr != NULL );
  CM_FUNC_START( "cm_m51_mode_reg()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the functions that change M51 mode.
     Use defaults if NULL value provided.
  */
  if(change_m511_mode_func != NULL)
  {
    m51_mode_ptr->change_m511_mode_func = change_m511_mode_func;
  }
  else
  {
    m51_mode_ptr->change_m511_mode_func = default_change_511_mode_function;
  }

  if(change_m512_mode_func != NULL)
  {
    m51_mode_ptr->change_m512_mode_func = change_m512_mode_func;
  }
  else
  {
    m51_mode_ptr->change_m512_mode_func = default_change_512_mode_function;
  }

  if(change_m513_mode_func != NULL)
  {
    m51_mode_ptr->change_m513_mode_func = change_m513_mode_func;
  }
  else
  {
    m51_mode_ptr->change_m513_mode_func = default_change_513_mode_function;
  }

  return TRUE;
#else
  /* Feature FEATURE_JCDMA_1X is undefined so we return FALSE */
  return FALSE;
#endif
}
/*lint +esym(715, change_m511_mode_func ) */
/*lint +esym(715, change_m512_mode_func ) */
/*lint +esym(715, change_m513_mode_func ) */

/*===========================================================================

FUNCTION cm_change_m511_mode

DESCRIPTION
  Request DS to change M511 mode.

  This functions checks with M51 mode object and calls handler function
  that was registered for handling this mode change.

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE depending on whether DS managed to change the mode

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_change_m511_mode(sys_jcdma_m511_e_type new_mode) {

  cm_m51_mode_settings_s_type    *m51_mode_ptr = CM_M51_MODE_PTR();
      /* point at M51 mode settings */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( m51_mode_ptr->change_m511_mode_func != NULL);

  return m51_mode_ptr->change_m511_mode_func(new_mode);
}

/*===========================================================================

FUNCTION cm_change_m512_mode

DESCRIPTION
  Request DS to change M512 mode.

  This functions checks with M51 mode object and calls handler function
  that was registered for handling this mode change.

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE depending on whether DS managed to change the mode

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_change_m512_mode(sys_jcdma_m512_e_type new_mode) {

  cm_m51_mode_settings_s_type    *m51_mode_ptr = CM_M51_MODE_PTR();
      /* point at M51 mode settings */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( m51_mode_ptr->change_m512_mode_func != NULL);

  return m51_mode_ptr->change_m512_mode_func(new_mode);
}

/*===========================================================================

FUNCTION cm_change_m513_mode

DESCRIPTION
  Request DS to change M513 mode.

  This functions checks with M51 mode object and calls handler function
  that was registered for handling this mode change.

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE depending on whether DS managed to change the mode

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_change_m513_mode(sys_jcdma_m513_e_type new_mode) {

  cm_m51_mode_settings_s_type    *m51_mode_ptr = CM_M51_MODE_PTR();
      /* point at M51 mode settings */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( m51_mode_ptr->change_m513_mode_func != NULL);

  return m51_mode_ptr->change_m513_mode_func(new_mode);
}


/*===========================================================================
=============================================================================
============================ UI INTERFACE BLOCK =============================
=============================================================================
===========================================================================*/


/* This interface block is needed until the new MC interface
** is in place, where CM becomes its own task and MC commands
** are sent directly to CM.
*/



/*===========================================================================

FUNCTION cm_get_pref_srv_type_for_packet_orig

DESCRIPTION
  This function returns service type on which the next packet data call is
  likely to be originated on.

DEPENDENCIES
  none

RETURN VALUE
  system mode.

SIDE EFFECTS
  none

===========================================================================*/
sys_sys_mode_e_type cm_get_pref_srv_type_for_packet_orig( void )
{

  sys_plmn_id_s_type          china_unicom_plmn;
  sys_plmn_id_s_type          china_mobile_plmn;
  cmss_s_type                *ss_ptr = cmss_ptr ();

  sys_sys_mode_e_type         acq_sys_mode;

  switch (cmph_get_ps_data_orig_prefs ())
  {
    case CM_PS_DATA_ORIG_PREF_CDMA_ONLY:
    case CM_PS_DATA_ORIG_PREF_CDMA_HDR:

      CM_MSG_HIGH ("PS Orig - Forcing to CDMA", 0, 0, 0);
      return SYS_SYS_MODE_CDMA;

    case CM_PS_DATA_ORIG_PREF_CDMA_HDR_IF_CHINA:
      /* Only force over to CDMA if ...
         plmn matches China &
         MS currently has GSM or WCDMA service &
         MS is CDMA capable
      */
      if (cmph_is_cdma_subscription_available ())
      {
        if (sd_misc_get_curr_acq_sys_mode () == SYS_SYS_MODE_GSM ||
            sd_misc_get_curr_acq_sys_mode () == SYS_SYS_MODE_WCDMA)
        {
          if (!sys_plmn_set_mcc_mnc (FALSE,
                                     CHINA_MOBILE_MCC,
                                     CHINA_MOBILE_MNC,
                                     &china_mobile_plmn))
          {
            CM_MSG_HIGH ("Unable to get China Mobile PLMN %d %d",
                         CHINA_MOBILE_MCC, CHINA_MOBILE_MNC, 0);
          }

          if (!sys_plmn_set_mcc_mnc (FALSE,
                                     CHINA_UNICOM_MCC,
                                     CHINA_UNICOM_MNC,
                                     &china_unicom_plmn))
          {
            CM_MSG_HIGH ("Unable to get China Unicom PLMN %d %d",
                         CHINA_UNICOM_MCC, CHINA_MOBILE_MNC, 0);
          }

          if ((sys_plmn_match (china_mobile_plmn, ss_ptr->info.sys_id.id.plmn))  ||
              (sys_plmn_match (china_unicom_plmn, ss_ptr->info.sys_id.id.plmn)))
          {
            CM_MSG_HIGH ("PS Orig - China plmn found, forcing over to CDMA", 0, 0, 0);
            acq_sys_mode = SYS_SYS_MODE_CDMA;
            return acq_sys_mode;
          }
        }
      }
      acq_sys_mode = sd_misc_get_curr_acq_sys_mode ();
      CM_MSG_HIGH ("PS Orig - Non China, using %d", acq_sys_mode, 0, 0);
      return acq_sys_mode;

    case CM_PS_DATA_ORIG_PREF_GW_ONLY:

        CM_MSG_HIGH ("PS Orig - Forcing over to GSM", 0, 0, 0);
        return SYS_SYS_MODE_GSM;

    case CM_PS_DATA_ORIG_PREF_WLAN_ONLY:

        CM_MSG_HIGH ("PS Orig - Forcing over to WLAN", 0, 0, 0);
        return SYS_SYS_MODE_WLAN;

  case CM_PS_DATA_ORIG_PREF_WLAN_LESS_PREF:
      
      acq_sys_mode = sd_misc_get_curr_acq_sys_mode();
      
      if(acq_sys_mode == SYS_SYS_MODE_GSM   ||
         acq_sys_mode == SYS_SYS_MODE_WCDMA ||
         acq_sys_mode == SYS_SYS_MODE_HDR   ||
         acq_sys_mode == SYS_SYS_MODE_CDMA)
      {
        CM_MSG_HIGH ("PS Orig - WLAN Less Pref, using %d", acq_sys_mode, 0, 0);
        return acq_sys_mode;
      }

      if(ss_ptr->info.wlan_srv_status == SYS_SRV_STATUS_SRV)
      {
        CM_MSG_HIGH ("PS Orig - WLAN Less Pref, No other, Forcing over to WLAN", 0, 0, 0);
        acq_sys_mode = SYS_SYS_MODE_WLAN;
        return acq_sys_mode;
      }

      CM_MSG_HIGH ("PS Orig - WLAN LESS PREF Using %d", acq_sys_mode, 0, 0);
      return acq_sys_mode;

    case CM_PS_DATA_ORIG_PREF_ANY:
    default:
      if( (ss_ptr->info.wlan_srv_status == SYS_SRV_STATUS_SRV) || 
    (cmph_ptr()->mode_pref == CM_MODE_PREF_WLAN_ONLY) )
      {
        CM_MSG_HIGH ("PS Orig - Any Pref,Forcing over to WLAN", 0, 0, 0);
        acq_sys_mode = SYS_SYS_MODE_WLAN;
        return acq_sys_mode;
      }
      acq_sys_mode = sd_misc_get_curr_acq_sys_mode ();
      CM_MSG_HIGH ("PS Orig - Any pref, using %d", acq_sys_mode, 0, 0);
      return acq_sys_mode;
  }

}


/*===========================================================================

FUNCTION cm_get_current_acq_sys_mode

DESCRIPTION
  This function returns the current system mode that is acquired.

DEPENDENCIES
  none

RETURN VALUE
  system mode.

SIDE EFFECTS
  none

===========================================================================*/
sys_sys_mode_e_type cm_get_current_acq_sys_mode( void )
{
  return sd_misc_get_curr_acq_sys_mode();
}


/*===========================================================================

FUNCTION cm_get_cur_nam

DESCRIPTION
  Returns the current NAM.

DEPENDENCIES
  Phone object must be initialized.

RETURNS
  The current NAM. If phone was not yet initialized, return CM_NAM_NONE.

SIDE EFFECTS
  None

===========================================================================*/
cm_nam_e_type  cm_get_curr_nam( void )
{
  return cmph_get_curr_nam();
}



/*===========================================================================

FUNCTION cm_num_fill

DESCRIPTION
  Initialize a CM number object and fill it with a number.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_num_fill(

    cm_num_s_type    *num_ptr,
        /* Pointer to a CM number to be initialized */

    const byte       *num_buf,
        /* Dialed address buffer.
        **
        ** NOTE! In the case where the digit_mode parameter is set to
        ** CM_DIGIT_MODE_4BIT_DTMF the number buffer may only contain
        ** 0-9, "#", or "*" characters.
        **
        ** In the case where the digit_mode parameter is set to
        ** CM_DIGIT_MODE_8BIT_ASCII the number buffer may contain any
        ** 8-bit ASCII characters */

    byte                   num_len,
        /* dialed address length */

    cm_digit_mode_e_type   digit_mode
        /* digit mode - see J-STD-008 2.7.1.3.2.4 */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( num_ptr != NULL );
  CM_ASSERT( num_len <= ARR_SIZE(num_ptr->buf));
  CM_ASSERT( BETWEEN(digit_mode,CM_DIGIT_MODE_NONE,CM_DIGIT_MODE_MAX ));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -e{774} always false only for when CM_DEBUG defined */
  if(num_ptr == NULL)
  {
    CM_ERR("num_ptr is NULL",0,0,0);
    return;
  }

  cm_num_init(num_ptr);

  if(num_len <= ARR_SIZE(num_ptr->buf))
  {
    /*lint -e{670} num_len is always less than num_buf */
    memcpy(num_ptr->buf, num_buf, num_len);
    num_ptr->len = num_len;
  }
  else
  {
    CM_ERR("Bad num_len = %d",num_len,0,0);
    num_ptr->len = 0;
  }

  num_ptr->digit_mode = digit_mode;
} /* cm_num_fill */



/*===========================================================================

FUNCTION cm_num_get

DESCRIPTION
  Copy a CM number into a specified byte buffer.

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  The numbers digits actually copied.

SIDE EFFECTS
  none

===========================================================================*/
byte cm_num_get(

    const cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number to copy from */

    byte*                  num_buf,
        /* Pointer to a byte buffer to copy CM number into */

    byte                   max_len
        /* Size of byte buffer */

)
{

  byte cpy_len = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL || max_len == 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If both pointers are not NULL, copy the number over.
  ** Make sure we do NOT copy more that the size of the buffer.
  */
  if( cm_num_ptr != NULL && num_buf != NULL ) /*lint !e774 */
  {
    cpy_len = MIN( max_len, cm_num_ptr->len );

    /* Buffer size should be greater than equal to actual size being copied
    */
    CM_ASSERT( max_len >= cpy_len );

    memcpy( num_buf, cm_num_ptr->buf, cpy_len );
  }

  return cpy_len;

} /* cm_num_get */


/*===========================================================================

FUNCTION cm_num_cmp

DESCRIPTION
  Compare a CM number byte with a byte buffer number over a specified
  length.

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  TRUE if both numbers match over the specified compare length,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_num_cmp(

    const cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number */

    const byte*             num_buf,
        /* Pointer to a byte buffer number */

    unsigned int           cmp_len
        /* Length to perform comparison over */
)
{

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL || cmp_len == 0 );
  CM_ASSERT( cmp_len <= ARR_SIZE(cm_num_ptr->buf) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If both pointers are not NULL, compare the numbers.
  */
  if( cm_num_ptr != NULL && num_buf != NULL ) /*lint !e774 */
  {
    /* Only compare if CM number is long enough.
    */
    if( cmp_len <= cm_num_ptr->len )
    {
      if( memcmp(cm_num_ptr->buf, num_buf, cmp_len) == 0 )
      {
        return TRUE;
      }
    }
  }

  return FALSE;
} /* cm_num_cmp */



/*===========================================================================

FUNCTION cm_num_cmp_exact

DESCRIPTION
  Compare a CM number byte with a byte buffer for an exact match -
  i.e. length and pattern.

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  TRUE if both numbers match exactly, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_num_cmp_exact(

    const cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number */

    const byte*            num_buf,
        /* Pointer to a byte buffer number */

    unsigned int           num_len
        /* Length off byte buffer number */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL || num_len == 0 );
  CM_ASSERT( num_len <= ARR_SIZE(cm_num_ptr->buf) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return BOOLEAN( (num_len == cm_num_ptr->len) &&
                  cm_num_cmp(cm_num_ptr, num_buf, num_len) );
} /* cm_num_cmp_exact */



/*===========================================================================

FUNCTION cm_num_copy

DESCRIPTION
  Copy a byte buffer number into a specified CM number.

DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_num_copy
(

    cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number */

    const byte*      num_buf,
        /* Pointer to a number to be copied into CM number */

    byte             num_len
        /* Length of number to be copied */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  byte max_len = ARR_SIZE( cm_num_ptr->buf );
      /* In any case do not copy more than the CM buffer size */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL );
  CM_ASSERT( num_len <= max_len );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -save -e774 */
  if((num_buf != NULL) && (num_len > 0))
  /*lint -restore */
  {

    cm_num_init( cm_num_ptr );

    /* Make sure we do NOT copy more that the max length.
    */
    cm_num_ptr->len = MIN( num_len, max_len );
    memcpy(cm_num_ptr->buf, num_buf, cm_num_ptr->len);
  }
  else
  {
    CM_ERR("Bad params to cm_num_copy()",0,0,0);
  }
} /* cm_num_copy */



/*===========================================================================

FUNCTION cm_num_ep_msg_copy

DESCRIPTION
  Copy an IS91 Extended Protocol Short Message into CM number.

DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_num_ep_msg_copy(

    cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number */

    const byte*       num_buf,
        /* Pointer to a number to be copied into CM number */

    byte             num_len
        /* Length of number to be copied */
)
{

  unsigned int i, j;
      /* loop counters */

  unsigned int max_len = ARR_SIZE( cm_num_ptr->buf );
      /* In any case do not copy more than the CM buffer size */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL || num_len == 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #define NAMPS_STX '\\'   /* Start-of-TX character */
  #define NAMPS_ETX 3      /* End-of_TX character */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( num_len > max_len )
  {
    CM_MSG_HIGH( "cm_num_ep_msg_copy - num_len > max_len",0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM number buffer.
  */
  cm_num_init( cm_num_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the IS91 Extended Protocol Short Message into
  ** the CM number as follows:
  ** 1. Strip any Start-of-TX characters.
  ** 2. Stop copying on End-of_TX character.
  ** 3. Make sure we do NOT copy more than the CM number buffer size.
  */
  for( i=0, j=0; i < MIN( num_len, max_len); i++ )
  {
    /* Stop if end of End-of_TX character.
    */
    if( num_buf[i] == NAMPS_ETX )
    {
      break;
    }

    /* Copy over if not Start-of-TX character.
    */
    if( num_buf[i] != NAMPS_STX )
    {
      cm_num_ptr->buf[j++] = num_buf[i];
    }
  }

  /* Set the CM number length appropriately.
  */
  cm_num_ptr->len = (byte) j;

} /* cm_num_ep_msg_copy */



/*===========================================================================

FUNCTION cm_num_ep_cli_copy

DESCRIPTION
  Copy IS91 Extended Protocol Caller Line ID into a CM number.

DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_num_ep_cli_copy(

    cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number */

    const byte*      num_buf,
        /* Pointer to a number to be copied into CM number */

    byte             num_len
        /* Length of number to be copied */
)
{

  unsigned int i;
      /* loop counter */

  unsigned int max_len = ARR_SIZE( cm_num_ptr->buf );
      /* In any case do not copy more than the CM buffer size */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL || num_len == 0 );
  CM_ASSERT( num_len <= max_len );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM number buffer.
  */
  cm_num_init( cm_num_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the IS91 Extended Protocol Short Message into
  ** the CM number as follows:
  ** 1. Replace none printable characters with space.
  ** 2. Stop copying on '\0' character.
  ** 3. Make sure we do NOT copy more than the CM number buffer size.
  */
  for( i=0; i < MIN( num_len, max_len); i++ )

  {
    /* Stop if end of message.
    */
    if( num_buf[i] == '\0' )
    {
      break;
    }

    /* If this is a non-printable control character,
    ** replace it with space, else, just copy the character.
    */
    if( BETWEEN(num_buf[i], 0, 0x20))
    {
      cm_num_ptr->buf[i] = ' ';
    }
    else
    {
      cm_num_ptr->buf[i] = num_buf[i];
    }
  }

  /* Set the CM number length appropriately.
  */
  cm_num_ptr->len = (byte) i;
} /* cm_num_ep_cli_copy */

#if( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

FUNCTION cm_num_attrib_copy

DESCRIPTION
  Set the attributes of a CM number to a specified set of attributes.

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_num_attrib_copy(

    cm_num_s_type           *num_ptr,
        /* Pointer to a CM number */

    byte                    number_type,
        /* Number type - see J-STD-008 2.7.1.3.2.4-2
        ** NOTE! ONLY VALID FOR CDMA CALLS */

    byte                    number_plan,
        /* Numbering Plan - see J-STD-008 2.7.1.3.2.4-3
        ** NOTE! ONLY VALID FOR CDMA CALLS */

    byte                    pi,
        /* Presentation Indicator - see J-STD-008  2.7.4.4-1 */

    byte                    si,
        /* Screening Indicator - see J-STD-008 2.7.4.4-2 */

    cm_digit_mode_e_type    digit_mode
        /* Origination digit mode - see J-STD-008 2.7.1.3.2.4 */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( num_ptr != NULL );

  CM_ASSERT( number_type <= CAI_ABB_NUM );

  CM_ASSERT( number_plan <= CAI_PRIV_PLAN );

  CM_ASSERT( pi <= CAI_NOT_AVAIL_PI );

  CM_ASSERT( si <= CAI_NET_SI );

  CM_ASSERT( digit_mode <= (CM_DIGIT_MODE_MAX-1) ); /*lint !e641 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  num_ptr->number_type   = number_type;
  num_ptr->number_plan   = number_plan;
  num_ptr->pi            = pi;
  num_ptr->si            = si;
  num_ptr->digit_mode    = digit_mode;

} /* cm_num_attrib_copy */
#endif // #if( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))



/*===========================================================================

FUNCTION cm_num_validate

DESCRIPTION
  This function validates a number.

DEPENDENCIES
  The number must have already been initialized with cm_num_init().

RETURN VALUE
  CM_NUM_ERR_NOERR is returned if the number is valid.  Otherwise, an
  appropriate error code is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_num_err_e_type cm_num_validate(

    const cm_num_s_type    *num_ptr
        /* Pointer to a CM number to be asserted */
)
{
  cm_num_err_e_type return_code = CM_NUM_ERR_NOERR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( num_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -save -e774 */
  if ( num_ptr == NULL )
  /*lint -restore */
  {
    return_code = CM_NUM_ERR_NULL_PTR;
  }
  else
  if ( num_ptr->len > ARR_SIZE(num_ptr->buf) )
  {
    return_code = CM_NUM_ERR_INVALID_NUM_LENGTH;
  }

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  #if ( !defined(FEATURE_GSM) && !defined(FEATURE_WCDMA) )
  else
  if ( num_ptr->number_type > (uint8)CM_NUM_TYPE_MAX )
  {
    return_code = CM_NUM_ERR_INVALID_NUM_TYPE;
  }
  else
  if ( num_ptr->number_plan > (uint8)CM_NUM_PLAN_MAX )
  {
    return_code = CM_NUM_ERR_INVALID_NUM_PLAN;
  }
  else
  if ( num_ptr->pi > CAI_NOT_AVAIL_PI )
  {
    return_code = CM_NUM_ERR_INVALID_PI;
  }
  else
  if ( num_ptr->si > CAI_NET_SI )
  {
    return_code = CM_NUM_ERR_INVALID_SI;
  }
  #endif /* ( !defined(FEATURE_GSM) && !defined(FEATURE_WCDMA) ) */
  #endif /* FEATURE_CDMA_800) || FEATURE_CDMA_1900 */

  return return_code;
} /* cm_num_validate() */

#if( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))


/*===========================================================================

FUNCTION cm_num_assert

DESCRIPTION
  Check a series of assertions over a CM number to validate its state.

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_num_assert(

    const cm_num_s_type    *num_ptr
        /* Pointer to a CM number to be asserted */
)
/*lint -esym(715, num_ptr) */
{

  CM_ASSERT( num_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( num_ptr->len <= ARR_SIZE(num_ptr->buf) );

  CM_ASSERT( num_ptr->number_type <= CAI_ABB_NUM );

  CM_ASSERT( num_ptr->number_plan <= CAI_PRIV_PLAN );

  CM_ASSERT( num_ptr->pi <= CAI_NOT_AVAIL_PI );

  CM_ASSERT( num_ptr->si <= CAI_NET_SI );
}
#endif // #if( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

FUNCTION cm_is_sys_mode_allowed

DESCRIPTION

  The function determines if the given sys_mode is included / conforms to
  given band and mode pref.

   For example, if the system mode is  SYS_SYS_MODE_GSM and  mode band
  preference is CM_MODE_PREF_GSM_WCDMA_ONLY / CM_BAND_PREF_GSM_850  the
  function returns TRUE but given SYS_SYS_MODE_GW it returns FALSE because
  CM_BAND_PREF_GSM_850 does not have WCDMA component.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If Valid
  FALSE - Otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_is_sys_mode_allowed(

    cm_mode_pref_e_type   mode_pref,
      /* The mode preference requested
      */

    cm_band_pref_e_type   band_pref,
      /* The band preference requested
      */

    sys_sys_mode_e_type   sys_mode
      /* The sys mode requested
      */
 )
{
  sd_ss_mode_pref_e_type sd_mode_pref  = SD_SS_MODE_PREF_NONE;

  CM_FUNC_START( "cm_is_sys_mode_allowed()",0,0,0 );
  CM_ASSERT( BETWEEN(mode_pref, CM_MODE_PREF_NONE, CM_MODE_PREF_MAX ));
  CM_ASSERT( BETWEEN(band_pref, CM_BAND_PREF_NONE, CM_BAND_PREF_MAX ));
  CM_ASSERT( BETWEEN(sys_mode , SYS_SYS_MODE_NONE, SYS_SYS_MODE_MAX ));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ----- - - - - -*/

  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref);

  if( sd_mode_pref == SD_SS_MODE_PREF_NONE )
  {
    CM_ERR("Mapping failed for %d! mode pref!", mode_pref ,0,0);
    return FALSE;
  }
  if(!(sd_misc_is_mode_pref((sd_ss_mode_pref_e_type)BM(sys_mode),sd_mode_pref)))
  {
    return FALSE;
  }

  return(sd_misc_is_supp_pref((sd_ss_mode_pref_e_type)BM(sys_mode),
         cmph_map_cm_band_pref_to_sd_band_pref(band_pref)));

}/* End function cm_is_sys_mode_allowed
 */

/*===========================================================================

FUNCTION cm_init_wlan_pref

DESCRIPTION
  Initialize WLAN preferences.

DEPENDENCIES

  cm_num_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_wlan_pref_init(

    cm_wlan_pref_s_type  *wlan_pref_ptr
        /* Pointer to a WLAN preference structure */
)
{

  CM_ASSERT( wlan_pref_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(wlan_pref_ptr == NULL)  /*lint !e774 */
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the memory
  */
  memset( wlan_pref_ptr, 0, sizeof(cm_wlan_pref_s_type) );
  wlan_pref_ptr->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;
  wlan_pref_ptr->scan_mode             = CM_WLAN_SCAN_PREF_NO_CHANGE;
  wlan_pref_ptr->tech_pref             = CM_WLAN_TECH_PREF_NO_CHANGE;
  wlan_pref_ptr->bss_type_pref         = CM_WLAN_BSS_TYPE_PREF_NO_CHANGE;
  wlan_pref_ptr->bss_type              = SYS_WLAN_BSS_TYPE_NONE;
  wlan_pref_ptr->ssid.len              = 0;
  wlan_pref_ptr->bss_id                = 0;
  wlan_pref_ptr->chan                  = NET_CHAN_WLAN_2400_ANY; /*lint !e641 */
  wlan_pref_ptr->band                  = SYS_BAND_MASK_EMPTY;
  wlan_pref_ptr->profile_id            = SYS_NW_PROFILE_ID_INVALID;
  wlan_pref_ptr->profile.len           = 0;
  wlan_pref_ptr->profile.profile_id    = SYS_NW_PROFILE_ID_INVALID;
  wlan_pref_ptr->profile.type          = NET_PROFILE_TYPE_NONE;  /*lint !e641 */
}

/*===========================================================================
FUNCTION cm_remove_mode_pref_components

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
cm_mode_pref_e_type cm_remove_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
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
  /*lint -e{64} -e{718} -e{746} -e{530} -e{516} */
  va_start(arg_list, number_of_components_to_remove);

  /*
  ** Process second and subsequent function arguments.
  ** These are the SD components to be removed from given
  ** CM mode pref
  */

  while(number_of_components_to_remove--)
  {

    /* Point at next function argument */
    /*lint -e{662} -e{826} -e{10} -e{718} -e{746} -e{718} */
    sd_mode_pref_to_remove = (sd_ss_mode_pref_e_type) va_arg(arg_list, int);

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
FUNCTION cm_add_mode_pref_components

DESCRIPTION

  Add given SD mode preference component(s) from given CM mode preference.

  Example:

  The below will add a given mode pref component into existing mode pref.

DEPENDENCIES
  sd_misc_add_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cm_add_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
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
  ** These are the SD components to be added to given
  ** CM mode pref
  */

  while(number_of_components_to_add--)
  {

    /* Point at next function argument */
    /*lint -e{10} -e{662} -e{826} */
    sd_mode_pref_to_add = (sd_ss_mode_pref_e_type) va_arg(arg_list, int);

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

FUNCTION cm_ph_net_list_copy

DESCRIPTION
  This function copies networks lists from PH event info to the client when
  requested bv client.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void cm_ph_net_list_copy
(
  cm_ph_net_lists_info_s_type   *info_ptr
)
{
  cmph_s_type                *ph_ptr  = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( info_ptr != NULL);
  CM_ASSERT( ph_ptr   != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(info_ptr == NULL)  /*lint !e774 */
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize lists.
  */

  info_ptr->gw_available_networks.length = 0;
  info_ptr->gw_user_pref_networks.length = 0;
  info_ptr->wlan_bss_list.num_bss        = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* More PH info is copied only when posting CMPH_EVENT.
  ** If a client attempts to access more PH info in any other
  ** circumtances, then no information will be copied.
  */
  if(! ph_ptr->is_event_process)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Network lists PH info is copied only when CM indicates that networks
  ** lists info is available.
  */
  if(! ph_ptr->is_net_lists_present)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy detailed information now.
  */

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif

  if(ph_ptr->wlan_bss_list.num_bss > 0 )
  {
    info_ptr->wlan_bss_list         = ph_ptr->wlan_bss_list;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
} /* cm_ph_net_list_copy() */

/*===========================================================================

FUNCTION cm_mbms_cmd_generic

DESCRIPTION
  API to be used by MBMS Apps to send commands to MBMS Service Manager.
  Clients should fill in the appropriate MBMS request type and also
  provide the corresponding parameters in the mbms_generic_req struct.
  Clients should check for CM_API_MBMS before using this function

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE   - Cmd has been queued to CM
  FALSE  - Cmd did not get queued to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mbms_cmd_generic (

  cm_mbms_cmd_cb_f_type                cmd_cb_func,
    /* client callback function */

  void                                 *data_block_ptr,
    /* pointer to client callback data block */

  const cm_client_id_type              client_id,
    /* originating client */

  const cm_mbms_generic_cmd_s_type    *mbms_generic_cmd
    /* Structure to hold MBMS commands and their parameters */

)
/*lint -esym(715,cmd_cb_func)*/
/*lint -esym(715,data_block_ptr)*/
/*lint -esym(715,client_id)*/
/*lint -esym(715,mbms_generic_cmd)*/
/*lint -esym(818, data_block_ptr) */

{
  #if defined (FEATURE_MODEM_MBMS)
#error code not present
  #else
  CM_ERR( "FEATURE_MODEM_MBMS is not defined ",0,0,0 );
  return FALSE;
  #endif /* #if defined (FEATURE_MODEM_MBMS) */
}
/*lint +esym(715,cmd_cb_func)*/
/*lint +esym(715,data_block_ptr)*/
/*lint +esym(715,client_id)*/
/*lint +esym(715,mbms_generic_cmd)*/
/*lint +esym(818, data_block_ptr) */
/*===========================================================================

FUNCTION cm_get_user_data

DESCRIPTION
  Call back client uses to get details regarding user data.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_get_user_data
(
  const cm_mm_call_info_s_type      *call_info_ptr,
    /* pointer to call info pointer sent to the clients */

  cm_call_event_user_data_s_type    *call_event_user_data
    /* pointer to a client before for user data details */
)
/*lint -save -e{818} call_event_user_data could be declared as pointing to const
*/
{
  #if ( defined FEATURE_UUS )
#error code not present
  #else
  SYS_ARG_NOT_USED(call_event_user_data);
  SYS_ARG_NOT_USED(call_info_ptr);
  #endif /* FEATURE_UUS */
  return;
}
/*lint -restore */


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
void cm_unlock_hdr_resources(void)
{

  #if defined(FEATURE_FULL_TIME_SHDR_3) && defined(FEATURE_HDR_HYBRID)
#error code not present
  #else

  CM_MSG_HIGH("FEATURE_FULL_TIME_SHDR_3 & FEATURE_HDR_HYBRID is undefined", 
              0,0,0);

  #endif

}

/*===========================================================================
@FUNCTION: cm_get_timing_advance  

@DESCRIPTION
  To get timing_advance information client has to allocate 
  memory before calling this API

@DEPENDENCIES
  None.

@RETURN VALUE
   TRUE :  Success
   FALSE:  Failure

@SIDE EFFECTS
  None
===========================================================================*/
boolean cm_get_timing_advance(

  uint32 *timing_advance_ptr 
    /* Timing_advance information */ 
)
{
  SYS_ARG_NOT_CONST(timing_advance_ptr);

  #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
#error code not present
  #else
  SYS_ARG_NOT_USED(timing_advance_ptr);
  CM_MSG_HIGH("FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE is undefined", 0,0,0);
  return FALSE;
  #endif  /* FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE */
}

/*===========================================================================
@FUNCTION: cm_get_cdma_position_info   

@DESCRIPTION
  To get UE current CDMA Cell position information  
  Client has to allocate memory before calling this API
@DEPENDENCIES
  None.

@RETURN VALUE
   TRUE : Success
   FALSE: Failure

@SIDE EFFECTS
  None
===========================================================================*/
boolean cm_get_cdma_position_info(

  cm_cdma_position_info_s_type   *position_info_ptr
   /* pointer to cdma position info sent to client */
)
{
  byte     base_class=0;       /* Base Station Class */
  byte     packet_zone_id=0;   /* Packet Zone Identification */
  uint16   mcc=0;              /* Mobile Country Code */
  SYS_ARG_NOT_CONST(position_info_ptr);

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  
  /* Sanity check on position_info_ptr */
  if ( position_info_ptr == NULL)
  {
    CM_ERR ("position_info_ptr is NULL", 0, 0, 0);
    return FALSE;
  }
  CM_MSG_MED (" CM returning CDMA postion information to client", 0, 0, 0);
  mccdma_get_cur_bs_info( &position_info_ptr->sid, 
                          &position_info_ptr->nid, 
                          &position_info_ptr->base_id, 
                          &base_class, 
                          &position_info_ptr->refpn,
                          &packet_zone_id, 
                          &mcc, 
                          &position_info_ptr->base_lat,
                          &position_info_ptr->base_long);
  return TRUE;
  #else
  SYS_ARG_NOT_USED(position_info_ptr);
  SYS_ARG_NOT_USED(base_class);
  SYS_ARG_NOT_USED(packet_zone_id);
  SYS_ARG_NOT_USED(mcc);
  CM_MSG_HIGH("FEATURE_CDMA_800 || FEATURE_CDMA_1900 is undefined", 0,0,0);
  return FALSE;
  #endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
}
