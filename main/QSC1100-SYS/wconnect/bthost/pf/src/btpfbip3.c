/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 

 B L U E T O O T H   B A S I C   I M A G I N G   P R O F I L E   S E R V E R

GENERAL DESCRIPTION
  This module contains the Bluetooth Basic Imaging Profile server data and 
  code.

              Copyright (c) 2004-2008 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary                        
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfbip3.c#1 $ 
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2008-11-07   pn  Validates image handle pointer param before use.
  2008-07-24   gs  Fixed Klocwork error. 

*   #26        04 Dec  2007           MH
* Added BD address support for BIP connect indication.
*
*    #25       21 Feb 2006            PN
* Fixed critical lint errors.
*
*    #24       03 Jan 2006            GS
* Initialize SCN parameter in server registration function call.
*
*    #23       12 Nov 2006            PR
* Updated API return values while merging OI Code v3.0.14.
*
*    #22       03 Nov 2006            GS
* Fixed problem in obex authentication.
*
*    #21       16 Jun 2006            DSN
* Removed superfluous header file inclusion.
*
*    #20       18 Jul 2005            GS
* Moved SDP related function definitions to oi_bip_srv.c (to get around 
* compile issues after adding support for BIP srv force dcn)
*
*    #19       12 Jul 2005            JH
* Added BT_PF_TMO_SIG to signal mask when calling rex_timed_wait() which was 
*  previously misused.
*
*    #18       19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #17       11 Jan 2005            JH
* Added back the change made in #15.
*
*    #16       11 Jan 2005            JH
* Modified to avoid possible misaligned memory accesses.
* Backed out changes made in #15. Saber 4262 release not picking these up.
*
*    #15       29 Dec 2004            JH
* Added SCN parameter when calling bt_cmd_sd_update_obex_profile_service_record
*
*    #14       11 Nov 2005            JH
* Eliminated the done field in bt_pf_bip_srv_req_cfm_type and use the signal 
*  itself to tell if a timeout has occurred in bt_pf_bip_srv_wait_for_cfm.
*
*    #13       05 Nov 2004            JH
* Modify deregister command not to free the assigned resources if deregister 
*  failed due to operation in progress.
*
*    #12       19 Oct 2004            JH
* Modified bt_pf_bip_srv_cmd_get_images_list_rsp to process the added number of 
*  returned handles parameter
*
*    #11       31 Aug 2004            JH
* Change OI_BISRV_CreateObjectsServerSDPRecord to 
*  OI_BIPSRV_CreateObjectsServerSDPRecord because the corresponding typo in OI 
*  code has been fixed in the latest revision.
*
*    #10       30 Aug 2004            JH
* Modified the registration commands to call 
*  bt_cmd_sd_update_obex_profile_service_record for updating service record 
*  instead of calling SD functions directly.
* Removed calls for manipulating object transfer bit in class of device field;
*  this is now handled by SD.
*
*    #9        27 Jul 2004
* Clear app id field in server table after deregistration.
*
*    #8        26 Jul 2004            JH
* Revamped BIP signal and timer scheme.
*
*    #7        06 Jun 2004            JH
* Added BIP server start print response to facilitate APIT testing.
* Fixed authentication problem.
* Modified ind callbacks to properly handle NULL pointers in request struct.
*
*    #6        14 May 2004            JH
* Further command and event struct changes
* Modified code for setting object transfer bit in class od service field
*
*    #4        23 Apr 2004            JH
* Replaced all bt_pf_cmd* with bt_cmd_pf*; replaced all BT_PF_EV* with
*  BT_EV_PF*
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
#ifdef FEATURE_BT_EXTPF_BIP

#include "bt.h"
#include "btpfi.h"
#include "bti.h"
#include "btmsg.h"
#include "btpfbipi.h" 
#include "btsd.h"
#include "rex.h"       
#include "oi_bip_srv.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/ 

/* Currently, only one BIP server of each type is supported */
typedef uint16 bt_pf_bip_srv_conn_id_type;

typedef struct
{
  bt_app_id_type             app_id;
  bt_pf_bip_srv_conn_id_type conn_id;
  uint8                      scn;
} bt_pf_bip_srv_info_type;

/* only one server of each type is supported */
LOCAL bt_pf_bip_srv_info_type bt_pf_bip_srv_info[BT_PF_BIP_NUM_SRV] =
{ 
  { BT_PF_APP_ID_NULL, BT_PF_BIP_NO_CONN_ID, BT_SPP_SCN_UNSPECIFIED },
  { BT_PF_APP_ID_NULL, BT_PF_BIP_NO_CONN_ID, BT_SPP_SCN_UNSPECIFIED },
  { BT_PF_APP_ID_NULL, BT_PF_BIP_NO_CONN_ID, BT_SPP_SCN_UNSPECIFIED }
};

typedef struct
{
  bt_pf_bip_srv_conn_id_type conn_id;
  bt_event_type              event;
  bt_cmd_status_type         status;
} bt_pf_bip_srv_req_cfm_type;

/* structure for holding info about the operation completion confirmation */
LOCAL bt_pf_bip_srv_req_cfm_type  bt_pf_bip_srv_req_cfm;

/* data structures for passing parameters into OI BIP calls */
LOCAL OI_BIP_GET_CAPABILITIES_PARAMS      bt_pf_bip_srv_get_capabilities_params;
LOCAL OI_BIP_PUT_IMAGE_PARAMS             bt_pf_bip_srv_put_image_params;
LOCAL OI_BIP_PUT_LINKED_ATTACHMENT_PARAMS bt_pf_bip_srv_put_attachment_params;
LOCAL OI_BIP_PUT_LINKED_THUMBNAIL_PARAMS  bt_pf_bip_srv_put_thumbnail_params;
LOCAL OI_BIP_GET_IMAGES_LIST_PARAMS       bt_pf_bip_srv_get_list_params;
LOCAL OI_BIP_GET_IMAGE_PROPERTIES_PARAMS  bt_pf_bip_srv_get_properties_params;
LOCAL OI_BIP_GET_IMAGE_PARAMS             bt_pf_bip_srv_get_image_params;
LOCAL OI_BIP_GET_LINKED_THUMBNAIL_PARAMS  bt_pf_bip_srv_get_thumbnail_params;
LOCAL OI_BIP_GET_LINKED_ATTACHMENT_PARAMS bt_pf_bip_srv_get_attachment_params;
LOCAL OI_BIP_DELETE_IMAGE_PARAMS          bt_pf_bip_srv_delete_image_params;
LOCAL OI_BIP_START_ARCHIVE_PARAMS         bt_pf_bip_srv_start_archive_params;
LOCAL OI_BIP_GET_PARTIAL_IMAGE_PARAMS     bt_pf_bip_srv_get_partial_image_params;
LOCAL OI_BIP_REMOTE_DISPLAY_PARAMS        bt_pf_bip_srv_remote_display_params;
LOCAL OI_BIP_GET_MONITORING_IMAGE_PARAMS  bt_pf_bip_srv_monitor_image_params;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif                                                                         

LOCAL uint16 local_handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_wait_for_cfm

DESCRIPTION
  Wait for application to finish processing incoming event. Returns status
  code suuplied by application.

