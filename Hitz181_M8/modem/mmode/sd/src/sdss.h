#ifndef SDSS_H
#define SDSS_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

        S Y S T E M   S E L E C T I O N   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with SDSS.C (i.e. the System Selection components of the SD).

Copyright (c) 2000 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/sdss.h#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
02/13/09   sn      For user pwr save exit event, in pwr save, continue with 
                          earlier  4 + 1 GW  band group scan sequence.
12/05/08   sn      UOOS GW lmtd search for multimode phones when GW subs not 
                   available.
11/28/08   rm      In UOOS report Limited service to NAS if sim state is
                   CS_PS_Invalid. Reverting part of the fix for CR-124247 
11/05/08   sv      Moved sdssscr_flag_e_type to sdss_flag_e_type
                   Added sdss_update_flag to update stack_sync_up_flag
10/23/08  ak      Fixing the no firmware scenario
09/26/08   cl      Fix the system to use TMR_HDR_REDIR under DO Redirection
09/15/08   pk      Added support for WLAN UOoS
08/12/08   ak      Implemented the new Sprint BSR algorithm
06/20/08   sn      BSR 2.0 changes
06/11/08   fj      Added sdss_get_mode_band_capability
04/30/08   cl      Adding support for API sd_misc_sys_avoid_list, which list all  
                   the avoided systems. HDR/SCHR use this info to update their 
                   scan list when doing OFS.
04/28/08   ak      Added sd_ss_ind_cdma_opr_prot_mis & ssscr_hdr_opr_prot_mis
04/28/08   pk\rn   Reset the acq_scan_status if user changes the preferences
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
03/19/08   cl      Fixing UOOS Time to support Auto Mode Builds.
03/17/08   vs      Added prev_sys_info to the sd state structure,
                   Added functions to get/set prev_sys_info,
03/14/08   rm      CM-NAS interface for new service req type
02/19/08   cl      Optimize the execution time of SD API sd_misc_is_fts_mode
02/13/08   vs      Added SDSS_IF_COND_SS_STATE to check selection state
01/22/08   rm      Reverted the changes done for- support for SIM Invalid
                   and UOOS
09/18/07   pk      Added support for non-Hybrid UOOS
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
06/15/07   rp      Adding sdss_update_rem_uoos_time()
06/06/07   pk      UOOS enhancements...
05/29/07   pk      Added support for SIM Invalid and UOOS
05/29/07   pk      UMTS UOOS enhancements
04/23/07   jqi     Added Filtering support for stopping HDR activity.
                   Added support for sd_misc_is_orig_mode.
03/28/07   sk      Added sdss_is_gprs_anite_gcf().
02/26/07   pk      Added sdss_get_ss_for_mode for all featurizations
01/04/07   pk      Added support for VoIP -> 1X Handoffs
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS
                   builds
12/03/06   ka      Adding sdss_map_mode_pref_to_mode()
11/29/06   ka      Limited service resel timer expiry results in sending
                   MEAS_BACK action instead of full service req. Satisfies
                   GCF 6.1.2.6.
11/16/06   pk      Using sys_simultaneous_mode_e_type for hybr_gw
11/15/06   pk      Added support for HYBR_GW
09/08/06   sk      Added event SDSS_EVT_MISC_WLAN_MEAS_RPRT.
08/29/06   sk      Added WLAN Adhoc support.
08/24/06   pk      Added support for IF construct and GCF test fixes
08/24/06   pk      Added support for sdss_nv_init
08/23/06   jqi     Added support for system reselection during BCMCS flow.
08/15/06   pk      Added support for GW UOOS
08/07/06   pk      Added support for 1X UOOS
07/13/06   jqi     Added the system lost reason for HDR system lost event.
07/07/06   sk      Added sdss_get_srv_status().
04/17/05   jqi     Added EGSRDM support.
04/09/06   jqi     Increased Register Failure Counter to 6 in over reach.
                   Made "1x Reselection Threshold" as a configurable NV item.
                   Mainline FEATURE_SILENT_REDIAL_20.
                   Connection Deny reason(network busy), avoid the channel
                   60s.
                   No TCA and NoRTC ACK, 3 failures avoid 60s or encountering
                   ACQ_FAIL, whichever come first.
                   Scan all channels during overreach wake up.
                   Do not cancel avoidance when entering power save mode.
                   Added null pointer check in sdsr_list_is_fade.
                   Mainline FEATURE_SILENT_REDIAL_20
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
                   Do not change sd state if only srv domain is changed.
03/06/06   jqi     Added comments for bad system.
12/02/05   jqi     Lint 8 fix.
11/07/05   jqi     Removed the featurization around sdss_is_hybr_pref().
10/18/05   jqi     Reorder the event.
09/01/05   jqi     Use different scripts based on the SS item during event
                   recovery process.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/11/05   jqi     Added avoidance timer for access failures and fades.
07/08/05   sk      Added event SDSS_EVT_HDR_OPR_SES_NEGO_TIMEOUT and a
                   function declaration.
                   Added two new enums:
                     - sdss_cnted_evt_e_type
                     - sdss_evt_cnt_updt_reas_e_type
05/13/05   ka      API for avoiding WLAN system.
05/04/05   jqi     Added support to process HDR bad system event while in
                   acquisition state.
01/30/05   sj      Added support for WLAN.
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
10/26/04   jqi     Read subscription items only when the subscription is
                   available.
09/08/04   jqi     Added ps dormant optimization.
08/08/04   sj      Added parameter to sdss_subsc_avail_update() to specify
                   SS-Main or SS-HDR.
08/03/04   sj      Added function to query the operating mode.
07/29/04   jqi     Quadband optimizatioin - eliminate scheduling.
07/11/04   sj      Added srv_domain parameter in update_srv_status().
07/09/04   sj      Changed prototype for pref_update()s.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
05/25/04   jqi     Fixed mode preference reset issue after power up.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
04/13/04   sj      Added support for acquisition throttling.
03/29/04   jqi     Added sdss_iact_pwr_scan_gw_s_type to wrap the sys info
                   for GW pwr scan request.
02/27/04   jqi     Eliminate system scan whose bands does not comply with
                   target capability or system mode or mode preference.
02/06/04   SJ      Changed param type for get_acc_prm().
01/26/04   SJ      Added orig_mode_manual to orig_mode_any enum.
01/23/04   JQ      Passed down most recent acquired band group to GW stack
                   during early power scan.
01/22/04   SJ      Added support for new script construct is_pref_chgd().
12/12/03   RC      Added support for FEATURE_INT_ROAMING
12/08/03   SJ      Added support for multistage full srv reselection.
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
10/24/03   RC      Changed SDSR_GW_LST to contain a single system of mode
                   SD_MODE_GW.
08/26/03   SJ      Added ss parameter to sdss_get_pref().
08/18/03   RC      Fixed issues with IS-683D implementation.
07/15/03   JQ      Added Extended PRL support
                   - D PRL support
06/10/03   RC      Added counter to track CDMA available indications.
04/28/03   RC      Added support for event logging.
04/23/03   SJ      Added early power scan support for GSM/WCDMA.
01/16/03   SJ      Featurized DB accesses.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
07/17/02   RC      Externalized sdss_event_recovery().
02/12/02   RC      Externalized sdss_eng_stack_get_state().
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
10/30/01   RC      Added support for IS-683C collcated systems.
09/07/01   RC      Added support for T53 and JCDMA features.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
01/12/01   RC      Changes per 6th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
12/14/00   RC      Changes per 3rd week of I&T.
12/04/00   RC      Changes per 2nd week of I&T.
11/17/00   RC      Initial release.

===========================================================================*/


#include "sd.h"       /* External interface to SD */
#include "sdi.h"      /* Internal interface to sd.c */
#include "sdsr.h"     /* External interface to SDSR.C */
#include "prl.h"      /* External interface to prl.c */

#include "comdef.h"   /* Definition for basic types and macros */

#ifdef SD_USES_DB_SERVICES
#include "db.h"       /* Interface to database services */
#endif


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================= SYSTEM SELECTION ==============================
=============================================================================
=============================================================================
===========================================================================*/

#define SDSS_DEFAULT_UOOS_DUTY_CYCLE 7

/* <EJECT> */
/*---------------------------------------------------------------------------
                          System Selection Events
---------------------------------------------------------------------------*/

