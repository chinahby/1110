/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               B L U E T O O T H   G E N E R I C   O B J E C T    
          
                      E X C H A N G E   P R O F I L E

GENERAL DESCRIPTION
  This module contains the Bluetooth Generic Object Exchange Profile data and 
  code which are not client or server specific.

Copyright (c) 2004-2009 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfgoep1.c#2 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-06-03   gs  Modified bt_pfgoep_reassemble_pkt() for handling asynchronous
                   Abort packet.

$Log: $
*    #23        28 Feb 2007           MH
* Added server disconnect response command
*
*    #22        26 Oct 2006           MH
* modified srv confirm set path to support all headers
*
*    #21      19 Sept 2006             MH
* Client Authentication response indication call back added
*
*    #19-20   10 Jul 2006             JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #18      23 Jun 2006             JH
* Added bt_cmd_pf_goep_free_header_list() to provide a mechanism for app to free
*  header list structure.
* Modified to support updated GOEP API for JSR82.
*
*    #17      28 Mar 2006             JH
* Removed extra semi-colon which leads to unintended error message.
*
*    #16      22 Oct 2005             JH
* Replaced FEATURE_PCAA with FEATURE_PCAA_SIRIUS.
*
*    #15      02 Sep 2005             RY
* Merged in Sirius changes for FEATURE_PCAA_SIRIUS for "+++" detection.
*
*    #14      31 May 2005             RY
* Merged in Sirius changes.
*
*    #13      16 Mar 2005             RY
* Fixed malformed comments.
*
*    #12      14 Mar 2005             JH
* Modified bt_pfgoep_reassemble_pkt() to ignore certain error conditions to
*  improve interoperability with other devices.
*
*    #11      01 Mar 2005             JH
* Added check in bt_pfgoep_reassemble_pkt() to ensure that length of received
*  packet <= the negotiated max value. 
*
*    #10      26 Jan 2005             JH
* Replaced OI debug message macros with BT equivalent.
*
*    #9       19 Jan 2005             JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #8        06 Jan 2005            JH
* Modified to avoid possible misaligned memory accesses.
*
*    #7        14 May 2004            JH
* Added bt_pf_goep_process_apit_data_event to support GOEP and SyncML APIT
*  testing
*
*    #4        14 Apr 2004            JH
* Made changes related to the replacement of bt_pf_unicode_str_type command 
* parameters with uint16* type
*
*    #2        25 Feb 2004            JH
* Renamed btpfgoep.h btpfgoepi.h
*

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_GOEP

#define BT_MSG_LAYER  BT_MSG_GN                         
#include "btpfi.h"
#include "btpfcmdi.h"
#include "btmsg.h"
#include "bti.h"
#include "btpfgoepi.h"
#include "memheap.h"
#include "obexcommon.c"    
#ifdef FEATURE_MT_OBEX
#include "oi_rfcomm_internal.h"
#endif /* FEATURE_MT_OBEX */                                      


#define BT_PF_OBEX_ABORT_PKT_SIZE 3

extern OI_STATUS MapResponse(OI_UINT8 rspCode, OI_STATUS status);

/*===========================================================================

FUNCTION
  bt_pf_goep_oi_security    
   
DESCRIPTION
  Returns the corresponding OI defined security type.
   
===========================================================================*/
OI_SECURITY_POLICY bt_pf_goep_oi_security(bt_security_type security)
{
  OI_SECURITY_POLICY oi_security;
  switch(security)
  { 
    case BT_SEC_AUTHENTICATE:
      oi_security = OI_SEC_REQUIRE_AUTHENTICATION;
      break;

    case BT_SEC_AUTHENTICATE_AND_ENCRYPT:
      oi_security = OI_SEC_REQUIRE_ENCRYPTION;
      break;

    default:
    case BT_SEC_NONE:
      oi_security = OI_SEC_REQUIRE_NOTHING;
  }
  return(oi_security);
}


/*===========================================================================

FUNCTION
  bt_cmd_pf_goep_free_header_list    
   
DESCRIPTION
  Frees the header list data structure allocated by the profile layer.
  This command must be called when headers received has been processed by the 
  app.
  This command is not queued.
   
===========================================================================*/
bt_cmd_status_type bt_cmd_pf_goep_free_headers( 
  bt_app_id_type            bt_app_id,
  bt_pf_goep_obex_hdr_type* hdrs_ptr )
{
  BT_MSG_API( "BT PF GOEP CMD RX: free hdrs AID %x", 
              bt_app_id, 0, 0 );

  TASKLOCK();
  OI_FreeIf( &hdrs_ptr );
  TASKFREE();

  return( BT_CS_GN_SUCCESS );
}

