#ifndef MMOC_H
#define MMOC_H
/**===========================================================================
@FILE_DOC

                    M U L T I M O D E   C O N T R O L L E R

                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for Call Manager
  (CM) and the protocols ( MC, HDR and REG) to interface with the
  Multi-MOde Controller (MMoC) task.

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/**===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM_MM/APPS/vcs/mmoc.h_v   1.3   10 Jun 2002 11:35:48   sjaikuma  $
  $Header: //source/qcom/qct/modem/mmode/mmoc/rel/08H1/inc/mmoc.h#1 $ $DateTime: 2008/12/02 19:16:20 $ $Author: svasired $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/08   sv      Added WLAN_WPS API
04/28/08   fj      Lint cleanup, CR 144389
04/17/07   rp      Added RPC metacomments for WLAN.   
04/16/07   ic      Moved inclusion of AEE* headers to mmoci.h
03/19/07   ka      Replacing banned apis
01/31/07   ka      Adding new state in MMOC to wait for TMC to ack operating
                   mode change.
11/15/06   sk      Added hybrid GW support.
08/29/06   sk      Added WLAN Adhoc support.
04/03/06   ka      Changing copyright.
03/02/05   ka      Added prot_subsc_chag param to mmoc_cmd_subscription_chgd
08/20/04   sj      Added support for prl_pref.
02/27/03   SJ      Added support sanity_timeout err_fatal in respective
                   protocols.
02/13/03   SJ      Added mmoc_cmd_wakeup_from_pwr_save API.
01/16/03   SJ      Added define to indicate if MMOC uses DB.
09/03/02   SJ      Unfeaturized GSM support in MMoC.
03/28/02   SJ      Initial release.

===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "prot.h"      /* Common types associated with MMoC <-> protocol
                       ** communciation.
                       */

/*===========================================================================
=============================================================================
=============================================================================
============================== SUPPORTED PROTOCOL DEFINES ===================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup SPD Supported Protocol Defines
** @{
*/

/** CDMA supported define.
*/
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#define MMOC_CDMA_SUPPORTED
#endif

/** AMPS supported define.
*/
#ifdef FEATURE_ACP
#error code not present
#endif

/** HDR supported define.
*/
#ifdef FEATURE_HDR
#error code not present
#endif

/** GSM and/or WCDMA supported define.
*/
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

/** Hybrid GSM and/or WCDMA supported define.
*/
#if defined(MMOC_GW_SUPPORTED) && defined(FEATURE_HYBR_GW) \
    && defined(FEATURE_TECH_IND_SD_STACK_ARCH)
#error code not present
#endif

/** WLAN supported define.
*/
#ifdef FEATURE_WLAN
#error code not present
#endif

/** HDR supported define.
*/
#if defined(FEATURE_HDR) || defined(MMOC_HYBR_GW_SUPPORTED)
#define MMOC_HYBR_1_SUPPORTED
#endif

/** FTM supported define.
*/
#ifdef FEATURE_FACTORY_TESTMODE
#define MMOC_FTM_SUPPORTED
#endif

/** Define to indicate if MMOC should use DB services.
*/
#if defined(MMOC_CDMA_SUPPORTED) || defined(MMOC_AMPS_SUPPORTED) || \
    defined(MMOC_HDR_SUPPORTED)
#define MMOC_USES_DB_SERVICES
#endif

/** Define if HDR hybrid is supported.
*/
#if defined(FEATURE_HDR) && defined(FEATURE_HDR_HYBRID) && \
    defined(MMOC_CDMA_SUPPORTED)
#error code not present
#endif

/** @}
**
*/

/*===========================================================================
=============================================================================
=============================================================================
============================== Common interface types =======================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup CIT Common Interface Types
** @{
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of subscription change type.
*/
typedef enum {

  MMOC_SUBSC_CHG_SUBSC_AVAIL,          /**< Subscription changed due to change
                                       ** in CDMA or GSM/WCDMA subscription
                                       ** availability.
                                       */

  MMOC_SUBSC_CHG_NAM_SEL,              /**< Subscription changed due to NAM
                                       ** selection change.
                                       */

  MMOC_SUBSC_CHG_MAX                   /**< For internal use */

} mmoc_subsc_chg_e_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of sanity error type.
*/
typedef enum {

  MMOC_SANITY_ERR_DEACT_CMD_CNF,       /**< MMoC's sanity error was because the
                                       ** protocol did not respond to
                                       ** deactivate request send by MMOC.
                                       */

  MMOC_SANITY_ERR_PH_STAT_CMD_CNF,     /**< MMoC's sanity error was because the
                                       ** protocol did not respond to phone
                                       ** status changed command sent by MMOC.
                                       */

  MMOC_SANITY_ERR_GENERIC_CMD_CNF,     /**< MMoC's sanity error was because the
                                       ** protocol did not respond to the
                                       ** generic cmd sent by MMOC.
                                       */

  MMOC_SANITY_ERR_MAX

} mmoc_sanity_err_e_type;

