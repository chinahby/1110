#ifndef SD_H
#define SD_H

/**===========================================================================
@FILE_DOC

                   S Y S T E M   D E T E R M I N A T I O N

                             H E A D E R   F I L E

@DESCRIPTION
  This header file contains all the definitions necessary for
  SD clients to interface with the System Determination front-end.

Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/**===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/inc/sd.h#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/09   rk      Adding new pref_reason SD_SS_PREF_REAS_MULTIMODE_SCAN
05/30/09   aj      Adding sd_daylt_savings_e_type for backward compatibility
05/27/09   aj      Adding support for updating clients with cdma local time
04/09/09   aj      Added support for sd_misc_is_target_supp_mode_band_chan
03/31/09   mh      Added support for prl match indicator in ss info
03/09/09   rm      Adding wrapper function - sd_ss_get_supp_mode_pref
02/13/09   mh      Added support for default roaming indicator in ss info
02/06/09   aj      WLAN subscription decoupling 
10/28/08   jd      Correcting daylight saving setting 
10/24/08   ak      Added the HTORPC comments for sd_ss_ind_wlan_acq_scan_failed2
10/23/08   ak      Fixing the no firmware scenario
10/02/08   aj      Bandclass support for BC17, 18, 19
09/26/08   cl      Changing the function description
09/18/08   rm      Adding acq_order_pref in sd_ss_act_get_net_gw_s_type.
09/15/08   pk      Added support for WLAN UOoS
09/04/08   sv      Added call-back function for WPS Result
07/30/08   sv      Added SD_SS_PREF_REAS_STACK_SYNC_UP to sync up GW and Hybr
                   HDR
06/20/08   sn      BSR 2.0 changes
06/18/08   sn      Adding cs_service_status for Managed Roaming
06/11/08   fj      Added API sd_misc_get_mode_band_capability
05/22/08   vk/sv   added roam only preference
04/30/08   cl      Adding sd_misc_sys_avoid_list to list all the avoided 
                   systems. HDR/SCHR use this info to update their scan list
                   when doing OFS.
04/28/08   ak      Added sd_ss_ind_cdma_opr_prot_mis & ssscr_hdr_opr_prot_mis
04/24/08   rm      Correcting comment style
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
04/14/08   ak      Updated the metacomments for sd_ss_act_acq_wlan_s_type.
04/07/08   cl      Update Function's Description sd_misc_is_fts_mode()
03/14/08   rm      CM-NAS interface for new service req type
03/11/08   vk      Corrected code for GW case for SD_SS_MODE_PREF_GW
03/05/08   vk      corrected code for HYBR_GW configuration
02/04/08   cl      Added SD API for sd_misc_is_fts_mode to determine if the 
                   phone is in FULL_TIME_SHDR Mode or not.
01/04/08   ka      Adding handset based plus code dial feature
                   New API for informing Daylight savings and LTM Offset as
                   part of schm.
11/21/07   dm      Added SD_SS_BAND_PREF_GSM_RGSM_900 in SD_BAND_GROUP1
10/02/07   bh      Fixed Metacomment for sd_ss_act_s_type.prm.void
09/14/07   dm      Added support for FEATURE_MM_SUPERSET
09/06/07   cl      KDDI request not to restart the TMR_HOLD_RESEL when we exit
                   session related traffic.
08/30/07   cl      Externalized sdtask_crit_sect_ptr()
08/25/07   jqi/cl  Added subblock I,J,K,L.
06/15/07   tb/ic   Fixed typo in RPC metacomments
06/15/07   rp      Adding sd_ss_ind_gw_opr_srv_lost2
06/15/07   hn/ic   Adjusted fix for RPC metacomments
06/15/07   sk      Fixed RPC metacomments.
06/11/07   sk      Adding support for passing netlist system record index
                   for WLAN acquisition and measurement.
06/06/07   pk      UOOS enhancements...
03/28/07   pk      FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH
05/07/07   sk      Added RPC metacomments for WLAN
04/23/07   jqi     Added Filtering support for stopping HDR activity.
                   Added support for sd_misc_is_orig_mode.
04/17/07   rp      Added RPC metacomments for WLAN
03/30/07   jqi     Fixed the link error.
03/28/07   jqi     Fixed the compilation error.
03/23/07   pk      Removing BM macro, already defined in sys.h
02/26/07   sk      Added SD_SS_PWR_SAVE_REAS_PWR_DOWN to request protocol to
                   deactivate with power-down deregistration (if supported by
                   protocol).
02/15/07   jqi     Added support for sd_ss_ind_cdma_opr_oh_info3
02/02/07   jqi     Removed the band class subblock validation from SD except
                   for CELL and PCS band.
01/04/07   pk      Added support for VoIP -> 1X Handoffs
11/29/06   ka      Limited service resel timer expiry results in sending
                   MEAS_BACK action instead of full service req. Satisfies
                   GCF 6.1.2.6.
11/17/06   jqi     Added support for HDR active protocol revision information.
11/16/06   pk      Using sys_simultaneous_mode_e_type for hybr_gw
11/15/06   pk      Added support for HYBR_GW
11/13/06   ic      Lint fixes
11/07/06   pk      Added support for hybrid GW
11/07/06   sk      Memory reduction effort.
                   - Featurized HDR SID size.
10/10/06   jqi     Added CDMA450 BLOCK L support.
10/02/06   jqi     Enable the 1x acquisition throttling when HDR SCL is low.
09/22/06   rp      Added BC15, BC16 support.
09/17/06   sk      Added SD_SS_BAND_PREF_WLAN_2400, SD_SS_BAND_PREF_WLAN_5000
                   and SD_SS_BAND_PREF_WLAN_ANY.
09/11/06   rp      Addded field wlan_tech_pref to sd_ss_act_acq_wlan_s_type
09/08/06   sk      Added WLAN scan pref as a parameter of measurement action
                   structure sd_ss_act_meas_s_type.
09/05/06   pk      FEATURE_UMTS_UNIFORM_OOS_HANDLING, added support to
                   query UOOS timers
08/29/06   sk      Added WLAN Adhoc support.
08/25/06   pk      Added support for sd_ss_ind_gw_acq_plmn_failed2
08/15/06   pk      Added support for GW UOOS
08/14/06   jqi     Added CDMA system exit due to unlock RF resources.
08/07/06   pk      Added support for UOOS
08/08/06   sk      Added SD_SS_MODE_PREF_ANY_BUT_HDR_WLAN and
                   SD_SS_MODE_PREF_HDR_WLAN.
07/13/06   jqi     Added the system lost reason for HDR system lost event.
05/08/06   sk      Added support for expected SID for HDR.
04/17/05   jqi     Added EGSRDM support.
04/01/06   ka      Adding plmn list status to network list cnf.
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
03/07/06   sk      Added support for SD_SS_SRV_DOMAIN_PREF_NONE.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
01/12/06   pk      Added sd_misc_is_sys_same
12/21/05   jqi     Added background PLMN search support.
12/07/05   ka      Adding sd_ss_ind_gw_data_suspended() to inform CM about
                   data suspend.
12/02/05   jqi     Lint fixes for lint ver8.
11/18/05   pk      Added support for sub-blocks I,J,K for BC 5 and BC 11
11/08/05   jqi     Added support for 1x acquisition throttling for BCMCS
                   activity.
11/02/05   sk      Fixed all Lint and compiler warnings.
                   Added WLAN bands/band_prefs mapping for all 2400 and 5000
                   bands.
10/18/05   pk      Added sd_misc_get_common_mode_pref
10/14/05   jqi     Added BC14 support.
09/10/05   pk      Added srv_acq_time to sd_si_info_s_type
08/18/05   jqi     Realign sdsr_s_type data structure.
07/11/05   jqi     Added avoidance timer for access failures and fades.
07/08/05   sk      Added declaration for 2 new APIs (see sd.c).
06/26/05   ic      Designated SD_SS_BAND_PREF_ANY as ULL (unsigned long long)
                   Fixed '3FFFFFFFFFFFFFFF' treated as '3FFFFFFFFFFFFFFFll'
                   compiler warnings (ADS 1.2 / RVDS 2.1)
05/23/05   ka      Correcting prototype for sd_ss_ind_wlan_bad_sys().
05/13/05   ka      API to avoid a bad WLAN system
05/04/05   jqi     Provided the API to return supported band preference
                   according to target capability and user mode/band pref.
                   Added support to process HDR bad system event while in
                   acquisition state.
04/29/05   pk      Added sd_misc_sys_id_print() to print the sys id
02/17/05   ka      Band preference macros use 64 bit masks.
01/30/05   sj      Added support for WLAN.
01/27/05   ka      Added enums and defines for Band class 12
11/16/04   sj      Changed names for AVOID_SYS types.
11/09/04   jqi     lint fix.
11/02/04   jqi     Added API to validate the mode preference and band class
                   against the target capability
09/08/04   jqi     Added ps dormant optimization.
08/26/04   jqi     Added WCDMA 1800 band support.
08/26/04   jqi     Lint fix.
08/24/04   ws      Remove references to std.h
08/20/04   sj      Added support for Auto A/B.
08/06/04   sj      Changed sd_ss_*_act_get() to return the action other than
                   CONTINUE.
08/02/04   sj      Backed out Featureize baseid/lat/long/srch_win_n in ss_info.
07/30/04   jqi     Featureize baseid/lat/long/srch_win_n in ss_info.
                   Qualband optimization - eliminate scheduling.
07/26/04   sj      Changes for camped_ind support.
07/23/04   sj      Added support for baseid/lat/long/srch_win_n in ss_info.
07/15/04   sj      Split pref_reas_orig_start to orig_start_cs &
                   orig_start_ps.
07/09/04   sj      Changed prototype for misc_get_gw_band_mode().
06/28/04   sj      Moved macros for mode/band pref from sdi.h
06/07/04   jqi     More quad-band enhancement for automatic mode.
05/25/04   jqi     Fixed mode preference reset issue after power up.
05/17/04   sj      Updated support for BCMCS.
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/29/04   ic      Removed SD_SS_MODE_PREF_ANY_BUT_HDR_AND_AMPS at it's
                   identical to SD_SS_MODE_PREF_DIGITAL_LESS_HDR
04/27/04   RC      Added SD_SS_ACC_REAS_TRAFFIC for TC to idle transition.
04/19/04   jqi     Lint fix - for error level: error.
04/13/04   sj      Added new SD indications for HDR conversation and release.
03/18/04   jqi     Introduced the true act type in sd_ss_s_type.
                   Kept band and mode consistent before passing to the GW
                   stack.
                   lint fix.
03/12/04   jqi     Use service domain form service indicator for call orig.
03/10/04   jqi     Propagated service domain for call origination.
03/02/04   ic      Added BCMCS support
02/18/04   JQ      Complied band pref with MS Radio Access capability before
                   passing down to the stack.
02/04/04   SJ      Added flag in acq_gw to indicate if orig is pending.
01/22/04   JQ      Changed SDSR_GW_LTD_SYS and SDSR_USER_SYS to lists with
                   appropriate grouping of GW bands.
01/20/04   jqi     Added band groups support.
12/12/03   RC      Added support for FEATURE_INT_ROAMING
12/09/03   jqi     CR 38023- MRU3 misaligned mem access due to pointer
                   casting.
12/04/03   jqi     Fixed a null pointer issue when populate MRU table from NV.
11/17/03   SJ      Updates for HDR usability and new hdr power save.
11/14/03   JQ      Fixes for IS683D support.
11/11/03   ic      Removed SD_MODE_NONE from #if and #else parts of STD_H
                   featurization
11/07/03   ic      Added SD_MODE_NONE
11/06/03   RC      Optimized the side of sd_sys_s_type and sd_plmn_s_type.
11/05/03   JQ      Do not error fatal while validating PRL.
                   More MRU3 updates.
10/31/03   JQ      Added support for latest IS-683D format changes.
                   Declared prototype of MRU3 table and NV only reserved the
                   space of MRU3 table.
10/14/03   SJ      Added @FUNCTION sd_misc_add_mode_pref.
10/09/03   jqi     Adjusted band preference enum.
09/05/03   SJ      Added @FUNCTIONs sd_misc_is_mode_pref &
                   sd_misc_remove_mode_pref.
                   Added new mode_pref values.
08/26/03   SJ      Added mode to ok_to_orig cb.
                   Added pwr save reason in act_pwr_save SS Action.
08/20/03   SJ      Added new SS-Indication sd_ss_ind_misc_cnf.
04/24/03   RC      Fixed the values of existing sd_hdr_acq_mode_e_type
08/18/03   RC      Fixed issues with IS-683D implementation.
07/18/03   JQ      Added Extended PRL support
                   - D PRL
                   - PRL auto detection.
                   - Passing down band preference to the stack.
07/17/03   SJ      Added SI call back to inform clients registration was
                   done succesfully.
04/24/03   RC      Fixed the values of existing enums that are being reported
                   in SD events.
03/04/03   SJ      Added support for limited automatic mode using API
                   sd_misc_get_curr_acq_sys_mode.
02/26/03   SJ      Added support for Band Class 8 & 9.
01/20/03   jqi     Added Band class 6 support.
01/16/03   SJ      Featurized SS-Indications for CDMA, changed DB_ROAM_*
                   to use SYS_ROAM_*
12/03/02   SJ      Added extern decl for sd_ss_ind_hdr_user_ss_pref2
11/07/02   RC      Added support for HDR acquisition mode.
05/17/02   RC      Added support for IS-683C, PN and data association fields.
04/03/02   RC      Move SD_SS_MODE_PREF_GPS past SD_SS_MODE_PREF_ANY.
03/08/02   hxw     Added GPS mode support for MSBased gpsOne.
02/04/02   mpa     Externalized sd_ss_log_last_action().
01/25/02   sj      Added support Tiered Service - User zone feature.
12/12/01   RC      Added support CDMA-HDR hybrid operation.
12/11/01   sj      Added "sd_si_roam_update" to set the roam indication on
                   serving system.
12/07/01   jqi     Added Band class 5 support.
10/30/01   RC      Added support for IS-683C collcated systems.
09/07/01   RC      Added support for T53 and JCDMA features.
07/20/01   RC      Added support for IS-683C.
04/24/01   RC      Added SD_SS_MODE_PREF_CDMA_AMPS
04/12/01   RC      Changes to support HDR subnet ID.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
02/01/01   RC      Changes per 8th week of I&T.
01/12/01   RC      Changes per 6th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
12/14/00   RC      Changes per 3rd week of I&T.
12/04/00   RC      Added sd_ss_ind_amps_opr_acc_sid().
11/17/00   RC      Initial9 release.

===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */

#include "sys.h"       /* Declarations for common types. */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_MM_SUPERSET)
#include "sys_plmn_selection.h"
                       /* Declaractions for Acquire GW types. */
#endif

#include "sys_wlan.h"

#include "nv.h"        /* Interface to NV services */

#ifdef FEATURE_USERZONE
#include "puzl.h"      /* interface to PUZL services */
#endif /* FEATURE_USERZONE */


/* <EJECT> */
/**===========================================================================
=============================================================================
=============================================================================
============================== COMMON TYPES =================================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup CT Common Types
*  @{
*/

/** SD value for a wildcard SID
*/
#define SD_WILDCARD_SID   0

/** SD value for a wildcard NID
*/
#define SD_WILDCARD_NID   65535

/**
** Invalid system record index.
** Currently used only for WLAN system record index.
*/
#define SD_INVALID_SYS_REC_INDEX  ((word) -1)

#ifdef FEATURE_USERZONE
/** Invalid user zone id
*/
#define SD_INVALID_UZID   -1

#endif /** FEATURE_USERZONE */

#define SD_MAX_TIME_DURATION_BETWEEN_FADE       20

/** @}
**
*/

/* <EJECT> */
/*---------------------------------------------------------------------------
                             System Attributes
---------------------------------------------------------------------------*/

/** @defgroup SA System Attributes
*  @{
*/

#define SD_MODE_NONE      SYS_SYS_MODE_NONE
#define SD_MODE_INACT     SYS_SYS_MODE_NO_SRV
#define SD_MODE_AMPS      SYS_SYS_MODE_AMPS
#define SD_MODE_CDMA      SYS_SYS_MODE_CDMA
#define SD_MODE_HDR       SYS_SYS_MODE_HDR
#define SD_MODE_GPS       SYS_SYS_MODE_GPS
#define SD_MODE_GSM       SYS_SYS_MODE_GSM
#define SD_MODE_WCDMA     SYS_SYS_MODE_WCDMA
#define SD_MODE_GW        SYS_SYS_MODE_GW
#define SD_MODE_WLAN      SYS_SYS_MODE_WLAN
#define SD_MODE_MAX       SYS_SYS_MODE_MAX
#define sd_mode_e_type    sys_sys_mode_e_type


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#define SD_BAND_NONE                 SYS_BAND_CLASS_NONE
#define SD_BAND_BC0                  SYS_BAND_BC0
#define SD_BAND_BC1                  SYS_BAND_BC1
#define SD_BAND_BC3                  SYS_BAND_BC3
#define SD_BAND_BC4                  SYS_BAND_BC4
#define SD_BAND_BC5                  SYS_BAND_BC5
#define SD_BAND_BC6                  SYS_BAND_BC6
#define SD_BAND_BC7                  SYS_BAND_BC7
#define SD_BAND_BC8                  SYS_BAND_BC8
#define SD_BAND_BC9                  SYS_BAND_BC9
#define SD_BAND_BC10                 SYS_BAND_BC10
#define SD_BAND_BC11                 SYS_BAND_BC11
#define SD_BAND_BC12                 SYS_BAND_BC12
#define SD_BAND_BC14                 SYS_BAND_BC14
#define SD_BAND_BC15                 SYS_BAND_BC15
#define SD_BAND_BC16                 SYS_BAND_BC16
#define SD_BAND_BC17                 SYS_BAND_BC17
#define SD_BAND_BC18                 SYS_BAND_BC18
#define SD_BAND_BC19                 SYS_BAND_BC19
#define SD_BAND_GSM_450              SYS_BAND_GSM_450
#define SD_BAND_GSM_480              SYS_BAND_GSM_480
#define SD_BAND_GSM_750              SYS_BAND_GSM_750
#define SD_BAND_GSM_850              SYS_BAND_GSM_850
#define SD_BAND_GSM_EGSM_900         SYS_BAND_GSM_EGSM_900
#define SD_BAND_GSM_RGSM_900         SYS_BAND_GSM_RGSM_900
#define SD_BAND_GSM_PGSM_900         SYS_BAND_GSM_PGSM_900
#define SD_BAND_GSM_DCS_1800         SYS_BAND_GSM_DCS_1800
#define SD_BAND_GSM_PCS_1900         SYS_BAND_GSM_PCS_1900
#define SD_BAND_WCDMA_I_IMT_2000     SYS_BAND_WCDMA_I_IMT_2000
#define SD_BAND_WCDMA_II_PCS_1900    SYS_BAND_WCDMA_II_PCS_1900
#define SD_BAND_WCDMA_III_1700       SYS_BAND_WCDMA_III_1700
#define SD_BAND_WCDMA_IV_1700        SYS_BAND_WCDMA_IV_1700
#define SD_BAND_WCDMA_V_850          SYS_BAND_WCDMA_V_850
#define SD_BAND_WCDMA_VI_800         SYS_BAND_WCDMA_VI_800
#define SD_BAND_WCDMA_VII_2600       SYS_BAND_WCDMA_VII_2600
#define SD_BAND_WCDMA_VIII_900       SYS_BAND_WCDMA_VIII_900
#define SD_BAND_WCDMA_IX_1700        SYS_BAND_WCDMA_IX_1700
#define SD_BAND_WLAN_US_2400         SYS_BAND_WLAN_US_2400
#define SD_BAND_WLAN_JAPAN_2400      SYS_BAND_WLAN_JAPAN_2400
#define SD_BAND_WLAN_EUROPE_2400     SYS_BAND_WLAN_EUROPE_2400
#define SD_BAND_WLAN_FRANCE_2400     SYS_BAND_WLAN_FRANCE_2400
#define SD_BAND_WLAN_SPAIN_2400      SYS_BAND_WLAN_SPAIN_2400
#define SD_BAND_WLAN_US_5000         SYS_BAND_WLAN_US_5000
#define SD_BAND_WLAN_JAPAN_5000      SYS_BAND_WLAN_JAPAN_5000
#define SD_BAND_WLAN_EUROPE_5000     SYS_BAND_WLAN_EUROPE_5000
#define SD_BAND_WLAN_FRANCE_5000     SYS_BAND_WLAN_FRANCE_5000
#define SD_BAND_WLAN_SPAIN_5000      SYS_BAND_WLAN_SPAIN_5000
#define SD_BAND_MAX                  SYS_BAND_CLASS_MAX
#define sd_band_e_type               sys_band_class_e_type

