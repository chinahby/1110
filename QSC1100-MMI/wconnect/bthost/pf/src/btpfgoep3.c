    /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               B L U E T O O T H   G E N E R I C   O B J E C T   
          
                E X C H A N G E   P R O F I L E   S E R V E R

GENERAL DESCRIPTION
  This module contains the Bluetooth Generic Object Exchange Profile server 
  data and code.

Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfgoep3.c#1 $ 

$Log: $

*   #25         21 Mar  2007           MH
* Added realm string type.
*
*   #24         28 Feb  2007           MH
* Modified disconnect indication to support optional headers
*
*   #23         13 Feb  2007           MH
* Modified set path indication to support optional headers
*
*   #22         29 Jan  2007           MH
* Added support for 128-bit UUID and update OBEX Profile service record.
*
*    #21        15 Dec 2006           MH
* Check provided in bt_cmd_pf_goep_srv_register before freeing memory
*
*    #20        26 Oct 2006           MH
* Modified srv conn ind call back to support all headers
*
*    #19        19 Sept 2006           MH
* Server authentication command to support sever initiating authentication.
*
*    #18        01 Aug 2006           BK
* Removed compiler warnings.
*
*    #17        28 Jul 2006           BK
* Modified for obex client authentication
*
*    #15-16     10 Jul 2006           JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #14        23 Jun 2006           JH
* Modified to support updated GOEP API for JSR82.
* Uncommented lines in bt_pf_goep_srv_put_ind_cb() and 
*  bt_pf_goep_srv_put_ind_cb() to prevent premature freeing of headers list.
* Add SD service record when GOEP server is registered.
*
*    #13        22 Oct 2005           JH
* Replaced FEATURE_PCAA with FEATURE_PCAA_SIRIUS.
*
*    #12        02 Sep 2005           RY
* Merged in Sirius changes for FEATURE_PCAA_SIRIUS for "+++" detection.
*
*    #11        31 May 2005           RY
* Merged in Sirius changes.
*
*    #10       19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #9        06 Dec 2004            JH
* Added NULL pointer checking in callback functions.
*
*    #8        05 Nov 2004            JH
* Added connection ID to set path indication event. 
*
*    #7        30 Aug 2004            JH
* Removed calls for manipulating object transfer bit in class of device field;
*  this is now handled by SD.
*
*    #6        14 May 2004            JH
* Added call to set/clear object transfer bit in class of device field during
*  registration/deregistration.
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
#include "obexsrv.c"

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif
                                                 
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/*-------------------------------------------------------------------------*/
/*                Target-Application Table Declaration                     */
/*-------------------------------------------------------------------------*/

/* 
  Each OBEX server can have multiple targets. Each target corresponding to
  one specific OBEX service the OBEX server can provide.
  There is one entry in the target-application table for each target
  registered on this device. For example, if there are 2 servers each with
  2 targets registered, this table will have 4 entries in use.
  
  Each entry in this table is used for mapping the application which registers
  the server/target. It also stores the target header which identifies the
  service, and the callback function pointers required by the OI OBEX driver.
  
  The mapping of the application ID and server/target allows events being
  generated to be routed to the proper application.
*/
typedef struct
{
  /* application ID identifies the application which registers this target */
  bt_app_id_type     app_id;

  /* RFCOMM channel number identifying the OBEX server hosting the target */
  uint8              ch_num;

  /* target ID identifies which target under the OBEX server */
  uint8              target_id; 

  /* list of cb function pointers passed into OI OBEX driver for this target */
  OI_OBEXSRV_CB_LIST cb_list;

  /* pointer to the memory location which stores the target header string */
  byte*              target_str_ptr;
} bt_pf_goep_srv_target_app_item_type;

LOCAL bt_pf_goep_srv_target_app_item_type 
  bt_pf_goep_srv_target_app_table[BT_PF_SIO_MAX_PORTS];

#define BT_PF_GOEP_SRV_APP_CONN_FULL  -1
#define BT_PF_GOEP_SRV_CH_NOT_FOUND   -2
#define BT_PF_GOEP_SRV_TARGET_ID_NULL 0xFF
#define BT_PF_GOEP_SRV_CH_NUM_NULL    0xFF
                         
/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_goep_srv_add_server    
   
DESCRIPTION
  Allocates an entry in the target-application table for a OBEX server.
   
===========================================================================*/
LOCAL int16 bt_pf_goep_srv_add_server( bt_app_id_type    app_id )
{
  int16 i;                                    

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    if (bt_pf_goep_srv_target_app_table[i].app_id == BT_PF_APP_ID_NULL)
    {
      bt_pf_goep_srv_target_app_table[i].app_id = app_id;
      bt_pf_goep_srv_target_app_table[i].target_id = 0;
      BT_MSG_DEBUG( "BT PF GOEP SRV: add srv to table, App: %x", 
                    app_id, 0, 0 );
      /* ch_num must be written by calling function when known */
      return(i);
    }
  }
  BT_ERR( "BT PF GOEP SRV: add srv, table full, App: %x", 
              app_id, 0, 0 );
  return(BT_PF_GOEP_SRV_APP_CONN_FULL);
}

/*===========================================================================

FUNCTION
  bt_pf_goep_srv_add_target    
   
DESCRIPTION
  Allocates an entry in the target-application table for a secondary target 
  of a server which is already registered.
   
===========================================================================*/
LOCAL int16 bt_pf_goep_srv_add_target( bt_app_id_type    app_id,
                                        uint8 ch_num, uint8* target_id_ptr )
{
  int16 i;
  int16 index = BT_PF_GOEP_SRV_APP_CONN_FULL;
  uint8 target_id = 0;

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    /* first locate an empty table entry */
    if ( (bt_pf_goep_srv_target_app_table[i].app_id == BT_PF_APP_ID_NULL) &&
         (index == BT_PF_GOEP_SRV_APP_CONN_FULL) )
    {
      index = i;
    }
    /* also need to make sure that the server identified by ch_num has
       already been registered; if it has been registered, the target
       ID will be the previously assigned target ID + 1 */
    if (bt_pf_goep_srv_target_app_table[i].ch_num == ch_num)
    {
      if (target_id <= bt_pf_goep_srv_target_app_table[i].target_id)
      {
        target_id = bt_pf_goep_srv_target_app_table[i].target_id + 1;
      }
    }
  }
  if (index != BT_PF_GOEP_SRV_APP_CONN_FULL) /* free table entry available */
  {
    if (target_id > 0) /* server has been registered */
    {
      bt_pf_goep_srv_target_app_table[index].app_id = app_id;
      bt_pf_goep_srv_target_app_table[index].ch_num = ch_num;
      bt_pf_goep_srv_target_app_table[index].target_id = target_id;
      *target_id_ptr = target_id;
      BT_MSG_DEBUG( "BT PF GOEP SRV: add targ to table, App: %x, Ch: %x, \
                     Target: %x", app_id, ch_num, target_id );
    }
    else /* server has not been registered */
    {
      *target_id_ptr = BT_PF_GOEP_SRV_TARGET_ID_NULL;
      BT_ERR( "BT PF GOEP SRV: add targ, srv not reg, App: %x", 
              app_id, 0, 0 ); 
      return(BT_PF_GOEP_SRV_CH_NOT_FOUND);
    }
  }
  else
  {
    BT_ERR( "BT PF GOEP SRV: add targ, table full, App: %x, Ch: %x", 
                app_id, ch_num, 0 );
  }
  return(index);
}

/*===========================================================================

FUNCTION
  bt_pf_goep_srv_remove_server    
   
DESCRIPTION
  Removes all entries in the target-application table corresponding to the
  server. Entries for secondary targets registered are also removed.
   
===========================================================================*/
LOCAL int16 bt_pf_goep_srv_remove_server( bt_app_id_type    app_id, 
                                           uint8 ch_num )
{
  int16 i, ret = BT_PF_GOEP_SRV_CH_NOT_FOUND;                                    

  for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
  {
    if ( (bt_pf_goep_srv_target_app_table[i].ch_num == ch_num) &&
         (bt_pf_goep_srv_target_app_table[i].app_id == app_id) )
    {
      BT_MSG_DEBUG( "BT PF GOEP SRV: rm srv from table, App: %x, Ch: %x, \
                    Target: %x", app_id, ch_num, 
                    bt_pf_goep_srv_target_app_table[i].target_id );
      bt_pf_goep_srv_target_app_table[i].app_id = BT_PF_APP_ID_NULL;
      bt_pf_goep_srv_target_app_table[i].ch_num = BT_PF_GOEP_SRV_CH_NUM_NULL;
      bt_pf_goep_srv_target_app_table[i].target_id = BT_PF_GOEP_SRV_TARGET_ID_NULL;
      if (bt_pf_goep_srv_target_app_table[i].target_str_ptr != NULL )
      {
        mem_free( &bt_pf_heap, bt_pf_goep_srv_target_app_table[i].target_str_ptr );
      }
      /* found at least one entry for that server */
      ret = OI_OK;
    }
  }
  return(ret);
}

