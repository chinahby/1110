#ifndef CMCALL_H
#define CMCALL_H
/*===========================================================================

         C A L L   M A N A G E R   C A L L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMCALL.C


Copyright (c) 1991 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmcall.h#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/09   aj      Adding support for FEATURE_OTASP_STATUS
05/27/09   aj      Adding support for passing otasp status to clients
03/09/09   rm      Made cmcall_misc_get_common_mode_pref non-static
02/25/09   sv      Correct Lint errors
02/12/09   mh      Added support for no SR after MAP for IS-707B
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
11/13/08   aj      Added API cmcall_end_each_call_with_type
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
10/15/08   rn      Corrected max line chars
08/05/08   aj      Added api cmcall_check_for_call_state_on_ss()
06/06/08   rn      Added cmcall_overall_ps_call_end()
06/06/08   sv      Removing unneccesary snd.h
05/28/08   ks      Updating the associated primary profile number when 
                   secondary PDP is promoted to Primary
04/29/08   ks      Reverting the changes of Updating the associated primary
                   profile number when secondary PDP is promoted to Primary
03/31/08   ks      Updating the associated primary profile number
                   when secondary PDP is promoted to Primary
03/19/08   pk/vs   Corrected CMCALL_IS_HDR_ORIG_ALLOWED, lint suppression
03/05/08   vk      corrected code for HYBR_GW configuration
12/13/07   sk/ak   Added DCM.
11/26/07   sv      Removing Feature FEATURE_GW_CNAP_CODING_SCHEME
11/22/07   sg      Adding is_network_initiated field to gsm_wcdma_cmcall_type.
11/05/07   sg/vs   Renaming FEATURE_CM_TEMP_NAS_DEP_TBD To
                   FEATURE_GW_CNAP_CODING_SCHEME
09/10/07   rn      Added enc_alpha to struct cmcall_s_type to send
                   coding scheme along with string
06/22/07   sk      Added macros for easily looping through call objects.
06/21/07  pk/cl    Added support for power suspend
04/17/07   rp      Making cmcall_get_favored_srv_domain_pref() as extern
03/09/07   sk      RAM usage reduction.
02/02/07   pk      Featurizing cmcall_get_voip_call_id under FEATURE_IP_CALL
01/24/07   pk      Fixed compiler warning
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
11/21/06   ka      Adding rab_id_present field to call object.
11/16/06   pk      Lint Fixes
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
10/03/06   ka      Adding support for IP call on WCDMA
09/14/06   ka      Adding support for call control on secondary ps calls
08/28/06   ka      Adding pdp header comp and data comp enums.
08/25/08   ka      Adding support for delaying setup response to let QTV
                   PAUSE streaming session.
08/16/06   ka/rp   Adding aoc_ready to Phone charge struct.
08/11/06   pk      Added support for UOOS
08/11/06   ka      Modifying cmcall_check_if_gw_cs_call_active() to check
                   for calls in any state.
08/08/06   sk      Added CMCALL_ACT_FORCE_NON_IP.
                   Added cmcall_remove_mode_and_force().
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
07/07/06   pk      Removed cm_hold_cmd_handle and cmcall_add_cmd_to_hold_q
07/04/06   ka      Changes to support call control on PS calls.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
06/05/06   ka/ic   Externalizing cmcall functions.
04/14/06   pk      Added support for CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT
04/03/06   pk      If HDR starts throttling during a Silent Redial, give it
                   one more chance
                   Once the system is throttled, don't send call end
                   to clients immediately, buffer the call end for 1 second
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
01/13/06   ka      Bringing in CMCALL_CALL_ID_HOLD_FATAL_ERR_TIME.
12/09/05   ic      Lint cleanup
11/17/05   pk      Added cmcall_sms_status_e_type and sms_status in the call
                   object
11/08/05   sk      Separated CM reports and commands.
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
                   moved cm_ss_e_type to cmi.h
09/12/05   pk      Added cmcall_pd_status_e_type, and GPS session related
                   changes
09/10/05   pk      Added FEATURE_HYBR_HDR_REDIAL_WAIT_ACQ_FAIL to decide
                   CMCALL_SR_HYBR_MAX_HDR_DIALS
08/15/05   pk      Added support for priority Queues for origination
                   parameters
08/16/05   ka      Exporting cmcall_is_voice_over_hdr_poss()
08/12/05   ka      Functions to check voice/emerg in origination and decide
                   on call types.
07/30/05   ka      Removed declaration of cmcall_set_call_params()
                   Added is_ccbs_recall_resp field to gsm_wcdma_cmcall_type
07/29/05   dk      Added CMCALL_ACT_WAIT_OTHER_END action to wait for some
                   other activity to end during silent redial.
07/21/05   pk      Modified the printing of Silent redial count and the
                   loging of the redial event. Moved the Silent redial
                   related functions to cmcall.c
07/21/05   sk      Removed internal use of is_jcdma_emergency flag.
07/14/05   ic      Removed is_user_originated_call from cdma_cmcall_type as
                   it is aready present in cmcall_s_type
07/08/05   pk      Changed redial action table for pkt data calls
                   modified CMCALL_SR_HYBR_MAX_HDR_DIALS CMCALL_SR_MAX_DIALS
                   for JCDMA builds. These changes are part of the silent
                   redial enhancements
07/08/05   ic      Report highest layer call origination failure as part of
                   call events.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/21/05   dk      Added cmcall_is_active_on_mode() to find if there is any
                   call on a given mode.
06/09/05   ic      Removed all varieties of EJECT comment
                   Cleaned up cmcall_cmd_copy_fields()
06/06/05   pk      Added CMCALL_ACT_REDIAL_HDR_THR, redial action for call
                   throttle
05/19/05   ka      Merging CIPHER_IND feature.
05/04/05   jqi      Fixed for hybr data switch over.
03/14/05   ka      Adding ipapp_id and ipapp_data_block to call obj
03/07/05   ic      Fixed ARM ADS compiler warnings
03/02/05   ic      Removed code under FEATURE_6500_4X (obsolete)
02/24/05   dk      Added cmcall_count_on_ss() function to address CR 57350.
02/18/05   sj      Adding new redial action - REDIAL_SHORT_SAME_SR.
02/14/05   pk      Added fields to store the line control information
02/01/05   dk      Added WLAN Support.
12/19/04   ic      Added cmcall_info_get()
12/02/04   dk      Moved Call Substate Enums to cm.h
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/19/04   ka      Adding support for user to user signaling
11/18/04   ka      Adding support for MT PDP
10/21/04   ka      Adding CMCALL_PS_DATA_SR_MAX_TIME to increase silent
                   redial time for ps calls and incr MAX_DIALS to 100
09/14/04   ic      Call direction clean up - removed direction field from
                   gsm_wcdma_cmcall_type
09/07/04   ka      Removed mode_in_use, is_orig_sent and added orig_sent_on_mode
08/30/04   sj      Added orig_mode in call object.
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/19/04   dk      Added call_ptr as parameter to
                   cmcall_determine_ss_for_origination().
08/18/04   ka      Call direction now is common to system modes.
                   Setting call direction using a function.
08/04/04   dk      Added support to hold Packet Data Call origination in HDR
                   if SMS/DBM is in progress.
07/28/04   ka      Added support for secondary PDP context
07/21/04   sj      Added support for umts to 1x handover.
07/12/04   ic      Added support for FRATURE_ALS
07/06/04   dk      Modified signature of cm_find_connection_id().
07/06/04   ws      Add field to call object to indicate waiting for hdr call end.
07/05/04   dk      Changed signature of cmcall_update_gsdi_and_log_call_state().
07/01/04   dk      Added Call Object Iterator.
06/30/04   ic      Fixed obvious RVCT 2.1 compiler warnings (Jaguar build)
06/25/04   dk      Modified call object so that call_obj_array just contains
                   the pointer to the call object. Moved all call relevent
                   information inside the call object.
                   Removed explicit call object allocation/de-allocation.
                   Now it is done as part of call_id allocate/deallocate.
                   Modified in general to support this new behavior.
                   Moved certain generic call obj array walk through functions
                   from cmxcall.c
06/22/04   ic      Comments clean up
06/16/04   ic      Restored cmcall_event() to take const pointer to call object.
                   Added prototype of cmcall_update_call_end_info()
06/15/04   dk      Removed overall_state from cm_overall_calls_s_type. Removed
                   cmcall_update_overall_state() and added
                   cmcall_update_gsdi_and_log_call_state().
06/10/04   dk      Added cmcall_check_each_call_in_call_state function.
06/03/04   ws      Add support for redirecting number.
06/02/04   ka      Added cmcall_check_call_type_in_call_state ()
06/01/04   npr     Added IP Header and Data compression requirements to GW PS
                   call origination and information structures for GPRS SNDCP.
05/27/04   ic      Fixed featurization errors so it compiles without
                   FEATURE_HDR defined.
05/20/04   ic      Put CMCALL_SR_MAX_DIALS = 4 under FEATURE_6500_4X
05/19/04   ic      Put #ifdef around CMCALL_SR_MAX_DIALS definition
04/08/04   ws      Merge from MSMSHARED_CM.01.00.32.00.03
04/01/04   ws      Added cmcall_ptr_callobject_allocate function.
03/31/04   ic      Mainlined FEATURE_REX_DYNA_MEM in CM and unit test.
                   Removed all code related to static allocation of call
                   objects.
03/26/04   ka      Added a new function for processing CMCALL_ACT_PROC_END_REASON
03/15/04   ka      Merge changes from 6250 branch
                   Reject PS data call origination when MB is in GSM voice call.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/12/04   ic      Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/06/04   ws      Initial jaguar/mainline merge.
01/29/04   ws      Added function for handling gsdi events in both modes.
01/19/04   ka      Call id and profile no support in packet call structure. New
                   fields to account for call conenct time, call end time and data traffic
                   in cmcall_s_type. Headers for call info list and data session reg.
01/13/04   ka      Merged changes from MSM6200
01/09/03   ws      Take a slot number when sending gstk voice envelopes.
12/11/03   ic      Replaced is_hybr_redialled with is_hybr_redialed
12/08/03   ws      Mainlined umts concurrency services feature.
11/14/03   ws      Added support for is_jcdma_emergency
10/20/03   ws      Dual Slot Support.
10/01/03   ws      Merged from GW Targets
09/23/03   ws      Merged fix for SIM fails during emergency call
                   (do not end the call).
08/26/03   ws      Removed RelA featurization for flash type.
08/21/03   ws      Added changes for automatic mode and silent redial merge.
08/13/03   prk     Added minimum_qos field to gsm_wcdma_cmcall_ps_type.
07/08/03   vt      Added sys mode to call object structure.
04/29/03   sun     Added new cc_reject in gsm_wcdma_cmcall_type
04/11/03   prk     Removed references to FEATURE_SUPS and FEATURE_AOC.
02/27/02   cs      Added MT/MO indication and transaction ID in CS GSM/WCDMA
02/25/03   RI      Added cmcall_check_if_cs_voice_calls_active() and
                   cmcall_check_if_ps_call_active() prototypes.
02/24/03   vt      Modified cm_ss enum type to use SD enums.
02/18/03   vt      Fixed problems to build for HDR.
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/03/03   ri      Added support for concurrent services.
           cs      Updated silent redial for WCDMA/GSM.
01/21/03   ws      Updated copyright information for 2003
12/17/02   vt      Defined fixed numbers to an enums for event logging.
                   Added system selection structure.
11/14/02   vt      Merged changes from GW mainline.
                   [PRK]Added FEATURE_UI_CM_NON_SVD.
                   [ws]Merged replaced is_orig_fail_l2 with orig_fail_layer in sr ^M
                   structure.
                   [CS]Added cmcall_obj_s_type definition.
                   Added support for UMTS silent redial for emergency call for GSM/WCDMA.
                   [prk]Added send_call_event_end field to cmcall_s_type.
                   [RI]Added ss_info field in gsm_wcdma_cmcall_type.
10/28/02   vt      Removed handling of NDSS origination failure.
10/22/02   ic      Under FEATURE_JCDMA_CM set CMCALL_SR_MAX_TIME to 40 sec.
10/22/02   vt      Created a function to return call_id from call object ptr.
                   Removed is_allocated and call id from call object for
                   dynamic memory allocation.
10/22/02   vt      Removed cmd check function that is not used.
10/09/02   vt      Added mode param to cmcall_callobject_allocate().
09/30/02   RI      Added ss_info field in gsm_wcdma_cmcall_type.
09/11/02   RI      Added redirect_party_number field in gsm_wcdma_cmcall_type.
08/30/02   vt      Added calling number to call object.
08/12/02   RI      Added ect_info field in gsm_wcdma_cmcall_type.
08/02/02   ATM     Fixed enum so it matches for logging no matter what FEATURE
                   is on/off.
07/29/02   RI      Added cause_of_no_cli to gsm_wcdma_cmcall_type.
07/10/02   PRK     Removed prototype for cm_call_id_allocate and
                   cm_call_id_deallocate.  They are now defined in cmll.h.
06/14/02   AT      Removed state added for silent redial.
06/11/02   RI      Added cmcall_object_deallocate() to deallocate a
                   call object.
                   Renamed cm_new_call_id() to cm_new_connection_id().
                   Deleted call object pointer parameter in
                   cmcall_client_cmd_check().
05/05/02   vt      Fixed compile problem with new mm cm.h
04/09/02   sh      Added is_hdr_not_avail and removed cmcall_is_in_hdr_call
03/13/02   sh      Renamed and modified some HDR related macros
03/10/02   sh      Added macros for checking HDR and NonHDR call orig/call status
02/06/02   sh      Added hybrid 3.0 support
01/17/02   ic      Added to the comment below a note about the fix for CR#17975
01/12/02   az      Added 2 variables that keep track of Roam preference(for SD2.0)
                   and Roam indicator(for SD1.0) when the call was originated.
                   Fixed CR#17975
12/08/01   sh      Added preliminary hybrid operation support
05/05/02   vt      Fixed compile problem with new mm cm.h
02/06/02   PRK     Added function cmcall_overall_call_state.
02/04/02   RI      Added new states in overall_call_state_e_type
02/13/02   ic      Changed the type of call_id variable from byte/uint8 etc
                   to cm_call_id_type as it should have been to begin with.
01/29/02   ic      Removed the restriction of one voice + one data call
                   (under FEATURE_SVD_CM_CP_INTEGRATION)
01/23/02   ic      Fixes related to running with FEATURE_GPSONE_* enabled
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
01/11/02   ic      Made is_voice() function global (i.e. independed of any
                   feature)
01/03/02   ic      Added ACCT (Access Control based on Call Type) support
                   Added silent redial support for
                   CM_CALL_END_NO_RESPONSE_FROM_BS call end reason.
12/20/01   ic      Under FEATURE_SVD_CM_CP_INTEGRATION, added support
                   for restricting the CM to one voice and one data call
                   at a time as this is what CP code currently supports.
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
11/30/01   sh      Added access network authentication support
11/17/01   sh      Added 1xhdr data session handoff support
11/01/01   sh      Fixed various bug/deficiencies found from code review
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/23/01   VT/IC   Baseline for CCS
08/21/01   ic      Main-lined FEATURE_NEWCM
05/25/01   CK      Changed FEATURE_RUIM to FEATURE_UIM_RUIM
04/13/01   SH      Added Preliminary HDR support
03/08/01   RC      Added CMCALL_ACT_REDIAL_SHORT to cmcall_act_e_type.
03/06/01   HQ      Added support for FEATURE_IS2000_REL_A.
02/07/01   RC      Added support for System Determination 2.0.
01/23/01   ych     Merged T53 and JCDMA features.
12/28/00   fc      Added support for retry order.
12/15/00   HQ      Added privacy_pref field.
10/18/00   HQ      Added ruim_last_poll_time in cmcall_s_type.
10/06/00   HD      Added support to pass the DRS bit setting to CP.
08/23/00   vt      Added new CM interface changes.
07/18/00   JQ      Added support for end_reason
03/20/00   RY      Removed cm_flush_hold_cmd_q()
03/13/00   RY      Added OTAPA support
12/13/99   IT      Added handling for next data call as data.
12/03/99   VT      Merged TriMode Code.
11/04/99   SH      CR11880 - Fixed a problem with NDSS.
10/19/99   SH      Fixed CR11844 - to NDSS connect or not to UI
09/01/99   SH      Featurized OTASP
08/31/99   SH      Added cmcall_end
08/25/99   SH      Added cmcall_offline
08/19/98   SH      Increased act_tbl size
08/09/99   SH      Added alert answer bypass feature
07/07/99   SH      Added CNAP feature
06/30/99   SH      Changed type of otasp_act_code to cm_activate_code_type
05/31/99   SH      Added CM 2.0 features
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/


#include "comdef.h"    /* Definition for basic types and macros */
#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"       /* Internal interface to cm.c */
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#include "cmxll.h"      /* MC to CM interface*/
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif

