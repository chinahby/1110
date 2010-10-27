/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      B L U E T O O T H    O B J E C T    P U S H    P R O F I L E

GENERAL DESCRIPTION
  This module contains the Bluetooth Object Push Profile data and code
  which are not client or server specific.

Copyright (c) 2004 - 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfopp1.c#1 $ 

$Log: $
*
*   #11        28 Nov 2006            RP 
* Updated the Debug messages to print the size of the file.
*
*   #10        12 Jun 2006            DSN
* Typecasting fixes for RVCT compiler.
*
*    #9        20 Apr 2005            JH
* Converted file operation done and abort functions into queued commands.
*
*    #8        19 Jan 2005             H
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #7        06 Jan 2005            JH
* Modified to avoid possible misaligned memory accesses.
*
*    #5        23 Apr 2004            JH
* Server register and deregister commands are now queued.
*
*    #4        14 Apr 2004            JH
* Made changes related to the replacement of bt_pf_unicode_str_type command 
* parameters with uint16* type
*
*    #2        25 Feb 2004            JH
* Renamed btpfopp.h btpfoppi.h
*

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h" 
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_OPP

#include "bt.h"
#include "btpfi.h"
#include "btpfcmdi.h"
#include "bti.h"
#include "btmsg.h"
#include "btpfoppi.h"
#include "oi_opp_client.h"
#include "oi_opp_server.h"
#include "opp_common.c" 

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
extern bt_app_id_type    bt_pf_opp_cli_app_id;
extern bt_pf_opp_cli_conn_id_type bt_pf_opp_cli_conn_id;

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_opp_process_command    
   
DESCRIPTION
  Processes the given command by calling the underlying OI OPP driver.
   
