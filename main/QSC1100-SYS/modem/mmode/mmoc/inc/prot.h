#ifndef PROT_H
#define PROT_H
/*===========================================================================

                        C O M M O N    P R O T O C O L
                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all common types required for the MMoC to
  communicate with the protocols and vice versa.

Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM_MM/APPS/vcs/prot.h_v   1.1   07 Jun 2002 13:23:22   sjaikuma  $
  $Header: //source/qcom/qct/modem/mmode/mmoc/rel/08H1/inc/prot.h#2 $ $DateTime: 2009/02/06 17:50:24 $ $Author: ajonas $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/09   aj      Update prot_subsc_chg_e_type
09/04/08   sv      Added WLAN_WPS_START and WLAN_WPS_ABORT generic protocol
                   commands
03/13/08   sk      Added PROT_DEACT_FROM_DORMANT to support CM/CMREGPRX
                   changes for allowing CDMA scans during suspended PS data
                   call.
04/17/07   rp      Added RPC metacomments for WLAN.   
01/04/07   pk      Added support for VoIP -> 1X Handoff
11/15/06   sk      Added hybrid GW support.
08/29/06   sk      Added WLAN Adhoc support.
04/03/06   ka      Changing copyright.
02/28/05   ka      Added new enums and macros for recording protocol that
                   had a subscription change.
09/28/04   sj      Added support for WLAN.
08/20/04   sj      Added support for prl_pref.
09/03/02   SJ      Unfeaturized GSM support in MMoC.
06/07/02   SJ      Renamed PROT_TRANS_NONE_PENDING to PROT_TRANS_NONE.
03/28/02   SJ      Initial release.

===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "sd.h"        /* Interface to System Determination 2.0 */
#include "sys.h"       /* common header file */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== COMMON TYPES =================================
=============================================================================
=============================================================================
===========================================================================*/

/* A type that defines the command sequence number type in MMOCs command to
** different protocols.
*/
typedef    byte    prot_trans_type;


/* Reserved values for prot_trans_type.
*/
/* To indicate that no transaction is pending.
*/
#define    PROT_TRANS_NONE            (prot_trans_type)(0)

