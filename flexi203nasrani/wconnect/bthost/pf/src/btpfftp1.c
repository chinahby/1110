/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      B L U E T O O T H   F I L E   T R A N S F E R   P R O F I L E

GENERAL DESCRIPTION
  This module contains the Bluetooth File Transfer Profile data and code
  which are not client or server specific.

Copyright (c) 2004 - 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfftp1.c#1 $ 

$Log: $
*
*    #13      28 Nov 2006             RP
* Updated the Debug messages to print the size of the file.
*    
*    #12      18 Oct 2006             PR
* Included files for unicode support while merging OI Code v3.0.14
*
*    #11      12 Jun 2006             DSN
* Typecasting fixes for RVCT compiler.
*
*    #10      12 Apr 2005             JH
* Converted file operation done and abort functions into queued commands.
*
*    #9       19 Jan 2005             JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #8        06 Jan 2005            JH
* Modified to avoid possible misaligned memory accesses.
*
*    #7        14 May 2004            JH
* Added FTP client browse support
*
*    #5        23 Apr 2004            JH
* Server register and deregister commands are now queued.
*
*    #4        14 Apr 2004            JH
* Made changes related to the replacement of bt_pf_unicode_str_type command 
* parameters with uint16* type
*
*    #2        25 Feb 2004            JH
* Renamed btpfftp.h btpfftpi.h
*

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_FTP

#include "bt.h"
#include "btpfi.h"
#include "btpfcmdi.h"
#include "bti.h"
#include "btmsg.h"
#include "btpfftpi.h"
#include "oi_ftp_client.h"
#include "oi_ftp_server.h"
#include "ftp_utils.c"
#undef __OI_MODULE__
#include "oi_unicode.c"
#include "ConvertUTF.c"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
extern bt_app_id_type              bt_pf_ftp_cli_app_id;
extern bt_pf_ftp_cli_conn_id_type  bt_pf_ftp_cli_conn_id;

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_ftp_cmn_process_command    
   
DESCRIPTION
  Processes the given command by calling the underlying OI FTP driver.
   
