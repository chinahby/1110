#ifndef _BTCMDI_H
#define _BTCMDI_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    I N T E R N A L    C O M M A N D S

DESCRIPTION
  This file contains declarations of commands internal to the Bluetooth
  driver.  These are the structures passed into the driver.  Applications
  using the driver call "wrapper" functions that fill out the structures
  and pass them to the BT driver.  The prototypes for these command
  function are in bt.h and the code implementing them is in btcmd.c

              Copyright (c) 2000-2009 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary                        
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btcmdi.h_v   1.35   05 Aug 2002 17:59:04   ryuen
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btcmdi.h#3 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btcmdi.h_v  $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-05-12   pg  Added support for bt_cmd_rm_register_link_status_ext() API.
  2009-02-18   hm  Created an API for Bond Cancel
  2008-11-05   pg  Generic handling for HCI Vendor Specific cmd group.
  2008-09-09   NM  Added event to handle retrieval of link key for automation
  2008-08-14   rb  Added support for FEATURE_BT_DCVS
  2008-03-24   vk  Added struct for HCI Read/Write Defalut Erroneous Data 
                   Reporting mode command.
                   
 *
 *   #64        05 Dec 2007            AG
 * Added #define for length of wr eir hci command.
 *
 *   #63        29 Nov 2007            HM
 * Added SD command to return service search results in XML format
 * Added command to cancel service search. 
 * Added a parameter to specify duration of a device search. 
 * 
 *   #62         07 Nov 2007            GS
 * Added bt_cmd_sd_cancel_idle_timer_type command.
 *
 *   #61         01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *   #60         19 Oct 2007            DSN\SSK
 * Add support for Controller to Host HCI flow control.
 *
 *   #59         16 Oct 2007            DSN
 * Added support for Rd Loc Supported commands and Rd local extended 
 * features command.
 *
 *    #58        11 SEP 2007            VK
 * Added Proto type for freeing command buffer.
 *
 *    #57        17 JUL 2007            PR
 * Remote name request cancel support in SD
 *
 *   #56        09 JUL 2007            PG
 * HCI Remote name req cancel and connection cancel support.
 *
 *    #55        02 Mar  2007           JN
 * Added structs for vendor specific commands used for BTS.
 *
 *    #54        29 Jan  2007           MH
 * Added support to update OBEX Profile service record with 128-bit UUID.
 *
 *    $53        03 Dec 2006            JTL
 * Added bt_cmd_rc_cancel_connect_type.
 *
 *    #52        22 Nov 2006            DM
 * Added HCI Flush Command.
 *
 *    #51        14 Nov 2006            RY
 * Added SD support for custom 128-bit UUID
 *
 *    #50        31 Oct  2006           VK
 * Fixing compilation conflict of bt_rm_atzrq_type enum.
 *
 *    #49        26 Oct 2006            VK
 * Added one parameter in bt_cmd_rm_enforce_sec_l2cap_psm_type structure.
 *
 *    #48        20 Oct 2006            SSK
 * External SOC suppport: H4DS and BCCMD modules.
 *
 *    #47        13 Sep 2006            MP
 * Added the L2CAP local_cid field to bt_cmd_hc_tx_acl_data_type and 
 * bt_cmd_rm_tx_acl_data_type structure.
 *
 *    #46        26 Jul 2006            DM
 * Added frame handle to l2cap data write commands.
 * Changed l2cap packet boundary flag to have new define for start and 
 * continuation of flushable packets
 *
 *    #45        24 Jun 2006            DSN
 * Removed Bonding App(BA) related references that are no longer used.
 *
 *    #44        01 Jun 2006            JTL
 * Add CID to L2Cap Mode 2 security.
 *
 *    #43        14 Mar 2006            JH
 * Modified to support updated bt_cmd_rm_connect_sco() command.
 * 
 *    #42        01 Dec 2005            PG
 * Added a offset define into bt_cmd_hc_tester_control_type to
 * keep lint happy while passing void * parameter.
 *
 *    #41        23 Nov 2005            DP
 * Added RM commands for White List and Extended Service Security.
 *
 *    #40        9  Nov 2005            DP
 * Added connectable field to SD service registration commands.
 *
 *    #39        19 Oct 2005            JH
 * Added support for the HCI read remote extended features command.
 *
 *    #38        26 Aug 2005            PN
 * Mainlined support for Response & Hold feature of HFP1.5.
 *
 *    #37        29 Jul 2005            PN
 * Mainlined support of battery charge and RSSI indicators.
 *
 *    #36        27 Jun 2005           JH
 * Added new BT 1.2 commands: Read_Inquiry_Scan_Type, Write_Inquiry_Scan_Mode,
 *  Read_Inquiry_Mode, and Write_Inquiry_Mode
 * Replaced BT 1.1 commands: Read_Page_Scan_Mode and Write_Page_Scan_Mode with
 *  BT 1.2 commands: Read_Page_Scan_Type and Write_Page_Scan_Type.
 *
 *    #35        02 May 2005           JTL
 * Adding command to disable the L2 initial timeout.
 *
 *    #34        10 Apr 2005            RY
 * Adding support for SD Get Discoverable Mode.
 *
 *    #33        06 Apr 2005            JH
 * Adding support for disabling BT radio.
 *
 *    #32        16 Mar 2005            DP
 * Fix Mode 2 security for client connections.
 *
 *    #31        05 Mar 2005            JH
 * Added support for AFH related RM commands.
 *
 *    #30        29 Dec 2004            JH
 * Added bt_cmd_rm_auto_serv_search_disable and bt_cmd_rm_dev_serv_update_type.
 *
 *    #29        29 Dec 2004            JH
 * Modified bt_cmd_sd_update_service_record_type, bt_cmd_sd_register_service_type,
 *  bt_cmd_sd_unregister_service_type, bt_cmd_sd_update_obex_profile_service_rec_type
 *
 *    #27-28     16 Dec 2004            JTL
 * Added SIO commands all IOCTLs and transmit.
 *
 *    #25        30 Aug 2004            JH
 * Modified bt_sd_srv_srch_pat_type to support 128-bit UUID
 * Added command structures for the following 4 commands:
 *  bt_cmd_sd_update_obex_profile_service_record
 *  bt_cmd_sd_register_syncml_service
 *  bt_cmd_sd_unregister_syncml_service
 *
 *    #24        25 Aug 2004            DP
 * Added battery charge and signal strength indicator setting commands
 * to AG.  Removed featurization of BT HF.
 *
 *    #23        12 Aug 2004            JH
 * Added TX continuous frequency HCI command
 *
 *    #21        7  Jul 2004            JH
 * Added BT 1.2 AFH and eSCO related command structs.   
 *
 *    #20        1  Jun 2004            DP
 * Implemented Mode 2 security and AG External I/O features.
 *
 *    #19        12 Apr 2004            PN
 * Added bt_cmd_ag_send_pb_entries_info_type.
 *
 *    #18        2  Apr 2004            DP
 * Added EMS support.
 *
 *    #17        25 Mar 2004            PN
 * - added more support for feature BT_AG_HF_EXTENSION
 *
 *    #16        12 Jan 2004            PN
 * Added bt_cmd_ag_update_vr_state_type.
 *
 *    #15        19 Dec 2003            DP
 * Added RM command to allow role switch requests from remote devices.
 * Fixed naming convention on LMP policy definitions.
 *
 *    #14        5  Nov 2003            DP
 * Fixed compilation error when test API not enabled and added this comment
 * omitted on previous revision #13.
 *
 *    #12        4  Nov 2003            DP
 * Added security APIs to RM.  General cleanup.
 *
 *    #11        26 Sep 2003            DP
 * Updated RM Device Manager API, AG connect/disconnect and VR capable APIs,
 * and made service/device names character strings.
 * 
 *    #10        06 Aug 2003            PN
 * Added support for RM Device DB.
 *
 *    #9         29 Jul 2003            RY
 * Added 'park_supported' to bt_cmd_hc_test_set_lm_params_type. 
 * 
 *    #8         12 Jun 2003            PN
 * - Added bt_cmd_ag_set_inband_ring().
 * - Added support for BT_AG_HF_EXTENSION.
 *
 *    Rev 1.35   05 Aug 2002 17:59:04   ryuen
 * Merged changes from MSM_BT.01.00.11.01.02 (MSM5100 2.1 Patch 6)
 * 
 *    Rev 1.34   12 Jul 2002 15:59:52   waynelee
 * Added new param to enable cert lpm testing.
 * 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "dsm.h"
#include "queue.h"
#include "sio.h"
#include "bt.h"
#include "bti.h"

#ifdef FEATURE_BT_SOC
#include "btsoc.h"
#endif /* FEATURE_BT_SOC */


/*-------------------------------------------------------------------------*/
/*             Driver Configuration and Event Control Events               */
/*                          (API Tester only)                              */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */


/*-------------------------------------------------------------------------*/
/*                      Audio Gateway (AG) Commands                        */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_BT_AG
typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_audio_device_type  audio_device;
} bt_cmd_ag_enable_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type       bd_addr;
  bt_audio_device_type  audio_device;
} bt_cmd_ag_connect_type;

typedef BT_PACKED struct
{
  bt_ag_idle_mode_type  idle_mode;
  uint16                idle_timeout;
} bt_cmd_ag_config_type;

typedef BT_PACKED struct
{
  boolean capable;
} bt_cmd_ag_set_vr_capable_type;

typedef BT_PACKED struct
{
  boolean vr_enabled;
} bt_cmd_ag_update_vr_state_type;

typedef BT_PACKED struct
{
  uint16  max_rings;
  uint16  ring_period;
} bt_cmd_ag_ring_audio_device_type;

typedef BT_PACKED struct
{
  uint16  max_time;
} bt_cmd_ag_audio_connect_type;

typedef BT_PACKED struct
{
  uint8   ad_speaker_gain;
} bt_cmd_ag_ad_spkr_gain_type;

typedef BT_PACKED struct
{
  uint8   ad_microphone_gain;
} bt_cmd_ag_ad_mic_gain_type;

typedef BT_PACKED struct
{
  boolean              enable_external_io;
  dsm_watermark_type*  rx_wm_ptr;
  dsm_watermark_type*  tx_wm_ptr;
  bt_ag_int_cmd_type   int_cmd_off_mask;
} bt_cmd_ag_ext_io_config_type;

typedef BT_PACKED struct
{
  boolean inband_enabled;
} bt_cmd_ag_set_inband_ring_type;

typedef BT_PACKED struct
{
  bt_ag_battchg_level_type  battchg;
} bt_cmd_ag_set_battchg_ind_type;

typedef BT_PACKED struct
{
  bt_ag_rssi_level_type  rssi;
} bt_cmd_ag_set_rssi_type;

typedef BT_PACKED struct
{
  boolean supported_rh[ BT_AG_RH_MAX ];
} bt_cmd_ag_send_supported_rh_type;

typedef BT_PACKED struct
{
  boolean               is_in_rh;
  bt_ag_rh_state_type  current_rh;
} bt_cmd_ag_send_current_rh_type;

#ifdef BT_AG_HF_EXTENSION
typedef BT_PACKED struct
{
  uint8               num_supported_phonebooks;
  bt_pap_device_type  supported_phonebooks[BT_PAP_DEV_MAX];
} bt_cmd_ag_send_pb_list_type;

typedef BT_PACKED struct
{
  bt_pap_device_type  pb_id;
  uint16              used_recs;
  uint16              total_recs;
} bt_cmd_ag_send_pb_info_type;

typedef BT_PACKED struct
{
  bt_pap_device_type  pb_id;
  bt_pap_return_type  result;
  uint16              start_idx;
  uint16              end_idx;
  uint16              max_num_len;
  uint16              max_text_len;
} bt_cmd_ag_send_pb_entries_info_type;

typedef BT_PACKED struct
{
  bt_pap_device_type  pb_id;
  bt_pap_return_type  result;
} bt_cmd_ag_send_pb_select_result_type;

typedef BT_PACKED struct
{
  bt_event_type       event;  /* event that prompted this pb read */
  bt_pap_device_type  pb_id;
  bt_pap_return_type  result;
  bt_pap_record_type  record;
} bt_cmd_ag_send_pb_read_result_type;

typedef BT_PACKED struct
{
  bt_pap_device_type  pb_id;
  bt_pap_return_type  result;
} bt_cmd_ag_send_pb_write_result_type;

typedef BT_PACKED struct
{
  bt_pap_subsc_num_info_type sni[3];  /* sni=subscriber number info */
} bt_cmd_ag_send_subsc_num_info_type;
#endif /* BT_AG_HF_EXTENSION */
#endif /* FEATURE_BT_AG */


/*-------------------------------------------------------------------------*/
/*               Service Discovery Protocol (SDP) Commands                 */
/*-------------------------------------------------------------------------*/

/* Enumeration for the two possible SDP database - local or the one to
 * store service search results of a SDP request  */
typedef enum
{
  BT_SD_DB_TYPE_LOCAL_DB,
  BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB
} bt_sd_db_enum_type;

/* Definition for a service search pattern */
typedef BT_PACKED struct
{
  uint16             pdu_type;
  uint16             max_srv_rec_count;
  uint16             max_attr_byte_count;

  uint32             srv_rec_handle;

  uint8              num_uuid;
  bt_sd_uuid_type    uuid[ BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID ];
  uint8              num_uuid128;
  bt_sd_uuid128_type uuid128[ BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_UUID128 ];
  uint8              syncml_flag;

  uint16             num_attr_id;
  BT_PACKED struct
  {
    boolean          is_range;
    uint32           value;
  } attr_id[ BT_SD_MAX_NUM_OF_SRV_SRCH_PAT_ATTR_ID ];

} bt_sd_srv_srch_pat_type;

typedef BT_PACKED struct
{
  bt_sd_service_discoverable_mode_type  srv_discv_mode;
} bt_cmd_sd_set_service_discoverable_mode_type;

typedef BT_PACKED struct
{
  boolean                    unused;
} bt_cmd_sd_get_discoverable_mode_type;

typedef BT_PACKED struct
{
  uint16                     service_class_bitmap;
  uint16                     max_num_response;
  uint8                      duration_secs;
} bt_cmd_sd_start_device_discovery_type;

typedef BT_PACKED struct
{
  bt_cmd_sd_start_device_discovery_type dev_discv;
  bt_bd_addr_type                       bd_addr;
} bt_cmd_sd_discover_devices_type;

typedef BT_PACKED struct
{
  boolean                    unused;
} bt_cmd_sd_stop_device_discovery_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type            bd_addr;
} bt_cmd_sd_get_device_name_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type            bd_addr;
} bt_cmd_sd_stop_service_search_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type            bd_addr;
} bt_cmd_sd_get_device_name_cancel_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type            bd_addr;
  boolean                    generate_xml;
  bt_sd_srv_srch_pat_type    srv_srch_pat;
} bt_cmd_sd_search_service_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type            bd_addr;
} bt_cmd_sd_cancel_idle_timer_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type            bd_addr;
  uint16                     service_class;
} bt_cmd_sd_get_server_channel_number_type;

typedef BT_PACKED struct
{
  bt_sd_uuid_type        uuid;
  uint8                  scn;
  uint8                  new_scn;
  uint16                 psm;
  uint16                 new_psm;
  char                   service_name_str[ BT_SD_MAX_SERVICE_NAME_LEN + 1 ];
} bt_cmd_sd_update_service_record_type;

#ifdef FEATURE_BT_EXTPF
typedef BT_PACKED struct
{
  bt_sd_uuid_type       uuid;
  bt_sd_uuid128_type    uuid128;
  uint8                 scn;
  char                  service_name_str[ BT_SD_MAX_SERVICE_NAME_LEN + 1 ]; 
  bt_sd_uuid128_type    service_id;
  bt_sd_uint8_list_type supported_formats_list;
  uint8                 supported_capabilities;
  uint16                supported_features;
  uint32                supported_functions;
  uint64                total_image_capacity;
} bt_cmd_sd_update_obex_profile_service_rec_type;
#endif /* FEATURE_BT_EXTPF */

