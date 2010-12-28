#ifndef BT_LOGI_H
#define BT_LOGI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    R F C O M M    R E C E I V E

GENERAL DESCRIPTION
  This file contains the Bluetooth code that contains the declarations for
  Bluetooth message logging via QXDM.

Copyright (c) 2000-2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btlogi.h_v   1.2   09 Oct 2002 12:14:52   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/protected/btlogi.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btlogi.h_v  $
 *   Rev 1.6    20 Oct 2006            SSK
 * Support for External SOC H4DS logging.
 *
 *   Rev 1.5    20 Sep 2006            DSN
 * Support for separate HCI command, event, ACL Tx and Rx log packets.
 *
 *   Rev 1.4    27 Jul 2006            DSN
 * Fixing a typo in featurization.
 *
 *   Rev 1.3    27 Jun 2006            DSN
 * Added support for Host HCI log packets.
 * 
 *    Rev 1.2   09 Oct 2002 12:14:52   waynelee
 * Add LOG_1X_BASE_C to BT log codes for 6xxx chips.
 * 
 *    Rev 1.1   07 Jun 2002 17:05:42   waynelee
 * Merged in changes necessary for Cheetah.
 * 
 *    Rev 1.0   29 Jan 2001 17:08:14   propach
 * Initial revision.
 * 
 *    Rev 1.3   11 Dec 2000 15:19:26   amitj
 * Defined 3 new literals for use in payload logging functions.
 * Added a header before each structure/literal-set definition with
 * proper explanation.
 *

===========================================================================*/

#ifdef FEATURE_BT
#include "comdef.h"
#include "log.h"
#include "ts.h"

#if defined( T_MSM6100 ) || defined( T_MSM6200 )
  #define BT_LOG_BASE  LOG_1X_BASE_C
#else
#error code not present
#endif

#ifdef  BT_SWDEV_QXDM_LOG_MSG
/*===========================================================================

DESCRIPTION
  The QXDM log messages are logged whenever a signalling message 
  is received or transmitted. The following log codes are defined
  for the messages being logged. The corresponding buffer sizes are
  also defined. Many of the message types use the same buffer size,
  hence the no. of buffer sizes defined are fewer in no. compared to
  the no. of message types.

===========================================================================*/

#define  log_bt_lp_rx         (BT_LOG_BASE + 72)
#define  log_bt_lp_tx         (BT_LOG_BASE + 73)
#define  log_bt_bbl2_rx       (BT_LOG_BASE + 74)
#define  log_bt_bbl2_tx       (BT_LOG_BASE + 75)
#define  log_bt_l2_sig_rx     (BT_LOG_BASE + 76)
#define  log_bt_l2_sig_tx     (BT_LOG_BASE + 77)
#define  log_bt_l2_pyld_rx    (BT_LOG_BASE + 78)
#define  log_bt_l2_pyld_tx    (BT_LOG_BASE + 79)
#define  log_bt_sd_sig_rx     (BT_LOG_BASE + 80)
#define  log_bt_sd_sig_tx     (BT_LOG_BASE + 81)
#define  log_bt_rc_sig_rx     (BT_LOG_BASE + 82)
#define  log_bt_rc_sig_tx     (BT_LOG_BASE + 83)
#define  log_bt_rc_pyld_rx    (BT_LOG_BASE + 84)
#define  log_bt_rc_pyld_tx    (BT_LOG_BASE + 85)


#define  BT_PYLD_BUFFER_SIZE       20
#define  BT_LP_LOG_BUFFER_SIZE     17
#define  BT_L2_LOG_BUFFER_SIZE     32
#define  BT_RC_LOG_BUFFER_SIZE     25
#define  BT_SD_RX_LOG_BUFFER_SIZE 128
#define  BT_SD_TX_LOG_BUFFER_SIZE  64

/*===========================================================================

DESCRIPTION
  These literals are used in QXDM payload logging functions, 
  such as bt_rc_pyld_logging_function. Currently, only the
  first and the last 10 bytes of the payload packet are
  being logged.

===========================================================================*/
#define  BT_FIRST_BYTE_OFFSET       0
#define  BT_FIRST_TEN_PYLD_BYTES   10
#define  BT_LAST_TEN_PYLD_BYTES    10

#define BT_LOG_ALLOC_VAR_SIZ( log_buf, var_buf_type, var_buf_len )   \
          ( sizeof( log_buf ) + ( var_buf_len - 1) * sizeof( var_buf_type ) )

