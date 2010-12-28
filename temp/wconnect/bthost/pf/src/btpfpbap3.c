/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H   P H O N E B O O K   A C C E S S   P R O F I L E   
                                C L I E N T

GENERAL DESCRIPTION
  This module contains the Bluetooth Phone Book Access Profile server 
  data and code.

Copyright (c) 2006-2008 QUALCOMM Incorporated.               
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfpbap3.c#1 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-06-19   rs  Added relevent checks during the cleanup process for incoming malformed
                   commands.
*
*    #6        04 Dec 2007            MH
* Added to support BD address with connect indication.
*
*    #5        21 May 2007            SSK
* Fix compiler warnings.
*
*    #4        03 Jan 2006            GS
* Initialize SCN paramter in server registration function call.
*
*    #3        16 Jun 2006            DSN
* Removed superfluous header file inclusion.
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
#include "btpf.h"
#include "bti.h"
#include "btmsg.h"
#include "btpfpbapi.h"
#include "btsd.h"
#include "rex.h"
#include "pbap_srv.h"       

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

typedef uint16 bt_pf_pbap_srv_conn_id_type;

typedef struct
{
  bt_app_id_type              app_id;
  bt_pf_pbap_srv_conn_id_type conn_id;
  uint8                       scn;
} bt_pf_pbap_srv_info_type;

/* only one server of each type is supported */
LOCAL bt_pf_pbap_srv_info_type bt_pf_pbap_srv_info =
  { BT_PF_APP_ID_NULL, BT_PF_PBAP_NO_CONN_ID, BT_SPP_SCN_UNSPECIFIED };

/* data structures for passing parameters into PBAP calls */  
LOCAL PBAP_PULL_PHONE_BOOK_PARAMS    bt_pf_pbap_srv_pull_phone_book_params;
LOCAL PBAP_PULL_VCARD_LISTING_PARAMS bt_pf_pbap_srv_pull_vcard_listing_params;
LOCAL PBAP_PULL_VCARD_ENTRY_PARAMS   bt_pf_pbap_srv_pull_vcard_entry_params;


/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_connect_ind_cb

DESCRIPTION
  Callback function invoked when an incoming connection request to the
  imaging responder is received.