===========================================================================*/
void bt_pf_opp_process_command( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_status_type         status;
  bt_pf_opp_cli_conn_id_type conn_id;
  OI_OBEX_UNICODE            name;
  OI_OBEX_UNICODE*           name_ptr;
  uint16                     name_str[BT_PF_MAX_FILENAME_LEN + 1];
  bt_bd_addr_type            bd_addr;

  switch ( cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_PF_CMD_OPP_CLI_CONNECT:
      bd_addr = cmd_ptr->cmd_msg.cmd_opp_cli_connect.bd_addr;
      //BT_BDA( MSG_API, "BT PF OPP CLI CMD RX: Connect", &bd_addr );
      status = bt_pf_opp_cli_cmd_connect(
        cmd_ptr->cmd_hdr.app_id,
        &bd_addr,
        cmd_ptr->cmd_msg.cmd_opp_cli_connect.channel,
        &conn_id );
      if (status == OI_OK)
      {
        status = bt_pf_opp_cli_set_conn_id( conn_id );
      }
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_CLI_DISCONNECT:
      BT_MSG_API( "BT PF OPP CLI CMD RX: Disconnect AID: %x CID %x",
                  cmd_ptr->cmd_hdr.app_id,
                  cmd_ptr->cmd_msg.cmd_opp_cli_disconnect.conn_id, 0 );
      status = OI_OPPClient_Disconnect( 
        (OI_OPP_CLIENT_CONNECTION_HANDLE)
        cmd_ptr->cmd_msg.cmd_opp_cli_disconnect.conn_id );
	  cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_CLI_PUSH:
      BT_MSG_API( "BT PF OPP CLI CMD RX: Push AID: %x CID %x",
                  cmd_ptr->cmd_hdr.app_id,
                  cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.conn_id, 0 );
      if (cmd_ptr->cmd_msg.cmd_opp_cli_push.name_len == 0)
      {
        name_ptr = NULL;
      }
      else
      {
        /* name is copied to avoid misaligned access when folder_str not 
           aligned */
        memcpy( (void*)name_str,
                (void*)cmd_ptr->cmd_msg.cmd_opp_cli_push.name_str,
                cmd_ptr->cmd_msg.cmd_opp_cli_push.name_len * sizeof( uint16 ) );
        name.len = cmd_ptr->cmd_msg.cmd_opp_cli_push.name_len;
        name.str = name_str;
        name_ptr = &name;
      } 
      status = OI_OPPClient_Push(
        (OI_OPP_CLIENT_CONNECTION_HANDLE)
        cmd_ptr->cmd_msg.cmd_opp_cli_push.conn_id,
        name_ptr,
        (OI_CHAR*)cmd_ptr->cmd_msg.cmd_opp_cli_push.type_str );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_CLI_PULL:
      BT_MSG_API( "BT PF OPP CLI CMD RX: Pull AID: %x CID %x",
                  cmd_ptr->cmd_hdr.app_id,
                  cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.conn_id, 0 );
      status = OI_OPPClient_Pull(
        (OI_OPP_CLIENT_CONNECTION_HANDLE)
        cmd_ptr->cmd_msg.cmd_opp_cli_push.conn_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_CLI_ABORT:
      BT_MSG_API( "BT PF OPP CMD RX: ABort AID: %x CID %x",
                  cmd_ptr->cmd_hdr.app_id,
                  cmd_ptr->cmd_msg.cmd_opp_cli_abort.conn_id, 0 );
      status = bt_pf_opp_cli_cmd_abort(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_opp_cli_abort.conn_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_CLI_OPEN_READ_DONE:
      BT_MSG_API( "BT PF OPP CMD RX: Open Read Done AID|CID 0x%04x Stat %x Size 0x%x",
                  (cmd_ptr->cmd_hdr.app_id << 16 |
                  cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.conn_id),
                  cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.status,
                  cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.size );
      status = bt_pf_opp_cli_cmd_open_read_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.conn_id,
        cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.handle,
        (uint16*)cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.name_str,
        cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.name_len,
        (char*)cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.type_str,
        cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.size,
        cmd_ptr->cmd_msg.cmd_opp_cli_open_read_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_CLI_OPEN_WRITE_DONE:
      BT_MSG_API( "BT PF OPP CMD RX: Open Write Done AID: %x CID %x Stat %x",
                  cmd_ptr->cmd_hdr.app_id,
                  cmd_ptr->cmd_msg.cmd_opp_cli_open_write_done.conn_id,
                  cmd_ptr->cmd_msg.cmd_opp_cli_open_write_done.status );
      status = bt_pf_opp_cli_cmd_open_write_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_opp_cli_open_write_done.conn_id,
        cmd_ptr->cmd_msg.cmd_opp_cli_open_write_done.handle,
        cmd_ptr->cmd_msg.cmd_opp_cli_open_write_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_CLI_READ_DONE:
      BT_MSG_API( "BT PF OPP CMD RX: Read Done AID|CID 0x%04x Stat %x Len 0x%x",
                  (cmd_ptr->cmd_hdr.app_id << 16 |
                  cmd_ptr->cmd_msg.cmd_opp_cli_read_done.conn_id),
                  cmd_ptr->cmd_msg.cmd_opp_cli_read_done.status,
                  cmd_ptr->cmd_msg.cmd_opp_cli_read_done.data_len );
      status = bt_pf_opp_cli_cmd_read_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_opp_cli_read_done.conn_id,
        cmd_ptr->cmd_msg.cmd_opp_cli_read_done.handle,
        cmd_ptr->cmd_msg.cmd_opp_cli_read_done.data_ptr,
        cmd_ptr->cmd_msg.cmd_opp_cli_read_done.data_len,
        cmd_ptr->cmd_msg.cmd_opp_cli_read_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_CLI_WRITE_DONE:
      BT_MSG_API( "BT PF OPP CMD RX: Write Done AID: %x CID %x Stat %x",
                  cmd_ptr->cmd_hdr.app_id,
                  cmd_ptr->cmd_msg.cmd_opp_cli_write_done.conn_id,
                  cmd_ptr->cmd_msg.cmd_opp_cli_write_done.status );
      status = bt_pf_opp_cli_cmd_write_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_opp_cli_write_done.conn_id,
        cmd_ptr->cmd_msg.cmd_opp_cli_write_done.handle,
        cmd_ptr->cmd_msg.cmd_opp_cli_write_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_SRV_REGISTER:
      BT_MSG_API( "BT PF OPP SRV CMD RX: Reg AID: %x", 
                  cmd_ptr->cmd_hdr.app_id, 0, 0 );
      status = bt_pf_opp_srv_cmd_register(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_opp_srv_register.supported_formats,
        (char *)cmd_ptr->cmd_msg.cmd_opp_srv_register.srv_name_str ); 
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_SRV_DEREGISTER:
      BT_MSG_API( "BT PF OPP SRV CMD RX: Dereg AID: %x", 
                  cmd_ptr->cmd_hdr.app_id, 0, 0 );
      status = bt_pf_opp_srv_cmd_deregister( cmd_ptr->cmd_hdr.app_id );  
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_SRV_ACCEPT:
      BT_MSG_API( "BT PF OPP SRV CMD RX: Accept AID: %x CID %x", 
                  cmd_ptr->cmd_hdr.app_id,
                  cmd_ptr->cmd_msg.cmd_opp_srv_accept.conn_id, 0 );
      status = OI_OPP_AcceptConnect( 
        (OI_OPP_SERVER_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_opp_srv_accept.conn_id,
        cmd_ptr->cmd_msg.cmd_opp_srv_accept.allow_push,
        cmd_ptr->cmd_msg.cmd_opp_srv_accept.allow_pull );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_SRV_FORCE_DCN:
      BT_MSG_API( "BT PF OPP SRV CMD RX: Force Disconnect AID: %x CID %x", 
                  cmd_ptr->cmd_hdr.app_id,
                  cmd_ptr->cmd_msg.cmd_opp_srv_force_dcn.conn_id, 0 );
      status = OI_OPPServer_ForceDisconnect( 
        (OI_OPP_SERVER_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_opp_srv_force_dcn.conn_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_SRV_OPEN_READ_DONE:
      BT_MSG_API( "BT PF OPP CMD RX: Open Read Done AID|CID 0x%04x Stat %x, Size 0x%x",
                  (cmd_ptr->cmd_hdr.app_id << 16 |
                  cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.conn_id),
                  cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.status,
                  cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.size );
      status = bt_pf_opp_srv_cmd_open_read_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.conn_id,
        cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.handle,
        (uint16*)cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.name_str,
        cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.name_len,
        (char*)cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.type_str,
        cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.size,
        cmd_ptr->cmd_msg.cmd_opp_srv_open_read_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_SRV_OPEN_WRITE_DONE:
      BT_MSG_API( "BT PF OPP CMD RX: Open Write Done AID: %x CID %x Stat %x",
                  cmd_ptr->cmd_hdr.app_id,
                  cmd_ptr->cmd_msg.cmd_opp_srv_open_write_done.conn_id,
                  cmd_ptr->cmd_msg.cmd_opp_srv_open_write_done.status );
      status = bt_pf_opp_srv_cmd_open_write_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_opp_srv_open_write_done.conn_id,
        cmd_ptr->cmd_msg.cmd_opp_srv_open_write_done.handle,
        cmd_ptr->cmd_msg.cmd_opp_srv_open_write_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_SRV_READ_DONE:
      BT_MSG_API( "BT PF OPP CMD RX: Read Done AID|CID 0x%04x Stat %x Len 0x%x",
                  (cmd_ptr->cmd_hdr.app_id << 16 |
                  cmd_ptr->cmd_msg.cmd_opp_srv_read_done.conn_id),
                  cmd_ptr->cmd_msg.cmd_opp_srv_read_done.status,
                  cmd_ptr->cmd_msg.cmd_opp_srv_read_done.data_len );
      status = bt_pf_opp_srv_cmd_read_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_opp_srv_read_done.conn_id,
        cmd_ptr->cmd_msg.cmd_opp_srv_read_done.handle,
        cmd_ptr->cmd_msg.cmd_opp_srv_read_done.data_ptr,
        cmd_ptr->cmd_msg.cmd_opp_srv_read_done.data_len,
        cmd_ptr->cmd_msg.cmd_opp_srv_read_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_OPP_SRV_WRITE_DONE:
      BT_MSG_API( "BT PF OPP CMD RX: Write Done AID: %x CID %x Stat %x",
                  cmd_ptr->cmd_hdr.app_id,
                  cmd_ptr->cmd_msg.cmd_opp_srv_write_done.conn_id,
                  cmd_ptr->cmd_msg.cmd_opp_srv_write_done.status );
      status = bt_pf_opp_srv_cmd_write_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_opp_srv_write_done.conn_id,
        cmd_ptr->cmd_msg.cmd_opp_srv_write_done.handle,
        cmd_ptr->cmd_msg.cmd_opp_srv_write_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    default:
      BT_ERR( "BT PF OPP CMD RX: Unknown: %x",
              cmd_ptr->cmd_hdr.cmd_type, 0, 0 );
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
  }
}

#endif /* FEATURE_BT_EXTPF_OPP */
#endif /* FEATURE_BT */ 
                                   