/*===========================================================================

DESCRIPTION
  The following structures define the QXDM packet format for logging
  Host HCI packets received/transmitted.

===========================================================================*/
LOG_RECORD_DEFINE( LOG_BT_HOST_HCI_TX_C )
  uint8         buffer[1]; /* Actual size of buffer determined during 
                              log packet allocation */
LOG_RECORD_END

LOG_RECORD_DEFINE( LOG_BT_HOST_HCI_RX_C )
  uint8         buffer[1]; /* Actual size of buffer determined during 
                              log packet allocation */
LOG_RECORD_END


/*===========================================================================

DESCRIPTION
  The following structures define the QXDM packet format for logging
  HCI command and event packets at the Host HCI layer.

===========================================================================*/
LOG_RECORD_DEFINE( LOG_BT_HCI_CMD_C )
 uint8           cmd_buffer[1]; /* Buffer for Command packet. Size determined 
                                   during logging */
LOG_RECORD_END

LOG_RECORD_DEFINE( LOG_BT_HCI_EV_C )
 uint8           hci_pkt_hdr;     /* Event packet header for HCI over UART */
 uint8           hci_event_code; /* Event Opcode */
 uint8           hci_ev_par_len; /* Length of event parameters */
 uint8           hci_ev_par[1];  /* Buffer for event parameters. Size determined 
                                    during logging */
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structures define the QXDM packet format for logging
  H4DS deep sleep packets transmitted and received at the Host HCI layer.

===========================================================================*/
LOG_RECORD_DEFINE( LOG_BT_SOC_H4DS_C )
 uint8           h4ds_byte; /* single byte of a H4DS message */
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structures define the QXDM packet format for logging
  HCI ACL data packets transmitted and received at the Host HCI layer.

===========================================================================*/
LOG_RECORD_DEFINE( LOG_BT_HCI_TX_ACL_C )
 uint8           acl_buf[1]; /* Buffer for ACL packet. Size determined during 
                                logging */
LOG_RECORD_END

/* HCI ACL data header */
typedef BT_PACKED struct
{
  uint8                      pkt_type;     /* HCI packet type */
  uint16                     acl_conn_hdr; /* Connection header */
  uint16                     data_len;     /* Data length */
} bt_log_acl_data_hdr_type;

LOG_RECORD_DEFINE( LOG_BT_HCI_RX_ACL_C )
 bt_log_acl_data_hdr_type  acl_hdr;     /* Connection header */
 uint8                     data_buf[1]; /* Data payload buffer. Size determined
                                           during logging */
LOG_RECORD_END



/*===========================================================================

DESCRIPTION
  The following structures define the QXDM packet format for logging
  LMP signalling messages received/transmitted.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_lp_rx)
  uint16         length;
  uint8          idx;
  uint8          buffer[BT_LP_LOG_BUFFER_SIZE];
LOG_RECORD_END

LOG_RECORD_DEFINE(log_bt_lp_tx)
  uint16         length;
  uint8          idx;
  uint8          buffer[BT_LP_LOG_BUFFER_SIZE];
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structures define the QXDM packet format for logging
  L2CAP data messages received/transmitted at baseband layer.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_bbl2_rx)
  uint16         length;
  uint8          idx;
  uint8          buffer[BT_PYLD_BUFFER_SIZE];
LOG_RECORD_END

LOG_RECORD_DEFINE(log_bt_bbl2_tx)
  uint16         length;
  uint8          idx;
  uint8          buffer[BT_PYLD_BUFFER_SIZE];
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structures define the QXDM packet format for logging
  L2CAP signalling messages received/transmitted.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_l2_sig_rx)
  uint16         length;
  uint16         local_cid;
  uint8          buffer[BT_L2_LOG_BUFFER_SIZE];
LOG_RECORD_END

LOG_RECORD_DEFINE(log_bt_l2_sig_tx)
  uint16         length;
  uint16         local_cid;
  uint8          buffer[BT_L2_LOG_BUFFER_SIZE];
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structures define the QXDM packet format for logging
  L2CAP data messages received/transmitted.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_l2_pyld_rx)
  uint16         length;
  uint16         local_cid;
  uint8          buffer[BT_PYLD_BUFFER_SIZE];
LOG_RECORD_END

LOG_RECORD_DEFINE(log_bt_l2_pyld_tx)
  uint16         length;
  uint16         local_cid;
  uint8          buffer[BT_PYLD_BUFFER_SIZE];
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structures define the QXDM packet format for logging
  SDP signalling messages received/transmitted.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_sd_sig_rx)
  uint16         length;
  uint16         local_cid;
  uint8          buffer[BT_SD_RX_LOG_BUFFER_SIZE];
LOG_RECORD_END

LOG_RECORD_DEFINE(log_bt_sd_sig_tx)
  uint16         length;
  uint16         local_cid;
  uint8          buffer[BT_SD_TX_LOG_BUFFER_SIZE];
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structures define the QXDM packet format for logging
  RFCOMM signalling messages received/transmitted.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_rc_sig_rx)
  uint16         length;
  uint8          dlci;
  uint16         local_cid;
  uint8          buffer[BT_RC_LOG_BUFFER_SIZE];
LOG_RECORD_END

LOG_RECORD_DEFINE(log_bt_rc_sig_tx)
  uint16         length;
  uint8          dlci;
  uint16         local_cid;
  uint8          buffer[BT_RC_LOG_BUFFER_SIZE];
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structures define the QXDM packet format for logging
  RFCOMM data messages received/transmitted.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_rc_pyld_rx)
  uint16         length;
  uint8          dlci;
  uint16         local_cid;
  uint8          buffer[BT_PYLD_BUFFER_SIZE];
LOG_RECORD_END

LOG_RECORD_DEFINE(log_bt_rc_pyld_tx)
  uint16         length;
  uint8          dlci;
  uint16         local_cid;
  uint8          buffer[BT_PYLD_BUFFER_SIZE];
LOG_RECORD_END

#endif  /* BT_SWDEV_QXDM_LOG_MSG */

