#ifndef CMLL_H
#define CMLL_H
/*===========================================================================

              C A L L   M A N A G E R   L O W E R   L A Y E R

                       H E A D E R   F I L E

DESCRIPTION
GENERAL DESCRIPTION
  This source file contains the function definitions necessary for the
  Call Manager module and the Lower Layers to send commands to one
  another. This file contains only the common interfaces, i.e. interfaces
  shared by all protocol stacks. Individual protocol interfaces are
  defined in their respective header files.


Copyright (c) 2001 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/cmll.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/09   aj      Adding support for passing otasp status to clients
04/09/09   aj      Added support for IRAT measurements
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
09/18/08   fj/aj   Add support for UMA-GAN feature
09/16/08   pk      Added support for WLAN UOoS
09/04/08   dm/sv   Added WPS result report type.
07/24/08   rn      Send regn reject ind to clients
07/18/08   ks      Added new info for Current Operator's Cell ID and
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
05/06/08   ak      BCMCS 2.0 changes
03/27/08   rn/ic   Added support for new command CM_PH_STATUS_CHGD_REQ
03/14/08   ks      Updating UI with service status when UE is OOS in Connected Mode
12/13/07   sk/ak   Added DCM.
12/07/07   ra      Added support for IMS VCC in HOMER files
10/03/07   rn      Added CM_PS_SIG_CON_REL_CNF in cm_name_type
08/17/07   ka/dm   New field to report HS call in progress
06/14/07   pk/cl   Added support for power suspend
06/06/07   pk/cl   Added support for LN status notification from HDR
04/15/07   rp      Removing FEATURE_MMGPS
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/26/07   ka      Changes to support Voip deregistration.
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
11/17/06   jqi     Added CDMA connect Oder Ack support.
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
05/17/06   pk      Added support for DDTM during BCMCS flows
04/17/06   ka      Adding support for HSDPA and DTM indications.
03/10/06   pk      Added support for high PSIST
03/09/06   pk      Added support for Generic reports
03/02/06   pk      Added VoIP - 1X handoff reports
02/17/06   ka      Adding support for videoshare
01/06/06   ka      Adding Release 5 changes including emerg call list,
                   traffic flow template for PDP modify and protocol config
                   options for some of the GW PS call reports.
12/02/05   ka      Adding new report primitive CM_MM_PS_DATA_SUSPEND_IND
11/30/05   ka      CM_HPLMN_SEARCH_CNF is sent from REG in response to
                   CM_HPLMN_SRCH_REQ
11/18/05   sk      Added CM API cm_sd_rpt_free_buf() to free CM SD reports.
09/29/05   pk      Added cm_privacy_setting_type to cm_mc_rpt_type union
09/26/05   ka      Adding CM_IP_CALL_IND_MT_RING_CNF.
09/09/05   ic      Restored CM_INTERCEPT_F enum value accounting for removal
                   of CM_REORDER_F
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/22/05   ic      Obsoleted CM_REORDER_F.
                   Use CM_CALL_ORIG_FAIL_F instead with orig_fail.code set to
                   CM_CALL_ORIG_ERR_REORDER and call_ptr->end_status set to
                   CM_CALL_END_REORDER
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
04/05/05   ka      Adding CM_CIPHER_IND to cm_name_e
03/14/05   ka      Adding report types for IP apps
02/01/05   dk      Added WLAN Support.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/03/04   ws      Remote WMS support.
11/11/04   sj      Added new call end cnf report for HDR.
11/01/04   ka      New primitive for user to user data signaling
09/22/04   ka      Adding counting comments to cm_name_e
09/08/04   jqi     More ps dormant optimization.
07/28/04   ka      Added support for secondary PDP context
07/21/04   sj      Added support for umts to 1x handover.
06/30/04   dk      Added CM_PS_SIG_CON_REL_IND report.
06/25/04   dk      Changed return type of cm_call_id_deallocate() from boolean
                   to void.
06/03/04   ws      Add support for redirecting number.
05/21/04   ka      Added CM_CAMPED_IND sent by REG when phone gets camped.
04/02/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.23 and MSMSHARED_CM.01.00.10.00.28
03/15/04   ka      Merged changes from 6250.
                   Use service domain form service indicator for call orig.
                   Propagated service domain for call origination.
                   Fixed forcing service domain on the fly for GW system.
03/12/04   jqi     Use service domain form service indicator for call orig.
03/10/04   jqi     Propagated service domain for call origination.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/06/04   ws      Initial jaguar/mainline merge.
11/19/03   ic      Marked CM_HDR_RESELECT_EXIT_HDR_F as obsolete
10/14/03   sj      Added CM_HDR_RESELECT_EXIT_HDR_F report.
10/10/03   ic      Modified cm_call_id_allocate() to die if CM ran out of
                   call ids.
05/01/03   jar     Added CM_GSDI_REFRESH_FCN_IND
04/28/03   AT      Lint cleanup.
04/14/03   ws      Added BSID changes
01/21/03   ws      Updated copyright information for 2003
11/14/02   vt      Merged changes from GW mainline.
                   Added card status and deep sleep reports.
10/31/02   vt      Removed handling of NDSS origination failure - really!.
                   Mainlined feature SD20.
10/28/02   vt      Removed handling of NDSS origination failure.
10/23/02   vt      Mainlined FEATURE_CM_USE_UMTS_REG_INTERFACE.
10/23/02   vt      Rearranged the order of LL reports in enum
10/05/02   vt      Made some functions external for SD20. Added new SD20
                   reports.
07/17/02   vt      Updated it for multimode.
07/08/02   vt      Added call id allocate & deallocate functions.
                   Removed inclusion of cmwll.h. cmwll.h should include
                   this file.
05/04/02   PRK     Included cmwll.h.
04/25/02   RI      Added process_uss_data_conf field to cm_rpt_type.
04/24/02   RI      Added CM_RAB_REESTAB_FAIL and renamed CM_RAB_ESTABLISH_IND to
                   CM_RAB_REESTABLISH_IND in cm_name_type.
                   Added  rabm_rab_reestablish_fail_T message and
                   renamed rabm_rab_establish_ind_T to
                   rabm_rab_reestablish_ind_T in cm_rpt_type.
02/06/02   PRK     Added cm_tc_get_buf_else_err_fatal and cm_tc_send_cmd
                   functions.
02/04/02   RI      Modifications based on mn_cm_exp.h for Supplementary Services.
                     - removed mn_cm_exp_t.h
11/21/01   RI      Added rabm_rab_release_ind_T in cm_rpt_type
                   for PS Data.
10/15/01   RI      Added Supplementary Services declarations.
08/23/01   HD      NAS header file reorg changes + RSSI reporting for GSM/WCDMA.
04/27/01   HD      Initial release.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "rex.h"
#include "queue.h"
#include "cm.h"
#include "sd.h"                       /* Interface to System Determination */
#include "cmutil.h"                   /* for cm_ipapp_cause_e_type */


