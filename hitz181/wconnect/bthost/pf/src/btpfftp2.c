/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 B L U E T O O T H   F I L E   T R A N S F E R   P R O F I L E   C L I E N T

GENERAL DESCRIPTION
  This module contains the Bluetooth File Transfer Profile client data and code.

Copyright (c) 2004-2009 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfftp2.c#2 $ 
$DateTime: 2009/04/17 11:28:04 $
$Author: ganeshs $


  when        who  what, where, why
  ----------  ---  ---------------------------------------------------------
  2009-01-14   gs  Updated bt_pf_ftp_cli_filesys_write() function to send
                   object size info in filesys write event.
  2008-08-14   rb  Added support for DCVS.

*    #21       28 Nov 2006            RP
* Updated the Debug messages to print the size of the file.
* 
*    #20       18 Oct 2006            PR
* Replaced OI_FTPClient_Abort() with OI_FTPClient_Cancel() while 
* merging OI Code v3.0.14.
* Updated the Code to handle Abort.
*
*    #19       10 Oct 2006            GS
* Updated bt_pf_ftp_cli_filesys_open() to reflect corresponding change
* in OI_FTP_OPEN function prototype.
*
*    #15       25 Sep 2006            GS
* Changed filesystem open and close debug message levels to "high".
*
*    #14       13 Jul 2005            JH
* Replaced all occurrences of bt_pf_opp_cli_conn_id_type with 
*  bt_pf_ftp_cli_conn_id_type.
*
*    #13       12 Jul 2005            JH
* Added BT_PF_TMO_SIG to signal mask when calling rex_timed_wait() which was 
*  previously misused.
*
*    #12        8 Jul 2005            JH
* Added status parameter to bt_pf_ftp_cli_filesys_close().
*
*    #11       12 Apr 2005            JH
* Converted file operation done and abort functions into queued commands.
*
*    #10       19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #9        26 Jul 2004            JH
* Revamped file system interface and eliminated the use of timeouts in file 
*  system interactions, except in folder list callback.
*
*    #8        7  Jul 2004            JH
* Modified bt_pf_ftp_folder_list_cb to properly support the use of 
*  bt_cmd_pf_ftp_cli_write_done for acknowledgement of the 
*  BT_EV_PF_FTP_CLI_GET_FOLDER_LIST_RSP event
*
*    #7        14 May 2004            JH
* Added FTP client folder browse support
*
*    #5        23 Apr 2004            JH
* Replaced all bt_pf_cmd* with bt_cmd_pf*; replaced all BT_PF_EV* with
*  BT_EV_PF*
*      
*    #4        14 Apr 2004            JH
* Modified to rex_timed_wait calls to properly implement timeout
*
*    #2        25 Feb 2004            JH
* Renamed btpfftp.h btpfftpi.h
*

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_FTP

#include "bt.h"
#include "btpfi.h"
#include "bti.h"
#include "btmsg.h"
#include "btpfftpi.h"
#include "rex.h"
#include "ftp_client.c"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/ 

/* Currently, only one FTP client is supported */
bt_app_id_type              bt_pf_ftp_cli_app_id = BT_PF_APP_ID_NULL;
bt_pf_ftp_cli_conn_id_type  bt_pf_ftp_cli_conn_id = BT_PF_FTP_NO_CONN_ID;

typedef enum
{
  FOLDER_LISTING_IDLE = 0,
  FOLDER_LISTING_SENT,
  FOLDER_LISTING_ACKNOWLEDGED
} bt_pf_ftp_folder_list_state_type;

LOCAL bt_pf_ftp_folder_list_state_type bt_pf_ftp_cli_folder_list_state = 
                                         FOLDER_LISTING_IDLE;
LOCAL OI_FTP_BROWSE_FOLDER_CFM bt_pf_ftp_cli_browse_cb_fptr = NULL;
LOCAL OI_FTP_OPEN_CFM          bt_pf_ftp_cli_open_cb_fptr = NULL;
LOCAL OI_FTP_READ_CFM          bt_pf_ftp_cli_read_cb_fptr = NULL;
LOCAL OI_FTP_WRITE_CFM         bt_pf_ftp_cli_write_cb_fptr = NULL;
LOCAL OI_FTP_SET_FOLDER_CFM    bt_pf_ftp_cli_set_folder_cb_fptr = NULL;

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_event_cb

DESCRIPTION
  Callback function for handling FTP client events.
  Pointer to this function is passed to OI_FTPClient_Connect.

===========================================================================*/

