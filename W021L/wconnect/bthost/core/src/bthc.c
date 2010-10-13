/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    H C I    M O D U L E

GENERAL DESCRIPTION
  This contains the highest level code for the Bluetooth HCI/LMP/BB layer.

Copyright (c) 2000-2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/bthc.c_v   1.15   20 Feb 2002 09:32:16   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/bthc.c#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/bthc.c_v  $
$Log:   O:/src/asw/COMMON/vcs/bthc.c_v  $
 *
 *   #27         31 Oct 2007            VK
 * Added support for Inquiry result event with RSSI.
 *
 *   #26        15 May 2007             VK
 * Queuing "Read Remote supported feature command" if controller is processing
 * other HCI command.
 *
 *   #24-25     18 Apr 2007             JH
 * Featurized code for handling optional BT 1.2 features.
 *
 *   #23        09 Feb 2007             DSN
 * Fix for a compile warning.
 *
 *   #22        20 Sep 2006             SSK
 * External SOC suppport: Featurize Vendor Specific event\command definitions.
 *
 *   #21        22 Aug 2006             DSN
 * Support HCI test mode on SOC solution.
 *
 *   #20        14 Aug 2006             DSN
 * Featurization to support SoC solution.
 *
 *   #19        26 Jul 2006             c_dmath
 * Added enabling of l2cap tx data cfm event by default.
 *
 *   #16        27 Jun 2006             DSN
 * Added featurization to support SoC solution.
 *
 *   #15        05 May 2006            MH
 * Added command complete log messages 
 *
 *    #14        16 Mar 2006            JH
 * Added missing call to bt_hc_cmd_reject_sync_conn_req().
 *
 *    #13        07 Mar 2006            RY
 * Added missing break statement in bt_hc_process_cmd.
 *
 *    #12        31 Oct 2005            JH
 * Added call to bt_hc_cmd_read_afh_chan_map() in bt_hc_process_cmd().
 *
 *    #11        19 Oct 2005            JH
 * Added support for the HCI read remote extended features command and read
 *  remote extended features complete event.
 * 
 *    #10        27 Jun 2005            JH
 * Added new BT 1.2 commands: Read_Inquiry_Scan_Type, Write_Inquiry_Scan_Mode,
 *  Read_Inquiry_Mode, and Write_Inquiry_Mode
 * Replaced BT 1.1 commands: Read_Page_Scan_Mode and Write_Page_Scan_Mode with
 *  BT 1.2 commands: Read_Page_Scan_Type and Write_Page_Scan_Type.
 * Added BT 1.2 events: Synchronous Connection Complete and 
 *  Synchronous Connection Changed.
 *
 *    #9         16 Jun 2005            DP
 * Implemented control of ACL packet types allowed on piconet.
 *
 *    #8         06 Apr 2005            JH
 * Adding support for disabling BT radio, featurized using BT_SWDEV_RADIO_OFF.
 *
 *    #7         12 Aug 2004            JH
 * Added TX continuous frequency HCI command
 *
 *    #6         7  Jul 2004            JH
 * Added support for BT 1.2 AFH and eSCO HCI commands
 * 
 *    Rev 1.15   20 Feb 2002 09:32:16   waynelee
 * Separated out RX and TX fast path, and fixed feature names.
 * 
 *    Rev 1.14   12 Feb 2002 14:16:22   waynelee
 * Renamed some functions.
 * 
 *    Rev 1.13   13 Sep 2001 16:31:20   waynelee
 * Added peek/poke functionality via QBTS.
 * 
 *    Rev 1.12   29 Aug 2001 11:44:56   propach
 * Merged certification tip onto main tip.
 * 
 *    Rev 1.11.1.3   14 Aug 2001 19:21:02   waynelee
 * Renamed CMD_HC_FIX_PKT_HDR to CMD_HC_CERT_CTRL.  Removed
 * obsolete qdsp image ctrl cmds.
 * 
 *    Rev 1.11.1.2   30 Jul 2001 17:07:50   waynelee
 * Added new hci cmds: rd_raw_rssi, test_set_lm_params,
 * test_send_lmp_msg.
 * 
 *    Rev 1.11.1.1   18 Jul 2001 16:51:20   propach
 * Added APIT HCI mode loading the stand alone DSP image.
 * 
 *    Rev 1.11.1.0   29 Jun 2001 15:22:44   waynelee
 * No change.
 * 
 *    Rev 1.11   29 Jun 2001 11:31:26   waynelee
 * Added HCI mechanism to not automatically unload QDSP image.
 * 
 *    Rev 1.10   28 Jun 2001 11:15:34   waynelee
 * fixed debug text
 * 
 *    Rev 1.9   27 Jun 2001 11:23:26   waynelee
 * Added cmds for telec and bb certification.
 * 
 *    Rev 1.8   11 Jun 2001 10:13:22   waynelee
 * Always get and reg app_id during powerup_init().
 * 
 *    Rev 1.7   08 Jun 2001 16:13:10   waynelee
 * Added support to the driver and not the phone for HCI_Reset.
 * Added support for HCI_Set_Tx_Power_Level.
 * 
 *    Rev 1.6   20 Apr 2001 16:23:20   waynelee
 * Changed symbols from bt_hci_ to bt_hc_
 * 
 *    Rev 1.5   21 Mar 2001 16:47:18   waynelee
 * Added support for these commands: Set_QDSP_Image,
 * Set_Incoming_Hold_Params, Set_Incoming_Sniff_Params,
 * Set_Incoming_Park_Params.
 * 
 *    Rev 1.4   02 Mar 2001 00:32:02   ryuen
 * Renamed bt_xx_offline to bt_xx_qdsp_reset_shutdown
 * 
 *    Rev 1.3   23 Feb 2001 13:08:04   ryuen
 * Added the call to bt_hci_pe_init to the powerup initialization sequence
 * 
 *    Rev 1.2   16 Feb 2001 16:10:48   propach
 * Completed EC routing of events by BT application ID.
 * 
 *    Rev 1.1   07 Feb 2001 10:17:00   propach
 * Reworked internal BT application IDs and made event control
 * route based on these.
 * 
 *    Rev 1.0   29 Jan 2001 17:06:24   propach
 * Initial revision.
 * 
 *    Rev 1.15   21 Dec 2000 17:39:00   waynelee
 * Removed T_CORE_EMUL.  Change semantic of BT_TEST_PYLD_FAST
 * to what the name says.
 * 
 *    Rev 1.14   22 Nov 2000 12:17:00   waynelee
 * Added support for enable_loopback HCI command
 * 
 *    Rev 1.13   27 Oct 2000 17:57:18   waynelee
 * Process new hci commands
 * 
 *    Rev 1.12   25 Oct 2000 09:53:36   waynelee
 * Correctly direct the data to the hci parser in hci mode.
 * 
 *    Rev 1.11   04 Oct 2000 15:10:44   propach
 * RF test mode changes per WAL (corrected).
 * 
 *    Rev 1.9   19 Sep 2000 18:11:10   ryuen
 * Updated bt_hci_online() to call bt_hci_etc_init()
 * 
 *    Rev 1.8   16 Sep 2000 15:17:12   propach
 * Avoid queueing payload data.
 *
 *    Rev 1.7   05 Sep 2000 16:43:42   waynelee
 * Added online, offline support.  Added debug MSGs.  Added Robustness.
 *
 *    Rev 1.6   30 Aug 2000 09:41:30   waynelee
 * Made changes for new BT_MSG macros
 *
 *    Rev 1.5   29 Aug 2000 08:24:50   propach
 * Moved comdef.h includes above FEATURE_BT to remove compiler
 * warning when feature is disabled.
 *
 *    Rev 1.4   17 Aug 2000 13:43:14   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.3   03 Aug 2000 08:41:18   waynelee
 * Adding Low Power Mode Support.  Code for UPF-3
 *
 *    Rev 1.2   19 Jul 2000 17:25:14   propach
 * Update BT API and featurization with cleanup.
 *
 *    Rev 1.1   06 Jul 2000 13:21:36   waynelee
 * Added V/I features: change to AUX packet types, set new bd addr.
 * Writes to QDSP CRC table for 0 length packets after connection.
 * Got HCI mode working on both standalone and aarvark targets. verified
 * AG/HS connection on AARVARK target.
 *
 *    Rev 1.0   04 Jul 2000 19:19:08   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#include "btdebug.h"