/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

typedef q_type          cm_cmd_q_type;


/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/*
** Reports to be sent to the Call Manager from lower layers.
*/
typedef enum cm_name_e
{
  CM_RPT_NONE=-1,
    /* FOR INTERNAL USE OF CM ONLY! */

  /*
  ** Reports from GSM/WCDMA protocol stack
  */

  CM_RPT_GW_BEGIN = 0,

  /* MN events */
  CM_MO_CALL_CONF,
  CM_MT_CALL_IND,
  CM_CALL_PROGRESS_INFO_IND,
  CM_CALL_CONNECTED_IND,

  /* 5 */
  CM_CONNECTED_NUMBER_IND,
  CM_MT_END_CALL_IND,
  CM_MT_USER_DATA_IND,
  CM_MT_SETUP_IND,

  /* 9 */
  CM_MT_MODIFY_IND,
  CM_MT_DISC_IND,
  CM_START_DTMF_CONF,
  CM_STOP_DTMF_CONF,

  /* 13 */
  CM_MANAGE_CALLS_CONF,
  CM_MODIFY_COMPLETE_CONF,
  CM_ABORT_CC_CONF,
  CM_ACTIVATE_SS_CONF,

  /* 17 */
  CM_DEACTIVATE_SS_CONF,
  CM_ERASE_SS_CONF,
  CM_INTERROGATE_SS_CONF,
  CM_REGISTER_PASSWORD_CONF,

  /* 21 */
  CM_REGISTER_SS_CONF,
  CM_PROCESS_UNSTRUCTURED_SS_CONF,
  CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF,
  CM_MT_CALL_SS_IND,

  /* 25 */
  CM_FORWARD_CHECK_SS_INDICATION_IND,
  CM_UNSTRUCTURED_SS_NOTIFY_IND,
  CM_UNSTRUCTURED_SS_IND,
  CM_RELEASE_UNSTRUCTURED_SS_IND,

  /* 29 */
  CM_CONNECTION_STATE_IND,
  CM_GET_PASSWORD_IND,

  CM_LCS_LOCATION_NOTIFICATION_IND,
  CM_LCS_MOLR_CONF,


  /* 33 */
  /* REG events */
  CM_LOAD_DATA_CNF,
  CM_MM_INFORMATION_IND,
  CM_NETWORK_LIST_CNF,
  CM_SAVE_DATA_CNF,

  /* 37 */
  CM_SERVICE_CNF,
  CM_SERVICE_IND,
  CM_SIM_AVAILABLE_CNF,
  CM_SIM_NOT_AVAILABLE_CNF,

  /* 41 */
  CM_STOP_MODE_CNF,
  CM_DEEP_SLEEP_IND,


  CM_RR_STATUS_IND,
  CM_RRC_STATUS_IND,

  /* 45 */
  CM_PDP_ACTIVATE_CNF,
  CM_PDP_ACTIVATE_REJ,
  CM_PDP_ACTIVATE_IND,
  CM_PDP_DEACTIVATE_CNF,

  /* 49 */
  CM_PDP_DEACTIVATE_IND,
  CM_PDP_DEACTIVATE_REJ,
  CM_PDP_MODIFY_IND,
  CM_PDP_MODIFY_CNF,

  /* 53 */
  CM_PDP_MODIFY_REJ,
  CM_PDP_ACTIVATE_SEC_CNF,
  CM_PDP_ACTIVATE_SEC_REJ,
  CM_PDP_ACTIVATE_REJ_RSP,

  /* 57 */
  CM_PS_SIG_CON_REL_IND,
  CM_RAB_RELEASE_IND,
  CM_RAB_REESTABLISH_IND,
  CM_RAB_REESTAB_CNF,

  /* 61 */
  CM_RAB_REESTAB_REJ,
  CM_RAB_REESTAB_FAIL,
  CM_PDP_UNKNOWN_REQ,
  CM_MT_DATA_CONNECT_IND,

  /* Test Control events */
  /* 65 */
  CM_CHANGE_TEST_MODE_IND,
  CM_SIM_STATE_CHGD_F,                    /* SIM state changed.            */
  CM_SI_NETWORK_LIST_F,
  CM_GSDI_CARD_STATUS_IND,

  /* 69 */
  CM_GSDI_REFRESH_FCN_IND,                /* File Change Notification      */
  CM_DATA_SUSPEND_CHGD_F,                 /* Data suspend flag changed     */
  CM_CS_SIG_REL_IND,                      /* CS RAB release indication     */
  CM_CAMPED_IND,                          /* Camped but Loc Update pending */

  /* 73 */
  CM_UMTS_CDMA_HANDOVER_REQ,
  CM_PDP_DEACTIVATE_PROMOTE_IND,          /* Primary deact,Sec Promoted to primary */
  CM_PDP_DEACTIVATE_PROMOTE_CNF,          /* Primary deact,Sec Promoted to primary */
  CM_USER_CONGESTION_IND,                 /* MN is full, cannot accept user to user data */

  /* 77 */
  CM_CB_DATA_IND_F,
  CM_CB_CELL_CHANGE_IND_F,
  CM_CB_DUPLICATION_DETECTION_IND_F,

  /* 80 */
  CM_SM_DATA_IND_F,
  CM_SM_REPORT_IND_F,
  CM_SM_LINK_CONTROL_IND_F,

  /* 83 */
  CM_CIPHER_IND,                          /* Begin of ciphering ind, GW only*/

  CM_5_SEND_CONF,                         /* Resp to 5_SEND_REQ, CCBS */
  CM_ERASE_CC_ENTRY_CONF,                  /* Cnf to CM_ERASE_CC_ENTRY_REQ */
  CM_CCBS_INFO_IND,               /* Informs if CCBS is allowed on the call */

  /* 87 */
  CM_RECALL_IND,                    /* After CCBS is activated netowork
                                    ** indication to re-originate the call
                                    */

  /* 88 */
  CM_HPLMN_SEARCH_CNF,              /* Sent in response to
                                    ** CM_HPLMN_SEARCH_REQ.
                                    */


  /* 89 */
  CM_MM_PS_DATA_SUSPEND_IND,        /* Sent by MM to indicate that data is
                                    ** suspended. Given by MM only when
                                    ** data suspend cannot be sent using
                                    ** CM_SERVICE_IND or CM_SERVICE_CNF.
                                    */

  /* 90 */
  CM_EMERGENCY_NUM_LIST_IND,        /* Emergency no list send by network */

  /* 91 */
  CM_CELL_SERVICES_IND,             /* Emergency no list sent by network */

  /* 92 */
  CM_HSPA_CALL_STATUS_IND,          /* HS call progress indication */

  /* 93 */
  CM_PS_SIG_CON_REL_CNF,            /* PS Signaling release ack */

  /* 94 */
  CM_PH_STATUS_CHGD_CNF,            /* Phone status changed confirmation
                                    ** sent by NAS. Sent in response to
                                    ** CM_PH_STATUS_CHGD_REQ
                                    */

  /* 95 */
  CM_CONN_MODE_SERVICE_STATUS_IND,  /* Report service status Information
                                    ** when UE is OOS in Connected Mode
                                    */

  /* 96 */
  CM_RR_CELL_INFO_IND,              /* Reoprt Cell_info Ind from RR */

  /* 97 */
  CM_RRC_CELL_INFO_IND,             /* Reoprt Cell_info Ind from RRC */

 
  /* MBMS */ 
  /* 98 */
  CM_RPT_MBMS_ACT_CNF,       /* ACT_CNF given in resp to MBMS Act Req */
  CM_RPT_MBMS_DEACT_CNF,     /* DEACT_CNF given in resp to MBMS Deact Req */
  CM_RPT_MBMS_ACT_IND,       /* ACT_IND sent asyncly to report MBMS status */
  CM_RPT_MBMS_GENERIC,       /* Generic MBMS report sent by lower layers */

  /* 102 */
  CM_REG_REJECT_IND,        /* Registration Reject Indication */
  CM_CONN_MODE_NO_SERVICE_WAKEUP_IND, /* Wake up for scan */

  /* GAN */
  /* 103 */
  CM_GAN_INFO_IND,           /* Report gan_info Ind from GAN library */

  /* 104 */
  CM_MMGSDI_CARD_STATUS_IND,  /* card status ind from MMGSDI */
  CM_MMGSDI_REFRESH_FCN_IND,  /* refresh event from MGSDI */


  CM_RPT_GW_END,


  /*
  ** Reports from CDMA/HDR protocol stack
  */

  /* 300 */
  CM_RPT_1X_BEGIN = 300,
  CM_ABRV_ALERT_F,               /* Generate a CDMA/AMPS abbreviated alert */
  CM_ABRV_REORDER_F,               /* Generate an AMPS abbreviated reorder */
  CM_ABRV_INTERCEPT_F,           /* Generate an AMPS abbreviated intercept */

  /* 304 */
  CM_ALERT_F,                         /* Analog display/process alert info */
  CM_INCOMING_CALL_F,                                /* CDMA Incoming call */
  CM_STOP_ALERT_F,             /* Stop the incoming alert or pending alert */

  /* 308 */
  CM_INTERCEPT_F = 308,                      /* Generate an intercept tone */
  CM_FLASH_F,                  /* Analog display/process flash information */
  CM_DTMF_BURST_F,     /* Play a series of DTMFs - FEATURE_HANDLE_FWD_DTMF */
  CM_DTMF_CONT_START_F,               /* Start playing a continuous DTMF - **
                                                   FEATURE_HANDLE_FWD_DTMF */

  /* 312 */
  CM_DTMF_CONT_STOP_F,                 /* Stop playing a continuous DTMF - **
                                                    FEATURE_HANDLE_FWD_DTMF*/
  CM_CALL_RELEASE_F,                          /* Base station call release */
  CM_CALL_FADE_F,                             /* Call has ended abnormally */
  CM_MARKOV_F,                                         /* Markov Call made */

  /* 316 */
  CM_CALL_CONNECTED_F,                        /* Call has been established */
  CM_DISPLAY_F,                                /* Display CDMA information */
  CM_CALLED_PARTY_F,                           /* Called Party information */
  CM_CALLING_PARTY_F,                         /* Calling Party information */

  /* 320 */
  CM_CONNECTED_NUM_F,                      /* Responding party information */
  CM_SIGNAL_F,                                       /* Signal information */
  CM_UPCALL_F,                              /* Immediately into voice call */
  CM_LOCK_F,                                    /* CDMA Lock order command */

  /* 324 */
  CM_UNLOCK_F,                                      /* CDMA unlock command */
  CM_MAINTREQ_F,                      /* CDMA Maintenance Required command */
  CM_N_VC_MSG_F,              /* NAMPS Short Message rcvd on Voice Channel */
  CM_N_VC_CLI_F,              /* NAMPS Call Line Id recvd on Voice Channel */

  /* 328 */
  CM_OTASP_STATUS_F,       /* Notify the OTASP call status - FEATURE_OTASP*/
  CM_OTASP_COMMIT_STATUS_F,   /* Notify if an OTASP commit is in progress -**
                                                             FEATURE_OTASP */
  CM_VOICE_AS_DATA_F,            /* Notify voice_as_data attribute changed */
  CM_LINE_CTRL_F,               /* Line control message - FEATURE_IS95B_WLL*/

  /* 332 */
  CM_SRV_SYS_INFO_F,                      /* Serving system parameter info */
  CM_CALL_ORIG_FAIL_F,                         /* Call origination failure */
  CM_MO_DBM_STATUS_F,                       /* Data burst message status - **
                                             FEATURE_CDSMS  || FEATURE_DBM */
  CM_MT_DBM_DATA_F,                           /* Data burst message data - **
                                              FEATURE_CDSMS || FEATURE_DBM */

  /* 336 */
  CM_MSG_WAITING_F,                  /* Display number of messages waiting */
  CM_N_VOICE_F,                                        /* NAMPS Voice mail */
  CM_N_MSG_F,                                       /* NAMPS Short Message */
  CM_N_CLI_F,                                /* NAMPS Call Line Identifier */

  /* 340 */
  CM_EXT_DISPLAY_F,    /* Extended Display record - FEATURE_IS95B_EXT_DISP */
  CM_NDSS_REDIRECTION_F,  /* NDSS redirection is started-FEATURE_IS95B_NDSS*/
  CM_HDR_CALL_ORIG_FAIL_F,                 /* HDR Call origination failure */
  CM_HDR_INCOMING_CALL_F,                             /* HDR Incoming call */

  /* 344 */
  CM_HDR_CALL_RELEASE_F,                  /* HDR Base station call release */
  CM_HDR_CALL_FADE_F,                     /* HDR Call has ended abnormally */
  CM_HDR_CALL_CONNECTED_F,                /* HDR Call has been established */
  CM_HDR_CONNECTION_DENY_F,                       /* HDR connection denied */

  /* 348 */
  CM_HDR_REDIRECT_F,                                     /* HDR Redirected */
  CM_HDR_PRL_NOT_PREFERRED_F,            /* System not preferred by System **
                          Determination due to QC redirect or ANID change  */
  CM_HDR_DEACTIVATE_ACK_F,                         /* HDRMC Deactivate Ack */
  CM_HDR_RESELECT_EXIT_HDR_F,      /* Obsolete Exit HDR due to reselection */

  /* 352 */
  CM_HDR_BCMCS_FLOW_STATUS_F,            /* Flow status report             */
  CM_HDR_BCMCS_BROADCAST_RESET_F,        /* Broadcast reset report         */
  CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F, /* Broadcast discontinued ACK     */
  CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F,/* Flow request success rprt */

  /* 356 */
  CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F,/* Flow request failure rprt */

  CM_IDLE_PGSLOT_CHANGED_F,           /* CDMA idle paging slot has changed */
  CM_GPS_MODE_GRANTED_F,     /* Entering GPS mode - FEATURE_GPSONE_MSBASED */
  CM_GPS_MODE_EXIT_F,        /* Aborting GPS mode - FEATURE_GPSONE_MSBASED */

  /* 360 */
  CM_EXT_BRST_INTL_F, /* Extended Burst Type - International - FEATURE_T53 */
  CM_NSS_CLIR_F,   /* National Supplementary services - CLIR - FEATURE_T53 */
  CM_NSS_REL_F, /* National Supplementary services - Release - FEATURE_T53 */
  CM_NSS_AUD_CTRL_F,  /* National Supplementary services - Audio Control - **
                                                               FEATURE_T53 */

  /* 364 */
  CM_SND_BRST_DTMF_ACK_F,          /* L2 Ack for send Burst DTMF message - **
                                                               FEATURE_T53 */
  CM_CALL_HOLD_L2ACK_F, /* Sent to CM after L2 Ack receipt for Call Hold - **
                                                               FEATURE_T53 */
  CM_REG_SUCCESS_F,                /* Registration Success - FEATURE_JCDMA */
  CM_REG_FAILURE_F,                /* Registration Failure - FEATURE_JCDMA */

  /* 368 */
  CM_CALL_FAIL_RETRY_ORDER_F,/* Retry order for call origination failure - **
                                                            FEATURE_IS2000 */
  CM_CALL_EMERGENCY_FLASHED_F,              /* Emergency request flashed - **
                                                            FEATURE_IS2000 */
  CM_EXIT_TC_F,                      /* Traffic channel has been torn down */
  CM_BS_STATUS_RSP_F,     /* Indicate Base Station Status Response Received */

  /* 372 */
  CM_REDIRECTING_NUMBER_F,                /* Redirecting Number information */
  CM_CDMA_CAPABILITY_UPDATE_F, /* Report from MC to update CDMA capability **
                                                               information */
  CM_UMTS_CDMA_HANDOVER_STATUS_F, /* Report from MC to inform the status of  **
                                                         UMTS->1x handover */
  CM_HDR_CALL_END_CNF_F, /* Report from HDRMC to confirm the call end req  */

  /* 376 */
  CM_PRIVACY_SETTING_F, /* Report from MC to inform of the privacy setting */
  CM_TUNNEL_MSG_F,                    /* Report to send a tunneled message */
  CM_HDR_TUNNEL_MSG_F,         /* Report to initiate HDR VoIP - 1X handoff */
  CM_PSIST_F,                  /* Report from MC informing the psist value */

  /* 380 */
  CM_HDR_BCMCS_FLOW_MONITORING_START_F,   /* HDR is monitoring BCMCS flows */
  CM_HDR_BCMCS_FLOW_MONITORING_END_F,   /* HDR is not motoring BCMCS flows */
  CM_CDMA_CONNECT_ORDER_ACK_F,               /* CDMA connect order ACK msg */
  CM_END_VOIP_CALL_F,           /* End the VoIP call, 1X Needs the vocoder */

  /* 384 */
  CM_HDR_LOCATION_NOTIFICATION_STATUS_F, /**< HDR location notification status */
  CM_GET_CSNA_MSG_CONFIG_F,       /* Get the list of allowed CSNA messages */
  CM_UPDATE_CSNA_MSG_CONFIG_F,         /* Update the allowed CSNA messages */
  CM_TUNNEL_MSG_STATUS_F,         /* Confirmation for the tunneled message */

  /* 388 */
  CM_HDR_SESSION_OPEN_FAILED,          /**< HDR session open failed. */
  CM_HDR_SESSION_CLOSED,               /**< HDR session was closed. */
  CM_HDR_BCMCS_REG_STATUS_F,      /**< Report for BCMCS_CMD_REG_REQUEST */

  /* 391 */
  CM_MMGSDI_REFRESH_FCN_IND_F,    /* refresh event from MGSDI */
  CM_HDR_MEAS_MODE_F,             /* Measurement mode report from HDRCP */
  CM_HDR_MEAS_RESP_F,             /* Measurement response from HDR CP */
  CM_RPT_1X_END,

  /*
  ** Reports from SD20
  */

  /* 500 */
  CM_RPT_SD_BEGIN = 500,
  CM_SRV_IND_INFO_F,                     /* Service indicators information */
  CM_ACQ_FAIL_F,                    /* Acquisition failed over all systems */
  CM_OK_TO_ORIG_F,                       /* Ok to send origination command */

  /* 504 */
  CM_NAM_CHANGED_F,                    /* NAM changed via Auto NAM command */
  CM_E911_CALLBACK_F,                          /* Enter E911 callback mode */
  CM_STANDBY_SLEEP_F,                     /* Entering powerdown sleep mode */
  CM_STANDBY_WAKE_F,                       /* Exiting powerdown sleep mode */

  /* 508 */
  CM_LPM_OK_F,                          /* Command to enter low power mode */
  CM_WLAN_BSS_LIST_F,                       /* WLAN (802.11) BSS List*/
  CM_IP_APP_REG_STATUS,               /* IP app's registration status      */
  CM_WLAN_WPS_STATUS_F,                     /**< WPS procedure end status. */
  CM_SRV_LOST_F,                   /* Service is lost, used to sync stacks */

  CM_RPT_SD_END,

  /*
  ** Reports from Voip Applications
  */

  /* 600 */
  CM_RPT_IP_BEGIN = 600,
  CM_IP_CALL_IND_MT_INVITE,        /* indicating invite message from network */
  CM_IP_CALL_IND_MT_RING_CNF,      /* Remote party has been informed about
                                   ** start of local ringing */
  CM_IP_CALL_IND_MO_PROGR_INFO,    /* progress of mobile originated call */

  /* 604 */
  CM_IP_CALL_IND_CONNECTED,        /* Message indicating call connection */
  CM_IP_CALL_IND_CALL_END,         /* Report for call end */
  CM_IP_CALL_IND_CALL_HOLD,        /* Call being put on hold indication */
  CM_IP_CALL_IND_CALL_RETRIEVE,    /* Call being retrieved indication   */

  /* 608 */
  CM_IP_CALL_IND_VS_STATUS,        /* Videoshare availability           */
  CM_IP_CALL_IND_SUPS_CONF,        /* Confirmation given by ipapp for call rel sups */
  CM_IP_CALL_IND_TRANSFER,         /* Indicates call being transferred to another party*/
  CM_IP_CALL_IND_CONNECTED_NUM,    /* Updates remote number call is connected to */

  /* 612 */
  CM_IP_APP_ALLAPPS_OPRT_CNF,         /* All IP app's deregistered
                                      ** Common to GW, CDMA, WLAN  so
                                      ** grouped with sd
                                      */

  CM_IP_CALL_IND_CALL_WAIT,        /* second incoming call is received 
                                   ** in busy state
                                   */

  CM_RPT_IP_END,

  /* 620 */
  CM_RPT_IP_SMS_BEGIN = 620,
  CM_IP_SMS_IND_MO_SMS_STATUS,     /* MO SMS status */
  CM_IP_SMS_IND_MT_SMS,            /* Incoming SMS */

  CM_RPT_IP_SMS_END,

  /*
  ** Generic reports
  */

  /* 800 */

  CM_RPT_GENERIC_BEGIN = 800,

  /* DEM reports
  */
  CM_DEM_RPT,                   /* Report from Dynamic Environment Manager */

  CM_WLAN_RPT,                   /* Report for WLAN */

  CM_RPT_GENERIC_END,

  /*
  ** End of definition of CM reports from lower layers
  */

  CM_NUM_CM_COMMANDS                                 /* Maximum CM reports */

} cm_name_type;


