/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
                        C M    R E G P R O X Y

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file is the main implementation for the RegProxy. The purpose of
  RegProxy is to act as a proxy to interface with the System determination
  for the events from REG and also interpreting the SD action for REG.
  This way, there would be no impact on CM <=> REG interface for multi-mode
  operation.

EXTERNALIZED FUNCTIONS

  Command Interface:
    cmregprx_cmd_gw_activate_protocol
    cmregprx_cmd_gw_deactivate_protocol
    cmregprx_cmd_gw_generic_cmd
    cmregprx_cmd_gw_ph_stat_chgd


  Report Interface:
    cmregprx_rpt_proc

  Functional Interface:
    cmregprx_proc_gw_resel_ok
    cmregprx_proc_gw_resel_not_ok
    cmregprx_cmd_proc
    cmregprx_init
    cmregprx_sanity_timeout

REGIONAL FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cmregprx_init() should be called before any APIs are called.


Copyright (c) 2002 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmregprx.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
02/25/09   sv      Correct Lint errors
02/20/09   rn      Go to ALT_SCAN only if GW subsc is available, and send 
                   PH_STAT_chgd to NAS even when in dormant state
09/09/08   cl      Unconditionally send CM_SIM_AVAILABLE or 
                   CM_SIM_UNAVAILABLE when there is a subscription change.
10/01/08   sv      Featurizing cmregprx_map_sd_mode_pref_to_sys to remove
                   compiler warning
09/18/08   rm      Adding acq_order_pref in sd_ss_act_get_net_gw_s_type.
                   Have to consider acq_order_pref when responding to get_networks 
09/17/08   ks      When OOS in CELL_FACH, UE needs to follow UOOS scanning 
                   pattern to re-acquire service
09/16/08   pk      Added support for WLAN UOoS
09/10/08   ak      Fixed scenario of data call drop during GSM scan
07/30/08   ak      Fix for the HICPS scenario.
06/18/08   sn      Adding Managed Roaming changes
04/26/08   rn      To send deact_cnf to mmoc in dormant state
04/24/08   rm      Removing feature flag FEATURE_GW_DORMANT_DURING_ALT_SCAN
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_PH_STAT_CHGD
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
03/19/08   rm      Corrected featurisation of FEATURE_CM_NAS_SERVICE_REQ_TYPE
03/19/08   rn      Added feature FEATURE_CM_NAS_PH_STAT_CHGD
03/14/08   rm      CM-NAS interface for new service req type
03/13/08   sk      Fix to allow CDMA scans during dormant GW PS data calls
                   without tearing down the PS data call.
02/15/08   rn      Added support for new command CM_PH_STATUS_CHGD_REQ
02/11/08   ic      Fixed compiler warnings
09/12/07   pk/rm   Merge to mainline - Adding support for handling calls in origination state
                   while service is lost
08/07/07   ic      Lint clean up
05/03/07   ic      Fixed compile errors on GSM-only builds (with respect to
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH)
06/19/07   ic/cl   Merge CL# 468160
06/15/07   rp      Adding support for
                   FEATURE_CONNECTED_MODE_UMTS_UNIFORM_OOS_HANDLING_UI_UPDATE
06/07/07   pk      UOOS enhancements
05/08/07   rp/cl   Passing is_resel_allowed as TRUE after background
                   service search is done.
04/14/06   ka/rp   Removed filtering of SERVICE_REQUESTS when the difference
                   is only in FOR fields.
11/30/06   rp      Added support for background PLMN search request.
11/16/06   pk      Lint Fixes
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
08/25/06   pk      Added support for sd_ss_ind_gw_acq_plmn_failed2
08/18/06   pk      Added support for GW UOOS
07/18/06   ka      Sending SYS_STOP_MODE_REASON_LPM for prot deact due to LPM.
06/12/06   jqi     Lint fix.
06/09/06   jqi     Only cleared the GW network list pending flag if SD stops
                   manual list search.
06/05/06   ka/ic   Sending SYS_STOP_MODE_REASON_POWER_OFF for prot deact due
                   to LPM.
04/01/06   ka      Adding sys to reg mapping function for net_list_cnf status.
01/26/06   ka      Adding cmregprxt_nam_sel_offline_sends_no_sim_avail_req()
                   Supporting success/abort result with network list cnf for
                   get networks request
01/12/06   ka      Correcting name of a field in CC, SS abort request.
01/12/06   ka      Sending abort reason along with SS, CC abort on
                   STOP_MODE_REQ.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
12/02/05   ka      Changes to handle new data suspend primitive.
11/30/05   ka      Adding support for HPLMN SRCH REQ and HPLMN SRCH CNF.
11/14/05   ic      Added missing CMREGPRX_ASSERT().
                   Added 613 Lint inhibit comments.
                   Lint80 clean up.
11/08/05   sk      Separated CM reports and commands.
11/01/05   ka      Clearing mmoc_trans_id when stop_mode_req is sent due to
                   either a default sd action or a PWR_SAVE sd action.
