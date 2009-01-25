/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 B L U E T O O T H   B A S I C   P R I N T I N G   P R O F I L E   C L I E N T

GENERAL DESCRIPTION
  This module contains the Bluetooth Basic Printing Profile client data and 
  code.

              Copyright (c) 2004-2009 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary 

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
                                                                              
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfbpp2.c#2 $ 
  $DateTime: 2011/03/17 00:37:53 $
  $Author: pvunnam $ 

 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2009-05-22   pn  Fixed compile errors when FEATURE_BT_EXTPF_BIP is undefined.
 2008-07-24   gs  Fixed Klocwork error.
 2008-06-16   pr  Added cmd to tear down connection from printer
                  to BPP server (object channel).
*
*    #24       28 Sep 2007            RS
* Added end of body parameter to the call back function 
* bt_pf_bpp_sndr_get_cfm_cb.
*
*    #23       30 JUL 2007            RS
* Setting suitable timer on the STS and DPS channel to timeout obex connection 
*
*    #22       21 May 2007            SSK
* Fix compiler warnings. 
*
*    #21       18 Oct 2006            PR
* Updated OI_BPP_SENDER_Register(),replaced OI_BPP_SENDER_Abort() 
* with OI_BPP_SENDER_Cancel() while merging OI Code v3.0.14
* 
*    #20       10 Jun 2006            GS
* Account for NULL char in Type header string length in 
* bt_pf_bpp_sndr_cmd_send_file().
*
*    #19       16 Jun 2006            DSN
* Removed superfluous header file inclusion.
*
*    #18      12 Jun 2006             DSN
* Typecasting fixes for RVCT compiler.
*
*    #17       12 Jul 2005            JH
* Added BT_PF_TMO_SIG to signal mask when calling rex_timed_wait() which was 
*  previously misused.
*
*    #16       22 Feb 2005            GS
* Fixed Lint issue in bt_pf_bpp_sndr_send_reference_cfm_cb()
*
*    #15       19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #14       11 Jan 2005            JH
* Added back the change made in #12.
*
*    #13       11 Jan 2005            JH
* Modified to avoid possible misaligned memory accesses.
* Backed out changes made in #12. Saber 4262 release not picking these up.
*
*    #12       29 Dec 2004            JH
* Added SCN parameter when calling bt_cmd_sd_update_obex_profile_service_record.
* Added support for status channel connection using a different RFCOMM channel.
*
*    #11       11 Nov 2005            JH
* Eliminated the done field in bt_pf_bpp_sndr_srv_wait_for_cfm_type and use the 
*  signal itself to tell if a timeout has occurred in the wait for cfm functions.
*
*    #10       05 Nov 2004            JH
* Added null pointer checking in callback functions.
* Modify deregister command not to free the assigned resources if deregister 
*  failed due to operation in progress.
*
*    #9       30 Aug 2004            JH
* Modified the registration command to call 
*  bt_cmd_sd_update_obex_profile_service_record for updating service record 
*  instead of calling SD functions directly.
* Removed calls for manipulating object transfer bit in class of device field;
*  this is now handled by SD.
* Removed pointer to service record field in bt_pf_bpp_sndr_conn_info.
*
*    #8        26 Jul 2004            JH
* Revamped BPP signal and timer scheme.
*
*    #7        15 Jun 2004
* Fixed bt_pf_bpp_sndr_connect_cfm_cb so that cleanup is performed only after 
* event is  constructed
*
*    #6        14 May 2004            JH
* Further command and event struct changes
* Added abort command
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
#ifdef FEATURE_BT_EXTPF_BPP

#include "bt.h"
#include "btpfi.h"
#include "bti.h"
#include "btmsg.h" 
#include "btpfbppi.h"
#include "btsd.h"
#include "rex.h"

#include "oi_bpp_sender.c"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/ 

LOCAL bt_app_id_type bt_pf_bpp_app_id = BT_APP_ID_NULL;
LOCAL bt_bd_addr_type bt_pf_bpp_null_bd_addr = BT_PF_BD_ADDR_NULL;
LOCAL bt_bd_addr_type bt_pf_bpp_bd_addr = BT_PF_BD_ADDR_NULL;
LOCAL bt_cmd_type bt_pf_bpp_cmd_in_prog = BT_CMD_NULL;

typedef struct
{
  bt_app_id_type              app_id;
  bt_pf_bpp_target_type       target;
  bt_pf_bpp_sndr_conn_id_type conn_id;
} bt_pf_bpp_sndr_conn_info_type;

typedef enum
{
  JOB_CHANNEL = 0,
  STATUS_CHANNEL,
  OBJECT_CHANNEL,
  NUM_BPP_SNDR
} bt_pf_bpp_sndr_conn_type;

/* only one job channel connection and one status channel connection
   is supported at any given time */
LOCAL bt_pf_bpp_sndr_conn_info_type bt_pf_bpp_sndr_conn_info[NUM_BPP_SNDR] =
{ 
  { BT_PF_APP_ID_NULL, BT_PF_BPP_NO_TARGET, BT_PF_BPP_NO_CONN_ID },
  { BT_PF_APP_ID_NULL, BT_PF_BPP_NO_TARGET, BT_PF_BPP_NO_CONN_ID },
  { BT_PF_APP_ID_NULL, BT_PF_BPP_NO_TARGET, BT_PF_BPP_NO_CONN_ID },
};

typedef struct
{
  bt_pf_bpp_sndr_conn_id_type conn_id;
  bt_event_type               event;
  bt_cmd_status_type          status;
} bt_pf_bpp_sndr_srv_wait_for_cfm_type;
                         

/* structure for holding info about the operation completion confirmation */
LOCAL bt_pf_bpp_sndr_srv_wait_for_cfm_type  bt_pf_bpp_sndr_srv_req_cfm;
LOCAL bt_pf_bpp_sndr_srv_wait_for_cfm_type  bt_pf_bpp_sndr_get_rsp_cfm;

OI_STATUS OI_BPP_DeregisterSenderServiceRecord(SENDER_SDP_DATA *sdp)
{
  return(OI_OK);
}

OI_STATUS OI_BPP_RegisterSenderServiceRecord(const OI_SDP_STRINGS *strings,
                                             SENDER_SDP_DATA **sdpData,
                                             OI_UINT8 objChannel,
                                             OI_UINT8 ruiObjChannel)
{
  return(OI_OK);
}

