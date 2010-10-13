#ifndef CMPH_H
#define CMPH_H
/*===========================================================================

         C A L L   M A N A G E R   P H   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMPH.C

  CM phone object. The phone object is responsible for:
  1. Processing clients' phone commands.
  2. Processing LL replies for clients' phone commands.
  3. Processing LL phone related notifications.
  4. Notifying the client list of phone events.

Copyright (c) 1998 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmph.h#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/09   rm      Updating correct mode pref of phone when data_avaialable
                   request comes. Added misc_intersect_mode_pref
04/09/09   aj      Added support for IRAT measurements 
03/26/09   ks      Added FEATURE_CM_DISABLE_CALL_TYPE feature support
03/11/09   sv      Lint cleanup.
03/09/09   rm      Updating correct mode pref of phone when data_avaialable
                   request comes. Added cmph_get_srv_type.
03/01/09   sv      Lint cleanup.
01/19/09   sv      Updated cmph_term_pending_get_networks_req to accept SS
12/08/08   fj      Added mode_capability and band_capability in cmph_s_type;
                   Updated function cmph_get_mode_band_capability();
12/25/08   sv      Updated the cmph_is_valid_network_selection_mode_pref
                   to validate WLAN network selection mode preference.
12/11/08   am      Changes for  mode retained after power cycle.
10/21/08   ks      Update default threshold value for sir and pathloss and
                   add masks for RSSI event
09/18/08   rm      RAT Balancning enhancement- Block user from changing mode pref
                   only if ENS flag is ON and UIM_USIM_RAT file is present
                   - Adding new field 'is_uim_usim_rat_present '
09/10/08   ak      Fixed scenario of data call drop during GSM scan
09/04/08   dm/sv   Implemented WPS support for WLAN
07/30/08   ak      Fix for the HICPS scenario.
06/11/08   fj      Added API cmph_get_mode_band_capability,
                   added API cmph_map_sd_band_pref_to_cm_band_pref.
06/11/08   sv      Exposing cmph_info_alloc for power collapse buffered
                   events notifications.
06/6/08    rn      Added fn for deleting ps activity from prio queue
06/06/08   sv      Removing unneccesary snd.h
05/09/08   sv      Featurized msm support under FEATURE_CM_DETECT_HW
05/08/08   sv      Added support for detecting the supported msm
                   and idenfity if modem is available.
                   Featurized snd.h under CDMA
04/16/08   vk      changed subscription behaviour to buffer the subscription command
                   if the mode pref does not contain the technology, but will allow
                   all commands to go through if anyone subscription goes through
                   also checks buffers and sends subscription command when mode
                   pref changes
12/16/07   ks      Added cm_ph_cmd_signal_strength_delta
12/13/07   sk/ak   Added DCM.
12/12/07   rm      Adding RAT-balancing feature support
11/16/07   rn      Added support for indicating clients of phone event in use
                   state when there is MT call coming in while MO call end
                   cnf is still expected
06/22/07   sk      DDTM rewrite.
06/05/07   pk      Added support to read mob_cai_rev from NV
04/03/07   sk      Added cmph_cmd_client_release()
03/09/07   sk      RAM usage reduction.
02/26/07   ka      Changes to support Voip deregistration.
11/17/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
08/29/06   sk      Added WLAN Adhoc support.
08/11/06   pk      Added support for UOOS
06/05/06   ka/ic   Ignoring LPM_OK report received from MMOC when CM is not
                   waiting on it.
04/06/06   sk      Added cmph_map_sd_ss_type_to_cm_ss_type()
04/01/06   ka      Changes for emerg serv category during orig, aborting
                   manual search and returning current band with srv ind.
03/10/06   pk      Added support for high PSIST
03/09/06   pk      Added support for POWER COLLAPSE
01/06/06   ka      Externing cmph_get_gprs_anite_gcf_ptr().
12/14/05   ic      Changed featurization for cmph_otasp_update_mode() so it
                   links for JCDMA builds
12/13/05   ic      Restored cmph_otasp_update_mode()
12/11/05   ic      Lint cleanup
11/10/05   pk      Added support for privacy_pref, and is_privacy in the
                   phone object
11/08/05   sk      Separated CM reports and commands.
11/01/05   ka      Helper functions to check if nam change is pending
10/25/05   pk      Fixed RVCT compiler warnings and added mapping functions
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
10/18/05   pk      Added cmph_map_sd_hybr_pref_to_cm_hybr_pref()
10/13/05   ic      Mainlined FEATURE_HDR_HICPS20 and FEATURE_HDR_HICPS30
09/14/05   pk      Extern definition for cmph_map_sd_hybr_pref_to_cm_hybr_pref
09/12/05   pk      Added cmph_pd_session_start(), cmph_pd_session_end()
08/15/05   pk      Added support for priority Queues for origination
                   parameters
08/15/05   pk      Extern definition for cmph_get_is_hdr_kicked_ptr()
08/11/05   ka      Adding CMPH_MAX_VAL_ACM
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/06/05   pk      Added access overload class information to the cmph_s_type
05/11/05   ic      Moved cmph_net_list_copy() to cm.h/.c
05/04/05   jqi     Fixed for hybr data switch over.
04/08/05   pk      Removed cmph_get_cur_hybr_status(void),
                   cmpref_is_hybr_status_on is the correct function
02/17/05   pk      Added cmph_map_cm_band_pref_to_sd_band_pref()
02/01/05   dk      Added WLAN Support.
01/24/05   ic      For consistency, renamed cmph_is_ph_in_emerg_cb_mode() to
                   cmph_is_in_emergency_cb()
11/11/04   sj      Rearranged featurization for DDTM_CNTL.
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/10/04   jqi     Any system preference change or operation mode change
                   should terminate a pending get network request.
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
09/03/04   dk      Added is_ps_srv_req_sent flag.
08/30/04   sj      Made orig_mode as an array. Added get/update orig mode funcs.
08/27/04   jqi     Added ps dormant optimization.
08/20/04   sj      Added support for PRL Preference.
08/12/04   ws      Remove cm20 support.
08/04/04   ic      Removed cmph_stop_mode_req() as it became obsolete
07/21/04   sj      Added support for umts to 1x handover.
07/12/04   ic      Added support for FEATURE_ALS
07/09/04   sj      Created fn. for checking if pref_term is temporary.
06/30/04   dk      Added cmph_is_waiting_for_ps_sig_down().
06/25/04   dk      Added cmph_set_ps_sig_down_ind_wait_flag() and
                   cmph_set_ps_sig_down_wait_time().
06/22/04   ic      Comments clean up
06/10/04   dk      Added cmph_call_end_req function.
05/14/04   ka      Fixed Lint errors.
04/13/04   ka      Moved cmph_unforce_mode_on_the_fly out of CDMA featurization
04/08/04   ws      Merged changes from MSMSHARED_CM.01.00.32.00.03
03/17/04   sj      Added new DDTM parameters to phone object.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/21/04   RI      Modified cmph_force_pref_on_the_fly() to include service
                   domain preference parameter.
01/13/04   ka      Merged changes from MSM6200
10/28/03   ws      Removed uim pref slot support.
10/03/03   ws      Added get function for ps data orig prefs.
10/01/03   ws      Merged changes from GW Targets
09/23/03   ws      Merged fix when SIM fails do not end the emergency call.
09/18/03   ws      Added support for dual slots.
09/18/03   RI      Added signaling_conn_rel_req field to cmph_s_type.
09/16/03   vt      Added preferred slot handling.
09/12/03   prk     Added ps_service_req_uptime field to cmph_s_type.
09/03/03   jds     Correct bug when SIM fails (or is removed) during an
                   emergency call (do NOT end the call).
08/28/03   ws      Added access functions for subscription information.
08/22/03   vt      Mainlined packet state handling.
04/28/03   AT      Lint cleanup.
04/11/03   prk     Removed references to FEATURE_AOC.
02/28/03   ws      Featurized cmph_send_stop_mode_req for GW
02/27/03   ws      Fixes for MO SDB timeout problems.
02/25/03   RI      Added orig_srv_domain_pref in cmph_s_type for PS attach/detach on
                   demand.
                   Modified the parameters in cmph_send_service_req().
02/06/03   vt      Replaced answer_duration with answer_uptime in
                   cmph_s_type structure.
02/06/03   prk     Added valid_card in cm_ph_s_type.
02/04/03   ws      Modified cmph_otasp_update_mode to not read from NV.
01/21/03   ic      For consistency, replaced answer_uptime with answer_duration
                   in cmph_s_type structure.
01/21/03   ws      Updated copyright information for 2003
11/22/02   PRK     Added support for processing user preferred PLMN list.
11/19/02   vt      Added changing RTRE configuration.
11/15/02   vt      Properly featurized the packet state.
11/14/02   vt      Merged changes from GW mainline.
                   [YR]Added test control type handling.
                   [CS]Added cmph_send_service_req defintion.
09/10/02   ws      Added initial cm20 backward compatibility.
07/22/02   PRK     Added available_networks to cmph_s_type.
04/09/02   sh      Modified cmph_mo_sms_end and added cmph_select_dbm_mode_pref
04/02/02   sh      added first_time_camp_on_1x
03/08/02   hxw     Modified cmph_mo_sms_end() to support MSBased gpsOne.
03/21/02   sh      Added orig start reason in CMPH_ON_COLLOC_AND_NEW_HYBR_OFF
03/03/02   sh      Used pref_reas instread of orig_mode in some hybrid related
                   macros.
02/20/02   sh      Added data session handoff support for hybrid operation
02/06/02   sh      Added hybrid 3.0 support
01/25/02   sj      Added support for Tiered services - user zone feature.
01/19/02   az      CR 17624, Added a parameter in cmph_force_mode_on_the_fly()
12/11/01   sh      Added preliminary hybrid operation NV item support
12/08/01   sh      Added preliminary hybrid operation support
05/04/02   PRK     Added prst_srv_domain_pref, prst_network_sel_mode_pref,
                   network_sel_mode_pref, prst_acq_order_pref and acq_order_pref
                   fields to cmph_s_type.
04/22/02   AT      Added cm_ph_aoc_no_funds_available and cm_ph_aoc_no_funds
                   function prototypes.
01/04/02   PRK     Replaced FEATURE_PLMN with FEATURE_NEW_PLMN.
                   Replaced terminology of rat and service type with system
                   mode and service domain respectively.
                   Added is_subscription_available flag to cmph_s_type.
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                     Added support for PLMN selection/reselection (FEATURE_PLMN).
                     Re-added Serving System group and moved srv_state, roam_status,
                     rssi from phone group to serving system group (FEATURE_NEWSS).
10/17/01   AT      Added support for Advice of Charge (AOC)
10/15/01   RI      Added support for Supplementary Services ( GSM/WCDMA ).
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/23/01   VT/IC   Baseline for CCS
08/21/01   ic      Main-lined FEATURE_NEWCM
08/23/01   HD      Added RSSI reporting for GSM/WCDMA.
08/21/01   ic      Main-lined FEATURE_NEWCM
04/27/01   HD      Added changes for WCDMA/GSM mode.
04/13/01   SH      Added Preliminary HDR support
03/27/01   RC      Added SD 2.0 trimode support.
02/16/01   SH      Added is_end_ack_pending flag
02/06/01   RC      Added support for System Determination 2.0.
01/23/01   ych     Merged T53 and JCDMA features.
12/03/99   VT      Merged TriMode Code.
05/31/99   SH      Added CM 2.0 features
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
12/08/98   RC      Changes per second week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/


/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"        /* system wide common types */
#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"       /* Internal interface to cm.c */
#include "cmcall.h"    /* Interface to CM call object */
#include "nv.h"        /* Interface to NV services */