/*
** Status from the CM task, in command buffer
*/

typedef enum
{
  CM_DONE_S,                      /* General OK return */
  CM_BUSY_S,                      /* Command is being processed */
  CM_BADCMD_S,                    /* Invalid command */
  CM_BADPARM_S                    /* Bad Parameters */
} cm_status_type;


/**--------------------------------------------------------------------------
** Macros
** --------------------------------------------------------------------------
*/

/* Macros to decide the type of a report
*/
#define CMLL_RPT_IS_SD( event ) \
                    ( ((event) > CM_RPT_SD_BEGIN) && \
                    ((event) < CM_RPT_SD_END) )

#define CMLL_RPT_IS_1X( event ) \
                    ( ((event) > CM_RPT_1X_BEGIN) && \
                    ((event) < CM_RPT_1X_END) )

#define CMLL_RPT_IS_GW( event ) \
                    ( ((event) > CM_RPT_GW_BEGIN) && \
                    ((event) < CM_RPT_GW_END) )

#define CMLL_RPT_IS_IP( event ) \
                    ( ((event) > CM_RPT_IP_BEGIN) && \
                    ((event) < CM_RPT_IP_END) )

#define CMLL_RPT_IS_IPSMS( event ) \
                    ( ((event) > CM_RPT_IP_SMS_BEGIN) && \
                    ((event) < CM_RPT_IP_SMS_END) )

