/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C A L L   M A N A G E R   D E B U G   M O D U L E

GENERAL DESCRIPTION
  This module contains the debug related functionality of call manager.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1991 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmdbg.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/09   aj      Adding support for passing otasp status to clients
05/06/09   aj      Added CM_PH_CMD_ERR_NO_SUBSCRIPTION_S
04/09/09   aj      Added support for IRAT measurements
02/12/09   mh      Added support for no SR after MAP for IS-707B
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
10/01/08   sv      Remove unnecessary string.h include to remove lint errors.
09/18/08   aj      Add support for UMA-GAN feature
09/16/08   pk      Added support for WLAN UOoS
09/12/08   fj/aj   Support Android RIL for ARM9 Reset
09/04/08   sv      Added WPS related command and event strings
08/14/08   rn      Send regn reject ind to clients
07/28/08   rk      Added error code to inform CM blocked mode pref change.
07/18/08   ks      Added new info for Current Operator's Cell ID and 
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
06/16/08   rk      Adding FEATURE_MM_SUPERSET to remaining feature flags
06/12/08   sv      Added SYS_ARGS_NOT_USED to remove compiler warnings.
06/17/08   sv      Added SYS_ARGS_NOT_USED to remove compiler warnings.
06/13/08   sv      Memory Reduction for ULC
05/22/08   rk      Added new event CM_CALL_EVENT_MPTY_CALL_IND 
05/22/08   rk      Added new event CM_CALL_EVENT_MPTY_CALL_IND
05/22/08   vk/sv   added functionality to handle roam only preference
05/06/08   ak      BCMCS 2.0 changes
05/02/08   vk      fixed messages for debug
03/14/08   ks      Updating UI with service status when UE is OOS in Connected Mode
02/05/08   rn      Added support for new command CM_PH_STATUS_CHGD_REQ
01/04/08   ka      Adding handset based plus code dial feature
12/16/07   ks      Added support for cm_ph_cmd_signal_strength_delta
12/13/07   sk/ak   Updated enum string maps.
12/07/07   ra      Added support for IMS VCC in HOMER files
10/15/07   rn      Added support for UE initiated dormancy
09/25/07   dm      Added support for FEATURE_MM_SUPERSET
08/17/07   ka/dm   New field to report HS call in progress
08/17/07   ka/dm   Adding HSDPA call indication enum
08/07/07   ic      Lint clean up
06/06/07   pk/clf  Added support for LN status notification from HDR
05/16/07   cl      Fix typo in include file AEEstd.h
04/15/07   rp      Removing FEATURE_MMGPS
04/03/07   sk      Added CM_PH_CMD_CLIENT_RELEASE to string map.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/27/07   pk      Changed va_start and va_end to AEEVA_START and AEEVA_END
02/27/07   ic      Fixed compiler error #167: argument of type "va_list" is
                   incompatible with parameter of type "AEEVaList"
02/23/07   pk      Banned API lint fixes
02/23/07   ic      Copy call_id for CM_SND_BRST_DTMF_ACK_F
01/19/07   pk      Reverting previous lint fixes
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
12/28/06   ka      Adding CM_HYBR_PREF_PERSISTENT.
11/17/06   jqi     Added CDMA connect Oder Ack support.
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
08/16/06   rp      Added CM_PH_CMD_ERR_AOC_NOT_READY_S to cm_ph_cmd_err_e_type_string_map
08/08/06   sk      Updated string map for cm_mode_pref_e_type.
05/17/06   pk      Added support for DDTM during BCMCS flows
05/03/06   pk      Lint Fixes
04/17/06   ka      Updating cm_name_gw_type_string_map
04/03/06   pk      Added support to print CM_CALL_END_ORIG_THR
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
03/15/06   sk      Fixed debug messages to print client type instead of
                   client id.
03/10/06   pk      Added support for high PSIST
03/09/06   pk      Added support for Power Collapse
03/03/06   pk      Adding string Maps for VoIP - 1X handoff
02/17/06   ka      Adding string maps for supporting videoshare
01/12/06   ic      Trimmed to minimum the list of lint errors that were
                   suppressed for va_list, va_start(), va_arg(), va_end()
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ka      Adding string maps for emergency num list
01/06/06   ic      Lint cleanup
12/02/05   ka      Updating cm_name_gw_type_string_map
12/05/05   ic      Mainlined FEATURE_LPM (left over from 2003 mainlining of
                   the same)
11/30/05   ka      Adding debug strings for HPLMN SRCH request.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/27/05   pk      Added support for CM_CALL_EVENT_CALL_ORIG_THR and
                   CM_SS_EVENT_THR_TBL_UPDATE
10/20/05   pk      Added support for CM_MODE_PREF_INTERSECT_OR_FORCE and
                   CM_SRV_TYPE_NON_AUTOMATIC
10/20/05   pk      Added support for CM_CALL_ORIG_ERR_ALL_ACCT_BLOCK
09/29/05   pk      Added functionality to print CM_PRIVACY_SETTING_F
08/19/05   pk      Added functionality to print CM_PD_EVENT_SESSION_DENIED
09/09/05   ic      Substitute OBSOLETE for CM_REORDER_F in
                   cm_name_1x_type_string_map to match change in cm_name_type
                   in cmll.h
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/22/05   ic      Obsoleted CM_REORDER_F.
                   Use CM_CALL_ORIG_FAIL_F instead with orig_fail.code set to
                   CM_CALL_ORIG_ERR_REORDER and call_ptr->end_status set to
                   CM_CALL_END_REORDER
08/02/05   ic      Print value that caused error in default switch case
06/30/05   pk      Removed feature flags from
                   CM_PH_EVENT_TERMINATE_GET_NETWORKS
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/10/05   ic      Lint clean up
06/06/05   sk      Renamed print_message() to cmdbg_print_message()
                   Added cm_dbm_cmd_e_type_string_map[]
                   Added cm_nas_cmd_e_type_string_map[]
                   Cleaned up and updated cmdbg_print_cm_cmd()
                   Renamed cmdbg_print_ll_rpt() to cmdbg_print_mc_rpt()
                   Renamed cmdbg_print_ll_cmd() to cmdbg_print_mc_cmd()
                   Cleaned up and updated cmdbg_print_mc_rpt()
                   Added cmdbg_print_sd_rpt()
                   Added cmdbg_print_gw_rpt()
                   Removed cmdbg_print_ccs_status()
                   Removed cm_ui_cmd_prt()
06/06/05   pk      Added functionality to print
                   CM_SS_ORIG_THR_CMD_UPDATE_TABLE
05/25/05   ic      Renamed CM_STRING_MAPS_ACTIVE to CMDEBUG_STRING_MAPS_ACTIVE
04/04/05   ka      Adding CM_CIPHER_IND to cm_name_gw_type_string_map
03/24/05   ic      Removed featurization from cm_call_cmd_e_type_string_map
03/14/05   ka      Adding new cmd cm_cmd_type_e_type_string_map for IP rpt
03/09/05   pk      Added functionality to print the CM_PH_EVENT_DDTM_STATUS
                   event
03/03/05   ka      Increasing MAX string Length
02/24/05   ic      Replaced CM__CALL with CM_CALL
02/14/05   pk      Added functionality to print the CM_CALL_EVENT_LINE_CTRL
                   event
02/01/05   dk      Added WLAN Support.
01/18/05   ic      Updated cm_name_1x_type_string_map[] - replaced
                   CM_CALL_END_CNF with CM_HDR_CALL_END_CNF_F
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
01/04/05   dk      Added enums for VT Call.
12/03/04   ws      Updated enums for remote WMS.
11/23/04   ic      Added CM_CALL_CMD_ERR_USER_DATA_LENGTH_P error code to
                   cm_call_cmd_err_e_type
11/19/04   ka      Adding support for user to user signaling
11/19/04   ic      Corrected typo in sys_sys_mode_e_type_string_map
11/16/04   ic      Removed unnecessary lint suppression comments, turned out
                   to be caused by multiline Lint comments in CM_ASSERT macro
                   expansion in cmdbg.h
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/11/04   ic      Lint cleanup
10/05/04   bpw     Fixed crash with print_message() and missing string arg.
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/14/04   ka      Added CM_SUPS_CMD_ERR_CALL_CONTROL_REJECTED to
                   cm_sups_cmd_err_e_type_string_map
09/10/04   ic      Fixed lint .\cm\cmdbg.c(460) : Error (Info) 785: Too few
                   initializers for aggregate
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
08/27/04   jqi     Added ps dormant optimization.
08/24/04   ws      Update to match sys.h changes.
08/12/04   ws      Remove cm20 support.
08/05/04   ic      Fixed RVCT 2.1 compiler errors (Saber, missing " at the
                   end of a string defined under FEATURE_UMTS_1X_HANDOVER_*
                   features).
                   Removed commas at the end of the last string in strings
                   arrays.
08/02/04   ic      CM Lint clean up - cmdbg.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/30/04   ws      Add strings for redirecting number events.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/12/04   ic      Updated cm_ph_cmd_prt()
06/22/04   ic      Comments clean up
06/21/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/16/04   ic      Removed CM_SMS_CMD_ERR_NOT_ALLOWED_IN_HDR_S from
                   cm_sms_cmd_err_e_type_string_map
06/10/04   dk      Added CM_CALL_EVENT_END_REQ in cm_call_event_prt().
05/14/04   ka      Fixed Lint errors.
04/29/04   ic      Fixes related to cm_mode_pref_e_type clean up.
04/13/04   ic      Featurization fixes
04/01/04   ic      Changed cmdbg_print_fav_mode_pref_for_origination() to take
                   5 parameters instead of a pointer to a structure that
                   contains those parameters.
                   cmdbg.c is shared across targets and the structure
                   cm_call_cmd_info_s_type became different for each target.
04/01/04   ws      Made print_fav_mode mode independent.
03/31/04   ic      Mainlined FEATURE_REX_DYNA_MEM in CM and unit test.
                   Removed all code related to static allocation of call
                   objects.
03/24/04   ic      Fixed featurization compile error.
                   Removed cmdbg_init()
03/05/04   ic      Fixed featurization compile error
03/02/04   ic      Fixed featurization compile error
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/27/03   ic      Mainlined FEATURE_DS_VOICE_AS_DATA
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/20/04   ka      Added support for multiple PDP and get call info list api.
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
11/03/03   ic      Removed printing of origination command fields in
                   cmdbg_print_cm_cmd() as it really did not belong there.
                   Added cmdbg_print_fav_mode_pref_for_origination().
10/27/03   ws      Removed uim pref slot selection enum
10/24/03   ws      Fixed featurization problem.
10/24/03   ic      Merge to support common cmdbg* files across targets
10/20/03   ws      Fixed misspelled available with several enums.
10/23/03   ic      Corrected typos, replaced UNAVIALABLE with UNAVAILABLE
                   Added cm_ss_cmd_prt(), cm_ss_event_prt(), cm_ss_cmd_err_prt(),
                   cmdbg_ss_info_print()
10/20/03   ic      Removed HDR-related commands from mc_name_type_string_map.
10/09/03   ic      Updates to *_string_map arrays to make them match what's in
                   cm.h, sys.h and mc.h.
10/07/03   vt      Updated cm_call_cmd_err_e_type strings.
10/06/03   vt      Modified to support common cm.h across all targets.
10/01/03   ws      Mainlined DS_VOICE_AS_DATA.
09/16/03   vt      Added preferred slot handling.
08/26/03   ws      Set the callid to none for otasp commit message.
09/05/03   ic      Updated cm_mode_pref_e_type_string_map array.
                   Added CM_PH_CMD_ERR_HYBR_PREF_S to cm_ph_cmd_err_prt().
                   Cleaned up cmdbg_ph_info_print().
08/27/03   ic      Changes to support HDR hybrid operation on Cougar.
08/13/03   vt      Added DDTM.
07/03/03   ic      Modified print_message() so it does not use static
                   buffer to print messages as this caused race conditions
                   where one task was overwriting the buffer before the
                   previous task was able to print the message.
05/28/03   vt      Added new service types.
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
04/28/03   AT      Lint cleanup.
04/11/03   prk     Removed references to FEATURE_SUPS.
02/27/03   ws      Updated cm_cmd_type_e list
02/18/03   vt      Fixed problems to build for HDR.
01/21/03   ws      Updated copyright information for 2003
11/27/02   vt      Fixed a compiler warning.
11/05/02   vt      Added changing RTRE configuration.
11/15/02   vt      Fixed compile errors for CM_CALL_FAIL_RETRY_ORDER_F report.
11/14/02   vt      Merged changes from GW mainline.
11/13/02   ic      More changes to monitor object interface - added
                   CM_MONIT_CMD_CLIENT_ACT command
11/13/02   ic      Changes to monitor object interface.
10/31/02   vt      Removed handling of NDSS origination failure.
                   Mainlined feature SD20.
10/30/02   vt      Undid - NDSS origination failure change.
10/28/02   vt      Removed handling of NDSS origination failure.
10/24/02   vt      Mainlined FEATURE_CM_USE_UMTS_REG_INTERFACE.
10/22/02   vt      Removed usage of is_allocated flag in call object.
09/05/02   vt      Fixed compile erros with usage of MSG_ERROR macro.
09/05/02   kar     Fixed compile errors
08/06/02   HQ      Added FEATURE_CDSMS.
05/05/02   vt      Fixed compile problem with new mm cm.h
03/25/02   ic      Added cm_call_orig_fail_e_type_string_map
03/06/02   ic      Replaced cm_call_cmd_e_string_map with
                   cm_call_cmd_e_type_string_map.
                   Added cm_inband_cmd_e_type_string_map.
                   Added cm_inband_cmd_err_e_type_string_map.
02/15/02   ic      Added cm_call_cmd_e_string_map array
02/11/02   ic      Made print_message() global
02/07/02   ic      Under FEATURE_IS2000_REL_A, added cm_flash_type_e_type_string_map
                   to be able to print flash type
01/29/02   ic      Removed the restriction of one voice + one data call
                   (under FEATURE_SVD_CM_CP_INTEGRATION)
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/20/01   ic      Under FEATURE_SVD_CM_CP_INTEGRATION, added support
                   for restricting the CM to one voice and one data call
                   at a time as this is what CP code currently supports.
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
11/17/00   RC      Added support for System Determination 2.0
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
12/08/98   RC      Changes per second week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/

