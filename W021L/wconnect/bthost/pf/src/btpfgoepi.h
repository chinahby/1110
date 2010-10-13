#ifndef _BTPFGOEP_H
#define _BTPFGOEP_H

#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_GOEP

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H    G E N E R I C    O B J E C T    E X C H A N G E
       
            P R O F I L E    I N T E R N A L    H E A D E R

DESCRIPTION
  This file contains declarations internal to the Bluetooth File Transfer 
  Profile.
  
Copyright (c) 2004-2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfgoepi.h#1 $ 

$Log: $

*    #12      28 Feb 2007           MH
* Added full access info to client auth chllenge call back. 
*
*    #11      19 Sept 2006           MH
* Client Authentication response call back added.
*
*    #10      25 AUG 2006            BK
* Fixed compile issues
*
*    #9       12 Aug 2006            BK
* Moved prototype of bt_obex_client_authenticate() to btpf.h
*  
*    #8       28 Jul 2006            BK
* Added prototype for bt_obex_client_authenticate()
*
*    #6-7     10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #5       23 Jun 2006            JH
* Modified to support updated GOEP API for JSR82.
*
*    #4       28 Mar 2006            JH
* Added prototype of bt_pf_goep_cli_app_conn_table_init().
*

===========================================================================*/
#include "memheap.h"
#include "oi_obexauth.h"
#include "oi_obexcli.h"
#include "oi_obexcommon.h"
#include "oi_obexspec.h"
#include "oi_obexsrv.h"
#include "dsm.h"
#include "bt.h"
#include "btpf.h"
#include "btpfcmdi.h"          
       
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        Data Definitions                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define BT_PF_GOEP_NO_CONN_ID_NULL 0xFFFF
#define BT_PF_GOEP_CLI_APP_CONN_FULL -1
#define BT_PF_GOEP_CLI_APP_CONN_NOT_FOUND -2 

extern mem_heap_type   bt_pf_heap;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* This is the replacement of RfcommRecvDataInd in obexsrv.c */
extern void bt_pfgoep_srv_rc_rcv_ind( OI_RFCOMM_LINK_HANDLE, 
                                      dsm_watermark_type* );

/* This is the replacement of RfcommRecvDataInd in obexcli.c */
extern void bt_pfgoep_cli_rc_rcv_ind( OI_RFCOMM_LINK_HANDLE, 
                                      dsm_watermark_type* );

/* Reassemblies OBEX oackets, replaces OI_OBEXCOMMON_ReassemblePacket */
extern OI_STATUS bt_pfgoep_reassemble_pkt(OBEX_COMMON*, dsm_watermark_type*);

/* Returns the corresponding OI security type */
OI_SECURITY_POLICY bt_pf_goep_oi_security( bt_security_type );

/* initializes GOEP connection table */
extern void bt_pf_goep_srv_target_app_table_init( void );
extern void bt_pf_goep_cli_app_conn_table_init( void );
     
/* assign an entry in the GOEP client application-connection table */
extern int16 bt_pf_goep_cli_app_conn_assign( bt_app_id_type    );
/* insert connection id into the GOEP client application-connection table
   entry for the corresponding application id */
extern int16 bt_pf_goep_cli_app_conn_link( bt_app_id_type   ,
                                           bt_pf_goep_cli_conn_id_type );

/* Client callback functions */ 
extern void bt_pf_goep_cli_con_cfm_cb( OI_OBEXCLI_CONNECTION_HANDLE,
                                       OI_BOOL, OI_OBEX_HEADER_LIST*, 
                                       OI_STATUS );

extern void bt_pf_goep_cli_dcn_ind_cb( OI_OBEXCLI_CONNECTION_HANDLE,
                                       OI_OBEX_HEADER_LIST* );

extern void bt_pf_goep_cli_auth_ind_cb( OI_OBEXCLI_CONNECTION_HANDLE,
                                        OI_BOOL,
                                        OI_BOOL );

extern void bt_pf_goep_cli_auth_rsp_ind_cb( OI_OBEXCLI_CONNECTION_HANDLE);

extern void bt_pf_goep_cli_put_cfm_cb( OI_OBEXCLI_CONNECTION_HANDLE,
                                       OI_OBEX_HEADER_LIST*, OI_STATUS );

extern void bt_pf_goep_cli_get_recv_data_cb( OI_OBEXCLI_CONNECTION_HANDLE,
                                             OI_OBEX_HEADER_LIST*, OI_STATUS );

extern void bt_pf_goep_cli_set_path_cfm_cb( OI_OBEXCLI_CONNECTION_HANDLE,
                                            OI_OBEX_HEADER_LIST*, OI_STATUS );



#endif /* FEATURE_BT_EXTPF_GOEP */

#endif /* FEATURE_BT */
 
#endif /* _BTPFGOEP_H */