#define SD_BAND_CELL         SYS_BAND_CLASS_CELL
#define SD_BAND_PCS          SYS_BAND_CLASS_PCS


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of system band classes and band classes' sub-bands.
*/
typedef enum {

  SD_SBAND_BC0_A        = SYS_SBAND_BC0_A,
                        /**< Band Class 0, A-System */

  SD_SBAND_BC0_B        = SYS_SBAND_BC0_B,
                        /**< Band Class 0, B-System */
                        /**< Band Class 0, AB - System */

  SD_SBAND_BC1          = SYS_SBAND_BC1,
                        /**< Band Class 1, all blocks*/

  SD_SBAND_BC2          = SYS_SBAND_BC2,
                        /**< Band Class 2 place holder */

  SD_SBAND_BC3          = SYS_SBAND_BC3,
                        /**< Band Class 3, A-System */

  SD_SBAND_BC4          = SYS_SBAND_BC4,
                        /**< Band Class 4, all blocks */

  SD_SBAND_BC5          = SYS_SBAND_BC5,
                        /**< Band Class 5, all blocks */

  SD_SBAND_GSM_DCS_1800 = SYS_SBAND_GSM_DCS_1800,
                        /**< GSM DCS 1800 band */

  SD_SBAND_GSM_EGSM_900 = SYS_SBAND_GSM_EGSM_900,
                        /**< GSM Extended GSM (E-GSM) 900 band (900 MHz) */

  SD_SBAND_GSM_PGSM_900 = SYS_SBAND_GSM_PGSM_900,
                        /**< GSM Primary GSM (P-GSM) 900 band */

  SD_SBAND_BC6          = SYS_SBAND_BC6,
                        /**< Band Class 6 (2 GHz) */

  SD_SBAND_BC7          = SYS_SBAND_BC7,
                        /**< Band Class 7 (Upper 700 MHz) */

  SD_SBAND_BC8          = SYS_SBAND_BC8,
                        /**< Band Class 8 (1800 MHz) */

  SD_SBAND_BC9          = SYS_SBAND_BC9,
                        /**< Band Class 9 (900 MHz) */

  SD_SBAND_BC10         = SYS_SBAND_BC10,
                        /**< Band Class 10 (Second 800 MHz) */

  SD_SBAND_BC11         = SYS_SBAND_BC11,
                        /**< Band Class 11 - European PAMR Band (400 MHz) */

  SD_SBAND_GSM_450      = SYS_SBAND_GSM_450,
                        /**< GSM 450 band (450 MHz) */

  SD_SBAND_GSM_480      = SYS_SBAND_GSM_480,
                        /**< GSM 480 band (480 MHz) */

  SD_SBAND_GSM_750      = SYS_SBAND_GSM_750,
                        /**< GSM 750 band (750 MHz) */

  SD_SBAND_GSM_850      = SYS_SBAND_GSM_850,
                        /**< GSM 850 band (850 MHz) */

  SD_SBAND_GSM_RGSM_900 = SYS_SBAND_GSM_RGSM_900,
                        /**< GSM Railways (R-GSM) 900 band (900 MHz) */

  SD_SBAND_GSM_PCS_1900 = SYS_SBAND_GSM_PCS_1900,
                        /**< GSM PCS band (1900 MHz) */

  SD_SBAND_WCDMA_I_IMT_2000 = SYS_SBAND_WCDMA_I_IMT_2000,
                        /**< WCDMA IMT EUROPE JAPAN & CHINA 2100 MHz */

  SD_SBAND_WCDMA_II_PCS_1900 = SYS_SBAND_WCDMA_II_PCS_1900,
                       /**< WCDMA US PCS 1900 band  */

  SD_SBAND_WCDMA_III_1700 = SYS_SBAND_WCDMA_III_1700,
                         /**< WCDMA EUROPE & CHINA 1800 band */

  SD_SBAND_WCDMA_IV_1700 = SYS_SBAND_WCDMA_IV_1700,
                         /**< WCDMA US 1700 band */

  SD_SBAND_WCDMA_V_850   = SYS_SBAND_WCDMA_V_850,
                         /**< WCDMA US 850 band */

  SD_SBAND_WCDMA_VI_800  = SYS_SBAND_WCDMA_VI_800,
                         /**< WCDMA JAPAN800 band (800 MHZ ) */

  SD_SBAND_BC12          = SYS_SBAND_BC12,
                         /**< Band Class 12 - PAMR Band (800 MHz) */

  SD_SBAND_BC14          = SYS_SBAND_BC14,
                         /**< Band Class 14 (US PCS 1.9GHz Band) */

  SD_SBAND_RESERVED_2    = SYS_SBAND_RESERVED_2,
                         /**< Reserved 2 */

  SD_SBAND_BC15         = SYS_SBAND_BC15,
                         /**< Band Class 15 1700-2100 MHz AWS */

  SD_SBAND_BC16         = SYS_SBAND_BC16,
                         /**< Band Class 16 US2.5GHz */

  SD_SBAND_BC17         = SYS_SBAND_BC17,
                         /**< Band Class 17  (US 2.5GHz Forward Link only band)   */

  SD_SBAND_BC18         = SYS_SBAND_BC18,
                         /**< Band Class 18 (700 MHz Public Safety Broadband)  */

  SD_SBAND_BC19         = SYS_SBAND_BC19,
                         /**< Band Class 19 (Lower 700 MHz band)   */

  SD_SBAND_WLAN_US_2400  = SYS_SBAND_WLAN_US_2400,
                         /**< WLAN US 2400 band */

  SD_SBAND_WLAN_JAPAN_2400 = SYS_SBAND_WLAN_JAPAN_2400,
                           /**< WLAN JAPAN 2400 band */

  SD_SBAND_WLAN_EUROPE_2400  = SYS_SBAND_WLAN_EUROPE_2400,
                         /**< WLAN EUROPE 2400 band */

  SD_SBAND_WLAN_FRANCE_2400 = SYS_SBAND_WLAN_FRANCE_2400,
                           /**< WLAN FRANCE 2400 band */

  SD_SBAND_WLAN_SPAIN_2400 = SYS_SBAND_WLAN_SPAIN_2400,
                           /**< WLAN SPAIN 2400 band */

  SD_SBAND_WLAN_US_5000  = SYS_SBAND_WLAN_US_5000,
                         /**< WLAN US 5000 band */

  SD_SBAND_WLAN_JAPAN_5000 = SYS_SBAND_WLAN_JAPAN_5000,
                           /**< WLAN JAPAN 5000 band */

  SD_SBAND_WLAN_EUROPE_5000  = SYS_SBAND_WLAN_EUROPE_5000,
                         /**< WLAN EUROPE 5000 band */

  SD_SBAND_WLAN_FRANCE_5000 = SYS_SBAND_WLAN_FRANCE_5000,
                           /**< WLAN FRANCE 5000 band */

  SD_SBAND_WLAN_SPAIN_5000 = SYS_SBAND_WLAN_SPAIN_5000,
                           /**< WLAN SPAIN 5000 band */

  SD_SBAND_WCDMA_VII_2600  = SYS_SBAND_WCDMA_VII_2600,
                         /**< WCDMA Europe 2600 band */

  SD_SBAND_WCDMA_VIII_900  = SYS_SBAND_WCDMA_VIII_900,
                         /**< WCDMA Europe & China 900 band */

  SD_SBAND_WCDMA_IX_1700   = SYS_SBAND_WCDMA_IX_1700,
                           /**< WCDMA JAPAN 1700 band  */

  SD_SBAND_MAX

} sd_sband_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of system selection band and sub-band preferences.
**
** Make sure to read the following before adding a new enumeration type.
**
**1. When update here need to update sys_band_mask_type and cm_band_pref_e.
**
**2. Bit 30 is reserved for band preference NO change indicattion.
**
**3. Whenever adding a new band pref enumeration type, make sure to update
**   SD_SS_BAND_PREF_ANY accordingly.
**
*/
typedef  uint64                      sd_ss_band_pref_e_type;

#define  SD_SS_BAND_PREF_NONE        ((sd_ss_band_pref_e_type)0)
                                     /**< Acquire no system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_BC0_A       ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC0_A ))
                                     /**< Acquire band class 0, A-Side systems only
                                     */

#define  SD_SS_BAND_PREF_BC0_B       ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC0_B ))
                                     /**< Acquire band class 0, B-Side systems only
                                     */

#define  SD_SS_BAND_PREF_BC0         ((sd_ss_band_pref_e_type)(SYS_BM_64BIT( SD_SBAND_BC0_A ) | SYS_BM_64BIT( SD_SBAND_BC0_B )))
                                     /**< Acquire band class 0 systems only */

#define  SD_SS_BAND_PREF_BC1         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC1 ))
                                     /**< Acquire band class 1 systems only */

#define  SD_SS_BAND_PREF_BC3         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC3 ))
                                     /**< Acquire band class 3 systems only */

#define  SD_SS_BAND_PREF_BC4         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC4 ))
                                     /**< Acquire band class 4 systems only */

#define  SD_SS_BAND_PREF_BC5         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC5 ))
                                     /**< Acquire band class 5 systems only */

#define  SD_SS_BAND_PREF_BC6         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC6 ))
                                     /**< Acquire band class 6 systems only */

#define  SD_SS_BAND_PREF_BC7         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC7 ))
                                     /**< Acquire band class 7 systems only */

#define  SD_SS_BAND_PREF_BC8         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC8 ))
                                     /**< Acquire band class 8 systems only */

#define  SD_SS_BAND_PREF_BC9         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC9 ))
                                     /**< Acquire band class 9 systems only */

#define  SD_SS_BAND_PREF_BC10        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC10 ))
                                     /**< Acquire band class 10 systems only */

#define  SD_SS_BAND_PREF_BC11        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC11 ))
                                     /**< Acquire band class 11 systems only */

#define  SD_SS_BAND_PREF_BC12        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC12 ))
                                     /**< Acquire band class 12 systems only */

#define  SD_SS_BAND_PREF_BC14        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC14 ))
                                     /**< Acquire band class 14 systems only */

#define  SD_SS_BAND_PREF_BC15        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC15 ))
                                     /**< Acquire band class 15 systems only */

#define  SD_SS_BAND_PREF_BC16        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC16 ))
                                     /**< Acquire band class 16 systems only */

#define  SD_SS_BAND_PREF_BC17        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC17 ))
                                     /**< Acquire band class 17 systems only */

#define  SD_SS_BAND_PREF_BC18        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC18 ))
                                     /**< Acquire band class 18 systems only */

#define  SD_SS_BAND_PREF_BC19        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC19 ))
                                     /**< Acquire band class 19 systems only */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_GSM_DCS_1800 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_DCS_1800 ))
                                     /**< Acquire GSM DCS systems only */

#define  SD_SS_BAND_PREF_GSM_EGSM_900 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_EGSM_900 ))
                                     /**< Acquire GSM Extended GSM systems only */

#define  SD_SS_BAND_PREF_GSM_PGSM_900 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_PGSM_900 ))
                                     /**< Acquire GSM Primary GSM systems only */

#define  SD_SS_BAND_PREF_GSM_450     ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_450 ))
                                     /**< Acquire GSM 450 systems only */

#define  SD_SS_BAND_PREF_GSM_480     ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_480 ))
                                     /**< Acquire GSM 480 systems only */

#define  SD_SS_BAND_PREF_GSM_750     ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_750 ))
                                     /**< Acquire GSM 750 systems only */

#define  SD_SS_BAND_PREF_GSM_850     ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_850 ))
                                     /**< Acquire GSM 850 systems only */

#define  SD_SS_BAND_PREF_GSM_RGSM_900 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_RGSM_900 ))
                                     /**< Acquire GSM Railways GSM systems only */

#define  SD_SS_BAND_PREF_GSM_PCS_1900 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_PCS_1900 ))
                                     /**< Acquire GSM PCS systems only */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_WCDMA_I_IMT_2000 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_I_IMT_2000 ))
                                     /**< Acquire WCDMA IMT 2000 systems only */

#define  SD_SS_BAND_PREF_WCDMA_II_PCS_1900 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_II_PCS_1900 ))
                                     /**< Acquire WCDMA PCS systems only */

#define  SD_SS_BAND_PREF_WCDMA_III_1700  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_III_1700 ))
                                     /**< Acquire WCDMA 1700 systems */

#define  SD_SS_BAND_PREF_WCDMA_IV_1700  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_IV_1700 ))
                                     /**< Acquire WCDMA 1700 systems */

#define  SD_SS_BAND_PREF_WCDMA_V_850    ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_V_850 ))
                                     /**< Acquire WCDMA US850 systems */

#define  SD_SS_BAND_PREF_WCDMA_VI_800  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_VI_800 ))
                                     /**< Acquire WCDMA JAPAN 800 systems */

#define  SD_SS_BAND_PREF_WCDMA_VII_2600  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_VII_2600 ))
                                     /**< Acquire WCDMA Europe 2600 systems */

#define  SD_SS_BAND_PREF_WCDMA_VIII_900  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_VIII_900 ))
                                     /**< Acquire WCDMA Europe JAPAN 900 systems */

#define  SD_SS_BAND_PREF_WCDMA_IX_1700  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_IX_1700 ))
                                     /**< Acquire WCDMA JAPAN 1700 systems */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_WLAN_US_2400  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WLAN_US_2400 ))
                                     /**< Acquire WLAN US 2400 systems */

#define  SD_SS_BAND_PREF_WLAN_JAPAN_2400  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WLAN_JAPAN_2400 ))
                                     /**< Acquire WLAN JAPAN 2400 systems */

#define  SD_SS_BAND_PREF_WLAN_EUROPE_2400  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WLAN_EUROPE_2400 ))
                                     /**< Acquire WLAN EUROPE 2400 systems */

#define  SD_SS_BAND_PREF_WLAN_SPAIN_2400  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WLAN_SPAIN_2400 ))
                                     /**< Acquire WLAN SPAIN 2400 systems */

#define  SD_SS_BAND_PREF_WLAN_FRANCE_2400  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WLAN_FRANCE_2400 ))
                                     /**< Acquire WLAN FRANCE 2400 systems */

#define  SD_SS_BAND_PREF_WLAN_2400        ( SD_SS_BAND_PREF_WLAN_US_2400     | \
                                            SD_SS_BAND_PREF_WLAN_JAPAN_2400  | \
                                            SD_SS_BAND_PREF_WLAN_EUROPE_2400 | \
                                            SD_SS_BAND_PREF_WLAN_FRANCE_2400 | \
                                            SD_SS_BAND_PREF_WLAN_SPAIN_2400 )
                                     /**< Acquire WLAN 2400 systems */

#define  SD_SS_BAND_PREF_WLAN_US_5000  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WLAN_US_5000 ))
                                     /**< Acquire WLAN US 5000 systems */

#define  SD_SS_BAND_PREF_WLAN_JAPAN_5000  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WLAN_JAPAN_5000 ))
                                     /**< Acquire WLAN JAPAN 5000 systems */

#define  SD_SS_BAND_PREF_WLAN_EUROPE_5000  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WLAN_EUROPE_5000 ))
                                     /**< Acquire WLAN EUROPE 5000 systems */

#define  SD_SS_BAND_PREF_WLAN_FRANCE_5000  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WLAN_FRANCE_5000 ))
                                     /**< Acquire WLAN FRANCE 5000 systems */

#define  SD_SS_BAND_PREF_WLAN_SPAIN_5000  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WLAN_SPAIN_5000 ))
                                     /**< Acquire WLAN SPAIN 5000 systems */

#define  SD_SS_BAND_PREF_WLAN_5000        ( SD_SS_BAND_PREF_WLAN_US_5000     | \
                                            SD_SS_BAND_PREF_WLAN_JAPAN_5000  | \
                                            SD_SS_BAND_PREF_WLAN_EUROPE_5000 | \
                                            SD_SS_BAND_PREF_WLAN_FRANCE_5000 | \
                                            SD_SS_BAND_PREF_WLAN_SPAIN_5000 )
                                     /**< Acquire WLAN 5000 systems */

#define  SD_SS_BAND_PREF_WLAN_ANY         ( SD_SS_BAND_PREF_WLAN_2400 | \
                                            SD_SS_BAND_PREF_WLAN_5000 )
                                     /**< Acquire any WLAN systems */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_ANY         ((sd_ss_band_pref_e_type)0x7FFFFFFFFFFFFFFFULL)
                                     /**< Acquire any band class systems */

 /* ULL designates it as "unsigned long long" to get rid of compiler warnings
 ** "literal treated as unsigned long long"
 **
 ** ANY refers to bands from bit position 0 up to 50.
 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_MAX         ((sd_ss_band_pref_e_type)(SD_SS_BAND_PREF_ANY + 1))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Macro for creating a Bit Mask by shifting 1 left by a specified number -
** useful for defining a bit-mask enum values.
*/
#define BM_32BIT( val ) ( 1<< ((uint32)(val)) )

/** Enumeration of system selection band and sub-band preferences in MRU
** formate.
*/
typedef uint32                        sd_mru_band_pref_e_type;


#define SD_MRU_BAND_PREF_NONE         ((sd_mru_band_pref_e_type)(0))

  /**< GSM band pref starts with bit 0 */

#define SD_MRU_BAND_PREF_GSM_450      ((sd_mru_band_pref_e_type)(BM_32BIT(0)))
  /**< GSM band (450 MHz)                          */

#define SD_MRU_BAND_PREF_GSM_480      ((sd_mru_band_pref_e_type)(BM_32BIT(1)))
  /**< GSM band (480 MHz)                          */

#define SD_MRU_BAND_PREF_GSM_750      ((sd_mru_band_pref_e_type)(BM_32BIT(2)))
  /**< GSM band (750 MHz)                          */

#define SD_MRU_BAND_PREF_GSM_850      ((sd_mru_band_pref_e_type)(BM_32BIT(3)))
  /**< GSM band (850 MHz)                          */

#define SD_MRU_BAND_PREF_GSM_EGSM_900 ((sd_mru_band_pref_e_type)(BM_32BIT(4)))
  /**< GSM band E-GSM (900 MHz)                    */

#define SD_MRU_BAND_PREF_GSM_PGSM_900 ((sd_mru_band_pref_e_type)(BM_32BIT(5)))
  /**< GSM band P-GSM (900 MHz)                    */

#define SD_MRU_BAND_PREF_GSM_RGSM_900 ((sd_mru_band_pref_e_type)(BM_32BIT(6)))
  /**< GSM band R-GSM (900 MHz)                    */

#define SD_MRU_BAND_PREF_GSM_DCS_1800 ((sd_mru_band_pref_e_type)(BM_32BIT(7)))
  /**< GSM band DCS-GSM (1800 MHz)                  */

#define SD_MRU_BAND_PREF_GSM_PCS_1900 ((sd_mru_band_pref_e_type)(BM_32BIT(8)))
  /**< GSM band PCS (1900 MHz)                     */

  /**< WCDMA band pref starts with bit 16 */

#define SD_MRU_BAND_PREF_WCDMA_I_IMT_2000  ((sd_mru_band_pref_e_type)(BM_32BIT(16)))
  /**< WCDMA EUROPE JAPAN & CHINA IMT 2100 band        */

#define SD_MRU_BAND_PREF_WCDMA_II_PCS_1900 ((sd_mru_band_pref_e_type)(BM_32BIT(17)))
  /**< WCDMA US PCS 1900 band                          */

#define SD_MRU_BAND_PREF_WCDMA_III_1700    ((sd_mru_band_pref_e_type)(BM_32BIT(18)))
  /**< WCDMA EUROPE&CHINA DCS 1800 band                */

#define SD_MRU_BAND_PREF_WCDMA_IV_1700     ((sd_mru_band_pref_e_type)(BM_32BIT(19)))
  /**< WCDMA US 1700 band                              */

#define SD_MRU_BAND_PREF_WCDMA_V_850       ((sd_mru_band_pref_e_type)(BM_32BIT(20)))
  /**< WCDMA US 850 band                               */

#define SD_MRU_BAND_PREF_WCDMA_VI_800      ((sd_mru_band_pref_e_type)(BM_32BIT(21)))
  /**< WCDMA JAPAN 800 band                            */

#define SD_MRU_BAND_PREF_WCDMA_VII_2600    ((sd_mru_band_pref_e_type)(BM_32BIT(22)))
  /**< WCDMA EUROPE 2600 band                          */

#define SD_MRU_BAND_PREF_WCDMA_VIII_900    ((sd_mru_band_pref_e_type)(BM_32BIT(23)))
  /**< WCDMA EUROPE & CHINA 900 band.                  */

#define SD_MRU_BAND_PREF_WCDMA_IX_1700     ((sd_mru_band_pref_e_type)(BM_32BIT(24)))
  /**< WCDMA JAPAN 1700 band                           */

#define SD_MRU_BAND_PREF_ANY               ((sd_mru_band_pref_e_type)(0xFFFFFFFF))

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for Technology preference.
*/
typedef  sys_tech_mask_type     sd_ss_tech_pref_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of prl preferences.
*/
typedef enum {

  SD_SS_PRL_PREF_NONE         = 0,
                                /**< Internal use only */

  SD_SS_PRL_PREF_AVAIL_BC0_A   = BM( SYS_PRL_AVAIL_BC0_A ),
                               /**< Acquire band class 0, A-Side available
                               ** systems only.
                               */

  SD_SS_PRL_PREF_AVAIL_BC0_B    = BM( SYS_PRL_AVAIL_BC0_B ),
                                /**< Acquire band class 0, A-Side available
                                ** systems only.
                                */

  SD_SS_PRL_PREF_ANY            = 0x3FFF,
                                /**< Acquire any systems */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_SS_PRL_PREF_MAX

} sd_ss_prl_pref_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for SD channel.
*/
#define sd_chan_type      sys_channel_num_type