LOCAL uint16 bt_pf_bpp_ustrlen(BT_PACKED uint16* ustr)
{
  uint16 i = 0;

  while (*ustr != 0)
  {
    i++;
    ustr++;
  }
  return(++i); /* includes terminating NULL character */
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_wait_for_get_rsp_cfm

DESCRIPTION
  Wait for application to finish processing incoming get response event.

===========================================================================*/
LOCAL void bt_pf_bpp_sndr_wait_for_get_rsp_cfm( 
  bt_pf_bpp_sndr_conn_id_type conn_id )
{
  bt_pf_bpp_sndr_get_rsp_cfm.conn_id = conn_id;
  rex_timed_wait( BT_PF_BPP_CLI_SIG | BT_PF_TMO_SIG, 
                  &bt_pf_bpp_cli_tmo_timer, 
                  BT_PF_BPP_OPS_TMO );

  if ( rex_get_sigs( &bt_pf_tcb ) & BT_PF_BPP_CLI_SIG )
  {
    rex_clr_timer( &bt_pf_bpp_cli_tmo_timer );
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BPP_CLI_SIG );  
  }
  else if ( rex_get_sigs( &bt_pf_tcb ) & BT_PF_TMO_SIG )
  {
    /* signal not set => timeout has occurred */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_TMO_SIG );
    BT_ERR( "BT PF BPP CB CFM: timeout", 0, 0, 0 );
  }

  return;
}


/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_wait_for_srv_req_cfm

DESCRIPTION
  Wait for application to finish processing incoming server request events. 
  Returns status code supplied by application.

===========================================================================*/
LOCAL bt_cmd_status_type bt_pf_bpp_sndr_wait_for_srv_req_cfm( 
  bt_pf_bpp_sndr_conn_id_type conn_id,
  bt_event_type               req_event )
{
  OI_STATUS status = OI_TIMEOUT;

  bt_pf_bpp_sndr_srv_req_cfm.conn_id = conn_id;
  rex_timed_wait( BT_PF_BPP_SRV_SIG | BT_PF_TMO_SIG, 
                  &bt_pf_bpp_srv_tmo_timer, 
                  BT_PF_BPP_OPS_TMO );

  if ( rex_get_sigs( &bt_pf_tcb ) & BT_PF_BPP_SRV_SIG )
  {
    rex_clr_timer( &bt_pf_bpp_srv_tmo_timer );
    rex_clr_sigs( &bt_pf_tcb, BT_PF_BPP_SRV_SIG );  
    if ( bt_pf_bpp_sndr_srv_req_cfm.event != req_event )
    {
      BT_ERR( "BT PF BPP CB CFM: ev mismatch, ev: %x", 
              req_event, 0, 0 ); 
      status = OI_OBEX_INTERNAL_SERVER_ERROR;
    }
    else
    {
      status = (OI_STATUS)bt_pf_bpp_sndr_srv_req_cfm.status;
    }
  }
  else if ( rex_get_sigs( &bt_pf_tcb ) & BT_PF_TMO_SIG )
  {
    /* signal not set => timeout has occurred */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_TMO_SIG );
    BT_ERR( "BT PF BPP CB CFM: timeout", 0, 0, 0 );
  }

  return(status);
}