/* To indicate that the report was generated autonomously.
*/
#define    PROT_TRANS_AUTONOMOUS      (prot_trans_type)(1)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of protocol states in the MMoC.
*/
typedef enum {

  PROT_STATE_NULL,                     /* Initial state upon powerup. */


  PROT_STATE_ONLINE_CDMA,              /* Phone is in ONLINE mode with CDMA
                                       ** as one of the active protocols.
                                       */

  PROT_STATE_OFFLINE_CDMA,             /* Phone is in OFFLINE mode with CDMA
                                       ** as the active protocol.
                                       */

  PROT_STATE_ONLINE_AMPS,              /* Phone is in ONLINE mode with AMPS
                                       ** as one of the active protocols.
                                       */

  PROT_STATE_OFFLINE_AMPS,             /* Phone is in OFFLINE mode with CDMA
                                       ** as the active protocol.
                                       */

  PROT_STATE_ONLINE_GW,                /* Phone is in ONLINE mode with GSM/
                                       ** WCDMA as one of the active
                                       ** protocols.
                                       */

  PROT_STATE_ONLINE_HDR,               /* Phone is in ONLINE mode with HDR
                                       ** as one of the active protocols.
                                       */

  PROT_STATE_ONLINE_WLAN,               /* Phone is in ONLINE mode with WLAN
                                       ** as one of the active protocols.
                                       */

  PROT_STATE_OFFLINE,                  /* Phone is in OFFLINE mode with no
                                       ** active protocols.
                                       */

  PROT_STATE_FTM,                      /* Phone is in FTM mode. Here CDMA is
                                       ** the active protocol, since FTM is
                                       ** implemented in MC task.
                                       */

  PROT_STATE_PWR_SAVE,                 /* Phone is in Power save mode with no
                                       ** active protocols.
                                       */


  PROT_STATE_ONLINE_DED_MEAS,          /* Phone is doing signal strength
                                       ** measurement in dedicated mode.
                                       ** Here CDMA is active protocol since
                                       ** the logic is implemented in MC
                                       ** task.
                                       */

  PROT_STATE_RESET,                    /* Phone is in RESET mode with no
                                       ** active protocols.
                                       */

  PROT_STATE_LPM,                      /* Phone is in LPM mode with no
                                       ** active protocols.
                                       */

  PROT_STATE_PWR_DOWN,                 /* Phone is powering down.
                                       */

  PROT_STATE_GPSONE_MSBASED,           /* Phone is in GPS state.
                                       */

  PROT_STATE_MAX                       /* For internal use. */


} prot_state_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of protocol activate reason type for CDMA, AMPS, HDR, GSM/WCDMA
** protocols.
*/
typedef enum {

  PROT_ACT_ONLINE_ACQ,                 /* Activate a protocol to acquire a
                                       ** system, acquisition parameters given
                                       ** by SD20.
                                       ** Applicable for CDMA, AMPS, HDR,
                                       ** GSM/WCDMA protocols.
                                       */

  PROT_ACT_OFFLINE,                    /* Activate the protocol and place it
                                       ** in OFFLINE mode.
                                       ** Applicable for CDMA and AMPS
                                       ** protocol.
                                       */

  PROT_ACT_FTM,                        /* Activate the CDMA protocol and
                                       ** place it in FTM(Factory Test
                                       ** mode).
                                       ** Applicable for CDMA protocol.
                                       */

  PROT_ACT_CDMA_AMPS_HANDOFF,          /* Activate the AMPS protocol for the
                                       ** reason of performing a CDMA to AMPS
                                       ** handoff. The handoff is provided in
                                       ** activate protocol command.
                                       ** Applicable for AMPS protocol.
                                       */

  PROT_ACT_GET_NETWORKS,               /* Activate the protocol for the
                                       ** reason of performing a Get networks
                                       ** command.
                                       ** Applicable for GSM/WCDMA protocol.
                                       */

  PROT_ACT_DED_MEAS_REQUEST,           /* Activate the CDMA protocol for the
                                       ** reason of initiating a measurement
                                       ** request to SRCH in dedicated mode.
                                       ** Applicable for CDMA protocol.
                                       */

  PROT_ACT_PSEUDO_ONLINE,              /* Activate the protocol to acquire a
                                       ** system but no Tx should be done by
                                       ** the protocol.
                                       */

  PROT_ACT_ONLINE_HDR_CDMA_HO,         /* Activate CDMA for Handoff
                                       */

  PROT_ACT_MAX                         /* For internal use */

} prot_act_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of protocol deactivate reason type for CDMA, AMPS, HDR,
** GSM/WCDMA protocols.
*/
typedef enum {

  PROT_DEACT_PWR_DOWN,                 /* Deactivate a protocol in order to
                                       ** perform power down command from UI
                                       ** Applicable for all protocols.
                                       */

  PROT_DEACT_RESET,                    /* Deactivate a protocol in order to
                                       ** perform reset command from UI
                                       ** Applicable for all protocols.
                                       */

  PROT_DEACT_LPM,                      /* Deactivate a protocol in order to
                                       ** perform LPM command from UI
                                       ** Applicable for all protocols.
                                       */

  PROT_DEACT_OFFLINE,                  /* Deactivate a protocol in order to
                                       ** perform offline command from UI
                                       ** Applicable for all protocols.
                                       */

  PROT_DEACT_CDMA_AMPS_HANDOFF,        /* Deactivated a protocol in order to
                                       ** perform a CDMA to AMPS handoff.
                                       ** Generated by CDMA protocol only.
                                       */

  PROT_DEACT_ACQ_OTHERS,               /* Generic deactivated protocol reason.
                                       ** Applicable for all protocols.
                                       */


  PROT_DEACT_SUBSC_CHGD,               /* Protocol should be deactivated
                                       ** because of change in subscription.
                                       */

  PROT_DEACT_FTM,                      /* Protocol should be deactivated
                                       ** because of FTM.
                                       */

  PROT_DEACT_FROM_DORMANT,             /* Protocol should be deactivated
                                       ** because dormancy is no longer
                                       ** required.
                                       */

  PROT_DEACT_MAX                       /* For internal use */

} prot_deact_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of generic command for CDMA, AMPS, HDR, GSM/WCDMA
** protocols.
*/
typedef enum {

  PROT_GEN_CMD_SS_TIMER_EXP,           /* Reselection timer expired command.
                                       */

  PROT_GEN_CMD_PREF_SYS_CHGD,          /* Preferred system selection changed
                                       ** command.
                                       */

  PROT_GEN_CMD_GET_NETWORKS_GW,        /* Get networks command for GSM/WCDMA.
                                       */

  PROT_GEN_CMD_TERM_GET_NETWORKS_GW,   /* Terminate Get networks command for
                                       ** GSM/WCDMA.
                                       */

  PROT_GEN_CMD_GET_NETWORKS_WLAN,      /* Get networks command for WLAN.
                                       */

  PROT_GEN_CMD_TERM_GET_NETWORKS_WLAN, /* Terminate Get networks command for
                                       ** WLAN.
                                       */

  PROT_GEN_CMD_WLAN_WPS_START,         /**< WPS start command for WLAN.
                                       */

  PROT_GEN_CMD_WLAN_WPS_ABORT,         /**< WPS abort command for WLAN.
                                       */

  PROT_GEN_CMD_MAX                     /* For internal use */

} prot_gen_cmd_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of MMoC command types to the protocols.
*/
typedef enum {

  PROT_CMD_ACTIVATE,                   /* Command to activate the protocol.
                                       */

  PROT_CMD_DEACTIVATE,                 /* Command to deactivate the protocol.
                                       */

  PROT_CMD_GENERIC,                    /* Generic system selection command
                                       ** to the protocol.
                                       */

  PROT_CMD_PH_STAT_CHGD,               /* Command to inform the protocols of
                                       ** a change either due to
                                       ** subscription availability or
                                       ** operating mode change.
                                       */

  PROT_CMD_MAX                         /* For internal use. */

} prot_cmd_e_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of operating mode status in MMoC.
*/
typedef enum
{
  PROT_OPRT_MODE_NULL,                    /* Operating mode status when the
                                          ** phone has powered up.
                                          */

  PROT_OPRT_MODE_PWROFF,                  /* Operating mode status when the
                                          ** phone was powered down.
                                          */

  PROT_OPRT_MODE_OFFLINE,                 /* Operating mode status when the
                                          ** phone moved to offline mode.
                                          */


  PROT_OPRT_MODE_OFFLINE_AMPS,            /* Operating mode status when the
                                          ** phone moved to offline AMPS mode.
                                          */


  PROT_OPRT_MODE_OFFLINE_CDMA,            /* Operating mode status when the
                                          ** phone moved to offline CDMA mode.
                                          */


  PROT_OPRT_MODE_ONLINE,                  /* Operating mode status when the
                                          ** phone moved to online mode.
                                          */

  PROT_OPRT_MODE_LPM,                     /* Operating mode status when the
                                          ** phone moved to LPM mode.
                                          */


  PROT_OPRT_MODE_RESET,                   /* Operating mode status when the
                                          ** phone moved to reset mode.
                                          */


  PROT_OPRT_MODE_FTM,                     /* Operating mode status when the
                                          ** phone moved to FTM mode.
                                          */


  PROT_OPRT_MODE_MAX                      /* For internal use.
                                          */

} prot_oprt_mode_e_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of phone status change type for CDMA, AMPS, HDR, GSM/WCDMA
** protocols. Applicable whenever there is a change in SIM availability,
** NAM selection change or operating mode change. The protocols will do
** specific housekeeping actions based on this type.
*/
typedef enum {

  PROT_PH_STAT_CHG_OPRT_MODE,          /* Phone status changed due to change
                                       ** in operating mode.
                                       */

  PROT_PH_STAT_CHG_SUBSC,              /* Phone status changed due to change
                                       ** in subscription.
                                       */

  PROT_PH_STAT_CHG_NAM,                /* Phone status changed due to change
                                       ** in NAM selection.
                                       */

  PROT_PH_STAT_CHG_MAX                 /* For internal use */

} prot_ph_stat_chg_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of protocols that have a change in subscription.
** Used when subscription available for a protocol changes. Has been
** added to prevent protocols from reloading NV when ph_status_chgd_cmd
** is called.
*/
typedef enum {

  PROT_SUBSC_CHG_NONE       = 0,                       /* subscription chng
                                                       ** for no protocol
                                                       */

  PROT_SUBSC_CHG_CDMA       = BM(SYS_SYS_MODE_CDMA),   /* subscription chng
                                                       ** for cdma only
                                                       */

  PROT_SUBSC_CHG_GSM        = BM(SYS_SYS_MODE_GSM) ,   /* subscription chng
                                                       ** for GSM only
                                                       */

  PROT_SUBSC_CHG_WCDMA      = BM(SYS_SYS_MODE_WCDMA) , /* subscription chng
                                                       ** for WCDMA only
                                                       */

  PROT_SUBSC_CHG_WLAN       = BM(SYS_SYS_MODE_WLAN) ,  /* subscription chng
                                                       ** for WLAN only
                                                       */

  PROT_SUBSC_CHG_GW         = BM(SYS_SYS_MODE_GSM) | BM(SYS_SYS_MODE_WCDMA) ,
                                                       /* subscription chng
                                                       ** for GW
                                                       */
  PROT_SUBSC_CHG_ALL        = BM(SYS_SYS_MODE_CDMA) | \
                              BM(SYS_SYS_MODE_GSM) | BM(SYS_SYS_MODE_WCDMA)| \
                              BM(SYS_SYS_MODE_WLAN),  /* subscription change for
                                                      ** all protocols
                                                      */


  PROT_SUBSC_CHG_MAX                                   /* For internal use */

} prot_subsc_chg_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Adding new protocol
*/
#define    ADD_PROTO(X,Y) (prot_subsc_chg_e_type) ((int)X | (int)Y)