/*===========================================================================

FUNCTION
  bt_pf_goep_srv_oi_auth    
   
DESCRIPTION
  Returns the corresponding OI defined authentication type.
   
===========================================================================*/
LOCAL OI_OBEXSRV_AUTHENTICATION bt_pf_goep_srv_oi_auth(
  bt_pf_goep_srv_auth_type auth )
{
  OI_OBEXSRV_AUTHENTICATION oi_auth;
  switch(auth)
  { 
    case BT_PF_GOEP_SRV_AUTH_USERID_AND_PASSWORD:
      oi_auth = OI_OBEXSRV_AUTH_USERID_AND_PASSWORD;
      break;

    case BT_PF_GOEP_SRV_AUTH_PASSWORD:
      oi_auth = OI_OBEXSRV_AUTH_PASSWORD;
      break;

    default:
    case BT_PF_GOEP_SRV_AUTH_NONE:
      oi_auth = OI_OBEXSRV_AUTH_NONE;
  }
  return(oi_auth);
}

/*===========================================================================

FUNCTION
  bt_pf_goep_srv_con_ind_cb    
   
DESCRIPTION
  This function handles the generation of the connection indication event to
  the application. The application ID of the target is retrieved from the
  target-application table.
   
===========================================================================*/
void bt_pf_goep_srv_con_ind_cb( uint16 index,
                                OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                OI_BOOL unauthorized,
                                OI_BYTE *user_id_ptr,
                                OI_UINT8 user_id_len,
                                OI_OBEX_HEADER_LIST *cmd_headers_ptr)
{  
  uint8 ch_num;
  uint8 target_id;
  bt_app_id_type    app_id;
  bt_ev_msg_type event;

  ch_num = bt_pf_goep_srv_target_app_table[index].ch_num;
  target_id = bt_pf_goep_srv_target_app_table[index].target_id;
  app_id = bt_pf_goep_srv_target_app_table[index].app_id;
  BT_MSG_DEBUG( "BT PF GOEP SRV CON IND CB %x: Ch: %x, Target: %x", 
              index, ch_num, target_id );
  BT_MSG_DEBUG( " Conn: %x, App: %x", conn_id, app_id, 0 );

  event.ev_hdr.ev_type = BT_EV_PF_GOEP_SRV_CON_IND;
  event.ev_hdr.bt_app_id = app_id;

  event.ev_msg.ev_goep_srv_con_ind.ch_num = ch_num;
  event.ev_msg.ev_goep_srv_con_ind.target_id = target_id;
  event.ev_msg.ev_goep_srv_con_ind.conn_id = 
    (bt_pf_goep_srv_conn_id_type)conn_id;
  event.ev_msg.ev_goep_srv_con_ind.unauthorized = unauthorized;

  /* OBEX should have rejected auth digest with invalid user id length */
  if (user_id_len > BT_PF_OBEX_MAX_USERID_LEN)
  {
    /* truncate just in case */
    user_id_len = BT_PF_OBEX_MAX_USERID_LEN;
  }
  else if ( user_id_ptr == NULL )
  {
    user_id_len = 0;
  }

  if ( user_id_len > 0 )
  {                  
    memcpy( (void*)event.ev_msg.ev_goep_srv_con_ind.user_id,
            (void*)user_id_ptr,
            user_id_len );
  }
  event.ev_msg.ev_goep_srv_con_ind.user_id_len = user_id_len;

  if ( cmd_headers_ptr != NULL )
  {
    event.ev_msg.ev_goep_srv_con_ind.cmd_headers_cnt = cmd_headers_ptr->count;
    event.ev_msg.ev_goep_srv_con_ind.cmd_headers_ptr = 
      (bt_pf_goep_obex_hdr_type*)(cmd_headers_ptr->list);
  }
  else
  {
    event.ev_msg.ev_goep_srv_con_ind.cmd_headers_cnt = 0;
    event.ev_msg.ev_goep_srv_con_ind.cmd_headers_ptr = NULL;
  }

  if ( cmd_headers_ptr != NULL )
  {
    /* Trick the OI_FreeIf inside Connect command into NOT freeing the header list */
    /* The application will need to free the list when done */
    cmd_headers_ptr->list = NULL;
  }

  bt_ec_send_event( &event );
}
/*===========================================================================
 FUNCTION
  bt_pf_goep_srv_auth_ind_cb
DESCRIPTION
  This function handles the generation of the authentication request event to
  the application. The application ID of the target is retrieved from the
  target-application table.
===========================================================================*/

OI_STATUS bt_pf_goep_srv_auth_ind_cb( uint16 index,
                                 OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                 OI_BOOL full_access,
                                 OI_BYTE *user_id_ptr,
                                 OI_BOOL  user_id_flag,
                                 OI_UINT8 user_id_len,
                                 OI_REALM_INFO *realm_info_ptr)

{

   uint8 ch_num;
   uint8 target_id;
   bt_app_id_type    app_id;
   bt_ev_msg_type event;

   ch_num = bt_pf_goep_srv_target_app_table[index].ch_num;
   target_id = bt_pf_goep_srv_target_app_table[index].target_id;
   app_id = bt_pf_goep_srv_target_app_table[index].app_id;
   BT_MSG_DEBUG( "BT PF GOEP SRV AUTH IND CB %x: Ch: %x, Target: %x",
               index, ch_num, target_id );
   BT_MSG_DEBUG( " Conn: %x, App: %x", conn_id, app_id, 0 );

   event.ev_hdr.ev_type = BT_EV_PF_GOEP_SRV_AUTH_IND;
   event.ev_hdr.bt_app_id = app_id;
   event.ev_msg.ev_goep_srv_auth_req_ind.ch_num = ch_num;
   event.ev_msg.ev_goep_srv_auth_req_ind.target_id = target_id;
   event.ev_msg.ev_goep_srv_auth_req_ind.conn_id =
                            (bt_pf_goep_srv_conn_id_type)conn_id;
   event.ev_msg.ev_goep_srv_auth_req_ind.full_access = full_access;
   event.ev_msg.ev_goep_srv_auth_req_ind.user_id_rqd = user_id_flag ;

   /* OBEX should have rejected auth digest with invalid user id length */
   if (user_id_len > BT_PF_OBEX_MAX_USERID_LEN)
   {
     /* truncate just in case */
       user_id_len = BT_PF_OBEX_MAX_USERID_LEN;
   }
   else if ( user_id_ptr == NULL )
   {
       user_id_len = 0;
   }

   if ( user_id_len > 0 )
   {
       memcpy( (void*)event.ev_msg.ev_goep_srv_auth_req_ind.user_id,
               (void*)user_id_ptr,
               user_id_len );
  }

   event.ev_msg.ev_goep_srv_auth_req_ind.user_id_len = user_id_len;
    
   if (realm_info_ptr->realmLen > BT_PF_OBEX_MAX_REALM_LEN)
   {
     /* truncate just in case */
       realm_info_ptr->realmLen = BT_PF_OBEX_MAX_REALM_LEN;
   }
   else if ( realm_info_ptr->realmStr == NULL )
   {
       realm_info_ptr->realmLen = 0;
   }
   if(realm_info_ptr->realmLen)
   {
       event.ev_msg.ev_goep_srv_auth_req_ind.realm_str_type =realm_info_ptr->realmStrType ;
       memcpy( (void*)event.ev_msg.ev_goep_srv_auth_req_ind.realm_str,
               (void*)realm_info_ptr->realmStr,
               realm_info_ptr->realmLen );
   }

   event.ev_msg.ev_goep_srv_auth_req_ind.realm_len = realm_info_ptr->realmLen;

  bt_ec_send_event( &event );
  return( OI_OK );
}