#define CMLL_RPT_IS_GENERIC( event) \
                    ( ((event) > CM_RPT_GENERIC_BEGIN) && \
                    ((event) < CM_RPT_GENERIC_END) )

/*
** Command header type.  Each command to CM starts with this header.
** These are the common fields (base class) that all command headers to CM_
** must !start! with.  They are defined as a macro to insure that all
** headers contain the same set.
*/
#define CM_CMD_HDR_COMMON_FIELDS                                             \
  q_link_type        link;         /* Queue link */                          \
  cm_name_type       cmd;          /* Command to execute */                  \
  rex_tcb_type       *task_ptr;    /* Pointer to requesting task TCB */      \
  rex_sigs_type      sigs;         /* Signal to set upon cmd completion */   \
  cm_status_type     status;       /* Completion code status */              \
  q_type             *done_q_ptr   /* Queue to place this cmd on when done */


/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/*
** Generic command header type - all cmds to CM must start with this or one
** of its supersets.
*/
typedef struct {
  CM_CMD_HDR_COMMON_FIELDS;       /* All CM headers must !start! with this */
} cm_hdr_type;

/*
** Info Record Command header type.  This provides more grouping info needed
** to process info records.  Any fields added must be added after the fields
** that cm_info_hdr_type and cm_xll_rpt_hdr_s_type share in common.
*/
typedef struct {
  CM_CMD_HDR_COMMON_FIELDS;       /* All CM headers must !start! with this */
  boolean            last_record;     /* Marks last msg in a group of msgs */
} cm_info_hdr_type;