#ifdef FEATURE_CM_DETECT_HW
#error code not present
#endif /* FEATURE_CM_DETECT_HW */

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#include "voc.h"       /* Interface to vocoder services */
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

#define CMPH_DEFAULT_RSSI_DELTA     5 /* Default RSSI delta change threshold
                                      ** for SIGNAL STRENGTH reporting
                                      */

#define CMPH_DEFAULT_ECIO_DELTA     2 /* Default ECIO delta change threshold
                                      ** for SIGNAL STRENGTH reporting
                                      */

#define CMPH_DEFAULT_IO_DELTA       5 /* Default IO delta change threshold
                                      ** for SIGNAL STRENGTH reporting
                                      */

#define CMPH_DEFAULT_SIR_DELTA      3 /* Default SIR delta change threshold
                                      ** for SIGNAL STRENGTH reporting
                                      */

#define CMPH_DEFAULT_PATHLOSS_DELTA 5 /* Default PATHLOSS delta change threshold
                                      ** for SIGNAL STRENGTH reporting
                                      */

#define CMPH_HOLD_MODE_PREF_TIME    4 /* seconds */
    /* At end of call, delay unforcing the mode preference for
    ** this long.
    **
    ** NOTE that we delay unforcing the mode preference in order to
    ** avoid loading MC with pref sys changes in a situation where
    ** the user is pressing SEND, END, SEND, END, ... */