typedef BT_PACKED struct
{
  bt_sd_uuid_type        uuid;
  uint16                 profile_version; 
  uint8                  scn;
  uint16                 psm;
  boolean                connectable;
  char                   service_name_str[ BT_SD_MAX_SERVICE_NAME_LEN + 1 ];
} bt_cmd_sd_register_service_type;

typedef BT_PACKED struct
{
  bt_sd_uuid_list_type   uuid_list;
  uint16                 profile_version; 
  uint8                  scn;
  uint16                 psm;
  boolean                connectable;
  char                   service_name_str[ BT_SD_MAX_SERVICE_NAME_LEN + 1 ];
} bt_cmd_sd_register_custom_service_type;

typedef BT_PACKED struct
{
  bt_sd_uuid_type        uuid;
  uint8                  scn;
  uint16                 psm;
} bt_cmd_sd_unregister_service_type;

typedef BT_PACKED struct
{
  bt_sd_uuid128_type     uuid128;
} bt_cmd_sd_unregister_custom_service_type;

#ifdef FEATURE_BT_EXTPF_SML
typedef BT_PACKED struct
{
  boolean                client;
  uint8                  scn;
  char                   service_name_str[ BT_SD_MAX_SERVICE_NAME_LEN + 1 ];
} bt_cmd_sd_register_syncml_service_type;

typedef BT_PACKED struct
{
  boolean                client;
} bt_cmd_sd_unregister_syncml_service_type;
#endif /* FEATURE_BT_EXTPF_SML */

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

/*-------------------------------------------------------------------------*/
/*       Serial Port IO (SIO) Commands                                     */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
  sio_ioctl_cmd_type     cmd;
} bt_cmd_sio_in_flow_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
  dsm_item_type*         dsm_ptr;
} bt_cmd_sio_transmit_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
  sio_stream_mode_type   stream_mode;
  dsm_watermark_type    *tx_queue;
  dsm_watermark_type    *rx_queue;
  sio_rx_func_ptr_type   rx_func_ptr;

} bt_cmd_sio_change_mode_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
} bt_cmd_sio_enable_autodetect_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
  sio_flow_ctl_type      tx_flow;
  sio_flow_ctl_type      rx_flow;
} bt_cmd_sio_set_flow_ctl_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
  sio_vv_func_ptr_type   enable_dte_ready_event;
} bt_cmd_sio_enable_dtr_event_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
} bt_cmd_sio_disable_dtr_event_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
} bt_cmd_sio_cd_assert_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
} bt_cmd_sio_cd_deassert_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
} bt_cmd_sio_ri_assert_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
} bt_cmd_sio_ri_deassert_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
} bt_cmd_sio_dsr_assert_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
} bt_cmd_sio_dsr_deassert_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
  sio_vv_func_ptr_type   func_ptr;
} bt_cmd_sio_record_pkt_func_ptr_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
  sio_vv_func_ptr_type   func_ptr;
} bt_cmd_sio_record_esc_func_ptr_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
  uint8                  baudrate;
  uint8                  format;
  uint8                  flow_ctrl;
  uint8                  xon_char;
  uint8                  xoff_char;
} bt_cmd_sio_bt_configure_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
} bt_cmd_sio_bt_disconnect_type;

typedef BT_PACKED struct
{
  sio_stream_id_type     stream_id;
  sio_vv_func_ptr_type   func_ptr;
} bt_cmd_sio_bt_flush_tx_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */


/*-------------------------------------------------------------------------*/
/*                             RFCOMM Commands                             */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  uint8                server_channel;
  dsm_watermark_type*  tx_wm_ptr;
  dsm_watermark_type*  rx_wm_ptr;
} bt_cmd_rc_register_type;

typedef BT_PACKED struct
{
  uint8                server_channel;
  bt_bd_addr_type      bd_addr;
  dsm_watermark_type*  tx_wm_ptr;
  dsm_watermark_type*  rx_wm_ptr;
} bt_cmd_rc_connect_type;

typedef BT_PACKED struct
{
  uint8                server_channel;
  bt_bd_addr_type      bd_addr;
  dsm_watermark_type*  tx_wm_ptr;
  dsm_watermark_type*  rx_wm_ptr;
  boolean              pn_negot_done;
  boolean              rpn_negot_done;
  uint16               max_frame_size;
  uint8                priority;
  boolean              cb_flow_ctrl;
  uint8                baudrate;
  uint8                format;
  uint8                flow_ctrl;
  uint8                xon_char;
  uint8                xoff_char;
} bt_cmd_rc_conn_custom_cfg_type;

typedef BT_PACKED struct
{
  uint16  pid;
} bt_cmd_rc_tx_data_type;

typedef BT_PACKED struct
{
  uint16   pid;
  boolean  enable_flow;
} bt_cmd_rc_set_port_rx_fc_type;

typedef BT_PACKED struct
{
  uint16   pid;
  boolean  enable_flow;
} bt_cmd_rc_set_session_rx_fc_type;

typedef BT_PACKED struct
{
  uint16   pid;
  uint8    ms_valid_mask;
  uint8    modem_status;
  boolean  break_present;
  uint8    break_length;
} bt_cmd_rc_write_ms_type;

typedef BT_PACKED struct
{
  uint16  pid;
  uint8   baudrate;
  uint8   format;     /*  Corresponds to value3 octet in RPN.  */
  uint8   flow_ctrl;
  uint8   xon_char;
  uint8   xoff_char;
} bt_cmd_rc_write_sp_cfg_type;

typedef BT_PACKED struct
{
  uint16   pid;
  boolean  overrun_error;
  boolean  parity_error;
  boolean  framing_error;
} bt_cmd_rc_line_error_type;

typedef BT_PACKED struct
{
  uint16  pid;
  uint16  byte_count;
  uint8   start_value;
  uint8   increment;
} bt_cmd_rc_ping_type;

typedef BT_PACKED struct
{
  uint16  pid;
} bt_cmd_rc_disconnect_type;

typedef BT_PACKED struct
{
  uint8  server_channel;
} bt_cmd_rc_unregister_type;

typedef BT_PACKED struct
{
  uint8           server_channel;
  bt_bd_addr_type bd_addr;
} bt_cmd_rc_cancel_connect_type;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */


/*-------------------------------------------------------------------------*/
/*                             L2CAP Commands                              */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  uint16          out_flush_to;
  uint16          in_mtu;
  bt_l2_qos_type  out_qos;
} bt_l2_psm_param_type;

typedef BT_PACKED struct
{
  uint16                cid;
  uint16                psm;
  bt_l2_psm_param_type  psm_params;
} bt_cmd_l2_set_params_type;

typedef BT_PACKED struct
{
  uint16  psm;
} bt_cmd_l2_register_type;

typedef BT_PACKED struct
{
  uint16           psm;
  bt_bd_addr_type  bd_addr;
} bt_cmd_l2_connect_type;

typedef BT_PACKED struct
{
  uint16          cid;
  dsm_item_type*  dsm_ptr;
  void*           frame_handle;
} bt_cmd_l2_write_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  uint16           length;
  uint8            data[ BT_L2_MAX_ECHO_DATA_LEN ];
} bt_cmd_l2_ping_type;

typedef BT_PACKED struct
{
  uint16  cid;
} bt_cmd_l2_disconnect_type;

typedef BT_PACKED struct
{
  uint16  psm;
} bt_cmd_l2_unregister_type;

typedef BT_PACKED struct
{
  uint16           psm;
  bt_bd_addr_type  bd_addr;
  uint16           cid;
} bt_cmd_l2_connect_immediate_cid_type;

typedef BT_PACKED struct
{
  uint16                cid;
} bt_cmd_l2_disable_timeout_type;

/*-------------------------------------------------------------------------*/
/*                       Radio Manager (RM) Commands                       */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint32                module;
  bt_module_state_type  state;
} bt_cmd_rm_update_module_st_type;

typedef BT_PACKED struct
{
  boolean      cod_valid;
  bt_cod_type  class_of_device;
  boolean      bt_name_valid;
  char         bt_name_str[ BT_MAX_LOCAL_DEVICE_NAME_LEN + 1 ];
} bt_cmd_rm_set_local_info_type;

#ifdef BT_SWDEV_2_1_EIR
typedef BT_PACKED struct
{
  boolean      cod_valid;
  bt_cod_type  class_of_device;
  boolean      bt_name_valid;
  char         bt_name_str[ BT_MAX_LOCAL_DEVICE_NAME_LEN + 1 ];
  boolean      bt_short_name_valid;
  char         bt_short_name_str[ BT_MAX_EIR_NAME_LEN + 1 ];
} bt_cmd_rm_set_local_info_ext_type;

typedef BT_PACKED struct
{
  uint8        eir_data_type;
  uint8        eir_data_size;
  uint8        eir_data[ BT_MAX_EIR_DATA_SIZE ];
} bt_cmd_rm_wr_eir_data_type;

typedef BT_PACKED struct
{
  bt_uuid_type uuid_type;
  boolean      uuid_list_complete;
  uint8        uuid_data_size;
  uint8        uuid_data[ BT_MAX_EIR_UUID_DATA_SIZE ];
} bt_cmd_rm_wr_eir_uuid_type;
#endif /* BT_SWDEV_2_1_EIR */

typedef BT_PACKED struct
{
  boolean                remove_app_config;
  bt_rm_act_vs_pwr_type  page_scan_act_vs_pwr;
  bt_rm_act_vs_pwr_type  inq_scan_act_vs_pwr;
  boolean                deliver_dev_updates;
} bt_cmd_rm_config_application_type;

typedef BT_PACKED struct
{
  boolean  disable;
} bt_cmd_rm_auto_serv_search_disable;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  boolean          security_changes;
  boolean          mode_changes;
  boolean          role_switches;
  boolean          new_acl_pkts;
  boolean          new_sco_pkts;
  uint16           rssi_delta;
  boolean          sync_param_updates;
} bt_cmd_rm_reg_link_status_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_cmd_rm_unreg_link_status_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type         bd_addr;
  bt_low_power_ctrl_type  low_power_ctrl;    
  uint16                  max_interval_ms;
  uint16                  idle_timeout_ms;
} bt_cmd_rm_low_power_config_type;

#ifdef BT_SWDEV_2_1_SSR
typedef BT_PACKED struct
{
  bt_bd_addr_type         bd_addr;
  bt_low_power_ctrl_type  low_power_ctrl;    
  uint16                  max_interval_ms;
  uint16                  idle_timeout_ms;
  uint16                  max_latency_ms;
  word                    local_cid;
  uint8                   server_channel;
  boolean                 stick_to_app_param;
  boolean                 sniff_initiator_oriented;
} bt_cmd_rm_low_power_config_ext_type; 
#endif /* BT_SWDEV_2_1_SSR */

typedef BT_PACKED struct
{
  boolean          bd_addr_valid;
  bt_bd_addr_type  bd_addr;
  bt_role_type     role;
} bt_cmd_rm_set_conn_role_type;

typedef BT_PACKED struct
{
  boolean          bd_addr_valid;
  bt_bd_addr_type  bd_addr;
  boolean          role_sw_req_ok;
} bt_cmd_rm_set_conn_role_sw_req_ok_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  bt_security_type  security;
} bt_cmd_rm_set_conn_security_type;

typedef BT_PACKED struct
{
  boolean           bd_addr_valid;
  bt_bd_addr_type   bd_addr;
  bt_security_type  security;
} bt_cmd_rm_set_dev_security_type;

typedef BT_PACKED struct
{
  bt_service_id_type  service_id;
  bt_security_type    security;
  boolean             authorize;
  boolean             authorize_first;
} bt_cmd_rm_set_serv_security_type;

typedef BT_PACKED struct
{
  bt_service_id_type  service_id;
  uint8               min_pin_len_bytes;
  boolean             comb_link_key_only;
  uint8               min_enc_key_len_bytes;
} bt_cmd_rm_set_serv_sec_ext_type;

typedef BT_PACKED struct
{
  bt_service_id_type  service_id;
  bt_bd_addr_type     bd_addr;
  boolean             authorized;
} bt_cmd_rm_authorize_reply_type;

typedef BT_PACKED struct
{
  uint8            rfcomm_scn;
  bt_bd_addr_type  bd_addr;
  boolean          local_client;
} bt_cmd_rm_enforce_sec_rfcomm_scn_type;

typedef BT_PACKED struct
{
  uint16           l2cap_psm;
  bt_bd_addr_type  bd_addr;
  uint16           l2cap_cid;
  bt_rm_atzrq_type conn_dir;
} bt_cmd_rm_enforce_sec_l2cap_psm_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  bt_pin_code_type  pin;
} bt_cmd_rm_bond_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
} bt_cmd_rm_bond_cancel_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_cmd_rm_unbond_type;

typedef BT_PACKED struct
{
  boolean  bondable;
} bt_cmd_rm_set_bondable_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;      
  boolean           pin_valid;
  bt_pin_code_type  pin;
} bt_cmd_rm_pin_reply_type;

typedef BT_PACKED struct
{
  bt_device_type  device;
} bt_cmd_rm_dev_add_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
  bt_rm_dev_update_ctrl_type  update_control;
  bt_device_type              device;
} bt_cmd_rm_dev_update_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  uint16           num_services;
  bt_service_type  services[ BT_SD_MAX_NUM_OF_SRV_REC ];
} bt_cmd_rm_dev_serv_update_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_cmd_rm_dev_remove_type;

typedef BT_PACKED struct
{
  boolean  enable;
} bt_cmd_rm_test_mode_enable_type;

typedef BT_PACKED struct
{
  boolean                connectable;
  bt_rm_act_vs_pwr_type  act_vs_pwr;
} bt_cmd_rm_set_connectable_type;

typedef BT_PACKED struct
{
  bt_rm_discoverable_type  discoverable;
  bt_rm_act_vs_pwr_type    act_vs_pwr;
} bt_cmd_rm_set_discoverable_type;

typedef BT_PACKED struct
{
  bt_iac_type  iac;
  uint8        duration_secs;
  bt_cod_type  class_of_device_filter;
  uint8        max_responses;
} bt_cmd_rm_inquire_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_cmd_rm_remote_name_req_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_cmd_rm_remote_name_req_cancel_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_cmd_rm_connect_acl_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_cmd_rm_connect_cancel_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_cmd_rm_accept_acl_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type        bd_addr;
  bt_rm_rej_reason_type  reason;
} bt_cmd_rm_reject_acl_type;

typedef BT_PACKED struct
{
  bt_rm_handle_type               handle;
  uint16                          local_cid; /* L2CAP local channel id */
  bt_hc_pkt_boundary_flag_type    l2cap_pkt_start;
  uint16                          flush_timeout;
  dsm_item_type*                  dsm_ptr;
  void*                           frame_handle;
} bt_cmd_rm_tx_data_acl_type;

typedef BT_PACKED struct
{
  bt_rm_handle_type       handle;
  bt_rm_disc_reason_type  reason;
} bt_cmd_rm_disconnect_acl_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  boolean          attempt_esco;
} bt_cmd_rm_connect_sco_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_cmd_rm_accept_sco_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type        bd_addr;
  bt_rm_rej_reason_type  reason;
} bt_cmd_rm_reject_sco_type;

typedef BT_PACKED struct
{
  bt_rm_handle_type  handle;
  dsm_item_type*     dsm_ptr;
} bt_cmd_rm_tx_data_sco_type;

