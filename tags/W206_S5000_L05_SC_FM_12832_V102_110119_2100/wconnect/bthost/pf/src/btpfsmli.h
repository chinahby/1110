#ifndef _BTPFSMLI_H_
#define _BTPFSMLI_H_
                 
#ifdef FEATURE_BT                 
#ifdef FEATURE_BT_EXTPF_SML

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   B L U E T O O T H    S Y N C - M L    A B S T R A C T I O N    L A Y E R 
  
                   I N T E R N A L    H E A D E R

DESCRIPTION
  This file contains declarations internal to the Bluetooth Basic Printing 
  Profile.
  
Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfsmli.h#1 $ 

$Log: $
*
*    #2        30 Aug 2004            JH
* Removed declaration of bt_pf_obex_srv_count; this is now kept within SD.
*
*    #1        30 Apr 2004            JH
* Initial version
*

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

#define BT_PF_SML_OBEX_TARGET_UUID "SYNCML-SYNC"

extern mem_heap_type  bt_pf_heap;
extern bt_app_id_type bt_pf_sml_app_id;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

extern bt_cmd_status_type bt_pf_sml_cmd_register_client(
  bt_app_id_type                   app_id,
  bt_pf_cmd_sml_register_cli_type* params_ptr );

extern bt_cmd_status_type bt_pf_sml_cmd_deregister_client(
  bt_app_id_type                   app_id );

extern bt_cmd_status_type bt_pf_sml_cmd_register_server(
  bt_app_id_type                   app_id,
  bt_pf_cmd_sml_register_srv_type* params_ptr );

extern bt_cmd_status_type bt_pf_sml_cmd_deregister_server(
  bt_app_id_type                   app_id );


#endif /* FEATURE_BT_EXTPF_SML */
#endif /* FEATURE_BT */
#endif /* _BTPFSMLI_H_ */
