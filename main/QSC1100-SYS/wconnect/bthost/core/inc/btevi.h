#ifndef _BTEVI_H
#define _BTEVI_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             B L U E T O O T H    I N T E R N A L    E V E N T S

DESCRIPTION
  This file contains declarations for Bluetooth driver internal events.

              Copyright (c) 2000-2008 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary                        

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btevi.h_v   1.20   09 Aug 2002 18:37:36   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/inc/btevi.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btevi.h_v  $
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-09-15   pg  Fixed Compiler warning.
  2008-08-21   pg  Moved type defintions used by read remote feature and 
                   synchronous connection parameters (bt_cmd APIs) to bt.h
  2008-03-24   vk  Added struct for command complete event of HCI Read Defalut
                   Erroneous Data Reporting mode command.

 *    #34        05 Dec 2007            AG
 * Added APIT support for 2.1
 *
 *    #33        01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *    #32        31 Oct 2007            VK
 *  Added struct to support Inquiry result event with RSSI.
 *
 *    #31        16 Oct 2007            DSN
 * Added support for Rd Loc Supported commands and Rd local extended 
 * features command.
 *
 *    #30        04 Sept 2007           BH
 * Added support for QSoC vender specific encryption key length command.  
 *
 *    #29        20 Aug 2007            PG
 * Seperated HCI generic vendor specific event handling from Qualcomm 
 * proprietary implementation used with embedded controller.
 *
 *    #28        09 May 2007            SSK
 * Changed types of fields in struct bt_ev_rc_ms_changed_type to
 * remove compiler warnings.
 *
 *    #27        02 Mar 2007            JN
 * Added new data type for BTS Vendor Specific event.
 *
 *    #26        20 Oct 2006            SSK
 * External SOC suppport: H4DS and BCCMD modules.
 *
 *    #25        13 Sep 2006            MP
 * Added the L2CAP local_cid field to bt_ev_rm_tx_data_cfm_type and 
 * bt_ev_hc_tx_data_cfm_type structure.
 *
 *    #24        26 Jul 2006            DM
 * Added new data types for l2cap tx data cfms
 * Added new types for l2cap packet boundary flags (flushable start/cont)
 *
 *    #23        01 Jun 2006            JTL
 * Add CID to L2Cap Mode 2 security.
 *
 *    #22        5  Apr 2006            DP
 * Added HC QC current connection encryption event to support Extended
 * Service Security.
 *
 *    #21        16 Mar 2006            JTL
 * Added RC Unregister event type.
 *
 *    #20        30 Jan 2006            PN
 * Added BT_EV_HC_QC_RADIO_ACTIVITY_CHANGED event.
 *
 *    #19        7  Dec 2005            DP
 * Changed HC QC Remote Auth or Pairing Failed event to use BD address
 * instead of HC handle to identify the connection.
 *
 *    #18        19 Oct 2005            JH
 * Added support for the HCI read remote extended features complete event.
 * 
 *    #17        27 Jun 2005            JH
 * Added BT 1.2 events: Synchronous Connection Complete and 
 *  Synchronous Connection Changed, and the associated data structures.
 * 
 *    #16        7  Feb 2005            PN
 * Added bt_ev_hc_qc_extended_cmd_status_type.
 *
 *    #15        7  Jul 2004            JH
 * Added BT 1.2 AFH and eSCO support
 *
 *    #14        7  Jul 2004            DP
 * Fixed name of QC HCI Remote Auth Pair Failed event. 
 *
 *    #13        1  Jun 2004            DP
 * Implemented Mode 2 security.
 *
 *    #12        2  Apr 2004            DP
 * Added EMS support.
 *
 *    #11        3  Mar 2004            DP
 * Added QC auth/pairing failed event.
 *
 *    #10        19 Feb 2004            DP
 * Added APIT SIO RX data event.
 *
 *    #9         19 Dec 2003            DP
 * Fixed naming convention on LMP policy definitions.
 *
 *    #8         4  Nov 2003            DP
 * Added security APIs to RM.  General cleanup.
 *
 *    #7         26 Sep 2003            DP
 * Updated RM Device Manager API, AG connect/disconnect and VR capable APIs,
 * and made service/device names character strings.
 *
 *    #6         06 Aug 2003            PN
 * Changed some RM event types to APIT event types.
 * 
 *    #5         22 Jul 2003            RY
 * Moved bt_pair_type to bti.h.
 * 
 *    Rev 1.20   09 Aug 2002 18:37:36   waynelee
 * Added MSM6100 support.
 * 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "dsm.h"
#include "queue.h"
#include "customer.h"


#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */


/*-------------------------------------------------------------------------*/
/*                       Network Access (NA) Events                        */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_ev_na_connected_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_event_reason_type  event_reason;
} bt_ev_na_disconnected_type;


#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */


/*-------------------------------------------------------------------------*/
/*                            RFCOMM Events                                */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  boolean  pn_negot_done;
  boolean  rpn_negot_done;
  uint16   max_frame_size;
  uint8    priority;
  boolean  cb_flow_ctrl;
  uint8    baudrate;
  uint8    format;     /*  Corresponds to value3 octet in RPN.  */
  uint8    flow_ctrl;
  uint8    xon_char;
  uint8    xoff_char;
} bt_rc_ev_config_type;

typedef BT_PACKED struct
{
  uint16                pid;
  bt_bd_addr_type       bd_addr;
  bt_rc_ev_config_type  config;
} bt_ev_rc_connected_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

typedef BT_PACKED struct
{
  uint16                pid;
  bt_event_reason_type  reason;
} bt_ev_rc_disconnected_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_event_reason_type  reason;
} bt_ev_rc_connection_failed_type;

typedef BT_PACKED struct
{
  uint16                 pid;
  bt_spp_ms_enum_type    ms_changed_mask;
  bt_spp_ms_enum_type    modem_status;
  boolean                break_present;
  uint8                  break_length;
  uint8                  breaks;
} bt_ev_rc_ms_changed_type;

typedef BT_PACKED struct
{
  uint16   pid;
  boolean  success;
  uint16   byte_count;
  uint8    start_value;
  uint8    increment;
  uint16   rxd_byte_count;
  boolean  bad_value;
  uint16   bad_value_idx;
  uint8    exp_value;
  uint8    rxd_value;
} bt_ev_rc_ping_result_type;

typedef BT_PACKED struct
{
  uint16                pid;
  bt_rc_ev_config_type  config;
} bt_ev_rc_reconfigured_type;

typedef BT_PACKED struct
{
  uint16                 pid;
  bt_rc_line_error_type  cur_line_error;
  uint8                  overruns;
  uint8                  parity_errors;
  uint8                  framing_errors;
} bt_ev_rc_line_error_type;

typedef BT_PACKED struct
{
  uint8                  server_channel;
} bt_ev_rc_unregistered_type;

/*-------------------------------------------------------------------------*/
/*                             L2CAP Events                                */
/*-------------------------------------------------------------------------*/

#define BT_L2_MAX_ECHO_DATA_LEN 4  /* 4 bytes */

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  uint16           cid;
  word             mtu;
} bt_ev_l2_connected_type;

typedef BT_PACKED struct
{
  uint16          cid;
  dsm_item_type*  dsm_ptr;
} bt_ev_l2_rx_data_type;

typedef BT_PACKED struct
{
  uint16                cid;
  bt_event_reason_type  reason;
} bt_ev_l2_disconnected_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  uint16                cid;
  bt_event_reason_type  reason;
} bt_ev_l2_connection_failed_type;

typedef BT_PACKED struct
{
  uint16                cid;
  bt_event_reason_type  reason;
  word                  out_mtu;
  word                  in_flush_to;
  bt_l2_qos_type        in_qos;
} bt_ev_l2_reconfigured_type;