/*lint -save -e656 -e641
** Turn off enum to int / operationuses compatible enums
*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "customer.h"
#include "cm.h"
#include "cmi.h"
#include "comdef.h"
#include "AEEstd.h"
#include "cmdbg.h"

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#include "cmxll.h"
#include "mc.h"
#endif

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

#ifdef CMDEBUG_STRING_MAPS_ACTIVE


#ifdef FEATURE_BCMCS
#error code not present
#endif

/* Provide strings for  values cm_ph_cmd_e_type */
static const char* cm_ph_cmd_e_type_string_map[ CM_PH_CMD_MAX ] = {
  "CM_PH_CMD_OPRT_MODE",
  "CM_PH_CMD_SYS_SEL_PREF",
  "CM_PH_CMD_ANSWER_VOICE",
  "CM_PH_CMD_NAM_SEL",
  "CM_PH_CMD_CLIENT_ACT",
  "CM_PH_CMD_INFO_GET",
  "CM_PH_CMD_RSSI_DELTA",
  "CM_PH_CMD_SUBSCRIPTION_AVAILABLE",
  "CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE",
  "CM_PH_CMD_SUBSCRIPTION_CHANGED",
  "CM_PH_CMD_GET_NETWORKS",
  "CM_PH_CMD_TERMINATE_GET_NETWORKS",
  "CM_PH_CMD_SET_PREFERRED_NETWORKS",
  "CM_PH_CMD_PACKET_STATE",
  "CM_PH_CMD_VOC_ACTIVITY",
  "CM_PH_CMD_AVOID_SYS",
  "CM_PH_CMD_WAKEUP_FROM_STANDBY",
  "CM_PH_CMD_CHANGE_RTRE_CONFIG",
  "CM_PH_CMD_DDTM_PREF",
  "CM_PH_CMD_RESET_ACM",
  "CM_PH_CMD_SET_ACMMAX",
  "CM_PH_CMD_LINE_SWITCHING",
  "CM_PH_CMD_SELECTED_LINE",
  "CM_PH_CMD_ALS_SIM_INFO_AVAIL",
  "CM_PH_CMD_LINE_SWITCHING_SAVED",
  "CM_PH_CMD_SELECTED_LINE_SAVED",
  "CM_PH_CMD_UPDATE_CDMA_CAPABILITY",
  "CM_PH_CMD_CLIENT_RELEASE",
  "CM_PH_CMD_SIGNAL_STRENGTH_DELTA",
  "CM_PH_CMD_WPS_START",
  "CM_PH_CMD_WPS_ABORT",
  "CM_PH_CMD_MEAS_MODE"
};


/* Provide strings for  values cm_call_cmd_e_type */
static const char* cm_call_cmd_e_type_string_map[ CM_CALL_CMD_MAX ] = {
  "CM_CALL_CMD_ORIG",
  "CM_CALL_CMD_ANSWER",
  "CM_CALL_CMD_END",
  "CM_CALL_CMD_SUPS",
  "CM_CALL_CMD_PRIVACY_PREF",
  "CM_CALL_CMD_INFO_GET",
  "CM_CALL_CMD_INFO_LIST_GET",
  "CM_CALL_CMD_SETUP_RES",
  "CM_CALL_CMD_PDP_ACT_RES",
  "CM_CALL_CMD_PDP_MODIFY",
  "CM_CALL_CMD_RAB_REESTAB",
  "CM_CALL_CMD_PS_DATA_AVAILABLE",
  "CM_CALL_CMD_CC_COMPLETE_REORIG",
  "CM_CALL_CMD_UMTS_CDMA_HANDOVER_REQ",
  "CM_CALL_CMD_UMTS_CDMA_HANDOVER_CNF",
  "CM_CALL_CMD_RECALL_RSP",
  "CM_CALL_CMD_MO_MODIFY_REQ",
  "CM_CALL_CMD_MO_MODIFY_RES",
  "CM_CALL_CMD_HANDOVER_REQ",
  "CM_CALL_CMD_PS_SIG_REL"
};


/* Provide strings for  values cm_inband_cmd_e_type */
static const char* cm_inband_cmd_e_type_string_map[ CM_INBAND_CMD_MAX ] = {
  "CM_INBAND_CMD_BURST_DTMF",
  "CM_INBAND_CMD_START_CONT_DTMF",
  "CM_INBAND_CMD_STOP_CONT_DTMF"
};


/* Provide strings for  values cm_ss_cmd_e_type */
static const char* cm_ss_cmd_e_type_string_map[ CM_SS_CMD_MAX ] = {
  "CM_SS_CMD_INFO_GET",
  "CM_SS_CMD_PS_DATA_AVAIL",
  "CM_SS_ORIG_THR_CMD_UPDATE_TABLE",
  "CM_SS_CMD_COUNTRY_SELECTED",
  "CM_SS_CMD_GET_MEASUREMENT"
};


/* Provide strings for  values cm_sms_cmd_e_type */
/*lint -esym(528, cm_sms_cmd_e_type_string_map)
** Not referenced for some configs
*/
static const char* cm_sms_cmd_e_type_string_map[ CM_SMS_CMD_MAX ] = {
  "CM_SMS_CMD_MO_MSG",
  "CM_SMS_CMD_AWI_ACK",
  "CM_SMS_CMD_BC_ENABLE"
} ;

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

/* Provide strings for  values cm_sups_cmd_e_type */
/*lint -esym(714, cm_sups_cmd_e_type_string_map) */
/*lint -esym(528, cm_sups_cmd_e_type_string_map)
** Not used in some configurations
*/
static const char* cm_sups_cmd_e_type_string_map[ CM_SUPS_CMD_MAX ] =
{

  "CM_SUPS_CMD_REGISTER",
  "CM_SUPS_CMD_ERASE",
  "CM_SUPS_CMD_ACTIVATE",
  "CM_SUPS_CMD_DEACTIVATE",
  "CM_SUPS_CMD_INTERROGATE",
  "CM_SUPS_CMD_PROCESS_USS",
  "CM_SUPS_CMD_REG_PASSWORD",
  "CM_SUPS_CMD_ABORT",
  "CM_SUPS_CMD_RELEASE",
  "CM_SUPS_CMD_GET_PASSWORD_RES",
  "CM_SUPS_CMD_USS_NOTIFY_RES",
  "CM_SUPS_CMD_USS_RES",

  "CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES",
  "CM_SUPS_CMD_LCS_MOLR",
  "CM_SUPS_CMD_LCS_MOLR_COMPLETED",

  "CM_SUPS_CMD_ERASE_CC_ENTRY"

};


/* Provide strings for  values cm_sups_cmd_e_type */
#ifdef FEATURE_DBM
static const char* cm_dbm_cmd_e_type_string_map[ CM_DBM_CMD_MAX ] = {

  "CM_DBM_CMD_SEND_MSG"

};
#endif

#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
#endif

/*
** <<CM strings
*/

/* Provide strings for cm_call_event_e_type string_map values */
static const char* cm_call_event_e_type_string_map[ CM_CALL_EVENT_MAX ] = {

        "CM_CALL_EVENT_ORIG",
        "CM_CALL_EVENT_ANSWER",
        "CM_CALL_EVENT_END_REQ",
        "CM_CALL_EVENT_END",
        "CM_CALL_EVENT_SUPS",
        "CM_CALL_EVENT_INCOM",
        "CM_CALL_EVENT_CONNECT",
        "CM_CALL_EVENT_SRV_OPT",
        "CM_CALL_EVENT_PRIVACY",
        "CM_CALL_EVENT_PRIVACY_PREF",
        "CM_CALL_EVENT_CALLER_ID",
        "CM_CALL_EVENT_ABRV_ALERT",
        "CM_CALL_EVENT_ABRV_REORDER",
        "CM_CALL_EVENT_ABRV_INTERCEPT",
        "CM_CALL_EVENT_SIGNAL",
        "CM_CALL_EVENT_DISPLAY",
        "CM_CALL_EVENT_CALLED_PARTY",
        "CM_CALL_EVENT_CONNECTED_NUM",
        "CM_CALL_EVENT_INFO",
        "CM_CALL_EVENT_EXT_DISP",
        "CM_CALL_EVENT_NDSS_START",
        "CM_CALL_EVENT_NDSS_CONNECT",
        "CM_CALL_EVENT_EXT_BRST_INTL",
        "CM_CALL_EVENT_NSS_CLIR_REC",
        "CM_CALL_EVENT_NSS_REL_REC",
        "CM_CALL_EVENT_NSS_AUD_CTRL",
        "CM_CALL_EVENT_L2ACK_CALL_HOLD",
        "CM_CALL_EVENT_SETUP_IND",
        "CM_CALL_EVENT_SETUP_RES",
        "CM_CALL_EVENT_CALL_CONF",
        "CM_CALL_EVENT_PDP_ACTIVATE_IND",
        "CM_CALL_EVENT_PDP_ACTIVATE_RES",
        "CM_CALL_EVENT_PDP_MODIFY_REQ",
        "CM_CALL_EVENT_PDP_MODIFY_IND",
        "CM_CALL_EVENT_PDP_MODIFY_REJ",
        "CM_CALL_EVENT_PDP_MODIFY_CONF",
        "CM_CALL_EVENT_RAB_REL_IND",
        "CM_CALL_EVENT_RAB_REESTAB_IND",
        "CM_CALL_EVENT_RAB_REESTAB_REQ",
        "CM_CALL_EVENT_RAB_REESTAB_CONF",
        "CM_CALL_EVENT_RAB_REESTAB_REJ",
        "CM_CALL_EVENT_RAB_REESTAB_FAIL",
        "CM_CALL_EVENT_PS_DATA_AVAILABLE",
        "CM_CALL_EVENT_MNG_CALLS_CONF",
        "CM_CALL_EVENT_CALL_BARRED",
        "CM_CALL_EVENT_CALL_IS_WAITING",
        "CM_CALL_EVENT_CALL_ON_HOLD",
        "CM_CALL_EVENT_CALL_RETRIEVED",
        "CM_CALL_EVENT_ORIG_FWD_STATUS",
        "CM_CALL_EVENT_CALL_FORWARDED",
        "CM_CALL_EVENT_CALL_BEING_FORWARDED",
        "CM_CALL_EVENT_INCOM_FWD_CALL",
        "CM_CALL_EVENT_CALL_RESTRICTED",
        "CM_CALL_EVENT_CUG_INFO_RECEIVED",
        "CM_CALL_EVENT_CNAP_INFO_RECEIVED",
        "CM_CALL_EVENT_EMERGENCY_FLASHED",
        "CM_CALL_EVENT_PROGRESS_INFO_IND",
        "CM_CALL_EVENT_CALL_DEFLECTION",
        "CM_CALL_EVENT_TRANSFERRED_CALL",
        "CM_CALL_EVENT_EXIT_TC",
        "CM_CALL_EVENT_REDIRECTING_NUMBER",
        "CM_CALL_EVENT_PDP_PROMOTE_IND",
        "CM_CALL_EVENT_UMTS_CDMA_HANDOVER_START",
        "CM_CALL_EVENT_UMTS_CDMA_HANDOVER_END",
        "CM_CALL_EVENT_SECONDARY_MSM",
        "CM_CALL_EVENT_ORIG_MOD_TO_SS",
        "CM_CALL_EVENT_USER_DATA_IND",
        "CM_CALL_EVENT_USER_DATA_CONG_IND",
        "CM_CALL_EVENT_MODIFY_IND",
        "CM_CALL_EVENT_MODIFY_REQ",
        "CM_CALL_EVENT_LINE_CTRL",
        "CM_CALL_EVENT_CCBS_ALLOWED",
        "CM_CALL_EVENT_ACT_CCBS_CNF",
        "CM_CALL_EVENT_CCBS_RECALL_IND",
        "CM_CALL_EVENT_CCBS_RECALL_RSP",
        "CM_CALL_EVENT_CALL_ORIG_THR",
        "CM_CALL_EVENT_VS_AVAIL",
        "CM_CALL_EVENT_VS_NOT_AVAIL",
        "CM_CALL_EVENT_MODIFY_COMPLETE_CONF",
        "CM_CALL_EVENT_MODIFY_RES",
        "CM_CALL_EVENT_CONNECT_ORDER_ACK",
        "CM_CALL_EVENT_TUNNEL_MSG",
        "CM_CALL_EVENT_END_VOIP_CALL",
        "CM_CALL_EVENT_VOIP_CALL_END_CNF",
        "CM_CALL_EVENT_PS_SIG_REL_REQ",
        "CM_CALL_EVENT_PS_SIG_REL_CNF",
        "CM_CALL_EVENT_HANDOFF_CALL_ORIG",
        "CM_CALL_EVENT_HANDOFF_CALL_CONNECT",
        "CM_CALL_EVENT_HANDOFF_CALL_END",
        "CM_CALL_EVENT_HANDOFF_MANUAL_CALL_ORIG",
        "CM_CALL_EVENT_MPTY_CALL_IND",
        "CM_CALL_EVENT_OTASP_STATUS"
  };

/* Provide strings for cm_ph_event_e_type string_map values */
static const char* cm_ph_event_e_type_string_map[ CM_PH_EVENT_MAX ] = {

  "CM_PH_EVENT_OPRT_MODE",
  "CM_PH_EVENT_TEST_CONTROL_TYPE",
  "CM_PH_EVENT_SYS_SEL_PREF",
  "CM_PH_EVENT_ANSWER_VOICE",
  "CM_PH_EVENT_NAM_SEL",
  "CM_PH_EVENT_CURR_NAM",
  "CM_PH_EVENT_IN_USE_STATE",
  "CM_PH_EVENT_CDMA_LOCK_MODE",
  "CM_PH_EVENT_UZ_CHANGED",
  "CM_PH_EVENT_MAINTREQ",
  "CM_PH_EVENT_STANDBY_SLEEP",
  "CM_PH_EVENT_STANDBY_WAKE",
  "CM_PH_EVENT_INFO",
  "CM_PH_EVENT_PACKET_STATE",
  "CM_PH_EVENT_INFO_AVAIL",
  "CM_PH_EVENT_SUBSCRIPTION_AVAILABLE",
  "CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE",
  "CM_PH_EVENT_SUBSCRIPTION_CHANGED",
  "CM_PH_EVENT_AVAILABLE_NETWORKS_CONF",
  "CM_PH_EVENT_PREFERRED_NETWORKS_CONF",
  "CM_PH_EVENT_FUNDS_LOW",
  "CM_PH_EVENT_WAKEUP_FROM_STANDBY",
  "CM_PH_EVENT_NVRUIM_CONFIG_CHANGED",
  "CM_PH_EVENT_PREFERRED_NETWORKS_SET",
  "CM_PH_EVENT_DDTM_PREF",
  "CM_PH_EVENT_PS_ATTACH_FAILED",
  "CM_PH_EVENT_RESET_ACM_COMPLETED",
  "CM_PH_EVENT_SET_ACMMAX_COMPLETED",
  "CM_PH_EVENT_CDMA_CAPABILITY_UPDATED",
  "CM_PH_EVENT_LINE_SWITCHING",
  "CM_PH_EVENT_SELECTED_LINE",
  "CM_PH_EVENT_SECONDARY_MSM",
  "CM_PH_EVENT_TERMINATE_GET_NETWORKS",
  "CM_PH_EVENT_DDTM_STATUS",
  "CM_PH_EVENT_CCBS_STORE_INFO_CHANGED",
  "CM_PH_EVENT_WPS_COMPLETED",
  "CM_PH_EVENT_WPS_ABORTED",
  "CM_PH_EVENT_WPS_START",
  "CM_PH_EVENT_MEAS_MODE"
};


