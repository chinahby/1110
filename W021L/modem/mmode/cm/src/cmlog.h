#ifndef CM_LOG_H
#define CM_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            CM Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging CM packets.

Copyright (c) 2002  - 2009 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmlog.h#6 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
05/27/09   aj      Adding support for updating clients with cdma local time
05/27/09   aj      Adding support for passing otasp status to clients
05/15/09   sg      Update SS EVENT log packet with new SS events added in CM
02/02/09   ks      Adding support for QDSP 6 based compiler
01/29/09   fj      Remove FEATURE_HDR_HYBRID for call events
03/17/08   am      Added two diag commands to support 
                   commands for CM_CALL_CMD_MODIFY_REQ / RSP
01/03/08   ka      Adding handset based plus code dialing feature.
04/15/07   rp       Removing FEATURE_MMGPS
11/17/06   jqi     Added support for HDR active protocol revision information.
11/09/06   rp      Added support for SS req/response packet
                   CM_GET_CELL_SERVICE_IND.
04/19/06   jqi     Added diag subsystem command(select system preference 
                   command2) to process 64-bit band preference. 
03/10/06   pk       Added support for IMSI and MCC for IS 95 systems. Changed
                    the changed fields of the ss log packet to 64 bits
01/05/06   pk       Modified the SS log packet structure. Replaced constants
                    with hard coded values, repalced boolean with uint8
12/10/05   ic       Lint cleanup
11/21/05   ic       Send EVENT_CM_OPERATIONAL_MODE on transition from 
                    SYS_OPRT_MODE_OFFLINE_CDMA to SYS_OPRT_MODE_RESET.
                    Fix the functionality that postpones sending of 
                    EVENT_CM_OPERATIONAL_MODE for SYS_OPRT_MODE_OFFLINE_CDMA 
                    until after TASK_OFFLINE_SIG from TMC comes (CR73050).
10/27/05   pk       Added support for SS log packet and its data structures
06/21/05   ic       Renamed DIAG_MAX_GSM_DIGITS to CM_DIAG_MAX_GSM_DIGITS  
06/10/05   sk       Moved user defined data types from cmlog.c to here.
01/24/05   ic       Hardcoded array sizes in declarations of phone and call 
                    event log packets
01/14/05   ic       Added comments about contents of num field in 
                    LOG_CM_CALL_EVENT_C_type declaration
01/06/05   ic       Mainlined FEATURE_CM_CALL_EVENT_LOGGING and 
                    FEATURE_CM_PH_EVENT_LOGGING. 
01/05/05   ic       Log packet version for call and phone event packets
12/20/04   ic       In LOG_CM_PH_EVENT_C_type and LOG_CM_CALL_EVENT_C_type
                    declarations changed all int to int32 
12/19/04   ic       Added support for sending LOG_CM_CALL_EVENT_C under
                    FEATURE_CM_CALL_EVENT_LOGGING
12/10/04   ic       Refinments for CR: 48569 - HDR-KDDI: Request for new EVENTs 
                    for DDTM mode (LOG_CM_PH_EVENT_C)
12/06/04   ic       Include log.h instead of log_dmss.h to correct RVCT 
                    compiler error on 6250 ("could not find log_dmss.h")
12/05/04   ic       Put definition of LOG_CM_PH_EVENT_C_type under 
                    FEATURE_CM_PH_EVENT_LOGGING
12/05/04   ic       Added support for sending LOG_CM_PH_EVENT_C  
08/10/04   ic       Removed cmlog_get_operating_mode() and cmlog_system_mode()
                    as they became obsolete. 
03/18/04   ka       Prototype for cmlog_set_cm_offline_signal_recvd ()
02/06/04   ws       Initial jaguar/mainline merge.
05/21/03   vt       Added new call events.
03/21/03   ATM      Added event logging for svc_cnf and nw_list_cnf
02/12/03   atm      Added pref logging and moved subsys command from UMTS to
                    CM.
01/21/03   ws       Updated copyright information for 2003
04/26/02   atm      Initial version.
===========================================================================*/

#include "cm.h"
#include "comdef.h" 
#include "sys.h"    
#include "sys_plmn_selection.h"    
#include "diagpkt.h"        /* for subsystem commands      */
#include "cmcall.h"
#include "log.h" 

#define CM_LOG_PACKET_CONTENTS_NOT_DEFINED   0
/* 
** Filler for log packet fields when there is
** nothing meaningful to write into them
*/

#define CM_LOG_PH_EVENT_LOG_PACKET_VERSION   2
/* 
** Version number for LOG_CM_PH_EVENT_C_type
** New fields are to be added at the end of the structure only
** (DIAG and tools requirement).
** Whenever a new field is added to the structure, the value of
** the constant below needs to be increased by 1 and
** DIAG and tools group need to be notified about the changes. 
*/

#define CM_LOG_CALL_EVENT_LOG_PACKET_VERSION 2
/* 
** Version number for LOG_CM_CALL_EVENT_C_type
** New fields are to be added at the end of the structure only
** (DIAG and tools requirement).
** Whenever a new field is added to the structure, the value of
** the constant below needs to be increased by 1 and
** DIAG and tools group need to be notified about the changes. 
*/

#define CM_LOG_SS_EVENT_LOG_PACKET_VERSION 7
/* 
** Version number for LOG_CM_SS_EVENT_C_type
** New fields are to be added at the end of the structure only
** (DIAG and tools requirement).
** Whenever a new field is added to the structure, the value of
** the constant below needs to be increased by 1 and
** DIAG and tools group need to be notified about the changes. 
*/



