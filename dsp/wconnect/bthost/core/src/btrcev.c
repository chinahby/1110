/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    R F C O M M    E V E N T S

GENERAL DESCRIPTION
  This file contains the Bluetooth code that handles BT events for the
  RFCOMM layer.

Copyright (c) 2000 - 2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btrcev.c_v   1.11   05 Aug 2002 17:59:18   ryuen
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btrcev.c#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btrcev.c_v  $
 *
 *   #16         22 May 2007            SSK
 * Fixed compiler warnings on RVCT2.2 compiler.
 *
 *   #15         21 Feb 2007            PN
 * Fixed critical lint errors.
 *
 *   #14         04 Dec 2006            JTL
 * Disconnect L2Cap link if we get an unexpected connect event.
 *
 *   #13         14 Aug 2006            JTL
 * Verify CID in connection failed event; don't clear the SSN info
 * for another connection.
 *
 *   #12         25 May 2006            JTL
 * Fixing some problems with debug messages printing incorrect info.
 *
 *   #11         21 Apr 2006            JTL
 * Send connect failed event when L2 is disconnected and the MCC is in the
 * CONNECTING_DLC state.
 *
 *   #10         16 Mar 2006            JTL
 * Added Unregister event.
 *
 *    #9         6  Jul 2005            JH
 * Roll back change #8. Timeout now disabled within L2 for all L2 connections.
 *
 *    #8         27 Jun 2005            JH
 * Disable L2CAP timeout in test mode when L2CAP connection for RFCOMM is setup.
 *
 *    #7         8  Apr 2005            DP
 * Make use of BT_BDA debug message macro.
 *
 *    #6         8  Jun 2004            DP
 * Added client side Mode 2 security.
 *
 *    #5         1  Jun 2004            DP
 * Implemented Mode 2 security.
 *
 *    #4         19 Dec 2003            DP
 * Improved disconnect and connection failure indications to applications.
 *
 *    #3         4  Nov 2003            DP
 * Notify application if MCC fails to come up.
 * 
 *    Rev 1.11   05 Aug 2002 17:59:18   ryuen
 * Fixed MFS calculation based on MTU
 * 
 *    Rev 1.10   12 Apr 2002 10:57:44   propach
 * Fixed MFS calculation based on L2CAP's MTU.
 * 
 *    Rev 1.9   03 Apr 2002 14:06:00   propach
 * Enhanced modem status changed events.
 * 
 *    Rev 1.8.1.0   24 Jul 2002 12:09:20   propach
 * Fixed MFS calculation based on MTU.  Merged improved RFCOMM modem
 * status changes onto branch.
 * 
 *    Rev 1.8   23 Oct 2001 16:52:00   propach
 * Added support for RFCOMM Credit Based Flow Control (CBFC).
 * 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>
#include "queue.h"
#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#include "btmsg.h"
#include "btrcev.h"
#include "btrci.h"
#include "btrcmsgi.h"
#include "btrctx.h"
#include "btutils.h"


#define BT_MSG_LAYER  BT_MSG_RC   /* necessary for btmsg.h */

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_rc_ev_l2_connected

DESCRIPTION
  Handles an L2CAP connected event sent to RFCOMM.  This means a remote
  client application has requested to connect to a resident server.