#include <stdarg.h>

#define CALL_MODE_INFO_PTR( xx_call_ptr ) ( &xx_call_ptr->cmcall_mode_info.info.cdma_info )

#define CMCALL_GREP_ALL  1
#define CMCALL_GREP_STOP (end_grep = TRUE)

#define CMCALL_FOR_EACH_CALL_DO                           \
/*lint -e{717} */                                         \
do                                                        \
{                                                         \
  cmcall_s_type    *this_call;                            \
  cm_iterator_type  grep_call_itr;                        \
  boolean           end_grep = FALSE;                     \
                                                          \
  cmcall_obj_iterator_init(&grep_call_itr);               \
                                                          \
  this_call = cmcall_obj_get_next(&grep_call_itr);        \
                                                          \
  while(this_call != NULL && !end_grep)                   \
  {


#define CMCALL_END_FOR                                    \
    /*lint -e{725} Ignore indentation */                \
    this_call = cmcall_obj_get_next(&grep_call_itr);      \
  }                                                       \
}while(0);

/* This macro executes a set of statements for each call object when the
** condition 'cond' is true. The call object can be referenced as 'each_call'.
*/
#define CMCALL_GREP_AND_DO(cond,statements)                       \
          CMCALL_FOR_EACH_CALL_DO                                 \
            if(cond)                                              \
            {                                                     \
              statements;                                         \
            }                                                     \
          CMCALL_END_FOR


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define CM_DUMMY_CALL_ID  0xFF