#define CMPH_GW_PKT_SIGNALING_DOWN_WAIT_TIME  5  /* seconds */
#define CMPH_GW_PKT_WAIT_TIME_NONE           (dword) (0)

    /* At the end of last Packet call for GSM/WCDMA, we would
    ** wait for 5 sec. This allows network to teardown signaling
    ** during this time. By introducing wait, we will avoid the
    ** race condition when a PDP CONTEXT ACTIVE request is sent
    ** while network is tearing down the PS signaling.
    */

#define CM_PH_ORIG_MODE_UPTIME_NONE (dword) (-1)
    /* This is a special value indicating that the call/activity is still
    ** in progress
    */

#define CMPH_MAX_VAL_ACM      0xFFFFFF
    /* ACM value should not exceed 0xFFFFFF (Spec 31.102, 4.2.9)
    ** Since SIM has only 3 bytes of storage allocated to this.
    */

#define CMPH_MAX_MC_REPLY_TIME          2    /* seconds */
    /* Upper bound on how long a client phone-command waits for a reply
    ** from LL. After such time the command considered failed due to
    ** communication problems with lower layers
    **
    ** Increased REPLY_TIME from 2 to 4 to account for bigger delay in lower
    ** layers confirming with LPM_OK
    */

#define CMPH_MAX_IPAPP_DEREG_TIME       5
    /* Max Time allowed by APP to report dereg.
    **
    ** If in service, bringing up PS call and deregistering
    ** should not take more than 5 secs
    **
    ** If not in service, ipapp will imeediately return DEREG.
    */

/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/* Enumeration of preferred system changed state when the hybrid operation
   is enabled.
*/
typedef enum cmph_pref_ss_state_e {

  CMPH_PREF_SS_STATE_NONE=-1,    /* FOR INTERNAL USE OF CM ONLY! */

  CMPH_PREF_SS_STATE_NULL,            /* Null state */
  CMPH_PREF_SS_STATE_WAIT_FOR_1X_SRV, /* Wait for 1x to acquire service */
  CMPH_PREF_SS_STATE_HDR_ONLY,        /* send pref sys changed command
                                         to HDR only */
  CMPH_PREF_SS_STATE_1X_HDR,          /* pref sys has been sent to
                                         MC and HDRMC and hybrid opertion
                                         is on */

  CMPH_PREF_SS_STATE_MAX         /* FOR INTERNAL USE OF CM ONLY! */

} cmph_pref_ss_state_e_type;


