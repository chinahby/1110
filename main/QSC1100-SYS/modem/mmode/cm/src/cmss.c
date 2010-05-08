/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     C A L L   M A N A G E R   S E R V I N G   S Y S T E M   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Serving System Object.

  The Serving System Object is responsible for:
  1. Processing MC serving system related notifications.
  2. Notifying the client list of serving system events.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmss_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1998 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmss.c#18 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/09   aj      Adding FEATURE_C2K_TIME_INFO
05/27/09   aj      Adding support for updating clients with cdma local time
05/20/09   ks      Adding support for camped cell info requirement
04/23/09   rm      Updating correct mode pref of phone when data_avaialable
                   request comes. Added cmph_get_srv_type.
04/20/09   ks      Avoid CM SS event sent to client when UE manually select
                   same plmn as acq before in automatic mode
04/09/09   aj      Added support for IRAT measurements 
03/31/09   mh      Added support for prl match indicator in ss info
03/17/09   aj      UMA GAN related changes
03/13/09   sg      Remove feature flag FEATURE_EGPRS_IND from CM code
02/13/09   mh      Added support for default roaming indicator in ss info
02/04/09   fj      Cast ss_ptr->daylt_savings into cm_daylt_savings_e_type
01/13/09   ks      Add spare_bits field in the mm_information to indicate 
                   no. of spare bits in end octet of n/w names(full/short)
12/28/08   np      IMSI_MCC converting from OTA - Binary to PRL decimal format 
12/25/08   rk      Fixing data resume issue in multimode targets.
12/08/08   sv      Fixing featurization FEATURE_EGPRS_IND
11/21/08   ks      Fixing compiler warnings
11/12/08   np      Convert OTA MCC to PRL MCC for HPCD  
11/12/08   ks      Reset cell_srv_ind values once UE moves to No Service or inter-rat 
                   happen W->G and G->W to avoid unnecessary event send to clients.
11/12/08   ks      Fixing Compilation error
11/11/08   ks      Resolve Klocwork defects
11/10/08   jd      Removed dead code in MC report processing
11/03/08   pk/ak   Updated the HICPS for RIM issues.
10/28/08   sv      Correct Compilation Warnings
10/27/08   rk      On data avail request pump the priority of all data calls 
                   over to phone object.
10/21/08   ks      Update default threshold value for sir and pathloss and
                   add masks for RSSI event
10/06/08   np      Added HPCD Rev B
10/01/08   sv      Corrected featurization for cmss_misc_is_sys_id_equal
                   to remove compiler warning.
09/27/08   cl      Support active channels for QMI
09/25/08   cl      1X icon disappear when enter & exit LPM or FTM
09/17/08   ks      When OOS in CELL_FACH, UE needs to follow UOOS scanning 
                   pattern to re-acquire service
09/16/08   pk      Added support for WLAN UOoS
09/05/08   sv      Remove compiler warnings.
09/05/08   sv      Memory Reduction for ULC
09/04/08   ks      Remove info for Neighbors Cell IDs
08/25/08   ak      Fixed the general compiler warnings.
08/24/08   rn      Send regn reject info to clients
08/20/08   st      Klockwork Fixes
07/30/08   ak      Fix for the HICPS scenario.
07/18/08   ks      Added new info for Current Operator's Cell ID and 
                   Neighbors Cell IDs
06/12/08   sv      Resolve Klocwork defects
06/11/08   sv      Exposing cmss_send_ss_info for power collapse buffered
                   events notifications.
05/27/08   sv      Updated report_rssi to send rssi info to individual clients
05/01/08   sv      Added CMSS_ECIO_VALUE_MAX to limit cdma ecio
04/14/08   rm      Update call_ptr->sys_mode when the phone's 
                   mode/service_status changes
03/26/08   vs      Updated the list of Call end status handled in
                   call throttling for DCTM 3.0.
03/14/08   ks      Updating UI with service status when UE is OOS in Connected Mode
03/13/08   sk      Fix to allow CDMA scans during dormant GW PS data calls
                   without tearing down the PS data call.
03/10/08   rn      Added support for indicating Limited_regional to clients
03/05/08   vk      corrected code for HYBR_GW configuration
02/20/08   rn      Initialized sim_state in cmss_reset function
02/15/08   ks      Adding SIR, Pathloss Implementation
01/24/08   ks      Display of UE signal strength indication
01/24/08   ks      Fix the flashing issue when moving from No Service to 
                   Limited to Full Service in a short time frame.
01/03/08   ka      Adding handset based plus code dialing feature.
12/16/07   ks      Added cm_ph_cmd_signal_strength_delta
12/13/07   sk/ak   Added DCM.
12/07/07   ra      Added support for IMS VCC in HOMER files 
12/03/07   vs      Fixing CR#129599, Updating srv_avail_info and info with
                   mccreg_is_registered()
08/17/07   dm      Featurised to report HSPA call status indication
08/17/07   ka/dm   New field to report HS call in progress
08/12/07   jq      Fixed DCTM timeline issue under JCDMA feature.
08/07/07   ic      Lint clean up
06/08/07   cl      Fix CR#: 119770 - cmss_orig_thr_tbl_entry_info_put should 
                   pass parameter by pointer
06/06/07   rp      Adding support for EGPRS indication
06/06/07   pk/cl   Added support for LN status notification from HDR
05/15/07   rp      Adding support for ECNO for GW in cmss_report_rssi()
05/15/07   rp      Adding cmss_update_last_reported_sysmode()
03/30/07   jqi     Changed the DCTM timeline to 0,0,180 under JCDMA feature.
03/29/07   rp      Adjusting length of BCD number sent in enum_list
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/23/07   ic      Initialize SS info buffer in cmss_info_alloc() before use
11/20/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/17/06   jqi     Added support for HDR active protocol revision information.
11/16/06   pk      Lint Fixes
11/11/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
11/07/06   sk      Memory reduction effort.
10/11/06   jqi     Added CDMA 450M(BC5) block L support.
08/16/06   ic      Lint fixes
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
04/17/06   ka      Adding support for HSDPA and DTM indications.
04/10/06   pk      Lint fixes
04/06/06   sk      Changed the BCMCS supported and BCMCS service (new)
                   indication behaviour. BCMCS supported never delayed and
                   changes only upon explicit indication from HDR. BCMCS
                   service status is an "AND" of the BCMCS supported and HDR
                   service status and is delayed if it changes due to HDR
                   service status change.
04/01/06   ka      Changes for emerg serv category during orig, aborting 
                   manual search and returning current band with srv ind.                  
03/14/06   pk      Added support for high PSIST
03/13/06   sk      Set usable flag to UNKNOWN when real service lost on HDR.
03/10/06   pk      Added support for IMSI and MCC for IS 95 systems
02/17/06   ka      Adding support for videoshare
02/15/06   ic      Removed copy/paste errors from cmss_info_get()
01/24/06   pk      Modified cmss_update_hdr_srv_ind to report the correct
                   HDR mask.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/09/06   ic      Adjusted for cm_num_init() / cm_num_copy() changes in cm.h
01/06/06   ka      Adding Release 5 changes including emerg call list, 
                   traffic flow template for PDP modify and protocol config
                   options for some of the GW PS call reports.
01/06/06   ic      Lint cleanup  
                   Mainlined FEATURE_ENHANCED_BAND_CLASS
11/18/05   pk      Added support for BC 5 and BC 11 sub blocks I,J,K
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
11/01/05   pk      Added support for the SD_SYS_USABLE_NOT_OK indication from
                   SD
10/27/05   pk      Added support for SS events for DCTM
10/23/05   jqi     Hold ps data call orig or data avail request if there is 
                   a pending get network list request.
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
10/18/05   pk      Changed map_cm_hybr_pref_to_sd_hybr_pref to
                   cmph_map_cm_hybr_pref_to_sd_hybr_pref
09/23/05   ic      Do not start BCMCS timer when BCMCS service did not change
09/15/05   pk      Modified call to cmpref_proc_cmd_pref_sys_chgd in 
                   cmss_proc_data_avail, passing NULL for wlan_ptr
09/15/05   pk      Changed DCTM messages to CM_MSG_LOW
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/15/05   pk      Added support for priority Queues for origination 
                   parameters
08/11/05   pk      DCTM modifications, corrected the backoff time for the
                   case where the update is zero. Not adding randomizing 
                   factor if the update is zero
08/02/05   ic      Print value that caused error in default switch case
07/26/05   pk      DCTM modifications, changed the backoff time line from 
                   1,2,4,8 to 0,0,1,2,8,15. Ignoring NID for DCTM, a system 
                   is uniquely identified by SID,PZID.
07/11/05   pk      Removed 
                   cmss_read_sys_mode_from_db( &ss_ptr->info.sys_mode );
                   from the processing of CM_SRV_SYS_INFO_F report from MC.
07/11/05   pk      Modified cmss_misc_is_sys_id_equal(), to consider PZID's
06/14/05   pk      Modified cmss_update_hdr_srv_ind(). While in power save
                   hdr_no_srv_uptime is updated. This is the fix for CR 66297
06/06/05   pk      Added Data Throttle Manager API's and helper functions
06/02/05   ic      Mainlined FEATURE_CIPHER_IND and FEATURE_EXTENDED_PRL
05/18/05   ka      Merging CIPHER_IND feature.
05/20/05   ic      Mainlined FEATURE_IS683C_PRL
05/12/05   ic      Function name change update to cmbcmcs_srv_status_is_srv()
04/20/05   ka      Informing IP apps when there is a service change
03/17/05   pk      Modified cmss_report_rssi to check for ecio,io and sinr
                   while reporting change in rssi.
02/01/05   dk      Added WLAN Support.
0/17/05    dk      Print CMSS Event and Changed Field.
01/10/05   dk      Generate Service Indicator event for LAC change as well.
11/16/04   sj      Updated avoid_type mapping to SD types.
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/11/04   ic      Lint cleanup
10/25/04   ic      Fixed "is_cmd_done" was declared but never referenced RVCT/ADS
                   compiler warning
10/22/04   jqi     Removed redundent RSSI type from cmss_report_rssi().
10/20/04   sj      Include is_colloc in CM SS event.
10/19/04   jqi     Sent the data available request upon the ps call orig
                   when the data is suspended.
09/27/04   ic      Fixed RVCT 2.1 compiler warning "..\..\services\cm\cmss.c"
                   line 2167: Warning:  #177-D: variable "cm_band_class" was
                   declared but never referenced"
09/20/04   jqi     Added debug messages for ps dormant optimization.
09/13/04   ka      Fixed a condition where serving parameter changes that are
                   recvd during throttling do not get passed on to call
                   objects at the end of throttling. There was no way to pass
                   on changes in serving system parameters to call objects
                   after throttling got ended.
09/03/04   ka      Added service throttling for HDR. Both HDR and MAIN throttling
                   is stopped if EVENT_SRV_CHANGED is generated due to other
                   triggers like sim state updation or data suspend flag change.
09/08/04   jqi     More ps dormant optimization.
08/27/04   jqi     Added ps dormant optimization.
08/12/04   ws      Remove cm20 support.
08/10/04   ic      Removed cmlog_system_mode() as it became obsolete.
08/04/04   ic      CM Lint clean up - cmregprx.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/07/04   ka      Fixed Lint errors.
07/06/04   ka      Throttling EVENT_SRV_CHANGED sent to clients in STATUS_SRV
                   or STATUS_LIMITED. First transition to SERVICE reported immed.
                   Subseqent change in SERVICE params reported in 2 secs interv.
                   If no change detected in SERVICE params for 2 secs or
                   transitioned out of SERVICE, we move out of throttling mode.
07/05/04   dk      Fixed cmss_map_sd_mode_to_cm_mode() to report WCDMA/GSM mode
                   also.
07/02/04   sj      Fixed lint errors.
06/30/04   ic      Fixed obvious RVCT 2.1 compiler warnings (Jaguar build)
06/23/04   ic      Fixed ADS 120 compiler warnings (Saber build)
06/22/04   ic      Comments clean up
06/07/04   dk      Optimized CM not to inform its clients of SS NO Service every
                   one second if it is still in NO service state and CM has already
                   notified.
06/07/04   sj      Removed call to generate ph event standby sleep and wakeup.
05/27/04   ic      In cmss_client_cmd_proc() call cmss_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
05/20/04   ic      Removed unnecessary comment
05/18/04   ic      In cmss_send_idle_digital_mode_event() copied current and
                   previous IDM from CM SS IDM object into CM SS object
05/17/04   ic      Changes related to bcmcs_srv_supported field being removed
                   from sd_si_info_s_type (sd.h)
05/14/04   ka      Fixed Lint errors.
04/23/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.28 and MSMSHARED_CM.01.00.10.00.31
                   Removed cmss_update_rssi() as it had been removed long ago
                   from Jaguar 4.X branch
                   Replaced calls to cmss_update_rssi() with calls to
                   cmss_report_rssi() so CM SS here behaves like on 4.X Jaguar
                   branch with respect to RSSI reporting.
04/22/04   ic      Defined CMSS_ECIO_VALUE_NO_SIGNAL / CMSS_IO_VALUE_NO_SIGNAL.
                   Used them to initialize ecio / hdr_ecio and io / hdr_io
                   fields in CM SS object.
04/19/04   ic      Added support for HDR Ec/Io & Io reporting.
                   HDR Ec/Io & Io will be reported with CM_SS_EVENT_RSSI &
                   CM_SS_EVENT_HDR_RSSI events.
03/23/04   ws      Merged code from 6300.
03/15/04   ka      Merged code from 6250. Fixed Lint error.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/12/04   ic      Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/06/04   ws      Initial jaguar/mainline merge.
01/13/04   ka      Merged changes from MSM6200
12/11/03   ic      Fixes to cmss_proc_call_connected(), send IDM changed event
                   when needed
11/03/03   ws      Fixed LTK compile problems.
11/04/03   sj      Updated to support srv_status_pwr_save.
10/30/03   prk     Modified featurization of cmss_timer_proc().
10/27/03   ws      Fixed compiler warnings.
10/21/03   ws      Merged BS Info Changes
10/17/03   ws      Fix problem with multiple service indications.
09/16/03   ws      Generate a SS event for uz_id changes
08/15/03   ws      Do not set srv status to no service until timer proc expires.
07/28/03   SJ      Support for circulating data suspend flag thru' SD.
07/25/03   vt      Attempts reporting of RSSI for CDMA only in valid modes.
07/10/03   jqi     Added Extended PRL support
                   -Removed FEATURE_IS683C_PRL
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
05/01/03   ws      Fixed problem with packet_zone_update () comparing wrong enum
04/28/03   AT      Lint cleanup.
04/22/03   prk     Removed runtime mode check before calling cmcall_proc_srv_status
                   in cmss_update_srv_ind.  Instead, made changes to
                   cmcall_proc_srv_status.
03/24/03   ws      Added runtime mode check before calling cmcall_proc_srv_status
                   in cmss_update_srv_ind
03/13/03   prk     Updated cmss_update_srv_ind() to assign the service status
                   from SD to the serving system information.  The limited
                   service service status was not being handled.
02/28/03   prk     Updated cmss_update_srv_ind() to re-initialize the mobility
                   management if there is a loss of service or a change in the
                   PLMN.
02/27/03   CS      Added handling for the new SS ps_data_suspend field.
                   Modified cmss_process_service_ind to only handle the
                   SS info that has not been processed by SD.
02/25/03   RI      Added cmcall_proc_srv_status() call in cmss_update_srv_ind()
02/21/03   vt      Fixed problems to build for HDR.
02/03/03   prk     Modified cmss_process_service_ind() to always call
                   cmcall_proc_srv_status() (even if the srv status did not
                   change).
01/21/03   ws      Updated copyright information for 2003
01/09/03   PRK     Added forward declartion for cm_ss_init_mm_information().
12/17/02   vt      Fixed a typo.
11/27/02   vt      Fixed a compiler warning.
11/22/02   PRK     Added processing of mobility management information.
11/15/02   vt      Updated RSSI value whenever service status changed.
11/14/02   vt      Merged changes from GW mainline.
                   [CS]Added notification of service change event to CM call Object.
11/05/02   vt      Changed the slope from 28 to 33 for FM RSSI.
10/31/02   vt      Initialized ss object for multimode build.
                   Corrected a compile error for JCDMA. Fixed a GW RSSI problem.
10/30/02   vt      Fixed compiler errors.
10/23/02   vt      Mainlined FEATURE_CM_USE_UMTS_REG_INTERFACE.
10/22/02   vt      Uncommented code for GPS mode handling.
10/05/02   vt      Replaced checking srv_is_on with srv_status
09/10/02   ws      Added initial cm20 backward compatibility.
09/03/02   PRk     Modified cmss_report_rssi to report RSSI regardless of
                   mode being GSM or WCDMA.
05/17/02   PRK     Modified cmss_reset to set service indicators to "no service"
12/07/01   jqi     Added support for band class 5 in cmss_xlat_block_or_system.
05/05/02   vt      Fixed compile problem with new mm cm.h
05/04/02   PRK     Updated to support new REG-CM interface.
                   Introduced FEATURE_CM_USE_UMTS_REG_INTERFACE.
04/26/02   ATM     Added calls to cmlog_system_mode()
02/06/02   PRK     Updated parameter list in call to cm_util_sys_id_match.
02/04/02   PRK     Updated parameter list in call to sys_PLMN_get_MCC_MNC.
01/04/02   PRK     Renamed types and structs used by PLMN selection/reselection.
                   Featurized future PLMN selection/reselection code with
                   FEATURE_NEW_PLMN.
                   Removed REAL_CODE pre-processor directive.
11/21/01   PRK     Added REAL_CODE pre-processor "feature" to remove code not
                   needed in initial Surf testing.  Upon completion of Surf
                   testing the code should be restored to its original form.
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                   Added support for PLMN selection/reselection (FEATURE_WCDMA,
                   FEATURE_GSM, FEATURE_PLMN).
                   Re-added Serving System group and moved srv_state, roam_status,
                   rssi from phone group to serving system group (FEATURE_NEWSS).
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
01/14/02   ic      Fixes related to running with FEATURE_IS2000_REL_A undefined
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
09/24/01   RC      Fixed CR 18443: MS powers down with fatal error cmss.c
                   477 on acquiring an invalid T53  channel.
08/21/01   ic      Main-lined FEATURE_NEWCM
01/23/01   ych     Merged JCDMA features.
09/09/99   SH      Added band class, block, registration status and p_rev
                   information to support data application
05/31/99   SH      Added CM 2.0 fetures
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
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
        enums
        macros

        functions
        - Internal [Common, 1x only, GW only]
        - External [Common, 1x only, GW only]

****************************************************************************/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "customer.h"
#include "cm.h"        /* External interface to cm.c */
#include "cmss.h"      /* Interface to CM Serving System Object */
#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"       /* System wide definitions */
#include "cmtask.h"    /* Interface to timer definitions */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmlog.h"     /* Interface for Diagnostic Logging */
#include "cmph.h"      /* Interface to CM phone */
#include "cmutil.h"    /* Interface for cm_util_undefine_sys_id */
#include "cmregprx.h"  /* Interface for CM Reg Proxy */
#include "clk.h"       /* Interface to clock services */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#include "mccreg.h"    /* CDMA registration service */
#include "srch.h"
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

#include "nv.h"        /* interface to NV */


#include "cmpref.h"    /* Interface to CM pref module. */

#if (defined(FEATURE_HDR) || defined(FEATURE_HDR_HYBRID))
#error code not present
#endif  /* (defined(FEATURE_HDR) || defined(FEATURE_HDR_HYBRID)) */

#ifdef FEATURE_HDR_HANDOFF
#error code not present
#endif // FEATURE_HDR_HANDOFF
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#endif


#ifdef FEATURE_WLAN
#error code not present
#endif

#ifdef FEATURE_IP_CALL
#include "cmipappi.h" /* Interface to inform cm ip app about service */
#endif

#ifdef FEATURE_BCMCS
#error code not present
#endif

#ifdef FEATURE_UMTS_1X_HANDOVER
#error code not present
#endif

#ifdef FEATURE_DEDICATED_C2K_MODE
#include "cmxdbm.h"
#include "cmxsms.h"
#endif

#if defined(FEATURE_WCDMA)
#error code not present
#endif

#if (defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK))
#error code not present
#endif /* defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK) */

#ifdef FEATURE_CM_GAN
#error code not present
#endif

#ifdef FEATURE_CM_SS_MEAS
#error code not present
#endif

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/
#ifdef FEATURE_IMS_VCC
static void cmss_wlan_link_qual_cb_reg(boolean is_reg);
static sys_wlan_link_qual_e_type cmss_map_wlan_link_qual_to_cm_sys_val(wlan_cp_link_qual_ind_enum_type  ind);
#endif

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
static uint16 cmss_get_fm_rssi (void);
static uint16 cmss_get_cdma_rssi (void);
static byte   cmss_get_cdma_ecio( void );
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */
static void cm_ss_init_reg_reject_info(

    cm_reg_reject_info_s_type  *cm_reg_reject_info_ptr
        /* The SS regn reject info structure to be initialized */
);


/* Compares Srv info and returns bit-mask of changed fields
** from main
*/
static uint64 cmss_get_ss_main_info_change(

    const cm_mm_ss_info_s_type     *new_srv_info,
        /*  New SS information */

    const cm_mm_ss_info_s_type     *old_srv_info
        /*  Old SS information */
);

/* Compares Srv info and returns bit-mask of changed fields
** from HDR
*/
static uint64 cmss_get_ss_hdr_info_change(

    const cm_mm_ss_info_s_type     *new_srv_info,
        /*  New SS information */

    const cm_mm_ss_info_s_type     *old_srv_info
        /*  Old SS information */
);

/* Compares Srv info and returns bit-mask of changed fields
** from GW
*/
static uint64 cmss_get_ss_gw_info_change(

    const cm_mm_ss_info_s_type     *new_srv_info,
        /*  New SS information */

    const cm_mm_ss_info_s_type     *old_srv_info
        /*  Old SS information */
);


/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

#define CMSS_RSSI_NO_SIGNAL                 125      /* -125dBm for no signal */

#define CMSS_RSCP_NO_SIGNAL                -125      /* -125 dBm for no signal */  

#define CMSS_RSSI2_NO_SIGNAL               -125      /* -125dBm for no signal */

#define CMSS_ECIO_VALUE_NO_SIGNAL             5

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#define CMSS_ECIO_VALUE_MAX                  14
#endif

#define CMSS_ECIO_VALUE_NO_SIGNAL_UMTS      0x3F

#define CMSS_PATHLOSS_VALUE_NO_SIGNAL       0xFF

#define CMSS_SIR_VALUE_NO_SIGNAL            0xFF

#if (defined FEATURE_WCDMA) && defined(FEATURE_UMTS_REPORT_ECIO)
#error code not present
#endif

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)||\
       defined(FEATURE_HYBR_GW)) 
#error code not present
#endif
#define CMSS_NO_SRV_UPTIME_NONE             (dword) (-1)
#ifdef FEATURE_PS_DORMANT_PWR_SAVE
#define CMSS_NO_PS_DATA_UPTIME_NONE         (dword) (-1)
#endif /* FEATURE_PS_DORMANT_PWR_SAVE */

#define CMSS_IO_VALUE_NO_SIGNAL       (-106)

#define CMSS_SINR_VALUE_NO_SIGNAL     (8)

#define CMSS_INFORM_SRV_CHANGE_INTER_NONE   (dword) (-1)

#define CMSS_INFORM_SRV_CHANGE_TIME        2    /* seconds */
     /* Time interval when the current service parameters are
     ** checked to see if EVENT_SRV_CHANGED should be posted
     */

#define CMSS_MAX_NO_SRV_TIME          4    /* seconds */
    /* Max time for phone to be with no service before reporting to CM
    ** clients of service lost event. If service is reacquired before such
    ** time is expired, CM clients never notified of the service lost
    ** event */
#ifdef FEATURE_PS_DORMANT_PWR_SAVE
#define CMSS_MAX_PS_DATA_PENDING_TIME  30    /* seconds */
    /* Max time for CM to pending a PS data available request.
    */


#define CMSS_MAX_NO_PS_DATA_SRV_TIME  30    /* seconds */
    /* Max time for CM to waiting for service upon PS data available request.
    */

#define CMSS_MAX_NO_PS_DATA_RESUME_TIME 120 /* seconds */
    /* Max time for CM to waiting for data resume after passing down ps data
    ** available request to stack
    */
#endif /* FEATURE_PS_DORMANT_PWR_SAVE */

#ifdef FEATURE_CALL_THROTTLE
#ifdef FEATURE_JCDMA
static const dword thr_time[CMSS_ORIG_THR_MAX_TIMER_INDEX] = 
                                                      {0,0,0,180};
#else
static const dword thr_time[CMSS_ORIG_THR_MAX_TIMER_INDEX] = 
                                                      {0,0,0,60,120,480,900};
#endif
#endif /* FEATURE_CALL_THROTTLE */

#if defined(FEATURE_GW_ELIST_TFT_PCO) /* For Lint */
#error code not present
#endif


/* 6th Bit position that decides negative LTM */
#define CMSS_LTM_NEG_BIT  0x20

/* Sign extending to 7th and 8th bits */
#define CMSS_LTM_NEG_SIGN_EXTN  0xC0


/**--------------------------------------------------------------------------
** Static objects
** --------------------------------------------------------------------------
*/


/* !!!! REUSE cmnv_data_buf from other files !!!!!! */
static cmnv_data_s_type      cmnv_data_buf;
    /* Static CM NV data buffer that is big enough to hold data
    ** that is associated with CM NV items.
    **
    ** Note that we could simply use nv_item_type, but this would
    ** be a waste of ~300 bytes of RAM */

/*lint -e{826} Suspicious pointer-to-pointer conversion (area too small) */
static nv_item_type  *cmnv_item_ptr = (nv_item_type*) &cmnv_data_buf;
    /* NV Item pointer to do cmnv_write() and cmnv_read(). Note that
    ** this pointer is initialized to point at a RAM buffer of adequate
    ** size to do all CM NV read/writes operations */




/**--------------------------------------------------------------------------
** Macros
** --------------------------------------------------------------------------
*/

#define CMABS(x) (((x) < 0) ? -(x) : (x))

#if defined(FEATURE_GW_ELIST_TFT_PCO) /* For Lint */
#error code not present
#endif

/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmss_get_sid_mcc_assoc_ptr

DESCRIPTION
  Returns pointer to user selected sid -> mcc association table.

DEPENDENCIES
 None

RETURN VALUE
  TRUE  if sys_id are equal
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
static cmss_sid_mcc_assoc_tbl_s_type *cmss_get_sid_mcc_assoc_ptr ()
{
  /** Holds user selected sid to mcc mapping
  */
  static cmss_sid_mcc_assoc_tbl_s_type cmss_sid_mcc_assoc_tbl;

  return &cmss_sid_mcc_assoc_tbl;

}


/* Note that miscellaneous functions (as opposed to serving system object
** functions) do NOT take a pointer to a serving system object as their
** first parameter. As a result these functions aren't capable of changing
** the serving system state nor do they have a lasting effect beyond their
** returned value.
*/



#ifdef FEATURE_CALL_THROTTLE
/*===========================================================================

FUNCTION cmss_misc_is_sys_id_equal

DESCRIPTION
  This function checks to see it two sys_ids are equal

DEPENDENCIES
 None

RETURN VALUE
  TRUE  if sys_id are equal
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(528, cmss_misc_is_sys_id_equal) not referenced */
/*
** called when FEATURE_CALL_THROTTLE on
*/
static boolean cmss_misc_is_sys_id_equal
(
  sys_sys_id_s_type sys_id1,

  sys_sys_id_s_type sys_id2,
    /* The sys_ids to compare
    */

  sys_band_class_e_type band_class1,

  sys_band_class_e_type band_class2,
    /* Band class information
    */

  uint16   packet_zone1,

  uint16   packet_zone2
    /* The packet zone information
    */
)
{
  if( sys_id1.id_type != sys_id2.id_type )
    return FALSE;
  
  switch( sys_id1.id_type )
  {
    case SYS_SYS_ID_TYPE_UNDEFINED:
    {
      return FALSE;
    }
    
    case SYS_SYS_ID_TYPE_IS95:
    /* IS-95 system ID
    */
      if( band_class1 != band_class2 || packet_zone1 != packet_zone2 )
      {
        return FALSE;
      }
      if(sys_id1.id.is95.nid == SD_WILDCARD_NID)
      {
        sys_id2.id.is95.nid = SD_WILDCARD_NID;
      }
      else if(sys_id2.id.is95.nid == SD_WILDCARD_NID)
      {
        sys_id1.id.is95.nid = SD_WILDCARD_NID;
      }
      return(cmss_sys_id_match(sys_id1,sys_id2));

    case SYS_SYS_ID_TYPE_IS856:
      if( band_class1 != band_class2 )
      {
        return FALSE;
      }
      return(cmss_sys_id_match(sys_id1,sys_id2));
    
    default:
      return(cmss_sys_id_match(sys_id1,sys_id2));

  }/* switch(sys_id1.id_type) */

}/* cmss_misc_is_sys_id_equal */
#endif /* FEATURE_CALL_THROTTLE */


#ifdef FEATURE_BCMCS
#error code not present
#endif /* FEATURE_BCMCS */

#ifdef FEATURE_CALL_THROTTLE
/*===========================================================================

FUNCTION cmss_orig_thr_get_orig_tbl_entry

DESCRIPTION
  This returns the entry for the sys_id if it is present, otherwise it 
  returns an Invalid sys_id

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  Pointer to the entry if found, 
  NULL otherwise

SIDE EFFECTS
  None.
===========================================================================*/
static cmss_orig_thr_table_entry_s_type* cmss_orig_thr_get_orig_tbl_entry
(
  cmss_orig_thr_table_s_type *thr_tbl_ptr,
    /* Pointer to the origination throttle table
    */
  
  sys_sys_id_s_type           sys_id,
    /* The sys_id which is to be searched in the throttle table
    */

  sys_band_class_e_type       band_class,
     /* The band class
     */

  uint16                      packet_zone
     /* The packet zone id
     */
)
{

  unsigned int i=0;

  CM_ASSERT( thr_tbl_ptr != NULL );


  for( i=0; i < ARR_SIZE(thr_tbl_ptr->cmss_orig_thr_tbl); ++i)
  {
    if( cmss_misc_is_sys_id_equal(
        thr_tbl_ptr->cmss_orig_thr_tbl[i].orig_call_info.sys_id, 
        sys_id, 
        thr_tbl_ptr->cmss_orig_thr_tbl[i].orig_call_info.mode_info.\
        cdma_info.band_class, 
        band_class,
        thr_tbl_ptr->cmss_orig_thr_tbl[i].orig_call_info.mode_info.\
        cdma_info.packet_zone,
        packet_zone ) )
    {

      /* Found a match, return the entry
      */
      return( &thr_tbl_ptr->cmss_orig_thr_tbl[i] );

     } /* if(cmss_misc_is_sys_id_equal */

  }/* for(i =0 , i < ARR ... )*/

  return NULL;
} /* cmss_orig_thr_get_orig_tbl_entry */
#endif /* FEATURE_CALL_THROTTLE */

/*===========================================================================

FUNCTION cmss_info_alloc

DESCRIPTION
 Allocate a serving system info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated serving system info buffer.

SIDE EFFECTS
  none

===========================================================================*/
static cm_mm_ss_info_s_type  *cmss_info_alloc( void )
{
  static cm_mm_ss_info_s_type   ss_info;
      /* Currently there is only a need for one serving system info buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the buffer
  */ 
  (void)memset( &ss_info, 0, sizeof( cm_mm_ss_info_s_type ));

  /* Return a pointer to the statically allocated
  ** serving system info buffer.
  */
  return &ss_info;

} /* cmss_info_alloc() */