/*===========================================================================

FUNCTION
  bt_pf_goep_process_command    
   
DESCRIPTION
  Processes the given command by calling the underlying OI OBEX driver.
   
===========================================================================*/
void bt_pf_goep_process_command( bt_pf_cmd_msg_type* cmd_ptr )
{
  OI_STATUS                    status;
  OI_OBEXSRV_CONNECTION_HANDLE conn_id;
  OI_OBEX_HEADER_LIST          headers_list;
  OI_BYTE*                     user_id_ptr;
  OI_CHAR*                     password_ptr;
  OI_CONNECT_POLICY            policy;
  bt_bd_addr_type              bd_addr;
  OI_OBEX_UNICODE              name;
  OI_OBEX_UNICODE*             name_ptr;
  uint16                       name_str[BT_PF_MAX_FILENAME_LEN + 1];

  switch ( cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_PF_CMD_GOEP_CLI_AUTH_RSP:
      BT_MSG_DEBUG( "BT PF GOEP CLI CMD RX: Auth Rsp", 0, 0, 0 );
      if (cmd_ptr->cmd_msg.cmd_goep_cli_auth_rsp.user_id_len == 0)
      {
        user_id_ptr = NULL;
      }
      else
      {
        user_id_ptr = (OI_BYTE*)cmd_ptr->cmd_msg.cmd_goep_cli_auth_rsp.user_id;
      }
      if (cmd_ptr->cmd_msg.cmd_goep_cli_auth_rsp.password_str[0] == '\0')
      {
        password_ptr = NULL;
      }
      else
      {
        password_ptr = (OI_CHAR*)
                       (cmd_ptr->cmd_msg.cmd_goep_cli_auth_rsp.password_str);
      }
      OI_OBEXCLI_AuthenticationResponse( 
        (OI_OBEXCLI_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_goep_cli_auth_rsp.conn_id,
        user_id_ptr,
        cmd_ptr->cmd_msg.cmd_goep_cli_auth_rsp.user_id_len,
        password_ptr );
      cmd_ptr->cmd_hdr.cmd_status = OI_OK;
      break;

    case BT_PF_CMD_GOEP_CLI_CONNECT:
      BT_MSG_DEBUG( "BT PF GOEP CLI CMD RX: Connect", 0, 0, 0 );
      bd_addr = cmd_ptr->cmd_msg.cmd_goep_cli_connect.bd_addr;
      policy.serviceUuid32 = (uint32)
                             (cmd_ptr->cmd_msg.cmd_goep_cli_connect.uuid);
      policy.securityPolicy = bt_pf_goep_oi_security(
                              cmd_ptr->cmd_msg.cmd_goep_cli_connect.security );
      policy.mustBeMaster = FALSE;
      policy.flowspec = NULL;
      policy.powerSavingDisables = 0;
      headers_list.list = (OI_OBEX_HEADER*)
        cmd_ptr->cmd_msg.cmd_goep_cli_connect.headers_ptr;
      headers_list.count = cmd_ptr->cmd_msg.cmd_goep_cli_connect.headers_cnt;
      status = OI_OBEXCLI_Connect(
        (OI_BD_ADDR*)&bd_addr,
        cmd_ptr->cmd_msg.cmd_goep_cli_connect.channel,
        ((headers_list.list != NULL) && (headers_list.count != 0)) ? 
          &headers_list : NULL,
        bt_pf_goep_cli_con_cfm_cb,
        bt_pf_goep_cli_dcn_ind_cb,
        bt_pf_goep_cli_auth_ind_cb,
        bt_pf_goep_cli_auth_rsp_ind_cb,
        (OI_OBEXCLI_CONNECTION_HANDLE*)&conn_id,
        (OI_CONNECT_POLICY*)&policy );
      cmd_ptr->cmd_hdr.cmd_status = status;
      if (status == OI_OK)
      {
        if ( ( bt_pf_goep_cli_app_conn_link( cmd_ptr->cmd_hdr.app_id, 
                 (bt_pf_goep_cli_conn_id_type)conn_id ) ) == 
             BT_PF_GOEP_CLI_APP_CONN_NOT_FOUND )
        {
          /* This should never happen since OI_OBEXCLI_Connect would have 
             failed if we area attempting to set up too many connections. */
          BT_ERR( "BT PF GOEP CLI: app conn table full, App: %x, Conn: %x", 
                  cmd_ptr->cmd_hdr.app_id, conn_id, 0);
        }
      }
      break;

    case BT_PF_CMD_GOEP_CLI_DISCONNECT:
      BT_MSG_DEBUG( "BT PF GOEP CLI CMD RX: Disconnect", 0, 0, 0 );
      headers_list.list = (OI_OBEX_HEADER*)
        cmd_ptr->cmd_msg.cmd_goep_cli_disconnect.headers_ptr;
      headers_list.count = cmd_ptr->cmd_msg.cmd_goep_cli_disconnect.headers_cnt;
      status = OI_OBEXCLI_Disconnect( 
        (OI_OBEXCLI_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_goep_cli_disconnect.conn_id,
        ((headers_list.list != NULL) && (headers_list.count != 0)) ? 
          &headers_list : NULL );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_GOEP_CLI_PUT:
      BT_MSG_DEBUG( "BT PF GOEP CLI CMD RX: Put", 0, 0, 0 );
      headers_list.list = (OI_OBEX_HEADER*)
        cmd_ptr->cmd_msg.cmd_goep_cli_put.headers_ptr;
      headers_list.count = cmd_ptr->cmd_msg.cmd_goep_cli_put.headers_cnt;
      status = OI_OBEXCLI_Put(
        (OI_OBEXCLI_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_goep_cli_put.conn_id,
        ((headers_list.list != NULL) && (headers_list.count != 0)) ? 
          &headers_list : NULL,
        bt_pf_goep_cli_put_cfm_cb,
        (OI_STATUS)cmd_ptr->cmd_msg.cmd_goep_cli_put.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_GOEP_CLI_GET:
      BT_MSG_DEBUG( "BT PF GOEP CLI CMD RX: Get", 0, 0, 0 );
      headers_list.list = (OI_OBEX_HEADER*)
        cmd_ptr->cmd_msg.cmd_goep_cli_get.headers_ptr;
      headers_list.count = cmd_ptr->cmd_msg.cmd_goep_cli_get.headers_cnt;
      status = OI_OBEXCLI_Get(
        (OI_OBEXCLI_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_goep_cli_get.conn_id,
        ((headers_list.list != NULL) && (headers_list.count != 0)) ? 
          &headers_list : NULL,
        bt_pf_goep_cli_get_recv_data_cb,
        cmd_ptr->cmd_msg.cmd_goep_cli_get.final );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_GOEP_CLI_SET_PATH:
      BT_MSG_DEBUG( "BT PF GOEP CLI CMD RX: SetPath", 0, 0, 0 );
      if (cmd_ptr->cmd_msg.cmd_goep_cli_set_path.folder_len == 0)
      {
        name_ptr = NULL;
      }
      else
      { 
        /* name is copied to avoid misaligned access when folder_str not 
           aligned */
        memcpy( (void*)name_str,
                (void*)cmd_ptr->cmd_msg.cmd_goep_cli_set_path.folder_str,
                cmd_ptr->cmd_msg.cmd_goep_cli_set_path.folder_len * 
                  sizeof( uint16 ) );
        name.len = cmd_ptr->cmd_msg.cmd_goep_cli_set_path.folder_len;
        name.str = name_str;
        name_ptr = &name;
      }
      headers_list.list = (OI_OBEX_HEADER*)
        cmd_ptr->cmd_msg.cmd_goep_cli_set_path.headers_ptr;
      headers_list.count = cmd_ptr->cmd_msg.cmd_goep_cli_set_path.headers_cnt;
      status = OI_OBEXCLI_SetPath(
        (OI_OBEXCLI_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_goep_cli_set_path.conn_id,
        name_ptr,
        cmd_ptr->cmd_msg.cmd_goep_cli_set_path.dont_create,
        cmd_ptr->cmd_msg.cmd_goep_cli_set_path.up_level,
        ((headers_list.list != NULL) && (headers_list.count != 0)) ? 
          &headers_list : NULL,
        bt_pf_goep_cli_set_path_cfm_cb );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_GOEP_SRV_ACCEPT:
      BT_MSG_DEBUG( "BT PF GOEP SRV CMD RX: Accept", 0, 0, 0 );
      headers_list.list = (OI_OBEX_HEADER*)
        cmd_ptr->cmd_msg.cmd_goep_srv_accept.headers_ptr;
      headers_list.count = cmd_ptr->cmd_msg.cmd_goep_srv_accept.headers_cnt;
      status = OI_OBEXSRV_AcceptConnect(
        (OI_OBEXSRV_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_goep_srv_accept.conn_id,
        cmd_ptr->cmd_msg.cmd_goep_srv_accept.accept,
        ((headers_list.list != NULL) && (headers_list.count != 0)) ? 
          &headers_list : NULL,
        (OI_STATUS)cmd_ptr->cmd_msg.cmd_goep_srv_accept.status );
      cmd_ptr->cmd_hdr.cmd_status = status; 
      break;

    case BT_PF_CMD_GOEP_SRV_CFM_SET_PATH:
      BT_MSG_DEBUG( "BT PF GOEP SRV CMD RX: Cfm SetPath", 0, 0, 0 );
      headers_list.list = (OI_OBEX_HEADER*)
        cmd_ptr->cmd_msg.cmd_goep_srv_cfm_set_path.headers_ptr;
      headers_list.count = cmd_ptr->cmd_msg.cmd_goep_srv_cfm_set_path.headers_cnt;
      status = OI_OBEXSRV_ConfirmSetpath(
        (OI_OBEXSRV_CONNECTION_HANDLE)
        cmd_ptr->cmd_msg.cmd_goep_srv_cfm_set_path.conn_id,
        ((headers_list.list != NULL) ? &headers_list : NULL),
        (OI_STATUS)cmd_ptr->cmd_msg.cmd_goep_srv_cfm_set_path.status );
      cmd_ptr->cmd_hdr.cmd_status = status; 
      break;

    case BT_PF_CMD_GOEP_SRV_AUTH_RSP:
      BT_MSG_DEBUG( "BT PF GOEP SRV CMD RX: Auth Rsp", 0, 0, 0 );
      OI_OBEXSRV_AuthenticationResponse(
        (OI_OBEXSRV_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_goep_srv_auth_rsp.conn_id,
        (char*)(cmd_ptr->cmd_msg.cmd_goep_srv_auth_rsp.password),
        cmd_ptr->cmd_msg.cmd_goep_srv_auth_rsp.read_only );
      cmd_ptr->cmd_hdr.cmd_status = OI_OK;
      break;

    case BT_PF_CMD_GOEP_SRV_GET_RSP:
      BT_MSG_DEBUG( "BT PF GOEP SRV CMD RX: Get Rsp", 0, 0, 0 );
      headers_list.list = (OI_OBEX_HEADER*)
        cmd_ptr->cmd_msg.cmd_goep_srv_get_rsp.headers_ptr;
      headers_list.count = cmd_ptr->cmd_msg.cmd_goep_srv_get_rsp.headers_cnt;
      status = OI_OBEXSRV_GetResponse(
        (OI_OBEXSRV_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_goep_srv_get_rsp.conn_id,
        ((headers_list.list != NULL) ? &headers_list : NULL),
        (OI_STATUS)cmd_ptr->cmd_msg.cmd_goep_srv_get_rsp.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_GOEP_SRV_PUT_RSP:
      BT_MSG_DEBUG( "BT PF GOEP SRV CMD RX: Put Rsp", 0, 0, 0 );
      headers_list.list = (OI_OBEX_HEADER*)
        cmd_ptr->cmd_msg.cmd_goep_srv_put_rsp.headers_ptr;
      headers_list.count = cmd_ptr->cmd_msg.cmd_goep_srv_put_rsp.headers_cnt;

      status = OI_OBEXSRV_PutResponse(
        (OI_OBEXSRV_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_goep_srv_put_rsp.conn_id,
        ((headers_list.list != NULL) ? &headers_list : NULL),
        (OI_STATUS)cmd_ptr->cmd_msg.cmd_goep_srv_put_rsp.status );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    case BT_PF_CMD_GOEP_SRV_DCN_RSP:
    BT_MSG_DEBUG( "BT PF GOEP SRV CMD RX: Dcn Rsp", 0, 0, 0 );
    headers_list.list = (OI_OBEX_HEADER*)
      cmd_ptr->cmd_msg.cmd_goep_srv_dcn_rsp.headers_ptr;
    headers_list.count = cmd_ptr->cmd_msg.cmd_goep_srv_dcn_rsp.headers_cnt;

    status = OBEXSRV_DisconnectResponse(
      (OI_OBEXSRV_CONNECTION_HANDLE)
        cmd_ptr->cmd_msg.cmd_goep_srv_dcn_rsp.conn_id,
      ((headers_list.list != NULL) ? &headers_list : NULL),
      (OI_STATUS)cmd_ptr->cmd_msg.cmd_goep_srv_dcn_rsp.status );
    cmd_ptr->cmd_hdr.cmd_status = status;
    break;

    case BT_PF_CMD_GOEP_SRV_FORCE_DCN:
      BT_MSG_DEBUG( "BT PF GOEP SRV CMD RX: Force Disconnect", 0, 0, 0 );
      status = OI_OBEXSRV_ForceDisconnect(
        (OI_OBEXSRV_CONNECTION_HANDLE)
          cmd_ptr->cmd_msg.cmd_goep_srv_force_dcn.conn_id );
      cmd_ptr->cmd_hdr.cmd_status = status;
      break;

    default:
      BT_ERR( "BT PF GOEP CMD RX: Unknown: %x",
              cmd_ptr->cmd_hdr.cmd_type, 0, 0 );
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
  }
}

/*===========================================================================

FUNCTION
  bt_pfgoep_reassemble_pkt    
   
DESCRIPTION
  This function performs the same operation as OI_OBEXCOMMON_ReassemblePacket.
  The difference is that this function takes a watermark pointer as input 
  parameter and pull data off the watermark queue and copies the data into 
  common->rcvBuffer. In this way, data is only copied once.
    
===========================================================================*/
OI_STATUS bt_pfgoep_reassemble_pkt(OBEX_COMMON *common, dsm_watermark_type* wm_ptr)
{    
  OI_STATUS status;       
  OI_UINT16 pkt_len = 0;
  OI_UINT16 data_len = 0;
  dsm_item_type* dsm_item_ptr;
  OI_UINT16 i;
  OI_UINT16 bytes_allowed;
  OI_UINT8 temp_byte;
  OI_BOOL overflow = FALSE;


  /*
   * Because OBEX is a request/response protocol we expect to have processed
   * each packet before we get another one. If we get multiple packets
   * something has gone wrong.  Only exception is Abort.
   */
  if (common->rcvState == OI_OBEX_RCV_COMPLETE) 
  {
    BT_ERR( "BT PF GOEP: reassemble_pkt discarding data", 0, 0, 0 );
    status = OI_OBEX_PACKET_OVERFLOW;
    goto Receive_Error;
  }

  /*
   * If this is a new packet, get the packet length to decide if we
   * need to do reassembly.
   */
  if (common->rcvState == OI_OBEX_RCV_BUF_EMPTY) 
  {
    dsm_item_ptr = (dsm_item_type*)q_check( wm_ptr->q_ptr );
    if (dsm_item_ptr == NULL)
    {                             
      BT_MSG_HIGH( "BT PF GOEP: reassemble_pkt received empty queue", 0, 0, 0 );
      /* ignore callback if queue is empty such that operation can keep going */
      return OI_OK;
    }
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

    pkt_len = GetUINT16_BigEndian( dsm_item_ptr->data_ptr + sizeof(OI_UINT8) );

    if (pkt_len > common->maxRecvPktLen)
    { 
      BT_ERR( "BT PF GOEP: reassemble_pkt pkt_len %x > max %x", 
              pkt_len, common->maxRecvPktLen, 0 );
      status = OI_OBEX_BAD_PACKET;
      goto Receive_Error;
    }

    if (wm_ptr->current_cnt < OI_OBEX_SMALLEST_PKT)
    {
      /*
       * This is almost certainly a packet error - even though a server
       * could send OBEX packets byte-by-byte no implementation will do
       * this.
       */                         
      BT_ERR( "BT PF GOEP: reassemble_pkt got a short frame", 0, 0, 0 );
      status = OI_OBEX_BAD_PACKET;
      goto Receive_Error;
    }

    if (wm_ptr->current_cnt > pkt_len)
    {            
      BT_MSG_DEBUG( "BT PF GOEP: reassemble_pkt, data len %x > pkt len %x",
                    wm_ptr->current_cnt, pkt_len, 0 );
      /* ignore extra bytes which will be discarded below */
    }
    /*
     * Packet will be reassembled in the receive buffer.
     */                                           
    ByteStream_Init(common->rcvPacket, common->rcvBuffer, pkt_len);
    ByteStream_Open(common->rcvPacket, BYTESTREAM_WRITE);
    common->rcvState = OI_OBEX_RCV_IN_PROGRESS;
  }

  /* Sanity check */
  if ( common->rcvPacket.__data == NULL ) 
  {
    /* Should never happen */
    BT_ERR( "BT PF GOEP: reassemble_pkt rcvPacket.__data is NULL!", 0, 0, 0 );
    status = OI_OBEX_INTERNAL_SERVER_ERROR;
    goto Receive_Error;
  }

  while ( (dsm_item_ptr = dsm_dequeue( wm_ptr )) != NULL )
  { 
    data_len = dsm_length_packet( dsm_item_ptr );
    bytes_allowed = ByteStream_NumWriteBytesAllowed( common->rcvPacket );
    /*
     * Check for packet overflow.
     */ 
    if (overflow)
    {
      BT_ERR( "BT PF GOEP: reasm pkt overflow, continue to ignore bytes",
              0, 0, 0 );

      /* continue discarding extra bytes from subsequent DSMs */
      for (i = 0; i < data_len; i++)
      {
        dsm_pullup( &dsm_item_ptr, &temp_byte, 1 );
      }
    }
    else if (data_len > bytes_allowed) 
    {
      uint8 cmd_code = 0;

      /* only retrieve up to the number of bytes specified by length header */
      dsm_pullup( &dsm_item_ptr, 
                  (void*)(common->rcvPacket.__data + common->rcvPacket.__pos), 
                  bytes_allowed );
      common->rcvPacket.__pos += bytes_allowed;

      // check if the next packet is Abort command.  If true, discard current packet
      // and process the Abort command.  Otherwise, discard the extra bytes.
      cmd_code = GetUINT8_BigEndian( dsm_item_ptr->data_ptr );
      if ( (OI_OBEX_FINAL(cmd_code) == OI_OBEX_CMD_ABORT) && 
           ( (data_len - bytes_allowed) >= BT_PF_OBEX_ABORT_PKT_SIZE) )
      {
        BT_MSG_DEBUG( "BT PF GOEP: reassemble_pkt, abort packet rx'ed, ignore curr pkt", 0, 0, 0);

        // throw away current packet.  For Abort packet, ignore optional headers (if present)
        ByteStream_Init(common->rcvPacket, common->rcvBuffer, BT_PF_OBEX_ABORT_PKT_SIZE);
        ByteStream_Open(common->rcvPacket, BYTESTREAM_WRITE);
        common->rcvState = OI_OBEX_RCV_IN_PROGRESS;
        bytes_allowed = ByteStream_NumWriteBytesAllowed( common->rcvPacket );

        // read the bytes corresponding to obex (abort) packet
        dsm_pullup( &dsm_item_ptr, 
                    (void*)(common->rcvPacket.__data + common->rcvPacket.__pos), 
                    bytes_allowed );
        common->rcvPacket.__pos += bytes_allowed;

        /* discard any extra bytes */
        for (i = 0; i < dsm_length_packet( dsm_item_ptr ); i++)
        {
          dsm_pullup( &dsm_item_ptr, &temp_byte, 1 );
        }
      }
      else
      {
        BT_ERR( "BT PF GOEP: reasm pkt overflow, ignoring bytes, DL %x BA %x",
                data_len, bytes_allowed, 0 );

        overflow = TRUE;

        /* discard the extra bytes from the DSM */
        for (i = 0; i < data_len - bytes_allowed; i++)
        {
          dsm_pullup( &dsm_item_ptr, &temp_byte, 1 );
        }
      }
    }
    else
    {
      /*
       * Copy data into receive buffer.
       */
      dsm_pullup( &dsm_item_ptr, 
                  (void*)(common->rcvPacket.__data + common->rcvPacket.__pos), 
                  data_len );
      common->rcvPacket.__pos += data_len;
    }
  }
  /*
   * If we cannot write any more data then we have a complete packet.
   */
  if (ByteStream_NumWriteBytesAllowed(common->rcvPacket) == 0) 
  {
    ByteStream_Close(common->rcvPacket);
    ByteStream_Open(common->rcvPacket, BYTESTREAM_READ);
    common->rcvState = OI_OBEX_RCV_COMPLETE;
  }

  return OI_OK;

Receive_Error:

  /*
   * Discard incomplete packets.
   */
  if (common->rcvState == OI_OBEX_RCV_IN_PROGRESS) 
  {
    common->rcvState = OI_OBEX_RCV_BUF_EMPTY;
  }                          
  BT_ERR( "BT PF GOEP: rcv pkt err, emptying watermark q", 0, 0, 0 );
  dsm_empty_queue( wm_ptr );
  return status;
}

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT_EXTPF_GOEP */
#endif /* FEATURE_BT */  
