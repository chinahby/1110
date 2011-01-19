#ifndef CMPREFI_H
#define CMPREFI_H
/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
                            P R E F   S Y S

                 I N T E R N A L    H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the internal types necessary for implementing
  the CM PREF module.

Copyright (c) 2003 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmprefi.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Remove EXTERN define & defined MMEXTN in cmi.h
03/01/09   sv      Lint cleanup.
03/05/08   vk      corrected code for HYBR_GW configuration
12/07/06   pk      Added CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
08/29/06   sk      Added WLAN Adhoc support.
02/01/05   dk      Added WLAN Support.
08/20/04   sj      Added support for PRL Preference.
02/06/04   ws      Initial jaguar/mainline merge.
10/22/03   SJ      Updated acq_to on hdr to 10 seconds.
09/08/03   SJ      Adjusted "lost_main_timer" appropriately.
06/22/03   SJ      Initial release.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "cmi.h"



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*===========================================================================
=============================================================================
=============================================================================
==================== PREF IMPLEMENTATION RELATED TYPES ==================
=============================================================================
=============================================================================
===========================================================================*/



/* defines
*/
#define CMPREF_UPTIME_NONE             (dword) (-1)

/* "Acq_hdr_timer", set to 7 days.
*/
#define CMPREF_ACQ_HDR_TIMER           (dword)(604800)

/* "Avoid_hybr_hdr_timer", set to 60 seconds.
*/
#define CMPREF_AVOID_HYBR_HDR_TIMER    (dword) (60)

#if defined(FEATURE_JCDMA)
/* "Lost_main_timer", set to 2 hrs
*/
#define CMPREF_LOST_MAIN_TIMER         (dword) (7200)
#else
/* "Lost_main_timer", set to 120 seconds.
*/
#define CMPREF_LOST_MAIN_TIMER         (dword) (120)
#endif /* defined(FEATURE_JCDMA) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of PREF states.
*/
typedef enum {

                                       /* 0 */
  CMPREF_STATE_NULL,                   /* Default powerup state.
                                       */

  CMPREF_STATE_HY_OFF_SS_MAIN,         /* Phone is in non-hybrid operation,
                                       ** and SS-Main is attempting
                                       ** acquisition.
                                       */

  CMPREF_STATE_HY_OFF_SS_MAIN_SRV,     /* Phone is in non-hybrid operation,
                                       ** and SS-Main has acquired idle
                                       ** system.
                                       */

  CMPREF_STATE_HY_ON_SS_MAIN,          /* Phone is in hybrid operation and
                                       ** SS-Main is atttempting acquisition.
                                       */

                                       /* 4 */
  CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR,  /* Phone is in hybrid operation and
                                       ** has found system on SS-MAIN and
                                       ** SS-HDR is attempting acquisition.
                                       */

  CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_ACQ_TO,
                                       /* Phone is in hybrid operation, SS-Main
                                       ** has found system to idle and SS-HDR
                                       ** has failed to acquire service within
                                       ** ACQ_HDR_TIMEOUT.
                                       */

  CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_ACQ_TO,
                                       /* Phone is in hybrid operation, SS-Main
                                       ** has lost service and SS-HDR has failed
                                       ** to acquire service within
                                       ** ACQ_HDR_TIMEOUT.
                                       */

  CMPREF_STATE_HY_ON_SS_MAIN_SRV_HDR_SRV,
                                       /* Phone is in hybrid operation, SS-Main
                                       ** and SS-HDR both have acquired service.
                                       */

                                       /* 8 */
  CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV,
                                       /* Phone is in hybrid operation, SS-Main
                                       ** and SS-HDR both have acquired service.
                                       */

  CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_UMTS_SRV,
                                       /* Phone is in hybrid operation, SS-Main
                                       ** has no service and UMTS has service
                                       */

  CMPREF_STATE_HY_ON_SS_MAIN_SRV_UMTS,
                                       /* Phone is in Hybrid operation, SS-Main
                                       ** has service and UMTS is attempting
                                       ** Acquisitions
                                       */

  CMPREF_STATE_HY_ON_SS_MAIN_SRV_UMTS_SRV,
                                       /* Phone is in hybrid operation, SS-Main
                                       ** has service (pseudo online )
                                       ** and UMTS has service
                                       */

                                       /* 12 */
  CMPREF_STATE_MAX                     /* For internal use. */

} cmpref_state_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#define CMPREF_IS_COLLOC_ORIG( state ) \
            ( (state) >= CMPREF_STATE_HY_ON_SS_MAIN && \
               (state) <= CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_HDR_SRV )

