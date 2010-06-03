/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H   P H O N E   B O O K   A C C E S S   P R O F I L E

GENERAL DESCRIPTION
  This module contains the Bluetooth Phone Book Access Profile data and code
  which are not client or server specific.

Copyright (c) 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfpbap1.c#1 $ 

$Log: $
*
*    #3        21 May 2007            SSK
* Remove redudant consecutive 'break' statements & add newline at End of File.
*
*    #2        05 Sep 2006            GS
* Typecasting for RVCT compiler.
*
*    #1        08 Feb 2006            GS
* Initial revision
*

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_PBAP

#include "bt.h"
#include "btpfi.h"
#include "btpfcmdi.h"
#include "bti.h"
#include "btmsg.h"
#include "btpfpbapi.h"
#include "pbap_private.c" 
#include "pbap_srv.c"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
extern bt_app_id_type               bt_pf_pbap_cli_app_id;
extern bt_pf_pbap_cli_conn_id_type  bt_pf_pbap_cli_conn_id;

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_pbap_process_command    
   
DESCRIPTION
  Processes the given command by calling the underlying PF PBAP driver.
   
===========================================================================*/
void bt_pf_pbap_process_command( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_status_type status;

  switch ( cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_PF_CMD_PBAP_CLI_CONNECT:
      BT_MSG_API( "BT PF PBAP CLI CMD RX: Connect", 0, 0, 0 );
      status = bt_pf_pbap_cli_cmd_connect(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_pbap_cli_connect) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_CLI_DISCONNECT:
      BT_MSG_API( "BT PF PBAP CLI CMD RX: Disconnect", 0, 0, 0 );
      status = bt_pf_pbap_cli_cmd_disconnect( 
          cmd_ptr->cmd_hdr.app_id,
          &(cmd_ptr->cmd_msg.cmd_pbap_cli_disconnect) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_CLI_AUTH_RSP:
      BT_MSG_API( "BT PF PBAP CLI CMD RX: Auth Rsp", 0, 0, 0 );
      status = bt_pf_pbap_cli_cmd_auth_rsp( 
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_pbap_cli_auth_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_CLI_REGISTER:
      BT_MSG_API( "BT PF PBAP CLI CMD RX: Reg", 0, 0, 0 );
      status = bt_pf_pbap_cli_cmd_register(
        cmd_ptr->cmd_hdr.app_id,
        (char*)cmd_ptr->cmd_msg.cmd_pbap_cli_register.cli_name_str );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_CLI_DEREGISTER:
      BT_MSG_API( "BT PF PBAP CLI CMD RX: Dereg", 0, 0, 0 );
      status = bt_pf_pbap_cli_cmd_deregister( cmd_ptr->cmd_hdr.app_id );  
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_CLI_PULL_PHONE_BOOK:
      BT_MSG_API( "BT PF PBAP CLI CMD RX: Pull Phone Book", 0, 0, 0 );
      status = bt_pf_pbap_cli_cmd_pull_phone_book( 
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_pbap_cli_pull_phone_book) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_CLI_SET_PHONE_BOOK:
      BT_MSG_API( "BT PF PBAP CLI CMD RX: Set Phone Book", 0, 0, 0 );
      status = bt_pf_pbap_cli_cmd_set_phone_book(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_pbap_cli_set_phone_book) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_CLI_PULL_VCARD_LISTING:
      BT_MSG_API( "BT PF PBAP CLI CMD RX: Pull vCard Listing", 0, 0, 0 );
      status = bt_pf_pbap_cli_cmd_pull_vcard_listing( 
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_list) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_CLI_PULL_VCARD_ENTRY:
      BT_MSG_API( "BT PF PBAP CLI CMD RX: Pull vCard Entry", 0, 0, 0 );
      status = bt_pf_pbap_cli_cmd_pull_vcard_entry( 
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_pbap_cli_pull_vcard_entry) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_CLI_ABORT:
      BT_MSG_API( "BT PF PBAP CLI CMD RX: Abort AID %x CID %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_pbap_cli_abort.conn_id, 
                    0 );
      status = bt_pf_pbap_cli_cmd_abort(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_pbap_cli_abort.conn_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_SRV_REGISTER:
      BT_MSG_API( "BT PF PBAP SRV CMD RX: Reg", 0, 0, 0 );
      status = bt_pf_pbap_srv_cmd_register(
        cmd_ptr->cmd_hdr.app_id,
        (char*)cmd_ptr->cmd_msg.cmd_pbap_srv_register.srv_name_str,
        cmd_ptr->cmd_msg.cmd_pbap_srv_register.supported_repositories,
        cmd_ptr->cmd_msg.cmd_pbap_srv_register.auth );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_SRV_DEREGISTER:
      BT_MSG_API( "BT PF PBAP SRV CMD RX: Dereg", 0, 0, 0 );
      status = bt_pf_pbap_srv_cmd_deregister( cmd_ptr->cmd_hdr.app_id );  
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_SRV_AUTH_RSP:
      BT_MSG_API( "BT PF PBAP SRV CMD RX: Auth Rsp", 0, 0, 0 );
      status = bt_pf_pbap_srv_cmd_auth_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_pbap_srv_auth_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_SRV_ACCEPT:
      BT_MSG_API( "BT PF PBAP SRV CMD RX: Accept", 0, 0, 0 );
      status = PBAPSRV_AcceptConnection( 
        (PBAPSRV_HANDLE)
        cmd_ptr->cmd_msg.cmd_pbap_srv_accept.conn_id,
        cmd_ptr->cmd_msg.cmd_pbap_srv_accept.accept );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_SRV_FORCE_DCN:
      BT_MSG_API( "BT PF PBAP SRV CMD RX: Force Disconnect", 0, 0, 0 );
      status = PBAPSRV_ForceDisconnect( 
        (PBAPSRV_HANDLE)
          cmd_ptr->cmd_msg.cmd_pbap_srv_force_dcn.conn_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break; 
 
    case BT_PF_CMD_PBAP_SRV_PULL_PHONE_BOOK_RSP:
      BT_MSG_API( "BT PF PBAP SRV CMD RX: Pull Phone Book Rsp AID %x CID %x Stat %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_pbap_srv_pull_phone_book_rsp.conn_id, 
                    cmd_ptr->cmd_msg.cmd_pbap_srv_pull_phone_book_rsp.status );
      status = bt_pf_pbap_srv_cmd_pull_phone_book_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_pbap_srv_pull_phone_book_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_SRV_SET_PHONE_BOOK_RSP:
      BT_MSG_API( "BT PF PBAP SRV CMD RX: Set Phone Book Rsp AID %x CID %x Stat %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_pbap_srv_set_phone_book_rsp.conn_id, 
                    cmd_ptr->cmd_msg.cmd_pbap_srv_set_phone_book_rsp.status );
      status = bt_pf_pbap_srv_cmd_set_phone_book_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_pbap_srv_set_phone_book_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_SRV_PULL_VCARD_LISTING_RSP:
      BT_MSG_API( "BT PF PBAP SRV CMD RX: Pull vCard Listing Rsp AID %x CID %x Stat %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_list_rsp.conn_id, 
                    cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_list_rsp.status );
      status = bt_pf_pbap_srv_cmd_pull_vcard_listing_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_list_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_PBAP_SRV_PULL_VCARD_ENTRY_RSP:
      BT_MSG_API( "BT PF PBAP SRV CMD RX: Pull vCard Entry Rsp AID %x CID %x Stat %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_entry_rsp.conn_id, 
                    cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_entry_rsp.status );
      status = bt_pf_pbap_srv_cmd_pull_vcard_entry_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_pbap_srv_pull_vcard_entry_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    default:
      BT_ERR( "BT PF PBAP CMD RX: Unknown: %x",
              cmd_ptr->cmd_hdr.cmd_type, 0, 0 );
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
  }
} 
 
#endif /* FEATURE_BT_EXTPF_PBAP */
#endif /* FEATURE_BT */