typedef BT_PACKED struct
{
  bt_rm_handle_type       handle;
  bt_rm_disc_reason_type  reason;
} bt_cmd_rm_disconnect_sco_type;

#ifdef FEATURE_BT_TELEC
typedef BT_PACKED struct
{
  boolean  transmit;
  boolean  hopping;
} bt_cmd_rm_start_telec_test_type;
#endif

typedef BT_PACKED struct
{
  boolean  disable;
} bt_cmd_rm_disable_radio_type;

typedef BT_PACKED struct
{
  boolean  disable;
} bt_cmd_rm_disable_discoverability_type;

typedef BT_PACKED struct
{
  uint16           dev_black_list_count;
  bt_bd_addr_type  dev_black_list[ BT_RM_MAX_DEV_BLACK_LIST_COUNT ];
} bt_cmd_rm_set_device_black_list_type;

typedef BT_PACKED struct
{
  uint16           dev_white_list_count;
  bt_bd_addr_type  dev_white_list[ BT_RM_MAX_DEV_WHITE_LIST_COUNT ];
} bt_cmd_rm_set_device_white_list_type;

#ifdef FEATURE_BT_1_2
typedef BT_PACKED struct
{
  uint16  afh_period;
  uint16  retx_threshold;
  uint16  crc_err_threshold;
} bt_cmd_rm_config_afh_type;

typedef BT_PACKED struct
{
  boolean                  enable;
  bt_rm_afh_threshold_type level;
} bt_cmd_rm_enable_afh_type;

typedef BT_PACKED struct
{
  bt_afh_ch_map_type  ch_map;
} bt_cmd_rm_set_afh_ch_map_type;
#endif /* FEATURE_BT_1_2 */

#ifdef BT_SWDEV_2_1_PB
typedef BT_PACKED struct
{
  uint32                flush_to;
  bt_rm_handle_type     rm_handle;
}bt_cmd_rm_set_auto_flush_to_type;
#endif /* BT_SWDEV_2_1_PB */

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#ifdef BT_SWDEV_2_1_SSP
typedef BT_PACKED struct
{
  bt_rm_io_cap_type io_cap;
} bt_cmd_rm_set_io_cap_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type      bd_addr;
  bt_service_id_type   service_id;
  bt_sec_level_type    sec_level;
  boolean              authorize;
  boolean              authorize_first;
} bt_cmd_rm_set_service_sm4_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;      
  boolean           answer;
} bt_cmd_rm_user_confirmation_reply_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  bt_pin_code_type  pin;
  boolean           mitm_protection_reqd;
} bt_cmd_rm_bond_ext_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type            bd_addr;
  boolean                    bond_allowed;
  bt_rm_authorize_bond_type  bond_req;
} bt_cmd_rm_authorize_rebond_type;

typedef BT_PACKED struct
{
  boolean  enable_debug_mode;
} bt_cmd_rm_set_sm4_debug_mode_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  bt_rm_keypress_notification_type key;
} bt_cmd_rm_keypress_notification_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  uint32            passkey;
  boolean  valid;
} bt_cmd_rm_passkey_reply_type;

typedef BT_PACKED struct
{
  uint8     size_oob_data;
  uint8     oob_data[ BT_MAX_OOB_DATA_SIZE ];        
} bt_cmd_rm_wrt_remote_oob_data_type;

#endif /* BT_SWDEV_2_1_SSP */
/*-------------------------------------------------------------------------*/
/*                Host Controller Interface (HCI) Commands                 */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_ev_cb_func_type*  ev_cb_ptr;
} bt_cmd_hc_register_type;

/* General HCI command fields.
*/
typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
} bt_cmd_hc_bd_addr_type;

typedef BT_PACKED struct
{
  uint16  conn_hndl;
} bt_cmd_hc_conn_hndl_type;


/*-------------------------------------------------------------------------*/
/* HCI Command:     Tx ACL Data Packet (4.4.3)                             */
/* Command type:    BT_CMD_HC_TX_ACL_DATA                                  */
/* Command struct:  bt_cmd_hc_tx_acl_data_type                             */
/* Notes:           This is not a HCI command according to the BT spec.    */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16                        conn_hndl;
  uint16                        local_cid; /* L2CAP local cid */
  bt_hc_pkt_boundary_flag_type  pkt_boundary_flag;
  bt_hc_broadcast_flag_type     broadcast_flag;
  dsm_item_type*                dsm_ptr;
  void*                         frame_handle;
} bt_cmd_hc_tx_acl_data_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Tx SCO Data Packet (4.4.3)                             */
/* Command type:    BT_CMD_HC_TX_SCO_DATA                                  */
/* Command struct:  bt_cmd_hc_tx_sco_data_type                             */
/* Notes:           This is not a HCI command according to the BT spec.    */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  dsm_item_type*   dsm_ptr;
} bt_cmd_hc_tx_sco_data_type;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                     Link Control HCI Commands                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Inquiry (4.5.1)                                        */
/* Command type:    BT_CMD_HC_INQUIRY                                      */
/* Command struct:  bt_cmd_hc_inquiry_type                                 */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_lap_type      lap;
  uint8            inquiry_len;
  uint8            num_responses;
} bt_cmd_hc_inquiry_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Inquiry_Cancel (4.5.2)                                 */
/* Command Type:    BT_CMD_HC_INQUIRY_CANCEL                               */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Periodic_Inquiry_Mode (4.5.3)                          */
/* Command Type:    BT_CMD_HC_PERIODIC_INQUIRY                             */
/* Command struct:  bt_cmd_hc_periodic_inq_type                            */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           max_per_len;
  uint16           min_per_len;
  bt_lap_type      lap;
  uint8            inquiry_len;
  uint8            num_responses;
} bt_cmd_hc_periodic_inq_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Exit_Periodic_Inquiry_Mode (4.5.4)                     */
/* Command Type:    BT_CMD_HC_EXIT_PERIODIC_INQUIRY                        */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Create_Connection (4.5.5)                              */
/* Command Type:    BT_CMD_HC_CREATE_CONNECTION                            */
/* Command struct:  bt_cmd_hc_create_conn_type                             */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
  uint16                      pkt_type;    /* Use BT_PKT_TYPE_* #defines */
  bt_page_scan_rep_mode_type  ps_rep_mode;
  bt_page_scan_mode_type      ps_mode;
  uint16                      clk_offset;
  bt_role_switch_type         role_switch;
} bt_cmd_hc_create_conn_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Disconnect (4.5.6)                                     */
/* Command Type:    BT_CMD_HC_DISCONNECT                                   */
/* Command struct:  bt_cmd_hc_disconn_type                                 */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint8            reason;
} bt_cmd_hc_disconn_type;

#ifdef FEATURE_BT_1_2
/*-------------------------------------------------------------------------*/
/* HCI Command:     Create_Connection_Cancel (7.1.7)                       */
/* Command Type:    BT_CMD_HC_CREATE_CONNECTION_CANCEL                     */
/* Command struct:  bt_cmd_hc_create_conn_cancel_type                      */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
} bt_cmd_hc_create_conn_cancel_type;
#endif /* FEATURE_BT_1_2 */

/*-------------------------------------------------------------------------*/
/* HCI Command:     Add_SCO_Connection (4.5.7)                             */
/* Command Type:    BT_CMD_HC_ADD_SCO_CONN                                 */
/* Command struct:  bt_cmd_hc_add_sco_conn_type                            */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;  /* conn handle of ACL link */
  uint16           pkt_type;   /* Use BT_PKT_TYPE_* #defines */
} bt_cmd_hc_add_sco_conn_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Accept_Connection_Request (4.5.8)                      */
/* Command Type:    BT_CMD_HC_ACCEPT_CONN                                  */
/* Command struct:  bt_cmd_hc_accept_conn_type                             */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  bt_role_type     role;
} bt_cmd_hc_accept_conn_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Reject_Connection_Request (4.5.9)                      */
/* Command Type:    BT_CMD_HC_REJECT_CONN                                  */
/* Command struct:  bt_cmd_hc_reject_conn_type                             */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  uint8            reason;
} bt_cmd_hc_reject_conn_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Link_Key_Request_Reply (4.5.10)                        */
/* Command Type:    BT_CMD_HC_LINK_KEY                                     */
/* Command struct:  bt_cmd_hc_link_key_type                                */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type   bd_addr;
  bt_link_key_type  link_key;
} bt_cmd_hc_link_key_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Link_Key_Request_Negative_Reply (4.5.11)               */
/* Command Type:    BT_CMD_HC_LINK_KEY_NEG                                 */
/* Command struct:  bt_cmd_hc_bd_addr_type                                 */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     PIN_Code_Request_Reply (4.5.12)                        */
/* Command Type:    BT_CMD_HC_PIN_CODE                                     */
/* Command struct:  bt_cmd_hc_pin_code_type                                */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  uint8            pin_code_len;
  uint8            pin_code[ BT_MAX_PIN_CODE_LEN ];
} bt_cmd_hc_pin_code_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     PIN_Code_Request_Negative_Reply (4.5.13)               */
/* Command Type:    BT_CMD_HC_PIN_CODE_NEG                                 */
/* Command struct:  bt_cmd_hc_bd_addr_type                                 */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Change_Connection_Packet_Type (4.5.14)                 */
/* Command Type:    BT_CMD_HC_CHANGE_PKT_TYPE                              */
/* Command struct:  bt_cmd_hc_change_pkt_type_type                         */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint16           pkt_type;   /* Use BT_PKT_TYPE_* #defines */
} bt_cmd_hc_change_pkt_type_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Authentication_Requested (4.5.15)                      */
/* Command Type:    BT_CMD_HC_AUTH_REQ                                     */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_Connection_Encryption (4.5.16)                     */
/* Command Type:    BT_CMD_HC_SET_CONN_ENCRYPT                             */
/* Command struct:  bt_cmd_hc_set_conn_encrypt_type                        */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16                  conn_hndl;
  bt_encrypt_enable_type  encrypt_en;
} bt_cmd_hc_set_conn_encrypt_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Change_Connection_Link_Key (4.5.17)                    */
/* Command Type:    BT_CMD_HC_CHANGE_LINK_KEY                              */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Master_Link_Key (4.5.18)                               */
/* Command Type:    BT_CMD_HC_MASTER_LINK_KEY                              */
/* Command struct:  bt_cmd_hc_master_link_key_type                         */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_link_key_select_type  key_flag;
} bt_cmd_hc_master_link_key_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Remote_Name_Request (4.5.19)                           */
/* Command Type:    BT_CMD_HC_REMOTE_NAME_REQ                              */
/* Command struct:  bt_cmd_hc_remote_name_req_type                         */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
  bt_page_scan_rep_mode_type  ps_rep_mode;
  bt_page_scan_mode_type      ps_mode;
  uint16                      clk_offset;
} bt_cmd_hc_remote_name_req_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Remote_Supported_Features (4.5.20)                */
/* Command Type:    BT_CMD_HC_RD_REMOTE_FEATURES                           */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_BT_1_2
/*-------------------------------------------------------------------------*/
/* HCI Command:     Remote_Name_Req_Cancel (7.1.20)                        */
/* Command Type:    BT_CMD_HC_REMOTE_NAME_REQ_CANCEL                       */
/* Command struct:  bt_cmd_hc_remote_name_req_cancel_type                  */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
} bt_cmd_hc_remote_name_req_cancel_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Remote_Extended_Features (7.1.22)                 */
/* Command Type:    BT_CMD_HC_RD_REMOTE_EXT_FEATURES                       */
/* Command struct:  bt_cmd_hc_rd_rmt_ext_feat_type                         */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
  uint8   page_number;
} bt_cmd_hc_rd_rmt_ext_feat_type;

#endif /* FEATURE_BT_1_2 */

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Remote_Version_Information (4.5.21)               */
/* Command Type:    BT_CMD_HC_RD_REMOTE_VERSION                            */
/* Command struct:  bt_cmd_hc_hc_conn_hndl_type                            */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Clock_Offset (4.5.22)                             */
/* Command Type:    BT_CMD_HC_RD_CLK_OFFSET                                */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

#ifdef BT_SWDEV_2_1_SSP

/*-------------------------------------------------------------------------*/
/* HCI Command:     IO_Capability_Response (7.1.29)                        */
/* Command Type:    BT_CMD_HC_IO_CAPABILITY_RESPONSE                       */
/* Command struct:  bt_cmd_hc_io_cap_response_type                         */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
  uint8                       io_capability;
  uint8                       oob_data_present;
  uint8                       auth_req;
} bt_cmd_hc_io_cap_response_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     User_Confirmation_Request_Reply (7.1.30)               */
/* Command Type:    BT_CMD_HC_USER_CFMTN_RQST_REPLY                        */
/* Command struct:  bt_cmd_hc_user_cfmtn_rqst_rep_type                     */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
} bt_cmd_hc_user_cfmtn_rqst_rep_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     User_Confirmation_Request_Negative_Reply (7.1.31)      */
/* Command Type:    BT_CMD_HC_USER_CFMTN_RQST_NEG_REPLY                    */
/* Command struct:  bt_cmd_hc_user_cfmtn_rqst_neg_rep_type                 */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
} bt_cmd_hc_user_cfmtn_rqst_neg_rep_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     User_Passkey_Request_Reply (7.1.32)                    */
/* Command Type:    BT_CMD_HC_USER_PASSKEY_RQST_REPLY                      */
/* Command struct:  bt_cmd_hc_user_passkey_rqst_rep_type                   */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
  uint32                      num_val; /* must <=0x000F423F */
} bt_cmd_hc_user_passkey_rqst_rep_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     User_Passkey_Request_Negative_Reply (7.1.33)           */
/* Command Type:    BT_CMD_HC_USER_PASSKEY_RQST_NEG_REPLY                  */
/* Command struct:  bt_cmd_hc_user_passkey_rqst_neg_rep_type               */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
} bt_cmd_hc_user_passkey_rqst_neg_rep_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Remote_OOB_Data_Request_Reply (7.1.34)                 */
/* Command Type:    BT_CMD_HC_REMOTE_OOB_DATA_RQST_REPLY                   */
/* Command struct:  bt_cmd_hc_remote_oob_data_rqst_rep_type                */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
  bt_sp_hash_type             sp_hash;
  bt_sp_rand_type             sp_rand;
} bt_cmd_hc_remote_oob_data_rqst_rep_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Remote_OOB_Data_Request_Negative_Reply (7.1.35)        */
/* Command Type:    BT_CMD_HC_REMOTE_OOB_DATA_RQST_NEG_REPLY               */
/* Command struct:  bt_cmd_hc_remote_oob_data_rqst_neg_rep_type            */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
} bt_cmd_hc_remote_oob_data_rqst_neg_rep_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     IO_Capability_Request_Negative_Reply (7.1.36)          */
/* Command Type:    BT_CMD_HC_IO_CAPABILITY_RQST_NEG_REPLY                 */
/* Command struct:  bt_cmd_hc_io_cap_rqst_neg_rep_type                     */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type             bd_addr;
  uint8                       reason;
} bt_cmd_hc_io_cap_rqst_neg_rep_type;

