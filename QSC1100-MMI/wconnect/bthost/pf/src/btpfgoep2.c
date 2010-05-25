/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               B L U E T O O T H   G E N E R I C   O B J E C T   
          
                E X C H A N G E   P R O F I L E   C L I E N T

GENERAL DESCRIPTION
  This module contains the Bluetooth Generic Object Exchange Profile client 
  data and code.

Copyright (c) 2004-2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfgoep2.c#1 $ 

$Log: $
*
*    #20       12 Mar 2007             MH    
* Removing debug message to remove compiler warnings
*
*    #19       28 Feb 2007             MH    
* Full access info provided with auth indication.
*
*    #18       10 Nov 2006             MH    
* Modified to initalize the table bt_cmd_pf_goep_cli_init_data  
*
*    #17       19 Sept 2006             MH    
* Client Auth response indication call back is added.
*
*    #16       25 AUG 2006             BH    
* Compile errors fixed
*
*    #15       12 AUG 2006             BH    
* Modified for obex client authentication prototype
*
*    #14       28 Jul 2006             BH    
* Modified for obex client authentication
*
*    #12-13    10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #11       23 Jun 2006            JH
* Modified to support updated GOEP API for JSR82.
*
*    #10       28 Mar 2006            JH
* Added function bt_pf_goep_cli_app_conn_table_init().
*
*    #9        26 Sep 2005            ABu
* Added IrDA include files to avoid compiler warning.
*
*    #8        31 May 2005            RY
* Merged in Sirius changes.
*
*    #7        19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #6        06 Dec 2004            JH
* Added NULL pointer checking in callback functions.
*
*    #2        25 Feb 2004            JH
* Renamed btpfgoep.h btpfgoepi.h
*

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_GOEP

#define BT_MSG_LAYER  BT_MSG_GN 
#include "bt.h"                        
#include "btpfi.h"
#include "btpfgoepi.h"
#include "bti.h"
#include "btmsg.h"
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */
#include "obexcli.c"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
                                                 
/*-------------------------------------------------------------------------*/
/*               Connection-Application Table Declaration                  */
/*-------------------------------------------------------------------------*/
/*
  Each item in the connection-application table maps the OBEX connection
  to the application which makes the connection. This mapping allows the
  events being generated to be routed to the proper application. 
*/
typedef struct
{
  /* application ID identifies the application which makes this connection */
  bt_app_id_type              app_id;

  /* connection ID for the OBEX connection */
  bt_pf_goep_cli_conn_id_type conn_id;
} bt_pf_goep_cli_app_conn_item_type;
                                            
LOCAL bt_pf_goep_cli_app_conn_item_type 
  bt_pf_goep_cli_app_conn[ BT_PF_SIO_MAX_PORTS ];

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_goep_cli_init_app_conn_table   
   
DESCRIPTION
  Initializes the AppID-ConnectionID table.
   
===========================================================================*/
void bt_pf_goep_cli_app_conn_table_init( void )
{
  uint32 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    bt_pf_goep_cli_app_conn[i].app_id  = BT_PF_APP_ID_NULL;
    bt_pf_goep_cli_app_conn[i].conn_id = BT_PF_GOEP_NO_CONN_ID_NULL;
  }  
}

/*===========================================================================

FUNCTION
  bt_pf_goep_cli_app_conn_assign    
   
DESCRIPTION
  Allocates an entry in the connection-application table for a OBEX client.
   
===========================================================================*/
int16 bt_pf_goep_cli_app_conn_assign( bt_app_id_type app_id )
{
  int16 i;   

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    if (bt_pf_goep_cli_app_conn[i].app_id == BT_PF_APP_ID_NULL)
    {
      bt_pf_goep_cli_app_conn[i].app_id = app_id;
      return(i);
    }
  } 
  
  return(BT_PF_GOEP_CLI_APP_CONN_FULL);
}

/*===========================================================================

FUNCTION
  bt_pf_goep_cli_app_conn_find    
   
DESCRIPTION
  Finds the application ID corresponding to the given connection ID.
   
===========================================================================*/
LOCAL bt_app_id_type bt_pf_goep_cli_app_conn_find(
  bt_pf_goep_cli_conn_id_type conn_id )
{
  uint32 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    if (bt_pf_goep_cli_app_conn[i].conn_id == conn_id)
    {
      return(bt_pf_goep_cli_app_conn[i].app_id);
    }
  }
  return(BT_PF_APP_ID_NULL);
}