#define CMPREF_IS_HYBR_GW_ORIG( state ) \
            ( (state) >= CMPREF_STATE_HY_ON_SS_MAIN_NO_SRV_UMTS_SRV && \
               (state) <= CMPREF_STATE_HY_ON_SS_MAIN_SRV_UMTS_SRV )

/* A type for parameters associated with the preferences for a particular
** SS instance.
*/
typedef struct {

  /* origination mode (normal, OTASP, emergency, other, etc.).
  */
  sd_ss_orig_mode_e_type               orig_mode;

  /* mode preference (AMPS, CDMA, any, etc.) - ignored when
  ** orig_mode = OTASP or emergency.
  */
  sd_ss_mode_pref_e_type               mode_pref;

  /* band preference (BC0, BC1, any, etc.) - ignored when
  ** orig_mode = OTASP  or emergency.
  */
  sd_ss_band_pref_e_type               band_pref;

  /* PRL preference (AVAIL_BC0_A, any, etc.) - ignored when
  ** orig_mode = OTASP  or emergency.
  */
  sd_ss_prl_pref_e_type                prl_pref;

  /* roam preference (any, home, affiliated, etc.) - ignored
  ** when orig_mode = OTASP or emergency.
  */
  sd_ss_roam_pref_e_type               roam_pref;

  /* hybrid preference (none, CDMA/HDR).
  */
  sd_ss_hybr_pref_e_type               hybr_pref;

  /* The band-class that is associated with OTASP origination mode.
  ** Note that this parameter is ignored if orig_mode != OTASP.
  */
  sd_band_e_type                       otasp_band;

  /* The PCS frequency block/Cellular system that is associated
  ** with OTASP origination mode. Note that this parameter is
  ** ignored if orig_mode != OTASP.
  */
  sd_blksys_e_type                     otasp_blksys;

  /* Type of the system to be avoided, valid only if
  ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
  */
  sd_ss_avoid_sys_e_type               avoid_type;

  /* Time in seconds for which the system is to be avoided,
  ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
  */
  dword                                avoid_time;

  /* When the net_sel_pref is manual, this field gives the Id
  ** of the system to acquire.
  */
  sys_manual_sys_info_s_type          sys_id;

  /* New service domain preference (CS, PS, CS_PS etc).
  ** Applicable for GSM/WCDMA modes.
  */
  sd_ss_srv_domain_pref_e_type         domain_pref;

  /* Preference for the order of acquisition between modes.
  ** ( WCDMA before GSM, GSM before WCDMA etc ).
  */
  sd_ss_acq_order_pref_e_type          acq_order_pref;

  /* WLAN Technology preference (a/b/g/bg/any).
  */
  sd_ss_tech_pref_e_type               wlan_tech_pref;

  sys_wlan_scan_pref_e_type            wlan_scan_pref;

  sys_wlan_bss_e_type                  wlan_bss_type_pref;


} cmpref_ss_pref_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that holds the state and other information of the
** PREF.
*/
typedef struct {

  /* Current state of PREF module, default state = NULL.
  */
  cmpref_state_e_type                  state;

  /* SS-Main's no service timer.
  */
  dword                                lost_main_uptime;

  /* SS-HDR's acquire timer.
  */
  dword                                acq_hybr_1_uptime;

  /* SS-HDR's avoid hybr hdr timer.
  */
  dword                                avoid_hybr_hdr_uptime;

  /* Is "pref_sys_chgd with pref_reas = USER" to SS-HDR pending.
  */
  boolean                              is_kick_hdr_pending;

  /* Mode preference mask, mode_preference in use would "mask & PH mode_pref"
  */
  sd_ss_mode_pref_e_type               mode_pref_mask;

  /* SS-HDR preferences to use.
  */
  cmpref_ss_pref_s_type                hybr_1_ss_pref;

  /* SS-WLAN preferences to use.
  */
  cmpref_ss_pref_s_type                wlan_ss_pref;

} cmpref_info_s_type;



/*===========================================================================
=============================================================================
=============================================================================
==================== PREF module's INTERNAL FUNCTIONS ============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmpref_get_info_ptr

DESCRIPTION
  This function will fetch the pointer to the PREF's info.

DEPENDENCIES
  None.

RETURN VALUE
  cmpref_info_s_type* - Pointer to PREF's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN cmpref_info_s_type*             cmpref_get_info_ptr
(
       void

);


/*===========================================================================

FUNCTION cmpref_set_wlan_pref

DESCRIPTION
  This function will modify cmpref's WLAN information based on input
  strcuture.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  CMPREF WLAN INFO is modified.
===========================================================================*/
MMEXTN  void                           cmpref_set_wlan_pref
(
        const cm_wlan_pref_s_type           *wlan_pref
);

#endif /* CMPREFI_H */