#include "bthc.h"
#ifndef FEATURE_BT_SOC 
#include "bthcetc.h"
#include "bthchcbc.h"
#endif /* FEATURE_BT_SOC */
#include "bthci.h"
#ifndef FEATURE_BT_SOC 
#include "bthclc.h"
#include "bthclp.h"
#endif /* FEATURE_BT_SOC */
#include "bthcpe.h"
#ifndef FEATURE_BT_SOC 
#include "bthcev.h"
#endif /* FEATURE_BT_SOC */
#include "btmsg.h"
#include "btutils.h"

#ifdef BT_TEST_PYLD_FAST_RX
#include "btrm.h"
#endif

#define BT_MSG_LAYER  BT_MSG_HC   /* necessary for btmsg.h */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

bt_app_id_type  bt_hc_app_id = BT_APP_ID_NULL;


/*  This variable that MUST NOT be initialized during powerup.  */
/*  It is set when HCI sends the Reset command and when we      */
/*  reset the driver, we need to make sure we end up in HCI     */
/*  mode after reset and then send the command complete.        */
/*  After that we clear this variable.  The powerup_init        */
/*  function is called during reset, so this variable cannot    */
/*  initialized in there or it breaks this functionality.       */
bt_hci_mode_type  bt_hc_prev_hci_mode = BT_HCIM_OFF;


/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/


#ifdef BT_SWDEV_RADIO_OFF
/*===========================================================================

FUNCTION
  bt_hc_reset

DESCRIPTION
  Resets the HCI layer of the driver.

===========================================================================*/
void bt_hc_reset( void )
{ 
#ifndef FEATURE_BT_SOC
  bt_hc_etc_init();
#endif /* FEATURE_BT_SOC */
  bt_hc_pe_init();
}
#endif /* BT_SWDEV_RADIO_OFF */


/*===========================================================================

FUNCTION
  bt_hc_powerup_init

DESCRIPTION
  Initializes the HCI Module upon powerup

===========================================================================*/
void bt_hc_powerup_init( void )
{

  bt_hc_app_id = bt_cmd_ec_get_application_id();
  ASSERT_DEBUG( bt_hc_app_id != BT_APP_ID_NULL );
  BT_MSG_API( "BT HC TRX: EC Get AID %x", bt_hc_app_id, 0, 0 );
  BT_MSG_HIGH( "BT HC App ID is %x", bt_hc_app_id, 0, 0 );

#ifdef BT_SWDEV_RADIO_OFF
  bt_hc_reset();
#else
  bt_hc_etc_init();
  bt_hc_pe_init();
#endif /* BT_SWDEV_RADIO_OFF */
}

/*===========================================================================

FUNCTION
  bt_hc_qdsp_ready_init

DESCRIPTION
  Initialize the HCI Module when the Bluetooth QDSP image is loaded

===========================================================================*/
void bt_hc_qdsp_ready_init( void )
{
}

/*===========================================================================

FUNCTION
  bt_hc_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the Bluetooth HCI Module when the Bluetooth QDSP image is
  unloaded

===========================================================================*/
void bt_hc_qdsp_reset_shutdown( void )
{
  // TBD - IMAGE_SWAP
#ifndef FEATURE_BT_SOC
  bt_hc_pe_qdsp_reset_shutdown();
#endif /* FEATURE_BT_SOC */
}

#ifndef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_hc_process_cmd

DESCRIPTION
  Process the BT command sent to the HCI/LMP/BB layer.