/* Provide strings for cm_inband_event_e_type string_map values */
static const char* cm_inband_event_e_type_string_map[ CM_INBAND_EVENT_MAX ] = {
  "CM_INBAND_EVENT_REV_BURST_DTMF",
  "CM_INBAND_EVENT_REV_START_CONT_DTMF",
  "CM_INBAND_EVENT_REV_START_CONT_DTMF_CONF",
  "CM_INBAND_EVENT_REV_STOP_CONT_DTMF",
  "CM_INBAND_EVENT_REV_STOP_CONT_DTMF_CONF",
  "CM_INBAND_EVENT_FWD_BURST_DTMF",
  "CM_INBAND_EVENT_FWD_START_CONT_DTMF",
  "CM_INBAND_EVENT_FWD_STOP_CONT_DTMF",
  "CM_INBAND_EVENT_REV_BURST_DTMF_ACK"
};


/* Provide strings for cm_ss_event_e_type string_map values */
static const char* cm_ss_event_e_type_string_map[ CM_SS_EVENT_MAX ] = {

  "CM_SS_EVENT_SRV_CHANGED",
  "CM_SS_EVENT_RSSI",
  "CM_SS_EVENT_INFO",
  "CM_SS_EVENT_REG_SUCCESS",
  "CM_SS_EVENT_REG_FAILURE",
  "CM_SS_EVENT_HDR_RSSI",
  "CM_SS_EVENT_WLAN_RSSI",
  "CM_SS_EVENT_SRV_NEW",
  "CM_SS_EVENT_SECONDARY_MSM",
  "CM_SS_EVENT_PS_DATA_AVAIL",
  "CM_SS_EVENT_PS_DATA_FAIL",
  "CM_SS_EVENT_PS_DATA_SUCCESS",
  "CM_SS_EVENT_WLAN_STATS",
  "CM_SS_EVENT_THR_TBL_UPDATE",
  "CM_SS_EVENT_EMERG_NUM_LIST",
  "CM_SS_EVENT_IPAPP_REG_STATUS",
  "CM_SS_EVENT_GW_RSSI",
  "CM_SS_EVENT_HDR_LN_STATUS",
  "CM_SS_EVENT_SELECT_COUNTRY",
  "CM_SS_EVENT_COUNTRY_SELECTED",
  "CM_SS_EVENT_REG_REJECT",
  "CM_SS_EVENT_MEAS_RESPONSE"
};


/* Provide strings for cm_sms_event_e_type string_map values */
/*lint -esym(528, cm_sms_event_e_type_string_map)
** Not referenced for some configs
*/
static const char* cm_sms_event_e_type_string_map[ CM_SMS_EVENT_MAX ] = {
  "CM_SMS_EVENT_MO_MSG",
  "CM_SMS_EVENT_AWI_ACK",
  "CM_SMS_EVENT_MO_STATUS",
  "CM_SMS_EVENT_MT_MSG",
  "CM_SMS_EVENT_BC_ENABLED",
  "CM_SMS_EVENT_MSG_WAITING",
  "CM_SMS_EVENT_N_VOICE"
};

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

/* Provide strings for cm_sups_event_e_type string_map values */
/*lint -esym(714, cm_sups_event_e_type_string_map) */
/*lint -esym(528, cm_sups_event_e_type_string_map)
** Not used in some configurations
*/
static const char* cm_sups_event_e_type_string_map[ CM_SUPS_EVENT_MAX ] =
{

"CM_SUPS_EVENT_REGISTER",
"CM_SUPS_EVENT_REGISTER_CONF",
"CM_SUPS_EVENT_ERASE",
"CM_SUPS_EVENT_ERASE_CONF",
"CM_SUPS_EVENT_ACTIVATE",
"CM_SUPS_EVENT_ACTIVATE_CONF",
"CM_SUPS_EVENT_DEACTIVATE",
"CM_SUPS_EVENT_DEACTIVATE_CONF",
"CM_SUPS_EVENT_INTERROGATE",
"CM_SUPS_EVENT_INTERROGATE_CONF",
"CM_SUPS_EVENT_REG_PASSWORD",
"CM_SUPS_EVENT_REG_PASSWORD_CONF",
"CM_SUPS_EVENT_PROCESS_USS",
"CM_SUPS_EVENT_PROCESS_USS_CONF",
"CM_SUPS_EVENT_RELEASE",
"CM_SUPS_EVENT_ABORT",
"CM_SUPS_EVENT_FWD_CHECK_IND",
"CM_SUPS_EVENT_USS_NOTIFY_IND",
"CM_SUPS_EVENT_USS_NOTIFY_RES",
"CM_SUPS_EVENT_USS_IND",
"CM_SUPS_EVENT_USS_RES",
"CM_SUPS_EVENT_RELEASE_USS_IND",
"CM_SUPS_EVENT_GET_PASSWORD_IND",
"CM_SUPS_EVENT_GET_PASSWORD_RES",

"CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_IND",
"CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_RES",
"CM_SUPS_EVENT_LCS_MOLR",
"CM_SUPS_EVENT_LCS_MOLR_COMPLETED",
"CM_SUPS_EVENT_LCS_MOLR_CONF",

"CM_SUPS_EVENT_SS_MOD_TO_ORIG"
};

#ifdef FEATURE_BCMCS
#error code not present
#endif /* FEATURE_BCMCS */

/* Provide strings for cm_call_cmd_err_e_type values */
static const char* cm_call_cmd_err_e_type_string_map[ CM_CALL_CMD_ERR_MAX ] = {

  "CM_CALL_CMD_ERR_NOERR",
  "CM_CALL_CMD_ERR_CLIENT_ID_P",
  "CM_CALL_CMD_ERR_CALL_ID_P",
  "CM_CALL_CMD_ERR_NUM_CALL_IDS_P",
  "CM_CALL_CMD_ERR_CALL_TYPE_P",
  "CM_CALL_CMD_ERR_SRV_TYPE_P",
  "CM_CALL_CMD_ERR_SRV_OPT_P",
  "CM_CALL_CMD_ERR_MODE_PREF_P",
  "CM_CALL_CMD_ERR_NO_NUM_P",
  "CM_CALL_CMD_ERR_NULL_NUM_P",
  "CM_CALL_CMD_ERR_NUM_P",
  "CM_CALL_CMD_ERR_NUM_LEN_P",
  "CM_CALL_CMD_ERR_NULL_ALPHA_P",
  "CM_CALL_CMD_ERR_ALPHA_LEN_P",
  "CM_CALL_CMD_ERR_MISMATCH_P",
  "CM_CALL_CMD_ERR_NULL_PARAM_P",
  "CM_CALL_CMD_ERR_ACTIVATE_CODE_P",
  "CM_CALL_CMD_ERR_CALL_SUPS_P",
  "CM_CALL_CMD_ERR_QOS_LEN_P",
  "CM_CALL_CMD_ERR_LAST_ACT_DATA_NET_P",
  "CM_CALL_CMD_ERR_INVALID_PRIMARY_CALL_ID_P",
  "CM_CALL_CMD_ERR_INVALID_GROUP_ID_P",
  "CM_CALL_CMD_ERR_PROFILE_NUMBER_IN_USE_P",
  "CM_CALL_CMD_ERR_UMTS_CDMA_HANDOVER_REQ_P",
  "CM_CALL_CMD_ERR_UMTS_CDMA_HANDOVER_CNF_P",
  "CM_CALL_CMD_ERR_OFFLINE_S",
  "CM_CALL_CMD_ERR_CDMA_LOCK_S",
  "CM_CALL_CMD_ERR_FLASH_S",
  "CM_CALL_CMD_ERR_ORIG_RES_S",
  "CM_CALL_CMD_ERR_NO_FUNDS_S",
  "CM_CALL_CMD_ERR_EMERGENCY_MODE_S",
  "CM_CALL_CMD_ERR_IN_USE_S",
  "CM_CALL_CMD_ERR_SRV_TYPE_S",
  "CM_CALL_CMD_ERR_CALL_TYPE_S",
  "CM_CALL_CMD_ERR_CALL_STATE_S",
  "CM_CALL_CMD_ERR_SRV_STATE_S",
  "CM_CALL_CMD_ERR_NO_SO_S",
  "CM_CALL_CMD_ERR_ALERT_ANS_BYPASS_S",
  "CM_CALL_CMD_ERR_UIM_NOT_PRESENT_S",
  "CM_CALL_CMD_ERR_BAD_PRIVACY_PREF_S",
  "CM_CALL_CMD_ERR_NO_CALL_ID_S",
  "CM_CALL_CMD_ERR_CALL_ORIG_FAIL_S",
  "CM_CALL_CMD_ERR_INVALID_MODE_S",
  "CM_CALL_CMD_ERR_INVALID_SIM_STATE_S",
  "CM_CALL_CMD_ERR_MAX_PRIMARY_PDP_CONTEXTS_S",
  "CM_CALL_CMD_ERR_MAX_SEC_PDP_CONTEXT_PER_PRI_S",
  "CM_CALL_CMD_ERR_NO_COLLOC_HDR",
  "CM_CALL_CMD_ERR_NO_BUF_L",
  "CM_CALL_CMD_ERR_MC_COM_L",
  "CM_CALL_CMD_ERR_OTHER",
  "CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE",
  "CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED",
  "CM_CALL_CMD_ERR_FAVORED_MODE_PREF_FOR_ORIGINATION",
  "CM_CALL_CMD_ERR_CONNECTION_ID_OUT_L",
  "CM_CALL_CMD_ERR_SUPS_SRV_ON_PROHIBITED_LINE_S",
  "CM_CALL_CMD_ERR_USER_DATA_LENGTH_P",
  "CM_CALL_CMD_ERR_NON_NEGOT_BEARER_P",
  "CM_CALL_CMD_ERR_INVALID_BC_P"
};


/* Provide strings for cm_ph_cmd_err_e_type values */
static const char* cm_ph_cmd_err_e_type_string_map[ CM_PH_CMD_ERR_MAX ] = {
  "CM_PH_CMD_ERR_NOERR",
  "CM_PH_CMD_ERR_ANSWER_VOICE_F",
  "CM_PH_CMD_ERR_CLIENT_ID_P",
  "CM_PH_CMD_ERR_OPRT_MODE_P",
  "CM_PH_CMD_ERR_MODE_PREF_P",
  "CM_PH_CMD_ERR_PREF_TERM_P",
  "CM_PH_CMD_ERR_SYS_PREF_P",
  "CM_PH_CMD_ERR_BAND_PREF_P",
  "CM_PH_CMD_ERR_ROAM_PREF_P",
  "CM_PH_CMD_ERR_ANSWER_VOICE_P",
  "CM_PH_CMD_ERR_ANSWER_DURATION_P",
  "CM_PH_CMD_ERR_NAM_P",
  "CM_PH_CMD_ERR_PACKET_STATE_P",
  "CM_PH_CMD_ERR_HYBR_PREF_P",
  "CM_PH_CMD_ERR_SERVICE_DOMAIN_P",
  "CM_PH_CMD_ERR_ACQ_ORDER_PREF_P",
  "CM_PH_CMD_ERR_NETWORK_LIST_TYPE_P",
  "CM_PH_CMD_ERR_NETWORK_SEL_MODE_P",
  "CM_PH_CMD_ERR_SYSTEM_MODE_P",
  "CM_PH_CMD_ERR_PLMN_P",
  "CM_PH_CMD_ERR_RTRE_CONFIG_P",
  "CM_PH_CMD_ERR_CDMA_CAPABILITY_P",
  "CM_PH_CMD_ERR_PRL_PREF_P",
  "CM_PH_CMD_ERR_WLAN_PREF_P",
  "CM_PH_CMD_ERR_IN_USE_S",
  "CM_PH_CMD_ERR_ONLINE_S",
  "CM_PH_CMD_ERR_OPRT_MODE_S",
  "CM_PH_CMD_ERR_MODE_PREF_S",
  "CM_PH_CMD_ERR_SYS_PREF_S",
  "CM_PH_CMD_ERR_SYS_SEL_PREF_S",
  "CM_PH_CMD_ERR_HYBR_PREF_S",
  "CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S",
  "CM_PH_CMD_ERR_GET_NETWORKS_S",
  "CM_PH_CMD_ERR_NO_BUF_L",
  "CM_PH_CMD_ERR_MC_COM_L",
  "CM_PH_CMD_ERR_AVOID_SYS_P",
  "CM_PH_CMD_ERR_SYS_NOT_IN_UZ",
  "CM_PH_CMD_ERR_OTHER",
  "CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE",
  "CM_PH_CMD_ERR_CURRENT_NAM_IS_NOT_RUIM_NAM_S",
  "CM_PH_CMD_ERR_USER_PREFERRED_NETWORK_LIST_ACCESS",
  "CM_PH_CMD_ERR_DDTM_PREF_P",
  "CM_PH_CMD_ERR_DDTM_NUM_SRV_OPTIONS_P",
  "CM_PH_CMD_ERR_LINE_SWITCHING_P",
  "CM_PH_CMD_ERR_LINE_SWITCHING_S",
  "CM_PH_CMD_ERR_SELECTED_LINE_P",
  "CM_PH_CMD_ERR_ALS_NOT_SUPPORTED_S",
  "CM_PH_CMD_ERR_ALS_SIM_WRITE_FAILED_S",
  "CM_PH_CMD_ERR_AOC_NOT_READY_S",
  "CM_PH_CMD_ERR_NET_TYPE_P",
  "CM_PH_CMD_ERR_DDTM_SRV_OPTIONS_LIST_P",
  "CM_PH_CMD_ERR_INVALID_HYBR_PREF_MODE_PREF",
  "CM_PH_CMD_ERR_MODE_PREF_CHANGE_NOT_ALLOWED_S",
  "CM_PH_CMD_ERR_INVALID_WPS_MODE",
  "CM_PH_CMD_ERR_INVALID_WPS_CONFIG_METHOD",
  "CM_PH_CMD_ERR_INVALID_WPS_DEV_PASSWD",
  "CM_PH_CMD_ERR_NO_WPS_IN_PROGRESS",
  "CM_PH_CMD_ERR_WPS_ALREADY_IN_PROGRESS",
  "CM_PH_CMD_ERR_NO_SUBSCRIPTION_S"
};