/*===========================================================================

FUNCTION cmss_info_get

DESCRIPTION
  Copy the current serving system state information into a specified buffer.

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  TRUE if serving system information is valid, FALSE otherwise.

  Note that serving system information only becomes valid after cmss_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmss_info_get(

    cm_mm_ss_info_s_type    *ss_info_ptr,
        /* Copy serving system state info into this buffer */

    const cm_mm_ss_info_s_type    *base_ss_info_ptr
        /* Base ss_info that gets copied into ss_info_ptr. Can be either
        ** cmss.info OR cmss.srv_avail_info
        */
)
{
  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF

  cmss_s_type          *ss_ptr = cmss_ptr();
      /* Point at serving system object */

  #ifdef FEATURE_CALL_THROTTLE
  cmss_orig_thr_table_entry_s_type* tbl_entry_ptr;
  #endif /* FEATURE_CALL_THROTTLE */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss_ptr != NULL );
  CM_ASSERT( ss_info_ptr != NULL );

  /* Verify that object was properly initialized.
  */
  CM_ASSERT( CM_INIT_CHECK(ss_ptr->init_mask) );


  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy appropriate serving system parameters into target buffer.
  */
  ss_info_ptr->changed_fields         = base_ss_info_ptr->changed_fields;
  ss_info_ptr->signal_strength_changed_fields = 
                         base_ss_info_ptr->signal_strength_changed_fields;
  ss_info_ptr->srv_status             = base_ss_info_ptr->srv_status;
  ss_info_ptr->true_srv_status        = base_ss_info_ptr->true_srv_status;
  ss_info_ptr->srv_domain             = base_ss_info_ptr->srv_domain;
  ss_info_ptr->srv_capability         = base_ss_info_ptr->srv_capability;
  ss_info_ptr->sys_mode               = base_ss_info_ptr->sys_mode;
  ss_info_ptr->active_band            = base_ss_info_ptr->active_band;
  ss_info_ptr->active_channel         = base_ss_info_ptr->active_channel;
  ss_info_ptr->hybrid_active_band     = base_ss_info_ptr->hybrid_active_band;
  ss_info_ptr->hybrid_active_channel  = base_ss_info_ptr->hybrid_active_channel;
  ss_info_ptr->roam_status            = base_ss_info_ptr->roam_status;
  ss_info_ptr->sys_id                 = base_ss_info_ptr->sys_id;
  ss_info_ptr->rssi                   = base_ss_info_ptr->rssi;
  ss_info_ptr->ecio                   = base_ss_info_ptr->ecio;
  ss_info_ptr->gw_ecio                = base_ss_info_ptr->gw_ecio;
  ss_info_ptr->pathloss               = base_ss_info_ptr->pathloss;
  ss_info_ptr->sir                    = base_ss_info_ptr->sir;
  ss_info_ptr->io                     = base_ss_info_ptr->io;
  ss_info_ptr->sinr                   = base_ss_info_ptr->sinr;
  ss_info_ptr->rssi2                  = base_ss_info_ptr->rssi2;
  ss_info_ptr->rscp                   = base_ss_info_ptr->rscp;
  ss_info_ptr->ecio_for_display       = base_ss_info_ptr->ecio_for_display;
  ss_info_ptr->ps_data_suspend        = base_ss_info_ptr->ps_data_suspend;

  ss_info_ptr->is_sys_prl_match       = base_ss_info_ptr->is_sys_prl_match;
  ss_info_ptr->is_sys_forbidden       = base_ss_info_ptr->is_sys_forbidden;

  ss_info_ptr->hdr_hybrid             = cmpref_is_hybr_status_on();
  ss_info_ptr->hdr_srv_status         = base_ss_info_ptr->hdr_srv_status;
  ss_info_ptr->hdr_roam_status        = base_ss_info_ptr->hdr_roam_status;
  ss_info_ptr->hdr_rssi               = base_ss_info_ptr->hdr_rssi;
  ss_info_ptr->hdr_ecio               = base_ss_info_ptr->hdr_ecio;
  ss_info_ptr->hdr_io                 = base_ss_info_ptr->hdr_io;
  ss_info_ptr->hdr_sinr               = base_ss_info_ptr->hdr_sinr;
  ss_info_ptr->hdr_active_prot        = base_ss_info_ptr->hdr_active_prot;
  ss_info_ptr->hdr_ln_status          = base_ss_info_ptr->hdr_ln_status;
  ss_info_ptr->cipher_domain          = base_ss_info_ptr->cipher_domain;
  ss_info_ptr->cell_info              = base_ss_info_ptr->cell_info;

  #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
  ss_info_ptr->gw_srv_status      = base_ss_info_ptr->gw_srv_status;
  ss_info_ptr->true_gw_srv_status = base_ss_info_ptr->true_gw_srv_status;
  ss_info_ptr->gw_srv_domain      = base_ss_info_ptr->gw_srv_domain;
  ss_info_ptr->gw_srv_capability  = base_ss_info_ptr->gw_srv_capability;
  ss_info_ptr->gw_sys_mode        = base_ss_info_ptr->gw_sys_mode;
  ss_info_ptr->gw_active_band     = base_ss_info_ptr->gw_active_band;
  ss_info_ptr->gw_roam_status     = base_ss_info_ptr->gw_roam_status;
  ss_info_ptr->gw_sys_id          = base_ss_info_ptr->gw_sys_id;
  ss_info_ptr->gw_rssi            = base_ss_info_ptr->gw_rssi;
  ss_info_ptr->gw_pathloss        = base_ss_info_ptr->gw_pathloss;
  ss_info_ptr->gw_sir             = base_ss_info_ptr->gw_sir;
  ss_info_ptr->gw_rssi2           = base_ss_info_ptr->gw_rssi2;
  ss_info_ptr->gw_rscp            = base_ss_info_ptr->gw_rscp;
  ss_info_ptr->gw_ecio_for_display= base_ss_info_ptr->gw_ecio_for_display;
  ss_info_ptr->gw_io              = base_ss_info_ptr->gw_io;
  ss_info_ptr->gw_sinr            = base_ss_info_ptr->gw_sinr;
  ss_info_ptr->gw_cell_info       = base_ss_info_ptr->gw_cell_info;
  #endif

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF

  #ifdef FEATURE_BCMCS
#error code not present
  #endif /* FEATURE_BCMCS */

  ss_info_ptr->is_colloc                    = FALSE;

  if (cmpref_is_hybr_status_on() )
  {
    ss_info_ptr->is_colloc                  = ss_ptr->is_colloc;
  }

  ss_info_ptr->mode_info                    = base_ss_info_ptr->mode_info;

  ss_info_ptr->wlan_srv_status              = base_ss_info_ptr->wlan_srv_status;
  ss_info_ptr->wlan_bss_info                = base_ss_info_ptr->wlan_bss_info;
  ss_info_ptr->wlan_stats                   = base_ss_info_ptr->wlan_stats;
  ss_info_ptr->wlan_rssi                    = base_ss_info_ptr->wlan_rssi;

  ss_info_ptr->main_thr_status.ss_thr_status = base_ss_info_ptr->main_thr_status.ss_thr_status;



  #ifdef FEATURE_CALL_THROTTLE
  tbl_entry_ptr = cmss_orig_thr_get_orig_tbl_entry( 
                              ss_ptr->cmss_orig_thr_tbl_ptr,
                              base_ss_info_ptr->sys_id,
                              base_ss_info_ptr->mode_info.cdma_info.band_class,
                              base_ss_info_ptr->mode_info.cdma_info.packet_zone );

  if( tbl_entry_ptr != NULL )
  {
    ss_info_ptr->main_thr_status.thr_uptime  = tbl_entry_ptr->thr_uptime;
    ss_info_ptr->main_thr_status.idx         = tbl_entry_ptr->idx;
    ss_info_ptr->main_thr_status.call_status = tbl_entry_ptr->orig_call_info.call_status;
  }
  else
  #endif /* FEATURE_CALL_THROTTLE */
  {
    ss_info_ptr->main_thr_status.thr_uptime  = 0;
    ss_info_ptr->main_thr_status.idx         = 0;
    ss_info_ptr->main_thr_status.call_status = CM_SS_ORIG_THR_REASON_NONE;
  }

  ss_info_ptr->hdr_sys_id                    = base_ss_info_ptr->hdr_sys_id;

  ss_info_ptr->hdr_thr_status.ss_thr_status  = base_ss_info_ptr->main_thr_status.ss_thr_status;

  if (cmpref_is_hybr_status_on() )
  {

    #ifdef FEATURE_CALL_THROTTLE
    tbl_entry_ptr = cmss_orig_thr_get_orig_tbl_entry( 
                              ss_ptr->cmss_orig_thr_tbl_ptr,
                              base_ss_info_ptr->hdr_sys_id,
                              base_ss_info_ptr->mode_info.cdma_info.band_class,
                              base_ss_info_ptr->mode_info.cdma_info.packet_zone );

    if( tbl_entry_ptr != NULL )
    {
      ss_info_ptr->hdr_thr_status.thr_uptime  = tbl_entry_ptr->thr_uptime;
      ss_info_ptr->hdr_thr_status.idx         = tbl_entry_ptr->idx;
      ss_info_ptr->hdr_thr_status.call_status = tbl_entry_ptr->orig_call_info.call_status;
    }
    else
    #endif /* FEATURE_CALL_THROTTLE */
    {
      ss_info_ptr->hdr_thr_status.thr_uptime  = 0;
      ss_info_ptr->hdr_thr_status.idx         = 0;
      ss_info_ptr->hdr_thr_status.call_status = CM_SS_ORIG_THR_REASON_NONE;
    }
  }
  else
  {
    ss_info_ptr->hdr_thr_status.thr_uptime  = 0;
    ss_info_ptr->hdr_thr_status.idx         = 0;
    ss_info_ptr->hdr_thr_status.call_status = CM_SS_ORIG_THR_REASON_NONE;
  }

  ss_info_ptr->curr_uptime                  = clk_uptime();


  ss_info_ptr->emerg_num_list               = base_ss_info_ptr->emerg_num_list;

  /* Copy registration status of ipapp
  */
  ss_info_ptr->ipapp_info                   = base_ss_info_ptr->ipapp_info;

  ss_info_ptr->psist                        = base_ss_info_ptr->psist;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* HS srv and call indication gets copied only when in WCDMA
  */
  if (base_ss_info_ptr->sys_mode == SYS_SYS_MODE_WCDMA)
  {
    ss_info_ptr->cell_srv_ind.hs_ind = base_ss_info_ptr->cell_srv_ind.hs_ind;
    #ifdef FEATURE_HSPA_CALL_STATUS_IND 
    ss_info_ptr->cell_srv_ind.hs_call_status = base_ss_info_ptr->cell_srv_ind.hs_call_status;
    #endif
  }
  else
  {
    ss_info_ptr->cell_srv_ind.hs_ind = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
    #ifdef FEATURE_HSPA_CALL_STATUS_IND
    ss_info_ptr->cell_srv_ind.hs_call_status = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
    #endif
  }

  /* DTM/EGPRS indication gets copied only when in GSM
  */
  #ifdef FEATURE_HYBR_GW
#error code not present
  #else
  if (base_ss_info_ptr->sys_mode    == SYS_SYS_MODE_GSM)
  #endif
  {
    ss_info_ptr->cell_srv_ind.dtm_supp   = base_ss_info_ptr->cell_srv_ind.dtm_supp;
    ss_info_ptr->cell_srv_ind.egprs_supp = base_ss_info_ptr->cell_srv_ind.egprs_supp;
  }
  else
  {
    ss_info_ptr->cell_srv_ind.dtm_supp   = SYS_DTM_SUPPORT_NOT_AVAIL;
    ss_info_ptr->cell_srv_ind.egprs_supp = SYS_EGPRS_SUPPORT_NOT_AVAIL;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* !!! CHANGE THE LOGIC OF POPULATING COUNTRY_LIST !!!! */
  /* Copy country list details to be given to the user
  */
  ss_info_ptr->country_list = ss_ptr->info.country_list;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy current country and home country and plus dial settings
  */
  ss_info_ptr->home_mcc = ss_ptr->info.home_mcc;

  ss_info_ptr->current_mcc = ss_ptr->info.current_mcc;

  ss_info_ptr->plus_dial_setting = ss_ptr->info.plus_dial_setting;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy Registration reject information
  */
  ss_info_ptr->reg_reject_info.reject_cause             = base_ss_info_ptr->reg_reject_info.reject_cause;
  ss_info_ptr->reg_reject_info.reject_srv_domain        = base_ss_info_ptr->reg_reject_info.reject_srv_domain;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy UMA GAN information
  */
  ss_info_ptr->gan_info  = base_ss_info_ptr->gan_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy measurement info
  */
  ss_info_ptr->meas_resp  = base_ss_info_ptr->meas_resp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy default roaming indicator
  */
  ss_info_ptr->def_roam_ind  = base_ss_info_ptr->def_roam_ind;

  /* Return boolean indicating whether serving system information is valid.
  */
  return BOOLEAN( CM_INIT_CHECK(ss_ptr->init_mask) );

} /* cmss_info_get() */

/*===========================================================================

FUNCTION cmss_update_last_reported_sysmode

DESCRIPTION
  Update the ss_ptr->last_system_reported field with the sysmode that is being 
  reported to the clients. 

DEPENDENCIES
  Serving system object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  Changes Serving System object
===========================================================================*/

static void cmss_update_last_reported_sysmode(void)
{
   cmss_s_type          *ss_ptr        =  cmss_ptr();
   CM_ASSERT( ss_ptr != NULL );
   CM_FUNC_START( "cmss_update_last_reported_sysmode()",
                  0,0,0 );

   #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
   if (BETWEEN(ss_ptr->info.gw_sys_mode,SYS_SYS_MODE_NO_SRV, SYS_SYS_MODE_MAX) ) 
   {

    /* Store the sys_mode that is being sent to the clients
    ** Dont copy if sys_mode that is being reported is SYS_SYS_MODE_NO_SRV,
    ** this makes sure that last_system_reported has the last successfully
    ** acquired system in case UE goes OOS. This helps DS to be in sync
    ** when call_events are sent. DS ignores SYS_SYS_MODE_NO_SRV (stores the 
    ** previously acquired system) and hence
    ** the call_event we send to them when we are in no service should have
    ** the last successfully acquired system.
    */
     ss_ptr->last_system_reported = ss_ptr->info.gw_sys_mode;

   }
   CM_MSG_HIGH("last_system_reported = %d, ss_ptr->info.gw_sys_mode %d",
               ss_ptr->last_system_reported,ss_ptr->info.gw_sys_mode,0);

   #else
   if (BETWEEN(ss_ptr->info.sys_mode,SYS_SYS_MODE_NO_SRV, SYS_SYS_MODE_MAX) ) 
   {

    /* Store the sys_mode that is being sent to the clients
    ** Dont copy if sys_mode that is being reported is SYS_SYS_MODE_NO_SRV,
    ** this makes sure that last_system_reported has the last successfully
    ** acquired system in case UE goes OOS. This helps DS to be in sync
    ** when call_events are sent. DS ignores SYS_SYS_MODE_NO_SRV (stores the 
    ** previously acquired system) and hence
    ** the call_event we send to them when we are in no service should have
    ** the last successfully acquired system.
    */
     ss_ptr->last_system_reported = ss_ptr->info.sys_mode;

   }
   CM_MSG_HIGH("last_system_reported = %d, ss_ptr->info.sys_mode %d",
               ss_ptr->last_system_reported,ss_ptr->info.sys_mode,0);

   #endif
   return;
}

/*===========================================================================

@FUNCTION cmss_copy_signal_strength_info_change

@DESCRIPTION
   Update the latest signal_strenfth information change information 
   to structure that contains info last send to client 

@DEPENDENCIES
   Phone object must have already been initialized with
   cmph_init().

@RETURN VALUE
   none
  
@SIDE EFFECTS
   none
===========================================================================*/
static void cmss_copy_signal_strength_info_change (

    cm_mm_ss_info_s_type           *avail_srv_info,
        /*  Available SS information send to client */

    const cm_mm_ss_info_s_type     *curr_srv_info
        /*  SS information current */
)
{
  avail_srv_info->signal_strength_changed_fields = 
                          curr_srv_info->signal_strength_changed_fields;

  avail_srv_info->rssi                          = curr_srv_info->rssi;
  avail_srv_info->rscp                          = curr_srv_info->rscp;
  avail_srv_info->gw_ecio                       = curr_srv_info->gw_ecio;
  avail_srv_info->io                            = curr_srv_info->io;
  avail_srv_info->sinr                          = curr_srv_info->sinr;
  avail_srv_info->sir                           = curr_srv_info->sir;
  avail_srv_info->pathloss                      = curr_srv_info->pathloss;
  avail_srv_info->ecio_for_display              = curr_srv_info->ecio_for_display;
} /* cmss_copy_signal_strength_info_change */


/*===========================================================================

@FUNCTION cmss_get_signal_strength_info_change

@DESCRIPTION
   Compares the signal_strenfth information change and returns
   a bit mask holding difference.

@DEPENDENCIES
   Phone object must have already been initialized with
   cmph_init().

@RETURN VALUE
   none
  
@SIDE EFFECTS
   none

===========================================================================*/
static void cmss_get_signal_strength_info_change(

    cm_mm_ss_info_s_type     *new_srv_info,
        /*  New SS information */

    cmss_s_type               *ss_ptr
        /*  SS information */
)
{
  uint64     signal_strength_changed_fields = 0;
      /** Records the changes in Signal Strength parameters 
      **  between new_srv_info and old_srv_info 
      */

  if ((new_srv_info != NULL) && (ss_ptr != NULL))
  {

    if (new_srv_info->rssi != ss_ptr->srv_avail_info.rssi)
    {
      signal_strength_changed_fields |= CM_SS_RSSI_MASK;
    }

    if (new_srv_info->rscp != ss_ptr->srv_avail_info.rscp)
    {
      signal_strength_changed_fields |= CM_SS_RSCP_MASK;
    }
    
    if (new_srv_info->gw_ecio != ss_ptr->srv_avail_info.gw_ecio)
    {
      signal_strength_changed_fields |= CM_SS_ECIO_MASK;
    }

    if (new_srv_info->io != ss_ptr->srv_avail_info.io)
    {
      signal_strength_changed_fields |= CM_SS_IO_MASK;
    }

    if (new_srv_info->sinr != ss_ptr->srv_avail_info.sinr)
    {
      signal_strength_changed_fields |= CM_SS_SINR_MASK;
    }

    if (new_srv_info->sir != ss_ptr->srv_avail_info.sir)
    {
      signal_strength_changed_fields |= CM_SS_SIR_MASK;
    }

    if (new_srv_info->pathloss != ss_ptr->srv_avail_info.pathloss)
    {
      signal_strength_changed_fields |= CM_SS_PATHLOSS_MASK;
    }

    ss_ptr->info.signal_strength_changed_fields  |= signal_strength_changed_fields;
    
    new_srv_info->signal_strength_changed_fields  |= signal_strength_changed_fields;

    /* Update srv_avail_info with latest information */
    cmss_copy_signal_strength_info_change(&ss_ptr->srv_avail_info, new_srv_info);

    CM_MSG_HIGH("CMSS Signal Strength Changed 0x%x 0x%x ",
                 QWORD_HIGH(new_srv_info->signal_strength_changed_fields),
                 QWORD_LOW(new_srv_info->signal_strength_changed_fields),
               0);
  } 
} /* cmss_get_signal_strength_info_change */

/*===========================================================================

FUNCTION cmss_event

DESCRIPTION
  Notify the client list of a specified serving system event.

DEPENDENCIES
  Serving system object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  Changes Serving System object
===========================================================================*/
static void cmss_event(

    cm_ss_event_e_type    ss_event
        /* notify client list of this serving system event */
)
{

  cm_mm_ss_info_s_type *ss_info_ptr   = cmss_info_alloc();
      /* Allocate serving system info buffer */

  cmss_s_type          *ss_ptr        =  cmss_ptr();
      /* Point at serving system object to be initialized */

  uint64               changed_fields = 0;
      /* Local variable to get bit mask for information throttled */
       
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(ss_event, CM_SS_EVENT_NONE, CM_SS_EVENT_MAX) );
  CM_ASSERT( ss_info_ptr != NULL );
  CM_FUNC_START( "cmss_event(), ServSys event=%d",ss_event,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH("CM SS Event: %d",ss_event,0,0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy serving system state information into
  ** allocated buffer.
  */
  (void) cmss_info_get( ss_info_ptr, &ss_ptr->info );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* HDR and MAIN serving parameters are throttled to avoid
  ** CM_SS_EVENT_SRV_CHANGED event being generated at rapid
  ** succession. If this event does get generated due to other
  ** factors like change in sim_state or data_suspend, then
  ** throttling gets terminated and fields are updated with right
  ** values.
  */
  if (ss_event == CM_SS_EVENT_SRV_CHANGED)
  {
    /*
    ** update bit masks to show right values. Changed fields given to clients
    ** should be difference between last info sent to client vs current
    ** info being sent. Calculating changed fields from ss_ptr->info gives
    ** the difference between last info sent to client vs current 
    ** internal values maintained by CM. Current info being sent can be 
    ** different from current internal values maintained by CM
    */

    changed_fields   =
      cmss_get_ss_main_info_change (ss_info_ptr, &ss_ptr->srv_avail_info);

    changed_fields |=
      cmss_get_ss_hdr_info_change  (ss_info_ptr, &ss_ptr->srv_avail_info);    

    changed_fields |=
      cmss_get_ss_wlan_info_change (ss_info_ptr, &ss_ptr->srv_avail_info);

    changed_fields |=
      cmss_get_ss_gw_info_change (ss_info_ptr, &ss_ptr->srv_avail_info);


    ss_ptr->info.changed_fields   |= changed_fields;

    ss_info_ptr->changed_fields   |= changed_fields;

    /* Update srv_avail_info with latest information */
    ss_ptr->srv_avail_info         =   *ss_info_ptr;

    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
    #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

    #if defined(FEATURE_IP_CALL)
    /* Inform CM ip application of change in serving system parameters.
    ** cmipapp is not a client to CM so this function can be called here
    */
    cmipapp_inform_srv_system_change ( ss_info_ptr );
    #endif

  }

  if (ss_event == CM_SS_EVENT_RSSI)
  {
    cmss_get_signal_strength_info_change (ss_info_ptr, ss_ptr);
  }

  CM_MSG_HIGH("CMSS Change 0x%x 0x%x ",QWORD_HIGH(ss_info_ptr->changed_fields),
                                       QWORD_LOW(ss_info_ptr->changed_fields),
                                       0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Debug message to indicate specific values of interest to DS client
  ** Data suspend and current mode
  */

  if ((ss_info_ptr->changed_fields & CM_SS_PS_DATA_SUSPEND_MASK) ||
      (ss_info_ptr->changed_fields & CM_SS_SYS_MODE_MASK))
  {
    CM_MSG_HIGH ("Data suspend = %d, sys_mode = %d rep to clients",
                 ss_info_ptr->ps_data_suspend, ss_info_ptr->sys_mode, 0);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmss_update_last_reported_sysmode();

  /* Notify the client list of the serving system event.
  */

  cmclient_list_ss_event_ntfy( ss_event, ss_info_ptr );

} /* cmss_event() */


/* Note that serving system object functions (as opposed to misc functions)
** take a pointer to a serving system object as their first parameter.
** If their serving system pointer is NOT "const" they are capable of changing
** the serving system state.
*/




#ifdef  FEATURE_CALL_THROTTLE
/*===========================================================================

FUNCTION cmss_orig_thr_reset_tbl_entry

DESCRIPTION
  This function resets a particular entry of the oroignation throttle table
  
DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

static void cmss_orig_thr_reset_tbl_entry(

  cmss_orig_thr_table_entry_s_type *tbl_entry_ptr
    /* The pointer to the particular table entry 
    */
 )
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 CM_ASSERT( tbl_entry_ptr != NULL )
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tbl_entry_ptr->orig_call_info.call_status = CM_SS_ORIG_THR_REASON_NONE;
  tbl_entry_ptr->orig_call_info.call_type   = CM_CALL_TYPE_NONE;
  tbl_entry_ptr->idx                        = 0;
  tbl_entry_ptr->thr_uptime                 = 0;

  (void) memset( &tbl_entry_ptr->orig_call_info.mode_info, 0, 
                 sizeof( cm_ss_orig_thr_ss_info_u_type ));

  sys_undefine_sys_id( &tbl_entry_ptr->orig_call_info.sys_id );
  return;
} /* cmss_orig_thr_reset_tbl_entry */

#endif  /* FEATURE_CALL_THROTTLE */



/*===========================================================================

FUNCTION cmss_orig_thr_table_reset

DESCRIPTION
  Reset all the cmss_orig_thr_table information


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmss_orig_thr_table_reset( 

  cmss_orig_thr_table_s_type *cmss_orig_thr_tbl_ptr 
    /* Throttle table pointer */

)
/*lint -esym(818,cmss_orig_thr_tbl_ptr)*/
{

  #ifdef  FEATURE_CALL_THROTTLE
  unsigned int i = 0;

  /*-----------------------------------------------------------------------*/
  CM_ASSERT( cmss_orig_thr_tbl_ptr != NULL );
  /*-----------------------------------------------------------------------*/

  CM_MSG_LOW("Orig. Thr. Tbl. Reset",0,0,0);

  for(i = 0; i < ARR_SIZE(cmss_orig_thr_tbl_ptr->cmss_orig_thr_tbl); ++i)
  {
    cmss_orig_thr_reset_tbl_entry( 
                              &cmss_orig_thr_tbl_ptr->cmss_orig_thr_tbl[i] );
  }

  cmss_orig_thr_tbl_ptr->num_valid_entries = 0;

  #else
  SYS_ARG_NOT_USED(cmss_orig_thr_tbl_ptr);
  #endif  /* FEATURE_CALL_THROTTLE */
  return;

}/* cmss_orig_throttle_table_reset */



/*===========================================================================

FUNCTION cmss_reset

DESCRIPTION
  Reset all serving system information


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmss_reset( void )
{
  cmss_s_type           *ss_ptr  =  cmss_ptr();
        /* Point at serving system object to be initialized */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF

  /*
  ** 1. Initialize SS object
  */
  ss_ptr->bit_err_rate                             = 0;
  ss_ptr->no_srv_uptime                            = CMSS_NO_SRV_UPTIME_NONE;
  ss_ptr->main_srv_inform_uptime                   = CMSS_INFORM_SRV_CHANGE_INTER_NONE;
  ss_ptr->hdr_srv_inform_uptime                    = CMSS_INFORM_SRV_CHANGE_INTER_NONE;
  ss_ptr->wlan_srv_inform_uptime                   = CMSS_INFORM_SRV_CHANGE_INTER_NONE;
  ss_ptr->is_pref_srv_acq                          = TRUE;
  ss_ptr->is_redir_or_ho                           = FALSE;
  ss_ptr->ss_main_usable                           = SD_SYS_USABLE_OK;
  ss_ptr->gw_rssi                                  = CMSS_RSSI_NO_SIGNAL;
  ss_ptr->gw_rscp                                  = CMSS_RSCP_NO_SIGNAL;
  ss_ptr->gw_ecio_for_display                      = FALSE;
  ss_ptr->gw_ecio                                  = CMSS_ECIO_VALUE_NO_SIGNAL_UMTS;
  ss_ptr->gw_pathloss                              = CMSS_PATHLOSS_VALUE_NO_SIGNAL;
  ss_ptr->gw_sir                                   = CMSS_SIR_VALUE_NO_SIGNAL;
  ss_ptr->is_colloc                                = FALSE;
  ss_ptr->is_connected_mode_oos                    = FALSE;
  ss_ptr->main_srv_status                          = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->hybr_1_srv_status                        = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->gw_cell_info.cell_id                     = CMSS_INVALID_CELL_ID_INFO;
  sys_plmn_undefine_plmn_id( &ss_ptr->gw_cell_info.plmn_id );
  ss_ptr->gw_cell_info.lac_id                      = CMSS_INVALID_LAC_INFO;
  ss_ptr->gw_cell_info.arfcn                       = CMSS_INVALID_ARFCN_INFO;
  ss_ptr->gw_cell_info.bsic                        = CMSS_INVALID_BSIC_INFO;
  ss_ptr->gw_cell_info.psc                         = CMSS_INVALID_PSC_INFO;
  ss_ptr->gw_cell_info.uarfcn_dl                   = CMSS_INVALID_UARFCN_DL_INFO;
  ss_ptr->gw_cell_info.uarfcn_ul                   = CMSS_INVALID_UARFCN_UL_INFO;
  ss_ptr->gw_cell_info.refpn                       = CMSS_INVALID_REFPN_INFO;

  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  ss_ptr->wlan_no_srv_uptime                       = CMSS_NO_SRV_UPTIME_NONE;

  /*
  ** 2. Initialize SS object: info
  */
  ss_ptr->info.changed_fields         = 0;
  ss_ptr->info.signal_strength_changed_fields = 0;  
  ss_ptr->info.srv_status             = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->info.true_srv_status        = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->info.srv_domain             = SYS_SRV_DOMAIN_NO_SRV;
  ss_ptr->info.srv_capability         = SYS_SRV_DOMAIN_NO_SRV;
  ss_ptr->info.sys_mode               = SYS_SYS_MODE_NO_SRV;
  ss_ptr->info.active_band            = SYS_BAND_CLASS_NONE;
  ss_ptr->info.hybrid_active_band     = SYS_BAND_CLASS_NONE;
  ss_ptr->info.active_channel         = 0;
  ss_ptr->info.hybrid_active_channel  = 0;
  ss_ptr->info.roam_status            = SYS_ROAM_STATUS_OFF;
  ss_ptr->info.hdr_active_prot        = SYS_ACTIVE_PROT_NONE;
  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif
  sys_undefine_sys_id( &ss_ptr->info.sys_id );
  sys_undefine_sys_id( &ss_ptr->info.hdr_sys_id );
  ss_ptr->info.cell_info.cell_id      = CMSS_INVALID_CELL_ID_INFO;
  sys_plmn_undefine_plmn_id( &ss_ptr->info.cell_info.plmn_id );
  ss_ptr->info.cell_info.lac_id       = CMSS_INVALID_LAC_INFO;
  ss_ptr->info.cell_info.arfcn        = CMSS_INVALID_ARFCN_INFO;
  ss_ptr->info.cell_info.bsic         = CMSS_INVALID_BSIC_INFO;
  ss_ptr->info.cell_info.psc          = CMSS_INVALID_PSC_INFO;
  ss_ptr->info.cell_info.uarfcn_dl    = CMSS_INVALID_UARFCN_DL_INFO;
  ss_ptr->info.cell_info.uarfcn_ul    = CMSS_INVALID_UARFCN_UL_INFO;
  ss_ptr->info.cell_info.refpn        = CMSS_INVALID_REFPN_INFO;
  ss_ptr->info.is_sys_prl_match       = FALSE;
  ss_ptr->info.is_sys_forbidden       = FALSE;
  ss_ptr->info.rssi                   = CMSS_RSSI_NO_SIGNAL;
  ss_ptr->info.ecio                   = CMSS_ECIO_VALUE_NO_SIGNAL;
  ss_ptr->info.gw_ecio                = CMSS_ECIO_VALUE_NO_SIGNAL_UMTS;
  ss_ptr->info.rssi2                  = CMSS_RSSI2_NO_SIGNAL;
  ss_ptr->info.rscp                   = CMSS_RSCP_NO_SIGNAL;
  ss_ptr->info.ecio_for_display       = FALSE;
  ss_ptr->info.io                     = CMSS_IO_VALUE_NO_SIGNAL;
  ss_ptr->info.sinr                   = CMSS_SINR_VALUE_NO_SIGNAL;
  ss_ptr->info.pathloss               = CMSS_PATHLOSS_VALUE_NO_SIGNAL;
  ss_ptr->info.sir                    = CMSS_SIR_VALUE_NO_SIGNAL;
  ss_ptr->info.psist                  = CM_PSIST_LOW;
  ss_ptr->info.ps_data_suspend        = FALSE;
  ss_ptr->info.hdr_rssi               = CMSS_RSSI_NO_SIGNAL;
  ss_ptr->info.hdr_hybrid             = FALSE;
  ss_ptr->info.hdr_ecio               = CMSS_ECIO_VALUE_NO_SIGNAL;
  ss_ptr->info.hdr_io                 = CMSS_IO_VALUE_NO_SIGNAL;
  ss_ptr->info.hdr_sinr               = CMSS_SINR_VALUE_NO_SIGNAL;
  ss_ptr->info.ps_data_suspend        = FALSE;
  ss_ptr->info.hdr_hybrid             = FALSE;
  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF
  ss_ptr->info.hdr_srv_status         = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->info.hdr_roam_status        = SYS_ROAM_STATUS_OFF;
  ss_ptr->info.hdr_ln_status          = SYS_HDR_LOCATION_NOTIFICATION_STATUS_NONE;
  ss_ptr->info.cipher_domain          = SYS_SRV_DOMAIN_NO_SRV;
  ss_ptr->info.cell_srv_ind.hs_ind    = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
  ss_ptr->info.cell_srv_ind.dtm_supp  = SYS_DTM_SUPPORT_NOT_AVAIL;
  ss_ptr->info.cell_srv_ind.egprs_supp      = SYS_EGPRS_SUPPORT_NOT_AVAIL;
  ss_ptr->info.wlan_bss_info.rssi     = CMSS_RSSI_NO_SIGNAL;
  ss_ptr->info.wlan_rssi              = CMSS_RSSI_NO_SIGNAL;
  ss_ptr->info.wlan_srv_status        = SYS_SRV_STATUS_NO_SRV;

  sys_undefine_wlan_ssid(&ss_ptr->info.wlan_bss_info.ssid);
  sys_undefine_wlan_bssid(&ss_ptr->info.wlan_bss_info.bssid);
  ss_ptr->wlan_is_pref_srv_acq        = TRUE;

  #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
  ss_ptr->info.gw_is_sys_forbidden       = FALSE; 
  ss_ptr->info.gw_rssi                   = CMSS_RSSI_NO_SIGNAL;
  ss_ptr->info.gw_rssi2                  = CMSS_RSSI2_NO_SIGNAL;
  ss_ptr->info.gw_rscp                   = CMSS_RSCP_NO_SIGNAL;
  ss_ptr->info.gw_ecio_for_display       = FALSE;
  ss_ptr->info.gw_io                     = CMSS_IO_VALUE_NO_SIGNAL;
  ss_ptr->info.gw_pathloss               = CMSS_PATHLOSS_VALUE_NO_SIGNAL;
  ss_ptr->info.gw_sir                    = CMSS_SIR_VALUE_NO_SIGNAL;
  ss_ptr->info.gw_sinr                   = CMSS_SINR_VALUE_NO_SIGNAL;
  ss_ptr->info.gw_srv_status             = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->info.true_gw_srv_status        = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->info.gw_srv_domain             = SYS_SRV_DOMAIN_NO_SRV;
  ss_ptr->info.gw_srv_capability         = SYS_SRV_DOMAIN_NO_SRV;
  ss_ptr->info.gw_sys_mode               = SYS_SYS_MODE_NO_SRV;
  ss_ptr->info.gw_active_band            = SYS_BAND_CLASS_NONE;
  ss_ptr->info.gw_roam_status            = SYS_ROAM_STATUS_OFF;
  sys_undefine_sys_id( &ss_ptr->info.gw_sys_id );
  ss_ptr->gw_is_pref_srv_acq             = TRUE;
  ss_ptr->hybr_1_is_pref_srv_acq         = TRUE;
  ss_ptr->info.gw_cell_info.cell_id      = CMSS_INVALID_CELL_ID_INFO;
  sys_plmn_undefine_plmn_id( &ss_ptr->info.gw_cell_info.plmn_id );
  ss_ptr->info.gw_cell_info.lac_id       = CMSS_INVALID_LAC_INFO;
  ss_ptr->info.gw_cell_info.arfcn        = CMSS_INVALID_ARFCN_INFO;
  ss_ptr->info.gw_cell_info.bsic         = CMSS_INVALID_BSIC_INFO;
  ss_ptr->info.gw_cell_info.psc          = CMSS_INVALID_PSC_INFO;
  ss_ptr->info.gw_cell_info.uarfcn_dl   = CMSS_INVALID_UARFCN_DL_INFO;
  ss_ptr->info.gw_cell_info.uarfcn_ul    = CMSS_INVALID_UARFCN_UL_INFO;
  ss_ptr->info.gw_cell_info.refpn        = CMSS_INVALID_REFPN_INFO;
  #endif

  ss_ptr->is_hdr_session_open            = FALSE;

  /* Initialize UMA GAN information 
  */
  ss_ptr->info.gan_info.gan_state       = SYS_GW_GAN_STATE_NONE;

  ss_ptr->info.gan_info.gan_conn_status = SYS_GW_GAN_CONN_STATUS_NONE;

  ss_ptr->info.gan_info.gan_sig_chan_status
                                        = SYS_GW_GAN_SIG_CHAN_STATUS_NONE;

  memset(&(ss_ptr->info.gan_info.gan_mode_display), 
         0, 
         sizeof(sys_gw_gan_mode_display_s_type));

  memset(&(ss_ptr->info.gan_info.gan_error), 
         0,
         sizeof(sys_gw_gan_error_s_type));

  /*
  ** 3.1 Initialize SS object: mode info - GW only build
  */
  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  /*
  ** 3.2 Initialize SS object: mode info - CDMA only build
  */
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  #if ( !defined(FEATURE_WCDMA) && !defined(FEATURE_GSM) )
  ss_ptr->info.mode_info.cdma_info.band_class      = SYS_BAND_CLASS_NONE;
  ss_ptr->info.mode_info.cdma_info.block_or_system = SYS_BLKSYS_NONE;
  ss_ptr->info.mode_info.cdma_info.reg_zone        = CM_SS_REG_ZONE_NONE;
  ss_ptr->info.mode_info.cdma_info.packet_zone     = CM_SS_PACKET_ZONE_NONE;
  ss_ptr->info.mode_info.cdma_info.bs_p_rev        = CM_SS_BS_P_REV_NONE;
  ss_ptr->info.mode_info.cdma_info.p_rev_in_use    = CM_SS_P_REV_IN_USE_NONE;
  ss_ptr->info.mode_info.cdma_info.is_registered   = FALSE;
  ss_ptr->info.mode_info.cdma_info.ccs_supported   = FALSE;
  ss_ptr->info.mode_info.cdma_info.uz_id           = CM_INVALID_UZID;
  ss_ptr->info.mode_info.cdma_info.base_id         = 0;
  ss_ptr->info.mode_info.cdma_info.base_lat        = 0;
  ss_ptr->info.mode_info.cdma_info.base_long       = 0;
  ss_ptr->info.mode_info.cdma_info.srch_win_n      = 0;
  ss_ptr->info.mode_info.cdma_info.time_info.sys_mode = SYS_SYS_MODE_NO_SRV;
  ss_ptr->srv_avail_info.mode_info.cdma_info.time_info.sys_mode = SYS_SYS_MODE_NO_SRV;
  ss_ptr->srv_avail_info.mode_info.cdma_info.base_id    = 0;
  ss_ptr->srv_avail_info.mode_info.cdma_info.base_lat   = 0;
  ss_ptr->srv_avail_info.mode_info.cdma_info.base_long  = 0;
  ss_ptr->srv_avail_info.mode_info.cdma_info.srch_win_n = 0;
  #endif /* ( !defined(FEATURE_WCDMA) && !defined(FEATURE_GSM) ) */
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  /*
  ** 3.3 Initialize SS object: mode info - MM (GW + CDMA) build
  */
  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
  #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  #ifdef FEATURE_BCMCS
#error code not present
  #endif /* FEATURE_BCMCS */

  /* Initalize measurement response info */
  ss_ptr->info.meas_resp.sys_mode                   = SYS_SYS_MODE_NONE;
  ss_ptr->info.meas_resp.info.hdr_resp.meas_status  = CM_SS_MEAS_STATUS_NONE;

  /* Initialize srv_avail_info object
  */
  ss_ptr->srv_avail_info.srv_status         = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->srv_avail_info.true_srv_status    = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->srv_avail_info.sys_mode           = SYS_SYS_MODE_NO_SRV;
  ss_ptr->srv_avail_info.roam_status        = SYS_ROAM_STATUS_OFF;
  ss_ptr->srv_avail_info.srv_domain         = SYS_SRV_DOMAIN_NO_SRV;
  ss_ptr->srv_avail_info.srv_capability     = SYS_SRV_DOMAIN_NO_SRV;
  sys_undefine_sys_id( &ss_ptr->srv_avail_info.sys_id );
  sys_undefine_sys_id( &ss_ptr->srv_avail_info.hdr_sys_id );
  ss_ptr->srv_avail_info.is_sys_prl_match   = FALSE;
  ss_ptr->srv_avail_info.is_sys_forbidden   = FALSE;

  ss_ptr->srv_avail_info.cell_info.cell_id   = CMSS_INVALID_CELL_ID_INFO;
  sys_plmn_undefine_plmn_id( &ss_ptr->srv_avail_info.cell_info.plmn_id );
  ss_ptr->srv_avail_info.cell_info.lac_id    = CMSS_INVALID_LAC_INFO;
  ss_ptr->srv_avail_info.cell_info.arfcn     = CMSS_INVALID_ARFCN_INFO;
  ss_ptr->srv_avail_info.cell_info.bsic      = CMSS_INVALID_BSIC_INFO;
  ss_ptr->srv_avail_info.cell_info.psc       = CMSS_INVALID_PSC_INFO;
  ss_ptr->srv_avail_info.cell_info.uarfcn_dl = CMSS_INVALID_UARFCN_DL_INFO;
  ss_ptr->srv_avail_info.cell_info.uarfcn_ul = CMSS_INVALID_UARFCN_UL_INFO;
  ss_ptr->srv_avail_info.cell_info.refpn     = CMSS_INVALID_REFPN_INFO;
  
  #ifdef FEATURE_PS_DORMANT_PWR_SAVE
  /* Initialize ps data state
  */
  ss_ptr->ps_data_stt                       = CM_PS_DATA_STT_NONE;
  ss_ptr->no_ps_data_uptime                 = CMSS_NO_PS_DATA_UPTIME_NONE;
  #endif /* FEATURE_PS_DORMANT_PWR_SAVE */

  ss_ptr->srv_avail_info.wlan_srv_status        =  SYS_SRV_STATUS_NO_SRV;
  ss_ptr->srv_avail_info.wlan_rssi              =  CMSS_RSSI_NO_SIGNAL;
  ss_ptr->srv_avail_info.wlan_bss_info.rssi     = CMSS_RSSI_NO_SIGNAL;
  ss_ptr->srv_avail_info.wlan_bss_info.ssid.len = 0;
  ss_ptr->srv_avail_info.wlan_bss_info.bssid    = 0;
  ss_ptr->info.cell_srv_ind.hs_call_status      = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
  ss_ptr->srv_avail_info.cell_srv_ind.hs_ind    = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
  ss_ptr->srv_avail_info.cell_srv_ind.dtm_supp  = SYS_DTM_SUPPORT_NOT_AVAIL;
  
  #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
  ss_ptr->srv_avail_info.gw_is_sys_forbidden       = FALSE; 
  ss_ptr->srv_avail_info.gw_rssi                   = CMSS_RSSI_NO_SIGNAL;
  ss_ptr->srv_avail_info.gw_ecio                   = CMSS_ECIO_VALUE_NO_SIGNAL_UMTS;
  ss_ptr->srv_avail_info.gw_io                     = CMSS_IO_VALUE_NO_SIGNAL;
  ss_ptr->srv_avail_info.gw_sinr                   = CMSS_SINR_VALUE_NO_SIGNAL;
  ss_ptr->srv_avail_info.gw_rssi2                  = CMSS_RSSI2_NO_SIGNAL;
  ss_ptr->srv_avail_info.gw_rscp                   = CMSS_RSCP_NO_SIGNAL;
  ss_ptr->srv_avail_info.gw_ecio_for_display       = FALSE;
  ss_ptr->srv_avail_info.gw_srv_status             = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->srv_avail_info.true_gw_srv_status        = SYS_SRV_STATUS_NO_SRV;
  ss_ptr->srv_avail_info.gw_srv_domain             = SYS_SRV_DOMAIN_NO_SRV;
  ss_ptr->srv_avail_info.gw_srv_capability         = SYS_SRV_DOMAIN_NO_SRV;
  ss_ptr->srv_avail_info.gw_sys_mode               = SYS_SYS_MODE_NO_SRV;
  ss_ptr->srv_avail_info.gw_active_band            = SYS_BAND_CLASS_NONE;
  ss_ptr->srv_avail_info.gw_roam_status            = SYS_ROAM_STATUS_OFF;
  sys_undefine_sys_id( &ss_ptr->srv_avail_info.gw_sys_id );
  ss_ptr->srv_avail_info.gw_cell_info.cell_id      = CMSS_INVALID_CELL_ID_INFO;
  sys_plmn_undefine_plmn_id( &ss_ptr->srv_avail_info.gw_cell_info.plmn_id );
  ss_ptr->srv_avail_info.gw_cell_info.lac_id       = CMSS_INVALID_LAC_INFO;
  ss_ptr->srv_avail_info.gw_cell_info.arfcn        = CMSS_INVALID_ARFCN_INFO;
  ss_ptr->srv_avail_info.gw_cell_info.bsic         = CMSS_INVALID_BSIC_INFO;
  ss_ptr->srv_avail_info.gw_cell_info.psc          = CMSS_INVALID_PSC_INFO;
  ss_ptr->srv_avail_info.gw_cell_info.uarfcn_dl    = CMSS_INVALID_UARFCN_DL_INFO;
  ss_ptr->srv_avail_info.gw_cell_info.uarfcn_ul    = CMSS_INVALID_UARFCN_UL_INFO;
  ss_ptr->srv_avail_info.gw_cell_info.refpn        = CMSS_INVALID_REFPN_INFO;
  #endif

  /* Initialize UMA GAN info in srv_avail_info 
  */
  ss_ptr->srv_avail_info.gan_info.gan_state           = 
                                                      SYS_GW_GAN_STATE_NONE;

  ss_ptr->srv_avail_info.gan_info.gan_conn_status     = 
                                                SYS_GW_GAN_CONN_STATUS_NONE;

  ss_ptr->srv_avail_info.gan_info.gan_sig_chan_status =
                                            SYS_GW_GAN_SIG_CHAN_STATUS_NONE;

  memset(&(ss_ptr->srv_avail_info.gan_info.gan_mode_display), 
         0, 
         sizeof(sys_gw_gan_mode_display_s_type));

  memset(&(ss_ptr->srv_avail_info.gan_info.gan_error), 
         0,
         sizeof(sys_gw_gan_error_s_type));

  /* Initalize measurement response in srv_avail_info */
  ss_ptr->srv_avail_info.meas_resp.sys_mode             = SYS_SYS_MODE_NONE;
  ss_ptr->srv_avail_info.meas_resp.info.hdr_resp.meas_status  = 
                                                     CM_SS_MEAS_STATUS_NONE;


  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif
  ss_ptr->last_system_reported = SYS_SYS_MODE_NO_SRV;
  /* Initialize the cmss_orig_thr_table 
  */
  cmss_orig_thr_table_reset( ss_ptr->cmss_orig_thr_tbl_ptr );

  /* Initialize country identifying fields */
  ss_ptr->info.country_list.present = FALSE;

  /* current country */
  ss_ptr->info.current_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;

  /* Home country */
  ss_ptr->info.home_mcc    = CM_INVALID_MOBILE_COUNTRY_CODE;

  /* Clear the sid to mcc user table */
  cmss_get_sid_mcc_assoc_ptr()->sid_mcc_tbl_len = 0;

  /* Registration reject information */
  cm_ss_init_reg_reject_info( &ss_ptr->info.reg_reject_info );

  /* Initialize default roaming indicator */
  ss_ptr->info.def_roam_ind = SYS_ROAM_STATUS_OFF;

} /* cmss_reset() */