/*-------------------------------------------------------------------------*/

/* Type for CM_SRV_IND_INFO_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  sd_ss_e_type       ss;                /* System selectrion - MAIN or HDR */
  sd_si_info_s_type  si_info;            /* Service Indicators information */
} cm_srv_ind_info_type;


/* Type for CM_ACQ_FAIL_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  sd_ss_e_type       ss;                /* System selectrion - MAIN or HDR */
} cm_acq_fail_type;


/* Type for CM_OK_TO_ORIG_F
*/
typedef struct {

  cm_hdr_type              hdr;         /* Header */
  sd_ss_e_type             ss;          /* System selectrion - MAIN or HDR */
  sd_si_ok_to_orig_s_type  si_ok_to_orig;
                         /* System indications related to call origination */
} cm_ok_to_orig_type;


/* Type for CM_E911_CALLBACK_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
} cm_e911_callback_type;


/* Type for CM_NAM_CHANGED_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  byte nam;                                              /* New NAM number */
} cm_nam_changed_type;

/* Type for CM_WLAN_BSS_LIST_F
*/
typedef struct {
  cm_hdr_type                    hdr;                    /* header */
  sys_wlan_bss_info_list_s_type  list;                   /* BSS List */
} cm_wlan_bss_list_type;

/* Type for CM_WLAN_WPS_STATUS_F
*/
typedef struct {
  cm_hdr_type                    hdr;                      /* header */
  sys_wlan_wps_result_s_type     result;                   /* WPS result */
} cm_wlan_wps_result_type;