typedef BT_PACKED struct
{
  void*  up_handle_ptr;
} bt_ev_l2_tx_data_cfm_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  uint16                length;
  uint8                 data[ BT_L2_MAX_ECHO_DATA_LEN ];
  bt_event_reason_type  reason;
} bt_ev_l2_ping_type;


/*-------------------------------------------------------------------------*/
/*                               RM Events                                 */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  bt_cod_type      class_of_device;
} bt_ev_rm_inquiry_result_type;

typedef BT_PACKED struct
{
  uint8  num_responses;
} bt_ev_rm_inquiry_complete_type;

typedef BT_PACKED struct
{
  bt_cmd_status_type  status;
  bt_bd_addr_type     bd_addr;
  char                name_str[ BT_SD_MAX_DEVICE_NAME_LEN + 1 ];
} bt_ev_rm_remote_name_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  bt_cod_type      class_of_device;
} bt_ev_rm_connect_req_acl_type;

typedef BT_PACKED struct
{
  bt_rm_handle_type  handle;
  bt_bd_addr_type    bd_addr;
} bt_ev_rm_connected_acl_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_event_reason_type  reason;
} bt_ev_rm_connection_failed_acl_type;

typedef BT_PACKED struct
{
  bt_rm_handle_type  handle;
  boolean            l2cap_pkt_start;
  boolean            broadcast;
  dsm_item_type*     dsm_ptr;
} bt_ev_rm_rx_data_acl_type;

typedef BT_PACKED struct
{
  bt_rm_handle_type     handle;
  uint16                local_cid; /* L2CAP local cid */
  void*                 up_handle_ptr;
}bt_ev_rm_tx_data_cfm_type;

typedef BT_PACKED struct
{
  bt_rm_handle_type     handle;
  bt_event_reason_type  reason;
} bt_ev_rm_disconnected_acl_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  bt_cod_type      class_of_device;
} bt_ev_rm_connect_req_sco_type;

typedef BT_PACKED struct
{
  bt_rm_handle_type  handle;
  bt_bd_addr_type    bd_addr;
} bt_ev_rm_connected_sco_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_event_reason_type  reason;
} bt_ev_rm_connection_failed_sco_type;

typedef BT_PACKED struct
{
  bt_rm_handle_type  handle;
  dsm_item_type*     dsm_ptr;
} bt_ev_rm_rx_data_sco_type;

typedef BT_PACKED struct
{
  bt_rm_handle_type     handle;
  bt_event_reason_type  reason;
} bt_ev_rm_disconnected_sco_type;

typedef BT_PACKED struct
{
  uint8            rfcomm_scn;
  bt_bd_addr_type  bd_addr;
  boolean          secure_enough;
} bt_ev_rm_rfcomm_scn_sec_result_type;

typedef BT_PACKED struct
{
  uint16           l2cap_psm;
  bt_bd_addr_type  bd_addr;
  uint16           l2cap_cid;
  boolean          secure_enough;
} bt_ev_rm_l2cap_psm_sec_result_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */


/*-------------------------------------------------------------------------*/
/*                Host Controller Interface (HCI) Events                   */
/*-------------------------------------------------------------------------*/

/* General HCI command return parameters.  Part of union for
** Command_Complete event
*/
typedef BT_PACKED struct
{
  uint8  status;
} bt_hc_rp_status_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  conn_hndl;
} bt_hc_rp_conn_hndl_type;

typedef BT_PACKED struct
{
  uint8            status;
  bt_bd_addr_type  bd_addr;
} bt_hc_rp_bd_addr_type;

typedef enum
{
  BT_HC_PKT_BOUNDARY_START,
  BT_HC_PKT_BOUNDARY_FRAGMENT,
  BT_HC_PKT_BOUNDARY_START_FLUSHABLE,
  BT_HC_PKT_BOUNDARY_CONT_FLUSHABLE
} bt_hc_pkt_boundary_flag_type;

#ifdef FEATURE_BT_2_1
typedef enum
{
   BT_HC_AUTO_FLUSHABLE_PKT_TYPE,
   BT_HC_RESERVED_PKT_TYPE
} bt_hc_flush_pkt_type;
#endif /* FEATURE_BT_2_1 */

typedef enum
{
  BT_HC_BROADCAST_NONE,    /* No Broadcast                                 */
  BT_HC_BROADCAST_ACTIVE,  /* Broadcast to all active slaves               */
  BT_HC_BROADCAST_PICONET  /* Broadcast to all slaves (including parked    */
} bt_hc_broadcast_flag_type;

typedef enum
{
  BT_LOOPBACK_NONE,
  BT_LOOPBACK_LOCAL,
  BT_LOOPBACK_REMOTE
} bt_loopback_type;

typedef enum
{
  BT_CC_N_AMERICA_EUROPE_JAPAN,
  BT_CC_FRANCE
} bt_country_code_type;

typedef enum
{
  BT_PIN_VARIABLE,
  BT_PIN_FIXED
} bt_pin_type;

#ifdef FEATURE_BT_1_2

typedef enum
{
  BT_INQ_RESULT_STANDARD,
  BT_INQ_RESULT_WITH_RSSI,
  BT_INQ_RESULT_EXTENDED
} bt_inq_mode_type;

typedef enum
{
  BT_IS_STANDARD,
  BT_IS_INTERLACED
} bt_inq_scan_mode_type;

#endif /* FEATURE_BT_1_2 */

typedef enum
{
#ifdef FEATURE_BT_1_2
  BT_PS_STANDARD = BT_IS_STANDARD,
  BT_PS_INTERLACED = BT_IS_INTERLACED
#else
  BT_PS_MANDATORY,
  BT_PS_OPTIONAL_MODE_1,
  BT_PS_OPTIONAL_MODE_2,
  BT_PS_OPTIONAL_MODE_3
#endif /* FEATURE_BT_1_2 */
} bt_page_scan_mode_type;

typedef enum
{
  BT_PSP_P0,
  BT_PSP_P1,
  BT_PSP_P2
} bt_page_scan_per_mode_type;

typedef enum
{
  BT_PSR_R0,
  BT_PSR_R1,
  BT_PSR_R2
} bt_page_scan_rep_mode_type;

/* CLK offset valid bit and value */
#define BT_CLK_OFFSET_VALID_B    0x8000  /* bit                    */
#define BT_CLK_OFFSET_VALID_V    0x8000  /* clk offset valid bit   */
#define BT_CLK_OFFSET_INVALID_V  0x0000  /* clk offset invalid bit */

typedef enum
{
  BT_ROLE_SWITCH_NOT_ALLOWED,
  BT_ROLE_SWITCH_ALLOWED
} bt_role_switch_type;

/* Encryption Enables according to BT 1.0A HCI Spec (sect 4.5.16) */
typedef enum
{
  BT_ENCRYPT_ENABLE_OFF,
  BT_ENCRYPT_ENABLE_ON
} bt_encrypt_enable_type;

/* Link Key Select according to BT 1.0B HCI Spec (setc 4.5.18) */
typedef enum
{
  BT_LINK_KEY_USE_SEMI_PERM,
  BT_LINK_KEY_USE_TEMP
} bt_link_key_select_type;

typedef enum
{
  BT_AUTO_ACCEPT_OFF = 1,
  BT_AUTO_ACCEPT_ON_ROLE_SWITCH_DIS,
  BT_AUTO_ACCEPT_ON_ROLE_SWITCH_EN
} bt_auto_accept_type;

typedef BT_PACKED struct
{
  uint8           status;
  dsm_item_type*  dsm_ptr;  /* contains up to 248 bytes of UTF-8 encoded */
                            /* characters; null terminated.              */
} bt_hc_rp_rd_local_name_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  conn_accept_timeout;  /* units of .625 ms Range: 0x0001-0xB540 */
} bt_hc_rp_rd_conn_accept_to_type;

