/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 B L U E T O O T H   F I L E   T R A N S F E R   P R O F I L E   S E R V E R

GENERAL DESCRIPTION
  This module contains the Bluetooth File Transfer Profile server data and code.

Copyright (c) 2004-2009 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfftp3.c#2 $ 
$DateTime: 2009/04/17 11:28:04 $
$Author: ganeshs $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2009-01-14   gs  Updated bt_pf_ftp_srv_filesys_write() function signature.
  2008-08-14   rb  Added support for DCVS.

*    #20       28 Nov 2006            RP
* Updated the Debug messages to print the size of the file.
*
*    #19       10 Oct 2006            GS
* Modified bt_pf_ftp_srv_filesys_open() to return object size in file open
* request event.
*
*    #18       16 Jun 2006           DSN
* Removed superfluous header file inclusion.
*
*    #17       13 Jul 2005            JH
* Replaced BT_PF_OPP_NO_CONN_ID with BT_PF_FTP_NO_CONN_ID.
*
*    #16        8 Jul 2005            JH
* Added status parameter to bt_pf_ftp_srv_filesys_close().
*
*    #15        3 May 2005            JH
* Clear app ID and function pointers only when deregister is successful.
*
*    #14       12 Apr 2005            JH
* Converted file operation done functions into queued commands.
*
*    #13       19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #12       29 Dec 2004            JH
* Added SCN parameter when calling bt_cmd_sd_update_obex_profile_service_record
*
*    #11       30 Aug 2004            JH
* Modified the registration command to call 
*  bt_cmd_sd_update_obex_profile_service_record for updating service record 
*  instead of calling SD functions directly.
* Removed calls for manipulating object transfer bit in class of device field;
*  this is now handled by SD.
*
*    #10       26 Jul 2004            JH
* Revamped file system interface and eliminated the use of timeouts in file 
*  system interactions.
*
*    #9        14 May 2004            JH
* Modified code for setting object transfer bit in class od service field
*
*    #7        23 Apr 2004            JH
* Replaced all bt_pf_cmd* with bt_cmd_pf*; replaced all BT_PF_EV* with
*  BT_EV_PF*
*      
*    #6        14 Apr 2004            JH
* Modified to rex_timed_wait calls to properly implement timeout
* Implemented OBEX server reference count for setting/clearing OBEX class of
*  service bit
*
*    #4        25 Feb 2004            JH
* Added logic to determine if object transfer bit in class of service field
* should be cleared, depending on whether an OPP server is also registered
*
*    #3        25 Feb 2004            JH
* Renamed btpfftp.h btpfftpi.h
*
*    #2        24 Feb 2004            JH
* Added macro calls to set and clear object transfer bit in class of
* service field during FTP server register/deregister.
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
#include "btpf.h"
#include "bti.h"
#include "btmsg.h"
#include "btsd.h"
#include "btpfftpi.h"
#include "rex.h"
#include "ftp_server.c"       

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Currently, only one FTP server is supported */
LOCAL bt_app_id_type             bt_pf_ftp_srv_app_id = BT_PF_APP_ID_NULL;
LOCAL bt_pf_ftp_srv_conn_id_type bt_pf_ftp_srv_conn_id = BT_PF_FTP_NO_CONN_ID;
LOCAL uint8                      bt_pf_ftp_scn = BT_SPP_SCN_UNSPECIFIED;

LOCAL OI_FTP_BROWSE_FOLDER_CFM bt_pf_ftp_srv_browse_cb_fptr = NULL;
LOCAL OI_FTP_OPEN_CFM          bt_pf_ftp_srv_open_cb_fptr = NULL;
LOCAL OI_FTP_READ_CFM          bt_pf_ftp_srv_read_cb_fptr = NULL;
LOCAL OI_FTP_WRITE_CFM         bt_pf_ftp_srv_write_cb_fptr = NULL;
LOCAL OI_FTP_SET_FOLDER_CFM    bt_pf_ftp_srv_set_folder_cb_fptr = NULL;
LOCAL OI_FTP_DELETE_CFM        bt_pf_ftp_srv_delete_cb_fptr = NULL;