10/23/05   jqi     Hold ps data call orig or data avail request if there is
                   a pending get network list request.
08/02/05   ic      Print value that caused error in default switch case
07/15/05   ic      Featurization changes to support CDMA + GSM build flavor
06/16/05   ka      Sending SMREG_PDP_ABORT_REQ to SM during stop_mode_request
06/02/05   ic      Mainlined FEATURE_EXTENDED_PRL and
                   FEATURE_LIMITED_MANUAL_ACQ
05/18/05   ka      Adding feature for Limited manual acquistion.
03/18/05   ka      Correcting condition to always check for
                   PROT_PH_STAT_CHG_SUBSC before sending sim avail request.
03/03/05   ka      Adding support to let mmoc know of protocol that has a
                   subs change when calling mmoc_cmd_subscription_chg
01/07/05   ic      Mainlined FEATURE_MANUAL_HPLMN_SELECTION
11/15/04   ic      Lint cleanup with CM_DEBUG on
10/21/04   sj      Changed throttling mechanism for CM_SERVICE_CNF.
                   Removed handling of unused sanity timer mechanism.
08/10/04   sj      Fixed issue with not reseting last_service_req buffer
                   during NETWORK_LIST_REQ.
08/04/04   ic      Removed enabling/disabling of Lint error 788 as it is
                   globally suppressed in Lint config files.
                   Enabling it locally in CM files caused inconsistent
                   Lint issues reports when a file was linted separately
                   versus when it was linted as part of *.c batch
08/04/04   ic      CM Lint clean up - cmregprx.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/30/04   sj      Added support for camped_ind.
07/26/04   ic      Added cmregprx_dummy() to get rid of ADS 1.2 compiler
                   warning
07/05/04   sj      Added msg_high for manual PLMN in service_req.
06/28/04   sj      Lint cleanup.
05/21/04   ka      Code for handling CM_CAMPED_IND from REG.
                   New fields being populated for both cs and ps follow on requests
                   to denote origination pending during service request.
04/20/04   jqi     Added debugging message for srv_req, act_req and
                   get_net_work.
03/31/04   sj      Fixed lockup issue where srv. cnf is buffered if hold_uptime
                   has expired.
03/17/04   ka      Merge from 6250. Added Support for Follow on request
                   Fixed Lint errors.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/27/04   SJ      Changed stop_mode reason for FTM.
01/13/04   ka      Merged changes from MSM6200
12/10/03   jqi     Set user_select_mode to false for power up user orig mode.
                   Replaced FEATURE_QUAD_BAND_SUPPORT with
                   FEATURE_GSM_BAND_PREF and FEATURE_MANUAL_HPLMN_SELECTION.
11/03/03   ws      Fixed LTK compile problems.
10/30/03   prk     Added support for new limited_regional service.
10/23/03   ws      Added jqi's changes for band preference enum.
10/21/03   RI      Fixed merges from MSM6200.
10/15/03   SJ      Added support for FTM deactivate reason.
08/21/03   SJ      Added support to distinguish entering power save due to
                   no service or no systems to acquire.
08/20/03   SJ      Added srv cnf hold period, if srv cnf comes back within
                   1 second.
                   Added support for moving phone to power save after
                   early power scan.
07/28/03   SJ      Support for circulating data suspend flag thru' SD.
07/10/03   jqi     Added Extended PRL support
                   -Passing down band pref to GSM/UMTS stack.
                   -Populate LAC, band and chan from GSM/UMTS stack.
04/28/03   AT      Lint cleanup.
04/15/03   prk     Replaced CMREGPRX_MSG_ERROR with CMREGPRX_MSG_HIGH
03/21/03   ATM     Added event logging for svc_cnf and nw_list_cnf
03/19/03   sj      Added pwr_save support.
03/18/03   sj      Added support to send _activation_req( early power scan).
02/28/03   prk     Mainlined FEATURE_MULTIMODE_ARCH
02/21/03   sj      Sending tc_stop_mode_req + abort connections before
                   sending stop_mode_req to REG.
02/05/03   sj      Modified cmregprx_process_sd_action() to send the correct
                   stop mode reason to REG.
01/21/03   ws      Updated copyright information for 2003
10/18/02   vt      Compiles for GSM/WCDMA only.
04/22/02   SJ      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(714,cmregprx_dummy)
**     714: Symbol 'cmregprx_dummy(unsigned short)' not referenced
**     Need the function or we get compiler warnings about empty file on 1X
**     targets
*/

/*lint -esym(766,customer.h) */
#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#else /* FEATURE_GSM || FEATURE_WCDMA */

/*===========================================================================
FUNCTION cmregprx_dummy

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
word cmregprx_dummy( word dummy_input )
/*lint -esym(714,cmregprx_dummy) */
/*lint -esym(765,cmregprx_dummy)
** Can't be static as it would result in compiler warnings
*/
{
  return dummy_input;
}


#endif /* FEATURE_GSM || FEATURE_WCDMA */