/* Enumeration of SS-Events.
*/
typedef enum {

  /* SS-Events that are a result of User Indications.
  */

  SDSS_EVT_USER                   = 0, /* FOR RANGE CHECKING */


  SDSS_EVT_USER_PWR_UP,           /* User commands phone to power-up */

  SDSS_EVT_USER_OFFLINE_CDMA,     /* User commands phone to enter offline
                                  ** CDMA mode */

  SDSS_EVT_USER_OFFLINE_AMPS,     /* User commands phone to enter offline
                                  ** AMPS mode */

                                  /* 4 */
  SDSS_EVT_USER_PWR_SAVE_EXIT,    /* User commands phone to exit Power Save
                                  ** Mode of operation */

  SDSS_EVT_USER_SS_PREF,          /* User changes the system selection
                                  ** preference and or origination mode */

  SDSS_EVT_USER_NAM_SEL,          /* User changed the NAM selection */

  SDSS_EVT_USER_PWR_DOWN,         /* User commands phone to power-off,
                                  ** perform a reset or enter LPM */

  SDSS_EVT_USER_AVOID_SYS,        /* User command to avoid a particular
                                  ** system
                                  */

  SDSS_EVT_USER_GET_NET_GW,       /* User command to get the available
                                  ** GSM/WCDMA systems.
                                  */

  SDSS_EVT_USER_TERM_GET_NET_GW,  /* User command to get the available
                                  ** GSM/WCDMA systems.
                                  */

  SDSS_EVT_USER_PROT_DEACTIVATE,  /* User command to deactivate the
                                  ** protocol.
                                  */

  SDSS_EVT_USER_MAX,              /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of Miscellaneous Indications.
  */

  SDSS_EVT_MISC                   = 100, /* FOR RANGE CHECKING */

  SDSS_EVT_MISC_TIMER_RESEL_OK,   /* The SS-Timer expires - reselection is
                                  ** allowed */

  SDSS_EVT_MISC_TIMER_RESEL_NO,   /* The SS-Timer expires - reselection is
                                  ** not allowed */

  SDSS_EVT_MISC_MEAS_RPRT,        /* The channel strength measurement reports
                                  ** that was requested by SD come back from
                                  ** Search */

                                  /* 104 */
  SDSS_EVT_MISC_ORIG_SUCCESS,     /* Origination is successful */

  SDSS_EVT_MISC_PWR_SAVE_ENTER,   /* Phone enters power save mode of
                                  ** operation */

  SDSS_EVT_MISC_FILLER,           /* Filler event - fill in for non-events */

  SDSS_EVT_MISC_GW_NET_LIST,      /* The GW network list report from REG.
                                  */

                                  /* 108 */
  SDSS_EVT_MISC_CNF,              /* Phone confirms the last instructed
                                  ** action. */

  SDSS_EVT_MISC_WLAN_SURVEY_LIST, /* Survey report from WLAN CP.
                                  */

  SDSS_EVT_MISC_WLAN_MEAS_RPRT,   /* The WLAN system strength measurement
                                  ** reports that was requested by SD come
                                  ** back from Search
                                  */


  SDSS_EVT_MISC_MAX,              /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are internally generated by the SS-Core.
  */

  SDSS_EVT_INT                    = 200, /* FOR RANGE CHECKING */


  SDSS_EVT_INT_SRV_LOST,          /* Service is lost */

  SDSS_EVT_INT_RESEL,             /* Reselection */

  SDSS_EVT_WLAN_INT_SRV_LOST,     /* WLAN Service is lost */

  SDSS_EVT_INT_GW_ACQ_PLMN_RETURN, /* GW PLMN search returned */

  SDSS_EVT_INT_MAX,               /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of CDMA Acquisition Indications.
  */

  SDSS_EVT_CDMA_ACQ               = 300, /* FOR RANGE CHECKING */


  SDSS_EVT_CDMA_ACQ_FAILED_PILOT, /* CDMA acquisition fails due to failure
                                  ** to acquire the pilot channel*/

  SDSS_EVT_CDMA_ACQ_FAILED_SYNC,  /* CDMA acquisition fails due to failure
                                  ** to successfully receive the sync channel
                                  ** message */

  SDSS_EVT_CDMA_ACQ_PROT_MIS,     /* CDMA acquisition fails due to protocol
                                  ** mismatch (i.e., MOB_P_REV < MIN_P_REV or
                                  ** MS does not support the Paging Channel
                                  ** data rate */

                                  /* 304 */
  SDSS_EVT_CDMA_ACQ_SCHM,         /* Successfully receives CDMA the Sync
                                  ** Channel Message and the protocol
                                  ** revision is compatible with MS */

  SDSS_EVT_CDMA_ACQ_HO_FAIL,      /* HDR to CDMA handoff failed  */


  SDSS_EVT_CDMA_ACQ_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of CDMA Operation Indications.
  */

  SDSS_EVT_CDMA_OPR               = 400, /* FOR RANGE CHECKING */


  SDSS_EVT_CDMA_OPR_OH_SID,       /* A new CDMA system is fully acquired.
                                  ** This new system differ in SID/NID (and
                                  ** possibly band-class and/or channel)
                                  ** from the previous serving system */

  SDSS_EVT_CDMA_OPR_OH_CHAN,      /* A new CDMA system is fully acquired.
                                  ** This new system differ only in band-
                                  ** class and/or channel from the previous
                                  ** serving system (i.e. the SID/NID is
                                  ** unchanged) */

  SDSS_EVT_CDMA_OPR_RESCAN,       /* MS receives a CDMA SPM with the RESCAN
                                  ** field set to 1 */

                                  /* 404 */
  SDSS_EVT_CDMA_OPR_REG_REJ,      /* CDMA Registration is rejected */

  SDSS_EVT_CDMA_OPR_SYS_LOST,     /* The acquired CDMA system is lost */

  SDSS_EVT_CDMA_OPR_REDIR,        /* MS receives CDMA redirection message */

  SDSS_EVT_CDMA_OPR_ACPT_REJ,     /* MS finishes rejecting/accepting a CDMA
                                  ** redirection */

                                  /* 408 */
  SDSS_EVT_CDMA_OPR_NDSS_OFF,     /* MS receives CDMA NDSS off indication */

  SDSS_EVT_CDMA_OPR_NEW_SYS,      /* CDMA idle-handoff to a BS with unknown
                                  ** configuration */

  SDSS_EVT_CDMA_OPR_SYS_RESEL,    /* CDMA pilot power below a specified
                                  ** threshold (switch to AMPS) */

  SDSS_EVT_CDMA_OPR_LOCK,         /* MS receives a CDMA lock order */

                                  /* 412 */
  SDSS_EVT_CDMA_OPR_UNLOCK,       /* MS receives a CDMA unlock order */

  SDSS_EVT_CDMA_OPR_ACC_TERM,     /* CDMA access attempt terminates due to
                                  ** access denied, system lost, access state
                                  ** timeout, max access probes, reorder,
                                  ** intercept, BS release, MS release or
                                  ** normal access completion */

  SDSS_EVT_CDMA_OPR_CAM_ERR,      /* MS receives a CDMA channel assignment
                                  ** that is not supported by the MS */

  SDSS_EVT_CDMA_OPR_CAM,          /* MS receives a CDMA channel assignment
                                  ** that is supported by the MS */

                                  /* 416 */
  SDSS_EVT_CDMA_OPR_HDM,          /* MS receives a CDMA handoff direction
                                  ** (i.e., HDM or xHDM message) */

  SDSS_EVT_CDMA_OPR_ITSPM,        /* MS receives a CDMA in traffic system
                                  ** parameters message with new SID/NID
                                  ** information */

  SDSS_EVT_CDMA_OPR_RELEASE,      /* CDMA call is released */

  SDSS_EVT_CDMA_OPR_PROT_MIS,     /* Protocol mismatch in Idle state */

  SDSS_EVT_CDMA_OPR_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of AMPS Acquisition Indications.
  */

  SDSS_EVT_AMPS_ACQ               = 500, /* FOR RANGE CHECKING */


  SDSS_EVT_AMPS_ACQ_FAILED_CCH,   /* AMPS acquisition attempt fails due to a
                                  ** failure to acquire any of the two
                                  ** strongest control channels */

  SDSS_EVT_AMPS_ACQ_CCH_SID,      /* MS receives SID from AMPS Control
                                  ** Channel OHM train */


  SDSS_EVT_AMPS_ACQ_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of AMPS Operation Indications.
  */

  SDSS_EVT_AMPS_OPR               = 600, /* FOR RANGE CHECKING */


  SDSS_EVT_AMPS_OPR_FAILED_PCH,   /* MS fails to acquire any of the two
                                  ** strongest paging channels */

  SDSS_EVT_AMPS_OPR_PCH_SID,      /* MS receives SID from AMPS Paging
                                  ** Channel OHM train */

  SDSS_EVT_AMPS_OPR_ACC_SID,      /* MS receives SID from AMPS access
                                  ** channel */

                                  /* 604 */
  SDSS_EVT_AMPS_OPR_RESCAN,       /* MS receives an AMPS RESCAN message */

  SDSS_EVT_AMPS_OPR_SYS_LOST,     /* The acquired AMPS system is lost */

  SDSS_EVT_AMPS_OPR_CDMA_AVAIL,   /* MS receives CDMA Available GAM (while
                                  ** in AMPS mode) */

  SDSS_EVT_AMPS_OPR_ACC_TERM,     /* AMPS access attempt terminates */

                                  /* 608 */
  SDSS_EVT_AMPS_OPR_CAM,          /* AMPS channel assignment */

  SDSS_EVT_AMPS_OPR_OVERLOAD,     /* MS receives AMPS overload message */


  SDSS_EVT_AMPS_OPR_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of HDR User Indications.
  */
  SDSS_EVT_HDR_USER               = 700, /* For range checking */

  SDSS_EVT_HDR_USER_SS_PREF,      /* HDR CP reports that user changes the
                                  ** system selection preference and or
                                  ** origination mode */

  SDSS_EVT_HDR_USER_HYBR_START,   /* HDR CP reports that user started
                                  ** CDMA+HDR hybrid mode of operation */

  SDSS_EVT_HDR_USER_HYBR_END,     /* HDR CP reports that user ended CDMA+HDR
                                  ** hybrid mode of operation */

                                  /* 704 */
  SDSS_EVT_HDR_USER_PWR_SAVE_EXIT,/* User commanded to exit power save mode.
                                  */

  SDSS_EVT_HDR_USER_MAX,         /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of HDR Acquisition Indications.
  */

  SDSS_EVT_HDR_ACQ                = 800, /* FOR RANGE CHECKING */


  SDSS_EVT_HDR_ACQ_FAILED_PILOT,  /* HDR acquisition fails due to failure to
                                  ** acquire the pilot channel */

  SDSS_EVT_HDR_ACQ_FAILED_SYNC,   /* HDR acquisition fails due to failure to
                                  ** successfully receive the sync channel
                                  ** message */

  SDSS_EVT_HDR_ACQ_PROT_MIS,      /* HDR acquisition fails due to protocol
                                  ** mismatch (i.e., the access terminal's
                                  ** revision number is not in the range
                                  ** defined by the MinimumRevision and
                                  ** MaximumRevision (inclusive) fields of
                                  ** the sync message */

                                  /* 804 */
  SDSS_EVT_HDR_ACQ_SID,           /* HDR system ID (i.e., AccessNetworkID) is
                                  ** successfully obtained for the first time
                                  ** since the system was acquired */


  SDSS_EVT_HDR_ACQ_BAD_SYS,       /* HDR system is bad
                                  ** 1.AP message with invalid max capsule
                                  ** length i.e. < 2
                                  ** 2.SP message with hash channel on
                                  ** unsupported band
                                  ** 3. TCA to unsupported band
                                  ** 4.TCA with no pilots
                                  */

  SDSS_EVT_HDR_ACQ_MAX,           /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of HDR Operation Indications.
  */

  SDSS_EVT_HDR_OPR                = 900, /* FOR RANGE CHECKING */

  SDSS_EVT_HDR_OPR_OH_SID,        /* A new HDR system is fully acquired.
                                  ** This new system differ in SID/NID (and
                                  ** possibly band-class and/or channel) from
                                  ** the previous serving system */

  SDSS_EVT_HDR_OPR_OH_CHAN,       /* A new HDR system is fully acquired.
                                  ** This new system differ only in band-
                                  ** class and/or channel from the previous
                                  ** serving system (i.e. the SID/NID is
                                  ** unchanged) */

  SDSS_EVT_HDR_OPR_REDIR,         /* MS receives an HDR redirection
                                  ** message */

                                  /* 904 */
  SDSS_EVT_HDR_OPR_SYS_LOST,      /* The acquired HDR system is lost */

  SDSS_EVT_HDR_OPR_RESEL_OK,      /* HDR protocol enters a state that allows
                                  ** reselection to take place */

  SDSS_EVT_HDR_OPR_ACC_TERM,      /* HDR access attempt terminates due to
                                  ** access denied, system lost, access state
                                  ** timeout, max access probes, reorder,
                                  ** intercept, BS release, MS release or
                                  ** normal access completion */

  SDSS_EVT_HDR_OPR_SES_ABORT,     /* MS does not receive an HDR OpenResponse
                                  ** message within T-SBPANResponse seconds
                                  ** after sending the OpenRequest message */

                                  /* 908 */
  SDSS_EVT_HDR_OPR_SES_CLOSE,     /* HDR session is closed */


  SDSS_EVT_HDR_OPR_BAD_SYS,       /* HDR system is bad in following cases.
                                  ** 1.AP message with invalid max capsule
                                  ** length i.e. < 2
                                  ** 2.SP message with hash channel on
                                  ** unsupported band
                                  ** 3. TCA to unsupported band
                                  ** 4.TCA with no pilots
                                  */

  SDSS_EVT_HDR_OPR_RELEASE,       /* HDR release the traffic channel */

  SDSS_EVT_HDR_OPR_SES_NEGO_TIMEOUT,
                                  /* Session negotiation timeout */

                                  /* 912 */
  SDSS_EVT_HDR_OPR_END_BCMCS,     /* HDR BCMCS flow monitoring end */

  SDSS_EVT_HDR_OPR_PROT_MIS,      /* Protocol mismatch in Idle state */

  SDSS_EVT_HDR_OPR_MAX,           /* FOR RANGE CHECKING */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of GSM/WCDMA Acquisition Indications.
  */

  SDSS_EVT_GW_ACQ                 = 1000, /* FOR RANGE CHECKING */


  SDSS_EVT_GW_ACQ_PLMN_FAILED,    /* Failed to acquire a GSM/WCDMA system.*/

                                  /* 1002 */
  SDSS_EVT_GW_ACQ_PLMN_SUCCESS,   /* Successfully acquired a GSM/WCDMA system
                                  */


  SDSS_EVT_GW_ACQ_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of GSM/WCDMA Operation Indications.
  */

  SDSS_EVT_GW_OPR                 = 1100, /* FOR RANGE CHECKING */


  SDSS_EVT_GW_OPR_SYS_LOST,       /* GSM/WCDMA system lost. */

                                  /* 1102 */
  SDSS_EVT_GW_OPR_SRV_INFO,       /* GSM/WCDMA serving system info changed.
                                  */

  SDSS_EVT_GW_OPR_ACC_DONE,       /* GSM/WCDMA  call is released.
                                  */

  SDSS_EVT_GW_OPR_MAX,          /* FOR RANGE CHECKING */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of WLAN User Indications.
  */
  SDSS_EVT_WLAN_USER               = 1200, /* For range checking */

  SDSS_EVT_WLAN_USER_SS_PREF,      /* user changes the system selection
                                   ** preference and or origination mode
                                   */

  SDSS_EVT_WLAN_USER_PROT_DEACTIVATE,
                                  /* WLAN protocol was deactivated.
                                  */

  SDSS_EVT_WLAN_USER_SURVEY,      /* User requested to survey wlan networks.
                                  */

                                  /* 1204 */
  SDSS_EVT_WLAN_USER_TERM_SURVEY,
                                  /* User terminated the previous survey
                                  ** request.
                                  */

  SDSS_EVT_WLAN_USER_PWR_SAVE_EXIT,/* User commanded to exit power save mode.
                                  */

  SDSS_EVT_WLAN_USER_MAX,         /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of WLAN Acquisition Indications.
  */

  SDSS_EVT_WLAN_ACQ                = 1300, /* FOR RANGE CHECKING */


  SDSS_EVT_WLAN_ACQ_SCAN_FAILED,  /* WLAN acquisition fails due to failure to
                                  ** find any BSS */

  SDSS_EVT_WLAN_ACQ_ATTACH_FAILED,/* WLAN acquisition fails due to failure to
                                  ** attach.
                                  */

  SDSS_EVT_WLAN_ACQ_ATTACH_OK,    /* WLAN CP successfully attached to BSS.
                                  */

  SDSS_EVT_WLAN_ACQ_BAD_SYS,      /* WLAN system is bad */


  SDSS_EVT_WLAN_ACQ_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of WLAN Operation Indications.
  */

  SDSS_EVT_WLAN_OPR                = 1400, /* FOR RANGE CHECKING */

  SDSS_EVT_WLAN_OPR_ATTACH_OK,    /* A new WLAN system is fully acquired.
                                  ** This new system differ in SSID/BSSID/chan
                                  ** from the previous serving system */

  SDSS_EVT_WLAN_OPR_SS_LOST,      /* The acquired WLAN system is lost */

  SDSS_EVT_WLAN_OPR_BSS_DISSOCIATED,
                                  /* BSS dissociated the MS.
                                  */

                                  /* 1404 */
  SDSS_EVT_WLAN_OPR_BAD_SYS,      /* WLAN system is bad */

  SDSS_EVT_WLAN_OPR_SYS_INACTIVE, /* WLAN system is inactive */

  SDSS_EVT_WLAN_OPR_MAX,          /* FOR RANGE CHECKING */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of HYBR_1 User Indications.
  */
  SDSS_EVT_HYBR_1_USER               = 1500, /* For range checking */

  SDSS_EVT_HYBR_1_USER_SS_PREF,      /* HDR CP reports that user changes the
                                  ** system selection preference and or
                                  ** origination mode */

  SDSS_EVT_HYBR_1_USER_HYBR_START,   /* HDR CP reports that user started
                                  ** CDMA+HDR hybrid mode of operation */

  SDSS_EVT_HYBR_1_USER_HYBR_END,     /* HDR CP reports that user ended CDMA+HDR
                                  ** hybrid mode of operation */

                                  /* 704 */
  SDSS_EVT_HYBR_1_USER_PWR_SAVE_EXIT,/* User commanded to exit power save mode.
                                  */

  SDSS_EVT_HYBR_1_USER_MAX,         /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_EVT_MAX
} sdss_evt_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of flags.
*/
typedef enum {

  SDSS_FLAG_NONE,                     /* Internal SD use */

  SDSS_FLAG_TRUE_EVENT_PROCESSED,     /* True event processed flag */

  SDSS_FLAG_SYNC_UP_IN_PROGRESS,     /* True event processed flag */

  SDSS_FLAG_ENG_SCR_TIMER,            /* Is the script timer set */

                                         /* 4 */
  SDSS_FLAG_CONN_MODE_UOOS_AWAKE,     /* Connected mode UOOS awake flag */

  SDSS_FLAG_MAX                       /* Internal SD use */

} sdss_flag_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of SS-Events groups.
*/
typedef enum {

  SDSS_EVT_GRP_USER,              /* User events group */
  SDSS_EVT_GRP_MISC,              /* Miscellaneous events group */
  SDSS_EVT_GRP_INT,               /* Internal events group */
  SDSS_EVT_GRP_CDMA_ACQ,          /* CDMA acquisition events group */
  SDSS_EVT_GRP_CDMA_OPR,          /* CDMA operation events group */
  SDSS_EVT_GRP_AMPS_ACQ,          /* AMPS acquisition events group */
  SDSS_EVT_GRP_AMPS_OPR,          /* AMPS operation events group */
  SDSS_EVT_GRP_HDR_USER,          /* HDR user events group */
  SDSS_EVT_GRP_HDR_ACQ,           /* HDR acquisition events group */
  SDSS_EVT_GRP_HDR_OPR,           /* HDR operation events group */
  SDSS_EVT_GRP_GW_ACQ,            /* GSM/WCDMA acquisition events group */
  SDSS_EVT_GRP_GW_OPR,            /* GSM/WCDMA operation events group */
  SDSS_EVT_GRP_WLAN_USER,         /* WLAN user events group */
  SDSS_EVT_GRP_WLAN_ACQ,          /* WLAN acquisition events group */
  SDSS_EVT_GRP_WLAN_OPR,          /* WLAN operation events group */

  SDSS_EVT_GRP_CALLED,            /* Called scripts */
  SDSS_EVT_GRP_HYBR_1_USER,       /* Hybr 1 evetns */
  SDSS_EVT_GRP_MAX

} sdss_evt_grp_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a user event.
*/
#define SDSS_EVT_IS_USER( event ) \
                        ( ((event) > SDSS_EVT_USER) && \
                          ((event) < SDSS_EVT_USER_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a miscellaneous event.
*/
#define SDSS_EVT_IS_MISC( event ) \
                        ( ((event) > SDSS_EVT_MISC) && \
                          ((event) < SDSS_EVT_MISC_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is an internal event.
*/
#define SDSS_EVT_IS_INT( event ) \
                        ( ((event) > SDSS_EVT_INT) && \
                          ((event) < SDSS_EVT_INT_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to decide whether a given event is CDMA Acquisition event.
*/
#define SDSS_EVT_IS_CDMA_ACQ( event ) \
                        ( ((event) > SDSS_EVT_CDMA_ACQ) && \
                          ((event) < SDSS_EVT_CDMA_ACQ_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is CDMA Operation event.
*/
#define SDSS_EVT_IS_CDMA_OPR( event ) \
                        ( ((event) > SDSS_EVT_CDMA_OPR) && \
                          ((event) < SDSS_EVT_CDMA_OPR_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a CDMA event.
*/
#define SDSS_EVT_IS_CDMA( event ) ( SDSS_EVT_IS_CDMA_ACQ(event) || \
                                    SDSS_EVT_IS_CDMA_OPR(event) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to decide whether a given event is an AMPS Acquisition event.
*/
#define SDSS_EVT_IS_AMPS_ACQ( event ) \
                        ( ((event) > SDSS_EVT_AMPS_ACQ) && \
                          ((event) < SDSS_EVT_AMPS_ACQ_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is an AMPS Operation event.
*/
#define SDSS_EVT_IS_AMPS_OPR( event ) \
                        ( ((event) > SDSS_EVT_AMPS_OPR) && \
                          ((event) < SDSS_EVT_AMPS_OPR_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is an AMPS event.
*/
#define SDSS_EVT_IS_AMPS( event ) ( SDSS_EVT_IS_AMPS_ACQ(event) || \
                                    SDSS_EVT_IS_AMPS_OPR(event) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a HDR user event.
*/
#define SDSS_EVT_IS_HDR_USER( event ) \
                        ( ((event) > SDSS_EVT_HDR_USER) && \
                          ((event) < SDSS_EVT_HDR_USER_MAX) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a HDR Acquisition event.
*/
#define SDSS_EVT_IS_HDR_ACQ( event ) \
                        ( ((event) > SDSS_EVT_HDR_ACQ) && \
                          ((event) < SDSS_EVT_HDR_ACQ_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a HDR Operation event.
*/
#define SDSS_EVT_IS_HDR_OPR( event ) \
                        ( ((event) > SDSS_EVT_HDR_OPR) && \
                          ((event) < SDSS_EVT_HDR_OPR_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a HDR event.
*/
/* Macro to decide whether a given event is a HDR event.
*/
#define SDSS_EVT_IS_HDR( event )  ( SDSS_EVT_IS_HDR_USER(event)|| \
                                    SDSS_EVT_IS_HDR_ACQ(event) || \
                                    SDSS_EVT_IS_HDR_OPR(event) )




/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a GSM/WCDMA Acquisition event.
*/
#define SDSS_EVT_IS_GW_ACQ( event ) \
                        ( ((event) > SDSS_EVT_GW_ACQ) && \
                          ((event) < SDSS_EVT_GW_ACQ_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a GSM/WCDMA Operation event.
*/
#define SDSS_EVT_IS_GW_OPR( event ) \
                        ( ((event) > SDSS_EVT_GW_OPR) && \
                          ((event) < SDSS_EVT_GW_OPR_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a HDR event.
*/
#define SDSS_EVT_IS_GW( event )  (  SDSS_EVT_IS_GW_ACQ(event) || \
                                    SDSS_EVT_IS_GW_OPR(event) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a WLAN user event.
*/
#define SDSS_EVT_IS_WLAN_USER( event ) \
                        ( ((event) > SDSS_EVT_WLAN_USER) && \
                          ((event) < SDSS_EVT_WLAN_USER_MAX) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a WLAN Acquisition event.
*/
#define SDSS_EVT_IS_WLAN_ACQ( event ) \
                        ( ((event) > SDSS_EVT_WLAN_ACQ) && \
                          ((event) < SDSS_EVT_WLAN_ACQ_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a WLAN Operation event.
*/
#define SDSS_EVT_IS_WLAN_OPR( event ) \
                        ( ((event) > SDSS_EVT_WLAN_OPR) && \
                          ((event) < SDSS_EVT_WLAN_OPR_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a WLAN event.
*/
#define SDSS_EVT_IS_WLAN( event ) ( SDSS_EVT_IS_WLAN_USER(event)|| \
                                    SDSS_EVT_IS_WLAN_ACQ(event) || \
                                    SDSS_EVT_IS_WLAN_OPR(event) )



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a user event.
*/
#define SDSS_EVT_IS_HYBR_1_USER( event ) \
                        ( ((event) > SDSS_EVT_HYBR_1_USER) && \
                          ((event) < SDSS_EVT_HYBR_1_USER_MAX) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is an acquisition event.
*/
#define SDSS_EVT_IS_XXX_ACQ( event ) \
                                  ( SDSS_EVT_IS_CDMA_ACQ(event) || \
                                    SDSS_EVT_IS_AMPS_ACQ(event) || \
                                    SDSS_EVT_IS_HDR_ACQ(event)  || \
                                    SDSS_EVT_IS_WLAN_ACQ(event) || \
                                    SDSS_EVT_IS_GW_ACQ(event))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is an operation event.
*/
#define SDSS_EVT_IS_XXX_OPR( event ) \
                                  ( SDSS_EVT_IS_CDMA_OPR(event) || \
                                    SDSS_EVT_IS_AMPS_OPR(event) || \
                                    SDSS_EVT_IS_HDR_OPR(event)  || \
                                    SDSS_EVT_IS_WLAN_OPR(event)  || \
                                    SDSS_EVT_IS_GW_OPR(event))




/* <EJECT> */
/*---------------------------------------------------------------------------
                          Preference change enum.
---------------------------------------------------------------------------*/


/* Enumeration of preference updated type.
*/
typedef enum {

  SDSS_PREF_UPDATED_NONE,            /* No preferences changed      */
  SDSS_PREF_UPDATED_PREF_REAS,       /* Only pref. reason changed   */
  SDSS_PREF_UPDATED_SRV_DOMAIN,      /* Only Service domain changed */
  SDSS_PREF_UPDATED_TRUE_PREF,       /* Actual preference changed   */
  SDSS_PREF_UPDATED_EMERG_ORIG_END,  /* Emergency call ended        */

  SDSS_PREF_UPDATE_MAX
} sdss_pref_updated_e_type;



/* <EJECT> */
/*---------------------------------------------------------------------------
                          System Selection States
---------------------------------------------------------------------------*/


/* Enumeration of System Selection States (SS-States).
*/
typedef enum {

  SDSS_STATE_ACQ,        /* Acquisition state */
  SDSS_STATE_VRFY,       /* Verify state */
  SDSS_STATE_OPR,        /* Operation state */
  SDSS_STATE_PWR_SAVE,   /* Protocol is in power save */

  SDSS_STATE_MAX
} sdss_state_e_type;

typedef struct {

  byte                        assn_tag;    
                              /* Association tag of the System */

  int                         geo_offset;  
                              /* The Offset of the systems Geo */

  int                         geo_count;
                              /* The No. Of Geo's with matching system */

} sdss_sys_addn_info_s_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                             Acquisition Modes
---------------------------------------------------------------------------*/

/* Enumeration of Acquisition Modes.
*/
typedef enum {
  #ifdef SD_USES_DB_SERVICES
  SDSS_ACQ_MODE_FULL             = DB_ACQ_FULL,
                                   /* Full acquisition mode */

  SDSS_ACQ_MODE_MINI             = DB_ACQ_MINI,
                                   /* Mini acquisition mode */

  SDSS_ACQ_MODE_MICRO            = DB_ACQ_MICRO,
                                   /* Micro acquisition mode */
  #else
  SDSS_ACQ_MODE_FULL,
                                   /* Full acquisition mode */

  SDSS_ACQ_MODE_MINI,
                                   /* Mini acquisition mode */

  SDSS_ACQ_MODE_MICRO,
                                   /* Micro acquisition mode */
  #endif


  SDSS_ACQ_MODE_FULL2,             /* Full2 acquisition mode */
  SDSS_ACQ_MODE_MINI2,             /* Mini2 acquisition mode */
  SDSS_ACQ_MODE_MICRO2,            /* Micro2 acquisition mode */

  SDSS_ACQ_MODE_DEEP,              /* Deep HDR acquisition mode */
  SDSS_ACQ_MODE_DEEP2,             /* Deep2 HDR acquisition mode */

  SDSS_ACQ_MODE_SHALLOW,           /* Shallow HDR acquisition mode */
  SDSS_ACQ_MODE_SHALLOW2,          /* Shallow2 HDR acquisition mode */

  SDSS_ACQ_MODE_PREV,              /* Previous acquisition mode */
  SDSS_ACQ_MODE_UNIFORM_FULL,      /* Uniform FULL scan            */
  SDSS_ACQ_MODE_UNIFORM_FULL2,     /* Uniform FULL2 scan           */

  SDSS_ACQ_MODE_UNIFORM_FULL_RTN,  /* Uniform Full scan, return to the script */
  SDSS_ACQ_MODE_UNIFORM_FULL_RTN2, /* Uniform Full2 scan, return to the script */

  SDSS_ACQ_MODE_HO,                /* Acquisition for handoff */
  SDSS_ACQ_MODE_MAX
} sdss_acq_mode_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                             Reselection States
---------------------------------------------------------------------------*/

/* Enumeration of Reselection States.
*/
typedef enum {

  SDSS_RESEL_STT_NONE,        /* Not doing reselection */

  SDSS_RESEL_STT_WAIT,        /* Waiting for next reselection period */
  SDSS_RESEL_STT_SRCH,        /* Searching for more preferred systems/
                              ** full srv */
  SDSS_RESEL_STT_REACQ,       /* Reacquiring the serving system */

  SDSS_RESEL_STT_SS_PREF,     /* Reselection after SS-Pref change */
  SDSS_RESEL_STT_RELEASE,     /* Reselection after call is released */
  SDSS_RESEL_STT_SETUP_SRCH,  /* Setup state before SRCH  */

  SDSS_RESEL_STT_MAX
} sdss_resel_stt_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                             Reselection Modes
---------------------------------------------------------------------------*/

/* Enumeration of Reselection Modes.
*/
typedef enum {

  SDSS_RESEL_MODE_INIT,       /* Initial reselection */
  SDSS_RESEL_MODE_NORM,       /* Normal reselection */
  #if defined( FEATURE_SD_ALT_CDMA_BSR_SCAN_1 )
#error code not present
  #endif
  SDSS_RESEL_MODE_FULL_SRV =  50,
                              /* Full service reselection, magic value
                              ** to indicate we are in any of the stages
                              ** in full srv reselection.
                              */
  SDSS_RESEL_MODE_FULL_SRV_1, /* 1st stage full service reselection */
  SDSS_RESEL_MODE_FULL_SRV_2, /* 2nd stage full service reselection */
  SDSS_RESEL_MODE_FULL_SRV_3, /* 3rd stage full service reselection */
  SDSS_RESEL_MODE_FULL_SRV_4, /* 4th stage full service reselection */
  SDSS_RESEL_MODE_FULL_SRV_5, /* 5th stage full service reselection */
  SDSS_RESEL_MODE_FULL_SRV_MAX,

  SDSS_RESEL_MODE_MAX
} sdss_resel_mode_e_type;



/* <EJECT> */
/*---------------------------------------------------------------------------
                          System Selection Action
---------------------------------------------------------------------------*/

/* Types for specifying the SS-Actions to be returned to the SS-Front-end
** when the SS-Front-end calls on the SS-Core to process an SS-Event.
*/


/* Enumeration of SS-Internal-Actions.
*/
typedef enum {

  SDSS_IACT_CONTINUE,   /* Continue with current mode of operation and
                        ** follow the actions that are specified by the
                        ** "continue" parameters */

  SDSS_IACT_ACQUIRE,    /* Acquire a system as specified by the "acquisition"
                        ** parameters */

  SDSS_IACT_MEAS,       /* Request an channel strength measurement from
                        ** Search, as specified by the "measurement request"
                        ** parameters. When Search comes back with the
                        ** measurement report, call on the SS-Core to process
                        ** the SDSS_EVT_MISC_MEAS_RPRT SS-Event */

  SDSS_IACT_PWR_SAVE,   /* Enter Power-Save mode of operation. Once actually
                        ** entered the Power-Save mode, the SS-Front-end
                        ** should call on the SS-Core to process the
                        ** SDSS_EVT_MISC_PWR_SAVE_ENTER SS-Event */

  SDSS_IACT_ACCEPT,     /* Accept BS proposed mode, band and/or CDMA channel/
                        ** AMPS system. This action is only valid for
                        ** Redirection, Channel Assignment, Handoff Direction
                        ** or ITSPM related SS-Event */

  SDSS_IACT_REJECT,     /* Reject BS proposed mode, band and/or CDMA channel/
                        ** AMPS system. This action is only valid for
                        ** Redirection, Channel Assignment, Handoff Direction
                        ** or ITSPM related SS-Events */

  SDSS_IACT_RELEASE,    /* Release call. This action is only valid for
                        ** ITSPM SS-Indication */

  SDSS_IACT_GET_NET_GW, /* Get the available GSM/WCDMA networks.
                        */

  SDSS_IACT_GET_NET_WLAN,/* Get the available WLAN networks.
                        */

  SDSS_IACT_PWR_SCAN_GW, /* Perform powr scan on GSM/WCDMA.
                        */

  SDSS_IACT_MAX
} sdss_iact_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for parameters that are associated with the SDSS_IACT_ACQUIRE
** internal action value.
*/
typedef struct {

  /* The system record list and the list position to be attempted
  ** acquisition.
  */
  sdsr_e_type               list;
  int                       pos;

  /* Mode of acquisition to be attempted.
  */
  sdss_acq_mode_e_type      mode;

  /* AMPS 2nd strongest CCH - indicate whether AMPS 2nd strongest control
  ** channel (as apposed to strongest control channel) is to be attempted
  ** acquisition.
  */
  boolean                   is_amps_2nd_cch;

  /* Maximum redirect delay. When the BS redirects the MS from a CDMA system
  ** to an AMPS system (using GSRDM), this field indicate the maximum
  ** redirect delay value to be used in the AMPS registration randomization
  ** algorithm.
  */
  word                      max_redirect_delay;

  /* Is this acquisition due to handoff
  */
  boolean                   is_handoff_acq;

} sdss_iact_acq_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Types for parameters that are associated with the SDSS_IACT_MEAS
** internal action value.
*/
typedef enum {

  SDSS_MEAS_MODE_DED,              /* Dedicated measurement mode */
  SDSS_MEAS_MODE_BACK,             /* Background measurement mode */

  SDSS_MEAS_MODE_MAX
} sdss_meas_mode_e_type;


typedef struct {

  sd_ss_meas_type_e_type    type;     /* Type of measurement */
  sdss_meas_mode_e_type     mode;     /* Mode of measurement */
  sdsr_e_type               list;     /* The system record list for which
                                      ** to perform the measurements */
} sdss_iact_meas_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for parameters that are associated with SDSS_IACT_PWR_SCAN_GW
** internal action value.
*/
typedef struct {

  sd_ss_band_pref_e_type       band_pref;       /* band preference */
  sd_ss_mode_pref_e_type       mode_pref;       /* mode preference */
  sd_ss_acq_order_pref_e_type  acq_order_pref;  /* acq order preference */

} sdss_iact_pwr_scan_gw_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for parameters that are associated with SDSS_IACT_GET_NET_GW
** internal action value.
*/
typedef struct {

  sd_ss_band_pref_e_type       band_pref;       /* band preference */
  sd_ss_mode_pref_e_type       mode_pref;       /* mode preference */
  sd_ss_acq_order_pref_e_type  acq_order_pref;  /* acq order preference */
  sdsr_ref_type                sdsr_ref;        /* system record reference */

} sdss_iact_get_net_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for holding an SS-Internal-Action and its associated parameters.
*/
typedef struct {

  /* Internal action.
  */
  sdss_iact_e_type      act;

  /* Internal action's specific parameters
  */
  union {
    sdss_iact_acq_s_type           acq;
    sdss_iact_meas_s_type          meas;
    sdss_iact_pwr_scan_gw_s_type   pwr_scan;
    sdss_iact_get_net_s_type       get_net;
  }                     prm;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Common parameters.
  */

  /* SS-State indicator - indicates the SS-State (i.e., acquisition,
  ** verification, or operation).
  */
  sdss_state_e_type     ss_state;

  /* SS-Timer - indicates the uptime in seconds where the SS-Timer should be
  ** set to expire, at which time the SS-Front-end is responsible for calling
  ** on the SS-Core to process the SDSS_EVT_MISC_TIMER_XXX SS-Event.
  */
  dword                 ss_timer;

} sdss_iact_s_type;




/* <EJECT> */
/*---------------------------------------------------------------------------
                            Acquisition States
---------------------------------------------------------------------------*/


/* Enumeration of Acquisition States.
*/
typedef enum {

  SDSS_ACQ_STT_NONE,       /* While in this state the acquisition process is
                           ** not doing any acquisitions.
                           */


  SDSS_ACQ_STT_START,     /* Upon starting a new acquisition process, the
                          ** acquisition state machine sets the state to this
                          ** one.
                          **
                          ** While in this state the acquisition state
                          ** machine tries to acquire a system in accordance
                          ** with the acquisition parameters that are
                          ** specified in the ACQ script construct.
                          **
                          ** If a system is acquired while in this state, The
                          ** the state is changed as follows:
                          **
                          ** - If the acquisition parameters indicate that a
                          **   more preferred system is not required, the
                          **   state is changed to SDSS_ACQ_STT_DONE.
                          **
                          ** - Else, if the acquired system is a preferred
                          **   PRL system and a more preferred system does
                          **   NOT exit within the GEO of the acquired
                          **   system, the state is changed to
                          **   SDSS_ACQ_STT_DONE.
                          **
                          ** - Else, If the acquired system is from the PRL
                          **   and a more preferred system exits within the
                          **   GEO of the acquired system, the state is
                          **   changed to SDSS_ACQ_STT_MORE_PREF.
                          **
                          ** - Else, the next system per the acquisition
                          **   parameters is attempted acquisition */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_MORE_PREF, /* While in this state the acquisition state
                          ** machine tries to acquire more preferred system
                          ** (most to least) within the GEO of the last
                          ** system that was acquired in the
                          ** SDSS_ACQ_STT_START state.
                          **
                          ** If a more preferred system is acquired, the
                          ** state is changed to SDSS_ACQ_STT_DONE.
                          **
                          ** If a more preferred system is not acquired and
                          ** the last system that was acquired in the
                          ** SDSS_ACQ_STT_START state is a preferred PRL
                          ** system, the state is changed to
                          ** SDSS_ACQ_STT_REACQ.
                          **
                          ** If a more preferred system is not acquired and
                          ** the last system that was acquired in the
                          ** SDSS_ACQ_STT_START state is a negative PRL
                          ** system, the state is changed to
                          ** SDSS_ACQ_STT_START */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_REACQ,     /* While in this state the acquisition state
                          ** machine tries to reacquire the system that was
                          ** last acquired in the SDSS_ACQ_STT_START state.
                          **
                          ** If the system is successfully reacquired, the
                          ** state is changed to SDSS_ACQ_STT_DONE.
                          **
                          ** If the system is not successfully reacquired,
                          ** the state is changed to SDSS_ACQ_STT_START */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_DONE,       /* While in this state the acquisition process is
                          ** done and no more SS-Event are normally expected
                          ** to be processed by the acquisition state machine
                          ** (i.e., a reselection script is expected to take
                          ** over control upon the next SS-Event).
                          **
                          ** If, however an SS-Event is received by the
                          ** acquisition state machine while in this state,
                          ** the state is changed to SDSS_ACQ_STT_START */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_PWR_SAVE_ENTER, /* While in this state, the ptorocol is
                               ** shutting down due to Uniform OOS.
                               ** On entering powersave, the acq state
                               ** will change to
                               ** SDSS_ACQ_STT_PWR_SAVE
                               */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_PWR_SAVE, /* While in this state, the protocol is in
                         ** power save state, waiting for the next wake-up
                         ** to complete the acquisition
                         */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_SCAN_CONT, /* Continue with the scan, applicable only for GCF
                          ** test mode
                          */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  SDSS_ACQ_STT_MAX
} sdss_acq_stt_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                           PRL System Designation
---------------------------------------------------------------------------*/

/* Enumeration of PRL designations.
*/
typedef enum {

  SDSS_PRL_NEG,           /* Negative system */

  SDSS_PRL_UNAVAIL,       /* Unavailable system since PRL forbids available
                          ** systems */

  SDSS_PRL_AVAIL,         /* Available system */

  SDSS_PRL_PREF,          /* Preferred, but not the most preferred system in
                          ** its GEO */

  SDSS_PRL_MOST_PREF,     /* Most preferred system in its GEO */

  SDSS_PRL_MAX
} sdss_prl_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of PRL designation combinations.
*/
typedef enum {

  SDSS_PRL_VAL_NEG                  = BM( SDSS_PRL_NEG ),
                                      /* Negative system */

  SDSS_PRL_VAL_UNAVAIL              = BM( SDSS_PRL_UNAVAIL ),
                                      /* Unavailable system since PRL forbids
                                      ** available systems */

  SDSS_PRL_VAL_FORBID               = BM( SDSS_PRL_NEG ) | \
                                      BM( SDSS_PRL_UNAVAIL ),
                                      /* Any forbidden system - negative or
                                      ** unavailable */

  SDSS_PRL_VAL_AVAIL                = BM( SDSS_PRL_AVAIL ),
                                      /* Available system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_PRL_VAL_PREF                 = BM( SDSS_PRL_PREF ),
                                      /* Preferred system, but not the most
                                      ** in its GEO */

  SDSS_PRL_VAL_NOT_MOST_PREF        = BM( SDSS_PRL_NEG )     | \
                                      BM( SDSS_PRL_UNAVAIL ) | \
                                      BM( SDSS_PRL_AVAIL )   | \
                                      BM( SDSS_PRL_PREF ),
                                      /* Negative, unavailable, available or
                                      ** preferred (not most) system  */

  SDSS_PRL_VAL_MOST_PREF            = BM( SDSS_PRL_MOST_PREF ),
                                      /* Most preferred in its GEO system */

  SDSS_PRL_VAL_ANY_PREF             = BM( SDSS_PRL_PREF ) | \
                                      BM( SDSS_PRL_MOST_PREF ),
                                      /* Any preferred system - most or not
                                      ** most in its GEO */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_PRL_VAL_LISTED               = BM( SDSS_PRL_NEG )  | \
                                      BM( SDSS_PRL_PREF ) | \
                                      BM( SDSS_PRL_MOST_PREF ),
                                      /* Any PRL listed system - negative
                                      ** or preferred */

  SDSS_PRL_VAL_ALLOWED              = BM( SDSS_PRL_AVAIL ) | \
                                      BM( SDSS_PRL_PREF )  | \
                                      BM( SDSS_PRL_MOST_PREF ),
                                      /* Any allowed system - available,
                                      ** preferred or most preferred */

  SDSS_PRL_VAL_MAX
} sdss_prl_val_e_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
                         Redirection Designation
---------------------------------------------------------------------------*/

/* Enumeration of Redirection type combinations.
*/
typedef enum {

  SDSS_REDIR_VAL_NONE               = BM( SD_SS_REDIR_NONE ),
                                      /* Note in redirection */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* CDMA redirections.
  */

  SDSS_REDIR_VAL_CDMA_SRDM          = BM( SD_SS_REDIR_CDMA_SRDM ),
                                      /* SRDM redirection */

  SDSS_REDIR_VAL_CDMA_GSRDM         = BM( SD_SS_REDIR_CDMA_GSRDM ),
                                      /* GSRDM redirection */

  SDSS_REDIR_VAL_CDMA_NDSS          = BM( SD_SS_REDIR_CDMA_NDSS ),
                                      /* NDSS redirection */

  SDSS_REDIR_VAL_CDMA_GSRDM_NDSS    = BM( SD_SS_REDIR_CDMA_GSRDM ) | \
                                      BM( SD_SS_REDIR_CDMA_NDSS ),
                                      /* GSRDM or NDSS redirection */

  SDSS_REDIR_VAL_CDMA_ANY           = BM( SD_SS_REDIR_CDMA_SRDM )  | \
                                      BM( SD_SS_REDIR_CDMA_GSRDM ) | \
                                      BM( SD_SS_REDIR_CDMA_NDSS ),
                                      /* Any CDMA redirection */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* HDR redirections.
  */
  SDSS_REDIR_VAL_HDR_QC             = BM( SD_SS_REDIR_HDR_QC ),
                                      /* HDR QC redirection */

  SDSS_REDIR_VAL_HDR_DIRECTED       = BM( SD_SS_REDIR_HDR_DIRECTED ),
                                      /* HDR directed redirection */

  SDSS_REDIR_VAL_HDR_ANY            = BM( SD_SS_REDIR_HDR_QC ) | \
                                      BM( SD_SS_REDIR_HDR_DIRECTED ),
                                      /* Any HDR redirection */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_REDIR_VAL_ANY                = BM( SD_SS_REDIR_CDMA_SRDM )  | \
                                      BM( SD_SS_REDIR_CDMA_GSRDM ) | \
                                      BM( SD_SS_REDIR_CDMA_NDSS )  | \
                                      BM( SD_SS_REDIR_HDR_QC )     | \
                                      BM( SD_SS_REDIR_HDR_DIRECTED ),
                                      /* Any redirection */
  SDSS_REDIR_VAL_MAX
} sdss_redir_val_e_type;



/* <EJECT> */
/*---------------------------------------------------------------------------
                           Handoff Designation
---------------------------------------------------------------------------*/

/* Enumeration of Handoff type combinations.
*/
typedef enum {

  SDSS_HO_VAL_NONE                  = BM(SD_SS_HO_NONE),
                                      /* Not in handoff */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* CDMA to CDMA.
  */
  SDSS_HO_VAL_CDMA_CAM_TO_CDMA_PAG  = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG),
                                      /* CAM to CDMA paging channel */

  SDSS_HO_VAL_CDMA_CAM_TO_CDMA_TRF  = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF),
                                      /* CAM to CDMA traffic channel */

  SDSS_HO_VAL_CDMA_CAM_TO_CDMA_ANY  = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF),
                                      /* Any CAM to CDMA */

  SDSS_HO_VAL_CDMA_HDM_TO_CDMA_TRF  = BM(SD_SS_HO_CDMA_HDM_TO_CDMA_TRF),
                                      /* HDM to CDMA traffic channel */

  SDSS_HO_VAL_CDMA_ANY_TO_CDMA_ANY  = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_CDMA_TRF),
                                      /* Any CDMA to CDMA handoff */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* CDMA to AMPS.
  */
  SDSS_HO_VAL_CDMA_CAM_TO_AMPS_SYS  = BM(SD_SS_HO_CDMA_CAM_TO_AMPS_SYS),
                                      /* CAM to AMPS system (A/B) */

  SDSS_HO_VAL_CDMA_CAM_TO_AMPS_VOICE= BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE),
                                      /* CAM to AMPS voice channel */

  SDSS_HO_VAL_CDMA_CAM_TO_AMPS_ANY  = BM(SD_SS_HO_CDMA_CAM_TO_AMPS_SYS) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE ),
                                      /* Any CAM to AMPS */

  SDSS_HO_VAL_CDMA_HDM_TO_AMPS_VOICE= BM(SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE),
                                      /* HDM to AMPS voice channel */

  SDSS_HO_VAL_CDMA_ANY_TO_AMPS_VOICE= BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE),
                                      /* Any CDMA handoff to AMPS voice */

  SDSS_HO_VAL_CDMA_ANY_TO_AMPS_ANY  = BM(SD_SS_HO_CDMA_CAM_TO_AMPS_SYS) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE),
                                      /* Any CDMA to AMPS handoff */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Combinations
  */
  SDSS_HO_VAL_CDMA_CAM_TO_ANY       = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_SYS) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE),
                                      /* Any CDMA CAM */

  SDSS_HO_VAL_CDMA_HDM_TO_ANY       = BM(SD_SS_HO_CDMA_HDM_TO_CDMA_TRF) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE),
                                      /* Any CDMA HDM */

  SDSS_HO_VAL_CDMA_ANY_TO_ANY       = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_CDMA_TRF) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_SYS) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE),
                                      /* Any CDMA handoff */

  SDSS_HO_VAL_HDR_ANY_TO_CDMA_ANY  = BM(SD_SS_HO_HDR_TO_CDMA),
                                      /* Any HDR to CDMA handoff */
  SDSS_HO_VAL_MAX
} sdss_ho_val_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                       SS-Preference System Designation
---------------------------------------------------------------------------*/

/* Enumeration of SS-Preference conflict combinations.
*/
typedef enum {

  SDSS_PREF_CONF_MODE            = BM(0),
                                   /* Conflicts with mode preference */

  SDSS_PREF_CONF_BAND            = BM(1),
                                   /* Conflicts with band preference */

  SDSS_PREF_CONF_ROAM            = BM(2),
                                   /* Conflicts with roam preference */

  SDSS_PREF_CONF_PRL             = BM(3),
                                   /* Conflicts with prl preference */

  SDSS_PREF_CONF_BSS_TYPE        = BM(4),
                                   /* Conflicts with BSS type preference */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_PREF_CONF_MODE_OR_BAND    = SDSS_PREF_CONF_MODE | \
                                   SDSS_PREF_CONF_BAND,
                                   /* Conflicts with mode or band
                                   ** preference */

  SDSS_PREF_CONF_ANY             = SDSS_PREF_CONF_MODE | \
                                   SDSS_PREF_CONF_BAND | \
                                   SDSS_PREF_CONF_ROAM | \
                                   SDSS_PREF_CONF_BSS_TYPE,
                                   /* Conflicts with mode, band, roam or
                                   ** bss type preference */

  SDSS_PREF_CONF_NONE            = SDSS_PREF_CONF_MODE | \
                                   SDSS_PREF_CONF_BAND | \
                                   SDSS_PREF_CONF_ROAM | \
                                   SDSS_PREF_CONF_BSS_TYPE |
                                   BM(5),
                                   /* Not conflicting with mode, band, roam
                                   ** and bss type preferences */
  SDSS_PREF_CONF_MAX
} sdss_pref_conf_e_type;



/* Enumeration of SS-Preference changed combinations.
*/
typedef enum {

  SDSS_PREF_CHGD_MODE            = BM(0),
                                   /* Mode preference changed */

  SDSS_PREF_CHGD_BAND            = BM(1),
                                   /* Band preference changed */

  SDSS_PREF_CHGD_ROAM            = BM(2),
                                   /* Roam preference changed */

  SDSS_PREF_CHGD_HYBR            = BM(3),
                                   /* Hybrid preference changed */

  SDSS_PREF_CHGD_DOMAIN          = BM(4),
                                   /* Domain preference changed */

  SDSS_PREF_CHGD_ACQ_ORDER       = BM(5),
                                   /* Acq. order preference changed */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_PREF_CHGD_DOMAIN_ACQ_ORDER = SDSS_PREF_CHGD_DOMAIN | \
                                    SDSS_PREF_CHGD_ACQ_ORDER,
                                    /* Either domain or acq_order changed.
                                    */

  SDSS_PREF_CHGD_ANY             = SDSS_PREF_CHGD_MODE | \
                                   SDSS_PREF_CHGD_BAND | \
                                   SDSS_PREF_CHGD_ROAM | \
                                   SDSS_PREF_CHGD_HYBR | \
                                   SDSS_PREF_CHGD_DOMAIN | \
                                   SDSS_PREF_CHGD_ACQ_ORDER,
                                   /* Any of mode, band, roam, hybr,
                                   ** srv_domain  or acq_order preference
                                   ** changed */


  SDSS_PREF_CHGD_NONE            = SDSS_PREF_CHGD_MODE | \
                                   SDSS_PREF_CHGD_BAND | \
                                   SDSS_PREF_CHGD_ROAM | \
                                   SDSS_PREF_CHGD_HYBR | \
                                   SDSS_PREF_CHGD_DOMAIN | \
                                   SDSS_PREF_CHGD_ACQ_ORDER |
                                   BM(6),
                                   /* None of mode, band, roam, hybr,
                                   ** srv_domain  or acq_order preference
                                   ** changed */

  SDSS_PREF_CHGD_MAX
} sdss_pref_chgd_e_type;



/* <EJECT> */
/*---------------------------------------------------------------------------
                               PREF OPERATION MACROS
---------------------------------------------------------------------------*/

/* Macros for preference conflict operations.
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Return TRUE if pref_conf1 contains pref_conf2 otherwise return FALSE.
*/
#define SDSS_PREF_CONTAIN( pref_conf1, pref_conf2 ) \
        ( BOOLEAN( (int)pref_conf1 & (int)pref_conf2 ) )

/* <EJECT> */
/*---------------------------------------------------------------------------
                          Operating Mode Parameters
---------------------------------------------------------------------------*/

/* Enumeration of Operating Modes.
*/
typedef enum {

  SDSS_OPR_MODE_PWR_UP,         /* Powering up */

  SDSS_OPR_MODE_ONLINE,         /* Online (CDMA/AMPS/HDR) */
  SDSS_OPR_MODE_OFFLINE_CDMA,   /* Offline CDMA */
  SDSS_OPR_MODE_OFFLINE_AMPS,   /* Offline AMPS */

  SDSS_OPR_MODE_PWR_DOWN,       /* Powering down */

  SDSS_OPR_MODE_MAX
} sdss_opr_mode_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                          SD SS preferences
---------------------------------------------------------------------------*/

/* Type that contains all the preferences in SD SS.
*/
typedef struct {

  /* Mode, band, roam and hybrid preferences - represent the current system
  ** selection preferences.
  */
  sd_ss_mode_pref_e_type      mode_pref;
  sd_ss_mode_pref_e_type      true_mode_pref;
  sd_ss_mode_pref_e_type      get_net_mode_pref;
  sd_ss_band_pref_e_type      band_pref;
  sd_ss_band_pref_e_type      get_net_band_pref;
  sd_ss_prl_pref_e_type       prl_pref;
  sd_ss_roam_pref_e_type      roam_pref;
  sd_ss_hybr_pref_e_type      hybr_pref;
  sd_ss_tech_pref_e_type      tech_pref;
  sys_wlan_scan_pref_e_type   wlan_scan_pref;
  sys_wlan_bss_e_type         bss_type_pref;


  /* Other preferences related to GSM/WCDMA.
  */
  sd_ss_srv_domain_pref_e_type domain_pref;
  sd_ss_acq_order_pref_e_type  acq_order_pref;
  sd_ss_srv_req_type_e_type    srv_req_type;

  /* The most recent reason for updating the SS-Preference.
  */
  sd_ss_pref_reas_e_type      pref_reason;
  sd_ss_orig_mode_e_type      orig_mode;

} sdss_pref_s_type;




/* <EJECT> */
/*---------------------------------------------------------------------------
                          Subscription Type
---------------------------------------------------------------------------*/

/* Enumeration of Subscription type
*/
typedef enum {

  SDSS_SUBSC_NONE               =0,    /* None subscription */

  SDSS_SUBSC_1X                 =BM(0),    /* 1X subscription */

  SDSS_SUBSC_GW                 =BM(1),    /* GW subscription */

  SDSS_SUBSC_1X_GW              =BM(SDSS_SUBSC_1X) | \
                                 BM(SDSS_SUBSC_GW),
                                           /* Both 1x and GW subscriptions */

  SDSS_SUBSC_MAX
} sdss_subsc_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                          Event counter types
---------------------------------------------------------------------------*/

/* Enumeration of Events that are counted
*/
typedef enum {

  SDSS_CNTED_EVT_NONE = -1,         /* For internal use only. */

  SDSS_CNTED_EVT_SES_NEGO_TIMEOUT,  /* Session negotiation timeout. */

  SDSS_CNTED_EVT_ACC_TIMEOUT,       /* Access timeout. */

  SDSS_CNTED_EVT_CON_DENY,          /* Connection deny due to general. */

  SDSS_CNTED_EVT_NET_BUSY,          /* Connection deny due to network busy.*/

  SDSS_CNTED_EVT_MAX

} sdss_cnted_evt_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                         Session open parameters
---------------------------------------------------------------------------*/

/* Enumeration of session open termination reasons (i.e., reason for leaving the
** session open state).
*/
typedef enum {

  SDSS_EVT_CNT_UPDT_REAS_SES_NEGO_TIMEOUT,  /* When session negotiation
                                            ** times out. */

  SDSS_EVT_CNT_UPDT_REAS_SES_OPENED,        /* When session is opened
                                            ** successfully */

  SDSS_EVT_CNT_UPDT_REAS_SID_CHANGED,       /* When sid changes. */

  SDSS_EVT_CNT_UPDT_REAS_ACC_TIMEOUT,       /* Access time out */

  SDSS_EVT_CNT_UPDT_REAS_HDR_ENTER_TRAFFIC, /* HDR enter traffic */

  SDSS_EVT_CNT_UPDT_REAS_CON_DENY,          /* Connection deny due to general
                                            */

  SDSS_EVT_CNT_UPDT_REAS_NET_BUSY,          /* Connection deny due to network
                                            ** busy
                                            */

  SDSS_EVT_CNT_UPDT_REAS_MAX

} sdss_evt_cnt_updt_reas_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------------
                         Conditional LOOP type
---------------------------------------------------------------------------*/

/* Enumeration of loop conditionals
*/
typedef enum {

  SDSS_LOOP_COND_NONE = -1,        /* For Internal SD use                 */

  SDSS_LOOP_COND_GW_SCAN_ABORTED,  /* Loop till GW scan is complete       */

  SDSS_LOOP_COND_IS_HDR_ACTIVITY,  /* Loop till HDR activity ends         */

  SDSS_LOOP_COND_MAX               /* For Internal SD USE                 */

} sdss_loop_cond_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------------
                        Conditions for IF construct
---------------------------------------------------------------------------*/
/* Enumeration of if conditions
*/
typedef enum {

  SDSS_IF_COND_NONE = -1,        /* For Internal SD use                   */

  SDSS_IF_COND_GCF_TESTING,      /* GCF testing                           */

  SDSS_IF_COND_TOP_SYS_MODE_GW,  /* Is the top system GW                  */

  SDSS_IF_COND_TRUE_EVENT,       /* Is true event, payload specifies evt  */

  SDSS_IF_COND_CURR_EVENT,       /* Is curr event, payload specifies evt  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                 /* 4 */
  SDSS_IF_COND_USER_1X_GW_MODE_PREF_AND_CURR_NOT_1X_GW,
                                 /* User has set the mode to 1X + GW, but
                                 ** the current mode is not 1X and GW     */

  SDSS_IF_COND_USER_HDR_GW_MODE_PREF_AND_CURR_NOT_HDR_GW,
                                 /* User has set the mode to HDR + GW, but
                                 ** the current mode is not HDR and GW    */

  SDSS_IF_COND_USER_1X_HDR_GW_MODE_PREF_AND_CURR_NOT_1X_HDR_GW,
                                 /* User has set the mode to 1X or HDR and GW,
                                 ** but the curr mode is not 1X/HDR and GW*/

  SDSS_IF_COND_ACQ_SYS_PRL_DESIG,/* Is the acquired sys of the PRL desig  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                 /* 8 */
  SDSS_IF_COND_NEW_TRUE_EVENT,   /* Is new true event, pld specifies evt  */

  SDSS_IF_COND_HYBR_PREF,        /* Is hybrid preference                  */

  SDSS_IF_COND_SS_STATE,         /* check the system selection state      */

  SDSS_IF_COND_ORIG_MODE,        /* Is origination mode                   */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                 /* 12 */
  SDSS_IF_COND_SS_PREF_CHGD,     /* Is SS pref changed                    */

  SDSS_IF_COND_SRV_PREF_LIST_SIZE, /* pref list size                      */

  SDSS_IF_COND_STACK_SYNC_UP,    /* Does the stacks need to be sync'ed    */

  SDSS_IF_COND_ENG_SCR_TIMER_FLAG, /* Is the script timer set             */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                 /* 16 */

  SDSS_IF_COND_TMR_CONN_MODE_UOOS_SCAN_SET,
                                 /* Is UOOS scan timer set                */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_IF_COND_IS_HDR_REDIR,     /* Is the system under HDR REDIR */

  SDSS_IF_COND_WLAN_SCAN_FAIL_REAS, /* Is WLAN firmware present */

  SDSS_IF_COND_TOP_SYS_MODE_GW_SUBS_AVAIL,
                             /* Is top system GW and Gw subscription is available */

  SDSS_IF_COND_GW_SUBS_AVAIL,
                            /* Is GW subscription available */

                           /* 20 */
  SDSS_IF_COND_GW_PWR_SAVE_ACQ_STAGE,
                           /* GW PWR SAVE ACQ STAGE value */

  SDSS_IF_COND_MAX                /* For Internal SD USE                   */

} sdss_if_cond_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*---------------------------------------------------------------------------
                          Comparision criteria
---------------------------------------------------------------------------*/

typedef enum sdss_cmp_criteria_e {

  SDSS_CMP_CRITERIA_NONE = -1,

  SDSS_CMP_CRITERIA_EQUAL_TO,

  SDSS_CMP_CRITERIA_NOT_EQUAL_TO,

  SDSS_CMP_CRITERIA_MAX

} sdss_cmp_criteria_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Subscription operation macro
*/
#define SDSS_SUBSC_ADD( subsc1, subsc2 ) \
        ( (sdss_subsc_e_type)( (int)subsc1 | (int)subsc2 ) )

#define SDSS_SUBSC_CONTAIN( subsc1, subsc2 ) \
        ( (sdss_subsc_e_type)( (int)subsc1 & (int)subsc2 ) )


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================ GENERAL ====================================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_map_event_to_group

DESCRIPTION
  Map SS-Event to SS-Event Group.

DEPENDENCIES
  None.

RETURN VALUE
  SS-Event group that is associated with the input SS-Event.
  SDSS_EVT_GRP_MAX if event is not associated with any event group.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_evt_grp_e_type           sdss_map_event_to_group(

        sdss_evt_e_type               event
            /* SS-Event for which to get event group.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_opr_mode

DESCRIPTION
  Returns the current operating mode for the specified ss.

DEPENDENCIES
  None.

RETURN VALUE
  sdss_opr_mode_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_opr_mode_e_type         sdss_get_opr_mode(

        sd_ss_e_type                  ss
            /* System selection - MAIN or HDR.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_cdma_locked

DESCRIPTION
  Return the value of the CDMA locked indicator.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the CDMA locked indicator is set to TRUE. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_cdma_locked( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_orig_mode0

DESCRIPTION
  Check whether a specified origination mode value includes the specified
  origination mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the specified
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.


===========================================================================*/
extern  boolean                    sdss_is_orig_mode0(

        sd_ss_orig_mode_e_type     orig_mode,
        /* Specified Origination mode enumeration type.
        */

        sd_ss_orig_mode_val_e_type orig_mode_val
        /* Origination mode value.
        */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_orig_mode

DESCRIPTION
  Check whether a specified origination mode value includes the current
  origination mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_orig_mode(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_orig_mode_val_e_type    orig_mode_val
            /* Origination mode value.
            */
);


/*===========================================================================

FUNCTION sdss_is_pref_reason0

DESCRIPTION
  Check whether a specified SS-Preference reason equals the current
  SS-Preference reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_pref_reason0(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        uint32                        pref_reason
            /* SS-Preference reason combination type.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_hybr_pref

DESCRIPTION
  Check whether a specified hybrid preference equals the current hybrid
  preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified hybrid preference equals the current hybrid
  preference. FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_hybr_pref(

        sd_ss_hybr_pref_e_type        hybr_pref
            /* Hybrid preference.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_band_pref

DESCRIPTION
  Check whether a specified band preference equals the current band
  preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified band preference equals the current band preference
  type FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_band_pref(

        sd_ss_band_pref_e_type        band_pref
            /* Band preference.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_acq_specific_sid

DESCRIPTION
  Indicate whether last acquisition is for acquiring a specific SID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if last acquisition is for acquiring a specific SID. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                     sdss_is_acq_specific_sid(

        sd_ss_e_type                ss
            /* System selection - MAIN or HDR.
            */
);

/*===========================================================================

@FUNCTION sdss_is_gprs_anite_gcf

@DESCRIPTION
  Indicates if a SS in underdoing GCF tests.

@DEPENDENCIES
  None.

@RETURNS
  TRUE if this SS is underdoing GCF tests.
  FALSE otherwise.

@SIDE_EFFECT
  None.

@x
===========================================================================*/
extern boolean                        sdss_is_gprs_anite_gcf(

       sd_ss_e_type                   ss
       /**< System Selection to check.
       */

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_pref

DESCRIPTION
   Fetches the current preferences from SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                       sdss_get_pref(

        sd_ss_e_type               ss,
            /* System selection - MAIN or HDR.
            */

        sdss_pref_s_type           *pref_info_ptr
            /* Pointer to preference struct which will be populated with
            ** current preferences.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_pref

DESCRIPTION
   Fetches the previous origination mode preferences from SD System selection
   core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_orig_mode_e_type     sdss_get_prev_orig_mode(

        sd_ss_e_type               ss
            /* System selection - MAIN or HDR.
            */

);


/*===========================================================================

FUNCTION sdss_get_prev_sys_info

DESCRIPTION
   Fetches the previous association tag from SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_sys_addn_info_s_type*     sdss_get_prev_sys_info(

        sd_ss_e_type                   ss
            /* System selection - MAIN or HDR.
            */

);

/*===========================================================================

FUNCTION sdss_addn_sys_info_init

DESCRIPTION
   Initialize the aditional sys info structure..

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_addn_sys_info_init(

        sdss_sys_addn_info_s_type*    sys_info
            /* aditional sys info pointer.
            */

);

/*===========================================================================

FUNCTION sdss_set_prev_sys_info

DESCRIPTION
   Sets the previous association tag from SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                       sdss_set_prev_sys_info(

        sd_ss_e_type               ss,
            /* System selection - MAIN or HDR.
            */

        const byte                 assn_tag,
           /* The Association Tag value.
           */
        const int                  geo_offset,
            /* Pointer to system record's geo.
            */
        const int                  multi_geo_count
            /* Pointer to the counter, for counting geo's
            ** in a multi-geo scenario.
            */

);


/*===========================================================================

FUNCTION sdss_update_wlan_fail_reason

DESCRIPTION
   Update the sd_ss_wlan_fail_reas_e_type in the SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void     sdss_update_wlan_fail_reason(

                sd_ss_wlan_fail_reas_e_type fail_reason
                /* Cause of failure
                */
);
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_acc_prm

DESCRIPTION
   Fetch the current access related parameters from the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                        sdss_get_acc_prm(

        sd_ss_e_type                ss,
            /* System selection - MAIN or HDR.
            */
        byte                        *rtrn_acc_reas_ptr,
            /* Pointer to a buffer where to deposit the access reason.
            */

        byte                        *rtrn_acc_term_ptr
            /* Pointer to a buffer where to deposit the access termination.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_sim_state

DESCRIPTION
   Fetches the current sim state from SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  Current sim state.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_sim_state_e_type    sdss_get_sim_state(

        void
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_update_sim_state

DESCRIPTION
   Updates the current SIM state of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_update_sim_state(

        sys_sim_state_e_type  sim_state
            /* New SIM state reported from REG.
            */
);


#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_update_srv_status

DESCRIPTION
   Updates the current service status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void             sdss_update_srv_status(

        sd_ss_e_type     ss,
            /* System selection - MAIN or HDR.
            */

        sys_srv_status_e_type srv_status,
            /* New service status.
            */

        sys_srv_domain_e_type  srv_domain
            /* New srv domain.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_srv_status

DESCRIPTION
   Get the current service status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  Return the current service status of the SS-Core.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_srv_status_e_type  sdss_get_srv_status(

        sd_ss_e_type     ss
            /* System selection - MAIN or HDR.
            */
);


/*===========================================================================

FUNCTION sdss_get_simultaneous_mode

DESCRIPTION
   Get the simultaneous mode of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  Return the simultaneous mode of the SS-Core.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_simultaneous_mode_e_type  sdss_get_simultaneous_mode(

        sd_ss_e_type     ss
            /* System selection - MAIN or HDR.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_redir

DESCRIPTION
  Check whether a specified redirection value includes the current
  redirection type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified redirection value includes the current redirection
  type FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_redir(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        sdss_redir_val_e_type         redir_val
            /* Redirection value.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_handoff

DESCRIPTION
  Check whether a specified handoff value includes the current handoff type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified handoff value includes the current handoff type FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_handoff(

        sdss_ho_val_e_type            handoff_val
            /* Handoff value.
            */
);


/*===========================================================================

FUNCTION sdss_misc_is_handoff

DESCRIPTION
  Check whether a specified handoff value includes the current handoff type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified handoff value includes the current handoff type FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_misc_is_handoff(

        sd_ss_ho_e_type               handoff_val
            /* Handoff value.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_resel_state

DESCRIPTION
  Check whether a specified reselection value equals the current reselection
  state.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified reselection value equal the current reselection
  state. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_resel_state(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        sdss_resel_stt_e_type         resel_state
            /* Reselection state value.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_prl_pref_chgd

DESCRIPTION
  Check whether the PRL preference was changed the last time.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if changed.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_prl_pref_chgd(

        sd_ss_e_type                  ss
          /* System selection - MAIN or HDR.
          */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_opr_mode_update

DESCRIPTION
  Update the operating mode status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_opr_mode_update(

        sdss_opr_mode_e_type          opr_mode
            /* New operating mode.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_network_pref_update

DESCRIPTION
  Update the mode and band preference for get network request of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_get_network_pref_update(

        sd_ss_e_type                  ss,
            /* The ss for which to update
            */
        sd_ss_mode_pref_e_type        mode_pref,
            /* system mode preference for get network request.
            */
        sd_ss_band_pref_e_type        band_pref
            /* system band preference for get network request.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_subsc_avail_update

DESCRIPTION
  Update the Subscription availability status in the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_subsc_avail_update(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        boolean                       is_gw_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                       is_1x_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

        boolean                       is_wlan_subsc_avail
           /* Indicates if subscription is available for providing service on
           ** WLAN network.
           */

);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_pref_update

DESCRIPTION
  Update the SS-Preference setting of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any of the new preferences is different than the existing
  preferences. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_pref_updated_e_type      sdss_pref_update(

        sd_ss_pref_reas_e_type        pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type        orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type        mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency.
            */

        sd_ss_band_pref_e_type        band_pref,
            /* New band preference (BC0, BC1, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency..
            */

        sd_ss_prl_pref_e_type         prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type        roam_pref,
            /* New roam preference (any, home, affiliated, etc.) - ignored
            ** when orig_mode = OTASP or emergency..
            */

        sd_ss_hybr_pref_e_type        hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type              otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */


        sd_ss_srv_domain_pref_e_type   domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_tech_pref_e_type         tech_pref,
           /* Technology preference setting.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref,
           /* WLAN scan preference.
           */

        sys_wlan_bss_e_type            bss_type_pref
           /* BSS type.
           */

);




#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */


#ifdef FEATURE_TECH_IND_SD_STACK_ARCH
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hybr_1_pref_update

DESCRIPTION
  Update the HDR SS-Preference setting of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any of the new preferences is different than the existing
  preferences. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_pref_updated_e_type      sdss_hybr_1_pref_update(

        sd_ss_pref_reas_e_type        pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type        orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type        mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency.
            */

        sd_ss_band_pref_e_type        band_pref,
            /* New band preference (BC0, BC1, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency..
            */

        sd_ss_prl_pref_e_type         prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type        roam_pref,
            /* New roam preference (any, home, affiliated, etc.) - ignored
            ** when orig_mode = OTASP or emergency..
            */

        sd_ss_hybr_pref_e_type        hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_ss_srv_domain_pref_e_type   domain_pref,
            /* Service domain preference that is associated with the selected
            ** NAM.
            ** Applies only for GSM/WCDMA modes.
            */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
            /* Preference for the order of acquisition ( WCDMA before GSM,
            ** GSM before WCDMA etc).
            */

        sd_ss_tech_pref_e_type         tech_pref,
           /* Technology preference setting.
           */

        sys_wlan_scan_pref_e_type      wlan_scan_pref,
           /* WLAN scan preference.
           */

        sys_wlan_bss_e_type            bss_type_pref
           /* BSS type.
           */

);
#endif /* FEATURE_TECH_IND_SD_STACK_ARCH */

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */





/* <EJECT> */
/*===========================================================================

FUNCTION sdss_redir_update

DESCRIPTION
  Update the redirection status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_redir_update(

        sd_ss_e_type                  ss,
          /* System selection - MAIN or HDR.
          */

        sd_ss_redir_e_type            redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        boolean                       is_valid,
            /* Indicate whether redirection is valid for this target.
            */

        word                          expected_sid,
            /* Expected SID. If the MS is being redirected to a specific
            ** system, indicate the SID of that system; otherwise, should be
            ** set to SD_WILDCARD_SID.
            */

        word                          expected_nid,
            /* Expected NID. If the MS is being redirected to a specific
            ** network, indicate the NID of that network; otherwise, should
            ** be set to SD_WILDCARD_NID.
            */

        boolean                       is_rif,
            /* Return if fail indicator.  Indicates whether the MS is
            ** required to return to the system from which it is being
            ** redirected upon failure to obtain service using the
            ** redirection criteria specified by this function call.
            */

        boolean                       is_ignore_cdma,
            /* IGNORE_CDMA indicator. Indicates whether the MS is to ignore
            ** the CDMA Capability Message on the analog system to which it
            ** is being redirected.
            */

        word                          max_redirect_delay
            /* Maximum delay upon redirection. Indicate the maximum delay
            ** time, in units of 8 second increments, to be used by MS in the
            ** event of a global redirection (GSRDM) to analog mode in order
            ** to avoid overloading an underlying analog cell's reverse
            ** control channel.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_redir_update2

DESCRIPTION
  Update the redirection status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                          sdss_redir_update2(

        sd_ss_e_type                  ss,
          /* System selection - MAIN or HDR.
          */

        sd_ss_redir_e_type            redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        boolean                       is_valid,
            /* Indicate whether redirection is valid for this target.
            */

        boolean                       is_rif,
            /* Return if fail indicator.  Indicates whether the MS is
            ** required to return to the system from which it is being
            ** redirected upon failure to obtain service using the
            ** redirection criteria specified by this function call.
            */

        const sd_redir_rec_s_type *rec_list_ptr
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_cdma_lock_update

DESCRIPTION
  Update the CDMA lock status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_cdma_lock_update(

        boolean                       is_cdma_locked
            /* Indicate whether MS is CDMA locked until power-cycle.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_cdma_avail_update

DESCRIPTION
  Update the CDMA-available related parameters of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_cdma_avail_update(

        sd_ss_e_type                  ss,
          /* System selection - MAIN or HDR.
          */

        boolean                       is_reset
          /* Indicate wether to reset the CDMA-available counter.
          */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acc_update

DESCRIPTION
  Update the access related parameters of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_acc_update(

        sd_ss_e_type                  ss,
          /* System selection - MAIN or HDR.
          */

        sd_ss_acc_reas_e_type         acc_reas,
            /* Access termination reason.
            */
        sd_ss_acc_term_e_type         acc_term
            /* Access termination reason.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_cnt_update

DESCRIPTION
  Update the event counters/timers of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_event_cnt_update(

        sd_ss_e_type                   ss,
          /* System selection - MAIN or HDR.
          */

        sdss_evt_cnt_updt_reas_e_type  evt_cnt_updt_reas
          /* Access termination reason.
          */

);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_sid_update
DESCRIPTION
  Update the event system ID of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_event_sid_update(

        sd_ss_e_type                   ss,
          /* System selection - MAIN or HDR.
          */

        const sd_sid2_u_type           *sid_ptr
            /* Pointer to system SID.
            */

);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_is_same_is856_sid

DESCRIPTION
  Check whether a system ID that is matching the specified
  IS-856 Sector ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system ID match the specified IS-856 Sector ID. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_event_is_same_is856_sid(

        sd_ss_e_type              ss,
            /* System selection enumeration type
            */

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        int                       len
            /* Length to match.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_is856_sid_update

DESCRIPTION
  Update the event SID of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_event_is856_sid_update(

        sd_ss_e_type                   ss,
          /* System selection - MAIN or HDR.
          */

        const byte                     sid_ptr[16]
            /* Pointer to system SID.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_update_avail_sys_status

DESCRIPTION
  Goes throught the entire system table and updates the AVAIL/UNAVAUL
  prl flag based on the prl_pref.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdss_update_avail_sys_status
(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list
            /* List for which to update the avail status.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_throttle_acq_on

DESCRIPTION
  This routine sets the throttle enable flag, so throttling can begin when
  acquisition begins.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_throttle_acq_on(

        sd_ss_e_type                   ss
          /* System Selection instance.
          */

);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_throttle_acq_off

DESCRIPTION
  This routine resets the throttle enable flag, so throttling can be disabled,
  when there is event on this engine.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_throttle_acq_off(

        sd_ss_e_type                   ss
          /* System Selection instance.
          */

);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_throttle_reset_cntrs

DESCRIPTION
  This function resets the throttling counters.Called when SD determines to
  idle on the acquired system.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_throttle_reset_cntrs(

        sd_ss_e_type                   ss
          /* System Selection instance.
          */

);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hdr_bcmcs_start_uptime_update

DESCRIPTION
  This function indicate the HDR BCMCS flow monitoring is started. It updates
  the start time with the uptime.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_hdr_bcmcs_start_uptime_update(

        sd_ss_e_type                   ss
          /* System Selection instance.
          */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hdr_bcmcs_stop_uptime_update

DESCRIPTION
  This function indicate the HDR BCMCS flow monitoring is stopped. It updates
  the stop time with the uptime.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_hdr_bcmcs_stop_uptime_update(

        sd_ss_e_type                   ss
          /* System Selection instance.
          */
);



#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_ho_update

DESCRIPTION
  Update the handoff status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_ho_update(

        sd_ss_ho_e_type               ho_type
            /* Handoff type.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_orig_success_update

DESCRIPTION
  Update the orig-success status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_orig_success_update( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_cdma_request_mode

DESCRIPTION
  Get the CDMA request_mode to be used in CDMA origination and page-response
  messages.

  The CDMA request mode is determined according to the current mode and
  band preferences.

DEPENDENCIES
  None.

RETURN VALUE
  CDMA request_mode as defined in ANSI J-STD-008 Table 2.7.1.3.2.4-1 and
  cai.h.

SIDE EFFECTS
  None.

===========================================================================*/
extern  byte                  sdss_get_cdma_request_mode( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_gsm_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported GSM
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported GSM bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sdss_get_supp_gsm_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference out of which to extract supported AMPS bands.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_wcdma_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported WCDMA
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported WCDMA bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sdss_get_supp_wcdma_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference out of which to extract supported AMPS bands.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_mode_band_capability

DESCRIPTION
  Fill in the modes and band preference based the mode and band capability.

DEPENDENCIES
  sdss_init() has been initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                   sdss_get_mode_band_capability(

        sd_ss_mode_pref_e_type    *mode_pref_ptr,
            /* Mode preferences to be filled in.
            */

        sd_ss_band_pref_e_type    *band_pref_ptr
            /* Band preferences to be filled in.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_mode_pref

DESCRIPTION
  Return a mode preference that corresponds to all the supported modes
  preferences out of the input mode preference.

DEPENDENCIES
  None.

RETURN VALUE
  Return a mode preference that corresponds to all the supported modes
  preferences out of the input mode preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_mode_pref_e_type    sdss_get_supp_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference from which to extract supported modes.
            */
        sd_ss_band_pref_e_type    band_pref
            /* band preference from which to extract supported modes.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_subsc_type

DESCRIPTION
  Get the subscription type from the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  Enumeration type of subscription

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_subsc_e_type                       sdss_get_subsc_type(

        sd_ss_e_type                  ss
            /* System selection - MAIN or HDR.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_band_pref

DESCRIPTION
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sdss_get_supp_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference from which to extract supported modes.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_band_pref

DESCRIPTION
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference and the specific mode.

DEPENDENCIES
  None.

RETURN VALUE
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference and the specific mode.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sdss_get_supp_mode_band_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference which to check for support.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_supp_mode_band_pref

DESCRIPTION
  Check whether a specified combination of mode and band preference is
  supported by the current target.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_is_supp_mode_band_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference which to check for support.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_supp_mode_band

DESCRIPTION
  Check whether a specified combination of mode and band is supported by the
  current target.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified combination of mode and band is supported by the
  current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_is_supp_mode_band(

        sd_mode_e_type            mode,
            /* Mode which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference which to check for support.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_mode_to_mode_pref

DESCRIPTION
  Map mode to mode preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_mode_pref_e_type    sdss_map_mode_to_mode_pref(

        sd_mode_e_type            mode
          /* Mode mapped to mode pref.
          */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_mode_pref_to_mode

DESCRIPTION
  Map mode preference to mode.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_mode_e_type    sdss_map_mode_pref_to_mode(

        sd_ss_mode_pref_e_type        mode_pref
          /* Mode pref mapped to mode.
          */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_band_to_band_pref

DESCRIPTION
  Map band to band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type     sdss_map_band_to_band_pref(

        sd_band_e_type            band
           /* Band mapped to band pref.
           */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_band_pref_to_band

DESCRIPTION
  Map band to band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_band_e_type            sdss_map_band_pref_to_band(

        sd_ss_band_pref_e_type    band_pref
          /* Band mapped to band pref.
          */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_mode_to_mode_pref

DESCRIPTION
  Find the intersection of two BSS types.

DEPENDENCIES
  None.

RETURN VALUE
  BSS Type.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_wlan_bss_e_type       sdss_get_common_bss_type(

        sys_wlan_bss_e_type       bss_type1,
          /* BSS type 1
          */

        sys_wlan_bss_e_type       bss_type2
          /* BSS type 1
          */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_supp_sys

DESCRIPTION
  Check whether a specified system (i.e., a combination of mode, band and
  CDMA channel/AMPS system is supported by the current target.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified system is supported by the current target. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_is_supp_sys(

        sd_mode_e_type            mode,
            /* Mode which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* Band which to check for support.
            */

        sd_chan_type              chan
            /* Channel which to check for support.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_set_flag2

DESCRIPTION
  Check whether a specified SS-Preference reason equals the current
  SS-Preference reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                       sdss_set_flag2(

        sd_ss_e_type        ss,
            /* System selection - MAIN or HDR.
            */

        sdss_flag_e_type    flag,
            /* SS-Preference reason combination type.
            */

        boolean             val
            /* Set the Stack Sync Up Flag to this value
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_group_print

DESCRIPTION
  Print an SS-Event group to which the input SS-Event belongs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_event_group_print(

        sdss_evt_e_type       event
            /* SS-Event for which to print the event group.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_act_print

DESCRIPTION
  Print an SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                        sdss_act_print(

        const sd_ss_act_s_type      *act_ptr
            /* Pointer to an SS-Action which to print.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_iact_print

DESCRIPTION
  Print an SS-Internal-action.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                        sdss_iact_print(

        const sdss_iact_s_type      *iact_ptr
            /* Pointer to an SS-Action which to print.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_map

DESCRIPTION
  Map list to the appropriate list according to the input system selection.

DEPENDENCIES
  None.

RETURN VALUE
  Mapped system record list.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_e_type             sdss_sr_list_map2(

        sd_ss_e_type            ss,
          /* System selection - MAIN or HDR.
          */

        sdsr_e_type             list
            /* List for which to check the mode designation.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_is_mode

DESCRIPTION
  Check whether a specified mode value includes the mode designation of at
  least one of the systems that are referenced by a specified list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified mode value includes the mode designation of at least
  one of the systems that are referenced by the specified list. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_sr_list_is_mode(

        sdsr_e_type                   list,
            /* List for which to check the mode designation.
            */

        sd_mode_e_type                mode_val
            /* Mode value against which to compare.
            */
);



/*===========================================================================

FUNCTION sdss_sr_list_is_comply

DESCRIPTION
  Check whether a specified list contains 1 or more systems that are
  compliant with the SS preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified list contains 1 or more systems that
  are currently available for acquisition attempts.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_sr_list_is_comply(

       sd_ss_e_type                   ss,
          /* System selection enum type.
          */

        sdsr_e_type                   list
            /* List for which to check.
            */

);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_get_state

DESCRIPTION
  Return the acquisition state of script engine that is at the top of the
  stack.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  The acquisition state of script engine that is at the top of the stack.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_acq_stt_e_type   sdss_eng_stack_get_state(

        sd_ss_e_type          ss
            /* System selection - MAIN or HDR.
            */
);




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
=============================== SYSTEM RECORD ===============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_sr_is_mode_pref

DESCRIPTION
  Check whether a specified system complies with a specified mode preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system complies with the specified mode preference. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_mode_pref(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record which to check for mode preference
            ** compliance.
            */

        sd_ss_mode_pref_e_type    mode_pref
            /* Mode preference against which to check the system.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_band_pref

DESCRIPTION
  Check whether a specified system complies with a specified band preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system complies with the specified band preference. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_band_pref(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record which to check for mode preference
            ** compliance.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference against which to check the system.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_bss_type_pref

DESCRIPTION
  Check whether a specified system complies with a specified bss type
  preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system complies with the specified bss type preference. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_bss_type_pref(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record which to check for mode preference
            ** compliance.
            */

        sys_wlan_bss_e_type       bss_type_pref
            /* Mode preference against which to check the system.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_pref_conf

DESCRIPTION
  Check whether a specified system conflicts with the current system
  selection preference (i.e., mode, band and roam preference).

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system conflicts with the current system selection preference (as
  per the pref_conf category parameter). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_pref_conf(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to check SS-Preference
            ** conflict.
            */

        sdss_pref_conf_e_type     pref_conf
            /* SS-Preference conflict category to be evaluated.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_prl

DESCRIPTION
  Check whether a system complies with a specific PRL designation value.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system complies with the PRL specified designation value.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_prl(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to check PRL designation.
            */

        sdss_prl_val_e_type       prl_desig_val
            /* PRL designation value against which to checked.
            */
);




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== MAJOR ROUTINES ===============================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION sdss_nv_init

DESCRIPTION
Initializes the NV parameters

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                    sdss_nv_init(

        sd_ss_e_type            ss
            /* The SS to initiaize
            */
);

/*===========================================================================

FUNCTION sdss_component_init

DESCRIPTION
  Initializes the SS-Core component.

  NOTE! This function only returns after the system selection related items
  are read from NV.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                    sdss_component_init( void );

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_srda_enabled_update

DESCRIPTION
  Update the srda field in SS-Core component.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                    sdss_srda_enabled_update( boolean enabled );




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_recovery

DESCRIPTION
  Performs error recovery when the SS-Client reports a system selection event
  that is not allowed relative to the current state of SD or when the END
  script construct is reached.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  TRUE if event processing should continue, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean               sdss_event_recovery(

        sd_ss_e_type          ss,
            /* System selection item.
            */

        sdss_evt_e_type       *event_ptr,
            /* Pointer to SS-Event received from SS-Front-end.
            */

        sdss_iact_s_type      *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_proc

DESCRIPTION
  Process an SS-Event called for by the SS-Front-end and instruct the
  SS-Front-end of the next SS-Internal-Action to take.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  None

  Note that SS-Internal-Action to be taken by the SS-Front-end is returned
  through the rtrn_iact_ptr parameter.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_event_proc(

        sdss_evt_e_type       event,
            /* SS-Event to process.
            */

        sdss_iact_s_type      *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
);


#ifdef FEATURE_TECH_IND_SD_STACK_ARCH
/*===========================================================================

FUNCTION sdss_hybr_1_event_proc

DESCRIPTION
  Process an HDR related SS-Event called for by the SS-Front-end and instruct
  the SS-Front-end of the next SS-Internal-Action to take.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  None

  Note that SS-Internal-Action to be taken by the SS-Front-end is returned
  through the rtrn_iact_ptr parameter.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_hybr_1_event_proc(

        sdss_evt_e_type       event,
            /* SS-Event to process.
            */

        sdss_iact_s_type      *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
);

#elif defined FEATURE_HDR_HYBRID
#error code not present
#endif /* FEATURE_HDR_HYBRID */


#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_avoid2

DESCRIPTION
  Mark the system records that are referenced by the list parameter to be
  avoided from any acquisition attempts for the number of seconds that is
  indicated by the time parameter.

  Note that marking a system to be avoided from acquisition attempts
  overwrites any reacquisition attempts that are scheduled for that system.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdss_sr_list_avoid2(

        const sd_ss_e_type        ss,
           /* System selection - MAIN or HDR.
           */

        sdsr_e_type               list,
           /* List to be avoided from acquisition attempts.
           */

        dword                      avoid_time
           /* The time period in seconds during which acquisition attempts
           ** should be avoided.
           */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_sys_resel_uptime

DESCRIPTION
  Return the start time for the specified system reselection timer.

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
extern  dword                    sdss_get_sys_resel_uptime(

        sd_ss_e_type             ss,
            /* System selection item.
            */

        int                      timer
            /* Specify the system reselection timer type for the start time.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_new_acq_cnt

DESCRIPTION
  Return the new_acq_cnt for the specified system reselection item.

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
extern  byte                     sdss_get_new_acq_cnt(

        sd_ss_e_type             ss
            /* System selection item.
            */
);

/*===========================================================================

FUNCTION sdss_get_ss_for_mode

DESCRIPTION
 Gets the SS for the Mode

DEPENDENCIES
  None.

RETURN VALUE
  The ss for the mode, if valid
  SD_SS_MAX otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_e_type                  sdss_get_ss_for_mode(

        sd_mode_e_type        mode_pref
            /* Hybrid preference.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sys_lost_update

DESCRIPTION
  Update the system lost reason of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_sys_lost_update(

        sd_ss_e_type                  ss,
          /* System selection - MAIN or HDR.
          */

        sd_ss_sys_lost_e_type         sys_lost
            /* System lost reason.
            */

);


#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#endif


/*===========================================================================

FUNCTION sdss_get_conn_mode_uoos_timers

DESCRIPTION
  Get the connected mode UOOS timers

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_get_conn_mode_uoos_timers(

        sd_ss_e_type ss,
            /* The instance
            */

        dword    *conn_mode_awake_time,
            /* The total scan time
            */

        dword    *conn_mode_sleep_time
            /* UMTS scan time
            */
);

/*===========================================================================

FUNCTION sdss_is_loop_cond

DESCRIPTION
  Returns TRUE is loop_cond is true

DEPENDENCIES
  None.

RETURN VALUE
 True : loop_cond is TRUE
 False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean               sdss_is_loop_cond(

        sd_ss_e_type          ss,
            /* The ss associated with the parser
            */

        sdss_loop_cond_e_type loop_cond
            /* The loop condition
            */
);


/*===========================================================================

FUNCTION sdss_is_if_cond

DESCRIPTION
  Returns TRUE is if_cond is true

DEPENDENCIES
  None.

RETURN VALUE
 True : if_cond is TRUE
 False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean               sdss_is_if_cond(


        sd_ss_e_type          ss,
            /* The ss associated with the parser
            */

        sdss_if_cond_e_type   if_cond,
            /* The if condition
            */

        sdss_cmp_criteria_e_type cmp_criteria,
            /* The Comparision criteria
            */

        word                  para1,
            /* Depending on the condition, we may require additional parameters
            */

        word                  para2
            /* Depending on the condition, we may require additional parameters
            */
);


/*===========================================================================

FUNCTION sdss_reset_gw_acq_para

DESCRIPTION
  Reset the GW scan parameters

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_reset_gw_acq_para(

        sd_ss_e_type          ss
            /* The ss associated with the parser
            */
);

#ifdef FEATURE_HYBR_GW
#error code not present
#endif

/*===========================================================================

FUNCTION sdss_is_prl_support_hdr

DESCRIPTION

  Return the indication whether PRL Support HDR or not based on current
  BAND PREF

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean    sdss_is_prl_support_hdr(void);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_avoid

DESCRIPTION
  Check whether a specified system is to be avoided from acquisition attempts
  at the current time.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified system is to be avoided from acquisition attempts
  at or before the current time. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_avoid(

        sd_ss_e_type              ss,
            /* Current system selection item type.
            **
            */
        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record for which to check the avoid
            ** from acquisition attempts status.
            */
);

/*===========================================================================

FUNCTION sdss_clear_bsr_timer

DESCRIPTION

  Clears BSR timer uptime.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN void sdss_clear_bsr_timer(void);

/*===========================================================================

FUNCTION sdss_check_bsr_timer

DESCRIPTION

  Checks whether to clear BSR timer uptime and if needed clears BSR timer uptime.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN void sdss_check_bsr_timer(void);

/*===========================================================================

FUNCTION sdss_set_gw_pwr_save_acq_stage

DESCRIPTION

  Set the GW power save acquisition stage.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN void sdss_set_gw_pwr_save_acq_stage(

            sd_ss_e_type                ss,
            /* SS */

            byte                             val
            /* STAGE value */
);

/**************************************************************************
FUNCTION sdss_gw_sim_domain_valid

DESCRIPTION

  Checks whether SIM is valid for all selected domains in current SS.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN boolean sdss_gw_sim_domain_valid(
           sd_ss_e_type          ss
            /* The ss associated with the parser
            */
);
#endif /* SDSS_H */