/** Wildcard channel.
*/
#define SD_WLAN_WILDCARD_CHAN (sd_chan_type)(-1)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of PCS frequency blocks/Cellular systems.
*/
typedef enum {

  SD_BLKSYS_PCS_A    = 0,    /**< PRL_PCS_A Block */
  SD_BLKSYS_PCS_B    = 1,    /**< PRL_PCS_B Block */
  SD_BLKSYS_PCS_C    = 2,    /**< PRL_PCS_C Block */
  SD_BLKSYS_PCS_D    = 3,    /**< PRL_PCS_D Block */
  SD_BLKSYS_PCS_E    = 4,    /**< PRL_PCS_E Block */
  SD_BLKSYS_PCS_F    = 5,    /**< PRL_PCS_F Block */

  SD_BLKSYS_CELL_A   = 6,    /**< Cellular A-System */
  SD_BLKSYS_CELL_B   = 7,    /**< Cellular B-System */

  SD_BLKSYS_PCS_G    = 8,    /**< PRL_PCS_G Block */
  SD_BLKSYS_PCS_H    = 9,    /**< PRL_PCS_H Block */
  SD_BLKSYS_PCS_I    = 10,   /**< PRL_PCS_I Block */
  SD_BLKSYS_PCS_J    = 11,   /**< PRL_PCS_J Block */
  SD_BLKSYS_PCS_K    = 12,   /**< PRL_PCS_K Block */
  SD_BLKSYS_PCS_L    = 13,   /**< PRL_PCS_L Block */

  SD_BLKSYS_PSB      = 14,   /**< Public Safety Broadband*/
  SD_BLKSYS_PSG      = 15,   /**< Public Safety Guardband*/

  SD_BLKSYS_MAX
} sd_blksys_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of system roam status.
**
** Note that the roaming indicator values (SYS_ROAM_STATUS_OFF and
** SYS_ROAM_STATUS_ON) that are defined in db.h are swapped compare to the
** PRL roaming indicator values that are defined in TSB-58.
*/
typedef enum {
  SD_ROAM_OFF        = SYS_ROAM_STATUS_OFF,       /**< Roaming indicator is OFF */
  SD_ROAM_ON         = SYS_ROAM_STATUS_ON,        /**< Roaming indicator is ON */
  SD_ROAM_FLASH      = SYS_ROAM_STATUS_BLINK,     /**< Roaming indicator is flashing */

  SD_ROAM_MAX        = 0xFF
} sd_roam_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of cellular systems (A/B.
*/
typedef enum {

  SD_CELL_SYS_A     = NV_SYSTEM_A,
                      /**< AMPS-A system */

  SD_CELL_SYS_B     = NV_SYSTEM_B,
                      /**< AMPS-B system */

  SD_CELL_SYS_MAX
} sd_cell_sys_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of sd network selection mode.
**
*/
typedef enum {

    SD_NET_SELECT_MODE_LIMITED,   /**< PRL
                                  **    plmn select mode: Any
                                  **
                                  ** CMREG
                                  **   Sys network selection type: limited
                                  */

    SD_NET_SELECT_MODE_AUTO,      /**< PRL
                                  **    plmn select mode: allowed
                                  **
                                  ** CMREG
                                  **    Sys network selection type: Auto
                                  */

    SD_NET_SELECT_MODE_HPLMN,     /**< PRL
                                  **    plmn select mode: home plmn
                                  **
                                  ** CMREG
                                  **    Sys network selection type: manual
                                  **    manual plmn select type: hplmn
                                  */

    SD_NET_SELECT_MODE_PRL_SYS,   /**< PRL
                                  **    with specified plmn id
                                  **
                                  ** CMREG
                                  **    Sys net selection type: manual plmn
                                  **                    network select mode
                                  **    manual plmn select type: user mode
                                  */

    SD_NET_SELECT_MODE_MANUAL_SYS,/**< MANUAL SYS
                                  **   with specified plmn id
                                  **
                                  ** CMREG
                                  **    Sys network selection type is manual.
                                  **    is reselection flag: true
                                  */

    SD_NET_SELECT_MODE_PRIV_SYS,  /**< Private system provided by user for
                                  ** automatic selection
                                  */

    SD_NET_SELECT_MODE_MANUAL_BSSID,
                                  /**< Manual sys, in which only BSS Id is
                                  ** provided.
                                  */

    SD_NET_SELECT_MODE_MAX

} sd_net_select_mode_e_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of system usable type.
**
*/
typedef enum {
  SD_SYS_USABLE_OK,                  /**< System is usable. */
  SD_SYS_USABLE_NOT_OK,              /**< System is not usable. */
  SD_SYS_USABLE_UNKNOWN,             /**< System usable status is unknown.*/
  SD_SYS_USABLE_MAX
} sd_sys_usable_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of service domain preferences. Currently, applicable for
** GSM/WCDMA acquisitions only. Do not change the value of existing enumes,
** as they are being reported in SD events.

*/
typedef enum {

  SD_SS_SRV_DOMAIN_PREF_NONE    = -1, /**< Do not select any systems. */

  SD_SS_SRV_DOMAIN_PREF_CS_ONLY = 0,  /**< Select systems which provide circuit
                                      ** switched service.
                                      */

  SD_SS_SRV_DOMAIN_PREF_PS_ONLY = 1,  /**< Select systems which provide packet
                                      ** switched service.
                                      */

  SD_SS_SRV_DOMAIN_PREF_CS_PS   = 2,  /**< Select systems which provide both
                                      ** circuit and packet switched service.
                                      */

  SD_SS_SRV_DOMAIN_PREF_MAX

} sd_ss_srv_domain_pref_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
/*--------------------------------------------------------------------------
                               BAND MODE MACROS
---------------------------------------------------------------------------*/

/* Macros for the band or mode operations.
*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Add the bands from band_pref1 to band_pref2 and return the result.
*/
#define SD_BAND_ADD( band_pref1, band_pref2 )  \
        ( (sd_ss_band_pref_e_type)( (uint64)band_pref1 | (uint64)band_pref2 ) )

#define SD_BAND_ADD2( band_pref1, band_pref2 )  \
        ( (sd_mru_band_pref_e_type)( (uint32)band_pref1 | (uint32)band_pref2 ) )

/** Return TRUE if band_pref1 contains band_pref2 otherwise return FALSE.
*/
#define SD_BAND_CONTAIN( band_pref1, band_pref2 ) \
        ( BOOLEAN( (uint64)band_pref1 & (uint64)band_pref2 ) )

/** Delete the bands of band_pref2 from band_pref1 and return the result.
*/
#define SD_BAND_DELETE( band_pref1, band_pref2 ) \
        ( (sd_ss_band_pref_e_type)( (uint64)band_pref1 & ~(uint64)band_pref2) )

/** Get the common set of bands from band_pref1 and band_pref2 and return the
** result.
*/
#define SD_GET_COMMON_BAND( band_pref1, band_pref2 ) \
        ( (sd_ss_band_pref_e_type)( (uint64)band_pref1 & (uint64)band_pref2 ) )

/** Add the modes from mode_pref1 to mode_pref2 and return the result.
*/
#define SD_MODE_ADD( mode_pref1, mode_pref2 )  \
        ( (sd_ss_mode_pref_e_type)( (word)mode_pref1 | (word)mode_pref2 ) )

/** Return TRUE if mode_pref1 contains mode_pref2 otherwise return FALSE.
*/
#define SD_MODE_CONTAIN( mode_pref1, mode_pref2 )  \
        ( BOOLEAN( (word)mode_pref1 & (word)mode_pref2 ) )

/** Get the common set of modes from mode_pref1 and mode_pref2 and return the
** result.
*/
#define SD_GET_COMMON_MODE( mode_pref1, mode_pref2) \
        ( (sd_ss_mode_pref_e_type)( (word)mode_pref1 & (word)mode_pref2 ) )

/** Delete the modes of mode_pref2 from mode_pref1 and return the result.
*/
#define SD_MODE_DELETE( mode_pref1, mode_pref2 ) \
        ( (sd_ss_mode_pref_e_type)( (word)mode_pref1 & ~(word)mode_pref2) )

/** Return the other mode preferences.
*/
#define SD_MODE_NOT( mode_pref1 ) \
        ( (sd_ss_mode_pref_e_type)( ~(word)mode_pref1) )


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**  SD GW band grouping definitions.
*/
#define SD_BAND_GROUP1               (SD_SS_BAND_PREF_GSM_DCS_1800     | \
                                      SD_SS_BAND_PREF_GSM_PGSM_900     | \
                                      SD_SS_BAND_PREF_GSM_EGSM_900     | \
                                      SD_SS_BAND_PREF_GSM_RGSM_900     | \
                                      SD_SS_BAND_PREF_WCDMA_I_IMT_2000 | \
                                      SD_SS_BAND_PREF_WCDMA_III_1700   | \
                                      SD_SS_BAND_PREF_WCDMA_VI_800     | \
                                      SD_SS_BAND_PREF_WCDMA_VIII_900   | \
                                      SD_SS_BAND_PREF_WCDMA_IX_1700)

#define SD_BAND_GROUP2               (SD_SS_BAND_PREF_GSM_850           | \
                                      SD_SS_BAND_PREF_GSM_PCS_1900      | \
                                      SD_SS_BAND_PREF_WCDMA_II_PCS_1900 | \
                                      SD_SS_BAND_PREF_WCDMA_IV_1700     | \
                                      SD_SS_BAND_PREF_WCDMA_V_850)

#define SD_BAND_GROUP3   SD_BAND_DELETE( SD_BAND_DELETE(                \
                                         SD_SS_BAND_PREF_ANY,           \
                                         SD_BAND_GROUP1),               \
                                         SD_BAND_GROUP2                 \
                                       )
                                       
#define SD_GW_BAND_GROUP             (SD_SS_BAND_PREF_GSM_DCS_1800      | \
                                      SD_SS_BAND_PREF_GSM_PGSM_900      | \
                                      SD_SS_BAND_PREF_GSM_EGSM_900      | \
                                      SD_SS_BAND_PREF_GSM_RGSM_900      | \
                                      SD_SS_BAND_PREF_WCDMA_I_IMT_2000  | \
                                      SD_SS_BAND_PREF_WCDMA_III_1700    | \
                                      SD_SS_BAND_PREF_WCDMA_VI_800      | \
                                      SD_SS_BAND_PREF_WCDMA_VIII_900    | \
                                      SD_SS_BAND_PREF_WCDMA_IX_1700     | \
                                      SD_SS_BAND_PREF_GSM_850           | \
                                      SD_SS_BAND_PREF_GSM_PCS_1900      | \
                                      SD_SS_BAND_PREF_WCDMA_II_PCS_1900 | \
                                      SD_SS_BAND_PREF_WCDMA_IV_1700     | \
                                      SD_SS_BAND_PREF_WCDMA_V_850)
                                       
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* No information on LTM is present
** 3.7.2.3.2.26 Sync Channel Message
** Could have also be named as NO_INFO.
*/
#define SD_LTM_OFFSET_INVALID   0xFF

/* Daylight savings information
** 3.7.2.3.2.26 Sync Channel Message
*/
typedef enum {

    SD_DAYLT_SAVINGS_NONE    = -1,
      /* INTERNAL USE */

    SD_DAYLT_SAVINGS_OFF = 0,
      /* daylight savings not in effect */

    SD_DAYLT_SAVINGS_ON = 1,
      /* daylight savings in effect */

    SD_DAYLT_SAVINGS_MAX
      /* INTERNAL USE */

} sd_daylt_savings_e_type;

#define SD_DAYLT_SAVINGS_NO_INFO SD_DAYLT_SAVINGS_OFF
#define   SD_DAYLT_SAVINGS_INVALID SD_DAYLT_SAVINGS_OFF

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for a SD system.
**
** Note! The order of fields in this structure is important in order to
** optimize the structure size (i.e. eliminate compiler padding) as this
** structure is used in the System Record Table (sdsr_tbl). Add new fields
** only at the end and try to order fields smartly to minimize compiler
** padding.
*/
typedef struct {
  sd_ss_band_pref_e_type band;     /**< Band class preference. */
  sd_chan_type           chan;     /**< CDMA Channel/AMPS A/B system,
                                   ** For WLAN B/G, it will be mask
                                   ** Bit 0 => channel 1, bit 1 => channel 2
                                   ** etc
                                   */
  sd_mode_e_type         mode;     /**< Mode - AMPS or CDMA */
} sd_sys_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for a SD PLMN.
**
** Note! The order of fields in this structure is important in order to
** optimize the structure size (i.e. eliminate compiler padding) as this
** structure is used in the System Record Table (sdsr_tbl). Add new fields
** only at the end and try to order fields smartly to minimize compiler
** padding.
*/
typedef struct {

  /**< PLMN ID only valid when plmn_lac_incl != PRL_PLMN_LAC_NOT_INCL.
  */
  sys_plmn_id_s_type        plmn_id;


  /**< local area code only valid when plmn_lac_incl == PRL_PLMN_LAC_INCL
  */
  sys_lac_type              lac;

} sd_plmn_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for system ID.
*/
typedef union {

  struct {
    word sid;       /**< IS-95 SID */
    word nid;       /**< IS-95 NID */

    word true_sid;  /**< The "true" SID when sid/nid above store the
                    ** MCC/IMSII_11_12 */

    word true_nid;  /**< The "true" NID when sid/nid above store the
                    ** MCC/IMSII_11_12 */
    word mcc;       /**< MCC - Mobile County Code */
    byte imsi_11_12;/**< IMSI_11_12 */

  } is95;           /**< IS-95 system ID - Identifies CDMA, and AMPS systems */

  #ifdef FEATURE_HDR
#error code not present
  #else
  byte  is856[1];  /**< IS-856 system ID - Identifies HDR systems */
  #endif

                    /**< PLMN that identifies a GSM/WCDMA systems. */
  sd_plmn_s_type  plmn;

  /**< A temporary solution to indicate both acquired system mode and plmn for
  ** GW systems.The plmn field in gw union exactly matches the plmn in plmn
  ** union.
  */
  struct {
    sd_plmn_s_type plmn;
    sd_mode_e_type mode;
  } gw;

  /** WLAN SSId.
  */
  sys_wlan_ssid_s_type   ssid;

} sd_sid_u_type;


/** Type for system ID except WLAN SSID.
*/
typedef union {

  struct {
    word sid;       /**< IS-95 SID */
    word nid;       /**< IS-95 NID */

    word true_sid;  /**< The "true" SID when sid/nid above store the
                    ** MCC/IMSII_11_12 */

    word true_nid;  /**< The "true" NID when sid/nid above store the
                    ** MCC/IMSII_11_12 */
    word mcc;       /**< MCC - Mobile County Code */
    byte imsi_11_12;/**< IMSI_11_12 */

  } is95;           /**< IS-95 system ID - Identifies CDMA, and AMPS systems */

  #ifdef FEATURE_HDR
#error code not present
  #else
  byte  is856[1];  /**< IS-856 system ID - Identifies HDR systems */
  #endif

                    /**< PLMN that identifies a GSM/WCDMA systems. */
  sd_plmn_s_type  plmn;

  /**< A temporary solution to indicate both acquired system mode and plmn for
  ** GW systems.The plmn field in gw union exactly matches the plmn in plmn
  ** union.
  */
  struct {
    sd_plmn_s_type plmn;
    sd_mode_e_type mode;
  } gw;


} sd_sid2_u_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for system ID, only SID and PLMN id.
*/
typedef struct {

  union
  {

    struct
    {
      word sid;       /**< IS-95 SID */
      word nid;       /**< IS-95 NID */
    } is95;           /**< IS-95 system ID - Identifies CDMA, and AMPS systems */

    /**< PLMN Id that identifies a GSM/WCDMA systems.
    */
    sd_plmn_s_type        plmn;

    struct
    {
      #ifdef FEATURE_HDR
#error code not present
      #else
      byte sid[1];   /**< The Sector ID */
      #endif
      byte subnet_mask_len;
                      /**< The subnet mask length of the Sector ID. */
    } is856; /**< IS-856 system ID - Identifies HDR systems */

    struct
    {
      sys_wlan_bssid_type   bssid;
      sys_wlan_ssid_s_type  ssid;
    } wlan;

  } prm;

  /**< Network selection mode type.
  */
  sd_net_select_mode_e_type net_select_mode;

} sd_sid_plmn_wlan_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of NAM selections.
*/
typedef enum {

  SD_NAM_1,           /**< NAM 1 */
  SD_NAM_2,           /**< NAM 2 - for targets with more than 1 NAM */

  SD_NAM_MAX
} sd_nam_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of system selections.
*/
typedef enum {


  SD_SS_MAIN,       /**< Main system selection */
  SD_SS_HYBR_1,     /**< HDR system selection (applicable only when doing
                    ** CDMA/HDR or CDMA/WCDMA hybrid operation) */
  SD_SS_HYBR_WLAN,  /**< WLAN system selection */
  SD_SS_MAX
} sd_ss_e_type;

#define SD_SS_HYBR_HDR     SD_SS_HYBR_1

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** MRU CDMA system data structure */
typedef struct{

  byte                                             mode;
  byte                                             band;
  word                                             channel;
  word                                             sid;
  word                                             nid;
  byte                                             spare[2];

} sd_cdma_sys_type;

/** MRU AMPS system data structure */
typedef struct{

  byte                                             mode;
  byte                                             band;
  word                                             sys;
  word                                             sid;
  byte                                             spare[4];

} sd_cellular_sys_type;

/** MRU HDR system data structure */
typedef struct{

  byte                                             mode;
  byte                                             band;
  word                                             channel;
  byte                                             subnet[6];

} sd_hdr_sys_type;

/** MRU GW system data structure */
/** Since the size of each MRU entry is 10, do not use dword.O/W
*  it will change to 12 after compile.
*/
typedef struct{
  word                                             bit_31_16;
  word                                             bit_15_0;
} sd_band_pref_s_type;

typedef struct{

  byte                                             mode;
  sys_plmn_id_s_type                               plmn_id;      /**< 3-byte */
  sd_band_pref_s_type                              band_pref;    /**< 4-byte */
  byte                                             net_select_mode;
  byte                                             spare;
} sd_gsm_sys_type;

typedef struct{

  byte                                             mode;
  sys_plmn_id_s_type                               plmn_id;      /**< 3-byte */
  sd_band_pref_s_type                              band_pref;    /**< 4-byte */
  byte                                             net_select_mode;
  byte                                             acq_mode;
} sd_gw_sys_type;

/** MRU wcdma and channel system data structure */
typedef struct{
  word                                             chan;
  byte                                             band;
} sd_band_chan_s_type;

typedef union{
  sd_band_pref_s_type                              band_pref;
  sd_band_chan_s_type                              band_chan;
} sd_band_chan_u_type;

typedef struct{

  byte                                             mode;
  sys_plmn_id_s_type                               plmn_id;      /**< 3-byte */
  sd_band_chan_u_type                              bc;           /**< 4-byte */
  byte                                             net_select_mode;
  byte                                             is_chan;

} sd_wcdma_sys_type;

/** An MRU table entry : each entry 10 bytes, which ares reserved in NV. */
typedef union {

  byte                                             mode;
  sd_cdma_sys_type                                 cdma;
  sd_cellular_sys_type                             cellular;
  sd_hdr_sys_type                                  hdr;
  sd_gsm_sys_type                                  gsm;
  sd_wcdma_sys_type                                wcdma;
  sd_gw_sys_type                                   gw;

} sd_mru_table_entry_u_type;

