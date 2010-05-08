/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H   P H O N E B O O K   A C C E S S   P R O F I L E   
                                C L I E N T

GENERAL DESCRIPTION
  This module contains the Bluetooth Phone Book Access Profile client 
  data and code.

              Copyright (c) 2006-2008 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary 

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfpbap2.c#1 $ 
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-07-24   gs  Fixed Klocwork error.
*
*    #3        21 May 2007            SSK
* Fix compiler warnings.
*
*    #2        15 Jun 2006            GS
* Added debug messages to print app parameter values.
*
*    #1        08 Feb 2006            GS
* Initial version
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
#include "bti.h"
#include "btmsg.h"
#include "btpfpbapi.h"
#include "btsd.h"
#include "rex.h"
#include "pbap_spec.h"
#include "pbap_cli.c"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/ 

#define BT_PF_PBAP_VERSION_NUMBER 0x0100

bt_app_id_type               bt_pf_pbap_cli_app_id  = BT_PF_APP_ID_NULL;
bt_pf_pbap_cli_conn_id_type  bt_pf_pbap_cli_conn_id = BT_PF_PBAP_NO_CONN_ID;

LOCAL PBAP_PULL_PHONE_BOOK_PARAMS     bt_pf_pbap_cli_pull_phone_book_params;
LOCAL PBAP_PULL_VCARD_LISTING_PARAMS  bt_pf_pbap_cli_pull_vcard_listing_params;
LOCAL PBAP_PULL_VCARD_ENTRY_PARAMS    bt_pf_pbap_cli_pull_vcard_entry_params;

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_connect_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the connect command is received.

