/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 B L U E T O O T H   B A S I C   I M A G I N G   P R O F I L E   C L I E N T

GENERAL DESCRIPTION
  This module contains the Bluetooth Basic Imaging Profile client data and 
  code.

Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfbip2.c#1 $ 

$Log: $
*
*    #16       18 Oct 2006            PR
* Replaced OI_BIPCLI_Abort() with OI_BIPCLI_Cancel()
* while merging OI Code v3.0.14
*
*    #15       18 Apr 2006            GS
* Updated bt_pf_bip_cli_cmd_remote_display() to send empty
* image-handle string when display command is not Select image.
*
*    #14       12 Jul 2005            JH
* Added BT_PF_TMO_SIG to signal mask when calling rex_timed_wait() which was 
*  previously misused.
*
*    #13       19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #12       06 Jan 2005            JH
* Modified to avoid possible misaligned memory accesses.
*
*    #11       11 Nov 2004            JH
* Use the signal itself to tell if a timeout has occurred in 
*  bt_pf_bip_cli_wait_for_cfm, eliminating the need of a global variable.
*
*    #10       29 Oct 2004            JH
* Fixed typo in bt_pf_bip_cli_remote_display_cfm_cb()
*
*    #9        19 Oct 2004            JH
* Modified bt_pf_bip_cli_get_images_list_cfm_cb to process the added the number 
*  of returned handles response parameter
*
*    #8        26 Jul 2004            JH
* Revamped BIP signal and timer scheme.
*
*    #7        24 May 2004            JH
* Modified cfm callbacks to properly handle NULL pointers in response struct.
*
*    #6        14 May 2004            JH
* Further command and event struct changes
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
#include "oi_bip_spec.h"
#include "oi_bip_cli.c"         

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/ 

/* Currently, only one BIP client is supported */
bt_app_id_type              bt_pf_bip_cli_app_id = BT_PF_APP_ID_NULL;
bt_pf_bip_cli_conn_id_type  bt_pf_bip_cli_conn_id = BT_PF_BIP_NO_CONN_ID;

/* type of event pending acknowledgement */
LOCAL bt_event_type  bt_pf_bip_cli_get_rsp_cfm_event;

/* data structures for passing parameters into OI BIP calls */
LOCAL OI_BIP_GET_CAPABILITIES_PARAMS      bt_pf_bip_cli_get_capabilities_params;
LOCAL OI_BIP_PUT_IMAGE_PARAMS             bt_pf_bip_cli_put_image_params;
LOCAL OI_BIP_PUT_LINKED_ATTACHMENT_PARAMS bt_pf_bip_cli_put_attachment_params;
LOCAL OI_BIP_PUT_LINKED_THUMBNAIL_PARAMS  bt_pf_bip_cli_put_thumbnail_params;
LOCAL OI_BIP_GET_IMAGES_LIST_PARAMS       bt_pf_bip_cli_get_list_params;
LOCAL OI_BIP_GET_IMAGE_PROPERTIES_PARAMS  bt_pf_bip_cli_get_properties_params;
LOCAL OI_BIP_GET_IMAGE_PARAMS             bt_pf_bip_cli_get_image_params;
LOCAL OI_BIP_GET_LINKED_THUMBNAIL_PARAMS  bt_pf_bip_cli_get_thumbnail_params;
LOCAL OI_BIP_GET_LINKED_ATTACHMENT_PARAMS bt_pf_bip_cli_get_attachment_params;
LOCAL OI_BIP_DELETE_IMAGE_PARAMS          bt_pf_bip_cli_delete_image_params;
LOCAL OI_BIP_START_PRINT_PARAMS           bt_pf_bip_cli_start_print_params;
LOCAL OI_BIP_START_ARCHIVE_PARAMS         bt_pf_bip_cli_start_archive_params;
LOCAL OI_BIP_GET_PARTIAL_IMAGE_PARAMS     bt_pf_bip_cli_get_partial_image_params;
LOCAL OI_BIP_REMOTE_DISPLAY_PARAMS        bt_pf_bip_cli_remote_display_params;
LOCAL OI_BIP_GET_MONITORING_IMAGE_PARAMS  bt_pf_bip_cli_monitor_image_params;

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_wait_for_cfm

DESCRIPTION
  Wait for application to finish processing data supplied in a respond
  to a get command.