LOCAL bt_cmd_status_type bt_pf_bpp_sndr_find_job_status_conn(
  bt_pf_bpp_sndr_conn_id_type conn_id,
  bt_pf_bpp_sndr_conn_type*   index_ptr )
{
  bt_cmd_status_type status = OI_STATUS_NOT_FOUND;
  uint16 i;

  for (i = 0; i < OBJECT_CHANNEL; i++)
  {
    if ( bt_pf_bpp_sndr_conn_info[i].conn_id == conn_id )
    { 
      status = OI_OK;
      break;
    }
  }
  if (index_ptr != NULL)
  {
    *index_ptr = (bt_pf_bpp_sndr_conn_type)i;
  }
  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_connect_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to the connect command is received.
  This applies to the job channel or status channel connection to the remote 
  BPP printer.

===========================================================================*/
LOCAL void bt_pf_bpp_sndr_connect_cfm_cb( 
  OI_OBEXSRV_CONNECTION_HANDLE conn_id, 
  OI_STATUS                    status )
{ 
  bt_ev_msg_type           event;
  bt_cmd_status_type       ret;
  bt_pf_bpp_sndr_conn_type conn_index;

  ret = bt_pf_bpp_sndr_find_job_status_conn( conn_id, &conn_index );

  if ( (ret != OI_OK) || (conn_index > OBJECT_CHANNEL))
  {
    BT_ERR( "BT PF BPP CB RX: con cfm, unknown conn: %x", 
            conn_id, 0, 0 );
    return;
  }

  BT_MSG_DEBUG( "BT PF BPP CB RX: con cfm, conn: %x, app: %x, stat %x", 
                conn_id, bt_pf_bpp_sndr_conn_info[conn_index].app_id, status );
 
  event.ev_hdr.bt_app_id = bt_pf_bpp_sndr_conn_info[conn_index].app_id;
  event.ev_hdr.ev_type = BT_EV_PF_BPP_SNDR_CON_CFM;
  event.ev_msg.ev_bpp_sndr_con_cfm.target = 
    bt_pf_bpp_sndr_conn_info[conn_index].target;
  event.ev_msg.ev_bpp_sndr_con_cfm.conn_id = 
    (bt_pf_bpp_sndr_conn_id_type)conn_id;
  event.ev_msg.ev_bpp_sndr_con_cfm.status = 
    (bt_cmd_status_type)status;

  if (status != OI_OK)
  {
    /* bad connection, clean up */
    bt_pf_bpp_sndr_conn_info[conn_index].app_id = BT_PF_APP_ID_NULL;
    bt_pf_bpp_sndr_conn_info[conn_index].target = BT_PF_BPP_NO_TARGET;
    bt_pf_bpp_sndr_conn_info[conn_index].conn_id = 
      BT_PF_BPP_NO_CONN_ID;
  }

  if (event.ev_msg.ev_bpp_sndr_con_cfm.target == BT_PF_BPP_TARGET_STS) {

      // For the status channel we need to keep the channel alive even  
      // though the printer does not respond to the get requests and 
      // hence the timer should be set to arbitrry large values 

      //OBEX transport layer implementations may require that timeouts 
      // be disabled to support this operation - SPECIFICATION

      OI_OBEXCLI_SetResponseTimeout(
         (OI_OBEXCLI_CONNECTION_HANDLE)(conn_id),
          OI_MINUTES(OI_UINT16_MAX));
  }

  bt_ec_send_event( &event );
  
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_connect_ind_cb

DESCRIPTION
  Callback function invoked when an incoming connection from a remote BPP
  printer to the local referenced object server is received.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bpp_sndr_connect_ind_cb( 
  OI_OBEXSRV_CONNECTION_HANDLE conn_id, 
  OI_BD_ADDR*                  bd_addr_ptr )
{                    
  bt_ev_msg_type     event;
  bt_cmd_status_type status;

  if (bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF BPP CB RX: con ind, srv not reg, Conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  BT_MSG_DEBUG( "BT PF BPP CB RX, con ind, conn: %x", conn_id, 0, 0 );

  event.ev_hdr.ev_type = BT_EV_PF_BPP_SNDR_CON_IND;
  event.ev_hdr.bt_app_id = bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id;
  event.ev_msg.ev_bpp_sndr_con_ind.conn_id = 
    (bt_pf_bpp_sndr_conn_id_type)conn_id;

  if ( bd_addr_ptr == NULL )
  {
    memset( (void*)&(event.ev_msg.ev_bpp_sndr_con_ind.bd_addr),
            0,
            sizeof( bt_bd_addr_type ) ); 
  }
  else
  {
    memcpy( (void*)&(event.ev_msg.ev_bpp_sndr_con_ind.bd_addr),
            (void*)bd_addr_ptr,
            sizeof( bt_bd_addr_type ) );
  }

  bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].conn_id = 
      (bt_pf_bpp_sndr_conn_id_type)conn_id;

  /* clear sig first in case an earlier cfm was received after tmo expired */ 
  rex_clr_sigs( &bt_pf_tcb, BT_PF_BPP_SRV_SIG );
  bt_ec_send_event( &event );

  status = bt_pf_bpp_sndr_wait_for_srv_req_cfm( 
    (bt_pf_bpp_sndr_conn_id_type)conn_id, 
    BT_EV_PF_BPP_SNDR_CON_IND );

  if (status != OI_OK) /* not accepting */
  { 
    if ( status == OI_TIMEOUT )
    {
      status = OI_OBEX_CONNECTION_TIMEOUT;
    }
    bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].conn_id = BT_PF_BPP_NO_CONN_ID;
  }

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_disconnect_ind_cb

DESCRIPTION
  Callback function invoked to indicate a connection to a remote BPP printer 
  has been terminated.
    
===========================================================================*/
LOCAL void bt_pf_bpp_sndr_disconnect_ind_cb( 
  OI_OBEXSRV_CONNECTION_HANDLE conn_id )
{
  bt_ev_msg_type           event;
  bt_cmd_status_type       ret;
  bt_pf_bpp_sndr_conn_type conn_index;

  ret = bt_pf_bpp_sndr_find_job_status_conn( conn_id, &conn_index );

  if ( (ret != OI_OK) || (conn_index > OBJECT_CHANNEL) )
  { 
    BT_ERR( "BT PF BPP CB RX: dcn, unknown conn: %x", 
            conn_id, 0, 0 );
    return;
  }

  BT_MSG_DEBUG( "BT PF BPP CB RX: dcn, conn: %x", conn_id, 0, 0 );

  event.ev_hdr.bt_app_id = bt_pf_bpp_sndr_conn_info[conn_index].app_id;
  event.ev_hdr.ev_type = BT_EV_PF_BPP_SNDR_DCN_IND;
  event.ev_msg.ev_bpp_sndr_dcn_ind.conn_id = 
    (bt_pf_bpp_sndr_conn_id_type)conn_id;

  bt_pf_bpp_sndr_conn_info[conn_index].app_id = BT_PF_APP_ID_NULL;
  bt_pf_bpp_sndr_conn_info[conn_index].target = BT_PF_BPP_NO_TARGET;
  bt_pf_bpp_sndr_conn_info[conn_index].conn_id = BT_PF_BPP_NO_CONN_ID;

  bt_ec_send_event( &event );

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_ref_obj_srv_disconnect_ind_cb

DESCRIPTION
  Callback function invoked to indicate an incoming connection to the local 
  referenced object server from a remote BPP printer has been terminated
  
===========================================================================*/
LOCAL void bt_pf_bpp_sndr_srv_disconnect_ind_cb( 
  OI_OBEXSRV_CONNECTION_HANDLE conn_id )
{
  bt_ev_msg_type        event;  

  if ( bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].conn_id != 
         (bt_pf_bpp_sndr_conn_id_type)conn_id )
  { 
    BT_ERR( "BT PF BPP CB RX: srv dcn, unknown conn: %x", 
            conn_id, 0, 0 );
    return;
  }

  BT_MSG_DEBUG( "BT PF BPP CB RX: srv dcn, conn: %x", conn_id, 0, 0 );

  event.ev_hdr.bt_app_id = bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id;
  event.ev_hdr.ev_type = BT_EV_PF_BPP_SNDR_SRV_DCN_IND;
  event.ev_msg.ev_bpp_sndr_srv_dcn_ind.conn_id = 
    (bt_pf_bpp_sndr_conn_id_type)conn_id;

  bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].conn_id = BT_PF_BPP_NO_CONN_ID;

  bt_ec_send_event( &event );

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_auth_req_cb

DESCRIPTION
  Callback function invoked when the BPP printer requests for authentication
  during a connection.

===========================================================================*/
LOCAL void bt_pf_bpp_sndr_auth_req_cb( 
  OI_OBEXSRV_CONNECTION_HANDLE conn_id, 
  OI_BOOL                      user_id_required )
{
  bt_ev_msg_type           event; 
  bt_cmd_status_type       ret;
  bt_pf_bpp_sndr_conn_type conn_index;
  
  ret = bt_pf_bpp_sndr_find_job_status_conn( conn_id, &conn_index );

  if ( (ret != OI_OK) || (conn_index > OBJECT_CHANNEL) )
  { 
    BT_ERR( "BT PF BPP CB RX: auth, unknown conn: %x", 
            conn_id, 0, 0 );
    return;
  }

  BT_MSG_DEBUG( "BT PF BPP CB RX: auth, conn: %x, trg %x", 
                conn_id, bt_pf_bpp_sndr_conn_info[conn_index].target, 0 );

  event.ev_hdr.bt_app_id = bt_pf_bpp_sndr_conn_info[conn_index].app_id;
  event.ev_hdr.ev_type = BT_EV_PF_BPP_SNDR_AUTH_REQ;
  event.ev_msg.ev_bpp_sndr_auth_req.target = 
    bt_pf_bpp_sndr_conn_info[conn_index].target;
  event.ev_msg.ev_bpp_sndr_auth_req.user_id_required = user_id_required;
  
  bt_ec_send_event( &event );
  
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_send_file_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to a send file request is received.

===========================================================================*/
LOCAL void bt_pf_bpp_sndr_send_file_cfm_cb( 
  OI_OBEXSRV_CONNECTION_HANDLE conn_id, 
  OI_STATUS                    status )
{
  bt_ev_msg_type        event;

  if ( bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].conn_id != 
         (bt_pf_bpp_sndr_conn_id_type)conn_id )
  { 
    BT_ERR( "BT PF BPP CB RX: snd file, unknown conn: %x", 
            conn_id, 0, 0 );
    return;
  }

  BT_MSG_DEBUG( "BT PF BPP CB RX: snd file, conn: %x, stat %x", 
                conn_id, status, 0 );

  event.ev_hdr.bt_app_id = bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].app_id;
  event.ev_hdr.ev_type = BT_EV_PF_BPP_SNDR_SEND_FILE_CFM;
  event.ev_msg.ev_bpp_sndr_send_file_cfm.conn_id = 
    (bt_pf_bpp_sndr_conn_id_type)conn_id;
  event.ev_msg.ev_bpp_sndr_send_file_cfm.status = 
    (bt_cmd_status_type)status;

  bt_ec_send_event( &event );

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_send_reference_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to a send file request is received.
  
  Caller must free the memory used for storing http challenge and url.

===========================================================================*/
LOCAL void bt_pf_bpp_sndr_send_reference_cfm_cb( 
  OI_OBEXSRV_CONNECTION_HANDLE conn_id,
  OI_OBEX_BYTESEQ*             url_ptr,
  OI_OBEX_BYTESEQ*             http_challenge_ptr, 
  OI_STATUS                    status )
{
  bt_ev_msg_type event;
  byte*          url_str_ptr;
  byte*          http_challenge_str_ptr;

  if ( bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].conn_id != 
         (bt_pf_bpp_sndr_conn_id_type)conn_id )
  { 
    BT_ERR( "BT PF BPP CB RX: snd ref, unknown conn: %x", 
            conn_id, 0, 0 );
    return;
  }

  BT_MSG_DEBUG( "BT PF BPP CB RX: snd ref, conn: %x, stat %x", 
                conn_id, status, 0 );

  if (url_ptr == NULL)
  {
    event.ev_msg.ev_bpp_sndr_send_reference_cfm.url_len = 0;
    event.ev_msg.ev_bpp_sndr_send_reference_cfm.url_ptr = NULL;
  }
  else
  {
    url_str_ptr = (byte*)mem_malloc( &bt_pf_heap, url_ptr->len );
    memcpy( (void*)url_str_ptr, 
            (void*)url_ptr->data,
            url_ptr->len );
    event.ev_msg.ev_bpp_sndr_send_reference_cfm.url_len = url_ptr->len;
    event.ev_msg.ev_bpp_sndr_send_reference_cfm.url_ptr = url_str_ptr;
  }
  if (http_challenge_ptr == NULL)
  {        
    event.ev_msg.ev_bpp_sndr_send_reference_cfm.http_challenge_len = 0;
    event.ev_msg.ev_bpp_sndr_send_reference_cfm.http_challenge_ptr = NULL;
  }
  else
  {
    http_challenge_str_ptr = (byte*)mem_malloc( &bt_pf_heap, 
                                                http_challenge_ptr->len );
    memcpy( (void*)http_challenge_str_ptr, 
            (void*)http_challenge_ptr->data,
            http_challenge_ptr->len );
    event.ev_msg.ev_bpp_sndr_send_reference_cfm.http_challenge_len = 
      http_challenge_ptr->len;
    event.ev_msg.ev_bpp_sndr_send_reference_cfm.http_challenge_ptr = 
      http_challenge_str_ptr;
  }

  event.ev_hdr.bt_app_id = bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].app_id;
  event.ev_hdr.ev_type = BT_EV_PF_BPP_SNDR_SEND_REFERENCE_CFM;
  event.ev_msg.ev_bpp_sndr_send_reference_cfm.conn_id = 
    (bt_pf_bpp_sndr_conn_id_type)conn_id;
  event.ev_msg.ev_bpp_sndr_send_reference_cfm.status = 
    (bt_cmd_status_type)status;

  bt_ec_send_event( &event );

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_get_cfm_cb

DESCRIPTION
  Callback function invoked when a respond to a SOAP request is received.

===========================================================================*/
LOCAL void bt_pf_bpp_sndr_get_cfm_cb( 
  OI_OBEXSRV_CONNECTION_HANDLE conn_id,
  OI_OBEX_BYTESEQ*             reply_ptr,
  OI_BPP_JOB_ID*               job_id_ptr, 
  OI_STATUS                    status,
  OI_BOOL                      final ) 
{
  bt_ev_msg_type           event;
  bt_cmd_status_type       ret;
  bt_pf_bpp_sndr_conn_type conn_index;

  ret = bt_pf_bpp_sndr_find_job_status_conn( conn_id, &conn_index );

  if ( (ret != OI_OK) || (conn_index > OBJECT_CHANNEL) )
  { 
    BT_ERR( "BT PF BPP CB RX: get cfm, unknown conn: %x", 
            conn_id, 0, 0 );
    return;
  }

  BT_MSG_DEBUG( "BT PF BPP CB RX: get cfm, conn: %x, stat %x", 
                conn_id, status, 0 );

  event.ev_hdr.bt_app_id = bt_pf_bpp_sndr_conn_info[conn_index].app_id;
  event.ev_hdr.ev_type = BT_EV_PF_BPP_SNDR_GET_CFM;
  event.ev_msg.ev_bpp_sndr_get_cfm.conn_id = 
    (bt_pf_bpp_sndr_conn_id_type)conn_id;

  if ( reply_ptr == NULL )
  {
    event.ev_msg.ev_bpp_sndr_get_cfm.reply_ptr = NULL;
    event.ev_msg.ev_bpp_sndr_get_cfm.reply_len = 0;
  }
  else
  {
    event.ev_msg.ev_bpp_sndr_get_cfm.reply_ptr = reply_ptr->data;
    event.ev_msg.ev_bpp_sndr_get_cfm.reply_len = reply_ptr->len;
  }

  if ( job_id_ptr == NULL )
  {
    event.ev_msg.ev_bpp_sndr_get_cfm.job_id = 0;
  }
  else
  {
    event.ev_msg.ev_bpp_sndr_get_cfm.job_id = 
      (bt_pf_bpp_job_id_type)(*job_id_ptr);
  }

  event.ev_msg.ev_bpp_sndr_get_cfm.status = 
    (bt_cmd_status_type)status;

  event.ev_msg.ev_bpp_sndr_get_cfm.final = final;

  /* clear sig first in case an earlier cfm was received after tmo expired */ 
  rex_clr_sigs( &bt_pf_tcb, BT_PF_BPP_CLI_SIG );
  bt_ec_send_event( &event );

  /* wait for data to be consumed */
  bt_pf_bpp_sndr_wait_for_get_rsp_cfm( (bt_pf_bpp_sndr_conn_id_type)conn_id );

  return;
}


/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_get_ref_obj_cfm_cb

DESCRIPTION
  Callback function invoked to indicate that a connected remote BPP printer
  is requesting a referenced object.

===========================================================================*/
LOCAL OI_STATUS bt_pf_bpp_sndr_get_ref_obj_ind_cb( 
  OI_OBEXSRV_CONNECTION_HANDLE conn_id,
  OI_OBEX_UNICODE*             name_ptr,
  OI_UINT32                    offset,
  OI_INT32                     count,
  OI_BOOL                      get_file_size, 
  OI_STATUS                    status )
{
  bt_ev_msg_type     event;
  bt_cmd_status_type ret;
  uint8              name_len;

  if ( bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].conn_id != 
         (bt_pf_bpp_sndr_conn_id_type)conn_id )
  { 
    BT_ERR( "BT PF BPP CB RX: get ref obj, unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  BT_MSG_DEBUG( "BT PF BPP CB RX: get ref obj, conn: %x, stat %x", 
                conn_id, status, 0 );

  event.ev_hdr.bt_app_id = bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id;
  event.ev_hdr.ev_type = BT_EV_PF_BPP_SNDR_GET_REF_OBJ_IND;
  event.ev_msg.ev_bpp_sndr_get_ref_obj_ind.conn_id = 
    (bt_pf_bpp_sndr_conn_id_type)conn_id;
  event.ev_msg.ev_bpp_sndr_get_ref_obj_ind.offset = 
    offset;
  event.ev_msg.ev_bpp_sndr_get_ref_obj_ind.count = 
    count;
  event.ev_msg.ev_bpp_sndr_get_ref_obj_ind.get_file_size = 
    get_file_size;
  event.ev_msg.ev_bpp_sndr_get_ref_obj_ind.status = 
    (bt_cmd_status_type)status;

  if ( (name_ptr == NULL) || (name_ptr->len == 0) )
  {
    name_len = 0;
    event.ev_msg.ev_bpp_sndr_get_ref_obj_ind.name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_bpp_sndr_get_ref_obj_ind.name_str,
      (bt_pf_unicode_str_type*)name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_bpp_sndr_get_ref_obj_ind.name_len = name_len;

  /* clear sig first in case an earlier cfm was received after tmo expired */ 
  rex_clr_sigs( &bt_pf_tcb, BT_PF_BPP_SRV_SIG );
  bt_ec_send_event( &event );

  /* wait for status from app and report to OBEX client (BPP printer) */
  ret = bt_pf_bpp_sndr_wait_for_srv_req_cfm( 
    (bt_pf_bpp_sndr_conn_id_type)conn_id, 
    BT_EV_PF_BPP_SNDR_GET_REF_OBJ_IND );

  if ( ret == OI_TIMEOUT )
  {
    ret = OI_OBEX_INTERNAL_SERVER_ERROR;
  }

  return(ret);
}


LOCAL OI_BPP_SENDER_CALLBACKS bt_pf_bpp_sndr_ref_obj_srv_cb_list =
{
  bt_pf_bpp_sndr_connect_ind_cb,
  bt_pf_bpp_sndr_srv_disconnect_ind_cb,
  bt_pf_bpp_sndr_get_ref_obj_ind_cb
};


/*===========================================================================

FUNCTION
  bt_pf_bpp_ev_cb

DESCRIPTION
  Event callback function for BPP.
  
===========================================================================*/
LOCAL void bt_pf_bpp_ev_cb( struct bt_ev_msg_struct* ev_msg_ptr )
{ 
  OI_STATUS                   status = OI_OK;
  bt_pf_bpp_sndr_conn_id_type conn_id;
  bt_ev_msg_type              event;

  switch (ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_GN_CMD_DONE:
      BT_MSG_DEBUG( "BT PF BPP: Cmd Done, Status: %x, App: %x, Cmd: %x", 
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.bt_app_id,
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type );
      break;

    case BT_EV_SD_SECONDARY_SERVER_CHANNEL_NUMBER_RESP:
      if ( ( bt_pf_bpp_cmd_in_prog == 
               BT_CMD_SD_GET_SECONDARY_SERVER_CHANNEL_NUMBER ) &&
           ( ev_msg_ptr->ev_msg.ev_sd_scn_resp.service_class == 
             BT_SD_SERVICE_CLASS_PRINTING_STATUS ) &&
           ( BT_BD_ADDRS_EQUAL( &ev_msg_ptr->ev_msg.ev_sd_scn_resp.bd_addr,
                                &bt_pf_bpp_bd_addr ) ) )
      {
        status = OI_BPP_SENDER_Connect(
          (OI_BD_ADDR*)&bt_pf_bpp_bd_addr,
          ev_msg_ptr->ev_msg.ev_sd_scn_resp.scn,
          (OI_BPP_TARGET)BT_PF_BPP_TARGET_STS,
          bt_pf_bpp_sndr_connect_cfm_cb,
          bt_pf_bpp_sndr_disconnect_ind_cb,
          bt_pf_bpp_sndr_auth_req_cb,
          (OI_OBEXCLI_CONNECTION_HANDLE*)&conn_id );

        bt_pf_bpp_cmd_in_prog = BT_CMD_NULL;
        bt_pf_bpp_bd_addr = bt_pf_bpp_null_bd_addr;

        if ( status != OI_OK )
        {
          event.ev_hdr.bt_app_id = 
            bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].app_id;
          event.ev_hdr.ev_type = BT_EV_PF_BPP_SNDR_CON_CFM;
          event.ev_msg.ev_bpp_sndr_con_cfm.target = BT_PF_BPP_TARGET_STS;
          event.ev_msg.ev_bpp_sndr_con_cfm.conn_id = BT_PF_BPP_NO_CONN_ID;
          event.ev_msg.ev_bpp_sndr_con_cfm.status = status;

          bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].app_id = BT_PF_APP_ID_NULL;
          bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].target = BT_PF_BPP_NO_TARGET;

          bt_ec_send_event( &event );
        }
        else
        {
          bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].conn_id = conn_id;
          bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].target = BT_PF_BPP_TARGET_STS;
        }
      }
      break;

    case BT_EV_SD_ERROR_RESP:
      if ( ( bt_pf_bpp_cmd_in_prog == 
               BT_CMD_SD_GET_SECONDARY_SERVER_CHANNEL_NUMBER ) &&
           ( BT_BD_ADDRS_EQUAL( &ev_msg_ptr->ev_msg.ev_sd_error_resp.bd_addr,
                                &bt_pf_bpp_bd_addr ) ) )
      { 
        bt_pf_bpp_cmd_in_prog = BT_CMD_NULL;
        bt_pf_bpp_bd_addr = bt_pf_bpp_null_bd_addr;

        event.ev_hdr.bt_app_id = 
          bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].app_id;
        event.ev_hdr.ev_type = BT_EV_PF_BPP_SNDR_CON_CFM;
        event.ev_msg.ev_bpp_sndr_con_cfm.target = BT_PF_BPP_TARGET_STS;
        event.ev_msg.ev_bpp_sndr_con_cfm.conn_id = BT_PF_BPP_NO_CONN_ID;
        event.ev_msg.ev_bpp_sndr_con_cfm.status = 
          (bt_cmd_status_type)ev_msg_ptr->ev_msg.ev_sd_error_resp.error_code;

        bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].app_id = BT_PF_APP_ID_NULL;
        bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].target = BT_PF_BPP_NO_TARGET;

        bt_ec_send_event( &event );
      }
      break;

    default:
      BT_MSG_DEBUG( "BT PF BPP: unknown event type %x", 
              ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_cmd_connect

DESCRIPTION
  Performs connection to remote BPP printer.

===========================================================================*/  
bt_cmd_status_type bt_pf_bpp_sndr_cmd_connect(
  bt_app_id_type                   app_id,
  bt_pf_cmd_bpp_sndr_connect_type* params_ptr )
{                 
  OI_STATUS                   status = OI_OK;
  bt_pf_bpp_sndr_conn_id_type conn_id;
  bt_pf_bpp_sndr_conn_type    conn_type;
  bt_bd_addr_type             bd_addr = params_ptr->bd_addr;

  if ( (params_ptr->target == BT_PF_BPP_TARGET_DPS) || 
       (params_ptr->target == BT_PF_BPP_TARGET_PBR) )
  {
    conn_type = JOB_CHANNEL;
  }
  else if (params_ptr->target == BT_PF_BPP_TARGET_STS)
  {
    conn_type = STATUS_CHANNEL;
  }
  else
  {
    BT_ERR( "BT PF BPP SNDR CON: invalid target, App: %x", app_id, 0, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  TASKLOCK();
  if (bt_pf_bpp_sndr_conn_info[conn_type].app_id == BT_PF_APP_ID_NULL)
  {                                                   
    bt_pf_bpp_sndr_conn_info[conn_type].app_id = app_id;
  }
  else
  {
    status = OI_STATUS_ALREADY_CONNECTED;
  } 
  TASKFREE();

  if (status != OI_OK)
  {
    BT_MSG_DEBUG( "BT PF BPP SNDR CON: alrdy conn, App: %x",
                  app_id, 0, 0 );
    return(status);
  }

  if ( conn_type == JOB_CHANNEL )
  {                             
    status = OI_BPP_SENDER_Connect(
      (OI_BD_ADDR*)&bd_addr,
      params_ptr->channel,
      (OI_BPP_TARGET)(params_ptr->target),
      bt_pf_bpp_sndr_connect_cfm_cb,
      bt_pf_bpp_sndr_disconnect_ind_cb,
      bt_pf_bpp_sndr_auth_req_cb,
      (OI_OBEXCLI_CONNECTION_HANDLE*)&conn_id );
  
    if ( status != OI_OK )
    {
      bt_pf_bpp_sndr_conn_info[conn_type].app_id = BT_PF_APP_ID_NULL;
    }
    else
    {
      bt_pf_bpp_sndr_conn_info[conn_type].conn_id = conn_id;
      bt_pf_bpp_sndr_conn_info[conn_type].target = params_ptr->target;
    }
  }
  else /* STATUS_CHANNEL */
  {
    status = bt_cmd_sd_get_secondary_server_channel_number( 
               bt_pf_bpp_app_id,
               &bd_addr,
               BT_SD_SERVICE_CLASS_PRINTING_STATUS );

    if ( status != BT_CS_GN_PENDING )
    {
      bt_pf_bpp_sndr_conn_info[conn_type].app_id = BT_PF_APP_ID_NULL;
    }
    else
    {
      bt_pf_bpp_cmd_in_prog = BT_CMD_SD_GET_SECONDARY_SERVER_CHANNEL_NUMBER;
      bt_pf_bpp_sndr_conn_info[conn_type].target = BT_PF_BPP_TARGET_STS;
      bt_pf_bpp_bd_addr = params_ptr->bd_addr;
    }
  }

  return(status);
}


/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_cmd_connect

DESCRIPTION
  Performs disconnection from remote BPP printer.

===========================================================================*/  
bt_cmd_status_type bt_pf_bpp_sndr_cmd_disconnect(
  bt_app_id_type                      app_id,
  bt_pf_cmd_bpp_sndr_disconnect_type* params_ptr )
{
  OI_STATUS                status;
  bt_pf_bpp_sndr_conn_type conn_index;
  bt_cmd_status_type       ret;

  ret = bt_pf_bpp_sndr_find_job_status_conn( params_ptr->conn_id, &conn_index );

  if ( (ret != OI_OK) || (conn_index > OBJECT_CHANNEL) || 
       (bt_pf_bpp_sndr_conn_info[conn_index].app_id != app_id) )
  {  
    BT_ERR( "BT PF BPP DCN: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  status = OI_BPP_SENDER_Disconnect( 
    (OI_OBEXCLI_CONNECTION_HANDLE)(params_ptr->conn_id) );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_auth_rsp

DESCRIPTION
  Responses to a authentication challenge from a BPP printer.

===========================================================================*/  
bt_cmd_status_type bt_pf_bpp_sndr_cmd_auth_rsp(
  bt_app_id_type                    app_id,
  bt_pf_cmd_bpp_sndr_auth_rsp_type* params_ptr )
{
  OI_STATUS                   status;
  uint16                      conn_index;
  OI_BYTE*                    user_id_ptr;
  OI_CHAR*                    password_ptr;

  for (conn_index = 0; conn_index < OBJECT_CHANNEL; conn_index++)
  {
    if (bt_pf_bpp_sndr_conn_info[conn_index].target == params_ptr->target)
    { 
      break;
    }
  }

  if ( (conn_index >= OBJECT_CHANNEL) ||
       (bt_pf_bpp_sndr_conn_info[conn_index].app_id != app_id) )
  {  
    BT_ERR( "BT PF BPP AUTH RSP: App: %x, invalid target: %x", 
            app_id, params_ptr->target, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  if (params_ptr->user_id_len == 0 )
  {
    user_id_ptr = NULL;
  }
  else
  {
    user_id_ptr = (OI_BYTE*)params_ptr->user_id;
  }

  if (params_ptr->password_str[0] == '\0')
  {
    password_ptr = NULL;
  }
  else
  {
    password_ptr = (OI_CHAR*)(params_ptr->password_str);
  }

  status = OI_OBEXCLI_AuthenticationResponse( 
    (OI_OBEXCLI_CONNECTION_HANDLE)
      (bt_pf_bpp_sndr_conn_info[conn_index].conn_id),
    user_id_ptr,
    params_ptr->user_id_len,
    password_ptr );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_cmd_send_file

DESCRIPTION
  Performs a FilePush or SendDocument operation towards remote BPP printer.

===========================================================================*/  
bt_cmd_status_type bt_pf_bpp_sndr_cmd_send_file(
  bt_app_id_type                     app_id,
  bt_pf_cmd_bpp_sndr_send_file_type* params_ptr )
{
  OI_STATUS              status;
  OI_OBEX_BYTESEQ        type;
  OI_OBEX_BYTESEQ*       type_ptr;
  OI_OBEX_UNICODE        name;
  OI_OBEX_UNICODE*       name_ptr;
  uint16                 name_str[BT_PF_MAX_FILENAME_LEN + 1];
  OI_OBEX_UNICODE        desc;
  OI_OBEX_UNICODE*       desc_ptr;
  OI_BPP_JOB_ID          job_id = (OI_BPP_JOB_ID)params_ptr->job_id;
  OI_BPP_JOB_ID*         job_id_ptr;
  OI_OBEX_BYTESEQ        data = *(OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);

  if ( (params_ptr->conn_id != 
          bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].conn_id) ||
       (app_id != bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].app_id) )
  {  
    BT_ERR( "BT PF BPP SND FILE: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  if (bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].target == BPP_TARGET_PBR)
  { 
    /* FilePush and SendDocument are DPS operations */
    BT_ERR( "BT PF BPP SND FILE: PBR op on DPS target, App: %x, Conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_STATUS_INVALID_COMMAND);
  }

  if (params_ptr->type_str[0] == 0)
  {
    type_ptr = NULL;
  }
  else
  {
    type.len  = (OI_UINT16)strlen( (char *)params_ptr->type_str ) + 1;
    type.data = (OI_BYTE*)(params_ptr->type_str);
    type_ptr = &type;
  }

  if (params_ptr->name_len == 0)
  {
    name_ptr = NULL;
  }
  else
  {
    /* handle is copied to avoid misaligned access when str pointer is not 
       aligned */
    memcpy( (void*)name_str,
            (void*)params_ptr->name_str,
            params_ptr->name_len * sizeof( uint16 ) );
    name.len = params_ptr->name_len;
    name.str = name_str;
    name_ptr = &name;
  }

  if (params_ptr->description_ptr == NULL)
  {
    desc_ptr = NULL;
  }
  else
  {
    desc.len = bt_pf_bpp_ustrlen( params_ptr->description_ptr );
    desc.str = params_ptr->description_ptr;
    desc_ptr = &desc;
  }

  if (job_id == 0)
  { 
    job_id_ptr = NULL;
  }
  else
  {
    job_id_ptr = &job_id;
  }

  status = OI_BPP_SENDER_SendFile(
    (OI_OBEXCLI_CONNECTION_HANDLE)(params_ptr->conn_id),
    bt_pf_bpp_sndr_send_file_cfm_cb,
    type_ptr,
    desc_ptr,
    name_ptr,
    job_id_ptr,
    &data,
    params_ptr->final );

  return(status);
} 

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_cmd_send_reference

DESCRIPTION
  Performs a SimpleReferencePush or SendReference operation towards remote 
  BPP printer.

===========================================================================*/  
bt_cmd_status_type bt_pf_bpp_sndr_cmd_send_reference(
  bt_app_id_type                          app_id,
  bt_pf_cmd_bpp_sndr_send_reference_type* params_ptr )
{
  OI_STATUS       status;
  OI_BPP_JOB_ID   job_id = (OI_BPP_JOB_ID)params_ptr->job_id;
  OI_BPP_JOB_ID*  job_id_ptr;
  OI_OBEX_BYTESEQ data = *(OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  OI_OBEX_BYTESEQ http_headers = 
                    *(OI_OBEX_BYTESEQ*)(params_ptr->http_headers_ptr);

  if ( (params_ptr->conn_id != 
          bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].conn_id) ||
       (app_id != bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].app_id) )
  {  
    BT_ERR( "BT PF BPP SND FILE: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  if (bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].target == BPP_TARGET_DPS)
  { 
    /* SimpleReferencePush and SendReference are PBR operations */
    BT_ERR( "BT PF BPP SND FILE: DPS op on PBR target, App: %x, Conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_STATUS_INVALID_COMMAND);
  }

  if (job_id == 0)
  { 
    job_id_ptr = NULL;
  }
  else
  {
    job_id_ptr = &job_id;
  }

  status = OI_BPP_SENDER_SendReference(
    (OI_OBEXCLI_CONNECTION_HANDLE)(params_ptr->conn_id),
    bt_pf_bpp_sndr_send_reference_cfm_cb,
    (OI_BPP_REF_TYPE)(params_ptr->type),
    &http_headers,
    job_id_ptr,
    &data,
    params_ptr->final );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_cmd_soap_request

DESCRIPTION
  Performs SOAP request from remote BPP printer.

===========================================================================*/  
bt_cmd_status_type bt_pf_bpp_sndr_cmd_soap_request(
  bt_app_id_type                    app_id,
  bt_pf_cmd_bpp_sndr_soap_req_type* params_ptr )
{
  OI_STATUS status;
  
  if ( ( bt_pf_bpp_sndr_find_job_status_conn( params_ptr->conn_id, NULL ) != OI_OK )
       ||
       ( (bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].conn_id == params_ptr->conn_id)
         && (bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].app_id != app_id) )
       ||
       ( (bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].conn_id == params_ptr->conn_id)
         && (bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].app_id != app_id) ) )
  {
    BT_ERR( "BT PF BPP SOAP REQ: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  status = OI_BPP_SENDER_SOAPRequest(
    (OI_OBEXCLI_CONNECTION_HANDLE)(params_ptr->conn_id),
    bt_pf_bpp_sndr_get_cfm_cb,
    (OI_OBEX_BYTESEQ*)(params_ptr->body_ptr),
    params_ptr->final );

  return(status);
} 


/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_cmd_send_ref_obj_response

DESCRIPTION
  Performs SOAP request from remote BPP printer.

===========================================================================*/  
bt_cmd_status_type bt_pf_bpp_sndr_cmd_send_ref_obj_response(
  bt_app_id_type                           app_id,
  bt_pf_cmd_bpp_sndr_get_ref_obj_rsp_type* params_ptr )
{
  OI_STATUS       status;
  OI_INT32        file_size = params_ptr->file_size;
  OI_INT32*       file_size_ptr;
  OI_OBEX_BYTESEQ data = *(OI_OBEX_BYTESEQ*)(params_ptr->data_ptr);
  
  if ( (params_ptr->conn_id != 
          bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].conn_id) 
       || (app_id != bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id) )
  {  
    BT_ERR( "BT PF BPP REF OBJ RSP: App: %x, unknown conn: %x", 
            app_id, params_ptr->conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  if (file_size < 0)
  {
    file_size_ptr = NULL;
  }
  else
  {
    file_size_ptr = &file_size;
  }

  status = OI_BPP_SENDER_GetReferencedObjectsResponse(
    (OI_OBEXCLI_CONNECTION_HANDLE)(params_ptr->conn_id),
    &data,
    file_size_ptr,
    (OI_STATUS)(params_ptr->status) );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_cmd_register

DESCRIPTION
  Registers a referenced object server on the local BPP sender.
  
===========================================================================*/
bt_cmd_status_type bt_pf_bpp_sndr_cmd_register( 
  bt_app_id_type    app_id,
  char*             srv_name_ptr )
{
  OI_STATUS                status = OI_OK;
  uint8                    scn;

  bt_pf_bpp_app_id = bt_cmd_ec_get_app_id_and_register( bt_pf_bpp_ev_cb );

  TASKLOCK();

  if (bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id != BT_PF_APP_ID_NULL)
  {
    status = OI_STATUS_ALREADY_REGISTERED;
  }
  else
  {
    bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id = app_id;
  }

  TASKFREE();

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BPP REF OBJ SRV REG: alrdy reg, App: %x, Stat: %x", 
            app_id, status, 0 );
    return(status);
  }
  status = OI_BPP_SENDER_Register(NULL, 
    &bt_pf_bpp_sndr_ref_obj_srv_cb_list, 
    &scn,
    NULL,
    NULL );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BPP REF OBJ SRV REG: returns %x, App %x", 
            status, app_id, 0 );
    bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id = BT_PF_APP_ID_NULL;
    return(status);
  }

  /* Updates service record with additional profile specific attributes */
  status = bt_cmd_sd_update_obex_profile_service_record(
             app_id,
             BT_SD_SERVICE_CLASS_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE,
             scn,
             srv_name_ptr,
             NULL, /* service ID */
             NULL, /* supported formats list */
             0,    /* supported capabilities */
             0,    /* supported features */
             0,    /* supported functions */
             0 );  /* imaging capacity */
  
  if (status != BT_CS_GN_PENDING)
  {
    BT_ERR( "BT PF BPP REF OBJ SRV REG: update srv rec failed, App %x", 
            app_id, 0, 0 );
    OI_BPP_SENDER_Deregister();
    return( status );
  }    

  bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].target = BT_PF_BPP_REF_OBJ;

  return( OI_OK );
}

/*===========================================================================

FUNCTION
  bt_pf_bpp_sndr_cmd_deregister

DESCRIPTION
  Deregisters a referenced object server on the local BPP sender.

===========================================================================*/
bt_cmd_status_type bt_pf_bpp_sndr_cmd_deregister( bt_app_id_type    app_id )
{
  OI_STATUS status;

  if (bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id == BT_PF_APP_ID_NULL)
  {
    BT_MSG_DEBUG( "BT PF BPP REF OBJ SRV DEREG: not reg, App: %x, Stat: %x",
                  app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }

  if (bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id != app_id)
  {
    BT_MSG_DEBUG( "BT PF BPP REF OBJ SRV DEREG: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }
  
  status = OI_BPP_SENDER_Deregister();

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BPP REF OBJ SRV DEREG: returns %x, App: %x", 
            status, app_id, 0 );
  }
    
  if ( status != OI_OBEX_OPERATION_IN_PROGRESS )
  {                                           
    bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id = BT_PF_APP_ID_NULL;
    bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].target = BT_PF_BPP_NO_TARGET;

    if ( bt_pf_bpp_app_id != BT_APP_ID_NULL )
    {
      bt_cmd_ec_free_application_id( bt_pf_bpp_app_id );
    }
  }

  return(status);
}


/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_get_rsp_cfm

DESCRIPTION
  This command is called by the application to confirm that a get response 
  event and the accompanying data has been processed. 
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/

bt_cmd_status_type bt_cmd_pf_bpp_sndr_get_rsp_cfm(
  bt_app_id_type              app_id,
  bt_pf_bpp_sndr_conn_id_type conn_id )
{
  if ( ( bt_pf_bpp_sndr_find_job_status_conn( conn_id, NULL ) != OI_OK )
       ||
       ( (bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].conn_id == conn_id)
         && (bt_pf_bpp_sndr_conn_info[JOB_CHANNEL].app_id != app_id) )
       ||
       ( (bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].conn_id == conn_id)
         && (bt_pf_bpp_sndr_conn_info[STATUS_CHANNEL].app_id != app_id) ) )
  {
    BT_ERR( "BT PF BPP GET RSP CFM: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  BT_MSG_API( "BT PF BPP GET RSP CFM: App: %x, Conn: %x", 
              app_id, conn_id, 0 );
  rex_set_sigs( &bt_pf_tcb, BT_PF_BPP_CLI_SIG );

  return(OI_OK);
}
/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_srv_req_cfm

DESCRIPTION
  This command is called by the application to confirm that a request event 
  is received. The application should supply an OBEX status code which will
  eventually be reported to the remote BPP printer. 
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_srv_req_cfm(
  bt_app_id_type              app_id,
  bt_pf_bpp_sndr_conn_id_type conn_id,
  bt_event_type               event,
  bt_cmd_status_type          status )
{
  if ( (bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].conn_id != conn_id)
         || (bt_pf_bpp_sndr_conn_info[OBJECT_CHANNEL].app_id != app_id) )
  {
    BT_ERR( "BT PF BPP SRV REQ CFM: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  if ( (event != BT_EV_PF_BPP_SNDR_CON_IND) && 
       (event != BT_EV_PF_BPP_SNDR_GET_REF_OBJ_IND) )
  {
    BT_ERR( "BT PF BPP SRV REQ CFM: App: %x, Conn: %x, unknown Ev: %x", 
            app_id, conn_id, event );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  BT_MSG_API( "BT PF BPP SRV REQ CFM: App: %x, Conn: %x, Ev: %x", 
              app_id, conn_id, event );
  bt_pf_bpp_sndr_srv_req_cfm.event = event;
  bt_pf_bpp_sndr_srv_req_cfm.status = status;
  rex_set_sigs( &bt_pf_tcb, BT_PF_BPP_SRV_SIG );

  return(OI_OK);
}
/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_abort

DESCRIPTION
  Terminates the current BPP operation.
  
  This command is returned immediately and is not placed on the command queue.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_bpp_sndr_abort(
  bt_app_id_type                app_id,
  bt_pf_bpp_sndr_conn_id_type   conn_id )
{
  OI_STATUS status;

  BT_MSG_API( "BT PF BPP SNDR ABORT: App: %x, Conn: %x", 
              app_id, conn_id, 0 );
  status = OI_BPP_SENDER_Cancel ( (OI_OBEXCLI_CONNECTION_HANDLE)conn_id, NULL );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BPP SNDR ABORT: OI_BPP_SENDER_Abort returns %x", 
                status, 0, 0 );
  }
  return(status);
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_bpp_sndr_force_disconnect

DESCRIPTION
  Terminate connection from printer to BPP server (object channel).

===========================================================================*/
bt_cmd_status_type bt_pf_bpp_sndr_cmd_force_disconnect(
  bt_app_id_type                              app_id,
  bt_pf_cmd_bpp_sndr_force_disconnect_type*   params_ptr)
{
  OI_STATUS status;

  BT_MSG_API( "BT PF BPP SNDR Force DCN: App: %x, Conn: %x", 
               app_id, params_ptr->conn_id, 0 );

  status = OI_BPP_SENDER_ForceDisconnect(
               (OI_OBEXSRV_CONNECTION_HANDLE)params_ptr->conn_id);

  if (status != OI_OK)
  {
    BT_ERR( "BT PF BPP SNDR Force DCN: OI_BPP_SENDER_ForceDisconnect returns %x", 
                status, 0, 0 );
  }
  return(status);
}
#endif /* FEATURE_BT_EXTPF_BPP */
#endif /* FEATURE_BT */
