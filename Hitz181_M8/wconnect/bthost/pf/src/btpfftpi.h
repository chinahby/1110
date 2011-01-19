#ifndef _BTPFFTP_H_
#define _BTPFFTP_H_
                 
#ifdef FEATURE_BT                 
#ifdef FEATURE_BT_EXTPF_FTP

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H    F I L E    T R A N S F E R    P R O F I L E
  
                   I N T E R N A L    H E A D E R

DESCRIPTION
  This file contains declarations internal to the Bluetooth File Transfer 
  Profile.
  
Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfftpi.h#1 $ 

$Log: $
*
*    #7        30 Aug 2004            JH
* Removed declaration of bt_pf_obex_srv_count; this is now kept within SD.
*
*    #6        26 Jul 2004            JH
* Eliminated all file operation confirm data structures.
*
*    #5        14 May 2004            JH
* Removed duplicate struct typedefs
*
*    #3        23 Apr 2004            JH
* Replaced bt_pf_cmd_status_type with bt_cmd_status_type
*
*    #2        14 Apr 2004            JH
* Made changes related to the replacement of bt_pf_unicode_str_type command 
*  parameters with uint16* type
*

===========================================================================*/

#define BT_MSG_LAYER  BT_MSG_GN
#include "btpf.h"
#include "btpfcmdi.h"
#include "memheap.h"
#include "oi_ftp_client.h"
#include "oi_obexspec.h"       

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        Data Definitions                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern mem_heap_type  bt_pf_heap;
extern rex_timer_type bt_pf_ftp_tmo_timer;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_pf_ftp_cli_cmd_auth_rsp(
  bt_app_id_type                   app_id,
  bt_pf_cmd_ftp_cli_auth_rsp_type* params_ptr );
                                      
extern void bt_pf_ftp_folder_list_cb( 
  OI_FTP_CLIENT_CONNECTION_HANDLE  connectionId,
  OI_OBEX_BYTESEQ*                 folderData,
  OI_STATUS                        status );

extern bt_cmd_status_type bt_pf_ftp_cli_cmd_connect(
  bt_app_id_type                   app_id,
  bt_bd_addr_type*                 bd_addr_ptr,
  uint8                            channel );

extern bt_cmd_status_type bt_pf_ftp_cli_cmd_abort(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id );

extern bt_cmd_status_type bt_pf_ftp_cli_cmd_browse_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_ftp_cli_cmd_open_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_ftp_cli_cmd_read_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        len,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_ftp_cli_cmd_write_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_ftp_cli_cmd_set_folder_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id, 
  bt_cmd_status_type            status );
                            
extern bt_cmd_status_type bt_pf_ftp_srv_cmd_register(
  bt_app_id_type                   app_id,
  bt_pf_goep_srv_auth_type         auth,
  boolean                          read_only,
  boolean                          allow_browse,            
  char*                            srv_name_ptr );

extern bt_cmd_status_type bt_pf_ftp_srv_cmd_deregister(
  bt_app_id_type                   app_id );


extern bt_cmd_status_type bt_pf_ftp_srv_cmd_browse_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_ftp_srv_cmd_open_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_ftp_srv_cmd_read_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        len,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_ftp_srv_cmd_write_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_ftp_srv_cmd_set_folder_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id, 
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_ftp_srv_cmd_delete_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id, 
  bt_cmd_status_type            status );

#endif /* FEATURE_BT_EXTPF_FTP */
#endif /* FEATURE_BT */
#endif /* _BTPFFTP_H_ */