/* unknown connection ID */
#define CM_UNKNOWN_CONN_ID 0xFF

/* profile number used for ps calls */
#define CMCALL_PROFILE_NUM_INVALID 0xFF

//#define CMCALL_MAX_CALL_OBJ      CM_CALL_ID_MAX

/* This time is the wait time for CM when a 1x voice origination happens
   when an HDR call is in progress.  CM should end the HDR call, wait 2 seconds
   for everything to clean up before sending a origination down to MC.
*/
#define CMCALL_HDR_WAIT_ORIG_UPTIME           2   /* seconds */

#define CMCALL_UPTIME_NONE   (dword)(0)

#define CMCALL_CALL_ID_HOLD_FATAL_ERR_TIME        5    /* seconds */
    /* Max time to hold from error fataling if CM can no longer allocate
    ** anymore call ids.
    */


/*---------------------------------------------------------------------------
                     TUNABLE SILENT REDIAL PARAMETERS
---------------------------------------------------------------------------*/


/* Silent redial feature to silently redial failed calls or to ride through
** limited periods of no service.
**
** In addition, silent redial can be configured to forced the mode
** preference over to analog if such service available and if calls fail
** under poor CDMA RSSI conditions. This force-over to analog is temporary,
** ending at call teardown.
**
** NOTE! generally the following relation should be maintained
** for the silent redial parameters (assuming MAX_DIALS >= 1):
**
** ((MAX_DIALS-1) * PERIOD_TIME) <= MAX_TIME
*/

#define CMCALL_SR_MAX_DIALS                  100
    /* Max number of dials per explicit origination attempt.
    **
    ** NOTE! This number includes the first explicit dial attempt
    ** and therefore should always be > 0.
    ** Changing it to large value since silent redial
    ** can happen for 75 secs on packet side
    */


#define CMCALL_SR_PERIOD_TIME                4  /* seconds */
    /* Time from origination failure to next silent redial attempt */


#ifdef FEATURE_JCDMA_CM
#define CMCALL_SR_MAX_TIME                   40  /* seconds */
    /* Maximum silent redial sequence time per explicit origination attempt.
    **
    ** Note that this time designate a limit for the last silent redial
    ** attempt to be launched. This last origination attempt might fail or
    ** be connected after this max time is expired */
#else
#define CMCALL_SR_MAX_TIME                   30  /* seconds */
    /* Maximum silent redial sequence time per explicit origination attempt.
    **
    ** Note that this time designate a limit for the last silent redial
    ** attempt to be launched. This last origination attempt might fail or
    ** be connected after this max time is expired */
#endif

#define CMCALL_PS_DATA_SR_MAX_TIME           75  /* seconds */
    /* GPRS attach procedure is governed by T3310 and T3311 timers.
    ** At the expiry of T3310 (15secs) after a PS attach request is
    ** sent 4 more retries are made at an interval of 15 secs. In
    ** total when PS attach fails after 75 secs T3311 is started.
    ** Comparitive study finds PS attach should be tried for atleast
    ** 75 secs.
    **/


#define CMCALL_SR_FORCE_AMPS_RSSI_THR        100  /* -dB */
    /* During silent redial origination force over to AMPS service
    ** if CDMA RSSI is below this threshold */

#define CMCALL_ORIG_THR_HOLD_UPTIME          1 /* Second */
    /* Time to hold the call end to clients, this is used to flow control
    ** the clients so that they don't queue too many call originations to CM
    */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Max number of HDR origination attempts allowed in hybrid mode.
**
** NOTE! This number includes the first explicit dial attempt
** and therefore should always be > 0.
*/

#if ( defined (FEATURE_JCDMA) || defined (FEATURE_HYBR_HDR_REDIAL_WAIT_ACQ_FAIL) )
#define CMCALL_SR_HYBR_MAX_HDR_DIALS CMCALL_SR_MAX_DIALS
#elif (defined FEATURE_NUM_HYBR_HDR_REDIALS_THREE)
#error code not present
#elif (defined  FEATURE_NUM_HYBR_HDR_REDIALS_TWO)
#error code not present
#elif (defined FEATURE_NUM_HYBR_HDR_REDIALS_ONE)
#error code not present
#else
#define CMCALL_SR_HYBR_MAX_HDR_DIALS         2
#endif


/* Enumeration of call origination action.
**
** When call origination fails (due to no-service, fade, reorder, intercept
** release, etc.) the call origination specifies which one of the following
** action should be invoked.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cmcall_act_e {

    CMCALL_ORIG_ACT_NONE                   = 0,  /* No action should be taken */
    CMCALL_ACT_END_ORIG                    = 1,  /* End the origination process */
    CMCALL_ACT_FORCE_AMPS                  = 2,  /* Force AMPS */
    CMCALL_ACT_FORCE_AMPS_RSSI             = 3,  /* Force AMPS if RSSI is low */
    CMCALL_ACT_REDIAL                      = 4,  /* Schedule next SR */
    CMCALL_ACT_FORCE_AUTO                  = 5,  /* Tri-mode */
    CMCALL_ACT_FORCE_AUTO_RSSI             = 6,  /* Tri-mode */
    CMCALL_ACT_REDIAL_SHORT                = 7,  /* Schedule next SR, shorten
                                                 ** if OK to orig
                                                 */
    CMCALL_ACT_WAIT_OK_TO_ORIG             = 8,  /* Wait for ok-to-orig
                                                 ** indication (SD 2.0)
                                                 */
    CMCALL_ACT_HYBR_REDIAL                 = 9,  /* Hybrid redial on either
                                                 ** CDMA or HDR
                                                 */
    CMCALL_ACT_PROC_END_REASON             = 10, /* Process END reason for WCDMA/GSM */
    CMCALL_ACT_FORCE_REDIAL                = 11, /* redial without checking for SRLeft */
    CMCALL_ACT_FORCE_NON_WLAN              = 12, /* Force Non-WLAN */
    CMCALL_ACT_REDIAL_SHORT_SAME_SR        = 13, /* Short redial without checking
                                                 ** for SRLeft no, but not
                                                 ** decrmenting SRLeft.
                                                 */
    CMCALL_ACT_REDIAL_HDR_THR              = 14, /* Redial on CDMA, due to
                                                 ** throttling on HDR
                                                 */

    CMCALL_ACT_WAIT_OTHER_END              = 15,  /* Wait for other activity to end */


    CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT       = 16, /* Wait for ok-to-orig
                                                 ** indication (SD 2.0)
                                                 ** the wait time is 4sec
                                                 */

    CMCALL_ACT_FORCE_NON_IP                = 17, /* Force to Non-IP (1x/GW)*/

    CMCALL_ORIG_ACT_MAX

} cmcall_act_e_type;


