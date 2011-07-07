/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 B L U E T O O T H    O B J E C T    P U S H    P R O F I L E    C L I E N T

GENERAL DESCRIPTION
  This module contains the Bluetooth Object Push Profile client data and code.

Copyright (c) 2004 - 2009 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfopp2.c#2 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-01-14   gs  Added support for BT_EV_PF_OPP_CLI_CON_PROG_IND event 

*    #15       16 Feb 2007            PN
* Made use of safe string functions.
*
*    #14       28 Nov 2006            RP
* Updated the Debug messages to print the size of the file.
*
*    #13       18 Oct 2006            PR
* Replaced OI_OPPClient_Abort() with OI_OPPClient_Cancel() while 
*  merging OI Code v3.0.14
*
*    #12       25 Sep 2006            GS
* Changed filesystem open and close debug message level to "high".
*
*    #11       12 Jun 2006            DSN
* Typecasting fixes for RVCT compiler.
*
*    #10       12 Aug 2005            JH
* Generate object close req prior to disconnect cfm if file is left open.
*
*    #9        20 Apr 2005            JH
* Converted file operation done and abort functions into queued commands.
*
*    #8        19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #7        26 Jul 2004            JH
* Revamped object system interface and eliminated the use of timeouts in object 
*  system interactions.
*
*    #5        23 Apr 2004            JH
* Replaced all bt_pf_cmd* with bt_cmd_pf*; replaced all BT_PF_EV* with
*  BT_EV_PF*
*
*    #4        14 Apr 2004            JH
* Made changes related to the replacement of bt_pf_unicode_str_type command 
*  parameters with uint16* type
* Modified to rex_timed_wait calls to properly implement timeout
*
*    #2        25 Feb 2004            JH
* Renamed btpfopp.h btpfoppi.h
*

===========================================================================*/ 

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_OPP

#include "bt.h"
#include "btpfi.h"
#include "btpf.h"
#include "btpfcmdi.h"
#include "bti.h"
#include "btutils.h"
#include "btmsg.h"
#include "btpfoppi.h"
#include "rex.h"
#include "opp_client.c"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Currently, only one OPP client is supported */
bt_app_id_type             bt_pf_opp_cli_app_id  = BT_PF_APP_ID_NULL;
bt_pf_opp_cli_conn_id_type bt_pf_opp_cli_conn_id = BT_PF_OPP_NO_CONN_ID;

/* object handle is actually the file pointer */
LOCAL bt_pf_opp_handle_type bt_pf_opp_obj_handle = NULL; 

LOCAL OI_OPP_OPEN_READ_CFM  bt_pf_opp_cli_open_read_cfm_fptr  = NULL;
LOCAL OI_OPP_OPEN_WRITE_CFM bt_pf_opp_cli_open_write_cfm_fptr = NULL;
LOCAL OI_OPP_READ_CFM       bt_pf_opp_cli_read_cfm_fptr       = NULL;
LOCAL OI_OPP_WRITE_CFM      bt_pf_opp_cli_write_cfm_fptr      = NULL;

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/
LOCAL void bt_pf_opp_cli_objsys_close(
  OI_OPP_HANDLE          handle,
  OI_STATUS              status,
  OI_OPP_CONNECTION      conn_id );

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_set_conn_id

DESCRIPTION
  Sets the connection ID for the OPP client connection.

===========================================================================*/ 
bt_cmd_status_type bt_pf_opp_cli_set_conn_id(
  bt_pf_opp_cli_conn_id_type conn_id )
{
  bt_ev_msg_type event;

  if (bt_pf_opp_cli_conn_id == BT_PF_OPP_NO_CONN_ID)
  {
    bt_pf_opp_cli_conn_id = (bt_pf_opp_cli_conn_id_type)conn_id;

    BT_MSG_HIGH("BT PF OPP CLI: client connecting, Conn: %x",
                conn_id, 0, 0 );

    event.ev_hdr.bt_app_id = bt_pf_opp_cli_app_id;

    /* send connecting evt to upper layer */
    event.ev_hdr.ev_type = BT_EV_PF_OPP_CLI_CON_PROG_IND;
    event.ev_msg.ev_opp_cli_con_prog_ind.conn_id = 
      (bt_pf_opp_cli_conn_id_type)conn_id;
    event.ev_msg.ev_opp_cli_con_prog_ind.status = BT_CS_GN_SUCCESS;
    bt_ec_send_event( &event );

    return(OI_OK);
  }
  else
  {
    BT_ERR( "BT PF OPP CLI SET CONN ID: %x, already set to: %x", 
            conn_id, bt_pf_opp_cli_conn_id, 0);
    return(OI_STATUS_INTERNAL_ERROR);
  }
}

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_event_cb