/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* Phone type - holds all the information that is associated
** with a phone.
*/
typedef struct cmph_s  {

  cm_init_mask                     init_mask;
      /* Indicate whether this object was properly initialized */

  boolean                          is_in_use;
       /* Indicate whether phone is currently in use */

  boolean                          is_in_use_reported;
       /* Indicate whether phone is in use as per client's knowledge*/

  sys_oprt_mode_e_type             oprt_mode;
      /* Indicate current operating mode - online, offline, LPM, etc. */

  cm_test_control_type_e_type      test_control_type;
      /* Indicate what type of test control test is */

  cm_cdma_lock_mode_e_type         cdma_lock_mode;
      /* Indicate whether phone is CMDA locked until power cycle */

  cm_answer_voice_e_type           answer_voice;
      /* Indicate whether phone should answer incoming calls as
      ** voice, modem, or fax */

  dword                            answer_uptime;
      /* Indicate uptime for temporary answer-voice settings - i.e. the
      ** uptime when the a temporary answer-voice setting is expired and
      ** phone should restore the normal CM_ANSWER_VOICE_AS_VOICE
      ** setting */

  cm_mode_pref_e_type              prst_mode_pref[NV_MAX_NAMS];
      /* Indicate the persistent mode preference setting */

  cm_mode_pref_e_type              mode_pref;
      /* Indicate the current mode preference -
      ** analog-only digital-only, etc. */

  cm_pref_term_e_type              pref_term;
      /* Indicate the current mode preference term -
      ** one-call, power-cycle, etc.*/

  dword                            mode_uptime;
      /* Indicate uptime for temporary mode terms - i.e. the uptime
      ** when the temporary mode is expired and phone should restore
      ** the persistent mode preference */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_band_pref_e_type              prst_band_pref[NV_MAX_NAMS];
      /* Indicate the persistent band preference setting */

  cm_band_pref_e_type              band_pref;
      /* Indicate the current band preference */

  cm_prl_pref_e_type               prst_prl_pref[NV_MAX_NAMS];
      /* The persistent prl preference setting */

  cm_prl_pref_e_type               prl_pref;
      /* The current prl preferred setting */

  cm_roam_pref_e_type              prst_roam_pref[NV_MAX_NAMS];
      /* Indicate the persistent roam preference setting */

  cm_roam_pref_e_type              roam_pref;
      /* Indicate the current roam preference */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  boolean                          is_spc_zero;
      /* Indicate whether the SPC is all zeros (i.e., "000000") */

  cm_hybr_pref_e_type              prst_hybr_pref;
       /* Indicate the persistent hybr preference setting */


  cm_hybr_pref_e_type              hybr_pref;
       /* Indicate the hybr preference setting */

  sd_ss_hybr_pref_e_type           int_hybr_pref;
       /* Indicate the hybr preference setting, only for internal CM use
       ** if HYBR_GW is turned ON, even if user sets the hybr pref to OFF
       ** CM will turn ON this hybrid preference
       */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_srv_domain_pref_e_type        prst_srv_domain_pref[NV_MAX_NAMS];
      /* The persistent servuce domain preference setting */

  cm_srv_domain_pref_e_type        srv_domain_pref;
      /* The current service domain preferred setting */

  cm_gw_acq_order_pref_e_type      prst_acq_order_pref[NV_MAX_NAMS];
      /* The persistent acquisition order preference setting */

  cm_gw_acq_order_pref_e_type      acq_order_pref;
      /* The current acquisition order preference setting */

  cm_network_sel_mode_pref_e_type  prst_network_sel_mode_pref[NV_MAX_NAMS];
      /* The persistent network selection mode preference setting */

  cm_network_sel_mode_pref_e_type  network_sel_mode_pref;
      /* The current network selection mode setting */

  sys_plmn_id_s_type               plmn;
      /* The identifier of the PLMN to be selected manually */

  cm_nam_e_type                    nam_sel;
      /* Indicate the NAM selection */

  cm_nam_e_type                    curr_nam;
      /* Indicate the current NAM -
      ** needed when NAM selection is set to AUTO-NAM */

  cm_reply_s_type                  reply;
      /* CM reply object to handles the details that are associated
      ** with waiting for a reply from MC */

  boolean                          is_sr_with_amps;
      /* Indicate whether the phone should try forcing to AMPS
      ** during a silent redial origination */


  uint8                            rssi_delta;
      /* RSSI change exceeds the delta threshold will be reported,
      ** default is 5dBm */

  uint8                            ecio_delta;
      /* ECIO change exceeds the delta threshold will be reported,
      ** default is 2dBm */

  uint8                            io_delta;
      /* IO  change exceeds the delta threshold will be reported,
      ** default is 5 dBm */

  uint8                            sir_delta;
      /* SIR  change exceeds the delta threshold will be reported,
      ** default is 3 dBm */

  uint8                            pathloss_delta;
      /* PATHLOSS  change exceeds the delta threshold will be reported,
      ** default is 5 dBm */


 cm_packet_state_e_type           packet_state;
     /* Indicate Packet State */

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

  boolean                          is_cdma_subscription_available;
    /* Indicate whether the cdma subscription/provisioning information
    ** is available for consumption.                              */

  boolean                          is_gw_subscription_available;
    /* Indicate whether the GSM/WCDMA subscription/provisioning information
    ** is available for consumption.                              */

  boolean                          is_wlan_subscription_available;
    /* Indicate whether the WLAN subscription/provisioning information
    ** is available for consumption.                              */

  cm_rtre_config_e_type            rtre_config;
    /* Current RTRE configuration - RUIM, NV, RUIM fallback NV */

  cm_rtre_control_e_type           rtre_control;
    /* Current RTRE control - RUIM or NV */

  cm_ps_data_orig_prefs_e_type     ps_data_orig_pref;
    /* Indicates the preferences for ps data originations */

  #ifdef FEATURE_MMGSDI
  boolean                          valid_card_1;
    /* Indicates if SIM card 1 is valid (i.e. inserted and no errors) */

  boolean                          valid_card_2;
    /* Indicates if SIM card 2 is valid (i.e. inserted and no errors) */
  #endif

  boolean                          ps_service_req_timer_active;
      /* Indicates whether the PS service request timer
      ** is active. */

  dword                            ps_service_req_uptime;
      /* Indicate the uptime when a PS attach request is
      ** expected to be completed. */

  cm_ddtm_pref_e_type         ddtm_pref;
    /* data dedicated transmission mode preference */

  boolean                     cur_ddtm_status;
    /* Last DDTM status sent to 1xCP */

  sys_ddtm_act_mask_e_type    ddtm_act_mask;

  sys_ddtm_act_mask_e_type    cur_ddtm_act_mask;
    /* Last DDTM activity mask send to 1xCP.
    */

  uint16                      ddtm_num_srv_opt;
    /* Number of SOs specified in list;
    */

  sys_srv_opt_type            ddtm_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
    /* List of SOs for which 1x pages have to ignored while in
    ** HDR connected state. if srv_opt_list[] contains CAI_SO_NULL
    ** & ddtm_pref is ON, ignore all pages.
    */

  uint16                      cur_ddtm_num_srv_opt;
    /* Last DDTM ignore SO list count sent to 1xCP.
    */

  sys_srv_opt_type            cur_ddtm_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
    /* Last DDTM ignore SO list sent to 1xCP.
    */


  dword                       ps_signaling_down_wait_time;


  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif

  cm_cphs_proc_e_type          cphs_allowed;
  /*
  ** FEATURE_ALS
  ** Whether Common PCN Handset Specifications procedures are allowed
  */

  cm_als_proc_e_type           als_allowed;
  /*
  ** FEATURE_ALS
  ** Whether ALS procedures are allowed.
  ** cphs_allowed must be CM_CPHS_PROC_ALLOWED for als_allowed to
  ** be CM_ALS_PROC_ALLOWED
  */

  cm_als_line_e_type           line;
    /*
    ** FEATURE_ALS
    ** Currently selected line (all MO voice calls will use it)
    */

  cm_als_line_switching_e_type line_switching;
    /*
    ** FEATURE_ALS
    ** Whether the user is allowed to switch between lines
    */

  #if defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM) || \
      defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
#error code not present
  #endif

  boolean                     is_ps_srv_req_sent;
    /* ps_service_req_sent flag is used to decide if we need to start the
    ** PS_SIG_DOWN timer after PS service is acquired.
    */

  cm_wlan_pref_s_type         wlan_pref;
    /* wlan preferences
    */

  cm_wlan_pref_s_type         prst_wlan_pref[NV_MAX_NAMS];
    /* The persistent WLAN preference setting */

  sys_wlan_bss_info_s_type    wlan_bss_info;
    /* WLAN BSS Info for the manualy selected WLAN*/

  sys_wlan_bss_info_list_s_type    wlan_bss_list;
    /* The available WLAN BSS network list */

  sys_wlan_wps_result_s_type  wlan_wps_result;
    /* The List of WLAN systems and Credentials returned
    ** by the WPS Procedure
    */

  boolean                     is_event_process;
    /* This flag controls if more details PH info
    ** should be copied or not. This flag restricts that client
    ** can request more PH info only while in the PH_EVENT callback
    ** processing.
    */

  boolean                     is_net_lists_present;
    /* This flag indicates if network lists info is available
    ** for clients.
    */

  boolean                     is_wps_pending;
    /* This flag indicates if there is another pending WPS start request.
    */

  cm_mode_pref_e_type         network_type;
    /* Type of network indicated in the get_networks API. */

  boolean                     is_data_switch_over_call_success;
    /* Indicate if there is a data switch over call that has been in
    ** conversation state.
    */

  byte                        accolc;
    /* The access overload class information
    */

  cm_ph_state_e_type          ph_state;
    /* The current operating mode of the phone
    ** Used to indicate emergency call back mode
    */

  boolean                     is_nam_change_pend_otasp_comm;
    /* After a succesfull OTASP commit nam change has to take place.
    ** This change is delayed till the Traffic channel gets torn down.
    ** CM_EXIT_TC_F gets sent on traffic channel getting torn down.
    **
    ** TRUE  - successfull OTASP commit occurred, send nam change after
    **         traffic is torn down.
    ** FALSE - No OTASP commit occurred so no need to send nam change on
    **         traffic channel being torn down.
    */

  cm_privacy_pref_e_type      privacy_pref;
    /* The privacy setting for calls, all calls share the privacy setting
    */

  boolean                     is_privacy;
    /* The state of the current pruvacy, this could be different than the
    ** privacy_setting
    */

  cm_dem_s_type              *dem_ptr;
    /* The pointer to the dem object
    */

  sys_plmn_list_status_e_type available_networks_list_cnf;
    /* Gives the status of plmn list request (success/abort).
    */

  byte                       mob_cai_rev;
    /* The mob p_rev of the mobile
    */

  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif

  boolean                     is_cdma_tx_disabled;
    /**
    ** Tells is CDMA Tx has been disabled using DDTM.
    */

  /* fields needed for subscription buffering
  */
  boolean                     is_pending_subsc_cdma;

  cm_ph_cmd_e_type            cdma_subsc_cmd;

  int                         cdma_client_id;

  boolean                     is_pending_subsc_gw;

  cm_ph_cmd_e_type            gw_subsc_cmd;

  int                         gw_client_id;

  boolean                     is_pending_subsc_wlan;

  cm_ph_cmd_e_type            wlan_subsc_cmd;

  int                         wlan_client_id;


  sys_sys_mode_mask_e_type    mode_capability;

  sys_band_mask_e_type        band_capability;
  
  cm_disable_call_type       disable_call_type_mask;
    /* The bit mask for disabling the call types. The call type mask is
    ** defined in CM.H CM_CALL_TYPE_VOICE_MASK ...
    */

  #ifdef FEATURE_CM_SS_MEAS
#error code not present
  #endif /*FEATURE_CM_SS_MEAS */

} cmph_s_type;