===========================================================================*/
LOCAL bt_cmd_status_type bt_pf_bip_srv_wait_for_cfm( 
  bt_pf_bip_srv_conn_id_type conn_id,
  bt_event_type              req_event )
{
  OI_STATUS status = OI_TIMEOUT;
                                   
  rex_timed_wait( BT_PF_BIP_SRV_SIG | BT_PF_TMO_SIG, 
                  &bt_pf_bip_srv_tmo_timer, 
                  BT_PF_BIP_OPS_TMO );

  if ( rex_get_sigs( &bt_pf_tcb ) & BT_PF_BIP_SRV_SIG )
  {
    rex_clr_timer( &bt_pf_bip_srv_tmo_timer );
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    if ( bt_pf_bip_srv_req_cfm.event != req_event )
    {
      BT_ERR( "BT PF BIP SRV CB CFM: ev mismatch, ev: %x", 
              req_event, 0, 0 ); 
      status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
    else
    {
      status = (OI_STATUS)bt_pf_bip_srv_req_cfm.status;
    }
  }
  else if ( rex_get_sigs( &bt_pf_tcb ) & BT_PF_TMO_SIG )
  {
    /* signal not set => timeout has occurred */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_TMO_SIG );
    BT_ERR( "BT PF BIP SRV CB CFM: timeout", 0, 0, 0 );
  }                                   

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_app_conn_find    
   
DESCRIPTION
  Finds the application ID corresponding to the given connection ID.
   
===========================================================================*/

LOCAL uint8 bt_pf_bip_srv_app_conn_find(
  bt_pf_bip_srv_conn_id_type conn_id,
  bt_app_id_type*            app_id_ptr)
{
  uint8 i;

  for (i = 0; i < BT_PF_BIP_NUM_SRV; i++)
  {
    if (bt_pf_bip_srv_info[i].conn_id == conn_id)
    { 
    if (app_id_ptr != NULL)
      *app_id_ptr = bt_pf_bip_srv_info[i].app_id;
      break;
    }
  }
  return(i);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_copy_handle
   
DESCRIPTION
  Handle is copied to avoid misaligned access when str pointer is not aligned
   
===========================================================================*/
LOCAL void bt_pf_bip_srv_copy_handle(
  OI_OBEX_UNICODE* dest_handle,
  uint16*          source_handle_ptr,
  OI_STATUS        status
)
{
  if ( (OI_OK == status) || (OI_OBEX_CONTINUE == status) ||
       (OI_OBEX_PARTIAL_CONTENT == status) )
  {
    memcpy( local_handle_str, source_handle_ptr, sizeof( local_handle_str ) );
    dest_handle->len = ARR_SIZE( local_handle_str );
    dest_handle->str = local_handle_str;
  }
  else
  {
    dest_handle->len = 0;
    dest_handle->str = NULL;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_imaging_responder_connect_ind_cb

DESCRIPTION
  Callback function invoked when an incoming connection request to the
  imaging responder is received.

===========================================================================*/
LOCAL void bt_pf_bip_srv_imaging_responder_connect_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE conn_id)
{
  bt_ev_msg_type event;
  OI_STATUS      ret_status;
  OI_BD_ADDR     client_addr;

  BT_MSG_DEBUG( "BT PF BIP SRV CB RX, img resp con, conn: %x", conn_id, 0, 0 );

  if ( ( bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].conn_id != 
           BT_PF_BIP_NO_CONN_ID) && 
       ( bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].conn_id !=
           (bt_pf_bip_srv_conn_id_type)conn_id ) )
  {
    /* authentication is in progress on another connection */
    BT_ERR( "BT PF BIP SRV CB RX: img resp con, unknown conn: %x", 
            conn_id, 0, 0 );
    return;
  }

  OI_OBEXSRV_GetClientAddr(conn_id, &client_addr);
  event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_CON_IND;
  event.ev_hdr.bt_app_id = 
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].app_id;
  event.ev_msg.ev_bip_srv_con_ind.conn_id = 
    (bt_pf_bip_srv_conn_id_type)conn_id;

  memcpy( (void*)&(event.ev_msg.ev_bip_srv_con_ind.bd_addr),
            (void*)&client_addr,
            sizeof( bt_bd_addr_type ) );

  bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].conn_id = 
    (bt_pf_bip_srv_conn_id_type)conn_id;

  bt_pf_bip_srv_req_cfm.conn_id = conn_id;

  /* clear sig first in case an earlier cfm was received after tmo expired */ 
  rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG );
  bt_ec_send_event( &event );

  ret_status = bt_pf_bip_srv_wait_for_cfm( 
    (bt_pf_bip_srv_conn_id_type)conn_id, 
    BT_EV_PF_BIP_SRV_CON_IND );

  if (ret_status != OI_OK) /* not accepting */
  {
    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_CONNECTION_TIMEOUT;
    }
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].conn_id = 
      BT_PF_BIP_NO_CONN_ID;
  }

  ret_status = OI_BIPSRV_AcceptConnection(conn_id,ret_status);
   if(!ret_status)
   {
     BT_ERR("AcceptConnect failed %!",ret_status,0,0);
   }

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_referenced_object_connect_ind_cb

DESCRIPTION
  Callback function invoked when an incoming connection request to the
  referenced object server is received.

===========================================================================*/
LOCAL void bt_pf_bip_srv_referenced_object_connect_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE conn_id)
{
  bt_ev_msg_type event;
  OI_STATUS      ret_status;
  OI_BD_ADDR     client_addr;

  BT_MSG_DEBUG( "BT PF BIP SRV CB RX, ref obj con, conn: %x", conn_id, 0, 0 );

  if ( ( bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].conn_id != 
           BT_PF_BIP_NO_CONN_ID) && 
       ( bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].conn_id !=
           (bt_pf_bip_srv_conn_id_type)conn_id ) )
  {
    /* authentication is in progress on another connection */
    BT_ERR( "BT PF BIP SRV CB RX: ref obj con, unknown conn: %x", 
            conn_id, 0, 0 );
    return;
  }

  OI_OBEXSRV_GetClientAddr(conn_id, &client_addr);
  event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_CON_IND;
  event.ev_hdr.bt_app_id = 
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].app_id;
  event.ev_msg.ev_bip_srv_con_ind.conn_id = 
    (bt_pf_bip_srv_conn_id_type)conn_id;

  memcpy( (void*)&(event.ev_msg.ev_bip_srv_con_ind.bd_addr),
            (void*)&client_addr,
            sizeof( bt_bd_addr_type ) );


  bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].conn_id = 
    (bt_pf_bip_srv_conn_id_type)conn_id;

  bt_pf_bip_srv_req_cfm.conn_id = conn_id;

  /* clear sig first in case an earlier cfm was received after tmo expired */ 
  rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG );
  bt_ec_send_event( &event );

  ret_status = bt_pf_bip_srv_wait_for_cfm( 
    (bt_pf_bip_srv_conn_id_type)conn_id, 
    BT_EV_PF_BIP_SRV_CON_IND );

  if (ret_status != OI_OK) /* not accepting */
  {
    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_CONNECTION_TIMEOUT;
    }
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].conn_id = 
      BT_PF_BIP_NO_CONN_ID;
  }

  ret_status = OI_BIPSRV_AcceptConnection(conn_id,ret_status);
   if(!ret_status)
   {
     BT_ERR("AcceptConnect failed %!",ret_status,0,0);
   }

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_archived_object_connect_ind_cb

DESCRIPTION
  Callback function invoked when an incoming connection request to the
  archived object server is received.

===========================================================================*/
LOCAL void bt_pf_bip_srv_archived_object_connect_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE conn_id)
{
  bt_ev_msg_type event;
  OI_STATUS      ret_status;
  OI_BD_ADDR     client_addr;

  BT_MSG_DEBUG( "BT PF BIP SRV CB RX, arch obj con, conn: %x", conn_id, 0, 0 );

  if ( ( bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].conn_id != 
           BT_PF_BIP_NO_CONN_ID) && 
       ( bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].conn_id !=
           (bt_pf_bip_srv_conn_id_type)conn_id ) )
  {
    /* authentication is in progress on another connection */
    BT_ERR( "BT PF BIP SRV CB RX: arch obj con, unknown conn: %x",
             conn_id, 0, 0 );
    return;
  }

  OI_OBEXSRV_GetClientAddr(conn_id, &client_addr);
  event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_CON_IND;
  event.ev_hdr.bt_app_id = 
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].app_id;
  event.ev_msg.ev_bip_srv_con_ind.conn_id = 
    (bt_pf_bip_srv_conn_id_type)conn_id;

  memcpy( (void*)&(event.ev_msg.ev_bip_srv_con_ind.bd_addr),
            (void*)&client_addr,
            sizeof( bt_bd_addr_type ) );

  bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].conn_id = 
    (bt_pf_bip_srv_conn_id_type)conn_id;

  bt_pf_bip_srv_req_cfm.conn_id = conn_id;

  /* clear sig first in case an earlier cfm was received after tmo expired */ 
  rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG );
  bt_ec_send_event( &event );

  ret_status = bt_pf_bip_srv_wait_for_cfm( 
    (bt_pf_bip_srv_conn_id_type)conn_id, 
    BT_EV_PF_BIP_SRV_CON_IND );

  if (ret_status != OI_OK) /* not accepting */
  {
    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_CONNECTION_TIMEOUT;
    }
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].conn_id = 
      BT_PF_BIP_NO_CONN_ID;
  }

  ret_status = OI_BIPSRV_AcceptConnection(conn_id,ret_status);
  if(!ret_status)
  {
    BT_ERR("AcceptConnect failed %!",ret_status,0,0);
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_disconnect_ind_cb

DESCRIPTION
  Callback function invoked when a remote BIP client is disconnecting from 
  the local BIP server.

===========================================================================*/
LOCAL void bt_pf_bip_srv_disconnect_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE conn_id)
{
  bt_ev_msg_type    event;
  uint8             srv_idx;
  bt_app_id_type    app_id = BT_PF_APP_ID_NULL;

  srv_idx = bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if ( (app_id != BT_PF_APP_ID_NULL) && (srv_idx < BT_PF_BIP_NUM_SRV) )
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: dcn, conn: %x", conn_id, 0, 0 );
    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_DCN_IND;
    event.ev_msg.ev_bip_srv_con_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    bt_ec_send_event( &event );

    bt_pf_bip_srv_info[srv_idx].conn_id = BT_PF_BIP_NO_CONN_ID;
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: dcn, unknown conn: %x, app_id: %x, idx: %x", 
            conn_id, app_id, srv_idx );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_auth_ind_cb