DESCRIPTION
  Callback function for handling OPP client events.
  Pointer to this function is passed to OI_OPPClient_Connect.

===========================================================================*/
LOCAL void bt_pf_opp_cli_event_cb( 
  OI_OPP_CLIENT_CONNECTION_HANDLE conn_id,
  OI_OPP_CLIENT_EVENT             cli_event,
  OI_STATUS                       status )
{
  bt_ev_msg_type event;          

  event.ev_hdr.bt_app_id = bt_pf_opp_cli_app_id;

  switch (cli_event)
  {
    case OI_OPP_CLIENT_CONNECTED:
      if (bt_pf_opp_cli_conn_id == (bt_pf_opp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF OPP CLI EV CB: con cfm, Conn: %x Stat %x", 
                      conn_id, status, 0 );

        bt_pf_opp_cli_open_read_cfm_fptr = NULL;
        bt_pf_opp_cli_open_write_cfm_fptr = NULL;
        bt_pf_opp_cli_read_cfm_fptr = NULL;
        bt_pf_opp_cli_write_cfm_fptr = NULL;

        event.ev_hdr.ev_type = BT_EV_PF_OPP_CLI_CON_CFM;
        event.ev_msg.ev_opp_cli_con_cfm.conn_id = 
          (bt_pf_opp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_opp_cli_con_cfm.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
        if (status != OI_OK) /* connection failed, cleanup */
        { 
          bt_pf_opp_cli_conn_id = BT_PF_OPP_NO_CONN_ID;
          bt_pf_opp_cli_app_id = BT_PF_APP_ID_NULL;
        }
      }
      else
      {
        BT_ERR( "BT PF OPP CLI EV CB: con cfm, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    case OI_OPP_CLIENT_DISCONNECT:
      if (bt_pf_opp_cli_conn_id == (bt_pf_opp_cli_conn_id_type)conn_id)
      {
        if ( bt_pf_opp_obj_handle != NULL )
        {
          /* if file is still open, close it */
          BT_MSG_HIGH( "BT PF OPP CLI: dcn while file still open,", 0, 0, 0 ); 
          bt_pf_opp_cli_objsys_close( bt_pf_opp_obj_handle, 
                                      BT_CS_PF_LINK_TERMINATED, 
                                      bt_pf_opp_cli_conn_id );
        }
        BT_MSG_DEBUG( "BT PF OPP CLI EV CB: dcn cfm, Conn: %x Stat %x", 
                      conn_id, status, 0 );

        event.ev_hdr.ev_type = BT_EV_PF_OPP_CLI_DCN_CFM;
        event.ev_msg.ev_opp_cli_dcn_cfm.conn_id = 
          (bt_pf_opp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_opp_cli_dcn_cfm.status = 
          (bt_cmd_status_type)status;
        bt_pf_opp_cli_conn_id = BT_PF_OPP_NO_CONN_ID;
        bt_pf_opp_cli_app_id = BT_PF_APP_ID_NULL;
        bt_ec_send_event( &event );
      }
      else
      {
        BT_ERR( "BT PF OPP CLI EV CB: dcn cfm, unknown conn: %x", 
                conn_id, 0, 0 );
      } 
      break;

    case OI_OPP_CLIENT_PUSH_COMPLETE:
      if (bt_pf_opp_cli_conn_id == (bt_pf_opp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF OPP CLI EV CB: push done, Conn: %x Stat %x", 
                      conn_id, status, 0 );
        event.ev_hdr.ev_type = BT_EV_PF_OPP_CLI_PSH_DNE;
        event.ev_msg.ev_opp_cli_push_done.conn_id = 
          (bt_pf_opp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_opp_cli_push_done.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
      }
      else
      {
        BT_ERR( "BT PF OPP CLI EV CB: push done, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    case OI_OPP_CLIENT_PULL_COMPLETE:
      if (bt_pf_opp_cli_conn_id == (bt_pf_opp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF OPP CLI EV CB: pull done, Conn: %x Stat %x", 
                      conn_id, status, 0 );
        event.ev_hdr.ev_type = BT_EV_PF_OPP_CLI_PUL_DNE;
        event.ev_msg.ev_opp_cli_pull_done.conn_id = 
          (bt_pf_opp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_opp_cli_pull_done.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
      }
      else
      {
        BT_ERR( "BT PF OPP CLI EV CB: pull done, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    default:
      BT_ERR( "BT PF OPP CLI EV CB: unknown, Conn: %x Stat %x", 
              conn_id, status, 0 );
  }
  
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_objsys_open_read

DESCRIPTION
  Callback function supplied to the OPP driver. This function is invoked
  by the driver to instruct the application to open an object for reading.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_opp_cli_objsys_open_read(
  const OI_OBEX_UNICODE* name_ptr,
  const OI_CHAR*         type_ptr,
  OI_OPP_OPEN_READ_CFM   open_read_cfm_fptr,
  OI_OPP_CONNECTION      conn_id )
{
  bt_ev_msg_type  event;
  uint8           name_len;

  /* send event to tell app to open object for reading */
  if (bt_pf_opp_cli_conn_id != (bt_pf_opp_cli_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF OPP CLI OS OPEN READ: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  BT_MSG_HIGH( "BT PF OPP CLI OS OPEN READ: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_opp_cli_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_OPP_CLI_OPEN_READ_REQ;
  event.ev_msg.ev_opp_cli_open_read_req.conn_id = 
    (bt_pf_opp_cli_conn_id_type)conn_id;

  if ( (name_ptr == NULL) || (name_ptr->len == 0) )
  {
    name_len = 0;
    event.ev_msg.ev_opp_cli_open_read_req.name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_opp_cli_open_read_req.name_str,
      (bt_pf_unicode_str_type*)name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_opp_cli_open_read_req.name_len = name_len;
  
  if (type_ptr == NULL)
  {
    event.ev_msg.ev_opp_cli_open_read_req.type_str[0] = 0;
  }
  else
  {
    /* type would be invalid if length exceeds limit, let app reject it */ 
    BT_STRCPY( (char *)event.ev_msg.ev_opp_cli_open_read_req.type_str, 
               type_ptr, 
               sizeof(event.ev_msg.ev_opp_cli_open_read_req.type_str) );
    event.ev_msg.ev_opp_cli_open_read_req.type_str[BT_PF_MAX_MIME_TYPE_LEN] = 
      '\0'; /* doesn't hurt if type string is shorter */
  }

  bt_pf_opp_cli_open_read_cfm_fptr = open_read_cfm_fptr;

  bt_ec_send_event( &event );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_objsys_open_write

DESCRIPTION
  Callback function supplied to the OPP driver. This function is invoked
  by the driver to instruct the application to open an object for writing.
  
NOTE
  The parameters data and len are no longer used and will be removed
  in the next release.  
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_opp_cli_objsys_open_write(
  const OI_OBEX_UNICODE* name_ptr,
  const OI_CHAR*         type_ptr,
  OI_UINT32              obj_size,
  OI_OPP_OPEN_WRITE_CFM  open_write_cfm_fptr,
  OI_OPP_CONNECTION      conn_id )
{
  bt_ev_msg_type event;
  uint8          name_len;

  /* send event to tell app to open object for writing */
  if (bt_pf_opp_cli_conn_id != (bt_pf_opp_cli_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF OPP CLI OS OPEN WRITE: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_HIGH( "BT PF OPP CLI OS OPEN WRITE: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_opp_cli_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_OPP_CLI_OPEN_WRITE_REQ;
  event.ev_msg.ev_opp_cli_open_write_req.conn_id = 
    (bt_pf_opp_cli_conn_id_type)conn_id;
  event.ev_msg.ev_opp_cli_open_write_req.obj_size =
    obj_size;

  if ( (name_ptr == NULL) || (name_ptr->len == 0) )
  {
    name_len = 0;
    event.ev_msg.ev_opp_cli_open_write_req.name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_opp_cli_open_write_req.name_str,
      (bt_pf_unicode_str_type*)name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_opp_cli_open_write_req.name_len = name_len;
  
  if (type_ptr == NULL)
  {
    event.ev_msg.ev_opp_cli_open_write_req.type_str[0] = 0;
  }
  else
  {
    /* type would be invalid if length exceeds limit, let app reject it */ 
    BT_STRCPY( (char *)event.ev_msg.ev_opp_cli_open_write_req.type_str, 
               type_ptr, 
               sizeof(event.ev_msg.ev_opp_cli_open_write_req.type_str) );
    event.ev_msg.ev_opp_cli_open_write_req.type_str[BT_PF_MAX_MIME_TYPE_LEN] = 
      '\0'; /* doesn't hurt if type string is shorter */
  }

  bt_pf_opp_cli_open_write_cfm_fptr = open_write_cfm_fptr;

  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_objsys_close

DESCRIPTION
  Callback function supplied to the OPP driver. This function is invoked
  by the driver to instruct the application to close an object.
  
===========================================================================*/
LOCAL void bt_pf_opp_cli_objsys_close(
  OI_OPP_HANDLE          handle,
  OI_STATUS              status,
  OI_OPP_CONNECTION      conn_id )
{
  bt_ev_msg_type event;

  /* send event to tell app to close object */
  if (bt_pf_opp_cli_conn_id == (bt_pf_opp_cli_conn_id_type)conn_id)
  {
    BT_MSG_HIGH( "BT PF OPP CLI OS CLOSE: Conn: %x", 
                  conn_id, 0, 0 ); 

    event.ev_hdr.bt_app_id = bt_pf_opp_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_OPP_CLI_CLOSE_REQ;
    event.ev_msg.ev_opp_cli_close_req.conn_id = 
      (bt_pf_opp_cli_conn_id_type)conn_id;
    event.ev_msg.ev_opp_cli_close_req.handle =
      (bt_pf_opp_handle_type)handle;
    event.ev_msg.ev_opp_cli_close_req.status =
      (bt_cmd_status_type)status;

    bt_ec_send_event( &event );

    bt_pf_opp_obj_handle = NULL;
  }
  else
  {
    BT_ERR( "BT PF OPP CLI OS CLOSE: unknown conn: %x", 
            conn_id, 0, 0 );
  }
  /* no respond from app is expected, object is assumed closed */
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_objsys_read

DESCRIPTION
  Callback function supplied to the OPP driver. This function is invoked
  by the driver to instruct the application to read an object.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_opp_cli_objsys_read(
  OI_OPP_HANDLE          handle,
  OI_UINT16              max_read,
  OI_OPP_READ_CFM        read_cfm_fptr,
  OI_OPP_CONNECTION      conn_id )
{
  bt_ev_msg_type event;

  /* send event to tell app to read object */
  if (bt_pf_opp_cli_conn_id != (bt_pf_opp_cli_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF OPP CLI OS READ: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_DEBUG( "BT PF OPP CLI OS READ: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_opp_cli_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_OPP_CLI_READ_REQ;
  event.ev_msg.ev_opp_cli_read_req.conn_id = 
    (bt_pf_opp_cli_conn_id_type)conn_id;
  event.ev_msg.ev_opp_cli_read_req.handle =
    (bt_pf_opp_handle_type)handle;
  event.ev_msg.ev_opp_cli_read_req.max_read =
    max_read;

  bt_pf_opp_cli_read_cfm_fptr = read_cfm_fptr;

  bt_ec_send_event( &event );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_objsys_write

DESCRIPTION
  Callback function supplied to the OPP driver. This function is invoked
  by the driver to instruct the application to write to an object.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_opp_cli_objsys_write(
  OI_OPP_HANDLE          handle,
  const OI_BYTE*         buffer_ptr,
  OI_UINT16              buffer_len,
  OI_OPP_WRITE_CFM       write_cfm_ptr,
  OI_OPP_CONNECTION      conn_id )
{
  bt_ev_msg_type event;

  /* send event to tell app to write to object */
  if (bt_pf_opp_cli_conn_id != (bt_pf_opp_cli_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF OPP CLI OS WRITE: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }          
  
  BT_MSG_DEBUG( "BT PF OPP CLI OS WRITE: Conn: %x Len 0x%x", 
                conn_id, buffer_len, 0 );

  event.ev_hdr.bt_app_id = bt_pf_opp_cli_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_OPP_CLI_WRITE_REQ;
  event.ev_msg.ev_opp_cli_write_req.conn_id = 
    (bt_pf_opp_cli_conn_id_type)conn_id;
  event.ev_msg.ev_opp_cli_write_req.handle =
    (bt_pf_opp_handle_type)handle;
  event.ev_msg.ev_opp_cli_write_req.buffer_ptr =
    (byte*)buffer_ptr;
  event.ev_msg.ev_opp_cli_write_req.buffer_len =
    buffer_len;

  bt_pf_opp_cli_write_cfm_fptr = write_cfm_ptr;

  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/* Structure storing the pointers to the above callback functions          */
/* This is passed into the driver when bt_pf_opp_cli_cmd_connect is called */
LOCAL OI_OPP_OBJSYS_FUNCTIONS bt_pf_opp_cli_objsys_ops =
{
  bt_pf_opp_cli_objsys_open_read,
  bt_pf_opp_cli_objsys_open_write,
  bt_pf_opp_cli_objsys_close,
  bt_pf_opp_cli_objsys_read,
  bt_pf_opp_cli_objsys_write
};

/*===========================================================================

FUNCTION
  bt_pf_opp_cmd_cli_connect

DESCRIPTION
  Performs connection to remote OPP server.

===========================================================================*/
bt_cmd_status_type bt_pf_opp_cli_cmd_connect(
  bt_app_id_type                app_id,
  bt_bd_addr_type*              bd_addr_ptr,
  uint8                         channel,
  bt_pf_opp_cli_conn_id_type*   conn_id_ptr )
{
  OI_STATUS status;

  if ( bt_pf_opp_cli_conn_id != BT_PF_OPP_NO_CONN_ID )
  {
    BT_MSG_DEBUG( "BT PF OPP CLI CONN: OPP client already connected, \
                  App: %x, Conn: %x", app_id, bt_pf_opp_cli_conn_id, 0 );
    return(OI_STATUS_ALREADY_CONNECTED);
  } 

  bt_pf_opp_cli_app_id = app_id;
  bt_pf_opp_cli_open_read_cfm_fptr = NULL;
  bt_pf_opp_cli_open_write_cfm_fptr = NULL;
  bt_pf_opp_cli_read_cfm_fptr = NULL;
  bt_pf_opp_cli_write_cfm_fptr = NULL;

  status = OI_OPPClient_Connect( 
    (OI_BD_ADDR*)bd_addr_ptr, 
    channel,
    (OI_OPP_CLIENT_CONNECTION_HANDLE*)conn_id_ptr,
    bt_pf_opp_cli_event_cb, 
    &bt_pf_opp_cli_objsys_ops );

  if (status != OI_OK)
  {
    bt_pf_opp_cli_app_id = BT_PF_APP_ID_NULL;
    BT_ERR( "BT PF OPP CLI CONN: OI_OPPClient_Connect returns %x", 
                status, 0, 0 );
  }
  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_cmd_abort

DESCRIPTION
  Processes an OPP client abort command.

===========================================================================*/
bt_cmd_status_type bt_pf_opp_cli_cmd_abort(
  bt_app_id_type               app_id,
  bt_pf_opp_srv_conn_id_type   conn_id )
{ 
  OI_STATUS             status;
  OI_OPP_OPEN_READ_CFM  open_read_cb_fptr  = NULL;
  OI_OPP_OPEN_WRITE_CFM open_write_cb_fptr = NULL;
  OI_OPP_READ_CFM       read_cb_fptr       = NULL;
  OI_OPP_WRITE_CFM      write_cb_fptr      = NULL;

  status = OI_OPPClient_Cancel( (OI_OPP_CLIENT_CONNECTION_HANDLE)conn_id,NULL );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF OPP CLI ABORT: OI_OPPClient_Abort returns %x", 
                status, 0, 0 );
  }

  TASKLOCK();
  if (bt_pf_opp_cli_open_read_cfm_fptr != NULL)
  {
    open_read_cb_fptr = bt_pf_opp_cli_open_read_cfm_fptr;
    bt_pf_opp_cli_open_read_cfm_fptr = NULL;
  }

  if (bt_pf_opp_cli_open_write_cfm_fptr != NULL)
  {
    open_write_cb_fptr = bt_pf_opp_cli_open_write_cfm_fptr;
    bt_pf_opp_cli_open_write_cfm_fptr = NULL;
  }

  if (bt_pf_opp_cli_read_cfm_fptr != NULL)
  {
    read_cb_fptr = bt_pf_opp_cli_read_cfm_fptr;
    bt_pf_opp_cli_read_cfm_fptr = NULL;
  }

  if (bt_pf_opp_cli_write_cfm_fptr != NULL)
  {
    write_cb_fptr = bt_pf_opp_cli_write_cfm_fptr;
    bt_pf_opp_cli_write_cfm_fptr = NULL;
  }    
  TASKFREE();

  if ( open_read_cb_fptr )
  {
    open_read_cb_fptr( 
      NULL, NULL, NULL, 0, OI_OBEX_CLIENT_ABORTED_COMMAND, NULL );
  }
  if ( open_write_cb_fptr )
  {
    open_write_cb_fptr( NULL, OI_OBEX_CLIENT_ABORTED_COMMAND, NULL ); 
  }
  if ( read_cb_fptr )
  {
    read_cb_fptr( NULL, NULL, 0, OI_OBEX_CLIENT_ABORTED_COMMAND, NULL );
  }
  if ( write_cb_fptr )
  {
    write_cb_fptr( NULL, OI_OBEX_CLIENT_ABORTED_COMMAND, NULL ); 
  }

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_cmd_open_read_done

DESCRIPTION
  Processes an OPP client open read command.
  
===========================================================================*/
bt_cmd_status_type bt_pf_opp_cli_cmd_open_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  uint16*                       name_ptr,
  uint8                         name_len,
  char*                         type_ptr,
  uint32                        size,
  bt_cmd_status_type            status )
{
  OI_OBEX_UNICODE      name;
  OI_OPP_OPEN_READ_CFM open_read_cb_fptr = NULL;
  bt_cmd_status_type   ret = OI_OK;

  TASKLOCK();

  if (bt_pf_opp_cli_open_read_cfm_fptr != NULL)
  {
    open_read_cb_fptr = bt_pf_opp_cli_open_read_cfm_fptr;
    bt_pf_opp_cli_open_read_cfm_fptr = NULL;
  }
  else
  {
    BT_ERR( "BT PF OPP CLI OPEN READ DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    return( ret );
  }     

  if ( ( status == OI_OK ) || ( status == OI_STATUS_END_OF_FILE ) )
  {
    if (conn_id != bt_pf_opp_cli_conn_id)
    { 
      BT_ERR( "BT PF OPP CLI OPEN READ DNE: App: %x, unknown conn: %x", 
              app_id, conn_id, 0 );
      return(OI_OBEX_CONNECTION_NOT_FOUND);
    }
  
    name.len = (uint16)name_len;
    name.str = name_ptr;
  }

  BT_MSG_API( "BT PF OPP CLI OPEN READ DNE: App: %x, Conn: %x, Stat: %x", 
                app_id, conn_id, status );

  bt_pf_opp_obj_handle = handle;
  open_read_cb_fptr( (OI_OPP_HANDLE)handle, &name, type_ptr, size, 
                     (OI_STATUS)status, (OI_OPP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_cmd_open_write_done

DESCRIPTION
  Process an OPP client open write command.

===========================================================================*/
bt_cmd_status_type bt_pf_opp_cli_cmd_open_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status )
{
  OI_OPP_OPEN_WRITE_CFM open_write_cb_fptr = NULL;
  bt_cmd_status_type    ret = OI_OK;

  TASKLOCK();

  if (bt_pf_opp_cli_open_write_cfm_fptr != NULL)
  {
    open_write_cb_fptr = bt_pf_opp_cli_open_write_cfm_fptr;
    bt_pf_opp_cli_open_write_cfm_fptr = NULL;
  }
  else
  {
    BT_ERR( "BT PF OPP CLI OPEN WRITE DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    return( ret );
  }             

  if (conn_id != bt_pf_opp_cli_conn_id)
  {
    BT_ERR( "BT PF OPP CLI OPEN WRITE DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }            
  
  BT_MSG_API( "BT PF OPP CLI OPEN WRITE DNE: App: %x, Conn: %x, Stat: %x", 
              app_id, conn_id, status );

  bt_pf_opp_obj_handle = handle;
  open_write_cb_fptr( (OI_OPP_HANDLE)handle, (OI_STATUS)status, 
                      (OI_OPP_CONNECTION)conn_id );
                                              
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_cmd_read_done

DESCRIPTION
  Processes an OPP client read done command.

===========================================================================*/
bt_cmd_status_type bt_pf_opp_cli_cmd_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        data_len,
  bt_cmd_status_type            status )
{
  OI_OPP_READ_CFM    read_cb_fptr = NULL;
  bt_cmd_status_type ret = OI_OK;

  TASKLOCK();

  if (bt_pf_opp_cli_read_cfm_fptr != NULL)
  {
    read_cb_fptr = bt_pf_opp_cli_read_cfm_fptr;
    bt_pf_opp_cli_read_cfm_fptr = NULL;
  }
  else
  {
    BT_ERR( "BT PF OPP CLI READ DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    return( ret );
  }             

  if (conn_id != bt_pf_opp_cli_conn_id)
  {
    BT_ERR( "BT PF OPP CLI READ DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }            
  
  BT_MSG_API( "BT PF OPP CLI READ DNE: App: %x, Conn: %x, Stat: %x", 
              app_id, conn_id, status );

  read_cb_fptr( (OI_OPP_HANDLE)handle, data_ptr, data_len, (OI_STATUS)status,
                (OI_OPP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_cli_cmd_write_done

DESCRIPTION
  Processes an OPP client write done command.

===========================================================================*/
bt_cmd_status_type bt_pf_opp_cli_cmd_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_cli_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status )
{
  OI_OPP_WRITE_CFM   write_cb_fptr = NULL;
  bt_cmd_status_type ret = OI_OK;

  TASKLOCK();

  if (bt_pf_opp_cli_write_cfm_fptr != NULL)
  {
    write_cb_fptr = bt_pf_opp_cli_write_cfm_fptr;
    bt_pf_opp_cli_write_cfm_fptr = NULL;
  }
  else
  {
    BT_ERR( "BT PF OPP CLI WRITE DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    return( ret );
  }             

  if (conn_id != bt_pf_opp_cli_conn_id)
  {
    BT_ERR( "BT PF OPP CLI WRITE DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_API( "BT PF OPP CLI WRITE DNE: App: %x, Conn: %x, Stat: %x", 
              app_id, conn_id, status );

  write_cb_fptr( (OI_OPP_HANDLE)handle, (OI_STATUS)status, 
                 (OI_OPP_CONNECTION)conn_id );

  return(OI_OK);
}

#endif /* FEATURE_BT_EXTPF_OPP */
#endif /* FEATURE_BT */