/** @}
**
*/

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================= SERVICE INDICATORS ============================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup SI Service Indicators
*  @{
*/

#ifdef FEATURE_USERZONE
/** User zone info format required in SD. It is scaled down compared
** to the on in PUZL.
*/
typedef struct {

  /** User zone Id of the selected user zone. Set to SD_INVALID_UZID
  ** if no user zone is selected by MS currently
  */
  int4                      uz_id;

  /** User zone name of the selected user zone, valid only if
  ** uz_id != SD_INVALID_UZID
  */
  puzl_uz_name_s_type       uz_name;

  /** User zone active registration type, valid only if
  ** uz_id != SD_INVALID_UZID
  */
  puzl_uz_act_flag_e_type   uz_act_flag;

} sd_si_uz_info_s_type;
#endif /* FEATURE_USERZONE */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** CDMA Mode specific information in si_info.
*/
typedef struct {

  /** Neighbour set window search size.
  */
  sys_srch_win_type                    srch_win_n;

  /** Base station latitude[0.25sec and already sign extended].
  */
  sys_base_lat_type                    base_lat;

  /** Base station latitude[0.25sec and already sign extended].
  */
  sys_base_long_type                   base_long;

  /** Base station Identification number.
  */
  sys_base_id_type                     base_id;

} sd_si_cdma_mode_info_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** WLAN Mode specific information in si_info.
*/
typedef struct {

  /** Access Point's BSS id.
  */
  sys_wlan_bssid_type                  bssid;

  /** BSS Type - Infrastructure or independent.
  */
  sys_wlan_bss_e_type                  bss_type;

  /** WLAN technology in use.
  */
  sys_tech_e_type                      tech;

} sd_si_wlan_mode_info_s_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Mode specific information in si_info.
*/
typedef struct {

  /** CDMA mode specific info - applicable if mode = SD_MODE_CDMA
  *  in sd_si_info_s_type.
  */
  sd_si_cdma_mode_info_s_type          cdma;

  /** WLAN mode specific info - applicable if mode = SD_MODE_WLAN in
  ** sd_si_info_s_type.
  */
  sd_si_wlan_mode_info_s_type          wlan;

} sd_si_mode_info_u_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for Service Indication (SI). Provides information about the service
** that is associated with the acquired/candidate system.
*/
typedef struct {

  /** Indicates the Service status ( Limited, Full service etc.,).
  **
  ** NOTE! When the value of this field is NO_SRV the value of all the fields
  ** that follows this field is undefined.
  */
  sys_srv_status_e_type                srv_status;

  /** Indicate whether system determination is still attempting to acquire
  ** a more preferred service.
  */
  boolean                              is_pref_srv_acq;

  /** Indicate whether redirection or handoff indication is currently ON.
  */
  boolean                              is_redir_or_ho;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** The mode of the acquired/candidate system.
  */
  sd_mode_e_type                       mode;

  /** Mode specific information - based on mode.
  */
  sd_si_mode_info_u_type               mode_info;

  /** The band class of the acquired/candidate system.
  */
  sd_band_e_type                       band;

  /** The CDMA Channel/AMPS A/B system of the acquired/candidate system.
  */
  sd_chan_type                         chan;

  /** The System ID of the acquired/candidate system.
  */
  sd_sid_u_type                        sid;

  /** The roaming indication of the acquired/candidate system.
  */
  sd_roam_e_type                       roam;

  /** Serving system's service capability.
  */
  sys_srv_capability_e_type            sys_srv_cap;

  /** PRL designation
  */
  byte                                 prl_desig;

  /** Is system prl match?
  */
  boolean                              is_sys_prl_match;

  /** Is system forbidden?
  */
  boolean                              is_sys_forbidden;

  /** Registered service domain.
  */
  sys_srv_domain_e_type                srv_domain;

  /** PRL association fields - Indicate whether this system is tag, PN and/or
  ** data associated.
  */
  boolean                              is_colloc;
  boolean                              is_pn_assn;
  boolean                              is_data_assn;

  #ifdef FEATURE_USERZONE
  /** User zone information associated with the acquired system.
  */
  sd_si_uz_info_s_type                 curr_uz_info;
  #endif /* FEATURE_USERZONE */

  /** This flag indicates whether the HDR instance needs to be re-aligned,
  ** Would typically occur whenever the SID/NID, band or is_colloc field
  ** changes in SD's main instance.
  */
  boolean                              realign_hdr;

  /** System usable status indicator.
  */
  sd_sys_usable_e_type                 usable_status;

  /** Broadcast/multicast data service availability
  ** This fields specify whether lower layer
  ** supports broadcast/multicast as per
  ** High Rate Broadcast-Multicast Packet data Air
  ** Interface Specification C.S0054 and
  ** BCMCS Framework Draft Document 0.1.3 XP0019
  */
  sys_bcmcs_srv_status_e_type          bcmcs_srv_status;

  /** The uptime the current service was acquired
  */
  dword                                srv_acq_time;

  /** Srv lost time
  */
  dword                                srv_lost_time;

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  /** The active HDR protocol revision
  */
  sys_active_prot_e_type               hdr_active_prot;

  /* Indicates the CDMA/HDR time info
  */
  sys_time_info_s_type                 time_info;

  /* Indicates the CS domain service status ( Limited, Full service etc.)
  */
  sys_srv_status_e_type                cs_srv_status;

  /* Default roaming indicator from PRL
  */
  sys_roam_status_e_type               def_roam_ind;
  
} sd_si_info_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for Service Indicators callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified of service
** indicators changes, needs to register a @FUNCTION of this type with the
** SD Service Indicators component.
*/
typedef void (sd_si_info_f_type) (

    sd_ss_e_type              ss,
        /**< System selection - MAIN or HDR.
        */

    const sd_si_info_s_type   *si_info_ptr
        /**< A const pointer to a structure containing the current values of
        ** the service indicators
        */
);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for Acquisition Failure callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** acquisition fails (i.e., SD have attempted full acquisitions on all
** systems with no success), needs to register a @FUNCTION of this type with
** the SD Service Indicators component.
*/
typedef void (sd_si_acq_fail_f_type) (

    sd_ss_e_type              ss
        /**< System selection - MAIN or HDR.
        */
);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for Service Indication (SI) OK to orig. Provides information about
** the service that is associated with a call origination.
*/
typedef struct {

  sd_mode_e_type                   mode;
     /**< Mode to originate the call.
     */

  sys_srv_domain_e_type            srv_domain;
    /**< Service domain indication */

  sys_srv_status_e_type            srv_status;
    /**< Service status indication */

  boolean                          ps_data_suspend;
    /**< ps data suspend flag indication */

} sd_si_ok_to_orig_s_type;

/** Type for Ok To Originate callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** the Ok To Originate status changes from FALSE to TRUE, needs to register a
** function of this type with the SD Service Indicators component.
*/
typedef void (sd_si_ok_to_orig_f_type) (

    sd_ss_e_type                     ss,
        /**< System selection - MAIN or HDR.
        */

    const sd_si_ok_to_orig_s_type    *sd_si_ok_to_orig_ptr
        /**< A constant pointer to a structure to contain the current values of
        ** the service status related to the call origination.
        */
);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for Emergency Callback Mode callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** the Emergency Callback Mode status changes from FALSE to TRUE, needs to
** register a @FUNCTION of this type with the SD Service Indicators component.
*/
typedef void (sd_si_emerg_cb_mode_f_type) ( void );


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for Auto-NAM change callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** auto-NAM change is appropriate, needs to register a @FUNCTION of this type
** with the SD Service Indicators component.
*/
typedef void (sd_si_autonam_change_f_type) (

    sd_nam_e_type             nam
        /**< The NAM that is proposed by the auto-NAM @FUNCTIONality.
        */
);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for GSM/WCDMA Networks list callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** results of Get Networks list request is received by SD, needs to register
** a @FUNCTION of this type with the SD Service Indicators component.
*/
typedef void (sd_si_gw_net_list_f_type) (


    sys_plmn_id_s_type       *rplmn_ptr,
       /**< Pointer to a structure containing the last registered PLMN id.
       */

    sys_detailed_plmn_list_s_type *plmn_list_ptr,
       /**< Pointer to a structure containing the list of available PLMNs.
       */

    sys_plmn_list_status_e_type    plmn_list_status
       /**< Gives the status of plmn list request (success/abort).
       */
);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for SIM state change callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** there is a change in SIM state reported from REG.
*/
typedef void (sd_si_sim_state_f_type) (


    sys_sim_state_e_type       sim_state
       /**< New sim state reported from REG.
       */
);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for registration completed call back @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** there is registration (including implicit) completed successfully.
*/
typedef void (sd_si_reg_done_f_type) (

    sd_ss_e_type              ss,
       /**< System selection - MAIN or HDR.
       */

    sd_mode_e_type             mode
       /**< Mode in which registration was done.
       */
);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for data suspended call back @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** there is a change in data suspend flag reported from REG.
*/
typedef void (sd_si_data_suspend_f_type) (

    boolean                   data_suspend
       /**< New data suspend flag reported from REG.
       */

);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for WLAN Networks list callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** results of Get Networks list request is received by SD, needs to register
** a @FUNCTION of this type with the SD Service Indicators component.
*/
typedef void (sd_si_wlan_net_list_f_type) (


    sys_wlan_bss_info_list_s_type           *wlan_bss_list_ptr
       /**< Pointer to a structure containing the list of available BSS.
       */
);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for WLAN Networks list callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** results of Get Networks list request is received by SD, needs to register
** a @FUNCTION of this type with the SD Service Indicators component.
*/
typedef void (sd_si_wlan_wps_result_f_type) (

   const  sys_wlan_wps_result_s_type             *wlan_wps_result_ptr
       /**< Pointer to a structure containing the result of a WPS procedure.
       */

);

/** Type for Service lost callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** service lost indication changes, needs to register
*  a @FUNCTION of this type with the SD Service lost component.
*/
typedef void (sd_si_srv_lost_f_type) (


        sd_ss_e_type              ss
       /**< System selection - MAIN or HDR.
       */       
);


/** @}
**
*/

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================= @FUNCTION DECLARATION ==========================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup FD @FUNCTION Declaration
*  @{
*/

/* <EJECT> */
/*===========================================================================

@FUNCTION sd_si_reg

@DESCRIPTION
  Register callback functions with the Service Indicators component.

  SD calls on the Service Indicators callback function whenever one or more
  of the service indicators changes their value.

  SD calls on the ok_to_orig callback function whenever the ok_to_orig status
  changes from FALSE to TRUE while the origination mode is other than none.

  SD calls on the emerg_cb_mode callback function whenever the emerg_cb_mode
  status changes from FALSE to TRUE.

  NOTE! A new call to this function overrides all the previously registered
  callback functions.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sd_si_reg(

        sd_si_info_f_type             *si_info_cb_func,
            /**< Pointer to a Service Indicators callback function. SD calls on
            ** this function whenever one or more of the service indicators
            ** changes their value.
            **
            ** The argument to this function is a const pointer to a
            ** structure containing the current value of the service
            ** indicators */

        sd_si_acq_fail_f_type         *acq_fail_cb_func,
            /**< Pointer to an Acquisition Fail callback function. SD calls on
            ** this function whenever acquisition fails (i.e., SD have
            ** attempted full acquisitions on all systems with no success).
            */

        sd_si_ok_to_orig_f_type       *ok_to_orig_cb_func,
            /**< Pointer to an Ok To Orig callback function. SD calls on this
            ** function whenever the Ok To Orig status changes from FALSE to
            ** TRUE while the origination mode is other than none */

        sd_si_emerg_cb_mode_f_type    *emerg_cb_mode_cb_func,
            /**< Pointer to an Emergency Callback Mode callback function. SD
            ** calls on this function whenever the Emergency Callback Mode
            ** status changes from FALSE to TRUE */

        sd_si_autonam_change_f_type   *autonam_change_cb_func,
            /**< Pointer to an auto-NAM callback function. SD calls on this
            ** function whenever auto-NAM change is appropriate */

        sd_si_gw_net_list_f_type      *gw_net_list_cb_func,
            /**< Pointer to the GW network list call function. SD calls on
            ** this function once it receives the results of the get network
            ** list on GSM/WCDMA systems.
            */

        sd_si_sim_state_f_type        *sim_state_cb_func,
           /**< Pointer to the sim state call back function. Sd calls on this
           ** function if the SIM state was changed.
           */

        sd_si_reg_done_f_type         *reg_done_cb_func,
           /**< Pointer to the reg. done call back function. SD calls this when
           ** a reg. is done successfully.
           */

        sd_si_data_suspend_f_type     *data_suspend_cb_func,
           /**< Pointer to the data suspend call back function. SD calls this when
           ** a REG reports change in data suspend flag.
           */

        sd_si_wlan_net_list_f_type    *wlan_net_list_cb_func,
           /**< Pointer to the WLAN network list call function. SD calls on
           ** this function once it receives the results of the get network
           ** list on WLAN systems.
           */

        sd_si_wlan_wps_result_f_type  *wlan_wps_result_cb_func,
           /**< Pointer to the WLAN network list call function. SD calls on
           ** this function once it receives the results of the get network
           ** list on WLAN systems.
           */

        sd_si_srv_lost_f_type         *srv_lost_cb_func
           /**< Pointer to the service lost indication call back. SD calls on
           ** this function on a service lost change
           */

);




/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_si_info_ptr_get

@DESCRIPTION
  This function is used to fetch the current SI info.

@DEPENDENCIES
  None.

@RETURN VALUE
  sd_si_info_s_type* - pointer to current SI information.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  const sd_si_info_s_type*            sd_si_info_ptr_get(

        void
);


/*===========================================================================

@FUNCTION sd_si_hybr_hdr_info_ptr_get

@DESCRIPTION
  This function is used to fetch the current SI info.

@DEPENDENCIES
  None.

@RETURN VALUE
  sd_si_info_s_type* - pointer to current SI information.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  const sd_si_info_s_type*            sd_si_hybr_hdr_info_ptr_get(

        void
);


/**===========================================================================

@FUNCTION sd_si_hybr_wlan_info_ptr_get

@DESCRIPTION
  This function is used to fetch the current SI info.

@DEPENDENCIES
  None.

@RETURN VALUE
  sd_si_info_s_type* - pointer to current SI information.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  const sd_si_info_s_type*            sd_si_hybr_wlan_info_ptr_get(

        void
);


#ifdef FEATURE_USERZONE
/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_si_uz_info_ptr_get

@DESCRIPTION
  This function is used to fetch the current UZ selected info.

@DEPENDENCIES
  None.

@RETURN VALUE
  sd_si_uz_info_s_type* - pointer to current SI  User zone selection information.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  const sd_si_uz_info_s_type*            sd_si_uz_info_ptr_get(

        void
);
#endif /* FEATURE_USERZONE */

#ifdef FEATURE_REG_ACC_ROAM_IND
/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_si_roam_update

@DESCRIPTION
  Sets the roam indicator to the value passed in TSB58 roam ind format

  If one or more of the service indicators has changed, inform the registered
  client (e.g., CM) of the new service indicators.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void                      sd_si_roam_update(

        sd_roam_e_type            roam_ind
        /**<
        ** roam indication to be updated
        */
);
#endif /* FEATURE_REG_ACC_ROAM_IND */

/** @}
**
*/

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================= SYSTEM SELECTION ==============================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup SS System Selection
*  @{
*/

/*--------------------------------------------------------------------------
                     System Selection Action Parameters
---------------------------------------------------------------------------*/


/** The System Selection Action (SS-Action) instructs the SS-Client of the
** action to take upon an SS-Indication @FUNCTION call return.
*/


/** Enumeration of SS-Actions to be followed by the SS-Client upon
** an SS-Indication @FUNCTION call return.
*/
typedef enum {

  SD_SS_ACT_CONTINUE    = BM(0),
                        /**< Continue with current mode of operation and
                        ** follow the actions that are specified by the
                        ** "continue" parameters */

  SD_SS_ACT_ACQ_CDMA    = BM(1),
                        /**< Attempt acquiring a CDMA system as specified by
                        ** the "CDMA acquisition" parameters */

  SD_SS_ACT_ACQ_AMPS    = BM(2),
                        /**< Attempt acquiring an AMPS system as specified by
                        ** the "AMPS acquisition" parameters */

  SD_SS_ACT_ACQ_HDR     = BM(3),
                        /**< Attempt acquiring an HDR system as specified by
                        ** the "HDR acquisition" parameters */


  SD_SS_ACT_MEAS_DED    = BM(4),
                        /**< Change to initialization state and request a
                        ** dedicated mode channel strength measurement from
                        ** SRCH. When SRCH completes the measurement report,
                        ** call on the SD_SS_IND_MISC_MEAS_RPRT SS-Indication
                        ** FUNCTION */

  SD_SS_ACT_MEAS_BACK   = BM(5),
                        /**< Stay in current state and request a background
                        ** mode channel strength measurement from SRCH. When
                        ** SRCH completes the measurement report, call on the
                        ** SD_SS_IND_MISC_MEAS_RPRT SS-Indication function */

  SD_SS_ACT_PWR_SAVE    = BM(6),
                        /**< Enter Power-Save mode of operation. Once it has
                        ** actually entered Power-Save mode the SS-Client
                        ** should call on the SD_SS_IND_MISC_PWR_SAVE_ENTER
                        ** SS-Indication function */

  SD_SS_ACT_ACCEPT      = BM(7),
                        /**< Accept BS proposed mode, band and/or CDMA channel/
                        ** AMPS system. This SS-Action is only valid for
                        ** Redirection, Channel Assignment or Handoff
                        ** Direction */

  SD_SS_ACT_REJECT      = BM(8),
                        /**< Reject BS proposed mode, band and/or CDMA channel/
                        ** AMPS system. This SS-Action is only valid for
                        ** Redirection, Channel Assignment or Handoff
                        ** Direction */

  SD_SS_ACT_RELEASE     = BM(9),
                        /**< Release call. This SS-Action is only valid for
                        ** ITSPM SS-Indication */

  SD_SS_ACT_ACQ_GPS     = BM(10),
                        /**< Attempt acquiring GPS mode. */

  SD_SS_ACT_ACQ_GW      = BM(11),
                        /**< Attempt acquiring a GSM/WCDMA system as specified by
                        ** the "GW acquisition" parameters */

  SD_SS_ACT_GET_NET_GW  = BM(12),
                        /**< Get the available GSM and/or WCDMA systems. */

  SD_SS_ACT_PWR_SCAN_GW = BM(13),
                        /**< Do pwr scan on GSM and/or WCDMA systems. Once
                        ** pwr_scan request has been issued, SS-Client
                        ** should call on SD_SS_IND_MISC_CNF SS-Indication
                        ** function. */

  SD_SS_ACT_ACQ_WLAN    = BM(14),
                        /**< Attempt acquiring a WLAN system as specified by
                        ** the "WLAN acquisition" parameters */

  SD_SS_ACT_SURVEY_WLAN = BM(15),
                        /**< Get the available WLAN systems as specified by
                        ** the "WLAN survey" parameters */

  SD_SS_ACT_MAX
} sd_ss_act_e_type;


/** Types for parameters that are associated with the SD_SS_ACT_MEAS
** returned SS-Action value.
*/
typedef enum {

  SD_SS_MEAS_TYPE_RXPWR,            /**< Rx-power measurement */
  SD_SS_MEAS_TYPE_PILOT,            /**< Pilot strength measurement */

  SD_SS_MEAS_TYPE_NETLIST,          /**< Measurement based on network list.
                                    ** Used by GW to work on top of PLMN list
                                    */

  SD_SS_MEAS_TYPE_MAX
} sd_ss_meas_type_e_type;



/* <EJECT> */
/*--------------------------------------------------------------------------
                    System Selection Preference Parameters
---------------------------------------------------------------------------*/


/* The System Selection Preference (SS-Preference) provides the SS-Client
** with means to limit the possible set of systems over which service is
** provided in order to ensure a particular characteristic of service, such
** as service over a particular band-class, mode or roaming indication.
**
** Note, however, that the SS-Preference is only consulted when the
** origination mode is SS_ORIG_MODE_OFF or SS_ORIG_MODE_OTHER. In other
** words, OTASP or Emergency call origination mode disregards the
** SS-Preference.
*/

/** Enumeration of system selection mode preferences.
*/
typedef enum {

  SD_SS_MODE_PREF_NONE        = 0,
                                /**< Acquire no system */

  SD_SS_MODE_PREF_AMPS        = BM( SD_MODE_AMPS ),
                                /**< Acquire AMPS mode systems only */

  SD_SS_MODE_PREF_CDMA        = BM( SD_MODE_CDMA ),
                                /**< Acquire CDMA mode systems only */

  SD_SS_MODE_PREF_CDMA_AMPS   = BM( SD_MODE_CDMA ) | BM( SD_MODE_AMPS ),
                                /**< Acquire CDMA or AMPS mode systems only */

  SD_SS_MODE_PREF_HDR         = BM( SD_MODE_HDR ),
                                /**< Acquire HDR mode systems only */

  SD_SS_MODE_PREF_CDMA_AMPS_HDR
                              = BM( SD_MODE_CDMA ) | BM( SD_MODE_AMPS ) | \
                                BM( SD_MODE_HDR),
                                /**< Acquire CDMA, AMPS or HDR mode
                                ** systems only */

  SD_SS_MODE_PREF_GPS         = BM(SD_MODE_GPS),
                                /**< Acquire GPS mode.
                                */

  SD_SS_MODE_PREF_GSM         = BM( SD_MODE_GSM ),
                                /**< Acquire GSM systems only */

  SD_SS_MODE_PREF_WCDMA       = BM( SD_MODE_WCDMA ),
                                /**< Acquire WCDMA systems only */

  SD_SS_MODE_PREF_GW          = BM( SD_MODE_GSM ) | BM( SD_MODE_WCDMA) | \
                                BM(SD_MODE_GW),
                                /**< Acquire GSM or WCDMA systems only */

  SD_SS_MODE_PREF_DIGITAL     = BM( SD_MODE_CDMA ) | BM( SD_MODE_HDR ) | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) |
                                BM( SD_MODE_WLAN )| BM(SD_MODE_GW),
                                /**< Acquire digital (CDMA or HDR or GSM or
                                ** WCDMA or WLAN ) mode systems only
                                */

  SD_SS_MODE_PREF_CDMA_HDR    = BM( SD_MODE_CDMA ) | BM( SD_MODE_HDR ),
                                /**< Acquire CDMA and HDR systems only.
                                */

  SD_SS_MODE_PREF_DIGITAL_LESS_HDR
                              = BM( SD_MODE_CDMA ) | BM( SD_MODE_GSM ) | \
                                BM( SD_MODE_WCDMA) | BM( SD_MODE_WLAN)| BM(SD_MODE_GW),

  SD_SS_MODE_PREF_ANY_BUT_HDR = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) |
                                BM( SD_MODE_WLAN )| BM(SD_MODE_GW),

  SD_SS_MODE_PREF_ANY_BUT_HDR_WLAN
                              = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA )| BM(SD_MODE_GW),

  SD_SS_MODE_PREF_WLAN        = BM( SD_MODE_WLAN ),
                                /**< Acquire WLAN systems only */


  SD_SS_MODE_PREF_CDMA_WLAN   = BM( SD_MODE_CDMA ) | BM( SD_MODE_WLAN ),
                                /**< Acquire CDMA or WLAN systems only */

  SD_SS_MODE_PREF_HDR_WLAN    = BM( SD_MODE_HDR ) | BM( SD_MODE_WLAN ),
                                /**< Acquire HDR or WLAN systems only */

  SD_SS_MODE_PREF_HDR_WCDMA   = BM( SD_MODE_HDR ) | BM( SD_MODE_WCDMA ),
                                /**< Acquire HDR or WCDMA systems only */

  SD_SS_MODE_PREF_HDR_GW      = BM( SD_MODE_HDR ) | BM( SD_MODE_GSM ) | \
                                BM( SD_MODE_WCDMA)| BM(SD_MODE_GW),
                                /**< Acquire HDR or GW systems only */

  SD_SS_MODE_PREF_CDMA_HDR_WLAN
                              = BM( SD_MODE_CDMA ) | BM( SD_MODE_HDR )  | \
                                BM( SD_MODE_WLAN ),
                                /**< Acquire CDMA, HDR or WLAN systems only */

  SD_SS_MODE_PREF_CDMA_AMPS_WLAN
                              = BM( SD_MODE_AMPS)  | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_WLAN ),
                                /**< Acquire AMPS, CDMA or WLAN systems only */

  SD_SS_MODE_PREF_CDMA_AMPS_HDR_WLAN
                              = BM( SD_MODE_AMPS)  | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_HDR )  | BM( SD_MODE_WLAN ),
                                /**< Acquire AMPS, CDMA, HDR or WLAN systems only */

  SD_SS_MODE_PREF_GSM_WLAN    = BM( SD_MODE_GSM ) | BM( SD_MODE_WLAN),
                              /**< Acquire GSM or WLAN systems only */

  SD_SS_MODE_PREF_WCDMA_WLAN  = BM( SD_MODE_WCDMA ) | BM( SD_MODE_WLAN),
                              /**< Acquire WCDMA or WLAN systems only */

  SD_SS_MODE_PREF_GW_WLAN     = BM( SD_MODE_GSM ) | BM( SD_MODE_WCDMA)  |
                                BM( SD_MODE_WLAN)| BM(SD_MODE_GW),
                              /**< Acquire GSM, WCDMA or WLAN systems only */

  SD_SS_MODE_PREF_ANY         = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA ) | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                BM( SD_MODE_HDR )  | BM( SD_MODE_WLAN )| BM(SD_MODE_GW),
                                /**< Acquire any mode systems */

  SD_SS_MODE_PREF_ALL_MODES   = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                BM( SD_MODE_HDR )  | BM( SD_MODE_GPS )   | \
                                BM( SD_MODE_WLAN)| BM(SD_MODE_GW),
                                /**< All possible modes */


  SD_SS_MODE_PREF_MAX

} sd_ss_mode_pref_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Cellular and PCS band preference to band-class preference mapping.
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
  #define SD_SS_BAND_PREF_CELL_A  SD_SS_BAND_PREF_BC0_A  /**< US Cell A */
  #define SD_SS_BAND_PREF_CELL_B  SD_SS_BAND_PREF_BC0_B  /**< US Cell B */
  #define SD_SS_BAND_PREF_CELL    SD_SS_BAND_PREF_BC0    /**< US Cell A/B */
  #define SD_SS_BAND_PREF_PCS   SD_SS_BAND_PREF_BC1  /**< US PCS */


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of system selection roam preferences.
*/
typedef enum {

  SD_SS_ROAM_PREF_HOME    = BM( SD_ROAM_OFF ),
                            /**< Acquire only systems for which the roaming
                            ** indication is off */

  SD_SS_ROAM_PREF_AFFIL   = BM( SD_ROAM_OFF ) | BM( SD_ROAM_ON ),
                            /**< Acquire only systems for which the roaming
                            ** indication is off or solid on - i.e. not
                            ** flashing */

  SD_SS_ROAM_PREF_ROAM_ONLY    = BM(SD_ROAM_ON),
                            /* Acquire only systems that is not home or 
                            ** custom home
                            */

  SD_SS_ROAM_PREF_ANY     = 0xFF,
                            /**< Acquire systems regardless of their roaming
                            ** indication */

  SD_SS_ROAM_PREF_MAX
} sd_ss_roam_pref_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of origination modes. Do not change the value of existing
** enumes, as they are being reported in SD events.

*/
typedef enum {

  SD_SS_ORIG_MODE_NONE      = 0,/**< Not in call origination mode */

  SD_SS_ORIG_MODE_NORMAL    = 1,/**< Normal call origination (i.e., call other
                                ** than OTASP or emergency */

  SD_SS_ORIG_MODE_COLLOC    = 2,/**< Collocated system origination (i.e., try
                                ** originating on a collocated system */

  SD_SS_ORIG_MODE_OTASP     = 3,/**< OTASP call origination */

  SD_SS_ORIG_MODE_EMERG_ORIG= 4,/**< Emergency call origination */

  SD_SS_ORIG_MODE_EMERG_CB  = 5,/**< Emergency callback mode.
                                **
                                ** NOTE! This mode is automatically assigned
                                ** by SD when the right conditions apply
                                ** during an emergency call origination */

  SD_SS_ORIG_MODE_MANUAL_IDLE = 6,/**< Manual system selection bypassing the
                                ** System Determination algorithms to select
                                ** a system */

  SD_SS_ORIG_MODE_MANUAL_ORIG = 7,
                                /**< Manual system selection in call
                                ** origination */

  SD_SS_ORIG_MODE_MAX
} sd_ss_orig_mode_e_type;

/*--------------------------------------------------------------------------
                           Call Origination Mode
---------------------------------------------------------------------------*/

