#ifndef _BTRCI_H
#define _BTRCI_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H    R F C O M M    I N T E R N A L    H E A D E R

DESCRIPTION
  This file contains declarations internal to the Bluetooth RFCOMM module.

Copyright (c) 2000-2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btrci.h_v   1.8   23 Oct 2001 16:52:04   propach
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btrci.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btrci.h_v  $
 *
 *    #14        29 May 2007            PR
 * Cleanup when p2p timer expires
 *
 *    #13        19 Oct 2006            DSN
 * Rename BB TX flow to ACL TX flow since HCI may provide this indication for
 * SOC solution.
 *
 *    #11-12     18 May 2006            JTL
 * Externalizing more functions for MFS calculations.
 *
 *   #10         16 Mar 2006            JTL
 * Added unreg_pending flag to keep track of pending unregistration
 * command processing.
 *
 *    #9         16 Nov 2005            GS
 * Added support for BD address based TX flow control in RFCOMM. 
 *
 *    #8         7  Apr 2005            DP
 * Don't allow CBFC to stall flow when RX watermark don't exceed count
 * is too small.
 *
 *    #7         16 Mar 2005            DP
 * Increased maximum number of BT SIO ports.
 *
 *    #6         8  Mar 2005            DP
 * Improved CBFC so it now uses actual space available in receive watermark
 * to determine number of transmit credits to give remote device.
 *
 *    #5         8  Jun 2004            DP
 * Added client side Mode 2 security.
 *
 *    #4         4  Nov 2003            DP
 * General cleanup.
 *
 *    #3         3  Oct 2003            DP
 * Added RFCOMM peer to peer communications monitoring.
 * 
 *    Rev 1.8   23 Oct 2001 16:52:04   propach
 * Added support for RFCOMM Credit Based Flow Control (CBFC).
 * 
 *    Rev 1.7   29 Aug 2001 11:55:24   propach
 * Merged certification tip onto main tip.
 * 
 *    Rev 1.6.1.1   17 Jul 2001 16:08:04   propach
 * Fixed priority checks.
 * 
 *    Rev 1.6.1.0   29 Jun 2001 15:25:02   waynelee
 * No change.
 * 
 *    Rev 1.6   08 Jun 2001 15:04:16   propach
 * Added RFCOMM link negotiations and modem status lines for SPP certification.
 * 
 *    Rev 1.5   26 Mar 2001 18:15:28   propach
 * Added RC commands and events to support serial port emulation settings
 * and modem status reading/writing.
 * 
 *    Rev 1.4   16 Mar 2001 17:28:22   propach
 * Added infrastructure for SPP including initial support of
 * RPN and RLS and reworking PN signaling messages.
 * 
 *    Rev 1.3   02 Mar 2001 11:57:56   propach
 * Added connected field to registration table.  Added credit
 * based flow control infrastructure to negotiation and DLC tables.
 * 
 *    Rev 1.2   26 Feb 2001 12:39:14   propach
 * Updated RC flow control.  Updated MSC and test cmd/resp.  Improved
 * QXDM message outputs.
 * 
 *    Rev 1.1   16 Feb 2001 16:11:36   propach
 * Completed EC routing of events by BT application ID.
 * 
 *    Rev 1.0   29 Jan 2001 17:09:24   propach
 * Initial revision.
 * 
 *    Rev 1.7   22 Nov 2000 11:45:20   sramacha
 * 
 *    Rev 1.6   24 Oct 2000 09:43:58   propach
 * Added fast FCS treatments for payload carrying UIH frames.
 *
 *    Rev 1.5   25 Aug 2000 16:45:54   propach
 * Memory leak fixes and some RFCOMM cleanup.
 *
 *    Rev 1.4   24 Aug 2000 13:47:28   propach
 * RFCOMM optimizations. Baseband fixes.
 *
 *    Rev 1.3   17 Aug 2000 13:45:34   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.2   02 Aug 2000 09:40:40   propach
 * Added QBTS support for BT.  Added to API.
 *
 *    Rev 1.1   19 Jul 2000 17:26:44   propach
 * Update BT API and featurization with cleanup.
 *
 *    Rev 1.0   04 Jul 2000 19:21:56   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include "dsm.h"