===========================================================================*/
void bt_hc_process_cmd
(
  bt_cmd_msg_type*  bt_cmd_ptr  /* The BT cmd to process */
)
{
  /* Assumes command succeeds */
  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;

  switch ( bt_cmd_ptr->cmd_hdr.cmd_type )
  {
    // ---------------------------------------------------------------------
    // HCI/LMP/BB Module Commands
    // ---------------------------------------------------------------------
    case BT_CMD_HC_REGISTER:
      BT_MSG_API( "BT HC CMD RX: HC Register", 0, 0, 0 );
// BT_TBD     bt_hc_cmd_register( &bt_cmd_ptr->cmd_msg.cmd_hc_reg );
      break;

    // ---------------------------------------------------------------------
    // Data Commands
    // ---------------------------------------------------------------------
    case BT_CMD_HC_TX_ACL_DATA:
      BT_MSG_API_PYLD( "BT HC CMD RX: HC TX ACL Data", 0, 0, 0 );
      bt_hc_cmd_tx_acl_data( &bt_cmd_ptr->cmd_msg.cmd_hc_tx_acl_data );
      break;

    case BT_CMD_HC_TX_SCO_DATA:
      BT_MSG_API_PYLD( "BT HC CMD RX: HC TX SCO Data", 0, 0, 0 );
      bt_hc_cmd_tx_sco_data( &bt_cmd_ptr->cmd_msg.cmd_hc_tx_sco_data );
      break;

    // ---------------------------------------------------------------------
    // Link Control Commands
    // ---------------------------------------------------------------------
    case BT_CMD_HC_INQUIRY:
      BT_MSG_API( "BT HC CMD RX: HC Inq", 0, 0, 0 );
      bt_hc_cmd_inquiry( &bt_cmd_ptr->cmd_msg.cmd_hc_inquiry );
      break;

    case BT_CMD_HC_INQUIRY_CANCEL:
      BT_MSG_API( "BT HC CMD RX: HC Inq Cancel", 0, 0, 0 );
      bt_hc_cmd_inquiry_cancel();
      break;

    case BT_CMD_HC_PERIODIC_INQUIRY:
      BT_MSG_API( "BT HC CMD RX: HC Per Inq", 0, 0, 0 );
      bt_hc_cmd_per_inq_mode( &bt_cmd_ptr->cmd_msg.cmd_hc_per_inq );
      break;

    case BT_CMD_HC_EXIT_PERIODIC_INQUIRY:
      BT_MSG_API( "BT HC CMD RX: HC Exit Per Inq", 0, 0, 0 );
      bt_hc_cmd_exit_per_inq_mode();
      break;

    case BT_CMD_HC_CREATE_CONNECTION:
      BT_MSG_API( "BT HC CMD RX: HC Create Conn", 0, 0, 0 );
      bt_hc_cmd_create_connection( &bt_cmd_ptr->cmd_msg.cmd_hc_create_conn );
      break;

    case BT_CMD_HC_DISCONNECT:
      BT_MSG_API( "BT HC CMD RX: HC Disconn", 0, 0, 0 );
      bt_hc_cmd_disconnect( &bt_cmd_ptr->cmd_msg.cmd_hc_disconn );
      break;

    case BT_CMD_HC_ADD_SCO_CONN:
      BT_MSG_API( "BT HC CMD RX: HC Add SCO", 0, 0, 0 );
#ifdef FEATURE_BT_1_2
      BT_MSG_API( "HC Add SCO Disallowed Depreciated Command in BT1.2", 0, 0, 0 );
      bt_hc_send_cmd_status_ev( BT_BE_UNKNOWN_HCI_CMD, BT_HCI_ADD_SCO_CONNECTION );
#else
      bt_hc_cmd_add_sco_conn( &bt_cmd_ptr->cmd_msg.cmd_hc_add_sco );
#endif
      break;



    case BT_CMD_HC_ACCEPT_CONN:
      BT_MSG_API( "BT HC CMD RX: HC Accept Conn", 0, 0, 0 );
      bt_hc_cmd_accept_conn_req( &bt_cmd_ptr->cmd_msg.cmd_hc_accept_conn );
      break;

    case BT_CMD_HC_REJECT_CONN:
      BT_MSG_API( "BT HC CMD RX: HC Reject Conn", 0, 0, 0 );
      bt_hc_cmd_reject_conn_req( &bt_cmd_ptr->cmd_msg.cmd_hc_reject_conn );
      break;

    case BT_CMD_HC_LINK_KEY:
      BT_MSG_API( "BT HC CMD RX: HC Link Key Req Reply", 0, 0, 0 );
      bt_hc_cmd_link_key_req_reply( &bt_cmd_ptr->cmd_msg.cmd_hc_key );
      break;

    case BT_CMD_HC_LINK_KEY_NEG:
      BT_MSG_API( "BT HC CMD RX: HC Link Key Req Neg Reply", 0, 0, 0 );
      bt_hc_cmd_link_key_req_neg_reply( &bt_cmd_ptr->cmd_msg.cmd_hc_bd_addr );
      break;

    case BT_CMD_HC_PIN_CODE:
      BT_MSG_API( "BT HC CMD RX: HC PIN Code Req Reply", 0, 0, 0 );
      bt_hc_cmd_pin_code_req_reply( &bt_cmd_ptr->cmd_msg.cmd_hc_pin );
      break;

    case BT_CMD_HC_PIN_CODE_NEG:
      BT_MSG_API( "BT HC CMD RX: HC PIN Code Req Neg Reply", 0, 0, 0 );
      bt_hc_cmd_pin_code_req_neg_reply( &bt_cmd_ptr->cmd_msg.cmd_hc_bd_addr );
      break;

    case BT_CMD_HC_CHANGE_PKT_TYPE:
      BT_MSG_API( "BT HC CMD RX: HC ChgPktType %x H %x",
                  bt_cmd_ptr->cmd_msg.cmd_hc_change_pkt.pkt_type,
                  bt_cmd_ptr->cmd_msg.cmd_hc_change_pkt.conn_hndl, 0 );
      bt_hc_cmd_change_conn_pkt_type( &bt_cmd_ptr->cmd_msg.cmd_hc_change_pkt );
      break;

    case BT_CMD_HC_AUTH_REQ:
      BT_MSG_API( "BT HC CMD RX: HC Auth Req", 0, 0, 0 );
      bt_hc_cmd_auth_req( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_SET_CONN_ENCRYPT:
      BT_MSG_API( "BT HC CMD RX: HC Set Conn Encrypt", 0, 0, 0 );
      bt_hc_cmd_set_conn_encrypt( &bt_cmd_ptr->cmd_msg.cmd_hc_set_conn_encrypt );
      break;

    case BT_CMD_HC_CHANGE_LINK_KEY:
      BT_MSG_API( "BT HC CMD RX: HC Change Link Key", 0, 0, 0 );
      cmd_status =
        bt_hc_cmd_change_conn_link_key( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_MASTER_LINK_KEY:
      BT_MSG_API( "BT HC CMD RX: HC Master Link Key", 0, 0, 0 );
      bt_hc_cmd_master_link_key( &bt_cmd_ptr->cmd_msg.cmd_hc_master_key );
      break;

    case BT_CMD_HC_REMOTE_NAME_REQ:
      BT_MSG_API( "BT HC CMD RX: HC Remote Name Req", 0, 0, 0 );
      cmd_status = bt_hc_cmd_remote_name_req( 
                     &bt_cmd_ptr->cmd_msg.cmd_hc_remote_name_req );
      break;

    case BT_CMD_HC_RD_REMOTE_FEATURES:
      BT_MSG_API( "BT HC CMD RX: HC Rd Remote Feat", 0, 0, 0 );
      cmd_status = bt_hc_cmd_rd_remote_supported_features( 
                        &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

#ifdef FEATURE_BT_1_2
    case BT_CMD_HC_RD_REMOTE_EXT_FEATURES:
      BT_MSG_API( "BT HC CMD RX: HC Rd Remote Ext Feat", 0, 0, 0 );
      bt_hc_cmd_rd_remote_extended_features( 
        &bt_cmd_ptr->cmd_msg.cmd_hc_rd_rmt_ext_feat );
      break;
#endif /* FEATURE_BT_1_2 */

    case BT_CMD_HC_RD_REMOTE_VERSION:
      BT_MSG_API( "BT HC CMD RX: HC Rd Remote Ver", 0, 0, 0 );
      bt_hc_cmd_rd_remote_version_info( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_RD_CLK_OFFSET:
      BT_MSG_API( "BT HC CMD RX: HC Rd Remote Clk Offset", 0, 0, 0 );
      bt_hc_cmd_rd_clk_offset( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    // ---------------------------------------------------------------------
    // Link Policy Commands
    // ---------------------------------------------------------------------
    case BT_CMD_HC_HOLD_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Hold Mode", 0, 0, 0 );
      bt_hc_cmd_hold_mode( &bt_cmd_ptr->cmd_msg.cmd_hc_hold_mode );
      break;

    case BT_CMD_HC_SNIFF_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Sniff Mode", 0, 0, 0 );
      bt_hc_cmd_sniff_mode( &bt_cmd_ptr->cmd_msg.cmd_hc_sniff_mode );
      break;

    case BT_CMD_HC_EXIT_SNIFF_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Exit Sniff Mode", 0, 0, 0 );
      bt_hc_cmd_exit_sniff_mode( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_PARK_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Park Mode", 0, 0, 0 );
      bt_hc_cmd_park_mode( &bt_cmd_ptr->cmd_msg.cmd_hc_park_mode );
      break;

    case BT_CMD_HC_EXIT_PARK_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Exit Park Mode", 0, 0, 0 );
      bt_hc_cmd_exit_park_mode( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_QOS_SETUP:
      BT_MSG_API( "BT HC CMD RX: HC QoS Setup", 0, 0, 0 );
      bt_hc_cmd_qos_setup( &bt_cmd_ptr->cmd_msg.cmd_hc_qos_setup );
      break;

    case BT_CMD_HC_ROLE_DISCOVERY:
      BT_MSG_API( "BT HC CMD RX: HC Role Discovery", 0, 0, 0 );
      bt_hc_cmd_role_discovery( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_SWITCH_ROLE:
      BT_MSG_API( "BT HC CMD RX: HC Switch Role", 0, 0, 0 );
      bt_hc_cmd_switch_role( &bt_cmd_ptr->cmd_msg.cmd_hc_switch_role );
      break;

    case BT_CMD_HC_RD_LINK_POLICY:
      BT_MSG_API( "BT HC CMD RX: HC Rd Link Policy", 0, 0, 0 );
      bt_hc_cmd_rd_link_policy( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_WR_LINK_POLICY:
      BT_MSG_API( "BT HC CMD RX: HC Wr Link Policy", 0, 0, 0 );
      bt_hc_cmd_wr_link_policy( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_link_policy );
      break;

    // ---------------------------------------------------------------------
    // Host Control & Baseband Commands
    // ---------------------------------------------------------------------
    case BT_CMD_HC_SET_EVENT_MASK:
      BT_MSG_API( "BT HC CMD RX: HC Set Event Mask", 0, 0, 0 );
      bt_hc_cmd_set_event_mask( &bt_cmd_ptr->cmd_msg.cmd_hc_set_event_mask );
      break;

    case BT_CMD_HC_RESET:
      BT_MSG_API( "BT HC CMD RX: HC Reset", 0, 0, 0 );
      bt_hc_cmd_reset();
      break;

    case BT_CMD_HC_SET_EVENT_FILTER:
      BT_MSG_API( "BT HC CMD RX: HC Set Event Filter", 0, 0, 0 );
      bt_hc_cmd_set_event_filter( &bt_cmd_ptr->cmd_msg.cmd_hc_set_event_filter );
      break;

    case BT_CMD_HC_FLUSH:
      BT_MSG_API( "BT HC CMD RX: HC Flush", 0, 0, 0 );
      bt_hc_cmd_flush( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_RD_PIN_TYPE:
      BT_MSG_API( "BT HC CMD RX: HC Rd PIN Type", 0, 0, 0 );
      bt_hc_cmd_rd_pin_type();
      break;

    case BT_CMD_HC_WR_PIN_TYPE:
      BT_MSG_API( "BT HC CMD RX: HC Wr PIN Type", 0, 0, 0 );
      bt_hc_cmd_wr_pin_type( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_pin_type );
      break;

    case BT_CMD_HC_CREATE_NEW_UNIT_KEY:
      BT_MSG_API( "BT HC CMD RX: HC Create New Unit Key", 0, 0, 0 );
      bt_hc_cmd_create_new_unit_key();
      break;

    case BT_CMD_HC_RD_STORED_LINK_KEY:
      BT_MSG_API( "BT HC CMD RX: HC Rd Stored Link Key", 0, 0, 0 );
      bt_hc_cmd_rd_stored_link_key( &bt_cmd_ptr->cmd_msg.cmd_hc_rd_link_key );
      break;

    case BT_CMD_HC_WR_STORED_LINK_KEY:
      BT_MSG_API( "BT HC CMD RX: HC Wr Stored Link Key", 0, 0, 0 );
      bt_hc_cmd_wr_stored_link_key( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_link_key );
      break;

    case BT_CMD_HC_DEL_STORED_LINK_KEY:
      BT_MSG_API( "BT HC CMD RX: HC Del Stored Link Key", 0, 0, 0 );
      bt_hc_cmd_del_stored_link_key( &bt_cmd_ptr->cmd_msg.cmd_hc_del_link_key );
      break;

    case BT_CMD_HC_CHANGE_LOCAL_NAME:
      BT_MSG_API( "BT HC CMD RX: HC Change Local Name", 0, 0, 0 );
      bt_hc_cmd_change_local_name( &bt_cmd_ptr->cmd_msg.cmd_hc_change_local_name );
      break;

    case BT_CMD_HC_RD_LOCAL_NAME:
      BT_MSG_API( "BT HC CMD RX: HC Rd Local Name", 0, 0, 0 );
      bt_hc_cmd_rd_local_name();
      break;

    case BT_CMD_HC_RD_CONN_ACCEPT_TO:
      BT_MSG_API( "BT HC CMD RX: HC Rd Conn Accept TO", 0, 0, 0 );
      bt_hc_cmd_rd_conn_accept_to();
      break;

    case BT_CMD_HC_WR_CONN_ACCEPT_TO:
      BT_MSG_API( "BT HC CMD RX: HC Wr Conn Accept TO", 0, 0, 0 );
      bt_hc_wr_conn_accept_to( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_conn_accept_to );
      break;

    case BT_CMD_HC_RD_PAGE_TO:
      BT_MSG_API( "BT HC CMD RX: HC Rd Page TO", 0, 0, 0 );
      bt_hc_cmd_rd_page_to();
      break;

    case BT_CMD_HC_WR_PAGE_TO:
      BT_MSG_API( "BT HC CMD RX: HC Wr Page TO", 0, 0, 0 );
      bt_hc_cmd_wr_page_to( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_page_to );
      break;

    case BT_CMD_HC_RD_SCAN_ENABLE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Scan Enable", 0, 0, 0 );
      bt_hc_cmd_rd_scan_enable();
      break;

    case BT_CMD_HC_WR_SCAN_ENABLE:
      BT_MSG_API( "BT HC CMD RX: HC Wr Scan Enable", 0, 0, 0 );
      bt_hc_cmd_wr_scan_enable( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_scan_enable );
      break;

    case BT_CMD_HC_RD_PAGE_SCAN_ACTIVITY:
      BT_MSG_API( "BT HC CMD RX: HC Rd Page Scan Activity", 0, 0, 0 );
      bt_hc_cmd_rd_page_scan_activity();
      break;

    case BT_CMD_HC_WR_PAGE_SCAN_ACTIVITY:
      BT_MSG_API( "BT HC CMD RX: HC Wr Page Scan Activity", 0, 0, 0 );
      bt_hc_cmd_wr_page_scan_activity( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_page_scan_act );
      break;

    case BT_CMD_HC_RD_INQ_SCAN_ACTIVITY:
      BT_MSG_API( "BT HC CMD RX: HC Rd Inq Scan Activity", 0, 0, 0 );
      bt_hc_cmd_rd_inq_scan_activity();
      break;

    case BT_CMD_HC_WR_INQ_SCAN_ACTIVITY:
      BT_MSG_API( "BT HC CMD RX: HC Wr Inq Scan Activity", 0, 0, 0 );
      bt_hc_cmd_wr_inq_scan_activity( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_inq_scan_act );
      break;

    case BT_CMD_HC_RD_AUTH_ENABLE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Auth Enable", 0, 0, 0 );
      bt_hc_cmd_rd_auth_enable();
      break;

    case BT_CMD_HC_WR_AUTH_ENABLE:
      BT_MSG_API( "BT HC CMD RX: HC Wr Auth Enable", 0, 0, 0 );
      bt_hc_cmd_wr_auth_enable( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_auth_enable );
      break;

    case BT_CMD_HC_RD_ENCRYPT_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Encrypt Mode", 0, 0, 0 );
      bt_hc_cmd_rd_encrypt_mode();
      break;

    case BT_CMD_HC_WR_ENCRYPT_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Wr Encrypt Mode", 0, 0, 0 );
      bt_hc_cmd_wr_encrypt_mode( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_encrypt_mode );
      break;

    case BT_CMD_HC_RD_CLASS_OF_DEVICE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Class of Device", 0, 0, 0 );
      bt_hc_cmd_rd_class_of_device();
      break;

    case BT_CMD_HC_WR_CLASS_OF_DEVICE:
      BT_MSG_API( "BT HC CMD RX: HC Wr Class of Device", 0, 0, 0 );
      bt_hc_cmd_wr_class_of_device( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_class_of_dev );
      break;

    case BT_CMD_HC_RD_VOICE_SETTING:
      BT_MSG_API( "BT HC CMD RX: HC Rd Voice Setting", 0, 0, 0 );
      bt_hc_cmd_rd_voice_setting();
      break;

    case BT_CMD_HC_WR_VOICE_SETTING:
      BT_MSG_API( "BT HC CMD RX: HC Wr Voice Setting", 0, 0, 0 );
      bt_hc_cmd_wr_voice_setting( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_voice_setting );
      break;

    case BT_CMD_HC_RD_AUTO_FLUSH_TO:
      BT_MSG_API( "BT HC CMD RX: HC Rd Set Auto Flush TO", 0, 0, 0 );
      bt_hc_cmd_rd_auto_flush_to( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_WR_AUTO_FLUSH_TO:
      BT_MSG_API( "BT HC CMD RX: HC Wr Set Auto Flush TO", 0, 0, 0 );
      bt_hc_cmd_wr_auto_flush_to( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_auto_flush_to );
      break;

    case BT_CMD_HC_RD_BROAD_RETRANS:
      BT_MSG_API( "BT HC CMD RX: HC Rd Broad Retrans", 0, 0, 0 );
      bt_hc_cmd_rd_broadcast_retrans();
      break;

    case BT_CMD_HC_WR_BROAD_RETRANS:
      BT_MSG_API( "BT HC CMD RX: HC Wr Broad Retrans", 0, 0, 0 );
      bt_hc_cmd_wr_broadcast_retrans( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_broad_retrans );
      break;

    case BT_CMD_HC_RD_HOLD_MODE_ACTIVITY:
      BT_MSG_API( "BT HC CMD RX: HC Rd Hold Mode Activity", 0, 0, 0 );
      bt_hc_cmd_rd_hold_mode_activity();
      break;

    case BT_CMD_HC_WR_HOLD_MODE_ACTIVITY:
      BT_MSG_API( "BT HC CMD RX: HC Wr Hold Mode Activity", 0, 0, 0 );
      bt_hc_cmd_wr_hold_mode_activity( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_hold_mode_act );
      break;

    case BT_CMD_HC_RD_TX_POWER_LEVEL:
      BT_MSG_API( "BT HC CMD RX: HC Rd Tx Power Level", 0, 0, 0 );
      bt_hc_cmd_rd_tx_power_level( &bt_cmd_ptr->cmd_msg.cmd_hc_rd_tx_power );
      break;

    case BT_CMD_HC_RD_SCO_FLOW_CTRL:
      BT_MSG_API( "BT HC CMD RX: HC Rd SCO Flow Ctrl", 0, 0, 0 );
      bt_hc_cmd_rd_sco_flow_ctrl();
      break;

    case BT_CMD_HC_WR_SCO_FLOW_CTRL:
      BT_MSG_API( "BT HC CMD RX: HC Wr SCO Flow Ctrl", 0, 0, 0 );
      bt_hc_cmd_wr_sco_flow_ctrl( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_sco_flow_ctrl );
      break;

    case BT_CMD_HC_SET_HC_TO_HOST_FC:
      BT_MSG_API( "BT HC CMD RX: HC Set HC to Host FC", 0, 0, 0 );
      bt_hc_cmd_set_hc_to_host_fc( &bt_cmd_ptr->cmd_msg.cmd_hc_set_hc_to_host_fc );
      break;

    case BT_CMD_HC_HOST_BUFFER_SIZE:
      BT_MSG_API( "BT HC CMD RX: HC Host Buffer Size", 0, 0, 0 );
      bt_hc_cmd_host_buffer_size( &bt_cmd_ptr->cmd_msg.cmd_hc_host_buf_size );
      break;

    case BT_CMD_HC_HOST_NUM_CMPLT_PACKETS:
      BT_MSG_API( "BT HC CMD RX: HC Host Num Cmplt Pkts", 0, 0, 0 );
      bt_hc_cmd_host_num_completed_pkts( &bt_cmd_ptr->cmd_msg.cmd_hc_host_num_cmplt_pkt );
      break;

    case BT_CMD_HC_RD_LINK_SUPERVISION_TO:
      BT_MSG_API( "BT HC CMD RX: HC Rd Link Supervision TO", 0, 0, 0 );
      bt_hc_cmd_rd_link_supervision_to( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_WR_LINK_SUPERVISION_TO:
      BT_MSG_API( "BT HC CMD RX: HC Wr Link Supervision TO", 0, 0, 0 );
      bt_hc_cmd_wr_link_supervision_to( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_link_super_to );
      break;

    case BT_CMD_HC_RD_NUM_SUPPORTED_IAC:
      BT_MSG_API( "BT HC CMD RX: HC Rd Num Supported IAC", 0, 0, 0 );
      bt_hc_cmd_rd_num_supported_iac();
      break;

    case BT_CMD_HC_RD_CURRENT_IAC_LAP:
      BT_MSG_API( "BT HC CMD RX: HC Rd Current IAC LAP", 0, 0, 0 );
      bt_hc_cmd_rd_current_iac_lap();
      break;

    case BT_CMD_HC_WR_CURRENT_IAC_LAP:
      BT_MSG_API( "BT HC CMD RX: HC Wr Current IAC LAP", 0, 0, 0 );
      bt_hc_cmd_wr_current_iac_lap( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_curr_iac_lap );
      break;

    case BT_CMD_HC_RD_PAGE_SCAN_PER_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Page Scan Per Mode", 0, 0, 0 );
      bt_hc_cmd_rd_ps_per_mode();
      break;

    case BT_CMD_HC_WR_PAGE_SCAN_PER_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Wr Page Scan Per Mode", 0, 0, 0 );
      bt_hc_cmd_wr_ps_per_mode( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_ps_per_mode );
      break;

#ifndef FEATURE_BT_1_2 /* BT 1.1 */
    case BT_CMD_HC_RD_PAGE_SCAN_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Page Scan Mode", 0, 0, 0 );
      bt_hc_cmd_rd_ps_mode();
      break;

    case BT_CMD_HC_WR_PAGE_SCAN_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Wr Page Scan Mode", 0, 0, 0 );
      bt_hc_cmd_wr_ps_mode( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_ps_mode );
      break;
#endif /* not FEATURE_BT_1_2 */

    // ---------------------------------------------------------------------
    // Information Paramters
    // ---------------------------------------------------------------------
    case BT_CMD_HC_RD_LOCAL_VERSION_INFO:
      BT_MSG_API( "BT HC CMD RX: HC Rd Local Ver Info", 0, 0, 0 );
      bt_hc_cmd_rd_local_ver_info();
      break;

    case BT_CMD_HC_RD_LOCAL_FEATURES:
      BT_MSG_API( "BT HC CMD RX: HC Rd Local Features", 0, 0, 0 );
      bt_hc_cmd_rd_local_features();
      break;

    case BT_CMD_HC_RD_BUFFER_SIZE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Buffer Size", 0, 0, 0 );
      bt_hc_cmd_rd_buffer_size();
      break;

    case BT_CMD_HC_RD_COUNTRY_CODE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Country Code", 0, 0, 0 );
      bt_hc_cmd_rd_country_code();
      break;

    case BT_CMD_HC_RD_BD_ADDR:
      BT_MSG_API( "BT HC CMD RX: HC Rd BD ADDR", 0, 0, 0 );
      bt_hc_cmd_rd_bd_addr();
      break;

    // ---------------------------------------------------------------------
    // Status Paramters
    // ---------------------------------------------------------------------
    case BT_CMD_HC_RD_FAILED_CONTACT_COUNTER:
      BT_MSG_API( "BT HC CMD RX: HC Rd Failed Contact Counter", 0, 0, 0 );
      bt_hc_cmd_rd_failed_contact_counter( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_RESET_FAILED_CONTACT_COUNTER:
      BT_MSG_API( "BT HC CMD RX: HC Reset Failed Contact Counter", 0, 0, 0 );
      bt_hc_cmd_reset_failed_contact_counter( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_GET_LINK_QUALITY:
      BT_MSG_API( "BT HC CMD RX: HC Get Link Quality", 0, 0, 0 );
      bt_hc_cmd_get_link_quality( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_RD_RSSI:
      BT_MSG_API( "BT HC CMD RX: HC Rd RSSI", 0, 0, 0 );
      bt_hc_cmd_read_rssi( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    // ---------------------------------------------------------------------
    // Test Commands
    // ---------------------------------------------------------------------
    case BT_CMD_HC_RD_LOOPBACK_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Loopback Mode", 0, 0, 0 );
      bt_hc_cmd_rd_loopback_mode();
      break;

    case BT_CMD_HC_WR_LOOPBACK_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Wr Lookback Mode", 0, 0, 0 );
      bt_hc_cmd_wr_loopback_mode( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_loopback_mode );
      break;

    case BT_CMD_HC_ENABLE_DUT_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Enable DUT Mode", 0, 0, 0 );
      bt_hc_cmd_enable_dut_mode();
      break;

    // ---------------------------------------------------------------------
    // Qualcomm vendor specific commands
    // ---------------------------------------------------------------------
    case BT_CMD_HC_FIX_INIT_WHITE:
      BT_MSG_API( "BT HC CMD RX: HC Fix Init White", 0, 0, 0 );
      bt_hc_cmd_fix_init_white( &bt_cmd_ptr->cmd_msg.cmd_hc_fix_init_white );
      break;

    case BT_CMD_HC_FIX_INIT_ENC:
      BT_MSG_API( "BT HC CMD RX: HC Fix Init Enc", 0, 0, 0 );
      bt_hc_cmd_fix_init_enc( &bt_cmd_ptr->cmd_msg.cmd_hc_fix_init_enc );
      break;

    case BT_CMD_HC_RD_RX_STATUS_EN:
      BT_MSG_API( "BT HC CMD RX: HC Rd RX Status En", 0, 0, 0 );
      bt_hc_cmd_rd_rx_status_en( &bt_cmd_ptr->cmd_msg.cmd_hc_rd_rx_status_en );
      break;

    case BT_CMD_HC_CONFIG_SCHR:
      BT_MSG_API( "BT HC CMD RX: HC Config Searcher", 0, 0, 0 );
      bt_hc_cmd_config_srch( &bt_cmd_ptr->cmd_msg.cmd_hc_config_srch );
      break;

    case BT_CMD_HC_RD_TX_BUFFER_EN:
      BT_MSG_API( "BT HC CMD RX: HC Rd Tx Buffer En", 0, 0, 0 );
      bt_hc_cmd_rd_tx_buffer_en( &bt_cmd_ptr->cmd_msg.cmd_hc_rd_tx_buf_en );
      break;

    case BT_CMD_HC_RD_RX_BUFFER_EN:
      BT_MSG_API( "BT HC CMD RX: HC Rd Rx Buffer En", 0, 0, 0 );
      bt_hc_cmd_rd_rx_buffer_en( &bt_cmd_ptr->cmd_msg.cmd_hc_rd_rx_buf_en );
      break;

    case BT_CMD_HC_SEND_RAW_MODE_PKT:
      BT_MSG_API( "BT HC CMD RX: HC Send Raw Mode Pkt", 0, 0, 0 );
      bt_hc_cmd_send_raw_mode_pkt( &bt_cmd_ptr->cmd_msg.cmd_hc_send_raw_mode_pkt );
      break;

#ifdef FEATURE_BT_TEST_MODE_TESTER
    case BT_CMD_HC_TESTER_ACTIVATE:
      BT_MSG_API( "BT HC CMD RX: HC Tester Activate", 0, 0, 0 );
      bt_hc_cmd_tester_activate( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;

    case BT_CMD_HC_TESTER_CONTROL:
      BT_MSG_API( "BT HC CMD RX: HC Tester Control", 0, 0, 0 );
      bt_hc_cmd_tester_control( &bt_cmd_ptr->cmd_msg.cmd_hc_tester_ctrl );
      break;

    case BT_CMD_HC_TESTER_START:
      BT_MSG_API( "BT HC CMD RX: HC Tester Start", 0, 0, 0 );
      bt_hc_cmd_tester_start( &bt_cmd_ptr->cmd_msg.cmd_hc_conn_hndl );
      break;
#endif /* FEATURE_BT_TEST_MODE_TESTER */

    case BT_CMD_HC_SET_NEW_BD_ADDR:
      BT_MSG_API( "BT HC CMD RX: HC Set New BD ADDR", 0, 0, 0 );
      bt_hc_cmd_set_new_bd_addr( &bt_cmd_ptr->cmd_msg.cmd_hc_bd_addr );
      break;

    case BT_CMD_HC_TIME_ACCURACY_REQ:
      BT_MSG_API( "BT HC CMD RX: HC Time Acc Req", 0, 0, 0 );
      bt_hc_cmd_time_acc_req();
      break;

    case BT_CMD_HC_MODIFY_BEACON:
      BT_MSG_API( "BT HC CMD RX: HC Modify Beacon", 0, 0, 0 );
      bt_hc_cmd_modify_beacon( &bt_cmd_ptr->cmd_msg.cmd_hc_modify_beacon );
      break;

    case BT_CMD_HC_RD_CLK:
      BT_MSG_API( "BT HC CMD RX: HC Rd Clk", 0, 0, 0 );
      bt_hc_cmd_rd_clk();
      break;

    case BT_CMD_HC_RD_SBI_REG:
      BT_MSG_API( "BT HC CMD RX: HC Rd SBI Reg", 0, 0, 0 );
      bt_hc_cmd_rd_sbi_reg( &bt_cmd_ptr->cmd_msg.cmd_hc_rd_sbi_reg );
      break;

    case BT_CMD_HC_WR_SBI_REG:
      BT_MSG_API( "BT HC CMD RX: HC Wr SBI Reg", 0, 0, 0 );
      bt_hc_cmd_wr_sbi_reg( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_sbi_reg );
      break;

    case BT_CMD_HC_DIAC_LAP_EN:
      BT_MSG_API( "BT HC CMD RX: HC DIAC LAP En", 0, 0, 0 );
      bt_hc_cmd_diac_lap_en( &bt_cmd_ptr->cmd_msg.cmd_hc_diag_lap_en );
      break;

    case BT_CMD_HC_RD_ENC_KEY_LENGTH:
      BT_MSG_API( "BT HC CMD RX: HC Rd Enc Key Length", 0, 0, 0 );
      bt_hc_cmd_rd_enc_key_len();
      break;

    case BT_CMD_HC_WR_ENC_KEY_LENGTH:
    /* need to replace this by BCCMD:- MAX_CRYPT_KEY_LENGTH for SOC*/
#ifndef FEATURE_BT_SOC
      BT_MSG_API( "BT HC CMD RX: HC Wr Enc Key Length", 0, 0, 0 );
      bt_hc_cmd_wr_enc_key_len( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_enc_key_length );
#endif /* !FEATURE_BT_SOC */

      break;

    case BT_CMD_HC_CHANGE_REMOTE_PKT_TYPE:
      BT_MSG_API( "BT HC CMD RX: HC Change Remote Pkt Type", 0, 0, 0 );
      bt_hc_cmd_change_remote_pkt_type( &bt_cmd_ptr->cmd_msg.cmd_hc_change_remote_pkt_type );
      break;

    case BT_CMD_HC_STATUS_VIEW_EN:
      BT_MSG_API( "BT HC CMD RX: HC Status View En", 0, 0, 0 );
      bt_hc_cmd_status_view_en( &bt_cmd_ptr->cmd_msg.cmd_hc_status_view_en );
      break;

    case BT_CMD_HC_ENABLE_LOOPBACK:
      BT_MSG_API( "BT HC CMD RX: HC Enable Loopback", 0, 0, 0 );
      bt_hc_cmd_enable_loopback( &bt_cmd_ptr->cmd_msg.cmd_hc_enable_loopback );
      break;

    case BT_CMD_HC_SET_INCOMING_HOLD_PARAMS:
      BT_MSG_API( "BT HC CMD RX: HC Set Incoming Hold Params", 0, 0, 0 );
      bt_hc_cmd_set_incoming_hold_params( &bt_cmd_ptr->cmd_msg.cmd_hc_set_inc_hold_params );
      break;

    case BT_CMD_HC_SET_INCOMING_SNIFF_PARAMS:
      BT_MSG_API( "BT HC CMD RX: HC Set Incoming Sniff Params", 0, 0, 0 );
      bt_hc_cmd_set_incoming_sniff_params( &bt_cmd_ptr->cmd_msg.cmd_hc_set_inc_sniff_params );
      break;

    case BT_CMD_HC_SET_INCOMING_PARK_PARAMS:
      BT_MSG_API( "BT HC CMD RX: HC Set Incoming Park Params", 0, 0, 0 );
      bt_hc_cmd_set_incoming_park_params( &bt_cmd_ptr->cmd_msg.cmd_hc_set_inc_park_params );
      break;

    case BT_CMD_HC_SET_TX_POWER_LEVEL:
      BT_MSG_API( "BT HC CMD RX: HC Set TX Power Level", 0, 0, 0 );
      bt_hc_cmd_set_tx_power_level( &bt_cmd_ptr->cmd_msg.cmd_hc_set_tx_power_level );
      break;

    case BT_CMD_HC_CERT_CTRL:
      BT_MSG_API( "BT HC CMD RX: HC Certification Control", 0, 0, 0 );
      bt_hc_cmd_cert_ctrl( &bt_cmd_ptr->cmd_msg.cmd_hc_cert_ctrl );
      break;

    case BT_CMD_HC_TX_CONTINUOUS:
      BT_MSG_API( "BT HC CMD RX: HC TX Continuous", 0, 0, 0 );
      bt_hc_cmd_tx_continuous( &bt_cmd_ptr->cmd_msg.cmd_hc_tx_continuous );
      break;

    case BT_CMD_HC_TX_CONTINUOUS_FREQ:
      BT_MSG_API( "BT HC CMD RX: HC TX Continuous Freq", 0, 0, 0 );
      bt_hc_cmd_tx_continuous_freq( &bt_cmd_ptr->cmd_msg.cmd_hc_tx_continuous_freq );
      break;

    case BT_CMD_HC_RD_RAW_RSSI:
      BT_MSG_API( "BT HC CMD RX: HC Rd Raw RSSI", 0, 0, 0 );
      bt_hc_cmd_rd_raw_rssi( &bt_cmd_ptr->cmd_msg.cmd_hc_rd_raw_rssi );
      break;

    case BT_CMD_HC_TEST_SET_LM_PARAMS:
      BT_MSG_API( "BT HC CMD RX: HC Test Set LM Params", 0, 0, 0 );
      bt_hc_cmd_test_set_lm_params( &bt_cmd_ptr->cmd_msg.cmd_hc_test_set_lm_params );
      break;

    case BT_CMD_HC_TEST_SEND_LMP_MSG:
      BT_MSG_API( "BT HC CMD RX: HC Send Test LMP Message", 0, 0, 0 );
      bt_hc_cmd_test_send_lmp_msg( &bt_cmd_ptr->cmd_msg.cmd_hc_test_send_lmp_msg );
      break;

    case BT_CMD_HC_WR_RAM:
      BT_MSG_API( "BT HC CMD RX: HC Write RAM", 0, 0, 0 );
      bt_hc_cmd_wr_ram( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_ram );
      break;

    case BT_CMD_HC_RD_RAM:
      BT_MSG_API( "BT HC CMD RX: HC Read RAM", 0, 0, 0 );
      bt_hc_cmd_rd_ram( &bt_cmd_ptr->cmd_msg.cmd_hc_rd_ram );
      break;


#ifdef FEATURE_BT_1_2  
                 
    case BT_CMD_HC_SET_AFH_HOST_CHAN_CLASS:
      BT_MSG_API( "BT HC CMD RX: HC Set AFH Host Chan Class", 0, 0, 0 );
      bt_hc_cmd_set_afh_host_chan_class(
        &bt_cmd_ptr->cmd_msg.cmd_hc_set_afh_host_chan_class
      );
      break;

    case BT_CMD_HC_READ_AFH_CHAN_ASSESS_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Read AFH Chan Asses Mode", 0, 0, 0 );
      bt_hc_cmd_read_afh_chan_assess_mode();
      break;

    case BT_CMD_HC_SET_AFH_CHAN_ASSESS_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Set AFH Chan Assess Mode", 0, 0, 0 );
      bt_hc_cmd_set_afh_chan_assess_mode(
        &bt_cmd_ptr->cmd_msg.cmd_hc_set_afh_chan_assess_mode
      );
      break;

    case BT_CMD_HC_READ_AFH_CHAN_MAP:
      BT_MSG_API( "BT HC CMD RX: HC Read AFH Chan Map", 0, 0, 0 );
      bt_hc_cmd_read_afh_chan_map(
        &bt_cmd_ptr->cmd_msg.cmd_hc_read_afh_chan_map
      );
      break;

    case BT_CMD_HC_SETUP_SYNC_CONNECTION:
      BT_MSG_API( "BT HC CMD RX: HC Setup Sync Conn", 0, 0, 0 );
      bt_hc_cmd_setup_sync_conn(
        &bt_cmd_ptr->cmd_msg.cmd_hc_setup_sync_connection
      );
      break;

    case BT_CMD_HC_ACCEPT_SYNC_CONN_REQ:
      BT_MSG_API( "BT HC CMD RX: HC Accept Sync Conn", 0, 0, 0 );
      bt_hc_cmd_accept_sync_conn_req(
        &bt_cmd_ptr->cmd_msg.cmd_hc_accept_sync_conn_req
      );
      break;

    case BT_CMD_HC_REJECT_SYNC_CONN_REQ:
      BT_MSG_API( "BT HC CMD RX: HC Reject Sync Conn", 0, 0, 0 );
      bt_hc_cmd_reject_sync_conn_req(
        &bt_cmd_ptr->cmd_msg.cmd_hc_reject_sync_conn_req
      );
      break;

    case BT_CMD_HC_CONTROL_AFH:
      BT_MSG_API( "BT HC CMD RX: HC Control AFH", 0, 0, 0 );
      bt_hc_cmd_control_afh(
        &bt_cmd_ptr->cmd_msg.cmd_hc_control_afh
      );
      break;

    case BT_CMD_HC_SET_AFH_CHAN_GOOD:
      BT_MSG_API( "BT HC CMD RX: HC Set AFH Chan Good", 0, 0, 0 );
      bt_hc_cmd_set_afh_chan_good(
        &bt_cmd_ptr->cmd_msg.cmd_hc_set_afh_chan_good
      );
      break;

#ifndef FEATURE_BT_1_2_BASIC
    case BT_CMD_HC_SET_ESCO_PKT_LEN:
      BT_MSG_API( "BT HC CMD RX: HC Set eSCO Pkt Len", 0, 0, 0 );
      bt_hc_cmd_set_esco_pkt_len(
        &bt_cmd_ptr->cmd_msg.cmd_hc_set_esco_pkt_len
      );
      break;
#endif /* !FEATURE_BT_1_2_BASIC */

    case BT_CMD_HC_RD_INQUIRY_SCAN_TYPE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Inq Scan Type", 0, 0, 0 );
      bt_hc_cmd_rd_is_type();
      break;

    case BT_CMD_HC_WR_INQUIRY_SCAN_TYPE:
      BT_MSG_API( "BT HC CMD RX: HC Wr Inq Scan Type", 0, 0, 0 );
      bt_hc_cmd_wr_is_type( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_is_type );
      break;

    case BT_CMD_HC_RD_INQUIRY_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Inq Mode", 0, 0, 0 );
      bt_hc_cmd_rd_inq_mode();
      break;

    case BT_CMD_HC_WR_INQUIRY_MODE:
      BT_MSG_API( "BT HC CMD RX: HC Wr Inq Mode", 0, 0, 0 );
      bt_hc_cmd_wr_inq_mode( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_inq_mode );
      break;

    case BT_CMD_HC_RD_PAGE_SCAN_TYPE:
      BT_MSG_API( "BT HC CMD RX: HC Rd Page Scan Type", 0, 0, 0 );
      bt_hc_cmd_rd_ps_type();
      break;

    case BT_CMD_HC_WR_PAGE_SCAN_TYPE:
      BT_MSG_API( "BT HC CMD RX: HC Wr Page Scan Type", 0, 0, 0 );
      bt_hc_cmd_wr_ps_type( &bt_cmd_ptr->cmd_msg.cmd_hc_wr_ps_type );
      break;

#endif /* FEATURE_BT_1_2 */

    default:
      BT_ERR( "BT HC ERR: Invalid BT HCI cmd: %d",
              bt_cmd_ptr->cmd_hdr.cmd_type, 0, 0 );
      break;
  }

  bt_cmd_ptr->cmd_hdr.cmd_status = cmd_status;
}

/*===========================================================================

FUNCTION
  bt_hc_ev_event_enabled

DESCRIPTION
  Returns true if the BT hci event is enabled, i.e. if the event is not
  filtered out and should be sent.

===========================================================================*/
boolean               /* TRUE if enabled; FALSE otherwise */
bt_hc_ev_event_enabled
(
  bt_event_type ev    /* the BT hci event to check        */
)
{
  boolean enabled = TRUE; // assume event is enabled

  switch ( ev )
  {
    case BT_EV_HC_RX_ACL_DATA:
      enabled = TRUE;
      break;

    case BT_EV_HC_RX_SCO_DATA:
      enabled = TRUE;
      break;

    case BT_EV_HC_TX_DATA_CFM:
      enabled = TRUE;
      break;

    case BT_EV_HC_INQUIRY_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 0 ] & BT_HCI_INQUIRY_CMPLT_EV_BIT);
      break;

    case BT_EV_HC_INQUIRY_RESULT:
      enabled = (bt_lm_data.event_mask[ 0 ] & BT_HCI_INQUIRY_RESULT_EV_BIT);
      break;

    case BT_EV_HC_CONNECTION_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 0 ] & BT_HCI_CONNECTION_CMPLT_EV_BIT);
      break;

    case BT_EV_HC_CONNECTION_REQUEST:
      enabled = (bt_lm_data.event_mask[ 0 ] & BT_HCI_CONNECTION_RQST_EV_BIT);
      break;

    case BT_EV_HC_DISCONNECTION_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 0 ] & BT_HCI_DISCONNECTION_CMPLT_EV_BIT);
      break;

    case BT_EV_HC_AUTH_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 0 ] & BT_HCI_AUTH_COMPLT_EV_BIT);
      break;

    case BT_EV_HC_REMOTE_NAME_REQ_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 0 ] & BT_HCI_REMOTE_NAME_RQST_CMPLT_EV_BIT);
      break;

    case BT_EV_HC_ENCRYPTION_CHANGE:
      enabled = (bt_lm_data.event_mask[ 0 ] & BT_HCI_ENCRYPTION_CHG_EV_BIT);
      break;

    case BT_EV_HC_CHANGE_CONN_KEY_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 1 ] &
                 (BT_HCI_CHG_CONN_LINK_KEY_CMPLT_EV_BIT >> 8));
      break;

    case BT_EV_HC_MASTER_LINK_KEY_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 1 ] &
                 (BT_HCI_MASTER_LINK_KEY_CMPLT_EV_BIT >> 8));
      break;

    case BT_EV_HC_RD_REMOTE_FEATURES_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 1 ] &
                 (BT_HCI_RD_REMOTE_SUPPORTED_FEAT_CMPLT_EV_BIT >> 8));
      break;

    case BT_EV_HC_RD_REMOTE_VERSION_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 1 ] &
                 (BT_HCI_RD_REMOTE_VERSION_INFO_CMPLT_EV_BIT >> 8));
      break;

    case BT_EV_HC_QOS_SETUP_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 1 ] &
                 (BT_HCI_QOS_SETUP_CMPLT_EV_BIT >> 8));
      break;