===========================================================================*/
LOCAL void bt_pf_bip_cli_wait_for_cfm(bt_event_type event)
{
  rex_timed_wait( BT_PF_BIP_CLI_SIG | BT_PF_TMO_SIG, 
                  &bt_pf_bip_cli_tmo_timer, 
                  BT_PF_BIP_OPS_TMO );

  if ( rex_get_sigs( &bt_pf_tcb ) & BT_PF_BIP_CLI_SIG )
  { 
    rex_clr_timer( &bt_pf_bip_cli_tmo_timer );
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_CLI_SIG );
    if ( bt_pf_bip_cli_get_rsp_cfm_event != event )
    {
      BT_ERR( "BT PF BIP CLI CB CFM: ev mismatch, ev: %x",
              event, 0, 0 );      
    }
  }
  else if ( rex_get_sigs( &bt_pf_tcb ) & BT_PF_TMO_SIG )
  { 
    /* signal not set => timeout has occurred */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_TMO_SIG );
    BT_ERR( "BT PF BIP CLI CB CFM: timeout", 0, 0, 0 );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_connect_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the connect command is received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_connect_cfm_cb( 
  OI_BIPCLI_HANDLE conn_id, 
  OI_STATUS status )
{
  bt_ev_msg_type event;

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: con, conn: %x", conn_id, 0, 0 );
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_CON_CFM;
    event.ev_msg.ev_bip_cli_con_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_con_cfm.status = 
      (bt_cmd_status_type)status;
    bt_ec_send_event( &event );
    if (status != OI_OK)
    {
      bt_pf_bip_cli_app_id = BT_PF_APP_ID_NULL;
      bt_pf_bip_cli_conn_id = BT_PF_BIP_NO_CONN_ID;
    }
  }
  else
  {
    BT_ERR( "BT PF BIP CB RX: con, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}
  
/*===========================================================================

FUNCTION
  bt_pf_bip_cli_disconnect_ind_cb

DESCRIPTION
  Callback function invoked to indicate a disconnection from remote BIP 
  server.

===========================================================================*/
LOCAL void bt_pf_bip_cli_disconnect_ind_cb( OI_BIPCLI_HANDLE conn_id )
{
  bt_ev_msg_type event;                   

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: dcn, conn: %x", conn_id, 0, 0 ); 

    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_DCN_CFM;
    event.ev_msg.ev_bip_cli_dcn_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;

    bt_pf_bip_cli_app_id = BT_PF_APP_ID_NULL;
    bt_pf_bip_cli_conn_id = BT_PF_BIP_NO_CONN_ID;

    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: dcn, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_auth_req_cb

DESCRIPTION
  Callback function invoked to request for authentication prior to
  connection setup.

===========================================================================*/
LOCAL void bt_pf_bip_cli_auth_req_cb( OI_BIPCLI_HANDLE conn_id )
{
  bt_ev_msg_type event;                   

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: auth, conn: %x", conn_id, 0, 0 );
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_AUTH_REQ;
    event.ev_msg.ev_bip_cli_auth_req.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: auth, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_get_capabilities_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the get capabilities 
  command is received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_get_capabilities_cfm_cb( 
  OI_BIPCLI_HANDLE                   conn_id,
  OI_BIP_GET_CAPABILITIES_PARAMS     *params_ptr,
  OI_STATUS                          result )
{
  bt_ev_msg_type event;                   

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: get cap, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_GET_CAPABILITIES_CFM;
    event.ev_msg.ev_bip_cli_get_capabilities_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_get_capabilities_cfm.final = 
      params_ptr->response.final;
    event.ev_msg.ev_bip_cli_get_capabilities_cfm.result = 
      (bt_cmd_status_type)result;
    
    if (params_ptr->response.data == NULL)
    {
      event.ev_msg.ev_bip_cli_get_capabilities_cfm.data_ptr = NULL;
      event.ev_msg.ev_bip_cli_get_capabilities_cfm.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_cli_get_capabilities_cfm.data_ptr = 
        params_ptr->response.data->data;
      event.ev_msg.ev_bip_cli_get_capabilities_cfm.data_len = 
        params_ptr->response.data->len;
    }

    /* clear sig first in case an earlier cfm was received after tmo expired */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_CLI_SIG );
    bt_ec_send_event( &event );

    bt_pf_bip_cli_wait_for_cfm( BT_EV_PF_BIP_CLI_GET_CAPABILITIES_CFM );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: get cap, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_put_image_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the put image command is 
  received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_put_image_cfm_cb(
  OI_BIPCLI_HANDLE                   conn_id,
  OI_BIP_PUT_IMAGE_PARAMS            *params_ptr,
  OI_STATUS                          result )
{
  bt_ev_msg_type event;
  uint16         len;

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  { 
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: put img, conn: %x", conn_id, 0, 0 );

    memset( (void*)&(event.ev_msg.ev_bip_cli_put_image_cfm.image_handle),
            0, (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
    if (params_ptr->response.imageHandle != NULL)
    {                                           
      if (params_ptr->response.imageHandle->len > BT_PF_BIP_IMAGE_HANDLE_LEN + 1)
      {
        /* image handle is always 7 characters, null terminated */
        len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
      }
      else
      {
        len = params_ptr->response.imageHandle->len;
      }
      memcpy( (void*)&(event.ev_msg.ev_bip_cli_put_image_cfm.image_handle), 
              (void*)params_ptr->response.imageHandle->str,
            len * sizeof( uint16 ) );
    }

    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_PUT_IMAGE_CFM;
    event.ev_msg.ev_bip_cli_put_image_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_put_image_cfm.result = 
      (bt_cmd_status_type)result;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: put img, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}
  
/*===========================================================================

FUNCTION
  bt_pf_bip_cli_put_linked_thumbnail_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the put linked thumbnail 
  command is received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_put_linked_thumbnail_cfm_cb(
  OI_BIPCLI_HANDLE                   conn_id,
  OI_BIP_PUT_LINKED_THUMBNAIL_PARAMS *params_ptr,    
  OI_STATUS                          result )
{
  bt_ev_msg_type event;               

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  { 
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: put tnl, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_PUT_LINKED_THUMBNAIL_CFM;
    event.ev_msg.ev_bip_cli_put_thumbnail_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_put_thumbnail_cfm.result = 
      (bt_cmd_status_type)result;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: put tnl, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_put_linked_attachment_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the put linked attachment 
  command is received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_put_linked_attachment_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,
  OI_BIP_PUT_LINKED_ATTACHMENT_PARAMS *params_ptr,   
  OI_STATUS                           result )
{
  bt_ev_msg_type event;               

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  { 
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: put att, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_PUT_LINKED_ATTACHMENT_CFM;
    event.ev_msg.ev_bip_cli_put_attachment_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_put_attachment_cfm.result = 
      (bt_cmd_status_type)result;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: put att, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_get_images_list_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the get images list 
  command is received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_get_images_list_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,
  OI_BIP_GET_IMAGES_LIST_PARAMS       *params_ptr,
  OI_STATUS                           result )
{
  bt_ev_msg_type event;               

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: get list, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_GET_IMAGES_LIST_CFM;
    event.ev_msg.ev_bip_cli_get_list_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_get_list_cfm.final = 
      params_ptr->response.final;
    event.ev_msg.ev_bip_cli_get_list_cfm.result = 
      (bt_cmd_status_type)result;
    
    event.ev_msg.ev_bip_cli_get_list_cfm.num_returned_handles =
      params_ptr->response.nbReturnedHandles;

    if (params_ptr->response.imageHandlesDescriptor == NULL)
    {
      event.ev_msg.ev_bip_cli_get_list_cfm.image_handles_descriptor_ptr = NULL;
      event.ev_msg.ev_bip_cli_get_list_cfm.image_handles_descriptor_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_cli_get_list_cfm.image_handles_descriptor_ptr = 
        params_ptr->response.imageHandlesDescriptor->data;
      event.ev_msg.ev_bip_cli_get_list_cfm.image_handles_descriptor_len = 
        params_ptr->response.imageHandlesDescriptor->len;
    }
    if (params_ptr->response.data == NULL)
    {
      event.ev_msg.ev_bip_cli_get_list_cfm.data_ptr = NULL;
      event.ev_msg.ev_bip_cli_get_list_cfm.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_cli_get_list_cfm.data_ptr = 
        params_ptr->response.data->data;
      event.ev_msg.ev_bip_cli_get_list_cfm.data_len = 
        params_ptr->response.data->len;
    }

    /* clear sig first in case an earlier cfm was received after tmo expired */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_CLI_SIG );
    bt_ec_send_event( &event );

    bt_pf_bip_cli_wait_for_cfm( BT_EV_PF_BIP_CLI_GET_IMAGES_LIST_CFM );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: get list, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_get_image_properties_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the get image properties 
  command is received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_get_image_properties_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,
  OI_BIP_GET_IMAGE_PROPERTIES_PARAMS  *params_ptr,
  OI_STATUS                           result )
{
  bt_ev_msg_type event;

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: get prop, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_GET_IMAGE_PROPERTIES_CFM;
    event.ev_msg.ev_bip_cli_get_properties_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_get_properties_cfm.final = 
      params_ptr->response.final;
    event.ev_msg.ev_bip_cli_get_properties_cfm.result = 
      (bt_cmd_status_type)result;

    if (params_ptr->response.data == NULL)
    {
      event.ev_msg.ev_bip_cli_get_properties_cfm.data_ptr = NULL;
      event.ev_msg.ev_bip_cli_get_properties_cfm.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_cli_get_properties_cfm.data_ptr = 
        params_ptr->response.data->data;
      event.ev_msg.ev_bip_cli_get_properties_cfm.data_len = 
        params_ptr->response.data->len;
    }

    /* clear sig first in case an earlier cfm was received after tmo expired */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_CLI_SIG );
    bt_ec_send_event( &event );
    
    bt_pf_bip_cli_wait_for_cfm( BT_EV_PF_BIP_CLI_GET_IMAGE_PROPERTIES_CFM );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: get prop, unknown conn: %x", 
            conn_id, 0, 0 );
  }

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_get_image_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the get image command is 
  received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_get_image_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,
  OI_BIP_GET_IMAGE_PARAMS             *params_ptr,
  OI_STATUS                           result )
{
  bt_ev_msg_type event;

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: get img, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_GET_IMAGE_CFM;
    event.ev_msg.ev_bip_cli_get_image_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_get_image_cfm.file_size = 
      params_ptr->response.imageFileSize;
    event.ev_msg.ev_bip_cli_get_image_cfm.final = 
      params_ptr->response.final;
    event.ev_msg.ev_bip_cli_get_image_cfm.result = 
      (bt_cmd_status_type)result;

    if (params_ptr->response.data == NULL)
    {
      event.ev_msg.ev_bip_cli_get_image_cfm.data_ptr = NULL;
      event.ev_msg.ev_bip_cli_get_image_cfm.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_cli_get_image_cfm.data_ptr = 
        params_ptr->response.data->data;
      event.ev_msg.ev_bip_cli_get_image_cfm.data_len = 
        params_ptr->response.data->len;
    }

    /* clear sig first in case an earlier cfm was received after tmo expired */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_CLI_SIG );
    bt_ec_send_event( &event );
    
    bt_pf_bip_cli_wait_for_cfm( BT_EV_PF_BIP_CLI_GET_IMAGE_CFM );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: get img, unknown conn: %x", 
            conn_id, 0, 0 );
  }

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_get_linked_thumbnail_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the get linked thumbnail 
  command is received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_get_linked_thumbnail_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,
  OI_BIP_GET_LINKED_THUMBNAIL_PARAMS  *params_ptr,
  OI_STATUS                           result )
{
  bt_ev_msg_type event;                   

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: get tnl, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_GET_LINKED_THUMBNAIL_CFM;
    event.ev_msg.ev_bip_cli_get_thumbnail_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_get_thumbnail_cfm.final = 
      params_ptr->response.final;
    event.ev_msg.ev_bip_cli_get_thumbnail_cfm.result = 
      (bt_cmd_status_type)result;

    if (params_ptr->response.data == NULL)
    {
      event.ev_msg.ev_bip_cli_get_thumbnail_cfm.data_ptr = NULL;
      event.ev_msg.ev_bip_cli_get_thumbnail_cfm.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_cli_get_thumbnail_cfm.data_ptr = 
        params_ptr->response.data->data;
      event.ev_msg.ev_bip_cli_get_thumbnail_cfm.data_len = 
        params_ptr->response.data->len;
    }

    /* clear sig first in case an earlier cfm was received after tmo expired */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_CLI_SIG );
    bt_ec_send_event( &event );
    
    bt_pf_bip_cli_wait_for_cfm( BT_EV_PF_BIP_CLI_GET_LINKED_THUMBNAIL_CFM );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: get tnl, unknown conn: %x", 
            conn_id, 0, 0 );
  }

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_get_linked_attachment_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the get linked attachment 
  command is received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_get_linked_attachment_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,
  OI_BIP_GET_LINKED_ATTACHMENT_PARAMS *params_ptr,
  OI_STATUS                           result )
{
  bt_ev_msg_type event;

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: get att, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_GET_LINKED_ATTACHMENT_CFM;
    event.ev_msg.ev_bip_cli_get_attachment_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_get_attachment_cfm.final = 
      params_ptr->response.final;
    event.ev_msg.ev_bip_cli_get_attachment_cfm.result = 
      (bt_cmd_status_type)result;

    if (params_ptr->response.data == NULL)
    {
      event.ev_msg.ev_bip_cli_get_attachment_cfm.data_ptr = NULL;
      event.ev_msg.ev_bip_cli_get_attachment_cfm.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_cli_get_attachment_cfm.data_ptr = 
        params_ptr->response.data->data;
      event.ev_msg.ev_bip_cli_get_attachment_cfm.data_len = 
        params_ptr->response.data->len;
    }

    /* clear sig first in case an earlier cfm was received after tmo expired */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_CLI_SIG );
    bt_ec_send_event( &event );
    
    bt_pf_bip_cli_wait_for_cfm( BT_EV_PF_BIP_CLI_GET_LINKED_ATTACHMENT_CFM );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: get att, unknown conn: %x", 
            conn_id, 0, 0 );
  }

  return;
}
/*===========================================================================

FUNCTION
  bt_pf_bip_cli_delete_image_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the delete image command is 
  received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_delete_image_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,   
  OI_STATUS                           result )
{
  bt_ev_msg_type event;               

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  { 
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: del img, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_DELETE_IMAGE_CFM;
    event.ev_msg.ev_bip_cli_delete_image_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_delete_image_cfm.result = 
      (bt_cmd_status_type)result;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: del img, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_get_status_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the get status command is 
  received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_get_status_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,   
  OI_STATUS                           result )
{
  bt_ev_msg_type event;

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  { 
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: get stat, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_GET_STATUS_CFM;
    event.ev_msg.ev_bip_cli_get_status_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_get_status_cfm.result = 
      (bt_cmd_status_type)result;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: get stat, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_get_partial_image_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the get partial image
  command is received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_get_partial_image_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,
  OI_BIP_GET_PARTIAL_IMAGE_PARAMS     *params_ptr,
  OI_STATUS                           result )
{
  bt_ev_msg_type event;

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: get part img, conn: %x", conn_id, 0, 0 ); 
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_GET_PARTIAL_IMAGE_CFM;
    event.ev_msg.ev_bip_cli_get_partial_image_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_get_partial_image_cfm.total_file_size = 
      params_ptr->response.totalFileSize;
    event.ev_msg.ev_bip_cli_get_partial_image_cfm.end_flag = 
      params_ptr->response.endFlag;
    event.ev_msg.ev_bip_cli_get_partial_image_cfm.final = 
      params_ptr->response.final;
    event.ev_msg.ev_bip_cli_get_partial_image_cfm.result = 
      (bt_cmd_status_type)result;

    if (params_ptr->response.data == NULL)
    {
      event.ev_msg.ev_bip_cli_get_partial_image_cfm.data_ptr = NULL;
      event.ev_msg.ev_bip_cli_get_partial_image_cfm.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_cli_get_partial_image_cfm.data_ptr = 
        params_ptr->response.data->data;
      event.ev_msg.ev_bip_cli_get_partial_image_cfm.data_len = 
        params_ptr->response.data->len;
    }

    /* clear sig first in case an earlier cfm was received after tmo expired */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_CLI_SIG );
    bt_ec_send_event( &event );
    
    bt_pf_bip_cli_wait_for_cfm( BT_EV_PF_BIP_CLI_GET_PARTIAL_IMAGE_CFM );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: get part img, unknown conn: %x", 
            conn_id, 0, 0 );
  }

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_start_print_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the start print command is 
  received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_start_print_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,
  OI_BIP_START_PRINT_PARAMS           *params_ptr,
  OI_STATUS                           result )
{
  bt_ev_msg_type event;               

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  { 
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: strt prn, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_START_PRINT_CFM;
    event.ev_msg.ev_bip_cli_start_print_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_start_print_cfm.result = 
      (bt_cmd_status_type)result;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: strt prn, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_start_archive_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the start archive command is 
  received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_start_archive_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,   
  OI_STATUS                           result )
{
  bt_ev_msg_type event;               

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  { 
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: strt arch, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_START_ARCHIVE_CFM;
    event.ev_msg.ev_bip_cli_start_archive_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_start_archive_cfm.result = 
      (bt_cmd_status_type)result;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: strt arch, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_remote_display_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the remote display command is 
  received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_remote_display_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,
  OI_BIP_REMOTE_DISPLAY_PARAMS        *params_ptr,
  OI_STATUS                           result )
{
  bt_ev_msg_type event;
  uint16         len;

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  { 
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: rmt disp, conn: %x", conn_id, 0, 0 );

    memset( (void*)&(event.ev_msg.ev_bip_cli_remote_display_cfm.image_handle),
            0, (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
    if (params_ptr->response.imageHandle != NULL)
    {                                           
      if (params_ptr->response.imageHandle->len > BT_PF_BIP_IMAGE_HANDLE_LEN + 1)
      {
        /* image handle is always 7 characters, null terminated */
        len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
      }
      else
      {
        len = params_ptr->response.imageHandle->len;
      }                                          
      memcpy( (void*)&(event.ev_msg.ev_bip_cli_remote_display_cfm.image_handle), 
              (void*)params_ptr->response.imageHandle->str,
              len * sizeof( uint16 ) );
    }

    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_REMOTE_DISPLAY_CFM;
    event.ev_msg.ev_bip_cli_remote_display_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_remote_display_cfm.result = 
      (bt_cmd_status_type)result;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: rmt disp, unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_get_monitoring_image_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the get monitoring image
  command is received.

===========================================================================*/
LOCAL void bt_pf_bip_cli_get_monitoring_image_cfm_cb(
  OI_BIPCLI_HANDLE                    conn_id,
  OI_BIP_GET_MONITORING_IMAGE_PARAMS  *params_ptr,
  OI_STATUS                           result )
{
  bt_ev_msg_type event; 
  uint16         len;

  if (bt_pf_bip_cli_conn_id == (bt_pf_bip_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CB RX: mnt img, conn: %x", conn_id, 0, 0 );
    
    event.ev_hdr.bt_app_id = bt_pf_bip_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_BIP_CLI_GET_MONITORING_IMAGE_CFM;
    event.ev_msg.ev_bip_cli_get_monitor_image_cfm.conn_id = 
      (bt_pf_bip_cli_conn_id_type)conn_id;
    event.ev_msg.ev_bip_cli_get_monitor_image_cfm.final = 
      params_ptr->response.final;
    event.ev_msg.ev_bip_cli_get_monitor_image_cfm.result = 
      (bt_cmd_status_type)result;

    memset( (void*)&(event.ev_msg.ev_bip_cli_get_monitor_image_cfm.image_handle),
            0, (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
    if (params_ptr->response.imageHandle != NULL)
    {                                           
      if (params_ptr->response.imageHandle->len > BT_PF_BIP_IMAGE_HANDLE_LEN + 1)
      {
        /* image handle is always 7 characters, null terminated */
        len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
      }
      else
      {
        len = params_ptr->response.imageHandle->len;
      }
      memcpy( (void*)&(event.ev_msg.ev_bip_cli_get_monitor_image_cfm.image_handle),
              (void*)params_ptr->response.imageHandle->str,
              len * sizeof( uint16 ) );
    }

    if (params_ptr->response.data == NULL)
    {
      event.ev_msg.ev_bip_cli_get_monitor_image_cfm.data_ptr = NULL;
      event.ev_msg.ev_bip_cli_get_monitor_image_cfm.data_len = 0;
    }
    else
    {
      event.ev_msg.ev_bip_cli_get_monitor_image_cfm.data_ptr = 
        params_ptr->response.data->data;
      event.ev_msg.ev_bip_cli_get_monitor_image_cfm.data_len = 
        params_ptr->response.data->len;                  
    }

    /* clear sig first in case an earlier cfm was received after tmo expired */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BIP_CLI_SIG );
    bt_ec_send_event( &event );
    
    bt_pf_bip_cli_wait_for_cfm( BT_EV_PF_BIP_CLI_GET_MONITORING_IMAGE_CFM );
  }
  else
  {
    BT_ERR( "BT PF BIP CLI CB RX: mnt img, unknown conn: %x", 
            conn_id, 0, 0 );
  }

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_connect

DESCRIPTION
  Performs connection to remote BIP server.

===========================================================================*/  
bt_cmd_status_type bt_pf_bip_cli_cmd_connect(
  bt_app_id_type                  app_id,
  bt_pf_cmd_bip_cli_connect_type* params_ptr )
{
  OI_STATUS                  status;
  bt_pf_bip_cli_conn_id_type conn_id;
  OI_OBEX_BYTESEQ            target;
  bt_bd_addr_type            bd_addr = params_ptr->bd_addr;

  if ( bt_pf_bip_cli_conn_id != BT_PF_BIP_NO_CONN_ID )
  {
    BT_MSG_DEBUG( "BT PF BIP CLI CONN: alrdy conn, App: %x",
                  app_id, 0, 0 );
    return(OI_STATUS_ALREADY_CONNECTED);
  } 

  bt_pf_bip_cli_app_id = app_id;
  target.len = BT_PF_UUID128_SIZE;
  target.data = (OI_BYTE*)(params_ptr->target);

  status = OI_BIPCLI_Connect(
    (OI_BD_ADDR*)&bd_addr,
    params_ptr->channel,
    &target,
    bt_pf_bip_cli_auth_req_cb,
    bt_pf_bip_cli_connect_cfm_cb,
    bt_pf_bip_cli_disconnect_ind_cb,
    (OI_BIPCLI_HANDLE*)&conn_id );

  if (status != OI_OK)
  {
    bt_pf_bip_cli_app_id = BT_PF_APP_ID_NULL;
  }
  else
  {
    bt_pf_bip_cli_conn_id = conn_id;
  }
  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_disonnect

DESCRIPTION
  Disconnect from a remote BIP server.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_disconnect(
  bt_app_id_type                     app_id,
  bt_pf_cmd_bip_cli_disconnect_type* params_ptr )
{
  OI_STATUS status;
 
  status = OI_BIPCLI_Disconnect( (OI_BIPCLI_HANDLE)params_ptr->conn_id );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_auth_rsp

DESCRIPTION
  Respond to an authentication request from a remote BIP server.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_auth_rsp(
  bt_app_id_type                   app_id,
  bt_pf_cmd_bip_cli_auth_rsp_type* params_ptr )
{
  OI_BIPCLI_AuthenticationResponse(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    (OI_CHAR*)params_ptr->pin_str );
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_get_capabilities

DESCRIPTION
  Performs a BIP GET_CAPABILITIES operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_get_capabilities(
  bt_app_id_type                           app_id,
  bt_pf_cmd_bip_cli_get_capabilities_type* params_ptr )
{
  OI_STATUS status;

  /* no request parameters in bt_pf_bip_cli_get_capabilities_params */
  
  status = OI_BIPCLI_GetCapabilities(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_get_capabilities_cfm_cb,
    &bt_pf_bip_cli_get_capabilities_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_put_image

DESCRIPTION
  Performs a BIP PUT_IMAGE operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_put_image(
  bt_app_id_type                    app_id,
  bt_pf_cmd_bip_cli_put_image_type* params_ptr )
{
  OI_STATUS        status;
  OI_OBEX_UNICODE  name;
  OI_OBEX_UNICODE* name_ptr;
  uint16           name_str[BT_PF_MAX_FILENAME_LEN + 1];

  if (params_ptr->name_len == 0)
  {
    name_ptr = NULL;
  }
  else
  { 
    /* name is copied to avoid misaligned access when str pointer is not 
       aligned */
    memcpy( (void*)name_str,
            (void*)params_ptr->name_str,
            params_ptr->name_len * sizeof( uint16 ) );
    name.len = params_ptr->name_len;
    name.str = name_str;
    name_ptr = &name;
  }
  
  bt_pf_bip_cli_put_image_params.request.imageName = name_ptr;
  bt_pf_bip_cli_put_image_params.request.imageDescriptor = 
    (OI_OBEX_BYTESEQ*)(params_ptr->descriptior_ptr);
  bt_pf_bip_cli_put_image_params.request.data = 
    (OI_OBEX_BYTESEQ*)params_ptr->data_ptr;
  bt_pf_bip_cli_put_image_params.request.final = 
    params_ptr->final;

  status = OI_BIPCLI_PutImage(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_put_image_cfm_cb,
    &bt_pf_bip_cli_put_image_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_put_linked_thumbnail

DESCRIPTION
  Performs a BIP PUT_LINKED_THUMBNAIL operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_put_linked_thumbnail(
  bt_app_id_type                        app_id,
  bt_pf_cmd_bip_cli_put_thumbnail_type* params_ptr )
{
  OI_STATUS       status;
  OI_OBEX_UNICODE handle;
  uint16          handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];

  /* handle is copied to avoid misaligned access when str pointer is not 
     aligned */
  memcpy( (void*)handle_str,
          (void*)params_ptr->image_handle_str,
          (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
  handle.len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
  handle.str = handle_str;
  
  bt_pf_bip_cli_put_thumbnail_params.request.imageHandle = &handle;
  bt_pf_bip_cli_put_thumbnail_params.request.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_bip_cli_put_thumbnail_params.request.final = 
    params_ptr->final;

  status = OI_BIPCLI_PutLinkedThumbnail(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_put_linked_thumbnail_cfm_cb,
    &bt_pf_bip_cli_put_thumbnail_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_put_linked_attachment

DESCRIPTION
  Performs a BIP PUT_LINKED_ATTACHMENT operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_put_linked_attachment(
  bt_app_id_type                         app_id,
  bt_pf_cmd_bip_cli_put_attachment_type* params_ptr )
{
  OI_STATUS       status;
  OI_OBEX_UNICODE handle;
  uint16          handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];

  /* handle is copied to avoid misaligned access when str pointer is not 
     aligned */
  memcpy( (void*)handle_str,
          (void*)params_ptr->image_handle_str,
          (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
  handle.len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
  handle.str = handle_str;
  
  bt_pf_bip_cli_put_attachment_params.request.imageHandle = &handle;
  bt_pf_bip_cli_put_attachment_params.request.attachmentDescriptor = 
    (OI_OBEX_BYTESEQ*)(params_ptr->descriptior_ptr);   
  bt_pf_bip_cli_put_attachment_params.request.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_bip_cli_put_attachment_params.request.final = 
    params_ptr->final;

  status = OI_BIPCLI_PutLinkedAttachment(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_put_linked_attachment_cfm_cb,
    &bt_pf_bip_cli_put_attachment_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_get_images_list

DESCRIPTION
  Performs a BIP GET_IMAGES_LIST operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_get_images_list(
  bt_app_id_type                   app_id,
  bt_pf_cmd_bip_cli_get_list_type* params_ptr )
{
  OI_STATUS status;

  bt_pf_bip_cli_get_list_params.request.handleCount = 
    params_ptr->handle_count; 
  bt_pf_bip_cli_get_list_params.request.handleOffset = 
    params_ptr->handle_offset;   
  bt_pf_bip_cli_get_list_params.request.latest = 
    params_ptr->latest;
  bt_pf_bip_cli_get_list_params.request.handlesDescriptor = 
    (OI_OBEX_BYTESEQ*)(params_ptr->image_handles_descriptor_ptr);

  status = OI_BIPCLI_GetImagesList(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_get_images_list_cfm_cb,
    &bt_pf_bip_cli_get_list_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_get_image_properties

DESCRIPTION
  Performs a BIP GET_IMAGE_PROPERTIES operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_get_image_properties(
  bt_app_id_type                         app_id,
  bt_pf_cmd_bip_cli_get_properties_type* params_ptr )
{
  OI_STATUS       status;
  OI_OBEX_UNICODE handle;
  uint16          handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];

  /* handle is copied to avoid misaligned access when str pointer is not 
     aligned */
  memcpy( (void*)handle_str,
          (void*)params_ptr->image_handle_str,
          (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
  handle.len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
  handle.str = handle_str;
  
  bt_pf_bip_cli_get_properties_params.request.imageHandle = &handle;

  status = OI_BIPCLI_GetImageProperties(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_get_image_properties_cfm_cb,
    &bt_pf_bip_cli_get_properties_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_get_image

DESCRIPTION
  Performs a BIP GET_IMAGE operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_get_image(
  bt_app_id_type                     app_id,
   bt_pf_cmd_bip_cli_get_image_type* params_ptr )
{
  OI_STATUS       status;
  OI_OBEX_UNICODE handle;
  uint16          handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];

  /* handle is copied to avoid misaligned access when str pointer is not 
     aligned */
  memcpy( (void*)handle_str,
          (void*)params_ptr->image_handle_str,
          (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
  handle.len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
  handle.str = handle_str;
  
  bt_pf_bip_cli_get_image_params.request.imageHandle = &handle;
  bt_pf_bip_cli_get_image_params.request.imageDescriptor = 
    (OI_OBEX_BYTESEQ*)(params_ptr->descriptior_ptr);

  status = OI_BIPCLI_GetImage(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_get_image_cfm_cb,
    &bt_pf_bip_cli_get_image_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_get_linked_thumbnail

DESCRIPTION
  Performs a BIP GET_LINKED_THUMBNAIL operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_get_linked_thumbnail(
  bt_app_id_type                        app_id,
  bt_pf_cmd_bip_cli_get_thumbnail_type* params_ptr )
{
  OI_STATUS       status;
  OI_OBEX_UNICODE handle;
  uint16          handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];

  /* handle is copied to avoid misaligned access when str pointer is not 
     aligned */
  memcpy( (void*)handle_str,
          (void*)params_ptr->image_handle_str,
          (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
  handle.len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
  handle.str = handle_str;
  
  bt_pf_bip_cli_get_thumbnail_params.request.imageHandle = &handle;

  status = OI_BIPCLI_GetLinkedThumbnail(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_get_linked_thumbnail_cfm_cb,
    &bt_pf_bip_cli_get_thumbnail_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_get_linked_attachment

DESCRIPTION
  Performs a BIP GET_LINKED_ATTACHMENT operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_get_linked_attachment(
  bt_app_id_type                         app_id,
  bt_pf_cmd_bip_cli_get_attachment_type* params_ptr )
{
  OI_STATUS       status;
  OI_OBEX_UNICODE handle;
  OI_OBEX_UNICODE name;
  uint16          handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];
  uint16          name_str[BT_PF_MAX_FILENAME_LEN + 1];

  /* handle is copied to avoid misaligned access when str pointer is not 
     aligned */
  memcpy( (void*)handle_str,
          (void*)params_ptr->image_handle_str,
          (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
  handle.len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
  handle.str = handle_str;

  memcpy( (void*)name_str,
          (void*)params_ptr->name_str,
          params_ptr->name_len * sizeof( uint16 ) );
  name.len = params_ptr->name_len;
  name.str = name_str;
  
  bt_pf_bip_cli_get_attachment_params.request.imageHandle = &handle;
  bt_pf_bip_cli_get_attachment_params.request.attachmentName = &name;

  status = OI_BIPCLI_GetLinkedAttachment(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_get_linked_attachment_cfm_cb,
    &bt_pf_bip_cli_get_attachment_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_delete_image

DESCRIPTION
  Performs a BIP DELETE_IMAGE operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_delete_image(
  bt_app_id_type                       app_id,
  bt_pf_cmd_bip_cli_delete_image_type* params_ptr )
{
  OI_STATUS       status;
  OI_OBEX_UNICODE handle;
  uint16          handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];

  /* handle is copied to avoid misaligned access when str pointer is not 
     aligned */
  memcpy( (void*)handle_str,
          (void*)params_ptr->image_handle_str,
          (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );
  handle.len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
  handle.str = handle_str;
  
  bt_pf_bip_cli_delete_image_params.request.imageHandle = &handle;

  status = OI_BIPCLI_DeleteImage(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_delete_image_cfm_cb,
    &bt_pf_bip_cli_delete_image_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_get_status

DESCRIPTION
  Performs a BIP GET_STATUS operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_get_status(
  bt_app_id_type                     app_id,
  bt_pf_cmd_bip_cli_get_status_type* params_ptr )
{
  OI_STATUS status;

  status = OI_BIPCLI_GetStatus(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_get_status_cfm_cb );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_get_partial_image

DESCRIPTION
  Performs a BIP GET_PARTIAL_IMAGE operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_get_partial_image(
  bt_app_id_type                            app_id,
  bt_pf_cmd_bip_cli_get_partial_image_type* params_ptr )
{
  OI_STATUS       status;
  OI_OBEX_UNICODE name;
  uint16          name_str[BT_PF_MAX_FILENAME_LEN + 1];

  /* name is copied to avoid misaligned access when str pointer is not 
     aligned */
  memcpy( (void*)name_str,
          (void*)params_ptr->name_str,
          params_ptr->name_len * sizeof( uint16 ) );
  name.len = params_ptr->name_len;
  name.str = name_str;
  
  bt_pf_bip_cli_get_partial_image_params.request.imageName = &name;
  bt_pf_bip_cli_get_partial_image_params.request.partialLength = 
    params_ptr->partial_length;
  bt_pf_bip_cli_get_partial_image_params.request.partialOffset = 
    params_ptr->partial_offset;

  status = OI_BIPCLI_GetPartialImage(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_get_partial_image_cfm_cb,
    &bt_pf_bip_cli_get_partial_image_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_start_print

DESCRIPTION
  Performs a BIP START_PRINT operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_start_print(
  bt_app_id_type                      app_id,
  bt_pf_cmd_bip_cli_start_print_type* params_ptr )
{
  OI_STATUS status;
  uint16    i;

  /* OI_UUID128 is not packed but bt_sd_uuid128_type is, cannot cast. 
     Use memcpy to avoid misaligned access */
  memcpy( (void*)&(bt_pf_bip_cli_start_print_params.request.secondaryServiceId.
                   ms32bits),
          (void*)&(params_ptr->ref_obj_srv_service_id),
          sizeof( uint32 ) );
  for (i = 0; i < OI_BT_UUID128_SIZE - sizeof( OI_UINT32 ); i++)
  {
    bt_pf_bip_cli_start_print_params.request.secondaryServiceId.base[i] = 
      params_ptr->ref_obj_srv_service_id.uuid_byte[i + 4];;
  }

  bt_pf_bip_cli_start_print_params.request.data = 
    (OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  bt_pf_bip_cli_start_print_params.request.final = 
    params_ptr->final;

  status = OI_BIPCLI_StartPrint(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_start_print_cfm_cb,
    &bt_pf_bip_cli_start_print_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_start_archive

DESCRIPTION
  Performs a BIP START_ARCHIVE operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_start_archive(
  bt_app_id_type                        app_id,
  bt_pf_cmd_bip_cli_start_archive_type* params_ptr )
{
  OI_STATUS status;
  uint16    i;

  /* OI_UUID128 is not packed but bt_sd_uuid128_type is, cannot cast. 
     Use memcpy to avoid misaligned access */
  memcpy( (void*)&(bt_pf_bip_cli_start_archive_params.request.secondaryServiceId.
                   ms32bits),
          (void*)&(params_ptr->arch_obj_srv_service_id),
          sizeof( uint32 ) );
  for (i = 0; i < OI_BT_UUID128_SIZE - sizeof( OI_UINT32 ); i++)
  {
    bt_pf_bip_cli_start_archive_params.request.secondaryServiceId.base[i] = 
      params_ptr->arch_obj_srv_service_id.uuid_byte[i + 4];
  }

  status = OI_BIPCLI_StartArchive(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_start_archive_cfm_cb,
    &bt_pf_bip_cli_start_archive_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_remote_display

DESCRIPTION
  Performs a BIP REMOTE_DISPLAY operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_remote_display(
  bt_app_id_type                         app_id,
  bt_pf_cmd_bip_cli_remote_display_type* params_ptr )
{
  OI_STATUS       status;
  OI_OBEX_UNICODE handle;
  uint16          handle_str[BT_PF_BIP_IMAGE_HANDLE_LEN + 1];

  /* handle is copied to avoid misaligned access when str pointer is not 
     aligned */
  memcpy( (void*)handle_str,
          (void*)params_ptr->image_handle_str,
          (BT_PF_BIP_IMAGE_HANDLE_LEN + 1) * sizeof( uint16 ) );

  if ( handle_str[0] == '\0' ) 
  {
    /* empty image-handle string */
    handle.len = 0;
  }
  else
  {
    handle.len = BT_PF_BIP_IMAGE_HANDLE_LEN + 1;
  }
  handle.str = handle_str;
  
  bt_pf_bip_cli_remote_display_params.request.imageHandle = &handle;
  bt_pf_bip_cli_remote_display_params.request.displayCommand = 
    params_ptr->display_command;

  status = OI_BIPCLI_RemoteDisplay(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_remote_display_cfm_cb,
    &bt_pf_bip_cli_remote_display_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bip_cli_cmd_get_monitoring_image

DESCRIPTION
  Performs a BIP GET_MONITORING_IMAGE operation.

===========================================================================*/
bt_cmd_status_type bt_pf_bip_cli_cmd_get_monitoring_image(
  bt_app_id_type                            app_id,
  bt_pf_cmd_bip_cli_get_monitor_image_type* params_ptr )
{
  OI_STATUS status;

  bt_pf_bip_cli_monitor_image_params.request.storeImage = 
    params_ptr->store_image;

  status = OI_BIPCLI_GetMonitoringImage(
    (OI_BIPCLI_HANDLE)params_ptr->conn_id,
    bt_pf_bip_cli_get_monitoring_image_cfm_cb,
    &bt_pf_bip_cli_monitor_image_params );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_abort

DESCRIPTION
  Terminates the current basic imaging operation.
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_abort(
  bt_app_id_type                       app_id,
  bt_pf_bip_cli_conn_id_type           conn_id )
{
  OI_STATUS status;

  BT_MSG_API( "BT PF BIP CLI: abort App: %x, Conn: %x", 
              app_id, conn_id, 0 );

  status = OI_BIPCLI_Cancel( (OI_BIPCLI_HANDLE)conn_id, NULL );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BIP CLI: OI_BIPCLI_Abort returns %x", 
                status, 0, 0 );
  }
  return(status);

}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bip_cli_get_rsp_cfm

DESCRIPTION
  This command is called by the application to confirm that a get response 
  event and the accompanying data has been processed. 
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bip_cli_get_rsp_cfm(
  bt_app_id_type             app_id,
  bt_pf_bip_cli_conn_id_type conn_id,
  bt_event_type              get_event )
{
  if (conn_id == bt_pf_bip_cli_conn_id)
  {
    BT_MSG_API( "BT PF BIP CLI GET RSP CFM: App: %x, Conn: %x, Ev: %x", 
                app_id, conn_id, get_event );
    bt_pf_bip_cli_get_rsp_cfm_event = get_event;
    rex_set_sigs( &bt_pf_tcb, BT_PF_BIP_CLI_SIG );
    return(OI_OK);
  }
  else
  {
    BT_ERR( "BT PF BIP CLI GET RSP CFM: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  } 
}

#endif /* FEATURE_BT_EXTPF_BIP */
#endif /* FEATURE_BT */