#ifdef FEATURE_HDR
#error code not present
#endif // FEATURE_HDR


/*===========================================================================
FUNCTION cmss_report_rssi

DESCRIPTION
  This function gets the RSSI. If the RSSI change exceeds the delta threshold,
  CM reports rssi change event to registered clients.

DEPENDENCIES
  Serving System and Phone objects must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmss_report_rssi (

  const void              *rpt_ptr,
    /* ptr to message received */

  cm_client_s_type        *client_ptr,
    /* pointer to a client struct */

  boolean                 ignore_updates
    /* flag to ignore any RSSI changes and always send RSSI events */

)
/*lint -esym(715,rpt_ptr)*/
{
  cmss_s_type          *ss_ptr       = cmss_ptr();
  cmph_s_type          *ph_ptr       = cmph_ptr();

  cm_mm_ss_info_s_type *ss_info_ptr   = cmss_info_alloc();
      /* Allocate serving system info buffer */

  sys_sys_mode_e_type   sys_mode     = ss_ptr->info.sys_mode;
  uint16                rssi         = ss_ptr->info.rssi;
  uint16                current_rssi = 0;
  uint8                 rssi_delta   = ph_ptr->rssi_delta;

  #ifdef FEATURE_HDR
#error code not present
  #endif

  #if ( defined(FEATURE_UMTS_REPORT_ECIO) || defined(FEATURE_CDMA_800) || \
        defined(FEATURE_CDMA_1900) || defined (FEATURE_HDR) )
  uint8                 ecio_delta   = ph_ptr->ecio_delta;
  #endif
  
  #if (defined(FEATURE_HDR) || defined(FEATURE_HDR_HYBRID))
#error code not present
  #endif

  #if (defined(FEATURE_HDR) || defined(FEATURE_HDR_HYBRID) || \
       defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  int16                 ecio         = ss_ptr->info.ecio ;
  int16                 current_ecio = 0;
  #endif

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && \
     defined(FEATURE_UMTS_REPORT_ECIO)
#error code not present
  #endif /* #if ...FEATURE_UMTS_REPORT_ECIO */   

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)||\
       defined(FEATURE_HYBR_GW)) 
#error code not present
  #endif

  #if (((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && \
     defined(FEATURE_UMTS_REPORT_ECIO)) || defined(FEATURE_HYBR_GW))
#error code not present
  #endif
  
  boolean               report_rssi              = FALSE;

  ss_ptr->info.signal_strength_changed_fields  = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Report WLAN RSSI is requested.
  */

  rssi        = ss_ptr->info.wlan_rssi;
  switch ( ss_ptr->info.wlan_srv_status )
  {
    case SYS_SRV_STATUS_NO_SRV:
    case SYS_SRV_STATUS_PWR_SAVE:
      if (rssi != CMSS_RSSI_NO_SIGNAL)
      {
        ss_ptr->info.wlan_bss_info.rssi = CMSS_RSSI_NO_SIGNAL;
        ss_ptr->info.wlan_rssi          = CMSS_RSSI_NO_SIGNAL;
        report_rssi  = TRUE;
      }
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SYS_SRV_STATUS_LIMITED:
    case SYS_SRV_STATUS_SRV:

      #ifdef FEATURE_WLAN
#error code not present
      #endif

      if ( CMABS( rssi - current_rssi) > rssi_delta )
      {
        ss_ptr->info.wlan_bss_info.rssi = current_rssi;
        ss_ptr->info.wlan_rssi          = current_rssi;
        report_rssi  = TRUE;
      }
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  }

  if ( report_rssi || ignore_updates )
  {
    /*
    ** Inform the client that the WLAN RSSI was modified.
    */
    if(client_ptr != NULL)
    {
      /* Copy serving system state information into
      ** allocated buffer.
      */
      (void) cmss_info_get( ss_info_ptr, &ss_ptr->info );

      cm_client_ss_event_ntfy(client_ptr,
                              CM_SS_EVENT_WLAN_RSSI,
                              ss_info_ptr);
    }
    else
    {
      cmss_event( CM_SS_EVENT_WLAN_RSSI );
    }
  } /* if ( report_rssi ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Handle RSSI for SS_MAIN/SS_HDR here.
  */

  report_rssi  = FALSE;
  sys_mode     = ss_ptr->info.sys_mode;
  rssi         = ss_ptr->info.rssi;
  rssi_delta   = ph_ptr->rssi_delta;
  current_rssi = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( sys_mode )
  {
    case SYS_SYS_MODE_NO_SRV:
      ss_ptr->info.sinr = CMSS_SINR_VALUE_NO_SIGNAL;
      ss_ptr->info.io   = CMSS_IO_VALUE_NO_SIGNAL;
      #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
      if ( !cmpref_is_hybr_status_on() )
      {
        ss_ptr->info.gw_sinr     = CMSS_SINR_VALUE_NO_SIGNAL;
        ss_ptr->info.gw_io       = CMSS_IO_VALUE_NO_SIGNAL;
        ss_ptr->info.gw_pathloss = CMSS_PATHLOSS_VALUE_NO_SIGNAL;
        ss_ptr->info.gw_sir      = CMSS_SIR_VALUE_NO_SIGNAL;
        ss_ptr->info.gw_ecio     = CMSS_ECIO_VALUE_NO_SIGNAL_UMTS;
        ss_ptr->info.hdr_sinr    = CMSS_SINR_VALUE_NO_SIGNAL;
        ss_ptr->info.hdr_io      = CMSS_IO_VALUE_NO_SIGNAL;
        if( ss_ptr->info.gw_rssi!= CMSS_RSSI_NO_SIGNAL )
        {
          rssi = ss_ptr->info.gw_rssi;
        }
        else if( ss_ptr->info.hdr_rssi != CMSS_RSSI_NO_SIGNAL )
        {
          rssi = ss_ptr->info.hdr_rssi;
        }
      }
      #endif

      if ( rssi != CMSS_RSSI_NO_SIGNAL )
      {
        ss_ptr->info.rssi             = CMSS_RSSI_NO_SIGNAL;
        ss_ptr->info.ecio             = CMSS_ECIO_VALUE_NO_SIGNAL;
        ss_ptr->info.pathloss         = CMSS_PATHLOSS_VALUE_NO_SIGNAL;
        ss_ptr->info.sir              = CMSS_SIR_VALUE_NO_SIGNAL;
        ss_ptr->info.gw_ecio          = CMSS_ECIO_VALUE_NO_SIGNAL_UMTS;
        ss_ptr->info.rssi2            = CMSS_RSSI2_NO_SIGNAL;
        ss_ptr->info.rscp             = CMSS_RSCP_NO_SIGNAL;
        ss_ptr->info.ecio_for_display = FALSE;

        #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
        if ( !cmpref_is_hybr_status_on() )
        {
          ss_ptr->info.gw_rssi             = CMSS_RSSI_NO_SIGNAL;
          ss_ptr->info.hdr_rssi            = CMSS_RSSI_NO_SIGNAL;
          ss_ptr->info.gw_pathloss         = CMSS_PATHLOSS_VALUE_NO_SIGNAL;
          ss_ptr->info.gw_sir              = CMSS_SIR_VALUE_NO_SIGNAL;
          ss_ptr->info.gw_ecio             = CMSS_ECIO_VALUE_NO_SIGNAL_UMTS;
          ss_ptr->info.gw_rssi2            = CMSS_RSSI2_NO_SIGNAL;
          ss_ptr->info.gw_rscp             = CMSS_RSCP_NO_SIGNAL;
          ss_ptr->info.gw_ecio_for_display = FALSE;
        }
        #endif
        
        report_rssi          = TRUE;
      }
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    case SYS_SYS_MODE_AMPS:

      current_rssi = cmss_get_fm_rssi();
      ss_ptr->info.sinr = CMSS_SINR_VALUE_NO_SIGNAL;
      ss_ptr->info.io   = CMSS_IO_VALUE_NO_SIGNAL;

      if ( CMABS( rssi - current_rssi ) > rssi_delta )
      {
        ss_ptr->info.rssi             = current_rssi;
        ss_ptr->info.ecio             = CMSS_ECIO_VALUE_NO_SIGNAL;
        ss_ptr->info.pathloss         = CMSS_PATHLOSS_VALUE_NO_SIGNAL;
        ss_ptr->info.sir              = CMSS_SIR_VALUE_NO_SIGNAL;
        ss_ptr->info.rssi2            = CMSS_RSSI2_NO_SIGNAL;
        ss_ptr->info.rscp             = CMSS_RSCP_NO_SIGNAL;
        ss_ptr->info.ecio_for_display = FALSE;
        report_rssi                   = TRUE;
      }

      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    case SYS_SYS_MODE_CDMA:

      current_rssi = cmss_get_cdma_rssi();
      current_ecio = cmss_get_cdma_ecio();

      if ( (CMABS( rssi - current_rssi ) > rssi_delta)||
           (CMABS( ecio - current_ecio ) > ecio_delta)
         )
      {
        ss_ptr->info.rssi = current_rssi;
        ss_ptr->info.ecio = current_ecio;
        report_rssi       = TRUE;
        /* There is no function provided to report Io only for CDMA
        */
        ss_ptr->info.io   = CMSS_IO_VALUE_NO_SIGNAL;
        ss_ptr->info.sinr = CMSS_SINR_VALUE_NO_SIGNAL;
      } /* if delta threshold exceeded */

      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

    #ifdef FEATURE_HDR
#error code not present
    #endif /* FEATURE_HDR */

    #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
    #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

    default:
      break;
  }

  if ( report_rssi || ignore_updates )
  {
    /* Copy serving system state information into
    ** allocated buffer.
    */
    (void) cmss_info_get( ss_info_ptr, &ss_ptr->info );

    /*
    ** Inform the clients that the RSSI was modified.
    */

    #ifdef FEATURE_CM_TECH_SPEC_SRV_IND

    switch( sys_mode )
    {
      case SYS_SYS_MODE_HDR:

        if(client_ptr != NULL)
        {
          cm_client_ss_event_ntfy(client_ptr, 
                                  CM_SS_EVENT_HDR_RSSI,
                                  ss_info_ptr);
        }
        else
        {
          cmss_event( CM_SS_EVENT_HDR_RSSI );
        }
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SYS_SYS_MODE_GSM:
      case SYS_SYS_MODE_WCDMA:
      case SYS_SYS_MODE_GW:

        if(client_ptr != NULL)
        {
          cm_client_ss_event_ntfy(client_ptr, 
                                  CM_SS_EVENT_GW_RSSI,
                                  ss_info_ptr);
        }
        else
        {
          cmss_event( CM_SS_EVENT_GW_RSSI );
        }
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:
        if(client_ptr != NULL)
        {
          cmss_get_signal_strength_info_change (ss_info_ptr, ss_ptr);
          cm_client_ss_event_ntfy(client_ptr, 
                                  CM_SS_EVENT_RSSI,
                                  ss_info_ptr);
        }
        else
        {
          cmss_event( CM_SS_EVENT_RSSI );
        }
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    }
    #else
    if(client_ptr != NULL)
    {
      cmss_get_signal_strength_info_change (ss_info_ptr, ss_ptr);
      cm_client_ss_event_ntfy(client_ptr, 
                              CM_SS_EVENT_RSSI,
                              ss_info_ptr);
    }
    else
    {
      cmss_event( CM_SS_EVENT_RSSI );
    }
    #endif
  } /* if ( report_rssi ) */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Report HDR RSSI, if changed
  */
  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

} /* cmss_report_rssi() */
/*lint +esym(715,rpt_ptr)*/



/*===========================================================================

FUNCTION  cmss_ss_cmd_get_ss_info_para_check

DESCRIPTION
  Check parameter errors for get serving system info command

DEPENDENCIES
  none

RETURN VALUE
  CM_SS_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_ss_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ss_cmd_err_e_type  cmss_ss_cmd_get_ss_info_para_check(
  const cm_ss_cmd_s_type       *ss_cmd_ptr
)
{
  cm_ss_cmd_err_e_type   cmd_err       = CM_SS_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_cmd_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check integrity of command parameters */
  if( ss_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
  {
    cmd_err  = CM_SS_CMD_ERR_CLIENT_ID_P;
    CM_DBG_ERR_FATAL( "CM_SS_CMD_ERR_CLIENT_ID_P",0,0,0 );
  }
  else if( (ss_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK )
  {
    cmd_err  = CM_SS_CMD_ERR_CLIENT_ID_P;
    CM_DBG_ERR_FATAL( "CM_SS_CMD_ERR_CLIENT_ID_P",0,0,0 );
  }

  return cmd_err;

} /* cmss_ss_cmd_get_ss_info_para_check() */

#ifdef FEATURE_PS_DORMANT_PWR_SAVE

/*===========================================================================

FUNCTION  cmss_ss_cmd_ps_data_avail_para_check

DESCRIPTION
  Check parameter errors for ps data available request.

DEPENDENCIES
  none

RETURN VALUE
  CM_SS_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_ss_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ss_cmd_err_e_type  cmss_ss_cmd_ps_data_avail_para_check(

  const cm_ss_cmd_s_type      *ss_cmd_ptr
)
{
  cmph_s_type            *ph_ptr     = cmph_ptr();
      /* Pointer point to phone object */

  cm_ss_cmd_err_e_type   cmd_err     = CM_SS_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_cmd_ptr != NULL); 
  CM_ASSERT(ph_ptr     != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( ( ss_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE ) ||
      ((ss_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK ) )
  {
    /*  Make sure the client has a valid ID.
    */
    cmd_err  = CM_SS_CMD_ERR_CLIENT_ID_P;
  }
  else if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
  {
    /* Reject the command if phone is not online.
    */
    cmd_err = CM_SS_CMD_ERR_OFFLINE_S;
  }

  return cmd_err;

} /* cmss_ss_cmd_ps_data_avail_para_check() */
#endif /* FEATURE_PS_DORMANT_PWR_SAVE */

#ifdef FEATURE_CM_SS_MEAS 
#error code not present
#endif /* FEATURE_CM_SS_MEAS */


/*===========================================================================

FUNCTION  cmss_ss_cmd_orig_thr_cmd_update_table_para_check

DESCRIPTION
  Check parameter errors for orig throttle update table request.

DEPENDENCIES
  none

RETURN VALUE
  CM_SS_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_ss_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ss_cmd_err_e_type  cmss_ss_cmd_orig_thr_cmd_update_table_para_check
(
  const cm_ss_cmd_s_type      *ss_cmd_ptr
    /* Command pointer 
    */
)
{
  cm_ss_cmd_err_e_type   cmd_err     = CM_SS_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_cmd_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( ( ss_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE ) ||
      ((ss_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK ) )
  {
    /*  Make sure the client has a valid ID.
    */
    cmd_err  = CM_SS_CMD_ERR_CLIENT_ID_P;
    MSG_HIGH( "CM_SS_ORIG_THR_CMD_UPDATE_TABLE: ERR_CLIENT_ID_P",0,0,0 );
  }
  return cmd_err;

} /* cmss_ss_cmd_orig_thr_cmd_update_table_para_check() */

/*===========================================================================

FUNCTION  cmss_ss_cmd_country_selected_para_check

DESCRIPTION
  Check parameter errors for country selected by user

DEPENDENCIES
  none

RETURN VALUE
  CM_SS_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_ss_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ss_cmd_err_e_type  cmss_ss_cmd_country_selected_para_check
(
  cm_ss_cmd_s_type      *ss_cmd_ptr
    /* Command pointer
    */
)
/*lint -esym(818,ss_cmd_ptr)*/
{
  cm_ss_cmd_err_e_type   cmd_err     = CM_SS_CMD_ERR_NOERR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check range of selected country */
  if ((!BETWEEN(ss_cmd_ptr->info.selected_country, 0,
                                       CM_MAX_MOBILE_COUNTRY_CODE_VAL))
     &&
    (ss_cmd_ptr->info.selected_country != CM_INVALID_MOBILE_COUNTRY_CODE)
   )
  {
    CM_ERR ("Country code selected in error %d",
               ss_cmd_ptr->info.selected_country, 0, 0);

    return CM_SS_CMD_ERR_OTHER;
  }

  /* Need not check sid range since new SID values can get added to expand the range */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  return cmd_err;

} /* cmss_ss_cmd_country_selected_para_check() */
/*lint +esym(818,ss_cmd_ptr)*/

/*===========================================================================

FUNCTION  cmss_client_cmd_check

DESCRIPTION
  Check for call command parameter errors and whether a specified serving system
  command is allowed in the current state of the call/phone.

DEPENDENCIES
  Serving system object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CM_SS_CMD_ERR_NONE if no error found, specific cm_ss_cmd_err_e_type otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ss_cmd_err_e_type  cmss_client_cmd_check(

    cm_cmd_s_type        *cmd_ptr
        /* Pointer to a CM command */
)
{
  cm_ss_cmd_s_type       *ss_cmd_ptr = NULL;
      /* Point at call command component */

  cm_ss_cmd_err_e_type    cmd_err    = CM_SS_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_SS );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ss_cmd_ptr = SS_CMD_PTR( cmd_ptr );
  CM_MSG_HIGH( "cmss_cmd_check, cmd=%d", ss_cmd_ptr->cmd, 0, 0);


  /* Check whether command is possible.
  */
  switch( ss_cmd_ptr->cmd )
  {

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------------*/
                     /* Get a snapshot of ss information   */
                     /*------------------------------------*/

    case CM_SS_CMD_INFO_GET:
      /* check command parameter errors */
      cmd_err = cmss_ss_cmd_get_ss_info_para_check(ss_cmd_ptr);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------------*/
                     /*      Get system measurements       */
                     /*------------------------------------*/
    #ifdef FEATURE_CM_SS_MEAS
#error code not present
    #endif /* FEATURE_CM_SS_MEAS */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #ifdef FEATURE_PS_DORMANT_PWR_SAVE
    case CM_SS_CMD_PS_DATA_AVAIL:
      /* check command parameter errors */
      cmd_err = cmss_ss_cmd_ps_data_avail_para_check(ss_cmd_ptr);

      break;
    #endif /* FEATURE_PS_DORMANT_PWR_SAVE */
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SS_CMD_ORIG_THR_UPDATE_TBL:
      /* check command parameter errors */
      cmd_err = cmss_ss_cmd_orig_thr_cmd_update_table_para_check(ss_cmd_ptr);

      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_SS_CMD_COUNTRY_SELECTED:
      /* check command parameter errors */
      cmd_err = cmss_ss_cmd_country_selected_para_check(ss_cmd_ptr);

      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



    default:

      cmd_err = CM_SS_CMD_ERR_OTHER;
      CM_SWITCH_ERR( "Bad call command %d", ss_cmd_ptr->cmd, 0, 0);
      break;

  }

  return cmd_err;

} /* cmss_client_cmd_check() */




/*===========================================================================

FUNCTION cmss_client_cmd_err

DESCRIPTION
  Notify clients of a specified serving system command error.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmss_client_cmd_err(

    const cm_cmd_s_type       *cmd_ptr,
        /* Pointer to a CM command */

    cm_ss_cmd_err_e_type       cmd_err
        /* Indicate the ss command error */
)
{
  const cm_ss_cmd_s_type    *ss_cmd_ptr = NULL;
      /* Point at ss command component */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_SS );
  CM_ASSERT( BETWEEN( cmd_err, CM_SS_CMD_ERR_NONE, CM_SS_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_cmd_ptr = SS_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cmss_client_cmd_err() ss_cmd=%d, cmd_err=%d, client=%p",
                  ss_cmd_ptr->cmd, cmd_err, ss_cmd_ptr->client_id );

  /* If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if( ss_cmd_ptr->cmd_cb_func != NULL )
  {
    ss_cmd_ptr->cmd_cb_func( ss_cmd_ptr->data_block_ptr,
                             ss_cmd_ptr->cmd,
                             cmd_err );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If error status is other than NO ERROR,
  ** notify the client list of command error, as well.
  */
  if( cmd_err != CM_SS_CMD_ERR_NOERR )
  {
    CM_ERR( "SS cmd err, cmd=%d, err=%d, client=%ld",
            ss_cmd_ptr->cmd, cmd_err, ss_cmd_ptr->client_id );
    cmclient_list_ss_cmd_err_ntfy( cmd_err, ss_cmd_ptr );
  }

} /* cmss_client_cmd_err() */



/*===========================================================================

FUNCTION cmss_send_ss_info

DESCRIPTION
  This function sends the serving system information to the requesting
  client.

DEPENDENCIES
  SS object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmss_send_ss_info(

  const cm_ss_cmd_s_type     *ss_cmd_ptr,
      /* The pointer to a serving system command */

        cm_ss_event_e_type    event_type
      /* The type of event to be sent to client */
)
{

  cm_mm_ss_info_s_type          *ss_info_ptr;
  
  cmss_s_type                   *ss_ptr = cmss_ptr();
      /* Point at serving system object */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_cmd_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  if ( ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA )
  {
    ss_ptr->info.mode_info.cdma_info.is_registered = mccreg_is_registered();
    ss_ptr->srv_avail_info.mode_info.cdma_info.is_registered = ss_ptr->info.mode_info.cdma_info.is_registered;
  }
  #endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

  /*
  ** Get a buffer for serving system information.
  */
  ss_info_ptr = cmss_info_alloc();

  CM_ASSERT( ss_info_ptr != NULL );

  /*
  ** Update the RSSI in the serving system object.
  */
  cmss_report_rssi( NULL, ss_cmd_ptr->client_ptr, TRUE );

  /* Update WLAN Stats.
  */
  cmss_report_wlan_stats();

  /*
  ** Copy serving system information into allocated buffer.
  */
  (void) cmss_info_get( ss_info_ptr, &ss_ptr->srv_avail_info );

  /*
  ** Notify only the requesting client.
  */
  cm_client_ss_event_ntfy( ss_cmd_ptr->client_ptr,
                           event_type,
                           ss_info_ptr );

} /* cmss_send_ss_info() */


#ifdef FEATURE_PS_DORMANT_PWR_SAVE