/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/*
** Data structure used to log CM phone events
** We log current PH event along with what's in cm_ph_info_s_type
*/
LOG_RECORD_DEFINE(LOG_CM_PH_EVENT_C)

  uint8                  version;
    /* Version of the packet (CM_LOG_PH_EVENT_LOG_PACKET_VERSION)
    */ 

  int32                  ph_event;
    /* PH event, as defined by cm_ph_event_e_type
    */

  boolean                is_in_use;
    /* Indicate whether phone is currently in use 
    */

  int32                  oprt_mode;
    /* Indicate current operating mode (sys_oprt_mode_e_type) 
    */

  int32                  test_control_type;
    /* Indicate current test control type (cm_test_control_type_e_type) 
        */

  int32                  cdma_lock_mode;
    /* 
    ** Indicate whether phone is CDMA locked until power cycle 
    ** (cm_cdma_lock_mode_e_type) 
    */

  int32                  answer_voice;
    /* 
    ** Indicate whether phone should answer incoming
    ** voice calls as voice, fax or modem. CDMA only (cm_answer_voice_e_type) 
    */

  int32                  mode_pref;
    /* Indicate current mode preference (cm_mode_pref_e_type) 
    */

  int32                  pref_term;
    /* 
    ** Indicate the current preference term - one-call, power-cycle, etc.
    ** (cm_pref_term_e_type)
    */

  int32                  acq_order_pref;
    /* 
    ** The acquisition order preferred by the client. GSM/WCDMA only. 
    ** (cm_gw_acq_order_pref_e_type) 
    */

  int32                  srv_domain_pref;
    /* 
    ** The service domain preferred by the client.
    ** This field is set after a call to cm_ph_cmd_sys_sel_pref.
    ** GSM/WCDMA only (cm_srv_domain_pref_e_type). 
    */

  int64                  band_pref;
    /* Indicate current band preference (cm_band_pref_e_type) 
    */

  int32                  roam_pref;
    /* Indicate current roam preference (cm_roam_pref_e_type) 
    */

  int32                  nam_sel;
    /* Indicate the NAM selection. CDMA only (cm_nam_e_type) 
    */

  int32                  curr_nam;
    /* 
    ** Indicate the current NAM -
    ** needed when NAM selection is set to AUTO-NAM. CDMA only 
    ** (cm_nam_e_type)
    */

  int32                  packet_state;
    /* Indicate current packet state. CDMA only (cm_packet_state_e_type) 
    */

  int32                  hybr_pref;
    /* Indicate hybrid preference (cm_hybr_pref_e_type) 
    */

  int32                  network_sel_mode_pref;
    /* 
    ** The network selection mode selected by the client. GSM/WCDMA only 
    ** (cm_network_sel_mode_pref_e_type)
    */
    
  PACKED struct PACKED_POST
  {
    byte                 identity[3];
  } plmn;
    /* 
    ** The identifier of the PLMN to be selected manually. GSM/WCDMA only
    ** (sys_plmn_id_s_type) 
    */

  PACKED struct PACKED_POST
  {
    byte                 length;
      /* The number of PLMNs in the list below 
      */
    
    PACKED struct PACKED_POST
    {
        
      PACKED struct PACKED_POST
      {
        byte             identity[3];
      } plmn;
        /* (sys_plmn_id_s_type) 
        */ 
            
      uint16             access_tech;
        /* sys_access_tech_e_type
        */
            
    } info[ 50 ];
      /* 
      ** (sys_user_pref_plmn_list_info_s_type) 
      ** Hardcoded to the value of SYS_USER_PPLMN_LIST_MAX_LENGTH
      ** at the time log packet was declared. 
      ** This is intentional as otherwise changes to the constant would
      ** change byte ordering in the log packet. 
      */
        
  } user_pref_networks;
    /* 
    ** The user preferred network list. GSM/WCDMA only 
    ** (sys_user_pref_plmn_list_s_type)
    */

  PACKED struct PACKED_POST
  {
    
    uint32               length;
    /* The number of PLMNs in the list 
    */

    PACKED struct PACKED_POST
    {
     
      PACKED struct PACKED_POST
      {
        byte             identity[3];
      } plmn;
        /* The ID of the PLMN (sys_plmn_id_s_type)                                        
        */

      int32              rat;
        /* The radio access technology of the PLMN (sys_radio_access_tech_e_type)                   
        */

      int32              plmn_service_capability;
        /* 
        ** Indicates the type of service domain the PLMN can provide 
        ** (sys_srv_capability_e_type)
        */

      boolean            plmn_forbidden;
        /* Indicates if the PLMN is forbidden                        
        */

      int32              list_category;
        /* Indicates the type of PLMN (sys_detailed_plmn_list_category_e_type)                               
        */

      int32              signal_quality;
        /* The signal quality of the PLMN (sys_signal_quality_e_type)                            
        */

      int32              signal_strength;
        /* The signal strength of the PLMN (sys_signal_strength_type)                        
        */

    } info[ 40 ]; 
      /* 
      ** (sys_detailed_plmn_list_info_s_type)
      ** Hardcoded to the value of SYS_PLMN_LIST_MAX_LENGTH
      ** at the time log packet was declared. 
      ** This is intentional as otherwise changes to the constant would
      ** change byte ordering in the log packet.      
      */
        
  } available_networks;
    /* Available networks. GSM/WCDMA only (sys_detailed_plmn_list_s_type)
    */

  boolean                is_cdma_subscription_available;
    /* 
    ** Indicate whether the cdma subscription/provisioning information
    ** is available for consumption.                              
    */

  boolean                is_gw_subscription_available;
    /* 
    ** Indicate whether the GSM/WCDMA subscription/provisioning information
    ** is available for consumption.                              
    */

  int32                  rtre_config;
    /* 
    ** Current RTRE configuration - RUIM, NV, RUIM fallback NV. CDMA only 
    ** (cm_rtre_config_e_type)
    */

  int32                  rtre_control;
    /* Current RTRE control - RUIM or NV. CDMA only (cm_rtre_control_e_type)
    */

  int32                  orig_srv_domain_pref;
    /* 
    ** service domain preference for PS attach/detach commands.
    ** This is different from the phone service domain preference
    ** (cm_srv_domain_pref_e_type)
    */

  int32                  ddtm_pref;
    /* Data Dedicated Tranmission mode (DDTM) preference (cm_ddtm_pref_e_type).
    */

  boolean                cur_ddtm_status;
    /* ddtm status 
    */


  int32                  ddtm_act_mask;
    /* DDTM action mask settings (sys_ddtm_act_mask_e_type).
    */

  uint16                 ddtm_num_srv_opt;
    /* Number of service options specified in below list.
    */

  uint16                 ddtm_srv_opt_list[ 10 ];
    /* 
    ** List of service options whose pages have to be ignored provided
    ** ddtm_pref & ddtm_act_mask allows it (sys_srv_opt_type).
    ** Hardcoded to the value of SYS_DDTM_MAX_SO_LIST_SIZE
    ** at the time log packet was declared. 
    ** This is intentional as otherwise changes to the constant would
    ** change byte ordering in the log packet.    
    */

  int32                  cphs_allowed;
    /*
    ** Whether Common PCN Handset Specifications procedures are allowed
    ** FEATURE_ALS (cm_cphs_proc_e_type)
    */

  int32                  als_allowed;
    /*
    ** Whether ALS procedures are allowed.
    ** FEATURE_ALS (cm_als_proc_e_type)
    */

  int32                  line;
    /*
    ** Currently selected line (all MO voice calls will use it)
    ** FEATURE_ALS (cm_als_line_e_type)
    */

  int32                  line_switching;
    /*
    ** Whether the user is allowed to switch between lines
    ** FEATURE_ALS (cm_als_line_switching_e_type)
    */

  int32                  requesting_client_id;
    /* Requesting client (cm_client_id_type) 
    */

  PACKED struct PACKED_POST {

    uint8                type;
      /* Message type.
      */

    uint32               payload_len;
      /* Message payload length in bits max is SYS_MAX_IS2000_MSG_SIZE * 8
      */

    byte                 payload[ 256 ];
      /* Message payload.
      */
  } cdma_capability;
    /* 
    ** CDMA capability in the form of IS2000 Status Message 
    ** (sys_is2000_msg_s_type)
    ** FEATURE_UMTS_1X_HANDOVER_1XMSM || FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    ** otherwise filled with 0s.
    ** Hardcoded to the value of SYS_MAX_IS2000_MSG_SIZE
    ** at the time log packet was declared. 
    ** This is intentional as otherwise changes to the constant would
    ** change byte ordering in the log packet.      
    */

  int32                  msm_id;
    /* Identifies the MSM which generated this event (sys_msm_id_e_type).
    ** FEATURE_UMTS_1X_HANDOVER_1XMSM || FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    ** otherwise filled with 0s
    */


  int32                  orig_ddtm_act_mask;
    /* DDTM action mask settings (sys_ddtm_act_mask_e_type).
    */

LOG_RECORD_END

