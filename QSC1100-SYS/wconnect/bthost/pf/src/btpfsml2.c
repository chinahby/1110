/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             B L U E T O O T H   S Y N C - M L   C L I E N T   
              
                    A B S T R A C T I O N   L A Y E R

GENERAL DESCRIPTION
  This module contains data and code for the SyncML client abstraction layer 
  on top of the Bluetooth Generic Object Exchange Profile.
  
  Copyright (c) 2004 - 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfsml2.c#1 $ 

$Log: $
*
*   #11         28 Feb  2007           MH
* Modified disconnect indication to support optional headers
*
*    #10        26 Oct 2006           MH
* Modified conn indication callback to support all headers
*
*    #9        16 Jun 2006            DSN
* Removed superfluous header file inclusion.
*
*    #8        12 Jun 2006            DSN
* Typecasting fixes for RVCT compiler.
*
*    #7        19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #6        30 Aug 2004            JH
* Modified bt_pf_sml_cmd_register_client() to use 
*  bt_cmd_sd_register_syncml_service() and bt_cmd_sd_unregister_syncml_service()
*  for SD registration.
* Removed calls for manipulating object transfer bit in class of device field;
*  this is now handled by SD.
*
*    #5        14 May 2004            JH
* Modified code for setting object transfer bit in class od service field
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

#include "bt.h"
#include "btpfi.h"
#include "bti.h"
#include "btmsg.h" 
#include "btpfsmli.h"     
#include "btsd.h"
#include "syncml_client.c"
                                                           
uint8 bt_pf_sml_cli_scn = BT_SPP_SCN_UNSPECIFIED;
const char bt_pf_sml_target_uuid_str[] = BT_PF_SML_OBEX_TARGET_UUID;

