#ifndef CMSS_H
#define CMSS_H
/*===========================================================================

C A L L   M A N A G E R   S E R V I N G   S Y S T E M   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMSS.C

Copyright (c) 1998 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmss.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/09   aj      Adding support for updating clients with cdma local time
05/20/09   ks      Added default values of camped cell info parameters
03/01/09   sv      Lint cleanup.
11/12/08   ks      Fixing Compilation error
10/21/08   ks      Update default threshold value for sir and pathloss and
                   add masks for RSSI event
09/17/08   ks      When OOS in CELL_FACH, UE needs to follow UOOS scanning 
                   pattern to re-acquire service
09/16/08   pk      Added support for WLAN UOoS
09/05/08   sv      Memory Reduction for ULC
09/04/08   ks      Remove info for Neighbors Cell IDs
07/18/08   ks      Added new info for Current Operator's Cell ID and 
                   Neighbors Cell IDs
06/11/08   sv      Exposing cmss_send_ss_info for Power Collapse
                   buffered events notifications.
02/15/08   ks      Adding SIR, Pathloss Implementation
01/24/08   ks      Display of UE signal strength indication
01/03/08   ka      Adding handset based plus code dialing feature.
12/13/07   sk/ak   Added DCM.
12/07/07   ra      Added support for IMS VCC in HOMER files 
08/12/07   jq      Fixed DCTM timeline issue under JCDMA feature.
06/08/07   cl      Fix CR#: 119770 - cmss_orig_thr_tbl_entry_info_put should 
                   pass parameter by pointer
05/15/07   rp      Adding support for ECIO for GW 
04/15/07   rp      Added last_system_reported
03/30/07   jqi     Changed the DCTM timeline to 0,0,180 under JCDMA feature.
03/09/07   sk      RAM usage reduction.
11/16/06   pk      Lint Fixes
11/11/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
04/06/06   sk      Removed the following fields from cmss_s_type:
                   - hybr_hdr_bcmcs_no_srv_uptime
                   - ss_main_bcmcs_no_srv_uptime
                   - ss_main_bcmcs_srv_status
                   - hybr_hdr_bcmcs_srv_status
03/10/06   pk      Changed the return type of cmss_get_ss_wlan_info_change to
                   uint64
01/06/06   ka      Adding Release 5 changes including emerg call list, 
                   traffic flow template for PDP modify and protocol config
                   options for some of the GW PS call reports.
01/06/06   ic      Lint cleanup  
11/08/05   sk      Separated CM reports and commands.
07/26/05   pk      DCTM modifications, changed the backoff time line from 
                   1,2,4,8 to 0,0,1,2,8,15. Ignoring NID for DCTM, a system 
                   is uniquely identified by SID,PZID.
07/11/05   pk      Modified cmss_check_if_orig_thr_true() added packet_zone 
                   as a additional parameter
06/06/05   pk      Added structures and function definitions for Data Throttle
                   Manager API's
05/19/05   ka      Merging support to inform clients about call ciphering.
02/01/05   dk      Added WLAN Support.
01/10/05   dk      Added cmss_sys_id_match().
10/22/04   jqi     Removed redundent RSSI type from cmss_report_rssi().
10/19/04   jqi     Sent the data available request upon the ps call orig
                   when the data is suspended.
09/03/04   ka      Added service throttling for HDR
08/27/04   jqi     Added ps dormant optimization.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/10/04   jq      Removed cmss_proc_call_connected prototype due to recursive
                   including of cmcall.h
02/06/04   ws      Initial jaguar/mainline merge.
10/21/03   ws      Fixed featurization of cmss_timer_proc
11/19/03   ic      Comment clean up.
11/04/03   ic      Moved cmss_IDM_timer_start_check() to forward declarations
                   section in the .c file
10/24/03   ic      Added cmss_IDM_timer_start_check()
10/22/03   ic/jai  Fixes in IDM code
09/22/03   vt      Generated IDM event on voice call connect.
07/10/03   jqi     Added Extended PRL support
                   Removed FEATURE_IS683C_PRL
02/18/03   vt      Fixed problems to build for HDR.
01/21/03   ws      Updated copyright information for 2003
05/05/02   vt      Fixed compile problem with new mm cm.h
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                     Re-added Serving System group and moved srv_state, roam_status,
                     rssi from phone group to serving system group (FEATURE_NEWSS).
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
09/09/99   SH      Added p_rev and is_registered to support data application
05/31/99   SH      Added CM 2.0 fetures
01/07/99   RC      Changes per ARM compiler.
11/24/98   RC      Initial release.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"       /* Internal interface to cm.c */