#include "queue.h"
#include "btrcrx.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*  RFCOMM Session and DLC definitions and declarations.                   */
/*-------------------------------------------------------------------------*/

#define BT_RC_MAX_PORTS_PER_SESSION BT_SPP_MAX_PORTS
#define BT_RC_MAX_SESSIONS 3
#define BT_RC_MAX_PORTS (BT_RC_MAX_SESSIONS * BT_RC_MAX_PORTS_PER_SESSION)


/*  Definitions Port IDs (AKA PIDs).  */

#define SIDX_DIDX_2_PID( sidx, didx ) ((sidx << 8) | didx)
#define SSN( sidx ) bt_rc_session[ sidx ]
#define DLC( sidx, didx ) SSN( sidx ).dlc[ didx ]

typedef enum
{
  BT_RCDS_DISCONNECTED = 1,
  BT_RCDS_CONNECTING_L2CAP,
  BT_RCDS_CONNECTING_DLC,
  BT_RCDS_CONNECTED,
  BT_RCDS_DISCONNECTING_DLC,
  BT_RCDS_DISCONNECTING_L2CAP
} bt_rc_dlc_state_type;


typedef struct  /*  Used for UIH frames carrying application data only.  */
{
  uint8           hdr_addr;
  uint8           hdr_ctrl;
  bt_rc_fcs_type  fcs;
} fast_uih_fcs_type;


typedef struct dlc_struct
{
  bt_rc_dlc_state_type  state;
  uint16                pid;
  uint16                expected_rx_mask;
  rex_timer_cnt_type    peer_to_peer_timer_ms_left;
  uint8                 dlci;  /*  Direction and server channel.  */
  boolean               direction;
  uint8                 server_channel;
  bt_app_id_type        bt_app_id;
  bt_rc_config_type     config;
  dsm_watermark_type*   tx_wm_ptr;
  dsm_watermark_type*   rx_wm_ptr;
  void                  (*orig_rx_wm_ge_fptr)( void );
  void                  (*my_rx_wm_ge_fptr)( void );
  dsm_item_type*        tx_pkt_ptr;
  fast_uih_fcs_type     tx_fast_uih_pf0;
  fast_uih_fcs_type     tx_fast_uih_pf1;
  fast_uih_fcs_type     rx_fast_uih_pf0;
  fast_uih_fcs_type     rx_fast_uih_pf1;
  uint16                ack_timeout;     /*  Units of 10 ms.  */
  uint16                mcc_timeout;     /*  Units of 10 ms.  */
  uint8                 local_v24_signals;
  uint8                 local_breaks;
  uint8                 local_last_break_length;
  uint8                 last_msc_tx_v24_signals;
  uint8                 last_msc_tx_breaks;
  uint8                 last_msc_tx_break_signals;
  uint8                 local_overruns;
  uint8                 local_parity_errors;
  uint8                 local_framing_errors;
  uint8                 last_rls_tx_overruns;
  uint8                 last_rls_tx_parity_errors;
  uint8                 last_rls_tx_framing_errors;
  uint8                 last_rls_tx_value1;
  uint8                 far_v24_signals;
  uint8                 far_breaks;
  uint8                 far_last_break_length;
  uint8                 far_overruns;
  uint8                 far_parity_errors;
  uint8                 far_framing_errors;
  uint16                tx_errors;
  uint16                rx_errors;
  uint32                tx_data_bytes;
  uint32                rx_data_bytes;
  uint32                tx_uih_frames;
  uint32                rx_uih_frames;
  uint8                 tx_sabm_frames;
  uint8                 rx_sabm_frames;
  uint8                 tx_ua_frames;
  uint8                 rx_ua_frames;
  uint8                 tx_dm_frames;
  uint8                 rx_dm_frames;
  uint8                 tx_disc_frames;
  uint8                 rx_disc_frames;
} bt_rc_dlc_type;


/*  dlci field definitions.  */

