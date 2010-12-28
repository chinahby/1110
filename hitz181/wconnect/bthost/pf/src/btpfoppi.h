#ifndef _BTPFOPP_H
#define _BTPFOPP_H

#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_OPP 

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         B L U E T O O T H    O B J E C T    P U S H    P R O F I L E   
    
                         I N T E R N A L    H E A D E R

DESCRIPTION
  This file contains declarations internal to the Bluetooth Object Push
  Profile.
  
Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfoppi.h#1 $ 

$Log: $
*
*    #7        20 Apr 2005            JH
* Converted file operation done and abort functions into queued commands.
*
*    #6        30 Aug 2004            JH
* Removed declaration of bt_pf_obex_srv_count; this is now kept within SD.
*
*    #5        26 Jul 2004            JH
* Eliminated all object operation confirm data structures.
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

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        Data Definitions                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_pf_opp_cli_set_conn_id(
  bt_pf_opp_cli_conn_id_type    conn_id );

extern bt_cmd_status_type bt_pf_opp_cli_cmd_connect(
  bt_app_id_type                app_id,
  bt_bd_addr_type*              bd_addr_ptr,
  uint8                         channel,
  bt_pf_opp_cli_conn_id_type*   conn_id_ptr );

extern bt_cmd_status_type bt_pf_opp_cli_cmd_abort(
  bt_app_id_type               app_id,
  bt_pf_opp_srv_conn_id_type   conn_id );

extern bt_cmd_status_type bt_pf_opp_cli_cmd_open_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  uint16*                       name_ptr,
  uint8                         name_len,
  char*                         type_ptr,
  uint32                        size,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_opp_cli_cmd_open_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_opp_cli_cmd_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        data_len,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_opp_cli_cmd_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_opp_srv_cmd_register(
  bt_app_id_type                app_id,   
  uint32                        supported_formats,
  char*                         srv_name_ptr );

extern bt_cmd_status_type bt_pf_opp_srv_cmd_deregister(
  bt_app_id_type                app_id );

extern bt_cmd_status_type bt_pf_opp_srv_cmd_open_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  uint16*                       name_ptr,
  uint8                         name_len,
  char*                         type_ptr,
  uint32                        size,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_opp_srv_cmd_open_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_opp_srv_cmd_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        data_len,
  bt_cmd_status_type            status );

extern bt_cmd_status_type bt_pf_opp_srv_cmd_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status );

#endif /* FEATURE_BT_EXTPF_OPP */
#endif /* FEATURE_BT */
#endif /* _BTPFOPP_H */