/** Enumeration of Call Origination Mode combinations.
*/
typedef enum {

  SD_SS_ORIG_MODE_VAL_NONE           = BM( SD_SS_ORIG_MODE_NONE ),
                                      /**< Not in origination */

  SD_SS_ORIG_MODE_VAL_NORMAL         = BM( SD_SS_ORIG_MODE_NORMAL ),
                                      /**< Normal call origination (i.e. call
                                      ** other than OTASP or Emergency) */


  SD_SS_ORIG_MODE_VAL_COLLOC         = BM( SD_SS_ORIG_MODE_COLLOC ),
                                      /**< Collocated system origination (i.e.,
                                      ** try originating on a collocated
                                      ** system */

  SD_SS_ORIG_MODE_VAL_OTASP          = BM( SD_SS_ORIG_MODE_OTASP ),
                                      /**< OTASP call origination */

  SD_SS_ORIG_MODE_VAL_EMERG_ORIG     = BM( SD_SS_ORIG_MODE_EMERG_ORIG ),
                                      /**< Emergency call origination */


  SD_SS_ORIG_MODE_VAL_EMERG_CB       = BM( SD_SS_ORIG_MODE_EMERG_CB ),
                                      /**< Emergency callback mode */

  SD_SS_ORIG_MODE_VAL_EMERG          = BM( SD_SS_ORIG_MODE_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_CB ),
                                      /**< Emergency mode - orig or callback */

  SD_SS_ORIG_MODE_VAL_EMERG_ORIG_OR_NONE_OR_MAN_IDLE = \
                                       BM( SD_SS_ORIG_MODE_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_NONE ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_IDLE ),
                                       /**< Emerg call orig or end of any call
                                       ** in Auto or Manual mode
                                       */

  SD_SS_ORIG_MODE_VAL_OTASP_OR_EMERG = BM( SD_SS_ORIG_MODE_OTASP ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_CB ),
                                       /**< OTASP or emergency mode (orig or
                                       ** callback) */

  SD_SS_ORIG_MODE_VAL_MANUAL_ORIG    = BM( SD_SS_ORIG_MODE_MANUAL_ORIG ),
                                       /**< User manually call origination. */

  SD_SS_ORIG_MODE_VAL_MANUAL_IDLE    = BM( SD_SS_ORIG_MODE_MANUAL_IDLE ),
                                       /**< User manually idle. */

  SD_SS_ORIG_MODE_VAL_MANUAL         = BM( SD_SS_ORIG_MODE_MANUAL_IDLE ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_ORIG ),
                                       /**< User manually selected system. */

  SD_SS_ORIG_MODE_VAL_MAN_OR_NONE    = BM( SD_SS_ORIG_MODE_MANUAL_IDLE ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_NONE ),
                                       /**< User manually selected system or
                                       ** none. */

  SD_SS_ORIG_MODE_VAL_MAN_OR_NORM    = BM( SD_SS_ORIG_MODE_MANUAL_IDLE ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_NORMAL ),
                                       /**< User manually selected system or
                                       ** normal. */

  SD_SS_ORIG_MODE_VAL_ORIG           = BM( SD_SS_ORIG_MODE_NORMAL ) | \
                                       BM( SD_SS_ORIG_MODE_OTASP ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_CB ) | \
                                       BM( SD_SS_ORIG_MODE_COLLOC ) |  \
                                       BM( SD_SS_ORIG_MODE_MANUAL_ORIG ),
                                       /**< Any call origination */

  SD_SS_ORIG_MODE_VAL_ANY            = BM( SD_SS_ORIG_MODE_NORMAL ) | \
                                       BM( SD_SS_ORIG_MODE_OTASP ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_CB ) | \
                                       BM( SD_SS_ORIG_MODE_COLLOC ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_IDLE ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_ORIG ),
                                       /**< Any call origination */


  SD_SS_ORIG_MODE_VAL_MAX

} sd_ss_orig_mode_val_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration to indicate the type of system to be avoided
*/
typedef enum {

  SD_SS_AVOID_SYS_IDLE_IF_UZ,  /**< Avoid idle system only
                               ** if MS has currently selected
                               ** user zone */

  SD_SS_AVOID_SYS_IDLE,        /**< Avoid idle system - SID/NID until pwr
                               ** cycle.
                               */

  SD_SS_AVOID_SYS_MAX
} sd_ss_avoid_sys_e_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of hybrid preferences.
*/
typedef enum {

  SD_SS_HYBR_PREF_NONE,            /**< Hybrid operation is not allowed */
  SD_SS_HYBR_PREF_CDMA_HDR,        /**< CDMA/HDR hybrid operation is allowed */
  SD_SS_HYBR_PREF_CDMA__WCDMA,     /**< CDMA/ WCDMA hybrid operation is allowed */
  SD_SS_HYBR_PREF_CDMA__HDR_WCDMA, /**< CDMA and either WCDMA or HDR whichever
                                    ** is available
                                    */
  SD_SS_HYBR_PREF_MAX              /**< Internal SD use */

} sd_ss_hybr_pref_e_type;

/** For backward compatibility and to avoid
** compiler errors / lint isues in switch()
*/
#define SD_SS_HYBR_PREF_CDMA__HDR SD_SS_HYBR_PREF_CDMA_HDR

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of reasons to enter power save.
*/
typedef enum {
  SD_SS_PWR_SAVE_REAS_NO_SRV,    /**< Entering pwr save due to acquisition
                                 ** failures.
                                 */

  SD_SS_PWR_SAVE_REAS_NO_SYS,    /**< Entering pwr save because no systems are
                                 ** available to attempt acquisitions possibly
                                 ** because user changed preferences or
                                 ** card became unavailable.
                                 */

  SD_SS_PWR_SAVE_REAS_PWR_DOWN,  /**< Entering power save because the protocol
                                 ** stack needs to be powered down with
                                 ** power-down deregistration (if supported by
                                 ** protocol).
                                 */

  SD_SS_PWR_SAVE_REAS_MAX

} sd_ss_pwr_save_reas_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**
**  Enumerated type used to specify the PLMN to search for during manual
**  PLMN selection.
*/
typedef enum
{
  /** User origination during power up.*/
  SD_MANUALLY_SELECTED_RPLMN,
  /** Plmn is specified. */
  SD_MANUALLY_SELECTED_SPECIFIED_PLMN,
  /** Plmn select mode is Home PLMN. */
  SD_MANUALLY_SELECTED_HPLMN,

  /** FOR INTERNAL USE ONLY! */
  SD_MANUALLY_SELECTED_MAX

} sd_manually_selected_plmn_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of preferences which determine the order in which systems
** must be attempted acquisitions when powering up the MS.
*/
typedef enum {

  SD_SS_ACQ_ORDER_PREF_GSM_WCDMA, /**< Attempt to acquire GSM systems before
                                  ** WCDMA systems.
                                  */

  SD_SS_ACQ_ORDER_PREF_WCDMA_GSM, /**< Attempt to acquire WCDMA systems before
                                  ** GSM systems.
                                  */

  SD_SS_ACQ_ORDER_PREF_AUTO,      /**< Acquisition order to be determined by
                                  ** system selection algorithms without
                                  ** this acquisition order preference.
                                  */

  SD_SS_ACQ_ORDER_PREF_MAX

} sd_ss_acq_order_pref_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of gw scan status
*/
typedef enum {

  SD_SS_GW_SCAN_STATUS_NONE = -1, /**< Internal SD use
                                  */

  SD_SS_GW_SCAN_STATUS_COMPLETE_NO_PLMN,  /**< Scan completed in its allotted
                                          ** time, NO PLMN found
                                          */

  SD_SS_GW_SCAN_STATUS_COMPLETE_PLMN_FOUND, /**< Scan completed, PLMN found
                                            */

  SD_SS_GW_SCAN_STATUS_ABORTED,   /**< Scan aborted due to timer expiry
                                  */

  SD_SS_GW_SCAN_STATUS_MAX        /**< Internal SD use
                                  */
} sd_ss_gw_scan_status_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of acq scan types
*/
typedef enum {

  SD_SS_ACQ_SCAN_TYPE_NONE = -1,  /**< Internal SD use
                                  */

  SD_SS_ACQ_SCAN_TYPE_NORMAL,     /**< Normal scan
                                  */

  SD_SS_ACQ_SCAN_TYPE_PWR_UP,     /**< Power up scan
                                  */

  SD_SS_ACQ_SCAN_TYPE_MAX         /**< Internal SD use
                                  */

} sd_ss_acq_scan_type_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of SS-Preference change reasons. Do not change the value of
** existing enumes, as they are being reported in SD events.
*/
typedef enum {

  SD_SS_PREF_REAS_USER        = 0,  /**< User changed the SS-Preference */
  SD_SS_PREF_REAS_ORIG_START_CS
                              = 1,  /**< Origination starts for CS call */
  SD_SS_PREF_REAS_REDIAL_NORM = 2,  /**< Normal silent redial */
  SD_SS_PREF_REAS_REDIAL_OPTI = 3,  /**< Optimized silent redial */
  SD_SS_PREF_REAS_ORIG_END    = 4,  /**< Origination ends */
  SD_SS_PREF_REAS_AVOID_SYS   = 5,  /**< Avoid system     */
  SD_SS_PREF_REAS_USER_RESEL  = 6,  /**< User initiated reselection. */
  SD_SS_PREF_REAS_ORIG_START_PS
                              = 7,  /**< Origination starts for PS call */
  SD_SS_PREF_REAS_ORIG_RESUME = 8,  /**< PS call reorignation in dormant */
  SD_SS_PREF_REAS_PSEUDO_ONLINE
                              = 9,  /**< Pseudo idle mode of operation */
  SD_SS_PREF_REAS_HDR_CDMA_HO = 10, /**< HDR to CDMA Handoff */
  SD_SS_PREF_REAS_STACK_SYNC_UP 
                              = 11, /**< GW and HYBR HDR sync up */
  SD_SS_PREF_REAS_MULTIMODE_SCAN 
                              = 12, /* <This is used for service aquisition with
                                    ** multimode preferences when PS call is in dormant
                                    */
  SD_SS_PREF_REAS_MAX
} sd_ss_pref_reas_e_type;


/* <EJECT> */
/*--------------------------------------------------------------------------
                          Redirection Parameters
---------------------------------------------------------------------------*/

/** Enumeration of Redirection Types.
*/
typedef enum {

  SD_SS_REDIR_NONE,             /**< Not in redirection - internal use only */

  SD_SS_REDIR_CDMA_SRDM,        /**< CDMA Directed Service Redirection */
  SD_SS_REDIR_CDMA_GSRDM,       /**< CDMA Global Service Redirection */
  SD_SS_REDIR_CDMA_NDSS,        /**< CDMA NDSS redirection */

  SD_SS_REDIR_HDR_QC,           /**< HDR QC Redirection */
  SD_SS_REDIR_HDR_DIRECTED,     /**< HDR Directed Redirection */

  SD_SS_REDIR_MAX
} sd_ss_redir_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of Redirection system ordering
** (see IS-2000-5, Table 3.7.2.3.2.16-3).
*/
typedef enum {

  SD_SS_SYS_ORD_A_OR_B,         /**< Attempt to obtain service on either
                                ** System A or B in accordance with the
                                ** custom system selection process */

  SD_SS_SYS_ORD_A_ONLY,         /**< Attempt to obtain service on System A
                                ** only */

  SD_SS_SYS_ORD_B_ONLY,         /**< Attempt to obtain service on System B
                                ** only */

  SD_SS_SYS_ORD_A_THEN_B,       /**< Attempt to obtain service on System A
                                ** first. If unsuccessful, attempt to obtain
                                ** service on System B */

  SD_SS_SYS_ORD_B_THEN_A,       /**< Attempt to obtain service on System B
                                ** first. If unsuccessful, attempt to obtain
                                ** service on System A */

  SD_SS_SYS_ORD_A_AND_B,        /**< Attempt to obtain service on either
                                ** System A or System B. If unsuccessful,
                                ** attempt to obtain service on the
                                ** alternate system (System A or System B)*/

  SD_SS_SYS_ORD_MAX
} sd_ss_sys_ord_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** A magic value that when assigned to a max_redirect_delay field/parameter
** indicates that the field/parameter should not be used to determine the
** AMPS registration time.
*/
#define  SD_SS_MAX_REDIR_DELAY_NONE   (0xFFFF)




/* <EJECT> */
/*--------------------------------------------------------------------------
                            OH Parameters
---------------------------------------------------------------------------*/
typedef enum {

  SD_SS_OVHD_STATUS_NONE,        /**< Internal use for range check */

  SD_SS_OVHD_STATUS_COMPLETE,    /**< All overheads are received */

  SD_SS_OVHD_STATUS_INCOMPLETE,  /**< Do not receive all the overheads */

  SD_SS_OVHD_STATUS_MAX          /**< Internal use for range check */

} sd_ss_ovhd_status_e_type;



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumberation type of addtional redirection record type
**
** The base station shall set this field to the RECORD_TYPE value shown in
** Table 3.7.2.3.2.16-2 corresponding to the type of redirection specified
** by this record.
**
*/
typedef enum {

  SD_REDIR_REC_NDSS_OFF = 0,   /**< NDSS off indication. */

  SD_REDIR_REC_AMPS     = 1,   /**< Redirection to an analog system as defined
                               ** in sd_redir_rec_amps_s_type */

  SD_REDIR_REC_CDMA     = 2,   /**< Redirection to a CDMA system as defined in
                               ** sd_redir_rec_cdma_s_type */

  SD_REDIR_REC_TACS     = 3,   /**< Redirection to a TACS analog system as
                               ** defined in Department of Trade and
                               ** Industry's TACS Mobile Station-Land Station
                               ** Compatibility Specification, Issue 4,
                               ** Amendment 1.
                               **
                               ** Not implemented.*/

  SD_REDIR_REC_JTACS    = 4,   /**< Redirection to a JTACS analog system as
                               ** defined in ARIB's RCR STD-36.
                               **
                               ** Not implemented */

  SD_REDIR_REC_DS41     = 5,   /**< Redirection to a DS-41 system
                               **
                               ** Not implemented */

  SD_REDIR_REC_MAX             /**< SD internal use only ! */

}sd_redir_rec_type_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type of an analog system redirection record structure
**
*/
typedef struct {

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** EXPECTED_SID field. If MS is redirected to a specific system, specify
  ** the SID of that system; otherwise, it should be 0
  */
  word                      expected_sid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** IGNORE_CDMA indicator. Indicates whether the MS is to ignore the CDMA
  ** Capability Message on the analog system to which it is being redirected
  */
  boolean                   is_ignore_cdma;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** System ordering. Indicate the order in which the mobile station is to
  ** attempt to obtain service on an analog system
  */
  sd_ss_sys_ord_e_type      sys_ord;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Maximum delay upon redirection. Indicate the maximum delay
  ** time, in units of 8 second increments, to be used by MS in the
  ** event of a global redirection (GSRDM) to analog mode in order
  ** to avoid overloading an underlying analog cell's reverse
  ** control channel with MS registration.
  **
  ** A value of SD_SS_MAX_REDIR_DELAY_NONE indicates that this
  ** field should not be used to determine the AMPS registration
  ** time.
  */
  word                      max_redirect_delay;

} sd_redir_rec_amps_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** The maximum number of channels per CDMA system redirection record.*/
#define SD_MAX_NUM_REDIR_CHAN    16

/** Type of a CDMA system redirection record structure
**
*/
typedef struct {

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Band class. Indicate the CDMA band class, as specified in
  ** TSB58-A, for which the MS is being directed.
  */
  sd_band_e_type            band_class;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Expected SID. If the MS is being redirected to a specific
  ** system, indicate the SID of that system; otherwise, should be
  ** set to SD_WILDCARD_SID.
  */
  word                      expected_sid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Expected NID. If the MS is being redirected to a specific
  ** network, indicate the NID of that network; otherwise, should
  ** be set to SD_WILDCARD_NID.
  */
  word                      expected_nid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Number of CDMA channels in the array that is pointed to by
  ** chans_ptr.
  */
  int                       num_chans;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Array of CDMA channels to which the MS is being redirected.
  */
  sd_chan_type             chan[SD_MAX_NUM_REDIR_CHAN];


} sd_redir_rec_cdma_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for the redirection record structure
**
*/
typedef struct {

  /** Specify the redirection record type */
  sd_redir_rec_type_e_type        record_type;

  /** The redirection record union type */
  union {

       sd_redir_rec_cdma_s_type cdma_rec; /**< AMPS system redirection record
                                          ** type
                                          */
       sd_redir_rec_amps_s_type amps_rec; /**< CDMA system redirection record
                                          ** type
                                          */
   } rec;

   /** If MS is required to delete the TMSI assigned to the mobile station;
   ** otherwise, the base station shall set this field to '0'.
   */
   boolean                        is_delete_tmsi;

} sd_redir_rec_s_type;



/* <EJECT> */
/*--------------------------------------------------------------------------
                            Handoff Parameters
---------------------------------------------------------------------------*/


/** Enumeration of Handoff types that results from CAM or HDM.
*/
typedef enum {

  SD_SS_HO_NONE,                    /**< Not in handoff */

  /* CDMA to CDMA handoffs.
  */
  SD_SS_HO_CDMA_CAM_TO_CDMA_PAG,    /**< CDMA CAM to CDMA paging channel */
  SD_SS_HO_CDMA_CAM_TO_CDMA_TRF,    /**< CDMA CAM to CDMA traffic channel */
  SD_SS_HO_CDMA_HDM_TO_CDMA_TRF,    /**< CDMA HDM to CDMA traffic channel */

  /* CDMA to AMPS handoffs.
  */
  SD_SS_HO_CDMA_CAM_TO_AMPS_SYS,    /**< CDMA CAM to AMPS system (A/B) */
  SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE,  /**< CDMA CAM to AMPS voice channel */
  SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE,  /**< CDMA HDM to AMPS voice channel */

  /* AMPS to AMPS handoffs.
  */
  SD_SS_HO_AMPS_CAM_TO_AMPS_VOICE,  /**< AMPS CAM to AMPS voice channel */

  SD_SS_HO_HDR_TO_CDMA,             /**< VoIP on HDR to CDMA voice channel */

  SD_SS_HO_MAX
} sd_ss_ho_e_type;



/* <EJECT> */
/*--------------------------------------------------------------------------
                           Access Reasons
---------------------------------------------------------------------------*/

/** Enumeration of access reasons.
*/
typedef enum {

  SD_SS_ACC_REAS_REG,           /**< Registration */
  SD_SS_ACC_REAS_ORIG,          /**< Origination */
  SD_SS_ACC_REAS_PAGE,          /**< Page response */
  SD_SS_ACC_REAS_DATA,          /**< Data burst */
  SD_SS_ACC_REAS_TRAFFIC,       /**< Traffic channel (to idle transition) */
  SD_SS_ACC_REAS_OTHER,         /**< Reason other than the above */

  SD_SS_ACC_REAS_MAX

} sd_ss_acc_reas_e_type;


/* <EJECT> */
/*--------------------------------------------------------------------------
                         Access Termination Parameters
---------------------------------------------------------------------------*/

/** Enumeration of access termination reasons (i.e., reason for leaving the
** access state).
*/
typedef enum {

  SD_SS_ACC_TERM_ACC_DEN,       /**< When access attempt terminates due to MS
                                ** failing the access persistence test */

  SD_SS_ACC_TERM_TIMEOUT,       /**< When access attempt terminates due to
                                ** access state timeout
                                ** i.e. for HDR
                                ** connection setup timeout.
                                */

  SD_SS_ACC_TERM_MAX_ACC,       /**< When access attempt terminates due to
                                ** max access probes being transmitted */

  SD_SS_ACC_TERM_REORDER,       /**< When access attempt terminates due to
                                ** receiving reorder order */

  SD_SS_ACC_TERM_INTERCEPT,     /**< When access attempt terminates due to
                                ** receiving intercept order */

  SD_SS_ACC_TERM_BS_REL,        /**< When access attempt terminates due to
                                ** receiving release order */

  SD_SS_ACC_TERM_MS_REL,        /**< When access attempt terminates due to
                                ** user release */

  SD_SS_ACC_TERM_CON_DEN,       /**< When HDR access attempt terminates due to
                                ** receiving connection deny with reason
                                ** set to general */

  SD_SS_ACC_TERM_NET_BUSY,      /**< When HDR access attempt terminates due to
                                ** receiving connection deny with reason
                                ** set to network busy */

  SD_SS_ACC_TERM_AUTH_FAIL,     /**< When HDR access attempt terminates due to
                                ** receiving connection deny with reason
                                ** set to authentication or billing
                                ** failure */

  SD_SS_ACC_TERM_ACC_FAIL,      /**< When access attempt terminates due to
                                ** failure other than one of the above */

  SD_SS_ACC_TERM_NORMAL,        /**< When access attempt terminates due to
                                ** normal access completion */

  SD_SS_ACC_TERM_SO_DENIED,     /**< When access attempt terminates due to
                                ** BS denying the SO. */


  SD_SS_ACC_TERM_MAX

} sd_ss_acc_term_e_type;


/* <EJECT> */
/*--------------------------------------------------------------------------
                         Bad system reason
---------------------------------------------------------------------------*/

/** Enumeration of reasons for invalid system.
*/
typedef enum {

  SD_SS_BAD_SYS_TCA_INV_BAND,
                                /**< System is invalid due to invalid band
                                ** in the TCA.
                                */

  SD_SS_BAD_SYS_TCA_INV_NUM_PILOTS,
                                /**< System is invalid due to invalid number
                                ** of pilots in the TCA.
                                */

  SD_SS_BAD_SYS_HASH_INV_BAND,
                                /**< System is invalid due to hashing to invalid
                                ** band.
                                */


  SD_SS_BAD_SYS_HDR_INV_AP_CAPSULE_LEN,
                                /**< System is invalid due to invalid capsule
                                ** length. << For HDR only >>
                                */

  SD_SS_BAD_SYS_MAX

} sd_ss_bad_sys_e_type;


/** Enumeration of reasons for invalid wlan system.
*/
typedef enum {

  SD_SS_WLAN_BAD_SYS_SECUR_BREACH,
                                /**< System is invalid due to a possible breach
                                ** in security
                                */

  SD_SS_WLAN_BAD_SYS_MAX

} sd_ss_wlan_bad_sys_e_type;


/* <EJECT> */
/*--------------------------------------------------------------------------
                     HDR activity enumeration type
---------------------------------------------------------------------------*/

/** Enumberation of HDR active activity bit mask.
*/
typedef enum {

  SD_SS_HDR_ACT_MASK_NONE     =0,
                                /**< NONE HDR activity, internal use only
                                */
  SD_SS_HDR_ACT_MASK_BCMCS    =BM(0),
                                /**< HDR BCMCS activity
                                */
  SD_SS_HDR_ACT_MASK_TRAFFIC  =BM(1),
                                /**< HDR enter/exit traffic channel\
                                */
  SD_SS_HDR_ACT_MASK_IDLE_LOW_SCI
                              =BM(2),
                                /**< HDR enter/exit idle with a slot sleep
                                ** cycle index <= 6
                                */

  SD_SS_HDR_ACT_MASK_SESSION = BM(3),
                                /**< HDR enter/exit session-related traffic
                                */


  SD_SS_HDR_ACT_MASK_ANY      =0x7F,
                                /**< HDR any activities
                                */
  SD_SS_HDR_ACT_MASK_MAX      =SD_SS_HDR_ACT_MASK_ANY+1
                                /**< HDR MAX activity, internal use only
                                */
} sd_ss_hdr_act_mask_e_type;

/* <EJECT> */
/*--------------------------------------------------------------------------
                       HDR Session Closed Parameters
---------------------------------------------------------------------------*/

/** Enumeration of access termination reasons.
*/
typedef enum {

  SD_SS_SES_CLOSE_NORMAL,       /**< Normal close */
  SD_SS_SES_CLOSE_REPLY,        /**< Replay to received SessionClose message */
  SD_SS_SES_CLOSE_ERR,          /**< Protocol error */
  SD_SS_SES_CLOSE_CONFIG_ERR,   /**< Protocol configuration error */
  SD_SS_SES_CLOSE_NEG_ERR,      /**< Protocol negotiation error */
  SD_SS_SES_CLOSE_CONFIG_FAIL,  /**< Session configuration failure */

  SD_SS_SES_CLOSE_MAX
} sd_ss_ses_close_e_type;

/* <EJECT> */
/*--------------------------------------------------------------------------
                       HDR System Lost Parameters
---------------------------------------------------------------------------*/