/*
** Data structure used to log CM call events
** We'll log current CM call event and what's in cm_mm_call_info_s_type
** (except for mode_info field)
*/
LOG_RECORD_DEFINE(LOG_CM_CALL_EVENT_C)
  
  uint8                  version;
    /* Version of the packet (CM_LOG_CALL_EVENT_LOG_PACKET_VERSION)
    */ 

  int32                  call_event;
    /* Call event, as defined by cm_call_event_e_type
    */

  int32                  call_client_id;
    /* Call client - originating/answering client (cm_client_id_type)
    */

  int32                  call_id;
    /* call ID of the call (cm_call_id_type) 
    */

  int32                  call_state;
    /* Call state - idle, orig, incom, conv (cm_call_state_e_type)
    */

  int32                  call_type;
    /* Call type - VOICE, DATA, OTASP, etc (cm_call_type_e_type) 
    */

  PACKED struct PACKED_POST
  {

    uint8                buf[ 64 ];
      /* Number buffer
      ** Hardcoded to the value of CM_MAX_NUMBER_CHARS
      ** at the time log packet was declared. 
      ** This is intentional as otherwise changes to the constant would
      ** change byte ordering in the log packet.           
      */

    uint8                len;
      /* Length of number stored in num_buf 
      */

    int32                digit_mode;
      /* Origination digit mode (cm_digit_mode_e_type)
      */

    uint8                number_type;
      /* Number type 
      */

    uint8                number_plan;
      /* Numbering Plan 
      */

    uint8                pi;
      /* Presentation Indicator (cm_npi_type)
      */

    uint8                si;
      /* Screening Indicator (cm_si_type) 
      */

    PACKED struct PACKED_POST
    {
      boolean            extension_bit;
        /* The extension bit 
        */

      uint8              subadd_type;
        /* Type of subaddress - NSAP/USER 
        */

      uint8              odd_even_ind;
        /* The indicator of even/odd bits - EVEN/ODD 
        */

      uint8              num_char;
        /* Number of characters in subaddress array 
        */

      uint8              chari[ 21 ];
        /* 
        ** Array of subaddress digits 
        ** Hardcoded to the value of CM_MAX_SUBADDRESS_DIGITS
        ** at the time log packet was declared. 
        ** This is intentional as otherwise changes to the constant would
        ** change byte ordering in the log packet.          
        */

    } subaddr;
      /* Subaddress (cm_subaddress_s_type) 
      */

  } num;
    /* 
    ** Number buffer (cm_num_s_type)
    ** 
    ** Contents: 
    **
    ** CM_CALL_EVENT_ORIG      - contains dialed digits string.
    ** CM_CALL_EVENT_FLASH     - contains dialed digits string.
    ** CM_CALL_EVENT_INCOM     - contains caller ID string.
    ** CM_CALL_EVENT_CALLER_ID - contains caller ID string. 
    */

  PACKED struct PACKED_POST
  {

    PACKED struct PACKED_POST
    {

      uint8              buf[ 64 ];
        /* Number buffer 
        ** Hardcoded to the value of CM_MAX_NUMBER_CHARS
        ** at the time log packet was declared. 
        ** This is intentional as otherwise changes to the constant would
        ** change byte ordering in the log packet.          
        */

      uint8              len;
        /* Length of number stored in num_buf 
        */

      int32              digit_mode;
       /* Origination digit mode (cm_digit_mode_e_type) 
       */

      uint8              number_type;
        /* Number type 
        */

      uint8              number_plan;
        /* Numbering Plan 
        */

      uint8              pi;
        /* Presentation Indicator (cm_npi_type)
        */

      uint8              si;
        /* Screening Indicator (cm_si_type) 
        */

      PACKED struct PACKED_POST
      {
        boolean          extension_bit;
          /* The extension bit 
          */

        uint8            subadd_type;
          /* Type of subaddress - NSAP/USER 
          */

        uint8            odd_even_ind;
          /* The indicator of even/odd bits - EVEN/ODD 
          */

        uint8            num_char;
          /* Number of characters in subaddress array 
          */

        uint8            chari[ 21 ];
          /* 
          ** Array of subaddress digits 
          ** Hardcoded to the value of CM_MAX_SUBADDRESS_DIGITS
          ** at the time log packet was declared. 
          ** This is intentional as otherwise changes to the constant would
          ** change byte ordering in the log packet.              
          */

      }  subaddr;
        /* Subaddress (cm_subaddress_s_type)
        */

    } num;
      /* Number that is redirecting (cm_num_s_type) 
      */

    int32                redirecting_reason;
      /* 
      ** Optional param -- redirecting reason
      ** REDIRECTING_REASON_UNKNOWN if no included 
      ** (cm_redirecting_reason_e_type).
      */

  } redirecting_number;
    /* Number buffer (cm_redirecting_num_s_type)
    */

  PACKED struct PACKED_POST
  {

    uint8                buf[ 64 ];
      /* 
      ** Alpha buffer 
      ** Hardcoded to the value of CM_MAX_ALPHA_TAG_CHARS
      ** at the time log packet was declared. 
      ** This is intentional as otherwise changes to the constant would
      ** change byte ordering in the log packet.          
      */

    uint8                len;
      /* Length of alpha tag stored in alpha_buf 
      */

  } alpha;
    /* 
    ** Alpha tag to be displayed along with dialed address
    ** of an originated call (cm_alpha_s_type).
    */

  PACKED struct PACKED_POST
  {

    boolean              is_signal_info_avail;
      /* Indicate whether signal information is available.
      **
      ** NOTE! if this variable is FALSE, the values found in other
      ** will be set to CM_LOG_PACKET_CONTENTS_NOT_DEFINED 
      */

    uint8                signal_type;
      /* Signal type - Tone Signal, ISDN Alerting or IS-54B Alerting 
      */

    uint8                alert_pitch;
      /* Alert pitch - Medium, High or Low 
      */

    uint8                signal;
      /* Indicate which Tone signals, ISDN Alerting or IS_54B Alerting 
      */

  } signal;
    /* 
    ** Alert signal information to be played for the incoming call 
    ** (cm_signal_s_type)
    */

  int32                  end_status;
    /* 
    ** Call end status (i.e. reason for ending a call in origination,
    ** incoming or conversation state) 
    ** (cm_call_end_e_type)
    */

  uint8                  num_call_ids_ended;
    /* number of calls ended 
    */

  PACKED struct PACKED_POST
  {

    uint8                call_id;
      /* Call ID to end (cm_call_id_type)
      */

    int32                info_type;
      /* Mode type (cm_call_mode_info_e_type) 
      */

    PACKED union PACKED_POST
    {

      PACKED struct PACKED_POST
      {

        uint8            reserved;
          /* empty for now 
          */

      } gw_cs_end;
        /* 
        ** GSM/WCDMA circuit switched call end parameters 
        ** (cm_gw_cs_end_params_s_type)
        */

      PACKED struct PACKED_POST
      {

        PACKED struct PACKED_POST
        {
          boolean        valid;
          byte           nsapi;
        } nsapi;
          /* NSAPI (nsapi_T) 
          */

        PACKED struct PACKED_POST
        {
          boolean        valid;
          boolean        ti_flag;
        } tear_down_ind;
          /* 
          ** Tear down indicator (ti_T)              
          */

        uint8            cause;
          /* 
          ** Cause for ending the call (sm_status_T)
          */

      } gw_ps_end;
        /* 
        ** GSM/WCDMA packet switched call end parameters 
        ** (cm_gw_ps_end_params_s_type)
        */

      PACKED struct PACKED_POST
      {
        boolean          end_reason_included;
          /* End reason included or not 
          */

        uint8            end_reason;
          /* End reson 
          */

      } cdma_end;
        /* CDMA call end parameters (cm_cdma_end_params_s_type)
        */

    } end_params;
      /* mode specific end parameters (cm_end_params_u_type) 
      */

  } end_params;
    /* call end information (cm_end_params_s_type) 
    */

  int32                  sys_mode;
    /* Indicates the system on which the call is made (sys_sys_mode_e_type) 
    */

  dword                  call_connect_time;
    /* The time the call was connected.  The units is seconds since
    ** the phone has powered on (clock uptime).
    **/

  dword                  call_end_time;
    /* The time the call was ended.  The units is seconds since
    ** the phone has powered on (clock uptime).
    **/

  uint32                 bytes_received;
    /* The number of bytes received during the call (PS data call only).
    ** This field is populated after the call has ended.
    */

  uint32                 bytes_transmitted;
    /* The number of bytes transmitted during the call (PS data call only).
    ** This field is populated after the call has ended.
    */


  int32                  line;
    /* What line is being used for this call (FEATURE_ALS)
    ** (cm_als_line_e_type)
    */

  int32                  requesting_client_id;
    /* Requesting client (cm_client_id_type)
    */

  int32                  msm_id;
    /* 
    ** Identifies the MSM which generated this event.
    ** Only set when FEATURE_UMTS_1X_HANDOVER_1XMSM or
    ** FEATURE_UMTS_1X_HANDOVER_UMTSMSM is defined
    ** otherwise set to 0
    ** (sys_msm_id_e_type)
    */

  int32                 direction;
    /* Holds direction of the current call.
    ** It can be Mobile originated or Mobile
    ** Terminated
    ** (cm_call_direction_e_type)
    */

  int32                 otasp_status;
    /* 
    ** OTASP status passed from 1xCP to CM,
    ** during OTASP/OTAPA call 
    ** (cm_otasp_status_e_type)
    */

LOG_RECORD_END


