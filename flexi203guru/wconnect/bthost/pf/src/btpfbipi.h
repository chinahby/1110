#ifndef _BTPFBIPI_H_
#define _BTPFBIPI_H_
                 
#ifdef FEATURE_BT                 
#ifdef FEATURE_BT_EXTPF_BIP      

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H    B A S I C    I M A G I N G    P R O F I L E
  
                   I N T E R N A L    H E A D E R

DESCRIPTION
  This file contains declarations internal to the Bluetooth Basic Imaging 
  Profile.
  
Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfbipi.h#1 $ 

$Log: $                                                       
*
*    #7        30 Aug 2004            JH
* Removed declaration of bt_pf_obex_srv_count; this is now kept within SD.
*
*    #6        26 Jul 2004            JH
* Revamped BIP signal and timer scheme.
*
*    #5        24 May 2004            JH
* Added BIP server start print response to facilitate APIT testing.
*
*    #2        23 Apr 2004            JH
* Replaced bt_pf_cmd_status_type with bt_cmd_status_type
*
*    #1        14 Apr 2004            JH
* Initial version

===========================================================================*/

#define BT_MSG_LAYER  BT_MSG_GN
#include "btpf.h"
#include "btpfcmdi.h"
#include "memheap.h"           

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        Data Definitions                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern mem_heap_type  bt_pf_heap;
extern rex_timer_type bt_pf_bip_cli_tmo_timer;
extern rex_timer_type bt_pf_bip_srv_tmo_timer;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_pf_bip_cli_cmd_connect(
  bt_app_id_type                  app_id,
  bt_pf_cmd_bip_cli_connect_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_disconnect(
  bt_app_id_type                     app_id,
  bt_pf_cmd_bip_cli_disconnect_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_auth_rsp(
  bt_app_id_type                   app_id,
  bt_pf_cmd_bip_cli_auth_rsp_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_get_capabilities(
  bt_app_id_type                           app_id,
  bt_pf_cmd_bip_cli_get_capabilities_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_put_image(
  bt_app_id_type                    app_id,
  bt_pf_cmd_bip_cli_put_image_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_put_linked_thumbnail(
  bt_app_id_type                        app_id,
  bt_pf_cmd_bip_cli_put_thumbnail_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_put_linked_attachment(
  bt_app_id_type                         app_id,
  bt_pf_cmd_bip_cli_put_attachment_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_get_images_list(
  bt_app_id_type                   app_id,
  bt_pf_cmd_bip_cli_get_list_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_get_image_properties(
  bt_app_id_type                         app_id,
  bt_pf_cmd_bip_cli_get_properties_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_get_image(
  bt_app_id_type                    app_id,
  bt_pf_cmd_bip_cli_get_image_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_get_linked_thumbnail(
  bt_app_id_type                        app_id,
  bt_pf_cmd_bip_cli_get_thumbnail_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_get_linked_attachment(
  bt_app_id_type                         app_id,
  bt_pf_cmd_bip_cli_get_attachment_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_delete_image(
  bt_app_id_type                       app_id,
  bt_pf_cmd_bip_cli_delete_image_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_get_status(
  bt_app_id_type                     app_id,
  bt_pf_cmd_bip_cli_get_status_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_get_partial_image(
  bt_app_id_type                            app_id,
  bt_pf_cmd_bip_cli_get_partial_image_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_start_print(
  bt_app_id_type                      app_id,
  bt_pf_cmd_bip_cli_start_print_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_start_archive(
  bt_app_id_type                        app_id,
  bt_pf_cmd_bip_cli_start_archive_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_remote_display(
  bt_app_id_type                         app_id,
  bt_pf_cmd_bip_cli_remote_display_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_cli_cmd_get_monitoring_image(
  bt_app_id_type                            app_id,
  bt_pf_cmd_bip_cli_get_monitor_image_type* param_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_register_imaging_responder(
  bt_app_id_type          app_id,
  char*                   srv_name_ptr,        
  uint8                   supported_capabilities,
  uint16                  supported_features,
  uint32                  supported_functions,
  uint64                  imaging_capacity,
  boolean                 auth );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_register_referenced_objects(
  bt_app_id_type          app_id,
  char*                   srv_name_ptr,
  bt_sd_uuid128_type*     service_id_ptr,
  uint32                  supported_functions,
  boolean                 auth );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_register_archived_objects(
  bt_app_id_type          app_id,
  char*                   srv_name_ptr,
  bt_sd_uuid128_type*     service_id_ptr,
  uint32                  supported_functions,
  boolean                 auth );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_deregister_imaging_responder(
  bt_app_id_type          app_id );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_deregister_referenced_objects(
  bt_app_id_type          app_id );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_deregister_archived_objects(
  bt_app_id_type          app_id );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_auth_rsp(
  bt_app_id_type                   app_id,
  bt_pf_cmd_bip_srv_auth_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_get_capabilities_rsp(
  bt_app_id_type                               app_id,
  bt_pf_cmd_bip_srv_get_capabilities_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_put_image_rsp(
  bt_app_id_type                        app_id,
  bt_pf_cmd_bip_srv_put_image_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_put_linked_thumbnail_rsp(
  bt_app_id_type                            app_id,
  bt_pf_cmd_bip_srv_put_thumbnail_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_put_linked_attachment_rsp(
  bt_app_id_type                             app_id,
  bt_pf_cmd_bip_srv_put_attachment_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_get_images_list_rsp(
  bt_app_id_type                       app_id,
  bt_pf_cmd_bip_srv_get_list_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_get_image_properties_rsp(
  bt_app_id_type                             app_id,
  bt_pf_cmd_bip_srv_get_properties_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_get_image_rsp(
  bt_app_id_type                        app_id,
  bt_pf_cmd_bip_srv_get_image_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_get_linked_thumbnail_rsp(
  bt_app_id_type                            app_id,
  bt_pf_cmd_bip_srv_get_thumbnail_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_get_linked_attachment_rsp(
  bt_app_id_type                             app_id,
  bt_pf_cmd_bip_srv_get_attachment_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_delete_image_rsp(
  bt_app_id_type                           app_id,
  bt_pf_cmd_bip_srv_delete_image_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_get_status_rsp(
  bt_app_id_type                         app_id,
  bt_pf_cmd_bip_srv_get_status_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_get_partial_image_rsp(
  bt_app_id_type                                app_id,
  bt_pf_cmd_bip_srv_get_partial_image_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_start_archive_rsp(
  bt_app_id_type                            app_id,
  bt_pf_cmd_bip_srv_start_archive_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_remote_display_rsp(
  bt_app_id_type                             app_id,
  bt_pf_cmd_bip_srv_remote_display_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bip_srv_cmd_get_monitoring_image_rsp(
  bt_app_id_type                                app_id,
  bt_pf_cmd_bip_srv_get_monitor_image_rsp_type* params_ptr );

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT_EXTPF_BIP */
#endif /* FEATURE_BT */
#endif /* _BTPFBIPI_H_ */