/* Type for CM_IP_APP_REG_STATUS
*/
typedef struct {
  cm_hdr_type                    hdr;                    /* header */
  cm_ipapp_reg_status_e_type     ipapp_reg_status;       /* Reg, de-reg, failure */
  cm_call_type_mask_type         call_type_mask;         /* Bit mask of CM call types */
  cm_ipapp_cause_e_type          ipapp_cause;            /* ipapp registration cause values */
  cm_ipapp_warn_info_s_type      warn_info;              /* warning info structure */
  sys_sys_mode_mask_e_type       sys_mode;               /* System modes app can support for calls */
} cm_ip_app_reg_status_type;



/* Type for CM_SRV_LOST_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  sd_ss_e_type       ss;                /* System selectrion - MAIN or HDR */
} cm_srv_lost_type;


/* Union of all SD report types.  The header is always present and it
** specifies the command type and attributes. If the command has
** arguments they follow in the union.
*/

typedef union cm_sd_rpt_u
{
  cm_hdr_type                   hdr;
  cm_srv_ind_info_type          srv_ind_info;
  cm_acq_fail_type              acq_fail;
  cm_ok_to_orig_type            ok_to_orig;
  cm_e911_callback_type         e911_callback;
  cm_nam_changed_type           nam_changed;
  cm_wlan_bss_list_type         wlan_bss_list;
  cm_wlan_wps_result_type       wlan_wps_result;
  cm_ip_app_reg_status_type     ip_app_reg_status;
  cm_srv_lost_type              srv_lost;

} cm_sd_rpt_u_type;