/*
** Data structure used to log CM SS events
** We log current SS event along with what's in cm_ss_info_s_type
*/
LOG_RECORD_DEFINE(LOG_CM_SS_EVENT_C)

  uint8                      version;
    /* Version of the packet (CM_LOG_SS_EVENT_LOG_PACKET_VERSION)
    */ 

  int32                      ss_event;
    /* SS event, as defined by cm_ss_event_e_type
    */

  uint64                     changed_fields;
    /* Bit mask indicating which of the following serving system fields
    ** were changed since last notification.  Refer to the #defines
    ** (CM_SS_*_MASK) above. */

  int32                      srv_status;
    /* The service status (SERVICE/NO SERVICE) of the system.
    ** Mask used is CM_SS_SRV_STATUS_MASK.
    */

  int32                      srv_domain;
    /* The service domain registered on the system. /
    ** Mask used is CM_SS_SRV_DOMAIN_MASK.
    */

  int32                      srv_capability;
    /* The system's service capability.
    ** Mask used is CM_SS_SRV_CAPABILITY_MASK.
    */

  int32                      sys_mode;
    /* The system's mode.
    ** Mask used is CM_SS_SYS_MODE_MASK.
    */

  int32                      roam_status;
    /* Indicate current roaming status.
    ** Mask used is CM_SS_ROAM_STATUS_MASK.
    */

  PACKED struct PACKED_POST
  {
    int32                    id_type;

    PACKED union PACKED_POST
    {
      PACKED struct PACKED_POST
      {
        uint16               sid;
          /* The system ID 
          */

        uint16               nid;
          /* The network ID 
          */

      } is95;
        /* IS 95 sys id
        */

      byte                   is856[16];
        /* HDR sys id
        */

      PACKED struct PACKED_POST
      {
        byte                 identity[3];

      } plmn;
        /* UMTS (WCDMA/GSM) system ID.
        */

      PACKED struct PACKED_POST
      {
        uint8                len;
          /* Length of the SSID, if == 0, then SSID = broadcast SSID.
          */
        char                 ssid[32];
          /* SSID of the wlan system.
          */

          /* 
          ** ssid 
          ** Hardcoded to the value of SYS_WLAN_SSID_MAX_SIZE
          ** at the time log packet was declared. 
          ** This is intentional as otherwise changes to the constant would
          ** change byte ordering in the log packet.          
          */

      } ssid;
        /* WLAN system Id.
        */

      PACKED struct PACKED_POST
      {
        PACKED struct PACKED_POST
        {
          byte               identity[3];
        } plmn;

        word                 lac;
          /* LAC
          */
      } plmn_lac;
        /* UMTS (WCDMA/GSM) PLMN and LAC ID  
        */
    }id;

  } sys_id;

    /*
    ** Type for system ID.
    */

  uint8                    is_sys_forbidden;
    /* Indicates whether the System is forbidden.
    ** Mask used is CM_SS_SYS_FORBIDDEN_MASK.
    */

  uint8                    is_colloc;
    /* Indicates whether the system is collocated.
    ** Mask used is CM_SS_SYS_ID_MASK.
    */

  uint16                     rssi;
    /* RSSI in positive dBm i.e. 75 means -75dBm. The range for
    ** AMPS:      -110 to -89 dBm
    ** 800 CDMA:  -105 to -90 dBm
    ** 1900 CDMA: -108 to -93 dBm
    ** GSM/WCDMA: -105 to -60 dBm
    */

  int16                      ecio;
  int                        io;
  byte                       sinr;
    /* ECIO in negative 0.5 dBm i.e. 63 means -31.5dBm.
    ** Io field is valid only when in HDR mode only operation
    ** SINR range: from 0 to 8, Level 8 represents highest SINR
    ** SINR applicable only when sys_mode = HDR.
    */

  uint8                    ps_data_suspend;
    /* Indicates that the current PS data service has been suspended.
    ** Mask used is CM_SS_PS_DATA_SUSPEND_MASK.
    */

  uint8                    hdr_hybrid;
    /* system is hybrid or not. FEATURE_HDR_HYBRID */

  int32                      hdr_srv_status;
     /* Indicate current service state - FEATURE_HDR_HYBRID.
     ** Mask used is CM_SS_HDR_SRV_STATUS_MASK.
     ** sys_srv_status_e_type   
     */

  int32                      hdr_roam_status;
     /* Indicate current hybrid hdr roaming status - FEATURE_HDR_HYBRID.
     ** Mask used is CM_SS_HDR_ROAM_STATUS_MASK.
     ** sys_roam_status_e_type 
     */

  uint16                     hdr_rssi;
    /* HDR RSSI in positive dBm i.e. 75 means -75dBm - FEATURE_HDR_HYBRID
    ** The range is -105 to -90 dBm
    */

  int16                      hdr_ecio;
  int                        hdr_io;
  byte                       hdr_sinr;
    /*
    ** HDR Ec/Io and Io values (used when mobile is in
    ** hybrid operation).
    ** Ec/Io range: from 5 ( -31.5 dB ) to 512 ( 0 dB )
    ** Io    range: from -106 dBm to -21 dBm
    ** SINR range: from 0 to 8, Level 8 represents highest SINR
    ** ( FEATURE_HDR )
    */

  int32                      prev_idle_digital_mode;
    /* previous idle digital mode. FEATURE_HDR_HANDOFF.
    ** Mask used is CM_SS_IDLE_DIGITAL_MODE_MASK.
    ** sys_sys_mode_e_type
    */

  int32                      cur_idle_digital_mode;
    /* current idle digital mode. FEATURE_HDR_HANDOFF.
    ** Mask used is CM_SS_IDLE_DIGITAL_MODE_MASK.
    ** sys_sys_mode_e_type
    */

  PACKED union PACKED_POST
  {

    PACKED struct PACKED_POST
    {
      uint8                  bit_err_rate;
        /* Bit Error Rate / RX QUAL in GSM 
        */

      int32                  sim_state;
        /* Current state of the SIM.
        ** Mask used is CM_SS_SIM_STATE_MASK.
        ** sys_sim_state_e_type
        */

      uint8                plmn_forbidden;
        /* Indicates whether the PLMN is forbidden.
        ** Mask used is CM_SS_PLMN_FORBIDDEN_MASK.
        */

      PACKED struct PACKED_POST
      {
        /*
        ** Flags that indicate whether the particular mm information is
        ** available.
        **/
        uint8              plmn_avail;
        uint8              full_name_avail;
        uint8              short_name_avail;
        uint8              univ_time_and_time_zone_avail;
        uint8              time_zone_avail;
        uint8              lsa_identity_avail;
        uint8              daylight_saving_adj_avail;

        PACKED struct PACKED_POST
        {
          byte               identity[3];
        } plmn;

        PACKED struct PACKED_POST
        {
          uint8              length;
            /* Number of characters in the full name 
            */

          int32              coding_scheme;
            /* The coding scheme used to encode the full name of the network 
            ** sys_network_name_coding_scheme_type_e_type   
            */

          uint8            add_country_initials;
            /* Flag that indicates whether MS should add the country initials 
            ** to the full name of the network 
            */

          uint8              name[255];
            /* The full name of the network
            */

            /* 
            ** name 
            ** Hardcoded to the value of SYS_MAX_FULL_NAME_CHARS
            ** at the time log packet was declared. 
            ** This is intentional as otherwise changes to the constant would
            ** change byte ordering in the log packet.          
            */

        } full_name;

        PACKED struct PACKED_POST
        {

          uint8              length;
            /* Number of characters in the full name 
            */

          int32              coding_scheme;
            /* The coding scheme used to encode the full name of the network 
            ** sys_network_name_coding_scheme_type_e_type   
            */

          uint8            add_country_initials;
            /* Flag that indicates whether MS should add the country initials 
            ** to the full name of the network 
            */

          uint8              name[255];
            /* The full name of the network
            */

            /* 
            ** name 
            ** Hardcoded to the value of SYS_MAX_SHORT_NAME_CHARS
            ** at the time log packet was declared. 
            ** This is intentional as otherwise changes to the constant would
            ** change byte ordering in the log packet.          
            */

        } short_name;

        PACKED struct PACKED_POST
        {

           uint8             year;
           uint8             month;
           uint8             day;
           uint8             hour;
           uint8             minute;
           uint8             second;
           int               time_zone;
             /*  sys_time_zone_type
             */

        } univ_time_and_time_zone;

        int                  time_zone;
          /* sys_time_zone_type    
          */
        PACKED struct PACKED_POST
        {
          int32              type;
          /*  sys_lsa_identity_type_e_type
          */

          uint8              identity[3];

        } lsa_identity;

        int32                daylight_saving_adj;
          /* sys_daylight_saving_adj_e_type            
          */

      } mm_information;

        /* Mobility management information, i.e. network name, date,
        ** time and time zone of registered network */

    } gw_info;

        /* GSM/WCDMA SS information 
        */

    PACKED struct PACKED_POST
    {


      int32                  band_class;
        /* cellular or PCS 
        ** sys_band_class_e_type       
        */

      int32                  block_or_system;
        /* PCS frequency block: A to F Cellular System A or B
        ** sys_blksys_e_type  
        */

      uint16                 reg_zone;
        /* 12 bit registration zone 
        */

      uint16                 packet_zone;
        /* 8  bit packet zone, 0xffff means PACKET ZONE NONE.
        ** Mask used is CM_SS_PACKET_ZONE_MASK.
        */

      uint8                  bs_p_rev;
        /* 8  bit base station p_rev.
        ** Mask used is CM_SS_BS_P_REV_MASK.
        */

      uint8                  p_rev_in_use;
        /* 8  bit p_rev in use the lessor of BS p_rev and mobile p_rev.
        ** Mask used is CM_SS_P_REV_IN_USE_MASK.
        */

      uint8                  is_registered;
        /* is mobile registered on the current system */

      uint8                  ccs_supported;
        /* is concurrent services supported.
        */

      int32                  uz_id;
         /* User zone Identification of the selected user zone,
         ** == CM_INVALID_UZID, if no user zone selected. FEATURE_USERZONE.
         ** Mask used is CM_SS_UZ_CHANGED_MASK.
         */

      PACKED struct PACKED_POST
      {
        byte                 uz_name_encoding;

        byte                 uz_name_n_octets;

        byte                 uz_name[32];


         /* 
         ** uz_name 
         ** Hardcoded to the value of SYS_PUZL_MAX_UZ_NAME_LEN
         ** at the time log packet was declared. 
         ** This is intentional as otherwise changes to the constant would
         ** change byte ordering in the log packet.          
         */

      }uz_name;
        /* User zone name of the selected user zone.
        ** Valid only if uz_id != CM_INVALID_UZID. FEATURE_USERZONE.
        ** Mask used is CM_SS_UZ_CHANGED_MASK.
        */

      byte                   srch_win_n;
        /* Neighbour set window search size.
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        */

      int32                  base_lat;
        /* Base station latitude[0.25sec and already sign extended].
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        ** sys_base_lat_type  
        */

      int32                  base_long;
        /* Base station latitude[0.25sec and already sign extended].
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        ** sys_base_long_type
        */

      word                   base_id;
        /* Base station Identification number.
        ** Mask used is CM_SS_BASE_STATION_PARMS_CHGD_MASK.
        ** sys_base_id_type
        */

      PACKED struct
      {
        int32                  sys_mode;
        /* Indicates the system from which the time was obtained (sys_sys_mode_e_type) 
        */
        PACKED union
        {
          PACKED struct
          {
            uint8                lp_sec;
            int8                 daylt_savings;
            uint8                ltm_offset;
          }cdma_time;

          PACKED struct
          {
            uint8                lp_sec;
            int16                ltm_offset;
          }hdr_time;

        }time;

      }time_info;
      /* Time obtained from sync channel / sector parameter msg
      ** Mask used is CM_SS_CDMA_TIME_CHGD_MASK.
      */

    } cdma_info;
        /* CDMA SS information 
        */
  }mode_info;

  /* Mode specific serving system information */

 
  /* FEATURE_BCMCS
  ** These fields specify whether lower layer
  ** supports broadcast/multicast as per
  ** High Rate Broadcast-Multicast Packet data Air
  ** Interface Specification C.S0054 and
  ** BCMCS Framework Draft Document 0.1.3 XP0019
  */

  uint8                    hybr_hdr_bcmcs_srv_supported;
    /* Specify whether HDR is configured to support BCMCS service.
    ** Mask used is CM_SS_HYBR_HDR_BCMCS_SRV_SUPPORTED_MASK.
    */

  uint8                    ss_main_bcmcs_srv_supported;
    /* Specify whether MAIN is configured to support BCMCS service.
    ** Mask used is CM_SS_MAIN_BCMCS_SRV_SUPPORTED_MASK.
    */

  int                        requesting_client_id;
    /* Requesting client */

  int32                      msm_id;
     /* Identifies the MSM which generated this event.
     ** sys_msm_id_e_type       
     */

  /* Clients check CM_API_WLAN for WLAN support for following
  **
  ** wlan_rssi, wlan_srv_status, wlan_bss_info, wlan_stats
  */
  uint16                     wlan_rssi;

  int32                      wlan_srv_status;
     /* Indicate current WLAN service state
     ** Mask used is CM_SS_WLAN_SRV_STATUS_MASK.
     ** sys_srv_status_e_type  
     */

  PACKED struct PACKED_POST
  {

    word                     chan;
      /* Channel on which this BSS provides service.
      ** sys_chan_type 
      */

    int32                    band;
      /* Band in which this BSS provides service.
      ** sys_band_class_e_type
      */

    int32                    bss_type;
      /* Type of BSS - Infrastructure or independent.
      ** sys_wlan_bss_e_type
      */

    uint64                   bssid;
      /*  BSS Id.
      **  sys_wlan_bssid_type      
      */

    PACKED struct PACKED_POST
    {
      uint8                  len;
        /* Length of the SSID, if == 0, then SSID = broadcast SSID.
        */

      char                   ssid[32];
        /* SSID of the wlan system.
        */

        /* 
        ** ssid
        ** Hardcoded to the value of SYS_WLAN_SSID_MAX_SIZE
        ** at the time log packet was declared. 
        ** This is intentional as otherwise changes to the constant would
        ** change byte ordering in the log packet.          
        */
    } ssid;
    /* SS Id.
    */

    uint16                   rssi;
      /* Measured RSSI of the BSS.
      */

    int32                    tech;
      /* WLAN technology in use.
      ** sys_tech_e_type
      */
  } wlan_bss_info;

    /* 802.11 (WLAN) System Information
    */

  PACKED struct PACKED_POST
  {
    int32                    current_xmit_rate;
      /* Xmit rate of the last packet successfully transmitted 
      ** sys_wlan_rate_e_type
      */

    uint32                   total_tx_bytes;
      /* Number of bytes transmitted over the WLAN interface
      */

    uint32                   total_rx_bytes;
      /* Number of bytes received over the WLAN interface     
      */

  } wlan_stats;
 
    /* WLAN stats such as Current Transmit Rate, TX/RX bytes.
    */

  int32   cipher_domain;
    /* Can be used only if CM_CIPHERING_IND_SUPP is defined.
    ** Ciphering on CS or PS domain or both.
    ** sys_srv_domain_e_type
    */

  PACKED struct PACKED_POST
  {

    uint8                  ss_thr_status;
      /* The throttle status of the current ss
      */

    dword                    thr_uptime;
      /* The throttling time period
      */

    unsigned int             idx;
      /* The index in the throttle_time table
      ** Used internally by CM to keep track of the next throttle time 
      ** increment
      */

    int32                    call_status;
      /* The call end reason
      ** cm_ss_orig_thr_reasons_e_type
      */

  } main_thr_status;

    /* The DCTM status, indicates the current throttle status of the MAIN 
    ** system
    */

  PACKED struct PACKED_POST
  {
    uint8                  ss_thr_status;
      /* The throttle status of the current ss
      */

    dword                    thr_uptime;
      /* The throttling time period
      */

    unsigned int             idx;
      /* The index in the throttle_time table
      ** Used internally by CM to keep track of the next throttle time 
      ** increment
      */

    int32                    call_status;
      /* The call end reason
      ** cm_ss_orig_thr_reasons_e_type
      */

  }  hdr_thr_status;

    /* The DCTM status, indicates the current throttle status of the HDR 
    ** system
    */

  PACKED struct PACKED_POST
  {
    int32                    id_type;

    PACKED union PACKED_POST
    {
      PACKED struct PACKED_POST
      {
        uint16               sid;
          /* The system ID 
          */

        uint16               nid;
          /* The network ID 
          */

      } is95;
        /* IS 95 sys id
        */

      byte                   is856[16];
        /* HDR sys id
        */

      PACKED struct PACKED_POST
      {
        byte                 identity[3];

      } plmn;
        /* UMTS (WCDMA/GSM) system ID.
        */

      PACKED struct PACKED_POST
      {
        uint8                len;
          /* Length of the SSID, if == 0, then SSID = broadcast SSID.
          */
        char                 ssid[32];
          /* SSID of the wlan system.
          */

          /* 
          ** ssid 
          ** Hardcoded to the value of SYS_WLAN_SSID_MAX_SIZE
          ** at the time log packet was declared. 
          ** This is intentional as otherwise changes to the constant would
          ** change byte ordering in the log packet.          
          */

      } ssid;
        /* WLAN system Id.
        */

      PACKED struct PACKED_POST
      {
        PACKED struct PACKED_POST
        {
          byte               identity[3];
        } plmn;

        word                 lac;
          /* LAC
          */
      } plmn_lac;
        /* UMTS (WCDMA/GSM) PLMN and LAC ID  
        */
    }id;
  } hdr_sys_id;

    /* The ID of the HDR sub system.
    ** Mask used is CM_SS_HDR_SYS_ID_MASK.
    */

  dword                     curr_uptime;
    /* Current uptime
    */

  int32                     psist;
   /* Indicate psist of the system.
   ** Mask used is CM_SS_PSIST_CHGD_MASK.
   ** cm_psist_e_type   
   */

  int32                     hdr_active_prot;
   /* Indicate HDR active protocol revision value.
   ** Mask used is CM_SS_HDR_ACTIVE_PROT_MASK.
   ** sys_active_prot_e_type
   */

 
   /* Below fields added for version = 5 and upwards */

  int32                     true_srv_status;
    /* The true service status (LIMITED/LIMITED_REGIONAL) of the system.
    ** Mask used is CM_SS_TRUE_SRV_STATUS_MASK.
    */

  uint64                    signal_strength_changed_fields;
    /**< Bit mask indicating which of the following signal strength fields
     ** were changed since last notification. Refer to the #defines
     ** (CM_SS_*_MASK) above. */

   int16                    pathloss;
    /**< Pathloss is applicable only in Connected mode (otherwise 0xFF)
    */

   int16                    sir;
    /**< SIR applicable and valid (otherwise 0xFF) only in WCDMA
    ** connected mode when DPCCH is active.
    */

  int16                     rssi2;
    /**< The -ve RSSI value, coming from RRC layer
    */

  int16                     rscp;
    /**< The range of RSCP would be from -28 to -121 dBm
    */

  boolean                   ecio_for_display;
    /**< TRUE if UE is in Data call
    */

  int16                     gw_ecio;
    /**< ECIO for WCDMA only
    ** Ec/Io is measured in -0.5 dB.
    ** What WCDMA L1 sends to RRC is in 0.5dB (positive value)
    ** For GSM it is defaulted to CMSS_ECIO_VALUE_INVALID_SIGNAL
    ** Clients should check for CM_API_GW_ECIO to use this field
    */

   PACKED struct PACKED_POST
   {
     uint32                 cell_id;  
	     /* Cell_id */
	   
   }cell_info;	
	
   PACKED struct PACKED_POST
   {
    int32                   reject_srv_domain;
    /**<   Srv domain on which registration is rejected */

    uint8                   reject_cause;
    /**<  Registration Reject Cause */
	
   }reg_reject_info;

 PACKED struct PACKED_POST
  {
    int32                    gan_state;
   /* Indicate GAN state .
   ** Mask used is CM_SS_GAN_STATE_MASK.
   ** sys_gw_gan_state_e_type
   */
    int32                    gan_conn_status;
   /* Indicate GAN connection status .
   ** Mask used is CM_SS_GAN_CONN_STATUS_MASK.
   ** sys_gw_gan_conn_status_e_type
   */
    int32                    gan_sig_chan_status;
   /* Indicate GAN signal channel status .
   ** Mask used is CM_SS_GAN_SIG_CHAN_STATUS_MASK.
   ** sys_gw_gan_sig_chan_status_e_type
   */
    PACKED struct PACKED_POST
    {
       boolean               is_mode_display_on;
 
       uint8                 gan_usz_icon_indicator;

       uint8                 gan_ms_location_status;

       uint8                 usz_str[34];

     }gan_mode_display;
   /* Indicate GAN mode display - whether GERAN or WLAN .
   ** Mask used is CM_SS_GAN_MODE_DISPLAY_MASK .
   ** sys_gw_gan_mode_display_s_type
   */        
    PACKED struct PACKED_POST
    {
        uint8                 gan_error_category;

        uint8                 gan_error_codepoint;

    }gan_error;
   /* Indicate GAN errors
   ** Mask used is CM_SS_GAN_ERROR_PROMPT_MASK  .
   ** sys_gw_gan_error_s_type
   */ 
   }gan_info;

   /* Below fields added for version = 6 and upwards */
	
  PACKED struct PACKED_POST
  {
  int32                       hs_ind;
    /**< High speed service indication (Over WCDMA) */

  int32                       dtm_supp;
    /**< Dual Transfer mode indication (Over GSM) */

  int32                       egprs_supp;
    /**< EGPRS indication  */

  int32                       hs_call_status;
    /**< Clients need to check for CM_API_CELL_SUPP_SERVICES
  ** and CM_API_CELL_SUPP_SERVICES_HS_CALL  before
    ** using this structure.
  */

  } cell_srv_ind;



   int32                      hybr_hdr_bcmcs_srv_status;
    /**< Specify whether BCMCS service is available on HDR.
    ** Mask used is CM_SS_HYBR_HDR_BCMCS_SRV_STATUS_MASK.
    */

    int32                     ss_main_bcmcs_srv_status;
    /**< Specify whether BCMCS service is available on MAIN.
    ** Mask used is CM_SS_MAIN_BCMCS_SRV_STATUS_MASK.
    */
	

   PACKED struct PACKED_POST
   {
    int32                     reg_status;
    /**< IP app registration status
    */

    int                       cause;
    /**< Cause given when app registration fails or deregisters
    */

    PACKED struct PACKED_POST
    {
     boolean                  present;
    /**< TRUE   - Warning is present
    ** FALSE  - Warning is not present
    */

    int                       cause;
    /**< Warning code - SIP RFC 3261, sect 20.43
    */

    char                      hostname[20];
    /**< Null terminated host name that
    ** generates the warning
    */

    char                      description[20];
    /**< Null terminated Warning text.
    */
    
    }warn_info;
    /**< Warning information given by ipapp
    */

  uint64                      call_type_mask;
    /**< Call tpyes that ipapp can service
    */

  int32                       sys_mode;
    /**< System modes app can support for calls
    ** Can be used only if CM_API_SMS_OVER_IP is defined.
    */

   }ipapp_info;


  int32                       active_band;
    /**< Clients need to check for CM_API_EMERGENCY_NUM_LIST before
    ** using this structure.
    **
    */

  uint16                      active_channel;
    /**< Report active channel of the current serving system.
    */

  int32                       hybrid_active_band;
    /**< Report active band of hybrid system 
    */

  uint16                      hybrid_active_channel;
    /**< Report active channel of the current serving system.
    */

  int32                       hdr_ln_status;
    /**< HDR LN status
    */

   PACKED struct PACKED_POST
   {
   boolean                    present; 
    /* present flag if this structure
    ** is populated
    */

   uint16                     sid;
    /* Sid that is in conflict with
    ** countries
    */

   uint8                      country_code_len;
    /* Length of country_code_list[]
    */

   uint16                     country_code_list [5];
    /* Array of mobile country codes that conflict
    ** with given sid
    */
   
   }country_list;


  uint16                      current_mcc;
    /* Clients need to check for CM_API_PLUS_DIALING before
    ** using this structure. Associated with CM_SS_SRV_STATUS_MASK
    **
    ** Carries country code of current serving country.
    */


  uint16                      home_mcc;
    /* Clients need to check for CM_API_PLUS_DIALING before
    ** using this structure. Associated with CM_SS_SRV_STATUS_MASK
    **
    ** Carries home country code.
    */


  int32                       plus_dial_setting;
    /* Clients need to check for CM_API_PLUS_DIALING before
    ** using this structure. Associated with CM_SS_SRV_STATUS_MASK
    **
    ** Carries NV setting corresp to plus dial
    */

  int32                       def_roam_ind;
   /** Default roaming indicator from PRL
   */