/* Enum of PD status
*/

typedef enum
{

  CMCALL_PD_STATUS_NONE = -1, /* FOR INTERNAL USE OF CM ONLY! */

  CMCALL_PD_STATUS_HOLD_ORIG, /* Hold the origination, GPS needs to clean up*/

  CMCALL_PD_STATUS_END_ORIG,  /* End the origination */

  CMCALL_PD_STATUS_MAX        /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
#error code not present
  #endif
}cmcall_pd_status_e_type;


/* Enumeration of SMS status for the call.
*/

typedef enum
{

  CMCALL_SMS_STATUS_NONE = -1, /* FOR INTERNAL USE OF CM ONLY! */

  CMCALL_SMS_STATUS_HOLD_ORIG, /* Hold the origination, SMS is waiting for confirmation */

  CMCALL_SMS_STATUS_MAX        /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
#error code not present
  #endif
}cmcall_sms_status_e_type;


/* Enumeration of Silent redial count actions.
*/
typedef enum {

  CM_SR_ACT_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */

  CM_SR_ACT_RESTART,    /* Restart silent redial left count */
  CM_SR_ACT_DEC,        /* Decrement silent redial left count */
  CM_SR_ACT_NO_CHANGE,  /* don't change silent redial left */

  CM_SR_ACT_MAX         /* FOR INTERNAL USE OF CM ONLY! */

} cm_sr_act_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for call origination plan.
** Holds all the information that is associated with a particular
** origination plan.
*/

typedef struct cmcall_action_table_entry_s {

    /* orig fail status */               /* orig action to take */
    cm_call_end_e_type end_status;
            /* Origination end status - no-service, fade, reorder, etc */

    cmcall_act_e_type  action;
            /* Action to take - redial, force-AMPS, etc */

} cmcall_action_table_entry_s_type;

typedef struct cmcall_orig_plan_action_table_s {

   unsigned int                        action_table_entry_count;

   cmcall_action_table_entry_s_type   *action_table_entry;

} cmcall_orig_plan_action_table_s_type;

typedef struct cmcall_orig_plan_s {
    /*                                    */
    /* condition for the origination plan */
    /*                                    */
    cm_feature_e_type    sr_with_amps;
        /* Is this origination plan is for when SR with AMPS feature
        ** is turned ON, OFF, or don't care (NONE=don't care) */

    cm_call_type_e_type  call_type;
        /* What call type is this origination plan for (NONE=don't care) */

    byte                 *dial_str;
        /* What dial string is this origination plan for (NULL=don't care) */

    boolean              is_hybr_allowed;
        /* whether or not packet data call is allowed to originate on
           either HDR or CDMA when the hybrid operation is enabled */

    cm_call_mode_info_e_type call_mode_info;
        /* Used to differentiate betweeb mode info, required to GW_PS calls
        */
    /*                                    */
    /*    Origination plan                */
    /*                                    */


    boolean              is_honor_mc_hold_orig;
        /* Indicate whether we should honor MC hold-orig during the
        ** origination process */

    dword                pwrup_hold_orig_time;
        /* Power-up hold orig uptime - i.e. at power-up how long we should
        ** hold the origination, giving service chance to appear */

    dword                sr_period_time;
        /* Silent redial off period - i.e. time from origination failure
        ** to next redial attempt */

    unsigned int         sr_max_dial;
        /* Max numbers of dial attempts */

    dword                sr_max_time;
        /* Max time for silent redial sequence */


    /*                                                                */
    /* Action table indicating action to take when call origination   */
    /* fails due to no-service, fade, reorder, intercept, release     */
    /* or calls being selectively blocked by the base station         */
    /*                                                                */
    cmcall_orig_plan_action_table_s_type *action_table;

} cmcall_orig_plan_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Silent redial type - holds all the variables that are
** associated with the silent redial mechanism.
*/
typedef struct cm_sr_s {

    const cmcall_orig_plan_s_type  *orig_plan_ptr;
        /* Pointer to call origination plan
        */

    dword                          sr_start_uptime;
        /* Silent redial sequence start uptime
        */

    dword                          sr_max_uptime;
        /* Silent redial sequence max uptime
        */

    boolean                        is_ok_to_orig;
        /* Indicate whether CM_OK_TO_ORIG_F was received
        */

    boolean                        hdr_is_ok_to_orig;
        /* Indicate whether CM_OK_TO_ORIG_F was received
        ** for the HDR system when the hybrid operation
        ** is enabled
        */
    boolean                        is_hdr_not_avail;
        /*  Indicate hdr can not be used for
        **  next silent redial because there is no
        **  HDR system
        */

    sys_sys_mode_e_type            orig_sent_on_mode;
        /* Records the system mode on which origination
        ** gets sent down. If origination is rejected
        ** and redial needs to be performed it is reset
        ** to None
        */

    dword                          hold_orig_uptime;
        /* Hold origination until this uptime
        */

    dword                          next_orig_uptime;
        /* Try the next origination at this uptime
        */

    dword                          sr_left;
        /* Indicate how many silent redial left
        */

    #if ( defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900) )
    boolean                        sr_tried;
        /* Indicates whether silent redial was tried even once
        */
    #endif

    #ifdef FEATURE_IS2000_REL_A
    boolean                        is_re_orig;
        /* whether the current origination is a re-origination
        */

    byte                           re_orig_count;
        /* number of re-originations due to layer 2 failure
        */

    boolean                        re_orig_reason;
        /* indicates if re-origination is due to layer 2 failure
        */

    cm_call_orig_fail_layer_e_type orig_fail_layer;
        /* Layer where the origination failed
        */
    #endif /* FEATURE_IS2000_REL_A */

    boolean                        is_hybr_redialed;
        /* Indicates if the call was hybrid redialled, i.e attempted
        ** first on hybrid HDR and then redialled on 1x.
        */

    boolean                        wlan_is_ok_to_orig;
        /* Indicate whether CM_OK_TO_ORIG_F was received
        ** for the HDR system when the hybrid operation
        ** is enabled
        */
    #ifdef FEATURE_CALL_THROTTLE
    boolean                        is_call_throttled;
        /* indicates if the call is throttled
        */
    #endif


} cm_sr_s_type;


/* Handover redial type - holds all the variables that are
** associated with the Handover redial mechanism.
*/
typedef struct
{


  dword                          hr_start_uptime;
      /* Handover redial sequence start uptime */

  dword                          hr_max_uptime;
      /* handover redial sequence max uptime */


  boolean                        is_ho_req_sent;
     /* Indicate whether Handover request command was sent to MC */


} cm_handover_retry_s_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Forward declarations to pacify the compiler.
*/
typedef struct cmph_s   *cmph_p_type;     /*lint -esym( 761, cmph_s_type ) */
typedef struct cmcall_s *cmcall_p_type; /*lint -esym( 761, cmcall_s_type ) */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* OTASP call types
*/
typedef enum cmcall_otasp_call_e
{
    CMCALL_OTASP_NONE          /* Regular call other than OTASP */
  , CMCALL_OTASP               /* User initiated service provisioning */
  , CMCALL_PC_OTAPA            /* Network initiated service provisioning
                               ** by paging the mobile station with SO 18/19
                               */
  , CMCALL_BUSY_OTAPA          /* Network initiated service provisioning
                               ** while the mobile station is already in
                               ** CDMA conversation state
                               */
} cmcall_otasp_call_type;


/* Type for OTASP/OTAPA Information.
*/
typedef struct cmcall_otasp_info_s {

    cmcall_otasp_call_type     otasp_call;
        /* What type of OTASP/OTAPA call */

    cm_activate_code_e_type    otasp_act_code;
        /* OTASP activation code (for OTASP calls) */

    cm_otasp_status_e_type     otasp_status;
        /* Indicate status of OTASP/OTAPA call */

    boolean                    is_commit_successful;
        /* Indicate whether OTASP/OTAPA activation is completed */

    boolean                    is_wait_for_commit_reply;
        /* Indicate whether we need to wait for OTASP-STATUS from MC
        ** at end of call */

    cm_reply_s_type            commit_reply;
        /* CM reply object to handles the details that are associated
        ** with waiting for an otasp commit status reply from MC */

} cmcall_otasp_info_type;