/* Provide strings for cm_inband_cmd_err_e_type values */
static const char* cm_inband_cmd_err_e_type_string_map[ CM_INBAND_CMD_ERR_MAX ] = {
  "CM_INBAND_CMD_ERR_NOERR",
  "CM_INBAND_CMD_ERR_CLIENT_ID_P",
  "CM_INBAND_CMD_ERR_ON_LEN_P",
  "CM_INBAND_CMD_ERR_OFF_LEN_P",
  "CM_INBAND_CMD_ERR_NULL_BUF_P",
  "CM_INBAND_CMD_ERR_DIGIT_P",
  "CM_INBAND_CMD_ERR_CNT_P",
  "CM_INBAND_CMD_ERR_CALL_ID_P",
  "CM_INBAND_CMD_ERR_BAD_STATE_S",
  "CM_INBAND_CMD_ERR_OFFLINE_S",
  "CM_INBAND_CMD_ERR_NO_BUF_L",
  "CM_INBAND_CMD_ERR_OTHER",
  "CM_INBAND_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE"
};


/* Provide strings for cm_ss_cmd_err_e_type values */
static const char* cm_ss_cmd_err_e_type_string_map[ CM_SS_CMD_ERR_MAX ] = {
  "CM_SS_CMD_ERR_NOERR",
  "CM_SS_CMD_ERR_CLIENT_ID_P",
  "CM_SS_CMD_ERR_NO_BUF_L",
  "CM_SS_CMD_ERR_OTHER",
  "CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE",
  "CM_SS_CMD_ERR_OFFLINE_S",
  "CM_SS_CMD_ERR_SRV_STATE_S",
  "CM_SS_CMD_ERR_PS_DATA_SUSPEND_S",
  "CM_SS_CMD_ERR_INCORRECT_MODE_S",
  "CM_SS_CMD_ERR_BAD_PARM_P",
};


/* Provide strings for cm_sms_cmd_err_e_type values */
/*lint -esym(528, cm_sms_cmd_err_e_type_string_map)
** Not referenced for some configs
*/
static const char* cm_sms_cmd_err_e_type_string_map[ CM_SMS_CMD_ERR_MAX ] = {
  "CM_SMS_CMD_ERR_NOERR",
  "CM_SMS_CMD_ERR_CLIENT_ID_P",
  "CM_SMS_CMD_ERR_NOT_IDLE_S",
  "CM_SMS_CMD_ERR_OFFLINE_S",
  "CM_SMS_CMD_ERR_CDMA_LOCK_S",
  "CM_SMS_CMD_ERR_CALL_ORIG_S",
  "CM_SMS_CMD_ERR_EMERGENCY_MODE_S",
  "CM_SMS_CMD_ERR_NO_SRV_S",
  "CM_SMS_CMD_ERR_NOT_ALLOWED_IN_AMPS_S",
  "CM_SMS_CMD_ERR_NOT_ANALOG_S",
  "CM_SMS_CMD_ERR_NO_BUF_L",
  "CM_SMS_CMD_ERR_OTHER",
  "CM_SMS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE",
  "CM_SMS_CMD_ERR_DDTM",
  "CM_SMS_CMD_ERR_MODE_PREF",
  "CM_SMS_CMD_ERR_SIP"
};

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

/* Provide strings for cm_sups_cmd_err_e_type values */
/*lint -esym(714, cm_sups_cmd_err_e_type_string_map) */
/*lint -esym(528, cm_sups_cmd_err_e_type_string_map)
** Not used in some configurations
*/
static const char* cm_sups_cmd_err_e_type_string_map[ CM_SUPS_CMD_ERR_MAX ] = {
  "CM_SUPS_CMD_ERR_NOERR",
  "CM_SUPS_CMD_ERR_CLIENT_ID_P",
  "CM_SUPS_CMD_ERR_NULL_PARAM_P",
  "CM_SUPS_CMD_ERR_OFFLINE_S",
  "CM_SUPS_CMD_ERR_SRV_STATE_S",
  "CM_SUPS_CMD_ERR_NO_BUF_L",
  "CM_SUPS_CMD_ERR_OTHER",
  "CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE",
  "CM_SUPS_CMD_ERR_CALL_CONTROL_FAILURE",
  "CM_SUPS_CMD_ERR_CALL_CONTROL_REJECTED"
} ;


/*
** CM<< strings
*/

/* Provide strings for cm_name_type values */
static const char* cm_name_sd_type_string_map [(CM_RPT_SD_END - CM_RPT_SD_BEGIN) - 1] =
{
  /*
  ** Reports from SD20
  */
  "CM_SRV_IND_INFO_F",
  "CM_ACQ_FAIL_F",
  "CM_OK_TO_ORIG_F",
  "CM_NAM_CHANGED_F",
  "CM_E911_CALLBACK_F",
  "CM_STANDBY_SLEEP_F",
  "CM_STANDBY_WAKE_F",
  "CM_LPM_OK_F",
  "CM_WLAN_BSS_LIST_F",
  "CM_IP_APP_REG_STATUS",
  "CM_WLAN_WPS_STATUS_F",
  "CM_SRV_LOST_F"
};

/* Provide strings for cm_name_type values */
static const char* cm_name_generic_type_string_map [(CM_RPT_GENERIC_END - CM_RPT_GENERIC_BEGIN) - 1] =
{
  /*
  ** Reports from DEM
  */
  "CM_DEM_RPT",
  "CM_WLAN_RPT"
};


/*lint -esym(528, cm_name_1x_type_string_map)
** Not referenced for some configs
*/
static const char* cm_name_1x_type_string_map [(CM_RPT_1X_END - CM_RPT_1X_BEGIN) - 1] =
{
  "CM_ABRV_ALERT_F",
  "CM_ABRV_REORDER_F",
  "CM_ABRV_INTERCEPT_F",
  "CM_ALERT_F",
  "CM_INCOMING_CALL_F",
  "CM_STOP_ALERT_F",
  "OBSOLETE",
  "CM_INTERCEPT_F",
  "CM_FLASH_F",
  "CM_DTMF_BURST_F",
  "CM_DTMF_CONT_START_F",
  "CM_DTMF_CONT_STOP_F",
  "CM_CALL_RELEASE_F",
  "CM_CALL_FADE_F",
  "CM_MARKOV_F",
  "CM_CALL_CONNECTED_F",
  "CM_DISPLAY_F",
  "CM_CALLED_PARTY_F",
  "CM_CALLING_PARTY_F",
  "CM_CONNECTED_NUM_F",
  "CM_SIGNAL_F",
  "CM_UPCALL_F",
  "CM_LOCK_F",
  "CM_UNLOCK_F",
  "CM_MAINTREQ_F",
  "CM_N_VC_MSG_F",
  "CM_N_VC_CLI_F",
  "CM_OTASP_STATUS_F",
  "CM_OTASP_COMMIT_STATUS_F",
  "CM_VOICE_AS_DATA_F",
  "CM_LINE_CTRL_F",
  "CM_SRV_SYS_INFO_F",
  "CM_CALL_ORIG_FAIL_F",
  "CM_MO_DBM_STATUS_F",
  "CM_MT_DBM_DATA_F",
  "CM_MSG_WAITING_F",
  "CM_N_VOICE_F",
  "CM_N_MSG_F",
  "CM_N_CLI_F",
  "CM_EXT_DISPLAY_F",
  "CM_NDSS_REDIRECTION_F",
  "CM_HDR_CALL_ORIG_FAIL_F",
  "CM_HDR_INCOMING_CALL_F",
  "CM_HDR_CALL_RELEASE_F",
  "CM_HDR_CALL_FADE_F",
  "CM_HDR_CALL_CONNECTED_F",
  "CM_HDR_CONNECTION_DENY_F",
  "CM_HDR_REDIRECT_F",
  "CM_HDR_PRL_NOT_PREFERRED_F",
  "CM_HDR_DEACTIVATE_ACK_F",
  "CM_HDR_RESELECT_EXIT_HDR_F",
  "CM_HDR_BCMCS_FLOW_STATUS_F",
  "CM_HDR_BCMCS_BROADCAST_RESET_F",
  "CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F",
  "CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F",
  "CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F",
  "CM_IDLE_PGSLOT_CHANGED_F",
  "CM_GPS_MODE_GRANTED_F",
  "CM_GPS_MODE_EXIT_F",
  "CM_EXT_BRST_INTL_F",
  "CM_NSS_CLIR_F",
  "CM_NSS_REL_F",
  "CM_NSS_AUD_CTRL_F",
  "CM_SND_BRST_DTMF_ACK_F",
  "CM_CALL_HOLD_L2ACK_F",
  "CM_REG_SUCCESS_F",
  "CM_REG_FAILURE_F",
  "CM_CALL_FAIL_RETRY_ORDER_F",
  "CM_CALL_EMERGENCY_FLASHED_F",
  "CM_EXIT_TC_F",
  "CM_BS_STATUS_RSP_F",
  "CM_REDIRECTING_NUMBER_F",
  "CM_CDMA_CAPABILITY_UPDATE_F",
  "CM_UMTS_CDMA_HANDOVER_STATUS_F",
  "CM_HDR_CALL_END_CNF_F",
  "CM_PRIVACY_SETTING_F",
  "CM_TUNNEL_MSG_F",
  "CM_HDR_TUNNEL_MSG_F",
  "CM_PSIST_F",
  "CM_HDR_BCMCS_FLOW_MONITORING_START_F",
  "CM_HDR_BCMCS_FLOW_MONITORING_END_F",
  "CM_CDMA_CONNECT_ORDER_ACK_F",
  "CM_END_VOIP_CALL_F",
  "CM_GET_CSNA_MSG_CONFIG_F",
  "CM_UPDATE_CSNA_MSG_CONFIG_F",
  "CM_TUNNEL_MSG_STATUS_F",
  "CM_HDR_LOCATION_NOTIFICATION_STATUS_F",
  "CM_HDR_SESSION_OPEN_FAILED",
  "CM_HDR_SESSION_CLOSED",
  "CM_HDR_BCMCS_REG_STATUS_F",
  "CM_MMGSDI_REFRESH_FCN_IND_F",
  "CM_HDR_MEAS_MODE_F",
  "CM_HDR_MEAS_RESP_F"
};


/*lint -esym(714, cm_name_gw_type_string_map) */
/*lint -esym(528, cm_name_gw_type_string_map)
** Not used in some configurations
*/
/*lint -save -e834 */
static const char* cm_name_gw_type_string_map [CM_RPT_GW_END - CM_RPT_GW_BEGIN - 1] =
/*lint -restore */
{
  "CM_MO_CALL_CONF",
  "CM_MT_CALL_IND",
  "CM_CALL_PROGRESS_INFO_IND",
  "CM_CALL_CONNECTED_IND",
  "CM_CONNECTED_NUMBER_IND",
  "CM_MT_END_CALL_IND",
  "CM_MT_USER_DATA_IND",
  "CM_MT_SETUP_IND",
  "CM_MT_MODIFY_IND",
  "CM_MT_DISC_IND",
  "CM_START_DTMF_CONF",
  "CM_STOP_DTMF_CONF",
  "CM_MANAGE_CALLS_CONF",
  "CM_MODIFY_COMPLETE_CONF",
  "CM_ABORT_CC_CONF",
  "CM_ACTIVATE_SS_CONF",
  "CM_DEACTIVATE_SS_CONF",
  "CM_ERASE_SS_CONF",
  "CM_INTERROGATE_SS_CONF",
  "CM_REGISTER_PASSWORD_CONF",
  "CM_REGISTER_SS_CONF",
  "CM_PROCESS_UNSTRUCTURED_SS_CONF",
  "CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF",
  "CM_MT_CALL_SS_IND",
  "CM_FORWARD_CHECK_SS_INDICATION_IND",
  "CM_UNSTRUCTURED_SS_NOTIFY_IND",
  "CM_UNSTRUCTURED_SS_IND",
  "CM_RELEASE_UNSTRUCTURED_SS_IND",
  "CM_CONNECTION_STATE_IND",
  "CM_GET_PASSWORD_IND",

  "CM_LCS_LOCATION_NOTIFICATION_IND",
  "CM_LCS_MOLR_CONF",

  "CM_LOAD_DATA_CNF",
  "CM_MM_INFORMATION_IND",
  "CM_NETWORK_LIST_CNF",
  "CM_SAVE_DATA_CNF",
  "CM_SERVICE_CNF",
  "CM_SERVICE_IND",
  "CM_SIM_AVAILABLE_CNF",
  "CM_SIM_NOT_AVAILABLE_CNF",
  "CM_STOP_MODE_CNF",
  "CM_DEEP_SLEEP_IND",
  "CM_RR_STATUS_IND",
  "CM_RRC_STATUS_IND",
  "CM_PDP_ACTIVATE_CNF",
  "CM_PDP_ACTIVATE_REJ",
  "CM_PDP_ACTIVATE_IND",
  "CM_PDP_DEACTIVATE_CNF",
  "CM_PDP_DEACTIVATE_IND",
  "CM_PDP_DEACTIVATE_REJ",
  "CM_PDP_MODIFY_IND",
  "CM_PDP_MODIFY_CNF",
  "CM_PDP_MODIFY_REJ",
  "CM_PDP_ACTIVATE_SEC_CNF",
  "CM_PDP_ACTIVATE_SEC_REJ",
  "CM_PDP_ACTIVATE_REJ_RSP",
  "CM_PS_SIG_CON_REL_IND",
  "CM_RAB_RELEASE_IND",
  "CM_RAB_REESTABLISH_IND",
  "CM_RAB_REESTAB_CNF",
  "CM_RAB_REESTAB_REJ",
  "CM_RAB_REESTAB_FAIL",
  "CM_PDP_UNKNOWN_REQ",
  "CM_MT_DATA_CONNECT_IND",
  "CM_CHANGE_TEST_MODE_IND",
  "CM_SIM_STATE_CHGD_F",
  "CM_SI_NETWORK_LIST_F",
  "CM_GSDI_CARD_STATUS_IND",
  "CM_GSDI_REFRESH_FCN_IND",
  "CM_DATA_SUSPEND_CHGD_F",
  "CM_CS_SIG_REL_IND",
  "CM_CAMPED_IND",
  "CM_UMTS_CDMA_HANDOVER_REQ",
  "CM_PDP_DEACTIVATE_PROMOTE_IND",
  "CM_PDP_DEACTIVATE_PROMOTE_CNF",
  "CM_USER_CONGESTION_IND",
  "CM_CB_DATA_IND",
  "CM_CB_CELL_CHANGE_IND_F",
  "CM_CB_DUPLICATION_DETECTION_IND_F",
  "CM_SM_DATA_IND_F",
  "CM_SM_REPORT_IND_F",
  "CM_SM_LINK_CONTROL_IND_F",
  "CM_CIPHER_IND",
  "CM_5_SEND_CONF",
  "CM_ERASE_CC_ENTRY_CONF",
  "CM_CCBS_INFO_IND",
  "CM_RECALL_IND",
  "CM_HPLMN_SEARCH_CNF",
  "CM_MM_PS_DATA_SUSPEND_IND",
  "CM_EMERGENCY_NUM_LIST_IND",
  "CM_CELL_SERVICES_IND",
  "CM_HSPA_CALL_STATUS_IND",
  "CM_PS_SIG_CON_REL_CNF",
  "CM_PH_STATUS_CHGD_CNF",
  "CM_CONN_MODE_SERVICE_STATUS_IND",
  "CM_RR_CELL_INFO_IND",
  "CM_RRC_CELL_INFO_IND",
  "CM_MMGSDI_CARD_STATUS_IND",
  "CM_MMGSDI_REFRESH_FCN_IND",
  "CM_RPT_MBMS_ACT_CNF",
  "CM_RPT_MBMS_DEACT_CNF",
  "CM_RPT_MBMS_ACT_IND",
  "CM_RPT_MBMS_GENERIC",
  "CM_REG_REJECT_IND",
  "CM_CONN_MODE_NO_SERVICE_WAKEUP_IND",
  "CM_GAN_INFO_IND"
 };