/*===========================================================================

FUNCTION cmss_update_data_avail_stt

DESCRIPTION
  Update data available state based on the data suspend status and mm
  command status.

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static  void cmss_update_data_avail_stt
(
        boolean                    data_suspend
            /* Indicate if data is suspended or not. */
)
{
  cmss_s_type                *ss_ptr = cmss_ptr();
      /* Point at serving system object */

  boolean is_cmd_done;
          /* Indicate if the command is successfully send to the mm.*/
  boolean is_req_done = FALSE;
          /* Indicate if the data avail request is done. */

  CM_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the mm command status.*/
  is_cmd_done = cmss_send_mm_data_avail();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform clients success if the data has been resumed.
  */
  if( ! data_suspend )
  {
    /* Data has been resumed so generate success event and reset
    ** the state.
    */
    CM_MSG_MED("Send PS DATA AVAIL SUCCESS EVENT", 0, 0, 0 );

    cmss_event( CM_SS_EVENT_PS_DATA_SUCCESS );

    is_req_done = TRUE;

  }
  else
  {
    if ( ! is_cmd_done )
    {
      cmss_event( CM_SS_EVENT_PS_DATA_FAIL );

      is_req_done = TRUE;

    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If data available request is done, reset the state. Otherwise update
  ** to the next state.
  */

  if( ! is_req_done )
  {
    ss_ptr->ps_data_stt = CM_PS_DATA_STT_RESUME;
    ss_ptr->no_ps_data_uptime = CMSS_MAX_NO_PS_DATA_RESUME_TIME +
                                clk_uptime();
  }
  else
  {
    ss_ptr->ps_data_stt = CM_PS_DATA_STT_NONE;
    ss_ptr->no_ps_data_uptime = CMSS_NO_PS_DATA_UPTIME_NONE;
  }

} /* cmss_update_data_avail_stt */




/*===========================================================================

FUNCTION cmss_proc_data_avail

DESCRIPTION
  This function process the data available request.
  If there is calls, holds the data available request until CM gets
  ok_to_orig from SD. If ps service is not available or data is not resumed
  within certain period of time, a failure event is generated. If data is
  resumed on time, a success event is generated.

  If there is no call, send the data available request imediately and wait
  for data to be resumed. If the data is resumed within certain period of
  time, an successful event is end otherwise a failure event is triggered.

DEPENDENCIES
  SS object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmss_proc_data_avail(

  const cm_ss_cmd_s_type     *ss_cmd_ptr
      /* The pointer to a serving system command */

)
{
  cmss_s_type                *ss_ptr = cmss_ptr();
      /* Point at serving system object */

  cmph_s_type                *ph_ptr = cmph_ptr();
      /* Point at phone object */

  cm_mm_ss_info_s_type       *ss_info_ptr;
      /* Point at serving system object */
  dword                      clk_time =  clk_uptime();
      /* Current clock time */

  cm_orig_q_s_type          *ph_orig_para_top_ptr = 
                               cmtask_orig_para_get_top( CM_SS_MAIN ); 
      /* The pointer to the top element of the priority queue */

  #ifdef FEATURE_CM_TECH_SPEC_SRV_IND

  cm_orig_q_s_type          *ph_hybr_1_orig_para_top_ptr = 
                               cmtask_orig_para_get_top( CM_SS_HYBR_1 ); 
      /* The pointer to the top element of the priority queue */
  #endif

  cm_mode_pref_e_type     fav_mode_pref; 
  cm_srv_type_e_type      srv_type;
  sys_sys_mode_e_type     sys_mode;


  CM_ASSERT( ss_cmd_ptr != NULL );

  CM_MSG_MED("RXD PS DATA AVAILABLE REQ", 0, 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Get a buffer for serving system information.
  */
  ss_info_ptr = cmss_info_alloc();

  CM_ASSERT( ss_info_ptr != NULL );

  /*
  ** Copy serving system information into allocated buffer.
  */
  (void) cmss_info_get( ss_info_ptr, &ss_ptr->srv_avail_info );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Notify only the requesting client.
  **
  ** Note: Client ID is set to the begining address of the client object
  ** during the client initialization.
  */
  cm_client_ss_event_ntfy( ss_cmd_ptr->client_ptr,
                           CM_SS_EVENT_PS_DATA_AVAIL,
                           ss_info_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** The data available process differently within or without a call.
  **
  ** If there is a call, wait for ok_to_orig before send the cmd to MM
  ** Otherwise, send the cmd directly to MM.
  */

  if( cmtask_orig_para_search_act_type(CM_SS_MAIN, CM_ACT_TYPE_DATA_CALL) != NULL
    #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
      || cmtask_orig_para_search_act_type(CM_SS_HYBR_1, CM_ACT_TYPE_DATA_CALL) != NULL
      #endif
  )
  {
    /* If there is a pending get network list request, update the ps data 
    ** state to pending state. 
    */
    if( ph_ptr->network_type != CM_MODE_PREF_NONE )
    {
      ss_ptr->ps_data_stt = CM_PS_DATA_STT_PENDING;
      ss_ptr->no_ps_data_uptime = clk_time + CMSS_MAX_PS_DATA_PENDING_TIME;

      return;
    }

    #ifdef FEATURE_CM_TECH_SPEC_SRV_IND

    if(cmpref_is_gw_hybr() )
    {
      if ( ph_hybr_1_orig_para_top_ptr != NULL )
      {
        /* Waiting for ok_to_orig report from SD if there is a call. */
        cmpref_proc_cmd_pref_sys_chgd(
        SD_SS_HYBR_1,
        SD_SS_PREF_REAS_ORIG_RESUME,
        ph_hybr_1_orig_para_top_ptr->orig.orig_mode,
        cmph_map_cm_mode_pref_to_sd_mode_pref(
        ph_hybr_1_orig_para_top_ptr->orig.orig_mode_pref ),
        (sd_ss_band_pref_e_type) ph_hybr_1_orig_para_top_ptr->orig.orig_band_pref,
        cmph_map_cm_prl_pref_to_sd_prl_pref
          (ph_hybr_1_orig_para_top_ptr->orig.orig_prl_pref),
        cmph_map_cm_roam_pref_to_sd_roam_pref
          (ph_hybr_1_orig_para_top_ptr->orig.orig_roam_pref),
        ph_hybr_1_orig_para_top_ptr->orig.orig_int_hybr_pref,
        SD_BAND_PCS,
        SD_BLKSYS_PCS_A,
        SD_SS_AVOID_SYS_IDLE,
        0,
        ph_ptr->plmn,
        cmph_map_cm_srv_domain_pref_to_sd(
          ph_hybr_1_orig_para_top_ptr->orig.orig_srv_domain_pref),
        cmph_map_cm_acq_order_pref_to_sd(
          ph_ptr->acq_order_pref),
        NULL
        );
      }
      else
      {
        CM_ERR ("Top element on priority queue of HDR Stack is NULL", 0, 0, 0);
      }
    }
    else
    #endif
    {

      srv_type = cmph_get_srv_type(CM_CALL_TYPE_PS_DATA); 
    
      /* If srv_type is AUTOMATIC, convert it based on sys mode */
      if (srv_type == CM_SRV_TYPE_AUTOMATIC)
      {
        sys_mode = sd_misc_get_curr_acq_sys_mode (); 
        srv_type = cmph_map_sys_mode_to_srv_type(sys_mode); 
      }

      /* Compute fav_mode_pref.
      ** It is the intersection of phone's mode preference and 
      ** srv_type of the call.
      */
      fav_mode_pref = cmph_misc_intersect_mode_pref(
                          srv_type,
                          ph_ptr->mode_pref
                          );            

      /* Change the mode_pref of all data calls to computed fav_mode_pref
      */
      if (fav_mode_pref != CM_MODE_PREF_NONE)
      {
        (void)cmtask_orig_para_change_mode_pref( CM_SS_MAIN,
                                                 CM_ACT_TYPE_DATA_CALL,
                                                 fav_mode_pref,
                                                 TRUE );
      }
      else
      {
        CM_MSG_MED("Intersection of call_ptr's srv_type and ph_ptr's mode_pref is \
                    NONE. mode_pref not changed", 0, 0, 0);
      }

      /* UE is already having full service on GW.*/
      if((ss_ptr->info.srv_status == SYS_SRV_STATUS_SRV) &&
         (ss_ptr->info.sys_mode == SYS_SYS_MODE_WCDMA ||
          ss_ptr->info.sys_mode == SYS_SYS_MODE_GSM ||
          ss_ptr->info.sys_mode == SYS_SYS_MODE_GW) &&
         (ss_ptr->info.srv_domain == SYS_SRV_DOMAIN_PS_ONLY ||
          ss_ptr->info.srv_domain == SYS_SRV_DOMAIN_CS_PS))
      {
        /*
        ** pump the priority of all data calls over to phone object
        */
   
        (void)cmtask_orig_para_change_act_priority( CM_SS_MAIN,
                                                 CM_ACT_TYPE_DATA_CALL,
                                                 CM_ACT_PRIORITY_BELOW_PH,
                                                 CM_ACT_PRIORITY_80,
                                                 TRUE );

        /* The pointer to the top element of the priority queue */
        ph_orig_para_top_ptr = cmtask_orig_para_get_top( CM_SS_MAIN ); 
      }

      if ( ph_orig_para_top_ptr!= NULL )
      {
         /* Waiting for ok_to_orig report from SD if there is a call. */
         cmpref_proc_cmd_pref_sys_chgd(
         SD_SS_MAIN,
         SD_SS_PREF_REAS_ORIG_RESUME,
         ph_orig_para_top_ptr->orig.orig_mode,
         cmph_map_cm_mode_pref_to_sd_mode_pref(
         ph_orig_para_top_ptr->orig.orig_mode_pref ),
         (sd_ss_band_pref_e_type) ph_orig_para_top_ptr->orig.orig_band_pref,
         cmph_map_cm_prl_pref_to_sd_prl_pref
            (ph_orig_para_top_ptr->orig.orig_prl_pref),
         cmph_map_cm_roam_pref_to_sd_roam_pref
            (ph_orig_para_top_ptr->orig.orig_roam_pref),
         ph_orig_para_top_ptr->orig.orig_int_hybr_pref,
         SD_BAND_PCS,
         SD_BLKSYS_PCS_A,
         SD_SS_AVOID_SYS_IDLE,
         0,
         ph_ptr->plmn,
         cmph_map_cm_srv_domain_pref_to_sd(
           ph_orig_para_top_ptr->orig.orig_srv_domain_pref),
         cmph_map_cm_acq_order_pref_to_sd(
           ph_ptr->acq_order_pref),
         NULL
         );
      }
      else
      {
        CM_ERR ("Top element on priority queue of Main Stack is NULL", 0, 0, 0);
      }
    }

    /* Update ps data state */
    ss_ptr->ps_data_stt = CM_PS_DATA_STT_SRV_ACQ;
    ss_ptr->no_ps_data_uptime = clk_time + CMSS_MAX_NO_PS_DATA_SRV_TIME;

  }
  else
  {
    /*
    ** If coming here, the request is received without a call.
    ** Send it anyway.
    */
    CM_MSG_LOW( "Wrong DATA AVAIL REQ, ss %d",
                CM_SS_MAIN,
                0,
                0 );

    cmss_update_data_avail_stt( ss_info_ptr->ps_data_suspend );

  }

} /* cmss_proc_data_avail */
#endif /* FEATURE_PS_DORMANT_PWR_SAVE */



/*===========================================================================

FUNCTION cmss_map_sd_sid_to_sys_sid

DESCRIPTION
 Map the sd sid value to sys_sys_sid value.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.


===========================================================================*/
static void cmss_map_sd_sid_to_sys_sid(

    sd_mode_e_type          sd_mode,
      /* Current serving system mode.
      */

    sys_sys_id_s_type       *sys_sid_ptr,
      /* Serving system sys SID ptr.
      */

    const sd_sid_u_type     *sd_sid_ptr
      /* serving system SD SID pointer */
)
{
  CM_ASSERT( sd_sid_ptr != NULL );
  CM_ASSERT( sys_sid_ptr != NULL );

  switch ( sd_mode )
  {
    case SD_MODE_AMPS:
    case SD_MODE_CDMA:
      sys_sid_ptr->id_type = SYS_SYS_ID_TYPE_IS95;
      sys_sid_ptr->id.is95.sid = sd_sid_ptr->is95.sid;
      sys_sid_ptr->id.is95.nid = sd_sid_ptr->is95.nid;
      sys_sid_ptr->id.is95.mcc = sd_sid_ptr->is95.mcc;
      sys_sid_ptr->id.is95.imsi_11_12 = sd_sid_ptr->is95.imsi_11_12;
      break;

    case SD_MODE_HDR:
      sys_sid_ptr->id_type = SYS_SYS_ID_TYPE_IS856;
      /*lint -e{506} Contant value boolean for MIN */
      memcpy( sys_sid_ptr->id.is856,
              sd_sid_ptr->is856,
              MIN( ARR_SIZE( sys_sid_ptr->id.is856 ),
                   ARR_SIZE( sd_sid_ptr->is856 )) );
      break;

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
      sys_sid_ptr->id_type          = SYS_SYS_ID_TYPE_UMTS;
      sys_sid_ptr->id.plmn_lac.plmn = sd_sid_ptr->plmn.plmn_id;
      sys_sid_ptr->id.plmn_lac.lac  = sd_sid_ptr->plmn.lac;
      break;

    default:
      break;

  } /* switch ( sd_mode ) */

} /* cmss_map_sd_sid_to_sys_sid() */


#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/*===========================================================================

FUNCTION  cmss_identify_serving_country

DESCRIPTION
  For a given sid this is the order followed
    1> If user has resolved the sid to map to a
     MCC then that MCC is returned.

  2> MCC is derived using sid, ltm_off
     and daylt savings flag.

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
   None

SIDE EFFECTS
  Updates current_mcc field of SS object

===========================================================================*/
/*lint -esym(528, cmss_identify_serving_country) not referenced */
/*
** called when GW, GW_WLAN on
*/
static void cmss_identify_serving_country (void)
{

  cmutil_current_country_e_type status;

  cm_country_code_type  current_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;

  cmss_sid_mcc_assoc_tbl_s_type  *sid_mcc_assoc_ptr
                               = cmss_get_sid_mcc_assoc_ptr ();

  byte  conv_arr[]        = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };

  cm_country_code_type  prl_mcc;

  cm_country_code_type  ota_mcc;

  int loop = 0;

  cmss_s_type  *ss_ptr = cmss_ptr();
  /* Pointer to SS object */

  /*-------------------------------------------------------------*/

  /* If plus dial feature is disabled, return */
  if (ss_ptr->info.plus_dial_setting == CM_HS_BASED_PLUS_DIAL_DISABLED)
  {
    CM_MSG_HIGH ("plus_dial_setting is disabled \n", 0, 0, 0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If serving MCC is a value between 0 and
  ** CM_MAX_MOBILE_COUNTRY_CODE_VAL then copy
  ** or else find MCC
  */
  if ( (ss_ptr->info.sys_id.id.is95.mcc != 0) &&
     (ss_ptr->info.sys_id.id.is95.mcc < CM_MAX_MOBILE_COUNTRY_CODE_VAL)
   )
  {
    ota_mcc= ss_ptr->info.sys_id.id.is95.mcc;
       
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
    
    /* Convert MCC from OTA format to HPCD same as PRL format MCC.
    ** As defined in IS-2000 section 2.3.1.3
    **  1. Represent the 3-digit Mobile Country Code as D1 D2 D3 
    **     with the digit equal to zero being given the value of ten.
    **  2. Compute 100 × D1 + 10 × D2 + D3 - 111.
    **  3. Convert the result in step (2) to binary by 
    ** a standard decimal-to-binary conversion as described in Table 2.3.1.1-1.
    ** So the follwing code is to reverse this processing
    */

    ota_mcc= ss_ptr->info.sys_id.id.is95.mcc;
    prl_mcc =  conv_arr[ ota_mcc%10 ];
    ota_mcc /= 10;
    prl_mcc += conv_arr[ ota_mcc%10 ] * 10;
    ota_mcc /= 10;
    prl_mcc += conv_arr[ ota_mcc%10 ] * 100;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    ss_ptr->info.current_mcc = prl_mcc;
    CM_MSG_HIGH ("OTA MCC = %d  PRL MCC=%d",
        ss_ptr->info.sys_id.id.is95.mcc,
                 prl_mcc, 0);

    /* Even though we received this MCC, we are not sure it is valid or not
    ** Becasue many 1x networks send garbage mcc
    ** So If HPCD algorithm indentifies the mcc using (sid, ltm, daylight)
    ** in the follwing code,if it is found that will overwrite the 
    ** prl mcc which we received from the network 
    */
    
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* First check user table to see if it has already been resolved
  ** by the user. If it is copy it to current_mcc
  */
  for (loop = 0; loop < sid_mcc_assoc_ptr->sid_mcc_tbl_len; ++loop)
  {
    if (sid_mcc_assoc_ptr->sid_mcc_tbl[loop].sid ==
                                              ss_ptr->info.sys_id.id.is95.sid)
    {

      ss_ptr->info.current_mcc = sid_mcc_assoc_ptr->sid_mcc_tbl[loop].country_code;

      CM_MSG_HIGH ("Copying MCC from user table %d",
                                 ss_ptr->info.current_mcc, 0, 0);
      return;
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Supply sid, LTM offset, Daylt savings
  ** to identify country
  */
  status =  cmutil_identify_current_country ( ss_ptr->info.sys_id.id.is95.sid,
                                                          ss_ptr->ltm_offset,
                             (sys_daylt_savings_e_type)ss_ptr->daylt_savings,
                                                               &current_mcc);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (status == CMUTIL_CURRENT_COUNTRY_RESOLVED)
  {
    ss_ptr->info.current_mcc = current_mcc;
    CM_MSG_HIGH ("MCC from resolution table %d",ss_ptr->info.current_mcc,
                                                                   0, 0);
  }
  else if (status == CMUTIL_CURRENT_COUNTRY_CONFLICT)
  {

    /* Clear country list length */
    ss_ptr->info.country_list.country_code_len = 0;


    /* Populate country list and send event
    */
    cmutil_populate_country_list ( ss_ptr->info.sys_id.id.is95.sid,
                                                ss_ptr->ltm_offset,
                   (sys_daylt_savings_e_type)ss_ptr->daylt_savings,
                                       &ss_ptr->info.country_list);


    cmss_event (CM_SS_EVENT_SELECT_COUNTRY);

  }
  else
  {

    ss_ptr->info.current_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;

    CM_ERR ("Logic to resolve country code failed %d %d %d",
                            ss_ptr->info.sys_id.id.is95.sid,
                                         ss_ptr->ltm_offset,
                                         ss_ptr->daylt_savings
           );

  }


} /* cmss_identify_serving_country () */
#endif


/*===========================================================================

FUNCTION cmss_update_prio_queue

DESCRIPTION
  Update the priority queue as below.
   
  Pump phone  preferences to top of the queue and change the PS call 
  preferences to below phone preferences in following scenario.
  (To do alternate scanning on GW+1X).
    - Data is suspended 
    - Serive is not full service.
    - PS call preferences are on top of the queue

  Pump ps call preferences to top of the queue in following scenario
  (To resume dormant data call).
    - Data is suspended 
    - UE is already having full service on GW. 
    - PS call preferences are below phone preferences.
    - Data available request is pending 

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmss_update_prio_queue( void )
{

  #ifdef FEATURE_PS_DORMANT_PWR_SAVE
  cmss_s_type            *ss_ptr = cmss_ptr();
      /* Point at serving system object */

  cmph_s_type *ph_ptr = cmph_ptr();
      /* Point at phone object */
  
  cm_orig_q_s_type   *ph_orig_para_top_ptr = cmtask_orig_para_get_top( CM_SS_MAIN ); 

  cm_mode_pref_e_type     fav_mode_pref; 
  cm_srv_type_e_type      srv_type;
  sys_sys_mode_e_type     sys_mode;

  /* In a CDMA+GW capable phone, if a data call is suspended due to loss of
  ** service, then move it's priority below that of the phone object.
  ** This ensures that a suspended data call doesn't prevent us from
  ** searching for CDMA systems.
  */ 
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) \
      && (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #endif

  if(ss_ptr->info.ps_data_suspend  
    #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
    && ss_ptr->info.gw_srv_status == SYS_SRV_STATUS_SRV
    #else
    && ss_ptr->info.srv_status == SYS_SRV_STATUS_SRV
    #endif
    && ss_ptr->ps_data_stt == CM_PS_DATA_STT_SRV_ACQ
    && (cmtask_orig_para_search_act_type_act_priority
                                         ( CM_SS_MAIN,
                                           CM_ACT_TYPE_DATA_CALL,
                                           CM_ACT_PRIORITY_BELOW_PH ) != NULL)
    && (ss_ptr->info.sys_mode == SYS_SYS_MODE_WCDMA ||
        ss_ptr->info.sys_mode == SYS_SYS_MODE_GSM ||
        ss_ptr->info.sys_mode == SYS_SYS_MODE_GW)
    #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
    && (ss_ptr->info.gw_srv_domain == SYS_SRV_DOMAIN_PS_ONLY ||
         ss_ptr->info.gw_srv_domain == SYS_SRV_DOMAIN_CS_PS)
    #else
    && (ss_ptr->info.srv_domain == SYS_SRV_DOMAIN_PS_ONLY ||
        ss_ptr->info.srv_domain == SYS_SRV_DOMAIN_CS_PS)
    #endif 
    && ph_ptr->hybr_pref != CM_HYBR_PREF_CDMA__WCDMA
    && ph_ptr->hybr_pref != CM_HYBR_PREF_CDMA__HDR_WCDMA)
  {
    /* Data is suspended 
    ** UE is already having full service on GW. 
    ** PS call preferences are below phone preferences.
    ** Data available request is pending 
    */

    srv_type = cmph_get_srv_type(CM_CALL_TYPE_PS_DATA); 
    
    /* If srv_type is AUTOMATIC, convert it based on sys mode */
    if (srv_type == CM_SRV_TYPE_AUTOMATIC)
    {
        sys_mode = sd_misc_get_curr_acq_sys_mode (); 
        srv_type = cmph_map_sys_mode_to_srv_type(sys_mode); 
    }

    /* Compute fav_mode_pref.
    ** It is the intersection of phone's mode preference and 
    ** srv_type of the call.
    */
    fav_mode_pref = cmph_misc_intersect_mode_pref(
                       srv_type,
                       ph_ptr->mode_pref
                       );           

    /* Change the mode_pref of all data calls to computed fav_mode_pref
    */
    if (fav_mode_pref != CM_MODE_PREF_NONE)
    {
      (void)cmtask_orig_para_change_mode_pref( CM_SS_MAIN,
                                               CM_ACT_TYPE_DATA_CALL,
                                               fav_mode_pref,
                                               TRUE );
    }
    else
    {
      CM_MSG_MED("Intersection of call_ptr's srv_type and ph_ptr's mode_pref is \
                  NONE. mode_pref not changed", 0, 0, 0);
    }
    
    /*
    ** pump the priority of all data calls over to phone object
    ** phone object
    */
    (void)cmtask_orig_para_change_act_priority( CM_SS_MAIN,
                                                CM_ACT_TYPE_DATA_CALL,
                                                CM_ACT_PRIORITY_BELOW_PH,
                                                CM_ACT_PRIORITY_80,
                                                TRUE );

    /* The pointer to the top element of the priority queue */
    ph_orig_para_top_ptr = cmtask_orig_para_get_top( CM_SS_MAIN ); 

    if ( ph_orig_para_top_ptr!= NULL )
    {
      /* Waiting for ok_to_orig report from SD if there is a call. */
      cmpref_proc_cmd_pref_sys_chgd(
        SD_SS_MAIN,
        SD_SS_PREF_REAS_ORIG_RESUME,
        ph_orig_para_top_ptr->orig.orig_mode,
        cmph_map_cm_mode_pref_to_sd_mode_pref(
        ph_orig_para_top_ptr->orig.orig_mode_pref ),
        (sd_ss_band_pref_e_type) ph_orig_para_top_ptr->orig.orig_band_pref,
        cmph_map_cm_prl_pref_to_sd_prl_pref(ph_orig_para_top_ptr->orig.orig_prl_pref),
        cmph_map_cm_roam_pref_to_sd_roam_pref(ph_orig_para_top_ptr->orig.orig_roam_pref),
        ph_orig_para_top_ptr->orig.orig_int_hybr_pref,
        SD_BAND_PCS,
        SD_BLKSYS_PCS_A,
        SD_SS_AVOID_SYS_IDLE,
        0,
        ph_ptr->plmn,
        cmph_map_cm_srv_domain_pref_to_sd(ph_orig_para_top_ptr->orig.orig_srv_domain_pref),
        cmph_map_cm_acq_order_pref_to_sd( ph_ptr->acq_order_pref),
        NULL);
    }
  }

  #endif

}

/*===========================================================================

FUNCTION cmss_update_srv_ind

DESCRIPTION
  Update the phone object per the new service indicators.

  If service state and/or roaming status is changed, Inform CM clients of
  corresponding events.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmss_update_srv_ind(

    cmss_s_type         *ss_ptr,
        /* Pointer to a phone object */

    const sd_si_info_s_type   *si_info_ptr
       /* Pointer to buffer containing new service info information */
)
/*lint -esym(529,prev_true_gw_srv_status)*/
{
  sys_sys_id_s_type        prev_sid;
    /* Remember the current System Id. */

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF

  #ifdef FEATURE_BCMCS
#error code not present
  #endif

  cmph_s_type *ph_ptr = cmph_ptr();

  sys_srv_status_e_type prev_srv_status  = SYS_SRV_STATUS_NONE;

  sys_time_info_s_type * time_info       = SS_CDMA_TIME_PTR(ss_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( si_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmss_update_srv_ind()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->info.changed_fields  = 0;

  prev_sid              = ss_ptr->info.sys_id;
  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF

  /* Default the values of regn reject info upon getting any service
  ** indication or confirmation from LL
  */
  cm_ss_init_reg_reject_info( &ss_ptr->info.reg_reject_info );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we lost service, set no_srv_uptime to uptime + CMSS_MAX_NO_SRV_TIME.
  ** The no_srv_uptime is checked periodically by the cmss_timer_proc()
  ** function. If no_srv_uptime expires before service is reacquired,
  ** cmss_timer_proc() reports to CM clients of a service lost event.
  */
  switch ( si_info_ptr->srv_status )
  {

    case SYS_SRV_STATUS_NO_SRV:
    {
      dword clk_time = clk_uptime();
      /* Otherwise, if we are in Low Power Mode, this means the users 
      ** somehow decides to turn off the radio. Then we have to report
      ** SERVICE LOST IMMEDIATELY 
      */
      if ((ph_ptr->oprt_mode == SYS_OPRT_MODE_LPM) ||
          (ph_ptr->oprt_mode == SYS_OPRT_MODE_FTM))
      {
        ss_ptr->no_srv_uptime = clk_time;
      }
      /* If we detect the SERVICE LOST, we added in CMSS_MAX_NO_SRV_TIME 
      ** before reporting to UI. This is to improve the user experience. 
      */
      else
      {
        ss_ptr->no_srv_uptime =
          MIN( ss_ptr->no_srv_uptime, clk_time + CMSS_MAX_NO_SRV_TIME );        
      }
      
      ss_ptr->is_pref_srv_acq = TRUE;

      ss_ptr->info.sys_mode   = SYS_SYS_MODE_NO_SRV;
      ss_ptr->info.srv_status = si_info_ptr->srv_status;
      ss_ptr->info.true_srv_status = si_info_ptr->cs_srv_status;
      prev_srv_status         = ss_ptr->main_srv_status;
      ss_ptr->main_srv_status = si_info_ptr->srv_status;
      ss_ptr->info.srv_domain = si_info_ptr->srv_domain;
      ss_ptr->info.current_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;


      /* Here we make cell_srv_ind values to invalid, as this are no more valid
      ** once UE move to NO Service,  and upon regaining of appropriate service, 
      ** cell_srv_ind will be sent by LL. Hence no need to retain old value in CM
      */

      ss_ptr->info.cell_srv_ind.hs_ind         = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
      
      #ifdef FEATURE_HSPA_CALL_STATUS_IND
      ss_ptr->info.cell_srv_ind.hs_call_status = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
      #endif

      ss_ptr->info.cell_srv_ind.dtm_supp       = SYS_DTM_SUPPORT_NOT_AVAIL;
      ss_ptr->info.cell_srv_ind.egprs_supp     = SYS_EGPRS_SUPPORT_NOT_AVAIL;

      /* Since we  moved out of service, clear the field */
      #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
      if ( ( ss_ptr->srv_avail_info.gw_srv_status == SYS_SRV_STATUS_LIMITED ) ||
         ( ss_ptr->srv_avail_info.gw_srv_status == SYS_SRV_STATUS_SRV ) )
      #else
      if ( ( ss_ptr->srv_avail_info.srv_status == SYS_SRV_STATUS_LIMITED ) ||
         ( ss_ptr->srv_avail_info.srv_status == SYS_SRV_STATUS_SRV ) )
      #endif
      {
        ss_ptr->main_srv_inform_uptime = clk_time + CMSS_INFORM_SRV_CHANGE_TIME;
      } 
      else
        /*lint -e{725} Ignore indentation */   
      {
        ss_ptr->main_srv_inform_uptime = CMSS_INFORM_SRV_CHANGE_INTER_NONE;
      }
   
      #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
      switch( ph_ptr->int_hybr_pref )
      {
        case SD_SS_HYBR_PREF_NONE:
          
          ss_ptr->info.hdr_srv_status = si_info_ptr->srv_status;
          ss_ptr->info.gw_srv_status  = si_info_ptr->srv_status;
          ss_ptr->info.true_gw_srv_status = si_info_ptr->cs_srv_status;
          ss_ptr->info.gw_srv_domain  = si_info_ptr->srv_domain;
          ss_ptr->info.gw_sys_mode    = SYS_SYS_MODE_NO_SRV;
          break;

        case SD_SS_HYBR_PREF_CDMA__HDR:

          ss_ptr->info.gw_srv_status  = si_info_ptr->srv_status;
          ss_ptr->info.true_gw_srv_status = si_info_ptr->cs_srv_status;
          ss_ptr->info.gw_srv_domain  = si_info_ptr->srv_domain;
          ss_ptr->info.gw_sys_mode    = SYS_SYS_MODE_NO_SRV;
          break;

        case SD_SS_HYBR_PREF_CDMA__WCDMA:

          ss_ptr->info.hdr_srv_status = si_info_ptr->srv_status;
          break;

        case SD_SS_HYBR_PREF_CDMA__HDR_WCDMA:
        default:
          break;
      }
      #endif

    }
    break;

    case SYS_SRV_STATUS_SRV:
    case SYS_SRV_STATUS_LIMITED:
    {
      dword clk_time = clk_uptime();
      /* Reset the no_srv_uptime.
      */
      ss_ptr->no_srv_uptime   = CMSS_NO_SRV_UPTIME_NONE;
      ss_ptr->info.sys_mode   = SYS_SYS_MODE_CDMA;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Update the CS service status.
      */
      ss_ptr->info.true_srv_status  = si_info_ptr->cs_srv_status;
      /* Update the service status.
      */
      ss_ptr->info.srv_status = si_info_ptr->srv_status;
      prev_srv_status         = ss_ptr->srv_avail_info.srv_status;
      ss_ptr->main_srv_status = si_info_ptr->srv_status;

      /* Copy band/channel information
      */
      ss_ptr->info.active_band    = si_info_ptr->band;
      ss_ptr->info.active_channel = si_info_ptr->chan;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Update the system mode.
      */
      switch( si_info_ptr->mode )
      {
        case SD_MODE_AMPS:
          ss_ptr->info.sys_mode      = SYS_SYS_MODE_AMPS;
          ss_ptr->info.mode_info.cdma_info.srch_win_n = 0;
          ss_ptr->info.mode_info.cdma_info.base_id = 0;
          ss_ptr->info.mode_info.cdma_info.base_lat = 0;
          ss_ptr->info.mode_info.cdma_info.base_long = 0;
          break;

        case SD_MODE_HDR:
          ss_ptr->info.sys_mode      = SYS_SYS_MODE_HDR;
          ss_ptr->info.mode_info.cdma_info.srch_win_n = 0;
          ss_ptr->info.mode_info.cdma_info.base_id = 0;
          ss_ptr->info.mode_info.cdma_info.base_lat = 0;
          ss_ptr->info.mode_info.cdma_info.base_long = 0;
      
          /* Update the HDR active protocol
          */          
          ss_ptr->info.hdr_active_prot = si_info_ptr->hdr_active_prot;
          time_info->sys_mode =  SYS_SYS_MODE_NO_SRV;
          #ifdef FEATURE_C2K_TIME_INFO
          /* Update the HDR time
          */   
          if(si_info_ptr->time_info.sys_mode == SYS_SYS_MODE_HDR)
          {
            time_info->sys_mode = SYS_SYS_MODE_HDR;
            time_info->time.hdr_time.lp_sec     = 
                              si_info_ptr->time_info.time.hdr_time.lp_sec;
            time_info->time.hdr_time.ltm_offset = 
                          si_info_ptr->time_info.time.hdr_time.ltm_offset;
          }
          #endif //FEATURE_C2K_TIME_INFO

          #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
          /* Update the service status.
          */
          ss_ptr->info.hdr_srv_status = si_info_ptr->srv_status;
          ss_ptr->info.srv_status     = SYS_SRV_STATUS_NO_SRV;
          ss_ptr->info.true_srv_status     = SYS_SRV_STATUS_NO_SRV;
          ss_ptr->info.gw_srv_status  = SYS_SRV_STATUS_NO_SRV;
          ss_ptr->info.true_gw_srv_status  = SYS_SRV_STATUS_NO_SRV;
          #endif
          /* Check if the system is usable
          */
          if( si_info_ptr->usable_status == SD_SYS_USABLE_NOT_OK )
          {
            #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
            ss_ptr->info.hdr_srv_status = SYS_SRV_STATUS_NO_SRV;
            #endif
            ss_ptr->info.srv_status = SYS_SRV_STATUS_NO_SRV;
            ss_ptr->info.true_srv_status     = SYS_SRV_STATUS_NO_SRV;
            ss_ptr->main_srv_status = SYS_SRV_STATUS_NO_SRV;
          }
          #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
          prev_srv_status = ss_ptr->srv_avail_info.hdr_srv_status;
          #endif
          
          break;

        case SD_MODE_CDMA:
          ss_ptr->info.sys_mode      = SYS_SYS_MODE_CDMA;
          ss_ptr->info.mode_info.cdma_info.srch_win_n = si_info_ptr->mode_info.cdma.srch_win_n;
          ss_ptr->info.mode_info.cdma_info.base_id = si_info_ptr->mode_info.cdma.base_id;
          ss_ptr->info.mode_info.cdma_info.base_lat = si_info_ptr->mode_info.cdma.base_lat;
          ss_ptr->info.mode_info.cdma_info.base_long = si_info_ptr->mode_info.cdma.base_long;

          /* Update the CDMA time
          */   
          time_info->sys_mode =  SYS_SYS_MODE_NO_SRV;
          
          if(si_info_ptr->time_info.sys_mode == SYS_SYS_MODE_CDMA)
          {
            #ifdef FEATURE_C2K_TIME_INFO
            time_info->sys_mode = SYS_SYS_MODE_CDMA;
            time_info->time.cdma_time.lp_sec 
               = si_info_ptr->time_info.time.cdma_time.lp_sec;
            time_info->time.cdma_time.ltm_offset 
               = si_info_ptr->time_info.time.cdma_time.ltm_offset;
            time_info->time.cdma_time.daylt_savings 
               = si_info_ptr->time_info.time.cdma_time.daylt_savings;
            #endif //FEATURE_C2K_TIME_INFO
      
            /* Convert ltm to signed value for use in HPCD
            **
            ** LTM value is stored in
            ** signed format. If 5th bit has 1
            ** sign extend or else copy as it is
            */
            if (si_info_ptr->time_info.time.cdma_time.ltm_offset & CMSS_LTM_NEG_BIT)
            {
              ss_ptr->ltm_offset =
                (int8)(si_info_ptr->time_info.time.cdma_time.ltm_offset | CMSS_LTM_NEG_SIGN_EXTN);
            }
            else
            {
              ss_ptr->ltm_offset = (int8)si_info_ptr->time_info.time.cdma_time.ltm_offset;
            }
            /* Day light savings info */
            ss_ptr->daylt_savings = si_info_ptr->time_info.time.cdma_time.daylt_savings;
          }
          break;


        case SD_MODE_GPS:
          ss_ptr->info.sys_mode      = SYS_SYS_MODE_GPS;
          break;

        case SD_MODE_GSM:
          #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
          ss_ptr->info.true_gw_srv_status  = si_info_ptr->cs_srv_status;
          ss_ptr->info.gw_srv_status = si_info_ptr->srv_status;
          ss_ptr->info.gw_sys_mode      = SYS_SYS_MODE_GSM;
          #endif
          ss_ptr->info.sys_mode      = SYS_SYS_MODE_GSM;

          #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
          prev_srv_status = ss_ptr->srv_avail_info.gw_srv_status;
          #endif

          /* it is required that CM default local copy of cell_srv_ind values during 
          ** inter-rat W->G, otherwise we see an unnecessary SS event going from 
          ** CM to clients (Default mismatch with local copy). 
          */     
          ss_ptr->info.cell_srv_ind.hs_ind         = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;

          #ifdef FEATURE_HSPA_CALL_STATUS_IND
          ss_ptr->info.cell_srv_ind.hs_call_status = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
          #endif

          break;

        case SD_MODE_WCDMA:
          #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
          ss_ptr->info.true_gw_srv_status  = si_info_ptr->cs_srv_status;
          ss_ptr->info.gw_srv_status = si_info_ptr->srv_status;
          ss_ptr->info.gw_sys_mode      = SYS_SYS_MODE_WCDMA;
          #endif
          ss_ptr->info.sys_mode      = SYS_SYS_MODE_WCDMA;

          #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
          prev_srv_status = ss_ptr->srv_avail_info.gw_srv_status;
          #endif

          /* it is required that CM default local copy of cell_srv_ind values during 
          ** inter-rat G->W, otherwise we see an unnecessary SS event going from 
          ** CM to clients (Default mismatch with local copy). 
          */ 
          ss_ptr->info.cell_srv_ind.dtm_supp    = SYS_DTM_SUPPORT_NOT_AVAIL;
          ss_ptr->info.cell_srv_ind.egprs_supp  = SYS_EGPRS_SUPPORT_NOT_AVAIL;

          break;

        case SD_MODE_INACT:
        case SD_MODE_MAX:
        default:
          CM_CASE_ERR( "sd_mode = %d",si_info_ptr->mode,0,0 );
          /*lint -save -e527 Unreachable
          **     CM_CASE_ERR exits when CM_DEBUG is on
          */
          ss_ptr->info.sys_mode      = SYS_SYS_MODE_NO_SRV;
          break;
          /*lint -restore */

      } /* switch( si_info_ptr->mode ) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
      switch( si_info_ptr->mode )
      {
        case SD_MODE_GSM:
        case SD_MODE_WCDMA:

          /* Update the roaming status.
          */
          ss_ptr->info.gw_roam_status       = (sys_roam_status_e_type) si_info_ptr->roam;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Update the service domain and capability.
          */
          ss_ptr->info.gw_srv_domain       = si_info_ptr->srv_domain;
          ss_ptr->info.gw_srv_capability   = (sys_srv_domain_e_type) (si_info_ptr->sys_srv_cap);

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Update the system forbidden status.
          */
          ss_ptr->info.gw_is_sys_forbidden = si_info_ptr->is_sys_forbidden;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Get the new system id.
          */
          cmss_map_sd_sid_to_sys_sid( si_info_ptr->mode,
                                      &ss_ptr->info.gw_sys_id,
                                      &si_info_ptr->sid );

          ss_ptr->info.gw_active_band = si_info_ptr->band;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          /* Default all the other service indicators
          */

          /* Default the unused indicators
          */
          ss_ptr->info.hdr_roam_status   = SYS_ROAM_STATUS_OFF;
          ss_ptr->info.roam_status       = SYS_ROAM_STATUS_OFF;

          sys_undefine_sys_id( &ss_ptr->info.sys_id );
          sys_undefine_sys_id( &ss_ptr->info.hdr_sys_id );
          
          ss_ptr->info.srv_status       = SYS_SRV_STATUS_NO_SRV;
          ss_ptr->info.hdr_srv_status   = SYS_SRV_STATUS_NO_SRV;

          ss_ptr->info.rssi             = CMSS_RSSI_NO_SIGNAL;
          ss_ptr->info.hdr_rssi         = CMSS_RSSI_NO_SIGNAL;

          ss_ptr->info.ecio             = CMSS_ECIO_VALUE_NO_SIGNAL;
          ss_ptr->info.hdr_ecio         = CMSS_ECIO_VALUE_NO_SIGNAL;

          ss_ptr->info.io               = CMSS_IO_VALUE_NO_SIGNAL;
          ss_ptr->info.hdr_io           = CMSS_IO_VALUE_NO_SIGNAL;

          ss_ptr->info.srv_domain       = SYS_SRV_DOMAIN_NO_SRV;
          ss_ptr->info.srv_capability   = SYS_SRV_DOMAIN_NO_SRV;
          ss_ptr->info.is_sys_prl_match = FALSE;
          ss_ptr->info.is_sys_forbidden = FALSE;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_HDR:

          /* Update the roaming status.
          */
          ss_ptr->info.hdr_roam_status       = (sys_roam_status_e_type) si_info_ptr->roam;

          /* Get the new system id.
          */
          cmss_map_sd_sid_to_sys_sid( si_info_ptr->mode,
                                      &ss_ptr->info.hdr_sys_id,
                                      &si_info_ptr->sid );
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Default the unused indicators
          */
          ss_ptr->info.gw_roam_status     = SYS_ROAM_STATUS_OFF;
          ss_ptr->info.roam_status        = SYS_ROAM_STATUS_OFF;

          sys_undefine_sys_id( &ss_ptr->info.sys_id );
          sys_undefine_sys_id( &ss_ptr->info.gw_sys_id );
          
          ss_ptr->info.srv_status          = SYS_SRV_STATUS_NO_SRV;
          ss_ptr->info.true_srv_status     = SYS_SRV_STATUS_NO_SRV;
          ss_ptr->info.gw_srv_status       = SYS_SRV_STATUS_NO_SRV;
          ss_ptr->info.true_gw_srv_status  = SYS_SRV_STATUS_NO_SRV;

          ss_ptr->info.rssi                = CMSS_RSSI_NO_SIGNAL;
          ss_ptr->info.gw_rssi             = CMSS_RSSI_NO_SIGNAL;

          ss_ptr->info.ecio                = CMSS_ECIO_VALUE_NO_SIGNAL;
          ss_ptr->info.gw_ecio             = CMSS_ECIO_VALUE_NO_SIGNAL_UMTS;

          ss_ptr->info.io                  = CMSS_IO_VALUE_NO_SIGNAL;
          ss_ptr->info.gw_io               = CMSS_IO_VALUE_NO_SIGNAL;
          
          ss_ptr->info.srv_domain          = SYS_SRV_DOMAIN_NO_SRV;
          ss_ptr->info.gw_srv_domain       = SYS_SRV_DOMAIN_NO_SRV;

          ss_ptr->info.gw_srv_capability   = SYS_SRV_DOMAIN_NO_SRV;
          ss_ptr->info.srv_capability      = SYS_SRV_DOMAIN_NO_SRV;

          ss_ptr->info.gw_is_sys_forbidden = FALSE;

          ss_ptr->info.rssi2               = CMSS_RSSI2_NO_SIGNAL;
          ss_ptr->info.gw_rssi2            = CMSS_RSSI2_NO_SIGNAL;

          ss_ptr->info.rscp                = CMSS_RSCP_NO_SIGNAL;
          ss_ptr->info.gw_rscp             = CMSS_RSCP_NO_SIGNAL;

          ss_ptr->info.ecio_for_display    = FALSE ;
          ss_ptr->info.gw_ecio_for_display = FALSE ;

         break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_CDMA:

          /* Update the roaming status.
          */
          ss_ptr->info.roam_status       = (sys_roam_status_e_type) si_info_ptr->roam;

          /* Update the service domain and capability.
          */
          ss_ptr->info.srv_domain       = si_info_ptr->srv_domain;
          ss_ptr->info.srv_capability   = (sys_srv_domain_e_type) (si_info_ptr->sys_srv_cap);
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Get the new system id.
          */
          cmss_map_sd_sid_to_sys_sid( si_info_ptr->mode,
                                      &ss_ptr->info.sys_id,
                                      &si_info_ptr->sid );
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          if( !ss_ptr->info.hdr_hybrid )
          {
            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            /* Default the unused indicators
            */
            ss_ptr->info.hdr_roam_status      = SYS_ROAM_STATUS_OFF;
            ss_ptr->info.gw_roam_status       = SYS_ROAM_STATUS_OFF;

            sys_undefine_sys_id( &ss_ptr->info.gw_sys_id );
            sys_undefine_sys_id( &ss_ptr->info.hdr_sys_id );
            
            ss_ptr->info.gw_srv_status       = SYS_SRV_STATUS_NO_SRV;
            ss_ptr->info.true_gw_srv_status  = SYS_SRV_STATUS_NO_SRV;
            ss_ptr->info.hdr_srv_status      = SYS_SRV_STATUS_NO_SRV;

            ss_ptr->info.hdr_rssi            = CMSS_RSSI_NO_SIGNAL;
            ss_ptr->info.gw_rssi             = CMSS_RSSI_NO_SIGNAL;

            ss_ptr->info.hdr_ecio            = CMSS_ECIO_VALUE_NO_SIGNAL;
            ss_ptr->info.gw_ecio             = CMSS_ECIO_VALUE_NO_SIGNAL_UMTS;

            ss_ptr->info.hdr_io              = CMSS_IO_VALUE_NO_SIGNAL;
            ss_ptr->info.gw_io               = CMSS_IO_VALUE_NO_SIGNAL;

            ss_ptr->info.gw_srv_domain       = SYS_SRV_DOMAIN_NO_SRV;
            ss_ptr->info.gw_srv_capability   = SYS_SRV_DOMAIN_NO_SRV;
            ss_ptr->info.gw_is_sys_forbidden = FALSE;

          }
         break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        default:
          break;
      }
      #else

      /* Update the roaming status.
      */
      ss_ptr->info.roam_status       = (sys_roam_status_e_type) si_info_ptr->roam;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Update the service domain and capability.
      */
      ss_ptr->info.srv_domain       = si_info_ptr->srv_domain;
      ss_ptr->info.srv_capability   = (sys_srv_domain_e_type) (si_info_ptr->sys_srv_cap);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Get the new system id.
      */
      cmss_map_sd_sid_to_sys_sid( si_info_ptr->mode,
                                  &ss_ptr->info.sys_id,
                                  &si_info_ptr->sid );
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     #endif

      /* Update the prl match status.
      */
      ss_ptr->info.is_sys_prl_match = si_info_ptr->is_sys_prl_match;

      /* Update the system forbidden status.
      */
      ss_ptr->info.is_sys_forbidden = si_info_ptr->is_sys_forbidden;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      /* Update the preferred acquisition indicator.
      */
      if(ss_ptr->is_pref_srv_acq  != si_info_ptr->is_pref_srv_acq)
      {
        ss_ptr->is_pref_srv_acq = si_info_ptr->is_pref_srv_acq;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Update the redirection/handoff indicator.
      */
      ss_ptr->is_redir_or_ho = si_info_ptr->is_redir_or_ho;
      ss_ptr->is_colloc      = si_info_ptr->is_colloc;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_USERZONE
      /* if the uz_id stored in ss_ptr is different from si_info_ptr, then
      ** user zone selection has changed, notify CM clients of the
      ** User zone changed event.
      */
      if ( ss_ptr->info.mode_info.cdma_info.uz_id != 
                                            si_info_ptr->curr_uz_info.uz_id )
      {
        ss_ptr->info.mode_info.cdma_info.uz_id   = 
                                             si_info_ptr->curr_uz_info.uz_id;
        ss_ptr->info.mode_info.cdma_info.uz_name = 
                                           si_info_ptr->curr_uz_info.uz_name;
        changed_fields |= CM_SS_UZ_CHANGED_MASK;
      }
      #endif    /* FEATURE_USERZONE */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
      /* Deals with identifying the current country */

      cmss_identify_serving_country ();
#endif
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
      #endif

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Update default roaming indicator
      */
      ss_ptr->info.def_roam_ind = si_info_ptr->def_roam_ind;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* If we were initially in no service or if we move from Limited to
      ** Full service, the inform the clients immediately.
      */
      if (ss_ptr->main_srv_inform_uptime == CMSS_INFORM_SRV_CHANGE_INTER_NONE
          || ss_ptr->main_srv_status > prev_srv_status 
          #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
          || ss_ptr->info.gw_sys_mode != ss_ptr->srv_avail_info.gw_sys_mode
          || ( !cmss_sys_id_match( ss_ptr->srv_avail_info.gw_sys_id, ss_ptr->info.gw_sys_id ) &&
               ( ss_ptr->main_srv_status == prev_srv_status ) )
          #else
          || ss_ptr->info.sys_mode != ss_ptr->srv_avail_info.sys_mode
          || ( !cmss_sys_id_match( ss_ptr->srv_avail_info.sys_id, ss_ptr->info.sys_id ) &&
               ( ss_ptr->main_srv_status == prev_srv_status ) )
          #endif
         )
      {

        /* uptime is CMSS_INFORM_SRV_CHANGE_INTER_NONE when srv indication
        ** transitions to SRV available from PWR_SAVE or NO_SRV. Clients need
        ** to be informed immediately of service here.
        */
        ss_ptr->main_srv_inform_uptime   = clk_time + CMSS_INFORM_SRV_CHANGE_TIME;

        /* srv_avail_info holds serving system parameters last reported to clients by
        ** CM_SS_EVENT_SRV_CHANGED. cmss_update_ss_bit_mask () computes the change in
        ** parameters that need to be reported to clients.
        */
        ss_ptr->info.changed_fields =
          cmss_get_ss_main_info_change (&ss_ptr->info, &ss_ptr->srv_avail_info);

        #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
        ss_ptr->info.changed_fields |=
          cmss_get_ss_hdr_info_change (&ss_ptr->info, &ss_ptr->srv_avail_info);

        ss_ptr->info.changed_fields |= 
          cmss_get_ss_gw_info_change(&ss_ptr->info, &ss_ptr->srv_avail_info);
        #endif
      }
      else
      {
        /* By this time main_srv_inform_uptime is active and serving system
        ** parameter change gets informed once in 2 secs only if there is a change
        ** in parameters.
        */
        ss_ptr->info.changed_fields                = 0;
      }


      /*
      ** If the system identifier has changed, notify CM clients of the
      ** corresponding event.
      */

      #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
      if( ( si_info_ptr->mode == SD_MODE_GSM ||
            si_info_ptr->mode == SD_MODE_WCDMA ) &&
          !cmss_sys_id_match( ss_ptr->info.gw_sys_id, prev_sid )
        )
      #else
      if ( !cmss_sys_id_match( ss_ptr->info.sys_id, prev_sid)  &&
           ( ss_ptr->info.sys_mode == SYS_SYS_MODE_WCDMA ||
             ss_ptr->info.sys_mode == SYS_SYS_MODE_GSM )
         )
      #endif
      {

        #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
        #endif  /* FEATURE_WCDMA || FEATURE_GSM */
      }

      #ifdef FEATURE_UMTS_1X_HANDOVER
#error code not present
      #endif

      break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SYS_SRV_STATUS_PWR_SAVE:
    {
      /* Check if srv_status transitioned to pwr_save.
      */
      #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
      
      switch ( ph_ptr->int_hybr_pref )
      {
        /* If not in hybrid operation, change the service indication to */

      case SD_SS_HYBR_PREF_NONE:

          if ( ss_ptr->info.gw_srv_status != si_info_ptr->srv_status )
          {
            /* Update the gsm/umts service status, true gsm/umts service 
            ** status and update the change field bit mask.
            */
            ss_ptr->info.true_gw_srv_status  = si_info_ptr->srv_status;
            ss_ptr->info.gw_srv_status = si_info_ptr->srv_status;
            ss_ptr->info.changed_fields |= CM_SS_GW_SRV_STATUS_MASK;
          }/*if ( ss_ptr->info.gw_srv_status != si_info_ptr->srv_status )*/
          if ( ss_ptr->info.hdr_srv_status != si_info_ptr->srv_status )
          {
            /* Update the hdr service status and change field bit mask.
            */
            ss_ptr->info.hdr_srv_status = si_info_ptr->srv_status;
            ss_ptr->info.changed_fields |= CM_SS_HDR_SRV_STATUS_MASK;
          }/*if ( ss_ptr->info.hdr_srv_status != si_info_ptr->srv_status )*/
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_SS_HYBR_PREF_CDMA__HDR:

          if ( ss_ptr->info.gw_srv_status != si_info_ptr->srv_status )
          {
            /* If was last succesfully acquired system is gsm/umts,
            ** update true gsm/umts service status and change field bit mask.
            */
            switch(ss_ptr->last_system_reported)
            {
              case SYS_SYS_MODE_GW:
              case SYS_SYS_MODE_GSM:
              case SYS_SYS_MODE_WCDMA:

                ss_ptr->info.true_gw_srv_status  = si_info_ptr->srv_status;
                ss_ptr->info.gw_srv_status = si_info_ptr->srv_status;
                ss_ptr->info.changed_fields |= CM_SS_GW_SRV_STATUS_MASK;
                break;

              default:
                break;
            } /* switch(ss_ptr->last_system_reported) */
             
          } /*if ( ss_ptr->info.gw_srv_status != si_info_ptr->srv_status )*/

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_SS_HYBR_PREF_CDMA__WCDMA:

          if ( ss_ptr->info.hdr_srv_status != si_info_ptr->srv_status )
          {
            /* Update the hdr service status and change field bit mask.
            */
            ss_ptr->info.hdr_srv_status = si_info_ptr->srv_status;
            ss_ptr->info.changed_fields |= CM_SS_HDR_SRV_STATUS_MASK;
          }/*if( ss_ptr->info.hdr_srv_status != si_info_ptr->srv_status)*/
          break;

        case SD_SS_HYBR_PREF_CDMA__HDR_WCDMA:
        default:
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      } /* switch ( ph_ptr->int_hybr_pref ) */

      #endif /* #ifdef FEATURE_CM_TECH_SPEC_SRV_IND */

      if ( ss_ptr->info.srv_status != si_info_ptr->srv_status )
      {
        /* yes, transitioned to pwr_save, generate event and stop
        ** no_srv timer.
        */
        ss_ptr->no_srv_uptime   = CMSS_NO_SRV_UPTIME_NONE;
        ss_ptr->info.srv_status = si_info_ptr->srv_status;
        ss_ptr->main_srv_status = si_info_ptr->srv_status;
        ss_ptr->info.changed_fields |= CM_SS_SRV_STATUS_MASK;

        /* Since we  moved out of service, clear the field */
        ss_ptr->main_srv_inform_uptime = CMSS_INFORM_SRV_CHANGE_INTER_NONE;

      } /* if ( ss_ptr->info.srv_status != si_info_ptr->srv_status ) */
    }
    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      CM_MSG_HIGH("Invalid srv_status %d", si_info_ptr->srv_status, 0, 0 );
      break;

  } /* switch ( si_info_ptr->srv_status ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the priority queue if necessary. As of now, done only for:
  ** - Suspended PS data call and no/limited service.
  */
  cmss_update_prio_queue();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF

  #ifdef FEATURE_BCMCS
#error code not present
  #endif /* FEATURE_BCMCS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update usable status.
  */
  /* This check is done so that CM code can differentiate between faked
  ** no service and a real no service. Used in 
  ** cmcall_is_ok_to_end_hold_orig_period().
  */
  if(si_info_ptr->srv_status == SYS_SRV_STATUS_NO_SRV &&
     si_info_ptr->mode == SD_MODE_HDR)
  {
    ss_ptr->ss_main_usable = SD_SYS_USABLE_UNKNOWN;
  }
  else
  {
    ss_ptr->ss_main_usable = si_info_ptr->usable_status;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if clients have to be notified of srv_changed.
  */
  if ( ss_ptr->info.changed_fields != 0 )
  {

    /* Update the RSSI in the SS object
    */
    cmss_report_rssi( NULL, NULL, FALSE );

    /* Notify CM clients of any changes in the serving system information.
    */
    cmss_event( CM_SS_EVENT_SRV_CHANGED );

  } /* if ( changed_fields != 0 ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_PS_DORMANT_PWR_SAVE
  /* If there is a srv lost while waiting for state to resume,
  ** immediately inform client the failure.
  */
  if( ( ss_ptr->ps_data_stt == CM_PS_DATA_STT_RESUME ) &&
      ( ! sys_srv_status_is_full_srv(si_info_ptr->srv_status) )
    )
  {
    ss_ptr->ps_data_stt = CM_PS_DATA_STT_NONE;
    ss_ptr->no_ps_data_uptime = CMSS_NO_PS_DATA_UPTIME_NONE;

    CM_MSG_MED("Send PS DATA AVAIL FAIL EVENT srv_status = %d ",
               si_info_ptr->srv_status,
               0,
               0 );

    cmss_event( CM_SS_EVENT_PS_DATA_FAIL );
  }
  #endif

} /* cmss_update_srv_ind() */
/*lint +esym(529,prev_true_gw_srv_status)*/

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
#error code not present
#endif /* FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE */


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================
FUNCTION cmss_get_fm_rssi

DESCRIPTION
  This function convert FM RSSI in the DB (database) to RSSI in positive dBm

DEPENDENCIES
  Phone is in the analog mode

RETURNS
  positive dBm from 86 dBM to 113 dBm

SIDE EFFECTS
  none

===========================================================================*/
static uint16 cmss_get_fm_rssi (void)
{

  db_items_value_type cmph_db;
  uint16              rssi;

  db_get( DB_RSSI, &cmph_db );

  /* convert to RSSI in dBm; also make it positive */
  rssi = (uint16)( -(-113+ (sint31)( (cmph_db.rssi * 33) >> 8 ) ) );

  return rssi;

} /* cmss_get_fm_rssi() */



/*===========================================================================
FUNCTION cmss_get_cdma_rssi

DESCRIPTION
  This function gets the CDMA RSSI from the DB

DEPENDENCIES
  Phone is in the digital mode

RETURNS
  positive dBm from 75 dBm to 125 dBm

SIDE EFFECTS
  none

===========================================================================*/
static uint16 cmss_get_cdma_rssi (void)
{
  #ifdef FEATURE_JCDMA

  return (srch_get_cdma_raw_rssi());

  #else

  db_items_value_type cmph_db;

  db_get( DB_CDMA_RSSI,&cmph_db );

  return cmph_db.cdma_rssi ;

  #endif /* FEATURE_JCDMA */

} /* cmss_get_cdma_rssi() */


/*===========================================================================
FUNCTION cmss_get_cdma_ecio

DESCRIPTION
  This function gets the CDMA ECIO from searcher

DEPENDENCIES
  Phone is in the digital mode

RETURNS
  negative Ec/IO value in 0.5 dBm steps.

SIDE EFFECTS
  none

===========================================================================*/
static byte cmss_get_cdma_ecio( void )
{
    byte ecio = CMSS_ECIO_VALUE_NO_SIGNAL;

    ecio = srch_get_cdma_raw_ecio();

    /* Limit the ecio value to 14 (-7) */
    ecio = (ecio < CMSS_ECIO_VALUE_MAX)?CMSS_ECIO_VALUE_MAX:ecio;

    return ecio;

} /* cmss_get_cdma_ecio() */


/*===========================================================================

FUNCTION cmss_xlat_band_class

DESCRIPTION
  translate to CM band class enum

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  CM band class enum

SIDE EFFECTS
  none

===========================================================================*/
static sys_band_class_e_type cmss_xlat_band_class(

    byte band_class
)
{

  return ((sys_band_class_e_type) band_class);

} /* cmss_xlat_band_class() */



/*===========================================================================

FUNCTION cmss_xlat_block_or_system
DESCRIPTION
  translate to CM block or system enum

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  CM block or system enum

SIDE EFFECTS
  none

===========================================================================*/
static sys_blksys_e_type  cmss_xlat_block_or_system(
 nv_block_or_sys_enum_type block_or_system
)
{

    sys_blksys_e_type cm_block_or_system=SYS_BLKSYS_NONE;

    switch(block_or_system)
    {
      case NV_PCS_BLOCK_A:
        cm_block_or_system = SYS_BLKSYS_PCS_A;
        break;
      case NV_PCS_BLOCK_B:
        cm_block_or_system = SYS_BLKSYS_PCS_B;
        break;
      case NV_PCS_BLOCK_C:
        cm_block_or_system = SYS_BLKSYS_PCS_C;
        break;
      case NV_PCS_BLOCK_D:
        cm_block_or_system = SYS_BLKSYS_PCS_D;
        break;
      case NV_PCS_BLOCK_E:
        cm_block_or_system = SYS_BLKSYS_PCS_E;
        break;
      case NV_PCS_BLOCK_F:
        cm_block_or_system = SYS_BLKSYS_PCS_F;
        break;
      case NV_PCS_BLOCK_G:
        cm_block_or_system = SYS_BLKSYS_PCS_G;
        break;
      case NV_PCS_BLOCK_H:
        cm_block_or_system = SYS_BLKSYS_PCS_H;
        break;
      case NV_PCS_BLOCK_I:
        cm_block_or_system = SYS_BLKSYS_PCS_I;
        break;
      case NV_PCS_BLOCK_J:
        cm_block_or_system = SYS_BLKSYS_PCS_J;
        break;
      case NV_PCS_BLOCK_K:
        cm_block_or_system = SYS_BLKSYS_PCS_K;
        break;
      case NV_PCS_BLOCK_L:
        cm_block_or_system = SYS_BLKSYS_PCS_L;
        break;
      case NV_CELLULAR_SYS_A:
        cm_block_or_system = SYS_BLKSYS_PCS_CELL_A;
        break;
      case NV_CELLULAR_SYS_B:
        cm_block_or_system = SYS_BLKSYS_PCS_CELL_B;
        break;

      default:
        CM_ERR("invalid block or system",0,0,0);
        break;
    }

    return cm_block_or_system;
} /* cmss_xlat_block_or_system() */



/*===========================================================================

FUNCTION cmss_band_and_block_update

DESCRIPTION
  Update band and blcok or system informatio

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  if update action is  CMSS_INFO_UPDATE_A_TO_N or CMSS_INFO_UPDATE_D_TO_N
  then srv_sys_info_ptr can be set to NULL

===========================================================================*/
static void cmss_band_and_block_update(
    cmss_update_act_e_type  action,
       /* update action */
    cmss_s_type             *ss_ptr,
       /* SS object pointer */
    const cm_srv_sys_info_type    *srv_sys_info_ptr
      /* serving system info pointer */

)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_ptr           != NULL); 
  CM_ASSERT(srv_sys_info_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(action)
  {
    case CMSS_INFO_UPDATE_N_TO_D:
    case CMSS_INFO_UPDATE_A_TO_D:
    case CMSS_INFO_UPDATE_N_TO_A:
    case CMSS_INFO_UPDATE_D_TO_A:
    case CMSS_INFO_UPDATE_A_TO_A:
    case CMSS_INFO_UPDATE_D_TO_D:
      ss_ptr->info.mode_info.cdma_info.band_class      = cmss_xlat_band_class(srv_sys_info_ptr->band_class);
      ss_ptr->info.mode_info.cdma_info.block_or_system = cmss_xlat_block_or_system(srv_sys_info_ptr->block_or_system);
      break;

    case CMSS_INFO_UPDATE_A_TO_N:
    case CMSS_INFO_UPDATE_D_TO_N:
      ss_ptr->info.mode_info.cdma_info.band_class      = SYS_BAND_CLASS_NONE;
      ss_ptr->info.mode_info.cdma_info.block_or_system = SYS_BLKSYS_NONE;
      break;

    default:
      CM_ERR("Invalid SID update action",0,0,0);
      break;
  }

} /* cmss_band_and_block_update() */



/*===========================================================================

FUNCTION cmss_srv_status_update

DESCRIPTION
  Update srv state info

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmss_srv_status_update(
    cmss_update_act_e_type  action,
       /* update action */
    cmss_s_type             *ss_ptr
       /* SS object pointer */

)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_ptr           != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(action)
  {
    case CMSS_INFO_UPDATE_N_TO_D:
    case CMSS_INFO_UPDATE_N_TO_A:
      ss_ptr->info.srv_status      = SYS_SRV_STATUS_SRV;
      ss_ptr->main_srv_status      = SYS_SRV_STATUS_SRV;
      ss_ptr->info.changed_fields |= CM_SS_SRV_STATUS_MASK;
      break;

    case CMSS_INFO_UPDATE_A_TO_A:
    case CMSS_INFO_UPDATE_A_TO_D:
    case CMSS_INFO_UPDATE_D_TO_D:
    case CMSS_INFO_UPDATE_D_TO_A:
      break;

    case CMSS_INFO_UPDATE_A_TO_N:
    case CMSS_INFO_UPDATE_D_TO_N:
      ss_ptr->info.srv_status      = SYS_SRV_STATUS_NO_SRV;
      ss_ptr->main_srv_status      = SYS_SRV_STATUS_NO_SRV;
      ss_ptr->info.changed_fields |= CM_SS_SRV_STATUS_MASK;
      break;

    default:
      CM_ERR("Invalid Srv status update action",0,0,0);
      break;

  }
} /* cmss_srv_status_update() */



/*===========================================================================

FUNCTION cmss_sid_update

DESCRIPTION
  Update SID info

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  if update action is  CMSS_INFO_UPDATE_A_TO_N or CMSS_INFO_UPDATE_D_TO_N
  then srv_sys_info_ptr can be set to NULL

===========================================================================*/
static void cmss_sid_update(
    cmss_update_act_e_type  action,
       /* update action */
    cmss_s_type             *ss_ptr,
       /* SS object pointer */
    const cm_srv_sys_info_type    *srv_sys_info_ptr
      /* serving system info pointer */

)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_ptr           != NULL); 
  CM_ASSERT(srv_sys_info_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(action)
  {
    case CMSS_INFO_UPDATE_N_TO_D:
    case CMSS_INFO_UPDATE_A_TO_D:
    case CMSS_INFO_UPDATE_N_TO_A:
    case CMSS_INFO_UPDATE_D_TO_A:
    case CMSS_INFO_UPDATE_A_TO_A:
    case CMSS_INFO_UPDATE_D_TO_D:

      if( srv_sys_info_ptr->rcv_fields & CM_SS_SID_MASK )
      {
        if(ss_ptr->info.sys_id.id.is95.sid != srv_sys_info_ptr->sid)
        {
          ss_ptr->info.sys_id.id.is95.sid  = srv_sys_info_ptr->sid;
          ss_ptr->info.changed_fields     |= CM_SS_SID_MASK;
          ss_ptr->info.changed_fields     |= CM_SS_SYS_ID_MASK;
        }
      }
      break;

    case CMSS_INFO_UPDATE_A_TO_N:
    case CMSS_INFO_UPDATE_D_TO_N:

        ss_ptr->info.sys_id.id.is95.sid  = CM_SS_SID_NONE;
        ss_ptr->info.changed_fields     |= CM_SS_SID_MASK;
        ss_ptr->info.changed_fields     |= CM_SS_SYS_ID_MASK;
      break;
    default:
      CM_ERR("Invalid SID update action",0,0,0);
      break;

  }

} /* cmss_sid_update() */



/*===========================================================================

FUNCTION cmss_nid_update

DESCRIPTION
  Update NID info

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  if update action is  CMSS_INFO_UPDATE_A_TO_N or CMSS_INFO_UPDATE_D_TO_N
  then srv_sys_info_ptr can be set to NULL

===========================================================================*/
static void cmss_nid_update(
    cmss_update_act_e_type  action,
       /* update action */
    cmss_s_type             *ss_ptr,
       /* SS object pointer */
    const cm_srv_sys_info_type    *srv_sys_info_ptr
      /* serving system info pointer */

)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_ptr           != NULL); 
  CM_ASSERT(srv_sys_info_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(action)
  {
    case CMSS_INFO_UPDATE_N_TO_D:
    case CMSS_INFO_UPDATE_A_TO_D:
    case CMSS_INFO_UPDATE_D_TO_D:

      if( srv_sys_info_ptr->rcv_fields & CM_SS_NID_MASK )
      {
        if(ss_ptr->info.sys_id.id.is95.nid != srv_sys_info_ptr->nid)
        {
          ss_ptr->info.sys_id.id.is95.nid  = srv_sys_info_ptr->nid;
          ss_ptr->info.changed_fields     |= CM_SS_NID_MASK;
          ss_ptr->info.changed_fields     |= CM_SS_SYS_ID_MASK;
        }
      }
      break;

    case CMSS_INFO_UPDATE_D_TO_A:

      if(ss_ptr->info.sys_id.id.is95.nid != CM_SS_NID_NONE)
      {
        ss_ptr->info.sys_id.id.is95.nid  = CM_SS_NID_NONE;
        ss_ptr->info.changed_fields     |= CM_SS_NID_MASK;
        ss_ptr->info.changed_fields     |= CM_SS_SYS_ID_MASK;
      }

      break;

    case CMSS_INFO_UPDATE_N_TO_A:
    case CMSS_INFO_UPDATE_A_TO_A:
    case CMSS_INFO_UPDATE_A_TO_N:

      break;

    case CMSS_INFO_UPDATE_D_TO_N:

      ss_ptr->info.sys_id.id.is95.nid  = CM_SS_NID_NONE;
      ss_ptr->info.changed_fields     |= CM_SS_NID_MASK;
      ss_ptr->info.changed_fields     |= CM_SS_SYS_ID_MASK;
      break;

    default:
      CM_ERR("Invalid NID update action",0,0,0);
      break;

  }

} /* cmss_nid_update() */



/*===========================================================================

FUNCTION cmss_is95_sys_id_update

DESCRIPTION
  Update the IS95 system identifier

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmss_is95_sys_id_update(
    cmss_update_act_e_type  action,
       /* update action */
    cmss_s_type             *ss_ptr,
       /* SS object pointer */
    const cm_srv_sys_info_type    *srv_sys_info_ptr
      /* serving system info pointer */

)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_ptr           != NULL); 
  CM_ASSERT(srv_sys_info_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmss_sid_update( action, ss_ptr, srv_sys_info_ptr );
  cmss_nid_update( action, ss_ptr, srv_sys_info_ptr );

  /*
  ** If the SID and NID are set to "none", then undefine the
  ** system ID.
  */
  if (ss_ptr->info.changed_fields & CM_SS_SYS_ID_MASK)
  {
    if ((ss_ptr->info.sys_id.id.is95.sid == CM_SS_SID_NONE ) &&
        (ss_ptr->info.sys_id.id.is95.nid == CM_SS_NID_NONE )    )
    {
      sys_undefine_sys_id( &ss_ptr->info.sys_id );
    }
    else
    {
      ss_ptr->info.sys_id.id_type = SYS_SYS_ID_TYPE_IS95;
    }
  }

} /* cmss_is95_sys_id_update() */



/*===========================================================================

FUNCTION cmss_reg_zone_update

DESCRIPTION
  Update registration zone info

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  if update action is  CMSS_INFO_UPDATE_A_TO_N or CMSS_INFO_UPDATE_D_TO_N
  then srv_sys_info_ptr can be set to NULL

===========================================================================*/
static void cmss_reg_zone_update(
    cmss_update_act_e_type  action,
       /* update action */
    cmss_s_type             *ss_ptr,
       /* SS object pointer */
    const cm_srv_sys_info_type    *srv_sys_info_ptr
      /* serving system info pointer */

)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_ptr           != NULL); 
  CM_ASSERT(srv_sys_info_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(action)
  {
    case CMSS_INFO_UPDATE_N_TO_D:
    case CMSS_INFO_UPDATE_A_TO_D:
    case CMSS_INFO_UPDATE_D_TO_D:

      if( srv_sys_info_ptr->rcv_fields & CM_SS_REG_ZONE_MASK )
      {
        if(ss_ptr->info.mode_info.cdma_info.reg_zone != srv_sys_info_ptr->reg_zone)
        {
          ss_ptr->info.mode_info.cdma_info.reg_zone  = srv_sys_info_ptr->reg_zone;
          ss_ptr->info.changed_fields     |= CM_SS_REG_ZONE_MASK;
        }
      }
      break;

    case CMSS_INFO_UPDATE_D_TO_A:

      if(ss_ptr->info.mode_info.cdma_info.reg_zone != CM_SS_REG_ZONE_NONE)
      {
        ss_ptr->info.mode_info.cdma_info.reg_zone  = CM_SS_REG_ZONE_NONE;
        ss_ptr->info.changed_fields     |= CM_SS_REG_ZONE_MASK;
      }
      break;

    case CMSS_INFO_UPDATE_N_TO_A:
    case CMSS_INFO_UPDATE_A_TO_A:
    case CMSS_INFO_UPDATE_A_TO_N:

      break;

    case CMSS_INFO_UPDATE_D_TO_N:

      ss_ptr->info.mode_info.cdma_info.reg_zone  = CM_SS_REG_ZONE_NONE;
      ss_ptr->info.changed_fields     |= CM_SS_REG_ZONE_MASK;
      break;

    default:
      CM_ERR("Invalid registration zone update action",0,0,0);
      break;

  }

} /* cmss_reg_zone_update() */



/*===========================================================================

FUNCTION cmss_packet_zone_update

DESCRIPTION
  Update packet zone info

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  if update action is  CMSS_INFO_UPDATE_A_TO_N or CMSS_INFO_UPDATE_D_TO_N
  then srv_sys_info_ptr can be set to NULL

===========================================================================*/
static void cmss_packet_zone_update(
    cmss_update_act_e_type  action,
       /* update action */
    cmss_s_type             *ss_ptr,
       /* SS object pointer */
    const cm_srv_sys_info_type    *srv_sys_info_ptr
      /* serving system info pointer */

)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_ptr           != NULL); 
  CM_ASSERT(srv_sys_info_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(action)
  {
    case CMSS_INFO_UPDATE_N_TO_D:
    case CMSS_INFO_UPDATE_A_TO_D:
    case CMSS_INFO_UPDATE_D_TO_D:

      if( srv_sys_info_ptr->rcv_fields & CM_SS_PACKET_ZONE_MASK )
      {
        if(ss_ptr->info.mode_info.cdma_info.packet_zone != srv_sys_info_ptr->packet_zone)
        {
          ss_ptr->info.mode_info.cdma_info.packet_zone  = srv_sys_info_ptr->packet_zone;
          ss_ptr->info.changed_fields        |= CM_SS_PACKET_ZONE_MASK;
        }
      }
      break;

    case CMSS_INFO_UPDATE_D_TO_A:

      if(ss_ptr->info.mode_info.cdma_info.packet_zone != CM_SS_PACKET_ZONE_NONE)
      {
        ss_ptr->info.mode_info.cdma_info.packet_zone  = CM_SS_PACKET_ZONE_NONE;
        ss_ptr->info.changed_fields        |= CM_SS_PACKET_ZONE_MASK;
      }
      break;

    case CMSS_INFO_UPDATE_N_TO_A:
    case CMSS_INFO_UPDATE_A_TO_A:
    case CMSS_INFO_UPDATE_A_TO_N:

      break;

    case CMSS_INFO_UPDATE_D_TO_N:

      ss_ptr->info.mode_info.cdma_info.packet_zone  = CM_SS_PACKET_ZONE_NONE;
      ss_ptr->info.changed_fields        |= CM_SS_PACKET_ZONE_MASK;
      break;

    default:
      CM_ERR("Invalid packet zone update action",0,0,0);
      break;

  }

} /* cmss_packet_zone_update() */