/* DEM report types
*/

typedef enum cm_dem_rpt_e
{
  CM_DEM_RPT_NONE = -1,         /* For internal use of CM */

  CM_DEM_RPT_APPS_PWR_COLLAPSE, /* Apps processor is power collapsed */

  CM_DEM_RPT_APPS_PWR_RESTORED, /* Power is restored to Apps processor */

  CM_DEM_RPT_APPS_SUSPEND,      /* Apps processor goes to standby mode */

  CM_DEM_RPT_MAX                /* For internal use of CM */

}cm_dem_rpt_e_type;

typedef struct cm_dem_rpt{

  cm_dem_rpt_e_type          dem_rpt_type;

} cm_dem_rpt_s_type;


/* Union of all generic reports
** currently only DEM reports fall into this category
*/

typedef union cm_generic_rpt_u
{
  cm_dem_rpt_s_type               dem_rpt;

} cm_generic_rpt_u_type;


/* Generic Report structure
*/

typedef struct cm_generic_rpt_s{

  cm_hdr_type                 hdr;

  cm_generic_rpt_u_type       generic_rpt;

} cm_generic_rpt_s_type;



/**--------------------------------------------------------------------------
** Externs
** --------------------------------------------------------------------------
*/


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION CM_CALL_ID_ALLOCATE