/*lint -restore */


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
/* Provide strings for cm_call_orig_fail_e_type values */
static const char* cm_call_orig_fail_e_type_string_map [ CM_CALL_ORIG_ERR_MAX ] = {
  "CM_CALL_ORIG_ERR_HOLD_ORIG",
  "CM_CALL_ORIG_ERR_NO_SRV",
  "CM_CALL_ORIG_ERR_ACC_FAIL",
  "CM_CALL_ORIG_ERR_ABORT",
  "CM_CALL_ORIG_ERR_REDIR",
  "CM_CALL_ORIG_ERR_HANDOFF",
  "CM_CALL_ORIG_ACC_IN_PROG",
  "CM_CALL_ORIG_ERR_CDMA_LOCK",
  "CM_CALL_ORIG_ERR_CDMA_PSIST_NG",
  "CM_CALL_ORIG_ERR_TIMEOUT",
  "CM_CALL_ORIG_NO_SESSION",
  "CM_CALL_ORIG_ERR_INCOMPATIBLE",
  "CM_CALL_ORIG_ERR_CCS_NOT_SUPPORTED",
  "CM_CALL_ORIG_ERR_NO_RESPONSE_FROM_BS",
  "CM_CALL_ORIG_ERR_REJECTED_BY_BS",
  "CM_CALL_ORIG_ERR_ACCT_BLOCK",
  "CM_CALL_ORIG_ERR_ALREADY_IN_TC",
  "CM_CALL_ORIG_ERR_HDR_NO_LOCK_GRANTED",
  "CM_CALL_ORIG_ERR_REORDER",
  "CM_CALL_ORIG_ERR_ALL_ACCT_BLOCK",
  "CM_CALL_ORIG_ERR_MAX_ACC"
};
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

/* Provide strings for cm_call_state_e_type values */
static const char* cm_call_state_e_type_string_map[ CM_CALL_STATE_MAX ] = {
  "CM_CALL_STATE_IDLE",
  "CM_CALL_STATE_ORIG",
  "CM_CALL_STATE_INCOM",
  "CM_CALL_STATE_CONV",
  "CM_CALL_STATE_CC_IN_PROGRESS",
  "CM_CALL_STATE_RECALL_RSP_PEND"
};


/*
** CM>> strings
*/

/* Provide strings for mc_name_type values */
/*lint -esym(528, mc_name_type_string_map)
** Not referenced for some configs
*/
static const char* mc_name_type_string_map [  ] = {
  "MC_ONLINE_F",
  "MC_ORIGINATION_F",
  "MC_ANSWER_F",
  "MC_FLASH_F",
  "MC_END_F",
  "MC_DATA_END_F",
  "MC_START_CONT_DTMF_F",
  "MC_STOP_CONT_DTMF_F",
  "MC_BURST_DTMF_F",
  "MC_MO_DBM_F",
  "MC_PRIVACY_PREF_F",
  "MC_PREF_SYS_CHANGED_F",
  "MC_NAM_CHANGED_F",
  "MC_OFFLINE_DIGITAL_F",
  "MC_OFFLINE_ANALOG_F",
  "MC_RESET_F",
  "MC_POWERDOWN_F",
  "MC_JUMP_TRAFFIC_F",
  "MC_JUMP_ACCESS_F",
  "MC_SECTOR_LOCK_F",
  "MC_LPM_F",
  "MC_AWI_SMS_ACK_F",
  "MC_BCSMS_ENABLE_F",
  "MC_SILENT_REDIAL_F",
  "MC_RES_REQ_F",
  "MC_HOLD_F",
  "MC_SS_TIMER_EXP_F",
  "MC_RES_REL_F",
  "MC_DDTM_CNTL_F",
  "MC_CDMA_PROT_ACT_CMD_F",
  "MC_CDMA_PROT_DEACT_CMD_F",
  "MC_CDMA_PROT_PH_STAT_CHGD_CMD_F",
  "MC_CDMA_PROT_GEN_CMD_F",
  "MC_AMPS_PROT_ACT_CMD_F ",
  "MC_AMPS_PROT_DEACT_CMD_F",
  "MC_AMPS_PROT_PH_STAT_CHGD_CMD_F",
  "MC_AMPS_PROT_GEN_CMD_F",
  "MC_BSSTATUS_REQ_F",
  "MC_UMTS_CDMA_HANDOVER_REQ_F"
};


#ifdef FEATURE_IS2000_REL_A
/* Provide strings for cm_call_sups_type_e_type values */
static const char* cm_call_sups_type_e_type_string_map [ CM_CALL_SUPS_TYPE_MAX ] = {
  "CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB",
  "CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL",
  "CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL",
  "CM_CALL_SUPS_TYPE_HOLD_CALL",
  "CM_CALL_SUPS_TYPE_SELECT_CALL_X",
  "CM_CALL_SUPS_TYPE_MULTIPARTY_CALL",
  "CM_CALL_SUPS_TYPE_CALL_DEFLECTION",
  "CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER",
  "CM_CALL_SUPS_TYPE_CALL_FORWARD",
  "CM_CALL_SUPS_TYPE_EMERGENCY_CALL",
  "CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD",
  "CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD",
  "CM_CALL_SUPS_TYPE_SIMPLE_FLASH",
  "CM_CALL_SUPS_TYPE_ACT_CCBS_REQ"
} ;
#endif /* FEATURE_IS2000_REL_A */


/*
** =CM= strings
*/

/* Provide strings for cm_call_type_e_type values */
/*lint -esym(765,cm_call_type_e_type_string_map)
** Can't be static, CM unit test uses it
*/
const char* cm_call_type_e_type_string_map[ CM_CALL_TYPE_MAX ] = {
  "CM_CALL_TYPE_VOICE",
  "CM_CALL_TYPE_CS_DATA",
  "CM_CALL_TYPE_PS_DATA",
  "CM_CALL_TYPE_SMS",
  "CM_CALL_TYPE_PD",
  "CM_CALL_TYPE_TEST",
  "CM_CALL_TYPE_OTAPA",
  "CM_CALL_TYPE_STD_OTASP",
  "CM_CALL_TYPE_NON_STD_OTASP",
  "CM_CALL_TYPE_EMERGENCY",
  "CM_CALL_TYPE_SUPS",
  "CM_CALL_TYPE_VT",
  "CM_CALL_TYPE_VT_LOOPBACK",
  "CM_CALL_TYPE_VS",
  "CM_CALL_TYPE_PS_DATA_IS707B"  
};


/* Provide strings for cm_srv_type_e_type values */
/*lint -esym(765,cm_srv_type_e_type_string_map)
** Can't be static, used by CM unit test
*/
const char* cm_srv_type_e_type_string_map[ CM_SRV_TYPE_MAX ] = {
  "CM_SRV_TYPE_AMPS",
  "CM_SRV_TYPE_HDR",
  "CM_SRV_TYPE_CDMA_SPECIFIC",
  "CM_SRV_TYPE_CDMA_AUTOMATIC",
  "CM_SRV_TYPE_AUTOMATIC",
  "CM_SRV_TYPE_CDMA_HDR",
  "CM_SRV_TYPE_GSM_WCDMA",
  "CM_SRV_TYPE_WCDMA",
  "CM_SRV_TYPE_GSM",
  "CM_SRV_TYPE_WLAN",
  "CM_SRV_TYPE_NON_AUTOMATIC"
};

/* Provide strings for cm_hybr_pref_e_type values */
/*lint -esym(765,cm_hybr_pref_e_type_string_map)
** Can't be static, used by CM unit test
*/
const char* cm_hybr_pref_e_type_string_map[ CM_HYBR_PREF_MAX ] = {
  "CM_HYBR_PREF_OFF",
  "CM_HYBR_PREF_ON",
  "CM_HYBR_PREF_NO_CHANGE",
  "CM_HYBR_PREF_PERSISTENT",
  "CM_HYBR_PREF_CDMA__HDR_WCDMA",
  "CM_HYBR_PREF_CDMA__WCDMA"
};

/* Provide strings for cm_mode_pref_e_type values */
/*lint -esym(765,cm_mode_pref_e_type_string_map)
** Can't be static, used by CM unit test
*/
const char* cm_mode_pref_e_type_string_map[ CM_MODE_PREF_MAX ] = {
  "CM_MODE_PREF_AMPS_ONLY",
  "CM_MODE_PREF_DIGITAL_ONLY",
  "CM_MODE_PREF_AUTOMATIC",
  "CM_MODE_PREF_EMERGENCY",
  "CM_MODE_PREF_DUMMY_04",
  "CM_MODE_PREF_DUMMY_05",
  "CM_MODE_PREF_DUMMY_06",
  "CM_MODE_PREF_DUMMY_07",
  "CM_MODE_PREF_DUMMY_08",
  "CM_MODE_PREF_CDMA_ONLY",
  "CM_MODE_PREF_HDR_ONLY",
  "CM_MODE_PREF_CDMA_AMPS_ONLY",
  "CM_MODE_PREF_GPS_ONLY",
  "CM_MODE_PREF_GSM_ONLY",
  "CM_MODE_PREF_WCDMA_ONLY",
  "CM_MODE_PREF_PERSISTENT",
  "CM_MODE_PREF_NO_CHANGE",
  "CM_MODE_PREF_ANY_BUT_HDR",
  "CM_MODE_PREF_CURRENT_LESS_HDR",
  "CM_MODE_PREF_GSM_WCDMA_ONLY",
  "CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY",
  "CM_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS",
  "CM_MODE_PREF_CDMA_HDR_ONLY",
  "CM_MODE_PREF_CDMA_AMPS_HDR_ONLY",
  "CM_MODE_PREF_CURRENT_LESS_AMPS",
  "CM_MODE_PREF_WLAN_ONLY",
  "CM_MODE_PREF_CDMA_WLAN",
  "CM_MODE_PREF_HDR_WLAN",
  "CM_MODE_PREF_CDMA_HDR_WLAN",
  "CM_MODE_PREF_GSM_WLAN",
  "CM_MODE_PREF_WCDMA_WLAN",
  "CM_MODE_PREF_GW_WLAN",
  "CM_MODE_PREF_CURRENT_PLUS_WLAN",
  "CM_MODE_PREF_CURRENT_LESS_WLAN",
  "CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY",
  "CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY",
  "CM_MODE_PREF_INTERSECT_OR_FORCE",
  "CM_MODE_PREF_ANY_BUT_HDR_WLAN"
} ;

/* Provide strings for sd_ss_e_type_type values */
/*lint -esym(528, sd_ss_e_type_string_map)
** Not used in some configurations
*/
static const char* sd_ss_e_type_string_map[ SD_SS_MAX + 1 ] = /*lint !e641 */
{

  "SD_SS_MAIN",
  "SD_SS_HYBR_HDR",
  "SD_SS_HYBR_WLAN",
  "SD_SS_MAX"
};

/* Provide strings for sys_sys_mode_e_type values */
/*lint -esym(714, sys_sys_mode_e_type_string_map)
** Not used in some configurations
*/
/*lint -esym(765,sys_sys_mode_e_type_string_map)
** Can't be static, used by CM unit test
*/
const char* sys_sys_mode_e_type_string_map[ SYS_SYS_MODE_MAX ] = {

  "SYS_SYS_MODE_NO_SRV",
  "SYS_SYS_MODE_AMPS",
  "SYS_SYS_MODE_CDMA",
  "SYS_SYS_MODE_GSM",
  "SYS_SYS_MODE_HDR",
  "SYS_SYS_MODE_WCDMA",
  "SYS_SYS_MODE_GPS",
  "SYS_SYS_MODE_GW",
  "SYS_SYS_MODE_WLAN"
};

#endif /* CMDEBUG_STRING_MAPS_ACTIVE */



/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmdbg_print_message

DESCRIPTION

  Behaves as printf() - takes the very same format string and any number of
  parameters and allows to print <stuff> to diagnostic monitor output
  screen.

 The problem was that %s in the format string does not work for
 MSG_HIGH (and other MSG_ macros) plus those macros support only
 maximum of 3 arguments, so this function was provided to remedy both problems.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
#define CM_MAX_STRING_LENGTH 300

void cmdbg_print_message
(
  const char *string,
  ...
)
{
  AEEVaList arg_ptr;
  char      temp_str[ CM_MAX_STRING_LENGTH + 1 ] = { 0 };

  /*lint -e{516} -e{64} -e{718} -e{530}*/
  AEEVA_START( arg_ptr, string );
  (void)std_vstrlprintf( temp_str, (int)sizeof(temp_str), string, arg_ptr );
  AEEVA_END( arg_ptr );

  MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "%s", temp_str);

} /* cmdbg_print_message() */

