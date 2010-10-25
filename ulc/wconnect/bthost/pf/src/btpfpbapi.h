#ifndef _BTPFPBAP_H_
#define _BTPFPBAP_H_
                 
#ifdef FEATURE_BT                 
#ifdef FEATURE_BT_EXTPF_PBAP

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H   P H O N E   B O O K   A C C E S S   P R O F I L E
  
                   I N T E R N A L    H E A D E R

DESCRIPTION
  This file contains declarations internal to the Bluetooth Phone Book  
  Access Profile.
  
Copyright (c) 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfpbapi.h#1 $ 

$Log: $
*
*    #1        08 Feb 2006            GS
* Initial revision
*

===========================================================================*/

#define BT_MSG_LAYER  BT_MSG_GN
#include "btpf.h"
#include "btpfcmdi.h"
#include "memheap.h"
#include "pbap_cli.h"
#include "oi_obexspec.h"       

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        Data Definitions                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern mem_heap_type  bt_pf_heap;
extern rex_timer_type bt_pf_pbap_tmo_timer;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/                          
extern bt_cmd_status_type bt_pf_pbap_cli_cmd_connect(
  bt_app_id_type                      app_id,
  bt_pf_cmd_pbap_cli_connect_type*    params_ptr );

extern bt_cmd_status_type bt_pf_pbap_cli_cmd_disconnect(
  bt_app_id_type                      app_id,
  bt_pf_cmd_pbap_cli_disconnect_type* params_ptr );

extern bt_cmd_status_type bt_pf_pbap_cli_cmd_auth_rsp(
  bt_app_id_type                      app_id,
  bt_pf_cmd_pbap_cli_auth_rsp_type*   params_ptr ); 

extern bt_cmd_status_type bt_pf_pbap_cli_cmd_register(
  bt_app_id_type                      app_id,
  char*                               cli_name_ptr );

extern bt_cmd_status_type bt_pf_pbap_cli_cmd_deregister(
  bt_app_id_type                      app_id );

extern bt_cmd_status_type bt_pf_pbap_cli_cmd_abort(
  bt_app_id_type                      app_id,
  bt_pf_pbap_cli_conn_id_type         conn_id );

extern bt_cmd_status_type bt_pf_pbap_cli_cmd_pull_phone_book(
  bt_app_id_type                           app_id,
  bt_pf_cmd_pbap_cli_pull_phone_book_type* params_ptr );

extern bt_cmd_status_type bt_pf_pbap_cli_cmd_set_phone_book(
  bt_app_id_type                           app_id,
  bt_pf_cmd_pbap_cli_set_phone_book_type*  params_ptr );

extern bt_cmd_status_type bt_pf_pbap_cli_cmd_pull_vcard_listing(
  bt_app_id_type                           app_id,
  bt_pf_cmd_pbap_cli_pull_vcard_list_type* params_ptr );

extern bt_cmd_status_type bt_pf_pbap_cli_cmd_pull_vcard_entry(
  bt_app_id_type                            app_id,
  bt_pf_cmd_pbap_cli_pull_vcard_entry_type* params_ptr );
                            
extern bt_cmd_status_type bt_pf_pbap_srv_cmd_register(
  bt_app_id_type                      app_id,
  char*                               srv_name_ptr,        
  uint8                               supported_repositories,
  bt_pf_goep_srv_auth_type            auth );

extern bt_cmd_status_type bt_pf_pbap_srv_cmd_deregister(
  bt_app_id_type                      app_id );

extern bt_cmd_status_type bt_pf_pbap_srv_cmd_auth_rsp(
  bt_app_id_type                      app_id,
  bt_pf_cmd_pbap_srv_auth_rsp_type*   params_ptr );

extern bt_cmd_status_type bt_pf_pbap_srv_cmd_force_disconnect(
  bt_app_id_type                      app_id,
  bt_pf_pbap_srv_conn_id_type         conn_id );

extern bt_cmd_status_type bt_pf_pbap_srv_cmd_pull_phone_book_rsp(
  bt_app_id_type                                app_id,
  bt_pf_cmd_pbap_srv_pull_phone_book_rsp_type*  params_ptr );

extern bt_cmd_status_type bt_pf_pbap_srv_cmd_set_phone_book_rsp(
  bt_app_id_type                                app_id,
  bt_pf_cmd_pbap_srv_set_phone_book_rsp_type*   params_ptr );

extern bt_cmd_status_type bt_pf_pbap_srv_cmd_pull_vcard_listing_rsp(
  bt_app_id_type                                app_id,
  bt_pf_cmd_pbap_srv_pull_vcard_list_rsp_type*  params_ptr );

extern bt_cmd_status_type bt_pf_pbap_srv_cmd_pull_vcard_entry_rsp(
  bt_app_id_type                                 app_id,
  bt_pf_cmd_pbap_srv_pull_vcard_entry_rsp_type*  params_ptr );

#endif /* FEATURE_BT_EXTPF_PBAP */
#endif /* FEATURE_BT */
#endif /* _BTPFPBAP_H_ */