/** Enumeration of system lost reasons.
*/
typedef enum {

  SD_SS_SYS_LOST_NONE,                    /**< System lost reason is not
                                          ** provided. Internal use only */

  SD_SS_SYS_LOST_LOST_ACQ_AFTER_SLEEP,    /**< Could not reacquire after sleep
                                          */

  SD_SS_SYS_LOST_RUP_PILOT_SUPERVISION,   /**< [6.6.5.5.2]  The strongest pilot
                                          ** is too weak for too long */

  SD_SS_SYS_LOST_SYNC_MSG_TIMEOUT,        /**< Too long in sync state waiting
                                          ** for sync message */

  SD_SS_SYS_LOST_NO_RESOURCES,           /**< Lock in RF Resources was absent
                                         ** for too long */

  SD_SS_SYS_LOST_MSTR_SLAM_TIMEOUT,      /**< MSTR slam could not be completed
                                         ** for too long due to unlocked
                                         ** fingers */

  SD_SS_SYS_LOST_RESELECT,               /**< HDR signal is weak - may be
                                         ** moving out of coverage */

  SD_SS_SYS_LOST_ADVISE_UNLOCK_RF,       /**< HDR/1x was told to release the RF
                                         ** lock, if it held it */

  SD_SS_SYS_LOST_SYNC_WEAK_HDR_COVERAGE, /**< HDR coverage is weak at sync
                                         ** state */

  SD_SS_SYS_LOST_DSP_NOT_READY,          /**< DSP application is not ready */

  SD_SS_SYS_LOST_SUP_FAILURE,            /**< Supervision failure */

  SD_SS_SYS_LOST_PROT_Q_FULL,            /**< Protocol Queue Full - force
                                         ** reacquisition */

  SD_SS_SYS_LOST_UNKNOWN,                /**< Unknown reason */

  SD_SS_SYS_LOST_MAX                     /**< System lost reas max, internal
                                         ** use only */
} sd_ss_sys_lost_e_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


typedef struct {

  /** Type of measurement to perform.
  */
  sd_ss_meas_type_e_type        type;

  /** Measurement ID. The SS-Client should save this ID and submit it back to
  ** SD when calling on sd_ss_misc_meas_rprt() to report the measurement.
  */
  word                          id;

  /** WLAN scan types allowed for measurment.
  */
  sys_wlan_scan_pref_e_type     wlan_scan_pref;

} sd_ss_act_meas_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for parameters that are associated with the SD_SS_ACT_ACQ_CDMA
** returned SS-Action value.
*/
typedef struct {

  /** The band class of the system to acquire.
  */
  sd_band_e_type          band;

  /** The CDMA channel of the system to acquire.
  */
  sd_chan_type            chan;

  /** The PCS frequency block/cellular system of the system to acquire.
  */
  sd_blksys_e_type        blksys;

  /** New mode or blksys. Set to TRUE if switching from using CDMA in a
  ** different band class, from using CDMA in a different Band Class 0
  ** serving system, from using CDMA in a different Band Class 1 frequency
  ** block, or from using the 800 MHz analog system.
  */
  boolean                 is_new_mode_or_blksys;

  /** Is CDMA in a handoff from HDR
  */
  boolean                 is_handoff_acq;

} sd_ss_act_acq_cdma_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for parameters that are associated with the SD_SS_ACT_ACQ_AMPS
** returned SS-Action value.
*/

typedef struct {

  /** The band class of the system to acquire.
  */
  sd_band_e_type          band;

  /** The cellular system to be acquired.
  */
  sd_cell_sys_e_type      sys;

  /** AMPS 2nd strongest CCH - indicate whether AMPS 2nd strongest control
  ** channel (as apposed to strongest control channel) is to be attempted
  ** acquisition.
  */
  boolean                 is_amps_2nd_cch;

  /** Maximum redirect delay. When the BS redirects the MS from a CDMA system
  ** to an AMPS system (using GSRDM), this field indicate the maximum
  ** redirect delay time, in units of 8 second increments, to be used in the
  ** AMPS registration randomization algorithm.
  **
  ** A value of SD_SS_MAX_REDIR_DELAY_NONE indicates that this field should
  ** not be used to determine the AMPS registration time.
  */
  word                    max_redirect_delay;

} sd_ss_act_acq_amps_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for parameters that are associated with the SD_SS_ACT_ACQ_HDR
** returned SS-Action value.
*/

#define  SD_HDR_ACQ_MODE_MICRO        SYS_HDR_ACQ_MODE_MICRO
#define  SD_HDR_ACQ_MODE_SHALLOW      SYS_HDR_ACQ_MODE_SHALLOW
#define  SD_HDR_ACQ_MODE_DEEP         SYS_HDR_ACQ_MODE_DEEP
#define  SD_HDR_ACQ_MODE_FULL         SYS_HDR_ACQ_MODE_FULL
#define  SD_HDR_ACQ_MODE_MAX          SYS_HDR_ACQ_MODE_MAX
#define  sd_hdr_acq_mode_e_type       sys_hdr_acq_mode_e_type

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for parameters that are associated with the SD_SS_ACT_ACQ_HDR
** returned SS-Action value.
*/
typedef struct {

  /** The band class of the system to acquire.
  */
  sd_band_e_type          band;

  /** The HDR channel of the system to acquire.
  */
  sd_chan_type            chan;

  /** HDR acquisition mode.
  */
  sd_hdr_acq_mode_e_type  acq_mode;

} sd_ss_act_acq_hdr_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** A type for parameters that are associated with the SD_SS_ACT_PWR_SAVE.
*/
typedef struct {

  /** Reason for entering pwr_save.
  */
  sd_ss_pwr_save_reas_e_type   reas;

} sd_ss_act_pwr_save_s_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The new service request reason that SD has to pass 
   to Reg Proxy.
*/
typedef enum 
{
  SD_SS_SRV_REQ_TYPE_NONE = -1,       /* For SD internal use */
  SD_SS_SRV_REQ_TYPE_SYNC_UP,         /* Sync up request */
  SD_SS_SRV_REQ_TYPE_USER_SELECTION,  /* User selects AUTO mode or a PLMN from UI 
                                         (or) periodic service request in OOS/after 
                                         deep sleep or Power ON */
  SD_SS_SRV_REQ_TYPE_SYSTEM_CHANGE,   /* System parameters change
                                         mode/band pref etc */    
  SD_SS_SRV_REQ_TYPE_REQ_PERIODIC,    /* Request type after BPLMN search */ 
  SD_SS_SRV_REQ_TYPE_NORMAL,          /* For rest of the cases */
  SD_SS_SRV_REQ_TYPE_BSR,             /* Better system re-selection */
  SD_SS_SRV_REQ_TYPE_MAX
}sd_ss_srv_req_type_e_type;

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_MM_SUPERSET)
/** Type for parameters that are associated with the SD_SS_ACT_ACQ_GW
** returned SS-Action value.
*/
typedef struct {

  /** Network selection type for GSM/WCDMA.
  */
  sys_network_selection_mode_e_type    net_sel_type;

  /** PLMN to be acquired, Valid only when net_sel_type ==
  ** SD_SS_NET_SEL_MANUAL.
  */
  sys_plmn_id_s_type                   plmn;

  /** Service domain preference for the acquisition.
  */
  sd_ss_srv_domain_pref_e_type         srv_domain_pref;

  /** System mode preference ( GSM only, WCDMA only or GSM/WCDMA ).
  */
  sd_ss_mode_pref_e_type               gw_mode_pref;

  /** Acquisition order preference.
  */
  sd_ss_acq_order_pref_e_type          acq_order_pref;

  /** Is this acquisition because of user (re)selection for automatic mode.
  */
  boolean                              is_user_resel;

  /** Manual selected plmn enum type only valid when network selection type
  ** is set to manual.
  */
  sd_manually_selected_plmn_e_type     selected_plmn_type;

  /** Band preference
  */
  sd_ss_band_pref_e_type               gw_band_pref;

  /** Flag to indicate if acquisition is being done due to pending
  ** origination or not.
  */
  boolean                              is_orig_pending;

  /** Parameters for GW uniform scan
  */

  /** Use timer, the timer values are set to valid values
  */
  boolean                              gw_use_timer;

  /** Is this a new acquisition or the continuation of previous scan
  */
  boolean                              gw_new_acq;

  /** Total time the scan should be completed
  */
  dword                                gw_total_scan_time;

  /** Time for UMTS scan
  */
  dword                                gw_wcdma_scan_time;

  /** Time for gsm scan
  */
  dword                                gw_gsm_scan_time;

  /** Type of scan
  */
  sd_ss_acq_scan_type_e_type           acq_scan_type;

  /** Simultaneous mode, applicable to UMTS + 1X mode
  */
  sys_simultaneous_mode_e_type       simultaneous_mode;

  /* The service request type to be sent to NAS */
  sd_ss_srv_req_type_e_type          srv_req_type;

} sd_ss_act_acq_gw_s_type;



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for parameters that are associated with the SD_SS_ACT_GET_NET_GW
** returned SS-Action value.
*/
typedef struct {

  /** Service domain preference for the request.
  */
  sd_ss_srv_domain_pref_e_type         srv_domain_pref;

  /** System mode preference ( GSM only, WCDMA only or GSM/WCDMA ).
  */
  sd_ss_mode_pref_e_type               gw_mode_pref;

  /** System band preference
  */
  sd_ss_band_pref_e_type               gw_band_pref;

  /** System record reference
  */
  byte                                 sdsr_ref;

  /** SD acquisition order preference
  */
  sd_ss_acq_order_pref_e_type     acq_order_pref; 

} sd_ss_act_get_net_gw_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for parameters that are associated with the SD_SS_ACT_PWR_SCAN_GW
** returned SS-Action value.
*/
typedef struct {

  /** System mode preference ( GSM only, WCDMA only or GSM/WCDMA ).
  */
  sd_ss_mode_pref_e_type               gw_mode_pref;

  /** Acquisition order preference.
  */
  sd_ss_acq_order_pref_e_type          acq_order_pref;

  /** System band preference
  */
  sd_ss_band_pref_e_type               gw_band_pref;

  /** Simultaneous mode
  */
  sys_simultaneous_mode_e_type       simultaneous_mode;

} sd_ss_act_pwr_scan_gw_s_type;

#endif /**< FEATURE_GSM || FEATURE_WCDMA || FEATURE_MM_SUPERSET */


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for parameters that are associated with the SD_SS_ACT_ACQ_WLAN
** returned SS-Action value.
*/
typedef struct {

  /** The band class of the system to acquire.
  */
  sd_band_e_type                       band;

  /** The WLAN channel of the system to acquire.
  ** LSB => channel 1, LSB+1 => channel 2, etc etc.
  */
  sd_chan_type                         chan;

  /** SS Id of the WLAN [ Can be any SSID if bssid is provided ].
  */
  sys_wlan_ssid_s_type                 ssid;

  /** BSS id of the WLAN system. if bssid = 0, implies use any access point.
  */
  sys_wlan_bssid_type                  bssid;

  /** Type of BSS attempting to acquire Adhoc or Infra.
  */
  sys_wlan_bss_e_type                  bss_type;

  /** Scan type to use.
  */
  sys_wlan_scan_type_e_type            scan_type;

  /** Indicates what type of system trying to acquire.
  ** Example manual, from priv lst, from PRL.
  ** From MANUAL_SYS/MANUAL_BSSID use the manual_profile_info.
  ** For others, use the profile_id to get the profile info.
  ** For select_mode = PRIV_SYS, use the profile_id and get the
  ** profile info from "NET" module else use the profile from OTA.
  */
  sd_net_select_mode_e_type            select_mode;

  /**
  **  To store the WLAN tech mask
  */
  sys_tech_mask_type                   wlan_tech_mask;

  union {
    /** Profile id to use for other than manual acquisition.
    */
    sys_profile_id_type                  id;

    /** Applicable for manual WLAN acquisition, use the profile info.
    */
    sys_manual_sys_profile_s_type        info;

  } profile;

  union {

    /** System record id used to read WLAN misc params for other than manual
    ** acquisition.
    */
    word                                 id;

    /** Misc WLAN params for manual acquisition.
    */
    sys_wlan_misc_params_s_type          info;

  } misc_params;

} sd_ss_act_acq_wlan_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sd_ss_act_acq_wlan_s_type.profile DISC sd_ss_act_acq_wlan_s_type.select_mode */

/*~ IF ( _DISC_ == SD_NET_SELECT_MODE_PRL_SYS ||
         _DISC_ == SD_NET_SELECT_MODE_AUTO ||
         _DISC_ == SD_NET_SELECT_MODE_PRIV_SYS ) sd_ss_act_acq_wlan_s_type.profile.id */

/*~ IF ( _DISC_ == SD_NET_SELECT_MODE_MANUAL_SYS ||
         _DISC_ == SD_NET_SELECT_MODE_MANUAL_BSSID ) sd_ss_act_acq_wlan_s_type.profile.info */

/*~ IF ( _DISC_ == SD_NET_SELECT_MODE_LIMITED ||
         _DISC_ == SD_NET_SELECT_MODE_HPLMN ) sd_ss_act_acq_wlan_s_type.profile.void */

/*~ FIELD sd_ss_act_acq_wlan_s_type.misc_params DISC sd_ss_act_acq_wlan_s_type.select_mode */
/*~ IF ( _DISC_ == SD_NET_SELECT_MODE_PRL_SYS    || 
         _DISC_ == SD_NET_SELECT_MODE_PRIV_SYS   ||
         _DISC_ == SD_NET_SELECT_MODE_AUTO )         sd_ss_act_acq_wlan_s_type.misc_params.id */
/*~ IF ( _DISC_ == SD_NET_SELECT_MODE_MANUAL_SYS ||
         _DISC_ == SD_NET_SELECT_MODE_MANUAL_BSSID ) sd_ss_act_acq_wlan_s_type.misc_params.info  */
/*~ DEFAULT sd_ss_act_acq_wlan_s_type.misc_params.void */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for parameters that are associated with the SD_SS_ACT_SURVEY_WLAN
** returned SS-Action value.
*/
typedef struct {

  /** Flag to indicate that this survey is for WPS Procedure 
  */
  boolean                             is_wps_survey;

  /** Parameters required for WPS procedure.
  */
  sys_wlan_wps_start_param_s_type     wps_start_param;

  /** The band class of the system to acquire.
  */
  sd_ss_band_pref_e_type              band;

  /** Scan type to use.
  */
  sys_wlan_scan_type_e_type           scan_type;

} sd_ss_act_survey_wlan_s_type;

/*
** The reason for WLAN acq_fail.
*/
typedef enum {

  SD_SS_WLAN_ACQ_FAIL_NONE = -1,

  SD_SS_WLAN_ACQ_FAIL_GENERAL,

  SD_SS_WLAN_ACQ_FAIL_WLAN_NOT_USABLE,

  SD_SS_WLAN_ACQ_FAIL_MAX

}sd_ss_wlan_fail_reas_e_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** A type for specifying the SS-Actions to be followed by the SS-Client upon
** an SS-Indication function call return.
*/
typedef struct {

  /** SS-Action to take.
  */
  sd_ss_act_e_type    act;

  /** SS-Action's parameters
  */
  union {
    sd_ss_act_acq_cdma_s_type          acq_cdma;
    sd_ss_act_acq_amps_s_type          acq_amps;
    sd_ss_act_acq_hdr_s_type           acq_hdr;
    sd_ss_act_pwr_save_s_type          pwr_save;
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_MM_SUPERSET)
    sd_ss_act_acq_gw_s_type            acq_gw;
    sd_ss_act_get_net_gw_s_type        get_net_gw;
    sd_ss_act_pwr_scan_gw_s_type       pwr_scan_gw;
    #endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_MM_SUPERSET */
    sd_ss_act_acq_wlan_s_type          acq_wlan;
    sd_ss_act_survey_wlan_s_type       survey_wlan;
    sd_ss_act_meas_s_type              meas;
  } prm;

} sd_ss_act_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sd_ss_act_s_type.prm DISC sd_ss_act_s_type.act        */
/*~ CASE SD_SS_ACT_ACQ_CDMA    sd_ss_act_s_type.prm.acq_cdma    */
/*~ CASE SD_SS_ACT_ACQ_AMPS    sd_ss_act_s_type.prm.acq_amps    */
/*~ CASE SD_SS_ACT_ACQ_HDR     sd_ss_act_s_type.prm.acq_hdr     */
/*~ CASE SD_SS_ACT_PWR_SAVE    sd_ss_act_s_type.prm.pwr_save    */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_MM_SUPERSET)
/*~ CASE SD_SS_ACT_ACQ_GW      sd_ss_act_s_type.prm.acq_gw      */
/*~ CASE SD_SS_ACT_GET_NET_GW  sd_ss_act_s_type.prm.get_net_gw  */
/*~ CASE SD_SS_ACT_PWR_SCAN_GW sd_ss_act_s_type.prm.pwr_scan_gw */
#endif /**< FEATURE_GSM || FEATURE_WCDMA || FEATURE_MM_SUPERSET */
/*~ CASE SD_SS_ACT_ACQ_WLAN    sd_ss_act_s_type.prm.acq_wlan    */
/*~ IF ( _DISC_ == SD_SS_ACT_SURVEY_WLAN )
            sd_ss_act_s_type.prm.survey_wlan */
/*~ IF ( _DISC_ == SD_SS_ACT_MEAS_DED ||
      _DISC_ == SD_SS_ACT_MEAS_BACK ) sd_ss_act_s_type.prm.meas */
/*~ IF ( _DISC_ == SD_SS_ACT_CONTINUE )
            sd_ss_act_s_type.prm.void */
/*~ DEFAULT sd_ss_act_s_type.prm.void */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_meas_list_get2

