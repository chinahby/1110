/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H   S Y N C - M L   A B S T R A C T I O N   L A Y E R

GENERAL DESCRIPTION
  This module contains data and code for the SyncML abstraction layer 
  on top of the Bluetooth Generic Object Exchange Profile which are not
  specific to the client or server.
  
  Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfsml1.c#1 $ 

$Log: $

*    #12        28 Feb 2007            MH
* Auth indication modified to support full access 
*
*    #10-11    10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #9        23 Jun 2006            JH
* Modified to work with updated OBEX code which supports JSR82.
*
*    #8        19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #7         6 Jan 2005            JH
* Modified to avoid possible misaligned memory accesses.
*
*    #6         9 Aug 2004            JH
* Renamed bt_pf_sml_cmd_auth_srv_rsp and bt_pf_sml_cmd_auth_cli_rsp to
*  bt_pf_sml_cmd_auth_req and bt_pf_sml_cmd_auth_rsp respectively
*
*    #4        30 Apr 2004            JH
* Initial populated version.
*

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_SML

#include "btpfi.h" 
#include "btpfcmdi.h"
#include "bti.h"
#include "btmsg.h"
#include "btpfsmli.h"             
#include "syncml_common.c" 
          
                             
bt_app_id_type        bt_pf_sml_app_id = BT_PF_APP_ID_NULL;
          
void bt_pf_sml_con_cfm_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id,
                           OI_BOOL read_only,
                           OI_OBEX_HEADER_LIST *rsp_headers_ptr, /* ignored */
                           OI_STATUS status )
{
  bt_ev_msg_type    event;

  BT_MSG_DEBUG( "BT PF SML CON CFM CB: Conn: %x, App: %x, Status: %x", 
                conn_id, bt_pf_sml_app_id, status );
  event.ev_hdr.ev_type = BT_EV_PF_SML_CON_CFM;
  event.ev_hdr.bt_app_id = bt_pf_sml_app_id;

  event.ev_msg.ev_sml_con_cfm.conn_id = (bt_pf_sml_conn_id_type)conn_id;
  event.ev_msg.ev_sml_con_cfm.read_only = read_only;
  event.ev_msg.ev_sml_con_cfm.status = (bt_cmd_status_type)status;

  bt_ec_send_event( &event );
  return;
}

void bt_pf_sml_dcn_ind_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id,                           
                           OI_OBEX_HEADER_LIST *rsp_headers_ptr ) /* ignored */
{
  bt_ev_msg_type    event;

  BT_MSG_DEBUG( "BT PF SML CON CFM CB: Conn: %x, App: %x", 
                conn_id, bt_pf_sml_app_id, 0 );
  event.ev_hdr.ev_type = BT_EV_PF_SML_DCN_CFM;
  event.ev_hdr.bt_app_id = bt_pf_sml_app_id;

  event.ev_msg.ev_sml_dcn_cfm.conn_id = (bt_pf_sml_conn_id_type)conn_id;

  bt_ec_send_event( &event );
  return;
}

void bt_pf_sml_auth_ind_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id,
                                 OI_BOOL fullAccess,
                            OI_BOOL user_id_required )
{
  bt_ev_msg_type    event;

  BT_MSG_DEBUG( "BT PF SML AUTH IND CB: Conn: %x, App: %x", 
                conn_id, bt_pf_sml_app_id, 0 );
  event.ev_hdr.ev_type = BT_EV_PF_SML_AUTH_IND;
  event.ev_hdr.bt_app_id = bt_pf_sml_app_id;

  event.ev_msg.ev_sml_auth_ind.conn_id = (bt_pf_sml_conn_id_type)conn_id;
  event.ev_msg.ev_sml_auth_ind.user_id_required = user_id_required;

  bt_ec_send_event( &event );
  return;
}