typedef BT_PACKED struct
{
  uint8            status;
  uint16           conn_hndl;
  bt_role_type     current_role;
} bt_hc_rp_role_discovery_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  page_timeout;  /* units of .625 ms Range: 0x0001-0xFFFF */
} bt_hc_rp_rd_page_to_type;

#define BT_LINK_POLICY_ROLE_SWITCH_B  0x0001
#define BT_LINK_POLICY_HOLD_MODE_B    0x0002
#define BT_LINK_POLICY_SNIFF_MODE_B   0x0004
#define BT_LINK_POLICY_PARK_MODE_B    0x0008

typedef BT_PACKED struct
{
  uint8   status;
  uint16  conn_hndl;
  uint16  link_policy_settings;  /* use BT_LINK_POLICY_**_B bit masks */
} bt_hc_rp_rd_link_policy_type;

typedef BT_PACKED struct
{
  uint8        status;
  bt_pin_type  pin_type;
} bt_hc_rp_rd_pin_type;

typedef enum
{
  BT_HC_ONE_LINK_KEY,
  BT_HC_ALL_STORED_LINK_KEYS,
  BT_HC_LINK_KEY_MAX
} bt_hc_link_key_flag_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  max_num_keys;
  uint16  num_keys_read;
} bt_hc_rp_rd_link_key_type;

typedef BT_PACKED struct
{
  uint8  status;
  uint8  num_keys_written;
} bt_hc_rp_wr_link_key_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  num_keys_deleted;
} bt_hc_rp_del_link_key_type;

typedef enum
{
  BT_HC_INQ_DIS_PAGE_DIS,
  BT_HC_INQ_EN_PAGE_DIS,
  BT_HC_INQ_DIS_PAGE_EN,
  BT_HC_INQ_EN_PAGE_EN
} bt_hc_scan_enable_type;

typedef BT_PACKED struct
{
  uint8                   status;
  bt_hc_scan_enable_type  scan_enable;
} bt_hc_rp_rd_scan_enable_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  page_scan_interval;  /* units of .625 ms Range: 0x0012-0x1000 */
  uint16  page_scan_window;    /* units of .625 ms Range: 0x0012-0x1000 */
} bt_hc_rp_rd_page_scan_act_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  inquiry_scan_interval;  /* units of .625 ms Range: 0x0012-0x1000 */
  uint16  inquiry_scan_window;    /* units of .625 ms Range: 0x0012-0x1000 */
} bt_hc_rp_rd_inq_scan_act_type;

typedef enum
{
  BT_HC_AUTH_DISABLED,
  BT_HC_AUTH_ALL_ENABLED
} bt_hc_auth_enable_type;

typedef BT_PACKED struct
{
  uint8                    status;
  bt_hc_auth_enable_type   auth_enable;
} bt_hc_rp_rd_auth_enable_type;

typedef BT_PACKED struct
{
  uint8                 status;
  bt_encrypt_mode_type  encrypt_mode;
} bt_hc_rp_rd_encrypt_mode_type;

typedef BT_PACKED struct
{
  uint8        status;
  bt_cod_type  class_of_device;
} bt_hc_rp_rd_class_of_dev_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  voice_setting;   /* use BT_HC_VS_**_M #defines */
} bt_hc_rp_rd_voice_setting_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  conn_hndl;
  uint16  flush_timeout;  /* 0    => timoeut= infinity; no automatic flush */
                          /* else => units- .625 ms.  Range: 0x0001-0x07FF */
} bt_hc_rp_rd_auto_flush_to_type;

typedef BT_PACKED struct
{
  uint8                 status;
  bt_country_code_type  country_code;
} bt_hc_rp_rd_country_code_type;

typedef BT_PACKED struct
{
  uint8  status;
  uint8  num_broadcast_retrans; /* range: 0x00-0xFF */
} bt_hc_rp_rd_broad_retrans_type;

typedef enum
{
  BT_HC_HOLD_MODE_MAINTAIN_CURRENT,
  BT_HC_HOLD_MODE_SUSPEND_PAGE_SCAN,
  BT_HC_HOLD_MODE_SUSPEND_INQUIRY_SCAN,
  BT_HC_HOLD_MODE_SUSPEND_PER_INQUIRY
} bt_hc_hold_mode_act_type;

typedef BT_PACKED struct
{
  uint8                     status;
  bt_hc_hold_mode_act_type  hold_mode_activity;
} bt_hc_rp_rd_hold_mode_act_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  conn_hndl;
  int8    transmit_power_level;  /* units: dBm  range: -30 <= N <= 20 */
} bt_hc_rp_rd_tx_power_type;

typedef BT_PACKED struct
{
  uint8  status;
  uint8  sco_flow_control_enable;  /* 0x00 => disable; 0x01 => enable */
} bt_hc_rp_rd_sco_flow_ctrl_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  conn_hndl;
  uint16  link_supervision_to;  /* 0 => no link supervision timeout     */
                                /* .625 ms units.  range: 0x0001-0xFFFF */
} bt_hc_rp_rd_link_super_to_type;

typedef BT_PACKED struct
{
  uint8  status;
  uint8  num_supported_iac;  /* range 0x01 -> 0x40 */
} bt_hc_rp_rd_num_iac_type;

typedef BT_PACKED struct
{
  uint8           status;
  uint8           num_current_iac;
  dsm_item_type*  dsm_ptr;

  /* dsm_ptr the equivalent of the following data:
  bt_lap_type      iac_lap[ num_current_iac ];
  */
} bt_hc_rp_rd_curr_iac_lap_type;

typedef BT_PACKED struct
{
  uint8                       status;
  bt_page_scan_per_mode_type  page_scan_period_mode;
} bt_hc_rp_rd_ps_per_mode_type;

#ifndef FEATURE_BT_1_2 /* BT 1.1 */
typedef BT_PACKED struct
{
  uint8                   status;
  bt_page_scan_mode_type  page_scan_mode;
} bt_hc_rp_rd_ps_mode_type;
#endif

typedef BT_PACKED struct
{
  uint8   status;
  uint8   hci_version;
  uint16  hci_revision;
  uint8   lmp_version;
  uint16  manufacturer_name;
  uint16  lmp_subversion;
} bt_hc_rp_rd_local_ver_type;

typedef BT_PACKED struct
{
  uint8                 status;
  bt_lmp_features_type  lmp_features;
} bt_hc_rp_rd_local_feat_type;


typedef BT_PACKED struct
{
  uint8  status;
  uint8  supported_cmds[64];
} bt_hc_rp_rd_local_cmds_type;

typedef BT_PACKED struct
{
  uint8  status;
  uint8  page_num;
  uint8  max_page_num;
  uint8  ext_lmp_feat[8];
} bt_hc_rp_rd_rd_local_ext_feat_type;


typedef BT_PACKED struct
{
  uint8   status;
  uint16  hc_acl_data_pkt_len;
  uint8   hc_sco_data_pkt_len;
  uint16  hc_total_num_acl_data_pkts;
  uint16  hc_total_num_sco_data_pkts;
} bt_hc_rp_rd_buffer_size_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  conn_hndl;
  uint16  failed_contact_counter;
} bt_hc_rp_rd_failed_contact_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  conn_hndl;
  uint8   link_quality;  /* relative units; range: 0x00-0xF; 0xFF is best */
} bt_hc_rp_get_link_quality_type;

typedef BT_PACKED struct
{
  uint8   status;
  uint16  conn_hndl;
  int8    rssi;       /* units: dB; range: -128 <= N <= 127 */
} bt_hc_rp_rd_rssi_type;