===========================================================================*/
void bt_pf_ftp_process_command( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_status_type status;
  OI_OBEX_UNICODE    local_name;
  uint16             local_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  OI_OBEX_UNICODE    remote_name;
  OI_OBEX_UNICODE*   remote_name_ptr;
  uint16             remote_name_str[BT_PF_MAX_FILENAME_LEN + 1];
  bt_bd_addr_type    bd_addr;

  switch ( cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_PF_CMD_FTP_CLI_CONNECT:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Connect", 0, 0, 0 );
      bd_addr = cmd_ptr->cmd_msg.cmd_ftp_cli_connect.bd_addr;
      status = bt_pf_ftp_cli_cmd_connect(
        cmd_ptr->cmd_hdr.app_id,
        &bd_addr,
        cmd_ptr->cmd_msg.cmd_ftp_cli_connect.channel );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_DISCONNECT:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Disconnect", 0, 0, 0 );
      status = OI_FTPClient_Disconnect( 
        (OI_FTP_CLIENT_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_ftp_cli_disconnect.conn_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_AUTH_RSP:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Auth Rsp", 0, 0, 0 );
      status = bt_pf_ftp_cli_cmd_auth_rsp( 
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_ftp_cli_auth_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_PUT:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Put", 0, 0, 0 );
      /* name is copied to avoid misaligned access when str pointer is  not 
         aligned */
      memcpy( (void*)local_name_str,
              (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_put.local_name_str,
              cmd_ptr->cmd_msg.cmd_ftp_cli_put.local_name_len * 
                sizeof( uint16 ) );
      local_name.len = cmd_ptr->cmd_msg.cmd_ftp_cli_put.local_name_len;
      local_name.str = local_name_str;
      if (cmd_ptr->cmd_msg.cmd_ftp_cli_put.remote_name_len == 0)
      {
        remote_name_ptr = NULL;
      }
      else
      {
        memcpy( (void*)remote_name_str,
                (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_put.remote_name_str,
                cmd_ptr->cmd_msg.cmd_ftp_cli_put.remote_name_len * 
                  sizeof( uint16 ) );
        remote_name.len = cmd_ptr->cmd_msg.cmd_ftp_cli_put.remote_name_len;
        remote_name.str = remote_name_str;
        remote_name_ptr = &remote_name;
      }
      status = OI_FTPClient_Put(
        (OI_FTP_CLIENT_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_ftp_cli_put.conn_id,
        &local_name,
        remote_name_ptr );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_DELETE:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Delete", 0, 0, 0 );
      /* name is copied to avoid misaligned access when str pointer is  not 
         aligned */
      memcpy( (void*)remote_name_str,
              (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_delete.name_str,
              cmd_ptr->cmd_msg.cmd_ftp_cli_delete.name_len * sizeof( uint16 ) );
      remote_name.len = cmd_ptr->cmd_msg.cmd_ftp_cli_delete.name_len;
      remote_name.str = remote_name_str;
      status = OI_FTPClient_Delete(
        (OI_FTP_CLIENT_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_ftp_cli_delete.conn_id,
        &remote_name );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_GET:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Get", 0, 0, 0 );
      /* name is copied to avoid misaligned access when str pointer is  not 
         aligned */
      memcpy( (void*)local_name_str,
              (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_get.local_name_str,
              cmd_ptr->cmd_msg.cmd_ftp_cli_get.local_name_len * 
                sizeof( uint16 ) );
      local_name.len = cmd_ptr->cmd_msg.cmd_ftp_cli_get.local_name_len;
      local_name.str = local_name_str;
      if (cmd_ptr->cmd_msg.cmd_ftp_cli_get.remote_name_len == 0)
      {
        remote_name_ptr = NULL;
      }
      else
      {
        memcpy( (void*)remote_name_str,
                (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_get.remote_name_str,
                cmd_ptr->cmd_msg.cmd_ftp_cli_get.remote_name_len * 
                  sizeof( uint16 ) );
        remote_name.len = cmd_ptr->cmd_msg.cmd_ftp_cli_get.remote_name_len;
        remote_name.str = remote_name_str;
        remote_name_ptr = &remote_name;
      }
      status = OI_FTPClient_Get(
        (OI_FTP_CLIENT_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_ftp_cli_get.conn_id,
        &local_name,
        remote_name_ptr );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_SET_PATH:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Set Path", 0, 0, 0 );
      if (cmd_ptr->cmd_msg.cmd_ftp_cli_set_path.folder_len == 0)
      {
        remote_name_ptr = NULL;
      }
      else
      {
        /* name is copied to avoid misaligned access when str pointer is  not 
           aligned */
        memcpy( (void*)remote_name_str,
                (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_set_path.folder_str,
                cmd_ptr->cmd_msg.cmd_ftp_cli_set_path.folder_len * 
                  sizeof( uint16 ) );
        remote_name.len = cmd_ptr->cmd_msg.cmd_ftp_cli_set_path.folder_len;
        remote_name.str = remote_name_str;
        remote_name_ptr = &remote_name;
      }
      status = OI_FTPClient_SetPath(
        (OI_FTP_CLIENT_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_ftp_cli_set_path.conn_id,
        remote_name_ptr,
        (OI_FTP_CLIENT_SETPATH_CONTROL)
          cmd_ptr->cmd_msg.cmd_ftp_cli_set_path.set_path_ctrl );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_NEW_SUBFOLDER:
      BT_MSG_API( "BT PF FTP CLI CMD RX: New Subfolder", 0, 0, 0 );
      /* name is copied to avoid misaligned access when str pointer is  not 
         aligned */
      memcpy( (void*)remote_name_str,
              (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_new_subfolder.folder_str,
              cmd_ptr->cmd_msg.cmd_ftp_cli_new_subfolder.folder_len * 
                sizeof( uint16 ) ); 
      remote_name.len = cmd_ptr->cmd_msg.cmd_ftp_cli_new_subfolder.folder_len;
      remote_name.str = remote_name_str;
      status = OI_FTPClient_NewSubfolder(
        (OI_FTP_CLIENT_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_ftp_cli_new_subfolder.conn_id,
        &remote_name );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_GET_FOLDER_LISTING:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Get Folder Listing", 0, 0, 0 );
      if (cmd_ptr->cmd_msg.cmd_ftp_cli_get_folder_listing.folder_len == 0)
      {
        remote_name_ptr = NULL;
      }
      else
      {
        /* name is copied to avoid misaligned access when str pointer is  not 
           aligned */
        memcpy( (void*)remote_name_str,
                (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_get_folder_listing.
                  folder_str,
                cmd_ptr->cmd_msg.cmd_ftp_cli_get_folder_listing.
                  folder_len * sizeof( uint16 ) );
        remote_name.len = cmd_ptr->
                            cmd_msg.cmd_ftp_cli_get_folder_listing.folder_len;
        remote_name.str = remote_name_str;
        remote_name_ptr = &remote_name;
      }
      status = OI_FTPClient_GetFolderListing(
        (OI_FTP_CLIENT_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_ftp_cli_get_folder_listing.conn_id,
        remote_name_ptr,
        bt_pf_ftp_folder_list_cb );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_TREE_PUT:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Tree Put", 0, 0, 0 );
      /* name is copied to avoid misaligned access when str pointer is  not 
         aligned */
      memcpy( (void*)local_name_str,
              (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.local_name_str,
              cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.local_name_len * 
                sizeof( uint16 ) );
      local_name.len = cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.local_name_len;
      local_name.str = local_name_str;
      if (cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.remote_name_len == 0)
      {
        remote_name_ptr = NULL;
      }
      else
      {
        memcpy( (void*)remote_name_str,
                (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.remote_name_str,
                cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.remote_name_len * 
                  sizeof( uint16 ) );
        remote_name.len = cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.remote_name_len;
        remote_name.str = remote_name_str;
        remote_name_ptr = &remote_name;
      }
      status = OI_FTPClient_TreePut(
        (OI_FTP_CLIENT_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_ftp_cli_tree_put.conn_id,
        &local_name,
        remote_name_ptr );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_TREE_GET:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Tree Get", 0, 0, 0 );
      /* name is copied to avoid misaligned access when str pointer is  not 
         aligned */
      memcpy( (void*)local_name_str,
              (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.local_name_str,
              cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.local_name_len * 
                sizeof( uint16 ) );
      local_name.len = cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.local_name_len;
      local_name.str = local_name_str;
      if (cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.remote_name_len == 0)
      {
        remote_name_ptr = NULL;
      }
      else
      {
        memcpy( (void*)remote_name_str,
                (void*)cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.remote_name_str,
                cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.remote_name_len * 
                  sizeof( uint16 ) );
        remote_name.len = cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.remote_name_len;
        remote_name.str = remote_name_str;
        remote_name_ptr = &remote_name;
      }
      status = OI_FTPClient_TreeGet(
        (OI_FTP_CLIENT_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_ftp_cli_tree_get.conn_id,
        &local_name,
        remote_name_ptr );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_BROWSE_DONE:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Browse Done AID %x CID %x Stat %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_cli_browse_done.conn_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_cli_browse_done.status );
      status = bt_pf_ftp_cli_cmd_browse_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_cli_browse_done.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_cli_browse_done.handle,
        cmd_ptr->cmd_msg.cmd_ftp_cli_browse_done.size,
        cmd_ptr->cmd_msg.cmd_ftp_cli_browse_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_OPEN_DONE:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Open Done AID|CID 0x%04x Stat %x Size 0x%x", 
                    (cmd_ptr->cmd_hdr.app_id << 16 | 
                    cmd_ptr->cmd_msg.cmd_ftp_cli_open_done.conn_id), 
                    cmd_ptr->cmd_msg.cmd_ftp_cli_open_done.status,
                    cmd_ptr->cmd_msg.cmd_ftp_cli_open_done.size );
      status = bt_pf_ftp_cli_cmd_open_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_cli_open_done.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_cli_open_done.handle,
        cmd_ptr->cmd_msg.cmd_ftp_cli_open_done.size,
        cmd_ptr->cmd_msg.cmd_ftp_cli_open_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_READ_DONE:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Read Done AID|CID 0x%04x Stat %x Len 0x%x", 
                    (cmd_ptr->cmd_hdr.app_id << 16 | 
                    cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.conn_id),
                    cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.status,
                    cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.len );
      status = bt_pf_ftp_cli_cmd_read_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.handle,
        cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.data_ptr,
        cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.len,
        cmd_ptr->cmd_msg.cmd_ftp_cli_read_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_WRITE_DONE:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Write Done AID %x CID %x Stat %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_cli_write_done.conn_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_cli_write_done.status );
      status = bt_pf_ftp_cli_cmd_write_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_cli_write_done.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_cli_write_done.handle,
        cmd_ptr->cmd_msg.cmd_ftp_cli_write_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_SET_FOLDER_DONE:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Set Folder Done AID %x CID %x Stat %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_cli_set_folder_done.conn_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_cli_set_folder_done.status );
      status = bt_pf_ftp_cli_cmd_set_folder_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_cli_set_folder_done.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_cli_set_folder_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_CLI_ABORT:
      BT_MSG_API( "BT PF FTP CLI CMD RX: Abort AID %x CID %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_cli_abort.conn_id, 
                    0 );
      status = bt_pf_ftp_cli_cmd_abort(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_cli_abort.conn_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_SRV_REGISTER:
      BT_MSG_API( "BT PF FTP SRV CMD RX: Reg", 0, 0, 0 );
      status = bt_pf_ftp_srv_cmd_register(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_register.auth,
        cmd_ptr->cmd_msg.cmd_ftp_srv_register.read_only,
        cmd_ptr->cmd_msg.cmd_ftp_srv_register.allow_browse,
        (char *)cmd_ptr->cmd_msg.cmd_ftp_srv_register.srv_name_str );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_SRV_DEREGISTER:
      BT_MSG_API( "BT PF FTP SRV CMD RX: Dereg", 0, 0, 0 );
      status = bt_pf_ftp_srv_cmd_deregister( cmd_ptr->cmd_hdr.app_id );  
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_SRV_AUTH_RSP:
      BT_MSG_API( "BT PF FTP SRV CMD RX: Auth Rsp", 0, 0, 0 );
      status = OI_FTPServer_AuthenticationRsp( 
        (OI_FTP_SERVER_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_ftp_srv_auth_rsp.conn_id,
        (OI_CHAR*)cmd_ptr->cmd_msg.cmd_ftp_srv_auth_rsp.password_str,
        cmd_ptr->cmd_msg.cmd_ftp_srv_auth_rsp.read_only,
        cmd_ptr->cmd_msg.cmd_ftp_srv_auth_rsp.allow_browse );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_SRV_ACCEPT:
      BT_MSG_API( "BT PF FTP SRV CMD RX: Accept", 0, 0, 0 );
      /* name is copied to avoid misaligned access when str pointer is  not 
         aligned */
      memcpy( (void*)remote_name_str,
              (void*)cmd_ptr->cmd_msg.cmd_ftp_srv_accept.root_folder_str,
              cmd_ptr->cmd_msg.cmd_ftp_srv_accept.root_folder_len * 
                sizeof( uint16 ) );
      remote_name.len = cmd_ptr->cmd_msg.cmd_ftp_srv_accept.root_folder_len;
      remote_name.str = remote_name_str;
      status = OI_FTPServer_AcceptConnection( 
        (OI_FTP_SERVER_CONNECTION_HANDLE)
        cmd_ptr->cmd_msg.cmd_ftp_srv_accept.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_accept.accept,
        &remote_name );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_SRV_FORCE_DCN:
      BT_MSG_API( "BT PF FTP SRV CMD RX: Force Disconnect", 0, 0, 0 );
      status = OI_FTPServer_ForceDisconnect( 
        (OI_FTP_SERVER_CONNECTION_HANDLE)
        cmd_ptr->cmd_msg.cmd_ftp_srv_force_dcn.conn_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break; 

    case BT_PF_CMD_FTP_SRV_BROWSE_DONE:
      BT_MSG_API( "BT PF FTP SRV CMD RX: Browse Done AID|CID 0x%04x Stat %x Size 0x%x", 
                    (cmd_ptr->cmd_hdr.app_id << 16|
                    cmd_ptr->cmd_msg.cmd_ftp_srv_browse_done.conn_id), 
                    cmd_ptr->cmd_msg.cmd_ftp_srv_browse_done.status,
                    cmd_ptr->cmd_msg.cmd_ftp_srv_browse_done.size );
      status = bt_pf_ftp_srv_cmd_browse_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_browse_done.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_browse_done.handle,
        cmd_ptr->cmd_msg.cmd_ftp_srv_browse_done.size,
        cmd_ptr->cmd_msg.cmd_ftp_srv_browse_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_SRV_OPEN_DONE:
      BT_MSG_API( "BT PF FTP SRV CMD RX: Open Done AID|CID 0x%04x Stat %x Size 0x%x", 
                    (cmd_ptr->cmd_hdr.app_id << 16 | 
                    cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.conn_id), 
                    cmd_ptr->cmd_msg.cmd_ftp_srv_open_done.status,
                    cmd_ptr->cmd_msg.cmd_ftp_srv_open_done.size );
      status = bt_pf_ftp_srv_cmd_open_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_open_done.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_open_done.handle,
        cmd_ptr->cmd_msg.cmd_ftp_srv_open_done.size,
        cmd_ptr->cmd_msg.cmd_ftp_srv_open_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_SRV_READ_DONE:
      BT_MSG_API( "BT PF FTP SRV CMD RX: Read Done AID|CID 0x%04x Stat %x Len 0x%x", 
                    (cmd_ptr->cmd_hdr.app_id << 16 | 
                    cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.conn_id), 
                    cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.status,
                    cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.len );
      status = bt_pf_ftp_srv_cmd_read_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.handle,
        cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.data_ptr,
        cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.len,
        cmd_ptr->cmd_msg.cmd_ftp_srv_read_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_SRV_WRITE_DONE:
      BT_MSG_API( "BT PF FTP SRV CMD RX: Write Done AID %x CID %x Stat %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_srv_write_done.conn_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_srv_write_done.status );
      status = bt_pf_ftp_srv_cmd_write_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_write_done.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_write_done.handle,
        cmd_ptr->cmd_msg.cmd_ftp_srv_write_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_SRV_SET_FOLDER_DONE:
      BT_MSG_API( "BT PF FTP SRV CMD RX: Set Folder Done AID %x CID %x Stat %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_srv_set_folder_done.conn_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_srv_set_folder_done.status );
      status = bt_pf_ftp_srv_cmd_set_folder_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_set_folder_done.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_set_folder_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_FTP_SRV_DELETE_DONE:
      BT_MSG_API( "BT PF FTP SRV CMD RX: Delete Done AID %x CID %x Stat %x", 
                    cmd_ptr->cmd_hdr.app_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_srv_delete_done.conn_id, 
                    cmd_ptr->cmd_msg.cmd_ftp_srv_delete_done.status );
      status = bt_pf_ftp_srv_cmd_delete_done(
        cmd_ptr->cmd_hdr.app_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_delete_done.conn_id,
        cmd_ptr->cmd_msg.cmd_ftp_srv_delete_done.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    default:
      BT_ERR( "BT PF FTP CMD RX: Unknown: %x",
              cmd_ptr->cmd_hdr.cmd_type, 0, 0 );
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
  }
} 
 
#endif /* FEATURE_BT_EXTPF_FTP */
#endif /* FEATURE_BT */