/*===========================================================================

FUNCTION
  bt_pf_goep_cli_app_conn_link    
   
DESCRIPTION
  Writes the connection ID to the entry in the connection-application table
  corresponding to the application.
   
===========================================================================*/
int16 bt_pf_goep_cli_app_conn_link( 
  bt_app_id_type              app_id,
  bt_pf_goep_cli_conn_id_type conn_id )
{
  int16 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    if ( (bt_pf_goep_cli_app_conn[i].app_id == app_id) &&
         (bt_pf_goep_cli_app_conn[i].conn_id == BT_PF_GOEP_NO_CONN_ID_NULL) )
    {
      bt_pf_goep_cli_app_conn[i].conn_id = conn_id;
      BT_MSG_DEBUG( "BT PF GOEP CLI: app conn link, App: %x, Conn: %x",
                    app_id, conn_id, 0 ); 
      return(i);
    }
  }
  return(BT_PF_GOEP_CLI_APP_CONN_NOT_FOUND);
}

/*===========================================================================

FUNCTION
  bt_pf_goep_cli_app_conn_free    
   
DESCRIPTION
  Frees the entry in the connection-application table corresponding to the
  given connection.
   
===========================================================================*/
LOCAL void bt_pf_goep_cli_app_conn_free( bt_pf_goep_cli_conn_id_type conn_id )
{
  uint32 i;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    if (bt_pf_goep_cli_app_conn[i].conn_id == conn_id)
    { 
      BT_MSG_DEBUG( "BT PF GOEP CLI: app conn free, App: %x, Conn: %x",
                    bt_pf_goep_cli_app_conn[i].app_id, conn_id, 0 );
      bt_pf_goep_cli_app_conn[i].app_id = BT_PF_APP_ID_NULL;
      bt_pf_goep_cli_app_conn[i].conn_id = BT_PF_GOEP_NO_CONN_ID_NULL;
      break;
    }
  }                                         
}