void bt_pf_sml_get_rcv_data_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id,
                                OI_OBEX_HEADER_LIST *rsp_headers_ptr,
                                OI_STATUS status )
{
  bt_ev_msg_type    event;

  BT_MSG_DEBUG( "BT PF SML GET RCV DATA CB: Conn: %x, App: %x, Stat: %x",
                 conn_id, bt_pf_sml_app_id, status );

  event.ev_hdr.ev_type = BT_EV_PF_SML_GET_RCV_DATA;
  event.ev_hdr.bt_app_id = bt_pf_sml_app_id;

  event.ev_msg.ev_sml_get_rcv_data.conn_id = (bt_pf_sml_conn_id_type)conn_id;
  event.ev_msg.ev_sml_get_rcv_data.rsp_headers_cnt = rsp_headers_ptr->count;
  event.ev_msg.ev_sml_get_rcv_data.rsp_headers_ptr =
    (bt_pf_goep_obex_hdr_type*)rsp_headers_ptr->list;
  event.ev_msg.ev_sml_get_rcv_data.status = (bt_cmd_status_type)status;

  /* Trick the OI_FreeIf inside GetResponse into NOT freeing the header list */
  /* The application will need to free the list when done */
  rsp_headers_ptr->list = NULL;
  
  bt_ec_send_event( &event );
  return;
}

void bt_pf_sml_put_cfm_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id,
                           OI_OBEX_HEADER_LIST *rsp_headers_ptr,
                           OI_STATUS status )
{  
  bt_ev_msg_type    event;

  BT_MSG_DEBUG( "BT PF SML PUT CFM CB: Conn: %x, App: %x, Stat: %x",
                 conn_id, bt_pf_sml_app_id, status );

  event.ev_hdr.ev_type = BT_EV_PF_SML_PUT_CFM;
  event.ev_hdr.bt_app_id = bt_pf_sml_app_id;

  event.ev_msg.ev_sml_put_cfm.conn_id = (bt_pf_sml_conn_id_type)conn_id;
  event.ev_msg.ev_sml_put_cfm.rsp_headers_cnt = rsp_headers_ptr->count;
  event.ev_msg.ev_sml_put_cfm.rsp_headers_ptr =
    (bt_pf_goep_obex_hdr_type*)(rsp_headers_ptr->list);  
  event.ev_msg.ev_sml_put_cfm.status = (bt_cmd_status_type)status;

  /* Trick the OI_FreeIf inside PutResponse into NOT freeing the header list */
  /* The application will need to free the list when done */
  rsp_headers_ptr->list = NULL;

  bt_ec_send_event( &event );
  return;
}