#ifdef  BT_SWDEV_QXDM_LOG_PER
/*===========================================================================

DESCRIPTION
  The QXDM snapshot log messages are logged periodically once every
  500 milliseconds. The following log codes are defined for the snaphot
  messages being logged. There is one message type for each bluetooth layer.
  The corresponding packet formats are defined below.

===========================================================================*/

#define  BT_DM_TIMER_MS   500

#define  log_bt_gen           (BT_LOG_BASE + 62)
#define  log_bt_bb_gen        (BT_LOG_BASE + 63)
#define  log_bt_bb_dev        (BT_LOG_BASE + 64)
#define  log_bt_lm            (BT_LOG_BASE + 65)
#define  log_bt_l2            (BT_LOG_BASE + 66)
#define  log_bt_sdp           (BT_LOG_BASE + 67)
#define  log_bt_rc_session    (BT_LOG_BASE + 68)
#define  log_bt_rc_dlci       (BT_LOG_BASE + 69)
#define  log_bt_ag            (BT_LOG_BASE + 70)
#define  log_bt_na            (BT_LOG_BASE + 71)


/*===========================================================================

DESCRIPTION
  The following structure defines the QXDM snapshot packet format 
  for logging general information about the bluetooth driver at the
  local device. There will be one packet logged every log period (500 ms).

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_gen)
  uint8          bt_driver_state;
  uint8          bt_hci_mode;
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structure defines the QXDM snapshot packet format 
  for logging baseband information about the local device. There will 
  be one packet logged every log period (500 ms).

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_bb_gen)
  uint8          bb_bt_state;
  uint8          bb_bd_addr[6];
  uint32         bb_bt_clk;
  uint32         bb_bt_clk_offset;
  uint16         bb_iac;

  uint32         bb_total_tx_pkts;
  uint32         bb_total_rx_pkts;
  uint16         bb_tx_voice_pkt;
  uint16         bb_rx_voice_pkt;

  uint16         bb_rx_status_read;
  uint8          bb_misc_rx_overflow;
  uint8          bb_event_overflow;
  uint8          bb_rf_chip;
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structure defines the QXDM snapshot packet format 
  for logging baseband information. There will be one packet logged for
  each physical layer connection to a remote BT device.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_bb_dev)
  uint8          bb_device_idx;
  uint8          bb_bt_device_state;
  uint8          bb_remote_bd_addr[6];
  uint8          bb_am_address;
  uint16         bb_est_poll_int;
  uint16         bb_enabled_pkt_type;
  uint8          bb_sco_encoding;

  uint16         bb_tx_power;
  uint16         bb_rx_power;
  uint16         bb_supervision_to;
  uint16         bb_poll_interval;

  uint8          bb_hold_mode_en;
  uint8          bb_sniff_mode_en;
  uint8          bb_park_mode_en;

  uint8          bb_l2cap_overflow;
  uint8          bb_tx_flow_on;
  uint8          bb_rx_flow_on;

  uint8          bb_encrypt_key[16];
  uint8          bb_tx_encrypt_on;
  uint8          bb_rx_encrypt_on;

  uint32         bb_pkt_err_rate;
  uint32         bb_rx_data_rate;
  uint32         bb_tx_data_bytes;
  uint32         bb_rx_data_bytes;

  uint16         bb_srch_hits;
  uint16         bb_tx_packet_cnt;
  uint16         bb_rx_packet_cnt;
  uint16         bb_retrans_pkt_cnt;
  uint16         bb_crc_fail_cnt; 
  uint16         bb_hec_fail_cnt; 
  uint16         bb_hdr_fec_corr_cnt; 
  uint16         bb_pyld_fec_corr_cnt; 
  uint16         bb_pyld_fec_fail_cnt; 
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structure defines the QXDM snapshot packet format 
  for logging LMP information. There will be one packet logged for
  each link layer connection to a remote BT device.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_lm)
  uint8          lm_device_idx;
  uint8          lm_acl_state;
  uint8          lm_role; 
  uint8          lm_sco1_state;
  uint8          lm_sco2_state;
  uint8          lm_link_mode; 
  uint8          lm_auth_state;
  uint8          lm_remote_auth_state;
  uint8          lm_encrypt_state;
  uint16         lm_conn_hndl;
LOG_RECORD_END


/*===========================================================================

DESCRIPTION
  The following structure defines the QXDM snapshot packet format 
  for logging L2CAP information. There will be one packet logged for
  each L2CAP channel.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_l2)
  uint8          l2cap_idx;
  uint8          l2cap_state;
  uint8          l2cap_config_req_state;
  uint8          l2cap_config_rsp_state;
  uint8          l2cap_conn_status;
  uint8          l2cap_bd_addr[6];
  uint16         l2cap_conn_hdl;
  uint16         l2cap_psm;
  uint16         l2cap_local_cid;
  uint16         l2cap_remote_cid;
  uint16         l2cap_result;
  uint16         l2cap_status;
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structure defines the QXDM snapshot packet format 
  for logging AG information. There will be one packet logged every
  log period (500 ms).

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_ag)
  uint8          ag_state;
  uint16         ag_app_id;
  uint8          ag_bd_addr[6];
  uint8          ag_my_server_channel;
  uint8          ag_remote_server_channel;
  int8           ag_rfcomm_pid;
  int8           ag_sco_handle;
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structure defines the QXDM snapshot packet format 
  for logging SDP information. There will be one packet logged for
  each SDP connection to a remote BT device.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_sdp)
  uint8          sdp_bd_addr[6];
  uint16         sdp_cid;
  uint16         sdp_tid;
  uint8          sdp_connected;
  uint8          sdp_request_pending;
  uint16         sdp_bt_app_id;
  uint16         sdp_num_resp_recv;
  uint16         sdp_tot_srv_rec_count;
  uint16         sdp_tot_attr_byte_count;
  uint16         sdp_attr_list_byte_count;
LOG_RECORD_END

/*===========================================================================

DESCRIPTION
  The following structures define the QXDM snapshot packet format 
  for logging RFCOMM session-specific and DLCI-specific information,
  respectively. There will be 1 packet logged for each RFCOMM session 
  and each RFCOMM DLCI.

===========================================================================*/
LOG_RECORD_DEFINE(log_bt_rc_session)
  uint8          rc_ssn_idx;
  uint8          rc_mcc_state;
  uint16         rc_cid;
  uint8          rc_bd_addr[6];
  uint8          rc_initiator;
LOG_RECORD_END

LOG_RECORD_DEFINE(log_bt_rc_dlci)
  uint8          rc_ssn_idx;
  uint8          rc_dlci;
  uint8          rc_dlci_state;
  uint16         rc_dlci_pid;
  uint8          rc_dlci_direction;
  uint8          rc_server_channel;
  uint16         rc_app_id;
  uint16         rc_max_frame_size;
  uint8          rc_expected_rx_mask; 
  uint8          rc_local_msc_v24_signals;
  uint8          rc_far_msc_v24_signals;
  uint16         rc_tx_errors;
  uint16         rc_rx_errors; 
  uint32         rc_tx_data_bytes;
  uint32         rc_rx_data_bytes;
  uint32         rc_tx_uih_frames;
  uint32         rc_rx_uih_frames;
  uint16         rc_tx_wm_max_cnt;
  uint16         rc_rx_wm_max_cnt;
  uint16         rc_tx_wm_current_cnt;
  uint16         rc_rx_wm_current_cnt;
LOG_RECORD_END

#endif /* BT_SWDEV_QXDM_LOG_PER */
#endif /* FEATURE_BT */
#endif /* BT_LOGI_H */