#endif /* BT_SWDEV_2_1_SSP */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                      Link Policy HCI Commands                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Hold_Mode (4.6.1)                                      */
/* Command Type:    BT_CMD_HC_HOLD_MODE                                    */
/* Command struct:  bt_cmd_hc_hold_mode_type                               */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint16  hold_mode_max_interval; /* units of .625 ms Range: 0x0001-0xFFFF */
  uint16  hold_mode_min_interval; /* units of .625 ms Range: 0x0001-0xFFFF */
} bt_cmd_hc_hold_mode_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Sniff_Mode (4.6.2)                                     */
/* Command Type:    BT_CMD_HC_SNIFF_MODE                                   */
/* Command struct:  bt_cmd_hc_sniff_mode_type                              */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint16  sniff_max_interval;  /* units of .625 ms Range: 0x0001-0xFFFF */
  uint16  sniff_min_interval;  /* units of .625 ms Range: 0x0001-0xFFFF */
  uint16  sniff_attempt;       /* units of .625 ms Range: 0x0001-0xFFFF */
  uint16  sniff_timeout;       /* units of .625 ms Range: 0x0001-0xFFFF */
} bt_cmd_hc_sniff_mode_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Exit_Sniff_Mode (4.6.3)                                */
/* Command Type:    BT_CMD_HC_EXIT_SNIFF_MODE                              */
/* Command struct:  bt_cmd_hc_exit_sniff_mode_type                         */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
} bt_cmd_hc_exit_sniff_mode_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Park_Mode (4.6.4)                                      */
/* Command Type:    BT_CMD_HC_PARK_MODE                                    */
/* Command struct:  bt_cmd_hc_park_mode_type                               */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint16  beacon_max_interval;  /* units of .625 ms Range: 0x0001-0xFFFF */
  uint16  beacon_min_interval;  /* units of .625 ms Range: 0x0001-0xFFFF */
} bt_cmd_hc_park_mode_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Exit_Park_Mode (4.6.5)                                 */
/* Command Type:    BT_CMD_HC_EXIT_PARK_MODE                               */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
} bt_cmd_hc_exit_park_mode_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     QoS_Setup (4.6.6)                                      */
/* Command Type:    BT_CMD_HC_QOS_SETUP                                    */
/* Command struct:  bt_cmd_hc_qos_setup_type                               */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16               conn_hndl;
  uint8                flags;
  bt_qos_service_type  service_type;
  uint32               token_rate;       /* in units of bytes per second */
  uint32               peak_bandwidth;   /* in units of bytes per second */
  uint32               latency;          /* in units of microseconds     */
  uint32               delay_variation;  /* in units of microseconds     */
} bt_cmd_hc_qos_setup_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Role_Discovery (4.6.7)                                 */
/* Command Type:    BT_CMD_HC_ROLE_DISCOVERY                               */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   bt_hc_rp_role_discovery_type                           */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Switch_Role (4.6.8)                                    */
/* Command Type:    BT_CMD_HC_SWITCH_ROLE                                  */
/* Command struct:  bt_cmd_hc_switch_role_type                             */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type  bd_addr;
  bt_role_type     role;
} bt_cmd_hc_switch_role_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Link_Policy_Setting (4.6.9)                       */
/* Command Type:    BT_CMD_HC_RD_LINK_POLICY                               */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   bt_hc_rp_rd_link_policy_type                           */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Link_Policy_Settings (4.6.10)                    */
/* Command Type:    BT_CMD_HC_WR_LINK_POLICY                               */
/* Command struct:  bt_cmd_hc_wr_link_policy_type                          */
/* Return struct:   bt_hc_rp_conn_hndl_type                                */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint16           link_policy_settings;  /*  Use BT_LINK_POLICY_**_B  */
                                          /*    bit masks.             */
} bt_cmd_hc_wr_link_policy_type;

#ifdef BT_SWDEV_2_1_SSR
/*-------------------------------------------------------------------------*/
/* HCI Command:     Sniff_Subrating (7.2.14)                               */
/* Command Type:    BT_CMD_HC_SNIFF_SUBRATING                              */
/* Command struct:  bt_cmd_hc_sniff_subrating_type                         */
/* Return struct:   bt_hc_rp_conn_hndl_type                                */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint16           max_latency;
  uint16           min_remote_to;
  uint16           min_local_to;
} bt_cmd_hc_sniff_subrating_type;
#endif /* BT_SWDEV_2_1_SSR */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                Host Controller & Baseband HCI Commands                  */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_Event_Mask (4.7.1)                                 */
/* Command Type:    BT_CMD_HC_SET_EVENT_MASK                               */
/* Command struct:  bt_cmd_hc_set_event_mask_type                          */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  // WAL - add macros to access, and comment here
  uint8            event_mask[ BT_EVENT_MASK_LEN ];
} bt_cmd_hc_set_event_mask_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Reset (4.7.2)                                          */
/* Command Type:    BT_CMD_HC_RESET                                        */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_Event_Filter (4.7.3)                               */
/* Command Type:    BT_CMD_HC_SET_EVENT_FILTER                             */
/* Command struct:  bt_cmd_hc_set_event_filter_type                        */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef enum
{
  BT_FILTER_CLEAR_ALL = 0,
  BT_FILTER_INQUIRY_RESULT,
  BT_FILTER_CONN_SETUP
} bt_filter_type;

typedef enum
{
  BT_INQUIRY_NEW_DEVICE = 0,
  BT_INQUIRY_FILTER_CLASS_OF_DEVICE,
  BT_INQUIRY_FILTER_BD_ADDR
} bt_inquiry_filter_cond_type;

typedef enum
{
  BT_CONN_FILTER_ALL_DEVICES = 0,
  BT_CONN_FILTER_CLASS_OF_DEVICE,
  BT_CONN_FILTER_BD_ADDR
} bt_conn_filter_cond_type;

typedef BT_PACKED struct
{
  bt_cod_type  class_of_device;
  bt_cod_type  class_of_device_mask;
} bt_inquiry_filter_class_of_device_cond_type;

typedef BT_PACKED struct
{
  bt_cod_type  class_of_device;
  bt_cod_type  class_of_device_mask;
  bt_auto_accept_type      auto_accept_flag;
} bt_conn_filter_class_of_device_cond_type;

typedef BT_PACKED struct
{
  bt_bd_addr_type      bd_addr;
  bt_auto_accept_type  auto_accept_flag;
} bt_conn_filter_bd_addr_cond_type;

typedef BT_PACKED struct
{
  bt_filter_type   filter_type;
  BT_PACKED union
  {
    bt_inquiry_filter_cond_type  inq_filter_cond;
    bt_conn_filter_cond_type     conn_filter_cond;
  } filter_cond_type;
  BT_PACKED union
  {
    bt_inquiry_filter_class_of_device_cond_type  inquiry_class_of_device;
    bt_bd_addr_type                              inquiry_bd_addr;
    bt_auto_accept_type                          auto_accept_flag;
    bt_conn_filter_class_of_device_cond_type     conn_class_of_device;
    bt_conn_filter_bd_addr_cond_type             conn_bd_addr;
  } condition;
} bt_cmd_hc_set_event_filter_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Flush (4.7.4)                                          */
/* Command Type:    BT_CMD_HC_FLUSH                                        */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   bt_hc_rp_conn_hndl_type                                */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_PIN_Type (4.7.5)                                  */
/* Command Type:    BT_CMD_HC_RD_PIN_TYPE                                  */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_pin_type                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_PIN_Type (4.7.6)                                 */
/* Command Type:    BT_CMD_HC_WR_PIN_TYPE                                  */
/* Command struct:  bt_cmd_hc_wr_pin_type                                  */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_pin_type      pin_type;
} bt_cmd_hc_wr_pin_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Create_New_Unit_Key (4.7.7)                            */
/* Command Type:    BT_CMD_HC_CREATE_NEW_UNIT_KEY                          */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Stored_Link_Key (4.7.8)                           */
/* Command Type:    BT_CMD_HC_RD_STORED_LINK_KEY                           */
/* Command struct:  bt_cmd_hc_rd_link_key_type                             */
/* Return struct:   bt_hc_rp_rd_link_key_type                              */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_bd_addr_type           bd_addr;
  bt_hc_link_key_flag_type  read_all_flag;
} bt_cmd_hc_rd_link_key_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Stored_Link_Key (4.7.9)                          */
/* Command Type:    BT_CMD_HC_WR_STORED_LINK_KEY                           */
/* Command struct:  bt_cmd_hc_wr_link_key_type                             */
/* Return struct:   bt_hc_rp_wr_link_key_type                              */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8             num_keys_to_write;
  dsm_item_type*    dsm_ptr;

  /* dsm_ptr the equivalent of the following data:
  bt_bd_addr_type   bd_addr[ num_keys_to_write ];
  bt_link_key_type  link_key[ num_keys_to_write ];
  */
} bt_cmd_hc_wr_link_key_type;


/*-------------------------------------------------------------------------*/
/* HCI Command:     Delete_Stored_Link_Key (4.7.10)                        */
/* Command Type:    BT_CMD_HC_DEL_STORED_LINK_KEY                          */
/* Command struct:  bt_cmd_hc_del_link_key_type                            */
/* Return struct:   bt_hc_rp_del_link_key_type                             */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type           bd_addr;
  bt_hc_link_key_flag_type  delete_all_flag;
} bt_cmd_hc_del_link_key_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Change_Local_Name (4.7.11)                             */
/* Command Type:    BT_CMD_HC_CHANGE_LOCAL_NAME                            */
/* Command struct:  bt_cmd_hc_change_local_name_type                       */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  dsm_item_type*   dsm_ptr;    /* contains up to 248 bytes of UTF-8 encode */
                               /* characters; null terminated.             */
} bt_cmd_hc_change_local_name_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Local_Name (4.7.12)                               */
/* Command Type:    BT_CMD_HC_RD_LOCAL_NAME                                */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_local_name_type                            */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Connection_Accept_Timeout (4.7.13)                */
/* Command Type:    BT_CMD_HC_RD_CONN_ACCEPT_TO                            */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_conn_accept_to_type                        */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Connection_Accept_Timeout (4.7.14)               */
/* Command Type:    BT_CMD_HC_WR_CONN_ACCEPT_TO                            */
/* Command struct:  bt_cmd_hc_wr_conn_accept_to_type                       */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_accept_timeout;  /* units of .625 ms Range: 0x0001-0xB540 */
} bt_cmd_hc_wr_conn_accept_to_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Page_Timeout (4.7.15)                             */
/* Command Type:    BT_CMD_HC_RD_PAGE_TO                                   */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_page_to_type                               */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Page_Timeout (4.7.16)                            */
/* Command Type:    BT_CMD_HC_WR_PAGE_TO                                   */
/* Command struct:  bt_cmd_hc_wr_page_to_type                              */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           page_timeout;  /* units of .625 ms Range: 0x0001-0xFFFF */
} bt_cmd_hc_wr_page_to_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Scan_Enable (4.7.17)                              */
/* Command Type:    BT_CMD_HC_RD_SCAN_ENABLE                               */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_scan_enable_type                           */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Scan_Enable (4.7.18)                             */
/* Command Type:    BT_CMD_HC_WR_SCAN_ENABLE                               */
/* Command struct:  bt_cmd_hc_wr_scan_enable_type                          */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_hc_scan_enable_type  scan_enable;
} bt_cmd_hc_wr_scan_enable_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Page_Scan_Activity (4.7.19)                       */
/* Command Type:    BT_CMD_HC_RD_PAGE_SCAN_ACTIVITY                        */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_page_scan_act_type                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Page_Scan_Activity (4.7.20)                      */
/* Command Type:    BT_CMD_HC_WR_PAGE_SCAN_ACTIVITY                        */
/* Command struct:  bt_cmd_hc_wr_page_scan_act_type                        */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  page_scan_interval;  /* units of .625 ms Range: 0x0012-0x1000 */
  uint16  page_scan_window;    /* units of .625 ms Range: 0x0012-0x1000 */
} bt_cmd_hc_wr_page_scan_act_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Inquiry_Scan_Activity (4.7.21)                    */
/* Command Type:    BT_CMD_HC_RD_INQ_SCAN_ACTIVITY                         */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_inq_scan_act_type                          */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Inquiry_Scan_Activity (4.7.22)                   */
/* Command Type:    BT_CMD_HC_WR_INQ_SCAN_ACTIVITY                         */
/* Command struct:  bt_cmd_hc_wr_inq_scan_act_type                         */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  inquiry_scan_interval;  /* units of .625 ms Range: 0x0012-0x1000 */
  uint16  inquiry_scan_window;    /* units of .625 ms Range: 0x0012-0x1000 */
} bt_cmd_hc_wr_inq_scan_act_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Authentication_Enable (4.7.23)                    */
/* Command Type:    BT_CMD_HC_RD_AUTH_ENABLE                               */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_auth_enable_type                           */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Authentication_Enable (4.7.24)                   */
/* Command Type:    BT_CMD_HC_WR_AUTH_ENABLE                               */
/* Command struct:  bt_cmd_hc_wr_auth_enable_type                          */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_hc_auth_enable_type  auth_enable;
} bt_cmd_hc_wr_auth_enable_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Encryption_Mode (4.7.25)                          */
/* Command Type:    BT_CMD_HC_RD_ENCRYPT_MODE                              */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_encrypt_mode_type                          */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Encryption_Mode (4.7.26)                         */
/* Command Type:    BT_CMD_HC_WR_ENCRYPT_MODE                              */
/* Command struct:  bt_cmd_hc_wr_encrypt_mode_type                         */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_encrypt_mode_type  encrypt_mode;
} bt_cmd_hc_wr_encrypt_mode_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Class_of_Device (4.7.27)                          */
/* Command Type:    BT_CMD_HC_RD_CLASS_OF_DEVICE                           */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_class_of_dev_type                          */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Class_of_Device (4.7.28)                         */
/* Command Type:    BT_CMD_HC_WR_CLASS_OF_DEVICE                           */
/* Command struct:  bt_cmd_hc_wr_class_of_dev_type                         */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_cod_type  class_of_device;
} bt_cmd_hc_wr_class_of_dev_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Voice_Setting (4.7.29)                            */
/* Command Type:    BT_CMD_HC_RD_VOICE_SETTING                             */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_voice_setting_type                         */
/*-------------------------------------------------------------------------*/

#define BT_HC_VS_INPUT_CODING_LINEAR  0x0000
#define BT_HC_VS_INPUT_CODING_MU_LAW  0x0100
#define BT_HC_VS_INPUT_CODING_A_LAW   0x0200
#define BT_HC_VS_INPUT_CODING_M       0x0300

#define BT_HC_VS_INPUT_DATA_FMT_1S_COMPL  0x0000
#define BT_HC_VS_INPUT_DATA_FMT_2S_COMPL  0x0040
#define BT_HC_VS_INPUT_DATA_FMT_SIGN_MAG  0x0080
#define BT_HC_VS_INPUT_DATA_FMT_M         0x00C0

#define BT_HC_VS_INPUT_SAMPLE_8_BIT   0x0000
#define BT_HC_VS_INPUT_SAMPLE_16_BIT  0x0020
#define BT_HC_VS_INPUT_SAMPLE_M       0x0020

#define BT_HC_VS_PCM_BIT_POS_M  0x001C0
#define BT_HC_VS_PCM_BIT_POS_S  2

#define BT_HC_VS_AIR_CODING_CVSD    0x0000
#define BT_HC_VS_AIR_CODING_MU_LAW  0x0001
#define BT_HC_VS_AIR_CODING_A_LAW   0x0002
#define BT_HC_VS_AIR_CODING_M       0x0003