/*===========================================================================

FUNCTION
  bt_pf_goep_cli_con_cfm_cb    
   
DESCRIPTION
  This function handles the generation of the connection confirmation event 
  to the application. The application ID of the target is retrieved from the
  connection-application table.
   
===========================================================================*/
void bt_pf_goep_cli_con_cfm_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id,
                                OI_BOOL read_only,
                                OI_OBEX_HEADER_LIST* rsp_headers_ptr,
                                OI_STATUS status )
{
  bt_app_id_type    app_id;
  bt_ev_msg_type    event;

  app_id = bt_pf_goep_cli_app_conn_find( 
             (bt_pf_goep_cli_conn_id_type)conn_id );

  if (app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF GOEP CLI CON CFM CB: app not found, Conn: %x",
            conn_id, 0, 0 );
    return;
  }

  BT_MSG_DEBUG( "BT PF GOEP CLI CON CFM CB: Conn: %x, App: %x, Status: %x", 
                conn_id, app_id, status );
  event.ev_hdr.ev_type = BT_EV_PF_GOEP_CLI_CON_CFM;
  event.ev_hdr.bt_app_id = app_id;

  event.ev_msg.ev_goep_cli_con_cfm.conn_id = 
    (bt_pf_goep_cli_conn_id_type)conn_id;
  event.ev_msg.ev_goep_cli_con_cfm.read_only = read_only;
  if ( rsp_headers_ptr != NULL )
  {
    event.ev_msg.ev_goep_cli_con_cfm.rsp_headers_cnt = rsp_headers_ptr->count;
    event.ev_msg.ev_goep_cli_con_cfm.rsp_headers_ptr =
      (bt_pf_goep_obex_hdr_type*)(rsp_headers_ptr->list);
  }
  else
  {
    event.ev_msg.ev_goep_cli_con_cfm.rsp_headers_cnt = 0;
    event.ev_msg.ev_goep_cli_con_cfm.rsp_headers_ptr = NULL;
  }
  event.ev_msg.ev_goep_cli_con_cfm.status = 
    (bt_cmd_status_type)status;

  bt_ec_send_event( &event );

  /* Trick the OI_FreeIf inside PutResponse into NOT freeing the header list */
  /* The application will need to free the list when done */
  if ( rsp_headers_ptr != NULL )
  {
    rsp_headers_ptr->list = NULL;
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_goep_cli_dcn_ind_cb    
   
DESCRIPTION
  This function handles the generation of the disconnection indication event 
  to the application. The application ID of the target is retrieved from the
  connection-application table.
   
===========================================================================*/
void bt_pf_goep_cli_dcn_ind_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id,
                                OI_OBEX_HEADER_LIST* rsp_headers_ptr )
{
  bt_app_id_type             app_id;
  bt_ev_msg_type             event;
  bt_pf_goep_obex_hdr_type*  hdrs_ptr;
  uint16                     hdrs_byte_size;
  uint16 len ;
  uint16 hdrs_cnt ;

  int i = 0;

  app_id = bt_pf_goep_cli_app_conn_find( 
             (bt_pf_goep_cli_conn_id_type)conn_id );

  if (app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF GOEP CLI CON CFM CB: app not found, Conn: %x",
            conn_id, 0, 0 );
    return;
  }
  BT_MSG_DEBUG( "BT PF GOEP CLI DCN IND CB: Conn: %x, App: %x", 
                conn_id, app_id, 0 );

  event.ev_hdr.ev_type = BT_EV_PF_GOEP_CLI_DCN_IND;
  event.ev_hdr.bt_app_id = app_id;
  event.ev_msg.ev_goep_cli_dcn_ind.conn_id = 
    (bt_pf_goep_cli_conn_id_type)conn_id;

  if ( rsp_headers_ptr != NULL )
  {
      hdrs_byte_size = (sizeof( bt_pf_goep_obex_hdr_type ) * rsp_headers_ptr->count);
      hdrs_cnt = rsp_headers_ptr->count ;

      BT_MSG_DEBUG("OBEX Client...Allocating memory for Size= %x : HdrsCount:%x", 
                                                    hdrs_byte_size,hdrs_cnt,0);
    
      hdrs_ptr = (bt_pf_goep_obex_hdr_type*)mem_malloc( &bt_pf_heap, 
                                                      hdrs_byte_size );
      if(hdrs_ptr == NULL){
          BT_ERR( "BT PF GOEP DCN IND CB: Failed to allocate memory: %x",
                                                        conn_id, 0, 0 );
      } else {

        memcpy( hdrs_ptr, rsp_headers_ptr->list, hdrs_byte_size );
        event.ev_msg.ev_goep_cli_dcn_ind.rsp_headers_cnt = rsp_headers_ptr->count;
        event.ev_msg.ev_goep_cli_dcn_ind.rsp_headers_ptr = hdrs_ptr;

        for (i=0; i<hdrs_cnt; i++) {
            BT_MSG_HIGH("CLI DCN IND : Scanning Header Id = %x",hdrs_ptr->id, 0, 0);
            switch (OI_OBEX_HDR_KIND(hdrs_ptr->id)) {
                case OI_OBEX_HDR_ID_BYTESEQ:
                    len = rsp_headers_ptr->list->val.byteseq.len;
                    if(len) {
                        hdrs_ptr->val.byteseq.data = (uint8 *)mem_malloc( &bt_pf_heap, len);
                        if(hdrs_ptr->val.byteseq.data == NULL) {
                                BT_ERR( "BT PF GOEP DCN IND CB: Failed to allocate memory: %x",
                                                                    conn_id, 0, 0 );
                        }
                        memcpy( hdrs_ptr->val.byteseq.data, rsp_headers_ptr->list->val.byteseq.data, len );
                    }
                    break;
                case OI_OBEX_HDR_ID_UNICODE:
                    len = rsp_headers_ptr->list->val.unicode.len;
                    len = (len + 1) * sizeof(uint16);
                    if(len) {
                        hdrs_ptr->val.unicode.str = (uint16 *)mem_malloc( &bt_pf_heap, len);
                        if(hdrs_ptr->val.unicode.str == NULL) {
                                BT_ERR( "BT PF GOEP DCN IND CB: Failed to allocate memory: %x",
                                                                    conn_id, 0, 0 );
                        }
                        memcpy( hdrs_ptr->val.unicode.str, rsp_headers_ptr->list->val.unicode.str, len );
                    }
                    break;
                case OI_OBEX_HDR_ID_UINT8:
                    break;
                case OI_OBEX_HDR_ID_UINT32:
                    break;
                default:
                    BT_ERR("Cli DCN IND : Unknown Header ID Kind recd",0,0,0);
                    break;
            }
            hdrs_ptr++;
        }
    }
  }
  else
  {
    event.ev_msg.ev_goep_cli_dcn_ind.rsp_headers_cnt = 0;
    event.ev_msg.ev_goep_cli_dcn_ind.rsp_headers_ptr = NULL;
  }

  bt_pf_goep_cli_app_conn_free( conn_id );

  bt_ec_send_event( &event );

  return;
}
/*===========================================================================

FUNCTION
  bt_pf_goep_cli_auth_ind_cb    
   
DESCRIPTION
  This function handles the generation of the authentication indication event 
  to the application. The application ID of the target is retrieved from the
  connection-application table.
   
===========================================================================*/
void bt_pf_goep_cli_auth_ind_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id,
                                 OI_BOOL full_access,
                                 OI_BOOL user_id_required )
{
  bt_app_id_type    app_id;
  bt_ev_msg_type    event;

  app_id = bt_pf_goep_cli_app_conn_find( 
             (bt_pf_goep_cli_conn_id_type)conn_id );

  if (app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF GOEP CLI CON CFM CB: app not found, Conn: %x",
            conn_id, 0, 0 );
    return;
  }
  BT_MSG_DEBUG( "BT PF GOEP CLI AUTH IND CB: Conn: %x, App: %x", 
                conn_id, app_id, 0 );
  event.ev_hdr.ev_type = BT_EV_PF_GOEP_CLI_AUTH_IND;
  event.ev_hdr.bt_app_id = app_id;

  event.ev_msg.ev_goep_cli_auth_ind.conn_id = 
    (bt_pf_goep_cli_conn_id_type)conn_id;
  event.ev_msg.ev_goep_cli_auth_ind.user_id_required = 
    user_id_required;
  event.ev_msg.ev_goep_cli_auth_ind.full_access = 
                                  full_access;

  bt_ec_send_event( &event );
  return;
}