/*===========================================================================

FUNCTION cmss_p_rev_update

DESCRIPTION
  Update protocol revision info

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  if update action is  CMSS_INFO_UPDATE_A_TO_N or CMSS_INFO_UPDATE_D_TO_N
  then srv_sys_info_ptr can be set to NULL


===========================================================================*/
static void cmss_p_rev_update(
    cmss_update_act_e_type  action,
       /* update action */
    cmss_s_type             *ss_ptr,
       /* SS object pointer */
    const cm_srv_sys_info_type    *srv_sys_info_ptr
      /* serving system info pointer */

)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_ptr           != NULL); 
  CM_ASSERT(srv_sys_info_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(action)
  {
    case CMSS_INFO_UPDATE_N_TO_D:
    case CMSS_INFO_UPDATE_A_TO_D:
    case CMSS_INFO_UPDATE_D_TO_D:

      if( srv_sys_info_ptr->rcv_fields & CM_SS_BS_P_REV_MASK )
      {
        if(ss_ptr->info.mode_info.cdma_info.bs_p_rev != srv_sys_info_ptr->bs_p_rev)
        {
          ss_ptr->info.mode_info.cdma_info.bs_p_rev  = srv_sys_info_ptr->bs_p_rev;
          ss_ptr->info.changed_fields     |= CM_SS_BS_P_REV_MASK;
        }

      }

      if( srv_sys_info_ptr->rcv_fields & CM_SS_P_REV_IN_USE_MASK )
      {

        if(ss_ptr->info.mode_info.cdma_info.p_rev_in_use != srv_sys_info_ptr->p_rev_in_use)
        {
          ss_ptr->info.mode_info.cdma_info.p_rev_in_use  = srv_sys_info_ptr->p_rev_in_use;
          ss_ptr->info.changed_fields         |= CM_SS_P_REV_IN_USE_MASK;
        }

      }

      #ifdef FEATURE_IS2000_REL_A_SVD
      if ( ss_ptr->info.mode_info.cdma_info.ccs_supported != srv_sys_info_ptr->ccs_supported )
      {
        ss_ptr->info.mode_info.cdma_info.ccs_supported = srv_sys_info_ptr->ccs_supported;
        ss_ptr->info.changed_fields          |= CM_SS_CCS_SUPPORTED_MASK;
      }
      #else
      ss_ptr->info.mode_info.cdma_info.ccs_supported = FALSE;
      #endif

      break;


    case CMSS_INFO_UPDATE_N_TO_A:
    case CMSS_INFO_UPDATE_A_TO_A:
    case CMSS_INFO_UPDATE_A_TO_N:

      break;

    case CMSS_INFO_UPDATE_D_TO_A:
    case CMSS_INFO_UPDATE_D_TO_N:

      ss_ptr->info.mode_info.cdma_info.bs_p_rev      = CM_SS_BS_P_REV_NONE;
      ss_ptr->info.mode_info.cdma_info.p_rev_in_use  = CM_SS_P_REV_IN_USE_NONE;
      ss_ptr->info.changed_fields         |= CM_SS_BS_P_REV_MASK;
      ss_ptr->info.changed_fields         |= CM_SS_P_REV_IN_USE_MASK;
      break;

    default:
      CM_ERR("Invalid p_rev update action",0,0,0);
      break;

  }

} /* cmss_p_rev_update() */

/*===========================================================================

FUNCTION cmss_info_new

DESCRIPTION
  New serving system info is available.

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void cmss_info_new( void )
{
  /* Client just needs an indication that new SS info is available, currently
  ** there is no need to fill in the individual fields.
  */
  cmss_event(CM_SS_EVENT_SRV_NEW);
} /* end cmss_info_new() */