/*Command complete event should always be sent and can not be masked */
    case BT_EV_HC_COMMAND_COMPLETE:
      enabled = TRUE;
      break;

/*Command status event should always be sent and can not be masked */
    case BT_EV_HC_COMMAND_STATUS:
      enabled = TRUE;
      break;

    case BT_EV_HC_HARDWARE_ERROR:
      enabled = (bt_lm_data.event_mask[ 1 ] & (BT_HCI_HW_ERR_EV_BIT >> 8));
      break;

    case BT_EV_HC_FLUSH_OCCURRED:
      enabled = (bt_lm_data.event_mask[ 2 ] & (BT_HCI_FLUSH_OCCURRED_EV_BIT >> 16));
      break;

    case BT_EV_HC_ROLE_CHANGE:
      enabled = (bt_lm_data.event_mask[ 2 ] & (BT_HCI_ROLE_CHG_EV_BIT >> 16));
      break;

/*Num complete event should always be sent and can not be masked */
    case BT_EV_HC_NUM_CMPLT_PKTS:
      enabled = TRUE;
      break;

    case BT_EV_HC_MODE_CHANGE:
      enabled = (bt_lm_data.event_mask[ 2 ] & (BT_HCI_MODE_CHG_EV_BIT >> 16));
      break;

    case BT_EV_HC_RETURN_LINK_KEYS:
      enabled = (bt_lm_data.event_mask[ 2 ] & (BT_HCI_RETURN_LINK_KEYS_EV_BIT >> 16));
      break;

    case BT_EV_HC_PIN_CODE_REQUEST:
      enabled = (bt_lm_data.event_mask[ 2 ] & (BT_HCI_PIN_CODE_RQST_EV_BIT >> 16));
      break;

    case BT_EV_HC_LINK_KEY_REQUEST:
      enabled = (bt_lm_data.event_mask[ 2 ] &
                 (BT_HCI_LINK_KEY_RQST_EV_BIT >> 16));
      break;

    case BT_EV_HC_LINK_KEY_NOTIFICATION:
      enabled = (bt_lm_data.event_mask[ 2 ] &
                 (BT_HCI_LINK_KEY_NOTIFICATION_EV_BIT >> 16));
      break;

    case BT_EV_HC_LOOPBACK_COMMAND:
      enabled = (bt_lm_data.event_mask[ 3 ] &
                 (BT_HCI_LOOPBACK_CMD_EV_BIT >> 24));
      break;

    case BT_EV_HC_DATA_BUFFER_OVERFLOW:
      enabled = (bt_lm_data.event_mask[ 3 ] &
                 (BT_HCI_DATA_BUF_OVERFLOW_EV_BIT >> 24));
      break;

    case BT_EV_HC_MAX_SLOTS_CHANGE:
      enabled = (bt_lm_data.event_mask[ 3 ] &
                 (BT_HCI_MAX_SLOTS_CHG_EV_BIT >> 24));
      break;

    case BT_EV_HC_RD_CLK_OFFSET_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 3 ] &
                 (BT_HCI_RD_CLOCK_OFFSET_CMPLT_EV_BIT >> 24));
      break;

    case BT_EV_HC_CONN_PKT_TYPE_CHANGED:
      enabled = (bt_lm_data.event_mask[ 3 ] &
                 (BT_HCI_CONN_PKT_TYPE_CHG_EV_BIT >> 24));
      break;

    case BT_EV_HC_QOS_VIOLATION:
      enabled = (bt_lm_data.event_mask[ 3 ] &
                 (BT_HCI_QOS_VIOLATION_EV_BIT >> 24));
      break;

    case BT_EV_HC_PAGE_SCAN_MODE_CHANGE:
      enabled = (bt_lm_data.event_mask[ 3 ] &
                 (BT_HCI_PG_SCAN_MODE_CHG_EV_BIT >> 24));
      break;

    case BT_EV_HC_PAGE_SCAN_REP_MODE_CHANGE:
      enabled = (bt_lm_data.event_mask[ 3 ] &
                 (BT_HCI_PG_SCAN_REP_MODE_CHG_EV_BIT >> 24));
      break;

    case BT_EV_HC_QUALCOMM:
      enabled = TRUE;
      break;