===========================================================================*/
LOCAL void bt_rc_ev_l2_connected
(
  bt_ev_msg_type*  l2_conn_ptr
)
{

  bt_rc_session_type*  ssn_ptr;

  BT_MSG_API( "BT RC EV RX: L2 Conn AID %x CID %x",
              l2_conn_ptr->ev_hdr.bt_app_id,
              l2_conn_ptr->ev_msg.ev_l2_conn.cid, 0 );
  //BT_BDA( MSG_API, "BT RC EV RX: L2 Conn",
  //        &l2_conn_ptr->ev_msg.ev_l2_conn.bd_addr );

  if ( (ssn_ptr = bt_rc_find_session_bd_addr(
                    &l2_conn_ptr->ev_msg.ev_l2_conn.bd_addr )) != NULL )
  {
    if ( ssn_ptr->mcc.state == BT_RCDS_CONNECTING_L2CAP )
    {
      if ( ssn_ptr->mcc.expected_rx_mask == 0 )
      {
        ssn_ptr->cid       = l2_conn_ptr->ev_msg.ev_l2_conn.cid;
        ssn_ptr->mcc.state = BT_RCDS_CONNECTING_DLC;
        
        ssn_ptr->mcc.config.max_frame_size =
          l2_conn_ptr->ev_msg.ev_l2_conn.mtu -
          BT_RC_MAX_OVERHEAD_BYTES;

        BT_MSG_HIGH( "BT RC: Client MCC Conn MTU %x CID %x",
                     ssn_ptr->mcc.config.max_frame_size,
                     ssn_ptr->cid, 0 );

        bt_rc_tx_sabm_frame( ssn_ptr, &ssn_ptr->mcc );
      }
      else
      {
        BT_ERR( "BT RC: Bad L2 Conn EM %x CID %x",
                ssn_ptr->mcc.expected_rx_mask,
                l2_conn_ptr->ev_msg.ev_l2_conn.cid, 0 );
      }
    }
    else
    {
      /*  Unexpected connected event - MCC DLC state not right.  */
      BT_ERR( "BT RC: Bad MCC DLC State %x",
              ssn_ptr->mcc.state, 0, 0 );
      bt_cmd_l2_disconnect( bt_rc_app_id, l2_conn_ptr->ev_msg.ev_l2_conn.cid );      
    }
  }
  else
  {
    if ( bt_rc_reg_count != 0 )
    {
      if ( (ssn_ptr = bt_rc_find_free_session_entry()) != NULL )
      {
        ssn_ptr->bd_addr = l2_conn_ptr->ev_msg.ev_l2_conn.bd_addr;
        ssn_ptr->cid     = l2_conn_ptr->ev_msg.ev_l2_conn.cid;
        
        ssn_ptr->mcc.config.max_frame_size =
          l2_conn_ptr->ev_msg.ev_l2_conn.mtu -
          BT_RC_MAX_OVERHEAD_BYTES;

        ssn_ptr->initiator            = FALSE;
        ssn_ptr->mcc.expected_rx_mask = BT_ERXM_SABM_B;
        ssn_ptr->mcc.state            = BT_RCDS_CONNECTING_DLC;

        BT_MSG_HIGH( "BT RC: Server MCC Conn MTU %x CID %x",
                     ssn_ptr->mcc.config.max_frame_size,
                     ssn_ptr->cid, 0 );
      }
      else
      {
        BT_ERR( "BT RC: L2 Conn SSN Full Cnt %x CID %x",
                bt_rc_reg_count, l2_conn_ptr->ev_msg.ev_l2_conn.cid, 0 );
      }
    }
    else
    {
      BT_ERR( "BT RC: L2 Conn No reg'd serv Cnt %x",
              bt_rc_reg_count, 0, 0 );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_ev_l2_rx_data

DESCRIPTION
  Handles an L2CAP received data event.  This means some data was
  received for RFCOMM.

===========================================================================*/
void bt_rc_ev_l2_rx_data
(
  bt_ev_msg_type*  l2_rxd_ptr
)
{

  bt_rc_rx_info_type  rx_info;

  rx_info.error = FALSE;
  rx_info.fh_dlci = 0xFF;  /*  Initialize rest to illegal values.  */
  rx_info.fh_command = 0xF0;
  rx_info.fh_direction = 0xF0;
  rx_info.fh_server_channel = 0xFF;
  rx_info.fh_frame_type = 0xFF;
  rx_info.fh_poll_final = 0xF0;
  rx_info.fh_frame_length = 0xFFFF;
  rx_info.fcs_calculated = 0x0;
  rx_info.fcs_received = 0x0;
  rx_info.mcch_type = 0xFF;
  rx_info.mcch_command = 0xF0;
  rx_info.mcch_length = 0xFF;
  rx_info.ssn_ptr = NULL;
  rx_info.dlc_ptr = NULL;
  rx_info.dsm_ptr = NULL;

  rx_info.dsm_ptr = l2_rxd_ptr->ev_msg.ev_l2_rxd.dsm_ptr;

  if( rx_info.dsm_ptr != NULL )
  {
    BT_SET_PKT_TRACER( rx_info.dsm_ptr, DSM_BT_RX_RC_DATA_EV );
  }

  rx_info.ssn_ptr = bt_rc_find_session_cid(
                      l2_rxd_ptr->ev_msg.ev_l2_rxd.cid );
  if ( rx_info.ssn_ptr != NULL )
  {
    rx_info.ssn_ptr->l2_rx_data_evs++;
    bt_rc_rx_unpack_frame_hdr_and_fcs( &rx_info );

    if( rx_info.dsm_ptr != NULL )
    {
      BT_SET_PKT_TRACER( rx_info.dsm_ptr, DSM_BT_RX_RC_UNPACKED );
    }
    if ( rx_info.error == FALSE )
    {
      if ( rx_info.fh_dlci == RC_DLCI_MCC_V )
      {
        rx_info.dlc_ptr = &rx_info.ssn_ptr->mcc;
      }
      else
      {
        if ( rx_info.fh_frame_type != RCFH_CTRL_FT_UIH_V )
        {
          rx_info.dlc_ptr = bt_rc_find_dlc_entry_dlci(
                              rx_info.ssn_ptr, rx_info.fh_dlci );
          if ( rx_info.dlc_ptr == NULL )
          {
            bt_rc_setup_local_server_dlc( &rx_info );
          }
        }
      }
      if ( rx_info.error == FALSE )
      {
        if ( rx_info.dlc_ptr != NULL )
        {
          bt_rc_rx_process_frame( &rx_info );
        }
        else
        {
          /*  Could not find DLC entry for received data.  */
          rx_info.error = TRUE;
          BT_ERR( "BT RC: No DLCI CID %x NReg %x",
                  rx_info.ssn_ptr->cid, bt_rc_reg_count, 0 );
        }
      }
    }
  }
  else
  {
    /*  Unexpected receive data event - session for CID not found.  */
    rx_info.error = TRUE;
    BT_ERR( "BT RC: No SSN CID %x",
            l2_rxd_ptr->ev_msg.ev_l2_rxd.cid, 0, 0);
  }

  if ( rx_info.error != FALSE )
  {
    if ( rx_info.dlc_ptr != NULL )
    {
      rx_info.dlc_ptr->rx_errors++;
    }
  }

  if ( rx_info.dsm_ptr != NULL )
  {
    BT_ERR( "BT RC RX: DSM ptr Not NULL %x", rx_info.dsm_ptr, 0, 0 );
    dsm_free_packet( &rx_info.dsm_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_ev_l2_disconnected

DESCRIPTION
  Handles an L2CAP disconnected event sent to the RFCOMM layer.  This
  means the multiplexer has been disconnected.

===========================================================================*/
LOCAL void bt_rc_ev_l2_disconnected
(
  bt_ev_msg_type*  l2_disc_ptr
)
{

  bt_rc_session_type*  ssn_ptr;
  uint8                i;
  boolean              conn_failed;

  BT_MSG_API( "BT RC EV RX: L2 Disc Rsn %x CID %x AID %x",
              l2_disc_ptr->ev_msg.ev_l2_disc.reason,
              l2_disc_ptr->ev_msg.ev_l2_disc.cid,
              l2_disc_ptr->ev_hdr.bt_app_id );

  if ( (ssn_ptr = bt_rc_find_session_cid(
                    l2_disc_ptr->ev_msg.ev_l2_disc.cid )) != NULL )
  {
    if ( ( (ssn_ptr->mcc.state == BT_RCDS_CONNECTING_L2CAP) ||
           (ssn_ptr->mcc.state == BT_RCDS_CONNECTING_DLC)     ) &&
         (ssn_ptr->dlc_count == 0) )
    {
      /*  Was bringing up MCC for first DLCI.        */
      /*  Inform application of connection failure.  */
      bt_rc_ev_send_connection_failed(
        ssn_ptr->mcc.bt_app_id,
        &ssn_ptr->bd_addr,
        l2_disc_ptr->ev_msg.ev_l2_disc.reason );
    }
    else if ( ssn_ptr->dlc_count != 0 )
    {
      /*  Disconnected Multiplexer with open DLCs.  */
      BT_VSTAT_INC( RC_L2_DISC_DLCS_ACTIVE );
    
      for ( i = 0; i < BT_RC_MAX_PORTS_PER_SESSION; i++ )
      {
        if ( ssn_ptr->dlc[ i ].state != BT_RCDS_DISCONNECTED )
        {
          if ( ssn_ptr->dlc[ i ].state == BT_RCDS_CONNECTING_DLC )
          {
            conn_failed = TRUE;
          }
          else
          {
            conn_failed = FALSE;
          }
          ssn_ptr->dlc[ i ].state = BT_RCDS_DISCONNECTING_DLC;
          bt_rc_update_disconnected_dlc(
            ssn_ptr, &(ssn_ptr->dlc[ i ]), conn_failed,
            l2_disc_ptr->ev_msg.ev_l2_disc.reason );
        }
      }
    }
  
    ssn_ptr->mcc.state = BT_RCDS_DISCONNECTING_DLC;
    bt_rc_update_disconnected_mcc( ssn_ptr );

    if ( ssn_ptr->mcc.state != BT_RCDS_DISCONNECTING_L2CAP )
    {
      /*  Bad MCC state for L2CAP disconnect.  */
      BT_ERR( "BT RC: Bad MCC St %x Rsn %x Sidx %x",
              ssn_ptr->mcc.state,
              l2_disc_ptr->ev_msg.ev_l2_disc.reason,
              ssn_ptr->ssn_idx );
    }

    bt_rc_initialize_session_entry( ssn_ptr->ssn_idx );
  }
  else
  {
    BT_ERR( "BT RC: No SSN L2 Disc CID %x",
            l2_disc_ptr->ev_msg.ev_l2_disc.cid, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_ev_l2_conn_failed

DESCRIPTION
  Handles an L2CAP connection failed event sent to the RFCOMM layer.

===========================================================================*/
LOCAL void bt_rc_ev_l2_conn_failed
(
  bt_ev_msg_type*  l2_cf_ptr
)
{

  bt_rc_session_type*  ssn_ptr;

  BT_MSG_API( "BT RC EV RX: L2 ConnF R %x AID %x",
              l2_cf_ptr->ev_msg.ev_l2_conn_failed.reason,
              l2_cf_ptr->ev_hdr.bt_app_id, 0 );
  //BT_BDA( MSG_API, "BT RC EV RX: L2 ConnF",
  //        &l2_cf_ptr->ev_msg.ev_l2_conn_failed.bd_addr );

  if ( (ssn_ptr = bt_rc_find_session_bd_addr(
                    &l2_cf_ptr->ev_msg.ev_l2_conn_failed.
                       bd_addr )) != NULL )
  {
    if ( (ssn_ptr->mcc.state == BT_RCDS_CONNECTING_L2CAP)         &&
         (ssn_ptr->dlc_count == 0)                                &&
         (ssn_ptr->cid == l2_cf_ptr->ev_msg.ev_l2_conn_failed.cid) )
    {
      /*  Was bringing up MCC for first DLCI.        */
      /*  Inform application of connection failure.  */
      bt_rc_ev_send_connection_failed(
        ssn_ptr->mcc.bt_app_id,
        &ssn_ptr->bd_addr,
        l2_cf_ptr->ev_msg.ev_l2_conn_failed.reason );
      bt_rc_initialize_session_entry( ssn_ptr->ssn_idx );
    }
    else
    {
      BT_ERR( "BT RC: L2 CF Bad SSN St %x DC %x CID %x",
              ssn_ptr->mcc.state, ssn_ptr->dlc_count,
              ssn_ptr->cid );
    }
  }
  else
  {
    //BT_BDA( ERR, "BT RC: No SSN L2 ConnF",
    //        &l2_cf_ptr->ev_msg.ev_l2_conn_failed.bd_addr );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_ev_rm_rfcomm_scn_sec_result

DESCRIPTION
  Handles a RM RFCOMM SCN Security Result event sent to the RFCOMM layer.

===========================================================================*/
LOCAL void bt_rc_ev_rm_rfcomm_scn_sec_result
(
  bt_ev_msg_type*  rm_rsr_ptr
)
{

  bt_rc_session_type*  ssn_ptr;
  bt_rc_dlc_type*      dlc_ptr = NULL;
  uint8                i;

  BT_MSG_API( "BT RC EV RX: RM SCN SecRes %x SCN %x",
              rm_rsr_ptr->ev_msg.ev_rm_srscn.secure_enough,
              rm_rsr_ptr->ev_msg.ev_rm_srscn.rfcomm_scn, 0 );
  //BT_BDA( MSG_API, "BT RC EV RX: RM SCN SecRes",
  //        &rm_rsr_ptr->ev_msg.ev_rm_srscn.bd_addr );

  if ( (ssn_ptr = bt_rc_find_session_bd_addr(
                    &rm_rsr_ptr->ev_msg.ev_rm_srscn.bd_addr )) != NULL )
  {
    /*  Find DLC coming up right now on this session  */
    /*  with the matching server channel number.      */

    for ( i = 0; i < BT_RC_MAX_PORTS_PER_SESSION; i++ )
    {
      if ( (ssn_ptr->dlc[ i ].state == BT_RCDS_CONNECTING_DLC) &&
           (ssn_ptr->dlc[ i ].server_channel == 
              rm_rsr_ptr->ev_msg.ev_rm_srscn.rfcomm_scn) )
      {
        dlc_ptr = &ssn_ptr->dlc[ i ];
        break;
      }
    }

    if ( dlc_ptr != NULL )
    {
      if ( dlc_ptr->direction == ssn_ptr->initiator )
      {
        /*  For local servers, the direction of the DLC is  */
        /*  equal to the multiplexer's initiator setting.   */

        /*  Received service security result due to prior   */
        /*  receipt of SABM for DLC setup.  Continue setup  */
        /*  procedure based on result.                      */ 
  
        bt_rc_rx_sabm_service_sec_result(
          ssn_ptr, dlc_ptr, rm_rsr_ptr->ev_msg.ev_rm_srscn.secure_enough );
      }
      else
      {
        /*  For local clients, the direction of the DLC is the  */
        /*  opposite of the multiplexer's initiator setting.    */

        /*  Received service security result due to prior  */
        /*  setup of MCC.  Continue setup of client DLC    */
        /*  procedure based on result.                     */ 
  
        bt_rc_loc_client_dlc_srv_sec_result(
          ssn_ptr, dlc_ptr, rm_rsr_ptr->ev_msg.ev_rm_srscn.secure_enough );
      }
    }
    else
    {
      BT_ERR( "BT RC: No DLC RM SecRes SCN %x Sidx %x CID %x",
              rm_rsr_ptr->ev_msg.ev_rm_srscn.rfcomm_scn,
              ssn_ptr->ssn_idx, ssn_ptr->cid );
    }
  }
  else
  {
    //BT_BDA( ERR, "BT RC: No SSN RM SecRes",
    //        &rm_rsr_ptr->ev_msg.ev_rm_srscn.bd_addr );
  }

}


/*===========================================================================

FUNCTION
  bt_rc_get_max_event_bytes

DESCRIPTION
  Returns the number of bytes in the largest event expected to be
  processed by RFCOMM.

===========================================================================*/
uint16 bt_rc_get_max_event_bytes
(
  void
)
{

  uint16  max_eb;

  max_eb = sizeof( bt_ev_gn_cmd_done_type );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_connected_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_rx_data_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_disconnected_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_l2_connection_failed_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_rfcomm_scn_sec_result_type ) );

  max_eb += sizeof( bt_ev_hdr_type );

  return( max_eb );

}


/*===========================================================================

FUNCTION
  bt_rc_process_event

DESCRIPTION
  Handles all BT driver events coming to the RFCOMM layer.

===========================================================================*/
void bt_rc_process_event
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
      {
        if ( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status !=
             BT_CS_GN_SUCCESS )
        {
          BT_ERR( "BT RC: Bad CMD Done Stat %x Typ %x AID %x",
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type,
                  ev_msg_ptr->ev_hdr.bt_app_id );
        }
        break;
      }

    case BT_EV_L2_CONNECTED:
      {
        bt_rc_ev_l2_connected( ev_msg_ptr );
        break;
      }

    case BT_EV_L2_RX_DATA:
      {
        bt_rc_ev_l2_rx_data( ev_msg_ptr );
        break;
      }

    case BT_EV_L2_DISCONNECTED:
      {
        bt_rc_ev_l2_disconnected( ev_msg_ptr );
        break;
      }

    case BT_EV_L2_CONNECTION_FAILED:
      {
        bt_rc_ev_l2_conn_failed( ev_msg_ptr );
        break;
      }

    case BT_EV_RM_RFCOMM_SCN_SEC_RESULT:
      {
        bt_rc_ev_rm_rfcomm_scn_sec_result( ev_msg_ptr );
        break;
      }

    default:
      {
        BT_ERR( "BT RC: Bad RX EV Type %x AID %x",
                ev_msg_ptr->ev_hdr.ev_type,
                ev_msg_ptr->ev_hdr.bt_app_id, 0 );
        break;
      }
  }

}


/*===========================================================================

FUNCTION
  bt_rc_ev_copy_config

DESCRIPTION
  Copies specified DLC configuration information into a specified
  RFCOMM event configuration field.

===========================================================================*/
void bt_rc_ev_copy_config
(
  bt_rc_ev_config_type*  ev_cfg_ptr,
  bt_rc_config_type*     dlc_cfg_ptr
)
{

  ev_cfg_ptr->pn_negot_done  = dlc_cfg_ptr->pn_negot_done;
  ev_cfg_ptr->rpn_negot_done = dlc_cfg_ptr->rpn_negot_done;
  ev_cfg_ptr->max_frame_size = dlc_cfg_ptr->max_frame_size;
  ev_cfg_ptr->priority       = dlc_cfg_ptr->priority;
  ev_cfg_ptr->cb_flow_ctrl   = dlc_cfg_ptr->cbfc_enabled;
  ev_cfg_ptr->baudrate       = dlc_cfg_ptr->baudrate;
  ev_cfg_ptr->format         = dlc_cfg_ptr->format;
  ev_cfg_ptr->flow_ctrl      = dlc_cfg_ptr->flow_ctrl;
  ev_cfg_ptr->xon_char       = dlc_cfg_ptr->xon_char;
  ev_cfg_ptr->xoff_char      = dlc_cfg_ptr->xoff_char;

}


/*===========================================================================

FUNCTION
  bt_rc_ev_send_connected

DESCRIPTION
  Sends an RFCOMM connected event with specified parameters.

===========================================================================*/
void bt_rc_ev_send_connected
(
  bt_app_id_type      bt_app_id,
  bt_bd_addr_type*    bd_addr_ptr,
  uint16              pid,
  bt_rc_config_type*  config_ptr
)
{

  bt_ev_msg_type  ev_rc_conn;

  BT_MSG_API( "BT RC EV TX: RC Conn AID %x PID %x", bt_app_id, pid, 0 );
  //BT_BDA( MSG_API, "BT RC EV TX: RC Conn", bd_addr_ptr );

  ev_rc_conn.ev_hdr.ev_type            = BT_EV_RC_CONNECTED;
  ev_rc_conn.ev_hdr.bt_app_id          = bt_app_id;
  ev_rc_conn.ev_msg.ev_rc_conn.pid     = pid;
  ev_rc_conn.ev_msg.ev_rc_conn.bd_addr = *bd_addr_ptr;
  
  bt_rc_ev_copy_config( &ev_rc_conn.ev_msg.ev_rc_conn.config, config_ptr );
  
  bt_ec_send_event( &ev_rc_conn );

}


/*===========================================================================

FUNCTION
  bt_rc_ev_send_ms_changed

DESCRIPTION
  Sends an RFCOMM Modem Status changed event with specified parameters.

===========================================================================*/
void bt_rc_ev_send_ms_changed
(
  bt_app_id_type  bt_app_id,
  uint16          pid,
  uint8           ms_changed_mask,
  uint8           modem_status,
  boolean         break_present,
  uint8           break_length,
  uint8           breaks
)
{

  bt_ev_msg_type  ev_rc_mschg;

  BT_MSG_API( "BT RC EV TX: MS Chgd %02x %02x PID %x",
              ms_changed_mask, modem_status, pid );
  
  ev_rc_mschg.ev_hdr.ev_type   = BT_EV_RC_MS_CHANGED;
  ev_rc_mschg.ev_hdr.bt_app_id = bt_app_id;
  
  ev_rc_mschg.ev_msg.ev_rc_ms_chgd.pid             = pid;
  ev_rc_mschg.ev_msg.ev_rc_ms_chgd.ms_changed_mask =
    (bt_spp_ms_enum_type)ms_changed_mask;
  ev_rc_mschg.ev_msg.ev_rc_ms_chgd.modem_status    =
    (bt_spp_ms_enum_type)(modem_status & RCMCCM_MSC_V24_MS_SIGS_M);
  ev_rc_mschg.ev_msg.ev_rc_ms_chgd.break_present   = break_present;
  ev_rc_mschg.ev_msg.ev_rc_ms_chgd.break_length    = break_length;
  ev_rc_mschg.ev_msg.ev_rc_ms_chgd.breaks          = breaks;
  
  bt_ec_send_event( &ev_rc_mschg );

}


/*===========================================================================

FUNCTION
  bt_rc_ev_send_connection_failed

DESCRIPTION
  Sends an RFCOMM connection failed event with specified parameters.

===========================================================================*/
void bt_rc_ev_send_connection_failed
(
  bt_app_id_type        bt_app_id,
  bt_bd_addr_type*      bd_addr_ptr,
  bt_event_reason_type  reason
)
{

  bt_ev_msg_type  ev_rc_cf;

  BT_MSG_API( "BT RC EV TX: RC ConnF R %x AID %x", reason, bt_app_id, 0 );
  //BT_BDA( MSG_API, "BT RC EV TX: RC ConnF", bd_addr_ptr );

  ev_rc_cf.ev_hdr.ev_type                   = BT_EV_RC_CONNECTION_FAILED;
  ev_rc_cf.ev_hdr.bt_app_id                 = bt_app_id;
  ev_rc_cf.ev_msg.ev_rc_conn_failed.reason  = reason;
  ev_rc_cf.ev_msg.ev_rc_conn_failed.bd_addr = *bd_addr_ptr;
  
  bt_ec_send_event( &ev_rc_cf );

}


/*===========================================================================

FUNCTION
  bt_rc_ev_send_disconnected

DESCRIPTION
  Sends an RFCOMM disconnected event with specified parameters.

===========================================================================*/
void bt_rc_ev_send_disconnected
(
  bt_app_id_type        bt_app_id,
  uint16                pid,
  bt_event_reason_type  reason
)
{

  bt_ev_msg_type  ev_rc_disc;

  BT_MSG_API( "BT RC EV TX: RC Disc AID %x PID %x",
              bt_app_id, pid, 0 );

  ev_rc_disc.ev_hdr.ev_type           = BT_EV_RC_DISCONNECTED;
  ev_rc_disc.ev_hdr.bt_app_id         = bt_app_id;
  ev_rc_disc.ev_msg.ev_rc_disc.pid    = pid;
  ev_rc_disc.ev_msg.ev_rc_disc.reason = reason;

  bt_ec_send_event( &ev_rc_disc );

}

/*===========================================================================

FUNCTION
  bt_rc_ev_send_unregistered

DESCRIPTION
  Sends an RFCOMM unregistered event with specified parameters.

===========================================================================*/
void bt_rc_ev_send_unregistered
(
  bt_app_id_type        bt_app_id,
  uint8                 server_channel
)
{

  bt_ev_msg_type  ev_rc_unreg;

  BT_MSG_API( "BT RC EV TX: RC Unreg AID %x SCN %x",
              bt_app_id, server_channel, 0 );

  ev_rc_unreg.ev_hdr.ev_type                           = BT_EV_RC_UNREGISTERED;
  ev_rc_unreg.ev_hdr.bt_app_id                         = bt_app_id;
  ev_rc_unreg.ev_msg.ev_rc_unregistered.server_channel = server_channel;

  bt_ec_send_event( &ev_rc_unreg );

}

#endif /* FEATURE_BT */