/*===========================================================================

FUNCTION cmss_info_update

DESCRIPTION
  Update serving system info

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  if update action is  CMSS_INFO_UPDATE_A_TO_N or CMSS_INFO_UPDATE_D_TO_N
  then srv_sys_info_ptr can be set to NULL


===========================================================================*/
static void cmss_info_update(
    cmss_update_act_e_type  action,
       /* update action */

    const cm_srv_sys_info_type    *srv_sys_info_ptr
      /* serving system info pointer */

)
{
  cmss_s_type               *ss_ptr = cmss_ptr();
        /* Point at serving system object */

  cmss_srv_status_update( action, ss_ptr );
  cmss_band_and_block_update(action, ss_ptr, srv_sys_info_ptr );
  cmss_is95_sys_id_update( action, ss_ptr, srv_sys_info_ptr );
  cmss_reg_zone_update( action, ss_ptr, srv_sys_info_ptr );
  cmss_packet_zone_update( action, ss_ptr, srv_sys_info_ptr );
  cmss_p_rev_update( action, ss_ptr, srv_sys_info_ptr );

} /* cmss_info_update() */


#if ( defined(FEATURE_HDR_HYBRID) || defined(FEATURE_HYBR_GW))
#error code not present
#endif /* #if ( defined(FEATURE_HDR_HYBRID) || defined(FEATURE_HYBR_GW)) */



/*===========================================================================

FUNCTION cmss_map_sd_mode_to_cm_mode

DESCRIPTION
  Map SD mode to CM system mode

DEPENDENCIES
  None

RETURN VALUE
  CM system mode

SIDE EFFECTS
  None

===========================================================================*/
sys_sys_mode_e_type   cmss_map_sd_mode_to_cm_mode(

       sd_mode_e_type        sd_mode
          /* cm system mode.
          */
)
{
  /* Map CM OTASP activation code to SD band.
  */
  switch( sd_mode )
  {
    case SD_MODE_AMPS:    return SYS_SYS_MODE_AMPS;
    case SD_MODE_CDMA:    return SYS_SYS_MODE_CDMA;
    case SD_MODE_HDR:     return SYS_SYS_MODE_HDR;
    case SD_MODE_WCDMA:   return SYS_SYS_MODE_WCDMA;
    case SD_MODE_GSM:     return SYS_SYS_MODE_GSM;
    case SD_MODE_INACT:   return SYS_SYS_MODE_NO_SRV;
    default:
      CM_CASE_ERR( "sd_mode %d",sd_mode,0,0 );
      /*lint -save -e527 Unreachable
      **     CM_CASE_ERR exits when CM_DEBUG is on
      */
      return SYS_SYS_MODE_NO_SRV;
      /*lint -restore */
  }

} /* map_sd_mode_to_cm_mode() */
#endif /* FEATURE_CDMA_800, FEATURE_CDMA_1900 */

/*===========================================================================

FUNCTION cmss_map_reg_emergtype_to_cm_emergtype

DESCRIPTION
  Emergency type bit masks from REG get mapped to CM type bit masks 

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  CM bit masks

SIDE EFFECTS
  None.
  
===========================================================================*/
uint32 cmss_map_reg_emergtype_to_cm_emergtype ( 

  byte reg_emerg_num_type
    /* Emergency number type
    */
)
{ 

#if defined(FEATURE_GW_ELIST_TFT_PCO)
#error code not present
#else

  SYS_ARG_NOT_USED (reg_emerg_num_type);

  return 0;

#endif /* FEATURE_GW_ELIST_TFT_PCO */

} /* cmss_map_reg_emergtype_to_cm_emergtype () */

/*===========================================================================

FUNCTION cm_ss_init_reg_reject_info

DESCRIPTION
  This function initializes a SS registration reject information structure.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_ss_init_reg_reject_info(

    cm_reg_reject_info_s_type  *cm_reg_reject_info_ptr
        /* The SS regn reject info structure to be initialized */
)
{
  if ( cm_reg_reject_info_ptr != NULL )
  {
    cm_reg_reject_info_ptr->reject_cause = 0;
    cm_reg_reject_info_ptr->reject_srv_domain = SYS_SRV_DOMAIN_NONE;
  } /* if ( cm_reg_reject_info_ptr != NULL ) */
  else
  {
    CM_MSG_ERROR("Null pointer passed for cm_reg_reject_info_ptr", 0, 0, 0);
  }

} /* cm_ss_init_reg_reject_info() */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* FEATURE_WCDMA, FEATURE_GSM */


#ifdef FEATURE_PS_DORMANT_PWR_SAVE

/*===========================================================================

FUNCTION cmss_proc_data_avail_ok_to_orig

DESCRIPTION
   Process the data available request upon ok to orig from SD.


DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static  void  cmss_proc_data_avail_ok_to_orig
(
        const cmss_s_type                    *ss_ptr,
           /* Pointer to a phone object */

        const cm_ok_to_orig_type       *ok_to_orig_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
)
{
  CM_ASSERT( ss_ptr != NULL );

  CM_ASSERT( ok_to_orig_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* PS Data Available Request to MM.
  ** If there is ps service, send the data available request to MM and
  ** update the ps data state.
  */
  if( ( ss_ptr->ps_data_stt == CM_PS_DATA_STT_SRV_ACQ ) &&
      ( ok_to_orig_ptr->si_ok_to_orig.srv_domain == SYS_SRV_DOMAIN_PS_ONLY ||
        ok_to_orig_ptr->si_ok_to_orig.srv_domain == SYS_SRV_DOMAIN_CS_PS ))
  {
    cmss_update_data_avail_stt(
                             ok_to_orig_ptr->si_ok_to_orig.ps_data_suspend );
  }

}/* cmss_update_data_avail_state */
#endif /* FEATURE_PS_DORMANT_PWR_SAVE */

#ifdef FEATURE_CALL_THROTTLE

/*===========================================================================

FUNCTION cmss_orig_thr_tbl_entry_info_put

DESCRIPTION
  This function initializes the table entry information from the status
  
DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/

static void cmss_orig_thr_tbl_entry_info_put( 

  cmss_orig_thr_table_entry_s_type *tbl_entry_ptr,
    /* Pointer to the table entry 
    */
 
  cm_ss_orig_thr_call_status_s_type *thr_call_status
    /* Information about the current call status
    */
)
{

  dword uptime = clk_uptime();
  cmph_s_type *ph_ptr = cmph_ptr();

  SYS_ARG_NOT_CONST(thr_call_status);

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( tbl_entry_ptr != NULL );
  CM_ASSERT( ph_ptr        != NULL );
  CM_ASSERT( thr_call_status != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Depending on the access overload class, add a random amount of time
  */

  tbl_entry_ptr->idx                        = 1;
  tbl_entry_ptr->orig_call_info.call_type   = thr_call_status->call_type;
  tbl_entry_ptr->orig_call_info.sys_id      = thr_call_status->sys_id;
  tbl_entry_ptr->orig_call_info.mode_info   = thr_call_status->mode_info;
  tbl_entry_ptr->orig_call_info.call_status = thr_call_status->call_status;
  if( (int) ph_ptr->accolc < 10)
  {
     /* If the time is not zero then randomize it with access overload class
    */
    tbl_entry_ptr->thr_uptime    = uptime + thr_time[1] + 
           ( thr_time[1] == 0 ? 0 : ph_ptr->accolc * CMSS_ORIG_THR_RND_INT);
  }
  else
  {
    tbl_entry_ptr->thr_uptime    = uptime + thr_time[1];
  }
  CM_MSG_HIGH( "Orig. Thr. Tbl. uptime %d curr uptime %d",
                                      tbl_entry_ptr->thr_uptime, uptime, 0 );
  return;
} /* cmss_orig_thr_tbl_entry_info_put */



/*===========================================================================

FUNCTION cmss_orig_thr_proc_update_tbl

DESCRIPTION
  This function should be called to update the origination throttle table, 
  to clear one entry of the origination table

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  True if throttling 
  False otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
static boolean cmss_orig_thr_proc_update_tbl( 
  const cm_ss_cmd_s_type     *ss_cmd_ptr 
    /* Pointer to the SS command 
    */
)
{
  /* Pointer to serving system object.
  */
  cmss_s_type              *ss_ptr               = cmss_ptr();
  boolean                   curr_thr_status      = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(ss_cmd_ptr != NULL);
  CM_ASSERT(ss_ptr     != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(ss_cmd_ptr->info.orig_thr_act)
  {
    case CM_SS_ORIG_THR_RESET_TBL:
      curr_thr_status = ss_ptr->info.main_thr_status.ss_thr_status;
      cmss_orig_thr_table_reset(ss_ptr->cmss_orig_thr_tbl_ptr);
      ss_ptr->info.main_thr_status.ss_thr_status = FALSE;
      cmss_event(CM_SS_EVENT_ORIG_THR_TBL_UPDATE);
      if( curr_thr_status )
      {
        cmss_event(CM_SS_EVENT_SRV_CHANGED);
      }
      return( FALSE );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SS_ORIG_THR_UPDATE_SS:

    /* We are ignoring NID, so replace it with the wild card nid
    */
    {
      cm_ss_orig_thr_call_status_s_type call_info = ss_cmd_ptr->info.call_info;
      if( call_info.sys_id.id_type == SYS_SYS_ID_TYPE_IS95 )
      {
        call_info.sys_id.id.is95.nid  = SD_WILDCARD_NID;
      }        
      return( cmss_orig_thr_update_tbl_entry_with_call_status( 
                                              ss_ptr->cmss_orig_thr_tbl_ptr, 
                                              &call_info ));
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SS_ORIG_THR_UPDATE_CURR_SS:
    {
      cm_ss_orig_thr_call_status_s_type call_info = 
                                                  ss_cmd_ptr->info.call_info;

      call_info.mode_info.cdma_info = ss_ptr->info.mode_info.cdma_info;
        
      /* Figure out which sys_id to use 
      */
      switch( ss_cmd_ptr->info.mode )
      {
        case SYS_SYS_MODE_CDMA:
          call_info.sys_id = ss_ptr->info.sys_id;
          
          break;

        case SYS_SYS_MODE_HDR:

          #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
          call_info.sys_id = ss_ptr->info.hdr_sys_id;
          #else
          if( ss_ptr->info.hdr_hybrid )
          {
            call_info.sys_id = ss_ptr->info.hdr_sys_id;
          }
          else
          {
            call_info.sys_id = ss_ptr->info.sys_id;
          }
          #endif
          break;

        default:
          call_info.sys_id = ss_ptr->info.sys_id;
          break;
        } /* switch( mode ) */

        /* We are ignoring NID, so replace it with the wild card nid
        */

        if( call_info.sys_id.id_type == SYS_SYS_ID_TYPE_IS95 )
        {
          call_info.sys_id.id.is95.nid  = SD_WILDCARD_NID;
        }
        
        return ( cmss_orig_thr_update_tbl_entry_with_call_status( 
                                              ss_ptr->cmss_orig_thr_tbl_ptr, 
                                              &call_info ));
        
    } /* case CM_SS_ORIG_THR_UPDATE_CURR_SS */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      CM_ERR(" Unknown throttle action %d ",ss_cmd_ptr->info.orig_thr_act,0,0);
      return FALSE;

  }/* switch(ss_cmd_ptr->info.orig_thr_act) */
  
}/* cmss_orig_thr_proc_update_tbl */

#endif /* FEATURE_CALL_THROTTLE */

#ifdef FEATURE_CM_SS_MEAS 
#error code not present
#endif /* FEATURE_CM_SS_MEAS */
/*===========================================================================

FUNCTION cmss_inform_ipapp_reg_status

DESCRIPTION
  Sends SS event to clients reporting ip app registration status.


DEPENDENCIES
  Serving system object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmss_inform_ipapp_reg_status
(

  const  cm_ip_app_reg_status_type        *app_reg_status_ptr
     /* App registration status */     
)
{
  cmss_s_type              *ss_ptr               = cmss_ptr();
    /* Pointer to serving system object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  /* ipapp reg status */
  ss_ptr->info.ipapp_info.reg_status     = app_reg_status_ptr->ipapp_reg_status;  

  /* Call type's that app can service */
  ss_ptr->info.ipapp_info.call_type_mask = app_reg_status_ptr->call_type_mask;

  /* cause for registration, de-registration */
  ss_ptr->info.ipapp_info.cause          = (int)app_reg_status_ptr->ipapp_cause;

  /* Warning info tied to registration */
  ss_ptr->info.ipapp_info.warn_info      = app_reg_status_ptr->warn_info;

  /* The mode preference over which SIP is registered */
  ss_ptr->info.ipapp_info.sys_mode       = app_reg_status_ptr->sys_mode;
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  cmss_event (CM_SS_EVENT_IPAPP_REG_STATUS);

} /* cmss_inform_ipapp_reg_status () */

/*===========================================================================

FUNCTION  cmss_proc_country_selected

DESCRIPTION
  User sid to mcc table is updated with user selection. Either a new sid entry
  is created or a old one is rewritten. In the end the table is written to NV.

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
static void cmss_proc_country_selected(
  const cm_ss_cmd_s_type     *ss_cmd_ptr
    /* Pointer to the SS command
    */
)
{

  /* Pointer to serving system object.
  */
  cmss_s_type              *ss_ptr               = cmss_ptr();

  /* Pointer to Sid to mcc table
  */
  cmss_sid_mcc_assoc_tbl_s_type *sid_mcc_tbl_ptr = cmss_get_sid_mcc_assoc_ptr();

  /* table entry */
  int tbl_entry = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(ss_cmd_ptr != NULL);
  CM_ASSERT(ss_ptr     != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Update sid to mcc table and also update NV
  */

  /* Check if sid is already present in the table */
  for (tbl_entry=0; tbl_entry<sid_mcc_tbl_ptr->sid_mcc_tbl_len; ++tbl_entry)
  {
    if (sid_mcc_tbl_ptr->sid_mcc_tbl[tbl_entry].sid == ss_cmd_ptr->info.sid)
    {
      break;
    }
  }

  /* If no entry is found create or replace */
  if (tbl_entry >= sid_mcc_tbl_ptr->sid_mcc_tbl_len )
  {
    CM_MSG_HIGH ("New sid entry created in sid_mcc tbl \n", 0, 0, 0);

     /* Table is Full, replace with first entry */
     if (sid_mcc_tbl_ptr->sid_mcc_tbl_len >=
                         ARR_SIZE(sid_mcc_tbl_ptr->sid_mcc_tbl))
     {
     /* !!! PURGE THE FIRST ENTRY, MOVE THE TABLE UP AND THEN WRITE AT
     ** BOTTOM !!!!
     */

       /* First entry */
       tbl_entry = 0;

       /* Copy sid into table */
       sid_mcc_tbl_ptr->sid_mcc_tbl[tbl_entry].sid =
                                 ss_cmd_ptr->info.sid;

       /* Copy mcc into table */
       sid_mcc_tbl_ptr->sid_mcc_tbl[tbl_entry].country_code =
                                 ss_cmd_ptr->info.selected_country;
     }
     /* Table is not-Full, create new entry */
     else
     {
       /* Increment the table length */
       tbl_entry = sid_mcc_tbl_ptr->sid_mcc_tbl_len;

       ++sid_mcc_tbl_ptr->sid_mcc_tbl_len;

       /* table entry to be written into shld be always less than
       ** CMSS_SID_MCC_TBL_MAX
       */
       if ( (tbl_entry >= (int)ARR_SIZE(sid_mcc_tbl_ptr->sid_mcc_tbl)) ||
            (sid_mcc_tbl_ptr->sid_mcc_tbl_len >=
                                (int)ARR_SIZE(sid_mcc_tbl_ptr->sid_mcc_tbl))
          )
       {
         CM_ERR ("Length exceeds MAX length %d %d \n",
                           tbl_entry, sid_mcc_tbl_ptr->sid_mcc_tbl_len, 0);

         return;
       }

       CM_ASSERT (tbl_entry < (int)ARR_SIZE(sid_mcc_tbl_ptr->sid_mcc_tbl));

       /* Copy sid into table */
       sid_mcc_tbl_ptr->sid_mcc_tbl[tbl_entry].sid =
                                 ss_cmd_ptr->info.sid;

       /* Copy mcc into table */
       sid_mcc_tbl_ptr->sid_mcc_tbl[tbl_entry].country_code =
                                 ss_cmd_ptr->info.selected_country;

     }
  }
  /* Exisitng sid entry is being updated */
  else
  {

   CM_MSG_HIGH ("Old sid entry updated in sid_mcc tbl \n", 0, 0, 0);

     /* Copy sid into table */
     sid_mcc_tbl_ptr->sid_mcc_tbl[tbl_entry].sid =
                                  ss_cmd_ptr->info.sid;
     /* Copy mcc into table */
     sid_mcc_tbl_ptr->sid_mcc_tbl[tbl_entry].country_code =
                                  ss_cmd_ptr->info.selected_country;
  } /* if (tbl_entry > sid_mcc_tbl_ptr->sid_mcc_tbl_len ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Write the table to NV
  */

  /* Write the length */
  cmnv_item_ptr->usr_sid_to_mcc_assoc_tbl.tbl_len =
                                           sid_mcc_tbl_ptr->sid_mcc_tbl_len;

  /* Update the sid mcc values in the table */
  for (tbl_entry=0; tbl_entry<sid_mcc_tbl_ptr->sid_mcc_tbl_len; ++tbl_entry)
  {
    /* 0-15bits form sid */
    cmnv_item_ptr->usr_sid_to_mcc_assoc_tbl.sid_to_mcc_list[tbl_entry] =
        sid_mcc_tbl_ptr->sid_mcc_tbl[tbl_entry].sid & 0xFFFF;

    /* 16-31 bits form country code */
    cmnv_item_ptr->usr_sid_to_mcc_assoc_tbl.sid_to_mcc_list[tbl_entry] |=
        ((sid_mcc_tbl_ptr->sid_mcc_tbl[tbl_entry].country_code & 0xFFFF) << 16);

  }

  cmnv_write (NV_USR_SID_TO_MCC_ASSOC_TBL_I, cmnv_item_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the selected country to SS object */
  ss_ptr->info.current_mcc = ss_cmd_ptr->info.selected_country;

  /* Send event to reflect country selected by user */
  cmss_event (CM_SS_EVENT_COUNTRY_SELECTED);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* cmss_proc_country_selected */



/**--------------------------------------------------------------------------
** Functions - external
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmss_ptr

DESCRIPTION
  Return a pointer to the one and only serving system object.

  The serving system object is responsible for:
  1. Processing MC serving system related notifications.
  2. Notifying the client list of serving system events.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmss_s_type  *cmss_ptr( void )
{
  static cmss_s_type cmss;       /* The one and only serving system object */

  return &cmss;

} /* cmss_ptr() */


#ifdef FEATURE_CALL_THROTTLE

/*===========================================================================

FUNCTION cmss_orig_thr_tbl_ptr

DESCRIPTION
  Return a pointer to the origination table.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the origination throttle table 

SIDE EFFECTS
  none

===========================================================================*/
static cmss_orig_thr_table_s_type *cmss_orig_thr_tbl_ptr
(
    void
)
{

  static cmss_orig_thr_table_s_type cmss_orig_thr_tbl;

  return &cmss_orig_thr_tbl;

}/* cmss_orig_thre_tbl_ptr */

#endif /* FEATURE_CALL_THROTTLE */

/*===========================================================================

FUNCTION cmss_init

DESCRIPTION
  Initializing the serving system object.

  This function must be called before the serving system object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmss_init( void )
{

  cmss_s_type    *ss_ptr  =  cmss_ptr();
      /* Point at serving system object to be initialized */

  cmss_sid_mcc_assoc_tbl_s_type *sid_mcc_assoc_tbl_ptr = NULL;
      /* Get sid to country association ptr */

  int loop = 0;
      /* For loop interator */

  byte  conv_arr[]        = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
      /* Array for converting binary OTA format to Decimal PRL format*/

  cm_country_code_type  prl_mcc;
      /* Temporay variable for Demcimal PRL MCC*/

  cm_country_code_type  ota_mcc;
      /* Temporay variable for Binary OTA MCC*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss_ptr != NULL );
  CM_FUNC_START( "cmss_init()",0,0,0 );

  /* Verify object was not already initialized.
  ** and then mark object as initialized.
  */
  CM_ASSERT( ! CM_INIT_CHECK(ss_ptr->init_mask) );
  CM_INIT_MARK( ss_ptr->init_mask );

  #ifdef  FEATURE_CALL_THROTTLE
  /* Get the pointer to the cmss_orig_throttle_table_ptr
  */
  ss_ptr->cmss_orig_thr_tbl_ptr = cmss_orig_thr_tbl_ptr();
  #endif  /* FEATURE_CALL_THROTTLE */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize SS object
  */
  cmss_reset();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_IMS_VCC
  /** Register callback with WLAN to receive WLAN link status indications
  */
  cmss_wlan_link_qual_cb_reg(TRUE);
  #endif /* FEATURE_IMS_VCC */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Copy pointer to sid-mcc assoc table */
  sid_mcc_assoc_tbl_ptr = cmss_get_sid_mcc_assoc_ptr();

  /* Read Home CC and user provided sid to
  ** country code mapping from NV
  */
  cmnv_read_wait (NV_HOME_MCC_I, cmnv_item_ptr);
  ss_ptr->info.home_mcc = cmnv_item_ptr->home_mcc;

  /* If home_mcc is not in range make it invalid
  ** !!! UNIFY THESE VALUES WITH SD...Create it in sys.h !!!!!
  */
  if (!BETWEEN(cmnv_item_ptr->home_mcc, 0, CM_MAX_MOBILE_COUNTRY_CODE_VAL))
  {
       
     /* Read from NV the current NAM selection.
     */
     cmnv_read_wait( NV_CURR_NAM_I, cmnv_item_ptr );
     cmnv_item_ptr->imsi_mcc.nam = (byte)cmnv_item_ptr->curr_nam;

     CM_MSG_HIGH (" curr_nam = %d", cmnv_item_ptr->imsi_mcc.nam, 0, 0);
     /* Read from NV the IMSI MCC.
     */
     cmnv_read_wait (NV_IMSI_MCC_I, cmnv_item_ptr);  
     ss_ptr->info.home_mcc = cmnv_item_ptr->imsi_mcc.imsi_mcc;   

     if (!BETWEEN(ss_ptr->info.home_mcc, 0, CM_MAX_MOBILE_COUNTRY_CODE_VAL))
     {
          ss_ptr->info.home_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;
     }    
     else
     {
         ota_mcc= ss_ptr->info.home_mcc;
         prl_mcc =  conv_arr[ ota_mcc%10 ];
         ota_mcc /= 10;
         prl_mcc += conv_arr[ ota_mcc%10 ] * 10;
         ota_mcc /= 10;
         prl_mcc += conv_arr[ ota_mcc%10 ] * 100;
         ss_ptr->info.home_mcc = prl_mcc; 
      }
  }

  CM_MSG_HIGH (" home_mcc = %d", ss_ptr->info.home_mcc, 0, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmnv_read_wait (NV_USR_SID_TO_MCC_ASSOC_TBL_I, cmnv_item_ptr);

  /* Check the length for validity */
  if (!BETWEEN(cmnv_item_ptr->usr_sid_to_mcc_assoc_tbl.tbl_len, 0, NV_SID_TO_MCC_TBL_LEN))
  {
    sid_mcc_assoc_tbl_ptr->sid_mcc_tbl_len = 0;
  }
  else
  {
    sid_mcc_assoc_tbl_ptr->sid_mcc_tbl_len =
                            cmnv_item_ptr->usr_sid_to_mcc_assoc_tbl.tbl_len;
  }

  /* Copy sid to mcc table from NV*/
  for (loop=0;loop<sid_mcc_assoc_tbl_ptr->sid_mcc_tbl_len;++loop)
  {
    /* Copy sid 0-15th bits */
    sid_mcc_assoc_tbl_ptr->sid_mcc_tbl[loop].sid          =
         ((uint16)cmnv_item_ptr->usr_sid_to_mcc_assoc_tbl.sid_to_mcc_list[loop]) & 0xFFFF;

    /* Copy country code 16-31 bits */
    sid_mcc_assoc_tbl_ptr->sid_mcc_tbl[loop].country_code =
       ((uint16)(cmnv_item_ptr->usr_sid_to_mcc_assoc_tbl.sid_to_mcc_list[loop] >> 16)) & 0xFFFF;

  }

  CM_MSG_HIGH (" sid to mcc tbl len = %d", sid_mcc_assoc_tbl_ptr->sid_mcc_tbl_len, 0, 0);

  CM_MSG_HIGH (" first entry sid %d mcc %d = %d",
                 ((uint16)cmnv_item_ptr->usr_sid_to_mcc_assoc_tbl.sid_to_mcc_list[0]) & 0xFFFF,
                 ((uint16)(cmnv_item_ptr->usr_sid_to_mcc_assoc_tbl.sid_to_mcc_list[0] >> 16)) & 0xFFFF,
                 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmnv_read_wait (NV_HS_BASED_PLUS_DIAL_SETTING_I, cmnv_item_ptr);

  ss_ptr->info.plus_dial_setting =
    (cm_hs_based_plus_dial_e_type) cmnv_item_ptr->hs_based_plus_dial_setting;

  CM_MSG_HIGH ("plus dial setting = %d", cmnv_item_ptr->hs_based_plus_dial_setting, 0, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #if (defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK))
#error code not present
  #endif

  #if defined(FEATURE_WCDMA)
#error code not present
  #endif

  /*
  ** Start the RSSI update timer.
  */
  (void) rex_set_timer( &cm_rssi_timer, CM_RSSI_TIME );

} /* cmss_init() */



/*===========================================================================

  FUNCTION cmss_sd_rpt_proc

  DESCRIPTION
    Process reports from System Determination.


  DEPENDENCIES
    Serving system object must have already been initialized with
    cmss_init().

  RETURN VALUE
    none

  SIDE EFFECTS
    none

===========================================================================*/
void cmss_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
)
{

  /*lint -e{826} */
  const cm_sd_rpt_u_type  *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
    /* Pointer to SD report */

  cmss_s_type                   *ss_ptr = cmss_ptr();
      /* Point at serving system object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sd_rpt_ptr != NULL );
  CM_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmss_sd_rpt_proc(), cmd=%d", sd_rpt_ptr->hdr.cmd, 0, 0);

  CM_MSG_LOW("Srv Sys handling client command...",0,0,0);

  /* This is a MC notification, so:
  ** 1. Updating appropriate serving system object fields.
  ** 2. Notifying clients of event.
  */
  switch( sd_rpt_ptr->hdr.cmd )
  {
    /* Service Indicators information is changed.
    */
    case CM_SRV_IND_INFO_F:

      switch(sd_rpt_ptr->srv_ind_info.ss)
      {
          case SD_SS_MAIN:
            #ifdef FEATURE_OOSC_USER_ACTION 
#error code not present
            #endif
            {
              cmss_update_srv_ind( ss_ptr, &sd_rpt_ptr->srv_ind_info.si_info );
            }
            break;
          #if (defined(FEATURE_HDR) || defined(FEATURE_HYBR_GW))
#error code not present
          #endif  /* FEATURE_HDR */

          case SD_SS_HYBR_WLAN:
           cmss_update_wlan_srv_ind( ss_ptr, &sd_rpt_ptr->srv_ind_info.si_info );
           break;

          default:
            CM_ERR("unrecognized SS indicator =%d",
                      sd_rpt_ptr->srv_ind_info.ss,0,0);
      } /*switch sd_rpt_ptr->srv_ind.info.ss */

      cmpref_proc_rpt_srv_ind( sd_rpt_ptr );

      break;

    case CM_OK_TO_ORIG_F:
      #ifdef FEATURE_PS_DORMANT_PWR_SAVE
      cmss_proc_data_avail_ok_to_orig( ss_ptr, &sd_rpt_ptr->ok_to_orig );
      #endif /* FEATURE_PS_DORMANT_PWR_SAVE */
      break;

    case CM_IP_APP_REG_STATUS:

      /* App registration status 
      */
      cmss_inform_ipapp_reg_status (&sd_rpt_ptr->ip_app_reg_status);
      break;

    case CM_SRV_LOST_F:

      /* Service is lost, need to re-align stack
      */
      cmss_proc_srv_lost( sd_rpt_ptr->srv_lost.ss );
      break;

    default:
      break;

  } /* switch( sd_rpt_ptr->hdr.cmd ) */
} /* cmss_sd_rpt_proc() */

/*===========================================================================

FUNCTION cmss_client_cmd_proc

DESCRIPTION
  Process clients' ss commands

DEPENDENCIES
  SS object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmss_client_cmd_proc(

    cm_cmd_s_type  *cmd_ptr
        /* pointer to a CM command */
)
{

  const cm_ss_cmd_s_type    *ss_cmd_ptr = NULL;

      /* Point at serving system object */

  cm_ss_cmd_err_e_type       ss_cmd_err;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmss_client_cmd_proc()",0,0,0 );

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_SS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_cmd_ptr = SS_CMD_PTR( cmd_ptr );

  /* Start by checking whether this is a valid command
  ** relative to the current state of the call/phone.
  */
  ss_cmd_err = cmss_client_cmd_check( cmd_ptr );

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmss_client_cmd_err( cmd_ptr, ss_cmd_err );
  if( ss_cmd_err != CM_SS_CMD_ERR_NOERR )
  {
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  switch(ss_cmd_ptr->cmd)
  {

    case CM_SS_CMD_INFO_GET:
      cmss_send_ss_info( ss_cmd_ptr, CM_SS_EVENT_INFO );
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #ifdef FEATURE_PS_DORMANT_PWR_SAVE
    case CM_SS_CMD_PS_DATA_AVAIL:
      cmss_proc_data_avail( ss_cmd_ptr );
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #endif /* FEATURE_PS_DORMANT_PWR_SAVE */

    #ifdef  FEATURE_CALL_THROTTLE
    case CM_SS_CMD_ORIG_THR_UPDATE_TBL:
      (void) cmss_orig_thr_proc_update_tbl( ss_cmd_ptr );
      break;

    #endif  /* FEATURE_CALL_THROTTLE */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef  FEATURE_CM_SS_MEAS
#error code not present
    #endif  /* FEATURE_CM_SS_MEAS */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SS_CMD_COUNTRY_SELECTED:
      cmss_proc_country_selected ( ss_cmd_ptr );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   default:

     CM_SWITCH_ERR( "Bad SS command %d", ss_cmd_ptr->cmd, 0, 0);

     break;
  }

} /* cmss_client_cmd_proc() */


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

FUNCTION cmss_mc_rpt_proc

DESCRIPTION
  Process MC reports


DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmss_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)
{
  /*lint -save -e826 */
  cm_mc_rpt_type    *cm_mc_rpt_ptr = (cm_mc_rpt_type *) rpt_ptr;
  /*lint -restore */
        /* Pointer to MC report */

  cmss_s_type                   *ss_ptr        = cmss_ptr();
      /* Point at serving system object */

  cm_ss_cdma_info_s_type        *cdma_info_ptr;
      /* Pointer to the CDMA specific serving system information */

  cmss_update_act_e_type        action         = CMSS_INFO_UPDATE_ACT_NONE;
      /* update action */

  sys_sys_mode_e_type           prev_sys_mode;
      /* Previous service state variable */

  sys_srv_status_e_type         prev_srv_status;
      /* Previous service state variable */

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_mc_rpt_ptr != NULL );
  CM_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW("Srv Sys Handling UI Command...",0,0,0);

  cdma_info_ptr = &ss_ptr->info.mode_info.cdma_info;

  /* This is a MC notification, so:
  ** 1. Updating appropriate serving system object fields.
  ** 2. Notifying clients of event.
  */
  switch( cm_mc_rpt_ptr->hdr.cmd )
  {
    /* Serving system info
    */
    case CM_SRV_SYS_INFO_F:

      prev_sys_mode               = ss_ptr->info.sys_mode;
      prev_srv_status             = ss_ptr->info.srv_status;
      ss_ptr->info.changed_fields = 0;
      
      if ( sys_srv_status_is_srv( ss_ptr->info.srv_status ))
      {
        cdma_info_ptr->band_class      =
          cmss_xlat_band_class( cm_mc_rpt_ptr->srv_sys_info.band_class);
        cdma_info_ptr->block_or_system =
          cmss_xlat_block_or_system( cm_mc_rpt_ptr->srv_sys_info.block_or_system);

          /* serving state not changed */
          if ( prev_sys_mode == SYS_SYS_MODE_AMPS )
          {
            action = CMSS_INFO_UPDATE_A_TO_A;
          }
          else if ( prev_sys_mode == SYS_SYS_MODE_CDMA )
          {
            action = CMSS_INFO_UPDATE_D_TO_D;
          }
    
      }

      /* Check if someone has registered for just the SRV Info event.
      */
      cmss_info_new();
      if( action != CMSS_INFO_UPDATE_ACT_NONE )
      {
        cmss_info_update( action, &cm_mc_rpt_ptr->srv_sys_info );
      }

      /* Update the RSSI if the service status changed.
      */
      if( ss_ptr->info.srv_status != prev_srv_status )
      {
        cmss_report_rssi( NULL, NULL, FALSE );
      }

      if ( ss_ptr->info.changed_fields != 0 )
      {
        /*
        ** Inform the clients that the serving system was modified.
        */
        cmss_event( CM_SS_EVENT_SRV_CHANGED );
      }
      break;

    #ifdef FEATURE_JCDMA
    /* Registration Succeeded
    */
    case CM_REG_SUCCESS_F:
      cmss_event(CM_SS_EVENT_REG_SUCCESS);
      break;

    /* Registration Failed
    */
    case CM_REG_FAILURE_F:
      cmss_event(CM_SS_EVENT_REG_FAILURE);
      break;
    #endif /* FEATURE_JCDMA */

    case CM_PSIST_F:

      if( ss_ptr->info.psist != cm_mc_rpt_ptr->psist.psist )
      {
        ss_ptr->info.psist = cm_mc_rpt_ptr->psist.psist;
        ss_ptr->info.changed_fields |= CM_SS_PSIST_CHGD_MASK;
        cmss_event( CM_SS_EVENT_SRV_CHANGED );
        cmph_update_ddtm_status(cmph_ptr());
      }
      break;

    case CM_HDR_LOCATION_NOTIFICATION_STATUS_F:

      #ifdef FEATURE_HDR_HANDOFF
#error code not present
      #endif

      break;

    case CM_HDR_SESSION_OPEN_FAILED:
    case CM_HDR_SESSION_CLOSED:

      #ifdef FEATURE_DEDICATED_C2K_MODE
      if( ss_ptr->is_hdr_session_open )
      {
        CM_MSG_HIGH("DCM: HDR session failed/closed",0,0,0);
        ss_ptr->is_hdr_session_open = FALSE;
        cmph_update_ddtm_status(cmph_ptr());
      }

      cmcall_timer_proc( 0 );
      cmxsms_timer_proc( 0 );
      #endif

      break;

    #ifdef FEATURE_CM_SS_MEAS
#error code not present
    #endif /* FEATURE_CM_SS_MEAS */

    default:
      break;

  }

} /* cmss_mc_rpt_proc() */
#endif   /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */



/*===========================================================================

FUNCTION cmss_timer_proc

DESCRIPTION
  Process timer events.

DEPENDENCIES
  Serving system object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmss_timer_proc(

    cm_timer_event_type    timer_event
        /* Indicate specific timer event.
        **
        ** Note that currently timer event is not being used
        */
)
/*lint -esym(715,timer_event) */
{

  cmss_s_type           *ss_ptr = cmss_ptr();
      /* Point at SS object */

  #if ( defined(FEATURE_HDR_HYBRID) || defined( FEATURE_HYBR_GW ) ||\
        defined( FEATURE_CM_TECH_SPEC_SRV_IND ) || \
      defined(FEATURE_PS_DORMANT_PWR_SAVE) )
  cmph_s_type           *ph_ptr = cmph_ptr();
  #endif
      /* Point at Phone object */
  #ifdef FEATURE_PS_DORMANT_PWR_SAVE
  // jessie need to  verify with Paragk
  cm_orig_q_s_type          *ph_orig_para_top_ptr = 
                                      cmtask_orig_para_get_top( CM_SS_MAIN ); 
      /* The pointer to the top element of the priority queue */

  #endif
  dword                 clk_time = clk_uptime();
      /* Current clock time */

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss_ptr != NULL );
  #if ( defined(FEATURE_HDR_HYBRID) || defined( FEATURE_HYBR_GW ) ||\
        defined( FEATURE_CM_TECH_SPEC_SRV_IND ) || \
      defined(FEATURE_PS_DORMANT_PWR_SAVE) )
  CM_ASSERT( ph_ptr != NULL );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->info.changed_fields = 0;

  /* For targets with System Determination 2.0 support we register a
  ** callback function with system determination during power-up
  ** initialization. This function is called by SD whenever the system
  ** indicators information is changed.
  */

  /* If CMPH_MAX_NO_SRV_TIME expired since we lost service, do the
  ** following:
  */

  /*
  ** We need not inform the client of NO_SRV if it has already
  ** been notified.
  **
  ** If the client has already been notified then no_srv_uptim
  ** will be > 0.
  */

  if(  ss_ptr->no_srv_uptime   <= clk_time &&
      !sys_srv_status_is_srv( ss_ptr->main_srv_status ) )
  {
    /* Set the service state to no-service and the preferred acquisition
    ** indicator to FALSE.
    */

    ss_ptr->no_srv_uptime        = CMSS_NO_SRV_UPTIME_NONE;

    ss_ptr->info.srv_status      = SYS_SRV_STATUS_NO_SRV;
    ss_ptr->info.changed_fields |= CM_SS_SRV_STATUS_MASK;

    #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
    switch (ph_ptr->int_hybr_pref ) 
    {
      case SD_SS_HYBR_PREF_NONE:

        ss_ptr->info.gw_srv_status = SYS_SRV_STATUS_NO_SRV;
        ss_ptr->info.changed_fields |= CM_SS_GW_SRV_STATUS_MASK;
        ss_ptr->info.gw_sys_mode = SYS_SYS_MODE_NO_SRV;
        ss_ptr->info.changed_fields |= CM_SS_GW_SYS_MODE_MASK;
        ss_ptr->info.gw_srv_domain = SYS_SRV_DOMAIN_NO_SRV;

        ss_ptr->info.hdr_srv_status = SYS_SRV_STATUS_NO_SRV;
        ss_ptr->info.changed_fields |= CM_SS_HDR_SRV_STATUS_MASK;
        break;

      case SD_SS_HYBR_PREF_CDMA__HDR:

        ss_ptr->info.gw_srv_status = SYS_SRV_STATUS_NO_SRV;
        ss_ptr->info.changed_fields |= CM_SS_GW_SRV_STATUS_MASK;
        ss_ptr->info.gw_sys_mode = SYS_SYS_MODE_NO_SRV;
        ss_ptr->info.changed_fields |= CM_SS_GW_SYS_MODE_MASK;
        ss_ptr->info.gw_srv_domain = SYS_SRV_DOMAIN_NO_SRV;

        break;

      case SD_SS_HYBR_PREF_CDMA__WCDMA:

        ss_ptr->info.hdr_srv_status = SYS_SRV_STATUS_NO_SRV;
        ss_ptr->info.changed_fields |= CM_SS_HDR_SRV_STATUS_MASK;
        break;

      case SD_SS_HYBR_PREF_CDMA__HDR_WCDMA:
      default:
        break;
    }
    #endif
    ss_ptr->is_pref_srv_acq   = FALSE;
    ss_ptr->info.sys_mode = SYS_SYS_MODE_NO_SRV;
    ss_ptr->info.changed_fields |= CM_SS_SYS_MODE_MASK;

    /* Update the RSSI in the SS object */
    cmss_report_rssi( NULL, NULL, FALSE );

    /* If roaming status changed, notify CM clients of the corresponding
    ** event.
    */
    if( ss_ptr->info.roam_status != SYS_ROAM_STATUS_OFF )
    {
      ss_ptr->info.roam_status     = SYS_ROAM_STATUS_OFF;
      ss_ptr->info.changed_fields |= CM_SS_ROAM_STATUS_MASK;
    }

    #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
    #endif   /* FEATURE_WCDMA || FEATURE_GSM */
  }

  /* Check to end throttling for MAIN and report change in serving
  ** system parameters.
  */
  if ((sys_srv_status_is_srv( ss_ptr->main_srv_status )) &&
      (ss_ptr->main_srv_inform_uptime <= clk_time))
  {

    /* srv_avail_info holds serving system parameters last reported to clients by
    ** CM_SS_EVENT_SRV_CHANGED. cmss_update_ss_bit_mask () computes the change in
    ** parameters that need to be reported to clients.
    */
    ss_ptr->info.changed_fields |=
              cmss_get_ss_main_info_change (&ss_ptr->info, &ss_ptr->srv_avail_info);

    /* Update srv_avail_info if we find srv parameters have changed */
    if (ss_ptr->info.changed_fields != 0)
    {
      /* main_srv_inform_uptime is incremented only if there is a change in
      ** ss_ptr->info from the last time CM_SS_EVENT_SRV_CHANGED was
      ** reported to client. If there is no change this field is cleared
      */
      ss_ptr->main_srv_inform_uptime = clk_time + CMSS_INFORM_SRV_CHANGE_TIME;

    } /* if (ss_ptr->info.changed_fields != 0) */
    else
    {
      ss_ptr->main_srv_inform_uptime = CMSS_INFORM_SRV_CHANGE_INTER_NONE;
    }
  }

  /* Check to end throttling for HDR and report change in serving
  ** system parameters.
  */
  if ((sys_srv_status_is_srv( ss_ptr->hybr_1_srv_status )) &&
      (ss_ptr->hdr_srv_inform_uptime <= clk_time))
  {

    /* srv_avail_info holds serving system parameters last reported to clients by
    ** CM_SS_EVENT_SRV_CHANGED. cmss_update_ss_bit_mask () computes the change in
    ** parameters that need to be reported to clients.
    */
    ss_ptr->info.changed_fields |=
              cmss_get_ss_hdr_info_change (&ss_ptr->info, &ss_ptr->srv_avail_info);

    ss_ptr->info.changed_fields |=
              cmss_get_ss_gw_info_change (&ss_ptr->info, &ss_ptr->srv_avail_info);

    /* Update srv_avail_info if we find srv parameters have changed */
    if (ss_ptr->info.changed_fields != 0)
    {
      /* hdr_srv_inform_uptime is incremented only if there is a change in
      ** ss_ptr->info from the last time CM_SS_EVENT_SRV_CHANGED was
      ** reported to client. If there is no change this field is cleared
      */
      ss_ptr->hdr_srv_inform_uptime = clk_time + CMSS_INFORM_SRV_CHANGE_TIME;

    } /* if (ss_ptr->info.changed_fields != 0) */
    else
    {
      ss_ptr->hdr_srv_inform_uptime = CMSS_INFORM_SRV_CHANGE_INTER_NONE;
    }
  }


  #if (defined (FEATURE_HDR_HYBRID) || defined( FEATURE_HYBR_GW ))
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ss_ptr->wlan_no_srv_uptime <= clk_time &&
      !sys_srv_status_is_srv( ss_ptr->info.wlan_srv_status ))
  {
    /* Update the WLAN_SRV_STATUS field.
    */

    ss_ptr->info.changed_fields |= CM_SS_WLAN_SRV_STATUS_MASK;


    ss_ptr->wlan_no_srv_uptime = CMSS_NO_SRV_UPTIME_NONE;

    /* Update WLAN RSSI so that it indicates NO_RSSI.
    */
    cmss_report_rssi( NULL, NULL, FALSE );


  }

  /* Check to end throttling for WLAN and report change in serving
  ** system parameters.
  */
  if ((sys_srv_status_is_srv( ss_ptr->info.wlan_srv_status )) &&
      (ss_ptr->wlan_srv_inform_uptime <= clk_time))
  {

    /* srv_avail_info holds serving system parameters last reported to clients by
    ** CM_SS_EVENT_SRV_CHANGED. cmss_update_ss_bit_mask () computes the change in
    ** parameters that need to be reported to clients.
    */
    ss_ptr->info.changed_fields |=
              cmss_get_ss_wlan_info_change (&ss_ptr->info, &ss_ptr->srv_avail_info);

    /* Update srv_avail_info if we find srv parameters have changed */
    if (ss_ptr->info.changed_fields != 0)
    {
      /* wlan_srv_inform_uptime is incremented only if there is a change in
      ** ss_ptr->info from the last time CM_SS_EVENT_SRV_CHANGED was
      ** reported to client. If there is no change this field is cleared
      */
      ss_ptr->wlan_srv_inform_uptime = clk_time + CMSS_INFORM_SRV_CHANGE_TIME;

      /* Since WLAN info is changed, lets get current stats.
      */
      cmss_report_wlan_stats();

      /* Update WLAN RSSI.
      */
      cmss_report_rssi(NULL, NULL, FALSE);

    } /* if (ss_ptr->info.changed_fields != 0) */
    else
    {
      ss_ptr->wlan_srv_inform_uptime = CMSS_INFORM_SRV_CHANGE_INTER_NONE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If main or hybrid instance serving status changed, recompute the BCMCS
  ** serving status.
  */
  #ifdef FEATURE_BCMCS
#error code not present
  #endif /* FEATURE_BCMCS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ss_ptr->info.changed_fields != 0 )
  {
    /* Inform the clients that the serving system was modified.
    */
    cmss_event( CM_SS_EVENT_SRV_CHANGED );
  }

  #ifdef FEATURE_PS_DORMANT_PWR_SAVE
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* In the middle of ps data available request, check ps data uptimer.
  ** If timeout, reset ps data available state and timer and info failure to
  ** clients.
  */
  if( ( ss_ptr->ps_data_stt != CM_PS_DATA_STT_NONE ) &&
      ( ss_ptr->no_ps_data_uptime <= clk_time )
    )
  {
    /* 30 seconds timer is expired and we did't get ok to orig or 2 mins NAS timer 
    ** expired and data is not resumed. So update the priority queue. 
    */
    cmss_update_prio_queue();
    
    ss_ptr->ps_data_stt = CM_PS_DATA_STT_NONE;
    ss_ptr->no_ps_data_uptime = CMSS_NO_PS_DATA_UPTIME_NONE;
    cmss_event( CM_SS_EVENT_PS_DATA_FAIL);

    CM_MSG_MED("Send PS DATA AVAIL FAIL EVENT due to timeout", 0, 0, 0 );
  }


  /* If get network list request is done, move ps data avail request to 
  ** waiting for ok_to_orig.
  */
  if( ss_ptr->ps_data_stt == CM_PS_DATA_STT_PENDING &&
      ph_ptr->network_type == CM_MODE_PREF_NONE )
  {
    if ( ph_orig_para_top_ptr != NULL )
    {
      /* Waiting for ok_to_orig report from SD if there is a call. */
      cmpref_proc_cmd_pref_sys_chgd(
        SD_SS_MAIN,
        SD_SS_PREF_REAS_ORIG_RESUME,
        ph_orig_para_top_ptr->orig.orig_mode,
        cmph_map_cm_mode_pref_to_sd_mode_pref(
        ph_orig_para_top_ptr->orig.orig_mode_pref ),
        (sd_ss_band_pref_e_type) ph_orig_para_top_ptr->orig.orig_band_pref,
        cmph_map_cm_prl_pref_to_sd_prl_pref(ph_orig_para_top_ptr->orig.orig_prl_pref),
        cmph_map_cm_roam_pref_to_sd_roam_pref(ph_orig_para_top_ptr->orig.orig_roam_pref),
        ph_orig_para_top_ptr->orig.orig_int_hybr_pref,
        SD_BAND_PCS,
        SD_BLKSYS_PCS_A,
        SD_SS_AVOID_SYS_IDLE,
        0,
        ph_ptr->plmn,
        cmph_map_cm_srv_domain_pref_to_sd(
          ph_orig_para_top_ptr->orig.orig_srv_domain_pref),
        cmph_map_cm_acq_order_pref_to_sd(
          ph_ptr->acq_order_pref),
        NULL
      );

      /* Update ps data state */
      ss_ptr->ps_data_stt = CM_PS_DATA_STT_SRV_ACQ;
      ss_ptr->no_ps_data_uptime = clk_time + CMSS_MAX_NO_PS_DATA_SRV_TIME;
    }
    else
    {
      CM_ERR ("Top element on priority queue of Main Stack is NULL", 0, 0, 0);
    }
  }
  #endif /* FEATURE_PS_DORMANT_PWR_SAVE */
} /* cmss_timer_proc() */
/*lint +esym(715,timer_event) */



/*===========================================================================
FUNCTION cmss_rssi_timer_handle

DESCRIPTION
  This function check the rssi. If RSSI change exceeds the delta threshold,
  CM reports rssi change event to registered clients.

DEPENDENCIES
  Phone object must be initialized

RETURNS
  none
SIDE EFFECTS
  none

===========================================================================*/
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
void cmss_rssi_timer_handle ( void )
{
  cmss_report_rssi(NULL, NULL, FALSE);
} /* cmss_rssi_timer_handle() */
#endif

#ifdef FEATURE_HDR_HANDOFF
#error code not present
#endif // FEATURE_HDR_HANDOFF

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */


/*===========================================================================

FUNCTION cmss_proc_hdr_reselect

DESCRIPTION
  This function should be called when the HDRMC reports Hdr reselect or not
  good for traffic.
  Implemented only for backwards compatability.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           cmss_proc_hdr_reselect
(
        void
)
{

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF
}


/*===========================================================================

FUNCTION cmss_proc_call_connected

DESCRIPTION
  This function should be called when the call enters conversation.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           cmss_proc_call_connected
(
        const cmcall_s_type                  *call_ptr
           /* Pointer to call object which entered conversation.
           */
)
/*lint -esym(714,cmss_proc_call_connected) */
/*lint -esym(765,cmss_proc_call_connected)
** Can't be static, used by cmxcall.c, can't externalize through cmss.h as then we
** run into circular header inclusion between cmxcall.c and cmss.h  
*/ 
/*lint -esym(715,call_ptr)*/
{
   /* Pointer to serving system object.
  */

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF


} /* cmss_proc_call_connected */
/*lint +esym(715,call_ptr)*/