#include "cmnv.h"      /* Includes NV services */

#include "sd.h"

#if (defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK)) 
#error code not present
#endif  

#ifdef FEATURE_IMS_VCC
#include "wlan_cp_cm.h" /* CM Interface to WLAN CP*/
#endif

#ifdef FEATURE_HDR_HANDOFF
#error code not present
#endif /* FEATURE_HDR_HANDOFF */

#ifdef FEATURE_CALL_THROTTLE
#define CMSS_ORIG_THR_TABLE_MAX_ENTRIES 20
    /* The size of the throttle table
    */
#else
#define CMSS_ORIG_THR_TABLE_MAX_ENTRIES 1
    /* The size of the throttle table when feature is not enabled.
    ** To reduce RAM usage.
    */
#endif


#ifdef FEATURE_JCDMA
#define CMSS_ORIG_THR_MAX_TIMER_INDEX    4
#else
#define CMSS_ORIG_THR_MAX_TIMER_INDEX    7
#endif
    /* Orig throttle timer slots
    */

#define CMSS_ORIG_THR_RND_INT            6
    /* Randomizing factor for the orig throttle time
    */


#define CMSS_SID_MCC_TBL_MAX             NV_SID_TO_MCC_TBL_LEN
    /* Max Sid to MCC table size
    */

#define CMSS_INVALID_CELL_ID_INFO        0xFFFFFFFF
    /* value for Invalid cell_id info 
    */

#define CMSS_INVALID_LAC_INFO            0xFFFF 
   /* value for Invalid LAC info  
   */ 
  
#define CMSS_INVALID_PSC_INFO            0xFFFF 
   /* value for Invalid PSC info  
   */ 

#define CMSS_INVALID_ARFCN_INFO          0xFFFF 
   /* value for Invalid ARFCN info  
   */ 
 
#define CMSS_INVALID_BSIC_INFO           0xFF 
   /* value for Invalid BSIC info  
   */ 
    
#define CMSS_INVALID_UARFCN_DL_INFO      0xFFFF 
   /* value for Invalid UARFCN_DL info  
   */ 
  
#define CMSS_INVALID_UARFCN_UL_INFO      0xFFFF 
   /* value for Invalid UARFCN_UL info  
   */ 
   
#define CMSS_INVALID_REFPN_INFO          0xFFFF 
   /* value for Invalid REFPN info  
   */ 


#define CDMA_TIME_PTR( xx_info ) (&(xx_info->mode_info.cdma_info.time_info))
   /* Pointer to cdma time info structure obtained from structure of type
   ** cm_mm_ss_info_s_type
   */

#define SS_CDMA_TIME_PTR( ss_ptr ) (&(ss_ptr->info.mode_info.cdma_info.time_info))
   /* Pointer to cdma time info structure obtained from SS object pointer
   */
/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/* Enumeration of serving system info update action
*/
typedef enum cmss_update_act_e {

  CMSS_INFO_UPDATE_ACT_NONE=-1,            /* FOR INTERNAL USE OF CM ONLY! */

  CMSS_INFO_UPDATE_N_TO_D,
  CMSS_INFO_UPDATE_A_TO_D,
  CMSS_INFO_UPDATE_N_TO_A,
  CMSS_INFO_UPDATE_D_TO_A,

  CMSS_INFO_UPDATE_A_TO_A,
  CMSS_INFO_UPDATE_D_TO_D,

  CMSS_INFO_UPDATE_A_TO_N,
  CMSS_INFO_UPDATE_D_TO_N,

  CMSS_INFO_UPDATE_ACT_MAX                 /* FOR INTERNAL USE OF CM ONLY! */

} cmss_update_act_e_type;


/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/


/* Throttle table entry type
*/

typedef struct cmss_orig_thr_table_entry_s
{

  cm_ss_orig_thr_call_status_s_type     orig_call_info;
    /* The call information
    */

  dword                                 thr_uptime;
    /* The time until throttling is in effect
    */

  unsigned int                          idx;
    /* The index in the throttle_time table
    ** Used internally by CM to keep track of the next throttle time 
    ** increment
    */
}cmss_orig_thr_table_entry_s_type;