/* Checking for particular protocol
** Checks if protocol Y of type prot_subsc_chg_e_type
** is currently present in X.
*/
#define    CHECK_PROTO(X,Y)  ((int) X & (int) Y)



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with the preferred system
** selection changed command, part of generic system selection command to
** the protocols.
*/
typedef struct {

  /* Reason for changing the SS-Preference.  */
  sd_ss_pref_reas_e_type               pref_reas;

  /* New origination mode (normal, OTASP, emergency, other, etc.).
  */
  sd_ss_orig_mode_e_type               orig_mode;

  /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
  ** orig_mode = OTASP or emergency.
  */
  sd_ss_mode_pref_e_type               mode_pref;

  /* New band preference (BC0, BC1, any, etc.) - ignored when
  ** orig_mode = OTASP  or emergency.
  */
  sd_ss_band_pref_e_type               band_pref;

  /* New prl preference.
  */
  sd_ss_prl_pref_e_type                prl_pref;

  /* New roam preference (any, home, affiliated, etc.) - ignored
  ** when orig_mode = OTASP or emergency.
  */
  sd_ss_roam_pref_e_type               roam_pref;

  /* New hybrid preference (none, CDMA/HDR).
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

  /* Information about the manual system.
  */
  sys_manual_sys_info_s_type           manual_sys_info;

  /* New service domain preference (CS, PS, CS_PS etc).
  ** Applicable for GSM/WCDMA modes.
  */
  sd_ss_srv_domain_pref_e_type         domain_pref;

  /* Preference for the order of acquisition between modes.
  ** ( WCDMA before GSM, GSM before WCDMA etc ).
  */
  sd_ss_acq_order_pref_e_type          acq_order_pref;

  /* Preference for the technology.
  */
  sd_ss_tech_pref_e_type               tech_pref;

  /* Preference for the wlan scanning.
  */
  sys_wlan_scan_pref_e_type            wlan_scan_pref;

  /* Preference for the wlan BSS type.
  */
  sys_wlan_bss_e_type                  wlan_bss_type_pref;

} prot_cmd_pref_sys_chgd_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with the GSM/WCDMA get networks
** command.
*/
typedef struct {

  /* Mode preference for the Get networks command. Can be GSM_ONLY,
  ** WCDMA_ONLY or GW_ONLY only.
  */
  sd_ss_mode_pref_e_type               mode_pref;


  /* Band preference that is associated with the Get networks
  ** command.
  */
  sd_ss_band_pref_e_type               band_pref;

} prot_cmd_gw_get_net_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with the WLAN get networks
** command.
*/
typedef struct {

  /* Bands to search for WLAN networks.
  */
  sd_ss_band_pref_e_type               bands;

} prot_cmd_wlan_get_net_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** A type for parameters that are associated with the WLAN WPS ** command.
*/
typedef struct {

  /** WLAN WPS start parameters.
  */
  sys_wlan_wps_start_param_s_type      start_params;

} prot_cmd_wlan_wps_start_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated generic system selection command
** to the protocols.
*/
typedef struct {

  /* Generic command type (reselection timeout, get networks etc).
  */
  prot_gen_cmd_e_type                  cmd_type;

  /* Transaction Id for the generic command.
  ** Note! trans_id is used only when communicating with the protocols.
  ** Invalid when it is in the MMoC's command queue.
  */
  prot_trans_type                      trans_id;


  /* Generic command to this SD SS Instance.
  */
  sd_ss_e_type                         ss;


  /* MMoC's current protocol state for ss.
  */
  prot_state_e_type                    prot_state;

  /* Parameters associated with the above ss command type.
  */
  union
  {

    prot_cmd_pref_sys_chgd_s_type        pref_sys_chgd;

    prot_cmd_gw_get_net_s_type           gw_get_net;

    prot_cmd_wlan_get_net_s_type         wlan_get_net;

    prot_cmd_wlan_wps_start_s_type       wlan_wps_start;

  } param;

} prot_gen_cmd_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD prot_gen_cmd_s_type.param DISC prot_gen_cmd_s_type.cmd_type */
/*~ CASE PROT_GEN_CMD_PREF_SYS_CHGD prot_gen_cmd_s_type.param.pref_sys_chgd */
/*~ CASE PROT_GEN_CMD_GET_NETWORKS_GW prot_gen_cmd_s_type.param.gw_get_net */
/*~ CASE PROT_GEN_CMD_TERM_GET_NETWORKS_WLAN prot_gen_cmd_s_type.param.wlan_get_net */
/*~ CASE PROT_GEN_CMD_WLAN_WPS_START prot_gen_cmd_s_type.param.wlan_wps_start */
/*~ DEFAULT prot_gen_cmd_s_type.param.void */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with information on Activate
** protocol command.
*/
typedef struct {

  /* Protocol Activate reason (Online acquire, offline etc).
  */
  prot_act_e_type                      reason;

} prot_act_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with information on deactivate
** protocol command.
*/
typedef struct {

  /* Protocol deactivate reason (Powerdown, reset etc).
  */
  prot_deact_e_type                    reason;

  /* Transaction Id that should be used for acknowledging this command.
  */
  prot_trans_type                      trans_id;

} prot_deact_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type for parameters that are associated with information on phone status
** changed protocol command.
*/
typedef struct {

  /* Phone status change type (NAM selection, Operating mode etc).
  */
  prot_ph_stat_chg_e_type              chg_type;

  /* Current NAM selection.
  */
  byte                                 nam;

  /* Current GSM/WCDMA subscription availability status.
  */
  boolean                              is_gw_subs_avail;

  /* Current CDMA/AMPS/HDR subscription availability status.
  */
  boolean                              is_cdma_subs_avail;

  /* Current operating mode of the phone.
  */
  prot_oprt_mode_e_type                oprt_mode;

  /* Transaction Id that should be used for acknowledging this command.
  */
  prot_trans_type                      trans_id;

  /* Enumeration of protocols that have a change in
  ** subscription available status
  */
  prot_subsc_chg_e_type                prot_subsc_chg;

} prot_ph_stat_chgd_s_type;


#endif /* PROT_H */