/*===========================================================================

FUNCTION cmss_proc_call_ended

DESCRIPTION
  This function should be called to process the call end event from
  CMCALL.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           cmss_proc_call_ended
(
        void
)
{

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif // FEATURE_HDR_HANDOFF
}

/*===========================================================================

FUNCTION cmss_proc_srv_lost

DESCRIPTION
   Process the service indication from SD/RRC.


DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void                           cmss_proc_srv_lost
(
        sd_ss_e_type           ss
            /* Instance on which service was lost
            */
)
{

  #ifdef FEATURE_WLAN_UNIFORM_SCAN_OOS
#error code not present
  #else

  SYS_ARG_NOT_USED(ss);

  #endif

  return;

} /* cmss_proc_srv_lost */


#ifdef FEATURE_CALL_THROTTLE

/*===========================================================================

FUNCTION cmss_orig_thr_add_table_enrty

DESCRIPTION
  This function adds a new entry to the origination throttle table

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
The throttle status of the added entry

SIDE EFFECTS
  None.
===========================================================================*/
static boolean cmss_orig_thr_add_tbl_entry
(

  cmss_orig_thr_table_s_type *thr_tbl_ptr,
    /* Pointer to the origination throttle table
    */

  cm_ss_orig_thr_call_status_s_type *thr_call_status
    /* Information about the current call status
    */
)
{

  unsigned int i      = 0;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( thr_tbl_ptr != NULL );
  CM_ASSERT( thr_call_status != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if there is a empty slot in the orig throttle table
  */
  if(thr_tbl_ptr->num_valid_entries < 
                                 ARR_SIZE( thr_tbl_ptr->cmss_orig_thr_tbl) )
  {
  
    for(i = 0; i < ARR_SIZE( thr_tbl_ptr->cmss_orig_thr_tbl ); ++i)
    {
      if( thr_tbl_ptr->cmss_orig_thr_tbl[i].orig_call_info.sys_id.id_type ==
             SYS_SYS_ID_TYPE_UNDEFINED )
      {
        break;
      }
    }

    if( i < ARR_SIZE( thr_tbl_ptr->cmss_orig_thr_tbl) )
    {
   
      /* Add this entry in the orig throttle table
      */
      cmss_orig_thr_tbl_entry_info_put( &thr_tbl_ptr->cmss_orig_thr_tbl[i], 
                                        thr_call_status );
      thr_tbl_ptr->num_valid_entries++;

      CM_MSG_HIGH(" Added new entry to Orig. Thr. Tbl. Slots avail. %d",
                    ARR_SIZE( thr_tbl_ptr->cmss_orig_thr_tbl ) - 
                    thr_tbl_ptr->num_valid_entries, 0, 0);

      sd_misc_sys_id_print( thr_call_status->sys_id );
                   
     /* Check if the throttle status is true and return;*/
    
      return(thr_time[thr_tbl_ptr->cmss_orig_thr_tbl[i].idx] > 0 );

    } /* if( i < ARR_SIZE( thr_tbl_ptr->cmss_orig_thr_tbl) ) */ 

  } /* if(thr_table_ptr->num_valid_entries < */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, we need to remove the oldest entry in the orig throttle
  ** table and add this in that slot.
  */
  {
    dword min_adj_time   = (dword) (-1);
    dword curr_adj_time  = 0;
    unsigned int ins_pos = 0;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    for(i=0; i < ARR_SIZE(thr_tbl_ptr->cmss_orig_thr_tbl); ++i)
    { 
      curr_adj_time = thr_tbl_ptr->cmss_orig_thr_tbl[i].thr_uptime - 
                        thr_time[thr_tbl_ptr->cmss_orig_thr_tbl[i].idx];

      CM_ASSERT( thr_tbl_ptr->cmss_orig_thr_tbl[i].thr_uptime >= 
                 thr_time[thr_tbl_ptr->cmss_orig_thr_tbl[i].idx] );

      if( curr_adj_time < min_adj_time )
      {
         min_adj_time = curr_adj_time;
         ins_pos      = i;
      }
    }

    CM_MSG_HIGH( "Deleted entry from Orig. Thr. Tbl.", 0, 0, 0);
    sd_misc_sys_id_print( 
            thr_tbl_ptr->cmss_orig_thr_tbl[ins_pos].orig_call_info.sys_id );

    /* Add this entry in the orig throttle table
    */
    cmss_orig_thr_tbl_entry_info_put(&thr_tbl_ptr->cmss_orig_thr_tbl[ins_pos],
                                      thr_call_status);

    return( thr_time[thr_tbl_ptr->cmss_orig_thr_tbl[ins_pos].idx] > 0 );

  }/* Remove oldest entry */

} /* cmss_orig_thr_add_tbl_entry */



/*===========================================================================

FUNCTION cmss_orig_thr_update_tbl_entry

DESCRIPTION
  This function should be called to update a particular origination throttle
  table with the call status
  
DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  The throttle status of the updated entry.

SIDE EFFECTS
  None.
===========================================================================*/

static boolean cmss_orig_thr_update_tbl_entry(

  cmss_orig_thr_table_entry_s_type *tbl_entry_ptr,
    /* The pointer to the particular table entry 
    */

  const cm_ss_orig_thr_call_status_s_type *thr_call_status
    /* The throttle status of the call
    */
)
{
  cmph_s_type  *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( tbl_entry_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( thr_call_status != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tbl_entry_ptr->orig_call_info.call_status = thr_call_status->call_status;
  tbl_entry_ptr->orig_call_info.call_type   = thr_call_status->call_type;
  tbl_entry_ptr->orig_call_info.sys_id      = thr_call_status->sys_id;
  tbl_entry_ptr->orig_call_info.mode_info   = thr_call_status->mode_info;

  if(tbl_entry_ptr->idx < CMSS_ORIG_THR_MAX_TIMER_INDEX - 1)
  {
    tbl_entry_ptr->idx++;
  }
  #ifdef FEATURE_JCDMA
  else
  {
    /* Restart the counter to 1.
    */
    tbl_entry_ptr->idx = 1;
  }
  #endif

  tbl_entry_ptr->thr_uptime                 = clk_uptime() + 
                                              thr_time[tbl_entry_ptr->idx];

  /* Check if we are starting the timer, if we are then randomize the backoff
  ** time
  */

  if( (tbl_entry_ptr->idx > 0) && (thr_time[tbl_entry_ptr->idx - 1] == 0) && 
      (thr_time[tbl_entry_ptr->idx] !=0) && ((int) ph_ptr->accolc < 10) )
  {
    tbl_entry_ptr->thr_uptime+= ph_ptr->accolc * CMSS_ORIG_THR_RND_INT;
  }
    
  CM_MSG_HIGH( " Orig. Thr. Tbl. updated thr uptime %d uptime %d",
                                            tbl_entry_ptr->thr_uptime,
                                            clk_uptime(),
                                            0 );
  sd_misc_sys_id_print( tbl_entry_ptr->orig_call_info.sys_id );

  if( thr_time[tbl_entry_ptr->idx] > 0)
  {
    return TRUE;
  }

  return FALSE;
} /* cmss_orig_thr_update_tbl_entry */



/*===========================================================================

FUNCTION cmss_is_orig_successful

DESCRIPTION
  This function checks if the throttle reason given is one of the reasons for
  call success
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  is the call was successful
  FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/

static boolean cmss_is_orig_successful(
  cm_ss_orig_thr_reasons_e_type thr_reason
    /* The end reason 
    */
)
{
  switch(thr_reason)
  {

    case CM_SS_ORIG_THR_DS_MIP_UP_SUCCESS:
      /* MIP Success 
      */
    case CM_SS_ORIG_THR_DS_PPP_UP_SUCCESS:
     /* PPP Success 
     */
      return TRUE;

    default:
      return FALSE;
  }
}


/*===========================================================================

FUNCTION cmss_orig_thr_update_tbl_with_call_status

DESCRIPTION
  This function should be called to update the origination throttle table
  with the call status
  
DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  The throttle status of the added/modified entry.

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmss_orig_thr_update_tbl_entry_with_call_status
(
  cmss_orig_thr_table_s_type *thr_tbl_ptr,
    /* Pointer to the origination throttle table
    */
  cm_ss_orig_thr_call_status_s_type *thr_call_status
    /* Information about the current call status
    */
)
/*lint -esym(818, thr_call_status) 
*/ 
{
  cmss_s_type  *ss_ptr                 = cmss_ptr();
  boolean       curr_thr_status        = FALSE;
  boolean       main_thr_status        = FALSE;
  boolean       hdr_thr_status         = FALSE;
  boolean       is_throttled           = FALSE;
  boolean       new_thr_status         = FALSE;

  cmss_orig_thr_table_entry_s_type* thr_tbl_entry_ptr;
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( thr_tbl_ptr != NULL );
  CM_ASSERT( ss_ptr      != NULL );
  CM_ASSERT( thr_call_status != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  curr_thr_status = ss_ptr->info.main_thr_status.ss_thr_status;


  thr_tbl_entry_ptr = cmss_orig_thr_get_orig_tbl_entry( 
                           thr_tbl_ptr, 
                           thr_call_status->sys_id, 
                           thr_call_status->mode_info.cdma_info.band_class,  
                           thr_call_status->mode_info.cdma_info.packet_zone );

  if( thr_tbl_entry_ptr != NULL )
  {
    if( cmss_is_orig_successful(thr_call_status->call_status) )
    {
      /* Reset the enrty as the call was succesful 
      */
      CM_MSG_HIGH( "Reseting Orig. Thr. Tbl. entry",0,0,0 );
      sd_misc_sys_id_print( thr_tbl_entry_ptr->orig_call_info.sys_id );
      cmss_orig_thr_reset_tbl_entry( thr_tbl_entry_ptr );
      thr_tbl_ptr->num_valid_entries--;
      
    } /* cmss_is_call_successful */
    else
    {

      /* If we are not in the throttle timeout, update the throttle table
      ** entry with the latest end reason and Increment the throttle_time.
      */
      if( clk_uptime() > thr_tbl_entry_ptr->thr_uptime )
      {
        is_throttled = cmss_orig_thr_update_tbl_entry( 
                                       thr_tbl_entry_ptr, thr_call_status );
      }
      else
      {
        /* We should never get here.
        */
        CM_ASSERT( FALSE );
      }
                                  
    } /* else cmss_is_call_successful */

  } /* if( thr_tbl_entry_ptr != NULL ) */
  else
  {
    /* We did not find a match for the SID/PZID in the table, add a new
    ** entry.
    */

    /* If we need to add new throttling information, add this entry in the
    ** origination throttle table.
    */
    if( !cmss_is_orig_successful( thr_call_status->call_status ))
    {
      is_throttled = cmss_orig_thr_add_tbl_entry( thr_tbl_ptr, thr_call_status );
    }

    /* Else, return FALSE
    */ 
    else
    {
      CM_MSG_HIGH(" Orig. Thr. Tbl. Entry not found",0,0,0);
      sd_misc_sys_id_print( thr_call_status->sys_id );
      
      return FALSE;
    }
  }/* else if( thr_tbl_entry_ptr != NULL ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* We either added a new entry or modified an exsisting entry, so need to
  ** report this to the clients
  */

  cmss_event( CM_SS_EVENT_ORIG_THR_TBL_UPDATE );

  /* Check if the current throttle status changed
  */
  #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
  if( ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR )
  {
    main_thr_status =  cmss_check_if_orig_thr_true(
                              ss_ptr->info.hdr_sys_id, 
                              ss_ptr->info.mode_info.cdma_info.band_class, 
                              ss_ptr->cmss_orig_thr_tbl_ptr,
                              CM_CALL_TYPE_NONE,
                              ss_ptr->info.mode_info.cdma_info.packet_zone );

  }
  else
  #endif
  {
    main_thr_status = cmss_check_if_orig_thr_true(
                              ss_ptr->info.sys_id, 
                              ss_ptr->info.mode_info.cdma_info.band_class, 
                              ss_ptr->cmss_orig_thr_tbl_ptr,
                              CM_CALL_TYPE_NONE,
                              ss_ptr->info.mode_info.cdma_info.packet_zone );
  }

  hdr_thr_status = TRUE;
  /* Check if Hybrid HDR has service.
  */
  #ifdef FEATURE_HDR_HYBRID
#error code not present
  #endif

  /* Check if both. MAIN and HDR instances are throttled.
  */
  new_thr_status = BOOLEAN( main_thr_status && hdr_thr_status );

  /* If throttle status changed, informed clients.
  */
  if( curr_thr_status != new_thr_status )
  {
    ss_ptr->info.main_thr_status.ss_thr_status = !curr_thr_status;
    cmss_event( CM_SS_EVENT_SRV_CHANGED );
  }

  return ( is_throttled );

}/* cmss_orig_thr_update_tbl_with_call_status */
/*lint +esym(818, thr_call_status) 
*/ 

/*===========================================================================

FUNCTION cmss_map_call_end_status_thr_status

DESCRIPTION
  This function maps the call_end status to the throttle status

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  Throttle reason.

SIDE EFFECTS
  None.
===========================================================================*/
static cm_ss_orig_thr_reasons_e_type cmss_map_call_end_status_to_thr_status
( 
  cm_call_end_e_type call_end_status
    /* The call end reason 
    */
)
{
  switch(call_end_status)
  {

    case CM_CALL_END_INTERCEPT: 
      /* Received Intercept from BS */
      return( CM_SS_ORIG_THR_CM_INTERCEPT );
  
    case CM_CALL_END_REL_SO_REJ:
      /* SO Reject */
       return( CM_SS_ORIG_THR_CM_REL_SO_REJ );
    
    case CM_CALL_END_CD_GEN_OR_BUSY:
      /* Connection Deny with deny code busy or general */
      return( CM_SS_ORIG_THR_CM_CD_GEN_OR_BUSY );

    case CM_CALL_END_CD_BILL_OR_AUTH:
      /* Billing or auth failure */
      return( CM_SS_ORIG_THR_CM_CD_BILL_OR_AUTH );

    default:
      return( CM_SS_ORIG_THR_REASON_NONE );
  
  } /* switch */
} /* cmss_map_call_end_status_to_thr_status */


/*===========================================================================

FUNCTION cmss_check_if_orig_thr_end_status

DESCRIPTION
  This function checks is the origination needs to be throttled due to 
  indication from lower layers

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  True if end status needs to be throttled
  False otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

static boolean cmss_check_if_orig_thr_end_status(
  cm_call_end_e_type end_status
    /* Call end reason
    */
)
{
 switch(end_status)
   {
      case CM_CALL_END_INTERCEPT: 
      case CM_CALL_END_REL_SO_REJ:
       return TRUE;
       /* Return true for all the above cases 
       */
     default:  
       return FALSE;
 } /* switch (end_status) */
}/* cmss_check_if_orig_thr_end_status */


/*===========================================================================

FUNCTION cmss_check_if_orig_thr_true

DESCRIPTION
  This function checks is the origination needs to be throttled

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  True if the origination needs to be throttled
  False otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmss_check_if_orig_thr_true
(
  sys_sys_id_s_type           sys_id,
    /* The sys id of the system, SID,NID/PZID
    */

  sys_band_class_e_type       band_class,
    /* The band class information
    */

  cmss_orig_thr_table_s_type *thr_tbl_ptr,
    /* Origination throttle table pointer
    */

  cm_call_type_e_type         call_type,
    /* Call type - VOICE, SMS, E911, etc
    */

  uint16                      packet_zone
    /* The packet zone id
    */

)
{

  dword uptime = clk_uptime();
  cmss_orig_thr_table_entry_s_type *thr_tbl_entry_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT(thr_tbl_ptr != NULL);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW(" Orig. Thr. Tbl. Looking for sys_id",0,0,0);
  sd_misc_sys_id_print( sys_id );

  thr_tbl_entry_ptr = cmss_orig_thr_get_orig_tbl_entry( 
                           thr_tbl_ptr, sys_id, band_class, packet_zone );

  if( thr_tbl_entry_ptr == NULL )
  {
    return FALSE;
  }

  CM_MSG_HIGH(" Orig. Thr. uptime %d curr uptime %d",
                                  thr_tbl_entry_ptr->thr_uptime, uptime, 0 );
  if( call_type != CM_CALL_TYPE_NONE )
  {
    return( BOOLEAN
            (
             ( call_type == thr_tbl_entry_ptr->orig_call_info.call_type )
                         &&
             ( uptime     < thr_tbl_entry_ptr->thr_uptime )
            )
           );

  }
  if( uptime < thr_tbl_entry_ptr->thr_uptime )
  { 
    return TRUE;
  }

  return FALSE;

} /* cmss_check_if_orig_thr_true */


/*===========================================================================

FUNCTION cmss_initialize_thr_status

DESCRIPTION
  This function initializes the throttle status pointer to the values passed

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void cmss_initialize_thr_status(

  cm_ss_orig_thr_call_status_s_type *throttle_status,
    /* The pointer to be initialized 
    */

  cm_call_type_e_type                call_type,
    /* Call Type 
    */

  cm_call_end_e_type                 end_status,
    /* Call end status 
    */

  const cmss_s_type                       *ss_ptr,
    /* Pointer to the ss object 
    */
  boolean                            is_hdr_sys_id
    /* Flag to use hdr_sys_id
    */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT(throttle_status != NULL);
  CM_ASSERT(ss_ptr          != NULL);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  throttle_status->call_type    = call_type;
  throttle_status->mode_info.cdma_info = ss_ptr->info.mode_info.cdma_info;
  throttle_status->call_status  = 
                          cmss_map_call_end_status_to_thr_status(end_status);

  if( !is_hdr_sys_id )
  {
    throttle_status->sys_id     = ss_ptr->info.sys_id;
  }
  else
  {
    throttle_status->sys_id     = ss_ptr->info.hdr_sys_id;
  }
  
  return;
} /* cmss_initialize_thr_status */
  


/*===========================================================================

FUNCTION cmss_check_if_orig_thr_and_add_entry

DESCRIPTION
  This function checks if the call origination needs to be throttled, and
  if it needs to be throttled then adds a entry in the origination throttle
  table

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  True: If the call is throttled and the entry made in the table
  False: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmss_check_if_orig_thr_and_add_entry(
  const cmss_s_type          *ss_ptr,
    /* The pointer to the serving system object
    */

  cm_call_type_e_type   call_type,
    /* Call type - VOICE, SMS, E911, etc
    */
  cm_call_end_e_type    end_status,
    /* Call end status 
    */
  boolean               is_hdr
    /* Flag to use hdr_sys_id
    */
 )
{

  boolean is_orig_throttle = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( call_type != CM_CALL_TYPE_PS_DATA )
  {
    return FALSE;
  }

  is_orig_throttle = cmss_check_if_orig_thr_end_status( end_status );

  if( is_orig_throttle )
  {
    cm_ss_orig_thr_call_status_s_type throttle_status;

    if( !is_hdr )
    {
      cmss_initialize_thr_status( &throttle_status, 
                                   call_type, 
                                   end_status, 
                                   ss_ptr,
                                   FALSE );
    }
    else
    {
      cmss_initialize_thr_status( &throttle_status, 
                                   call_type, 
                                   end_status, 
                                   ss_ptr,
                                   TRUE );
    }

    /* We are ignoring NID, so replace it with the wild card nid
    */

    if( throttle_status.sys_id.id_type == SYS_SYS_ID_TYPE_IS95 )
    {
      throttle_status.sys_id.id.is95.nid  = SD_WILDCARD_NID;
    }
     
    is_orig_throttle =  cmss_orig_thr_update_tbl_entry_with_call_status(
                                               ss_ptr->cmss_orig_thr_tbl_ptr, 
                                               &throttle_status );
    return( is_orig_throttle ) ;

  } /* if(is_orig_throttle) */

  return FALSE;
}/* cmss_check_if_orig_thr_add_entry */


/*===========================================================================

FUNCTION cmss_misc_set_ss_info_thr_status

DESCRIPTION
  This function ssets the ss_thr_status of the ss object

DEPENDENCIES
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void cmss_misc_set_ss_info_thr_status( boolean new_thr_status )
{
  cmss_ptr()->info.main_thr_status.ss_thr_status = new_thr_status;
  return;

}/* cmss_misc_set_ss_info_thr_status */ 

#endif /* FEATURE_CALL_THROTTLE */


