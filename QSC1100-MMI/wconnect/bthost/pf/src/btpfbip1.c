/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      B L U E T O O T H   B A S I C   I M A G I N G   P R O F I L E

GENERAL DESCRIPTION
  This module contains the Bluetooth Basic Imaging Profile data and code
  which are not client or server specific.

Copyright (c) 2004-2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfbip1.c#1 $ 

$Log: $
*
*   #11        21 May 2007            SSK
* Fixing compiler warning by introducing new line at end of file.
*
*   #10        12 Jun 2006            DSN
* Typecasting fixes for RVCT compiler.
*
*    #9        18 Jul 2005            GS
* Added support for BIP server force disconnect.
*
*    #8        19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #7         7 Jan 2005            JH
* Modified to avoid possible misaligned memory accesses.
*
*    #6        24 May 2004            JH
* Added BIP server start print response to facilitate APIT testing.
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
#ifdef FEATURE_BT_EXTPF_BIP

#include "bt.h"
#include "btpfi.h"
#include "btpfcmdi.h"
#include "bti.h"
#include "btmsg.h"
#include "btpfbipi.h"
#include "oi_bip_private.c" 
#include "oi_bip_srv.c"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items shared by both the client and server.

===========================================================================*/

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_bip_process_command    
   
DESCRIPTION
  Processes the given command by calling the underlying OI BIP driver.
   