/*===========================================================================

FUNCTION CMDBG_PRINT_CALL

DESCRIPTION
  Print the call object

===========================================================================*/
void cmdbg_print_call(const cmcall_s_type *call_ptr)
{




  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmdbg_print_message("Call object's fields:");
  cmdbg_print_message("- init_mask = %d", call_ptr->init_mask);
  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */


  #ifdef CMDEBUG_STRING_MAPS_ACTIVE
  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif  /* FEATURE_HDR_HYBRID */

  cmdbg_print_message("- favored_mode_pref = %d(%s)",
                      call_ptr->favored_mode_pref,
                      cm_mode_pref_e_type_string_map[call_ptr->favored_mode_pref]);

  cmdbg_print_message("- call_state = %d(%s)",
                      call_ptr->call_state,
                      cm_call_state_e_type_string_map[ call_ptr->call_state]);

  cmdbg_print_message("- call_type = %d(%s)",
                      call_ptr->call_type,
                      cm_call_type_e_type_string_map[ call_ptr->call_type ]);

  cmdbg_print_message("- srv_type = %d(%s)",
                      call_ptr->srv_type,
                      cm_srv_type_e_type_string_map[call_ptr->srv_type]);
  #else
  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif  /* FEATURE_HDR_HYBRID */
  cmdbg_print_message("- favored_mode_pref = %d", call_ptr->favored_mode_pref);
  cmdbg_print_message("- call_state = %d", call_ptr->call_state);
  cmdbg_print_message("- call_type = %d", call_ptr->call_type );
  cmdbg_print_message("- srv_type = %d", call_ptr->srv_type );
  #endif /* FEATURE_HDR_HYBRID */




}

/*===========================================================================

FUNCTION CMDBG_PRINT_CM_CMD

DESCRIPTION
  Print the command that was received from a client.

===========================================================================*/
void cmdbg_print_cm_cmd
(
  const cm_cmd_s_type  *cmd_ptr                       /* pointer to a CM command */
)
{




  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( CMD_TYPE(cmd_ptr) )
  {
  #ifdef FEATURE_BCMCS
#error code not present
  #endif

  case CM_CMD_TYPE_PH:
    CMDBG_CMD_MSG( "phcmd", cmd_ptr->cmd.ph.cmd,
                   cm_ph_cmd_e_type_string_map,
                   cmd_ptr->cmd.ph.client_id );
    break;

  case CM_CMD_TYPE_CALL:
    CMDBG_CMD_MSG( "callcmd", cmd_ptr->cmd.call.cmd,
                   cm_call_cmd_e_type_string_map,
                   cmd_ptr->cmd.call.client_id );
    break; /* CM_CMD_TYPE_CALL */

  case CM_CMD_TYPE_INBAND:
    CMDBG_CMD_MSG( "inbandcmd", cmd_ptr->cmd.inband.cmd,
                   cm_inband_cmd_e_type_string_map,
                   cmd_ptr->cmd.inband.client_id );
    break;

  case CM_CMD_TYPE_SS:
    CMDBG_CMD_MSG( "sscmd", cmd_ptr->cmd.ss.cmd,
                   cm_ss_cmd_e_type_string_map,
                   cmd_ptr->cmd.ss.client_id );
    break;

  #ifdef FEATURE_CDSMS
  case CM_CMD_TYPE_SMS:
    CMDBG_CMD_MSG( "smscmd", cmd_ptr->cmd.sms.cmd,
                   cm_sms_cmd_e_type_string_map,
                   cmd_ptr->cmd.sms.client_id );
    break;
  #endif

  #ifdef FEATURE_GPSONE
#error code not present
  #endif

  #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
  #endif

  #ifdef FEATURE_DBM
  case CM_CMD_TYPE_DBM:
    CMDBG_CMD_MSG( "dbmcmd", cmd_ptr->cmd.dbm_msg.cmd,
                   cm_dbm_cmd_e_type_string_map,
                   cmd_ptr->cmd.dbm_msg.client_id );
    break;
  #endif /* FEATURE_DBM */

  #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
  #endif   /* FEATURE_WCDMA || FEATURE_GSM */

  default:
    CM_MSG_HIGH( ">>CM <unknown>cmd %d", CMD_TYPE(cmd_ptr), 0, 0);
    break;
  } /* switch ( CMD_TYPE(cmd_ptr) ) */

} /* cmdbg_print_cm_cmd() */



/*===========================================================================

FUNCTION CMDBG_PRINT_CM_EVENT

DESCRIPTION
  Print the events sent to clients.

===========================================================================*/
void cmdbg_print_cm_event
(
  cm_cmd_type_e_type  cmd_type,
  word                event,
  const void          *info_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e529
  ** This is for all local variables inside of the switch
  ** 529: Symbol 'ph_event' (line 1594) not subsequently referenced
  ** 529: Symbol 'inband_event' (line 1607) not subsequently referenced
  ** 529: Symbol 'inband_info_ptr' (line 1608) not subsequently referenced
  ** etc
  ** Lint is obviousely confused as those ARE used after they were defined
  */
  switch ( cmd_type )
  {
  case CM_CMD_TYPE_CALL:
    {
    cm_call_event_e_type    call_event = (cm_call_event_e_type) event;
    const cm_mm_call_info_s_type *call_info_ptr =
      (const cm_mm_call_info_s_type *) info_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM callevt=%d <%d> (%s)", call_event,
      call_info_ptr->call_id, cm_call_event_e_type_string_map[ call_event ]);
    #else
    CM_MSG_HIGH("<<CM callevt=%d <%d>", call_event, call_info_ptr->call_id, 0);
    #endif

    SYS_ARG_NOT_USED(call_info_ptr);
    SYS_ARG_NOT_USED(call_event);

    }
    break;

  case CM_CMD_TYPE_PH:
    {
    cm_ph_event_e_type    ph_event = (cm_ph_event_e_type) event;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM phevt=%d (%s)", ph_event,
      cm_ph_event_e_type_string_map[ ph_event ], 0 );
    #else
    CM_MSG_HIGH("<<CM phevt=%d", ph_event, 0, 0);
    #endif

    SYS_ARG_NOT_USED(ph_event);

    }
    break;

  case CM_CMD_TYPE_INBAND:
    {
    cm_inband_event_e_type    inband_event = (cm_inband_event_e_type) event;
    const cm_inband_info_s_type *inband_info_ptr =
      (const cm_inband_info_s_type *) info_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM inbandevt=%d <%d> (%s)", inband_event,
      inband_info_ptr->call_id,
      cm_inband_event_e_type_string_map[ inband_event ]);
    #else
    CM_MSG_HIGH("<<CM inbandevt=%d <%d>", inband_event,
      inband_info_ptr->call_id, 0);
    #endif

    SYS_ARG_NOT_USED(inband_info_ptr);
    SYS_ARG_NOT_USED(inband_event);

    }
    break;

  case CM_CMD_TYPE_SS:
    {
    cm_ss_event_e_type    ss_event = (cm_ss_event_e_type) event;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM ssevt=%d (%s)", ss_event,
      cm_ss_event_e_type_string_map[ ss_event ], 0 );
    #else
    CM_MSG_HIGH("<<CM ssevt=%d", ss_event, 0, 0);
    #endif

    SYS_ARG_NOT_USED(ss_event);

    }
    break;

  #ifdef FEATURE_CDSMS
  case CM_CMD_TYPE_SMS:
    {
    cm_sms_event_e_type    sms_event = (cm_sms_event_e_type) event;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM smsevt=%d (%s)", sms_event,
      cm_sms_event_e_type_string_map[ sms_event ], 0 );
    #else
    CM_MSG_HIGH("<<CM smsevt=%d", sms_event, 0, 0);
    #endif

    SYS_ARG_NOT_USED(sms_event);

    }
    break;
  #endif

  #ifdef FEATURE_GPSONE
#error code not present
  #endif

  #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
  #endif

  #ifdef FEATURE_BCMCS
#error code not present
  #endif /* FEATURE_BCMCS */

  default:
    CM_MSG_HIGH( ">>CM <unknown>evt %d", cmd_type, 0, 0);
    break;
  }
  /*lint -restore */
} /* cmdbg_print_cm_event() */