LOCAL void bt_pf_ftp_cli_event_cb( 
  OI_FTP_CLIENT_CONNECTION_HANDLE conn_id,
  OI_FTP_CLIENT_EVENT             cli_event,
  OI_STATUS                       status)
{
  bt_ev_msg_type event;          

  event.ev_hdr.bt_app_id = bt_pf_ftp_cli_app_id;

  switch (cli_event)
  {
    case OI_FTP_CLIENT_CONNECTED:
      if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF FTP CLI EV CB: con cfm, Conn: %x Stat %x", 
                      conn_id, status, 0 );                         
        event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_CON_CFM;
        event.ev_msg.ev_ftp_cli_con_cfm.conn_id = 
          (bt_pf_ftp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_ftp_cli_con_cfm.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
        if (status != OI_OK) /* connection failed, cleanup */
        {
          bt_pf_ftp_cli_conn_id = BT_PF_FTP_NO_CONN_ID;
          bt_pf_ftp_cli_app_id = BT_PF_APP_ID_NULL;
        }
      }
      else
      {
        BT_ERR( "BT PF FTP CLI EV CB: con cfm, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    case OI_FTP_CLIENT_CONNECTED_READ_ONLY:
      if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF FTP CLI EV CB: con r/o cfm, Conn: %x Stat %x", 
                      conn_id, status, 0 );                         
        event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_CON_READ_ONLY_CFM;
        event.ev_msg.ev_ftp_cli_con_read_only_cfm.conn_id = 
          (bt_pf_ftp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_ftp_cli_con_read_only_cfm.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
        if (status != OI_OK) /* connection failed, cleanup */
        {
          bt_pf_ftp_cli_conn_id = BT_PF_FTP_NO_CONN_ID;
          bt_pf_ftp_cli_app_id = BT_PF_APP_ID_NULL;
        }
      }
      else
      {
        BT_ERR( "BT PF FTP CLI EV CB: con r/o cfm, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    case OI_FTP_CLIENT_DISCONNECT:
      if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF FTP CLI EV CB: dcn cfm, Conn: %x Stat %x", 
                      conn_id, status, 0 );
        
        bt_pf_ftp_cli_browse_cb_fptr = NULL;
        bt_pf_ftp_cli_open_cb_fptr = NULL;
        bt_pf_ftp_cli_read_cb_fptr = NULL;
        bt_pf_ftp_cli_write_cb_fptr = NULL;
        bt_pf_ftp_cli_set_folder_cb_fptr = NULL;
        bt_pf_ftp_cli_folder_list_state = FOLDER_LISTING_IDLE;
        bt_pf_ftp_cli_conn_id = BT_PF_FTP_NO_CONN_ID;
        bt_pf_ftp_cli_app_id = BT_PF_APP_ID_NULL;

        event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_DCN_CFM;
        event.ev_msg.ev_ftp_cli_dcn_cfm.conn_id = 
          (bt_pf_ftp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_ftp_cli_dcn_cfm.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
      }
      else
      {
        BT_ERR( "BT PF FTP CLI EV CB: dcn cfm, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    case OI_FTP_CLIENT_GET_COMPLETE:
      if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF FTP CLI EV CB: get rsp, Conn: %x Stat %x", 
                      conn_id, status, 0 );                         
        event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_GET_RSP;
        event.ev_msg.ev_ftp_cli_get_rsp.conn_id = 
          (bt_pf_ftp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_ftp_cli_get_rsp.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
      }
      else
      {
        BT_ERR( "BT PF FTP CLI EV CB: get rsp, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    case OI_FTP_CLIENT_PUT_COMPLETE:
      if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF FTP CLI EV CB: put done, Conn: %x Stat %x", 
                      conn_id, status, 0 );                         
        event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_PUT_DNE;
        event.ev_msg.ev_ftp_cli_put_dne.conn_id = 
          (bt_pf_ftp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_ftp_cli_put_dne.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
      }
      else
      {
        BT_ERR( "BT PF FTP CLI EV CB: put done, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    case OI_FTP_CLIENT_DELETE_COMPLETE:
      if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF FTP CLI EV CB: del done, Conn: %x Stat %x", 
                      conn_id, status, 0 );                         
        event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_DELETE_DNE;
        event.ev_msg.ev_ftp_cli_delete_dne.conn_id = 
          (bt_pf_ftp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_ftp_cli_delete_dne.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
      }
      else
      {
        BT_ERR( "BT PF FTP CLI EV CB: del done, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    case OI_FTP_CLIENT_SETPATH_COMPLETE:
      if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF FTP CLI EV CB: set path done, Conn: %x Stat %x", 
                      conn_id, status, 0 );                         
        event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_SET_PATH_DNE;
        event.ev_msg.ev_ftp_cli_set_path_dne.conn_id = 
          (bt_pf_ftp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_ftp_cli_set_path_dne.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
      }
      else
      {
        BT_ERR( "BT PF FTP CLI EV CB: set path done, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    case OI_FTP_CLIENT_NEW_SUBFOLDER_COMPLETE:
      if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF FTP CLI EV CB: new subfolder done, Conn: %x Stat %x", 
                      conn_id, status, 0 );                         
        event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_NEW_SUBFOLDER_DNE;
        event.ev_msg.ev_ftp_cli_new_subfolder_dne.conn_id = 
          (bt_pf_ftp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_ftp_cli_new_subfolder_dne.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
      }
      else
      {
        BT_ERR( "BT PF FTP CLI EV CB: new subfolder done, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    case OI_FTP_CLIENT_TREE_PUT_COMPLETE:
      if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF FTP CLI EV CB: tree put done, Conn: %x Stat %x", 
                      conn_id, status, 0 );                         
        event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_TREE_PUT_DNE;
        event.ev_msg.ev_ftp_cli_tree_put_dne.conn_id = 
          (bt_pf_ftp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_ftp_cli_tree_put_dne.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
      }
      else
      {
        BT_ERR( "BT PF FTP CLI EV CB: tree put done, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;
      
    case OI_FTP_CLIENT_TREE_GET_COMPLETE:
      if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
      {
        BT_MSG_DEBUG( "BT PF FTP CLI EV CB: tree get rsp, Conn: %x Stat %x", 
                      conn_id, status, 0 );                         
        event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_TREE_GET_RSP;
        event.ev_msg.ev_ftp_cli_tree_get_rsp.conn_id = 
          (bt_pf_ftp_cli_conn_id_type)conn_id;
        event.ev_msg.ev_ftp_cli_tree_get_rsp.status = 
          (bt_cmd_status_type)status;
        bt_ec_send_event( &event );
      }
      else
      {
        BT_ERR( "BT PF FTP CLI EV CB: tree get rsp, unknown conn: %x", 
                conn_id, 0, 0 );
      }
      break;

    default:
      BT_ERR( "BT PF FTP CLI EV CB: unknown, Conn: %x Stat %x", 
              conn_id, status, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_auth_cb

DESCRIPTION
  Callback function for notifying app that FTP authentication is required by
  the server.
  Pointer to this function is passed into OI_FTPClient_Connect.

===========================================================================*/
LOCAL void bt_pf_ftp_cli_auth_cb( OI_FTP_CLIENT_CONNECTION_HANDLE conn_id,
                                  OI_BOOL user_id_required )
{
  bt_ev_msg_type event;                   

  if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF FTP CLI AUTH CB: Conn: %x Req'd: %x", 
                  conn_id, user_id_required, 0 );                         
    event.ev_hdr.bt_app_id = bt_pf_ftp_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_AUTH_REQ;
    event.ev_msg.ev_ftp_cli_auth_req.conn_id = 
      (bt_pf_ftp_cli_conn_id_type)conn_id;
    event.ev_msg.ev_ftp_cli_auth_req.user_id_required = user_id_required;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF FTP CLI AUTH CB: unknown conn: %x", 
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_folder_list_cb

DESCRIPTION
  Callback function for notifying app that the requested FTP folder listing 
  data from the server is available.
  Pointer to this function is passed into OI_FTPClient_GetFolderListing.
  
  Caller is responsible for freeing the memory allocated for storing the
  folder list data.

===========================================================================*/
void bt_pf_ftp_folder_list_cb( OI_FTP_CLIENT_CONNECTION_HANDLE conn_id,
                               OI_OBEX_BYTESEQ* folder_list_data_ptr, 
                               OI_STATUS status )
{
  bt_ev_msg_type event;
  byte*          data_ptr;

  if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF FTP CLI FOLDER LIST CB: Conn: %x Stat: %x", 
                  conn_id, status, 0 ); 
    if ( (folder_list_data_ptr == NULL) || (folder_list_data_ptr->len == 0) )
    {
      /* the final cb, which marks the completion of the get folder listing 
         operation, does not contain data */
      bt_pf_ftp_cli_folder_list_state = FOLDER_LISTING_IDLE;
      data_ptr = NULL;
      event.ev_msg.ev_ftp_cli_get_folder_list_rsp.folder_list_data_len = 0;
    }
    else
    {
      bt_pf_ftp_cli_folder_list_state = FOLDER_LISTING_SENT;
      data_ptr = folder_list_data_ptr->data;
      event.ev_msg.ev_ftp_cli_get_folder_list_rsp.folder_list_data_len = 
        folder_list_data_ptr->len;
    }

    event.ev_hdr.bt_app_id = bt_pf_ftp_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_GET_FOLDER_LIST_RSP;
    event.ev_msg.ev_ftp_cli_get_folder_list_rsp.conn_id = 
      (bt_pf_ftp_cli_conn_id_type)conn_id;
    event.ev_msg.ev_ftp_cli_get_folder_list_rsp.folder_list_data_ptr =
      data_ptr;
    event.ev_msg.ev_ftp_cli_get_folder_list_rsp.status = 
      (bt_cmd_status_type)status;

    /* clear sig first in case an earlier cfm was received after tmo expired */
    rex_clr_sigs( &bt_pf_tcb, BT_PF_FTP_SIG );
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF FTP CLI FOLDER LIST CB: unknown conn: %x", 
            conn_id, 0, 0 );
    return;
  }
  
  /* only wait for app acknowledgement when data is actually received */
  if ( bt_pf_ftp_cli_folder_list_state == FOLDER_LISTING_SENT )
  {    
    /* Wait for the folder listing data to be consumed.                 */
    /* App will call bt_cmd_pf_ftp_cli_write_done to signal completion. */
    rex_timed_wait( BT_PF_FTP_SIG | BT_PF_TMO_SIG, 
                    &bt_pf_ftp_tmo_timer, 
                    BT_PF_FTP_FILESYS_OPS_TMO );
  
    if (bt_pf_ftp_cli_folder_list_state == FOLDER_LISTING_ACKNOWLEDGED )
    {
      rex_clr_timer( &bt_pf_ftp_tmo_timer );
      rex_clr_sigs( &bt_pf_tcb, BT_PF_FTP_SIG );
    }
    else
    { 
      rex_clr_sigs( &bt_pf_tcb, BT_PF_TMO_SIG );
      BT_ERR( "BT PF FTP CLI FOLDER LIST CB: timeout, conn: %x", 
              conn_id, 0, 0 );
    }
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_filesys_browse

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to browse a folder in the file 
  system.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_ftp_cli_filesys_browse(
  const OI_OBEX_UNICODE*   folder_name_ptr,
  OI_FTP_BROWSE_FOLDER_CFM browse_cfm_fptr,
  OI_FTP_CONNECTION        conn_id )
{
  bt_ev_msg_type event;
  uint8          folder_name_len;

  /* send event to tell app to browse folder */
  if (bt_pf_ftp_cli_conn_id != (bt_pf_ftp_cli_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF FTP CLI FS BROWSE: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  } 
  
  BT_MSG_DEBUG( "BT PF FTP CLI FS BROWSE: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_ftp_cli_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_BROWSE_REQ;
  event.ev_msg.ev_ftp_cli_browse_req.conn_id = 
    (bt_pf_ftp_cli_conn_id_type)conn_id;

  if ( (folder_name_ptr == NULL) || (folder_name_ptr->len == 0) )
  {
    folder_name_len = 0;
    event.ev_msg.ev_ftp_cli_browse_req.folder_name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    folder_name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_ftp_cli_browse_req.folder_name_str,
      (bt_pf_unicode_str_type*)folder_name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_ftp_cli_browse_req.folder_name_len = folder_name_len;

  bt_pf_ftp_cli_browse_cb_fptr = browse_cfm_fptr;

  bt_ec_send_event( &event );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_filesys_open

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to open a file in the file 
  system.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_ftp_cli_filesys_open( 
  const OI_OBEX_UNICODE* name_ptr,
  OI_FTP_MODE            mode,
  OI_UINT32              obj_size /* not used */,
  OI_FTP_OPEN_CFM        open_cfm_fptr,
  OI_FTP_CONNECTION      conn_id)
{
  bt_ev_msg_type event;
  uint8          name_len;

  /* send event to tell app to open file */
  if (bt_pf_ftp_cli_conn_id != (bt_pf_ftp_cli_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF FTP CLI FS OPEN: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  bt_cmd_rm_update_bt_module_state (bt_pf_ftp_cli_app_id,
                                    BT_MODULE_ID_FTP, 
                                    BT_MODULE_ST_ACTIVE );
  
  BT_MSG_HIGH( "BT PF FTP CLI FS OPEN: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_ftp_cli_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_OPEN_REQ;
  event.ev_msg.ev_ftp_cli_open_req.conn_id = 
    (bt_pf_ftp_cli_conn_id_type)conn_id;
  event.ev_msg.ev_ftp_cli_open_req.mode =
    (bt_pf_ftp_mode_type)mode;

  if ( (name_ptr == NULL) || (name_ptr->len == 0) )
  {
    name_len = 0;
    event.ev_msg.ev_ftp_cli_open_req.name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_ftp_cli_open_req.name_str,
      (bt_pf_unicode_str_type*)name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_ftp_cli_open_req.name_len = name_len;

  bt_pf_ftp_cli_open_cb_fptr = open_cfm_fptr;
  
  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_filesys_close

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to close a file in the file 
  system.
  
===========================================================================*/
LOCAL void bt_pf_ftp_cli_filesys_close( 
  OI_FTP_HANDLE          handle,
  OI_FTP_CONNECTION      conn_id,
  OI_STATUS              status )
{
  bt_ev_msg_type event;

  /* send event to tell app to close file */
  if (bt_pf_ftp_cli_conn_id == (bt_pf_ftp_cli_conn_id_type)conn_id)
  {
    BT_MSG_HIGH( "BT PF FTP CLI FS CLOSE: Conn: %x", 
                  conn_id, 0, 0 ); 

    event.ev_hdr.bt_app_id = bt_pf_ftp_cli_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_CLOSE_REQ;
    event.ev_msg.ev_ftp_cli_close_req.conn_id = 
      (bt_pf_ftp_cli_conn_id_type)conn_id;
    event.ev_msg.ev_ftp_cli_close_req.handle = 
      (bt_pf_ftp_handle_type)handle;
    event.ev_msg.ev_ftp_cli_close_req.status = 
      (bt_cmd_status_type)status;
    bt_ec_send_event( &event );

    bt_cmd_rm_update_bt_module_state (bt_pf_ftp_cli_app_id,
                                      BT_MODULE_ID_FTP,
                                      BT_MODULE_ST_IDLE);
  }
  else
  {
    BT_ERR( "BT PF FTP CLI FS CLOSE: unknown conn: %x", 
            conn_id, 0, 0 );
  }
  /* no respond from app is expected, file is assumed closed */
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_filesys_read

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to read a file in the file 
  system.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_ftp_cli_filesys_read( 
  OI_FTP_HANDLE          handle,
  OI_UINT16              max_read,
  OI_FTP_READ_CFM        read_cfm_fptr,
  OI_FTP_CONNECTION      conn_id )
{  
  bt_ev_msg_type event;

  /* send event to tell app to read file */
  if (bt_pf_ftp_cli_conn_id != (bt_pf_ftp_cli_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF FTP CLI FS READ: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_DEBUG( "BT PF FTP CLI FS READ: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_ftp_cli_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_READ_REQ;
  event.ev_msg.ev_ftp_cli_read_req.conn_id = 
    (bt_pf_ftp_cli_conn_id_type)conn_id;
  event.ev_msg.ev_ftp_cli_read_req.handle = 
    (bt_pf_ftp_handle_type)handle;
  event.ev_msg.ev_ftp_cli_read_req.max_read =
    max_read;

  bt_pf_ftp_cli_read_cb_fptr = read_cfm_fptr;
  
  bt_ec_send_event( &event );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_filesys_write

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to write to a file in the file 
  system.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_ftp_cli_filesys_write( 
  OI_FTP_HANDLE          handle,
  const OI_BYTE*         buffer_ptr,
  OI_UINT16              buffer_len,
  OI_FTP_WRITE_CFM       write_cfm_fptr,
  OI_FTP_CONNECTION      conn_id,
  OI_UINT32              obj_size)
{
  bt_ev_msg_type event;

  /* send event to tell app to write to file */
  if (bt_pf_ftp_cli_conn_id != (bt_pf_ftp_cli_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF FTP CLI FS WRITE: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_DEBUG( "BT PF FTP CLI FS WRITE: Conn: %x, Len 0x%x", 
                conn_id, buffer_len, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_ftp_cli_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_WRITE_REQ;
  event.ev_msg.ev_ftp_cli_write_req.conn_id = 
    (bt_pf_ftp_cli_conn_id_type)conn_id;
  event.ev_msg.ev_ftp_cli_write_req.handle = 
    (bt_pf_ftp_handle_type)handle;
  event.ev_msg.ev_ftp_cli_write_req.buffer_ptr = 
    (byte*)buffer_ptr;
  event.ev_msg.ev_ftp_cli_write_req.buffer_len =
    buffer_len;
  event.ev_msg.ev_ftp_cli_write_req.obj_size = obj_size;

  bt_pf_ftp_cli_write_cb_fptr = write_cfm_fptr;

  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_filesys_set_folder

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to set the current folder in
  the file system.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_ftp_cli_filesys_set_folder( 
  const OI_OBEX_UNICODE* folder_name_ptr,
  OI_UINT8               level,
  OI_BOOL                create,
  OI_FTP_SET_FOLDER_CFM  set_folder_cfm_fptr,
  OI_FTP_CONNECTION      conn_id )
{
  bt_ev_msg_type event;
  uint8          folder_name_len;

  /* send event to tell app to set folder */
  if (bt_pf_ftp_cli_conn_id != (bt_pf_ftp_cli_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF FTP CLI FS SET FOLDER: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_DEBUG( "BT PF FTP CLI FS SET FOLDER: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_ftp_cli_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_FTP_CLI_SET_FOLDER_REQ;
  event.ev_msg.ev_ftp_cli_set_folder_req.conn_id = 
    (bt_pf_ftp_cli_conn_id_type)conn_id;
  event.ev_msg.ev_ftp_cli_set_folder_req.level = level;
  event.ev_msg.ev_ftp_cli_set_folder_req.create = create;

  if ( (folder_name_ptr == NULL) || (folder_name_ptr->len == 0) )
  {
    folder_name_len = 0;
    event.ev_msg.ev_ftp_cli_set_folder_req.folder_name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    folder_name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_ftp_cli_set_folder_req.folder_name_str,
      (bt_pf_unicode_str_type*)folder_name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_ftp_cli_set_folder_req.folder_name_len = folder_name_len;

  bt_pf_ftp_cli_set_folder_cb_fptr = set_folder_cfm_fptr;
  
  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/* Structure storing the pointers to the above callback functions          */
/* This is passed into the driver when bt_pf_ftp_cli_cmd_connect is called */
LOCAL OI_FTP_FILESYS_FUNCTIONS bt_pf_ftp_cli_filesys_ops = 
{
  bt_pf_ftp_cli_filesys_browse, 
  bt_pf_ftp_cli_filesys_open,
  bt_pf_ftp_cli_filesys_close,
  bt_pf_ftp_cli_filesys_read,
  bt_pf_ftp_cli_filesys_write,
  NULL, /* delete callback only applicable to FTP server */
  bt_pf_ftp_cli_filesys_set_folder
};


/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_cmd_connect

DESCRIPTION
  Performs connection to remote FTP server.

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_cli_cmd_connect(
  bt_app_id_type                       app_id,
  bt_bd_addr_type*                     bd_addr_ptr,
  uint8                                channel )
{
  OI_STATUS status;
  bt_pf_ftp_cli_conn_id_type conn_id;

  if ( bt_pf_ftp_cli_conn_id != BT_PF_FTP_NO_CONN_ID )
  {
    BT_MSG_DEBUG( "BT PF FTP CLI CONN: FTP client already connected, App: %x",
                  app_id, 0, 0 );
    return(OI_STATUS_ALREADY_CONNECTED);
  } 

  bt_pf_ftp_cli_app_id = app_id;
  bt_pf_ftp_cli_browse_cb_fptr = NULL;
  bt_pf_ftp_cli_open_cb_fptr = NULL;
  bt_pf_ftp_cli_read_cb_fptr = NULL;
  bt_pf_ftp_cli_write_cb_fptr = NULL;
  bt_pf_ftp_cli_set_folder_cb_fptr = NULL;
  bt_pf_ftp_cli_folder_list_state = FOLDER_LISTING_IDLE;

  status = OI_FTPClient_Connect( 
    (OI_BD_ADDR*)bd_addr_ptr, 
    channel,
    (OI_FTP_CLIENT_CONNECTION_HANDLE*)&conn_id,
    bt_pf_ftp_cli_event_cb,
    bt_pf_ftp_cli_auth_cb,
    &bt_pf_ftp_cli_filesys_ops );

  if (status != OI_OK)
  {
    bt_pf_ftp_cli_app_id = BT_PF_APP_ID_NULL;
    BT_ERR( "BT PF FTP CLI CONN: OI_FTPClient_Connect returns %x", 
                status, 0, 0 );
  }
  else
  {
    bt_pf_ftp_cli_conn_id = conn_id;
  }
  return(status);
}


/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_cmd_auth_rsp

DESCRIPTION
  Responds to authentication request from a remote FTP server.

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_cli_cmd_auth_rsp(
  bt_app_id_type                   app_id,
  bt_pf_cmd_ftp_cli_auth_rsp_type* params_ptr )
{
  OI_STATUS                   status;
  OI_BYTE*                    user_id_ptr;
  OI_CHAR*                    password_ptr;

  if (params_ptr->user_id_len == 0 )
  {
    user_id_ptr = NULL;
  }
  else
  {
    user_id_ptr = (OI_BYTE*)(params_ptr->user_id);
  }

  if (params_ptr->password_str[0] == '\0')
  {
    password_ptr = NULL;
  }
  else
  {
    password_ptr = (OI_CHAR*)(params_ptr->password_str);
  }

  status = OI_FTPClient_AuthenticationRsp( 
    (OI_FTP_CLIENT_CONNECTION_HANDLE)params_ptr->conn_id,
    user_id_ptr,
    params_ptr->user_id_len,
    password_ptr );

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_cmd_abort

DESCRIPTION
  Processes a FTP client abort command.

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_cli_cmd_abort(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id )
{
  OI_STATUS                status;
  OI_FTP_BROWSE_FOLDER_CFM browse_cb_fptr = NULL;
  OI_FTP_READ_CFM          read_cb_fptr = NULL;
  OI_FTP_WRITE_CFM         write_cb_fptr = NULL;
  OI_FTP_SET_FOLDER_CFM    set_folder_cb_fptr = NULL;

  status = OI_FTPClient_Cancel( (OI_FTP_CLIENT_CONNECTION_HANDLE)conn_id, NULL);

  if (status != OI_OK)
  {
    BT_ERR( "BT PF FTP CLI ABORT: OI_FTPClient_Abort returns %x", 
                status, 0, 0 );
  }
  
  TASKLOCK();
  if (bt_pf_ftp_cli_browse_cb_fptr != NULL)
  {
    browse_cb_fptr = bt_pf_ftp_cli_browse_cb_fptr;
    bt_pf_ftp_cli_browse_cb_fptr = NULL;
  }

  if (bt_pf_ftp_cli_read_cb_fptr != NULL)
  {
    read_cb_fptr = bt_pf_ftp_cli_read_cb_fptr;
    bt_pf_ftp_cli_read_cb_fptr = NULL;
  }

  if (bt_pf_ftp_cli_write_cb_fptr != NULL)
  {
    write_cb_fptr = bt_pf_ftp_cli_write_cb_fptr;
    bt_pf_ftp_cli_write_cb_fptr = NULL;
  }

  if (bt_pf_ftp_cli_set_folder_cb_fptr != NULL)
  {
    set_folder_cb_fptr = bt_pf_ftp_cli_set_folder_cb_fptr;
    bt_pf_ftp_cli_set_folder_cb_fptr = NULL;
  }
  TASKFREE();

  if ( browse_cb_fptr )
  {
    browse_cb_fptr( NULL, 0, OI_OBEX_CLIENT_ABORTED_COMMAND, NULL );
  }

  if ( read_cb_fptr )
  {
    read_cb_fptr( NULL, NULL, 0, OI_OBEX_CLIENT_ABORTED_COMMAND, NULL );
  }
  if ( write_cb_fptr )
  {
    write_cb_fptr( NULL, OI_OK, NULL ); 
    /* use OI_OK here so that the underlying OI_OBEXCli_Get is called. 
       This in turn trigger the abort response to be sent */
  }
  if ( set_folder_cb_fptr )
  {
    set_folder_cb_fptr( OI_OBEX_CLIENT_ABORTED_COMMAND, NULL );
  }

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_cmd_browse_done

DESCRIPTION                                   
  Processes a FTP client browse done command.

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_cli_cmd_browse_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status )
{ 
  OI_FTP_BROWSE_FOLDER_CFM browse_cb_fptr = NULL;
  bt_cmd_status_type       ret = OI_OK;

  TASKLOCK();

  if (bt_pf_ftp_cli_browse_cb_fptr != NULL)
  {
    browse_cb_fptr = bt_pf_ftp_cli_browse_cb_fptr;
    bt_pf_ftp_cli_browse_cb_fptr = NULL;
  }
  else
  {
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    BT_ERR( "BT PF FTP CLI BROWSE DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    return( ret );
  }        

  if (conn_id != bt_pf_ftp_cli_conn_id)
  {
    BT_ERR( "BT PF FTP CLI BROWSE DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }            
 

  browse_cb_fptr( (OI_FTP_HANDLE)handle, size, (OI_STATUS)status, 
                  (OI_FTP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_cmd_open_done

DESCRIPTION
  Processes a FTP client open done command.

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_cli_cmd_open_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status )
{                                  
  OI_FTP_OPEN_CFM    open_cb_fptr = NULL;
  bt_cmd_status_type ret = OI_OK;

  TASKLOCK();

  if (bt_pf_ftp_cli_open_cb_fptr != NULL)
  {
    open_cb_fptr = bt_pf_ftp_cli_open_cb_fptr;
    bt_pf_ftp_cli_open_cb_fptr = NULL;
  }
  else
  {
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    BT_ERR( "BT PF FTP CLI OPEN DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    return( ret );
  }        

  if (conn_id != bt_pf_ftp_cli_conn_id)
  {
    BT_ERR( "BT PF FTP CLI OPEN DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }            
    
  open_cb_fptr( (OI_FTP_HANDLE)handle, size, (OI_STATUS)status, 
                (OI_FTP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_cmd_read_done

DESCRIPTION                                   
  Processes a FTP client read done command.
  
===========================================================================*/
bt_cmd_status_type bt_pf_ftp_cli_cmd_read_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        len,
  bt_cmd_status_type            status )
{
  OI_FTP_READ_CFM    read_cb_fptr = NULL;
  bt_cmd_status_type ret = OI_OK;

  TASKLOCK();

  if (bt_pf_ftp_cli_read_cb_fptr != NULL)
  {
    read_cb_fptr = bt_pf_ftp_cli_read_cb_fptr;
    bt_pf_ftp_cli_read_cb_fptr = NULL;
  }
  else
  {
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    BT_ERR( "BT PF FTP CLI READ DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    return( ret );
  }        

  if (conn_id != bt_pf_ftp_cli_conn_id)
  {
    BT_ERR( "BT PF FTP CLI READ DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }            
  
  read_cb_fptr( (OI_FTP_HANDLE)handle, data_ptr, len, (OI_STATUS)status,
                (OI_FTP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_cmd_write_done

DESCRIPTION                                   
  Processes a FTP client write done command.

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_cli_cmd_write_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  bt_cmd_status_type            status )
{
  OI_FTP_WRITE_CFM   write_cb_fptr = NULL;
  bt_cmd_status_type ret = OI_OK;

  if (conn_id != bt_pf_ftp_cli_conn_id)
  {
    BT_ERR( "BT PF FTP CLI WRITE DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }          
  
  TASKLOCK();

  if (bt_pf_ftp_cli_write_cb_fptr != NULL)
  {
    write_cb_fptr = bt_pf_ftp_cli_write_cb_fptr;
    bt_pf_ftp_cli_write_cb_fptr = NULL;
  }
  else
  {
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();
  
  if ( ret != OI_OK )
  {
    BT_ERR( "BT PF FTP CLI WRITE DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    return( ret );
  } 

  write_cb_fptr( (OI_FTP_HANDLE)handle, (OI_STATUS)status, 
                 (OI_FTP_CONNECTION)conn_id );
    
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_cli_cmd_set_folder_done

DESCRIPTION                                   
  Processes a FTP client set folder done command.

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_cli_cmd_set_folder_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id, 
  bt_cmd_status_type            status )
{
  OI_FTP_SET_FOLDER_CFM  set_folder_cb_fptr = NULL;
  bt_cmd_status_type     ret = OI_OK;

  TASKLOCK();

  if (bt_pf_ftp_cli_set_folder_cb_fptr != NULL)
  {
    set_folder_cb_fptr = bt_pf_ftp_cli_set_folder_cb_fptr;
    bt_pf_ftp_cli_set_folder_cb_fptr = NULL;
  }
  else
  {
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    BT_ERR( "BT PF FTP CLI SET FOLDER DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    return( ret );
  }        

  if (conn_id != bt_pf_ftp_cli_conn_id)
  {
    BT_ERR( "BT PF FTP CLI SET FOLDER DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }            

  set_folder_cb_fptr( (OI_STATUS)status, (OI_FTP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_cmd_pf_ftp_cli_write_done

DESCRIPTION                                   
  This command is called by the application when the file write operation is
  completed.

===========================================================================*/
bt_cmd_status_type bt_cmd_pf_ftp_cli_write_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_cli_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  bt_cmd_status_type            status )
{
  bt_pf_cmd_msg_type*   cmd_ptr;
  bt_cmd_status_type    cmd_status;

  if ( bt_pf_ftp_cli_folder_list_state == FOLDER_LISTING_SENT )
  { 
    BT_MSG_API( "BT PF FTP CLI CMD RX: Write Done, AID %x CID %x Stat %x", 
                app_id, conn_id, status );
    BT_MSG_DEBUG( "BT PF FTP CLI WRITE DNE: folder listing ack", 0, 0, 0 );

    if (conn_id != bt_pf_ftp_cli_conn_id)
    {
      BT_ERR( "BT PF FTP CLI WRITE DNE: unknown conn: %x", conn_id, 0, 0 );
      return( OI_OBEX_CONNECTION_NOT_FOUND );
    }
    bt_pf_ftp_cli_folder_list_state = FOLDER_LISTING_ACKNOWLEDGED;

    rex_set_sigs( &bt_pf_tcb, BT_PF_FTP_SIG );
    return( OI_OK );
  }
  else
  {
    if ( (cmd_ptr = bt_pf_get_free_cmd()) == NULL )
    {
      BT_ERR( "BT PF CMD FTP CLI WRT DNE: cmd q full, cmd dropped, App: %x",
              app_id, 0, 0 );
      return(BT_CS_GN_CMD_Q_FULL);
    }
  
    cmd_ptr->cmd_hdr.cmd_type = BT_PF_CMD_FTP_CLI_WRITE_DONE;
    cmd_ptr->cmd_hdr.app_id = app_id;
    cmd_ptr->cmd_msg.cmd_ftp_cli_write_done.conn_id = conn_id;
    cmd_ptr->cmd_msg.cmd_ftp_cli_write_done.handle = handle;
    cmd_ptr->cmd_msg.cmd_ftp_cli_write_done.status = status;
  
    cmd_status = bt_pf_cmd( cmd_ptr );
  }

  return( cmd_status );
}



#endif /* FEATURE_BT_EXTPF_FTP */
#endif /* FEATURE_BT */