/*===========================================================================

FUNCTION
  bt_pf_goep_cli_auth_rsp_ind_cb    
   
DESCRIPTION
  This function handles the generation of the authentication response indication 
  event to the application. The application ID of the target is retrieved from the
  connection-application table.
   
===========================================================================*/
void bt_pf_goep_cli_auth_rsp_ind_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id)
{
  bt_app_id_type    app_id;
  bt_ev_msg_type    event;

  app_id = bt_pf_goep_cli_app_conn_find( 
             (bt_pf_goep_cli_conn_id_type)conn_id );

  if (app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF GOEP CLI CON CFM CB: app not found, Conn: %x",
            conn_id, 0, 0 );
    return;
  }
  BT_MSG_DEBUG( "BT PF GOEP CLI AUTH IND CB: Conn: %x, App: %x", 
                conn_id, app_id, 0 );
  event.ev_hdr.ev_type = BT_EV_PF_GOEP_CLI_AUTH_RESP_IND;
  event.ev_hdr.bt_app_id = app_id;

  event.ev_msg.ev_goep_cli_auth_res_ind.conn_id = 
                    (bt_pf_goep_cli_conn_id_type)conn_id;

  bt_ec_send_event( &event );
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_goep_cli_put_cfm_cb    
   
DESCRIPTION
  This function handles the generation of the put confirmation event 
  to the application. The application ID of the target is retrieved from the
  connection-application table.
  
  Caller is responsible for freeing the memory allocated for storing the
  response headers pointed to by rsp_headers_ptr.
   