LOG_RECORD_END

/*lint -e717 */             /* DIAG uses a do ... while (0) construct. */
/*lint -e754 */             /* Local structure member for not referenced */
/*lint -e778 */             /* Re reference NONE+1, which evaluates to 0 */
/*===========================================================================
                        TYPE DEFINITIONS FOR EVENTS
===========================================================================*/
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */


/* This Table is from ICD (80-?????-) section x.x */
#define CM_STATE_INFO                       0
#define CM_PLMN_SELECTION_MODE              1
#define CM_SYSTEM_SELECTION_PREFERENCE      2


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#define CM_CDMA_CALL_ORIGINATION            3
#endif /* if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

#define CM_CALL_END                         4
#define CM_WCDMA_CALL_ORIGINATION           5
#define CM_GSM_CALL_ORIGINATION             6
#define CM_GET_CALL_INFORMATION_LIST        7
#define CM_SET_PASSWORD                     8
#define CM_SET_CLOSED_USER_GROUP_PREFERENCE 9
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */
#define CM_SEND_USSD_REPLY                  11

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */


#define CM_SEND_SUPS_LCS_MOLR               13
#define CM_SEND_SUPS_LCS_MOLR_COMPLETED     14


#if defined(FEATURE_CALL_THROTTLE)
#define CM_ORIG_THR_TBL_RESET               15
#endif /* defined(FEATURE_CALL_THROTTLE) */