typedef struct cmss_orig_thr_table_s
{
  unsigned int   num_valid_entries;
    /* Number of valid entries in the table
    */

  cmss_orig_thr_table_entry_s_type 
                 cmss_orig_thr_tbl[CMSS_ORIG_THR_TABLE_MAX_ENTRIES];
    /* The origination throttle table
    */

}cmss_orig_thr_table_s_type;



/* Serving System type - holds all the information that is associated
** with a Serving System.
*/
typedef struct cmss_s  {

  /* Private Serving System Information
  */

  cm_init_mask                init_mask;
    /* Indicate whether this object was properly initialized 
    */

  uint8                       bit_err_rate;
    /* Bit Error Rate / RX QUAL in GSM 
    */

  uint16                      gw_rssi;
    /* Indicate the GSM/WCDMA RSSI value. Only valide for GW mode.
    ** Updated when there is RSSI report from GW stack.
    */
 
  int16                       gw_ecio;
    /* Indicate the WCDMA ecio value. Only valide for WCDMA mode.
    ** Updated when there is ECIO report from GW stack.
    */

  int16                       gw_pathloss; 
    /* Pathloss is applicable only in Connected mode (otherwise 0xFF) 
    */
  
  int16                       gw_sir; 
    /* SIR applicable and valid (otherwise 0xFF) only in WCDMA 
    ** connected mode when DPCCH is active
    */


  int16                       gw_rscp;  
    /* Signal strength in RSSI, change for WCDMA only (otherwise 0xFF) 
    */

  boolean                     gw_ecio_for_display;
    /* TRUE if UE is in a data call, else FALSE 
    */

  dword                       no_srv_uptime;
    /* Indicate the uptime where the service state should be set to
    ** CM_SRV_STATE_NO_SRV 
    */

  dword                       main_srv_inform_uptime;
    /*  Indicates the time interval before clients are informed about
    **  change in main system parameters when service is present 
    */

  dword                       hdr_srv_inform_uptime;
    /*  Indicates the time interval before clients are informed about
    **  change in hdr system parameters when service is present 
    */

  boolean                     is_pref_srv_acq;
    /* Indicate whether phone is currently attempting to acquire a more
    ** preferred service. NOTE! This field is undefined when sys_mode =
    ** CM_SYS_MODE_NO_SRV 
    */

  boolean                     is_redir_or_ho;
    /* Indicate whether redirection or handoff indication is currently
    ** ON 
    */

  boolean                     is_colloc;
    /* Indicate whether a collocated system is available 
    */

  #if (defined (FEATURE_HDR_HYBRID) || defined( FEATURE_HYBR_GW ))
#error code not present
  #endif /* FEATURE_HDR_HYBRID */

  sd_sys_usable_e_type        hybr_hdr_usable;
    /* Indicates if the system in SS-HDR SD's instance is usable or not.
    */

  sd_sys_usable_e_type        ss_main_usable;
    /* Indicates if the system in SS-MAIN SD's instance is usable or not.
    */

  cm_mm_ss_info_s_type        srv_avail_info;
    /* Copies system parameters when CM_SS_EVENT_SRV_CHANGED
    ** event is sent to clients about main srv being available.
    ** Used to compare with system parameters after main_srv_inform_uptime
    ** expires. Used for both MAIN and HDR 
    */

  /* Public Serving System Information
  */

  cm_mm_ss_info_s_type        info;
    /* Contains serving system information that is shared 
    ** with the clients.  
    */

  #ifdef FEATURE_PS_DORMANT_PWR_SAVE
  cm_ps_data_stt_e_type       ps_data_stt;
    /* Indicate the state when phone is attempting to resume data. 
    */

  dword                       no_ps_data_uptime;
    /* Indicate the uptime when data resume is time out 
    */
  #endif /* FEATURE_PS_DORMANT_PWR_SAVE */

  dword                       wlan_no_srv_uptime;
    /* Indicate the uptime where the service state should be set to
    ** CM_SRV_STATE_NO_SRV 
    */

  dword                       wlan_srv_inform_uptime;
    /*  Indicates the time interval before clients are informed about
    **  change in wlan system parameters when service is present 
    */

  boolean                     wlan_is_pref_srv_acq;
    /* Indicate whether phone is currently attempting to acquire a more
    ** preferred service. NOTE! This field is undefined when srv_state =
    ** CM_SRV_STATE_NO_SRV 
    */

  cmss_orig_thr_table_s_type *cmss_orig_thr_tbl_ptr;
    /* Origination throttle table pointer
    */

  #ifdef FEATURE_CM_TECH_SPEC_SRV_IND
  boolean                     gw_is_pref_srv_acq;
  #endif
  
  boolean                     hybr_1_is_pref_srv_acq;
  
  sys_srv_status_e_type          main_srv_status;
    /* Main service status
    */

  sys_srv_status_e_type          hybr_1_srv_status;
    /* Hybr 1 service status
  */
   sys_sys_mode_e_type         last_system_reported;
    /*
    **  Stores the last sys_mode reported to clients.
    **  This variable shouldnt be assigned NO_SRV (except in cmss_reset).
    **  If NO_SRV is reported, this variable holds the system 
    **  that was last succesfully acquired before NO_SRV was reported.
    **  Helps keep DS in sync.
    */

  boolean                     is_hdr_session_open;
    /**<
    ** Stores whether a HDR session is open or not. A dormant
    ** session is considered as an open session.
    */

  int8                        ltm_offset;
    /* LTM offset for current CDMA system.
    ** COPY TEXT FROM SPEC. IT IS A 2s compliment
    ** value. It is the signed version of the value obtained from
    ** the CDMA sync channel message. Used for HPCD
    */

  sys_daylt_savings_e_type     daylt_savings;
    /* Day light savings for current CDMA
    ** system. Used for HPCD
    */

  sys_cell_info_s_type        gw_cell_info;
    /*  Indicates the cell_info value. Only valid for GW mode
    */

  boolean                     is_connected_mode_oos;
    /* Flag to store the current OOS status updated when 
    ** RRC send CM_CONN_MODE_SERVICE_STATUS_IND to CM
    ** Flag = TRUE means Currently in OOS mode
    ** Flag = FALSE means FACH OOS state is left
    */

} cmss_s_type;