===========================================================================*/
LOCAL OI_STATUS bt_pf_pbap_srv_connect_ind_cb( 
  PBAPSRV_HANDLE conn_id,
  byte*          user_id,
  uint8          user_id_len,
  OI_BD_ADDR*    client_addr_ptr )
{
  bt_ev_msg_type event;

  BT_MSG_DEBUG( "BT PF PBAP SRV CB RX, con ind, conn: %x", conn_id, 0, 0 );

  if ( bt_pf_pbap_srv_info.conn_id != BT_PF_PBAP_NO_CONN_ID && 
       bt_pf_pbap_srv_info.conn_id != (bt_pf_pbap_srv_conn_id_type)conn_id )
  {
    /* authentication is in progress on another connection */
    BT_ERR( "BT PF PBAP SRV CB RX: con ind, unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_NO_MORE_CONNECTIONS);
  }

  event.ev_hdr.ev_type = BT_EV_PF_PBAP_SRV_CON_IND;
  event.ev_hdr.bt_app_id = bt_pf_pbap_srv_info.app_id;
  event.ev_msg.ev_pbap_srv_con_ind.conn_id = 
    (bt_pf_pbap_srv_conn_id_type)conn_id;
  bt_pf_copy_ascii_str((char*) event.ev_msg.ev_pbap_srv_con_ind.user_id, 
                       (char*) user_id, BT_PF_OBEX_MAX_USERID_LEN);
                           
  event.ev_msg.ev_pbap_srv_con_ind.user_id_len = user_id_len;
  
  event.ev_msg.ev_pbap_srv_con_ind.bd_addr = *(bt_bd_addr_type*)client_addr_ptr;

  bt_pf_pbap_srv_info.conn_id = (bt_pf_pbap_srv_conn_id_type)conn_id;

  bt_ec_send_event( &event );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_disconnect_ind_cb

DESCRIPTION
  Callback function invoked when a remote PBAP client is disconnecting from 
  the local PBAP server.

===========================================================================*/
LOCAL void bt_pf_pbap_srv_disconnect_ind_cb( PBAPSRV_HANDLE conn_id )
{
  bt_ev_msg_type    event;

  if (bt_pf_pbap_srv_info.app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF PBAP SRV CB RX: dcn, conn: %x", conn_id, 0, 0 );                         
    event.ev_hdr.bt_app_id = bt_pf_pbap_srv_info.app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_SRV_DCN_IND;
    event.ev_msg.ev_pbap_srv_con_ind.conn_id = 
      (bt_pf_pbap_srv_conn_id_type)conn_id;
    bt_ec_send_event( &event );

    bt_pf_pbap_srv_info.conn_id = BT_PF_PBAP_NO_CONN_ID;
  }
  else
  {
    BT_ERR( "BT PF PBAP SRV CB RX: dcn, unknown conn: %x", conn_id, 0, 0 );
  }

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_auth_ind_cb

DESCRIPTION
  Callback function invoked to indicate an authentication request from a
  remote PBAP client.

===========================================================================*/
LOCAL void bt_pf_pbap_srv_auth_ind_cb( PBAPSRV_HANDLE conn_id )
{
  bt_ev_msg_type    event;                                          

  if (bt_pf_pbap_srv_info.app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF PBAP SRV CB RX: auth, conn: %x, app: %x", 
                  conn_id, bt_pf_pbap_srv_info.app_id, 0 );
  
    bt_pf_pbap_srv_info.conn_id = (bt_pf_pbap_srv_conn_id_type)conn_id;
  
    event.ev_hdr.bt_app_id = bt_pf_pbap_srv_info.app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_SRV_AUTH_IND;
    event.ev_msg.ev_pbap_srv_auth_ind.conn_id = 
      (bt_pf_pbap_srv_conn_id_type)conn_id;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF PBAP SRV CB RX: auth, unexpected conn: %x", conn_id, 0, 0 );
  }

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_pull_phone_book_ind_cb

DESCRIPTION
  Callback function invoked to indicate a pull phone book request from a
  remote PBAP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_pbap_srv_pull_phone_book_ind_cb(
  PBAPSRV_HANDLE               conn_id,
  PBAP_PULL_PHONE_BOOK_PARAMS  *params_ptr,
  PBAP_REQUEST_STATE           state,
  OI_STATUS                    status)
{                            
  bt_ev_msg_type    event;
  uint8             obj_name_len;
  OI_STATUS         ret_status = OI_OK;

  if (bt_pf_pbap_srv_info.app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF PBAP SRV CB RX: PullPhoneBook, conn: %x, app: %x", 
                  conn_id, bt_pf_pbap_srv_info.app_id, 0 );                         

    event.ev_hdr.bt_app_id = bt_pf_pbap_srv_info.app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_SRV_PULL_PHONE_BOOK_IND;
    event.ev_msg.ev_pbap_srv_pull_phone_book_ind.conn_id = 
      (bt_pf_pbap_srv_conn_id_type)conn_id;
    event.ev_msg.ev_pbap_srv_pull_phone_book_ind.state = 
      (bt_pf_pbap_srv_req_state_type)state;
    event.ev_msg.ev_pbap_srv_pull_phone_book_ind.status = 
      (bt_cmd_status_type)status;

    if ( params_ptr == NULL ) 
    {
      /* Cleanup : To be safe with the obj names and len */
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.obj_name_len = 0;
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.obj_name[0] = 0;
      bt_ec_send_event( &event );
      return(ret_status);
    }

    if ( params_ptr->request.flags & PBAP_PARAM_FILTER ) 
    {
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.attribute_mask = 
        params_ptr->request.filter;

      BT_MSG_DEBUG( "    vcard attrib = %d", params_ptr->request.filter, 0, 0 );  
    }
    else
    {
      /* default to all attributes */
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.attribute_mask = 0;
    }

    if ( params_ptr->request.flags & PBAP_PARAM_FORMAT ) 
    {
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.format = 
        params_ptr->request.format;

      BT_MSG_DEBUG( "    vcard format = %d", params_ptr->request.format, 0, 0 );
    }
    else
    {
      /* default to vCard 2.1 format */
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.format = 0;
    }

    if ( params_ptr->request.flags & PBAP_PARAM_MAX_LIST_COUNT ) 
    {
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.max_list_count = 
        params_ptr->request.listCount;

      BT_MSG_DEBUG( "    list count = %d", params_ptr->request.listCount, 0, 0 );
    }
    else
    {
      /* default to 65535 */
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.max_list_count =
        0xFFFF;
    }

    if ( params_ptr->request.flags & PBAP_PARAM_LIST_START_OFFSET ) 
    {
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.list_start_offset = 
        params_ptr->request.startOffset;

      BT_MSG_DEBUG( "    start offset = %d", params_ptr->request.startOffset, 0, 0 );
    }
    else
    {
      /* default to 0 */
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.list_start_offset = 0;
    }

    if ( (params_ptr->request.objName == NULL) || 
         (params_ptr->request.objName->len == 0) )
    {
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.obj_name_len = 0;
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.obj_name[0] = 0;
    }
    else
    {
      /* copy name, truncate when necessary */
      obj_name_len = bt_pf_process_unicode_str(
        (uint16*)event.ev_msg.ev_pbap_srv_pull_phone_book_ind.obj_name,
        (bt_pf_unicode_str_type*)(params_ptr->request.objName),
        BT_PF_MAX_FILENAME_LEN ); 
      event.ev_msg.ev_pbap_srv_pull_phone_book_ind.obj_name_len = obj_name_len;
    }

    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF PBAP SRV CB RX: PullPhoneBook, unknown conn: %x", 
            conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_set_phone_book_ind_cb

DESCRIPTION
  Callback function invoked to indicate a set phone book request from a
  remote PBAP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_pbap_srv_set_phone_book_ind_cb(
  PBAPSRV_HANDLE               conn_id,
  OI_OBEX_UNICODE*             folder_ptr,
  OI_BOOL                      level)
{                            
  bt_ev_msg_type    event;
  uint8             folder_len;
  OI_STATUS         ret_status = OI_OK;

  if (bt_pf_pbap_srv_info.app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF PBAP SRV CB RX: SetPhoneBook, conn: %x, app: %x", 
                  conn_id, bt_pf_pbap_srv_info.app_id, 0 );                         

    event.ev_hdr.bt_app_id = bt_pf_pbap_srv_info.app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_SRV_SET_PHONE_BOOK_IND;
    event.ev_msg.ev_pbap_srv_set_phone_book_ind.conn_id = 
      (bt_pf_pbap_srv_conn_id_type)conn_id;
    event.ev_msg.ev_pbap_srv_set_phone_book_ind.level = (uint8) level;

    if ( (folder_ptr == NULL) || (folder_ptr->len == 0) )
    {
      folder_len = 0;
      event.ev_msg.ev_pbap_srv_set_phone_book_ind.folder_str[0] = 0;
    }
    else
    {
      /* copy name, truncate when necessary */
      folder_len = bt_pf_process_unicode_str(
        (uint16*)event.ev_msg.ev_pbap_srv_set_phone_book_ind.folder_str,
        (bt_pf_unicode_str_type*)folder_ptr,
        BT_PF_MAX_FILENAME_LEN );
    }
    event.ev_msg.ev_pbap_srv_set_phone_book_ind.folder_len = folder_len;

    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF PBAP SRV CB RX: SetPhoneBook, unknown conn: %x", 
            conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_pull_vcard_listing_ind_cb

DESCRIPTION
  Callback function invoked to indicate a pull vCard listing request from a
  remote PBAP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_pbap_srv_pull_vcard_listing_ind_cb(
  PBAPSRV_HANDLE                 conn_id,
  PBAP_PULL_VCARD_LISTING_PARAMS *params_ptr,
  PBAP_REQUEST_STATE             state,
  OI_STATUS                      status)
{                            
  bt_ev_msg_type    event;
  uint8             obj_name_len;
  OI_STATUS         ret_status = OI_OK;

  if (bt_pf_pbap_srv_info.app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF PBAP SRV CB RX: PullvCardListing, conn: %x, app: %x", 
                  conn_id, bt_pf_pbap_srv_info.app_id, 0 );                         

    event.ev_hdr.bt_app_id = bt_pf_pbap_srv_info.app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_SRV_PULL_VCARD_LISTING_IND;
    event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.conn_id = 
      (bt_pf_pbap_srv_conn_id_type)conn_id;
    event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.state = 
      (bt_pf_pbap_srv_req_state_type)state;
    event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.status = 
      (bt_cmd_status_type)status;

    if ( params_ptr == NULL ) 
    {
      /* Cleanup : To be safe with the obj names and len */
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.obj_name_len = 0;
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.obj_name[0] = 0;
      bt_ec_send_event( &event );
      return(ret_status);
    }

    if ( params_ptr->request.flags & PBAP_PARAM_ORDER ) 
    {
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.order = 
        params_ptr->request.order;

      BT_MSG_DEBUG( "    sort order = %d", params_ptr->request.order, 0, 0 );

    }
    else
    {
      /* default to "indexed" */
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.order =  0;
    }

    if ( params_ptr->request.flags & PBAP_PARAM_ATTRIBUTE ) 
    {
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.search_attrib = 
        params_ptr->request.searchAttrib;

      BT_MSG_DEBUG( "    search attrib = %d", params_ptr->request.searchAttrib, 0, 0 );
    }
    else
    {
      /* default to "name" */
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.search_attrib = 0;
    }

    if ( params_ptr->request.flags & PBAP_PARAM_MAX_LIST_COUNT ) 
    {
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.max_list_count = 
        params_ptr->request.listCount;

      BT_MSG_DEBUG( "    list count = %d", params_ptr->request.listCount, 0, 0 );
    }
    else
    {
      /* default to 655535 */
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.max_list_count =
        0xFFFF;
    }

    if ( params_ptr->request.flags & PBAP_PARAM_LIST_START_OFFSET ) 
    {
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.list_start_offset = 
        params_ptr->request.startOffset;

      BT_MSG_DEBUG( "    start offset = %d", params_ptr->request.startOffset, 0, 0 );
    }
    else
    {
      /* default to 0 */
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.list_start_offset = 0;
    }

    if ( (params_ptr->request.searchValue.data == NULL) || 
         (params_ptr->request.searchValue.len == 0) )
    {
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.value_len = 0;
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.value[0] = 0;
    }
    else
    {
      /* copy search value string */
      memcpy((void *) event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.value,
             (void *) params_ptr->request.searchValue.data,
             params_ptr->request.searchValue.len);

      /* null terminate */
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.value[
        params_ptr->request.searchValue.len] = '\0';
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.value_len = 
        params_ptr->request.searchValue.len;

      BT_MSG_DEBUG( "    search value = %s", 
                  event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.value, 0, 0 );
    }

    if ( (params_ptr->request.objName == NULL) || 
         (params_ptr->request.objName->len == 0) )
    {
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.obj_name_len = 0;
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.obj_name[0] = 0;
    }
    else
    {
      /* copy name, truncate when necessary */
      obj_name_len = bt_pf_process_unicode_str(
        (uint16*)event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.obj_name,
        (bt_pf_unicode_str_type*)(params_ptr->request.objName),
        BT_PF_MAX_FILENAME_LEN ); 
      event.ev_msg.ev_pbap_srv_pull_vcard_listing_ind.obj_name_len = 
        obj_name_len;
    }

    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF PBAP SRV CB RX: PullvCardListing, unknown conn: %x", 
            conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_pull_vcard_entry_ind_cb

DESCRIPTION
  Callback function invoked to indicate a pull vcard entry request from a
  remote PBAP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_pbap_srv_pull_vcard_entry_ind_cb(
  PBAPSRV_HANDLE                conn_id,
  PBAP_PULL_VCARD_ENTRY_PARAMS  *params_ptr,
  PBAP_REQUEST_STATE            state,
  OI_STATUS                     status)
{                            
  bt_ev_msg_type    event;
  uint8             obj_name_len;
  OI_STATUS         ret_status = OI_OK;

  if (bt_pf_pbap_srv_info.app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF PBAP SRV CB RX: PullvCardEntry, conn: %x, app: %x", 
                  conn_id, bt_pf_pbap_srv_info.app_id, 0 );                         

    event.ev_hdr.bt_app_id = bt_pf_pbap_srv_info.app_id;
    event.ev_hdr.ev_type = BT_EV_PF_PBAP_SRV_PULL_VCARD_ENTRY_IND;
    event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.conn_id = 
      (bt_pf_pbap_srv_conn_id_type)conn_id;
    event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.state = 
      (bt_pf_pbap_srv_req_state_type)state;
    event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.status = 
      (bt_cmd_status_type)status;

    if ( params_ptr == NULL ) 
    {
      /* Cleanup : To be safe with the obj names and len */
      event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.obj_name_len = 0;
      event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.obj_name[0] = 0;
      bt_ec_send_event( &event );
      return(ret_status);
    }

    if ( params_ptr->request.flags & PBAP_PARAM_FILTER ) 
    {
      event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.attribute_mask = 
        params_ptr->request.filter;

      BT_MSG_DEBUG( "    vcard attrib = %d", params_ptr->request.filter, 0, 0 );
    }
    else
    {
      /* default to 65535 */
      event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.attribute_mask = 
        0xFFFF;
    }

    if ( params_ptr->request.flags & PBAP_PARAM_FORMAT  ) 
    {
      event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.format = 
        params_ptr->request.format;

      BT_MSG_DEBUG( "    vcard format = %d", params_ptr->request.format, 0, 0 );
    }
    else
    {
      /* default to vCard 2.1 format */
      event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.format = 0;
    }

    if ( (params_ptr->request.objName == NULL) || 
         (params_ptr->request.objName->len == 0) )
    {
      event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.obj_name_len = 0;
      event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.obj_name[0] = 0;
    }
    else
    {
      /* copy name, truncate when necessary */
      obj_name_len = bt_pf_process_unicode_str(
        (uint16*)event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.obj_name,
        (bt_pf_unicode_str_type*)(params_ptr->request.objName),
        BT_PF_MAX_FILENAME_LEN ); 
      event.ev_msg.ev_pbap_srv_pull_vcard_entry_ind.obj_name_len = obj_name_len;
    }

    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF PBAP SRV CB RX: PullvCardEntry, unknown conn: %x", 
            conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}
 
LOCAL PBAPSRV_CALLBACKS bt_pf_pbap_srv_cb_list =
{
  bt_pf_pbap_srv_connect_ind_cb,
  bt_pf_pbap_srv_disconnect_ind_cb,
  bt_pf_pbap_srv_auth_ind_cb,
  bt_pf_pbap_srv_pull_phone_book_ind_cb,
  bt_pf_pbap_srv_set_phone_book_ind_cb,
  bt_pf_pbap_srv_pull_vcard_listing_ind_cb,
  bt_pf_pbap_srv_pull_vcard_entry_ind_cb
};


/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_deregister

DESCRIPTION
  Deregisters a PBAP server specified by the server channel number and removes
  the associated service record from the SDP database.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_srv_cmd_deregister( bt_app_id_type app_id )
{
  OI_STATUS           status;

  if (bt_pf_pbap_srv_info.app_id == BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF PBAP SRV DEREG: not reg, App: %x, Stat: %x",
                  app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }

  if (bt_pf_pbap_srv_info.app_id != app_id)
  {
    BT_MSG_DEBUG( "BT PF PBAP SRV DEREG: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = PBAPSRV_DeregisterServer( bt_pf_pbap_srv_info.scn );
  if (status == OI_OK)
  {
    bt_pf_pbap_srv_info.scn = BT_SPP_SCN_UNSPECIFIED;
    bt_pf_pbap_srv_info.app_id = BT_PF_APP_ID_NULL;
  }
  else
  {
    BT_ERR( "BT PF PBAP SRV DEREG: returns %x, scn: %x", 
            status, bt_pf_pbap_srv_info.scn, 0 );
  }

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_cmd_auth_rsp

DESCRIPTION
  Responds to an authentication request from a remote PBAP client.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_srv_cmd_auth_rsp(
  bt_app_id_type                    app_id,
  bt_pf_cmd_pbap_srv_auth_rsp_type* params_ptr )
{
  if (bt_pf_pbap_srv_info.app_id != app_id)
  {
    BT_ERR( "BT PF PBAP SRV AUTH RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  PBAPSRV_AuthenticationResponse(
    (PBAPSRV_HANDLE)params_ptr->conn_id,
    (OI_CHAR*)params_ptr->pin_str );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_cmd_pull_phone_book_rsp

DESCRIPTION
  Responds to a pull phone book request from a remote PBAP client.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_srv_cmd_pull_phone_book_rsp(
  bt_app_id_type                                app_id,
  bt_pf_cmd_pbap_srv_pull_phone_book_rsp_type*  params_ptr )
{
  OI_STATUS status; 

  if (bt_pf_pbap_srv_info.app_id != app_id)
  {
    BT_ERR( "BT PF PBAP SRV PL PB RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  /* initialize response app. params flags */
  bt_pf_pbap_srv_pull_phone_book_params.response.flags = PBAP_PARAM_NONE;

  if ( params_ptr->phone_book_size_ptr ) 
  {
    bt_pf_pbap_srv_pull_phone_book_params.response.flags |= 
      PBAP_PARAM_PHONE_BOOK_SIZE;
    bt_pf_pbap_srv_pull_phone_book_params.response.phoneBookSize = 
      *(params_ptr->phone_book_size_ptr);
  }

  if ( params_ptr->num_missed_calls_ptr ) 
  {
    bt_pf_pbap_srv_pull_phone_book_params.response.flags |=
      PBAP_PARAM_NEW_MISSED_CALLS;
    bt_pf_pbap_srv_pull_phone_book_params.response.missedCalls = 
      *(params_ptr->num_missed_calls_ptr);
  }

  bt_pf_pbap_srv_pull_phone_book_params.response.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_pbap_srv_pull_phone_book_params.response.final = 
    params_ptr->final;

  status = PBAPSRV_PullPhoneBookResponse(
    (PBAPSRV_HANDLE)params_ptr->conn_id,
    &bt_pf_pbap_srv_pull_phone_book_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_cmd_set_phone_book_rsp

DESCRIPTION
  Responds to a set phone book request from a remote PBAP client.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_srv_cmd_set_phone_book_rsp(
  bt_app_id_type                               app_id,
  bt_pf_cmd_pbap_srv_set_phone_book_rsp_type*  params_ptr )
{
  OI_STATUS status; 

  if (bt_pf_pbap_srv_info.app_id != app_id)
  {
    BT_ERR( "BT PF PBAP SRV Set PB RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  status = PBAPSRV_SetPhoneBookResponse(
    (PBAPSRV_HANDLE)params_ptr->conn_id,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_cmd_pull_phone_book_rsp

DESCRIPTION
  Responds to a pull phone book request from a remote PBAP client.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_srv_cmd_pull_vcard_listing_rsp(
  bt_app_id_type                                app_id,
  bt_pf_cmd_pbap_srv_pull_vcard_list_rsp_type*  params_ptr )
{
  OI_STATUS status; 

  if (bt_pf_pbap_srv_info.app_id != app_id)
  {
    BT_ERR( "BT PF PBAP SRV PL VL RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  /* initialize response app. params flags */
  bt_pf_pbap_srv_pull_vcard_listing_params.response.flags = PBAP_PARAM_NONE;

  if ( params_ptr->phone_book_size_ptr ) 
  {
    bt_pf_pbap_srv_pull_vcard_listing_params.response.flags |= 
      PBAP_PARAM_PHONE_BOOK_SIZE;
    bt_pf_pbap_srv_pull_vcard_listing_params.response.phoneBookSize = 
      *(params_ptr->phone_book_size_ptr);
  }

  if ( params_ptr->num_missed_calls_ptr ) 
  {
    bt_pf_pbap_srv_pull_vcard_listing_params.response.flags |=
      PBAP_PARAM_NEW_MISSED_CALLS;
    bt_pf_pbap_srv_pull_vcard_listing_params.response.missedCalls = 
      *(params_ptr->num_missed_calls_ptr);
  }

  bt_pf_pbap_srv_pull_vcard_listing_params.response.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_pbap_srv_pull_vcard_listing_params.response.final = 
    params_ptr->final;

  status = PBAPSRV_PullvCardListingResponse(
    (PBAPSRV_HANDLE)params_ptr->conn_id,
    &bt_pf_pbap_srv_pull_vcard_listing_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_cmd_pull_vcard_entry_rsp

DESCRIPTION
  Responds to a pull phone book request from a remote PBAP client.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_srv_cmd_pull_vcard_entry_rsp(
  bt_app_id_type                                app_id,
  bt_pf_cmd_pbap_srv_pull_vcard_entry_rsp_type* params_ptr )
{
  OI_STATUS status; 

  if (bt_pf_pbap_srv_info.app_id != app_id)
  {
    BT_ERR( "BT PF PBAP SRV PL VE RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  bt_pf_pbap_srv_pull_vcard_entry_params.response.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_pbap_srv_pull_vcard_entry_params.response.final = 
    params_ptr->final;

  status = PBAPSRV_PullvCardEntryResponse(
    (PBAPSRV_HANDLE)params_ptr->conn_id,
    &bt_pf_pbap_srv_pull_vcard_entry_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_pbap_srv_cmd_register

DESCRIPTION
  Initialize the PBAP imaging responder and register it in the service discovery 
  database.

===========================================================================*/
bt_cmd_status_type bt_pf_pbap_srv_cmd_register(
  bt_app_id_type           app_id,
  char*                    srv_name_ptr,        
  uint8                    supported_repositories,
  bt_pf_goep_srv_auth_type auth )
{
  OI_STATUS                        status = OI_OK;
  PBAPSRV_INFO                     srv_info;
  uint8                            scn = 0; /* implicitly select BT 
                                               transport in 
                                               OI_RFCOMM_RegisterServer() */

  TASKLOCK();

  if (bt_pf_pbap_srv_info.app_id != BT_PF_APP_ID_NULL)
  {
    status = OI_STATUS_ALREADY_REGISTERED;
  }
  else
  {
    bt_pf_pbap_srv_info.app_id = app_id;
  }

  TASKFREE();

  if (status != OI_OK)
  {
    BT_ERR( "BT PF PBAP SRV REG: App %x, Stat: %x", app_id, status, 0 );
    return(status);
  }

  srv_info.supportedRepositories = supported_repositories;

  status = PBAPSRV_Register(
    &bt_pf_pbap_srv_cb_list,
    &srv_info,
    NULL, /* SDP registration not done here */
    (OI_OBEXSRV_AUTHENTICATION)auth,
    &scn );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF PBAP SRV REG: returns %x, App %x", status, app_id, 0 );
    bt_pf_pbap_srv_info.app_id = BT_PF_APP_ID_NULL;
    return( status);
  }                                                    

  /* Updates service record with additional profile specific attributes */
  status = bt_cmd_sd_update_obex_profile_service_record(
             app_id,
             BT_SD_SERVICE_CLASS_PBAP_PSE,
             scn,
             srv_name_ptr,
             NULL, /* service ID */
             NULL, /* supported formats list */
             supported_repositories, /* using supported_capabilities param */
             0,
             0,
             0 );

  if (status != BT_CS_GN_PENDING)
  {
    BT_ERR( "BT PF PBAP SRV REG: update srv rec failed, App %x", 
            app_id, 0, 0 );
    PBAPSRV_DeregisterServer( scn );
    return( status );
  }
                                
  bt_pf_pbap_srv_info.scn = scn;

  return( OI_OK );
}

#endif /* FEATURE_BT_EXTPF_PBAP */
#endif /* FEATURE_BT */