DESCRIPTION
  Allocates a call object.
  Call ids are allocated in round robin fashion, from 0 to
  CM_CALL_ID_MAX - 1, then it wraps around with next call id
  allocated being 0 again.

DEPENDENCIES
  None

RETURN VALUE
  Call id or CM_CALL_ID_INVALID if CM ran out of call ids.
  If CM_ERR_FATAL() is compiled and CM ran out of call ids, this
  function will die.

SIDE EFFECTS
  None

===========================================================================*/
extern cm_call_id_type cm_call_id_allocate
(
  void
);



/*===========================================================================

FUNCTION CM_CALL_ID_DEALLOCATE

DESCRIPTION
  A synchronous function that deallocates the specified call object

DEPENDENCIES
  none

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_call_id_deallocate
(
  cm_call_id_type call_id             /* Call id that is to be deallocated */
);



/*===========================================================================

FUNCTION cm_sd_rpt_get_buf_else_err_fatal

DESCRIPTION
  Allocate a CM report command buffer from the cm_mc_rpt_free_q. Error
  fatal if allocation is NOT successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated CM report command buffer.

SIDE EFFECTS
  none

===========================================================================*/
cm_sd_rpt_u_type *cm_sd_rpt_get_buf_else_err_fatal
(
  void
);


/*===========================================================================

FUNCTION cm_sd_rpt_free_buf

DESCRIPTION
  Free a SD report buffer allocated from CM. Do NOT use to function to
  indicate the completion of processing of the report. Use cm_ret_rpt_buf for
  that.

DEPENDENCIES
  none

RETURN VALUE
  none.

SIDE EFFECTS
  none

===========================================================================*/
void cm_sd_rpt_free_buf( cm_sd_rpt_u_type *sd_rpt_ptr );


/*===========================================================================

FUNCTION CM_SD_RPT

DESCRIPTION
  Allows MC to queue up messages to the CM.  Grab a buffer from the
  cm_mc_rpt_free_q, fill it in, and call this function with it. All this will
  do is queue it and set a signal for the CM task.  When the command
  is done, the buffer will be queued as directed in the command header,
  and a task you specify may be signaled.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cm_sd_rpt
(
  cm_sd_rpt_u_type *cmd_ptr               /* Pointer to a CM report buffer */
);


/*===========================================================================

FUNCTION cm_generic_rpt_get_buf_else_err_fatal

DESCRIPTION
  Allocate a CM report command buffer from the heap. Error
  fatal if allocation is NOT successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated CM report command buffer.

SIDE EFFECTS
  none

===========================================================================*/
cm_generic_rpt_s_type *cm_generic_rpt_get_buf_else_err_fatal( void );


/*===========================================================================

FUNCTION CM_GENERIC_RPT

DESCRIPTION
  Allows generic clients to queue up messages to the CM. Dynamically allocate
  a buffer, fill it in, and call this function with it. All this will
  do is queue it and set a signal for the CM task.  When the command
  is done, the buffer will be queued as directed in the command header,
  and a task you specify may be signaled.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_generic_rpt
(
  cm_generic_rpt_s_type *cmd_ptr               /* Pointer to a CM report buffer */
);

#endif /* CMLL_H */