/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Voice_Setting (4.7.30)                           */
/* Command Type:    BT_CMD_HC_WR_VOICE_SETTING                             */
/* Command struct:  bt_cmd_hc_wr_voice_setting_type                        */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           voice_setting;   /* use BT_HC_VS_**_M #defines */
} bt_cmd_hc_wr_voice_setting_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Automatic_Flush_Timeout (4.7.31)                  */
/* Command Type:    BT_CMD_HC_RD_AUTO_FLUSH_TO                             */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   bt_hc_rp_rd_auto_flush_to_type                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Flush_Command                                          */
/* Command Type:    BT_CMD_HC_FLUSH                                        */
/* Command struct:  bt_cmd_hc_flush                                        */
/* Return struct:   bt_hc_rp_conn_hndl_type                                */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16            conn_hndl;
} bt_cmd_hc_flush_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Automatic_Flush_Timeout (4.7.32)                 */
/* Command Type:    BT_CMD_HC_WR_AUTO_FLUSH_TO                             */
/* Command struct:  bt_cmd_hc_wr_auto_flush_to_type                        */
/* Return struct:   bt_hc_rp_conn_hndl_type                                */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint16  flush_timeout;  /* 0    => timoeut= infinity; no automatic flush */
                          /* else => units- .625 ms.  Range: 0x0001-0x07FF */
} bt_cmd_hc_wr_auto_flush_to_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Num_Broadcast_Retransmissions (4.7.33)            */
/* Command Type:    BT_CMD_HC_RD_BROAD_RETRANS                             */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_broad_retrans_type                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Num_Broadcast_Retransmissions (4.7.34)           */
/* Command Type:    BT_CMD_HC_WR_BROAD_RETRANS                             */
/* Command struct:  bt_cmd_hc_wr_broad_retrans_type                        */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8            num_broadcast_retrans; /* range: 0x00-0xFF */
} bt_cmd_hc_wr_broad_retrans_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Hold_Mode_Activity (4.7.35)                       */
/* Command Type:    BT_CMD_HC_RD_HOLD_MODE_ACTIVITY                        */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_hold_mode_act_type                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Hold_Mode_Activity (4.7.36)                      */
/* Command Type:    BT_CMD_HC_WR_HOLD_MODE_ACTIVITY                        */
/* Command struct:  bt_cmd_hc_wr_hold_mode_act_type                        */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_hc_hold_mode_act_type  hold_mode_activity;
} bt_cmd_hc_wr_hold_mode_act_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Transmit_Power_Level (4.7.37)                     */
/* Command Type:    BT_CMD_HC_RD_TX_POWER_LEVEL                            */
/* Command struct:  bt_cmd_hc_rd_tx_power_type                             */
/* Return struct:   bt_hc_rp_rd_tx_power_type                              */
/*-------------------------------------------------------------------------*/
typedef enum
{
  BT_HC_RD_TX_POWER_CURRENT,
  BT_HC_RD_TX_POWER_MAX
} bt_hc_rd_tx_power_type;

typedef BT_PACKED struct
{
  uint16                  conn_hndl;
  bt_hc_rd_tx_power_type  type;
} bt_cmd_hc_rd_tx_power_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_SCO_Flow_Control_Enable (4.7.38)                  */
/* Command Type:    BT_CMD_HC_RD_SCO_FLOW_CTRL                             */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_sco_flow_ctrl_type                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_SCO_Flow_Control_Enable (4.7.39)                 */
/* Command Type:    BT_CMD_HC_WR_SCO_FLOW_CTRL                             */
/* Command struct:  bt_cmd_hc_wr_sco_flow_ctrl_type                        */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8            sco_flow_control_enable;  /* 0 => disable; 1 => enable */
} bt_cmd_hc_wr_sco_flow_ctrl_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_Host_Controller_To_Host_Flow_Control (4.7.40)      */
/* Command Type:    BT_CMD_HC_SET_HC_TO_HOST_FC                            */
/* Command struct:  bt_cmd_hc_set_hc_to_host_fc_type                       */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
#define BT_HC_FLOW_TO_HOST_ACL_M  0x01
#define BT_HC_FLOW_TO_HOST_SCO_M  0x02

typedef BT_PACKED struct
{
  uint8            flow_control_enable;
} bt_cmd_hc_set_hc_to_host_fc_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Host_Buffer_Size (4.7.41)                              */
/* Command Type:    BT_CMD_HC_HOST_BUFFER_SIZE                             */
/* Command struct:  bt_cmd_hc_host_buf_size_type                           */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_HCI_HOST_FCTL
#define BT_HC_HOST_TOT_NUM_ACL_DATA_PKTS     15
#define BT_HC_HOST_ACL_DATA_PKT_LEN          BT_SOC_MAX_RX_PKT_SIZE
#define BT_HC_HOST_TOT_NUM_SCO_DATA_PKTS     0
#define BT_HC_HOST_SCO_DATA_PKT_LEN          0
#else
#define BT_HC_HOST_TOT_NUM_ACL_DATA_PKTS     0
#define BT_HC_HOST_ACL_DATA_PKT_LEN          0
#define BT_HC_HOST_TOT_NUM_SCO_DATA_PKTS     0
#define BT_HC_HOST_SCO_DATA_PKT_LEN          0
#endif /* FEATURE_BT_HCI_HOST_FCTL */

typedef BT_PACKED struct
{
  uint16           acl_data_pkt_len;
  uint8            sco_data_pkt_len;
  uint16           tot_acl_data_pkts;
  uint16           tot_sco_data_pkts;
} bt_cmd_hc_host_buf_size_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Host_Number_Of_Completed_Packets (4.7.42)              */
/* Command Type:    BT_CMD_HC_HOST_NUM_CMPLT_PACKETS                       */
/* Command struct:  bt_cmd_hc_host_num_cmplt_pkt_type                      */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8            number_of_handles;
  dsm_item_type*   dsm_ptr;

  /* dsm_ptr the equivalent of the following data:
  uint16           conn_hndl[ number_of_handles ];
  uint16           host_num_cmplt_pkts[ number_of_handles ];
  */
} bt_cmd_hc_host_num_cmplt_pkt_type;
#define BT_CMD_HC_HOST_NUM_CMPLT_PKTS_MAX_SIZE ( sizeof( uint8 ) + \
                                                 ( BT_MAX_BT_DEV*2* \
                                                 sizeof( uint16 )))
                                                 

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Link_Supervision_Timeout (4.7.43)                 */
/* Command Type:    BT_CMD_HC_RD_LINK_SUPERVISION_TO                       */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   bt_hc_rp_rd_link_super_to_type                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Link_Supervision_Timeout (4.7.44)                */
/* Command Type:    BT_CMD_HC_WR_LINK_SUPERVISION_TO                       */
/* Command struct:  bt_cmd_hc_wr_link_super_to_type                        */
/* Return struct:   bt_hc_rp_conn_hndl_type                                */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint16           link_supervision_to;  /* 0 => no link supervision TO  */
                                         /* units: .625 ms;              */
                                         /* range: 0x0001-0xFFFF         */
} bt_cmd_hc_wr_link_super_to_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Number_Of_Supported_IAC (4.7.45)                  */
/* Command Type:    BT_CMD_HC_RD_NUM_SUPPORTED_IAC                         */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_num_iac_type                               */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Current_IAC_LAP (4.7.46)                          */
/* Command Type:    BT_CMD_HC_RD_CURRENT_IAC_LAP                           */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_curr_iac_lap_type                          */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Current_IAC_LAP (4.7.47)                         */
/* Command Type:    BT_CMD_HC_WR_CURRENT_IAC_LAP                           */
/* Command struct:  bt_cmd_hc_wr_curr_iac_lap_type                         */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8            num_current_iac;
  dsm_item_type*   dsm_ptr;

  /* dsm_ptr the equivalent of the following data:
  bt_lap_type      iac_lap[ num_current_iac ];
  */
} bt_cmd_hc_wr_curr_iac_lap_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Page_Scan_Period_Mode (4.7.48)                    */
/* Command Type:    BT_CMD_HC_RD_PAGE_SCAN_PER_MODE                        */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_page_scan_per_mode_type                    */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Page_Scan_Period_Mode (4.7.49)                   */
/* Command Type:    BT_CMD_HC_WR_PAGE_SCAN_PER_MODE                        */
/* Command struct:  bt_cmd_hc_wr_ps_per_mode_type                          */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_page_scan_per_mode_type  page_scan_period_mode;
} bt_cmd_hc_wr_ps_per_mode_type;

#ifndef FEATURE_BT_1_2 /* BT 1.1 */

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Page_Scan_Mode (4.7.50)                           */
/* Command Type:    BT_CMD_HC_RD_PAGE_SCAN_MODE                            */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_ps_mode_type                               */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Page_Scan_Mode (4.7.51)                          */
/* Command Type:    BT_CMD_HC_WR_PAGE_SCAN_MODE                            */
/* Command struct:  bt_cmd_hc_wr_ps_mode_type                              */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_page_scan_mode_type  page_scan_mode;
} bt_cmd_hc_wr_ps_mode_type;

#else /* BT 1.2 */

/*-------------------------------------------------------------------------*/
/* HCI Command:     Setup_Synchronous_Connection (7.1.26)                  */
/* Command Type:    BT_CMD_HC_SETUP_SYNC_CONNECTION                        */
/* Command struct:  bt_cmd_hc_setup_sync_connection_type                   */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{ 
  uint16           conn_hndl;  
  uint32           tx_bandwidth;
  uint32           rx_bandwidth;
  uint16           max_latency;
  uint16           voice_setting;
  uint8            retransmission_effort;
  uint16           pkt_type;   
} bt_cmd_hc_setup_sync_connection_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Accept_Synchronous_Connection_Request (7.1.27)         */
/* Command Type:    BT_CMD_HC_ACCEPT_SYNC_CONN_REQ                         */
/* Command struct:  bt_cmd_hc_accept_sync_conn_req_type                    */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{ 
  bt_bd_addr_type  bd_addr;
  uint32           tx_bandwidth;
  uint32           rx_bandwidth;
  uint16           max_latency;
  uint16           content_format;
  uint8            retransmission_effort;
  uint16           pkt_type;   
} bt_cmd_hc_accept_sync_conn_req_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Reject_Synchronous_Connection_Request (7.1.28)         */
/* Command Type:    BT_CMD_HC_REJECT_SYNC_CONN_REQ                         */
/* Command struct:  bt_cmd_hc_reject_sync_conn_req_type                    */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{ 
  bt_bd_addr_type  bd_addr;
  uint8            reason;   
} bt_cmd_hc_reject_sync_conn_req_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_eSCO_Packet_Length                                 */
/* Command Type:    BT_CMD_HC_SET_ESCO_PKT_LEN                             */
/* Command struct:  bt_cmd_hc_set_esco_pkt_len_type                        */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{ 
  uint16            pkt_len_m_s; /* master to slave */
  uint16            pkt_len_s_m;
  uint8             tesco;
  uint8             wesco;
} bt_cmd_hc_set_esco_pkt_len_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_AFH_Host_Channel_Classification (7.3.50)           */
/* Command Type:    BT_CMD_HC_SET_AFH_HOST_CHAN_CLASS                      */
/* Command struct:  bt_cmd_hc_set_afh_host_chan_class_type                 */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  afh_chan_class[ 10 ]; /* MSB of byte 0 is not used and should be 0 */
                               /* than one bit per channel 78..0            */
                               /* 0 = bad, 1 = unknown                      */
} bt_cmd_hc_set_afh_host_chan_class_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_AFH_Channel_Assessment_Mode (7.3.57)              */
/* Command Type:    BT_CMD_HC_READ_AFH_CHAN_ASSESS_MODE                    */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_afh_chan_assess_mode_type                  */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_AFH_Channel_Assessment_Mode (7.3.58)               */
/* Command Type:    BT_CMD_HC_SET_AFH_CHAN_ASSESS_MODE                     */
/* Command struct:  bt_cmd_hc_set_afh_chan_assess_mode_type                */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  afh_chan_asses_mode;
} bt_cmd_hc_set_afh_chan_assess_mode_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_AFH_Channel_Map (7.5.5)                           */
/* Command Type:    BT_CMD_HC_READ_AFH_CHAN_MAP                            */
/* Command struct:  bt_cmd_hc_read_afh_chan_map_type                       */
/* Return struct:   bt_hc_rp_rd_afh_chan_map_type                          */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16 conn_hndl;
} bt_cmd_hc_read_afh_chan_map_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_AFH_Channel_Good                                   */
/* Command Type:    BT_CMD_HC_SET_AFH_CHAN_GOOD                            */
/* Command struct:  bt_cmd_hc_set_afh_chan_good_type                       */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  afh_chan_good[ 10 ]; /* MSB of byte 0 is not used and should be 0 */
                              /* than one bit per channel 78..0            */
                              /* 0 = leave it alone, 1 = good              */
} bt_cmd_hc_set_afh_chan_good_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Control_AFH                                            */
/* Command Type:    BT_CMD_HC_CONTROL_AFH                                  */
/* Command struct:  bt_cmd_hc_control_afh_type                             */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  enable; /* 1 = enable; 0 = disable */
} bt_cmd_hc_control_afh_type; 

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Inquiry_Scan_Type (7.3.51)                        */
/* Command Type:    BT_CMD_HC_RD_INQUIRY_SCAN_TYPE                         */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_pi_scan_type_type                          */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Inquiry_Scan_Mode (7.3.52)                       */
/* Command Type:    BT_CMD_HC_WR_INQUIRY_SCAN_TYPE                         */
/* Command struct:  bt_cmd_hc_wr_is_type_type                              */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_inq_scan_mode_type  inq_scan_type;
} bt_cmd_hc_wr_is_type_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Inquiry_Mode (7.3.53)                             */
/* Command Type:    BT_CMD_HC_RD_INQUIRY_MODE                              */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_inq_mode_type                              */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Inquiry_Mode (7.3.54)                            */
/* Command Type:    BT_CMD_HC_WR_INQUIRY_SCAN_TYPE                            */
/* Command struct:  bt_cmd_hc_wr_is_type_type                              */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_inq_mode_type  inq_mode;
} bt_cmd_hc_wr_inq_mode_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Page_Scan_Type (7.3.55)                           */
/* Command Type:    BT_CMD_HC_RD_PAGE_SCAN_TYPE                            */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_pi_scan_type_type                          */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Page_Scan_Type (7.3.56)                          */
/* Command Type:    BT_CMD_HC_WR_PAGE_SCAN_TYPE                            */
/* Command struct:  bt_cmd_hc_wr_ps_type_type                              */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_page_scan_mode_type  page_scan_type;
} bt_cmd_hc_wr_ps_type_type;

#endif /* FEATURE_BT_1_2 */

#ifdef BT_SWDEV_2_1_SSP

/*-------------------------------------------------------------------------*/
/* HCI Command:     Refresh_Encryption_Key (7.3.55)                        */
/* Command Type:    BT_CMD_HC_REFRESH_ENCRYPTION_KEY                       */
/* Command struct:  bt_cmd_hc_refresh_encription_key_type                  */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
} bt_cmd_hc_refresh_encription_key_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Simple_Pairing_mode (7.3.56)                      */
/* Command Type:    BT_CMD_HC_RD_SIMPLE_PAIRING_MODE                       */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_sp_mode_type                                  */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Simple_Pairing_mode (7.3.57)                     */
/* Command Type:    BT_CMD_HC_WR_SIMPLE_PAIRING_MODE                       */
/* Command struct:  bt_cmd_hc_wr_sp_mode_type                              */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8    sp_mode;
} bt_cmd_hc_wr_sp_mode_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_OOB_Data (7.3.58)                                 */
/* Command Type:    BT_CMD_HC_RD_LOCAL_OOB_DATA                            */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_oob_data_type                                 */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Send_Keypress_Notification (7.3.61)                    */
/* Command Type:    BT_CMD_HC_SEND_KEYPRESS_NOTIFICATION                   */
/* Command struct:  bt_cmd_hc_send_keypress_ntfctn_type_type               */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_bd_addr_type bd_addr;
  uint8           notification_type;
} bt_cmd_hc_send_keypress_ntfctn_type_type;

#endif /* BT_SWDEV_2_1_SSP */


/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Default_Erroneous_Data_Reporting (7.3.64)         */
/* Command Type:    BT_CMD_HC_RD_DEFAULT_ERR_DATA_REPORTING                */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Default_Erroneous_Data_Reporting (7.3.65)        */
/* Command Type:    BT_CMD_HC_WR_DEFAULT_ERR_DATA_REPORTING                */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8    default_err_reporting_mode;
} bt_cmd_hc_wr_err_data_reporting_type;