/*===========================================================================

FUNCTION
  bt_pf_sml_cli_con_ind_cb    
   
DESCRIPTION
  This function handles the generation of the connection indication event to
  the application.
   
===========================================================================*/
void bt_pf_sml_cli_con_ind_cb( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                               OI_BOOL unauthorized,
                               OI_BYTE *user_id_ptr,
                               OI_UINT8 user_id_len,
                               OI_OBEX_HEADER_LIST *cmd_headers_ptr)
{  
  bt_ev_msg_type event;

  BT_MSG_DEBUG( "BT PF SML CLI CON IND CB: Conn: %x", conn_id, 0, 0 ); 

  event.ev_hdr.ev_type = BT_EV_PF_SML_CLI_CON_IND;
  event.ev_hdr.bt_app_id = bt_pf_sml_app_id;

  event.ev_msg.ev_sml_cli_con_ind.conn_id = (bt_pf_sml_conn_id_type)conn_id;
  event.ev_msg.ev_sml_cli_con_ind.unauthorized = unauthorized;

  /* OBEX should have rejected auth digest with invalid user id length */
  if (user_id_len > BT_PF_OBEX_MAX_USERID_LEN)
  {
    /* truncate just in case */
    user_id_len = BT_PF_OBEX_MAX_USERID_LEN;
  }
  if (user_id_len > 0)
  {                  
    memcpy( (void*)event.ev_msg.ev_sml_cli_con_ind.user_id,
            (void*)user_id_ptr,
            user_id_len );
  }
  event.ev_msg.ev_sml_cli_con_ind.user_id_len = user_id_len;

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_sml_cli_dcn_ind_cb    
   
DESCRIPTION
  This function handles the generation of the disconnection indication event 
  to the application.
   
===========================================================================*/
void bt_pf_sml_cli_dcn_ind_cb( OI_OBEXSRV_CONNECTION_HANDLE conn_id, 
                               OI_OBEX_HEADER_LIST *headers)
{
  bt_ev_msg_type event;

  BT_MSG_DEBUG( "BT PF SML CLI DCN IND CB: Conn: %x", conn_id, 0, 0 );

  event.ev_hdr.ev_type = BT_EV_PF_SML_CLI_DCN_IND;
  event.ev_hdr.bt_app_id = bt_pf_sml_app_id;

  event.ev_msg.ev_sml_cli_dcn_ind.conn_id = (bt_pf_sml_conn_id_type)conn_id;

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_sml_cli_get_ind_cb    
   
DESCRIPTION
  This function handles the generation of the get indication event to
  the application.
   
===========================================================================*/
OI_STATUS bt_pf_sml_cli_get_ind_cb( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                    OI_OBEX_HEADER_LIST *cmd_headers_ptr,
                                    OI_STATUS status )
{
  bt_ev_msg_type event;

  BT_MSG_DEBUG( "BT PF SML CLI GET IND CB: Conn: %x", conn_id, 0, 0 );

  event.ev_hdr.ev_type = BT_EV_PF_SML_CLI_GET_IND;
  event.ev_hdr.bt_app_id = bt_pf_sml_app_id;

  event.ev_msg.ev_sml_cli_get_ind.conn_id = (bt_pf_sml_conn_id_type)conn_id;
  event.ev_msg.ev_sml_cli_get_ind.cmd_headers_cnt = cmd_headers_ptr->count;
  event.ev_msg.ev_sml_cli_get_ind.cmd_headers_ptr =
    (bt_pf_goep_obex_hdr_type*)(cmd_headers_ptr->list);
  event.ev_msg.ev_sml_cli_get_ind.status = (bt_cmd_status_type)status;

  /* Trick the OI_FreeIf inside GetCommand into NOT freeing the header list */
  /* The application will need to free the list when done */
  cmd_headers_ptr->list = NULL;

  bt_ec_send_event( &event );

  return( OI_OK );
}

/*===========================================================================

FUNCTION
  bt_pf_sml_cli_put_ind_cb    
   
DESCRIPTION
  This function handles the generation of the put indication event to
  the application. 
   
===========================================================================*/
OI_STATUS bt_pf_sml_cli_put_ind_cb( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                    OI_OBEX_HEADER_LIST *cmd_headers_ptr,
                                    OI_STATUS status )
{
  bt_ev_msg_type event;

  BT_MSG_DEBUG( "BT PF SML CLI PUT IND CB: Conn: %x", conn_id, 0, 0 );

  event.ev_hdr.ev_type = BT_EV_PF_SML_CLI_PUT_IND;
  event.ev_hdr.bt_app_id = bt_pf_sml_app_id;

  event.ev_msg.ev_sml_cli_put_ind.conn_id = (bt_pf_sml_conn_id_type)conn_id;
  event.ev_msg.ev_sml_cli_put_ind.cmd_headers_cnt = cmd_headers_ptr->count;
  event.ev_msg.ev_sml_cli_put_ind.cmd_headers_ptr =
    (bt_pf_goep_obex_hdr_type*)(cmd_headers_ptr->list);
  event.ev_msg.ev_sml_cli_put_ind.status = (bt_cmd_status_type)status;

  /* Trick the OI_FreeIf inside PutCommand into NOT freeing the header list */
  /* The application will need to free the list when done */
  cmd_headers_ptr->list = NULL;

  bt_ec_send_event( &event );

  return( OI_OK );
}

OI_OBEXSRV_CB_LIST bt_pf_sml_cli_cb_list =
{
  bt_pf_sml_cli_con_ind_cb,
  bt_pf_sml_cli_dcn_ind_cb,
  bt_pf_sml_cli_get_ind_cb,
  bt_pf_sml_cli_put_ind_cb,
  NULL /* set path inapplicable */
};

/*===========================================================================

FUNCTION
  bt_pf_sml_cmd_register_client

DESCRIPTION
  Initialize the OBEX SyncML client and register it in the service 
  discovery database.

===========================================================================*/
bt_cmd_status_type bt_pf_sml_cmd_register_client(
  bt_app_id_type                   app_id,
  bt_pf_cmd_sml_register_cli_type* params_ptr )
{ 
  OI_STATUS                        status = OI_OK;
  OI_SDP_STRINGS                   dummy_sdp_str = { NULL, 0 };
  uint8                            scn;

  TASKLOCK();

  if ( bt_pf_sml_app_id != BT_PF_APP_ID_NULL )
  {
    status = OI_STATUS_ALREADY_REGISTERED;
  }
  else
  {
    bt_pf_sml_app_id = app_id;
  } 

  TASKFREE();

  if (status != OI_OK)
  {
    BT_ERR( "BT PF SML REG CLI: App %x, Stat: %x", 
            app_id, status, 0 );
    return(status);
  }

  status = OI_SyncML_RegisterClient(
    &dummy_sdp_str,
    params_ptr->auth,
    &bt_pf_sml_cli_cb_list, 
    (char*)bt_pf_sml_target_uuid_str,
    &scn );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF SML REG CLI: returns %x, App %x", 
            status, app_id, 0 );
    bt_pf_sml_app_id = BT_PF_APP_ID_NULL;
    return(status);
  }              

  /* add service record to SDP database */
  status = bt_cmd_sd_register_syncml_service( 
             app_id,
             TRUE, /* client */
             scn,
             (char *)params_ptr->srv_name_str );

  if (status != BT_CS_GN_PENDING)
  {
    BT_ERR( "BT PF SML REG CLI: SDP reg failed, App: %x", app_id, 0, 0 );
    OI_SyncML_DeregisterClient( scn );
    return(status);
  }

  bt_pf_sml_cli_scn = scn;

  return(OI_OK);
}


/*===========================================================================

FUNCTION
  bt_pf_sml_cmd_deregister_client

DESCRIPTION
  Deregisters a OBEX SyncML client
  
===========================================================================*/
bt_cmd_status_type bt_pf_sml_cmd_deregister_client(
  bt_app_id_type                   app_id )
{
  OI_STATUS           status;

  if (bt_pf_sml_app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF SML DEREG CLI: not reg, App: %x, Stat: %x",
            app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }
  
  if (bt_pf_sml_app_id != app_id)
  {
    BT_ERR( "BT PF SML DEREG CLI: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }                                                              
  
  status = OI_SyncML_DeregisterClient( bt_pf_sml_cli_scn );
  if (status != OI_OK)
  {
    BT_ERR( "BT PF SML DEREG CLI: OI_SyncML_DeregisterClient returns %x", 
                status, 0, 0 );
  }

  status = bt_cmd_sd_unregister_syncml_service( app_id, TRUE );

  if (status != BT_CS_GN_PENDING)
  {
    BT_ERR( "BT PF SML DEREG CLI: sd_unregister returns %x", 
                status, 0, 0 );
  }

  bt_pf_sml_app_id = BT_PF_APP_ID_NULL;
  bt_pf_sml_cli_scn = BT_SPP_SCN_UNSPECIFIED;
  
  return(status); 
}

#endif /* FEATURE_BT_EXTPF_SML */
#endif /* FEATURE_BT */