/* Enumeration of overall call states. At all times a CM call must be in one
** of the states below. This enum is used by logging!! Please notify
** cfa.team of any changes made.
*/
typedef enum cm_overall_call_state_e {

    CM_OVERALL_CALL_STATE_NONE=-1,      /* FOR INTERNAL USE OF CM ONLY! */

    CM_OVERALL_CALL_STATE_IDLE=0,       /* Call is in idle state - i.e. no call */
    CM_OVERALL_CALL_STATE_ORIG=1,       /* Call is in origination state */
    CM_OVERALL_CALL_STATE_INCOM=3,      /* Call is in alerting state */
    CM_OVERALL_CALL_STATE_ORIG_INCOM=4, /* There is an originating call as well
                                           as incoming
                                        */
    CM_OVERALL_CALL_STATE_CONV=5,       /* Call is in conversation state */
    CM_OVERALL_CALL_STATE_RELEASE=6,    /* Call is being released*/
    CM_OVERALL_CALL_STATE_RECALL_RSP_PEND=7, /* Recall rsp is pend on call */

    CM_OVERALL_CALL_STATE_MAX           /* FOR INTERNAL USE OF CM ONLY! */

} cm_overall_call_state_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */



#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

typedef struct
{
    word                       srv_opt;
        /* service option */

    cm_privacy_pref_e_type     privacy_pref;
        /* Indicate the user preference of privacy mode */

    boolean                    is_privacy;
        /* Indicate whether privacy mode is on */

    cmcall_otasp_info_type     otasp_info;
          /* Contains the necessary information for OTASP/OTAPA calls */

    boolean                    is_last_cdma_info_rec;
        /* this field indicates whether this is the last
           CDMA info record or not */

    #ifdef FEATURE_IS95B_EXT_DISP
    byte                        ext_disp_type;
        /* display type for extended disaply info record */
    #endif

    #ifdef FEATURE_IS95B_ALERT_ANS_BYPASS
    boolean                    alert_ans_bypass;
        /* Indicate whether the incoming call need to be answered by the clients */
    #endif /* FEATURE_IS95B_ALERT_ANS_BYPASS  */

    #ifdef FEATURE_IS95B_NDSS
    boolean                     ndss_in_progress;
    /* MC is perfoming NDSS redirection */
    boolean                     ndss_connect;
    /* send NDSS connect instaead of connect */
    #endif

    #ifdef FEATURE_IS2000
    byte end_reason;
      /* the reason for ending a call, it's different from end_status to keep
       * the old interface unchanged while adding more IS2000 specific reasons
       * */
    boolean                     drs_bit;
      /* to set the DRS bit or not */

    cm_retry_order_s_type         retry_order;
      /* retry order info */

    cm_call_id_type             emergency_flash_call_id;
      /* The call used in sending emergency flash */

    boolean                     emergency_flash_over_incoming_call;
      /* whether a previous emergency request was flashed on this
      incoming call or not */
    #endif /* FEATURE_IS2000 */

    cm_call_sups_type_e_type      flash_type;
      /* flash type */

    #ifdef FEATURE_T53
    cm_ext_brst_intl_s_type       ext_brst_intl_msg;
    /* Contains charge rate information */
    cm_nss_clir_rec_s_type        nss_clir_rec;
    /* National Supplementary services - CLIR */
    cm_nss_aud_ctrl_s_type        nss_aud_ctrl_rec;
    /* National Supplementary services - Audio Control */
    #endif /* FEATURE_T53 */

    #ifdef FEATURE_IS2000_REL_A
    boolean               qos_parms_incl;
    byte                  qos_parms_len;
    byte                  qos_parms[CAI_ORIG_QOS_LEN_MAX];
    boolean               sr_id_included;
    byte                  sr_id;
    #endif /* FEATURE_IS2000_REL_A */

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
    #endif

    /* Line control parameters
    */
    boolean                              line_ctrl_polarity_included;
      /* TRUE if polarity info included
      */

    boolean                              line_ctrl_toggle;
      /* TRUE = toggle polarity
      */

    boolean                              line_ctrl_reverse;
      /* TRUE = reverse polarity
      ** FALSE = normal polarity
      */

    byte                                 line_ctrl_power_denial;
      /* Power denial time
      */

    cm_call_orig_fail_layer_e_type       orig_fail_layer;
      /* Highest layer where the call origination setup failed.
      ** This field is adjusted after every silent redial failure.
      */

} cdma_cmcall_type;

#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


#if (defined(FEATURE_IP_CALL))
typedef struct
{
  cm_call_prog_info_e_type              call_prog_info;

} ip_cmcall_type;
#endif



typedef struct cmcall_info_s {
    #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
    #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    cdma_cmcall_type             cdma_info;
    #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

    #if defined (FEATURE_IP_CALL)
    ip_cmcall_type               ip_info;
    #endif /* defined (FEATURE_IP_CALL) */
} cmcall_info_s_type;


typedef struct
{
    cm_call_mode_info_e_type     info_type;
    cmcall_info_s_type           info;

} cmcall_mode_info_type;