/**--------------------------------------------------------------------------
** Macros
** --------------------------------------------------------------------------
*/

/* Macro to return a pointer to the persistent mode preference
** of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_MODE_PREF( ph_ptr ) \
                     ( ph_ptr->prst_mode_pref[ ph_ptr->curr_nam ] )

/* Macro to return a pointer to the persistent system preference
** of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_SYS_PREF( ph_ptr ) \
                     ( ph_ptr->prst_sys_pref[ ph_ptr->curr_nam ] )

/* Macro to return a pointer to the persistent band preference
** of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_BAND_PREF( ph_ptr ) \
                     ( ph_ptr->prst_band_pref[ ph_ptr->curr_nam ] )


/* Macro to return a pointer to the persistent prl preference
** of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_PRL_PREF( ph_ptr ) \
                     ( ph_ptr->prst_prl_pref[ ph_ptr->curr_nam ] )

/* Macro to return a pointer to the persistent roam preference
** of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_ROAM_PREF( ph_ptr ) \
                     ( ph_ptr->prst_roam_pref[ ph_ptr->curr_nam ] )

#define CMPH_PRST_HYBR_PREF( ph_ptr ) \
                     ( ph_ptr->prst_hybr_pref )


/* Macro to return a pointer to the persistent acquisition order preference
** of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_ACQ_ORDER_PREF( ph_ptr ) \
                     ( ph_ptr->prst_acq_order_pref[ ph_ptr->curr_nam ] )

/* Macro to return a pointer to the persistent network selection mode preference
** of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr ) \
                     ( ph_ptr->prst_network_sel_mode_pref[ ph_ptr->curr_nam ] )

/* Macro to return a pointer to the persistent service domain preference
** of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr ) \
                     ( ph_ptr->prst_srv_domain_pref[ ph_ptr->curr_nam ] )

/* Macro to return the persistent WLAN preference of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_WLAN_PREF( ph_ptr ) \
                     ( ph_ptr->prst_wlan_pref[ ph_ptr->curr_nam ] )

/* Macro to return the persistent WLAN tech preference of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_WLAN_TECH_PREF( ph_ptr ) \
                     ( ph_ptr->prst_wlan_pref[ ph_ptr->curr_nam ].tech_pref )

/* Macro to return the persistent WLAN BSS type pref of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_WLAN_BSS_TYPE_PREF( ph_ptr ) \
                 ( ph_ptr->prst_wlan_pref[ ph_ptr->curr_nam ].bss_type_pref )

/* Macro to return the persistent WLAN scan preference of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_WLAN_SCAN_PREF( ph_ptr ) \
                     ( ph_ptr->prst_wlan_pref[ ph_ptr->curr_nam ].scan_mode )

/* Macro to return the persistent WLAN tech preference of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#define CMPH_PRST_WLAN_NET_SEL_MODE_PREF( ph_ptr ) \
                     ( ph_ptr->prst_wlan_pref[ ph_ptr->curr_nam ].network_sel_mode_pref )

#ifdef FEATURE_CM_DETECT_HW
#error code not present
#endif /* FEATURE_CM_DETECT_HW */

/**--------------------------------------------------------------------------
** Functions - common
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmph_dem_get_apps_pwr_status

DESCRIPTION
  Return the current apps power state

DEPENDENCIES
  none

RETURN VALUE
  current apps power status

SIDE EFFECTS
  none

===========================================================================*/

extern cm_apps_pwr_status_e_type cmph_dem_get_apps_pwr_status(void);



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
extern cmph_s_type  *cmph_ptr( void );



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
extern void cmph_init( void );



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
extern boolean cmph_is_pref_term_temporary(

       cm_pref_term_e_type       pref_term
        /* pref_term to check for temporary */

);


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
extern boolean cmph_is_valid_mode_band_pref(

    cm_mode_pref_e_type    mode_pref,
        /* Mode pref to check for validity */

    cm_band_pref_e_type    band_pref
        /* Band pref to check for validity */
);

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
nv_mode_enum_type cmph_map_mode_pref_cm2nv( cm_mode_pref_e_type  cm_mode );


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
extern void cmph_get_mode_band_capability(

  sys_sys_mode_mask_e_type *    mode_capability,
    /* Mode capability supported by the current target */

  sys_band_mask_e_type *        band_capability
    /* Band capability supported by the current target */


);


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

    cm_network_sel_mode_pref_e_type              network_sel_mode_pref,
        /* Network selection mode pref to check for validity */

    cm_network_sel_mode_pref_e_type              wlan_network_sel_mode_pref
        /* WLAN Network selection mode pref to check for validity */
);


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
    /* CM Service domain preference that gets mapped to sd srv mode preference */
);


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
);




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
);




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
    /* CM acq order preference that gets mapped to SD acq order preference */

);


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
extern sd_ss_orig_mode_e_type   cmph_map_cm_call_type_to_sd_orig_mode(

    cm_call_type_e_type         call_type
        /* Call type to map */
);


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
);