#define BT_DLC_DLCI_DIR_B  0x01  /*  Direction       */
#define BT_DLC_DLCI_SC_M   0x3E  /*  Server Channel  */
#define BT_DLC_SC_2_DLCI( sc ) (((sc) & (BT_DLC_DLCI_SC_M >> 1)) << 1)
#define BT_DLC_DLCI_2_SC( dlci ) (((dlci) & BT_DLC_DLCI_SC_M) >> 1)

#define BT_RC_SC_NULL 0xFF  /*  Invalid server channel number. */


/*  Maximum Frame Size definitions.  */

#define BT_RC_MFS_DEFAULT 127
#define BT_RC_MFS_MIN     23
#define BT_RC_MFS_MAX     32767


/*  Priority definition.  */

#define BT_RC_PRI_MAX 63


typedef struct ssn_struct
{
  uint8            ssn_idx;
  boolean          initiator;
  bt_bd_addr_type  bd_addr;
  boolean          acl_tx_flow_on;
  uint16           cid;
  uint32           l2_write_cmds;
  uint32           l2_rx_data_evs;
  bt_rc_dlc_type   mcc;
  uint8            dlc_count;
  bt_rc_dlc_type   dlc[ BT_RC_MAX_PORTS_PER_SESSION ];
  uint16           test_cmds_txd;
  uint16           test_cmds_rxd;
  uint16           test_resps_txd;
  uint16           test_resps_rxd;
  uint16           test_errors;
  uint16           last_test_tx_bt_app_id;
  uint16           last_test_tx_pid;
  uint16           last_test_tx_byte_count;
  uint8            last_test_tx_start_value;
  uint8            last_test_tx_increment;
  boolean          l2_disc_wait;
} bt_rc_session_type;


/*-------------------------------------------------------------------------*/
/*  RFCOMM registration table definitions.                                 */
/*-------------------------------------------------------------------------*/

typedef struct
{
  bt_app_id_type       bt_app_id;
  uint8                server_channel;
  dsm_watermark_type*  tx_wm_ptr;
  dsm_watermark_type*  rx_wm_ptr;
  boolean              connected;
  boolean              unreg_pending;
} bt_rc_reg_table_type;


/*-------------------------------------------------------------------------*/
/*  RFCOMM negotiation table definitions.                                  */
/*-------------------------------------------------------------------------*/

typedef struct
{
  bt_app_id_type     bt_app_id;
  uint8              dlci;
  uint8              server_channel;
  bt_bd_addr_type    bd_addr;
  uint16             expected_rx_mask;
  bt_rc_config_type  config;
} bt_rc_negot_table_type;


/*-------------------------------------------------------------------------*/
/*  DLC and negotiation table expected_rx_mask field definitions.          */
/*-------------------------------------------------------------------------*/

#define BT_ERXM_SABM_B         0x0001
#define BT_ERXM_UA_OR_DM_B     0x0002
#define BT_ERXM_MSC_CMD_B      0x0004
#define BT_ERXM_MSC_RESP_B     0x0008
#define BT_ERXM_FCON_RESP_B    0x0010
#define BT_ERXM_FCOFF_RESP_B   0x0020
#define BT_ERXM_TEST_RESP_B    0x0040
#define BT_ERXM_PN_RESP_B      0x0080
#define BT_ERXM_RPN_RESP_B     0x0100
#define BT_ERXM_RLS_RESP_B     0x0200


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                                Data                                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern bt_app_id_type bt_rc_app_id;
extern bt_rc_session_type bt_rc_session[];
extern uint8 bt_rc_reg_count;
extern bt_rc_reg_table_type bt_rc_reg_table[];
extern uint8 bt_rc_negot_count;
extern bt_rc_negot_table_type bt_rc_negot_table[];


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        Functions Prototypes                             */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_rc_calculate_fcs

DESCRIPTION
  Calculates the Frame Check Sequence (FCS) fields for a specified
  number of bytes.  byte_count must be 2 or 3 into this routine.

RETURN VALUE
  FCS

===========================================================================*/
extern bt_rc_fcs_type bt_rc_calculate_fcs
(
  uint8*  src_ptr,
  uint8   byte_count
);