#define CM_SYSTEM_SELECTION_PREFERENCE2     16
#define CM_GET_CELL_SERVICE_IND             17

#define CM_COUNTRY_SELECTED_BY_USER         18

#if ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK))
#error code not present
#endif /* ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK)) */ 

#define CMLOG_MAX_F                         22 /* Must be one greater than highest cmd */


#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */


#define CM_MLC_NUM_DATA_MAX               9 /* MLC number used for MOLR */
#define CM_LCS_CLIENT_EXT_ADD_DATA_MAX    20 /* Ext add data used for MOLR */
#define CM_GPS_ASSIS_DATA_MAX             38 /* GPS assisted data for MOLR */


/*lint -esym(750, CMLOG_MAX_F)*/

/*---------------------------------------------------------------------------
    CM STATE INFO -- SUBCOMMAND 0
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_STATE_INFO)
 /* No additional fields */
DIAGPKT_RSP_END
/*lint -esym(751, DIAG_SUBSYS_CM_CM_STATE_INFO_req_type)*/

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_STATE_INFO)
  int  call_state;                 /* ENUM for EVENT_ID 416 */
  int  oprt_mode;                  /* ENUM for EVENT_ID 417 */
  int  system_mode;                /* ENUM for EVENT_ID 418 */
  int  mode_pref;                  /* ENUM for EVENT_ID 520 */
  int  band_pref;                  /* ENUM for EVENT_ID 521 */
  int  roam_pref;                  /* ENUM for EVENT_ID 522 */
  int  srv_domain_pref;            /* ENUM for EVENT_ID 523 */
  int  acq_order_pref;             /* ENUM for EVENT_ID 524 */
  int  hybr_pref;                  /* ENUM for EVENT_ID 525 */
  int  network_sel_mode_pref;      /* ENUM for EVENT_ID 526 */
  int  srv_status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM PLMN LIST -- SUBCOMMAND 1
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_PLMN_SELECTION_MODE)
 /* No additional fields */
DIAGPKT_RSP_END
/*lint -esym(751, DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_req_type)*/

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_PLMN_SELECTION_MODE)
  int  status;                     /* SUCCESS/FAILURE_REASON */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM SYSTEM SELECTION REQUEST -- SUBCOMMAND 2
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE)
  byte         network_sel_mode_pref;     /* AUTOMATIC/MANUAL      */
  byte         PLMN[3];                   /* PLMN (if manual)      */
  unsigned int mode_pref;                 /* ENUM for EVENT_ID 520 */
  unsigned int acq_order_pref;            /* ENUM for EVENT_ID 524 */
  unsigned int band_pref;                 /* ENUM for EVENT_ID 521 */
  unsigned int roam_pref;                 /* ENUM for EVENT_ID 522 */
  unsigned int hybr_pref;                 /* ENUM for EVENT_ID 525 */
  unsigned int srv_domain_pref;           /* ENUM for EVENT_ID 523 */
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE)
  int  status;                            /* SUCCESS/FAILURE_REASON */
DIAGPKT_RSP_END

#define DIAG_MAX_WCDMA_DIGITS 64
#define CM_DIAG_MAX_GSM_DIGITS 64
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