/*===========================================================================

FUNCTION cmph_is_in_emergency_cb

DESCRIPTION
  Check if the phone is in emergency call back mode.
  valid.

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
);


/*===========================================================================

FUNCTION cmph_call_start

DESCRIPTION
  Does necessary phone's call start activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_call_start(

    cmcall_s_type    *call_ptr
        /* Pointer to a call object */
);



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
extern void cmph_call_end(
    const cmcall_s_type    *call_ptr
        /* Pointer to a call object */
);


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
extern void cmph_pd_session_start( void );


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
extern void cmph_pd_session_end( void );



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


extern void cmph_check_and_unforce_orig_mode(

  cm_ss_e_type ss
    /* The ss on which to process the orig update
    */
);


/*===========================================================================

FUNCTION cmph_remove_orig_mode

DESCRIPTION
  Remove the orig modes specified

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


extern void cmph_remove_orig_mode(
  cm_ss_e_type ss,
    /* The ss on which to process the orig update
    */

  cm_act_type_e_type act_type,
    /* The activity type to check
    */

  cm_act_priority_e_type prio_type
    /* The priority type to check
    */
);

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
void cmph_notify_mmoc_no_sim ( void );



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
extern boolean cmph_is_waiting_for_reply( void );



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
extern boolean cmph_info_get(

    cm_ph_info_s_type    *ph_info_ptr
        /* Copy phone state info into this buffer */
);



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
extern void cmph_client_cmd_proc(

    cm_cmd_s_type  *cmd_ptr
        /* pointer to a CM command */
);


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
extern void cmph_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD reports */
);


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
extern void cmph_generic_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to generic reports */
);



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
extern void cmph_timer_proc(

    cm_timer_event_type    timer_event
        /* Indicate specific timer event */
);



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
extern cm_nam_e_type cmph_get_curr_nam( void );



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
extern boolean cmph_autonam_is_enabled( void );



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
void cmph_offline (void);



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
extern void cmph_orig_mode_reset(

    cmph_s_type             *ph_ptr,
        /* Pointer to a phone object */

    boolean                  force_top
        /* Force the top preferences */
);



/**--------------------------------------------------------------------------
** Functions - cdma
** --------------------------------------------------------------------------
*/

//#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)

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
extern boolean cmph_is_valid_mode_pref(

    cm_mode_pref_e_type    mode_pref
        /* mode preference to check */
);

//#endif /*FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

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
extern dword cmph_force_pref_on_the_fly(

    cm_ss_e_type            ss,
      /* SS to use for origination.
      */

    cmph_s_type             *ph_ptr,
        /* Pointer to a phone object */

    sd_ss_pref_reas_e_type  force_reason,
        /* Reason for forcing the SS-preference.
        */

    cm_act_type_e_type      act_type,
        /* Act type for which to force the preferences */

    cm_pref_term_e_type     force_term,
        /* Term for which to force the preferences - one-call, etc. */

    cm_mode_pref_e_type     mode_pref,
        /* Mode preference which to force */

    cm_band_pref_e_type     band_pref,
        /* Band preference which to force */

    cm_prl_pref_e_type      prl_pref,
        /* PRL preference which to force */

    cm_roam_pref_e_type     roam_pref,
        /* Roam preference which to force */

    cm_hybr_pref_e_type     hybr_pref,
        /* hybr preference which to force */

    sys_plmn_id_s_type      plmn_id,
       /* The PLMN id to be acquired, valid when the net_sel_mode_pref
       ** is MANUAL.
       ** Applies only for GSM/WCDMA modes.
       */

    cm_srv_domain_pref_e_type srv_domain_pref,
       /* service domain preference to force */

    cm_activate_code_e_type otasp_act_code,
        /* OTASP call activation code */

    const cm_wlan_pref_s_type       *wlan_pref,
        /* WLAN preference */

    cm_gw_acq_order_pref_e_type      acq_order_pref,
       /* The current acquisition order preference setting */

    cm_network_sel_mode_pref_e_type network_sel_mode_pref,
      /* The current network sel mode */

    cm_act_id_type            act_id,
        /* The uniques id of the activity that is forcing the mode */

    cm_act_update_reas_e_type act_update_reas,
        /* Reason for the update */

    boolean                   force_top_pref
        /* Force the top ptreven if the top didn't change */
);


/*===========================================================================

FUNCTION cmph_unforce_mode_on_the_fly

DESCRIPTION
  Unforce the mode preference on the fly

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  A ballpark estimation for the number of seconds it takes the phone to
  reacquire service. If mode is not forced return 0.

SIDE EFFECTS
  none

===========================================================================*/
extern dword cmph_unforce_mode_on_the_fly(

    cmph_s_type    *ph_ptr
        /* Pointer to a phone object */
);



#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)


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
extern void cmph_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);



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
extern void cmph_mo_sms_end(

    dword          duration,
       /* when to unforce the mode */

    cm_act_id_type act_id
       /* The id of the activity, used to access the orig parameter Q */
);



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
extern void cmph_send_nam_change_to_mc(

    const cmph_s_type    *ph_ptr,
        /* Pointer to a phone object */

    cm_nam_e_type  nam
        /* The current NAM */
);



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
cm_mode_pref_e_type cmph_get_user_mode_pref (void);


#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

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
void  cmph_update_ddtm_status
(

      cmph_s_type      *ph_ptr
        /* Pointer to a phone object */
);


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
extern void cmph_send_hdrmc_session_close_cmd( void );
#endif


/**--------------------------------------------------------------------------
** Functions - gsm/wcdma
** --------------------------------------------------------------------------
*/

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* defined(FEATURE_GSM) || defined (FEATURE_WCDMA) */