@DESCRIPTION
  Extract the mode, band pref and channel of each one of the systems that
  are on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_meas_list_get2(

        int                       pos,
            /**< List position for which to return the mode band and channel.
            */

        sd_ss_mode_pref_e_type    *rtrn_mode_ptr,
            /**< Pointer to a buffer where to copy the system's mode.
            */

        sd_ss_band_pref_e_type    *rtrn_band_ptr,
            /**< Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr
            /**< Pointer to a buffer where to copy the system's chan.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_meas_list_get

@DESCRIPTION
  Extract the mode, band and channel of each one of the systems that are
  on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_meas_list_get(

        int                       pos,
            /**< List position for which to get the mode band and channel.
            */

        sd_mode_e_type            *rtrn_mode_ptr,
            /**< Pointer to a buffer where to copy the system's mode.
            */

        sd_band_e_type            *rtrn_band_ptr,
            /**< Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr
            /**< Pointer to a buffer where to copy the system's chan.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_wlan_meas_list_get

@DESCRIPTION
  Extract the mode, band and channel of each one of the systems that are on
  the measurement list (i.e., systems for which the channel strength
  measurement is requested).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_wlan_meas_list_get(

        int                       pos,
            /**< List position for which to return the mode band and channel.
            */

        sd_mode_e_type            *rtrn_mode_ptr,
            /**< Pointer to a buffer where to copy the system's mode.
            */

        sd_band_e_type            *rtrn_band_ptr,
            /**< Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr,
            /**< Pointer to a buffer where to copy the system's chan.
            */

        sys_tech_mask_type        *rtrn_wlan_tech_ptr,
            /**< Pointer to a buffer where to copy the system's tech mask.
            */

        sys_wlan_bss_e_type       *rtrn_wlan_bss_type_ptr,
            /**< Pointer to a buffer where to copy the system's bss type.
            */

        sys_wlan_ssid_s_type      *rtrn_ssid_ptr,
            /**< Pointer to a buffer where a WLAN system's SSID should be
            ** copied.
            */

        byte                      *rtrn_profile_id_ptr
            /**< Pointer to a buffer where a WLAN system's profile id should be
            ** copied.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /* FEATURE_HTORPC_METACOMMENTS */


/**===========================================================================

@FUNCTION sd_ss_wlan_meas_list_get2

@DESCRIPTION
  Extract the mode, band and channel of each one of the systems that are on
  the measurement list (i.e., systems for which the channel strength
  measurement is requested).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_wlan_meas_list_get2(

        int                       pos,
            /**< List position for which to return the mode band and channel.
            */

        sd_mode_e_type            *rtrn_mode_ptr,
            /**< Pointer to a buffer where to copy the system's mode.
            */

        sd_band_e_type            *rtrn_band_ptr,
            /**< Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr,
            /**< Pointer to a buffer where to copy the system's chan.
            */

        sys_tech_mask_type        *rtrn_wlan_tech_ptr,
            /**< Pointer to a buffer where to copy the system's tech mask.
            */

        sys_wlan_bss_e_type       *rtrn_wlan_bss_type_ptr,
            /**< Pointer to a buffer where to copy the system's bss type.
            */

        sys_wlan_ssid_s_type      *rtrn_ssid_ptr,
            /**< Pointer to a buffer where a WLAN system's SSID should be
            ** copied.
            */

        byte                      *rtrn_profile_id_ptr,
            /**< Pointer to a buffer where a WLAN system's profile id should be
            ** copied.
            */

        word                      *rtrn_sys_rec_idx_ptr
            /**< Pointer to a buffer where a WLAN system's position in netlist
            ** should be copied.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /* FEATURE_HTORPC_METACOMMENTS */



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_meas_list_rprt

@DESCRIPTION
  Report channel strength measurement for each one of the systems that are
  on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

  Note that reported measurement values are in units of -1dB for Rx-power and
  -0.5dB for Pilot-strength.

  Note that a reported value of 255 implies that no measurement was obtained.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_meas_list_rprt(

        int                       pos,
            /**< List position for which measurement is reported.
            */

        int2                      meas_val
            /**< Reported channel strength measurement value in units
            ** of -1dB for Rx-power and -0.5dB for Pilot strength. 255
            ** implies that no measurement was obtained.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_wlan_meas_list_rprt

@DESCRIPTION
  Report channel strength measurement for each one of the systems that are
  on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

  Note that reported measurement values are in units of -1dB for Rx-power and
  -0.5dB for Pilot-strength.

  Note that a reported value of 255 implies that no measurement was obtained.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if reported measurement ID matches the requested measurement ID and
  list position <= position of last system on the measurement list. FALSE
  otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_wlan_meas_list_rprt(

        int                       pos,
            /**< List position for which measurement is reported.
            */

        int2                      meas_val
            /**< Reported channel strength measurement value in units
            ** of -1dB for Rx-power and -0.5dB for Pilot strength. 255
            ** implies that no measurement was obtained.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /* FEATURE_HTORPC_METACOMMENTS */


/* <EJECT> */
/*--------------------------------------------------------------------------
                              NAM Parameters
---------------------------------------------------------------------------*/

/** Type for HDR redirection channels extraction function.
**
** HDR CP should submit a pointer to a function of this type when calling
** on the sd_ss_ind_hdr_opr_redir() SS-Indication function, so that SD can
** extract the systems that are included in the redirection message.
**
** This function should extract the system at the specified redirection
** channel list position into the buffer that is pointed to by sys_ptr.
**
** This function should return TRUE if pos < number of channel included with
** the HDR redirection message. Otherwise this function should return FALSE.
*/
typedef boolean (sd_ss_sys_get_f_type) (

    /** Pointer to SS-Client callback data block.
    */
    void                    *data_ptr,

    /** Redirection channel list position from which to get the mode, band
    ** and channel.
    */
    int                     pos,

    /** Pointer to buffer where to place the returned mode, band and channel.
    */
    sd_sys_s_type           *sys_ptr
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_act_get

@DESCRIPTION
  Return the SS-Action that was most recently returned to the SS-Client
  other than CONTINUE.
  (HDR SS-Client should use sd_ss_hdr_act_get).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_act_get_true_curr_act

@DESCRIPTION
  Return the true SS-Action that was most recently returned to the SS-Client.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_hdr_act_get

@DESCRIPTION
  Return the SS-Action that was most recently returned to the HDR SS-Client
  other than CONTINUE


@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called by the HDR client.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_hdr_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

FUNCTION sd_ss_hdr_act_get_true_curr_act

@DESCRIPTION
  Return to the HDR client the true SS-Action that was most recently returned to
  the SS-Client.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_hdr_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/**===========================================================================

@FUNCTION sd_ss_gw_act_get

@DESCRIPTION
  Return to the HDR client the true SS-Action that was most recently returned to
  the SS-Client.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_gw_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/**===========================================================================

@FUNCTION sd_ss_hybr_1_act_get_true_curr_act

@DESCRIPTION
  Return to the HYBR_1 client the SS-Action that was most recently returned to
  the SS-Client.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_gw_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
/**===========================================================================

@FUNCTION sd_ss_hybr_1_act_get

@DESCRIPTION
  Return to the HDR client the true SS-Action that was most recently returned to
  the SS-Client.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_hybr_1_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_hybr_1_act_get_true_curr_act

@DESCRIPTION
  Return to the HYBR_1 client the SS-Action that was most recently returned to
  the SS-Client.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_hybr_1_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_wlan_act_get

@DESCRIPTION
  Return the SS-Action that was most recently returned to the HYBR_WLAN
  SS-Client other than CONTINUE


@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called by the HDR client.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_wlan_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION sd_ss_wlan_act_get */
/*~ PARAM OUT rtrn_act_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_wlan_act_get_true_curr_act

@DESCRIPTION
  Return to the HYBR_WLAN client the true SS-Action that was most recently
  returned to the SS-Client.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_wlan_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




#ifdef FEATURE_SD20_LOGGING
/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_log_last_action

@DESCRIPTION
  Logs information about System Determination state and variables

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_ss_log_last_action(

        sd_ss_e_type              ss
            /**< System selection - MAIN or HDR.
            */
);
#endif /* FEATURE_SD20_LOGGING */


/** @}
**
*/


/* <EJECT> */
/**===========================================================================
=============================================================================
============================ USER SS-INDICATIONS ============================
=============================================================================
===========================================================================*/

/** @defgroup US User SS-Indications
*  @{
*/

/* SS-Indications that are a direct result of a user action, such as system
** preference change or NAM selection change. These indications are usually
** mode independent; or in other words, they can be received in more than one
** of the SS-Client operating modes (e.g. CDMA, AMPS or PWR_SAVE).
*/

/**===========================================================================

@FUNCTION sd_ss_ind_user_pwr_up

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to power-up (from OFF or from LPM).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_pwr_up(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_offline_cdma

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to enter offline CDMA mode of operation.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with CDMA offline operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_offline_cdma(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/*===========================================================================

@FUNCTION sd_ss_ind_user_offline_amps

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to enter offline AMPS mode of operation.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with AMPS offline operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_offline_amps(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_prot_deactivate

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user informs that
  protocol must be deactivated.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with CDMA offline operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_prot_deactivate(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_pwr_save_exit

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to exit power save mode of operation.

  Note that the UI is the one to determine when the user wants the phone to
  exit power save mode of operation. Typically, however, the UI interprets
  any key press (while MS in power-save) as indication that the user wants
  the MS to exit power-save mode and to start searching for systems actively.

  Note that the actual SD behavior when getting this indication from the
  SS-Client is controlled by the SS-Script for this indication. This script,
  however, is expected to be fairly similar to the power-up script.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_ss_pref

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

  NOTE:: Function maintained for backwards compatability.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_ss_pref(

        sd_ss_pref_reas_e_type    pref_reason,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type    orig_mode,
            /**< New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /**< New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type    band_pref,
            /**< New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_roam_pref_e_type    roam_pref,
            /**< New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type    hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type            otasp_band,
            /**< The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type          otasp_blksys,
            /**< The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        sd_ss_avoid_sys_e_type    avoid_type,
            /**< Type of the system to be avoided, valid only if
            ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        dword                     avoid_time,
            /**< Time in seconds for which the system is to be avoided,
            ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_ss_pref2

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

  Note: This interface includes the GSM/WCDMA parameters as well.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type                sd_ss_ind_user_ss_pref2(

        sd_ss_pref_reas_e_type          pref_reason,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type          orig_mode,
            /**< New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type          mode_pref,
            /**< New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type          band_pref,
            /**< New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type           prl_pref,
            /**< New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type          roam_pref,
            /**< New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type          hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                  otasp_band,
            /**< The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type                otasp_blksys,
            /**< The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        sd_ss_avoid_sys_e_type              avoid_type,
            /**< Type of the system to be avoided, valid only if
            ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        dword                              avoid_time,
           /**< Time in seconds for which the system is to be avoided,
            ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
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
           /**< Preference for the technology version type.
           */

        sys_wlan_scan_pref_e_type           wlan_scan_pref,
           /**< WLAN scan preference.
           */

        sd_ss_act_s_type                    *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_ss_pref3

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type                    sd_ss_ind_user_ss_pref3(
            /*lint -esym(715,avoid_type) *//**< param not referenced */
        sd_ss_pref_reas_e_type              pref_reason,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type              orig_mode,
            /**< New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type              mode_pref,
            /**< New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type              band_pref,
            /**< New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /**< New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /**< New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                      otasp_band,
            /**< The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type                    otasp_blksys,
            /**< The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        sd_ss_avoid_sys_e_type              avoid_type,
            /**< Type of the system to be avoided, valid only if
            ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        dword                               avoid_time,
            /**< Time in seconds for which the system is to be avoided,
            ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
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

        sd_ss_tech_pref_e_type               tech_pref,
           /**< Technology preference settings.
           */

        sys_wlan_scan_pref_e_type            wlan_scan_pref,
           /**< WLAN scan preference.
           */

        sys_wlan_bss_e_type                  bss_type,
           /**< BSS type.
           */

        sd_ss_act_s_type                    *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_nam_sel

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  NAM selection or information.

@DEPENDENCIES
  sd_init() and must have already been called to initialize SD.

  sd_nam_sel() must have already been called to load the selected NAM's PRL
  from NV.

@RETURN VALUE
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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_nam_sel(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_pwr_down

@DESCRIPTION
  Inform SD that the user commanded the MS power-off, perform a reset or
  enter LPM.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_pwr_down(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

/** @}
**
*/

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/* <EJECT> */
/**===========================================================================
=============================================================================
====================== CDMA ACQUISITION SS-INDICATIONS ======================
=============================================================================
===========================================================================*/

/** @defgroup CAS CDMA Acquisition SS-Indications
*  @{
*/

/** Indications that are specific to CDMA pilot and sync channel acquisition,
** such as pilot acquisition failure or protocol mismatch. These indications
** can be received from the SS-Client during CDMA pilot and sync channel
** acquisition only; that is, before the SS-Client enters the CDMA idle
** state.
*/


/**===========================================================================

@FUNCTION sd_ss_ind_cdma_acq_failed_pilot

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to failure to acquire the pilot channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_pilot(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_acq_failed_sync

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to failure to successfully receive the sync channel message.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_sync(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_acq_prot_mis

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to protocol mismatch (i.e., MOB_P_REV < MIN_P_REV or MS does not
  support the Paging Channel data rate).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_prot_mis

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA idle handoff
  fails due to protocol mismatch (i.e., MOB_P_REV < MIN_P_REV or MS does not
  support the Paging Channel data rate).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_acq_schm

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when successfully receives
  the CDMA Sync Channel Message (SCHM) and the protocol revision is
  compatible.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_schm(

        word                      sid,
            /**< SID from SCHM.
            */

        word                      nid,
            /**< NID from SCHM.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_schm4

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
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_schm4 (

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
            /* Leap seconds since start of system time
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);


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
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_schm3 (

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
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_acq_failed_unlock_rf

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to unlock RF resources.

  This indication is called before CDMA protocol acquired the sync channel
  messages.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_unlock_rf(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

@FUNCTION sd_ss_ind_cdma_acq_ho_fail

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the handoff from
  HDR fails

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.


@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_PWR_SAVE -  Instructs the protocol to enter powersave

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_ho_fail(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

/** @}
**
*/


#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/* <EJECT> */
/**===========================================================================
=============================================================================
======================= CDMA OPERATION SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/** @defgroup COS CDMA Operation SS-Indications
*  @{
*/

/** Indications that are specific to CDMA operation, such as CDMA Redirection
** or a CDMA new system. These indications can be received from the SS-Client
** during CDMA operation only; that is, after the SS-Client enters the CDMA
** idle state.
*/

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_oh_info3

@DESCRIPTION
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

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

@SIDE EFFECTS
  None.

===========================================================================*/
 extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info3(

        sd_band_e_type            band,
            /**< Serving system's band class.
            */

        sd_chan_type              chan,
            /**< Serving system's CDMA channel.
            */

        word                      sid,
            /**< Serving system's SID.
            */

        word                      nid,
            /**< Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /**< Indicate whether a GSRDM is pending.
            */

        word                      mcc,
            /**< Serving system's MCC.
            */
        byte                      imsi_11_12,
            /**< Serving system's IMISI_11_12.
            */

        sys_srch_win_type         srch_win_n,
            /**< Neighbor set window search size.
            */

        sys_base_lat_type         base_lat,
            /**< Base station latitude.
            */

        sys_base_long_type        base_long,
            /**< Base station latitude.
            */

        sys_base_id_type          base_id,
            /**< Base station Id.
            */

        sd_ss_ovhd_status_e_type  ovhd_status,
           /**< Indicate if the overhead message is complete.
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_oh_info2

@DESCRIPTION
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

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info2(

        sd_band_e_type            band,
            /**< Serving system's band class.
            */

        sd_chan_type              chan,
            /**< Serving system's CDMA channel.
            */

        word                      sid,
            /**< Serving system's SID.
            */

        word                      nid,
            /**< Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /**< Indicate whether a GSRDM is pending.
            */

        word                      mcc,
            /**< Serving system's MCC.
            */
        byte                      imsi_11_12,
            /**< Serving system's IMISI_11_12.
            */

        sys_srch_win_type         srch_win_n,
            /**< Neighbor set window search size.
            */

        sys_base_lat_type         base_lat,
            /**< Base station latitude.
            */

        sys_base_long_type        base_long,
            /**< Base station latitude.
            */

        sys_base_id_type          base_id,
            /**< Base station Id.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_oh_info

@DESCRIPTION
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

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info(

        sd_band_e_type            band,
            /**< Serving system's band class.
            */

        sd_chan_type              chan,
            /**< Serving system's CDMA channel.
            */

        word                      sid,
            /**< Serving system's SID.
            */

        word                      nid,
            /**< Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /**< Indicate whether a GSRDM is pending.
            */
        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_rescan

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS gets a CDMA SPM
  with a RESCAN field that is set to 1.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_rescan(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_reg_rej

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA registration is
  rejected.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_reg_rej(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_sys_lost

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when an acquired CDMA
  system is lost.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_sys_lost(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_sys_lost2

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when an acquired CDMA
  system is lost and the system lost reason is provided.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_sys_lost2(

        sd_ss_sys_lost_e_type     sys_lost,
            /**< System lost reason
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_redir_cdma

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  CDMA redirection message that redirects MS to CDMA systems.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_redir_cdma(

        boolean                   is_rif,
            /**< RETURN_IF_FAILED. Indicates whether the MS is required to
            ** return to the system from which it is being redirected upon
            ** failure to obtain service using the redirection criteria
            ** specified by this function call.
            */

        sd_ss_redir_e_type        redir_type,
            /**< Redirection type - GSRDM, SRDM, NDSS.
            */

        sd_band_e_type            band_class,
            /**< Band class. Indicate the CDMA band class, as specified in
            ** TSB58-A, for which the MS is being directed.
            */

        word                      expected_sid,
            /**< Expected SID. If the MS is being redirected to a specific
            ** system, indicate the SID of that system; otherwise, should be
            ** set to SD_WILDCARD_SID.
            */

        word                      expected_nid,
            /**< Expected NID. If the MS is being redirected to a specific
            ** network, indicate the NID of that network; otherwise, should
            ** be set to SD_WILDCARD_NID.
            */

        const word                *chans_ptr,
            /**< Pointer to array of CDMA channels to which the MS is being
            ** redirected.
            */

        int                       num_chans,
            /**< Number of CDMA channels in the array that is pointed to by
            ** chans_ptr.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_redir_amps

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  CDMA redirection message that redirects MS to AMPS systems.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_redir_amps(


        boolean                   is_rif,
            /**< RETURN_IF_FAILED. Indicates whether the MS is required to
            ** return to the system from which it is being redirected upon
            ** failure to obtain service using the redirection criteria
            ** specified by this function call.
            */

        sd_ss_redir_e_type        redir_type,
            /**< Redirection type - GSRDM, SRDM, NDSS.
            */

        word                      expected_sid,
            /**< Expected SID. If the MS is being redirected to a specific
            ** system, indicate the SID of that system; otherwise, should be
            ** set to SD_WILDCARD_SID.
            */

        sd_ss_sys_ord_e_type      sys_ord,
            /**< System ordering. Indicate the order in which the mobile
            ** station is to attempt to obtain service on an analog system.
            */

        word                      max_redirect_delay,
            /**< Maximum delay upon redirection. Indicate the maximum delay
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
            /**< IGNORE_CDMA indicator. Indicates whether the MS is to ignore
            ** the CDMA Capability Message on the analog system to which it
            ** is being redirected.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_redir

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  redirection message containing one or more redirection recoreds that
  redirects MS to either AMPS or CDMA systems.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_redir (

        boolean                   is_rif,
            /**< RETURN_IF_FAILED. Indicates whether the MS is required to
            ** return to the system from which it is being redirected upon
            ** failure to obtain service using the redirection criteria
            ** specified by this function call.
            */

        sd_ss_redir_e_type        redir_type,
            /**< Redirection type - GSRDM, SRDM, NDSS.
            */

        const sd_redir_rec_s_type *rec_list_ptr,
            /**< Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec,
            /**< number of records in the redirection record list.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_acpt_rej

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the SS-Clients is
  done with accepting/rejecting a CDMA redirection.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_acpt_rej(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_ndss_off

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS receives a
  CDMA NDSS off message.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_ndss_off(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_new_sys

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when performing a CDMA
  idle-handoff to a BS with unknown configuration.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_new_sys(

        sd_band_e_type            band,
            /**< The band class of the new system.
            */

        sd_chan_type              chan,
            /**< The CDMA channel of the new system.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_sys_resel

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS detects that CDMA
  pilot power is below a specified threshold (switch to AMPS).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_sys_resel(

        const sd_sys_s_type       *sys_ptr,
            /**< Pointer to array of systems from which to re-select.
            */

        int                       sys_num,
            /**< Number of systems in the array that is pointed to by sys_ptr.
            */


        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_lock

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  lock order.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_lock(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_unlock

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  unlock order.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_unlock(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_acc_term

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA access attempt
  terminates due to access denied, system lost, access state timeout, max
  access probes, reorder, intercept, BS release, MS release or normal access
  completion.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_acc_term(

        sd_ss_acc_reas_e_type     acc_reas,
            /**< Access termination reason.
            */

        sd_ss_acc_term_e_type     acc_term,
            /**< Access termination reason.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_cam_err

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment that is not supported by the MS.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_err(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_cam_cdma

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment to a CDMA paging/traffic channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed channel assignment.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_cdma(

        sd_ss_ho_e_type           ho_type,
            /**< The handoff type the MS needs to perform as part of this
            ** channel assignment:
            **
            **   SD_SS_HO_CDMA_CAM_TO_CDMA_PAG
            **   SD_SS_HO_CDMA_CAM_TO_CDMA_TRF
            */

        sd_band_e_type            band,
            /**< Assigned CDMA band class.
            */

        sd_chan_type              chan,
            /**< Assigned CDMA traffic channel number.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_cam_amps

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment to an AMPS system/voice channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed channel assignment.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_amps(

        sd_ss_ho_e_type           ho_type,
            /**< The handoff type the MS needs to perform as part of this
            ** channel assignment:
            **
            **   SD_SS_HO_CDMA_CAM_TO_AMPS_SYS
            **   SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE
            */

        sd_cell_sys_e_type        cell_sys,
            /**< The assigned cellular system (A/B). Note that this parameter
            ** is ignored when the MS is being assigned an AMPS voice
            ** channel.
            */

        word                      voice_chan,
            /**< The assigned AMPS voice channel number. Note that this
            ** parameter is ignored when the MS is being assigned an AMPS
            ** system.
            */

        word                      sid,
            /**< SID from channel assignment. If no SID is supplied with
            ** channel assignment, must be set to SD_WILDCARD_SID.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_hdm_cdma

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  handoff direction to a CDMA traffic channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed handoff direction.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_hdm_cdma(

        sd_band_e_type            band,
            /**< The assigned CDMA band class.
            */

        word                      chan,
            /**< The Assigned CDMA traffic channel number.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_hdm_amps

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  handoff direction to an AMPS voice channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed handoff direction.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_hdm_amps(

        word                      voice_chan,
            /**< The assigned AMPS voice channel number.
            */

        word                      sid,
            /**< SID from handoff direction. If no SID is supplied with
            ** handoff direction message, must be set to SD_WILDCARD_SID.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_itspm

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA In
  Traffic System Parameters Message or Mobile Station Registered Message.
  with new SID/NID information.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_RELEASE  -  Reales call.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_itspm(

        word                      sid,
            /**< SID from ITSPM.
            */

        word                      nid,
            /**< NID from ITSPM.
            */
        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_release

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA call is
  released.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_release(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);
#endif /**< defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

/** @}
**
*/


#ifdef FEATURE_ACP
#error code not present
#endif /**< FEATURE_ACP */

/** @}
**
*/


#ifdef FEATURE_HDR
#error code not present
#endif


/**===========================================================================

@FUNCTION sd_ss_ind_hybr_1_user_ss_pref3

@DESCRIPTION
  Instructs the HDR SS-Client of the SS-Action to take when the user changes
  the System Selection Preference (SS-Preference) and/or origination mode.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_1_user_ss_pref3(

        sd_ss_pref_reas_e_type    pref_reason,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type    orig_mode,
            /**< New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /**< New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type    band_pref,
            /**< New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type     prl_pref,
            /**< New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type    roam_pref,
            /**< New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type    hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type            otasp_band,
            /**< The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type          otasp_blksys,
            /**< The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        const sys_manual_sys_info_s_type
                                  *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type   domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type         tech_pref,
           /**< Technology preference settings.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref,
           /**< WLAN scan preference.
           */

        sys_wlan_bss_e_type            bss_type_pref,
           /**< BSS type.
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


#ifdef FEATURE_HDR
#error code not present
#endif /**< FEATURE_HDR */




#if (defined(FEATURE_HDR) || defined(FEATURE_TECH_IND_SD_STACK_ARCH))

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_user_pwr_save_exit

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to exit power save mode of operation.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_PWR_SAVE - Nothing to acquire, enter pwr_save

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_1_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/**< To maintain backward compatibility
*/
#define sd_ss_ind_hdr_user_pwr_save_exit(x) sd_ss_ind_hybr_1_user_pwr_save_exit(x)


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hybr_1_misc_pwr_save_enter

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_1_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/**< To maintain backward compatibility
*/
#define sd_ss_ind_hdr_misc_pwr_save_enter( x ) sd_ss_ind_hybr_1_misc_pwr_save_enter( x )

#endif

/** @}
**
*/

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /**< #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

/** @}
**
*/

/* <EJECT> */
/**===========================================================================
=============================================================================
============================ WLAN USER INDICATIONS ===========================
=============================================================================
===========================================================================*/

/** @defgroup WUI WLAN User Indications
*  @{
*/

/**===========================================================================

@FUNCTION sd_ss_ind_wlan_user_ss_pref

@DESCRIPTION
  Instructs the WLAN SS-Client of the SS-Action to take when the user changes
  the System Selection Preference (SS-Preference) and/or origination mode.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_ACQ_WLAN -  Acquire a WLAN system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type                    sd_ss_ind_wlan_user_ss_pref(

        sd_ss_pref_reas_e_type              pref_reason,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type              orig_mode,
            /**< New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type              mode_pref,
            /**< New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type              band_pref,
            /**< New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /**< New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /**< New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                      otasp_band,
            /**< The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type                    otasp_blksys,
            /**< The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
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
           /**< Technology preference settings.
           */

        sys_wlan_scan_pref_e_type           wlan_scan_pref,
           /**< WLAN scan preference.
           */

        sd_ss_act_s_type                    *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/**===========================================================================

@FUNCTION sd_ss_ind_wlan_user_ss_pref2

@DESCRIPTION
  Instructs the WLAN SS-Client of the SS-Action to take when the user changes
  the System Selection Preference (SS-Preference) and/or origination mode.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

    SD_SS_ACT_ACQ_WLAN -  Acquire an WLAN system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type                    sd_ss_ind_wlan_user_ss_pref2(
        /**<lint -esym(715, otasp_band) */ /**< param not referenced */
        /**<lint -esym(715, otasp_blksys) */ /**< param not referenced */

        sd_ss_pref_reas_e_type              pref_reason,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type              orig_mode,
            /**< New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type              mode_pref,
            /**< New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type              band_pref,
            /**< New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /**< New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /**< New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                      otasp_band,
            /**< The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type                    otasp_blksys,
            /**< The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
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
           /**< Technology preference settings.
           */

        sys_wlan_scan_pref_e_type           wlan_scan_pref,
           /**< WLAN scan preference.
           */

        sys_wlan_bss_e_type                 bss_type,
           /**< BSS type.
           */

        sd_ss_act_s_type                    *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_user_prot_deactivate

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user informs that
  protocol must be deactivated.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_user_prot_deactivate(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_user_get_networks_wlan

@DESCRIPTION
  Inform SD that the user commanded to get the available networks on WLAN.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_SURVEY_WLAN - Get the available WLAN systems.

    SD_SS_ACT_CONTINUE   - Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_user_get_networks(

        sd_ss_band_pref_e_type    band_pref,
           /**< Band preference to use while obtaining the available GSM/WCDMA
           ** networks.
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_user_term_get_networks_wlan

@DESCRIPTION
  Inform SD that the user commanded to terminate the previous get networks
  command.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_SURVEY_WLAN -  Survey WLAN networks.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_user_term_get_networks(


        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/**===========================================================================

@FUNCTION sd_ss_ind_wlan_user_wps_start

@DESCRIPTION
  Inform SD that the user commanded to get the available networks on WLAN.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_SURVEY_WLAN - Get the available WLAN systems.

    SD_SS_ACT_CONTINUE   - Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_user_wps_start(

        sys_wlan_wps_start_param_s_type    wps_start_param,
            /**< Parameters to use for WPS procedure.
            */


        sd_ss_act_s_type                   *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_user_wps_abort

@DESCRIPTION
  Inform SD that the user commanded to terminate the previous get networks
  command.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_SURVEY_WLAN -  Survey WLAN networks.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_user_wps_abort(


        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_user_pwr_save_exit

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to exit power save mode of operation.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_WLAN -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_PWR_SAVE - Nothing to acquire, enter pwr_save

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/** @}
**
*/

/* <EJECT> */
/**===========================================================================
=============================================================================
============================ WLAN MISC INDICATIONS ===========================
=============================================================================
===========================================================================*/

/** @defgroup WMI WLAN MISC Indications
*  @{
*/

/**===========================================================================

@FUNCTION sd_ss_ind_wlan_misc_timer

@DESCRIPTION
  Instructs the HYBR_WLAN client of the SS-Action to take when the system
  selection timer expires.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_WLAN -  Attempt acquiring an WLAN system as specified by
                          the "WLAN acquisition" parameters

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_misc_timer(

        boolean                   resel_is_allowed,
            /**< Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_misc_survey_rpt

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the Lower layers
  report the results of previous WLAN survey request.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type               sd_ss_ind_wlan_misc_survey_rpt
(

        sys_wlan_bss_info_list_s_type  *bss_list_ptr,
            /**< Pointer to a structure containing the list of available WLAN
            ** networks.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /**< FEATURE_HTORPC_METACOMMENTS */

/* ^L<EJECT> */

/**===========================================================================

@FUNCTION sd_ss_ind_wlan_misc_wps_rpt

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the Lower layers
  report the results of previous WLAN survey request.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type               sd_ss_ind_wlan_misc_wps_rpt
(

        sys_wlan_wps_result_s_type    *wps_cred_list_ptr,
            /* Pointer to a structure containing the result of the WPS
            ** procedure.
            */

        sd_ss_act_s_type              *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /**< FEATURE_HTORPC_METACOMMENTS */



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_misc_pwr_save_enter

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation for HYBR_WLAN instance.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_WLAN -   Attempt acquiring a WLAN system as specified by
                          the "WLAN acquisition" parameters

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /**< FEATURE_HTORPC_METACOMMENTS */


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_misc_meas_rprt

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the channel
  strength measurement that was requested by SD is completed and reported
  back to SD.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_misc_meas_rprt(

        word                      meas_id,
            /**< The measurement ID. This IS should match the ID that was
            ** provided to the SS-Client with the measurement request (i.e.,
            ** SD_SS_ACT_MEAS_BACK or SD_SS_ACT_MEAS_DED) SS-Action.
            */

        boolean                   resel_is_allowed,
            /**< Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /* FEATURE_HTORPC_METACOMMENTS */

/** @}
**
*/

/* ^L<EJECT> */
/**===========================================================================
=============================================================================
======================== WLAN ACQUISITION SS-INDICATIONS ====================
=============================================================================
===========================================================================*/

/** @defgroup WAS WLAN Acquisition SS-Indications
*  @{
*/

/** Indications that are specific to WLAN operation. These indications
** can be received from the SS-Client during WLAN operation only; that
** is, after the SS-Client enters the WLAN active state.
*/

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_acq_scan_failed

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when WLAN acquisition
  scan failure.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_ACQ_WLAN  - Acquire a WLAN system as specified by the returned
                          "WLAN acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_acq_scan_failed
(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /* FEATURE_HTORPC_METACOMMENTS */

/**===========================================================================

@FUNCTION sd_ss_ind_wlan_acq_scan_failed2

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when WLAN acquisition
  scan failure.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_ACQ_WLAN  - Acquire a WLAN system as specified by the returned
                          "WLAN acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_acq_scan_failed2
(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */

        sd_ss_wlan_fail_reas_e_type fail_reason
            /* Cause of failure
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_acq_attach_failed

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when WLAN acquisition
  fails during attach( association/authentication ) procedure.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_ACQ_WLAN -  Acquire a WLAN system as specified by the returned
                          "WLAN acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.



@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_acq_attach_failed
(

        const sys_wlan_acq_fail_list_info_s_type
                                  *fail_info_ptr,
            /**< Pointer to failure info.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_acq_attach_ok

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when WLAN acquisition
  succeeds.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type                sd_ss_ind_wlan_acq_attach_ok
(

        sys_wlan_bssid_type             bssid,
            /**< BSS id with which MS associated.
            */

        const sys_wlan_ssid_s_type      *ssid_ptr,
            /**< SS id with which MS is associated.
            */

        sys_wlan_bss_e_type             bss_type,
            /**< BSS type.
            */

        sys_chan_type                   chan,
            /**< Channel on which BSS is present.
            */

        sys_tech_e_type                 tech,
            /**< WLAN technology type 802.11A, B or G.
            */

        sd_ss_act_s_type                *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* ^L<EJECT> */
/**===========================================================================
=============================================================================
======================== WLAN OPERATION SS-INDICATIONS ====================
=============================================================================
===========================================================================*/

/**< Indications that are specific to WLAN operation. These indications
** can be received from the SS-Client during WLAN operation only; that
** is, after the SS-Client enters the WLAN active state.
*/

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_opr_ss_lost


@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when WLAN service is lost
  with the SSID provided in ACQ_WLAN parameters.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_opr_ss_lost
(

       /**< Add reason JAI.
        sys_wlan_acq_fail_list_info_s_type
                                  *fail_info_ptr,
       */

       sd_ss_act_s_type          *rtrn_act_ptr
           /**< Pointer to a buffer where to place returned SS-Action.
           */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /* FEATURE_HTORPC_METACOMMENTS */


/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_opr_sys_inactive

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when WLAN system is
  inactive.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

    SD_SS_ACT_CONTINUE -  Continue

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_opr_sys_inactive
(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_opr_bss_dissociated


@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when WLAN service is lost
  because the BSS dissociated the MS and there are no other BSS with the same
  SSID.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_opr_bss_dissociated
(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_bad_sys

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when WLAN system is bad.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_WLAN -  Acquire a WLAN system as specified by the returned
                          "WLAN acquisition" parameters.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

    SD_SS_ACT_CONTINUE -  Continue

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wlan_bad_sys(

        sd_ss_wlan_bad_sys_e_type reas,
            /**< Bad system due to 'reas'.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_wlan_opr_attach_ok

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when WLAN service
  parameters changes due to handoff to different BSS etc.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type                sd_ss_ind_wlan_opr_attach_ok
(

        sys_wlan_bssid_type             bssid,
            /**< BSS id with which MS associated.
            */

        const sys_wlan_ssid_s_type      *ssid_ptr,
            /**< SS id with which MS is associated.
            */

        sys_wlan_bss_e_type             bss_type,
            /**< BSS type.
            */

        sys_chan_type                   chan,
            /**< Channel on which BSS is present.
            */

        sys_tech_e_type                 tech,
            /**< WLAN technology type 802.11A, B or G.
            */

        sd_ss_act_s_type                *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
#ifdef FEATURE_WLAN
#error code not present
#endif
#endif /* FEATURE_HTORPC_METACOMMENTS */

/** @}
**
*/

/* <EJECT> */
/**===========================================================================
=============================================================================
======================== MISCELLANEOUS SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/** @defgroup MS Miscellaneous SS-Indications
*  @{
*/

/** Indications that are not USER, CDMA or AMPS related. These indications are
** usually mode independent; or in other words, they can be received in more
** than one of the CP modes (e.g. CDMA, AMPS or PWR_SAVE).
*/



/**===========================================================================

@FUNCTION sd_ss_ind_misc_timer

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the system selection
  timer expires.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_timer(

        boolean                   resel_is_allowed,
            /**< Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/**===========================================================================

@FUNCTION sd_ss_ind_hybr_1_misc_timer

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the system selection
  timer expires.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_1_misc_timer(

        boolean                   resel_is_allowed,
            /**< Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

#define sd_ss_ind_hdr_misc_timer(x, y) sd_ss_ind_hybr_1_misc_timer(x, y)

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_misc_meas_rprt

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the channel
  strength measurement that was requested by SD is completed and reported
  back to SD.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_meas_rprt(

        word                      meas_id,
            /**< The measurement ID. This IS should match the ID that was
            ** provided to the SS-Client with the measurement request (i.e.,
            ** SD_SS_ACT_MEAS_BACK or SD_SS_ACT_MEAS_DED) SS-Action.
            */

        boolean                   resel_is_allowed,
            /**< Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_misc_orig_success

@DESCRIPTION
  Inform SD of origination success.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_orig_success(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_misc_pwr_save_enter

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_misc_cnf

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS confirms
  the last instructed action like pwr_scan_gw

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
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

    SD_SS_ACT_PWR_SCAN_GW - Request to early power scan in GSM/WCDMA before
                            actual acquisition.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_cnf(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/** @}
**
*/

/* <EJECT> */
/**===========================================================================
=============================================================================
=============================================================================
============================= MISCELLANEOUS SERVICE =========================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup MS Miscellaneous Service
*  @{
*/

/**===========================================================================

@FUNCTION sd_misc_prl_ver_get

@DESCRIPTION
  Returns the version number of the PRL that is stored in NV and associated
  with the currently selected NAM.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  Version number of the PRL that is stored in NV and associated with the
  currently selected NAM. PRL_DEFAULT_VER if no PRL is stored in NV or PRL
  has no version number associated with it.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  word                      sd_misc_prl_ver_get( void );




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_target_supp_mode_band

  Inform the caller to whether a specified combination of mode and band
  preference is supported by the current target.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_target_supp_mode_band(

        sd_ss_mode_pref_e_type    mode_pref,
            /**< Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref
            /**< Band preference to be validated.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_target_supp_mode_band_chan

  Inform the caller to whether a specified combination of mode  mode , band 
  class and channel number is supported by the current target.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if the specified combination of mode , band class and channel number
  is supported by the current target. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean  sd_misc_is_target_supp_mode_band_chan(

        sd_mode_e_type            mode,
            /* Mode to be validated.
            */

        sd_band_e_type            band,
            /* Band to be validated.
            */

        sys_channel_num_type      chan
            /* Channel number to be validated.
            */
);


/* <EJECT> */
/*===========================================================================

@FUNCTION sd_misc_get_mode_band_capability

  Inform the called the modes and bands preference supported by the target.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  None

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                   sd_misc_get_mode_band_capability(

        sd_ss_mode_pref_e_type    *mode_pref_ptr,
            /**< Mode preferences to be filled in.
            */

        sd_ss_band_pref_e_type    *band_pref_ptr
            /**< Band preferences to be filled in.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_target_supp_mode_band_class

  Inform the caller to whether a specified combination of mode and band
  is supported by the current target.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if the specified combination of mode and band is supported by the
  current target. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_target_supp_mode_band_class(

        sd_ss_mode_pref_e_type    mode_pref,
            /**< Mode preference to be validated.
            */

        sd_band_e_type            band
            /**< Band to be validated.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_supp_pref

  Inform the caller to whether a specified combination of mode and band
  preference is supported by the current target, as well as, the PRL that is
  associated with the currently selected NAM.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable and a PRL that contains AMPS acquisition records.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.
  sd_nam_sel() must already been called to load the PRL.

@RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target, as well as, the PRL that is associated with the
  currently selected NAM. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_supp_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /**< Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref
            /**< Band preference to be validated.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_cdma_locked

@DESCRIPTION
  Return the value of the CDMA locked indicator.

@DEPENDENCIES
  None.

@RETURN VALUE
  TRUE if the CDMA locked indicator is set to TRUE. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_cdma_locked( void );



/**===========================================================================

@FUNCTION sd_misc_is_cdma_pseudo_online

  Inform the caller if CDMA is in pseudo online state

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if CDMA was activated with pseudo online. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_cdma_pseudo_online( void );



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_get_cdma_request_mode

@DESCRIPTION
  Get the CDMA request_mode to be used in CDMA origination and page-response
  messages.

  The CDMA request mode is determined according to the current system
  selection preferences.

@DEPENDENCIES
  None.

@RETURN VALUE
  CDMA request_mode as defined in ANSI J-STD-008 Table 2.7.1.3.2.4-1 and
  cai.h.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  byte                      sd_misc_get_cdma_request_mode( void );




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_get_cdma_return_cause

@DESCRIPTION
  Get the CDMA return_cause to be used in CDMA registration and origination
  messages.

@DEPENDENCIES
  None.

@RETURN VALUE
  CDMA return_cause as defined in IS-2000-5 Table 2.7.1.3.2.1-2 and cai.h.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  byte                      sd_misc_get_cdma_return_cause( void );



/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_get_curr_acq_sys_mode

@DESCRIPTION
   Returns the mode of system over which service is currently provided. If
   no service is currently provided, return the mode of the system that
   is being attempted acquisition.

@DEPENDENCIES
  None.

@RETURN VALUE
  Current mode acquired or attempting to be acquired.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_sys_mode_e_type            sd_misc_get_curr_acq_sys_mode( void );


/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_mode_pref

@DESCRIPTION
  Check whether a specified mode_pref2 complies with a specified mode_pref1.

@DEPENDENCIES
  None.

@RETURN VALUE
  TRUE if mode_pref2 complies with mode_pref1
  otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /**< Specified mode_Pref1
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /**< Mode preference against which to check the compliance.
            */
);

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_remove_mode_pref

@DESCRIPTION
  Remove mode_pref2 from mode_pref1.

@DEPENDENCIES
  None.

@RETURN VALUE
  Resultant mode_pref.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_mode_pref_e_type   sd_misc_remove_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /**< Specified mode_Pref1.
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /**< Specified mode_pref2.
            */
);

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_add_mode_pref

@DESCRIPTION
  Add mode_pref2 with mode_pref1.

@DEPENDENCIES
  None.

@RETURN VALUE
  Resultant mode_pref.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_mode_pref_e_type   sd_misc_add_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /**< Specified mode_Pref1.
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /**< Specified mode_pref2.
            */
);



/**===========================================================================

@FUNCTION sd_misc_get_common_mode_pref

@DESCRIPTION
  Gets the common mode preference from mode_pref1 and mode_pref2.

@DEPENDENCIES
  None.

@RETURN VALUE
  Resultant mode_pref.

@SIDE EFFECTS
  None.

===========================================================================*/
extern sd_ss_mode_pref_e_type    sd_misc_get_common_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /**< Specified mode_Pref1.
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /**< Specified mode_pref2.
            */
);


/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_hybr_opr

@DESCRIPTION
  function to query if we are in hybrid operation, based on SS-HDR instance.

@DEPENDENCIES
  None.

@RETURN VALUE
  boolean
    TRUE - hybrid operation on,
    FALSE - otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_hybr_opr(

        void
);

/**===========================================================================

@FUNCTION sd_misc_is_handoff

@DESCRIPTION
  Function to query if the AT is in Handoff.

@DEPENDENCIES
  None.

@RETURN VALUE
  boolean
    TRUE - SD is in ho_type handoff,
    FALSE - otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                  sd_misc_is_handoff(

        sd_ss_ho_e_type          ho_type
);

/**===========================================================================

@FUNCTION sd_misc_get_ss_for_mode

@DESCRIPTION
  Function to get the SS for the mode, to be used internally by CM and MMOC
  Only

@DEPENDENCIES
  None.

@RETURN VALUE
 ss for the mode
 SD_SS_MAX if invalid call

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_e_type             sd_misc_get_ss_for_mode(

        sys_sys_mode_e_type      sys_mode
);



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_filter_out_overlap_band

@DESCRIPTION
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

@DEPENDENCIES
  None

@RETURN VALUE
  band preference enum type

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sd_misc_filter_out_overlap_band(

        sd_ss_band_pref_e_type    band_pref
            /**< Enumeration of system selection band and sub-band preferences.*/
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_get_band_mode

@DESCRIPTION
  Filter out the band and mode not supported by the target capability.
  Filter out the band and mode that are not in part of system preference.
  Filter out any overlapped bands.
  Bands that do not have corresponding modes.
  Modes that do not have corresponding bands.

  MODE
@DEPENDENCIES
  None

@RETURN VALUE
  Return the filtered mode and band preference as described.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_misc_get_band_mode(

        sd_ss_mode_pref_e_type    *mode_ptr,
            /**< Pointer to the mode preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr,
            /**< Pointer to the band preference that needs to be filtered.
            */

        sd_ss_mode_pref_e_type    sys_mode_pref,
            /**< Enumeration of system mode perference.
            */

        sd_ss_band_pref_e_type    sys_band_pref
            /**< Enumeration of system selection band and sub-band preferences.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_get_supp_band_pref

@DESCRIPTION

  Return the band preference that is based on the target capability and user
  preference.

  MODE
@DEPENDENCIES
  None

@RETURN VALUE
  Return the filtered band preference as described.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sd_misc_get_supp_band_pref(void);




/**===========================================================================

@FUNCTION sd_misc_is_sys_same

@DESCRIPTION

Checks if both the sys are same

@DEPENDENCIES
  None.

@RETURN VALUE
  TRUE if sys_2 is equal to sys_1
  otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_sys_same(

        sd_sys_s_type    sys_1,
            /**< Specified sys_1
            */

        sd_sys_s_type    sys_2
            /**< Specified sys_2
            */
);




/**===========================================================================

@FUNCTION sd_misc_sys_id_print

@DESCRIPTION
  This function prints the sys_id

@DEPENDENCIES
  None

@RETURN VALUE
  None

@SIDE EFFECTS
  None

===========================================================================*/
extern  void                      sd_misc_sys_id_print(

        sys_sys_id_s_type     sys_id
            /**< Sys Id
            */

);




/**===========================================================================

@FUNCTION sd_misc_is_del_tmsi

@DESCRIPTION
  This function indicates that whether tmsi should be delete or not.

@DEPENDENCIES
  None

@RETURN VALUE
  None

@SIDE EFFECTS
  None

===========================================================================*/
extern  boolean                      sd_misc_is_del_tmsi(void);


/*===========================================================================

@FUNCTION sd_misc_is_fts_mode

@DESCRIPTION
  Return the an boolean indication whether the system is in Full-time SHDR 
  mode or not, based on the target capability and user preference.
  
@DEPENDENCIES
  None

@RETURN VALUE
  TRUE: If the phone is in FTS Mode.
  FALSE: If the phone is not in FTS Mode.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean    sd_misc_is_fts_mode(void);


#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#endif


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_orig_mode

@DESCRIPTION
  Check whether a specified origination mode value includes the current
  origination mode.

@DEPENDENCIES
  None.

@RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sd_misc_is_orig_mode(

        sd_ss_e_type                  ss,
            /**< System selection - MAIN or HDR.
            */

        sd_ss_orig_mode_val_e_type    orig_mode_val
            /**< Origination mode value.
            */
);

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
extern  void                       sd_misc_sys_avoid_list(

        sd_mode_e_type               mode,   
            /**< Origination mode value.
            */

        sys_avoided_channel_record_type     *avoid_list_ptr,
            /**< Pointer to avoided channels
            */
        
        byte                        *num_sys,

        byte                         max_entry
);
/** @}
**
*/

/* <EJECT> */
/**===========================================================================
=============================================================================
=============================================================================
================================= GENERAL ===================================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup G General
*  @{
*/

/**===========================================================================

@FUNCTION sd_init_before_app_task_start

@DESCRIPTION
  Perform SD initialization that needs to be done before application tasks
  have started and NV task has been started.

  NOTE! Any initialization that must be done before SD clients tries to
  contact SD, must be done in this function, as opposed to sd_init().

  NOTE! Any initialization that result in SD contacting some other tasks
  must be done in sd_init(), as opposed to this function.

@DEPENDENCIES
  This function must be called before any other SD function is ever called.
  This function shall not be called a second time prior to calling on
  sd_term().

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_init_before_app_task_start( void );




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_init

@DESCRIPTION
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

@DEPENDENCIES
  This function must be called after sd_init_before_task_start() and
  before any other SD function is ever called. This function shall not be
  called a second time prior to calling on sd_term().

@RETURN VALUE
  TRUE if SD initialization is successful (i.e. SD NV items are successfully
  read). FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_init(

        rex_sigs_type             nv_sig,
            /**< Rex signal to be used by SD for waiting on NV to completes a
            ** read/write operation.
            */

        rex_sigs_type             dog_sig,
            /**< Rex signal to be used by SD for watchdog reporting while SD
            ** waits on NV to complete a read/write operation.
            */

        void                      (*dog_rpt_func_ptr) (void),
            /**< Pointer to a watchdog reporting function to be called by SD
            ** when the watchdog sig is set while SD waits on NV to complete
            ** a read/write operation.
            */

        void                      (*timer_func_ptr) (int4  ms),
            /**< Pointer to an SS-Timer callback function.
            **
            ** SD calls on this function to set/disable the SS-Timer. Upon
            ** SS-Timer expiration the SS-Client should call on the
            ** sd_ss_ind_misc_timer() SS-Indication function.
            **
            ** The argument to this function is the time in ms with which
            ** to set the SS-Timer. An input of 0 should disable the
            ** SS-Timer */


        void                      (*hdr_timer_func_ptr) (int4  ms),
            /**< Pointer to an HDR SS-Timer callback function for CDMA/HDR
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of CDMA/HDR hybrid operation, this
            ** parameter can be set to NULL */


        void                      (*wlan_timer_func_ptr) (int4  ms)
            /**< Pointer to an SS-HYBR_WLAN SS-Timer callback function for
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of any+WLAN hybrid operation, this
            ** parameter can be set to NULL */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_nam_sel

@DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                             sd_nam_sel(

        sd_nam_e_type                       curr_nam,
            /**< Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type              orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type              mode_pref,
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
           /**< Technology preference setting.
           */

        sys_wlan_scan_pref_e_type           wlan_scan_pref,
           /**< WLAN scan preference.
           */

        boolean                             is_gw_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

        boolean                             is_wlan_subsc_avail
           /**< Indicates if subscription is available for providing service on
           ** WLAN network.
           */

);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_nam_sel2

@DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                          sd_nam_sel2(

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
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_wlan_nam_sel

@DESCRIPTION
  Inform SD of a new WLAN preferences as per the new NAM selection

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  None

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sd_wlan_nam_sel(

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
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_term

@DESCRIPTION
  Terminate SD.

  NOTE! This function must be called when powering-off the phone. After
  calling this function no other public SD function shall be called prior to
  calling sd_init().

@DEPENDENCIES
  This function must be called when powering-off the phone. After calling
  this function no other public SD function shall be called prior to
  calling sd_init().

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_term( void );

/**===========================================================================

@FUNCTION sdtask_crit_sect_ptr

@DESCRIPTION

  Return pointer to CM critical section.

@DEPENDENCIES
  none

@RETURN VALUE
  none

@SIDE EFFECTS
  none

===========================================================================*/
extern rex_crit_sect_type* sdtask_crit_sect_ptr(void);


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
extern sd_ss_mode_pref_e_type    sd_ss_get_supp_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference from which to extract supported modes.
            */

        sd_ss_band_pref_e_type    band_pref
            /* band preference from which to extract supported modes.
            */
);

#endif /**< SD_H */


/** @}
**
*/