typedef BT_PACKED struct
{
  uint8             status;
  bt_loopback_type  loopback_mode;
} bt_hc_rp_rd_loopback_mode_type;

#ifdef FEATURE_BT_1_2

typedef BT_PACKED struct
{
  uint8             status;
  uint8             afh_chan_assess_mode;
} bt_hc_rp_rd_afh_chan_assess_mode_type;

typedef BT_PACKED struct
{
  uint8             status;
  uint16            conn_hndl;
  uint8             afh_mode;
  uint8             afh_chan_map[ 10 ];
} bt_hc_rp_rd_afh_chan_map_type;

typedef BT_PACKED struct
{
  uint8                  status;
  bt_inq_scan_mode_type  inq_scan_type;
} bt_hc_rp_rd_inq_scan_type_type;

typedef BT_PACKED struct
{
  uint8             status;
  bt_inq_mode_type  inq_mode;
} bt_hc_rp_rd_inq_mode_type;

typedef BT_PACKED struct
{
  uint8                   status;
  bt_page_scan_mode_type  page_scan_type;
} bt_hc_rp_rd_page_scan_type_type;

#endif /* FEATURE_BT_1_2 */

#ifdef FEATURE_BT_2_1

typedef BT_PACKED struct
{
  uint8            status;
  uint8            sp_mode;
} bt_hc_rp_sp_mode_type;

typedef BT_PACKED struct
{
  uint8            status;
  bt_sp_hash_type  sp_hash;
  bt_sp_rand_type  sp_rand;
} bt_hc_rp_oob_data_type;


typedef BT_PACKED struct
{
  uint8            status;
  uint8            default_err_data_reporting_mode;
} bt_hc_rp_rd_default_err_data_reporting_type;


typedef BT_PACKED struct
{
  uint8           status;
  uint8           fec_required;
  dsm_item_type*  dsm_ptr;  /* contains 240 octets of EIR data */
} bt_hc_rp_rd_ext_inq_rsp_type;

typedef BT_PACKED struct
{
  uint8           status;
  int8            tx_power_level;
} bt_hc_rp_rd_inq_rsp_tx_pwr;
#endif /* FEATURE_BT_2_1 */

/*-------------------------------------------------------------------------*/
/* HCI Event:       RX ACL Data Packet (4.4.3)                             */
/* Event type:      BT_EV_HC_RX_ACL_DATA                                   */
/* Event struct:    bt_ev_hc_rx_acl_data_type                              */
/* Notes:           This is not a HCI event according to the BT spec.      */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16                        conn_hndl;
  bt_hc_pkt_boundary_flag_type  pkt_boundary_flag;
  bt_hc_broadcast_flag_type     broadcast_flag;
  uint16                        data_length;
  dsm_item_type*                dsm_ptr;
} bt_ev_hc_rx_acl_data_type;

typedef BT_PACKED struct
{
  uint16                       conn_hndl;
  uint16                       local_cid; /* L2CAP local cid */
  void*                        up_handle_ptr;
}bt_ev_hc_tx_data_cfm_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       RX SCO Data Packet (4.4.3)                             */
/* Event type:      BT_CMD_HC_RX_SCO_DATA                                  */
/* Event struct:    bt_cmd_hc_rx_sco_data_type                             */
/* Notes:           This is not a HCI command according to the BT spec.    */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint8            data_length;
  dsm_item_type*   dsm_ptr;
} bt_ev_hc_rx_sco_data_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Inquiry Complete (5.2.1)                               */
/* Event Type:      BT_EV_HC_INQUIRY_COMPLETE                              */
/* Event struct:    bt_ev_hc_inquiry_cmplt_type                            */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           status;
} bt_ev_hc_inquiry_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Inquiry Result (5.2.2)                                 */
/* Event Type:      BT_EV_HC_INQUIRY_RESULT                                */
/* Event struct:    bt_ev_hc_inquiry_result_type                           */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           num_responses;
  dsm_item_type*  dsm_ptr;       /* num_responses number of these:   */
                                 /*   bt_bd_addr_type[]              */
                                 /*   bt_page_scan_rep_mode_type[]   */
                                 /*   bt_page_scan_per_mode_type[]   */
                                 /*   bt_page_scan_mode_type[]       */
                                 /*   bt_cod_type[]                  */
                                 /*   uint16 clk_offset[]            */
} bt_ev_hc_inquiry_result_type;

#define BT_EV_HC_INQUIRY_RESULT_ELEM_SIZE \
  ( sizeof( bt_bd_addr_type ) + \
    sizeof( bt_page_scan_rep_mode_type ) + \
    sizeof( bt_page_scan_per_mode_type ) + \
    sizeof( bt_page_scan_mode_type ) + \
    sizeof( bt_cod_type ) + \
    sizeof( uint16 ) )


/*-------------------------------------------------------------------------*/
/* HCI Event:       Inquiry Result with RSSI (BT spec 2.0 sec 7.7.33 )     */
/* Event Type:      BT_EV_HC_INQUIRY_RESULT_WITH_RSSI                      */
/* Event struct:    bt_ev_hc_inquiry_result_with_rssi_type                 */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           num_responses;
  dsm_item_type*  dsm_ptr;       /* num_responses number of these:   */
                                 /*   bt_bd_addr_type[]              */
                                 /*   bt_page_scan_rep_mode_type[]   */
                                 /*   reserved[]                     */
                                 /*   bt_cod_type[]                  */
                                 /*   uint16 clk_offset[]            */
                                 /*   uint8  RSSI[]                  */
} bt_ev_hc_inquiry_result_with_rssi_type;

#define BT_EV_HC_INQUIRY_RESULT_WITH_RSSI_ELEM_SIZE \
  ( sizeof( bt_bd_addr_type ) + \
    sizeof( bt_page_scan_rep_mode_type ) + \
    sizeof( uint8 ) + \
    sizeof( bt_cod_type ) + \
    sizeof( uint16 ) + \
    sizeof( uint8) )

#ifdef FEATURE_BT_2_1
/*-------------------------------------------------------------------------*/
/* HCI Event:       Extended Inquiry Result                                */ 
/* Event Type:      BT_EV_HC_INQUIRY_RESULT_EXTENDED                       */
/* Event struct:    bt_ev_hc_inquiry_result_extended_type                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           num_responses;
  dsm_item_type*  dsm_ptr;       /* num_responses  (always 1)        */
                                 /*   bt_bd_addr_type[]              */
                                 /*   bt_page_scan_rep_mode_type[]   */
                                 /*   uint8 * reserved               */
                                 /*   bt_cod_type[]                  */
                                 /*   uint16 clk_offset[]            */
                                 /*   uint8 rssi                     */
                                 /*   eir                            */
} bt_ev_hc_inquiry_result_extended_type;

#define BT_EV_HC_INQUIRY_RESULT_EXTENDED_ELEM_SIZE \
  ( sizeof( bt_bd_addr_type ) + \
    sizeof( bt_page_scan_rep_mode_type ) + \
    sizeof( uint8 ) + \
    sizeof( bt_cod_type ) + \
    sizeof( uint16 ) + \
    sizeof( uint8 ) + \
    sizeof( uint8 )*BT_MAX_EIR_HCI_LEN )

#endif /* FEATURE_BT_2_1 */