/*===========================================================================

FUNCTION
  bt_pf_goep_srv_dcn_ind_cb    
   
DESCRIPTION
  This function handles the generation of the disconnection indication event 
  to the application. The application ID of the target is retrieved from the
  target-application table.
   
===========================================================================*/
void bt_pf_goep_srv_dcn_ind_cb( uint16 index, 
                                OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                OI_OBEX_HEADER_LIST *cmd_headers_ptr )
{
  uint8 ch_num;
  uint8 target_id;
  bt_app_id_type    app_id;
  bt_ev_msg_type event;

  ch_num = bt_pf_goep_srv_target_app_table[index].ch_num;
  target_id = bt_pf_goep_srv_target_app_table[index].target_id;
  app_id = bt_pf_goep_srv_target_app_table[index].app_id;
  BT_MSG_DEBUG( "BT PF GOEP SRV DCN IND CB %x: Ch: %x, Target: %x", 
              index, ch_num, target_id );
  BT_MSG_DEBUG( " Conn: %x, App: %x", conn_id, app_id, 0 );

  event.ev_hdr.ev_type = BT_EV_PF_GOEP_SRV_DCN_IND;
  event.ev_hdr.bt_app_id = app_id;

  event.ev_msg.ev_goep_srv_dcn_ind.ch_num = ch_num;
  event.ev_msg.ev_goep_srv_dcn_ind.target_id = target_id; 
  event.ev_msg.ev_goep_srv_dcn_ind.conn_id = 
        (bt_pf_goep_srv_conn_id_type)conn_id;

  if ( cmd_headers_ptr != NULL )
  {
    event.ev_msg.ev_goep_srv_dcn_ind.cmd_headers_cnt = cmd_headers_ptr->count;
    event.ev_msg.ev_goep_srv_dcn_ind.cmd_headers_ptr = 
      (bt_pf_goep_obex_hdr_type*)(cmd_headers_ptr->list);
  }
  else
  {
    event.ev_msg.ev_goep_srv_dcn_ind.cmd_headers_cnt = 0;
    event.ev_msg.ev_goep_srv_dcn_ind.cmd_headers_ptr = NULL;
  }        
  
  bt_ec_send_event( &event );

  if ( cmd_headers_ptr != NULL )
  {
    /* Trick the OI_FreeIf inside GetCommand into NOT freeing the header list */
    /* The application will need to free the list when done */
    cmd_headers_ptr->list = NULL;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_goep_srv_get_ind_cb    
   
DESCRIPTION
  This function handles the generation of the get indication event to
  the application. The application ID of the target is retrieved from the
  target-application table.
   
===========================================================================*/
OI_STATUS bt_pf_goep_srv_get_ind_cb( uint16 index,
                                     OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                     OI_OBEX_HEADER_LIST *cmd_headers_ptr,
                                     OI_STATUS status )
{
  uint8 ch_num;
  uint8 target_id;
  bt_app_id_type    app_id;
  bt_ev_msg_type event;

  ch_num = bt_pf_goep_srv_target_app_table[index].ch_num;
  target_id = bt_pf_goep_srv_target_app_table[index].target_id;
  app_id = bt_pf_goep_srv_target_app_table[index].app_id;
  BT_MSG_DEBUG( "BT PF GOEP SRV GET IND CB %x: Ch: %x, Target: %x", 
              index, ch_num, target_id );
  BT_MSG_DEBUG( " Conn: %x, App: %x", conn_id, app_id, 0 );

  event.ev_hdr.ev_type = BT_EV_PF_GOEP_SRV_GET_IND;
  event.ev_hdr.bt_app_id = app_id;

  event.ev_msg.ev_goep_srv_get_ind.ch_num = ch_num;
  event.ev_msg.ev_goep_srv_get_ind.target_id = target_id;
  event.ev_msg.ev_goep_srv_get_ind.conn_id = 
    (bt_pf_goep_srv_conn_id_type)conn_id;
  if ( cmd_headers_ptr != NULL )
  {
    event.ev_msg.ev_goep_srv_get_ind.cmd_headers_cnt = cmd_headers_ptr->count;
    event.ev_msg.ev_goep_srv_get_ind.cmd_headers_ptr = 
      (bt_pf_goep_obex_hdr_type*)(cmd_headers_ptr->list);
  }
  else
  {
    event.ev_msg.ev_goep_srv_get_ind.cmd_headers_cnt = 0;
    event.ev_msg.ev_goep_srv_get_ind.cmd_headers_ptr = NULL;
  }        
  event.ev_msg.ev_goep_srv_get_ind.status = (bt_cmd_status_type)status;
  
  bt_ec_send_event( &event );

  if ( cmd_headers_ptr != NULL )
  {
    /* Trick the OI_FreeIf inside GetCommand into NOT freeing the header list */
    /* The application will need to free the list when done */
    cmd_headers_ptr->list = NULL;
  }
  return( OI_OK );
}

/*===========================================================================

FUNCTION
  bt_pf_goep_srv_put_ind_cb    
   
DESCRIPTION
  This function handles the generation of the put indication event to
  the application. The application ID of the target is retrieved from the
  target-application table.
   
===========================================================================*/
OI_STATUS bt_pf_goep_srv_put_ind_cb( uint16 index,
                                     OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                     OI_OBEX_HEADER_LIST *cmd_headers_ptr,
                                     OI_STATUS status )
{
  uint8 ch_num;
  uint8 target_id;
  bt_app_id_type    app_id;
  bt_ev_msg_type event;

  ch_num = bt_pf_goep_srv_target_app_table[index].ch_num;
  target_id = bt_pf_goep_srv_target_app_table[index].target_id;
  app_id = bt_pf_goep_srv_target_app_table[index].app_id;
  BT_MSG_DEBUG( "BT PF GOEP SRV PUT IND CB %x: Ch: %x, Target: %x", 
              index, ch_num, target_id );
  BT_MSG_DEBUG( " Conn: %x, App: %x", conn_id, app_id, 0 );

  event.ev_hdr.ev_type = BT_EV_PF_GOEP_SRV_PUT_IND;
  event.ev_hdr.bt_app_id = app_id;

  event.ev_msg.ev_goep_srv_put_ind.ch_num = ch_num;
  event.ev_msg.ev_goep_srv_put_ind.target_id = target_id;
  event.ev_msg.ev_goep_srv_put_ind.conn_id = 
    (bt_pf_goep_srv_conn_id_type)conn_id;
  if ( cmd_headers_ptr != NULL )
  {
    event.ev_msg.ev_goep_srv_put_ind.cmd_headers_cnt = cmd_headers_ptr->count;
    event.ev_msg.ev_goep_srv_put_ind.cmd_headers_ptr = 
      (bt_pf_goep_obex_hdr_type*)(cmd_headers_ptr->list);
  }
  else
  {
    event.ev_msg.ev_goep_srv_put_ind.cmd_headers_cnt = 0;
    event.ev_msg.ev_goep_srv_put_ind.cmd_headers_ptr = NULL;
  }  
  event.ev_msg.ev_goep_srv_put_ind.status = (bt_cmd_status_type)status;

  bt_ec_send_event( &event );

  if ( cmd_headers_ptr != NULL )
  {
    /* Trick the OI_FreeIf inside GetCommand into NOT freeing the header list */
    /* The application will need to free the list when done */
    cmd_headers_ptr->list = NULL;
  }
  return( OI_OK );
}

/*===========================================================================

FUNCTION
  bt_pf_goep_srv_set_path_ind_cb    
   
DESCRIPTION
  This function handles the generation of the set path indication event to
  the application. The application ID of the target is retrieved from the
  target-application table.
   
===========================================================================*/
OI_STATUS bt_pf_goep_srv_set_path_ind_cb( uint16 index,
                                          OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                          OI_OBEX_UNICODE *folder_ptr,
                                          OI_BOOL up_level,
                                          OI_BOOL create, 
                                          OI_OBEX_HEADER_LIST *cmd_headers_ptr)
{
  uint8 ch_num;
  uint8 target_id;
  bt_app_id_type    app_id;
  bt_ev_msg_type event;
  uint16* folder_str_copy_ptr = NULL;
  uint16 folder_byte_len;

  ch_num = bt_pf_goep_srv_target_app_table[index].ch_num;
  target_id = bt_pf_goep_srv_target_app_table[index].target_id;
  app_id = bt_pf_goep_srv_target_app_table[index].app_id;
  BT_MSG_DEBUG( "BT PF GOEP SRV SET PATH IND CB %x: Ch: %x, Target: %x", 
              index, ch_num, target_id );
  BT_MSG_DEBUG( "BT PF GOEP SRV SET PATH IND CB: App: %x UpLevel:%x Create:%x", 
                                                    app_id, up_level, create );

  if ((folder_ptr == NULL) || (folder_ptr->len == 0 ))
  {
    event.ev_msg.ev_goep_srv_set_path_ind.folder_ptr = NULL;
  }
  else
  {
    folder_byte_len = folder_ptr->len * sizeof( uint16 );
    if (*(folder_ptr->str + folder_ptr->len - 1) != 0)
    {
      folder_byte_len += sizeof( uint16 ); /* add space for NULL termination */
    }
    folder_str_copy_ptr = (uint16*)(mem_malloc( &bt_pf_heap, 
                                                (size_t)folder_byte_len ));
    if (folder_str_copy_ptr == NULL)
    {
      BT_ERR( "BT PF GOEP SRV SET PATH IND CB %x, out of memory",
              index, 0, 0 );
      return( OI_STATUS_OUT_OF_MEMORY );
    }
    memcpy( folder_str_copy_ptr, folder_ptr->str, (size_t)folder_byte_len );
    *(folder_str_copy_ptr + folder_byte_len/sizeof(uint16) - 1) = 0;
    event.ev_msg.ev_goep_srv_set_path_ind.folder_ptr = folder_str_copy_ptr; 
  }                                                                       

  event.ev_hdr.ev_type = BT_EV_PF_GOEP_SRV_SET_PATH_IND;
  event.ev_hdr.bt_app_id = app_id;
  
  event.ev_msg.ev_goep_srv_set_path_ind.ch_num = ch_num; 
  event.ev_msg.ev_goep_srv_set_path_ind.target_id = target_id;
  event.ev_msg.ev_goep_srv_set_path_ind.conn_id = 
    (bt_pf_goep_srv_conn_id_type)conn_id;            
  event.ev_msg.ev_goep_srv_set_path_ind.up_level = up_level;
  event.ev_msg.ev_goep_srv_set_path_ind.create = create;

  if ( cmd_headers_ptr != NULL )
  {
    event.ev_msg.ev_goep_srv_set_path_ind.cmd_headers_cnt = cmd_headers_ptr->count;
    event.ev_msg.ev_goep_srv_set_path_ind.cmd_headers_ptr = 
      (bt_pf_goep_obex_hdr_type*)(cmd_headers_ptr->list);
  }
  else
  {
    event.ev_msg.ev_goep_srv_set_path_ind.cmd_headers_cnt = 0;
    event.ev_msg.ev_goep_srv_set_path_ind.cmd_headers_ptr = NULL;
  }  

  if ( cmd_headers_ptr != NULL )
  {
    /* Trick the OI_FreeIf inside GetCommand into NOT freeing the header list */
    /* The application will need to free the list when done */
    cmd_headers_ptr->list = NULL;
  }

  bt_ec_send_event( &event );

  if(folder_str_copy_ptr != NULL) 
    mem_free( &bt_pf_heap, (void *)folder_str_copy_ptr );
  return( OI_OK );
}

/*===========================================================================

FUNCTIONS
   bt_pf_goep_srv_con_ind_cb_0-3
   bt_pf_goep_srv_dcn_ind_cb_0-3
   bt_pf_goep_srv_get_ind_cb_0-3
   bt_pf_goep_srv_put_ind_cb_0-3
   bt_pf_goep_srv_set_path_ind_cb_0-3
   
DESCRIPTION
   Per-connection event callback functions.
   These functions are hardcoded in the target-application table. When 
   invoked, the corresponding general event callback function above will be
   called with the application-connection table index being an input 
   parameter. This index is used to retrieve the application ID from the
   target-application table so that the event generated can be directed
   to the appropriate application.
   
   The pointers to these functions are passed to the OI OBEX driver when
   OI_OBEXSRV_RegisterServer or OI_OBEXSRV_RegisterSecondaryTarget is
   called. The OI OBEX driver will invoke these callback functions when the
   corresponding event occurs.
   
===========================================================================*/
void bt_pf_goep_srv_con_ind_cb_0( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL unauthorized,
                                  OI_BYTE *user_id,
                                  OI_UINT8 user_id_len,
                                  OI_OBEX_HEADER_LIST *cmd_headers_ptr)
{
  bt_pf_goep_srv_con_ind_cb( 0, conn_id,unauthorized, user_id, user_id_len, cmd_headers_ptr ); 
}

void bt_pf_goep_srv_con_ind_cb_1( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL unauthorized,
                                  OI_BYTE *user_id,
                                  OI_UINT8 user_id_len,
                                  OI_OBEX_HEADER_LIST *cmd_headers_ptr )
{
  bt_pf_goep_srv_con_ind_cb( 1, conn_id,unauthorized, user_id, user_id_len, cmd_headers_ptr );
}

void bt_pf_goep_srv_con_ind_cb_2( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL unauthorized,
                                  OI_BYTE *user_id,
                                  OI_UINT8 user_id_len,
                                  OI_OBEX_HEADER_LIST *cmd_headers_ptr )
{
  bt_pf_goep_srv_con_ind_cb( 2, conn_id,unauthorized, user_id, user_id_len, cmd_headers_ptr );
}

void bt_pf_goep_srv_con_ind_cb_3( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL unauthorized,
                                  OI_BYTE *user_id,
                                  OI_UINT8 user_id_len,
                                  OI_OBEX_HEADER_LIST *cmd_headers_ptr )
{
  bt_pf_goep_srv_con_ind_cb( 3, conn_id,unauthorized, user_id, user_id_len, cmd_headers_ptr );
}

void bt_pf_goep_srv_con_ind_cb_4( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL unauthorized,
                                  OI_BYTE *user_id,
                                  OI_UINT8 user_id_len,
                                  OI_OBEX_HEADER_LIST *cmd_headers_ptr )
{
  bt_pf_goep_srv_con_ind_cb( 4, conn_id,unauthorized, user_id, user_id_len, cmd_headers_ptr );
}

void bt_pf_goep_srv_con_ind_cb_5( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL unauthorized,
                                  OI_BYTE *user_id,
                                  OI_UINT8 user_id_len,
                                  OI_OBEX_HEADER_LIST *cmd_headers_ptr )
{
  bt_pf_goep_srv_con_ind_cb( 5, conn_id,unauthorized, user_id, user_id_len, cmd_headers_ptr );
}

void bt_pf_goep_srv_con_ind_cb_6( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL unauthorized,
                                  OI_BYTE *user_id,
                                  OI_UINT8 user_id_len,
                                  OI_OBEX_HEADER_LIST *cmd_headers_ptr )
{
  bt_pf_goep_srv_con_ind_cb( 6, conn_id,unauthorized, user_id, user_id_len, cmd_headers_ptr );
}

void bt_pf_goep_srv_con_ind_cb_7( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL unauthorized,
                                  OI_BYTE *user_id,
                                  OI_UINT8 user_id_len,
                                  OI_OBEX_HEADER_LIST *cmd_headers_ptr )
{
  bt_pf_goep_srv_con_ind_cb( 7, conn_id,unauthorized, user_id, user_id_len, cmd_headers_ptr );
}

OI_STATUS bt_pf_goep_srv_auth_ind_cb_0(OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL full_access,
                                  OI_UINT8 *user_id_ptr,
                                  OI_BOOL user_id_flag,
                                  OI_UINT8 user_id_len,
                                  OI_REALM_INFO *realmInfo)
{
   bt_pf_goep_srv_auth_ind_cb(0,
                              conn_id,
                              full_access,
                              user_id_ptr,
                              user_id_flag,
                              user_id_len,
                              realmInfo);
   return( OI_OK );
}

OI_STATUS bt_pf_goep_srv_auth_ind_cb_1(OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL full_access,
                                  OI_UINT8 *user_id_ptr,
                                  OI_BOOL user_id_flag,
                                  OI_UINT8 user_id_len,
                                  OI_REALM_INFO *realmInfo)
{
    bt_pf_goep_srv_auth_ind_cb(1,conn_id,
                              full_access,
                              user_id_ptr,
                              user_id_flag,
                              user_id_len,
                              realmInfo);
    return( OI_OK );
}

OI_STATUS bt_pf_goep_srv_auth_ind_cb_2(OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL full_access,
                                  OI_UINT8 *user_id_ptr,
                                  OI_BOOL user_id_flag,
                                  OI_UINT8 user_id_len,
                                  OI_REALM_INFO *realmInfo)
{
    bt_pf_goep_srv_auth_ind_cb(2,conn_id,
                              full_access,
                              user_id_ptr,
                              user_id_flag,
                              user_id_len,
                              realmInfo);
    return( OI_OK );
}

OI_STATUS bt_pf_goep_srv_auth_ind_cb_3(OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL full_access,
                                  OI_UINT8 *user_id_ptr,
                                  OI_BOOL user_id_flag,
                                  OI_UINT8 user_id_len,
                                  OI_REALM_INFO *realmInfo)
{
    bt_pf_goep_srv_auth_ind_cb(3,conn_id,
                              full_access,
                              user_id_ptr,
                              user_id_flag,
                              user_id_len,
                              realmInfo);
    return( OI_OK );
}


OI_STATUS bt_pf_goep_srv_auth_ind_cb_4(OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL full_access,
                                  OI_UINT8 *user_id_ptr,
                                  OI_BOOL user_id_flag,
                                  OI_UINT8 user_id_len,
                                  OI_REALM_INFO *realmInfo)
{
    bt_pf_goep_srv_auth_ind_cb(4,conn_id,
                              full_access,
                              user_id_ptr,
                              user_id_flag,
                              user_id_len,
                              realmInfo);
    return( OI_OK );
}

OI_STATUS bt_pf_goep_srv_auth_ind_cb_5(OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL full_access,
                                  OI_UINT8 *user_id_ptr,
                                  OI_BOOL user_id_flag,
                                  OI_UINT8 user_id_len,
                                  OI_REALM_INFO *realmInfo)
{
    bt_pf_goep_srv_auth_ind_cb(5,conn_id,
                              full_access,
                              user_id_ptr,
                              user_id_flag,
                              user_id_len,
                              realmInfo);
    return( OI_OK );
}


OI_STATUS bt_pf_goep_srv_auth_ind_cb_6(OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL full_access,
                                  OI_UINT8 *user_id_ptr,
                                  OI_BOOL user_id_flag,
                                  OI_UINT8 user_id_len,
                                  OI_REALM_INFO *realmInfo)
{
    bt_pf_goep_srv_auth_ind_cb(6,conn_id,
                              full_access,
                              user_id_ptr,
                              user_id_flag,
                              user_id_len,
                              realmInfo);
    return( OI_OK );
}

OI_STATUS bt_pf_goep_srv_auth_ind_cb_7(OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_BOOL full_access,
                                  OI_UINT8 *user_id_ptr,
                                  OI_BOOL user_id_flag,
                                  OI_UINT8 user_id_len,
                                  OI_REALM_INFO *realmInfo)
{
    bt_pf_goep_srv_auth_ind_cb(7,conn_id,
                              full_access,
                              user_id_ptr,
                              user_id_flag,
                              user_id_len,
                              realmInfo);
    return( OI_OK );
}


void bt_pf_goep_srv_dcn_ind_cb_0( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_OBEX_HEADER_LIST *cmd_headers  )
{
  bt_pf_goep_srv_dcn_ind_cb( 0, conn_id, cmd_headers );
}

void bt_pf_goep_srv_dcn_ind_cb_1( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_OBEX_HEADER_LIST *cmd_headers  )
{
  bt_pf_goep_srv_dcn_ind_cb( 1, conn_id, cmd_headers );
}

void bt_pf_goep_srv_dcn_ind_cb_2( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_OBEX_HEADER_LIST *cmd_headers  )
{
  bt_pf_goep_srv_dcn_ind_cb( 2, conn_id, cmd_headers );
}

void bt_pf_goep_srv_dcn_ind_cb_3( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_OBEX_HEADER_LIST *cmd_headers  )
{
  bt_pf_goep_srv_dcn_ind_cb( 3, conn_id, cmd_headers );
}

void bt_pf_goep_srv_dcn_ind_cb_4( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_OBEX_HEADER_LIST *cmd_headers  )
{
  bt_pf_goep_srv_dcn_ind_cb( 4, conn_id, cmd_headers );
}

void bt_pf_goep_srv_dcn_ind_cb_5( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_OBEX_HEADER_LIST *cmd_headers  )
{
  bt_pf_goep_srv_dcn_ind_cb( 5, conn_id, cmd_headers );
}

void bt_pf_goep_srv_dcn_ind_cb_6( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_OBEX_HEADER_LIST *cmd_headers  )
{
  bt_pf_goep_srv_dcn_ind_cb( 6, conn_id, cmd_headers );
}

void bt_pf_goep_srv_dcn_ind_cb_7( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                  OI_OBEX_HEADER_LIST *cmd_headers )

{
  bt_pf_goep_srv_dcn_ind_cb( 7, conn_id, cmd_headers);
}


OI_STATUS bt_pf_goep_srv_get_ind_cb_0( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                       OI_OBEX_HEADER_LIST *cmd_headers,
                                       OI_STATUS status )
{
  return( bt_pf_goep_srv_get_ind_cb( 0, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_get_ind_cb_1( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                       OI_OBEX_HEADER_LIST *cmd_headers,
                                       OI_STATUS status )
{
  return( bt_pf_goep_srv_get_ind_cb( 1, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_get_ind_cb_2( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                       OI_OBEX_HEADER_LIST *cmd_headers,
                                       OI_STATUS status )
{
  return( bt_pf_goep_srv_get_ind_cb( 2, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_get_ind_cb_3( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                       OI_OBEX_HEADER_LIST *cmd_headers,
                                       OI_STATUS status )
{
  return( bt_pf_goep_srv_get_ind_cb( 3, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_get_ind_cb_4( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                       OI_OBEX_HEADER_LIST *cmd_headers,
                                       OI_STATUS status )
{
  return( bt_pf_goep_srv_get_ind_cb( 4, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_get_ind_cb_5( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                       OI_OBEX_HEADER_LIST *cmd_headers,
                                       OI_STATUS status )
{
  return( bt_pf_goep_srv_get_ind_cb( 5, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_get_ind_cb_6( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                       OI_OBEX_HEADER_LIST *cmd_headers,
                                       OI_STATUS status )
{
  return( bt_pf_goep_srv_get_ind_cb( 6, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_get_ind_cb_7( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                       OI_OBEX_HEADER_LIST *cmd_headers,
                                       OI_STATUS status )
{
  return( bt_pf_goep_srv_get_ind_cb( 7, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_put_ind_cb_0( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                     OI_OBEX_HEADER_LIST *cmd_headers,
                                     OI_STATUS status )
{
  return( bt_pf_goep_srv_put_ind_cb( 0, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_put_ind_cb_1( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                     OI_OBEX_HEADER_LIST *cmd_headers,
                                     OI_STATUS status )
{
  return( bt_pf_goep_srv_put_ind_cb( 1, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_put_ind_cb_2( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                     OI_OBEX_HEADER_LIST *cmd_headers,
                                     OI_STATUS status )
{
  return( bt_pf_goep_srv_put_ind_cb( 2, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_put_ind_cb_3( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                     OI_OBEX_HEADER_LIST *cmd_headers,
                                     OI_STATUS status )
{
  return( bt_pf_goep_srv_put_ind_cb( 3, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_put_ind_cb_4( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                     OI_OBEX_HEADER_LIST *cmd_headers,
                                     OI_STATUS status )
{
  return( bt_pf_goep_srv_put_ind_cb( 4, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_put_ind_cb_5( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                     OI_OBEX_HEADER_LIST *cmd_headers,
                                     OI_STATUS status )
{
  return( bt_pf_goep_srv_put_ind_cb( 5, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_put_ind_cb_6( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                     OI_OBEX_HEADER_LIST *cmd_headers,
                                     OI_STATUS status )
{
  return( bt_pf_goep_srv_put_ind_cb( 6, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_put_ind_cb_7( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                     OI_OBEX_HEADER_LIST *cmd_headers,
                                     OI_STATUS status )
{
  return( bt_pf_goep_srv_put_ind_cb( 7, conn_id, cmd_headers, status ) );
}

OI_STATUS bt_pf_goep_srv_set_path_ind_cb_0( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                            OI_OBEX_UNICODE *folder,
                                            OI_BOOL up_level,
                                            OI_BOOL create,
                                            OI_OBEX_HEADER_LIST *cmd_headers)
{
  return( bt_pf_goep_srv_set_path_ind_cb( 
            0, conn_id, folder, up_level, create, cmd_headers ) );
}

OI_STATUS bt_pf_goep_srv_set_path_ind_cb_1( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                            OI_OBEX_UNICODE *folder,
                                            OI_BOOL up_level,
                                            OI_BOOL create,
                                            OI_OBEX_HEADER_LIST *cmd_headers )
{
  return( bt_pf_goep_srv_set_path_ind_cb( 
            1, conn_id, folder, up_level, create , cmd_headers) );
}

OI_STATUS bt_pf_goep_srv_set_path_ind_cb_2( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                            OI_OBEX_UNICODE *folder,
                                            OI_BOOL up_level,
                                            OI_BOOL create,
                                            OI_OBEX_HEADER_LIST *cmd_headers )
{
  return( bt_pf_goep_srv_set_path_ind_cb( 
            2, conn_id, folder, up_level, create, cmd_headers ) );
}

OI_STATUS bt_pf_goep_srv_set_path_ind_cb_3( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                            OI_OBEX_UNICODE *folder,
                                            OI_BOOL up_level,
                                            OI_BOOL create,
                                            OI_OBEX_HEADER_LIST *cmd_headers )
{
  return( bt_pf_goep_srv_set_path_ind_cb( 
            3, conn_id, folder, up_level, create, cmd_headers ) );
}

OI_STATUS bt_pf_goep_srv_set_path_ind_cb_4( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                            OI_OBEX_UNICODE *folder,
                                            OI_BOOL up_level,
                                            OI_BOOL create,
                                            OI_OBEX_HEADER_LIST *cmd_headers )
{
  return( bt_pf_goep_srv_set_path_ind_cb(
            4, conn_id, folder, up_level, create, cmd_headers ) );
}

OI_STATUS bt_pf_goep_srv_set_path_ind_cb_5( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                            OI_OBEX_UNICODE *folder,
                                            OI_BOOL up_level,
                                            OI_BOOL create,
                                            OI_OBEX_HEADER_LIST *cmd_headers )
{
  return( bt_pf_goep_srv_set_path_ind_cb(
            5, conn_id, folder, up_level, create, cmd_headers ) );
}

OI_STATUS bt_pf_goep_srv_set_path_ind_cb_6( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                            OI_OBEX_UNICODE *folder,
                                            OI_BOOL up_level,
                                            OI_BOOL create,
                                            OI_OBEX_HEADER_LIST *cmd_headers )
{
  return( bt_pf_goep_srv_set_path_ind_cb(
            6, conn_id, folder, up_level, create, cmd_headers ) );
}

OI_STATUS bt_pf_goep_srv_set_path_ind_cb_7( OI_OBEXSRV_CONNECTION_HANDLE conn_id,
                                            OI_OBEX_UNICODE *folder,
                                            OI_BOOL up_level,
                                            OI_BOOL create,
                                            OI_OBEX_HEADER_LIST *cmd_headers )
{
  return( bt_pf_goep_srv_set_path_ind_cb(
            7, conn_id, folder, up_level, create, cmd_headers ) );
}


/*===========================================================================

FUNCTION
  bt_pf_goep_srv_target_app_table_init

DESCRIPTION
  Initialize the application-connection table.

===========================================================================*/
void bt_pf_goep_srv_target_app_table_init()
{
  bt_pf_goep_srv_target_app_table[0].app_id = BT_PF_APP_ID_NULL;
  bt_pf_goep_srv_target_app_table[0].ch_num = BT_PF_GOEP_SRV_CH_NUM_NULL;
  bt_pf_goep_srv_target_app_table[0].target_id = BT_PF_GOEP_SRV_TARGET_ID_NULL;
  bt_pf_goep_srv_target_app_table[0].target_str_ptr = NULL;
  bt_pf_goep_srv_target_app_table[0].cb_list.connectInd = 
    bt_pf_goep_srv_con_ind_cb_0;
  /* This should be initialized to NULL  as the registration
   * will happen through different API.
   * Only the application that needs a different event
   * for the  AUTH INDICATION should call the new API
   * else the new event will get passed to all APPS
   */
  bt_pf_goep_srv_target_app_table[0].cb_list.authChallengeIndCB =
    NULL;
  bt_pf_goep_srv_target_app_table[0].cb_list.disconnectInd = 
    bt_pf_goep_srv_dcn_ind_cb_0;
  bt_pf_goep_srv_target_app_table[0].cb_list.getInd = 
    bt_pf_goep_srv_get_ind_cb_0;
  bt_pf_goep_srv_target_app_table[0].cb_list.putInd = 
    bt_pf_goep_srv_put_ind_cb_0;
  bt_pf_goep_srv_target_app_table[0].cb_list.setPathInd = 
    bt_pf_goep_srv_set_path_ind_cb_0;

  bt_pf_goep_srv_target_app_table[1].app_id = BT_PF_APP_ID_NULL;
  bt_pf_goep_srv_target_app_table[1].ch_num = BT_PF_GOEP_SRV_CH_NUM_NULL;
  bt_pf_goep_srv_target_app_table[1].target_id = BT_PF_GOEP_SRV_TARGET_ID_NULL;
  bt_pf_goep_srv_target_app_table[1].target_str_ptr = NULL;
  bt_pf_goep_srv_target_app_table[1].cb_list.connectInd = 
    bt_pf_goep_srv_con_ind_cb_1;
  bt_pf_goep_srv_target_app_table[1].cb_list.authChallengeIndCB =
    NULL;
  bt_pf_goep_srv_target_app_table[1].cb_list.disconnectInd = 
    bt_pf_goep_srv_dcn_ind_cb_1;
  bt_pf_goep_srv_target_app_table[1].cb_list.getInd = 
    bt_pf_goep_srv_get_ind_cb_1;
  bt_pf_goep_srv_target_app_table[1].cb_list.putInd = 
    bt_pf_goep_srv_put_ind_cb_1;
  bt_pf_goep_srv_target_app_table[1].cb_list.setPathInd = 
    bt_pf_goep_srv_set_path_ind_cb_1;

  bt_pf_goep_srv_target_app_table[2].app_id = BT_PF_APP_ID_NULL;
  bt_pf_goep_srv_target_app_table[2].ch_num = BT_PF_GOEP_SRV_CH_NUM_NULL;
  bt_pf_goep_srv_target_app_table[2].target_id = BT_PF_GOEP_SRV_TARGET_ID_NULL;
  bt_pf_goep_srv_target_app_table[2].target_str_ptr = NULL;
  bt_pf_goep_srv_target_app_table[2].cb_list.connectInd = 
    bt_pf_goep_srv_con_ind_cb_2;
  bt_pf_goep_srv_target_app_table[2].cb_list.authChallengeIndCB =
    NULL;
  bt_pf_goep_srv_target_app_table[2].cb_list.disconnectInd = 
    bt_pf_goep_srv_dcn_ind_cb_2;
  bt_pf_goep_srv_target_app_table[2].cb_list.getInd = 
    bt_pf_goep_srv_get_ind_cb_2;
  bt_pf_goep_srv_target_app_table[2].cb_list.putInd = 
    bt_pf_goep_srv_put_ind_cb_2;
  bt_pf_goep_srv_target_app_table[2].cb_list.setPathInd = 
    bt_pf_goep_srv_set_path_ind_cb_2;

  bt_pf_goep_srv_target_app_table[3].app_id = BT_PF_APP_ID_NULL;
  bt_pf_goep_srv_target_app_table[3].ch_num = BT_PF_GOEP_SRV_CH_NUM_NULL;
  bt_pf_goep_srv_target_app_table[3].target_id = BT_PF_GOEP_SRV_TARGET_ID_NULL;
  bt_pf_goep_srv_target_app_table[3].target_str_ptr = NULL;
  bt_pf_goep_srv_target_app_table[3].cb_list.connectInd = 
    bt_pf_goep_srv_con_ind_cb_3;
  bt_pf_goep_srv_target_app_table[3].cb_list.authChallengeIndCB =
    NULL;
  bt_pf_goep_srv_target_app_table[3].cb_list.disconnectInd = 
    bt_pf_goep_srv_dcn_ind_cb_3;
  bt_pf_goep_srv_target_app_table[3].cb_list.getInd = 
    bt_pf_goep_srv_get_ind_cb_3;
  bt_pf_goep_srv_target_app_table[3].cb_list.putInd = 
    bt_pf_goep_srv_put_ind_cb_3;
  bt_pf_goep_srv_target_app_table[3].cb_list.setPathInd = 
    bt_pf_goep_srv_set_path_ind_cb_3;

  bt_pf_goep_srv_target_app_table[4].app_id = BT_PF_APP_ID_NULL;
  bt_pf_goep_srv_target_app_table[4].ch_num = BT_PF_GOEP_SRV_CH_NUM_NULL;
  bt_pf_goep_srv_target_app_table[4].target_id = BT_PF_GOEP_SRV_TARGET_ID_NULL;
  bt_pf_goep_srv_target_app_table[4].target_str_ptr = NULL;
  bt_pf_goep_srv_target_app_table[4].cb_list.connectInd =
    bt_pf_goep_srv_con_ind_cb_4;
  bt_pf_goep_srv_target_app_table[4].cb_list.authChallengeIndCB =
    NULL;
  bt_pf_goep_srv_target_app_table[4].cb_list.disconnectInd =
    bt_pf_goep_srv_dcn_ind_cb_4;
  bt_pf_goep_srv_target_app_table[4].cb_list.getInd =
    bt_pf_goep_srv_get_ind_cb_4;
  bt_pf_goep_srv_target_app_table[4].cb_list.putInd =
    bt_pf_goep_srv_put_ind_cb_4;
  bt_pf_goep_srv_target_app_table[4].cb_list.setPathInd =
    bt_pf_goep_srv_set_path_ind_cb_4;

  bt_pf_goep_srv_target_app_table[5].app_id = BT_PF_APP_ID_NULL;
  bt_pf_goep_srv_target_app_table[5].ch_num = BT_PF_GOEP_SRV_CH_NUM_NULL;
  bt_pf_goep_srv_target_app_table[5].target_id = BT_PF_GOEP_SRV_TARGET_ID_NULL;
  bt_pf_goep_srv_target_app_table[5].target_str_ptr = NULL;
  bt_pf_goep_srv_target_app_table[5].cb_list.connectInd =
    bt_pf_goep_srv_con_ind_cb_5;
  bt_pf_goep_srv_target_app_table[5].cb_list.authChallengeIndCB =
    NULL;
  bt_pf_goep_srv_target_app_table[5].cb_list.disconnectInd =
    bt_pf_goep_srv_dcn_ind_cb_5;
  bt_pf_goep_srv_target_app_table[5].cb_list.getInd =
    bt_pf_goep_srv_get_ind_cb_5;
  bt_pf_goep_srv_target_app_table[5].cb_list.putInd =
    bt_pf_goep_srv_put_ind_cb_5;
  bt_pf_goep_srv_target_app_table[5].cb_list.setPathInd =
    bt_pf_goep_srv_set_path_ind_cb_5;

  bt_pf_goep_srv_target_app_table[6].app_id = BT_PF_APP_ID_NULL;
  bt_pf_goep_srv_target_app_table[6].ch_num = BT_PF_GOEP_SRV_CH_NUM_NULL;
  bt_pf_goep_srv_target_app_table[6].target_id = BT_PF_GOEP_SRV_TARGET_ID_NULL;
  bt_pf_goep_srv_target_app_table[6].target_str_ptr = NULL;
  bt_pf_goep_srv_target_app_table[6].cb_list.connectInd =
    bt_pf_goep_srv_con_ind_cb_6;
  bt_pf_goep_srv_target_app_table[6].cb_list.authChallengeIndCB =
    NULL;
  bt_pf_goep_srv_target_app_table[6].cb_list.disconnectInd =
    bt_pf_goep_srv_dcn_ind_cb_6;
  bt_pf_goep_srv_target_app_table[6].cb_list.getInd =
    bt_pf_goep_srv_get_ind_cb_6;
  bt_pf_goep_srv_target_app_table[6].cb_list.putInd =
    bt_pf_goep_srv_put_ind_cb_6;
  bt_pf_goep_srv_target_app_table[6].cb_list.setPathInd =
    bt_pf_goep_srv_set_path_ind_cb_6;

  bt_pf_goep_srv_target_app_table[7].app_id = BT_PF_APP_ID_NULL;
  bt_pf_goep_srv_target_app_table[7].ch_num = BT_PF_GOEP_SRV_CH_NUM_NULL;
  bt_pf_goep_srv_target_app_table[7].target_id = BT_PF_GOEP_SRV_TARGET_ID_NULL;
  bt_pf_goep_srv_target_app_table[7].target_str_ptr = NULL;
  bt_pf_goep_srv_target_app_table[7].cb_list.connectInd =
    bt_pf_goep_srv_con_ind_cb_7;
  bt_pf_goep_srv_target_app_table[7].cb_list.authChallengeIndCB =
    NULL;
  bt_pf_goep_srv_target_app_table[7].cb_list.disconnectInd =
    bt_pf_goep_srv_dcn_ind_cb_7;
  bt_pf_goep_srv_target_app_table[7].cb_list.getInd =
    bt_pf_goep_srv_get_ind_cb_7;
  bt_pf_goep_srv_target_app_table[7].cb_list.putInd =
    bt_pf_goep_srv_put_ind_cb_7;
  bt_pf_goep_srv_target_app_table[7].cb_list.setPathInd =
    bt_pf_goep_srv_set_path_ind_cb_7;
}

/*===========================================================================

FUNCTION
  bt_pf_goep_srv_get_target

DESCRIPTION
  This functions gets the connection target associated with an OBEX server 
  connection handle. If no target was used to connect, target->data is set 
  to NULL and target->len to zero.
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_pf_goep_srv_get_target(
  bt_app_id_type                app_id,
  bt_pf_goep_cli_conn_id_type   conn_id,
  bt_pf_byte_seq_type*          target_ptr /* output */ )
{
  BT_MSG_API( "BT PF GOEP SRV CMD: get target, App: %x", app_id, 0, 0 );

  return(OI_OBEXSRV_GetTarget( (OI_OBEXSRV_CONNECTION_HANDLE)conn_id,
                               (OI_OBEX_BYTESEQ*)target_ptr ));
}

/*===========================================================================

FUNCTION
  bt_pf_goep_srv_get_client_addr

DESCRIPTION
  This function gets the Bluetooth Device Address (BD_ADDR) associated with 
  an OBEX server connection handle.
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_pf_goep_srv_get_client_addr(
  bt_app_id_type                app_id,
  bt_pf_goep_cli_conn_id_type   conn_id,
  bt_bd_addr_type*              bd_addr_ptr /* output */ )
{
  BT_MSG_API( "BT PF GOEP SRV CMD: get BD addr, App: %x", app_id, 0, 0 );

  return(OI_OBEXSRV_GetClientAddr( (OI_OBEXSRV_CONNECTION_HANDLE)conn_id,
                                   (OI_BD_ADDR*)bd_addr_ptr ));
} 

/*===========================================================================

FUNCTION
  bt_pf_goep_srv_get_rfcomm_channel

DESCRIPTION
  This function gets the RFCOMM channel number associated with an OBEX 
  server connecting handle.
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_pf_goep_srv_get_rfcomm_channel(
  bt_app_id_type                app_id,
  bt_pf_goep_cli_conn_id_type   conn_id,
  uint8*                        rfcomm_ch_ptr /* output */ )
{
  BT_MSG_API( "BT PF GOEP SRV CMD: get RFCOMM channel, App: %x", 
              app_id, 0, 0 );

  return(OI_OBEXSRV_GetRfcommChannel( (OI_OBEXSRV_CONNECTION_HANDLE)conn_id,
                                      rfcomm_ch_ptr )); 
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_srv_register

DESCRIPTION
  This command registers an OBEX server to accept put and/or get requests 
  from OBEX clients.
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_srv_register(
  bt_app_id_type                app_id,
  bt_pf_byte_seq_type*          target_ptr,
  bt_pf_goep_srv_auth_type      auth,
  bt_sd_uuid_type               uuid,
  bt_security_type              security,
  uint8*                        ch_num /* output */ )
{ 
  bt_pf_byte_seq_type  target_copy;
  bt_pf_byte_seq_type* target_copy_ptr = NULL;
  OI_STATUS            status;
  OI_CONNECT_POLICY    policy;
  int16                index;

  BT_MSG_API( "BT PF GOEP SRV CMD: register: App: %x", app_id, 0, 0 );  
    
  index = bt_pf_goep_srv_add_server( app_id );
  if (index == BT_PF_GOEP_SRV_APP_CONN_FULL)
  {
    return(OI_STATUS_NO_RESOURCES);
  }

  if ( (target_ptr != NULL) && (target_ptr->len != 0) )
  {
    /* target name needs to be stored */
    bt_pf_goep_srv_target_app_table[index].target_str_ptr = (byte*)
      mem_malloc( &bt_pf_heap, (size_t)(target_ptr->len * sizeof( byte )) );

    if (bt_pf_goep_srv_target_app_table[index].target_str_ptr == NULL)
    { 
      BT_ERR( "BT PF GOEP SRV CMD: out of memory, App: %x", app_id, 0, 0 );
      bt_pf_goep_srv_target_app_table[index].app_id = 
        BT_PF_APP_ID_NULL;
      bt_pf_goep_srv_target_app_table[index].target_id = 
        BT_PF_GOEP_SRV_TARGET_ID_NULL;
      return(OI_STATUS_OUT_OF_MEMORY);
    }

    memcpy( bt_pf_goep_srv_target_app_table[index].target_str_ptr, target_ptr->data, 
            (size_t)(target_ptr->len * sizeof( byte )) );
    target_copy.len = target_ptr->len;
    target_copy.data = bt_pf_goep_srv_target_app_table[index].target_str_ptr;
    target_copy_ptr = &target_copy;
  }

  policy.serviceUuid32 = (uint32)uuid;
  policy.securityPolicy = bt_pf_goep_oi_security( security );
  policy.mustBeMaster = FALSE;
  policy.flowspec = NULL;
  policy.powerSavingDisables = 0;

  /*Indicated Disconnect Indication shall be sent to higher app and 
  OBEX should not send disconnect response.
  */
  policy.discInd = TRUE;

  status = OI_OBEXSRV_RegisterServer(
    (OI_OBEX_BYTESEQ*)target_copy_ptr,
    &(bt_pf_goep_srv_target_app_table[index].cb_list),
    bt_pf_goep_srv_oi_auth( auth ),
    ch_num,
    &policy );

  if (status == OI_OK)
  {
    status = bt_cmd_sd_update_obex_profile_service_record(
               app_id,
               uuid,
               *ch_num,
               NULL, /* service name */
               NULL, /* service ID */
               NULL, /* supported_formats_list */
               0,    /* supported capabilities */
               0,    /* supported features */
               0,    /* supported functions */
               0 );  /* imaging capacity */
  }

  if ( status == BT_CS_GN_PENDING )
  {
    /* stores the channel number in the target-application table */
    bt_pf_goep_srv_target_app_table[index].ch_num = *ch_num;
    status = BT_CS_GN_SUCCESS;
  }
  else
  { 
    BT_ERR( "BT PF GOEP SRV CMD: register failed, App: %x, Stat: %x", 
                                                 app_id, status, 0 );
    /*
    This function may receive target_ptr NULL or target_ptr length as zero, in this case
    memory is not allocated to target string. 
    safe to check NULL pointer before freeing it.
    */
    if(bt_pf_goep_srv_target_app_table[index].target_str_ptr != NULL){
        mem_free( &bt_pf_heap, bt_pf_goep_srv_target_app_table[index].target_str_ptr );
    }
    bt_pf_goep_srv_target_app_table[index].app_id = BT_PF_APP_ID_NULL;
    bt_pf_goep_srv_target_app_table[index].target_id = BT_PF_GOEP_SRV_TARGET_ID_NULL;
  }
  return( status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_srv_register_secondary_target

DESCRIPTION
  This function registers an additional target on an existing OBEX server.
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_srv_register_secondary_target(
  bt_app_id_type                app_id,
  bt_pf_byte_seq_type*          target_ptr,
  bt_pf_goep_srv_auth_type      auth,
  uint8                         ch_num,
  uint8*                        target_id /* output */)
{ 
  bt_pf_byte_seq_type  target_copy;
  bt_pf_byte_seq_type* target_copy_ptr = NULL;
  OI_STATUS            status;
  int16                index;

  BT_MSG_API( "BT PF GOEP SRV CMD: reg sec target: App: %x", app_id, 0, 0 );  
    
  index = bt_pf_goep_srv_add_target( app_id, ch_num, target_id );
  if (index == BT_PF_GOEP_SRV_APP_CONN_FULL)
  {
    return(OI_STATUS_NO_RESOURCES);
  }
  else if (index == BT_PF_GOEP_SRV_CH_NOT_FOUND)
  {
    return(OI_STATUS_CHANNEL_NOT_FOUND);
  }

  if (target_ptr != NULL)
  {
    /* target name needs to be stored */
    bt_pf_goep_srv_target_app_table[index].target_str_ptr = (byte*)
      mem_malloc( &bt_pf_heap, (size_t)(target_ptr->len * sizeof( byte )) ); 
    memcpy( bt_pf_goep_srv_target_app_table[index].target_str_ptr, target_ptr->data, 
            (size_t)(target_ptr->len * sizeof( byte )) );
    target_copy.len = target_ptr->len;
    target_copy.data = bt_pf_goep_srv_target_app_table[index].target_str_ptr;
    target_copy_ptr = &target_copy;
  }

  status = OI_OBEXSRV_RegisterSecondaryTarget(
    (OI_OBEX_BYTESEQ*)target_copy_ptr,
    &(bt_pf_goep_srv_target_app_table[index].cb_list),
    bt_pf_goep_srv_oi_auth( auth ),
    ch_num );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF GOEP SRV CMD: reg sec target failed, App: %x, Stat: %x", 
                app_id, status, 0 );
    mem_free( &bt_pf_heap, bt_pf_goep_srv_target_app_table[index].target_str_ptr );
    bt_pf_goep_srv_target_app_table[index].app_id = BT_PF_APP_ID_NULL;
    bt_pf_goep_srv_target_app_table[index].ch_num = BT_PF_GOEP_SRV_CH_NUM_NULL;
    bt_pf_goep_srv_target_app_table[index].target_id = BT_PF_GOEP_SRV_TARGET_ID_NULL;
  }

  return( status );
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_srv_deregister_server

DESCRIPTION
  This function deregisters an OBEX server so that it will no longer accept 
  put and/or get requests from OBEX clients for the specified target. This 
  will also deregister any secondary targets associated with this server.

  To deregister a server that has active connections, terminate the 
  connections using bt_cmd_pf_goep_srv_force_disconnect() first; then 
  deregister the server.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_srv_deregister_server(
  bt_app_id_type                app_id,
  uint8                         ch_num )
{                 
  BT_MSG_API( "BT PF GOEP SRV CMD: deregister: App: %x", app_id, 0, 0 );  

  if ( (bt_pf_goep_srv_remove_server( app_id, ch_num )) != OI_OK )
  {
    return(OI_OBEX_SERVICE_UNAVAILABLE);
  }

  return(OI_OBEXSRV_DeregisterServer( ch_num ));
}

/*
 * This function performs the same operation as RfcommRecvDataInd.
 * The difference is that this function takes a watermark pointer as input parameter
 * instead of OI_BYTE *dataBuf and OI_UINT16 dataLen. It calls bt_pfgoep_cmn_reassemble_pkt
 * instead of OI_OBEXCOMMON_ReassemblePacket which takes in dataBuf and dataLen.
 */
void bt_pfgoep_srv_rc_rcv_ind(OI_RFCOMM_LINK_HANDLE link, dsm_watermark_type* wm_ptr)
{ 
  OBEXSRV_CONNECTION *connection = LookupConnection(link);
  OI_STATUS status;

  if (connection == NULL) 
  {
    OI_LOG_ERROR(("OBEX received data for unknown connection"));
    return;
  }

  OI_DBGPRINT2(("OBEX server received data"));

  status = bt_pfgoep_reassemble_pkt( &connection->common, wm_ptr );

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif

  if (!OI_SUCCESS(status)) 
  {
    OI_DBGPRINT(("OBEX server receive error %!", status));
    return;
  }

  if (connection->common.rcvState == OI_OBEX_RCV_COMPLETE) 
  {
    OI_DBGPRINT2(("OBEX server packet complete"));
    /*
     * If there is a packet being sent, we must defer processing this
     * request until we have confirmation that the send has completed.
     * This is unlikely but possible because write complete is reported
     * asynchronously.
     */
    if (connection->common.mbuf != NULL) 
    {
      OI_DBGPRINT2(("OBEX server postponing request"));
    }
    else 
    {
      HandleRequestPkt( connection );
    }
  }
}

/*===========================================================================
 FUNCTION
   bt_cmd_pf_goep_srv_reg_authcb

DESCRIPTION
   This API  is used to update the need for event for the authentication
   request. For Java apps the value can be TRUE / FALSE.
   For other apps it is currently FALSE.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_srv_reg_auth_cb(bt_app_id_type  app_id,
                                             uint8       ch_num,
                                             boolean     authcb_rqd)
{
    int16 i;
    OI_STATUS status = OI_OK;
    for (i = 0; i < BT_PF_SIO_MAX_PORTS; i++)
    {
        if ( (bt_pf_goep_srv_target_app_table[i].ch_num == ch_num) &&
             (bt_pf_goep_srv_target_app_table[i].app_id == app_id) )
        {
            if(status != OI_OK)
            {
                return BT_CS_PF_CHANNEL_NOT_FOUND;
            }
              /* Update internal APP table */
            switch(i)
            {
                case 0:
                bt_pf_goep_srv_target_app_table[0].cb_list.authChallengeIndCB =
                bt_pf_goep_srv_auth_ind_cb_0;
                break;

                case 1:
                bt_pf_goep_srv_target_app_table[1].cb_list.authChallengeIndCB =
                bt_pf_goep_srv_auth_ind_cb_1;
                break;

                case 2:
                bt_pf_goep_srv_target_app_table[2].cb_list.authChallengeIndCB =
                bt_pf_goep_srv_auth_ind_cb_2;
                break;

                case 3:
                bt_pf_goep_srv_target_app_table[3].cb_list.authChallengeIndCB =
                bt_pf_goep_srv_auth_ind_cb_3;
                break;

                case 4:
                bt_pf_goep_srv_target_app_table[4].cb_list.authChallengeIndCB =
                bt_pf_goep_srv_auth_ind_cb_4;
                break;

                case 5:
                bt_pf_goep_srv_target_app_table[5].cb_list.authChallengeIndCB =
                bt_pf_goep_srv_auth_ind_cb_5;
                break;

                case 6:
                bt_pf_goep_srv_target_app_table[6].cb_list.authChallengeIndCB =
                bt_pf_goep_srv_auth_ind_cb_6;
                break;

                case 7:
                bt_pf_goep_srv_target_app_table[7].cb_list.authChallengeIndCB =
                bt_pf_goep_srv_auth_ind_cb_7;
                break;

                default:
                return BT_CS_PF_CHANNEL_NOT_FOUND;
            }
              
            status  = OI_OBEXSRV_RegisterServerAuthCb(ch_num,
                                                       &(bt_pf_goep_srv_target_app_table[i].cb_list));
                            
           return status;
        }
    }
    return BT_CS_GN_BAD_APP_ID;
}


/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_srv_authenticate

DESCRIPTION
  This command initiates OBEX Server authentication.

  This command is returned immediately and is not placed on the command queue.

===========================================================================*/

bt_cmd_status_type bt_cmd_pf_goep_srv_authenticate(
bt_pf_goep_srv_conn_id_type             conn_id,
bt_bd_addr_type                         *bd_addr_ptr,
uint8                                   channel,
bt_pf_realm_info_type                 *realm_info_ptr,
boolean                                 uid_reqd,
boolean                                full_access )
{
    return OI_OBEXSRV_Authenticate(
                           (OI_OBEXSRV_CONNECTION_HANDLE) conn_id,
                           (OI_BD_ADDR *)bd_addr_ptr,
                           channel,
                           (OI_REALM_INFO *)realm_info_ptr,
                           uid_reqd,
                           full_access);
}

/*===========================================================================

FUNCTION
  bt_pf_goep_sd_register_custom_service

DESCRIPTION
  This regitsers custom 128 bit with SD

  This command is returned immediately and is not placed on the command queue.

===========================================================================*/

bt_cmd_status_type bt_pf_goep_sd_register_custom_service(
                bt_app_id_type               bt_app_id,
                bt_sd_uuid128_type          *uuid128,
                uint8                        scn,
                char*                        service_name_str_ptr)
{
    uint8 i ; 
    bt_cmd_status_type status;
    bt_sd_uuid_list_type uuid_list;
    uint16  psm = BT_L2_PSM_INVALID;
    uint16  ver = 0;

    BT_MSG_DEBUG( "Custom 128 Bit UUID Service Register App: %x, SCN: %x",
                                                        bt_app_id, scn, 0);
    for(i = 0; i < 16 ; i++)
    {
        uuid_list.uuid128[0].uuid_byte[i]= uuid128->uuid_byte[i];
    }

    uuid_list.num_uuid128 = 1;
    uuid_list.num_uuid=0;

    status = bt_cmd_sd_register_custom_service(
                                    bt_app_id,
                                    &uuid_list,
                                    ver,
                                    scn,
                                    psm,
                                    service_name_str_ptr);

    if(status == BT_CS_GN_PENDING)
    {
    status = bt_cmd_sd_custom_update_obex_profile_service_record(
               bt_app_id,
               uuid128,
               scn,
               service_name_str_ptr, /* service name */
               NULL, /* service ID */
               NULL, /* supported_formats_list */
               0,    /* supported capabilities */
               0,    /* supported features */
               0,    /* supported functions */
               0 );  /* imaging capacity */

        if(status == BT_CS_GN_PENDING)
        {
            BT_MSG_DEBUG( "BT PF GOEP SRV : SD OBEX Update done",0,0,0 );
            status = BT_CS_GN_SUCCESS;
        }
        else
        {
            BT_ERR( "Custom 128bitUUID OBEX Record update Failed. App: %x, \
                            SCN: %x, status: %x", bt_app_id, scn, status);
        }
    }
    else
    {
        BT_ERR( "Custom 128 Bit UUID Service Register Failed. App: %x, \
                            SCN: %x, status: %x", bt_app_id, scn, status);
    }
    return status ;
}

#endif /* FEATURE_BT_EXTPF_GOEP */
#endif /* FEATURE_BT */