/* User sid to country mapping
*/
typedef struct cmss_sid_mcc_assoc_s {

  cm_sid_type           sid;
    /* sid of country */

  cm_country_code_type  country_code;
    /* Country code corresponding to above sid */
} cmss_sid_mcc_assoc_s_type;


/* Table that holds user generated
** sid to mcc mapping
*/
typedef struct cmss_sid_mcc_assoc_tbl {

  uint8                        sid_mcc_tbl_len;
    /* Size of sid to mcc table length */

  cmss_sid_mcc_assoc_s_type    sid_mcc_tbl [CMSS_SID_MCC_TBL_MAX];
    /* Sid to mcc table */

} cmss_sid_mcc_assoc_tbl_s_type ;



/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmss_ptr

DESCRIPTION
  Return a pointer to the one and only SS object.

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
extern cmss_s_type  *cmss_ptr
(
  void
);



/*===========================================================================

FUNCTION cmss_init

DESCRIPTION
  Initializes the serving system object.

  This function must be called before the serving system object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmss_init
(
  void
);


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

FUNCTION cmss_mc_rpt_proc

DESCRIPTION
  Process MC reports


DEPENDENCIES
  Serving system object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmss_mc_rpt_proc
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */



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
extern void cmss_sd_rpt_proc
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
);



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
extern void cmss_client_cmd_proc
(
  cm_cmd_s_type  *cmd_ptr
    /* pointer to a CM command */
);



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
extern void cmss_timer_proc
(
  cm_timer_event_type    timer_event
    /* Indicate specific timer event.
    **
    ** Note that currently timer event is not being used */
    /*lint -esym(715,timer_event) */
);


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================
FUNCTION cmss_rssi_timer_handle

DESCRIPTION
  This function check the rssi. If RSSI change exceeds the delta threshold,
  CM reports rssi change event to registered clients.

DEPENDENCIES
  Serving System object must be initialized

RETURNS
  none
SIDE EFFECTS
  none

===========================================================================*/
extern void cmss_rssi_timer_handle
(
  void
);
#endif /* FEATURE_CDMA_800, FEATURE_CDMA_1900 */