/*-------------------------------------------------------------------------*/
/* HCI Event:       Connection Complete (5.2.3)                            */
/* Event Type:      BT_EV_HC_CONNECTION_COMPLETE                           */
/* Event struct:    bt_ev_hc_conn_cmplt_type                               */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8                 status;
  uint16                conn_hndl;
  bt_bd_addr_type       bd_addr;
  bt_link_type          link_type;
  bt_encrypt_mode_type  encryption_mode;
} bt_ev_hc_conn_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Connection Request (5.2.4)                             */
/* Event Type:      BT_EV_HC_CONNECTION_REQUEST                            */
/* Event struct:    bt_ev_hc_conn_request_type                             */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  bt_cod_type      class_of_device;
  bt_link_type     link_type;
} bt_ev_hc_conn_request_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Disconnection Complete (5.2.5)                         */
/* Event Type:      BT_EV_HC_DISCONNECTION_COMPLETE                        */
/* Event struct:    bt_ev_hc_disc_cmplt_type                               */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           status;
  uint16          conn_hndl;
  uint8           reason;
} bt_ev_hc_disc_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Authentication Complete (5.2.6)                        */
/* Event Type:      BT_EV_HC_AUTH_COMPLETE                                 */
/* Event struct:    bt_ev_hc_auth_cmplt_type                               */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           status;
  uint16          conn_hndl;
} bt_ev_hc_auth_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Remote Name Request Complete (5.2.7)                   */
/* Event Type:      BT_EV_HC_REMOTE_NAME_REQ_COMPLETE                      */
/* Event struct:    bt_ev_hc_remote_name_req_cmplt_type                    */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8            status;
  bt_bd_addr_type  bd_addr;
  dsm_item_type*   dsm_ptr;    /* contains 248 bytes of UTF-8 encode       */
                               /* characters; null terminated.             */
} bt_ev_hc_remote_name_req_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Encryption Change (5.2.8)                              */
/* Event Type:      BT_EV_HC_ENCRYPTION_CHANGE                             */
/* Event struct:    bt_ev_hc_encrypt_change_type                           */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           status;
  uint16          conn_hndl;
  uint8           encrypt_enable;  /* bt_encrypt_enable_type */
} bt_ev_hc_encrypt_change_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Change Connection Link Key Complete (5.2.9)            */
/* Event Type:      BT_EV_HC_CHANGE_CONN_KEY_COMPLETE                      */
/* Event struct:    bt_ev_hc_chng_conn_key_cmplt_type                      */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8   status;
  uint16  conn_hndl;
} bt_ev_hc_chng_conn_key_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Master Link Key Complete (5.2.10)                      */
/* Event Type:      BT_EV_HC_MASTER_LINK_KEY_COMPLETE                      */
/* Event struct:    bt_ev_hc_master_key_cmplt_type                         */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8                    status;
  uint16                   conn_hndl;
  bt_link_key_select_type  key_flag;
} bt_ev_hc_master_key_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Read Remote Supported Features Complete (5.2.11)       */
/* Event Type:      BT_EV_HC_RD_REMOTE_FEATURES_COMPLETE                   */
/* Event struct:    bt_ev_hc_rd_remote_feat_cmplt_type                     */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8                 status;
  uint16                conn_hndl;
  bt_lmp_features_type  lmp_features;
} bt_ev_hc_rd_remote_feat_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Read Remote Version Information Complete (5.2.12)      */
/* Event Type:      BT_EV_HC_RD_REMOTE_VERSION_COMPLETE                    */
/* Event struct:    bt_ev_hc_rd_remote_ver_cmplt_type                      */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           status;
  uint16          conn_hndl;
  uint8           lmp_version;
  uint16          manufacturer_name;
  uint16          lmp_subversion;
} bt_ev_hc_rd_remote_ver_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       QoS Setup Complete (5.2.13)                            */
/* Event Type:      BT_EV_HC_QOS_SETUP_COMPLETE                            */
/* Event struct:    bt_ev_hc_qos_setup_cmplt_type                          */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8                status;
  uint16               conn_hndl;
  uint8                flags;
  bt_qos_service_type  service_type;
  uint32               token_rate;       /* in units of bytes per second */
  uint32               peak_bandwidth;   /* in units of bytes per second */
  uint32               latency;          /* in units of microseconds     */
  uint32               delay_variation;  /* in units of microseconds     */
} bt_ev_hc_qos_setup_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Command Complete (5.2.14)                              */
/* Event Type:      BT_EV_HC_COMMAND_COMPLETE                              */
/* Event struct:    bt_ev_hc_cmd_cmplt_type                                */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED union
{
  bt_hc_rp_status_type                   status;             
  bt_hc_rp_bd_addr_type                  bd_addr;            
  bt_hc_rp_conn_hndl_type                conn_hndl;          
  bt_hc_rp_role_discovery_type           role_discovery;     
  bt_hc_rp_rd_link_policy_type           rd_link_policy;     
  bt_hc_rp_rd_pin_type                   rd_pin_type;        
  bt_hc_rp_rd_link_key_type              rd_link_key;        
  bt_hc_rp_wr_link_key_type              wr_link_key;        
  bt_hc_rp_del_link_key_type             del_link_key;       
  bt_hc_rp_rd_local_name_type            rd_local_name;      
  bt_hc_rp_rd_conn_accept_to_type        rd_conn_accept_to;  
  bt_hc_rp_rd_page_to_type               rd_page_to;         
  bt_hc_rp_rd_scan_enable_type           rd_scan_enable;     
  bt_hc_rp_rd_page_scan_act_type         rd_page_scan_act;   
  bt_hc_rp_rd_inq_scan_act_type          rd_inq_scan_act;    
  bt_hc_rp_rd_auth_enable_type           rd_auth_enable;     
  bt_hc_rp_rd_encrypt_mode_type          rd_encrypt_mode;    
  bt_hc_rp_rd_class_of_dev_type          rd_class_of_dev;    
  bt_hc_rp_rd_voice_setting_type         rd_voice_setting;   
  bt_hc_rp_rd_auto_flush_to_type         rd_auto_flush_to;   
  bt_hc_rp_rd_broad_retrans_type         rd_broad_retrans;   
  bt_hc_rp_rd_hold_mode_act_type         rd_hold_mode_act;   
  bt_hc_rp_rd_tx_power_type              rd_tx_power;        
  bt_hc_rp_rd_sco_flow_ctrl_type         rd_sco_flow_ctrl;   
  bt_hc_rp_rd_link_super_to_type         rd_link_super_to;   
  bt_hc_rp_rd_num_iac_type               rd_num_iac;         
  bt_hc_rp_rd_curr_iac_lap_type          rd_curr_iac_lap;    
  bt_hc_rp_rd_ps_per_mode_type           rd_ps_per_mode;
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
  bt_hc_rp_rd_ps_mode_type               rd_ps_mode;         
#endif
  bt_hc_rp_rd_local_ver_type             rd_local_ver;       
  bt_hc_rp_rd_local_feat_type            rd_local_feat;      
  bt_hc_rp_rd_buffer_size_type           rd_buffer_size;     
  bt_hc_rp_rd_country_code_type          rd_country_code;    
  bt_hc_rp_rd_failed_contact_type        rd_failed_counter;  
  bt_hc_rp_get_link_quality_type         rd_get_link_quality;
  bt_hc_rp_rd_rssi_type                  rd_rssi;            
  bt_hc_rp_rd_loopback_mode_type         rd_loopback_mode;   
#ifdef FEATURE_BT_1_2
  bt_hc_rp_rd_afh_chan_assess_mode_type  rd_afh_chan_assess_mode;
  bt_hc_rp_rd_afh_chan_map_type          rd_afh_chan_map;     
  bt_hc_rp_rd_inq_scan_type_type         rd_is_type;
  bt_hc_rp_rd_inq_mode_type              rd_inq_mode;
  bt_hc_rp_rd_page_scan_type_type        rd_ps_type;
#endif /* FEATURE_BT_1_2 */
#ifdef FEATURE_BT_2_1
  bt_hc_rp_sp_mode_type                  sp_mode;
  bt_hc_rp_oob_data_type                 oob_data;
  bt_hc_rp_rd_inq_rsp_tx_pwr             rd_inq_rsp_tx_pwr;
  bt_hc_rp_rd_ext_inq_rsp_type           rd_ext_inq_rsp_type;
#endif /* FEATURE_BT_2_1 */
} bt_ev_hc_cmd_cmplt_rp_type;