/*===========================================================================

FUNCTION
  bt_rc_get_default_priority_for_dlci

DESCRIPTION
  This returns the default priority for a given DLCI value as specified
  in GSM 07.10 section 5.6.

RETURN VALUE
  Default priority for DLCI value.

===========================================================================*/
extern uint8 bt_rc_get_default_priority_for_dlci
(
  uint8  dlci
);


/*===========================================================================

FUNCTION
  bt_rc_write_sp_config

DESCRIPTION
  Updates the fields corresponding to an MCC RPN command of a
  specified configuration element.

RETURN VALUE
  TRUE if any field is changed, otherwise FALSE.

===========================================================================*/
extern boolean bt_rc_write_sp_config
(
  uint8                 dlci,
  bt_rc_config_type*    cfg_ptr,
  bt_rc_mcc_rpne_type*  rpne_msg_ptr
);

/*===========================================================================

FUNCTION
  bt_rc_find_regd_server_channel

DESCRIPTION
  Finds a specified server channel's entry in the RFCOMM
  registration table.

RETURN VALUE
  Pointer to RFCOMM registration table entry, if found; otherwise NULL.

===========================================================================*/
extern bt_rc_reg_table_type* bt_rc_find_regd_server_channel
(
  uint8  server_channel
);

/*===========================================================================

FUNCTION
  bt_rc_find_negot_table_entry

DESCRIPTION
  Finds the negotiation table entry with a specified BD address and DLCI.
  If not found, this routine will create a new one if specified.  This
  will either be a currently unused entry or the oldest entry in the
  table will be used.

RETURN VALUE
  Pointer to negotiation table entry if found; otherwise NULL.

===========================================================================*/
extern bt_rc_negot_table_type* bt_rc_find_negot_table_entry
(
  uint8             dlci,
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           create_entry
);

/*===========================================================================

FUNCTION
  bt_rc_calc_optimal_mfs

DESCRIPTION
  Determines optimal MFS size given various constraints.

===========================================================================*/
extern uint32 bt_rc_calc_optimal_mfs
(
  uint32              mtu,
  dsm_watermark_type* wm
);

/*===========================================================================

FUNCTION
  bt_rc_initialize_session_entry

DESCRIPTION
  Initializes a specified session table entry.

===========================================================================*/
extern void bt_rc_initialize_session_entry
(
  uint8  ssn_idx
);


/*===========================================================================

FUNCTION
  bt_rc_find_free_session_entry

DESCRIPTION
  Finds a free session table entry.

RETURN VALUE
  Pointer to free session entry, if found; otherwise NULL.

===========================================================================*/
extern bt_rc_session_type* bt_rc_find_free_session_entry
(
  void
);


/*===========================================================================

FUNCTION
  bt_rc_find_session_cid

DESCRIPTION
  Finds the session table entry of a specified L2CAP channel ID.

RETURN VALUE
  Pointer to session table entry, if found; otherwise NULL.

===========================================================================*/
extern bt_rc_session_type* bt_rc_find_session_cid
(
  uint16  cid
);