/** @}
** 
*/




/**===========================================================================
=============================================================================
=============================================================================
============================== CM -> MMOC commands ==========================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup CMC CM->MMOC commands
** @{
*/

/* The following section defines the API when the clients ( example CM )
** have to communicate to MMoC using the command queue.
*/


/**===========================================================================

@FUNCTION mmoc_cmd_subscription_chgd

@DESCRIPTION
  This function should be called when the subscription changed because of NAM
  change or GSM/WCDMA subscription availability change or CDMA/AMPS/HDR
  subscription availability change. This function will queue the subscription
  changed command to the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_subscription_chgd
(
        mmoc_subsc_chg_e_type          chg_type,
             /**< Subscription change type.
             */

        byte                           nam,
            /**< Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /**< The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /**< Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /**< PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /**< Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /**< New hybrid preference (none, CDMA/HDR).
           */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
          */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type         tech_pref,
           /**< Preference for the technology versions to use.
           */

        sys_wlan_scan_pref_e_type     wlan_scan_pref,
           /**< WLAN scan preference.
           */

        boolean                        is_gw_subsc_avail,
           /**< Current GSM/WCDMA subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_cdma_subsc_avail,
           /**< Current CDMA/AMPS/HDR subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_wlan_subsc_avail,
           /**< Current WLAN subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        prot_subsc_chg_e_type          prot_subsc_chg
           /**< Enumeration of protocols that have a change in
           ** subscription available status
           */
);



/**===========================================================================

@FUNCTION mmoc_cmd_subscription_chgd2

@DESCRIPTION
  This function should be called when the subscription changed because of NAM
  change or GSM/WCDMA subscription availability change or CDMA/AMPS/HDR
  subscription availability change. This function will queue the subscription
  changed command to the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_subscription_chgd2
(
        mmoc_subsc_chg_e_type          chg_type,
             /**< Subscription change type.
             */

        byte                           nam,
            /**< Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /**< The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /**< Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /**< PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /**< Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /**< New hybrid preference (none, CDMA/HDR).
           */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type         tech_pref,
           /**< Preference for the technology versions to use.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref,
           /**< WLAN scan preference.
           */

        sys_wlan_bss_e_type            wlan_bss_type_pref,
           /**< Preference for the WLAN BSS type.
           */

        boolean                        is_gw_subsc_avail,
           /**< Current GSM/WCDMA subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_cdma_subsc_avail,
           /**< Current CDMA/AMPS/HDR subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_wlan_subsc_avail,
           /**< Current WLAN subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        prot_subsc_chg_e_type          prot_subsc_chg
           /**< Enumeration of protocols that have a change in
           ** subscription available status
           */
);



/**===========================================================================

@FUNCTION mmoc_cmd_oprt_mode_chgd

@DESCRIPTION
  This function will be used to command the MMoC to do operating mode change;
  the new operating mode is specified in the oprt_mode parameter. This
  function will queue the operating mode changed command to the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_oprt_mode_chgd
(

        sys_oprt_mode_e_type           oprt_mode
            /**< The operating mode to be switched to.
            */

);



/**===========================================================================

@FUNCTION mmoc_cmd_pref_sys_chgd

@DESCRIPTION
  This function is used to command the MMoC to do Preferred system selection
  change. This function will queue the preferred system selection changed
  command to the MMoC Task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_pref_sys_chgd
(

        sd_ss_e_type                   ss,
            /**< Indicates the SS-Instance ( SS_MAIN or SS_HDR) this
            ** pref_sys_chgd command is intended for.
            */


        sd_ss_pref_reas_e_type         pref_reas,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /**< The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /**< Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /**< PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /**< Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /**< New hybrid preference (none, CDMA/HDR).
           */

        sd_band_e_type                 otasp_band,
           /**< The band-class that is associated with OTASP origination mode.
           ** Note that this parameter is ignored if orig_mode != OTASP.
           */

        sd_blksys_e_type               otasp_blksys,
          /**< The PCS frequency block/Cellular system that is associated
          ** with OTASP origination mode. Note that this parameter is ignored
          ** if orig_mode != OTASP.
          */

        sd_ss_avoid_sys_e_type         avoid_type,
          /**< Type of the system to be avoided, valid only
          ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        dword                          avoid_time,
          /**< Time in seconds for which the system is to be avoided, valid
          ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
          */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type         tech_pref,
           /**< Preference for the technology versions to use.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref
           /**< WLAN scan preference.
           */
);