/* Call Type - holds all the information that is associated with
** a specific call.
*/
typedef struct cmcall_s {

    cm_init_mask                       init_mask;
      /* Indicate whether this object was properly initialized
      */

    cm_call_id_type                    call_id;
      /* call ID of this call Object
      */

    cm_call_state_e_type               call_state;
      /* Call state - idle, orig, etc.
      */

    cm_call_substate_type              call_subst;
      /* Call substate.
      */

    cm_client_s_type                   *call_client_ptr;
      /* Call client - i.e. originating/answering client
      */

    cm_call_type_e_type                call_type;
      /* Call type - VOICE, SMS, E911, etc
      */

    cm_srv_type_e_type                 srv_type;
      /* service type - AMPS, CDMA, automatic, etc.
      */

    sys_ho_type_e_type                 ho_type;
      /* Flag to indicate if it is umts to 1x handover.
      */

    cm_num_s_type                      num;
      /* Number buffer:
      ** CM_CALL_EVENT_ORIG           - contains dialed digits string.
      ** CM_CALL_EVENT_FLASH          - contains dialed digits string.
      ** CM_CALL_EVENT_INCOM          - contains caller ID string.
      ** CM_CALL_EVENT_CALLER_ID      - contains caller ID string.
      ** CM_CALL_EVENT_CONNECTED_NUM  - contains connected number string.
      ** CM_CALL_EVENT_CALLED_PARTY   - contains called party string.
      **                                Note: presentation and screening indicator
      **                                      fields for called party info are not
      **                                      used
      */

    cm_num_s_type                      calling_num;
      /* Mobile number information
      */
    #ifdef FEATURE_IP_CALL
    cm_num_s_type                      waiting_num;
      /* Mobile number information
      */
    #endif
    cm_redirecting_num_s_type          redirecting_number;
      /* Number buffer:
      ** CM_CALL_EVENT_REDIRECTING_NUM          - contains redirecting num.
      */

    cm_alpha_s_type                    alpha;
      /* Alpha tag to be displayed along with dialed address
      ** of an originated call.
      **
      ** NOTE! this is not a phone book name match, but rather
      ** an indication for the UI to display some text for an
      ** outgoing call
      **
      ** We probably need to change this one to a UI text component to
      ** support I18N
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

    cm_signal_s_type                   signal;
      /* Alert signal information to be played for an incoming call
      */

    cm_call_end_e_type                 end_status;
      /* Call end status (i.e. reason for ending a call in origination,
      ** incoming or conversation state)
      */

    void (*orig_func)                  ( cmcall_p_type call_ptr );
      /* Pointer to origination processing function. Different call types
      ** might have different origination processing functions associated
      ** with them
      */

    cm_sr_s_type                       sr;
      /* Silent redial component. Holds all the information that is
      ** associated with the silent redial mechanism
      */


    #if (defined (FEATURE_UMTS_1X_HANDOVER_1XMSM) || defined(FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X))
#error code not present
    #endif

    sys_umts_cdma_handover_e_type      umts_cdma_ho_status;

    sys_ho_status_e_type               ho_status;

    cm_reply_s_type                    reply;
      /* CM reply object to handles the details that are associated
      ** with waiting for a reply from MC
      */

    cmph_p_type                        ph_ptr;
      /* Pointer to phone object. Holds all the information that is
      ** associated with the phone and its settings
      */

    boolean                            is_user_originated_call;
      /* is this a call that we are originating?
      */

    #ifdef FEATURE_UIM_RUIM
    dword                              ruim_last_poll_time;
    #endif /* FEATURE_UIM_RUIM */

    cmcall_mode_info_type              cmcall_mode_info;
    #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #ifdef FEATURE_HDR_AN_AUTH
#error code not present
    #endif
    #ifdef FEATURE_HDR_HANDOFF
#error code not present
    #endif /* FEATURE_HDR_HANDOFF */

    cm_mode_pref_e_type                favored_mode_pref;
      /* Favored mode preference to originate the call
      */

    cm_prl_pref_e_type                 favored_prl_pref;
      /* Favored prl preference to originate the call
      */

    cm_roam_pref_e_type                favored_roam_pref;
      /* Favored roam preference to originate the call
      */

    cm_hybr_pref_e_type                favored_hybr_pref;
      /* Favored hybrid preference to originate the call
      */
    cm_ss_e_type                       ss;
      /* indicate which SD's system slection info should be
      **used for the call
      */

    #if (defined( FEATURE_HDR_HYBRID ) || defined( FEATURE_HYBR_GW ))
#error code not present
    #endif

    #ifdef FEATURE_HDR_HYBRID
#error code not present
    #endif /* FEATURE_HDR_HYBRID */

    boolean                            send_call_event_end;
      /* indicate whether the call event end should be sent to the
      ** clients.
      */

    cm_end_params_s_type               end_params;
      /* end parameter information
      */

    sys_sys_mode_e_type                sys_mode;
      /* Indicates the system on which the call is made
      */

    cm_orig_call_control_s_type        cdma_cc_modified_info;
      /* Stores call control modified cdma information
      */

    dword                              call_connect_time;
      /* The time the call was connected.  The units is seconds since
      ** the phone has powered on (clock uptime).
      **/

    dword                              call_end_time;
      /* The time the call was ended.  The units is seconds since
      ** the phone has powered on (clock uptime).
      **/

    sd_ss_orig_mode_e_type             orig_mode;
      /* Indicates the current call orig. mode.
      */

    #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
    #endif

    uint32                             bytes_received;
      /* The number of bytes received during the call (PS data call only).
      ** This field is populated after the call has ended.
      */

    uint32                             bytes_transmitted;
      /* The number of bytes transmitted during the call (PS data call only).
      ** This field is populated after the call has ended.
      */

    byte                               gw_connection_id;
      /*
      ** This holds the connection Id field for the GSM/WCDMA calls.
      */

    cm_conn_id_src_e_type              gw_conn_id_src;
      /* This holds the source of the connection Id, i.e. SM or MN.
      */

    cm_als_line_e_type                 line;
      /* what line is being used for this call
      */

    cm_call_direction_e_type           direction;
      /* Holds direction of the current call.
      ** It can be Mobile originated or Mobile
      ** Terminated
      */

    cm_cc_mode_pref_e_type      main_ss_mode;

    dword                       end_req_wait_uptime;

    #if defined(FEATURE_IP_CALL)
    cmipapp_id_type             ipapp_id;
      /* Holds the IP app id the call is associated with
      */
    #endif

    uint32                      ipapp_data_block;
      /* Token created and used by registered IP apps.
      ** App gives CM this value whenever cmipapp_rpt_ind () is called.
      ** CM passes the most recent value given by app while calling
      ** app registered call back functions
      */
    sys_srv_status_e_type        prev_srv_status[CM_SS_MAX];
      /* Holds the previous service status information
      */
    boolean                      force_top_pref;
      /* Force the preferences to the top of the origination parameter Queue
      ** even if the top ptr didn't change
      */

    cmcall_pd_status_e_type      pd_status;
      /* Whether a PD session was ended, and we need to wait for some
      ** time before sending the origination down
      */

    cmcall_sms_status_e_type     sms_status;
      /* Do we need to hold orig because SMS is waiting for confirmation
      */

    boolean                      is_hdr_acq_fail;
      /* Is HDR acq failed received before for this call
      */

    boolean                      waiting_for_hdr_session_close;
      /**<
      ** Any active HDR session needs to be closed before originating this
      ** call.
      */

    boolean                      is_707b_ps_data;
      /*
      ** Is IS-707B Addendum PS Data Call
      */

} cmcall_s_type;


/* Overall Calls information - holds the information
** associated with all call objects
*/
typedef struct cm_overall_calls_s
{
    byte                  num_of_call_ids_allocated;
          /* number of call IDs allocated */

    byte                  num_of_calls_active;
          /* number of calls that are active */

    //cm_overall_call_state_e_type  overall_state;
          /* overall state of the call objects */

} cm_overall_calls_s_type;


/* Macros to check the call state and substate.
*/
#define CMCALL_IS_INCOM_OTAPA(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_INCOM) && \
   ((call_ptr)->call_subst.incom == CM_CALL_INCOM_SUBST_OTAPA))

#define CMCALL_IS_INCOM_REGULAR(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_INCOM) && \
   ((call_ptr)->call_subst.incom == CM_CALL_INCOM_SUBST_REGULAR))

#define CMCALL_IS_CONV_REGULAR(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_CONV) && \
   ((call_ptr)->call_subst.conv == CM_CALL_CONV_SUBST_REGULAR))

#define CMCALL_IS_CONV_COMMIT(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_CONV) && \
   ((call_ptr)->call_subst.conv == CM_CALL_CONV_SUBST_COMMIT))

#define CMCALL_IS_CONV_PC_OTAPA_REGULAR(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_CONV) && \
   ((call_ptr)->call_subst.conv == CM_CALL_CONV_SUBST_PC_OTAPA_REGULAR))

#define CMCALL_IS_CONV_PC_OTAPA_COMMIT(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_CONV) && \
   ((call_ptr)->call_subst.conv == CM_CALL_CONV_SUBST_PC_OTAPA_COMMIT))

#define CMCALL_IS_PC_OTAPA(call_ptr) \
  (CMCALL_IS_INCOM_OTAPA(call_ptr) || \
         CMCALL_IS_CONV_PC_OTAPA_REGULAR(call_ptr) || \
         CMCALL_IS_CONV_PC_OTAPA_COMMIT(call_ptr) )

#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */





/*===========================================================================

FUNCTION cmcall_mc_rpt_proc

DESCRIPTION
  Process MC reports


DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);



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
extern void cmcall_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
);



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
extern void cmcall_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
);



#if (defined (FEATURE_UMTS_1X_HANDOVER_1XMSM) || defined(FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X))
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */


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

extern void cmcall_init( void );


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
extern cm_mm_call_info_s_type  *cmcall_info_alloc( void );




/*===========================================================================

FUNCTION cmcall_ptr

DESCRIPTION
  Return a pointer to the one and only call object.

  CM call object.
  The call object is responsible for:
  1. Processing clients' call commands.
  2. Processing MC replies for clients' call commands.
  3. Processing MC call related notifications.
  4. Notifying the client list of call events.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cmcall_s_type  *cmcall_ptr(

         byte     call_index
             /* index into the call object array */
);



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
extern cm_call_id_type cmcall_call_id_get( const cmcall_s_type *call_ptr );



