/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      B L U E T O O T H   B A S I C   P R I N T I N G   P R O F I L E

GENERAL DESCRIPTION
  This module contains the Bluetooth Basic Printing Profile data and code
  which are not client or server specific.

              Copyright (c) 2004-2006, 2008 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfbpp1.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $

 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2008-06-16   pr  Added cmd to tear down connection from printer 
                  to BPP server (object channel).
*
*    #8        12 Jun 2006            DSN
* Typecasting fixes for RVCT compiler.
*
*    #7        19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #6        29 Dec 2004            JH
* Added more details to debug prints.
*
*    #4        23 Apr 2004            JH
* Server register and deregister commands are now queued.
*
*    #3        14 Apr 2004            JH
* Initial working version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_BPP

#include "btpfi.h" 
#include "btpfcmdi.h"
#include "bti.h"
#include "btmsg.h"
#include "btpfbppi.h"             
#include "oi_bpp_private.c" 

/*===========================================================================

FUNCTION
  bt_pf_bpp_process_command    
   
DESCRIPTION
  Processes the given command by calling the underlying OI BPP driver.
   
===========================================================================*/
void bt_pf_bpp_process_command( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_status_type status;     

  switch ( cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_PF_CMD_BPP_SNDR_CONNECT:
      BT_MSG_DEBUG( "BT PF BPP SNDR CMD RX: con, target %x", 
                    cmd_ptr->cmd_msg.cmd_bpp_sndr_connect.target, 0, 0 );
      status = bt_pf_bpp_sndr_cmd_connect(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bpp_sndr_connect) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BPP_SNDR_DISCONNECT:
      BT_MSG_DEBUG( "BT PF BPP SNDR CMD RX: dcn, cid %x", 
                    cmd_ptr->cmd_msg.cmd_bpp_sndr_disconnect.conn_id, 0, 0 );
      status = bt_pf_bpp_sndr_cmd_disconnect(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bpp_sndr_disconnect) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BPP_SNDR_AUTH_RSP:
      BT_MSG_DEBUG( "BT PF BPP SNDR CMD RX: auth rsp, target %x", 
                    cmd_ptr->cmd_msg.cmd_bpp_sndr_auth_rsp.target, 0, 0 );
      status = bt_pf_bpp_sndr_cmd_auth_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bpp_sndr_auth_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BPP_SNDR_SEND_FILE:
      BT_MSG_DEBUG( "BT PF BPP SNDR CMD RX: snd file, cid %x", 
                    cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file.conn_id, 0, 0 );
      status = bt_pf_bpp_sndr_cmd_send_file(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bpp_sndr_send_file) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BPP_SNDR_SEND_REFERENCE:
      BT_MSG_DEBUG( "BT PF BPP SNDR CMD RX: snd ref, cid %x",  
                    cmd_ptr->cmd_msg.cmd_bpp_sndr_send_reference.conn_id, 
                    0, 0 );
      status = bt_pf_bpp_sndr_cmd_send_reference(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bpp_sndr_send_reference) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BPP_SNDR_SOAP_REQ:
      BT_MSG_DEBUG( "BT PF BPP SNDR CMD RX: soap req, cid %x",  
                    cmd_ptr->cmd_msg.cmd_bpp_sndr_soap_req.conn_id, 0, 0 );
      status = bt_pf_bpp_sndr_cmd_soap_request(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bpp_sndr_soap_req) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BPP_SNDR_REGISTER:
      BT_MSG_DEBUG( "BT PF BPP SNDR CMD RX: reg", 0, 0, 0 );
      status = bt_pf_bpp_sndr_cmd_register(
        cmd_ptr->cmd_hdr.app_id,
        (char *)cmd_ptr->cmd_msg.cmd_bpp_sndr_register.srv_name_str );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BPP_SNDR_DEREGISTER:
      BT_MSG_DEBUG( "BT PF BPP SNDR CMD RX: dereg", 0, 0, 0 );
      status = bt_pf_bpp_sndr_cmd_deregister(
        cmd_ptr->cmd_hdr.app_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BPP_SNDR_GET_REF_OBJ_RSP:
      BT_MSG_DEBUG( "BT PF BPP SNDR CMD RX: get ref obj rsp, cid %x",  
                    cmd_ptr->cmd_msg.cmd_bpp_sndr_get_ref_obj_rsp.conn_id, 
                    0, 0 );
      status = bt_pf_bpp_sndr_cmd_send_ref_obj_response(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bpp_sndr_get_ref_obj_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BPP_SNDR_FORCE_DISCONNECT:
      BT_MSG_DEBUG( "BT PF BPP SNDR CMD RX: forc dcn", 0, 0, 0 );
      status = bt_pf_bpp_sndr_cmd_force_disconnect(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bpp_sndr_force_disconnect));
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    default:
      BT_ERR( "BT PF BPP CMD RX: Unknown: %x",
              cmd_ptr->cmd_hdr.cmd_type, 0, 0 );
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
  }
}

#endif /* FEATURE_BT_EXTPF_BPP */
#endif /* FEATURE_BT */