DESCRIPTION
  Callback function invoked to indicate an authentication request from a
  remote BIP client.

===========================================================================*/
LOCAL void bt_pf_bip_srv_auth_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE conn_id)
{
  bt_pf_bip_srv_info_type* bsi_ptr = NULL;
  bt_ev_msg_type    event;
  uint8             scn;
  uint16            i;

  /* at this point, connection is not established yet, so we cannot obtain  */
  /* app_id from conn_id; need to use the SCN of the incoming connection to */
  /* get the app_id for sending event */

  /* no need to check for error here because the conn_id and scn mapping */
  /* must be valid for this callback to be invoked */
  OI_OBEXSRV_GetRfcommChannel( conn_id, &scn );

  for (i = 0; i < BT_PF_BIP_NUM_SRV; i++)
  {
    if ( bt_pf_bip_srv_info[i].scn == scn )
    {
      bsi_ptr = &bt_pf_bip_srv_info[i];
      break;
    }
  }

  if (bsi_ptr != NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: auth, conn: %x, app: %x", 
                  conn_id, bsi_ptr->app_id, 0 );
  
    bsi_ptr->conn_id = (bt_pf_bip_srv_conn_id_type)conn_id;
  
    event.ev_hdr.bt_app_id = bsi_ptr->app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_AUTH_IND;
    event.ev_msg.ev_bip_srv_auth_ind.target = (bt_pf_bip_target_type)i;
    event.ev_msg.ev_bip_srv_auth_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: auth, unexpected conn: %x", conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_get_capabilities_ind_cb

DESCRIPTION
  Callback function invoked to indicate a get capabilities request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_get_capabilities_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE     conn_id,
  OI_BIP_GET_CAPABILITIES_PARAMS  *params_ptr,
  OI_BIP_REQUEST_STATE            state,
  OI_STATUS                       status)
{                            
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: get cap, conn: %x, app: %x", 
                  conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_GET_CAPABILITIES_IND;
    event.ev_msg.ev_bip_srv_get_capabilities_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_get_capabilities_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_get_capabilities_ind.status = 
      (bt_cmd_status_type)status;

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG );
    bt_ec_send_event( &event );

    ret_status = bt_pf_bip_srv_wait_for_cfm( 
       (bt_pf_bip_srv_conn_id_type)conn_id, 
       BT_EV_PF_BIP_SRV_GET_CAPABILITIES_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: get cap, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_put_image_ind_cb

DESCRIPTION
  Callback function invoked to indicate a put image request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_put_image_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE     conn_id,
  OI_BIP_PUT_IMAGE_PARAMS         *params_ptr,
  OI_BIP_REQUEST_STATE            state,
  OI_STATUS                       status)
{                            
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;
  uint8             image_name_len;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: put img, conn: %x, app: %x", 
                  conn_id, app_id, 0 );
    
    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_PUT_IMAGE_IND;
    event.ev_msg.ev_bip_srv_put_image_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_put_image_ind.final = 
      params_ptr->request.final;
    event.ev_msg.ev_bip_srv_put_image_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_put_image_ind.status = 
      (bt_cmd_status_type)status;

    if ( (params_ptr->request.imageName == NULL) || 
         (params_ptr->request.imageName->len == 0) )
    {
      event.ev_msg.ev_bip_srv_put_image_ind.image_name_len = 0;
      event.ev_msg.ev_bip_srv_put_image_ind.image_name_str[0] = 0;
    }
    else
    {
      /* copy name, truncate when necessary */
      image_name_len = bt_pf_process_unicode_str(
        (uint16*)event.ev_msg.ev_bip_srv_put_image_ind.image_name_str,
        (bt_pf_unicode_str_type*)(params_ptr->request.imageName),
        BT_PF_MAX_FILENAME_LEN ); 
      event.ev_msg.ev_bip_srv_put_image_ind.image_name_len = image_name_len;
    }

    if ( params_ptr->request.imageDescriptor == NULL )
    {   
      event.ev_msg.ev_bip_srv_put_image_ind.image_descriptior_ptr = NULL;
      event.ev_msg.ev_bip_srv_put_image_ind.image_descriptior_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_srv_put_image_ind.image_descriptior_ptr = 
        params_ptr->request.imageDescriptor->data;
      event.ev_msg.ev_bip_srv_put_image_ind.image_descriptior_len = 
        params_ptr->request.imageDescriptor->len;
    }

    if ( params_ptr->request.data == NULL )
    {
      event.ev_msg.ev_bip_srv_put_image_ind.data_ptr = NULL;
      event.ev_msg.ev_bip_srv_put_image_ind.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_srv_put_image_ind.data_ptr = 
        params_ptr->request.data->data;
      event.ev_msg.ev_bip_srv_put_image_ind.data_len = 
        params_ptr->request.data->len;
    }

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG );
    bt_ec_send_event( &event );

    ret_status = bt_pf_bip_srv_wait_for_cfm( 
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_PUT_IMAGE_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: put img, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_put_linked_thumbnail_ind_cb

DESCRIPTION
  Callback function invoked to indicate a put linked thumbnail request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_put_linked_thumbnail_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE         conn_id,
  OI_BIP_PUT_LINKED_THUMBNAIL_PARAMS  *params_ptr,
  OI_BIP_REQUEST_STATE                state,
  OI_STATUS                           status)
{
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;
  uint16            len;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: put tnl, conn: %x, app: %x", 
                  conn_id, app_id, 0 );
    
    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_PUT_LINKED_THUMBNAIL_IND;
    event.ev_msg.ev_bip_srv_put_thumbnail_ind.conn_id =
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_put_thumbnail_ind.final = 
      params_ptr->request.final;
    event.ev_msg.ev_bip_srv_put_thumbnail_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_put_thumbnail_ind.status = 
      (bt_cmd_status_type)status;

    memset( (void*)&(event.ev_msg.ev_bip_srv_put_thumbnail_ind.image_handle),
            0, (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
    if (params_ptr->request.imageHandle != NULL)
    {                                    
      if (params_ptr->request.imageHandle->len > BT_PF_BIP_IMAGE_HANDLE_LEN + 1)
      {
        /* image handle is always 7 characters, null terminated */
        len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
      }
      else
      {
        len = params_ptr->request.imageHandle->len;
      }
      memcpy( (void*)&(event.ev_msg.ev_bip_srv_put_thumbnail_ind.image_handle),
              (void*)params_ptr->request.imageHandle->str,
              len * sizeof( uint16 ) );
    }

    if ( params_ptr->request.data == NULL )
    {
      event.ev_msg.ev_bip_srv_put_thumbnail_ind.data_ptr = NULL;
      event.ev_msg.ev_bip_srv_put_thumbnail_ind.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_srv_put_thumbnail_ind.data_ptr = 
        params_ptr->request.data->data;
      event.ev_msg.ev_bip_srv_put_thumbnail_ind.data_len = 
        params_ptr->request.data->len;
    } 

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );

    ret_status = bt_pf_bip_srv_wait_for_cfm(
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_PUT_LINKED_THUMBNAIL_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: put tnl, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_put_linked_attachment_ind_cb

DESCRIPTION
  Callback function invoked to indicate a put linked attachment request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_put_linked_attachment_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE         conn_id,
  OI_BIP_PUT_LINKED_ATTACHMENT_PARAMS *params_ptr,
  OI_BIP_REQUEST_STATE                state,
  OI_STATUS                           status)
{
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;
  uint16            len;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
          &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: put att, conn: %x, app: %x", 
                  conn_id, app_id, 0 );
    
    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_PUT_LINKED_ATTACHMENT_IND;
    event.ev_msg.ev_bip_srv_put_attachment_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_put_attachment_ind.final = 
      params_ptr->request.final;
    event.ev_msg.ev_bip_srv_put_attachment_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_put_attachment_ind.status = 
      (bt_cmd_status_type)status;

    memset( (void*)&(event.ev_msg.ev_bip_srv_put_attachment_ind.image_handle),
            0, (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
    if (params_ptr->request.imageHandle != NULL)
    {                                          
      if (params_ptr->request.imageHandle->len > BT_PF_BIP_IMAGE_HANDLE_LEN + 1)
      {
        /* image handle is always 7 characters, null terminated */
        len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
      }
      else
      {
        len = params_ptr->request.imageHandle->len;
      }
      memcpy( (void*)&(event.ev_msg.ev_bip_srv_put_attachment_ind.image_handle),
              (void*)params_ptr->request.imageHandle->str,
              len * sizeof( uint16 ) );
    }

    if (params_ptr->request.attachmentDescriptor == NULL)
    {
      event.ev_msg.ev_bip_srv_put_attachment_ind.attachment_descriptor_ptr = NULL;
      event.ev_msg.ev_bip_srv_put_attachment_ind.attachment_descriptor_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_srv_put_attachment_ind.attachment_descriptor_ptr = 
        params_ptr->request.attachmentDescriptor->data;
      event.ev_msg.ev_bip_srv_put_attachment_ind.attachment_descriptor_len = 
        params_ptr->request.attachmentDescriptor->len;
    }

    if (params_ptr->request.data == NULL)
    {
      event.ev_msg.ev_bip_srv_put_attachment_ind.data_ptr = NULL;
      event.ev_msg.ev_bip_srv_put_attachment_ind.data_len = 0;
    }
    else
    { 
      event.ev_msg.ev_bip_srv_put_attachment_ind.data_ptr = 
        params_ptr->request.data->data;
      event.ev_msg.ev_bip_srv_put_attachment_ind.data_len = 
        params_ptr->request.data->len;
    }

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );

    ret_status = bt_pf_bip_srv_wait_for_cfm(
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_PUT_LINKED_ATTACHMENT_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: put att, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_get_images_list_ind_cb

DESCRIPTION
  Callback function invoked to indicate a get images list request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_get_images_list_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE     conn_id,
  OI_BIP_GET_IMAGES_LIST_PARAMS   *params_ptr,
  OI_BIP_REQUEST_STATE            state,
  OI_STATUS                       status)
{
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
  &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: get list, conn: %x, app: %x", 
                  conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_GET_IMAGES_LIST_IND;
    event.ev_msg.ev_bip_srv_get_list_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
  event.ev_msg.ev_bip_srv_get_list_ind.handle_count = 
      params_ptr->request.handleCount;
  event.ev_msg.ev_bip_srv_get_list_ind.list_offset = 
      params_ptr->request.handleOffset;
  event.ev_msg.ev_bip_srv_get_list_ind.latest = 
      params_ptr->request.latest;
    event.ev_msg.ev_bip_srv_get_list_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_get_list_ind.status = 
      (bt_cmd_status_type)status;

    if (params_ptr->request.handlesDescriptor == NULL)
    {
      event.ev_msg.ev_bip_srv_get_list_ind.handles_descriptor_ptr = NULL;
      event.ev_msg.ev_bip_srv_get_list_ind.handles_descriptor_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_srv_get_list_ind.handles_descriptor_ptr = 
        params_ptr->request.handlesDescriptor->data;
      event.ev_msg.ev_bip_srv_get_list_ind.handles_descriptor_len = 
        params_ptr->request.handlesDescriptor->len;
    }

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );
  
    ret_status = bt_pf_bip_srv_wait_for_cfm( 
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_GET_IMAGES_LIST_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: get list, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_get_image_properties_ind_cb

DESCRIPTION
  Callback function invoked to indicate a get image properties request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_get_image_properties_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE         conn_id,
  OI_BIP_GET_IMAGE_PROPERTIES_PARAMS  *params_ptr,
  OI_BIP_REQUEST_STATE                state,
  OI_STATUS                           status)
{
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;
  uint16            len;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
  &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: get prop, conn: %x, app: %x", 
                  conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_GET_IMAGE_PROPERTIES_IND;
    event.ev_msg.ev_bip_srv_get_properties_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_get_properties_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_get_properties_ind.status = 
      (bt_cmd_status_type)status;

    memset( (void*)&(event.ev_msg.ev_bip_srv_get_properties_ind.image_handle),
             0, (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
    if (params_ptr->request.imageHandle != NULL)
    {                                          
      if (params_ptr->request.imageHandle->len > BT_PF_BIP_IMAGE_HANDLE_LEN + 1)
      {
        /* image handle is always 7 characters, null terminated */
        len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
      }
      else
      {
        len = params_ptr->request.imageHandle->len;
      }
      memcpy( (void*)&(event.ev_msg.ev_bip_srv_get_properties_ind.image_handle),
              (void*)params_ptr->request.imageHandle->str,
              len * sizeof( uint16 ) );
    }

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );
    
    ret_status = bt_pf_bip_srv_wait_for_cfm(
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_GET_IMAGE_PROPERTIES_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: get prop, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_get_image_ind_cb

DESCRIPTION
  Callback function invoked to indicate a get image request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_get_image_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE     conn_id,
  OI_BIP_GET_IMAGE_PARAMS         *params_ptr,
  OI_BIP_REQUEST_STATE            state,
  OI_STATUS                       status)
{                            
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;
  uint16            len;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: get img, conn: %x, app: %x", 
            conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_GET_IMAGE_IND;
    event.ev_msg.ev_bip_srv_get_image_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_get_image_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_get_image_ind.status = 
      (bt_cmd_status_type)status;

    memset( (void*)&(event.ev_msg.ev_bip_srv_get_image_ind.image_handle),
            0, (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
    if (params_ptr->request.imageHandle != NULL)
    {                                          
      if (params_ptr->request.imageHandle->len > BT_PF_BIP_IMAGE_HANDLE_LEN + 1)
      {
        /* image handle is always 7 characters, null terminated */
        len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
      }
      else
      {
        len = params_ptr->request.imageHandle->len;
      }                                                               
      memcpy( (void*)&(event.ev_msg.ev_bip_srv_get_image_ind.image_handle),
              (void*)params_ptr->request.imageHandle->str,
              len * sizeof( uint16 ) );
    }

    if (params_ptr->request.imageDescriptor == NULL)
    {
      event.ev_msg.ev_bip_srv_get_image_ind.image_descriptor_ptr = NULL;
      event.ev_msg.ev_bip_srv_get_image_ind.image_descriptor_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_srv_get_image_ind.image_descriptor_ptr = 
        params_ptr->request.imageDescriptor->data;
      event.ev_msg.ev_bip_srv_get_image_ind.image_descriptor_len = 
        params_ptr->request.imageDescriptor->len;
    }

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );

    ret_status = bt_pf_bip_srv_wait_for_cfm( 
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_GET_IMAGE_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: get img, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_get_linked_thumbnail_ind_cb

DESCRIPTION
  Callback function invoked to indicate a get linked thumbnail request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_get_linked_thumbnail_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE         conn_id,
  OI_BIP_GET_LINKED_THUMBNAIL_PARAMS  *params_ptr,
  OI_BIP_REQUEST_STATE                state,
  OI_STATUS                           status)
{
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;
  uint16            len;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: get tnl, conn: %x, app: %x", 
                  conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_GET_LINKED_THUMBNAIL_IND;
    event.ev_msg.ev_bip_srv_get_thumbnail_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;           
    event.ev_msg.ev_bip_srv_get_thumbnail_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_get_thumbnail_ind.status = 
      (bt_cmd_status_type)status;

    memset( (void*)&(event.ev_msg.ev_bip_srv_get_thumbnail_ind.image_handle),
            0, (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
    if (params_ptr->request.imageHandle != NULL)
    {                                           
      if (params_ptr->request.imageHandle->len > BT_PF_BIP_IMAGE_HANDLE_LEN + 1)
      {
        /* image handle is always 7 characters, null terminated */
        len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
      }
      else
      {
        len = params_ptr->request.imageHandle->len;
      }                                                                
      memcpy( (void*)&(event.ev_msg.ev_bip_srv_get_thumbnail_ind.image_handle),
              (void*)params_ptr->request.imageHandle->str,
              len * sizeof( uint16 ) );
    }

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );

    ret_status = bt_pf_bip_srv_wait_for_cfm(
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_GET_LINKED_THUMBNAIL_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: get tnl, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;  
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_get_linked_attachment_ind_cb

DESCRIPTION
  Callback function invoked to indicate a get linked attachment request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_get_linked_attachment_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE         conn_id,
  OI_BIP_GET_LINKED_ATTACHMENT_PARAMS *params_ptr,
  OI_BIP_REQUEST_STATE                state,
  OI_STATUS                           status)
{
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;
  uint8             attachment_name_len;
  uint16            len;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: get att, conn: %x, app: %x", 
                  conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_GET_LINKED_ATTACHMENT_IND;
    event.ev_msg.ev_bip_srv_get_attachment_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_get_attachment_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_get_attachment_ind.status = 
      (bt_cmd_status_type)status;

    memset( (void*)&(event.ev_msg.ev_bip_srv_get_attachment_ind.image_handle),
            0, (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
    if (params_ptr->request.imageHandle != NULL)
    {                                          
      if (params_ptr->request.imageHandle->len > BT_PF_BIP_IMAGE_HANDLE_LEN + 1)
      {
        /* image handle is always 7 characters, null terminated */
        len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
      }
      else
      {
        len = params_ptr->request.imageHandle->len;
      }
      memcpy( (void*)&(event.ev_msg.ev_bip_srv_get_attachment_ind.image_handle),
              (void*)params_ptr->request.imageHandle->str,
              len * sizeof( uint16 ) );
    }

    if ( (params_ptr->request.attachmentName == NULL) || 
         (params_ptr->request.attachmentName->len == 0) )
    {
      event.ev_msg.ev_bip_srv_get_attachment_ind.attachment_name_len = 0;
      event.ev_msg.ev_bip_srv_get_attachment_ind.attachment_name_str[0] = 0;
    }
    else
    {
      /* copy name, truncate when necessary */
      attachment_name_len = bt_pf_process_unicode_str(
        (uint16*)event.ev_msg.ev_bip_srv_get_attachment_ind.attachment_name_str,
        (bt_pf_unicode_str_type*)(params_ptr->request.attachmentName),
        BT_PF_MAX_FILENAME_LEN );
      event.ev_msg.ev_bip_srv_get_attachment_ind.attachment_name_len = 
        attachment_name_len;
    }

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );
    
    ret_status = bt_pf_bip_srv_wait_for_cfm(
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_GET_LINKED_ATTACHMENT_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: get att, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_delete_image_ind_cb

DESCRIPTION
  Callback function invoked to indicate a delete image request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_delete_image_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE         conn_id,
  OI_BIP_DELETE_IMAGE_PARAMS          *params_ptr,
  OI_BIP_REQUEST_STATE                state,
  OI_STATUS                           status)
{                            
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;
  uint16            len;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: del img, conn: %x, app: %x", 
                  conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_DELETE_IMAGE_IND;
    event.ev_msg.ev_bip_srv_delete_image_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_delete_image_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_delete_image_ind.status = 
      (bt_cmd_status_type)status;

    memset( (void*)&(event.ev_msg.ev_bip_srv_delete_image_ind.image_handle),
            0, (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
    if (params_ptr->request.imageHandle != NULL)
    {                                          
      if (params_ptr->request.imageHandle->len > BT_PF_BIP_IMAGE_HANDLE_LEN + 1)
      {
        /* image handle is always 7 characters, null terminated */
        len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
      }
      else
      {
        len = params_ptr->request.imageHandle->len;
      }
      memcpy( (void*)&(event.ev_msg.ev_bip_srv_delete_image_ind.image_handle),
              (void*)params_ptr->request.imageHandle->str,
              len * sizeof( uint16 ) );
    }

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );
    
    ret_status = bt_pf_bip_srv_wait_for_cfm(
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_DELETE_IMAGE_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: del img, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_get_status_ind_cb

DESCRIPTION
  Callback function invoked to indicate a get status request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_get_status_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE         conn_id,
  OI_STATUS                           status)
{                            
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: get stat, conn: %x, app: %x", 
                conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_GET_STATUS_IND;
    event.ev_msg.ev_bip_srv_get_status_ind.conn_id =
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_get_status_ind.status = 
      (bt_cmd_status_type)status;

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );

    ret_status = bt_pf_bip_srv_wait_for_cfm(
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_GET_STATUS_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: get stat, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_get_partial_image_ind_cb

DESCRIPTION
  Callback function invoked to indicate a get partial image request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_get_partial_image_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE         conn_id,
  OI_BIP_GET_PARTIAL_IMAGE_PARAMS     *params_ptr,
  OI_BIP_REQUEST_STATE                state,
  OI_STATUS                           status)
{
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;
  uint8             image_name_len;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: get part img, conn: %x, app: %x", 
                  conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_GET_PARTIAL_IMAGE_IND;
    event.ev_msg.ev_bip_srv_get_partial_image_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_get_partial_image_ind.partial_length = 
      params_ptr->request.partialLength;
    event.ev_msg.ev_bip_srv_get_partial_image_ind.partial_offset = 
      params_ptr->request.partialOffset;
    event.ev_msg.ev_bip_srv_get_partial_image_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_get_partial_image_ind.status = 
      (bt_cmd_status_type)status;

    if ( (params_ptr->request.imageName == NULL) || 
         (params_ptr->request.imageName->len == 0) )
    {
      event.ev_msg.ev_bip_srv_get_partial_image_ind.image_name_len = 0;
      event.ev_msg.ev_bip_srv_get_partial_image_ind.image_name_str[0] = 0;
    }
    else
    {
      /* copy name, truncate when necessary */
      image_name_len = bt_pf_process_unicode_str(
        (uint16*)event.ev_msg.ev_bip_srv_get_partial_image_ind.image_name_str,
        (bt_pf_unicode_str_type*)(params_ptr->request.imageName),
        BT_PF_MAX_FILENAME_LEN );   
      event.ev_msg.ev_bip_srv_get_partial_image_ind.image_name_len = 
        image_name_len;
    }

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );

    ret_status = bt_pf_bip_srv_wait_for_cfm(
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_GET_PARTIAL_IMAGE_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: get part img, unknown conn: %x", 
            conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_start_archive_ind_cb

DESCRIPTION
  Callback function invoked to indicate a start archive request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_start_archive_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE         conn_id,
  OI_BIP_START_ARCHIVE_PARAMS         *params_ptr,
  OI_BIP_REQUEST_STATE                state,
  OI_STATUS                           status)
{                            
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;
  uint16            i;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: strt arch, conn: %x, app: %x", 
                  conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_START_ARCHIVE_IND;
    event.ev_msg.ev_bip_srv_start_archive_ind.conn_id =
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_start_archive_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_start_archive_ind.status = 
      (bt_cmd_status_type)status;
    
    /* OI_UUID128 is not packed but bt_sd_uuid128_type is, cannot cast.
       Use memcpy to avoid unaligned memory access. */
    memcpy( (void*)&(event.ev_msg.ev_bip_srv_start_archive_ind.
                     arch_obj_srv_service_id),
            (void*)&(params_ptr->request.secondaryServiceId.ms32bits),
            sizeof( uint32 ) );
    for (i = 0; i < OI_BT_UUID128_SIZE - sizeof( OI_UINT32 ); i++)
    {
      event.ev_msg.ev_bip_srv_start_archive_ind.arch_obj_srv_service_id.
        uuid_byte[i + 4] = params_ptr->request.secondaryServiceId.base[i];
    }

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );

    ret_status = bt_pf_bip_srv_wait_for_cfm(
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_START_ARCHIVE_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: strt arch, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_remote_display_ind_cb

DESCRIPTION
  Callback function invoked to indicate a remote display request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_remote_display_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE         conn_id,
  OI_BIP_REMOTE_DISPLAY_PARAMS        *params_ptr,
  OI_BIP_REQUEST_STATE                state,
  OI_STATUS                           status)
{                            
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;
  uint16            len;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: rmt disp, conn: %x, app: %x", 
                  conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_REMOTE_DISPLAY_IND;
    event.ev_msg.ev_bip_srv_remote_display_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_remote_display_ind.display_command = 
      params_ptr->request.displayCommand;
    event.ev_msg.ev_bip_srv_remote_display_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_remote_display_ind.status = 
      (bt_cmd_status_type)status;

    memset( (void*)&(event.ev_msg.ev_bip_srv_remote_display_ind.image_handle),
            0, (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
    if (params_ptr->request.imageHandle != NULL)
    {                                          
      if (params_ptr->request.imageHandle->len > BT_PF_BIP_IMAGE_HANDLE_LEN + 1)
      {
        /* image handle is always 7 characters, null terminated */
        len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
      }
      else
      {
        len = params_ptr->request.imageHandle->len;
      }                                                                
      memcpy( (void*)&(event.ev_msg.ev_bip_srv_remote_display_ind.image_handle),
              (void*)params_ptr->request.imageHandle->str,
              len * sizeof( uint16 ) );
    }

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );
    
    ret_status = bt_pf_bip_srv_wait_for_cfm(
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_REMOTE_DISPLAY_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: rmt disp, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_get_monitoring_image_ind_cb

DESCRIPTION
  Callback function invoked to indicate a get monitoring image request from a
  remote BIP client.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bip_srv_get_monitoring_image_ind_cb(
  OI_BIPSRV_CONNECTION_HANDLE         conn_id,
  OI_BIP_GET_MONITORING_IMAGE_PARAMS  *params_ptr,
  OI_BIP_REQUEST_STATE                state,
  OI_STATUS                           status)
{                        
  bt_ev_msg_type    event;
  bt_app_id_type    app_id;
  OI_STATUS         ret_status;

  (void)bt_pf_bip_srv_app_conn_find( 
    (bt_pf_bip_srv_conn_id_type)conn_id,
    &app_id );

  if (app_id != BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP SRV CB RX: mnt img, conn: %x, app: %x", 
                  conn_id, app_id, 0 );

    event.ev_hdr.bt_app_id = app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_SRV_GET_MONITORING_IMAGE_IND;
    event.ev_msg.ev_bip_srv_get_monitor_image_ind.conn_id = 
      (bt_pf_bip_srv_conn_id_type)conn_id;
    event.ev_msg.ev_bip_srv_get_monitor_image_ind.store_image = 
      params_ptr->request.storeImage;
    event.ev_msg.ev_bip_srv_get_monitor_image_ind.state = 
      (bt_pf_bip_srv_req_state_type)state;
    event.ev_msg.ev_bip_srv_get_monitor_image_ind.status = 
      (bt_cmd_status_type)status;

    bt_pf_bip_srv_req_cfm.conn_id = conn_id;

    /* clear sig first in case an earlier cfm was received after tmo expired */ 
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG ); 
    bt_ec_send_event( &event );
    
    ret_status = bt_pf_bip_srv_wait_for_cfm(
      (bt_pf_bip_srv_conn_id_type)conn_id,
      BT_EV_PF_BIP_SRV_GET_MONITORING_IMAGE_IND );

    if ( ret_status == OI_TIMEOUT )
    {
      ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP SRV CB RX: mnt img, unknown conn: %x", conn_id, 0, 0 );
    ret_status = OI_OBEX_INTERNAL_SERVER_ERROR;
  }
  return(ret_status);
}

#ifdef FEATURE_BT_TEST_API                           
#error code not present
#endif /* FEATURE_BT_TEST_API */

 
LOCAL OI_BIPSRV_IMAGING_RESPONDER_CALLBACKS bt_pf_bip_srv_image_resp_cb_list =
{
  bt_pf_bip_srv_imaging_responder_connect_ind_cb,
  bt_pf_bip_srv_disconnect_ind_cb,
  bt_pf_bip_srv_auth_ind_cb,
  bt_pf_bip_srv_get_capabilities_ind_cb,
  bt_pf_bip_srv_put_image_ind_cb,
  bt_pf_bip_srv_put_linked_attachment_ind_cb,
  bt_pf_bip_srv_put_linked_thumbnail_ind_cb,
  bt_pf_bip_srv_remote_display_ind_cb,
  bt_pf_bip_srv_get_images_list_ind_cb,
  bt_pf_bip_srv_get_image_properties_ind_cb,
  bt_pf_bip_srv_get_image_ind_cb,
  bt_pf_bip_srv_get_linked_thumbnail_ind_cb,
  bt_pf_bip_srv_get_linked_attachment_ind_cb,
  bt_pf_bip_srv_delete_image_ind_cb,
#ifdef FEATURE_BT_TEST_API
#error code not present
#else
  NULL, /* we are not a printer */
#endif
  bt_pf_bip_srv_start_archive_ind_cb,
  bt_pf_bip_srv_get_status_ind_cb,
  bt_pf_bip_srv_get_monitoring_image_ind_cb
};

LOCAL OI_BIPSRV_REFERENCED_OBJECTS_CALLBACKS bt_pf_bip_srv_ref_obj_cb_list =
{
  bt_pf_bip_srv_referenced_object_connect_ind_cb,
  bt_pf_bip_srv_disconnect_ind_cb,
  bt_pf_bip_srv_auth_ind_cb,
  bt_pf_bip_srv_get_capabilities_ind_cb,
  bt_pf_bip_srv_get_partial_image_ind_cb
};

LOCAL OI_BIPSRV_ARCHIVED_OBJECTS_CALLBACKS bt_pf_bip_srv_arch_obj_cb_list =
{
  bt_pf_bip_srv_archived_object_connect_ind_cb,
  bt_pf_bip_srv_disconnect_ind_cb,
  bt_pf_bip_srv_auth_ind_cb,
  bt_pf_bip_srv_get_capabilities_ind_cb,
  bt_pf_bip_srv_get_images_list_ind_cb,
  bt_pf_bip_srv_get_image_properties_ind_cb,
  bt_pf_bip_srv_get_image_ind_cb,
  bt_pf_bip_srv_get_linked_thumbnail_ind_cb,
  bt_pf_bip_srv_get_linked_attachment_ind_cb,
  bt_pf_bip_srv_delete_image_ind_cb
};


/*===========================================================================

FUNCTION
  bt_pf_bip_srv_deregister

DESCRIPTION
  Deregisters a BIP server specified by the server channel number and removes
  the associated service record from the SDP database.

===========================================================================*/
LOCAL bt_cmd_status_type bt_pf_bip_srv_deregister(
  uint8*               scn_ptr )
{
  OI_STATUS status; 

  status = OI_BIPSRV_DeregisterServer( *scn_ptr );
  if (status == OI_OK)
  {
    *scn_ptr = BT_SPP_SCN_UNSPECIFIED;
  }
  else
  {
    BT_ERR( "BT PF BIP SRV DEREG: returns %x, scn: %x", 
            status, *scn_ptr, 0 );
  }
  return(status);
} 

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_auth_rsp

DESCRIPTION
  Responds to an authentication request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_auth_rsp(
  bt_app_id_type                   app_id,
  bt_pf_cmd_bip_srv_auth_rsp_type* params_ptr )
{
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV AUTH RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  OI_BIPSRV_AuthenticationResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    (OI_CHAR*)params_ptr->pin_str );
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_get_capabilities_rsp

DESCRIPTION
  Responds to a get capabilities request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_get_capabilities_rsp(
  bt_app_id_type                               app_id,
  bt_pf_cmd_bip_srv_get_capabilities_rsp_type* params_ptr )
{
  OI_STATUS status; 
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP GET CAP RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  bt_pf_bip_srv_get_capabilities_params.response.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_bip_srv_get_capabilities_params.response.final = 
    params_ptr->final;

  status = OI_BIPSRV_GetCapabilitiesResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_get_capabilities_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_put_image_rsp

DESCRIPTION
  Responds to a put image request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_put_image_rsp(
  bt_app_id_type                        app_id,
  bt_pf_cmd_bip_srv_put_image_rsp_type* params_ptr )
{
  OI_STATUS         status;
  bt_app_id_type    correct_app_id;
  OI_OBEX_UNICODE   handle;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV PUT IMG RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  bt_pf_bip_srv_copy_handle( &handle, 
                             (uint16*)params_ptr->image_handle_str, 
                             params_ptr->status );

  bt_pf_bip_srv_put_image_params.response.imageHandle = &handle;

  status = OI_BIPSRV_PutImageResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_put_image_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_put_linked_thumbnail_rsp

DESCRIPTION
  Responds to a put linked thumbnail request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_put_linked_thumbnail_rsp(
  bt_app_id_type                            app_id,
  bt_pf_cmd_bip_srv_put_thumbnail_rsp_type* params_ptr )
{
  OI_STATUS status; 
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV PUT TNL RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  /* no response parameters for put linked attachment */

  status = OI_BIPSRV_PutLinkedThumbnailResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_put_thumbnail_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_put_linked_attachment_rsp

DESCRIPTION
  Responds to a put linked attachment request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_put_linked_attachment_rsp(
  bt_app_id_type                             app_id,
  bt_pf_cmd_bip_srv_put_attachment_rsp_type* params_ptr )
{
  OI_STATUS status;
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV PUT ATT RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  /* no response parameters for put linked thumbnail */

  status = OI_BIPSRV_PutLinkedAttachmentResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_put_attachment_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_get_images_list_rsp

DESCRIPTION
  Responds to a get images list request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_get_images_list_rsp(
  bt_app_id_type                       app_id,
  bt_pf_cmd_bip_srv_get_list_rsp_type* params_ptr )
{
  OI_STATUS status;
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV GET LIST RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  bt_pf_bip_srv_get_list_params.response.nbReturnedHandles =
    params_ptr->num_returned_handles;
  bt_pf_bip_srv_get_list_params.response.imageHandlesDescriptor = 
    (OI_OBEX_BYTESEQ*)(params_ptr->image_handles_descriptor_ptr);
  bt_pf_bip_srv_get_list_params.response.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_bip_srv_get_list_params.response.final = 
    params_ptr->final;

  status = OI_BIPSRV_GetImagesListResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_get_list_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_get_image_properties_rsp

DESCRIPTION
  Responds to a get image properties request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_get_image_properties_rsp(
  bt_app_id_type                             app_id,
  bt_pf_cmd_bip_srv_get_properties_rsp_type* params_ptr )
{
  OI_STATUS status;
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV GET PROP RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  bt_pf_bip_srv_get_properties_params.response.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_bip_srv_get_properties_params.response.final = 
    params_ptr->final;

  status = OI_BIPSRV_GetImagePropertiesResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_get_properties_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_get_image_rsp

DESCRIPTION
  Responds to a get image request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_get_image_rsp(
  bt_app_id_type                        app_id,
  bt_pf_cmd_bip_srv_get_image_rsp_type* params_ptr )
{
  OI_STATUS status;
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV GET IMG RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  bt_pf_bip_srv_get_image_params.response.imageFileSize = 
    params_ptr->file_size;
  bt_pf_bip_srv_get_image_params.response.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_bip_srv_get_image_params.response.final = 
    params_ptr->final;

  status = OI_BIPSRV_GetImageResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_get_image_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_get_linked_thumbnail_rsp

DESCRIPTION
  Responds to a get linked thumbnail request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_get_linked_thumbnail_rsp(
  bt_app_id_type                            app_id,
  bt_pf_cmd_bip_srv_get_thumbnail_rsp_type* params_ptr )
{
  OI_STATUS status;
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV GET TNL RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  bt_pf_bip_srv_get_thumbnail_params.response.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_bip_srv_get_thumbnail_params.response.final = 
    params_ptr->final;

  status = OI_BIPSRV_GetLinkedThumbnailResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_get_thumbnail_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_get_linked_attachment_rsp

DESCRIPTION
  Responds to a get linked attachment request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_get_linked_attachment_rsp(
  bt_app_id_type                             app_id,
  bt_pf_cmd_bip_srv_get_attachment_rsp_type* params_ptr )
{
  OI_STATUS status;
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV GET ATT RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  bt_pf_bip_srv_get_attachment_params.response.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_bip_srv_get_attachment_params.response.final = 
    params_ptr->final;

  status = OI_BIPSRV_GetLinkedAttachmentResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_get_attachment_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_delete_image_rsp

DESCRIPTION
  Responds to a delete image request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_delete_image_rsp(
  bt_app_id_type                           app_id,
  bt_pf_cmd_bip_srv_delete_image_rsp_type* params_ptr )
{
  OI_STATUS status;
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV DEL IMG RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  /* no response parameters for delete image */

  status = OI_BIPSRV_DeleteImageResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_delete_image_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_get_status_rsp

DESCRIPTION
  Responds to a get status request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_get_status_rsp(
  bt_app_id_type                         app_id,
  bt_pf_cmd_bip_srv_get_status_rsp_type* params_ptr )
{
  OI_STATUS status;
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV GET STAT RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  status = OI_BIPSRV_GetStatusResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_get_partial_image_rsp

DESCRIPTION
  Responds to a get partial image request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_get_partial_image_rsp(
  bt_app_id_type                                app_id,
  bt_pf_cmd_bip_srv_get_partial_image_rsp_type* params_ptr )
{
  OI_STATUS status;
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV GET PART IMG RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  bt_pf_bip_srv_get_partial_image_params.response.totalFileSize = 
    params_ptr->total_file_size;  
  bt_pf_bip_srv_get_partial_image_params.response.endFlag = 
    params_ptr->end_flag;
  bt_pf_bip_srv_get_partial_image_params.response.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_bip_srv_get_partial_image_params.response.final = 
    params_ptr->final;

  status = OI_BIPSRV_GetPartialImageResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_get_partial_image_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_start_archive_rsp

DESCRIPTION
  Responds to a start archive request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_start_archive_rsp(
  bt_app_id_type                            app_id,
  bt_pf_cmd_bip_srv_start_archive_rsp_type* params_ptr )
{
  OI_STATUS status;
  bt_app_id_type    correct_app_id;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV STA ARCH RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  /* no response parameters for start archive */

  status = OI_BIPSRV_StartArchiveResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_start_archive_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_remote_display_rsp

DESCRIPTION
  Responds to a remote display request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_remote_display_rsp(
  bt_app_id_type                             app_id,
  bt_pf_cmd_bip_srv_remote_display_rsp_type* params_ptr )
{
  OI_STATUS         status;
  bt_app_id_type    correct_app_id;
  OI_OBEX_UNICODE   handle;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV RMT DISP RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  bt_pf_bip_srv_copy_handle( &handle, 
                             (uint16*)params_ptr->image_handle_str, 
                             params_ptr->status );

  bt_pf_bip_srv_remote_display_params.response.imageHandle = &handle;

  status = OI_BIPSRV_RemoteDisplayResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_remote_display_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_get_monitoring_image_rsp

DESCRIPTION
  Responds to a get monitoring image request from a remote BIP client.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_get_monitoring_image_rsp(
  bt_app_id_type                                app_id,
  bt_pf_cmd_bip_srv_get_monitor_image_rsp_type* params_ptr )
{
  OI_STATUS         status;
  bt_app_id_type    correct_app_id;
  OI_OBEX_UNICODE   handle;

  (void)bt_pf_bip_srv_app_conn_find( params_ptr->conn_id, &correct_app_id );

  if (correct_app_id != app_id)
  {
    BT_ERR( "BT PF BIP SRV MNT IMG RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  bt_pf_bip_srv_copy_handle( &handle, 
                             (uint16*)params_ptr->image_handle_str, 
                             params_ptr->status );

  bt_pf_bip_srv_monitor_image_params.response.imageHandle = &handle;
  bt_pf_bip_srv_monitor_image_params.response.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_bip_srv_monitor_image_params.response.final = 
    params_ptr->final; 

  status = OI_BIPSRV_GetMonitoringImageResponse(
    (OI_BIPSRV_CONNECTION_HANDLE)params_ptr->conn_id,
    &bt_pf_bip_srv_monitor_image_params,
    (OI_STATUS)params_ptr->status );

  return(status);
}

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */


/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_register_imaging_responder

DESCRIPTION
  Initialize the BIP imaging responder and register it in the service discovery 
  database.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_register_imaging_responder(
  bt_app_id_type          app_id,
  char*                   srv_name_ptr,        
  uint8                   supported_capabilities,
  uint16                  supported_features,
  uint32                  supported_functions,
  uint64                  imaging_capacity,
  boolean                 auth )
{
  OI_STATUS                        status = OI_OK;
  uint8                            scn = 0; /* implicitly select BT 
                                               transport in 
                                               OI_RFCOMM_RegisterServer() */
  OI_BIPSRV_IMAGING_RESPONDER_INFO image_resp_info;

  TASKLOCK();

  if (bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].app_id != 
      BT_PF_APP_ID_NULL)
  {
    status = OI_STATUS_ALREADY_REGISTERED;
  }
  else
  {
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].app_id = app_id;
  }

  TASKFREE();

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BIP IMG RESP REG: App %x, Stat: %x", 
            app_id, status, 0 );
    return(status);
  }

  image_resp_info.supportedFeatures = supported_features;
  /* other fields of image_resp_info are used for SDP registration   */
  /* which is now done outside of OI_BIPSRV_RegisterImagingResponder */

  status = OI_BIPSRV_RegisterImagingResponder(
    &bt_pf_bip_srv_image_resp_cb_list,
    &image_resp_info,
    NULL, /* SDP registration not done here */
    auth,
    &scn );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BIP IMG RESP REG: returns %x, App %x", 
            status, app_id, 0 );
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].app_id = 
      BT_PF_APP_ID_NULL;
    return( status);
  }

  /* Updates service record with additional profile specific attributes */
  status = bt_cmd_sd_update_obex_profile_service_record(
             app_id,
             BT_SD_SERVICE_CLASS_IMAGING_RESPONDER,
             scn,
             srv_name_ptr,
             NULL, /* service ID */
             NULL, /* supported formats list */
             supported_capabilities,
             supported_features,
             supported_functions,
             imaging_capacity );

  if (status != BT_CS_GN_PENDING)
  {
    BT_ERR( "BT PF BIP IMG RESP REG: update srv rec failed, App %x", 
            app_id, 0, 0 );
    OI_BIPSRV_DeregisterServer( scn );
    return( status );
  }

  bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].scn = scn;

  return( OI_OK );
}


/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_register_referenced_objects

DESCRIPTION
  Initialize the BIP referenced objects server and register it in the service 
  discovery database.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_register_referenced_objects(
  bt_app_id_type      app_id,
  char*               srv_name_ptr,
  bt_sd_uuid128_type* service_id_ptr,
  uint32              supported_functions,
  boolean             auth )
{ 
  OI_STATUS                        status = OI_OK;
  uint8                            scn = 0; /* implicitly select BT 
                                               transport in 
                                               OI_RFCOMM_RegisterServer() */

  TASKLOCK();

  if (bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].app_id != 
      BT_PF_APP_ID_NULL)
  {
    status = OI_STATUS_ALREADY_REGISTERED;
  }
  else
  {
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].app_id = app_id;
  }

  TASKFREE();

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BIP REF OBJ SRV REG: App %x, Stat: %x", 
            app_id, status, 0 );
    return(status);
  }

  status = OI_BIPSRV_RegisterReferencedObjects(
    &bt_pf_bip_srv_ref_obj_cb_list,
    NULL,
    NULL, /* SDP registration not done here */
    auth,
    &scn );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BIP REF OBJ SRV REG: returns %x, App %x", 
            status, app_id, 0 );
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].app_id = 
      BT_PF_APP_ID_NULL;
    return(status);
  }

  /* Updates service record with additional profile specific attributes */
  status = bt_cmd_sd_update_obex_profile_service_record(
             app_id,
             BT_SD_SERVICE_CLASS_IMAGING_REFERENCED_OBJECTS,
             scn,
             srv_name_ptr,
             service_id_ptr, 
             NULL, /* supported formats list */
             0,    /* supported capabilities */
             0,    /* supported features */
             supported_functions,
             0 );  /* imaging capacity */

  if (status != BT_CS_GN_PENDING)
  {
    BT_ERR( "BT PF BIP IMG REF OBJ REG: update srv rec failed, App %x", 
            app_id, 0, 0 );
    OI_BIPSRV_DeregisterServer( scn );
    return( status );
  }
  
  bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].scn = scn;

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_register_archived_objects

DESCRIPTION
  Initialize the BIP archived objects server and register it in the service 
  discovery database.
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_register_archived_objects(
  bt_app_id_type      app_id,
  char*               srv_name_ptr,
  bt_sd_uuid128_type* service_id_ptr,
  uint32              supported_functions,
  boolean             auth )
{ 
  OI_STATUS                        status = OI_OK;
  uint8                            scn = 0; /* implicitly select BT 
                                               transport in 
                                               OI_RFCOMM_RegisterServer() */

  TASKLOCK();

  if (bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].app_id != 
      BT_PF_APP_ID_NULL)
  {
    status = OI_STATUS_ALREADY_REGISTERED;
  }
  else
  {
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].app_id = app_id;
  }

  TASKFREE();

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BIP ARCH OBJ SRV REG: App %x, Stat: %x", 
            app_id, status, 0 );
    return(status);
  }

  status = OI_BIPSRV_RegisterArchivedObjects(
    &bt_pf_bip_srv_arch_obj_cb_list,
    NULL,
    NULL, /* SDP registration not done here */
    auth,
    &scn );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BIP ARCH OBJ SRV REG: returns %x, App %x", 
            status, app_id, 0 );
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].app_id = 
      BT_PF_APP_ID_NULL;
    return(status);
  }     

  /* Updates service record with additional profile specific attributes */
  status = bt_cmd_sd_update_obex_profile_service_record(
             app_id,
             BT_SD_SERVICE_CLASS_IMAGING_AUTOMATIC_ARCHIVE,
             scn,
             srv_name_ptr,
             service_id_ptr, 
             NULL, /* supported formats list */
             0,    /* supported capabilities */
             0,    /* supported features */
             supported_functions,
             0 );  /* imaging capacity */

  if (status != BT_CS_GN_PENDING)
  {
    BT_ERR( "BT PF BIP IMG ARCH OBJ REG: update srv rec failed, App %x", 
            app_id, 0, 0 );
    OI_BIPSRV_DeregisterServer( scn );
    return( status );
  }        

  bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].scn = scn;

  return(OI_OK);
}
  
/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_deregister_imaging_responder

DESCRIPTION
  This function terminates the BIP imaging responder server and deregisters 
  it from the SDP database.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_deregister_imaging_responder(
  bt_app_id_type          app_id )
{
  OI_STATUS           status;

  if (bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].app_id == 
      BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP IMG RSP DEREG: not reg, App: %x, Stat: %x",
                  app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }

  if (bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].app_id != app_id)
  {
    BT_MSG_DEBUG( "BT PF BIP IMG RSP DEREG: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = bt_pf_bip_srv_deregister( 
             &(bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].scn) );

  if ( status != OI_OBEX_OPERATION_IN_PROGRESS )
  {
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].app_id = 
      BT_PF_APP_ID_NULL;
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].conn_id =
      BT_PF_BIP_NO_CONN_ID;
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_IMAGING_RESPONDER].scn = 
      BT_SPP_SCN_UNSPECIFIED;
  }

  return(status);
}
    
/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_deregister_referenced_objects

DESCRIPTION
  This function terminates the BIP rferenced objects server and deregisters 
  it from the SDP database.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_deregister_referenced_objects(
  bt_app_id_type          app_id )
{
  OI_STATUS           status;

  if (bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].app_id == 
      BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP REF OBJ SRV DEREG: not reg, App: %x, Stat: %x",
                  app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }

  if (bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].app_id != app_id)
  {
    BT_MSG_DEBUG( "BT PF BIP REF OBJ SRV DEREG: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = bt_pf_bip_srv_deregister( 
             &(bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].scn) );

  if ( status != OI_OBEX_OPERATION_IN_PROGRESS )
  {
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].app_id = 
      BT_PF_APP_ID_NULL;
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].conn_id =
      BT_PF_BIP_NO_CONN_ID;
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_REFERENCED_OBJ_SRV].scn = 
      BT_SPP_SCN_UNSPECIFIED;
  }

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_srv_cmd_deregister_archived_objects