/**===========================================================================

@FUNCTION mmoc_cmd_pref_sys_chgd2

@DESCRIPTION
  This function is used to command the MMoC to do Preferred system selection
  change. This function will queue the preferred system selection changed
  command to the MMoC Task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_pref_sys_chgd2
(

        sd_ss_e_type                   ss,
            /**< Indicates the SS-Instance ( SS_MAIN or SS_HDR) this
            ** pref_sys_chgd command is intended for.
            */


        sd_ss_pref_reas_e_type         pref_reas,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /**< The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /**< Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /**< PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /**< Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /**< New hybrid preference (none, CDMA/HDR).
           */

        sd_band_e_type                 otasp_band,
           /**< The band-class that is associated with OTASP origination mode.
           ** Note that this parameter is ignored if orig_mode != OTASP.
           */

        sd_blksys_e_type               otasp_blksys,
          /**< The PCS frequency block/Cellular system that is associated
          ** with OTASP origination mode. Note that this parameter is ignored
          ** if orig_mode != OTASP.
          */

        sd_ss_avoid_sys_e_type         avoid_type,
          /**< Type of the system to be avoided, valid only
          ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        dword                          avoid_time,
          /**< Time in seconds for which the system is to be avoided, valid
          ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
          */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type         tech_pref,
           /**< Preference for the technology versions to use.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref,
           /**< WLAN scan preference.
           */

        sys_wlan_bss_e_type            wlan_bss_type_pref
           /**< WLAN scan preference.
           */
);



/**===========================================================================

@FUNCTION mmoc_cmd_get_networks_gw

@DESCRIPTION
  The function will be used to command the MMoC to get the networks on GSM/
  WCDMA technologies. This function will queue the Get networks command to
  the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_get_networks_gw
(

        sd_ss_mode_pref_e_type         mode_pref,
           /**< Mode preference that is associated with the Get networks
           ** command.
           */


        sd_ss_band_pref_e_type         band_pref
          /**< Band preference that is associated with the Get networks
          ** command.
          */
);


/**===========================================================================

@FUNCTION mmoc_cmd_term_get_networks_gw

@DESCRIPTION
  This function will be used to inform the MMoC that the user has terminated
  the Get networks request on GSM/WCDMA networks. This function will queue
  the Terminate get networks command to the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                          mmoc_cmd_term_get_networks_gw
(

        void
);



/**===========================================================================

@FUNCTION mmoc_cmd_get_networks_wlan

@DESCRIPTION
  The function will be used to command the MMoC to get the networks on WLAN
  mode. This function will queue the Get networks command to the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_get_networks_wlan
(

        sd_ss_band_pref_e_type         bands
          /**< Band preference that is associated with the Get networks
          ** command.
          */
);



/**===========================================================================

@FUNCTION mmoc_cmd_term_get_networks_wlan

@DESCRIPTION
  This function will be used to inform the MMoC that the user has terminated
  the Get networks request on WLAN networks. This function will queue
  the Terminate get networks command to the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                          mmoc_cmd_term_get_networks_wlan
(

        void
);

/**===========================================================================

@FUNCTION mmoc_cmd_wlan_wps_start

@DESCRIPTION
  The function is used to command the MMoC to get WLAN credentails using WPS.
  This function will queue the WPS start command to the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_wlan_wps_start
(

        sys_wlan_wps_start_param_s_type     start_param
          /**< WPS start parameters that are associated with the WLAN WPS start
          ** command.
          */
);



/**===========================================================================

@FUNCTION mmoc_cmd_wlan_wps_abort

@DESCRIPTION
  This function will be used to inform the MMoC that the user has aborted the
  WPS procedure on WLAN. This function will queue the WPS abort command to
  the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_wlan_wps_abort
(
        void
);



/**===========================================================================

@FUNCTION mmoc_cmd_wakeup_from_pwr_save

@DESCRIPTION
  This function will be used to command the MMoC to wakeup the phone from
  power save.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_wakeup_from_pwr_save
(

        sd_ss_e_type                   ss
          /**< Wakeup the 'ss' System Selection instance from power save.
          */

);

