#ifndef _BTPFBPPI_H_
#define _BTPFBPPI_H_
                 
#ifdef FEATURE_BT                 
#ifdef FEATURE_BT_EXTPF_BPP

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H    B A S I C    P R I N T I N G    P R O F I L E
  
                   I N T E R N A L    H E A D E R

DESCRIPTION
  This file contains declarations internal to the Bluetooth Basic Printing 
  Profile.
  
              Copyright (c) 2004,2008 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary 

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfbppi.h#1 $ 
$DateTime: 2009/01/07 18:14:54 $

 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2008-06-16   pr  Added cmd to tear down connection from printer to
                  BPP server (object channel).
*
*    #5        30 Aug 2004            JH
* Removed declaration of bt_pf_obex_srv_count; this is now kept within SD.
*
*    #4        26 Jul 2004            JH
* Revamped BPP signal and timer scheme.
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
extern rex_timer_type bt_pf_bpp_cli_tmo_timer;
extern rex_timer_type bt_pf_bpp_srv_tmo_timer;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_pf_bpp_sndr_cmd_connect(
  bt_app_id_type                   app_id,
  bt_pf_cmd_bpp_sndr_connect_type* params_ptr );

extern bt_cmd_status_type bt_pf_bpp_sndr_cmd_disconnect(
  bt_app_id_type                      app_id,
  bt_pf_cmd_bpp_sndr_disconnect_type* params_ptr );

extern bt_cmd_status_type bt_pf_bpp_sndr_cmd_auth_rsp(
  bt_app_id_type                    app_id,
  bt_pf_cmd_bpp_sndr_auth_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bpp_sndr_cmd_send_file(
  bt_app_id_type                     app_id,
  bt_pf_cmd_bpp_sndr_send_file_type* params_ptr );

extern bt_cmd_status_type bt_pf_bpp_sndr_cmd_send_reference(
  bt_app_id_type                          app_id,
  bt_pf_cmd_bpp_sndr_send_reference_type* params_ptr );

extern bt_cmd_status_type bt_pf_bpp_sndr_cmd_soap_request(
  bt_app_id_type                    app_id,
  bt_pf_cmd_bpp_sndr_soap_req_type* params_ptr );

extern bt_cmd_status_type bt_pf_bpp_sndr_cmd_send_ref_obj_response(
  bt_app_id_type                           app_id,
  bt_pf_cmd_bpp_sndr_get_ref_obj_rsp_type* params_ptr );

extern bt_cmd_status_type bt_pf_bpp_sndr_cmd_register( 
  bt_app_id_type    app_id,
  char*             srv_name_ptr );

extern bt_cmd_status_type bt_pf_bpp_sndr_cmd_deregister(
  bt_app_id_type    app_id );

extern bt_cmd_status_type bt_pf_bpp_sndr_cmd_force_disconnect(
  bt_app_id_type                            app_id,
  bt_pf_cmd_bpp_sndr_force_disconnect_type* params_ptr );
#endif /* FEATURE_BT_EXTPF_BPP */
#endif /* FEATURE_BT */
#endif /* _BTPFBPPI_H_ */