===========================================================================*/
void bt_pf_goep_cli_put_cfm_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id,
                                OI_OBEX_HEADER_LIST *rsp_headers_ptr,
                                OI_STATUS status )
{  
  bt_app_id_type    app_id;
  bt_ev_msg_type    event;

  app_id = bt_pf_goep_cli_app_conn_find( 
             (bt_pf_goep_cli_conn_id_type)conn_id );

  if (app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF GOEP CLI CON CFM CB: app not found, Conn: %x",
            conn_id, 0, 0 );
    return;
  }
  BT_MSG_DEBUG( "BT PF GOEP CLI PUT CFM CB: Conn: %x, App: %x, Status: %x", 
                conn_id, app_id, status );

  event.ev_hdr.ev_type = BT_EV_PF_GOEP_CLI_PUT_CFM;
  event.ev_hdr.bt_app_id = app_id;
  event.ev_msg.ev_goep_cli_put_cfm.conn_id = 
    (bt_pf_goep_cli_conn_id_type)conn_id;
  if ( rsp_headers_ptr != NULL )
  {
    event.ev_msg.ev_goep_cli_put_cfm.rsp_headers_cnt = rsp_headers_ptr->count;
    event.ev_msg.ev_goep_cli_put_cfm.rsp_headers_ptr =
      (bt_pf_goep_obex_hdr_type*)(rsp_headers_ptr->list);
  }
  else
  {
    event.ev_msg.ev_goep_cli_put_cfm.rsp_headers_cnt = 0;
    event.ev_msg.ev_goep_cli_put_cfm.rsp_headers_ptr = NULL;
  }
  event.ev_msg.ev_goep_cli_put_cfm.status = (bt_cmd_status_type)status;

  bt_ec_send_event( &event );

  /* Trick the OI_FreeIf inside PutResponse into NOT freeing the header list */
  /* The application will need to free the list when done */
  if ( rsp_headers_ptr != NULL )
  {
    rsp_headers_ptr->list = NULL;
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_goep_cli_get_recv_data_cb    
   
DESCRIPTION
  This function handles the generation of the data receive event 
  to the application. The application ID of the target is retrieved from the
  connection-application table.
  
  Caller is responsible for freeing the memory allocated for storing the
  response headers pointed to by rsp_headers_ptr.
   
===========================================================================*/
void bt_pf_goep_cli_get_recv_data_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id,
                                      OI_OBEX_HEADER_LIST *rsp_headers_ptr,
                                      OI_STATUS status )
{
  bt_app_id_type    app_id;
  bt_ev_msg_type    event;

  app_id = bt_pf_goep_cli_app_conn_find( 
             (bt_pf_goep_cli_conn_id_type)conn_id );

  if (app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF GOEP CLI CON CFM CB: app not found, Conn: %x",
            conn_id, 0, 0 );
    return;
  }
  BT_MSG_DEBUG( "BT PF GOEP CLI GET RCV DATA: Conn: %x, App: %x, Status: %x", 
               conn_id, app_id, status );
  event.ev_hdr.ev_type = BT_EV_PF_GOEP_CLI_GET_RCV_DATA;
  event.ev_hdr.bt_app_id = app_id;
  event.ev_msg.ev_goep_cli_get_rcv_data.conn_id = 
    (bt_pf_goep_cli_conn_id_type)conn_id;
  if ( rsp_headers_ptr != NULL)
  {
    event.ev_msg.ev_goep_cli_get_rcv_data.rsp_headers_cnt = 
      rsp_headers_ptr->count;
    event.ev_msg.ev_goep_cli_get_rcv_data.rsp_headers_ptr =
      (bt_pf_goep_obex_hdr_type*)(rsp_headers_ptr->list);
  }
  else
  {
    event.ev_msg.ev_goep_cli_get_rcv_data.rsp_headers_cnt = 0;
    event.ev_msg.ev_goep_cli_get_rcv_data.rsp_headers_ptr = NULL;
  }
  event.ev_msg.ev_goep_cli_get_rcv_data.status = (bt_cmd_status_type)status;

  bt_ec_send_event( &event );
  
  /* Trick the OI_FreeIf inside GetCommand into NOT freeing the header list */
  /* The application will need to free the list when done */
  if(rsp_headers_ptr != NULL)
  {
    rsp_headers_ptr->list = NULL;
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_goep_cli_set_path_cfm_cb    
   
DESCRIPTION
  This function handles the generation of the set path confirmation event 
  to the application. The application ID of the target is retrieved from the
  connection-application table.
   
===========================================================================*/
void bt_pf_goep_cli_set_path_cfm_cb( OI_OBEXCLI_CONNECTION_HANDLE conn_id,
                                     OI_OBEX_HEADER_LIST *rsp_headers_ptr,
                                     OI_STATUS status )
{
  bt_app_id_type    app_id;
  bt_ev_msg_type    event;

  app_id = bt_pf_goep_cli_app_conn_find( 
             (bt_pf_goep_cli_conn_id_type)conn_id );

  if (app_id == BT_PF_APP_ID_NULL)           
  {
    BT_ERR( "BT PF GOEP CLI CON CFM CB: app not found, Conn: %x",
            conn_id, 0, 0 );
    return;
  }
  BT_MSG_DEBUG( "BT PF GOEP CLI SET PATH CFM: Conn: %x, App: %x, Status: %x", 
                conn_id, app_id, status );
  event.ev_hdr.ev_type = BT_EV_PF_GOEP_CLI_SET_PATH_CFM;
  event.ev_hdr.bt_app_id = app_id;
  event.ev_msg.ev_goep_cli_set_path_cfm.conn_id = 
    (bt_pf_goep_cli_conn_id_type)conn_id;          
  if ( rsp_headers_ptr != NULL )
  {
    event.ev_msg.ev_goep_cli_set_path_cfm.rsp_headers_cnt = 
      rsp_headers_ptr->count;
    event.ev_msg.ev_goep_cli_set_path_cfm.rsp_headers_ptr =
      (bt_pf_goep_obex_hdr_type*)(rsp_headers_ptr->list);
  }
  else
  {
    event.ev_msg.ev_goep_cli_set_path_cfm.rsp_headers_cnt = 0;
    event.ev_msg.ev_goep_cli_set_path_cfm.rsp_headers_ptr = NULL;
  }
  event.ev_msg.ev_goep_cli_set_path_cfm.status = 
    (bt_cmd_status_type)status;

  bt_ec_send_event( &event );

  /* Trick the OI_FreeIf inside PutResponse into NOT freeing the header list */
  /* The application will need to free the list when done */
  if ( rsp_headers_ptr != NULL )
  {
    rsp_headers_ptr->list = NULL;
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pfgoep_cli_rc_rcv_ind

DESCRIPTION
  This function performs the same operation as RfcommRecvDataInd.
  The difference is that this function takes a watermark pointer as input 
  parameter instead of OI_BYTE *dataBuf and OI_UINT16 dataLen. It calls 
  bt_pfgoep_cmn_reassemble_pkt instead of OI_OBEXCOMMON_ReassemblePacket 
  which takes in dataBuf and dataLen.

===========================================================================*/
void bt_pfgoep_cli_rc_rcv_ind(
  OI_RFCOMM_LINK_HANDLE link, 
  dsm_watermark_type* wm_ptr ) 
{
  OBEXCLI_CONNECTION *connection = LookupConnection( link );
  OI_STATUS status;

  if (connection == NULL)
  {
    OI_LOG_ERROR(("OBEX received data for unknown connection"));
    return;
  }

  OI_DBGPRINT2(("OBEX client received data"));

  status = bt_pfgoep_reassemble_pkt( &connection->common, wm_ptr );
  if (!OI_SUCCESS(status)) 
  {
    OI_DBGPRINT(("OBEX client receive error %!", status));
    ConnectionError(connection, status);
    return;
  }

  if (connection->common.rcvState == OI_OBEX_RCV_COMPLETE) 
  {
    OI_DBGPRINT2(("OBEX client packet complete"));
    /*
     * If there is a packet being sent, we must defer processing this
     * response until we have confirmation that the send has completed.
     */
    if (connection->common.mbuf != NULL) 
    {
      OI_DBGPRINT2(("OBEX server postponing response"));
    }
    else 
    {
      HandleResponsePkt( connection );
    }
  }
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_cli_abort_operation

DESCRIPTION
  Terminate the current put or get operation on this connection.

  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_cli_abort_operation(
  bt_app_id_type              app_id,
  bt_pf_goep_cli_conn_id_type conn_id )
{
  BT_MSG_API( "BT PF GOEP CLI CMD: abort, App: %x", app_id, 0, 0 );
  return(OI_OBEXCLI_AbortOperation( (OI_OBEXCLI_CONNECTION_HANDLE)conn_id ));
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_cli_init_data

DESCRIPTION
  This command initializes client Authentication data.
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/

void bt_cmd_pf_goep_cli_init_data(void)
{
    uint16 i = 0;
    for(i = 0 ; i < OI_MAX_NUM_APPS ; i++)
    {
        OI_OBEXCLI_InitData(i);
    }
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_cli_authenticate

DESCRIPTION
  This command initiates client OBEX authentication.

  This command is returned immediately and is not placed on the command queue.

===========================================================================*/

bt_cmd_status_type bt_cmd_pf_goep_cli_authenticate(
    bt_bd_addr_type         *bd_addr_ptr,
    uint8                   channel,
    bt_pf_realm_info_type   *realm_info_ptr,
    boolean                 uid_reqd,
    boolean                 full_access )
{
    return OI_OBEXCLI_Authenticate((OI_BD_ADDR *)bd_addr_ptr,
                                   channel,
                                   (OI_REALM_INFO *)realm_info_ptr,
                                   uid_reqd,
                                   full_access);
}
#endif /* FEATURE_BT_EXTPF_GOEP */
#endif /* FEATURE_BT */         