/*
* cmlog_check_request_length () checks for the correctness of
* input packet structure depending on the declaration of that structure.
* cmlog_check_request_length () has to be modified for any change
* to the structures of WCDMA/GSM call origination, call end,
* set closed user group, set password and send USSD reply.
*/

/*---------------------------------------------------------------------------
    CM_WCDMA_CALL_ORIGINATION -- SUBCOMMAND 5 
---------------------------------------------------------------------------*/
/* WCDMA call origination request */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_WCDMA_CALL_ORIGINATION)

 /* DM shall set it to the number of valid digits in DIGITS field */
 uint8  num_digits;

 /* Rate of the vocoder: 0 ~ 7 for
    4.75, 5.15, 5.90, 6.70, 7.40, 7.95, 10.2, 12.2 kbps
 */
 uint8  amr_rate;

 /* Dialed digits; ASCII chars; 0-9, #, * only */
 uint8  digits[DIAG_MAX_WCDMA_DIGITS];

DIAGPKT_REQ_END

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */


/*---------------------------------------------------------------------------
    CM_GSM_CALL_ORIGINATION -- SUBCOMMAND 6
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GSM_CALL_ORIGINATION)

 /* DM shall set it to the number of valid digits in DIGITS field */
 uint8  num_digits;

 /* Dialed digits; ASCII chars; 0-9, #, * only */
 uint8  digits[CM_DIAG_MAX_GSM_DIGITS];

DIAGPKT_REQ_END

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))


#define CMLOG_CM_MAX_NUMBER_CHARS       64
#define CMLOG_CM_MAX_SUBADDRESS_DIGITS  21
#define CMLOG_CM_NUMBER_MAX             2
#define CMLOG_CM_MAX_ALPHA_DIGITS       12
#define CMLOG_CM_ORIG_QOS_LEN_MAX       31


/*---------------------------------------------------------------------------
    CM_CDMA_CALL_ORIGINATION -- SUBCOMMAND 3
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_CDMA_CALL_ORIGINATION)
  uint32    call_type;                              /* cm_call_type_e_type */
  uint32    srv_type;                                /* cm_srv_type_e_type */
  uint8     called_calling_num_count;
  uint8     alpha_digits_count;
  uint16    service_option;
  PACKED struct PACKED_POST {
    uint8   digits[CMLOG_CM_MAX_NUMBER_CHARS];
    uint8   subaddr[CMLOG_CM_MAX_SUBADDRESS_DIGITS];
    uint8   digit_mode;
    uint8   number_type;
    uint8   number_plan;
    uint8   presentation_ind;
    uint8   screening_ind;
    uint8   subaddr_extension;
    uint8   subaddr_type;
    uint8   subaddr_odd_even_ind;
    uint8   subaddr_count;
    uint8   digits_count;
    uint8   reserved;
  } number[CMLOG_CM_NUMBER_MAX];
  uint8     alpha_digits[CMLOG_CM_MAX_ALPHA_DIGITS];
  uint8     otasp_activate_code;
  boolean   drs_bit;
  boolean   sr_id_included;
  uint8     sr_id;
  uint8     qos_params[CMLOG_CM_ORIG_QOS_LEN_MAX];
  boolean   qos_params_included;
  uint32    hdr_last_act_data_net;
  uint8     qos_params_count;
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_CDMA_CALL_ORIGINATION)
  boolean  status;
  byte     call_id;
DIAGPKT_RSP_END
#endif /* if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


#define CM_DIAG_CALL_ID_MAX  8
#define MAX_DIAG_PWD_CHAR    4
#define MAX_DIAG_USS_CHAR    160 /* Max num of char is USS data*/
#define CMLOG_MAX_EVENT_PAYLOAD 8

/*---------------------------------------------------------------------------
    CM_CDMA_CALL_ORIGINATION -- SUBCOMMAND 4
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_CALL_END)
  byte         call_id_count;
  PACKED struct PACKED_POST {
    uint8        call_id;
    uint8        call_mode_type;             /* CDMA - 0, GW_CS=1, GW_PS=2 */
    boolean      cdma_end_reason_included;
    uint8        cdma_end_reason;
    boolean      gw_tear_down_ind_valid;
    boolean      gw_tear_down_ind_ti_flag;
    uint8        gw_cause;
  } end_params[CM_DIAG_CALL_ID_MAX];
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_CALL_END)
  boolean  status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_GET_CALL_INFORMATION_LIST -- SUBCOMMAND 7
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GET_CALL_INFORMATION_LIST)
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_GET_CALL_INFORMATION_LIST)
  boolean  status;
  uint8    num_calls;

  PACKED struct PACKED_POST {
    uint8    call_id;
    uint8    call_type;
    uint8    call_state;
    boolean  is_ps_call_dormant;
  } call_info [CM_CALL_ID_MAX];

DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_SET_CLOSED_USER_GROUP_PREFERENCE -- SUBCOMMAND 9
---------------------------------------------------------------------------*/
/* Set CUG preference */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SET_CLOSED_USER_GROUP_PREFERENCE)
uint8 use_cug;
uint8 cug_suppress_oa;
uint16 cug_group_id;

DIAGPKT_REQ_END

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

/*---------------------------------------------------------------------------
    CM_SET_PASSWORD -- SUBCOMMAND 8
---------------------------------------------------------------------------*/
/* Set Password request message */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SET_PASSWORD)
char  ss_password[MAX_DIAG_PWD_CHAR];
uint8  invoke_id;
DIAGPKT_REQ_END

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

/*---------------------------------------------------------------------------
    CM_SEND_USSD_REPLY -- SUBCOMMAND 11
---------------------------------------------------------------------------*/
/* Request message for sending USSD reply  */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SEND_USSD_REPLY)
uint8  invoke_id;
uint8  ussd_type;
byte  reply_data_size;
byte  reply_data[MAX_DIAG_USS_CHAR];
DIAGPKT_REQ_END

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */




/*---------------------------------------------------------------------------
    CM_SEND_SUPS_LCS_MOLR -- SUBCOMMAND 13
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR)
uint8   ss_ref;
uint8   molr_type;
boolean location_method_present;
uint8   location_method_data;
boolean lcs_qos_present;
boolean loc_qos_hor_accur_present;
uint8   lcs_qos_hor_accur_data;
boolean lcs_qos_ver_cord_req_present;
boolean lcs_qos_ver_accur_present;
uint8   lcs_qos_ver_accur_data;
boolean lcs_qos_rsp_time_present;
uint8   lcs_qos_rsp_time_categ;
boolean lcs_client_external_id_present;
boolean lcs_client_external_add_present;
uint8   lcs_client_external_add_size;
uint8   lcs_client_external_add_data[CM_LCS_CLIENT_EXT_ADD_DATA_MAX];
boolean mlc_number_present;
uint8   mlc_number_size;
uint8   mlc_number_data[CM_MLC_NUM_DATA_MAX];
boolean gps_assis_data_present;
uint8   gps_assis_data_size;
uint8   gps_assis_data[CM_GPS_ASSIS_DATA_MAX];
boolean supp_gad_shapes_present;
uint8   supp_gad_shapes_data;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR)
uint8 status;
uint8 ss_ref;
DIAGPKT_RSP_END


/*---------------------------------------------------------------------------
    CM_SEND_SUPS_LCS_MOLR_COMPLETED -- SUBCOMMAND 14
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR_COMPLETED)
uint8   ss_ref;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR_COMPLETED)
uint8 status;
uint8 ss_ref;
DIAGPKT_RSP_END




#if (defined (FEATURE_CALL_THROTTLE))
/*---------------------------------------------------------------------------
    CM_ORIG_THR_TBL_RESET -- SUBCOMMAND 15
---------------------------------------------------------------------------*/

/* Request Packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_ORIG_THR_TBL_RESET)
/* No more fields required
*/
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE (CM, CM_ORIG_THR_TBL_RESET)
  boolean status;
DIAGPKT_RSP_END
#endif /* #if (defined (FEATURE_CALL_THROTTLE))*/


/*---------------------------------------------------------------------------
    CM_SYSTEM_SELECTION_PREFERENCE2 -- SUBCOMMAND 16
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE2)
  byte         network_sel_mode_pref;     /* AUTOMATIC/MANUAL      */
  byte         PLMN[3];                   /* PLMN (if manual)      */
  unsigned int mode_pref;                 /* cm_mode_pref_e_type   */
  unsigned int acq_order_pref;            /* cm_gw_acq_order_pref_e*/
  uint64       band_pref;                 /* cm_band_pref_e_type   */
  unsigned int roam_pref;                 /* cm_roam_pref_e_type   */
  unsigned int hybr_pref;                 /* cm_hybr_pref_e_type   */
  unsigned int srv_domain_pref;           /* cm_srv_domain_pref_e_type */
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE2)
  int  status;                            /* SUCCESS/FAILURE_REASON */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
     CM_GET_CELL_SERVICE_IND  -- SUBCOMMAND 17
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GET_CELL_SERVICE_IND)
/* No more fields required
*/
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_GET_CELL_SERVICE_IND)
  uint8 hsdpa_hsupa_support; /* sys_hs_ind_e_type       */
  uint8 dtm_support;         /* sys_dtm_support_e_type  */
  uint8 reserved1;           /* Reserved for future use */
  uint8 reserved2;           /* Reserved for future use */
  uint8 reserved3;           /* Reserved for future use */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_COUNTRY_SELECTED_BY_USER -- SUBCOMMAND 18
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_COUNTRY_SELECTED_BY_USER)
  uint16 curr_sid;                 /* Sid that is being resolved */
  uint16 country_sel_for_sid; /* Mob country code of user sel country   */
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_COUNTRY_SELECTED_BY_USER)
  int  status;                            /* SUCCESS/FAILURE_REASON */