typedef BT_PACKED struct
{
  uint8                       num_hci_cmd_pkts;
  uint16                      cmd_opcode;
  bt_ev_hc_cmd_cmplt_rp_type  ret_params;
} bt_ev_hc_cmd_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Command Status (5.2.15)                                */
/* Event Type:      BT_EV_HC_COMMAND_STATUS                                */
/* Event struct:    bt_ev_hc_cmd_status_type                               */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           status;
  uint8           num_hci_cmd_pkts;
  uint16          cmd_opcode;
} bt_ev_hc_cmd_status_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Hardware Error (5.2.16)                                */
/* Event Type:      BT_EV_HC_HARDWARE_ERROR                                */
/* Event struct:    bt_ev_hc_hw_error_type                                 */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           hardware_code;
} bt_ev_hc_hw_error_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Flush Occured (5.2.17)                                 */
/* Event Type:      BT_EV_HC_FLUSH_OCCURRED                                */
/* Event struct:    bt_ev_hc_flush_occurred_type                           */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16          conn_hndl;
} bt_ev_hc_flush_occurred_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Role Change (5.2.18)                                   */
/* Event Type:      BT_EV_HC_ROLE_CHANGE                                   */
/* Event struct:    bt_ev_hc_role_change_type                              */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8            status;
  bt_bd_addr_type  bd_addr;
  bt_role_type     new_role;
} bt_ev_hc_role_change_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Number Of Completed Packets (5.2.19)                   */
/* Event Type:      BT_EV_HC_NUM_CMPLT_PKTS                                */
/* Event struct:    bt_ev_hc_num_cmplt_pkts_type                           */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8            num_handles;
  dsm_item_type*   dsm_ptr;

  /* dsm_ptr the equivalent of the following data:
  uint16           conn_hndl[ num_handles ];
  uint16           hc_num_cmplt_pkts[ num_handles ];
  */
} bt_ev_hc_num_cmplt_pkts_type;

#define BT_EV_HC_NUM_CMPLT_PKTS_ELEM_SIZE \
  ( sizeof( uint16 ) + sizeof( uint16 ) )

/*-------------------------------------------------------------------------*/
/* HCI Event:       Mode Change (5.2.20)                                   */
/* Event Type:      BT_EV_HC_MODE_CHANGE                                   */
/* Event struct:    bt_ev_hc_mode_change_type                              */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8              status;
  uint16             conn_hndl;
  bt_link_mode_type  current_mode;
  uint16             interval;    /* units: .625 ms.  range: 0x0000-0xFFFF */
} bt_ev_hc_mode_change_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Return Link Keys (5.2.21)                              */
/* Event Type:      BT_EV_HC_RETURN_LINK_KEYS                              */
/* Event struct:    bt_ev_hc_ret_key_type                                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8             num_keys;
  dsm_item_type*    dsm_ptr;

  /* dsm_ptr the equivalent of the following data:
  bt_bd_addr_type   bd_addr[ num_keys ];
  bt_link_key_type  link_key[ num_keys ];
  */
} bt_ev_hc_ret_key_type;

#define BT_EV_HC_RETURN_LINK_KEYS_ELEM_SIZE \
  ( sizeof( bt_bd_addr_type ) + sizeof( bt_link_key_type ) )

/*-------------------------------------------------------------------------*/
/* HCI Event:       PIN Code Request (5.2.22)                              */
/* Event Type:      BT_EV_HC_PIN_CODE_REQUEST                              */
/* Event struct:    bt_ev_hc_pin_request_type                              */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_ev_hc_pin_request_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Link Key Request (5.2.23)                              */
/* Event Type:      BT_EV_HC_LINK_KEY_REQUEST                              */
/* Event struct:    bt_ev_hc_key_request_type                              */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_ev_hc_key_request_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Link Key Notification (5.2.24)                         */
/* Event Type:      BT_EV_HC_LINK_KEY_NOTIFICATION                         */
/* Event struct:    bt_ev_hc_key_notif_type                                */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type     bd_addr;
  bt_link_key_type    link_key;
  bt_hc_link_key_type key_type_key;
} bt_ev_hc_key_notif_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Loopback Command (5.2.25)                              */
/* Event Type:      BT_EV_HC_LOOPBACK_COMMAND                              */
/* Event struct:    bt_ev_hc_loopback_cmd_type                             */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  // WAL This needs to be worked out
  boolean  unused;
} bt_ev_hc_loopback_cmd_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Data Buffer Overflow (5.2.26)                          */
/* Event Type:      BT_EV_HC_DATA_BUFFER_OVERFLOW                          */
/* Event struct:    bt_ev_hc_data_overflow_type                            */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_link_type    link_type;
} bt_ev_hc_data_overflow_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Max Slots Change (5.2.27)                              */
/* Event Type:      BT_EV_HC_MAX_SLOTS_CHANGE                              */
/* Event struct:    bt_ev_hc_max_slots_change_type                         */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16          conn_hndl;
  uint8           lmp_max_slots;
} bt_ev_hc_max_slots_change_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Read Clock Offset Complete (5.2.28)                    */
/* Event Type:      BT_EV_HC_RD_CLK_OFFSET_COMPLETE                        */
/* Event struct:    bt_ev_hc_rd_clk_offset_cmplt_type                      */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           status;
  uint16          conn_hndl;
  uint16          clk_offset;
} bt_ev_hc_rd_clk_offset_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Connection Packet Type Changed (5.2.29)                */
/* Event Type:      BT_EV_HC_CONN_PKT_TYPE_CHANGED                         */
/* Event struct:    bt_ev_hc_conn_pkt_type_chng_type                       */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           status;
  uint16          conn_hndl;
  uint16          pkt_type;   /* Use BT_PKT_TYPE_* #defines */
} bt_ev_hc_conn_pkt_type_chng_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       QoS Violation (5.2.30)                                 */
/* Event Type:      BT_EV_HC_QOS_VIOLATION                                 */
/* Event struct:    bt_ev_hc_qos_violation_type                            */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16          conn_hndl;
} bt_ev_hc_qos_violation_type;

#ifndef FEATURE_BT_1_2 /* BT 1.1 */
/*-------------------------------------------------------------------------*/
/* HCI Event:       Page Scan Mode Change (5.2.31)                         */
/* Event Type:      BT_EV_HC_PAGE_SCAN_MODE_CHANGE                         */
/* Event struct:    bt_ev_hc_ps_mode_chng_type                             */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type         bd_addr;
  bt_page_scan_mode_type  page_scan_mode;
} bt_ev_hc_ps_mode_chng_type;
#endif

/*-------------------------------------------------------------------------*/
/* HCI Event:       Page Scan Repetition Mode Change (5.2.32)              */
/* Event Type:      BT_EV_HC_PAGE_SCAN_REP_MODE_CHANGE                     */
/* Event struct:    bt_ev_hc_ps_rep_mode_chng_type                         */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
  bt_page_scan_rep_mode_type  ps_rep_mode;
} bt_ev_hc_ps_rep_mode_chng_type;

#ifdef FEATURE_BT_1_2