/*===========================================================================
FUNCTION cmss_send_idle_digital_mode_event

DESCRIPTION
  This function sends the idle digital mode event

DEPENDENCIES
  Serving System object must be initialized

RETURNS
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmss_send_idle_digital_mode_event
(
  void
);




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
extern sys_sys_mode_e_type   cmss_map_sd_mode_to_cm_mode(

       sd_mode_e_type        sd_mode
          /* cm system mode.
          */
);



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
================================================================n===========*/
extern  void                           cmss_proc_hdr_reselect
(
        void
);


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
extern  void                           cmss_proc_call_ended
(
  void
);


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
extern  boolean                           cmss_is_cdma_svd_supported
(
        void
);


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
extern  boolean                           cmss_send_mm_data_avail
(
        void
);

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
extern boolean cmss_sys_id_match(

  sys_sys_id_s_type     sys_id_1,

  sys_sys_id_s_type     sys_id_2
);

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
uint64 cmss_get_ss_wlan_info_change
(

    const cm_mm_ss_info_s_type     *new_srv_info,
        /*  New SS information */

    const cm_mm_ss_info_s_type     *old_srv_info
        /*  Old SS information */
);


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
void cmss_update_wlan_srv_ind
(

    cmss_s_type         *ss_ptr,
        /* Pointer to a serving system object */

    const sd_si_info_s_type   *si_info_ptr
       /* Pointer to buffer containing new service info information */
);

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
void cmss_report_wlan_stats
(
  void
);

/*===========================================================================
@FUNCTION cmss_report_wlan_link_quality

@DESCRIPTION
  This function gets the WLAN link quality, And reports change event to registered clients.

@DEPENDENCIES
  Serving System and Phone objects must be initialized

@RETURNS
  none

@SIDE EFFECTS
  none

@x
===========================================================================*/
void cmss_report_wlan_link_quality
(
   void
);

/*===========================================================================

FUNCTION cmss_rel_cipher_ind

DESCRIPTION
  This function removes the ciphering indication for a particular domain if
  it is active and informs clients.

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmss_rel_cipher_ind
(
  sys_srv_domain_e_type cipher_domain
    /* Domain of ciphering to be removed
    */
);

#ifdef  FEATURE_CALL_THROTTLE

/*===========================================================================

FUNCTION cmss_check_if_orig_thr_true

DESCRIPTION
  This function checks is the origination needs to be throttled

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern boolean cmss_check_if_orig_thr_true
(
  sys_sys_id_s_type   sys_id,
    /* The sys id of the system, SID,NID
    */

   sys_band_class_e_type band_class,
    /* The band class information
    */

  cmss_orig_thr_table_s_type *thr_table_ptr,
    /* Origination throttle table pointer
    */
  cm_call_type_e_type         call_type,
     /* Call type - VOICE, SMS, E911, etc
     */

  uint16                      packet_zone
    /* The packet zone id
    */

);


/*===========================================================================

FUNCTION cmss_orig_throttle_update_table_with_call_status

DESCRIPTION
  This function should be called to update the origination throttle table
  with the call status
  
DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern boolean cmss_orig_thr_update_tbl_entry_with_call_status
(
  cmss_orig_thr_table_s_type *throttle_table_ptr,
    /* Pointer to the origination throttle table
    */
  cm_ss_orig_thr_call_status_s_type *throttle_call_status
    /* Information about the current call status
    */
);

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
    /* Is the HDR instance throttled
    */
);
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

);

#ifdef  FEATURE_CALL_THROTTLE
/*===========================================================================

FUNCTION cmss_misc_set_ss_info_thr_status

DESCRIPTION
  This function sets the ss_thr_status of the ss object

DEPENDENCIES
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void cmss_misc_set_ss_info_thr_status( boolean new_thr_status );

#endif  /* FEATURE_CALL_THROTTLE */

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
);

#ifdef FEATURE_IMS_VCC
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
extern void cmss_wlan_link_qual_ind_cb(     

   wlan_cp_link_qual_ind_enum_type  ind,
   /*< WLAN Link quality */

   void                             *user_data_ptr
   /*< Pointer to the used data*/
);
#endif /*FEATURE_IMS_VCC*/

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
extern void cmss_send_ss_info(

  const cm_ss_cmd_s_type     *ss_cmd_ptr,
      /* The pointer to a serving system command */

        cm_ss_event_e_type    event_type
      /* The type of event to be sent to client */
);


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
extern  void  cmss_proc_srv_lost
(
        sd_ss_e_type            ss
);

#endif /* CMSS_H */