DESCRIPTION
  This function terminates the BIP rferenced objects server and deregisters 
  it from the SDP database.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_srv_cmd_deregister_archived_objects(
  bt_app_id_type          app_id )
{
  OI_STATUS           status;

  BT_MSG_API( "BT PF BIP SRV CMD: dereg arch obj, App: %x", app_id, 0, 0 );

  if (bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].app_id == 
      BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BIP ARCH OBJ SRV DEREG: not reg, App: %x, Stat: %x",
                  app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }

  if (bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].app_id != app_id)
  {
    BT_MSG_DEBUG( "BT PF BIP ARCH OBJ SRV DEREG: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = bt_pf_bip_srv_deregister( 
             &(bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].scn) );

  if ( status != OI_OBEX_OPERATION_IN_PROGRESS )
  {
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].app_id = 
      BT_PF_APP_ID_NULL;
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].conn_id =
      BT_PF_BIP_NO_CONN_ID;
    bt_pf_bip_srv_info[BT_PF_BIP_TARGET_ARCHIVED_OBJ_SRV].scn = 
      BT_SPP_SCN_UNSPECIFIED;
  }

  return(status);
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_srv_req_cfm

DESCRIPTION
  This command is called by the application to confirm that a request event 
  is received. The application should supply an OBEX status code which will
  eventually be reported to the remote BIP client.
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_srv_req_cfm(
  bt_app_id_type             app_id,
  bt_pf_bip_srv_conn_id_type conn_id,
  bt_event_type              req_event,
  bt_cmd_status_type         status )
{
  if (conn_id == bt_pf_bip_srv_req_cfm.conn_id)
  {
    BT_MSG_API( "BT PF BIP SRV REQ CFM: App: %x, Conn: %x, Stat: %x", 
                app_id, conn_id, status );
    bt_pf_bip_srv_req_cfm.event = req_event;
    bt_pf_bip_srv_req_cfm.status = status;
    rex_set_sigs( &bt_pf_tcb, BT_PF_BIP_SRV_SIG );
    return(OI_OK);
  }
  else
  {
    BT_ERR( "BT PF BIP SRV REQ CFM: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
}

#endif /* FEATURE_BT_EXTPF_BIP */
#endif /* FEATURE_BT */