/*-------------------------------------------------------------------------*/
/* HCI Event:       Read Remote Extended Features Complete (7.7.34)        */
/* Event Type:      BT_EV_HC_RD_REMOTE_EXT_FEATURES_COMPLETE               */
/* Event struct:    bt_ev_hc_rd_remote_ext_feat_cmplt_type                 */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8                 status;
  uint16                conn_hndl;
  uint8                 page_number;
  uint8                 max_page_number;
  bt_lmp_features_type  ext_lmp_features;
} bt_ev_hc_rd_remote_ext_feat_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Synchronous Connection Complete (7.7.35)               */
/* Event Type:      BT_EV_HC_SYNC_CONN_COMPLETE                            */
/* Event struct:    bt_ev_hc_sync_conn_cmplt_type                          */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8                 status;
  uint16                conn_hndl;
  bt_bd_addr_type       bd_addr;
  bt_link_type          link_type;
  uint8                 tx_interval;
  uint8                 retx_window;
  uint16                rx_pkt_length;
  uint16                tx_pkt_length;
  bt_sco_air_mode_type  air_mode;
} bt_ev_hc_sync_conn_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Synchronous Connection Changed (7.7.36)                */
/* Event Type:      BT_EV_HC_SYNC_CONN_CHANGE                              */
/* Event struct:    bt_ev_hc_sync_conn_change_type                         */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8                 status;
  uint16                conn_hndl;
  uint8                 tx_interval;
  uint8                 retx_window;
  uint16                rx_pkt_length;
  uint16                tx_pkt_length;
} bt_ev_hc_sync_conn_change_type;

#endif /* FEATURE_BT_1_2 */

#ifdef FEATURE_BT_2_1

/*-------------------------------------------------------------------------*/
/* HCI Event:       Encryption Key Refresh Complete (7.7.39)               */
/* Event Type:      BT_EV_HC_ENCRYPTION_KEY_REFRESH_COMPLETE               */
/* Event struct:    bt_ev_hc_enc_key_ref_complt_type                       */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8                 status;
  uint16                conn_hndl;
} bt_ev_hc_enc_key_ref_complt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       IO Capability Request (7.7.40)                         */
/* Event Type:      BT_EV_HC_IO_CAPABILITY_REQUEST                         */
/* Event struct:    bt_ev_hc_io_cap_rqst_type                              */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
} bt_ev_hc_io_cap_rqst_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       IO Capability Response (7.7.41)                        */
/* Event Type:      BT_EV_HC_IO_CAPABILITY_RESPONSE                        */
/* Event struct:    bt_ev_hc_io_cap_res_type                               */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  uint8                 io_capability;
  uint8                 oob_data_present;
  uint8                 auth_req;
} bt_ev_hc_io_cap_res_type;
    
/*-------------------------------------------------------------------------*/
/* HCI Event:       User Confirmation Request (7.7.42)                     */
/* Event Type:      BT_EV_HC_USER_CONFIRMATION_REQUEST                     */
/* Event struct:    bt_ev_hc_user_cfmtn_rqst_type                          */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  uint32                num_val; /* must <=0x000F423F */
} bt_ev_hc_user_cfmtn_rqst_type;
 
/*-------------------------------------------------------------------------*/
/* HCI Event:       User Passkey Request (7.7.43)                          */
/* Event Type:      BT_EV_HC_USER_PASSKEY_REQUEST                          */
/* Event struct:    bt_ev_hc_user_passkey_rqst_type                        */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
} bt_ev_hc_user_passkey_rqst_type;
  
/*-------------------------------------------------------------------------*/
/* HCI Event:       Remote OOB Data Request (7.7.44)                       */
/* Event Type:      BT_EV_HC_REMOTE_OOB_DATA_REQUEST                       */
/* Event struct:    bt_ev_hc_remote_oob_data_rqst_type                     */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
} bt_ev_hc_remote_oob_data_rqst_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Simple Pairing Complete (7.7.45)                       */
/* Event Type:      BT_EV_HC_SIMPLE_PAIRING_COMPLETE                       */
/* Event struct:    bt_ev_hc_sp_complt_type                                */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8                 status;
  bt_bd_addr_type       bd_addr;
} bt_ev_hc_sp_complt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       User Passkey Notification (7.7.48)                     */
/* Event Type:      BT_EV_HC_USER_PASSKEY_NOTIFICATION                     */
/* Event struct:    bt_ev_hc_user_passkey_ntfctn_type                      */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  uint32                passkey; /* must <=0x000F423F */
} bt_ev_hc_user_passkey_ntfctn_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Keypress Notification (7.7.49)                         */
/* Event Type:      BT_EV_HC_KEYPRESS_NOTIFICATION                         */
/* Event struct:    bt_ev_hc_keypress_ntfctn_type                          */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  uint8                 notification_type;
} bt_ev_hc_keypress_ntfctn_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Remote Host Supported Features Notification (7.7.50)   */
/* Event Type:      BT_EV_HC_REMOTE_HOST_SUPPORTED_FEAT_NTFCTN             */
/* Event struct:    bt_ev_hc_rem_host_sup_feat_ntfctn_type                 */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_lmp_features_type  host_supported_feat;
} bt_ev_hc_rem_host_sup_feat_ntfctn_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Sniff Subrating (7.7.37)                               */
/* Event Type:      BT_EV_HC_SNIFF_SUBRATING                               */
/* Event struct:    bt_ev_hc_sniff_subrating_type                          */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8                 status;
  uint16                conn_hndl;
  uint16                max_tx_latency;
  uint16                max_rx_latency;
  uint16                min_remote_to;
  uint16                min_local_to;
} bt_ev_hc_sniff_subrating_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Link Supervision TO (7.7.46)                           */
/* Event Type:      BT_EV_HC_LINK_SUPERVISION_TIMEOUT_CHANGED              */
/* Event struct:    bt_ev_hc_lk_sup_to_changed_type                        */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16                conn_hdl;
  uint16                link_supervision_to;
} bt_ev_hc_lk_sup_to_changed_type;

#endif /* FEATURE_BT_2_1 */

/*-------------------------------------------------------------------------*/
/*                   QUALCOMM Specific HCI Events                          */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Event:       Read_Rx_Status_Result (QC)                             */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_RD_RX_STATUS_RESULT                        */
/* Event struct:    bt_ev_hc_qc_rd_rx_status_result_type                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
  uint32  rd_rx_status;
} bt_ev_hc_qc_rd_rx_status_result_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Read_Tx_Buffer_Result (QC)                             */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_RD_TX_BUFFER_RESULT                        */
/* Event struct:    bt_ev_hc_qc_rd_tx_buffer_result_type                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16            conn_hndl;
  uint16            pkt_hdr;
  uint16            payload_hdr;
} bt_ev_hc_qc_rd_tx_buffer_result_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Read_Rx_Buffer_Result (QC)                             */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_RD_RX_BUFFER_RESULT                        */
/* Event struct:    bt_ev_hc_qc_rd_rx_buffer_result_type                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16            conn_hndl;
  uint16            pkt_hdr;
  uint16            payload_hdr;
} bt_ev_hc_qc_rd_rx_buffer_result_type;

#ifdef FEATURE_BT_TEST_MODE_TESTER
/*-------------------------------------------------------------------------*/
/* HCI Event:       Tester_Activate_Result (QC)                            */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_TESTER_ACTIVATE_RESULT                     */
/* Event struct:    bt_ev_hc_qc_tester_activate_result_type                */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
  uint8   status;      /* 0 - success */
} bt_ev_hc_qc_tester_activate_result_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Tester_Control_Result (QC)                             */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_TESTER_CONTROL_RESULT                      */
/* Event struct:    bt_ev_hc_qc_tester_control_result_type                 */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
  uint8   status;      /* 0 - success */
} bt_ev_hc_qc_tester_control_result_type;
#endif /* FEATURE_BT_TEST_MODE_TESTER */