/*===========================================================================

FUNCTION CMDBG_PRINT_CM_CMDERR

DESCRIPTION
  Print the command error sent to clients.

===========================================================================*/
void cmdbg_print_cm_cmderr
(
  cm_cmd_type_e_type  cmd_type,                      /* Type of CM command */
  word                cmd_err,                  /* call command error code */
  const void          *cmd_ptr           /* pointer to call command struct */
)
{

  CM_ASSERT(cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( cmd_type )
  {
  case CM_CMD_TYPE_CALL:
    {
    cm_call_cmd_err_e_type call_cmd_err = (cm_call_cmd_err_e_type) cmd_err;
    const cm_call_cmd_s_type *call_cmd_ptr =
      (const cm_call_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmcall_s_type *call_ptr = cmcall_ptr(call_cmd_ptr->info.call_id);

    cmdbg_print_message("<<CM call command error:");
    cmdbg_print_message("<<CM cmd=%d (%s)", call_cmd_ptr->cmd,
      cm_call_cmd_e_type_string_map[ call_cmd_ptr->cmd ] );
    cmdbg_print_message("<<CM err=%d (%s)", call_cmd_err,
      cm_call_cmd_err_e_type_string_map[ call_cmd_err ] );
    cmdbg_print_message("<<CM client id=%ld, call id=%d",
      cmclient_get_client_type(call_cmd_ptr->client_id),
      call_cmd_ptr->info.call_id);

    if(call_ptr != NULL)
    {
      cmdbg_print_message("<<CM call state=%d (%s)", call_ptr->call_state,
        cm_call_state_e_type_string_map[ call_ptr->call_state ] );
    }
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    MSG_ERROR( "<<CM call cmd err, cmd=%d, err=%d, client=%ld",
      call_cmd_ptr->cmd, call_cmd_err,
      cmclient_get_client_type(call_cmd_ptr->client_id) );
    MSG_ERROR( "<<CM call cmd err, call id=%d",
      call_cmd_ptr->info.call_id, 0, 0 );
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(call_cmd_err);
    SYS_ARG_NOT_USED(call_cmd_ptr);

    }
    break;


  case CM_CMD_TYPE_PH:
    {
    cm_ph_cmd_err_e_type ph_cmd_err = (cm_ph_cmd_err_e_type) cmd_err;
    const cm_ph_cmd_s_type *ph_cmd_ptr =
      (const cm_ph_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message("<<CM ph command error:");
    cmdbg_print_message("<<CM cmd=%d (%s)", ph_cmd_ptr->cmd,
      cm_ph_cmd_e_type_string_map[ ph_cmd_ptr->cmd ] );
    cmdbg_print_message("<<CM err=%d (%s)", ph_cmd_err,
      cm_ph_cmd_err_e_type_string_map[ ph_cmd_err ] );
    cmdbg_print_message("<<CM client id=%ld",
      cmclient_get_client_type(ph_cmd_ptr->client_id) );
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    MSG_ERROR( "<<CM ph cmd err, cmd=%d, err=%d, client=%ld",
      ph_cmd_ptr->cmd, ph_cmd_err,
      cmclient_get_client_type(ph_cmd_ptr->client_id) );
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(ph_cmd_err);
    SYS_ARG_NOT_USED(ph_cmd_ptr);

    }
    break;

  case CM_CMD_TYPE_INBAND:
    {
    cm_inband_cmd_err_e_type inband_cmd_err = (cm_inband_cmd_err_e_type) cmd_err;
    const cm_inband_cmd_s_type *inband_cmd_ptr =
      (const cm_inband_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message("<<CM inband command error:");
    cmdbg_print_message("<<CM cmd=%d (%s)", inband_cmd_ptr->cmd,
      cm_inband_cmd_e_type_string_map[ inband_cmd_ptr->cmd ] );
    cmdbg_print_message("<<CM err=%d (%s)", inband_cmd_err,
      cm_inband_cmd_err_e_type_string_map[ inband_cmd_err ] );
    cmdbg_print_message("<<CM client id=%ld, call id=%d",
      cmclient_get_client_type(inband_cmd_ptr->client_id),
      inband_cmd_ptr->info.call_id );
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    MSG_ERROR( "<<CM inband cmd err, cmd=%d, err=%d, client=%ld",
      inband_cmd_ptr->cmd, inband_cmd_err,
      cmclient_get_client_type(inband_cmd_ptr->client_id) );
    MSG_ERROR( "<<CM inband cmd err, call id=%d",
      inband_cmd_ptr->info.call_id, 0, 0 );
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(inband_cmd_err);
    SYS_ARG_NOT_USED(inband_cmd_ptr);

    }
    break;

  case CM_CMD_TYPE_SS:
    {
    cm_ss_cmd_err_e_type ss_cmd_err = (cm_ss_cmd_err_e_type) cmd_err;
    const cm_ss_cmd_s_type *ss_cmd_ptr =
      (const cm_ss_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message("<<CM ss command error:");
    cmdbg_print_message("<<CM cmd=%d (%s)", ss_cmd_ptr->cmd,
      cm_ss_cmd_e_type_string_map[ ss_cmd_ptr->cmd ] );
    cmdbg_print_message("<<CM err=%d (%s)", ss_cmd_err,
      cm_ss_cmd_err_e_type_string_map[ ss_cmd_err ] );
    cmdbg_print_message("<<CM client id=%ld",
      cmclient_get_client_type(ss_cmd_ptr->client_id) );
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    MSG_ERROR( "<<CM ss cmd err, cmd=%d, err=%d, client=%ld",
      ss_cmd_ptr->cmd, ss_cmd_err,
      cmclient_get_client_type(ss_cmd_ptr->client_id) );
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(ss_cmd_err);
    SYS_ARG_NOT_USED(ss_cmd_ptr);

    }
    break;

  #ifdef FEATURE_CDSMS
  case CM_CMD_TYPE_SMS:
    {
    cm_sms_cmd_err_e_type sms_cmd_err = (cm_sms_cmd_err_e_type) cmd_err;
    const cm_sms_cmd_s_type *sms_cmd_ptr =
      (const cm_sms_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message("<<CM sms command error:");
    cmdbg_print_message("<<CM cmd=%d (%s)", sms_cmd_ptr->cmd,
      cm_sms_cmd_e_type_string_map[ sms_cmd_ptr->cmd ] );
    cmdbg_print_message("<<CM err=%d (%s)", sms_cmd_err,
      cm_sms_cmd_err_e_type_string_map[ sms_cmd_err ] );
    cmdbg_print_message("<<CM client id=%ld",
      cmclient_get_client_type(sms_cmd_ptr->client_id) );
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    MSG_ERROR( "<<CM sms cmd err, cmd=%d, err=%d, client=%ld",
      sms_cmd_ptr->cmd, sms_cmd_err,
      cmclient_get_client_type(sms_cmd_ptr->client_id) );
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(sms_cmd_err);
    SYS_ARG_NOT_USED(sms_cmd_ptr);

    }
    break;
  #endif

  #ifdef FEATURE_GPSONE
#error code not present
  #endif

  #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
  #endif

  default:
    CM_MSG_HIGH( ">>CM <unknown>cmd %d", cmd_type, 0, 0);
    break;
  }
} /* cmdbg_print_cm_cmderr() */

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

FUNCTION CMDBG_PRINT_MC_RPT

DESCRIPTION
 Process MC reports

===========================================================================*/
void cmdbg_print_mc_rpt
(
  const cm_mc_rpt_type    *mc_rpt_ptr                  /* Report from MC */
)
/*lint -esym(550, call_id) */
{

  cm_call_id_type   call_id = CM_CALL_ID_NONE;          /* Call id */
  int i;                                                /* Loop counter */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(mc_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* This is here to figure out call id to be printed with message
  */
  switch( mc_rpt_ptr->hdr.cmd )
  {
  case CM_ABRV_ALERT_F:
    call_id = mc_rpt_ptr->abrv.call_id;
    break;

  case CM_ABRV_REORDER_F:
    call_id = mc_rpt_ptr->abrv_reorder.call_id;
    break;

  case CM_ABRV_INTERCEPT_F:
    call_id = mc_rpt_ptr->abrv_intercept.call_id;
    break;

  case CM_ALERT_F:
    call_id = mc_rpt_ptr->alert.call_id;
    break;

  case CM_INCOMING_CALL_F:
  #ifdef FEATURE_HDR
#error code not present
  #endif
    call_id = mc_rpt_ptr->incoming_call.call_id;
    break;

  case CM_STOP_ALERT_F:
    call_id = mc_rpt_ptr->stop.call_id;
    break;

  case CM_INTERCEPT_F:
    call_id = mc_rpt_ptr->inter.call_id;
    break;

  case CM_FLASH_F:
    call_id = mc_rpt_ptr->flash.call_id;
    break;

  #ifdef FEATURE_HANDLE_FWD_DTMF
  case CM_DTMF_BURST_F:
    call_id = mc_rpt_ptr->dtmf.call_id;
    break;

  case CM_DTMF_CONT_START_F:
    call_id = mc_rpt_ptr->dtmf_cont_start.call_id;
    break;

  case CM_DTMF_CONT_STOP_F:
    call_id = mc_rpt_ptr->dtmf_cont_stop.call_id;
    break;
  #endif /* FEATURE_HANDLE_FWD_DTMF */

  case CM_MARKOV_F:
    call_id = mc_rpt_ptr->markov.call_id;
    break;

  case CM_CALL_CONNECTED_F:
  #ifdef FEATURE_HDR
#error code not present
  #endif
    call_id = mc_rpt_ptr->connect.call_id;
    break;

  case CM_DISPLAY_F:
    call_id = mc_rpt_ptr->display.call_id;
    break;

  #ifdef FEATURE_IS95B_EXT_DISP
  case CM_EXT_DISPLAY_F:
    call_id = mc_rpt_ptr->ext_disp.call_id;
    break;
  #endif /*FEATURE_IS95B_EXT_DISP*/

  case CM_CALLED_PARTY_F:
    call_id = mc_rpt_ptr->called_party.call_id;
    break;

  case CM_CALLING_PARTY_F:
    call_id = mc_rpt_ptr->calling_party.call_id;
    break;

  case CM_CONNECTED_NUM_F:
    call_id = mc_rpt_ptr->connected_num.call_id;
    break;

  case CM_SIGNAL_F:
    call_id = mc_rpt_ptr->signal.call_id;
    break;

  #ifdef FEATURE_OTASP
  case CM_OTASP_STATUS_F:
    call_id = CM_CALL_ID_NONE;
    break;

  case CM_OTASP_COMMIT_STATUS_F:
    call_id = CM_CALL_ID_NONE;
    break;
  #endif /* FEATURE_OTASP */

  case CM_LINE_CTRL_F:
    call_id = mc_rpt_ptr->line_ctrl.call_id;
    break;

  case CM_N_MSG_F:
    call_id = mc_rpt_ptr->n_msg.call_id;
    break;

  case CM_N_CLI_F:
    call_id = mc_rpt_ptr->n_cli.call_id;
    break;

  case CM_CALL_ORIG_FAIL_F:
  #ifdef FEATURE_HDR
#error code not present
  #endif
    call_id = mc_rpt_ptr->orig_fail.call_id;
    break;

  #ifdef FEATURE_IS2000
  case CM_CALL_FAIL_RETRY_ORDER_F:
    call_id = mc_rpt_ptr->retry_order.call_id;
    break;
  #endif /* FEATURE_IS2000 */

  #ifdef FEATURE_T53
  case CM_EXT_BRST_INTL_F:
    /* We don't have call id for this message */
    call_id = CM_CALL_ID_NONE;
    break;

  case CM_NSS_CLIR_F:
    call_id = mc_rpt_ptr->nss_clir_rec.call_id;
    break;

  case CM_NSS_REL_F:
    /* We don't have call id for this message */
    call_id = CM_CALL_ID_NONE;
    break;

  case CM_NSS_AUD_CTRL_F:
    call_id = mc_rpt_ptr->nss_aud_ctrl_rec.call_id;
    break;

  case CM_SND_BRST_DTMF_ACK_F:
    call_id = mc_rpt_ptr->snd_brst_dtmf_ack.call_id;
    break;

  case CM_CALL_HOLD_L2ACK_F:
    /* We don't have call id for this message */
    call_id = CM_CALL_ID_NONE;
    break;
  #endif /* FEATURE_T53 */

  #ifdef FEATURE_IS95B_NDSS
  case CM_NDSS_REDIRECTION_F:
    call_id = mc_rpt_ptr->ndss_redirection.call_id;
    break;
  #endif /*FEATURE_IS95B_NDSS */

  case CM_REDIRECTING_NUMBER_F:
    call_id = mc_rpt_ptr->redirecting_number.call_id;
    break;

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
  #endif

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */
  case CM_VOICE_AS_DATA_F:
  #if(defined(FEATURE_CDSMS) || defined(FEATURE_DBM))
  case CM_MO_DBM_STATUS_F:
  case CM_MT_DBM_DATA_F:
  #endif /* defined(FEATURE_CDSMS) || defined(FEATURE_DBM) */
  #ifdef FEATURE_JCDMA
  case CM_REG_SUCCESS_F:
  case CM_REG_FAILURE_F:
  #endif /* FEATURE_JCDMA */
  case CM_UPCALL_F:
  case CM_LOCK_F:
  case CM_UNLOCK_F:
  case CM_MAINTREQ_F:
  case CM_N_VC_MSG_F:
  case CM_N_VC_CLI_F:
  case CM_SRV_SYS_INFO_F:
  case CM_MSG_WAITING_F:
  case CM_N_VOICE_F:
  case CM_EXIT_TC_F:
  case CM_RPT_NONE:
  case CM_RPT_GW_BEGIN:
  case CM_MO_CALL_CONF:
  case CM_MT_CALL_IND:
  case CM_CALL_PROGRESS_INFO_IND:
  case CM_CALL_CONNECTED_IND:
  case CM_CONNECTED_NUMBER_IND:
  case CM_MT_END_CALL_IND:
  case CM_MT_USER_DATA_IND:
  case CM_MT_SETUP_IND:
  case CM_MT_MODIFY_IND:
  case CM_MT_DISC_IND:
  case CM_START_DTMF_CONF:
  case CM_STOP_DTMF_CONF:
  case CM_MANAGE_CALLS_CONF:
  case CM_MODIFY_COMPLETE_CONF:
  case CM_ABORT_CC_CONF:
  case CM_ACTIVATE_SS_CONF:
  case CM_DEACTIVATE_SS_CONF:
  case CM_ERASE_SS_CONF:
  case CM_INTERROGATE_SS_CONF:
  case CM_REGISTER_PASSWORD_CONF:
  case CM_REGISTER_SS_CONF:
  case CM_PROCESS_UNSTRUCTURED_SS_CONF:
  case CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF:
  case CM_MT_CALL_SS_IND:
  case CM_FORWARD_CHECK_SS_INDICATION_IND:
  case CM_UNSTRUCTURED_SS_NOTIFY_IND:
  case CM_UNSTRUCTURED_SS_IND:
  case CM_RELEASE_UNSTRUCTURED_SS_IND:
  case CM_CONNECTION_STATE_IND:
  case CM_GET_PASSWORD_IND:
  case CM_LCS_LOCATION_NOTIFICATION_IND:
  case CM_LCS_MOLR_CONF:
  case CM_LOAD_DATA_CNF:
  case CM_MM_INFORMATION_IND:
  case CM_NETWORK_LIST_CNF:
  case CM_SAVE_DATA_CNF:
  case CM_SERVICE_CNF:
  case CM_SERVICE_IND:
  case CM_SIM_AVAILABLE_CNF:
  case CM_SIM_NOT_AVAILABLE_CNF:
  case CM_STOP_MODE_CNF:
  case CM_DEEP_SLEEP_IND:
  case CM_RR_STATUS_IND:
  case CM_RRC_STATUS_IND:
  case CM_PDP_ACTIVATE_CNF:
  case CM_PDP_ACTIVATE_REJ:
  case CM_PDP_ACTIVATE_IND:
  case CM_PDP_DEACTIVATE_CNF:
  case CM_PDP_DEACTIVATE_IND:
  case CM_PDP_DEACTIVATE_REJ:
  case CM_PDP_MODIFY_IND:
  case CM_PDP_MODIFY_CNF:
  case CM_PDP_MODIFY_REJ:
  case CM_PDP_ACTIVATE_SEC_CNF:
  case CM_PDP_ACTIVATE_SEC_REJ:
  case CM_PDP_ACTIVATE_REJ_RSP:
  case CM_PS_SIG_CON_REL_IND:
  case CM_RAB_RELEASE_IND:
  case CM_RAB_REESTABLISH_IND:
  case CM_RAB_REESTAB_CNF:
  case CM_RAB_REESTAB_REJ:
  case CM_RAB_REESTAB_FAIL:
  case CM_PDP_UNKNOWN_REQ:
  case CM_MT_DATA_CONNECT_IND:
  case CM_CHANGE_TEST_MODE_IND:
  case CM_SIM_STATE_CHGD_F:
  case CM_SI_NETWORK_LIST_F:
  case CM_MMGSDI_CARD_STATUS_IND:
  #ifdef FEATURE_GSTK
  case CM_MMGSDI_REFRESH_FCN_IND:
  #endif  /* FEATURE_GSTK */
  case CM_MMGSDI_REFRESH_FCN_IND_F:
  case CM_GSDI_CARD_STATUS_IND:
  case CM_GSDI_REFRESH_FCN_IND:
  case CM_DATA_SUSPEND_CHGD_F:
  case CM_CS_SIG_REL_IND:
  case CM_CAMPED_IND:
  case CM_PDP_DEACTIVATE_PROMOTE_IND:
  case CM_PDP_DEACTIVATE_PROMOTE_CNF:
  case CM_USER_CONGESTION_IND:
  case CM_CB_DATA_IND_F:
  case CM_CB_CELL_CHANGE_IND_F:
  case CM_CB_DUPLICATION_DETECTION_IND_F:
  case CM_SM_DATA_IND_F:
  case CM_SM_REPORT_IND_F:
  case CM_SM_LINK_CONTROL_IND_F:
  case CM_CIPHER_IND:
  case CM_5_SEND_CONF:
  case CM_ERASE_CC_ENTRY_CONF:
  case CM_CCBS_INFO_IND:
  case CM_RECALL_IND:
  case CM_HPLMN_SEARCH_CNF:
  case CM_MM_PS_DATA_SUSPEND_IND:
  case CM_EMERGENCY_NUM_LIST_IND:
  case CM_CELL_SERVICES_IND:
  case CM_HSPA_CALL_STATUS_IND:
  case CM_PS_SIG_CON_REL_CNF:
  case CM_PH_STATUS_CHGD_CNF:
  case CM_CONN_MODE_SERVICE_STATUS_IND:
  case CM_CONN_MODE_NO_SERVICE_WAKEUP_IND:
  case CM_RPT_GW_END:
  case CM_RPT_1X_BEGIN:
  case CM_HDR_DEACTIVATE_ACK_F:
  case CM_HDR_RESELECT_EXIT_HDR_F:
  case CM_GPS_MODE_GRANTED_F:
  case CM_GPS_MODE_EXIT_F:
  case CM_BS_STATUS_RSP_F:
  case CM_PRIVACY_SETTING_F:
  case CM_TUNNEL_MSG_F:
  case CM_HDR_TUNNEL_MSG_F:
  case CM_PSIST_F:
  case CM_HDR_BCMCS_FLOW_MONITORING_START_F:
  case CM_HDR_BCMCS_FLOW_MONITORING_END_F:
  case CM_CDMA_CONNECT_ORDER_ACK_F:
  case CM_HDR_LOCATION_NOTIFICATION_STATUS_F:
  case CM_GET_CSNA_MSG_CONFIG_F:
  case CM_UPDATE_CSNA_MSG_CONFIG_F:
  case CM_TUNNEL_MSG_STATUS_F:
  case CM_HDR_SESSION_OPEN_FAILED:
  case CM_HDR_SESSION_CLOSED:
  case CM_RPT_1X_END:
  case CM_RPT_SD_BEGIN:
  case CM_SRV_IND_INFO_F:
  case CM_ACQ_FAIL_F:
  case CM_OK_TO_ORIG_F:
  case CM_NAM_CHANGED_F:
  case CM_E911_CALLBACK_F:
  case CM_STANDBY_SLEEP_F:
  case CM_STANDBY_WAKE_F:
  case CM_LPM_OK_F:
  case CM_WLAN_BSS_LIST_F:
  case CM_IP_APP_REG_STATUS:
  case CM_SRV_LOST_F:
  case CM_RPT_SD_END:
  case CM_RPT_IP_BEGIN:
  case CM_IP_CALL_IND_MT_INVITE:
  case CM_IP_CALL_IND_MT_RING_CNF:
  case CM_IP_CALL_IND_MO_PROGR_INFO:
  case CM_IP_CALL_IND_CONNECTED:
  case CM_IP_CALL_IND_CALL_END:
  case CM_IP_CALL_IND_CALL_HOLD:
  case CM_IP_CALL_IND_CALL_RETRIEVE:
  case CM_IP_CALL_IND_VS_STATUS:
  case CM_IP_CALL_IND_SUPS_CONF:
  case CM_IP_CALL_IND_TRANSFER:
  case CM_IP_CALL_IND_CONNECTED_NUM:
  case CM_IP_APP_ALLAPPS_OPRT_CNF:
  case CM_RPT_IP_END:
  case CM_RPT_IP_SMS_BEGIN:
  case CM_IP_SMS_IND_MO_SMS_STATUS:
  case CM_IP_SMS_IND_MT_SMS:
  case CM_RPT_IP_SMS_END:
  case CM_RPT_GENERIC_BEGIN:
  case CM_DEM_RPT:
  case CM_WLAN_RPT:
  case CM_RPT_GENERIC_END:
  case CM_NUM_CM_COMMANDS:
  #ifdef FEATURE_BCMCS
#error code not present
  #endif // FEATURE_BCMCS

    /* We don't have call id for this message */
    call_id = CM_CALL_ID_NONE;
    break;

  case CM_END_VOIP_CALL_F:
    call_id = mc_rpt_ptr->end_voip_call.call_id;
    break;

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
  #endif
  case CM_CALL_FADE_F:
  case CM_CALL_RELEASE_F:
  #ifdef FEATURE_HDR
#error code not present
  #endif
  case CM_CALL_EMERGENCY_FLASHED_F:
    /* This is a special case. It will be handled below */
    call_id = CM_CALL_ID_NONE;           /* To get rid of compiler warning */
    break;

  default:
    MSG_HIGH("CM<< Unknown LL rpt = %d", mc_rpt_ptr->hdr.cmd, 0, 0);
    break;
  }

  /*
  ** NOW WE'RE AT THE POINT WHERE WE CAN PRINT IT ALL
  */
  /* Print the report */
  /*lint -save -e656 */
  CMDBG_RPT_MSG("MC", mc_rpt_ptr->hdr.cmd, CM_RPT_1X_BEGIN,
                 cm_name_1x_type_string_map, call_id); /*lint !e641 */
  /*lint -restore */

  /* Print report specific information */
  switch(mc_rpt_ptr->hdr.cmd)
  {
    case CM_CALL_RELEASE_F:
    case CM_CALL_FADE_F:
    #ifdef FEATURE_HDR_HYBRID
#error code not present
    #endif
    #ifdef FEATURE_HDR
#error code not present
    #endif
      /*
      ** Assuming num_call_ids and call_ids[] start in the same offset for all
      ** the above reports.
      */
      MSG_HIGH( "CM<< #cids=%d", mc_rpt_ptr->release.num_call_ids, 0, 0);
      for(i = 0; i < mc_rpt_ptr->release.num_call_ids; i++)
      {
        MSG_HIGH( "CM<< %d", mc_rpt_ptr->release.call_ids[i], 0, 0);
      }
      break;

    case CM_CALL_EMERGENCY_FLASHED_F:
      MSG_HIGH( "CM<< emer_f_cid=%d, emer_o_cid=%d",
                 mc_rpt_ptr->call_emergency_flashed.emergency_flash_call_id,
                 mc_rpt_ptr->call_emergency_flashed.original_call_id, 0);
      break;

    case CM_CALL_ORIG_FAIL_F:
      CMDBG_RPT_MSG("orig fail code", mc_rpt_ptr->orig_fail.code,
                     -1 /* CM_CALL_ORIG_ERR_NONE is not defined */,
                     cm_call_orig_fail_e_type_string_map, 0);  /*lint !e641 */
      break;

  #ifdef FEATURE_BCMCS
#error code not present
  #endif // FEATURE_BCMCS
  #ifdef FEATURE_CM_SS_MEAS
#error code not present
  #endif 
  default:
    MSG_MED("MC report = %d", mc_rpt_ptr->hdr.cmd,0,0);
  }
} /* cmdbg_print_mc_rpt() */
/*lint +esym(550, call_id) */

#endif // #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))


/*===========================================================================

FUNCTION CMDBG_PRINT_SD_RPT

DESCRIPTION
 Process SD reports

===========================================================================*/
void cmdbg_print_sd_rpt
(
  const cm_sd_rpt_u_type    *sd_rpt_ptr                 /* Report from MC */
)
/*lint -esym(715, sd_rpt_ptr) */
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(sd_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CMDBG_RPT_MSG("SD",
                sd_rpt_ptr->hdr.cmd,
                CM_RPT_SD_BEGIN,
                cm_name_sd_type_string_map, 0); /*lint !e641 */

} /* cmdbg_print_sd_rpt() */
/*lint +esym(715, sd_rpt_ptr) */

/*===========================================================================
FUNCTION CMDBG_PRINT_GENERIC_RPT

DESCRIPTION
 Print Generic reports

===========================================================================*/
void cmdbg_print_generic_rpt
(
  const cm_generic_rpt_s_type *generic_rpt_ptr
    /* Generic report
    */
)
/*lint -esym(715, generic_rpt_ptr) */
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(generic_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CMDBG_RPT_MSG("GEN",
                generic_rpt_ptr->hdr.cmd,
                CM_RPT_GENERIC_BEGIN,
                cm_name_generic_type_string_map, generic_rpt_ptr->generic_rpt.dem_rpt.dem_rpt_type); /*lint !e641 */

} /* cmdbg_print_generic_rpt */

/*lint +esym(715, generic_rpt_ptr) */


#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

FUNCTION CMDBG_PRINT_MC_CMD

DESCRIPTION
  Print the command that was sent to the MC task.

===========================================================================*/
void cmdbg_print_mc_cmd
(
  const mc_msg_type *mc_cmd_ptr                         /* Pointer to mc command */
)
{
  cm_call_id_type   call_id;                                    /* call id */
  CM_ASSERT( mc_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This is here to figure out call id to be printed with message */
  switch(mc_cmd_ptr->hdr.cmd)
  {
  case MC_ORIGINATION_F:
    call_id = mc_cmd_ptr->origination.call_id;
    break;

  case MC_ANSWER_F:
    call_id = mc_cmd_ptr->answer.call_id;
    break;

  case MC_FLASH_F:
    call_id = mc_cmd_ptr->flash.call_id;
    break;

  case MC_START_CONT_DTMF_F:
    call_id = mc_cmd_ptr->start_cont_dtmf.call_id;
    break;

  case MC_STOP_CONT_DTMF_F:
    call_id = mc_cmd_ptr->stop_cont_dtmf.call_id;
    break;

  case MC_BURST_DTMF_F:
    call_id = mc_cmd_ptr->burst_dtmf.call_id;
    break;

  #ifdef FEATURE_T53
  case MC_HOLD_F:
    call_id = mc_cmd_ptr->hold.call_id;
    break;
  #endif /* FEATURE_T53 */

  case MC_LPM_F:
  #ifdef FEATURE_CDSMS
  case MC_AWI_SMS_ACK_F:
  #endif
  #ifdef FEATURE_BROADCAST_SMS
  case MC_BCSMS_ENABLE_F:
  #endif
  #if defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
  case MC_SILENT_REDIAL_F:
  #endif
  #ifdef FEATURE_IS2000_R_SCH
  case MC_RES_REQ_F:
  #endif

  #if defined(FEATURE_DDTM_CNTL) && defined(FEATURE_HDR)
#error code not present
  #endif /* FEATURE_DDTM_CNTL */
  #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
  #endif
  case MC_SS_TIMER_EXP_F:
  case MC_ONLINE_F:
  case MC_DATA_END_F:
  case MC_MO_DBM_F:
  case MC_PRIVACY_PREF_F:
  case MC_PREF_SYS_CHANGED_F:
  case MC_NAM_CHANGED_F:
  case MC_OFFLINE_DIGITAL_F:
  case MC_OFFLINE_ANALOG_F:
  case MC_RESET_F:
  case MC_POWERDOWN_F:
  case MC_JUMP_TRAFFIC_F:
  case MC_JUMP_ACCESS_F:
  case MC_SECTOR_LOCK_F:
  case MC_RES_REL_F:
  case MC_CDMA_PROT_ACT_CMD_F:
  case MC_CDMA_PROT_DEACT_CMD_F:
  case MC_CDMA_PROT_PH_STAT_CHGD_CMD_F:
  case MC_CDMA_PROT_GEN_CMD_F:
  case MC_AMPS_PROT_ACT_CMD_F:
  case MC_AMPS_PROT_DEACT_CMD_F:
  case MC_AMPS_PROT_PH_STAT_CHGD_CMD_F:
  case MC_AMPS_PROT_GEN_CMD_F:
  case MC_BSSTATUS_REQ_F:
    /* We don't have call id for this message */
    call_id = CM_CALL_ID_NONE;
    break;
  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
  #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */
  #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
  #endif
  case MC_END_F:
    /* This is a special case. It will be handled below */
    call_id = CM_CALL_ID_NONE;           /* To get rid of compiler warning */
    break;

  default:
     MSG_HIGH("CM>> Unknown LL cmd = %d", mc_cmd_ptr->hdr.cmd, 0, 0);
     return;
  }

  /*
  ** NOW WE'RE AT THE POINT WHERE WE CAN PRINT IT ALL
  */

  /* Print the command */
  #ifdef CMDEBUG_STRING_MAPS_ACTIVE
  cmdbg_print_message( "CM>> LL cmd=%d (%s)", mc_cmd_ptr->hdr.cmd,
    mc_name_type_string_map[ mc_cmd_ptr->hdr.cmd ] );
  #else
    MSG_HIGH( "CM>> LL cmd=%d", mc_cmd_ptr->hdr.cmd, 0, 0);
  #endif

  /* Print command specific information */
  if(mc_cmd_ptr->hdr.cmd == MC_END_F)
  {
    int i;

    MSG_HIGH( "CM>> #end calls=%d", mc_cmd_ptr->end.num_call_ids, 0, 0);
    for(i = 0; i < mc_cmd_ptr->end.num_call_ids; i++)
    {
      MSG_HIGH( "CM>> call id=%d; end reason= %d;",
        mc_cmd_ptr->end.call_ids[i], mc_cmd_ptr->end.end_reasons[i], 0);
    } /* for(i = 0; i < mc_rpt_ptr->release.num_call_ids; i++) */
  }
  else
  {
    if ( call_id != CM_CALL_ID_NONE )
    {
      MSG_HIGH( "CM>> call id=%d", call_id, 0, 0 );
    }

    #ifdef FEATURE_IS2000_REL_A
    if (mc_cmd_ptr->hdr.cmd == MC_FLASH_F)
    {
      #ifdef CMDEBUG_STRING_MAPS_ACTIVE
      cmdbg_print_message( "CM>> flash type=%d (%s)", mc_cmd_ptr->flash.flash_type,
        cm_call_sups_type_e_type_string_map[ mc_cmd_ptr->flash.flash_type ]);
      #else /* !CMDEBUG_STRING_MAPS_ACTIVE */
      MSG_HIGH( "CM>> flash type=%d", mc_cmd_ptr->flash.flash_type, 0, 0 );
      #endif /* CMDEBUG_STRING_MAPS_ACTIVE */
    }
    #endif /* FEATURE_IS2000_REL_A */
  } /* else */

} /* cmdbg_print_mc_cmd() */
#endif // #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))


#ifdef CM_DEBUG
#error code not present
#endif /* CM_DEBUG */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Print information associated with cm_ph_info_s_type buffer.
*/
void cmdbg_ph_info_print( const cm_ph_info_s_type  *ph_info_ptr )
/*lint -esym(715, ph_info_ptr) */
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_IN_USE_PRT( ph_info_ptr->is_in_use );
  CM_OPRT_MODE_PRT( ph_info_ptr->oprt_mode );           /*lint !e641 !e732 */
  CM_CDMA_LOCK_MODE_PRT( ph_info_ptr->cdma_lock_mode ); /*lint !e641 !e732 */
  CM_ANSWER_VOICE_PRT( ph_info_ptr->answer_voice );     /*lint !e641 !e732 */
  CM_MODE_PREF_PRT( ph_info_ptr->mode_pref );           /*lint !e641 !e732 */
  CM_ROAM_PREF_PRT( ph_info_ptr->roam_pref );           /*lint !e641 !e732 */

  CM_NAM_PRT( ph_info_ptr->nam_sel );                   /*lint !e641 !e732 */
  CM_NAM_PRT( ph_info_ptr->curr_nam );                  /*lint !e641 !e732 */

}
/*lint +esym(715, ph_info_ptr) */

void cmdbg_ss_info_print( const cm_mm_ss_info_s_type   *ss_info_ptr )
/*lint -esym(715,ss_info_ptr)*/
{
}
/*lint +esym(715,ss_info_ptr)*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Print information associated with cm_call_info_s_type buffer.
*/
void cmdbg_call_info_print( const cm_mm_call_info_s_type  *call_info_ptr )
/*lint -esym(715,call_info_ptr) */
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_CALL_STATE_PRT( call_info_ptr->call_state );   /*lint !e641 !e732 */
  CM_CLIENT_TYPE_PRT(cmclient_get_client_type(call_info_ptr->call_client_id));
  CM_CALL_TYPE_PRT( call_info_ptr->call_type );     /*lint !e641 !e732 */
  CM_CALL_END_PRT( call_info_ptr->end_status );

}
/*lint +esym(715,call_info_ptr) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Print favored mode preferences for origination
*/
void cmdbg_print_fav_mode_pref_for_origination(

  cm_call_type_e_type            call_type,
    /* requested call type
    */

  cm_srv_type_e_type             srv_type,
    /* requested service type
    */

  cm_mode_pref_e_type            mode_pref,
    /* mode preference at the time call origination request was made
    */

  cm_mode_pref_e_type            favored_mode_pref,
    /* computed favored mode preference for the call
    */

  cm_hybr_pref_e_type            favored_hybr_pref
    /* computed favored hybrid preference for the call
    */

)
{

  CM_MSG_HIGH("Fav pref for origination:",0,0,0);
  #ifdef CMDEBUG_STRING_MAPS_ACTIVE
  cmdbg_print_message("- call_type =%d(%s)",
                   call_type,
                   cm_call_type_e_type_string_map[ call_type]);

  cmdbg_print_message("- srv_type = %d(%s)",
                   srv_type,
                   cm_srv_type_e_type_string_map[srv_type]);

  cmdbg_print_message("- mode pref = %d(%s)",
                   mode_pref,
                   cm_mode_pref_e_type_string_map[mode_pref]);

  cmdbg_print_message("- fav mode pref = %d(%s)",
                   favored_mode_pref,
                   cm_mode_pref_e_type_string_map[favored_mode_pref]);

  cmdbg_print_message("- fav hybr pref = %d(%s)",
                   favored_hybr_pref,
                   cm_hybr_pref_e_type_string_map[favored_hybr_pref]);
  #else
  cmdbg_print_message("- call_type =%d", call_type);
  cmdbg_print_message("- srv_type = %d", srv_type);
  cmdbg_print_message("- mode pref = %d", mode_pref);
  cmdbg_print_message("- fav mode pref = %d", favored_mode_pref);
  cmdbg_print_message("- fav hybr pref = %d", favored_hybr_pref);
  #endif

}
/*lint -restore */