/*===========================================================================

FUNCTION
  bt_rc_find_session_bd_addr

DESCRIPTION
  Finds the session table entry of a specified remote BT device address.

RETURN VALUE
  Pointer to session table entry, if found; otherwise NULL.

===========================================================================*/
extern bt_rc_session_type* bt_rc_find_session_bd_addr
(
  bt_bd_addr_type*  bd_addr_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_find_free_dlc_entry

DESCRIPTION
  Finds a free DLC table entry within a specified session table entry.

RETURN VALUE
  Pointer to free DLC table entry, if found; otherwise NULL.

===========================================================================*/
extern bt_rc_dlc_type* bt_rc_find_free_dlc_entry
(
  bt_rc_session_type*  ssn_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_find_dlc_entry_dlci

DESCRIPTION
  Finds the DLC entry within a specified session corresponding to a
  specified DLCI.

RETURN VALUE
  Pointer to DLC table entry found; otherwise NULL.

===========================================================================*/
extern bt_rc_dlc_type* bt_rc_find_dlc_entry_dlci
(
  bt_rc_session_type*  ssn_ptr,
  uint8                dlci
);


/*===========================================================================

FUNCTION
  bt_rc_setup_local_server_dlc

DESCRIPTION
  Sets up a new DLC for a local server based on a received RFCOMM frame.
  Consistency checks are performed to validate this operation:
    1)  The received frame must be an SABM frame
    2)  The direction within the DLCI must be correct
    3)  The server channel must be correct and registered locally

RETURN VALUE
  dlc_ptr in rx_info:
    Updated if all is OK;  otherwise set to NULL.

===========================================================================*/
extern void bt_rc_setup_local_server_dlc
(
  bt_rc_rx_info_type*  rxi_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_set_local_server_dlc_config

DESCRIPTION
  Fills in the configuration settings of a specified local server DLC.
  Defaults are used if there is no entry in the negotiation table.
  The negotiation table would have an entry if a client performed
  Port Negotiations or Remote Port Negotiations via PN and/or RPN MCC
  messages prior to sending the SABM frame for this DLC.

===========================================================================*/
extern void bt_rc_set_local_server_dlc_config
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_set_local_client_dlc_config

DESCRIPTION
  Fills in the configuration settings of a specified local client DLC.
  Defaults are used if there was no configuration information previously
  set via a Connect with Custom Configuration command.

===========================================================================*/
extern void bt_rc_set_local_client_dlc_config
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
);

/*===========================================================================

FUNCTION
  bt_rc_bring_up_local_client_dlc

DESCRIPTION
  Transmits the required MCC commands and/or RFCOMM frames to bring
  up a specified local client DLC.

===========================================================================*/
extern void bt_rc_bring_up_local_client_dlc
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_loc_client_dlc_srv_sec_result

DESCRIPTION
  Processes service security results initiated by startup of a local
  client DLC.

===========================================================================*/
extern void bt_rc_loc_client_dlc_srv_sec_result
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr,
  boolean              secure_enough
);


/*===========================================================================

FUNCTION
  bt_rc_update_disconnected_dlc

DESCRIPTION
  Updates the session table entry and DLC table entry for a specified
  DLC that is now disconnected.

RETURN VALUE
  FALSE if tables are consistent with this disconnection, otherwise TRUE.

===========================================================================*/
extern boolean bt_rc_update_disconnected_dlc
(
  bt_rc_session_type*   ssn_ptr,
  bt_rc_dlc_type*       dlc_ptr,
  boolean               conn_failed,
  bt_event_reason_type  reason
);


/*===========================================================================

FUNCTION
  bt_rc_update_disconnected_mcc

DESCRIPTION
  Updates the session table entry for a specified MCC that is now
  disconnected.

RETURN VALUE
  FALSE if tables are consistent with this disconnection, otherwise TRUE.

===========================================================================*/
extern boolean bt_rc_update_disconnected_mcc
(
  bt_rc_session_type*  ssn_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_bring_down_dlc

DESCRIPTION
  Performs the next action necessary to bring down a specified DLC.

RETURN VALUE
  TRUE if tables are consistent with this disconnection, otherwise FALSE.

===========================================================================*/
extern boolean bt_rc_bring_down_dlc
(
  bt_rc_session_type*   ssn_ptr,
  bt_rc_dlc_type*       dlc_ptr,
  boolean               conn_failed,
  bt_event_reason_type  reason
);


/*===========================================================================

FUNCTION
  bt_rc_max_far_tx_credits

DESCRIPTION
  Determines the maximum number of transmit credits the remote side can
  currently be given without overflowing the receive watermark on a
  specified DLC.

RETURN VALUE
  Maximum number of transmit credits the far side can currently be given.
  
===========================================================================*/
extern bt_rc_cbfc_credits_type bt_rc_max_far_tx_credits
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
);


/*===========================================================================

FUNCTION
  bt_rc_far_tx_credits_needed

DESCRIPTION
  Determines whether the remote device of a specified DLC needs
  transmit credits sent to it.

RETURN VALUE
  TRUE if credits need to be sent to the remote device.
  FALSE when no credits need to be sent currently.
  
===========================================================================*/
extern boolean bt_rc_far_tx_credits_needed
(
  bt_rc_session_type*  ssn_ptr,
  bt_rc_dlc_type*      dlc_ptr
);


#endif /* FEATURE_BT */

#endif  /*  _BTRCI_H  */