/* this string is passed into OI_FTPServer_Register only as a place holder
   because the OI SDP calls within OI_FTPServer_Register are not used */
LOCAL const OI_SDP_STRINGS dummy_ftp_sdp_str = { NULL, 0 };

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_con_ind_cb

DESCRIPTION
  Callback function for indicating client connection.
  Pointer to this function is passed to OI_FTPServer_Register.

===========================================================================*/
LOCAL void bt_pf_ftp_srv_con_ind_cb( 
  OI_BD_ADDR*                     client_addr_ptr,
  OI_BOOL                         unauthorized,
  OI_BYTE*                        user_id_ptr,
  OI_UINT8                        user_id_len,
  OI_FTP_SERVER_CONNECTION_HANDLE conn_id )
{ 
  bt_ev_msg_type event;

  BT_MSG_DEBUG( "BT PF FTP SRV CON IND CB, Conn: %x", conn_id, 0, 0 );

  event.ev_hdr.ev_type = BT_EV_PF_FTP_SRV_CON_IND;
  event.ev_hdr.bt_app_id = bt_pf_ftp_srv_app_id;
  event.ev_msg.ev_ftp_srv_con_ind.conn_id = 
    (bt_pf_ftp_srv_conn_id_type)conn_id;
  event.ev_msg.ev_ftp_srv_con_ind.unauthorized = unauthorized;
  event.ev_msg.ev_ftp_srv_con_ind.bd_addr = *(bt_bd_addr_type*)client_addr_ptr;

  /* OBEX should have rejected auth digest with invalid user id length */
  if (user_id_len > BT_PF_OBEX_MAX_USERID_LEN)
  {
    /* truncate just in case */
    user_id_len = BT_PF_OBEX_MAX_USERID_LEN;
  }
  if (user_id_len > 0)
  {                  
    memcpy( (void*)event.ev_msg.ev_ftp_srv_con_ind.user_id,
            (void*)user_id_ptr,
            user_id_len );
  }
  event.ev_msg.ev_ftp_srv_con_ind.user_id_len = user_id_len;

  bt_pf_ftp_srv_conn_id = conn_id;

  bt_ec_send_event( &event );

  return;
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_dcn_ind_cb

DESCRIPTION
  Callback function for indicating client is disconnecting.
  Pointer to this function is passed to OI_FTPServer_Register.

===========================================================================*/
LOCAL void bt_pf_ftp_srv_dcn_ind_cb(OI_FTP_SERVER_CONNECTION_HANDLE conn_id)
{
  bt_ev_msg_type event;

  if (bt_pf_ftp_srv_conn_id == (bt_pf_ftp_srv_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF FTP SRV DCN IND CB, Conn: %x", conn_id, 0, 0 );
  
    bt_pf_ftp_srv_browse_cb_fptr = NULL;
    bt_pf_ftp_srv_open_cb_fptr = NULL;
    bt_pf_ftp_srv_read_cb_fptr = NULL;
    bt_pf_ftp_srv_write_cb_fptr = NULL;
    bt_pf_ftp_srv_delete_cb_fptr = NULL;
    bt_pf_ftp_srv_set_folder_cb_fptr = NULL;

    event.ev_hdr.ev_type = BT_EV_PF_FTP_SRV_DCN_IND;
    event.ev_hdr.bt_app_id = bt_pf_ftp_srv_app_id;
  
    event.ev_msg.ev_ftp_srv_dcn_ind.conn_id = 
      (bt_pf_ftp_srv_conn_id_type)conn_id;
  
    bt_pf_ftp_srv_conn_id = BT_PF_FTP_NO_CONN_ID;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF FTP SRV DCN IND CB, unknown conn: %x",
            conn_id, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_filesys_browse

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to browse a folder in the file 
  system.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_ftp_srv_filesys_browse(
  const OI_OBEX_UNICODE*   folder_name_ptr,
  OI_FTP_BROWSE_FOLDER_CFM browse_cfm_fptr,
  OI_FTP_CONNECTION        conn_id )
{
  bt_ev_msg_type event;
  uint8          folder_name_len;

  /* send event to tell app to browse folder */
  if (bt_pf_ftp_srv_conn_id != (bt_pf_ftp_srv_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF FTP SRV FS BROWSE: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  } 
  
  BT_MSG_DEBUG( "BT PF FTP SRV FS BROWSE: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_ftp_srv_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_FTP_SRV_BROWSE_REQ;
  event.ev_msg.ev_ftp_srv_browse_req.conn_id = 
    (bt_pf_ftp_srv_conn_id_type)conn_id;

  if ( (folder_name_ptr == NULL) || (folder_name_ptr->len == 0) )
  {
    folder_name_len = 0;
    event.ev_msg.ev_ftp_srv_browse_req.folder_name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    folder_name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_ftp_srv_browse_req.folder_name_str,
      (bt_pf_unicode_str_type*)folder_name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_ftp_srv_browse_req.folder_name_len = folder_name_len;

  bt_pf_ftp_srv_browse_cb_fptr = browse_cfm_fptr;

  bt_ec_send_event( &event );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_filesys_open

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to open a file in the file 
  system.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_ftp_srv_filesys_open( 
  const OI_OBEX_UNICODE*   name_ptr,
  OI_FTP_MODE              mode,
  OI_UINT32                obj_size,
  OI_FTP_OPEN_CFM          open_cfm_fptr,
  OI_FTP_CONNECTION        conn_id)
{
  bt_ev_msg_type event;
  uint8          name_len;

  /* send event to tell app to open file */
  if (bt_pf_ftp_srv_conn_id != (bt_pf_ftp_srv_conn_id_type)conn_id){
    BT_ERR( "BT PF FTP SRV FS OPEN: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_DEBUG( "BT PF FTP SRV FS OPEN: Conn: %x", 
                conn_id, 0, 0 ); 
  bt_cmd_rm_update_bt_module_state (bt_pf_ftp_srv_app_id,
                                    BT_MODULE_ID_FTP,
                                    BT_MODULE_ST_ACTIVE);

  event.ev_hdr.bt_app_id = bt_pf_ftp_srv_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_FTP_SRV_OPEN_REQ;
  event.ev_msg.ev_ftp_srv_open_req.conn_id = 
    (bt_pf_ftp_srv_conn_id_type)conn_id;
  event.ev_msg.ev_ftp_srv_open_req.mode =
    (bt_pf_ftp_mode_type)mode;

  if ( (name_ptr == NULL) || (name_ptr->len == 0) )
  {
    name_len = 0;
    event.ev_msg.ev_ftp_srv_open_req.name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_ftp_srv_open_req.name_str,
      (bt_pf_unicode_str_type*)name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_ftp_srv_open_req.name_len = name_len;
  event.ev_msg.ev_ftp_srv_open_req.obj_size = obj_size;

  bt_pf_ftp_srv_open_cb_fptr = open_cfm_fptr;

  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_filesys_close

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to close a file in the file 
  system.
  
===========================================================================*/
LOCAL void bt_pf_ftp_srv_filesys_close( 
  OI_FTP_HANDLE            handle,
  OI_FTP_CONNECTION        conn_id,
  OI_STATUS                status )
{
  bt_ev_msg_type event;

  /* send event to tell app to close file */
  if (bt_pf_ftp_srv_conn_id == (bt_pf_ftp_srv_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF FTP SRV FS CLOSE: Conn: %x", 
                  conn_id, 0, 0 ); 

    event.ev_hdr.bt_app_id = bt_pf_ftp_srv_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_FTP_SRV_CLOSE_REQ;
    event.ev_msg.ev_ftp_srv_close_req.conn_id = 
      (bt_pf_ftp_srv_conn_id_type)conn_id;
    event.ev_msg.ev_ftp_srv_close_req.handle = 
      (bt_pf_ftp_handle_type)handle;
    event.ev_msg.ev_ftp_srv_close_req.status = 
      (bt_cmd_status_type)status;
    bt_ec_send_event( &event );
    bt_cmd_rm_update_bt_module_state (bt_pf_ftp_srv_app_id,
                                      BT_MODULE_ID_FTP,
                                      BT_MODULE_ST_IDLE);
  }
  else
  {
    BT_ERR( "BT PF FTP SRV FS CLOSE: unknown conn: %x", 
            conn_id, 0, 0 );            
  }
  /* no respond from app is expected, file is assumed closed */
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_filesys_read

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to read a file in the file 
  system.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_ftp_srv_filesys_read( 
  OI_FTP_HANDLE            handle,
  OI_UINT16                max_read,
  OI_FTP_READ_CFM          read_cfm_fptr,
  OI_FTP_CONNECTION        conn_id )
{
  bt_ev_msg_type event;

  /* send event to tell app to read file */
  if (bt_pf_ftp_srv_conn_id != (bt_pf_ftp_srv_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF FTP SRV FS READ: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_DEBUG( "BT PF FTP SRV FS READ: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_ftp_srv_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_FTP_SRV_READ_REQ;
  event.ev_msg.ev_ftp_srv_read_req.conn_id = 
    (bt_pf_ftp_srv_conn_id_type)conn_id;
  event.ev_msg.ev_ftp_srv_read_req.handle = 
    (bt_pf_ftp_handle_type)handle;
  event.ev_msg.ev_ftp_srv_read_req.max_read =
    max_read;

  bt_pf_ftp_srv_read_cb_fptr = read_cfm_fptr;

  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_filesys_write

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to write to a file in the file 
  system.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_ftp_srv_filesys_write( 
  OI_FTP_HANDLE            handle,
  const OI_BYTE*           buffer_ptr,
  OI_UINT16                buffer_len,
  OI_FTP_WRITE_CFM         write_cfm_fptr,
  OI_FTP_CONNECTION        conn_id,
  OI_UINT32                obj_size)
{
  bt_ev_msg_type event;

  /* send event to tell app to write to file */
  if (bt_pf_ftp_srv_conn_id != (bt_pf_ftp_srv_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF FTP SRV FS WRITE: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_DEBUG( "BT PF FTP SRV FS WRITE: Conn: %x Len 0x%x", 
                conn_id, buffer_len, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_ftp_srv_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_FTP_SRV_WRITE_REQ;
  event.ev_msg.ev_ftp_srv_write_req.conn_id = 
    (bt_pf_ftp_srv_conn_id_type)conn_id;
  event.ev_msg.ev_ftp_srv_write_req.handle = 
    (bt_pf_ftp_handle_type)handle;
  event.ev_msg.ev_ftp_srv_write_req.buffer_ptr = 
    (byte*)buffer_ptr;
  event.ev_msg.ev_ftp_srv_write_req.buffer_len =
    buffer_len;

  bt_pf_ftp_srv_write_cb_fptr = write_cfm_fptr;

  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_filesys_delete

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to delete a file in the file 
  system.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_ftp_srv_filesys_delete(
  const OI_OBEX_UNICODE*   name_ptr,
  OI_FTP_DELETE_CFM        delete_cfm_fptr,
  OI_FTP_CONNECTION        conn_id )
{
  bt_ev_msg_type event;
  uint8          name_len;

  /* send event to tell app to delete file */
  if (bt_pf_ftp_srv_conn_id != (bt_pf_ftp_srv_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF FTP SRV FS DELETE: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_DEBUG( "BT PF FTP SRV FS DELETE: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_ftp_srv_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_FTP_SRV_DELETE_REQ;
  event.ev_msg.ev_ftp_srv_delete_req.conn_id = 
    (bt_pf_ftp_srv_conn_id_type)conn_id;

  if ( (name_ptr == NULL) || (name_ptr->len == 0) )
  {
    name_len = 0;
    event.ev_msg.ev_ftp_srv_delete_req.name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_ftp_srv_delete_req.name_str,
      (bt_pf_unicode_str_type*)name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_ftp_srv_delete_req.name_len = name_len;

  bt_pf_ftp_srv_delete_cb_fptr = delete_cfm_fptr;
  
  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_filesys_set_folder

DESCRIPTION
  Callback function supplied to the FTP driver. This function is invoked
  by the driver to instruct the application to set the current folder in
  the file system.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_ftp_srv_filesys_set_folder( 
  const OI_OBEX_UNICODE*   folder_name_ptr,
  OI_UINT8                 level,
  OI_BOOL                  create,
  OI_FTP_SET_FOLDER_CFM    set_folder_cfm_fptr,
  OI_FTP_CONNECTION        conn_id )
{
  bt_ev_msg_type event;
  uint8          folder_name_len;

  /* send event to tell app to set folder */
  if (bt_pf_ftp_srv_conn_id != (bt_pf_ftp_srv_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF FTP SRV FS SET FOLDER: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_DEBUG( "BT PF FTP SRV FS SET FOLDER: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_ftp_srv_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_FTP_SRV_SET_FOLDER_REQ;
  event.ev_msg.ev_ftp_srv_set_folder_req.conn_id = 
    (bt_pf_ftp_srv_conn_id_type)conn_id;
  event.ev_msg.ev_ftp_srv_set_folder_req.level = 
    level;
  event.ev_msg.ev_ftp_srv_set_folder_req.create =
    create;

  if ( (folder_name_ptr == NULL) || (folder_name_ptr->len == 0) )
  {
    folder_name_len = 0;
    event.ev_msg.ev_ftp_srv_set_folder_req.folder_name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    folder_name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_ftp_srv_set_folder_req.folder_name_str,
      (bt_pf_unicode_str_type*)folder_name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_ftp_srv_set_folder_req.folder_name_len = folder_name_len;

  bt_pf_ftp_srv_set_folder_cb_fptr = set_folder_cfm_fptr;

  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/* Structure storing the pointers to the above callback functions           */
/* This is passed into the driver when bt_cmd_pf_ftp_srv_register is called */
LOCAL OI_FTP_FILESYS_FUNCTIONS bt_pf_ftp_srv_filesys_ops = 
{
  bt_pf_ftp_srv_filesys_browse,
  bt_pf_ftp_srv_filesys_open,
  bt_pf_ftp_srv_filesys_close,
  bt_pf_ftp_srv_filesys_read,
  bt_pf_ftp_srv_filesys_write,
  bt_pf_ftp_srv_filesys_delete,
  bt_pf_ftp_srv_filesys_set_folder
};

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_cmd_register

DESCRIPTION
  Initialize the file transfer server and register it in the service discovery 
  database.

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_srv_cmd_register(
  bt_app_id_type                      app_id,
  bt_pf_goep_srv_auth_type            auth,
  boolean                             read_only,
  boolean                             allow_browse,            
  char*                               srv_name_ptr )
{
  OI_STATUS                    status = OI_OK;

  TASKLOCK();

  if ( bt_pf_ftp_srv_app_id != BT_PF_APP_ID_NULL )
  {
    status = OI_STATUS_ALREADY_REGISTERED;
  }
  else
  {
    bt_pf_ftp_srv_app_id = app_id;
  } 

  TASKFREE();

  if ( status != OI_OK )
  {
    BT_ERR( "BT PF FTP SRV REG: alrdy reg, App: %x, Stat: %x",
            app_id, status, 0 );
    return(status);
  }

  bt_pf_ftp_srv_browse_cb_fptr = NULL;
  bt_pf_ftp_srv_open_cb_fptr = NULL;
  bt_pf_ftp_srv_read_cb_fptr = NULL;
  bt_pf_ftp_srv_write_cb_fptr = NULL;
  bt_pf_ftp_srv_delete_cb_fptr = NULL;
  bt_pf_ftp_srv_set_folder_cb_fptr = NULL;
  
  status = OI_FTPServer_Register( 
    (OI_OBEXSRV_AUTHENTICATION)auth,
    read_only,
    allow_browse,
    bt_pf_ftp_srv_con_ind_cb, 
    bt_pf_ftp_srv_dcn_ind_cb,
    &bt_pf_ftp_srv_filesys_ops,
    &dummy_ftp_sdp_str,
    &bt_pf_ftp_scn );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF OPP SRV REG: OI_FTPServer_Register returns %x", 
            status, 0, 0 );
    bt_pf_ftp_srv_app_id = BT_PF_APP_ID_NULL;
    return(status);
  }

  /* Updates service record with additional profile specific attributes */ 
  status = bt_cmd_sd_update_obex_profile_service_record(
             app_id,
             BT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER,
             bt_pf_ftp_scn,
             srv_name_ptr,
             NULL, /* service ID */
             NULL, /* supported formats list */
             0,    /* supported capabilities */
             0,    /* supported features */
             0,    /* supported functions */
             0 );  /* imaging capacity */
  
  if (status != BT_CS_GN_PENDING)
  {
    BT_ERR( "BT PF FTP SRV REG: update srv rec failed, App %x", 
            app_id, 0, 0 );
    OI_FTPServer_Deregister( bt_pf_ftp_scn );
    return(status);
  } 

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_cmd_deregister

DESCRIPTION
  This function terminates the File Transfer server and deregisters it from 
  the SDP database.

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_srv_cmd_deregister(
  bt_app_id_type                app_id ) 
{
  OI_STATUS           status;

  if (bt_pf_ftp_srv_app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF FTP SRV DEREG: not reg, App: %x, Stat: %x",
            app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }

  if (app_id != bt_pf_ftp_srv_app_id)
  {
    BT_ERR( "BT PF OPP SRV DEREG: unknown app: %x, Stat: %x",
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = OI_FTPServer_Deregister( bt_pf_ftp_scn );
  if (status != OI_OK)
  {
    BT_ERR( "BT PF FTP SRV DEREG: OI_FTPServer_Deregister returns %x", 
            status, 0, 0 );
  }
  else
  {
    bt_pf_ftp_srv_app_id = BT_PF_APP_ID_NULL;
    bt_pf_ftp_srv_browse_cb_fptr = NULL;
    bt_pf_ftp_srv_open_cb_fptr = NULL;
    bt_pf_ftp_srv_read_cb_fptr = NULL;
    bt_pf_ftp_srv_write_cb_fptr = NULL;
    bt_pf_ftp_srv_delete_cb_fptr = NULL;
    bt_pf_ftp_srv_set_folder_cb_fptr = NULL;
  }

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_cmd_browse_done

DESCRIPTION
  Processes a FTP server browse done command.                                   

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_srv_cmd_browse_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status )
{
  OI_FTP_BROWSE_FOLDER_CFM browse_cb_fptr = NULL;
  bt_cmd_status_type       ret = OI_OK;

  TASKLOCK();

  if (bt_pf_ftp_srv_browse_cb_fptr != NULL)
  {
    browse_cb_fptr = bt_pf_ftp_srv_browse_cb_fptr;
    bt_pf_ftp_srv_browse_cb_fptr = NULL;
  }
  else
  {
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    BT_ERR( "BT PF FTP SRV BROWSE DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    return( ret );
  }        

  if (conn_id != bt_pf_ftp_srv_conn_id)
  {
    BT_ERR( "BT PF FTP SRV BROWSE DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  browse_cb_fptr( (OI_FTP_HANDLE)handle, size, (OI_STATUS)status, 
                  (OI_FTP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_cmd_open_done

DESCRIPTION                                   
  Processes a FTP server open done command.                                   

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_srv_cmd_open_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  uint32                        size,
  bt_cmd_status_type            status )
{
  OI_FTP_OPEN_CFM    open_cb_fptr = NULL;
  bt_cmd_status_type ret = OI_OK;

  TASKLOCK();

  if (bt_pf_ftp_srv_open_cb_fptr != NULL)
  {
    open_cb_fptr = bt_pf_ftp_srv_open_cb_fptr;
    bt_pf_ftp_srv_open_cb_fptr = NULL;
  }
  else
  {
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    BT_ERR( "BT PF FTP SRV OPEN DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    return( ret );
  }        

  if (conn_id != bt_pf_ftp_srv_conn_id)
  {
    BT_ERR( "BT PF FTP SRV OPEN DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
    
  open_cb_fptr( (OI_FTP_HANDLE)handle, size, (OI_STATUS)status, 
                (OI_FTP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_cmd_read_done

DESCRIPTION                                   
  Processes a FTP server read done command.                                   

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_srv_cmd_read_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        len,
  bt_cmd_status_type            status )
{
  OI_FTP_READ_CFM    read_cb_fptr = NULL;
  bt_cmd_status_type ret = OI_OK;

  TASKLOCK();

  if (bt_pf_ftp_srv_read_cb_fptr != NULL)
  {
    read_cb_fptr = bt_pf_ftp_srv_read_cb_fptr;
    bt_pf_ftp_srv_read_cb_fptr = NULL;
  }
  else
  {
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    BT_ERR( "BT PF FTP SRV READ DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    return( ret );
  }        

  if (conn_id != bt_pf_ftp_srv_conn_id)
  {
    BT_ERR( "BT PF FTP SRV READ DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }            
  
  read_cb_fptr( (OI_FTP_HANDLE)handle, data_ptr, len, (OI_STATUS)status,
                (OI_FTP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_cmd_write_done

DESCRIPTION                                   
  Processes a FTP server write done command.                                   

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_srv_cmd_write_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id,
  bt_pf_ftp_handle_type         handle,
  bt_cmd_status_type            status )
{
  OI_FTP_WRITE_CFM   write_cb_fptr = NULL;
  bt_cmd_status_type ret = OI_OK;
  
  TASKLOCK();

  if (bt_pf_ftp_srv_write_cb_fptr != NULL)
  {
    write_cb_fptr = bt_pf_ftp_srv_write_cb_fptr;
    bt_pf_ftp_srv_write_cb_fptr = NULL;
  }
  else
  {
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();
  
  if ( ret != OI_OK )
  {
    BT_ERR( "BT PF FTP SRV WRITE DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    return( ret );
  } 

  if (conn_id != bt_pf_ftp_srv_conn_id)
  {
    BT_ERR( "BT PF FTP SRV WRITE DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }           

  write_cb_fptr( (OI_FTP_HANDLE)handle, (OI_STATUS)status, 
                 (OI_FTP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_cmd_delete_done

DESCRIPTION                                   
  Processes a FTP server delete done command.                                   

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_srv_cmd_delete_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id, 
  bt_cmd_status_type            status )
{
  OI_FTP_DELETE_CFM  delete_cb_fptr = NULL;
  bt_cmd_status_type ret = OI_OK;
  
  TASKLOCK();

  if (bt_pf_ftp_srv_delete_cb_fptr != NULL)
  {
    delete_cb_fptr = bt_pf_ftp_srv_delete_cb_fptr;
    bt_pf_ftp_srv_delete_cb_fptr = NULL;
  }
  else
  {
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();
  
  if ( ret != OI_OK )
  {
    BT_ERR( "BT PF FTP SRV DELETE DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    return( ret );
  } 
  if (conn_id != bt_pf_ftp_srv_conn_id)
  {
    BT_ERR( "BT PF FTP SRV DELETE DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }            
  
  delete_cb_fptr( (OI_STATUS)status, (OI_FTP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_ftp_srv_cmd_set_folder_done

DESCRIPTION                                   
  Processes a FTP server set folder done command.                                   

===========================================================================*/
bt_cmd_status_type bt_pf_ftp_srv_cmd_set_folder_done(
  bt_app_id_type                app_id,
  bt_pf_ftp_srv_conn_id_type    conn_id, 
  bt_cmd_status_type            status )
{
  OI_FTP_SET_FOLDER_CFM  set_folder_cb_fptr = NULL;
  bt_cmd_status_type     ret = OI_OK;

  TASKLOCK();

  if (bt_pf_ftp_srv_set_folder_cb_fptr != NULL)
  {
    set_folder_cb_fptr = bt_pf_ftp_srv_set_folder_cb_fptr;
    bt_pf_ftp_srv_set_folder_cb_fptr = NULL;
  }
  else
  {
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    BT_ERR( "BT PF FTP SRV SET FOLDER DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    return( ret );
  }        

  if (conn_id != bt_pf_ftp_srv_conn_id)
  {
    BT_ERR( "BT PF FTP SRV SET FOLDER DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }            

  set_folder_cb_fptr( (OI_STATUS)status, (OI_FTP_CONNECTION)conn_id );

  return(OI_OK);
}

#endif /* FEATURE_BT_EXTPF_FTP */
#endif /* FEATURE_BT */