/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                 Informational Parameters HCI Commands                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Local_Version_Information (4.8.1)                 */
/* Command Type:    BT_CMD_HC_RD_LOCAL_VERSION_INFO                        */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_local_ver_type                             */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Local_Supported_Commands                          */
/* Command Type:    BT_CMD_HC_RD_LOCAL_CMDS                                */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_local_cmds_type                            */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Local_Supported_Features (4.8.2)                  */
/* Command Type:    BT_CMD_HC_RD_LOCAL_FEATURES                            */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_local_feat_type                            */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Local_Extended_Features                           */
/* Command Type:    BT_CMD_HC_RD_LOCAL_EXTENDED_FEATURES                   */
/* Command struct:  bt_cmd_hc_rd_local_ext_feat                            */
/* Return struct:   bt_hc_rp_rd_rd_local_ext_feat_type                     */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  page_num;
} bt_cmd_hc_rd_local_ext_feat_type;


/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Buffer_Size (4.8.3)                               */
/* Command Type:    BT_CMD_HC_RD_BUFFER_SIZE                               */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_buffer_size_type                           */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Country_Code (4.8.4)                              */
/* Command Type:    BT_CMD_HC_RD_COUNTRY_CODE                              */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_country_code_type                          */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_BD_ADDR (4.8.5)                                   */
/* Command Type:    BT_CMD_HC_RD_BD_ADDR                                   */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_bd_addr_type                                  */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                      Status Parameters HCI Commands                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Failed_Contact_Counter (4.9.1)                    */
/* Command Type:    BT_CMD_HC_RD_FAILED_CONTACT_COUNTER                    */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   bt_hc_rp_rd_failed_contact_type                        */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Reset_Failed_Contact_Counter (4.9.2)                   */
/* Command Type:    BT_CMD_HC_RESET_FAILED_CONTACT_COUNTER                 */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   bt_hc_rp_conn_hndl_type                                */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Get_Link_Quality (4.9.3)                               */
/* Command Type:    BT_CMD_HC_GET_LINK_QUALITY                             */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   bt_hc_rp_get_link_quality_type                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_RSSI (4.9.4)                                      */
/* Command Type:    BT_CMD_HC_RD_RSSI                                      */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   bt_hc_rp_rd_rssi_type                                  */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Testing HCI Commands                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Loopback_Mode (4.10.1)                            */
/* Command Type:    BT_CMD_HC_RD_LOOPBACK_MODE                             */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_loopback_mode_type                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Loopback_Mode (4.10.2)                           */
/* Command Type:    BT_CMD_HC_WR_LOOPBACK_MODE                             */
/* Command struct:  bt_cmd_hc_wr_loopback_mode_type                        */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_loopback_type  loopback_mode;
} bt_cmd_hc_wr_loopback_mode_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Enable_Device_Under_Test_Mode (4.10.3)                 */
/* Command Type:    BT_CMD_HC_ENABLE_DUT_MODE                              */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/

#ifdef BT_SWDEV_2_1_SSP

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Simple_Pairing_Debug_mode (7.6.4)                */
/* Command Type:    BT_CMD_HC_WR_SIMPLE_PAIRING_DEBUG_MODE                 */
/* Command struct:  bt_cmd_hc_wr_sp_debug_mode_type                        */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8    sp_debug_mode;
} bt_cmd_hc_wr_sp_debug_mode_type;

#endif /* BT_SWDEV_2_1_SSP */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                   Qualcomm Specific HCI Commands                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Fix_Init_White                                         */
/* Command Type:    BT_CMD_HC_FIX_INIT_WHITE                               */
/* Command struct:  bt_cmd_hc_fix_init_white_type                          */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  boolean          fix_white_en;
  uint8            init_whitening_value;  /* Least-sig 7 bits used */
} bt_cmd_hc_fix_init_white_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Fix_Init_Enc                                           */
/* Command Type:    BT_CMD_HC_FIX_INIT_ENC                                 */
/* Command struct:  bt_cmd_hc_fix_init_enc_type                            */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16            conn_hndl;
  boolean           fix_enc_en;
  uint32            clk_26_1;    /* bits 1..26 of CLK */
  bt_link_key_type  kc_prime;
  bt_bd_addr_type   bd_addr;
} bt_cmd_hc_fix_init_enc_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Rx_Status_Enable                                  */
/* Command Type:    BT_CMD_HC_RD_RX_STATUS_EN                              */
/* Command struct:  bt_cmd_hc_rd_rx_status_en_type                         */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  boolean          rd_rx_status_en;
} bt_cmd_hc_rd_rx_status_en_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Config_Searcher                                        */
/* Command Type:    BT_CMD_HC_CONFIG_SCHR                                  */
/* Command struct:  bt_cmd_hc_config_srch_type                             */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8            preamble_srch_ena;   /* 0/1 - disabled/enabled */
  uint8            postamble_srch_ena;  /* 0/1 - disabled/enabled */
  uint8            ac1_srch_ena;        /* 0/1 - disabled/enabled */
  uint8            ac2_srch_ena;        /* 0/1 - disabled/enabled */
  uint8            page_type_thrshld;   /* bits [4..0] used       */
  uint16           ac_thrshld;          /* bits [9..0] used       */
  uint8            first_sample_bit;    /* bits [2..0] used       */
  uint8            last_sample_bit;     /* bits [2..0] used       */
  uint8            searcher_hit_delay;  /* bits [6..0] used       */
  uint8            sync_word_delay;     /* bits [5..0] used       */
  uint8            window_size;         /* bits [4..0] used       */
  uint8            majority_vote;       /* bits [2..0] used       */
} bt_cmd_hc_config_srch_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Tx_Buffer_Enable                                  */
/* Command Type:    BT_CMD_HC_RD_TX_BUFFER_EN                              */
/* Command struct:  bt_cmd_hc_rd_tx_buffer_en_type                         */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  boolean          rd_tx_buffer_en;
} bt_cmd_hc_rd_tx_buffer_en_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Rx_Buffer_Enable                                  */
/* Command Type:    BT_CMD_HC_RD_RX_BUFFER_EN                              */
/* Command struct:  bt_cmd_hc_rd_rx_buffer_en_type                         */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  boolean          rd_rx_buffer_en;
} bt_cmd_hc_rd_rx_buffer_en_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Send_Raw_Mode_Packet                                   */
/* Command Type:    BT_CMD_HC_SEND_RAW_MODE_PKT                            */
/* Command struct:  bt_cmd_hc_send_raw_mode_pkt_type                       */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint8            start;          /* this is the start of data to send */
  uint8            end;            /* this is the last of data to send  */
  uint8            num_slots;
  uint16           data_byte_len;  /* Not to exceed 360-- size of a HV5 */
  dsm_item_type*   dsm_ptr;        /* The data to transmit */
} bt_cmd_hc_send_raw_mode_pkt_type;
#define BT_HC_RAW_MODE_NON_DSM_SIZE 7  /*  dsm_ptr offset above TBD  */



#ifdef FEATURE_BT_TEST_MODE_TESTER
/*-------------------------------------------------------------------------*/
/* HCI Command:     Tester_Activate                                        */
/* Command Type:    BT_CMD_HC_TESTER_ACTIVATE                              */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Tester_Control                                         */
/* Command Type:    BT_CMD_HC_TESTER_CONTROL                               */
/* Command struct:  bt_cmd_hc_tester_control_type                          */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16           conn_hndl;
  uint8            test_senario;
  uint8            hopping_mode;
  uint8            dut_tx_freq_k;
  uint8            dut_rx_freq_k;
  uint8            power_control_mode;
  uint8            poll_period;         /* in units of 1.25 ms */
  uint8            packet_type;
  uint16           payload_len;
  uint8            start_end;           /* start = 0; end = 1 */
  dsm_item_type*   dsm_ptr;
} bt_cmd_hc_tester_control_type;
#define BT_HC_TESTER_CTRL_NON_DSM_SIZE 12  /*  dsm_ptr offset above TBD  */
#define BT_HC_TESTER_CTRL_TEST_SENARIO 2 /* test_senario(sic) offset */

/*-------------------------------------------------------------------------*/
/* HCI Command:     Tester_Start                                           */
/* Command Type:    BT_CMD_HC_TESTER_START                                 */
/* Command struct:  bt_cmd_hc_conn_hndl_type                               */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/

#endif /* FEATURE_BT_TEST_MODE_TESTER */

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_New_BD_ADDR                                        */
/* Command Type:    BT_CMD_HC_SET_NEW_BD_ADDR                              */
/* Command struct:  bt_cmd_hc_bd_addr_type                                 */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Time_Accuracy_Request                                  */
/* Command Type:    BT_CMD_HC_TIME_ACCURACY_REQ                            */
/* Command struct:  none                                                   */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Modify_Beacon                                          */
/* Command Type:    BT_CMD_HC_MODIFY_BEACON                                */
/* Command struct:  bt_cmd_hc_modify_beacon_type                           */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16    conn_hndl;
  uint16    t_b;
  uint8     n_b;
  uint8     delta_b;
  uint8     pm_addr;
  uint8     ar_addr;
  uint8     n_b_sleep;
  uint8     d_b_sleep;
  uint8     d_access;
  uint8     t_access;
  uint8     n_acc_slots;
  uint8     n_poll;
  uint8     m_access;
  uint8     access_scheme;
} bt_cmd_hc_modify_beacon_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Clock                                             */
/* Command Type:    BT_CMD_HC_RD_CLK                                       */
/* Command struct:  none                                                   */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_SBI_Register                                      */
/* Command Type:    BT_CMD_HC_RD_SBI_REG                                   */
/* Command struct:  bt_cmd_hc_rd_sbi_reg_type                              */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  addr;
} bt_cmd_hc_rd_sbi_reg_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_SBI_Register                                     */
/* Command Type:    BT_CMD_HC_WR_SBI_REG                                   */
/* Command struct:  bt_cmd_hc_wr_sbi_reg_type                              */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  addr;
  uint8  value;
} bt_cmd_hc_wr_sbi_reg_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     DIAC_LAP_Enable                                        */
/* Command Type:    BT_CMD_HC_DIAC_LAP_EN                                  */
/* Command struct:  bt_cmd_hc_diac_lap_en_type                             */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  enable_diac;
  uint8  lap;
} bt_cmd_hc_diac_lap_en_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Encryption_Key_Length                             */
/* Command Type:    BT_CMD_HC_RD_ENC_KEY_LENGTH                            */
/* Command struct:  none                                                   */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_Encryption_Key_Length                            */
/* Command Type:    BT_CMD_HC_WR_ENC_KEY_LENGTH                            */
/* Command struct:  bt_cmd_hc_wr_enc_key_length_type                       */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  min_enc_key_size;
  uint8  max_enc_key_size;
} bt_cmd_hc_wr_enc_key_length_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Change_Remote_Packet_Type                              */
/* Command Type:    BT_CMD_HC_CHANGE_REMOTE_PKT_TYPE                       */
/* Command struct:  bt_cmd_hc_change_remote_pkt_type_type                  */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
  uint16  pkt_type;
} bt_cmd_hc_change_remote_pkt_type_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Status_View_Enable                                     */
/* Command Type:    BT_CMD_HC_STATUS_VIEW_EN                               */
/* Command struct:  bt_cmd_hc_status_view_en_type                          */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  status_view_en;
} bt_cmd_hc_status_view_en_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Enable_Loopback                                        */
/* Command Type:    BT_CMD_HC_ENABLE_LOOPBACK                              */
/* Command struct:  bt_cmd_hc_enable_loopback_type                         */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  fix_freq;
  uint8  dut_tx_freq_k;
  uint8  dut_rx_freq_k;
  uint8  whitening_enabled;
} bt_cmd_hc_enable_loopback_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_Incoming_Hold_Params                               */
/* Command Type:    BT_CMD_HC_SET_INCOMING_HOLD_PARAMS                     */
/* Command struct:  bt_cmd_hc_set_incoming_hold_params_type                */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
  uint16  min_interval;
  uint16  max_interval;
} bt_cmd_hc_set_incoming_hold_params_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_Incoming_Sniff_Params                              */
/* Command Type:    BT_CMD_HC_SET_INCOMING_SNIFF_PARAMS                    */
/* Command struct:  bt_cmd_hc_set_incoming_sniff_params_type               */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
  uint16  min_interval;
  uint16  max_interval;
} bt_cmd_hc_set_incoming_sniff_params_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_Incoming_Park_Params                               */
/* Command Type:    BT_CMD_HC_SET_INCOMING_PARK_PARAMS                     */
/* Command struct:  bt_cmd_hc_set_incoming_park_params_type                */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
  uint16  min_interval;
  uint16  max_interval;
} bt_cmd_hc_set_incoming_park_params_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Set_Tx_Power_Level                                     */
/* Command Type:    BT_CMD_HC_SET_TX_POWER_LEVEL                           */
/* Command struct:  bt_cmd_hc_set_tx_power_level_type                      */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
  uint8   tx_power_level;
} bt_cmd_hc_set_tx_power_level_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Certification_Control                                  */
/* Command Type:    BT_CMD_HC_CERT_CTRL                                    */
/* Command struct:  bt_cmd_hc_cert_ctrl_type                               */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  fix_seqn_bit;     /* 0x01 to set SENQ=1; 0x00 for normal oper.    */ 
  uint8  fix_arqn_bit;     /* 0x01 to set ARQN=NAK; 0x00 for normal oper.  */
  uint8  fix_uap;          /* uap value; 0x00 for normal operation.        */
  uint8  fix_packet_flow;  /* 0x01 to set NAK for RX and Packet flow=STOP  */
  uint8  fix_l2cap_flow;   /* 0x01 to set L2CAP FLOW=STOP on TX            */
  uint8  fix_am_addr;      /* am_addr value; 0x00 for normal operation     */
  uint8  fix_implicit_nack;/* num pkt till implicit nack; 0x00 for normal. */
  uint8  fix_clk_offset;   /* 0x01 to fix clk offset; 0x00 for normal oper.*/
} bt_cmd_hc_cert_ctrl_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Tx_Continuous                                          */
/* Command Type:    BT_CMD_HC_TX_CONTINUOUS                                */
/* Command struct:  bt_cmd_hc_tx_continuous_type                           */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  hopping;        /* 0x01 for hopping mode; 0x00 for single freq.   */
} bt_cmd_hc_tx_continuous_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Tx_Continuous_Freq                                     */
/* Command Type:    BT_CMD_HC_TX_CONTINUOUS_FREQ                           */
/* Command struct:  bt_cmd_hc_tx_continuous_freq_type                      */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8  channel_number; /* transmit freq = 2402 + channel number (MHz)    */
} bt_cmd_hc_tx_continuous_freq_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_Raw_RSSI                                          */
/* Command Type:    BT_CMD_HC_RD_RAW_RSSI                                  */
/* Command struct:  bt_cmd_hc_rd_raw_rssi_type                             */
/* Return struct:   none                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16  conn_hndl;
} bt_cmd_hc_rd_raw_rssi_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Test_Set_LM_Params                                     */
/* Command Type:    BT_CMD_HC_TEST_SET_LM_PARAMS                           */
/* Command struct:  bt_cmd_hc_test_set_lm_params_type                      */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  bt_pair_type           pair_type;
  bt_encrypt_mode_type   max_encrypt_mode;
  boolean                cert_mode;
  boolean                park_supported;
  boolean                toggle_seqn_bit;
} bt_cmd_hc_test_set_lm_params_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Test_Send_LMP_Message                                  */
/* Command Type:    BT_CMD_HC_TEST_SEND_LMP_MSG                            */
/* Command struct:  bt_cmd_hc_test_send_lmp_msg_type                       */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16                      conn_hndl;
  uint8                       lmp_opcode; /* supported opcodes w/ this param
                                          BT_LMP_SET_BROADCAST_SCAN_WINDOW
                                          BT_LMP_TIMING_ACCURACY_REQ
                                          BT_LMP_MAX_SLOT
                                          BT_LMP_QUALITY_OF_SERVICE
                                          BT_LMP_PAGE_MODE_REQ
                                          BT_LMP_PAGE_SCAN_MODE_REQ
                                          BT_LMP_INCR_POWER_REQ
                                          BT_LMP_DECR_POWER_REQ
                                          */
  uint8                       max_slots;
  uint16                      poll_int_slots;
  uint16                      broadcast_scan_window;
  bt_page_scan_mode_type      psm;
  bt_page_scan_rep_mode_type  psrm;
} bt_cmd_hc_test_send_lmp_msg_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write_RAM                                              */
/* Command Type:    BT_CMD_HC_WR_RAM                                       */
/* Command struct:  bt_cmd_hc_wr_ram_type                                  */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8   num_bytes;
  uint32  addr;
  uint32  data;
} bt_cmd_hc_wr_ram_type;

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read_RAM                                               */
/* Command Type:    BT_CMD_HC_RD_RAM                                       */
/* Command struct:  bt_cmd_hc_rd_ram_type                                  */
/* Return struct:   bt_hc_rp_rd_ram_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8   num_bytes;
  uint32  addr;
} bt_cmd_hc_rd_ram_type;

#ifdef BT_SWDEV_2_1_PB
/*-------------------------------------------------------------------------*/
/* HCI Command:     Enhanced Flush                                         */
/* Command Type:    BT_CMD_HC_END_FLUSH                                    */
/* Command struct:  bt_cmd_hc_enh_flush_type                               */
/* Return struct:   None                                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint16   conn_hndl;
  uint8    pkt_type;
} bt_cmd_hc_enh_flush_type;
#endif /* BT_SWDEV_2_1_PB */