/*===========================================================================

FUNCTION cmcall_assert

DESCRIPTION
  Check a series of assertions over the call object to make sure that
  none of its fields are corrupted and to validate its state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_assert(

    const cmcall_s_type    *call_ptr
        /* pointer to a call object */
);


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
extern void cmcall_event(

    cmcall_s_type    *call_ptr,
        /* pointer to a call object. */

    cm_call_event_e_type    call_event
        /* notify client list of this call event */
);


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
extern void cmcall_end(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmcall_emergency_call_active

DESCRIPTION
  Check all call id's to see if there is an emergency call active

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean  cmcall_emergency_call_active ( void );



/*===========================================================================

FUNCTION cmcall_overall_end_calls

DESCRIPTION
  Ends all calls, except emergency calls when card is removed/in error.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void  cmcall_overall_end_calls ( void );


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */




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
extern byte cmcall_call_ids_allocated(
                               void
);


/*===========================================================================

FUNCTION cmcall_overall_call_state

DESCRIPTION
  This function returns the overall call state.

DEPENDENCIES
  none

RETURN VALUE
  overall call state

SIDE EFFECTS
  none

===========================================================================*/
extern cm_overall_call_state_e_type  cmcall_overall_call_state
(
    void
);


/*===========================================================================

FUNCTION cmcall_update_gsdi_and_log_call_state

DESCRIPTION
  This function updates the GSDI status about traffic/non-traffic
  status.

  This function also logs the call state.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_update_gsdi_and_log_call_state(void);


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
extern int cm_number_of_allocated_calls( void );


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
extern boolean cm_call_id_is_allocated( cm_call_id_type call_id );



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
extern cm_call_mode_info_e_type cmcall_call_id_mode
(
  cm_call_id_type call_id
);



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
extern boolean cmcall_is_call_type_valid
(
  cm_call_type_e_type    call_type
    /* call type */
);



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
extern boolean cmcall_is_mobile_in_hdr_call
(
  void
);



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
extern cm_call_cmd_err_e_type  cmcall_call_cmd_answer_para_check
(
  const cm_call_cmd_s_type       *call_cmd_ptr
);


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
extern cm_call_cmd_err_e_type  cmcall_call_cmd_get_call_info_para_check(
  const cm_call_cmd_s_type       *call_cmd_ptr
);



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
extern cm_roam_pref_e_type  cmcall_get_favored_roam_pref(

    cm_call_type_e_type    call_type,
        /* Call which we try to originate */

    sys_sys_mode_e_type    srv_mode,
        /* The current service mode */

    sys_roam_status_e_type roam_status,
        /* The current roaming status */

    cm_roam_pref_e_type    roam_pref
        /* The current roaming preference */

);



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
extern cm_prl_pref_e_type         cmcall_get_favored_prl_pref(

    cm_call_type_e_type    call_type
        /* Call which we try to originate */

);



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
extern dword  cmcall_force_pref_on_the_fly(

    cmcall_s_type             *call_ptr,
        /* Pointer to a call object */

    sd_ss_pref_reas_e_type    force_reason
        /* Reason for forcing the SS-preference.
        */
);



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
extern void cmcall_client_cmd_err(

    const cm_cmd_s_type       *cmd_ptr,
        /* Pointer to a CM command */

    cm_call_cmd_err_e_type    cmd_err
        /* Indicate the call command error */
);



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
extern void cmcall_orig_proc(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);



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
extern void cmcall_orig_start(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmcall_timer_proc

DESCRIPTION
  Process call timer events

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_timer_proc(

    cm_timer_event_type    timer_event
        /* Indicate specific timer event */
);


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
extern void cmcall_offline (void);


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
extern void  cmcall_overall_call_end
(
    void
);

/*===========================================================================

FUNCTION cmcall_end_each_call_with_mode

DESCRIPTION
  Ends specified calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void  cmcall_end_each_call_with_mode(

    cm_call_mode_info_e_type call_mode_info,
    /* Given call mode info */

    cm_call_end_e_type call_end_reason
    /* Given reason for ending the call */
);

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
extern void cmcall_end_each_call_with_type
(
  cm_call_type_e_type call_type,
  /* Given call mode info */

  cm_call_end_e_type  call_end_reason
  /* Given reason for ending the call */
);

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
extern cmcall_act_e_type  cmcall_act_get(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
);


/*===========================================================================

FUNCTION CMCALL_CLIENT_CMD_PROC

DESCRIPTION
  Invoke appropriate mode routine to process clients' call commands.

DEPENDENCIES
  Call object must have already been initialized with cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_client_cmd_proc
(
  cm_cmd_s_type  *cmd_ptr                       /* pointer to a CM command */
);


/*===========================================================================

FUNCTION cmcall_check_if_cs_voice_calls_active

DESCRIPTION
  Checks if any CS voice calls are active

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if CS voice calls active
  FALSE: otherwise

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern boolean cmcall_check_if_cs_voice_calls_active( void );


/*===========================================================================

FUNCTION cmcall_check_if_ps_call_active

DESCRIPTION
  Checks if any PS calls are active

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if PS call is active
  FALSE: otherwise

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern boolean cmcall_check_if_ps_call_active( void );


/*===========================================================================

FUNCTION cmcall_get_call_info_list

DESCRIPTION
  Get a sanpshot of the state of all call objects maintained by CM.  This
  is a synchronous function.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  This function returns the state of the call objects in the info parameter.

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_get_call_info_list(

  cm_call_state_info_list_s_type  *info_list
    /* The state of the call objects maintained by CM. */

);



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
extern cm_data_session_status_e_type cmcall_data_session_reg(

  cm_get_data_session_total_byte_ctrs_cb_f_type
                              *data_session_total_byte_ctrs_cb_func_ptr
    /* Pointer to data session total byte counters callback function */

);

/*===========================================================================

FUNCTION cmcall_determine_ss_for_origination

DESCRIPTION

  Based on mode and hybrid preference, determine ss for call object /
  cmpref_proc_cmd_pref_sys_chgd() function.

  The assumption is that for call originations sys pref change will be sent
  to either 1X or HDR (depending on where the origination will go) but not both.
  It goes to both only when hybr_pref is toggled from on to off or vice versa.

DEPENDENCIES
  None

RETURN VALUE
  ss

SIDE EFFECTS
  None

===========================================================================*/
extern cm_ss_e_type cmcall_determine_ss_for_origination(
  cm_mode_pref_e_type    mode_pref,

  cm_hybr_pref_e_type    hybr_pref,

  cm_cc_mode_pref_e_type mode_usage,

  const cmcall_s_type   *call_ptr
);

/*===========================================================================

FUNCTION cmcall_determine_orig_srv_opt

DESCRIPTION
  Determine the best service option to be used for call originations.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE - SO found FALSE - SO not found

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_determine_orig_srv_opt(

    cmcall_s_type  *call_ptr
);

/*===========================================================================
FUNCTION cmcall_remove_mode_pref_components

DESCRIPTION

  Remove given SD mode preference component(s) from given CM mode preference.

DEPENDENCIES
  sd_misc_remove_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
extern cm_mode_pref_e_type cmcall_remove_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
                                                        int number_of_components_to_remove,
                                                        ... );

/*===========================================================================
FUNCTION cmcall_add_mode_pref_components

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
cm_mode_pref_e_type cmcall_add_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
                                                    int number_of_components_to_add,
                                                    ... );



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
extern void cmcall_call_control_complete_cb(

  cm_call_id_type                         call_id,
    /* call id */

  cm_cc_status_e_type                     status,
    /* status of command */

  cm_call_type_e_type                     call_type,
    /* call type */

  const cm_orig_call_control_s_type       *cdma_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on cdma */

  const cm_orig_call_control_s_type       *gw_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on gw */

  void                                    *user_data
    /* data user wanted passed back */

);

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
extern boolean cmcall_is_cc_required
(
  cm_cc_subscription_source_e_type          subscription_source
);

/*===========================================================================

FUNCTION cmcall_send_gstk_voice_envelope

DESCRIPTION
  Perform call control and send a command to gstk.
  GSTK will call a callback when complete.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_send_gstk_voice_envelope
(
  cm_call_id_type                              call_id,
    /* call id */

  const cm_num_s_type                         *orig_num_ptr,
   /* original calling number */

  cm_cmd_s_type                               *new_cmd_ptr,
   /* cmd ptr for re-queueing of command back to CM */

  cm_nc_call_control_complete_cb_f_type       *cm_call_control_complete_cb_func,

  cm_cc_subscription_source_e_type             subscription_source
);


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
extern cmcall_s_type *cmcall_get_ps_call_obj_in_orig_mode( void );

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif // #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

/*===========================================================================

FUNCTION cmcall_set_favored_mode_pref_for_origination

DESCRIPTION

  Given call type, srv_type, DRS bit, compute favored mode preference and
  favored hybrid preference.

DEPENDENCIES

  None

RETURN VALUE

  TRUE if favored mode and hybrid preference were computed successfully,
  in which case they are written into favored_mode_pref_ptr and
  favored_hybr_pref_ptr, FALSE otherwise

SIDE EFFECTS

  None

===========================================================================*/
extern boolean  cmcall_set_favored_mode_pref_for_origination(

                                cm_call_type_e_type    call_type,
                                cm_srv_type_e_type     srv_type,
                                boolean                drs_bit,
                                word                   srv_opt,
                                /* In */

                                cm_mode_pref_e_type   *favored_mode_pref_ptr,
                                cm_hybr_pref_e_type   *favored_hybr_pref_ptr
                                /* Out */

);


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
extern cm_call_id_type cmcall_is_there_a_call_type
(
  cm_call_type_e_type call_type,

  cm_call_id_type     ignore_call_id

);



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