bt_cmd_status_type bt_pf_sml_cmd_auth_req(
  bt_app_id_type               app_id,
  bt_pf_cmd_sml_auth_req_type* params_ptr )
{
  OI_STATUS status;
  char*     password_ptr;

  if (params_ptr->password_str[0] == '\0')
  {
    password_ptr = NULL;
  }
  else
  {
    password_ptr = (char*)params_ptr->password_str;
  }

  status = OI_OBEXSRV_AuthenticationResponse(
    (OI_OBEXSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    password_ptr,
    params_ptr->read_only );

  return(status);
}

bt_cmd_status_type bt_pf_sml_cmd_accept(
  bt_app_id_type             app_id,
  bt_pf_cmd_sml_accept_type* params_ptr )
{
  OI_STATUS status;

  status = OI_OBEXSRV_AcceptConnect(
    (OI_OBEXSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    params_ptr->accept, NULL, 
    (OI_STATUS)params_ptr->status );

  return(status);
}          

bt_cmd_status_type bt_pf_sml_cmd_auth_rsp(
  bt_app_id_type               app_id,
  bt_pf_cmd_sml_auth_rsp_type* params_ptr )
{
  OI_STATUS status;
  byte*     user_id_ptr;
  char*     password_ptr;

  if (params_ptr->user_id_len == '\0')
  {
    user_id_ptr = NULL;
  }
  else
  {
    user_id_ptr = (byte*)params_ptr->user_id;
  }

  if (params_ptr->password_str[0] == '\0')
  {
    password_ptr = NULL;
  }
  else
  {
    password_ptr = (char*)params_ptr->password_str;
  }

  status = OI_OBEXCLI_AuthenticationResponse( 
    (OI_OBEXCLI_CONNECTION_HANDLE)params_ptr->conn_id,
    user_id_ptr,
    params_ptr->user_id_len,
    password_ptr );

  return(status);
}

bt_cmd_status_type bt_pf_sml_cmd_connect(
  bt_app_id_type              app_id,
  bt_pf_cmd_sml_connect_type* params_ptr )
{
  OI_OBEXCLI_CONNECTION_HANDLE conn_id;
  OI_STATUS                    status;
  bt_bd_addr_type              bd_addr = params_ptr->bd_addr;
  
  if (bt_pf_sml_app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF SML CON: not reg, App: %x, Stat: %x",
            app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }
  if (bt_pf_sml_app_id != app_id)
  {
    BT_ERR( "BT PF SML CON: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = OI_SyncML_Connect(
    (OI_BD_ADDR*)&bd_addr,
    params_ptr->channel,
    (char*)params_ptr->target_uuid_str,
    bt_pf_sml_con_cfm_cb,
    bt_pf_sml_dcn_ind_cb,
    bt_pf_sml_auth_ind_cb,
    &conn_id );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF SML CON: OI_SyncML_Connect returns %x", 
                status, 0, 0 );
  }
  return(status);
}


bt_cmd_status_type bt_pf_sml_cmd_disconnect(
  bt_app_id_type                 app_id,
  bt_pf_cmd_sml_disconnect_type* params_ptr )
{                                      
  OI_STATUS                    status;
  
  if (bt_pf_sml_app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF SML DCN: not reg, App: %x, Stat: %x",
            app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }
  if (bt_pf_sml_app_id != app_id)
  {
    BT_ERR( "BT PF SML DCN: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = OI_SyncML_Disconnect( params_ptr->conn_id );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF SML DCN: OI_SyncML_Disconnect returns %x", 
                status, 0, 0 );
  }
  return(status);
}

bt_cmd_status_type bt_pf_sml_cmd_get(
  bt_app_id_type          app_id,
  bt_pf_cmd_sml_get_type* params_ptr )
{                                      
  OI_STATUS                    status;
  
  if (bt_pf_sml_app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF SML GET: not reg, App: %x, Stat: %x",
            app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }
  if (bt_pf_sml_app_id != app_id)
  {
    BT_ERR( "BT PF SML GET: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = OI_SyncML_Get(
    params_ptr->conn_id,
    bt_pf_sml_get_rcv_data_cb,
    (char*)params_ptr->type_str );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF SML GET: OI_SyncML_Get returns %x", 
                status, 0, 0 );
  }
  return(status);
}

bt_cmd_status_type bt_pf_sml_cmd_put(
  bt_app_id_type          app_id,
  bt_pf_cmd_sml_put_type* params_ptr )
{                             
  OI_STATUS                    status;
  uint16                       put_len;
  
  if (bt_pf_sml_app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF SML PUT: not reg, App: %x, Stat: %x",
            app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }
  if (bt_pf_sml_app_id != app_id)
  {
    BT_ERR( "BT PF SML PUT: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = OI_SyncML_Put(
    params_ptr->conn_id,
    bt_pf_sml_put_cfm_cb,
    (char*)params_ptr->type_str,
    params_ptr->data_ptr->data,
    params_ptr->data_ptr->len,
    &put_len,
    params_ptr->status );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF SML PUT: OI_SyncML_Put returns %x", 
                status, 0, 0 );
  }
  return(status);
}