/*===========================================================================

FUNCTION cmss_get_ss_main_info_change

DESCRIPTION
  Compares the serving system information change and returns
  a bit mask holding difference.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  Bit Mask indicating which of the Serving System parameters changed

SIDE EFFECTS
  none

===========================================================================*/
static uint64 cmss_get_ss_main_info_change(

    const cm_mm_ss_info_s_type     *new_srv_info,
        /*  New SS information */

    const cm_mm_ss_info_s_type     *old_srv_info
        /*  Old SS information */
)
{

  uint64                        changed_fields = 0;
      /* Records the changes in SS parameters between new_srv_info and old_srv_info */

  #ifdef FEATURE_C2K_TIME_INFO
  const sys_time_info_s_type         *old_time = CDMA_TIME_PTR(old_srv_info);

  const sys_time_info_s_type         *new_time = CDMA_TIME_PTR(new_srv_info);
  #endif //FEATURE_C2K_TIME_INFO
  if ((new_srv_info != NULL) && (old_srv_info != NULL))
  {

    #ifdef FEATURE_USERZONE
    if (new_srv_info->mode_info.cdma_info.uz_id != old_srv_info->mode_info.cdma_info.uz_id)
    {
      changed_fields |= CM_SS_UZ_CHANGED_MASK;
    }
    #endif    /* FEATURE_USERZONE */

    /*
    ** If service state changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->srv_status != old_srv_info->srv_status )
    {
      changed_fields |= CM_SS_SRV_STATUS_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If true service status changed, 
    ** then also notify CM clients of the event.
    */
    if (new_srv_info->true_srv_status != old_srv_info->true_srv_status )
    {
      changed_fields |= CM_SS_TRUE_SRV_STATUS_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If system mode changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->sys_mode != old_srv_info->sys_mode )
    {
      changed_fields |= CM_SS_SYS_MODE_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If active band changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->active_band != old_srv_info->active_band )
    {
      changed_fields |= CM_SS_ACTIVE_BAND_MASK;
    }

    /*
    ** If active chan changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->active_channel != old_srv_info->active_channel )
    {
      changed_fields |= CM_SS_ACTIVE_CHANNEL_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If roaming status changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->roam_status != old_srv_info->roam_status )
    {
      changed_fields |= CM_SS_ROAM_STATUS_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If srv domain changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->srv_domain != old_srv_info->srv_domain )
    {
      changed_fields |= CM_SS_SRV_DOMAIN_MASK;
    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If srv capability changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->srv_capability != old_srv_info->srv_capability )
    {
      changed_fields |= CM_SS_SRV_CAPABILITY_MASK;
    }

    /*
    ** If the system identifier has changed, notify CM clients of the
    ** corresponding event.
    */
    if ( !cmss_sys_id_match( new_srv_info->sys_id, old_srv_info->sys_id ) )
    {
      changed_fields |= CM_SS_SYS_ID_MASK;

      #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
      #endif  /* FEATURE_WCDMA || FEATURE_GSM */

      /*
      ** Decide whether the SID and NID Mask fields need to be set also.
      */
      if ((new_srv_info->sys_id.id_type == SYS_SYS_ID_TYPE_IS95) &&
          (old_srv_info->sys_id.id_type == SYS_SYS_ID_TYPE_IS95))
      {
        if (new_srv_info->sys_id.id.is95.nid != old_srv_info->sys_id.id.is95.nid)
        {
          changed_fields |= CM_SS_NID_MASK;
        }

        if (new_srv_info->sys_id.id.is95.sid != old_srv_info->sys_id.id.is95.sid)
        {
          changed_fields |= CM_SS_SID_MASK;
        }
        if( new_srv_info->sys_id.id.is95.mcc != old_srv_info->sys_id.id.is95.mcc )
        {
          changed_fields |= CM_SS_MCC_MASK;
        }

        if(new_srv_info->sys_id.id.is95.imsi_11_12 != old_srv_info->sys_id.id.is95.imsi_11_12 )
        {
          changed_fields |= CM_SS_IMSI_11_12_MASK;
        }
      }
      else if ((new_srv_info->sys_id.id_type == SYS_SYS_ID_TYPE_IS95) &&
               (old_srv_info->sys_id.id_type != SYS_SYS_ID_TYPE_IS95))
      {
         changed_fields |= CM_SS_NID_MASK;
         changed_fields |= CM_SS_SID_MASK;
         changed_fields |= CM_SS_MCC_MASK;
         changed_fields |= CM_SS_IMSI_11_12_MASK;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If default roaming indicator changed, notify CM clients of the
    ** corresponding event.
    */
    if ( new_srv_info->def_roam_ind != old_srv_info->def_roam_ind )
    {
      changed_fields |= CM_SS_DEF_ROAM_IND_MASK;
    }

    /*
    ** If prl match status changed, notify CM clients of the
    ** corresponding event.
    */
    if ( new_srv_info->is_sys_prl_match != old_srv_info->is_sys_prl_match )
    {
      changed_fields |= CM_SS_SYS_PRL_MATCH_MASK;
    }

    /*
    ** If system forbidden status changed, notify CM clients of the
    ** corresponding event.
    */
    if ( new_srv_info->is_sys_forbidden != old_srv_info->is_sys_forbidden )
    {
      changed_fields |= CM_SS_SYS_FORBIDDEN_MASK;
    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** Check if any mode specific info changed - base station lat or long or
    ** base id or srch_win_n.
    */
    if ( new_srv_info->sys_mode == SYS_SYS_MODE_CDMA  &&
         old_srv_info->sys_mode == new_srv_info->sys_mode &&
         ( new_srv_info->mode_info.cdma_info.base_lat !=
                                 old_srv_info->mode_info.cdma_info.base_lat ||
           new_srv_info->mode_info.cdma_info.base_long !=
                                 old_srv_info->mode_info.cdma_info.base_long ||
           new_srv_info->mode_info.cdma_info.base_id !=
                                     old_srv_info->mode_info.cdma_info.base_id ||
           new_srv_info->mode_info.cdma_info.srch_win_n !=
                                     old_srv_info->mode_info.cdma_info.srch_win_n
         )
       )
    {
      changed_fields |= CM_SS_BASE_STATION_PARMS_CHGD_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If throttle status changed notify CM clients of the corresponding
    ** event.
    */
    #ifdef FEATURE_CALL_THROTTLE
    if ( new_srv_info->main_thr_status.ss_thr_status != old_srv_info->main_thr_status.ss_thr_status )
    {
      changed_fields |= CM_SS_ORIG_THR_STATUS_MASK;
    }
    #endif /* FEATURE_CALL_THROTTLE */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If the HDR system identifier has changed, notify CM clients of the
    ** corresponding event.
    */
    #ifdef FEATURE_HDR_HYBRID
#error code not present
    #endif /* FEATURE_HYBR_HDR */

    if( new_srv_info->psist != old_srv_info->psist )
    {
      changed_fields |= CM_SS_PSIST_CHGD_MASK;
    }

    #ifdef FEATURE_BCMCS
#error code not present
    #endif
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -save -e539 -e525 */
    /* If HS indication has changed, set mask
    */
    if( new_srv_info->cell_srv_ind.hs_ind 
    != old_srv_info->cell_srv_ind.hs_ind )
    {
      changed_fields |= CM_SS_SRV_IND_MASK;
    }

  /* If HS call indication has changed, set mask
    */
  if ( new_srv_info->cell_srv_ind.hs_call_status
    != old_srv_info->cell_srv_ind.hs_call_status)
    {
      changed_fields |= CM_SS_SRV_IND_MASK;
    }

    /* If DTM indication has changed, set mask
    */
    if ( new_srv_info->cell_srv_ind.dtm_supp 
    != old_srv_info->cell_srv_ind.dtm_supp )
    {
      changed_fields |= CM_SS_SRV_IND_MASK;
    }
    /* If EGPRS indication has changed, set mask
    */
    if ( new_srv_info->cell_srv_ind.egprs_supp 
    != old_srv_info->cell_srv_ind.egprs_supp )
    {
      changed_fields |= CM_SS_SRV_IND_MASK;
    }

    /*
    ** Check if cell_info specific info changed
    */
    #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
    if((new_srv_info->gw_cell_info.cell_id != old_srv_info->gw_cell_info.cell_id )||
       (new_srv_info->gw_cell_info.plmn_id.identity[0] != old_srv_info->gw_cell_info.plmn_id.identity[0]) ||
       (new_srv_info->gw_cell_info.plmn_id.identity[1] != old_srv_info->gw_cell_info.plmn_id.identity[1]) ||
       (new_srv_info->gw_cell_info.plmn_id.identity[2] != old_srv_info->gw_cell_info.plmn_id.identity[2]))
    #else
    if ((new_srv_info->cell_info.cell_id != old_srv_info->cell_info.cell_id )||
        (new_srv_info->cell_info.plmn_id.identity[0] != old_srv_info->cell_info.plmn_id.identity[0]) ||
        (new_srv_info->cell_info.plmn_id.identity[1] != old_srv_info->cell_info.plmn_id.identity[1]) ||
        (new_srv_info->cell_info.plmn_id.identity[2] != old_srv_info->cell_info.plmn_id.identity[2]))
    #endif
    {
      changed_fields |= CM_SS_CELL_INFO_MASK;
    }

  #ifdef FEATURE_HYBR_GW
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*lint -restore */

  #ifdef FEATURE_HDR
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CM_TECH_SPEC_SRV_IND

  /*
  ** If service state changed, notify CM clients of the corresponding
  ** event.
  */
  if( !new_srv_info->hdr_hybrid )
  {
    if ( new_srv_info->gw_srv_status != old_srv_info->gw_srv_status )
    {
      changed_fields |= CM_SS_GW_SRV_STATUS_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If true gw service status changed, 
    ** then also notify CM clients of the event.
    */
    if (new_srv_info->true_gw_srv_status != old_srv_info->true_gw_srv_status )
    {
      changed_fields |= CM_SS_TRUE_GW_SRV_STATUS_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -restore */
    /*
    ** If system mode changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->gw_sys_mode != old_srv_info->gw_sys_mode )
    {
      changed_fields |= CM_SS_GW_SYS_MODE_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If active band changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->gw_active_band != old_srv_info->gw_active_band )
    {
      changed_fields |= CM_SS_GW_ACTIVE_BAND_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If roaming status changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->gw_roam_status != old_srv_info->gw_roam_status )
    {
      changed_fields |= CM_SS_GW_ROAM_STATUS_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If srv domain changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->gw_srv_domain != old_srv_info->gw_srv_domain )
    {
      changed_fields |= CM_SS_GW_SRV_DOMAIN_MASK;
    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If srv capability changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->gw_srv_capability != old_srv_info->gw_srv_capability )
    {
      changed_fields |= CM_SS_GW_SRV_CAPABILITY_MASK;
    }

    /*
    ** If the system identifier has changed, notify CM clients of the
    ** corresponding event.
    */
    if ( !cmss_sys_id_match( new_srv_info->gw_sys_id, old_srv_info->gw_sys_id ) )
    {
      changed_fields |= CM_SS_GW_SYS_ID_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If system forbidden status changed, notify CM clients of the
    ** corresponding event.
    */
    if ( new_srv_info->gw_is_sys_forbidden != old_srv_info->gw_is_sys_forbidden )
    {
      changed_fields |= CM_SS_GW_SYS_FORBIDDEN_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If service state changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->hdr_srv_status != old_srv_info->hdr_srv_status )
    {
      changed_fields |= CM_SS_HDR_SRV_STATUS_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If roaming status changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->hdr_roam_status != old_srv_info->hdr_roam_status )
    {
      changed_fields |= CM_SS_HDR_ROAM_STATUS_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If the system identifier has changed, notify CM clients of the
    ** corresponding event.
    */
    if ( !cmss_sys_id_match( new_srv_info->hdr_sys_id, old_srv_info->hdr_sys_id ) )
    {
      changed_fields |= CM_SS_HDR_SYS_ID_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } /* if( !new_srv_info->hdr_hybrid ) */

  #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #ifdef FEATURE_CM_GAN
#error code not present
    #endif /* FEATURE_CM_GAN */
  #ifdef FEATURE_C2K_TIME_INFO
  /*
  ** If there is a CDMA time change, notify clients 
  */
  if(new_time->sys_mode != old_time->sys_mode)
  {
    CM_MSG_HIGH("time:main-old mode %d new mode %d",
                                 old_time->sys_mode,
                                 new_time->sys_mode,
                                                 0); 
    changed_fields |= CM_SS_CDMA_TIME_CHGD_MASK;
  }

  if (new_time->sys_mode == SYS_SYS_MODE_CDMA)
  {
    if( (new_time->time.cdma_time.daylt_savings !=
         old_time->time.cdma_time.daylt_savings) ||
        (new_time->time.cdma_time.ltm_offset !=
         old_time->time.cdma_time.ltm_offset) ||
        (new_time->time.cdma_time.lp_sec !=
         old_time->time.cdma_time.lp_sec) )
    {
      changed_fields |= CM_SS_CDMA_TIME_CHGD_MASK;
      CM_MSG_HIGH("time:cdma - old daylt %d new daylt %d old ltm %d",
                              old_time->time.cdma_time.daylt_savings,
                              new_time->time.cdma_time.daylt_savings,
                                old_time->time.cdma_time.ltm_offset); 
      CM_MSG_HIGH("new ltm %d old lpsec %d new lpsec %d",
                     new_time->time.cdma_time.ltm_offset,
                         old_time->time.cdma_time.lp_sec,
                        new_time->time.cdma_time.lp_sec); 
    }
  }//if (new_time->sys_mode == SYS_SYS_MODE_CDMA)
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (new_time->sys_mode == SYS_SYS_MODE_HDR)
  {
    if( (new_time->time.hdr_time.ltm_offset != old_time->time.hdr_time.ltm_offset) ||
        (new_time->time.hdr_time.lp_sec     != old_time->time.hdr_time.lp_sec) )
    {
      changed_fields |= CM_SS_CDMA_TIME_CHGD_MASK;
      CM_MSG_HIGH("time:hdr - old ltm %d new ltm %d ",
                   old_time->time.hdr_time.ltm_offset,
                   new_time->time.hdr_time.ltm_offset,
                                                   0); 
      CM_MSG_HIGH("old lpsec %d new lpsec %d",
               old_time->time.hdr_time.lp_sec,
               new_time->time.hdr_time.lp_sec,
                                           0); 
    }
  } //if (new_time->sys_mode == SYS_SYS_MODE_HDR)
  #endif //FEATURE_C2K_TIME_INFO
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* if ((new_srv_info != NULL) && (old_srv_info != NULL)) */
  
  return changed_fields;

} /* cmss_get_ss_main_info_change () */


/*===========================================================================

FUNCTION cmss_is_cdma_svd_supported

DESCRIPTION
  This function should be called to check if SVD is supported on 1x.

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean                                         cmss_is_cdma_svd_supported
(
        void
)
{
  /* Pointer to serving system object.
  */
  cmss_s_type              *ss_ptr               = cmss_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if we have service on 1x and ccs is supported.
  */
  if ( sys_srv_status_is_srv(ss_ptr->info.srv_status) &&
       ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA &&
       ss_ptr->info.mode_info.cdma_info.ccs_supported
     )
  {
    /* CCS supported.
    */
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* CCS not supported.
  */
  return FALSE;

} /* cmss_is_cdma_svd_supported() */


/*===========================================================================

FUNCTION cmss_get_ss_hdr_info_change

DESCRIPTION
  Compares the serving system information change and returns
  a bit mask holding difference.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  Bit Mask indicating which of the Serving System parameters changed

SIDE EFFECTS
  none

===========================================================================*/
static uint64 cmss_get_ss_hdr_info_change(

    const cm_mm_ss_info_s_type     *new_srv_info,
        /*  New SS information */

    const cm_mm_ss_info_s_type     *old_srv_info
        /*  Old SS information */
)
{

  uint64                        changed_fields = 0;
        /* Records the changes in SS parameters between new_srv_info and old_srv_info */
  
  #ifdef FEATURE_C2K_TIME_INFO
  const sys_time_info_s_type         *old_time = CDMA_TIME_PTR(old_srv_info);

  const sys_time_info_s_type         *new_time = CDMA_TIME_PTR(new_srv_info);
  #endif //FEATURE_C2K_TIME_INFO
  if ((new_srv_info != NULL) && (old_srv_info != NULL))
  {

    if (new_srv_info->hdr_srv_status != old_srv_info->hdr_srv_status)
    {
      changed_fields |= CM_SS_HDR_SRV_STATUS_MASK;
    }

    if (new_srv_info->hdr_roam_status != old_srv_info->hdr_roam_status)
    {
      changed_fields |= CM_SS_HDR_ROAM_STATUS_MASK;
    }

    #ifdef FEATURE_BCMCS
#error code not present
    #endif


    #ifdef FEATURE_HYBR_GW
#error code not present
    #endif


    /* If the HDR active protocol is changed, set the mask.
    */
    if( new_srv_info->hdr_active_prot != old_srv_info->hdr_active_prot )
    {
      changed_fields |= CM_SS_HDR_ACTIVE_PROT_MASK;
    }

    /* If the HYBRID active band is changed, set the mask.
    */
    if (new_srv_info->hybrid_active_band != old_srv_info->hybrid_active_band )
    {
      changed_fields |= CM_SS_HYBRID_ACTIVE_BAND_MASK;
    }

    /* If the HYBRID active channel is changed, set the mask.
    */
    if (new_srv_info->hybrid_active_channel != old_srv_info->hybrid_active_channel )
    {
      changed_fields |= CM_SS_HYBRID_ACTIVE_CHANNEL_MASK;
    }
    #ifdef FEATURE_C2K_TIME_INFO
    /*
    ** If there is a CDMA time change, notify clients 
    */
    if (old_time->sys_mode != new_time->sys_mode)
    {
      CM_MSG_HIGH("time:hybr- old mode %d new mode %d",
                                    old_time->sys_mode,
                                    new_time->sys_mode,
                                                    0); 
      changed_fields |= CM_SS_CDMA_TIME_CHGD_MASK;
    }
    if (new_time->sys_mode == SYS_SYS_MODE_HDR)
    {
      if( (new_time->time.hdr_time.ltm_offset !=
           old_time->time.hdr_time.ltm_offset) ||
          (new_time->time.hdr_time.lp_sec !=
           old_time->time.hdr_time.lp_sec) )
      {
        changed_fields |= CM_SS_CDMA_TIME_CHGD_MASK;
        CM_MSG_HIGH("time:hybr- old ltm %d new ltm %d ",
                     old_time->time.hdr_time.ltm_offset,
                     new_time->time.hdr_time.ltm_offset,
                                                     0); 
        CM_MSG_HIGH("old lpsec %d new lpsec %d",
                 old_time->time.hdr_time.lp_sec,
                 new_time->time.hdr_time.lp_sec,
                                             0); 
      }
    } 
  #endif //FEATURE_C2K_TIME_INFO
  } /* if ((new_srv_info != NULL) && (old_srv_info != NULL)) */

  return changed_fields;

} /* cmss_get_ss_hdr_info_change () */


/*===========================================================================

FUNCTION cmss_get_ss_gw_info_change

DESCRIPTION
  Compares the serving system information change and returns
  a bit mask holding difference.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  Bit Mask indicating which of the Serving System parameters changed

SIDE EFFECTS
  none

===========================================================================*/
static uint64 cmss_get_ss_gw_info_change(

    const cm_mm_ss_info_s_type     *new_srv_info,
        /*  New SS information */

    const cm_mm_ss_info_s_type     *old_srv_info
        /*  Old SS information */
)
{

  uint64                        changed_fields = 0;
      /* Records the changes in SS parameters between new_srv_info and old_srv_info */

  if ((new_srv_info != NULL) && (old_srv_info != NULL))
  {

    #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
    if (new_srv_info->gw_srv_status != old_srv_info->gw_srv_status)
    {
      changed_fields |= CM_SS_GW_SRV_STATUS_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If true gw service status changed, 
    ** then also notify CM clients of the event.
    */
    if (new_srv_info->true_gw_srv_status != old_srv_info->true_gw_srv_status )
    {
      changed_fields |= CM_SS_TRUE_GW_SRV_STATUS_MASK;
    }

    if (new_srv_info->gw_roam_status != old_srv_info->gw_roam_status)
    {
      changed_fields |= CM_SS_GW_ROAM_STATUS_MASK;
    }
    
    /*
    ** If system mode changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->gw_sys_mode != old_srv_info->gw_sys_mode )
    {
      changed_fields |= CM_SS_GW_SYS_MODE_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If active band changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->gw_active_band != old_srv_info->gw_active_band )
    {
      changed_fields |= CM_SS_GW_ACTIVE_BAND_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /*
    ** If srv domain changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->gw_srv_domain != old_srv_info->gw_srv_domain )
    {
      changed_fields |= CM_SS_GW_SRV_DOMAIN_MASK;
    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*
    ** If srv capability changed, notify CM clients of the corresponding
    ** event.
    */
    if ( new_srv_info->gw_srv_capability != old_srv_info->gw_srv_capability )
    {
      changed_fields |= CM_SS_GW_SRV_CAPABILITY_MASK;
    }
    
    if( old_srv_info->simultaneous_mode !=  SYS_SIMULTANEOUS_MODE_NONE && 
        new_srv_info->simultaneous_mode != old_srv_info->simultaneous_mode )
    {
      changed_fields |= CM_SS_SIMULTANEOUS_MODE_MASK;
    }
    
    /*
    ** If the system identifier has changed, notify CM clients of the
    ** corresponding event.
    */
    #if (defined(FEATURE_HYBR_GW))
#error code not present
    #else
    if ( !cmss_sys_id_match( new_srv_info->sys_id, old_srv_info->sys_id ) )
    {
      changed_fields |= CM_SS_SYS_ID_MASK;

      #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
      #endif  /* FEATURE_WCDMA || FEATURE_GSM */

      /*
      ** Decide whether the SID and NID Mask fields need to be set also.
      */
      if ((new_srv_info->sys_id.id_type == SYS_SYS_ID_TYPE_IS95) &&
        (old_srv_info->sys_id.id_type == SYS_SYS_ID_TYPE_IS95))
      {
        if (new_srv_info->sys_id.id.is95.nid != old_srv_info->sys_id.id.is95.nid)
        {
          changed_fields |= CM_SS_NID_MASK;
        }

        if (new_srv_info->sys_id.id.is95.sid != old_srv_info->sys_id.id.is95.sid)
        {
          changed_fields |= CM_SS_SID_MASK;
        }
        if( new_srv_info->sys_id.id.is95.mcc != old_srv_info->sys_id.id.is95.mcc )
        {
          changed_fields |= CM_SS_MCC_MASK;
        }

        if(new_srv_info->sys_id.id.is95.imsi_11_12 != old_srv_info->sys_id.id.is95.imsi_11_12 )
        {
          changed_fields |= CM_SS_IMSI_11_12_MASK;
        }
      }
      else if ((new_srv_info->sys_id.id_type == SYS_SYS_ID_TYPE_IS95) &&
          (old_srv_info->sys_id.id_type != SYS_SYS_ID_TYPE_IS95))
      {
        changed_fields |= CM_SS_NID_MASK;
        changed_fields |= CM_SS_SID_MASK;
        changed_fields |= CM_SS_MCC_MASK;
        changed_fields |= CM_SS_IMSI_11_12_MASK;
      }
    } /* if ( !cmss_sys_id_match( new_srv_info->sys_id, old_srv_info->sys_id ) ) */
    #endif /* FEATURE_HYBR_GW */
    #endif /* FEATURE_CM_TECH_SPEC_SRV_IND */

  } /* if ((new_srv_info != NULL) && (old_srv_info != NULL)) */

  return changed_fields;

} /* cmss_get_ss_gw_info_change () */


/*===========================================================================

FUNCTION cmss_get_ss_wlan_info_change

DESCRIPTION
  Compares the serving system information change and returns
  a bit mask holding difference.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  Bit Mask indicating which of the Serving System parameters changed

SIDE EFFECTS
  none

===========================================================================*/
uint64 cmss_get_ss_wlan_info_change(

    const cm_mm_ss_info_s_type     *new_srv_info,
        /*  New SS information */

    const cm_mm_ss_info_s_type     *old_srv_info
        /*  Old SS information */
)
{
  uint64                        changed_fields = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( new_srv_info != NULL );
  CM_ASSERT( old_srv_info != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Records the changes in SS parameters between new_srv_info and old_srv_info */


  if (new_srv_info->wlan_srv_status != old_srv_info->wlan_srv_status)
  {
      changed_fields |= CM_SS_WLAN_SRV_STATUS_MASK;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check and report WLAN BSS Info only if we still have WLAN service.
  */

  if( new_srv_info->wlan_srv_status != SYS_SRV_STATUS_SRV)
  {
      return changed_fields;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (! (sys_is_wlan_ssid_match(&(new_srv_info->wlan_bss_info.ssid),
                                  &(old_srv_info->wlan_bss_info.ssid))) ||

        (new_srv_info->wlan_bss_info.chan !=
                                      old_srv_info->wlan_bss_info.chan) ||

        (new_srv_info->wlan_bss_info.tech !=
                                      old_srv_info->wlan_bss_info.tech))
  {
      changed_fields |= CM_SS_WLAN_BSS_INFO_MASK;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return changed_fields;

} /* cmss_get_ss_wlan_info_change () */


/*===========================================================================

FUNCTION cmss_update_wlan_srv_ind

DESCRIPTION
  Update the serving system object per the new WLAN service indicators.

  If service status or WLAN channel/BSS etc. is changed, Inform CM clients of
  corresponding events.

DEPENDENCIES
  Serving system object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmss_update_wlan_srv_ind(

    cmss_s_type               *ss_ptr,
        /* Pointer to a serving system object */

    const sd_si_info_s_type   *si_info_ptr
       /* Pointer to buffer containing new service info information */
)
{

  sys_srv_status_e_type prev_srv_status   =  SYS_SRV_STATUS_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss_ptr != NULL );
  CM_ASSERT( si_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmss_update_wlan_srv_ind()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->info.changed_fields = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Default the values of regn reject info upon getting service
  ** indication or confirmation from LL
  */
  cm_ss_init_reg_reject_info( &ss_ptr->info.reg_reject_info );

  /* If we lost service, set no_srv_uptime to uptime + CMSS_MAX_NO_SRV_TIME.
  ** The no_srv_uptime is checked periodically by the cmss_timer_proc()
  ** function. If no_srv_uptime expires before service is reacquired,
  ** cmss_timer_proc() reports to CM clients of a service lost event.
  */

  prev_srv_status  = ss_ptr->srv_avail_info.wlan_srv_status;
  switch ( si_info_ptr->srv_status )
  {
    case SYS_SRV_STATUS_NO_SRV:
    {
      dword clk_time = clk_uptime();
      ss_ptr->wlan_no_srv_uptime   = MIN( ss_ptr->wlan_no_srv_uptime,
                                         clk_time + CMSS_MAX_NO_SRV_TIME );


      ss_ptr->info.wlan_srv_status = si_info_ptr->srv_status;

      /* Since we  moved out of service, clear the field */
      if ( ( prev_srv_status == SYS_SRV_STATUS_LIMITED ) ||
           ( prev_srv_status == SYS_SRV_STATUS_SRV ) )
      {
        ss_ptr->wlan_srv_inform_uptime = clk_time + CMSS_INFORM_SRV_CHANGE_TIME;
      }
      else
        /*lint -e{725} Ignore indentation */   
      {
        ss_ptr->wlan_srv_inform_uptime = CMSS_INFORM_SRV_CHANGE_INTER_NONE;
      }
    
      ss_ptr->info.wlan_rssi          = CMSS_RSSI_NO_SIGNAL;
      ss_ptr->info.wlan_bss_info.rssi = CMSS_RSSI_NO_SIGNAL;
      ss_ptr->wlan_is_pref_srv_acq  = TRUE;   
    }
    break;


    case SYS_SRV_STATUS_SRV:
    case SYS_SRV_STATUS_LIMITED:
    {
      dword clk_time = clk_uptime();

      /* Process service report only if it is coming from WLAN.
      */
      CM_ASSERT( si_info_ptr->mode == SD_MODE_WLAN );

      /* Reset the no_srv_uptime.
      */
      ss_ptr->wlan_no_srv_uptime   = CMSS_NO_SRV_UPTIME_NONE;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Update the service state.
      */
      ss_ptr->info.wlan_srv_status = si_info_ptr->srv_status;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Update the preferred acquisition indicator.
      */
      ss_ptr->wlan_is_pref_srv_acq   = si_info_ptr->is_pref_srv_acq;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Update WLAN BSS Info.
      */

      ss_ptr->info.wlan_bss_info.bssid     =    si_info_ptr->mode_info.wlan.bssid;

      ss_ptr->info.wlan_bss_info.bss_type  =    si_info_ptr->mode_info.wlan.bss_type;

      ss_ptr->info.wlan_bss_info.tech      =    si_info_ptr->mode_info.wlan.tech;

      ss_ptr->info.wlan_bss_info.chan      =    si_info_ptr->chan;

      ss_ptr->info.wlan_bss_info.ssid      =    si_info_ptr->sid.ssid;

      ss_ptr->info.wlan_bss_info.band      =    si_info_ptr->band;


      if (ss_ptr->wlan_srv_inform_uptime == CMSS_INFORM_SRV_CHANGE_INTER_NONE)
      {

        /* uptime is CMSS_INFORM_SRV_CHANGE_INTER_NONE when srv indication
        ** transitions to SRV available from PWR_SAVE or NO_SRV. Clients need
        ** to be informed immediately of service here.
        */
        ss_ptr->wlan_srv_inform_uptime   = clk_time + CMSS_INFORM_SRV_CHANGE_TIME;

        /* srv_avail_info holds serving system parameters last reported to clients by
        ** CM_SS_EVENT_SRV_CHANGED. cmss_update_ss_bit_mask () computes the change in
        ** parameters that need to be reported to clients.
        */
        ss_ptr->info.changed_fields =
                     cmss_get_ss_wlan_info_change (&ss_ptr->info, &ss_ptr->srv_avail_info);

      }
      else
      {
        /* By this time hdr_srv_inform_uptime is active and serving system
        ** parameter change gets informed once in 2 secs only if there is a change
        ** in parameters.
        */
        ss_ptr->info.changed_fields                = 0;
      }

    }
    break;


    case SYS_SRV_STATUS_PWR_SAVE:
    {
      /* Check if srv_status transitioned to pwr_save.
      */
      if ( ss_ptr->info.wlan_srv_status  != si_info_ptr->srv_status )
      {
        /* yes, transitioned to pwr_save, generate event and stop
        ** no_srv timer.
        */

        ss_ptr->wlan_no_srv_uptime    = CMSS_NO_SRV_UPTIME_NONE;
        ss_ptr->info.wlan_srv_status  = si_info_ptr->srv_status;
        ss_ptr->info.changed_fields  |= CM_SS_WLAN_SRV_STATUS_MASK;
   
        /* Since we  moved out of service, clear the field */
        ss_ptr->wlan_srv_inform_uptime = CMSS_INFORM_SRV_CHANGE_INTER_NONE;

        ss_ptr->info.wlan_rssi           = CMSS_RSSI_NO_SIGNAL;
        ss_ptr->info.wlan_bss_info.rssi  = CMSS_RSSI_NO_SIGNAL;

      } /* if ( ss_ptr->info.srv_status != si_info_ptr->srv_status ) */
    }
    break;


    default:
      CM_MSG_HIGH("Invalid srv_status %d", si_info_ptr->srv_status, 0, 0 );
      break;

  } /* switch ( si_info_ptr->srv_status ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if clients have to be notified of srv_changed.
  */
  if ( ss_ptr->info.changed_fields != 0 )
  {

    /* Update the RSSI.
    */
    cmss_report_rssi( NULL, NULL, FALSE);

    /* Update WLAN Stats.
    */
    cmss_report_wlan_stats();

    /* Notify CM clients of any changes in the serving system information.
    */
    cmss_event( CM_SS_EVENT_SRV_CHANGED );

  } /* if ( changed_fields != 0 ) */


} /* cmss_update_wlan_srv_ind() */

/*===========================================================================
FUNCTION cmss_report_wlan_stats

DESCRIPTION
  This function gets the WLAN Stats. If the Stats have changed since last
  query, CM reports change event to registered clients.

DEPENDENCIES
  Serving System and Phone objects must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmss_report_wlan_stats()
{

  #ifdef FEATURE_WLAN
#error code not present
  #endif

}

/*===========================================================================
@FUNCTION cmss_report_wlan_link_quality

@DESCRIPTION
  This function gets the WLAN link quality, And reports change event 
  to registered clients.

@DEPENDENCIES
  Serving System and Phone objects must be initialized

@RETURNS
  none

@SIDE EFFECTS
  none

@x
===========================================================================*/
void cmss_report_wlan_link_quality()
{
   #if (defined( FEATURE_IMS_VCC) && defined(FEATURE_WLAN))
#error code not present
   #endif /* defined( FEATURE_IMS_VCC) && defined(FEATURE_WLAN) */
}



/*===========================================================================

FUNCTION cmss_send_mm_ps_data_avail_req

DESCRIPTION
  Send the ps data available request to MM

DEPENDENCIES

RETURN VALUE
  Return TRUE if the command is successfully send. Otherwise FALSE.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmss_send_mm_data_avail( void )
{

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#else /* ! (FEATURE_GSM || FEATURE_WCDMA) */

  return TRUE;

#endif /* FEATURE_GSM || FEATURE_WCDMA */

}

/*===========================================================================

FUNCTION cmss_sys_id_match

DESCRIPTION
  Verfies if two system identities are equal or not.

DEPENDENCIES

RETURN VALUE
  Return TRUE if identities are equal. Otherwise FALSE.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmss_sys_id_match(
  sys_sys_id_s_type     sys_id_1,
  sys_sys_id_s_type     sys_id_2
)
{
   if( sys_sys_id_match( sys_id_1, sys_id_2) )
   {
     if( sys_lac_match(sys_id_1,sys_id_2) )
     {
       return TRUE;
     }
   }

   return FALSE;
}

#ifdef FEATURE_IMS_VCC
/*===========================================================================

@FUNCTION cmss_wlan_link_qual_cb_reg

@DESCRIPTION
  Registers a callback with WLAN to receive any changes in WLAN link status. 
  Currently required by the IMS VCC HOM.

@DEPENDENCIES

@RETURN VALUE
  none

@SIDE EFFECTS
  none

@x
===========================================================================*/
static void cmss_wlan_link_qual_cb_reg( 

                            boolean is_reg
                        /**< to reg or de-reg */
)
{

 wlan_cp_link_qual_ind_cback_type   cb = NULL;

 /** IF is_reg is TRUE, set the callback function, otherwise pass NULL
 */
 if ( is_reg )
 {
    cb = cmss_wlan_link_qual_ind_cb;
 }
 
 /** Register callback with WLAN
 */
 if ( wlan_cp_link_qual_ind_cback_reg(cb, NULL) == 0 )
 {
      CM_MSG_HIGH( "wlan trigger reg failed", 0, 0, 0); 
 }

} /** cmss_wlan_link_qual_cb_reg */


/*===========================================================================
@FUNCTION cmss_wlan_link_qual_ind_cb

@DESCRIPTION
  Call back function to be registered with wlan cp for link quality indication.

@DEPENDENCIES
  None.

@RETURN VALUE
  None

@SIDE EFFECTS
  None.

@x
===========================================================================*/
void cmss_wlan_link_qual_ind_cb(     

    wlan_cp_link_qual_ind_enum_type  ind,
      /**< wlan link quality */

    void *user_data_ptr
      /**< user data */

)
/*lint -esym(715, user_data_ptr)*/
/*lint -esym(818, user_data_ptr)*/
{
   cmss_s_type          *ss_ptr       = cmss_ptr();

   cm_generic_rpt_s_type *cm_rpt_ptr;

   /*---------------------------------------------------------------*/ 

   CM_ASSERT(ss_ptr     != NULL);

   /*---------------------------------------------------------------*/


   /** Check that the indication provided by WLAN is valid
   */
   if ( ind <= WLAN_CP_LINK_QUAL_MIN_IND ||
           ind >= WLAN_CP_LINK_QUAL_MAX_IND )
   {
       return;
   }


   /** Map the WLAN LINK QUALITY indication value to a CM enum, 
   ** and store it in the WLAN stats structure
   */

   ss_ptr->info.wlan_stats.wlan_link_quality = 
                        cmss_map_wlan_link_qual_to_cm_sys_val(ind);
  

   /** Generate a generic report 
   */
   cm_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal();

   if ( cm_rpt_ptr == NULL )
   {
       return;
   }

   cm_rpt_ptr->hdr.cmd = CM_WLAN_RPT;
   cm_generic_rpt( cm_rpt_ptr );
        
} /** cmss_wlan_link_qual_ind_cb */
/*lint +esym(715, user_data_ptr)*/
/*lint +esym(818, user_data_ptr)*/


/*===========================================================================
@FUNCTION cmss_map_wlan_link_qual_to_cm_sys_val

@DESCRIPTION
Maps the link quality indication value returned by WLAN to a CM specific value

@DEPENDENCIES
  None.

@RETURN VALUE
 CM specific value for WLAN link quality

@SIDE EFFECTS
  None.

@x
===========================================================================*/

static sys_wlan_link_qual_e_type cmss_map_wlan_link_qual_to_cm_sys_val(

    wlan_cp_link_qual_ind_enum_type  ind
    /**< wlan link quality */

)
{
   /** Map WLAN link quality values to CM SYS value
   */
   switch( ind )
   {
       case WLAN_CP_LINK_QUAL_POOR_IND:
            return SYS_WLAN_LINK_QUAL_POOR;

       case WLAN_CP_LINK_QUAL_GOOD_IND:
            return SYS_WLAN_LINK_QUAL_GOOD;

       case WLAN_CP_LINK_QUAL_VERY_GOOD_IND:
            return SYS_WLAN_LINK_QUAL_VERY_GOOD;

       case WLAN_CP_LINK_QUAL_EXCELLENT_IND:
            return SYS_WLAN_LINK_QUAL_EXCELLENT;

       default:
            CM_ERR( "WLAN qual ind %d not mapped to CM quality indication"
                    "!!!", ind, 0,0 );
            CM_ASSERT(FALSE);
            return SYS_WLAN_LINK_QUAL_NONE;
   }

}

#endif /* FEATURE_IMS_VCC */