/*===========================================================================^M

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
void cmph_update_orig_mode_uptime (cm_ss_e_type ss, cm_act_id_type act_id );


/*===========================================================================^M

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
boolean cmph_is_cdma_subscription_available (void);


/*===========================================================================^M

FUNCTION cmph_is_gw_subscription_available

DESCRIPTION
  This function returns a boolean indicating whether GSM/WCDMA subscription is available.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_gw_subscription_available (void);


/*===========================================================================

FUNCTION cmph_get_cdma_subscription_source

DESCRIPTION
  This function will return where cdma is currently getting it's subscription or none if there
  is no subscription available.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  source of cdma subscription

SIDE EFFECTS
  none

===========================================================================*/
cm_cc_subscription_source_e_type cmph_get_cdma_subscription_source (void);


/*===========================================================================

FUNCTION cmph_get_gw_subscription_source

DESCRIPTION
  This function will return where gw is currently getting it's subscription or none if there
  is no subscription available.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  source of gw subscription

SIDE EFFECTS
  none

===========================================================================*/
cm_cc_subscription_source_e_type cmph_get_gw_subscription_source (void);


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
cm_ps_data_orig_prefs_e_type cmph_get_ps_data_orig_prefs (void);



/*===========================================================================

FUNCTION cmph_map_cm_mode_pref_to_sd_mode_pref

DESCRIPTION
  Map CM mode-preference type to SD mode-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The SD mode-preference that corresponds to the input CM mode-preference.

SIDE EFFECTS
  none

===========================================================================*/
extern sd_ss_mode_pref_e_type   cmph_map_cm_mode_pref_to_sd_mode_pref(

    cm_mode_pref_e_type    mode_pref
        /* Mode pref to map */
);

/*===========================================================================
FUNCTION cmph_map_sd_mode_pref_to_cm_mode_pref

DESCRIPTION
  Map SD mode-preference type to CM mode-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The CM mode-preference that corresponds to the input SD mode-preference
  or CM_MODE_PREF_MAX if not mapped.

SIDE EFFECTS
  None

===========================================================================*/
extern cm_mode_pref_e_type cmph_map_sd_mode_pref_to_cm_mode_pref(

    sd_ss_mode_pref_e_type    mode_pref
        /* Mode pref to map */
);


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
extern sd_mode_e_type cmph_map_sys_sys_mode_to_sd_mode(sys_sys_mode_e_type mode);



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
void cmph_activate_ps_service_req_timer( void );



/*===========================================================================

FUNCTION cmph_map_mode_pref_to_sd_mode_pref

DESCRIPTION
  Map CM mode-preference type to SD mode-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The SD mode-preference that corresponds to the input CM mode-preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_mode_pref_e_type   cmph_map_mode_pref_to_sd_mode_pref(

    cm_mode_pref_e_type    mode_pref
        /* Mode pref to map */
);


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
extern sd_ss_orig_mode_e_type  cmph_map_cm_network_sel_mode_pref_to_orig_mode
(
  cm_network_sel_mode_pref_e_type    network_sel_mode_pref
);



/*===========================================================================

FUNCTION cmph_call_end_req

DESCRIPTION
  Does necessary phone's call end req activities. It is applicable
  for GSM/WCDMA only.

  Currently this function just sends the CMPH_EVENT_IN_USE_STATE if
  the overall call state is IDLE.

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
extern void cmph_call_end_req
(
    const cmcall_s_type    *call_ptr
      /* Pointer to a call object */
);

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
extern void cmph_last_gw_pkt_call(void);


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
extern void cmph_set_ps_sig_down_wait_time(dword time);

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
extern boolean cmph_is_waiting_for_ps_sig_down(void);


/*===========================================================================

FUNCTION cmph_als_reset

DESCRIPTION

  ALS custom object was reset due to PH receiving subscription available,
  not available or changed event, so reset ALS settings in PH object.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_als_reset(void);

/*===========================================================================

FUNCTION cmph_als_supported

DESCRIPTION

  ALS is supported when:

  - CPHS is allowed
  - ALS is allowed
  - ALS custom object has all custom logic functions registered

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

  ALS custom logic object must have already been initialized with
  cmals_init().


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cmph_als_supported(void);

/*===========================================================================

FUNCTION cmph_term_pending_get_networks_req

DESCRIPTION

  This function should be called when the phone preferences have changed.

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmph_term_pending_get_networks_req
(
    sd_ss_e_type                   ss
        /* Indicates the SS-Instance ( SS_MAIN or SS_HDR) based on which
        ** the corresponding getNetworks requests are terminated.
        */
);


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
extern boolean cmph_is_valid_wlan_pref
(
  const cm_wlan_pref_s_type               *wlan_pref_ptr
    /* WLAN Network selection mode pref to check for validity
    */
);

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
boolean cmph_is_wlan_subscription_available
(
  void
);


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
);


/*===========================================================================

FUNCTION cmph_process_wlan_wps_result

DESCRIPTION
  This function processes a WLAN WPS result received from SD.

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_process_wlan_wps_result
(
  const cm_wlan_wps_result_type *result_ptr
    /* Pointer to lower level report data */
);

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
);

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
);

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
);
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
);

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
);

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
);


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
net_file_parser_s_type* cmph_get_net_parser_ptr
(
  void
);

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
byte* cmph_get_net_wlan_file_buf
(
  void
);

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
net_profile_s_type* cmph_get_wlan_profile_ptr
(
  void
);

/*===========================================================================

FUNCTION cmph_add_ccbs_entry

DESCRIPTION

  ccbs index and called party get stored in phone object

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE  - ccbs index could be added with success
  FALSE - ccbs index could not be added

SIDE EFFECTS
  ccbs_indx_store gets changed

===========================================================================*/
extern boolean cmph_add_ccbs_entry (

    byte                              ccbs_index,
      /* ccbs index to be added */

   cm_called_party_bcd_no_s_type      called_party
      /* Called party number */

);


/*===========================================================================

FUNCTION cmph_check_ccbs_entry

DESCRIPTION

  checks if ccbs index is present in any entry in phone object

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE

  TRUE  - ccbs index is present as a phone object entry
  FALSE - ccbs index is not present in phone object

SIDE EFFECTS
  ccbs_indx_store gets changed

===========================================================================*/
extern boolean cmph_check_ccbs_entry (

    byte                              ccbs_index
      /* ccbs index to be checked for */
);