#ifdef FEATURE_BT_1_2
    case BT_EV_HC_INQUIRY_RESULT_WITH_RSSI:
        enabled = (bt_lm_data.event_mask[ 4 ] &
                   (BT_HCI_INQUIRY_RESULT_WITH_RSSI_EV_BIT >> 32));
      break;

    case BT_EV_HC_RD_REMOTE_EXT_FEATURES_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 4 ] &
                 (BT_HCI_RD_REMOTE_EXTENDED_FEAT_CMPLT_EV_BIT >> 32));
      break;

    case BT_EV_HC_SYNC_CONN_COMPLETE:
      enabled = (bt_lm_data.event_mask[ 5 ] &
                 (BT_HCI_SYNC_CONN_CMPLT_EV_BIT >> 40));
      break;

    case BT_EV_HC_SYNC_CONN_CHANGE:
      enabled = (bt_lm_data.event_mask[ 5 ] &
                 (BT_HCI_SYNC_CONN_CHANGE_EV_BIT >> 40));
      break;
#endif

    default:
      enabled = FALSE;
      break;
  }
  return( enabled );
}
#endif /* FEATURE_BT_SOC */


/*===========================================================================

FUNCTION
  bt_hc_send_event

DESCRIPTION
  Sends the BT HCI event if this event is not filtered out by calling
  the registered event callback function.

===========================================================================*/
void bt_hc_send_event
(
  bt_ev_msg_type*  ev_msg_ptr    /* ptr to BT HCI event to send            */
)
{
#ifndef FEATURE_BT_SOC
  boolean ev_filt_enabled;
#endif /* !FEATURE_BT_SOC */

  if ( BT_CE_GROUP( ev_msg_ptr->ev_hdr.ev_type, HC ) )
  {

#ifndef FEATURE_BT_SOC
    ev_filt_enabled = bt_hc_ev_event_enabled( ev_msg_ptr->ev_hdr.ev_type);

    if ( ev_filt_enabled )
    {
#endif /* FEATURE_BT_SOC */
      /*  BT HCI event not filtered out... send it.  */

      // WAL - BT_TBD: free dsms for those events that are masked out.

      ev_msg_ptr->ev_hdr.bt_app_id = bt_dc_last_hci_ev_reg_app_id;

#ifdef BT_TEST_PYLD_FAST_RX
      if ( ( ev_msg_ptr->ev_hdr.ev_type == BT_EV_HC_RX_ACL_DATA ) &&
           ( bt_dc.cur_hci_mode != BT_HCIM_STANDARD_HCI_ON ) &&
           ( bt_dc.cur_hci_mode != BT_HCIM_STANDARD_HCI_VOC_ON )
         )
      {
        bt_rm_ev_hc_rx_acl_data( ev_msg_ptr );
      }
      else
      {
        if (ev_msg_ptr->ev_hdr.ev_type == BT_EV_HC_COMMAND_COMPLETE) 
        {
            BT_MSG_API( "BT HC EV TX: HC Cmd Cmplt", 0, 0, 0 );
        }
        bt_ec_send_event( ev_msg_ptr );
      }
#else
      if (ev_msg_ptr->ev_hdr.ev_type == BT_EV_HC_COMMAND_COMPLETE) 
      {
          BT_MSG_API( "BT HC EV TX: HC Cmd Cmplt", 0, 0, 0 );
      }
      bt_ec_send_event( ev_msg_ptr );
#endif
#ifndef FEATURE_BT_SOC
    }
#endif /* FEATURE_BT_SOC */
  }
  else
  {
    BT_ERR( "BT HC ERR: Attempting to send invalid HCI event: %d",
            ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
  }
}

/*===========================================================================

FUNCTION
  bt_hc_finish_reset

DESCRIPTION
  Called after the driver has been reset.  If the reset was initiated by
  HCI_Reset, then make sure the driver is in HCI mode.  Also, send the
  command complete to indiate that reset is done.

===========================================================================*/
void bt_hc_finish_reset( void )
{
  if ( bt_hc_prev_hci_mode != BT_HCIM_OFF )
  {
    /* If HCI mode is off, then enable it to what it previously was.
    ** This will be the case for an HCI_Reset when HCI Mode was enabled
    ** on the UI.
    */
    if ( bt_dc.cur_hci_mode == BT_HCIM_OFF )
    {
      bt_cmd_dc_set_hci_mode( bt_hc_prev_hci_mode );
    }
    bt_hc_prev_hci_mode = BT_HCIM_OFF;

#ifndef FEATURE_BT_SOC
    bt_hc_send_cmd_cmplt_status_ev( BT_HCI_RESET, BT_BE_SUCCESS );
#endif /* FEATURE_BT_SOC */
  }
}

#endif /* FEATURE_BT */