===========================================================================*/
void bt_pf_bip_process_command( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_status_type status;
  bt_sd_uuid128_type service_id;

  switch ( cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_PF_CMD_BIP_CLI_CONNECT:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: con", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_connect(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_connect) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_DISCONNECT:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: dcn", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_disconnect(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_disconnect) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_AUTH_RSP:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: auth rsp", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_auth_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_auth_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_GET_CAPABILITIES:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: get cap", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_get_capabilities(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_get_capabilities) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_PUT_IMAGE:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: put img", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_put_image(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_put_image) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_PUT_LINKED_THUMBNAIL:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: put tnl", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_put_linked_thumbnail(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_put_thumbnail) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_PUT_LINKED_ATTACHMENT:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: put att", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_put_linked_attachment(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_put_attachment) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_GET_IMAGES_LIST:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: get list", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_get_images_list(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_get_list) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_GET_IMAGE_PROPERTIES:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: get prop", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_get_image_properties(
        cmd_ptr->cmd_msg.cmd_bip_cli_get_properties.conn_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_get_properties) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_GET_IMAGE:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: get img", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_get_image(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_get_image) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_GET_LINKED_THUMBNAIL:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: get tnl", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_get_linked_thumbnail(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_get_thumbnail) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_GET_LINKED_ATTACHMENT:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: get att", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_get_linked_attachment(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_get_attachment) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_DELETE_IMAGE:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: del img", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_delete_image(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_delete_image) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_GET_STATUS:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: get stat", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_get_status(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_get_status) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_GET_PARTIAL_IMAGE:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: get part img", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_get_partial_image(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_get_partial_image) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_START_PRINT:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: strt prn", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_start_print(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_start_print) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_START_ARCHIVE:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: strt arch", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_start_archive(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_start_archive) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_REMOTE_DISPLAY:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: rmt disp", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_remote_display(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_remote_display) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_CLI_GET_MONITORING_IMAGE:
      BT_MSG_DEBUG( "BT PF BIP CLI CMD RX: mnt img", 0, 0, 0 );
      status = bt_pf_bip_cli_cmd_get_monitoring_image(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_cli_get_monitor_image) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_REG_IMAGING_RERSPONDER:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: reg img rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_register_imaging_responder(
        cmd_ptr->cmd_hdr.app_id,
        (char *)cmd_ptr->cmd_msg.cmd_bip_srv_reg_img_responder.srv_name_str,
        cmd_ptr->cmd_msg.cmd_bip_srv_reg_img_responder.supported_capabilities,
        cmd_ptr->cmd_msg.cmd_bip_srv_reg_img_responder.supported_features,
        cmd_ptr->cmd_msg.cmd_bip_srv_reg_img_responder.supported_functions,
        cmd_ptr->cmd_msg.cmd_bip_srv_reg_img_responder.imaging_capacity,
        cmd_ptr->cmd_msg.cmd_bip_srv_reg_img_responder.auth );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_REG_REFERENCED_OBJECTS:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: reg ref obj", 0, 0, 0 );
      service_id = cmd_ptr->cmd_msg.cmd_bip_srv_reg_ref_objects.service_id;
      status = bt_pf_bip_srv_cmd_register_referenced_objects(
        cmd_ptr->cmd_hdr.app_id,
        (char *)cmd_ptr->cmd_msg.cmd_bip_srv_reg_ref_objects.srv_name_str,         
        &service_id,
        cmd_ptr->cmd_msg.cmd_bip_srv_reg_ref_objects.supported_functions,
        cmd_ptr->cmd_msg.cmd_bip_srv_reg_ref_objects.auth );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_REG_ARCHIVED_OBJECTS:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: reg arch obj", 0, 0, 0 );
      service_id = cmd_ptr->cmd_msg.cmd_bip_srv_reg_arch_objects.service_id;
      status = bt_pf_bip_srv_cmd_register_archived_objects(
        cmd_ptr->cmd_hdr.app_id,
        (char *)cmd_ptr->cmd_msg.cmd_bip_srv_reg_arch_objects.srv_name_str,
        &service_id,
        cmd_ptr->cmd_msg.cmd_bip_srv_reg_arch_objects.supported_functions,
        cmd_ptr->cmd_msg.cmd_bip_srv_reg_arch_objects.auth );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_DEREG_IMAGING_RERSPONDER:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: dereg img rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_deregister_imaging_responder(
        cmd_ptr->cmd_hdr.app_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_DEREG_REFERENCED_OBJECTS:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: dereg ref obj", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_deregister_referenced_objects(
        cmd_ptr->cmd_hdr.app_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_DEREG_ARCHIVED_OBJECTS:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: dereg arch obj", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_deregister_archived_objects(
        cmd_ptr->cmd_hdr.app_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;   

    case BT_PF_CMD_BIP_SRV_AUTH_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: auth rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_auth_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_auth_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_FORCE_DCN:
      BT_MSG_API( "BT PF BIP SRV CMD RX: Force Disconnect", 0, 0, 0 );
      status = OI_BIPSRV_ForceDisconnect( 
        (OI_BIPSRV_CONNECTION_HANDLE)
        cmd_ptr->cmd_msg.cmd_bip_srv_force_dcn.conn_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break; 

    case BT_PF_CMD_BIP_SRV_GET_CAPABILITIES_RSP: 
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: get cap rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_get_capabilities_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_get_capabilities_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_PUT_IMAGE_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: put img rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_put_image_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_put_image_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_PUT_LINKED_THUMBNAIL_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: put tnl rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_put_linked_thumbnail_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_put_thumbnail_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status; 
      break;

    case BT_PF_CMD_BIP_SRV_PUT_LINKED_ATTACHMENT:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: put att rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_put_linked_attachment_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_put_attachment_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_GET_IMAGES_LIST_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: get list rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_get_images_list_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_get_list_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_GET_IMAGE_PROPERTIES_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: get prop rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_get_image_properties_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_get_properties_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_GET_IMAGE_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: get img rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_get_image_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_get_image_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_GET_LINKED_THUMBNAIL_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: get tnl rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_get_linked_thumbnail_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_get_thumbnail_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_GET_LINKED_ATTACHMENT_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: get att rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_get_linked_attachment_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_get_attachment_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_DELETE_IMAGE_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: del img rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_delete_image_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_delete_image_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_GET_STATUS_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: get stat rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_get_status_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_get_status_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_GET_PARTIAL_IMAGE_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: get part img rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_get_partial_image_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_get_partial_image_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_START_ARCHIVE_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: sta arch rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_start_archive_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_start_archive_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_REMOTE_DISPLAY_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: rmt disp rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_remote_display_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_remote_display_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_BIP_SRV_GET_MONITORING_IMAGE_RSP:
      BT_MSG_DEBUG( "BT PF BIP SRV CMD RX: mnt img rsp", 0, 0, 0 );
      status = bt_pf_bip_srv_cmd_get_monitoring_image_rsp(
        cmd_ptr->cmd_hdr.app_id,
        &(cmd_ptr->cmd_msg.cmd_bip_srv_get_monitor_image_rsp) );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

    default:
      BT_ERR( "BT PF BIP CMD RX: Unknown: %x",
              cmd_ptr->cmd_hdr.cmd_type, 0, 0 );
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
  }
}



#endif /* FEATURE_BT_EXTPF_BIP */
#endif /* FEATURE_BT */ 