extern boolean cmcall_check_call_type_in_call_state (

                  cm_call_type_e_type   call_type,
                  /* Given call type */

                  cm_call_state_e_type   call_state
                  /* Given call state */
);



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

extern boolean cmcall_check_each_call_in_call_state (

                  cm_call_state_e_type   call_state
                  /* Given call state */
);

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
extern void cmcall_update_call_end_info(

  cmcall_s_type           *call_ptr
      /* pointer to a call object. */
);

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

extern boolean cmcall_check_for_call_state_on_ss (
                  cm_ss_e_type    ss,
                  /* Given call type */
                  
                  cm_call_state_e_type   call_state
                  /* Given call state  */
);

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
extern void cmcall_reset(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);

/*===========================================================================

FUNCTION cmcall_is_call_id_in_range
DESCRIPTION
  Checks if a given Call Id is in a valid range from 0 to CALL_ID_MAX.
DEPENDENCIES
  Call Id must have already been alloacated.
RETURN VALUE
  TRUE if valid.
  FALSE if not valid.
SIDE EFFECTS
  None
===========================================================================*/
extern boolean cmcall_is_call_id_in_range(

                    cm_call_id_type call_id
);

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
extern void cmcall_set_type
(
  cmcall_s_type             *call_ptr,
  cm_call_type_e_type       call_type
);


/*===========================================================================

FUNCTION cmcall_is_a_hdr_call

DESCRIPTION
  Checks whether there is an HDR call up.

DEPENDENCIES
  None

RETURN VALUE
  Valid call id of a HDR call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern cm_call_id_type cmcall_is_hdr_call
(
  void
);


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
extern boolean cmcall_is_hybr_hdr_call (cm_call_id_type *call_id_ptr);


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
extern cmcall_s_type* cmcall_is_hybr_hdr_orig (void);


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
extern cm_call_id_type cmcall_is_there_a_cdma_call_in_conv (void);


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
extern boolean cmcall_generate_connection_id
(
  cmcall_s_type             *call_ptr
);


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
extern void cmcall_set_cdma_cc_result
(
  cmcall_s_type             *call_ptr,
  cm_cc_result_e_type       cc_result
);


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
extern void cmcall_set_gw_cc_result
(
  cmcall_s_type             *call_ptr,
  cm_cc_result_e_type       cc_result
);


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
extern void cmcall_obj_iterator_init
(
  cm_iterator_type *iterator_ptr
);


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
extern cmcall_s_type* cmcall_obj_get_next
(
  cm_iterator_type *iterator_ptr
);


/*===========================================================================

FUNCTION cmcall_set_call_direction

DESCRIPTION

  Sets the call direction.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
extern void cmcall_set_call_direction
(
  cmcall_s_type             *call_ptr,
    /* Pointer to a call object */

  cm_call_direction_e_type   call_direction
    /* Direction of the call MO or MT*/
);

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
extern int cmcall_count_calls_with_call_type (

  cm_call_type_e_type call_type
    /* call type */
);

/*===========================================================================

FUNCTION cmcall_get_call_obj_mo_in_orig

DESCRIPTION

  Returns call ptr of first call of a particular type in
  origination state.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  call pointer for the call type in origination state or NULL if none

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
extern cmcall_s_type *cmcall_get_call_obj_mo_in_orig (

  cm_call_type_e_type call_type
    /* call type */
);

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
extern boolean cmcall_info_get(

    const cmcall_s_type          *call_ptr,
        /* Point at call object */

    cm_mm_call_info_s_type    *call_info_ptr
        /* Copy call state info into this buffer */
);

/*===========================================================================
FUNCTION cmcall_force_non_wlan

DESCRIPTION
  Select non-WLAN for the next origination

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
dword  cmcall_force_non_wlan(

  cmcall_s_type    *call_ptr
        /* pointer to a call object */
);

/*===========================================================================

FUNCTION cmcall_remove_mode_and_force

DESCRIPTION
  Select non-HDR for the next origination

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
);

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
);

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


);


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

);

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

);

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
);


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
);

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
);



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
boolean cmcall_is_no_call_in_gw_cs (void);




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
boolean cmcall_is_no_call_in_gw_ps (void);

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
extern boolean cmcall_is_active_on_mode
(
  sys_sys_mode_e_type  sys_mode
);

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
extern boolean cmcall_check_voice_emerg_in_orig(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
);


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
extern boolean cmcall_check_if_csdata_and_voice(

    cm_call_type_e_type  call_type_1,
        /* call type */

    cm_call_type_e_type  call_type_2
        /* call type */
);

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
extern  boolean cmcall_hybr_pref_was_toggled_for_origination(

     cm_hybr_pref_e_type requested_hybr_pref
     /* Hybrid preference requested for the next call */
);


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
extern boolean cmcall_misc_is_mode_pref(

   cm_mode_pref_e_type fav_mode_pref_1,

   cm_mode_pref_e_type fav_mode_pref_2

);


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
extern boolean cmcall_check_if_no_srv(boolean check_hdr);

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

extern cmcall_s_type* cmcall_get_call_obj_in_call_state (

                  cm_call_state_e_type   call_state
                  /* Given call state */
);

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

extern cmcall_s_type* cmcall_get_otasp_call_obj(void);

#endif // FEATURE_OTASP
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
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern boolean cmcall_check_if_gw_cs_call_active (

  cm_call_state_e_type call_state
    /* Call state for the queried call */
);


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
extern boolean cmcall_is_gw_cs_call_type
(
  cm_call_type_e_type call_type
);

/*===========================================================================

FUNCTION cmcall_get_sec_pdp_callid

DESCRIPTION
  Gives the call id of a secondary PDP call.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  call id of secondary pdp call.

SIDE EFFECTS
  none

===========================================================================*/

extern cm_call_id_type cmcall_get_sec_pdp_callid (void);

/*===========================================================================

FUNCTION cmcall_is_ipcall_over_mode_poss

DESCRIPTION

  Returns TRUE if given call can be placed on the mode as a IP call.

DEPENDENCIES

   None

RETURN VALUE

  TRUE  -  Call can be placed as a IP call on given mode
  FALSE -  Call cannot be placed as a IP call.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_is_ipcall_over_mode_poss(

  sys_sys_mode_e_type              sys_mode,
     /* Specific technology - WCDMA, WLAN */

  const cmcall_s_type              *call_ptr
    /* Call object */
);


/*===========================================================================

FUNCTION cmcall_det_ss_from_sys_mode

DESCRIPTION
  Determines ss instance on which the given sys_mode is present

DEPENDENCIES
  None.

RETURN VALUE
  SS instance

SIDE EFFECTS
  none

===========================================================================*/
extern cm_ss_e_type  cmcall_det_ss_from_sys_mode(

    sys_sys_mode_e_type   sys_mode
        /* sys mode. WLAN, CDMA ...*/
);

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
extern byte cmcall_get_prim_with_groupid  (

  byte pdp_group_id
    /* Group id of primary pdp */
);

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
extern sys_sys_mode_e_type cmcall_det_sys_mode_from_ss(

    cm_ss_e_type   ss
      /* system instance, MAIN, HYBR_HDR ... */
);


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
extern boolean  cmcall_dem_is_apps_wakeup_from_suspend_event(

    cm_call_event_e_type   call_event
        /* Call event */
);


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
extern cm_call_id_type cmcall_get_voip_call_id( void );
#endif

#ifdef FEATURE_IMS_VCC
/*===========================================================================
FUNCTION cmcall_switch_call_obj

DESCRIPTION
  Switch call object id from call_id2 to call_id1.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: success; FALSE: failure

SIDE EFFECTS
  none

===========================================================================*/
extern boolean  cmcall_switch_call_id(

    cm_call_id_type    call_id1,
      /* call id to be swapped */

    cm_call_id_type call_id2
      /* call id to be swapped */

);
#endif
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
extern cm_srv_domain_pref_e_type        cmcall_get_favored_srv_domain_pref(

    cm_call_type_e_type          call_type
        /* The Call type enum type in call object*/
);


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
extern cm_mode_pref_e_type cmcall_misc_get_common_mode_pref
(
  cm_mode_pref_e_type cm_mode_pref,
    /* CM mode pref
    */

  int number_of_components,
  ...
);


#endif /* CMCALL_H */

