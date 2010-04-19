#ifndef CMPREF_H
#define CMPREF_H
/*===========================================================================

           P R E F    H E A D E R    F I L E

DESCRIPTION
  This file contains all the definitions necessary for the Call Manager
  to interface with the PREF module.

Copyright (c) 2003 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmpref.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
03/03/05   ka      Adding support to let mmoc know of protocol that has a
                   subs change when calling mmoc_cmd_subscription_chg
02/01/05   dk      Added WLAN Support.
08/20/04   sj      Added support for PRL Preference.
02/06/04   ws      Initial jaguar/mainline merge.
11/06/03   sj      Fixed issue with kicking HDR if SID/NID changes during
                   HDR call.
10/14/03   sj      Added support for HDR reselect feature.
08/27/03   ic      Added declaration of cmpref_is_hybr_allowed()
06/22/03   sj      Initial release.

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "sd.h"
#include "cmll.h"
#include "mmoc.h"



/*===========================================================================
=============================================================================
=============================================================================
============================== CM -> PREF procs =============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmpref_proc_rpt_srv_ind

DESCRIPTION
  This function should be called to process the CM_SRV_IND_INFO_F report
  from SD20.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmpref_proc_rpt_srv_ind
(
        const cm_sd_rpt_u_type         *sd_rpt_ptr
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */

);


/*===========================================================================

FUNCTION cmpref_proc_cmd_oprt_mode_chgd

DESCRIPTION
  This function should be called when the operating mode of the phone has
  changed.This function should be called just before sending the operating
  mode changed command to MMOC.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmpref_proc_cmd_oprt_mode_chgd
(
        sys_oprt_mode_e_type           oprt_mode
           /* New operating mode of the phone.
           */

);


/*===========================================================================

FUNCTION cmpref_proc_cmd_pref_sys_chgd

DESCRIPTION
  This function should be called when the phone preferences have changed.
  This module will be responsible for queing pref_sys_chgd command to MMOC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmpref_proc_cmd_pref_sys_chgd
(

        sd_ss_e_type                   ss,
            /* Indicates the SS-Instance ( SS_MAIN or SS_HDR) this
            ** pref_sys_chgd command is intended for.
            */

        sd_ss_pref_reas_e_type         pref_reas,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /* The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /* Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /* PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /* Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /* New hybrid preference (none, CDMA/HDR).
           */

        sd_band_e_type                 otasp_band,
           /* The band-class that is associated with OTASP origination mode.
           ** Note that this parameter is ignored if orig_mode != OTASP.
           */

        sd_blksys_e_type               otasp_blksys,
          /* The PCS frequency block/Cellular system that is associated
          ** with OTASP origination mode. Note that this parameter is ignored
          ** if orig_mode != OTASP.
          */

        sd_ss_avoid_sys_e_type         avoid_type,
          /* Type of the system to be avoided, valid only
          ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        dword                          avoid_time,
          /* Time in seconds for which the system is to be avoided, valid
          ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        sys_plmn_id_s_type               plmn_id,
           /* The PLMN id to be acquired, valid when the net_sel_mode_pref
           ** is MANUAL.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_srv_domain_pref_e_type       domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type        acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        const cm_wlan_pref_s_type          *wlan_pref_ptr
           /* WLAN System Preferences.
           */

);


/*===========================================================================

FUNCTION cmpref_proc_cmd_subsc_chgd

DESCRIPTION
  This function should be called when the subscrption availability has
  changed.This function should be called just before sending the subsc_chgd
  command to MMOC.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmpref_proc_cmd_subsc_chgd
(

        mmoc_subsc_chg_e_type          chg_type,
             /* Subscription change type.
             */

        byte                           nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /* The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /* Band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /* PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /* Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /* New hybrid preference (none, CDMA/HDR).
           */

        sys_plmn_id_s_type            plmn_id,
           /* The PLMN id to be acquired, valid when the net_sel_mode_pref
           ** is MANUAL.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        boolean                        is_gw_subsc_avail,
           /* Current GSM/WCDMA subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_cdma_subsc_avail,
           /* Current CDMA/AMPS/HDR subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_wlan_subsc_avail,
           /* Current WLAN subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        const cm_wlan_pref_s_type          *wlan_pref_ptr,
           /* WLAN System Preferences.
           */


        prot_subsc_chg_e_type               prot_subsc_chg
           /* Enumeration of protocols that have a change in
           ** subscription available status
           */



);


/*===========================================================================

FUNCTION cmpref_proc_cmd_call_end

DESCRIPTION
  This function should be called to process the call end event from CMCALL.
  This function would reinitiate SS-HDR acquisition if necessary.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmpref_proc_cmd_call_end
(
        void
);




/*===========================================================================

FUNCTION cmpref_proc_cmd_disable_hybr_hdr

DESCRIPTION
  This function should be called to disable the HDR acquisitions while in
  hybrid operation.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmpref_proc_cmd_disable_hybr_hdr
(
        void
);


/*===========================================================================

FUNCTION cmpref_proc_cmd_enable_hybr_hdr

DESCRIPTION
  This function should be called to enable the HDR acquisitions while in
  hybrid operation.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmpref_proc_cmd_enable_hybr_hdr
(
        boolean                        send_pref_sys_cmd
          /* Flag to indicate if pref. sys changed command has to be
          ** sent.
          */
);



/*===========================================================================

FUNCTION cmpref_proc_cmd_avoid_hybr_hdr_orig

DESCRIPTION
  This function should be called to avoid hybr hdr for sometime.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmpref_proc_cmd_avoid_hybr_hdr_orig
(
        void
);


/*===========================================================================

FUNCTION cmpref_is_hybr_status_on

DESCRIPTION
  This function should be called to determine if hybrid is ON.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  boolean                         cmpref_is_hybr_status_on
(
        void
);

/*===========================================================================

FUNCTION cmpref_is_gw_hybr

DESCRIPTION
  This function should be called to determine if hybrid GW is ON.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  boolean                         cmpref_is_gw_hybr( void );


/*===========================================================================

FUNCTION cmpref_is_hybr_hdr_call_allowed

DESCRIPTION
  This function should be called to determine if hybrid HDR is allowed.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  boolean                         cmpref_is_hybr_hdr_call_allowed
(
        void
);


/*===========================================================================

FUNCTION cmpref_is_hybr_gw_call_allowed

DESCRIPTION
  This function should be called to determine if hybrid GW is allowed.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  boolean                         cmpref_is_hybr_gw_call_allowed
(
        void
);

/*===========================================================================

FUNCTION cmpref_proc_timer

DESCRIPTION
  This function should be called to process the CM's 1 second timer tick.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmpref_proc_timer
(

        void
);


/*===========================================================================

FUNCTION cmpref_init

DESCRIPTION
  This function should be called to initialize the PREF module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmpref_init
(
        void

);

/*===========================================================================

FUNCTION cmpref_is_hybr_allowed

DESCRIPTION
  This function returns whether hybrid is allowed or not based on the
  mode_pref and hybrid_pref passed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean                         cmpref_is_hybr_allowed
(

       sd_ss_mode_pref_e_type          mode_pref,
          /* Mode preference to use.
          */

       sd_ss_hybr_pref_e_type          hybr_pref
          /* Hybrid preference to use.
          */
);
#endif /* CMPREF_H */