===========================================================================*/
LOCAL void bt_pf_pbap_cli_connect_cfm_cb( 
  PBAPCLI_HANDLE conn_id, 
  OI_STATUS      status )
{
  bt_ev_msg_type event;                   

  if (bt_pf_pbap_cli_conn_id == (bt_pf_pbap_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF PBAP CLI CB RX: con, conn: %x stat %x", 
                  conn_id, status, 0 );                         
    event.ev_hdr.bt_app_id = bt_pf_pbap_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_CLI_CON_CFM;
    event.ev_msg.ev_pbap_cli_con_cfm.conn_id = 
      (bt_pf_pbap_cli_conn_id_type)conn_id;
    event.ev_msg.ev_pbap_cli_con_cfm.status = 
      (bt_cmd_status_type)status;
    bt_ec_send_event( &event );
    if (status != OI_OK)
    {
      bt_pf_pbap_cli_conn_id = BT_PF_PBAP_NO_CONN_ID;
    }
  }
  else
  {
    BT_ERR( "BT PF PBAP CB RX: con, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}
  
/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_disconnect_ind_cb

DESCRIPTION
  Callback function invoked to indicate a disconnection from remote PBAP 
  server.

===========================================================================*/
LOCAL void bt_pf_pbap_cli_disconnect_ind_cb( PBAPCLI_HANDLE conn_id )
{
  bt_ev_msg_type event;                   

  if (bt_pf_pbap_cli_conn_id == (bt_pf_pbap_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF PBAP CLI CB RX: dcn, conn: %x", conn_id, 0, 0 ); 

    event.ev_hdr.bt_app_id = bt_pf_pbap_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_CLI_DCN_CFM;
    event.ev_msg.ev_pbap_cli_dcn_cfm.conn_id = 
      (bt_pf_pbap_cli_conn_id_type)conn_id;

    bt_pf_pbap_cli_conn_id = BT_PF_PBAP_NO_CONN_ID;

    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF PBAP CLI CB RX: dcn, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_auth_req_cb

DESCRIPTION
  Callback function invoked to request for authentication prior to
  connection setup.

===========================================================================*/
LOCAL void bt_pf_pbap_cli_auth_req_cb( 
  PBAPCLI_HANDLE conn_id,
  boolean        user_id_required )
{
  bt_ev_msg_type event;                   

  if (bt_pf_pbap_cli_conn_id == (bt_pf_pbap_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF PBAP CLI CB RX: auth, conn: %x", conn_id, 0, 0 );                         
    event.ev_hdr.bt_app_id = bt_pf_pbap_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_CLI_AUTH_REQ;
    event.ev_msg.ev_pbap_cli_auth_req.conn_id = 
      (bt_pf_pbap_cli_conn_id_type)conn_id;
    event.ev_msg.ev_pbap_cli_auth_req.user_id_required = user_id_required;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF PBAP CLI CB RX: auth, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_pull_phone_book_cfm_cb

DESCRIPTION
  Callback function invoked when a response to PullPhoneBook 
  command is received.

===========================================================================*/
LOCAL void bt_pf_pbap_cli_pull_phone_book_cfm_cb( 
  PBAPCLI_HANDLE              conn_id,
  PBAP_PULL_PHONE_BOOK_PARAMS *params_ptr,
  OI_STATUS                   result )
{
  bt_ev_msg_type event;                   

  if (bt_pf_pbap_cli_conn_id == (bt_pf_pbap_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF PBAP CLI CB RX: PullPhoneBook Cfm, conn: %x stat %x", 
                  conn_id, result, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_pbap_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_CLI_PULL_PHONE_BOOK_CFM;
    event.ev_msg.ev_pbap_cli_pull_phone_book_cfm.conn_id = 
      (bt_pf_pbap_cli_conn_id_type)conn_id;
    event.ev_msg.ev_pbap_cli_pull_phone_book_cfm.final = 
      params_ptr->response.final;
    event.ev_msg.ev_pbap_cli_pull_phone_book_cfm.status = 
      (bt_cmd_status_type)result;

    if ( params_ptr->response.flags & PBAP_PARAM_PHONE_BOOK_SIZE ) 
    {
      event.ev_msg.ev_pbap_cli_pull_phone_book_cfm.phone_book_size_ptr = 
        &(params_ptr->response.phoneBookSize);
    }
    else
    {
      event.ev_msg.ev_pbap_cli_pull_phone_book_cfm.phone_book_size_ptr = 
        NULL;
    }

    if ( params_ptr->response.flags & PBAP_PARAM_NEW_MISSED_CALLS ) 
    {
      event.ev_msg.ev_pbap_cli_pull_phone_book_cfm.new_missed_calls_ptr = 
        &(params_ptr->response.missedCalls);
    }
    else
    {
      event.ev_msg.ev_pbap_cli_pull_phone_book_cfm.new_missed_calls_ptr = 
        NULL;
    }
    
    if (params_ptr->response.data == NULL)
    {
      event.ev_msg.ev_pbap_cli_pull_phone_book_cfm.data_ptr = NULL;
      event.ev_msg.ev_pbap_cli_pull_phone_book_cfm.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_pbap_cli_pull_phone_book_cfm.data_ptr = 
        params_ptr->response.data->data;
      event.ev_msg.ev_pbap_cli_pull_phone_book_cfm.data_len = 
        params_ptr->response.data->len;
    }

    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF PBAP CLI CB RX: PullPhoneBook cfm, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_set_phone_book_cfm_cb

DESCRIPTION
  Callback function invoked when a response to SetPhoneBook 
  command is received.

===========================================================================*/
LOCAL void bt_pf_pbap_cli_set_phone_book_cfm_cb( 
  PBAPCLI_HANDLE              conn_id,
  OI_STATUS                   result )
{
  bt_ev_msg_type event;                   

  if (bt_pf_pbap_cli_conn_id == (bt_pf_pbap_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF PBAP CLI CB RX: SetPhoneBook Cfm, conn: %x stat %x", 
                  conn_id, result, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_pbap_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_CLI_SET_PHONE_BOOK_CFM;
    event.ev_msg.ev_pbap_cli_set_phone_book_cfm.conn_id = 
      (bt_pf_pbap_cli_conn_id_type)conn_id;
    event.ev_msg.ev_pbap_cli_set_phone_book_cfm.status = 
      (bt_cmd_status_type)result;

    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF PBAP CLI CB RX: SetPhoneBook cfm, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_pull_vcard_listing_cfm_cb

DESCRIPTION
  Callback function invoked when a response to PullvCardListing 
  command is received.

===========================================================================*/
LOCAL void bt_pf_pbap_cli_pull_vcard_listing_cfm_cb( 
  PBAPCLI_HANDLE                 conn_id,
  PBAP_PULL_VCARD_LISTING_PARAMS *params_ptr,
  OI_STATUS                      result )
{
  bt_ev_msg_type event;                   

  if (bt_pf_pbap_cli_conn_id == (bt_pf_pbap_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF PBAP CLI CB RX: PullvCardListing Cfm, conn: %x stat %x", 
                  conn_id, result, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_pbap_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_CLI_PULL_VCARD_LISTING_CFM;
    event.ev_msg.ev_pbap_cli_pull_vcard_listing_cfm.conn_id = 
      (bt_pf_pbap_cli_conn_id_type)conn_id;
    event.ev_msg.ev_pbap_cli_pull_vcard_listing_cfm.final = 
      params_ptr->response.final;
    event.ev_msg.ev_pbap_cli_pull_vcard_listing_cfm.status = 
      (bt_cmd_status_type)result;

    if ( params_ptr->response.flags & PBAP_PARAM_PHONE_BOOK_SIZE ) 
    {
      event.ev_msg.ev_pbap_cli_pull_vcard_listing_cfm.phone_book_size_ptr = 
        &(params_ptr->response.phoneBookSize);
    }
    else
    {
      event.ev_msg.ev_pbap_cli_pull_vcard_listing_cfm.phone_book_size_ptr = 
        NULL;
    }

    if ( params_ptr->response.flags & PBAP_PARAM_NEW_MISSED_CALLS ) 
    {
      event.ev_msg.ev_pbap_cli_pull_vcard_listing_cfm.new_missed_calls_ptr = 
        &(params_ptr->response.missedCalls);
    }
    else
    {
      event.ev_msg.ev_pbap_cli_pull_vcard_listing_cfm.new_missed_calls_ptr = 
        NULL;
    }
    
    if (params_ptr->response.data == NULL)
    {
      event.ev_msg.ev_pbap_cli_pull_vcard_listing_cfm.data_ptr = NULL;
      event.ev_msg.ev_pbap_cli_pull_vcard_listing_cfm.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_pbap_cli_pull_vcard_listing_cfm.data_ptr = 
        params_ptr->response.data->data;
      event.ev_msg.ev_pbap_cli_pull_vcard_listing_cfm.data_len = 
        params_ptr->response.data->len;
    }

    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF PBAP CLI CB RX: PullvCardListing cfm, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_pull_vcard_entry_cfm_cb

DESCRIPTION
  Callback function invoked when a response to PullvCardEntry
  command is received.

===========================================================================*/
LOCAL void bt_pf_pbap_cli_pull_vcard_entry_cfm_cb( 
  PBAPCLI_HANDLE                 conn_id,
  PBAP_PULL_VCARD_ENTRY_PARAMS   *params_ptr,
  OI_STATUS                      result )
{
  bt_ev_msg_type event;                   

  if (bt_pf_pbap_cli_conn_id == (bt_pf_pbap_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF PBAP CLI CB RX: PullvCardEntry Cfm, conn: %x stat %x", 
                  conn_id, result, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_pbap_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_CLI_PULL_VCARD_ENTRY_CFM;
    event.ev_msg.ev_pbap_cli_pull_vcard_entry_cfm.conn_id = 
      (bt_pf_pbap_cli_conn_id_type)conn_id;
    event.ev_msg.ev_pbap_cli_pull_vcard_entry_cfm.final = 
      params_ptr->response.final;
    event.ev_msg.ev_pbap_cli_pull_vcard_entry_cfm.status = 
      (bt_cmd_status_type)result;
    
    if (params_ptr->response.data == NULL)
    {
      event.ev_msg.ev_pbap_cli_pull_vcard_entry_cfm.data_ptr = NULL;
      event.ev_msg.ev_pbap_cli_pull_vcard_entry_cfm.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_pbap_cli_pull_vcard_entry_cfm.data_ptr = 
        params_ptr->response.data->data;
      event.ev_msg.ev_pbap_cli_pull_vcard_entry_cfm.data_len = 
        params_ptr->response.data->len;
    }

    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF PBAP CLI CB RX: PullvCardEntry cfm, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_cmd_connect

DESCRIPTION
  Performs connection to remote PBAP server.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_cli_cmd_connect(
  bt_app_id_type                   app_id,
  bt_pf_cmd_pbap_cli_connect_type* params_ptr )
{
  OI_STATUS                   status;
  bt_pf_pbap_cli_conn_id_type conn_id;
  bt_bd_addr_type             bd_addr = params_ptr->bd_addr;

  if ( bt_pf_pbap_cli_conn_id != BT_PF_PBAP_NO_CONN_ID )
  {
    BT_MSG_DEBUG( "BT PF PBAP CLI CONN: alrdy conn, App: %x",
                  app_id, 0, 0 );
    return(OI_STATUS_ALREADY_CONNECTED);
  } 

  if ( bt_pf_pbap_cli_app_id == BT_PF_APP_ID_NULL ) 
  {
    BT_MSG_DEBUG( "BT PF PBAP CLI CONN: not registered, App: %x",
                  app_id, 0, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }

  status = PBAPCLI_Connect(                   
    (OI_BD_ADDR*)&bd_addr,
    params_ptr->channel,                      
    bt_pf_pbap_cli_auth_req_cb,
    bt_pf_pbap_cli_connect_cfm_cb,
    bt_pf_pbap_cli_disconnect_ind_cb,
    (PBAPCLI_HANDLE*)&conn_id );

  if (status == OI_OK)
  {
    bt_pf_pbap_cli_conn_id = conn_id;
  }

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_cmd_disonnect

DESCRIPTION
  Disconnect from a remote PBAP server.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_cli_cmd_disconnect(
  bt_app_id_type                     app_id,
  bt_pf_cmd_pbap_cli_disconnect_type* params_ptr )
{
  OI_STATUS status;                  
 
  status = PBAPCLI_Disconnect( (PBAPCLI_HANDLE)params_ptr->conn_id );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_cmd_auth_rsp

DESCRIPTION
  Responds to authentication request from a remote PBAP server.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_cli_cmd_auth_rsp(
  bt_app_id_type                    app_id,
  bt_pf_cmd_pbap_cli_auth_rsp_type* params_ptr )
{
  OI_STATUS                   status;
  OI_BYTE*                    user_id_ptr;
  OI_CHAR*                    pin_ptr;

  if (params_ptr->user_id_len == 0 )
  {
    user_id_ptr = NULL;
  }
  else
  {
    user_id_ptr = (OI_BYTE*)(params_ptr->user_id);
  }

  if (params_ptr->pin_str[0] == '\0')
  {
    pin_ptr = NULL;
  }
  else
  {
    pin_ptr = (OI_CHAR*)(params_ptr->pin_str);
  }

  status = PBAPCLI_AuthenticationResponse( 
    (PBAPCLI_HANDLE)params_ptr->conn_id,
    user_id_ptr,
    params_ptr->user_id_len,
    pin_ptr );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_cmd_register

DESCRIPTION
  Initialize the PBAP Client and register it in the service discovery 
  database.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_cli_cmd_register(
  bt_app_id_type          app_id,
  char*                   cli_name_ptr )
{
  OI_STATUS                        status = OI_OK;

  TASKLOCK();

  if (bt_pf_pbap_cli_app_id != BT_PF_APP_ID_NULL)
  {
    status = OI_STATUS_ALREADY_REGISTERED;
  }
  else
  {
    bt_pf_pbap_cli_app_id = app_id;
  }

  TASKFREE();

  if (status != OI_OK)
  {
    BT_ERR( "BT PF PBAP CLI REG: App %x, Stat: %x", app_id, status, 0 );
    return(status);
  }

  status = bt_cmd_sd_register_service( bt_pf_pbap_cli_app_id,
                                       BT_SD_SERVICE_CLASS_PBAP_PCE,
                                       BT_PF_PBAP_VERSION_NUMBER,
                                       0,
                                       cli_name_ptr );

  if ( status != BT_CS_GN_PENDING )
  {
    BT_ERR( "BT PF PBAP CLI REG: returns %x, App %x", status, app_id, 0 );
    bt_pf_pbap_cli_app_id = BT_PF_APP_ID_NULL;
    return( status );
  }                                                    

  return( OI_OK );
}

/*===========================================================================

FUNCTION
   bt_pf_pbap_cmd_deregister

DESCRIPTION
   Process a PBAP deregister command
===========================================================================*/
bt_cmd_status_type bt_pf_pbap_cli_cmd_deregister(  
  bt_app_id_type          app_id )
{
  OI_STATUS           status = OI_OK;

  if (bt_pf_pbap_cli_app_id == BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF PBAP CLI DEREG: not reg, App: %x, Stat: %x",
                  app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }

  if (bt_pf_pbap_cli_app_id != app_id)
  {
    BT_MSG_DEBUG( "BT PF PBAP CLI DEREG: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = bt_cmd_sd_unregister_service( bt_pf_pbap_cli_app_id,
                                         BT_SD_SERVICE_CLASS_PBAP_PCE );
 
  if (status == BT_CS_GN_PENDING)
  {
    (void) bt_cmd_rm_set_connectable( bt_pf_pbap_cli_app_id, 
                                      FALSE,
                                      BT_RM_AVP_AUTOMATIC );
    bt_pf_pbap_cli_app_id = BT_PF_APP_ID_NULL;
  }
  else
  {
    BT_ERR( "BT PF PBAP CLI DEREG: returns %x", status, 0, 0 );
    return( status );
  }

  return ( OI_OK );
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_cmd_abort

DESCRIPTION
  Processes a PBAP client abort command.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_cli_cmd_abort(
  bt_app_id_type                 app_id,
  bt_pf_pbap_cli_conn_id_type    conn_id )
{
  OI_STATUS                status;

  status = PBAPCLI_Abort( (PBAPCLI_HANDLE)conn_id );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF PBAP CLI ABORT: PBAPClient_Abort returns %x", 
                status, 0, 0 );
  }

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_cmd_pull_phone_book

DESCRIPTION
  Performs a PBAP PullPhoneBook operation.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_cli_cmd_pull_phone_book(
  bt_app_id_type                           app_id,
  bt_pf_cmd_pbap_cli_pull_phone_book_type* params_ptr )
{
  OI_STATUS           status;
  OI_OBEX_BYTESEQ     obj_type;
  OI_OBEX_BYTESEQ*    obj_type_ptr;
  uint16              obj_type_str[BT_PF_MAX_FILENAME_LEN + 1];

  OI_OBEX_UNICODE     obj_name;
  OI_OBEX_UNICODE*    obj_name_ptr;
  uint16              obj_name_str[BT_PF_MAX_FILENAME_LEN + 1];

  /* Name is copied to avoid misaligned access when str pointer is  not 
     aligned */
  memcpy( (void*)obj_name_str,
          (void*)params_ptr->name_str,
          params_ptr->name_len * sizeof( uint16 ) );
  obj_name.len = params_ptr->name_len;
  obj_name.str = obj_name_str;
  obj_name_ptr = &obj_name;

  memcpy( (void*)obj_type_str,
          (void*)PBAP_OBJ_TYPE_PHONE_BOOK,
          sizeof(PBAP_OBJ_TYPE_PHONE_BOOK) );
  obj_type.len = sizeof(PBAP_OBJ_TYPE_PHONE_BOOK);
  obj_type.data = (OI_BYTE*)obj_type_str;
  obj_type_ptr = &obj_type;

  /* Initialize app. params flag */
  bt_pf_pbap_cli_pull_phone_book_params.request.flags = PBAP_PARAM_NONE;

  bt_pf_pbap_cli_pull_phone_book_params.request.objName = obj_name_ptr;
  bt_pf_pbap_cli_pull_phone_book_params.request.objType = obj_type_ptr;

  if ( params_ptr->attribute_mask_ptr ) 
  {
    bt_pf_pbap_cli_pull_phone_book_params.request.flags |= 
      PBAP_PARAM_FILTER;
    bt_pf_pbap_cli_pull_phone_book_params.request.filter = 
      *params_ptr->attribute_mask_ptr; 

    BT_MSG_DEBUG( "    vcard attrib = %d", *params_ptr->attribute_mask_ptr, 0, 0 );
  }

  if ( params_ptr->format_ptr ) 
  {
    bt_pf_pbap_cli_pull_phone_book_params.request.flags |= 
      PBAP_PARAM_FORMAT;
    bt_pf_pbap_cli_pull_phone_book_params.request.format = 
      *params_ptr->format_ptr;   

    BT_MSG_DEBUG( "    vcard format = %d", *params_ptr->format_ptr, 0, 0 );
  }

  if ( params_ptr->max_list_count_ptr ) 
  {
    bt_pf_pbap_cli_pull_phone_book_params.request.flags |= 
      PBAP_PARAM_MAX_LIST_COUNT;
    bt_pf_pbap_cli_pull_phone_book_params.request.listCount = 
      *params_ptr->max_list_count_ptr;   

    BT_MSG_DEBUG( "    list count = %d", *params_ptr->max_list_count_ptr, 0, 0 );

  }

  if ( params_ptr->list_start_offset_ptr ) 
  {
    bt_pf_pbap_cli_pull_phone_book_params.request.flags |= 
      PBAP_PARAM_LIST_START_OFFSET;
    bt_pf_pbap_cli_pull_phone_book_params.request.startOffset = 
      *params_ptr->list_start_offset_ptr; 

    BT_MSG_DEBUG( "    start offset = %d", *params_ptr->list_start_offset_ptr, 0, 0 );
    
  }

  status = PBAPCLI_PullPhoneBook(
    (PBAPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_pbap_cli_pull_phone_book_cfm_cb,
    &bt_pf_pbap_cli_pull_phone_book_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_cmd_set_phone_book

DESCRIPTION
  Performs a PBAP SetPhoneBook operation.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_cli_cmd_set_phone_book(
  bt_app_id_type                           app_id,
  bt_pf_cmd_pbap_cli_set_phone_book_type*  params_ptr )
{
  OI_STATUS           status;
  OI_OBEX_UNICODE     folder_name;
  OI_OBEX_UNICODE*    folder_name_ptr = NULL;
  uint16              folder_name_str[BT_PF_MAX_FILENAME_LEN + 1];

  /* Name is copied to avoid misaligned access when str pointer is  not 
     aligned */
  if ( params_ptr->folder_len > 0 ) 
  {
    memcpy( (void*)folder_name_str,
            (void*)params_ptr->folder_str,
            ARR_SIZE (folder_name_str) );
    folder_name.len = params_ptr->folder_len;
    folder_name.str = folder_name_str;
    folder_name_ptr = &folder_name;
  }

  status = PBAPCLI_SetPhoneBook(
    (PBAPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_pbap_cli_set_phone_book_cfm_cb,
    folder_name_ptr,
    (PBAP_CLIENT_SETPATH_CONTROL) params_ptr->set_path_ctrl );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_cmd_pull_vcard_listing

DESCRIPTION
  Performs a PBAP PullvCardListing operation.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_cli_cmd_pull_vcard_listing(
  bt_app_id_type                           app_id,
  bt_pf_cmd_pbap_cli_pull_vcard_list_type* params_ptr )
{
  OI_STATUS           status;
  OI_OBEX_BYTESEQ     obj_type;
  OI_OBEX_BYTESEQ*    obj_type_ptr;
  uint16              obj_type_str[BT_PF_MAX_FILENAME_LEN + 1];

  OI_OBEX_UNICODE     obj_name;
  OI_OBEX_UNICODE*    obj_name_ptr;
  uint16              obj_name_str[BT_PF_MAX_FILENAME_LEN + 1];

  OI_OBEX_BYTESEQ     value;
  uint8               value_str[BT_PF_MAX_MIME_TYPE_LEN + 1];

  /* Name is copied to avoid misaligned access when str pointer is  not 
     aligned */
  memcpy( (void*)obj_name_str,
          (void*)params_ptr->name_str,
          params_ptr->name_len * sizeof( uint16 ) );
  obj_name.len = params_ptr->name_len;
  obj_name.str = obj_name_str;
  obj_name_ptr = &obj_name;

  memcpy( (void*)obj_type_str,
          (void*)PBAP_OBJ_TYPE_VCARD_LISTING,
          sizeof(PBAP_OBJ_TYPE_VCARD_LISTING) );
  obj_type.len = sizeof(PBAP_OBJ_TYPE_VCARD_LISTING);
  obj_type.data = (OI_BYTE*)obj_type_str;
  obj_type_ptr = &obj_type;

  memcpy( (void*)value_str,
          (void*)params_ptr->value_str,
          params_ptr->value_len );
  value.len  = params_ptr->value_len;
  value.data = value_str;

  /* Initialize app. params flag */
  bt_pf_pbap_cli_pull_vcard_listing_params.request.flags = PBAP_PARAM_NONE;

  bt_pf_pbap_cli_pull_vcard_listing_params.request.objName     = obj_name_ptr;
  bt_pf_pbap_cli_pull_vcard_listing_params.request.objType     = obj_type_ptr;

  if ( params_ptr->value_len > 0 ) 
  {
    bt_pf_pbap_cli_pull_vcard_listing_params.request.flags |= 
      PBAP_PARAM_VALUE;
    bt_pf_pbap_cli_pull_vcard_listing_params.request.searchValue = value;  

    BT_MSG_DEBUG( "    search value = %s", value.data, 0, 0 );
  }

  if ( params_ptr->order_ptr ) 
  {
    bt_pf_pbap_cli_pull_vcard_listing_params.request.flags |= 
      PBAP_PARAM_ORDER;
    bt_pf_pbap_cli_pull_vcard_listing_params.request.order = 
      *params_ptr->order_ptr;  

    BT_MSG_DEBUG( "    sort order = %d", *params_ptr->order_ptr, 0, 0 );
  }

  if ( params_ptr->search_attrib_ptr ) 
  {
    bt_pf_pbap_cli_pull_vcard_listing_params.request.flags |= 
      PBAP_PARAM_ATTRIBUTE;
    bt_pf_pbap_cli_pull_vcard_listing_params.request.searchAttrib = 
      *params_ptr->search_attrib_ptr;  

    BT_MSG_DEBUG( "    vcard attrib = %d", *params_ptr->search_attrib_ptr, 0, 0 );
  }

  if ( params_ptr->max_list_count_ptr ) 
  {
    bt_pf_pbap_cli_pull_vcard_listing_params.request.flags |= 
      PBAP_PARAM_MAX_LIST_COUNT;
    bt_pf_pbap_cli_pull_vcard_listing_params.request.listCount = 
      *params_ptr->max_list_count_ptr;   

    BT_MSG_DEBUG( "    list count = %d", *params_ptr->max_list_count_ptr, 0, 0 );
  }

  if ( params_ptr->list_start_offset_ptr ) 
  {
    bt_pf_pbap_cli_pull_vcard_listing_params.request.flags |=
      PBAP_PARAM_LIST_START_OFFSET;
    bt_pf_pbap_cli_pull_vcard_listing_params.request.startOffset = 
      *params_ptr->list_start_offset_ptr; 

    BT_MSG_DEBUG( "    start offset = %d", *params_ptr->list_start_offset_ptr, 0, 0 );
  }

  status = PBAPCLI_PullvCardListing(
    (PBAPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_pbap_cli_pull_vcard_listing_cfm_cb,
    &bt_pf_pbap_cli_pull_vcard_listing_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_cli_cmd_pull_vcard_entry

DESCRIPTION
  Performs a PBAP PullvCardEntry operation.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_cli_cmd_pull_vcard_entry(
  bt_app_id_type                            app_id,
  bt_pf_cmd_pbap_cli_pull_vcard_entry_type* params_ptr )
{
  OI_STATUS           status;
  OI_OBEX_BYTESEQ     obj_type;
  OI_OBEX_BYTESEQ*    obj_type_ptr;
  uint16              obj_type_str[BT_PF_MAX_FILENAME_LEN + 1];

  OI_OBEX_UNICODE     obj_name;
  OI_OBEX_UNICODE*    obj_name_ptr;
  uint16              obj_name_str[BT_PF_MAX_FILENAME_LEN + 1];

  /* Name is copied to avoid misaligned access when str pointer is  not 
     aligned */
  memcpy( (void*)obj_name_str,
          (void*)params_ptr->name_str,
          params_ptr->name_len * sizeof( uint16 ) );
  obj_name.len = params_ptr->name_len;
  obj_name.str = obj_name_str;
  obj_name_ptr = &obj_name;

  memcpy( (void*)obj_type_str,
          (void*)PBAP_OBJ_TYPE_VCARD_ENTRY,
          sizeof(PBAP_OBJ_TYPE_VCARD_ENTRY) );
  obj_type.len = sizeof(PBAP_OBJ_TYPE_VCARD_ENTRY);
  obj_type.data = (OI_BYTE*)obj_type_str;
  obj_type_ptr = &obj_type;

  bt_pf_pbap_cli_pull_vcard_entry_params.request.objName = obj_name_ptr;
  bt_pf_pbap_cli_pull_vcard_entry_params.request.objType = obj_type_ptr;  

    /* Initialize app. params flag */
  bt_pf_pbap_cli_pull_vcard_entry_params.request.flags = PBAP_PARAM_NONE;

  if ( params_ptr->attribute_mask_ptr ) 
  {
    bt_pf_pbap_cli_pull_vcard_entry_params.request.flags |= 
      PBAP_PARAM_FILTER;
    bt_pf_pbap_cli_pull_vcard_entry_params.request.filter = 
      *params_ptr->attribute_mask_ptr;   

    BT_MSG_DEBUG( "    vcard attrib = %d", *params_ptr->attribute_mask_ptr, 0, 0 );
  }

  if ( params_ptr->format_ptr ) 
  {
    bt_pf_pbap_cli_pull_vcard_entry_params.request.flags |= 
      PBAP_PARAM_FORMAT;
    bt_pf_pbap_cli_pull_vcard_entry_params.request.format = 
      *params_ptr->format_ptr; 

    BT_MSG_DEBUG( "    vcard format = %d", *params_ptr->format_ptr, 0, 0 );
  }

  status = PBAPCLI_PullvCardEntry(
    (PBAPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_pbap_cli_pull_vcard_entry_cfm_cb,
    &bt_pf_pbap_cli_pull_vcard_entry_params );

  return(status);
}

#endif /* FEATURE_BT_EXTPF_PBAP */
#endif /* FEATURE_BT */