bt_cmd_status_type bt_pf_sml_cmd_get_rsp(
  bt_app_id_type              app_id,
  bt_pf_cmd_sml_get_rsp_type* params_ptr )
{
  OI_STATUS                    status;
  bt_ev_msg_type               event;
  uint16                       get_rsp_len;
  
  if (bt_pf_sml_app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF SML GET RSP: not reg, App: %x, Stat: %x",
            app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }
  if (bt_pf_sml_app_id != app_id)
  {
    BT_ERR( "BT PF SML GET RSP: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = OI_SyncML_GetResponse(
    params_ptr->conn_id,
    params_ptr->data_ptr->data,
    params_ptr->data_ptr->len,
    &get_rsp_len,
    params_ptr->status );

  bt_ec_send_event( &event );
  if (status != OI_OK)
  {
    BT_ERR( "BT PF SML GET RSP: OI_SyncML_GetResponse returns %x", 
                status, 0, 0 );
  }
  return(status);    
}

bt_cmd_status_type bt_pf_sml_cmd_put_rsp(
  bt_app_id_type              app_id,
  bt_pf_cmd_sml_put_rsp_type* params_ptr )
{
  OI_STATUS                    status;
  
  if (bt_pf_sml_app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF SML PUT RSP: not reg, App: %x, Stat: %x",
            app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }
  if (bt_pf_sml_app_id != app_id)
  {
    BT_ERR( "BT PF SML PUT RSP: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = OI_SyncML_PutResponse(
    params_ptr->conn_id,
    params_ptr->status );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF SML PUT RSP: OI_SyncML_PutResponse returns %x", 
                status, 0, 0 );
  }
  return(status);
}


          
/*===========================================================================

FUNCTION
  bt_pf_sml_process_command    
   
DESCRIPTION
  Processes the given command by calling the underlying SML driver.
   
===========================================================================*/
void bt_pf_sml_process_command( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_status_type status;     

  switch ( cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_PF_CMD_SML_REGISTER_CLI:
      BT_MSG_DEBUG( "BT PF SML CMD RX: reg cli", 0, 0, 0 );
      status = bt_pf_sml_cmd_register_client(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_sml_register_cli) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_REGISTER_SRV:                        
      BT_MSG_DEBUG( "BT PF SML CMD RX: reg srv", 0, 0, 0 );
      status = bt_pf_sml_cmd_register_server(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_sml_register_srv) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_DEREGISTER_CLI:
      BT_MSG_DEBUG( "BT PF SML CMD RX: dereg cli", 0, 0, 0 );
      status = bt_pf_sml_cmd_deregister_client(
        cmd_ptr->cmd_hdr.app_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_DEREGISTER_SRV:
      BT_MSG_DEBUG( "BT PF SML CMD RX: dereg srv", 0, 0, 0 );
      status = bt_pf_sml_cmd_deregister_server(
        cmd_ptr->cmd_hdr.app_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_AUTH_REQ:
      BT_MSG_DEBUG( "BT PF SML CMD RX: auth req", 0, 0, 0 );
      status = bt_pf_sml_cmd_auth_req(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_sml_auth_req) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_ACCEPT:
      BT_MSG_DEBUG( "BT PF SML CMD RX: accept", 0, 0, 0 );
      status = bt_pf_sml_cmd_accept(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_sml_accept) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_AUTH_RSP:
      BT_MSG_DEBUG( "BT PF SML CMD RX: auth rsp", 0, 0, 0 );
      status = bt_pf_sml_cmd_auth_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_sml_auth_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_CONNECT:
      BT_MSG_DEBUG( "BT PF SML CMD RX: con", 0, 0, 0 );
      status = bt_pf_sml_cmd_connect( 
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_sml_connect) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_DISCONNECT:
      BT_MSG_DEBUG( "BT PF SML CMD RX: dcn", 0, 0, 0 );
      status = bt_pf_sml_cmd_disconnect( 
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_sml_disconnect) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_GET:
      BT_MSG_DEBUG( "BT PF SML CMD RX: get", 0, 0, 0 );
      status = bt_pf_sml_cmd_get( 
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_sml_get) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_PUT:
      BT_MSG_DEBUG( "BT PF SML CMD RX: put", 0, 0, 0 );
      status = bt_pf_sml_cmd_put( 
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_sml_put) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_GET_RSP:
      BT_MSG_DEBUG( "BT PF SML CMD RX: get rsp", 0, 0, 0 );
      status = bt_pf_sml_cmd_get_rsp( 
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_sml_get_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_SML_PUT_RSP:
      BT_MSG_DEBUG( "BT PF SML CMD RX: put rsp", 0, 0, 0 );
      status = bt_pf_sml_cmd_put_rsp( 
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_sml_put_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    default:
      BT_ERR( "BT PF SML CMD RX: Unknown: %x",
              cmd_ptr->cmd_hdr.cmd_type, 0, 0 );
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
  }
}


#endif /* FEATURE_BT_EXTPF_SML */
#endif /* FEATURE_BT */