/*===========================================================================

FUNCTION cmph_remove_ccbs_entry

DESCRIPTION

  Remove ccbs index from phone object entry.

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE

  TRUE  - ccbs index remove oepration succeeded.
  FALSE - ccbs index remove operation failed.

SIDE EFFECTS
  ccbs_indx_store gets changed

===========================================================================*/
extern boolean cmph_remove_ccbs_entry (

    byte                              ccbs_index
      /* Remove ccbs entry from phone object */
);


/*===========================================================================

FUNCTION cmph_clear_ccbs_entries

DESCRIPTION

  Clears current list of ccbs indexes in phone object

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE

  None

SIDE EFFECTS
  ccbs_indx_store gets changed

===========================================================================*/
extern void cmph_clear_ccbs_entries(void);


/*===========================================================================

FUNCTION cmph_cmd_client_release

DESCRIPTION
  Inform CM that a client was released so that CM can inform asynchronously
  free the client object.

  This is mostly important when a client release happens in the client
  callback function that is called during an event, or immediately after it
  (in case of multiprocessor builds).

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
extern void cmph_cmd_client_release(

  cm_client_s_type        *client_ptr
    /**< Client object that should be released */

);

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

extern boolean cmph_update_orig_param(

  cm_act_id_type            act_id,
    /* The id of the activity whose orig_params are to be updated
    */
  cm_ss_e_type              ss,
    /* The ss on which the activity was going on
    */
  cm_act_update_reas_e_type act_update_reas
    /* The reason for updating the activity
    */
);

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

extern boolean cmph_update_orig_param_incoming_call
(
  const cmcall_s_type* call_ptr
    /* Pointer to the call object
    */
);

/*===========================================================================

FUNCTION cmph_get_act_priority

DESCRIPTION
  Maps the act_type to act_priority
DEPENDENCIES


RETURN VALUE
  The activity type cm_act_type_e_type

SIDE EFFECTS
  None.

===========================================================================*/

extern cm_act_priority_e_type cmph_get_act_priority(

  cm_act_type_e_type act_type
    /* Activity Type */

);

/*===========================================================================

FUNCTION cmph_map_call_type_to_act_type

DESCRIPTION
  Maps the call_type to activity type

DEPENDENCIES


RETURN VALUE
  The activity type cm_act_type_e_type

SIDE EFFECTS
  None.

===========================================================================*/


extern cm_act_type_e_type cmph_map_call_type_to_act_type(

  cm_call_type_e_type call_type
    /* Call Type */

);

/*===========================================================================

FUNCTION cmph_cmp_act_priorities

DESCRIPTION
  Maps the act_type to act_priority
DEPENDENCIES


RETURN VALUE
  -1 if act_priority_1 <  act_priority_2
   0 if act_priority_1 == act_priority_2
   1 if act_priority_1 >  act_priority_2

SIDE EFFECTS
  None.

===========================================================================*/

extern int cmph_cmp_act_priorities(

  cm_act_priority_e_type act_priority_1,
    /* 1st activity's priority */

  cm_act_priority_e_type act_priority_2
    /* 2nd activity's priority */

);


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

extern boolean cmph_insert_orig_mode(

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

  const cm_wlan_pref_s_type      *wlan_pref,
      /* WLAN preference */

  cm_srv_domain_pref_e_type srv_domain_pref,
     /* service domain preference to force */

  cm_act_id_type            act_id,
      /* The uniques id of the activity that is forcing the mode */

  cm_act_update_reas_e_type act_update_reas
      /* Reason for the update */
);


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
extern cm_srv_domain_pref_e_type cmph_add_srv_domain_pref(

  cm_srv_domain_pref_e_type orig_srv_domain_pref,
    /* The original srv_domain pref
    */

  cm_srv_domain_pref_e_type srv_domain_pref_to_add
    /* The srv_domain_pref to add
    */
);

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
extern cm_act_orig_s_type* cmph_get_last_pref_sent_on_main(void);


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
extern boolean * cmph_get_is_kicked_hybr_ptr(void);

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

extern boolean * cmph_hdr_kick_pending( void );
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
);


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
sd_ss_hybr_pref_e_type cmph_map_cm_hybr_pref_to_sd_hybr_pref
(
  cm_hybr_pref_e_type       hybr_pref
    /* hybr pref to map */
);

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
);

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

extern cm_ss_e_type cmph_map_sd_ss_type_to_cm_ss_type(

  sd_ss_e_type sd_ss

);

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
    /* cm_prl_pref */
);


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
);

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
);


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
);


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
);


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
);

/*===========================================================================

FUNCTION cmph_set_nam_chg_pend

DESCRIPTION
  Sets the nam change to pending state.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void  cmph_set_nam_chg_pend (

  boolean      nam_chg_pend
    /*  Nam change pending flag */
);


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
extern boolean  cmph_is_nam_chg_pend (void);

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
/*===========================================================================

FUNCTION cmph_otasp_update_mode

DESCRIPTION
  Check the mode in NV and update the mode

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

  CM will not write this mode to NV.  It is expected this value has already
  been written to NV.

RETURN VALUE
  none

SIDE EFFECTS
  Phone mode may be changed to reflect the NV item

===========================================================================*/
void cmph_otasp_update_mode(
   cm_nam_e_type        nam,
   nv_mode_enum_type    nv_mode
);
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

/*===========================================================================

FUNCTION cmph_get_gprs_anite_gcf_ptr

DESCRIPTION
  Return a pointer to gprs anite gcf flag

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
extern boolean *cmph_get_gprs_anite_gcf_ptr(void);


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
extern void cmph_ddtm_disable_cdma_tx
(

       cmph_s_type          *ph_ptr
         /* Pointer to phone object.
         */
);

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
extern void cmph_send_ddtm_status
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

);


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
extern void cmph_ip_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD reports */
);

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
extern cm_ph_info_s_type  *cmph_info_alloc( void );

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
extern cm_srv_type_e_type cmph_get_srv_type( cm_call_type_e_type );

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
extern cm_srv_type_e_type cmph_map_sys_mode_to_srv_type(
         sys_sys_mode_e_type sys_mode
);

#if defined( FEATURE_HICPS_STACK_SYNC_UP ) || defined( FEATURE_BSR2)
/*===========================================================================

FUNCTION cmph_kick_hdr_if_pending

DESCRIPTION
 Check is HDR stack needs to be kicked and if yes deliver it.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_kick_hdr_if_pending( void );
#endif

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
);

#endif /* CMPH_H */