/*-------------------------------------------------------------------------*/
/*  Union of all BT commands.                                              */
/*-------------------------------------------------------------------------*/

typedef enum
{
  BT_CST_QUEUED = 0xDA01,
  BT_CST_PROCESSING,
  BT_CST_DONE,
  BT_CST_ERROR
} bt_cmd_state_type;

typedef struct
{
  q_link_type         link;
  bt_cmd_type         cmd_type;
  bt_app_id_type      bt_app_id;
  bt_cmd_status_type  cmd_status;
  bt_cmd_state_type   cmd_state;
  uint16              cmd_retries;
} bt_cmd_hdr_type;

#ifdef FEATURE_BT_SOC1
#error code not present
#endif /*FEATURE_BT_SOC1*/

/* dsm_ptr MUST be the first parameter in this structure !!                 */
/* This hc cmd is a special case. In hcdrv, for all other HC cmds, opcode   */
/* is found from a table using bt_cmd enum as the index. In this case, from */
/* the cmd name we only know OGF (6 bits) was VS OGF. But the full 16bit    */
/* opcode can be be any possible combination of the 10 bit OCF              */
/* Hence, opcode is supplied as a parameter to be used in hcdrv as a        */
/* "special" case. Table entry for this cmd ensures that only the first     */
/* parameter (i.e. dsm_ptr) is copied to the hci cmd payload                */
typedef BT_PACKED struct
{
  dsm_item_type  *dsm_ptr;
  uint16         opcode;
} bt_cmd_hc_vs_any_type;

#ifdef BT_SWDEV_2_1_EIR
/*-------------------------------------------------------------------------*/
/* HCI Command:     Read Extended Inquiry Response                         */
/* Command Type:    BT_CMD_HC_RD_EXT_INQ_RSP                               */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_ext_inq_rsp_type                           */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write Extended Inquiry Response                        */
/* Command Type:    BT_CMD_HC_WR_EXT_INQ_RSP                               */
/* Command struct:  bt_cmd_hc_wr_eir_type                                  */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8           fec_required;
  dsm_item_type*  dsm_ptr;  /* contains 240 octets of EIR data */ 
} bt_cmd_hc_wr_eir_type;

#define BT_HCI_CMD_WR_EIR_PARAM_LEN 241

/*-------------------------------------------------------------------------*/
/* HCI Command:     Read Inquiry Response TX Power                         */
/* Command Type:    BT_CMD_HC_RD_INQ_RSP_TX_PWR                            */
/* Command struct:  none                                                   */
/* Return struct:   bt_hc_rp_rd_inq_rsp_tx_pwr                             */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* HCI Command:     Write Inquiry TX Power                                 */
/* Command Type:    BT_CMD_HC_WR_INQ_TX_PWR                                */
/* Command struct:  bt_cmd_hc_wr_inq_tx_pwr_type                           */
/* Return struct:   bt_hc_rp_status_type                                   */
/*-------------------------------------------------------------------------*/
typedef BT_PACKED struct
{
  uint8               tx_power_level;
} bt_cmd_hc_wr_inq_tx_pwr_type;
#endif /* BT_SWDEV_2_1_EIR */