DIAGPKT_RSP_END


/*---------------------------------------------------------------------------
     CM_CALL_CMD_MODIFY_REQ -- SUBCOMMAND 20
----------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_CALL_CMD_MODIFY_REQ)
  uint8        call_id;           
DIAGPKT_RSP_END

#if ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK))
#error code not present
#endif  /* ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

/*---------------------------------------------------------------------------
     CM_CALL_CMD_MODIFY_RSP -- SUBCOMMAND 21
----------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_CALL_CMD_MODIFY_RSP)
  uint8                   call_id;   
  boolean                 gw_cs_modify_res_para;
  ie_cm_cc_cause_s_type   *cc_cause_ptr_para;
DIAGPKT_RSP_END

#if ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK))
#error code not present
#endif  /* ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

/*===========================================================================
                         DATA DECLARATIONS
===========================================================================*/

/* Type for CM test client.
*/
typedef struct cmlog_client_s {

  cm_client_id_type                client;

  sys_oprt_mode_e_type             oprt_mode;
    /* Indicate current operating mode */

  sys_sys_mode_e_type              sys_mode;
    /* Indicate current system mode */

  cm_mode_pref_e_type              mode_pref;
    /* Indicate current mode preference */

  cm_gw_acq_order_pref_e_type      acq_order_pref;
    /* The acquisition order preferred by the client. */

  cm_srv_domain_pref_e_type        srv_domain_pref;
    /* The service domain preferred by the client.
    ** This field is set after a call to cm_ph_cmd_sys_sel_pref. */

  cm_band_pref_e_type              band_pref;
    /* FEATURE_SD20 - Indicate current band preference */

  cm_roam_pref_e_type              roam_pref;
    /* FEATURE_SD20 - Indicate current band preference */

  cm_hybr_pref_e_type              hybr_pref;
    /* Indicate hybrid preference */

  cm_network_sel_mode_pref_e_type  network_sel_mode_pref;
    /* The network selection mode selected by the client */

  sys_srv_status_e_type            srv_status;
    /* Indicate current service status */

  boolean                          offline_signal_recvd;
    /* Indicates if CM received TASK_OFFLINE_SIG signal from TMC */

  cm_cell_srv_ind_s_type           cell_srv_ind;
    /* Indicates the CELL SERVICE Type */

} cmlog_client_s_type;

typedef struct cmlog_call_orig_s {
  uint8 call_id;
  uint8 call_type;
  uint8 srv_type;
} cmlog_call_orig_s_type;

typedef struct cmlog_call_connect_s {
  uint8 call_id;
  uint8 call_type;
  uint8 sys_mode;
} cmlog_call_connect_s_type;

typedef struct cmlog_call_end_s {
  uint8 num_calls;
  uint8 call_id;
} cmlog_call_end_s_type;

/* Holds call_state, call_type and call_id for allocated call_id's
*/
typedef struct cmlog_call_state_s {

  cm_call_id_type            call_id;
        /* call id */

  cm_call_state_e_type       call_state;
        /* Call state - idle, orig, etc. */

  cm_call_type_e_type        call_type;
        /* Call state - idle, orig, etc. */

  boolean                    is_ps_call_dormant;
        /* Gives status of ps call in converation
        ** Only applicable to ps calls.
        ** FALSE - PS call is active
        ** TRUE  - PS call is dormant
        */

} cmlog_call_state_s_type;

/*=========================================================================*/


/*===========================================================================
FUNCTION  cmlog_init

DESCRIPTION
  This function should be called at phone startup.
  It registers the subsystem command handlers with diag.

RETURN VALUE
  None

===========================================================================*/
void cmlog_init ( void );

/*===========================================================================
FUNCTION  cmlog_enter_emergency_cb

DESCRIPTION
  This function should be called when CM enters emergency callback state.
  It will send an event packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_enter_emergency_cb ( void );


/*===========================================================================
FUNCTION  cmlog_exit_emergency_cb

DESCRIPTION
  This function should be called when CM exits emergency callback state.
  It will send an event packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_exit_emergency_cb ( void );


/*===========================================================================
FUNCTION  cmlog_call_state

DESCRIPTION
  This function should be called when call state info changes.
  It will send an event packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  Local Copy of the call state is updated

===========================================================================*/
void cmlog_call_state ( void );


/*===========================================================================
FUNCTION  cm_get_call_state

DESCRIPTION
  This function should be called to retrieve the call state.
  It can be used by DIAG, for example, to fill status packet responses.

RETURN VALUE
  byte -  The call state

SIDE EFFECTS
  None

===========================================================================*/
byte cmlog_get_call_state ( void );

/*===========================================================================
FUNCTION  cmlog_get_operating_mode

DESCRIPTION
  This function should be called to retrieve the operating mode.
  It can be used by DIAG, for example, to fill status packet responses.

RETURN VALUE
  byte -  The operating mode

SIDE EFFECTS
  None

===========================================================================*/
byte cmlog_get_operating_mode ( void );

/*===========================================================================
FUNCTION  cmlog_get_system_mode

DESCRIPTION
  This function should be called to retrieve the system mode.
  It can be used by DIAG, for example, to fill status packet responses.

RETURN VALUE
  byte -  The system mode

SIDE EFFECTS
  None

===========================================================================*/
byte cmlog_get_system_mode ( void );


/*===========================================================================
FUNCTION  cmlog_plmn_list_event

DESCRIPTION
  This function sends a diag event when the PLMN NETWORK LIST arrives.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmlog_plmn_list_event
(
        sys_detailed_plmn_list_s_type *plmn_list_ptr
            /* Pointer to a structure containing the list of available PLMNs.
            */
);


/*===========================================================================
FUNCTION  cmlog_service_cnf_event

DESCRIPTION
       This function sends a diag event when a SERVICE CONFIRMATION arrives.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmlog_service_cnf_event
(
    sys_plmn_service_state_s_type   *srv_info_ptr
       /* Pointer to the service state info received in the report.
       */
);

#if defined FEATURE_HDR_HYBRID && defined FEATURE_GPSONE
#error code not present
#endif // defined FEATURE_HDR_HYBRID && defined FEATURE_GPSONE

/*===========================================================================
FUNCTION cmlog_call_orig_start

DESCRIPTION
   This function logs the call orig start event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_start(
  const cmcall_s_type  *call_ptr
    /*  pointer for the call object */
);


/*===========================================================================
FUNCTION  cmlog_call_orig_redial

DESCRIPTION
   This function logs the call orig redial event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_redial(
  cm_call_end_e_type    end_status,
    /* the call end status */
  cmcall_act_e_type     redial_act,
    /* the redial action */
  dword                 sr_left
    /* number of silent redial left */
);


/*===========================================================================
FUNCTION  cmlog_call_orig_send_hdr_orig

DESCRIPTION
   This function logs the orig sent to HDR event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_send_hdr_orig( void );


/*===========================================================================
FUNCTION  cmlog_call_orig_send_mc_orig

DESCRIPTION
   This function logs the orig sent to MC event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_send_mc_orig( void );


/*===========================================================================
FUNCTION  cmlog_call_orig_end

DESCRIPTION
   This function logs the call orig end event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_end(
  cm_call_end_e_type         end_status
    /* call end status */
);


/*===========================================================================
FUNCTION  cmlog_call_orig_connected

DESCRIPTION
   This function logs the call orig connected event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_connected(
  cm_call_type_e_type         call_type,
    /* origination call type */
  cm_srv_type_e_type          srv_type,
    /* origination srv type */
  word                        srv_opt
    /* origination srv opt */
);

/*===========================================================================
FUNCTION  cmlog_set_cm_offline_signal_recvd

DESCRIPTION

  CM calls this after receiving TASK_OFFLINE_SIG signal from TMC. 
  This happens some time after cm_ph_cmd_oprt_mode() with 
  SYS_OPRT_MODE_OFFLINE_CDMA had been processed by CM.
  Call on log object oprt mode event handler to force sending of DIAG 
  EVENT_CM_OPERATIONAL_MODE event now that we got TASK_OFFLINE_SIG 
  from TMC 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  cmlog_set_cm_offline_signal_recvd(void);

#endif /* CM_LOG_H */