/*-------------------------------------------------------------------------*/
/* HCI Event:       Timing_Accuracy_Response (QC)                          */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_TIME_ACCURACY_RESP                         */
/* Event struct:    bt_ev_hc_qc_time_accuracy_resp_type                    */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  jitter;
  uint8  drift;
} bt_ev_hc_qc_time_accuracy_resp_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Read_Clock_Response (QC)                               */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_RD_CLK_RESP                                */
/* Event struct:    bt_ev_hc_qc_rd_clk_resp_type                           */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  dsp_clk;
  uint16  sw_clk;
} bt_ev_hc_qc_rd_clk_resp_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Read_SBI_Register_Complete (QC)                        */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_RD_SBI_REG_CMPLT                           */
/* Event struct:    bt_ev_hc_qc_rd_sbi_reg_cmplt_type                      */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  status;
  uint8  value;
} bt_ev_hc_qc_rd_sbi_reg_cmplt_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Read_Encryption_Key_Length (QC)                        */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_RD_ENC_KEY_LENGTH                          */
/* Event struct:    bt_ev_hc_qc_rd_enc_key_length_type                     */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  min_enc_key_size;
  uint8  max_enc_key_size;
} bt_ev_hc_qc_rd_enc_key_length_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Read_Raw_RSSI (QC)                                     */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_RD_RAW_RSSI                                */
/* Event struct:    bt_ev_hc_qc_rd_raw_rssi_type                           */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8   status;
  uint16  conn_hndl;
  int8    raw_rssi;   /* (in units of dBm) */
} bt_ev_hc_qc_rd_raw_rssi_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Read_RAM (QC)                                          */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_RD_RAM                                     */
/* Event struct:    bt_ev_hc_qc_rd_ram_type                                */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8   status;
  uint8   num_bytes;
  uint32  addr;
  uint32  data;
} bt_ev_hc_qc_rd_ram_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Remote_Auth_Or_Pair_Failed (QC)                        */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_REM_AUTH_OR_PAIR_FAILED                    */
/* Event struct:    bt_ev_hc_qc_rem_auth_pair_failed_type                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type     bd_addr;
  bt_error_code_type  error;
} bt_ev_hc_qc_rem_auth_pair_failed_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Extended_Cmd_Status (QC)                               */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_EXTENDED_CMD_STATUS                        */
/* Event struct:    bt_ev_hc_qc_extended_cmd_status                        */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_ev_hc_cmd_status_type  norm_cs;
  bt_bd_addr_type           bd_addr;
  uint16                    conn_hndl;
} bt_ev_hc_qc_extended_cmd_status_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Page_Start (QC)                                        */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_RADIO_ACTIVITY_CHANGED                     */
/* Event struct:    bt_ev_hc_qc_radio_activity_changed                     */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_radio_activity_type  ra;
} bt_ev_hc_qc_radio_activity_changed;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Cur_Conn_Encryption (QC)                               */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* QC Event Type:   BT_EV_HC_QC_CUR_CONN_ENCRYPT                           */
/* Event struct:    bt_ev_hc_qc_cur_conn_encrypt_type                      */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
  uint8   encrypt_enable;
  uint8   cur_enc_key_len_bytes;
} bt_ev_hc_qc_cur_conn_encrypt_type;

#ifdef FEATURE_BT_2_1
/*-------------------------------------------------------------------------*/
/* HCI Event:       Enhanced Flush Complete                                */
/* Event Type:      BT_EV_HC_ENH_FLUSH_CMPLT                               */
/* Event struct:    bt_ev_hc_enh_flush_cmplt_type                          */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16 conn_hndl;
} bt_ev_hc_enh_flush_cmplt_type;
#endif /* FEATURE_BT_2_1 */

#ifndef FEATURE_BT_SOC
typedef enum
{
  BT_EV_HC_QC_RD_RX_STATUS_RESULT      = 0,
  BT_EV_HC_QC_RD_TX_BUFFER_RESULT      = 1,
  BT_EV_HC_QC_RD_RX_BUFFER_RESULT      = 2,
  BT_EV_HC_QC_TESTER_ACTIVATE_RESULT   = 3,
  BT_EV_HC_QC_TESTER_CONTROL_RESULT    = 4,
  BT_EV_HC_QC_TIME_ACCURACY_RESP       = 5,
  BT_EV_HC_QC_RD_CLK_RESP              = 7,
  BT_EV_HC_QC_RD_SBI_REG_CMPLT         = 8,
  BT_EV_HC_QC_RD_ENC_KEY_LENGTH        = 9,
  BT_EV_HC_ASDF                        = 10,
  BT_EV_HC_QC_RD_RAW_RSSI              = 11,
  BT_EV_HC_QC_RD_RAM                   = 12,
  BT_EV_HC_QC_REM_AUTH_OR_PAIR_FAILED  = 14,
  BT_EV_HC_QC_EXTENDED_CMD_STATUS      = 15,
  BT_EV_HC_QC_RADIO_ACTIVITY_CHANGED   = 16,
  BT_EV_HC_QC_CUR_CONN_ENCRYPT         = 17,
  BT_EV_HC_QC_MAX
} bt_ev_hc_qualcomm_ev_type;

/*-------------------------------------------------------------------------*/
/* HCI Event:       Qualcomm (Vendor-specific event)                       */
/* Event Type:      BT_EV_HC_QUALCOMM                                      */
/* Event struct:    bt_ev_hc_qualcomm_type                                 */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_ev_hc_qualcomm_ev_type  qc_event;
  BT_PACKED union
  {
    bt_ev_hc_qc_rd_rx_status_result_type     rd_rx_status_result;
    bt_ev_hc_qc_rd_tx_buffer_result_type     rd_tx_buffer_result;
    bt_ev_hc_qc_rd_rx_buffer_result_type     rd_rx_buffer_result;
#ifdef FEATURE_BT_TEST_MODE_TESTER
    bt_ev_hc_qc_tester_activate_result_type  tester_activate_result;
    bt_ev_hc_qc_tester_control_result_type   tester_control_result;
#endif /* FEATURE_BT_TEST_MODE_TESTER */
    bt_ev_hc_qc_time_accuracy_resp_type      time_acc_resp;
    bt_ev_hc_qc_rd_clk_resp_type             rd_clk_resp;
    bt_ev_hc_qc_rd_sbi_reg_cmplt_type        rd_sbi_reg_cmplt;
    bt_ev_hc_qc_rd_enc_key_length_type       rd_enc_key_length;
    bt_ev_hc_qc_rd_raw_rssi_type             rd_raw_rssi;
    bt_ev_hc_qc_rd_ram_type                  rd_ram;
    bt_ev_hc_qc_rem_auth_pair_failed_type    rmapf;
    bt_ev_hc_qc_extended_cmd_status_type     extcs;
    bt_ev_hc_qc_radio_activity_changed       ract_chgd;
    bt_ev_hc_qc_cur_conn_encrypt_type        ccenc;
  } qc_event_params;
} bt_ev_hc_qualcomm_type;
#endif /* !FEATURE_BT_SOC */

#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

#ifdef FEATURE_BT_QSOC
/*-------------------------------------------------------------------------*/
/*                 BTS Related Vendor Specific HCI Event                   */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  uint8           sub_event;
  dsm_item_type   *evt_data;
} bt_ev_hc_vs_var_type;

typedef BT_PACKED struct
{
  uint16  conn_hndl;
  uint8   ekl;
}bt_ev_hc_vs_ekl_params_type;

typedef BT_PACKED union
{
  bt_ev_hc_vs_ekl_params_type ekl;
  /* TBD - add others in same family */
} bt_ev_hc_vs_cmd_params_type;

typedef BT_PACKED struct
{
  uint8                       num_hci_cmd_pkts;
  uint16                      cmd_opcode;
  uint8                       status;
  uint8                       sub_event;
bt_ev_hc_vs_cmd_params_type   params;
} bt_ev_hc_vs_cmd_cmplt_type;
#endif /* FEATURE_BT_QSOC */

#endif /* FEATURE_BT */

#endif /* _BTEVI_H */