typedef BT_PACKED union
{
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#ifdef FEATURE_BT_AG
  bt_cmd_ag_enable_type                        cmd_ag_enable;
  bt_cmd_ag_connect_type                       cmd_ag_connect;
  bt_cmd_ag_config_type                        cmd_ag_config;
  bt_cmd_ag_ring_audio_device_type             cmd_ag_ring_audio_device;
  bt_cmd_ag_audio_connect_type                 cmd_ag_audio_conn;
  bt_cmd_ag_ad_spkr_gain_type                  cmd_ag_spkr;
  bt_cmd_ag_ad_mic_gain_type                   cmd_ag_mic;
  bt_cmd_ag_set_vr_capable_type                cmd_ag_set_vr;
  bt_cmd_ag_update_vr_state_type               cmd_ag_update_vr_state;
  bt_cmd_ag_ext_io_config_type                 cmd_ag_extio;
  bt_cmd_ag_set_inband_ring_type               cmd_ag_set_inband_ring;
  bt_cmd_ag_set_battchg_ind_type               cmd_ag_setbc;
  bt_cmd_ag_set_rssi_type                      cmd_ag_setrs;
  bt_cmd_ag_send_supported_rh_type             cmd_ag_send_supported_rh;
  bt_cmd_ag_send_current_rh_type               cmd_ag_send_current_rh;
#ifdef BT_AG_HF_EXTENSION
  bt_cmd_ag_send_pb_list_type                  cmd_ag_send_pb_list;
  bt_cmd_ag_send_pb_info_type                  cmd_ag_send_pb_info;
  bt_cmd_ag_send_pb_entries_info_type          cmd_ag_send_pb_entries_info;
  bt_cmd_ag_send_pb_select_result_type         cmd_ag_send_pb_select_result;
  bt_cmd_ag_send_pb_read_result_type           cmd_ag_send_pb_read_result;
  bt_cmd_ag_send_pb_write_result_type          cmd_ag_send_pb_write_result;
  bt_cmd_ag_send_subsc_num_info_type           cmd_ag_send_subsc_num_info;
#endif /* BT_AG_HF_EXTENSION */
#endif /* FEATURE_BT_AG */

  bt_cmd_sd_set_service_discoverable_mode_type cmd_sd_set_srv_discv_mode;
  bt_cmd_sd_get_discoverable_mode_type         cmd_sd_get_discv_mode;
  bt_cmd_sd_discover_devices_type              cmd_sd_discv_dev;
  bt_cmd_sd_start_device_discovery_type        cmd_sd_start_dev_discv;
  bt_cmd_sd_stop_device_discovery_type         cmd_sd_stop_dev_discv;
  bt_cmd_sd_get_device_name_type               cmd_sd_get_dev_name;
  bt_cmd_sd_get_device_name_cancel_type        cmd_sd_get_dev_name_cancel;
  bt_cmd_sd_search_service_type                cmd_sd_srv_srch;
  bt_cmd_sd_stop_service_search_type           cmd_sd_stop_srv_srch;
  bt_cmd_sd_get_server_channel_number_type     cmd_sd_get_scn;
  bt_cmd_sd_update_service_record_type         cmd_sd_update_srv_rec;
  bt_cmd_sd_cancel_idle_timer_type             cmd_sd_cancel_idle_timer;
#ifdef FEATURE_BT_EXTPF
  bt_cmd_sd_update_obex_profile_service_rec_type cmd_sd_update_obex_srv_rec;
#endif /* FEATURE_BT_EXTPF */
  bt_cmd_sd_register_service_type              cmd_sd_reg_srv;
  bt_cmd_sd_register_custom_service_type       cmd_sd_reg_custom_srv;
#ifdef FEATURE_BT_EXTPF_SML
  bt_cmd_sd_register_syncml_service_type       cmd_sd_reg_syncml_srv;
  bt_cmd_sd_unregister_syncml_service_type     cmd_sd_unreg_syncml_srv;
#endif
  bt_cmd_sd_unregister_service_type            cmd_sd_unreg_srv;
  bt_cmd_sd_unregister_custom_service_type     cmd_sd_unreg_custom_srv;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

  bt_cmd_sio_in_flow_type                      cmd_sio_in_flow;
  bt_cmd_sio_transmit_type                     cmd_sio_transmit;
  bt_cmd_sio_change_mode_type                  cmd_sio_change_mode;
  bt_cmd_sio_enable_autodetect_type            cmd_sio_enable_autodetect;
  bt_cmd_sio_set_flow_ctl_type                 cmd_sio_set_flow_ctl;
  bt_cmd_sio_enable_dtr_event_type             cmd_sio_enable_dtr_event;
  bt_cmd_sio_disable_dtr_event_type            cmd_sio_disable_dtr_event;
  bt_cmd_sio_cd_assert_type                    cmd_sio_cd_assert;
  bt_cmd_sio_cd_deassert_type                  cmd_sio_cd_deassert;
  bt_cmd_sio_ri_assert_type                    cmd_sio_ri_assert;
  bt_cmd_sio_ri_deassert_type                  cmd_sio_ri_deassert;
  bt_cmd_sio_dsr_assert_type                   cmd_sio_dsr_assert;
  bt_cmd_sio_dsr_deassert_type                 cmd_sio_dsr_deassert;
  bt_cmd_sio_record_pkt_func_ptr_type          cmd_sio_record_pkt_func_ptr;
  bt_cmd_sio_record_esc_func_ptr_type          cmd_sio_record_esc_func_ptr;
  bt_cmd_sio_bt_configure_type                 cmd_sio_bt_configure;
  bt_cmd_sio_bt_disconnect_type                cmd_sio_bt_disconnect;
  bt_cmd_sio_bt_flush_tx_type                  cmd_sio_bt_flush_tx;
  
  bt_cmd_rc_register_type                      cmd_rc_reg;
  bt_cmd_rc_connect_type                       cmd_rc_conn;
  bt_cmd_rc_conn_custom_cfg_type               cmd_rc_conn_cfg;
  bt_cmd_rc_tx_data_type                       cmd_rc_txd;
  bt_cmd_rc_set_port_rx_fc_type                cmd_rc_sprxfc;
  bt_cmd_rc_set_session_rx_fc_type             cmd_rc_ssrxfc;
  bt_cmd_rc_write_ms_type                      cmd_rc_wms;
  bt_cmd_rc_write_sp_cfg_type                  cmd_rc_wspc;  
  bt_cmd_rc_line_error_type                    cmd_rc_lerr;
  bt_cmd_rc_ping_type                          cmd_rc_ping;
  bt_cmd_rc_disconnect_type                    cmd_rc_disc;
  bt_cmd_rc_unregister_type                    cmd_rc_unreg;
  bt_cmd_rc_cancel_connect_type                cmd_rc_cncl_conn;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

  bt_cmd_l2_set_params_type                    cmd_l2_params;
  bt_cmd_l2_register_type                      cmd_l2_reg;
  bt_cmd_l2_connect_type                       cmd_l2_conn;
  bt_cmd_l2_write_type                         cmd_l2_write;
  bt_cmd_l2_disconnect_type                    cmd_l2_disc;
  bt_cmd_l2_unregister_type                    cmd_l2_unreg;
  bt_cmd_l2_ping_type                          cmd_l2_ping;
  bt_cmd_l2_connect_immediate_cid_type         cmd_l2_conn_imd_cid;
  bt_cmd_l2_disable_timeout_type               cmd_l2_disable_timeout;

  bt_cmd_rm_set_local_info_type                cmd_rm_setif;
#ifdef BT_SWDEV_2_1_EIR
  bt_cmd_rm_set_local_info_ext_type            cmd_rm_setif_ext;
  bt_cmd_rm_wr_eir_data_type                   cmd_rm_wr_eir;
  bt_cmd_rm_wr_eir_uuid_type                   cmd_rm_wr_uuid;        
#endif /* BT_SWDEV_2_1_EIR */
  bt_cmd_rm_config_application_type            cmd_rm_apcfg;
  bt_cmd_rm_auto_serv_search_disable           cmd_rm_ssdis;
  bt_cmd_rm_set_connectable_type               cmd_rm_setc;
  bt_cmd_rm_set_discoverable_type              cmd_rm_setd;
  bt_cmd_rm_inquire_type                       cmd_rm_inq;
  bt_cmd_rm_remote_name_req_type               cmd_rm_rname;
  bt_cmd_rm_remote_name_req_cancel_type        cmd_rm_rname_cancel;
  bt_cmd_rm_connect_acl_type                   cmd_rm_conna;
  bt_cmd_rm_connect_cancel_type                cmd_rm_conn_cancel;
  bt_cmd_rm_accept_acl_type                    cmd_rm_acpta;
  bt_cmd_rm_reject_acl_type                    cmd_rm_reja;
  bt_cmd_rm_tx_data_acl_type                   cmd_rm_txda;
  bt_cmd_rm_disconnect_acl_type                cmd_rm_disca;
  bt_cmd_rm_connect_sco_type                   cmd_rm_conns;
  bt_cmd_rm_accept_sco_type                    cmd_rm_acpts;
  bt_cmd_rm_reject_sco_type                    cmd_rm_rejs;
  bt_cmd_rm_tx_data_sco_type                   cmd_rm_txds;
  bt_cmd_rm_disconnect_sco_type                cmd_rm_discs;
  bt_cmd_rm_low_power_config_type              cmd_rm_lpcfg;
#ifdef BT_SWDEV_2_1_SSR
  bt_cmd_rm_low_power_config_ext_type          cmd_rm_lpcfg_ext;
#endif /* BT_SWDEV_2_1_SSR */
  bt_cmd_rm_set_conn_role_type                 cmd_rm_setrl;
  bt_cmd_rm_set_conn_role_sw_req_ok_type       cmd_rm_scrsr;
  bt_cmd_rm_set_conn_security_type             cmd_rm_setcs;
  bt_cmd_rm_set_dev_security_type              cmd_rm_setds;
  bt_cmd_rm_set_serv_security_type             cmd_rm_setss;
  bt_cmd_rm_set_serv_sec_ext_type              cmd_rm_setse;
  bt_cmd_rm_authorize_reply_type               cmd_rm_atzrp;
  bt_cmd_rm_enforce_sec_rfcomm_scn_type        cmd_rm_esscn;
  bt_cmd_rm_enforce_sec_l2cap_psm_type         cmd_rm_espsm;
  bt_cmd_rm_reg_link_status_type               cmd_rm_regls;
  bt_cmd_rm_unreg_link_status_type             cmd_rm_urgls;
  bt_cmd_rm_bond_type                          cmd_rm_bond;
  bt_cmd_rm_bond_cancel_type                   cmd_rm_bondc;
  bt_cmd_rm_unbond_type                        cmd_rm_unbnd;
  bt_cmd_rm_set_bondable_type                  cmd_rm_setbd;
  bt_cmd_rm_pin_reply_type                     cmd_rm_pin;
  bt_cmd_rm_dev_add_type                       cmd_rm_dvadd;
  bt_cmd_rm_dev_update_type                    cmd_rm_dvupd;
  bt_cmd_rm_dev_serv_update_type               cmd_rm_dsupd;
  bt_cmd_rm_dev_remove_type                    cmd_rm_dvrmv;
  bt_cmd_rm_test_mode_enable_type              cmd_rm_testm;
#ifdef FEATURE_BT_TELEC
  bt_cmd_rm_start_telec_test_type              cmd_rm_telec;
#endif
  bt_cmd_rm_disable_radio_type                 cmd_rm_dsrad;
  bt_cmd_rm_disable_discoverability_type       cmd_rm_dsdcv;
  bt_cmd_rm_set_device_black_list_type         cmd_rm_setbl;
  bt_cmd_rm_set_device_white_list_type         cmd_rm_setwl;
#ifdef FEATURE_BT_1_2
  bt_cmd_rm_config_afh_type                    cmd_rm_config_afh;
  bt_cmd_rm_enable_afh_type                    cmd_rm_enable_afh;
  bt_cmd_rm_set_afh_ch_map_type                cmd_rm_set_afh_ch_map;
#endif
  bt_cmd_rm_update_module_st_type              cmd_rm_upd_bms;
#ifdef BT_SWDEV_2_1_PB
  bt_cmd_rm_set_auto_flush_to_type             cmd_rm_set_auto_flush_to;
#endif /* BT_SWDEV_2_1_PB */
#ifdef BT_SWDEV_2_1_SSP
  bt_cmd_rm_set_io_cap_type                    cmd_rm_siocap;
  bt_cmd_rm_set_service_sm4_type               cmd_rm_setsssm4;
  bt_cmd_rm_user_confirmation_reply_type       cmd_rm_uconfrep;
  bt_cmd_rm_bond_ext_type                      cmd_rm_bondext;
  bt_cmd_rm_authorize_rebond_type              cmd_rm_areb;
  bt_cmd_rm_set_sm4_debug_mode_type            cmd_rm_sm4debug;
  bt_cmd_rm_passkey_reply_type                 cmd_rm_pkreply;
  bt_cmd_rm_keypress_notification_type         cmd_rm_kpn;
  bt_cmd_rm_wrt_remote_oob_data_type           cmd_rm_wro;
#endif /* BT_SWDEV_2_1_SSP */
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif
  
  bt_cmd_hc_register_type                      cmd_hc_reg;
  bt_cmd_hc_tx_acl_data_type                   cmd_hc_tx_acl_data;
  bt_cmd_hc_tx_sco_data_type                   cmd_hc_tx_sco_data;
  bt_cmd_hc_bd_addr_type                       cmd_hc_bd_addr;
  bt_cmd_hc_conn_hndl_type                     cmd_hc_conn_hndl;
  bt_cmd_hc_inquiry_type                       cmd_hc_inquiry;
  bt_cmd_hc_periodic_inq_type                  cmd_hc_per_inq;
  bt_cmd_hc_create_conn_type                   cmd_hc_create_conn;
  bt_cmd_hc_disconn_type                       cmd_hc_disconn;
  #ifdef FEATURE_BT_1_2
  bt_cmd_hc_create_conn_cancel_type            cmd_hc_create_conn_cancel;
  #endif
  bt_cmd_hc_add_sco_conn_type                  cmd_hc_add_sco;
  bt_cmd_hc_accept_conn_type                   cmd_hc_accept_conn;
  bt_cmd_hc_reject_conn_type                   cmd_hc_reject_conn;
  bt_cmd_hc_link_key_type                      cmd_hc_key;
  bt_cmd_hc_pin_code_type                      cmd_hc_pin;
  bt_cmd_hc_change_pkt_type_type               cmd_hc_change_pkt;
  bt_cmd_hc_set_conn_encrypt_type              cmd_hc_set_conn_encrypt;
  bt_cmd_hc_master_link_key_type               cmd_hc_master_key;
  bt_cmd_hc_remote_name_req_type               cmd_hc_remote_name_req;
  #ifdef FEATURE_BT_1_2
  bt_cmd_hc_remote_name_req_cancel_type        cmd_hc_remote_name_req_cancel;
  #endif
  bt_cmd_hc_hold_mode_type                     cmd_hc_hold_mode;
  bt_cmd_hc_sniff_mode_type                    cmd_hc_sniff_mode;
  bt_cmd_hc_exit_sniff_mode_type               cmd_hc_exit_sniff_mode;
  bt_cmd_hc_park_mode_type                     cmd_hc_park_mode;
  bt_cmd_hc_exit_park_mode_type                cmd_hc_exit_park_mode;
  bt_cmd_hc_qos_setup_type                     cmd_hc_qos_setup;
  bt_cmd_hc_switch_role_type                   cmd_hc_switch_role;
  bt_cmd_hc_wr_link_policy_type                cmd_hc_wr_link_policy;
  bt_cmd_hc_set_event_mask_type                cmd_hc_set_event_mask;
  bt_cmd_hc_set_event_filter_type              cmd_hc_set_event_filter;
  bt_cmd_hc_flush_type                         cmd_hc_flush;
  bt_cmd_hc_wr_pin_type                        cmd_hc_wr_pin_type;
  bt_cmd_hc_rd_link_key_type                   cmd_hc_rd_link_key;
  bt_cmd_hc_wr_link_key_type                   cmd_hc_wr_link_key;
  bt_cmd_hc_del_link_key_type                  cmd_hc_del_link_key;
  bt_cmd_hc_change_local_name_type             cmd_hc_change_local_name;
  bt_cmd_hc_wr_conn_accept_to_type             cmd_hc_wr_conn_accept_to;
  bt_cmd_hc_wr_page_to_type                    cmd_hc_wr_page_to;
  bt_cmd_hc_wr_scan_enable_type                cmd_hc_wr_scan_enable;
  bt_cmd_hc_wr_page_scan_act_type              cmd_hc_wr_page_scan_act;
  bt_cmd_hc_wr_inq_scan_act_type               cmd_hc_wr_inq_scan_act;
  bt_cmd_hc_wr_auth_enable_type                cmd_hc_wr_auth_enable;
  bt_cmd_hc_wr_encrypt_mode_type               cmd_hc_wr_encrypt_mode;
  bt_cmd_hc_wr_class_of_dev_type               cmd_hc_wr_class_of_dev;
  bt_cmd_hc_wr_voice_setting_type              cmd_hc_wr_voice_setting;
  bt_cmd_hc_wr_auto_flush_to_type              cmd_hc_wr_auto_flush_to;
  bt_cmd_hc_wr_broad_retrans_type              cmd_hc_wr_broad_retrans;
  bt_cmd_hc_wr_hold_mode_act_type              cmd_hc_wr_hold_mode_act;
  bt_cmd_hc_rd_tx_power_type                   cmd_hc_rd_tx_power;
  bt_cmd_hc_wr_sco_flow_ctrl_type              cmd_hc_wr_sco_flow_ctrl;
  bt_cmd_hc_set_hc_to_host_fc_type             cmd_hc_set_hc_to_host_fc;
  bt_cmd_hc_host_buf_size_type                 cmd_hc_host_buf_size;
  bt_cmd_hc_host_num_cmplt_pkt_type            cmd_hc_host_num_cmplt_pkt;
  bt_cmd_hc_wr_link_super_to_type              cmd_hc_wr_link_super_to;
  bt_cmd_hc_wr_curr_iac_lap_type               cmd_hc_wr_curr_iac_lap;
  bt_cmd_hc_wr_ps_per_mode_type                cmd_hc_wr_ps_per_mode;
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
  bt_cmd_hc_wr_ps_mode_type                    cmd_hc_wr_ps_mode;
#endif
  bt_cmd_hc_wr_loopback_mode_type              cmd_hc_wr_loopback_mode;
  bt_cmd_hc_fix_init_white_type                cmd_hc_fix_init_white;
  bt_cmd_hc_fix_init_enc_type                  cmd_hc_fix_init_enc;
  bt_cmd_hc_rd_rx_status_en_type               cmd_hc_rd_rx_status_en;
  bt_cmd_hc_config_srch_type                   cmd_hc_config_srch;
  bt_cmd_hc_rd_tx_buffer_en_type               cmd_hc_rd_tx_buf_en;
  bt_cmd_hc_rd_rx_buffer_en_type               cmd_hc_rd_rx_buf_en;
  bt_cmd_hc_send_raw_mode_pkt_type             cmd_hc_send_raw_mode_pkt;
#ifdef FEATURE_BT_TEST_MODE_TESTER
  bt_cmd_hc_tester_control_type                cmd_hc_tester_ctrl;
#endif /* FEATURE_BT_TEST_MODE_TESTER */
  bt_cmd_hc_modify_beacon_type                 cmd_hc_modify_beacon;
  bt_cmd_hc_rd_sbi_reg_type                    cmd_hc_rd_sbi_reg;
  bt_cmd_hc_wr_sbi_reg_type                    cmd_hc_wr_sbi_reg;
  bt_cmd_hc_diac_lap_en_type                   cmd_hc_diag_lap_en;
  bt_cmd_hc_wr_enc_key_length_type             cmd_hc_wr_enc_key_length;
  bt_cmd_hc_change_remote_pkt_type_type        cmd_hc_change_remote_pkt_type;
  bt_cmd_hc_status_view_en_type                cmd_hc_status_view_en;
  bt_cmd_hc_enable_loopback_type               cmd_hc_enable_loopback;
  bt_cmd_hc_set_incoming_hold_params_type      cmd_hc_set_inc_hold_params;
  bt_cmd_hc_set_incoming_sniff_params_type     cmd_hc_set_inc_sniff_params;
  bt_cmd_hc_set_incoming_park_params_type      cmd_hc_set_inc_park_params;
  bt_cmd_hc_set_tx_power_level_type            cmd_hc_set_tx_power_level;
  bt_cmd_hc_cert_ctrl_type                     cmd_hc_cert_ctrl;     
  bt_cmd_hc_tx_continuous_type                 cmd_hc_tx_continuous;
  bt_cmd_hc_tx_continuous_freq_type            cmd_hc_tx_continuous_freq;

  bt_cmd_hc_rd_raw_rssi_type                   cmd_hc_rd_raw_rssi;
  bt_cmd_hc_test_set_lm_params_type            cmd_hc_test_set_lm_params;
  bt_cmd_hc_test_send_lmp_msg_type             cmd_hc_test_send_lmp_msg;
  bt_cmd_hc_wr_ram_type                        cmd_hc_wr_ram;
  bt_cmd_hc_rd_ram_type                        cmd_hc_rd_ram;

#ifdef FEATURE_BT_1_2
  bt_cmd_hc_rd_rmt_ext_feat_type               cmd_hc_rd_rmt_ext_feat;
  bt_cmd_hc_set_afh_host_chan_class_type       cmd_hc_set_afh_host_chan_class;
  bt_cmd_hc_set_afh_chan_assess_mode_type      cmd_hc_set_afh_chan_assess_mode;
  bt_cmd_hc_read_afh_chan_map_type             cmd_hc_read_afh_chan_map;
  bt_cmd_hc_setup_sync_connection_type         cmd_hc_setup_sync_connection;
  bt_cmd_hc_accept_sync_conn_req_type          cmd_hc_accept_sync_conn_req;
  bt_cmd_hc_reject_sync_conn_req_type          cmd_hc_reject_sync_conn_req;
  bt_cmd_hc_control_afh_type                   cmd_hc_control_afh;
  bt_cmd_hc_set_afh_chan_good_type             cmd_hc_set_afh_chan_good;
  bt_cmd_hc_set_esco_pkt_len_type              cmd_hc_set_esco_pkt_len;
  bt_cmd_hc_wr_is_type_type                    cmd_hc_wr_is_type;
  bt_cmd_hc_wr_inq_mode_type                   cmd_hc_wr_inq_mode;
  bt_cmd_hc_wr_ps_type_type                    cmd_hc_wr_ps_type;
  bt_cmd_hc_rd_local_ext_feat_type                  cmd_hc_rd_loc_ext_feat;
#endif /* FEATURE_BT_1_2 */

#ifdef BT_SWDEV_2_1_PB
  bt_cmd_hc_enh_flush_type                     cmd_hc_enh_flush;
#endif /* BT_SWDEV_2_1_PB */
#ifdef BT_SWDEV_2_1_SSP
  bt_cmd_hc_io_cap_response_type               cmd_hc_io_cap_res;
  bt_cmd_hc_user_cfmtn_rqst_rep_type           cmd_hc_user_cfm_rqst_rep;
  bt_cmd_hc_user_cfmtn_rqst_neg_rep_type       cmd_hc_user_cfm_rqst_neg_rep;
  bt_cmd_hc_user_passkey_rqst_rep_type         cmd_hc_user_passkey_rqst_rep;
  bt_cmd_hc_user_passkey_rqst_neg_rep_type     cmd_hc_user_passkey_rqst_neg_rep;
  bt_cmd_hc_remote_oob_data_rqst_rep_type      cmd_hc_remote_oob_data_rqst_rep;
  bt_cmd_hc_remote_oob_data_rqst_neg_rep_type  cmd_hc_remote_oob_data_rqst_neg_rep;
  bt_cmd_hc_io_cap_rqst_neg_rep_type           cmd_hc_io_cap_rqst_neg_rep;
  bt_cmd_hc_refresh_encription_key_type        cmd_hc_refresh_enc_key;
  bt_cmd_hc_wr_sp_mode_type                    cmd_hc_wr_sp_mode;
  bt_cmd_hc_send_keypress_ntfctn_type_type     cmd_hc_send_keypress_ntfctn_type;
  bt_cmd_hc_wr_sp_debug_mode_type              cmd_hc_wr_sp_debug_mode;
#endif /* BT_SWDEV_2_1_SSP */

#ifdef FEATURE_BT_2_1
  bt_cmd_hc_wr_err_data_reporting_type         cmd_hc_wr_default_err_data_reporting;
#endif

#ifdef BT_SWDEV_2_1_SSR
  bt_cmd_hc_sniff_subrating_type               cmd_hc_ssr;
#endif /* BT_SWDEV_2_1_SSR */

#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

  bt_cmd_hc_vs_any_type                        cmd_hc_vs_any;

#ifdef BT_SWDEV_2_1_EIR
  //bt_cmd_hc_rd_eir_type                        cmd_hc_rd_eir_type;
  bt_cmd_hc_wr_eir_type                        cmd_hc_wr_eir;
  //bt_cmd_hc_rd_inq_rsp_tx_pwr_type             cmd_hc_rd_inq_rsp_tx_pwr_type;
  bt_cmd_hc_wr_inq_tx_pwr_type                 cmd_hc_wr_inq_tx_pwr; 
#endif /* BT_SWDEV_2_1_EIR */
} bt_cmd_params_union;

typedef struct bt_cmd_msg_struct
{
  bt_cmd_hdr_type      cmd_hdr;
  bt_cmd_params_union  cmd_msg;
} bt_cmd_msg_type;

/*-------------------------------------------------------------------------*/
/*         BT DRIVER functions to construct and send BT Commands           */
/*-------------------------------------------------------------------------*/

extern bt_cmd_msg_type* bt_get_free_cmd( void );

extern void bt_initialize_cmd_buffer( bt_cmd_msg_type*  bt_cmd_ptr );

extern void bt_free_cmd_buffer( bt_cmd_msg_type*   bt_cmd_ptr );

#endif /* FEATURE_BT */

#endif /* _BTCMDI_H */