/** @}
**
*/

/**===========================================================================
=============================================================================
=============================================================================
============================== Protocols -> MMOC reports ====================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup PMR Protocols->MMOC Reports
*  @{
*/

/* The following section defines the API when the clients ( example
** protocols ) have to communicate to MMoC using the report queue.
*/


/**===========================================================================

@FUNCTION mmoc_rpt_prot_deactd_cnf

@DESCRIPTION
  This function will be used to confirm that the MMoC that the protocol has
  been deactivated. This function will queue the protocol deactivated report
  to the MMoC task.

  Note! This function should be called only in response to protocol
  deactivate request from MMoC.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_prot_deactd_cnf
(

        prot_trans_type                trans_id
           /**< Protocols set this same as the trans_id received in their
           ** Deactivate request.
           */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mmoc_rpt_prot_deactd_cnf */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/**===========================================================================

@FUNCTION mmoc_rpt_prot_auto_deactd_ind

@DESCRIPTION
  This function will be used to inform the MMoC that the protocol has been
  deactivated autonomously. This function will queue the deactivated
  indication report to the MMoC task.

  Note! This function should not be called in response to a protocol
  deactivate request from MMoC.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_prot_auto_deactd_ind
(

        prot_deact_e_type              deactd_reason,
           /**< Reason that the protocol was deactivated.
           */

        prot_state_e_type              prot_state
           /**< Protocol which sent the autonomous deactivation.
           */


);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mmoc_rpt_prot_auto_deactd_ind */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/**===========================================================================

@FUNCTION mmoc_rpt_ph_stat_chgd_cnf

@DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "phone status changed (due to either NAM change, Subscription
  availability change(GSM/WCDMA or CDMA/AMPS/HDR) or operating mode change
  request. This function will queue the phone status changed confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_ph_stat_chgd_cnf
(

        prot_trans_type                trans_id,
          /**< Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */

        sd_mode_e_type                 prot
          /**< Protocol responding to the Phone status changed command.
          */


);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mmoc_rpt_ph_stat_chgd_cnf */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/**===========================================================================

@FUNCTION mmoc_rpt_tmc_mode_cnf

@DESCRIPTION
  Function gets called by TMC to acknowledge that it has processed the 
  mode change request. Mode change is given to TMC for Offline, online and
  power down transactions.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_tmc_mode_cnf
(

        prot_trans_type                trans_id
          /**< Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */

);

/** @}
**
*/


/*===========================================================================
=============================================================================
=============================================================================
============================== Protocols -> MMOC functional interface =======
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup PMFI Protocols->MMOC Functional Interface
*  @{
*/

/* The following section defines the API when the clients ( example
** protocols ) have to communicate to MMoC using the functional interface.
*/



/**===========================================================================

FUNCTION mmoc_proc_prot_gen_cmd

@DESCRIPTION
  This function should be used by protocols to process the Generic command
  received from MMoC.  The Generic system could be received because of
  Reselection timeout, Get networks request, Terminate get networks or
  preferred system selection change.


@DEPENDENCIES
  None.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an HDR system.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH.

    SD_SS_ACT_PWR_SAVE  - Enter power save mode.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type               mmoc_proc_prot_gen_cmd
(

        boolean                        is_resel_allowed,
          /**< Protocol indicated if reselection is allowed at this time or not.
          ** true = reselection allowed. This flag is passed on to SD20 in the
          ** call to sd_ss_ind_misc_timer(). So, this parameter is used only
          ** if the gen_ss_cmd type is Resetecion timer expired, for other
          ** types it is not used.
          */

        boolean                        is_prot_active,
          /**< Flag which specifies if the protocol calling this function is
          ** ACTIVE or not.
          */

        const prot_gen_cmd_s_type            *gen_cmd_ptr
           /**< Pointer to generic system selection command information.
           ** Same as what was received by the protocols.
           */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mmoc_proc_prot_gen_cmd */
/*~ PARAM gen_cmd_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/**===========================================================================

@FUNCTION mmoc_task

@DESCRIPTION
   This function is the entrance for the MMoC task. This is an infinite loop
   outside of the state machine which handles the input signals.

@DEPENDENCIES

@RETURN VALUE
  void.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           mmoc_task(

        dword                          ignored
           /**< Parameter received from Rex - ignored.
           */

);

#endif /* MMOC_H */

/** @}
**
*/
