/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 B L U E T O O T H    O B J E C T    P U S H    P R O F I L E    S E R V E R

GENERAL DESCRIPTION
  This module contains the Bluetooth Object Push Profile server data and code.

Copyright (c) 2004 - 2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfopp3.c#1 $

$Log: $
*
*    #24       16 Feb 2007            PN
* Made use of safe string functions.
*
*    #23       28 Nov 2006            RP
* Updated the Debug messages to print the size of the file.
* 
*    #22       18 Oct 2006            PR
* Updated bt_pf_opp_srv_objsys_open_write()while merging 
* OI Code v3.0.14
*
*    #21       14 Jun 2006            GS
* Updated bt_pf_opp_srv_cmd_register() to use revised definition of
* OPP supported formats attribute.
*
*    #20       16 Jun 2006            DSN
* Removed superfluous header file inclusion.
*
*    #19       12 Jun 2006            DSN
* Typecasting fixes for RVCT compiler.
*
*    #18       25 Apr 2006            JH
* Fixed bug in handling supported format list during server registration.
*
*    #17       31 May 2005            RY
* Merged in Sirius changes.
*
*    #16        3 May 2005            JH
* Clear app ID and function pointers only when deregister is successful.
*
*    #15       20 Apr 2005            JH
* Converted file operation done functions into queued commands.
*
*    #14       19 Jan 2005            JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*
*    #13       29 Dec 2004            JH
* Added SCN parameter when calling bt_cmd_sd_update_obex_profile_service_record
*
*    #12        1 Sep 2004            JH
* Modified bt_cmd_pf_opp_srv_open_read_done such that parameter checking will
*  only be performed when a non-error status is passed in.
*
*    #11       30 Aug 2004            JH
* Modified the registration command to call 
*  bt_cmd_sd_update_obex_profile_service_record for updating service record 
*  instead of calling SD functions directly.
* Removed calls for manipulating object transfer bit in class of device field;
*  this is now handled by SD.
*
*    #10       26 Jul 2004            JH
* Revamped object system interface and eliminated the use of timeouts in object 
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
* Made changes related to the replacement of bt_pf_unicode_str_type command 
*  parameters with uint16* type
* Modified to rex_timed_wait calls to properly implement timeout
*
*    #4        25 Feb 2004            JH
* Added logic to determine if object transfer bit in class of service field
* should be cleared, depending on whether an FTP server is also registered
*
*    #3        25 Feb 2004            JH
* Renamed btpfopp.h btpfoppi.h
*
*    #2        24 Feb 2004            JH
* Added macro calls to set and clear object transfer bit in class of
* service field during OPP server register/deregister.
*

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_OPP

#ifdef FEATURE_MT_OBEX
#include "mt_obex_opp.h"
#endif /* FEATURE_MT_OBEX */


#include "bt.h"
#include "btpfi.h"
#include "btpf.h"
#include "btpfcmdi.h"
#include "bti.h"
#include "btutils.h"
#include "btmsg.h"
#include "btsd.h"
#include "btpfoppi.h"
#include "rex.h" 
#include "opp_server.c"  

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Currently, only one OPP server is supported */
LOCAL bt_app_id_type             bt_pf_opp_srv_app_id = BT_PF_APP_ID_NULL;
LOCAL bt_pf_opp_srv_conn_id_type bt_pf_opp_srv_conn_id = BT_PF_OPP_NO_CONN_ID;
LOCAL uint8                      bt_pf_opp_scn = BT_SPP_SCN_UNSPECIFIED;

/* this string is passed into OI_FTPServer_Register only as a place holder
   because the OI SDP calls within OI_FTPServer_Register are not used */
LOCAL const OI_SDP_STRINGS dummy_opp_sdp_str = { NULL, 0 };

LOCAL OI_OPP_OPEN_READ_CFM  bt_pf_opp_srv_open_read_cfm_fptr  = NULL;
LOCAL OI_OPP_OPEN_WRITE_CFM bt_pf_opp_srv_open_write_cfm_fptr = NULL;
LOCAL OI_OPP_READ_CFM       bt_pf_opp_srv_read_cfm_fptr       = NULL;
LOCAL OI_OPP_WRITE_CFM      bt_pf_opp_srv_write_cfm_fptr      = NULL;

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_con_ind_cb

DESCRIPTION
  Callback function for indicating client connection.
  Pointer to this function is passed to OI_OPPServer_Register.

===========================================================================*/
LOCAL void bt_pf_opp_srv_con_ind_cb(
  OI_BD_ADDR* client_addr_ptr,
  OI_OPP_SERVER_CONNECTION_HANDLE conn_id )
{ 
  bt_ev_msg_type event;

  BT_MSG_DEBUG( "BT PF OPP SRV CON IND CB, Conn: %x", conn_id, 0, 0 );

  bt_pf_opp_srv_conn_id = (bt_pf_opp_srv_conn_id_type)conn_id;
  event.ev_hdr.ev_type = BT_EV_PF_OPP_SRV_CON_IND;
  event.ev_hdr.bt_app_id = bt_pf_opp_srv_app_id;

  event.ev_msg.ev_opp_srv_con_ind.conn_id = 
    (bt_pf_opp_srv_conn_id_type)conn_id;
  memcpy( (void*)&(event.ev_msg.ev_opp_srv_con_ind.bd_addr), 
          (void*)client_addr_ptr, sizeof( bt_bd_addr_type ) );

  bt_ec_send_event( &event );
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_dcn_ind_cb

DESCRIPTION
  Callback function for indicating client is disconnecting.
  Pointer to this function is passed to OI_OPPServer_Register.

===========================================================================*/
LOCAL void bt_pf_opp_srv_dcn_ind_cb( 
  OI_OPP_SERVER_CONNECTION_HANDLE conn_id )
{
  bt_ev_msg_type event;

  if (bt_pf_opp_srv_conn_id == (bt_pf_opp_srv_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF OPP SRV DCN IND CB, Conn: %x", conn_id, 0, 0 );

    bt_pf_opp_srv_open_read_cfm_fptr = NULL;
    bt_pf_opp_srv_open_write_cfm_fptr = NULL;
    bt_pf_opp_srv_read_cfm_fptr = NULL;
    bt_pf_opp_srv_write_cfm_fptr = NULL;
  
    event.ev_hdr.ev_type = BT_EV_PF_OPP_SRV_DCN_IND;
    event.ev_hdr.bt_app_id = bt_pf_opp_srv_app_id;
  
    event.ev_msg.ev_opp_srv_con_ind.conn_id = 
      (bt_pf_opp_srv_conn_id_type)conn_id;
  
    bt_pf_opp_srv_conn_id = BT_PF_OPP_NO_CONN_ID;
    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF OPP SRV DCN IND CB, unknown conn: %x",
            conn_id, 0, 0 );
  }
  return;
} 

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_objsys_open_read

DESCRIPTION
  Callback function supplied to the OPP driver. This function is invoked
  by the driver to instruct the application to open an object for reading.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_opp_srv_objsys_open_read(
  const OI_OBEX_UNICODE* name_ptr,
  const OI_CHAR*         type_ptr,
  OI_OPP_OPEN_READ_CFM   open_read_cfm_fptr,
  OI_OPP_CONNECTION      conn_id )
{
  bt_ev_msg_type  event;
  uint8           name_len;

  /* send event to tell app to open object for reading */
  if (bt_pf_opp_srv_conn_id != (bt_pf_opp_srv_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF OPP SRV OS OPEN READ: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
   
  BT_MSG_DEBUG( "BT PF OPP SRV OS OPEN READ: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_opp_srv_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_OPP_SRV_OPEN_READ_REQ;
  event.ev_msg.ev_opp_srv_open_read_req.conn_id = 
    (bt_pf_opp_srv_conn_id_type)conn_id;
  
  if ( (name_ptr == NULL) || (name_ptr->len == 0) )
  {
    name_len = 0;
    event.ev_msg.ev_opp_srv_open_read_req.name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_opp_srv_open_read_req.name_str,
      (bt_pf_unicode_str_type*)name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_opp_srv_open_read_req.name_len = name_len;
  
  if (type_ptr == NULL)
  {
    event.ev_msg.ev_opp_srv_open_read_req.type_str[0] = 0;
  }
  else
  {
    /* type would be invalid if length exceeds limit, let app reject it */ 
    BT_STRCPY( (char *)event.ev_msg.ev_opp_srv_open_read_req.type_str, 
               type_ptr, 
               sizeof(event.ev_msg.ev_opp_srv_open_read_req.type_str) );
    event.ev_msg.ev_opp_srv_open_read_req.type_str[BT_PF_MAX_MIME_TYPE_LEN] = 
      '\0'; /* doesn't hurt if type string is shorter */
  }

  bt_pf_opp_srv_open_read_cfm_fptr = open_read_cfm_fptr;

  bt_ec_send_event( &event );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_objsys_open_write

DESCRIPTION
  Callback function supplied to the OPP driver. This function is invoked
  by the driver to instruct the application to open an object for writing.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_opp_srv_objsys_open_write(
  const OI_OBEX_UNICODE* name_ptr,
  const OI_CHAR*         type_ptr,
  OI_UINT32              obj_size,
  OI_OPP_OPEN_WRITE_CFM  open_write_cfm_fptr,
  OI_OPP_CONNECTION      conn_id )
{
  bt_ev_msg_type event;
  uint8          name_len;
  
  /* send event to tell app to open object for writing */
  if (bt_pf_opp_srv_conn_id != (bt_pf_opp_srv_conn_id_type)conn_id)
  { 
    BT_ERR( "BT PF OPP SRV OS OPEN WRITE: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }

  BT_MSG_DEBUG( "BT PF OPP SRV OS OPEN WRITE: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_opp_srv_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_OPP_SRV_OPEN_WRITE_REQ;
  event.ev_msg.ev_opp_srv_open_write_req.conn_id = 
    (bt_pf_opp_srv_conn_id_type)conn_id;
  event.ev_msg.ev_opp_srv_open_write_req.obj_size =
    obj_size;                           

  if ( (name_ptr == NULL) || (name_ptr->len == 0) )
  {
    name_len = 0;
    event.ev_msg.ev_opp_srv_open_write_req.name_str[0] = 0;
  }
  else
  {
    /* copy name, truncate when necessary */
    name_len = bt_pf_process_unicode_str(
      (uint16*)event.ev_msg.ev_opp_srv_open_write_req.name_str,
      (bt_pf_unicode_str_type*)name_ptr,
      BT_PF_MAX_FILENAME_LEN );
  }
  event.ev_msg.ev_opp_srv_open_write_req.name_len = name_len;  
  
  if (type_ptr == NULL)
  {
    event.ev_msg.ev_opp_srv_open_write_req.type_str[0] = 0;
  }
  else
  {
    /* type would be invalid if length exceeds limit, let app reject it */ 
    BT_STRCPY( (char *)event.ev_msg.ev_opp_srv_open_write_req.type_str, 
               type_ptr, 
               sizeof(event.ev_msg.ev_opp_srv_open_write_req.type_str) );
    event.ev_msg.ev_opp_srv_open_write_req.type_str[BT_PF_MAX_MIME_TYPE_LEN] = 
      '\0'; /* doesn't hurt if type string is shorter */
  }

  bt_pf_opp_srv_open_write_cfm_fptr = open_write_cfm_fptr;

  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_objsys_close

DESCRIPTION
  Callback function supplied to the OPP driver. This function is invoked
  by the driver to instruct the application to close an object.
  
===========================================================================*/
LOCAL void bt_pf_opp_srv_objsys_close(
  OI_OPP_HANDLE          handle,
  OI_STATUS              status,
  OI_OPP_CONNECTION      conn_id )
{
  bt_ev_msg_type event;

  /* send event to tell app to close object */
  if (bt_pf_opp_srv_conn_id == (bt_pf_opp_srv_conn_id_type)conn_id)
  {
    BT_MSG_DEBUG( "BT PF OPP SRV OS CLOSE: Conn: %x", 
                  conn_id, 0, 0 ); 

    event.ev_hdr.bt_app_id = bt_pf_opp_srv_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_OPP_SRV_CLOSE_REQ;
    event.ev_msg.ev_opp_srv_close_req.conn_id = 
      (bt_pf_opp_srv_conn_id_type)conn_id;
    event.ev_msg.ev_opp_srv_close_req.handle =
      (bt_pf_opp_handle_type)handle;
    event.ev_msg.ev_opp_srv_close_req.status =
      (bt_cmd_status_type)status;

    bt_ec_send_event( &event );
  }
  else
  {
    BT_ERR( "BT PF OPP SRV OS CLOSE: unknown conn: %x", 
            conn_id, 0, 0 );
  }
  /* no respond from app is expected, object is assumed closed */
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_objsys_read

DESCRIPTION
  Callback function supplied to the OPP driver. This function is invoked
  by the driver to instruct the application to read an object.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_opp_srv_objsys_read(
  OI_OPP_HANDLE          handle,
  OI_UINT16              max_read,
  OI_OPP_READ_CFM        read_cfm_fptr,
  OI_OPP_CONNECTION      conn_id )
{
  bt_ev_msg_type event;

  /* send event to tell app to read object */
  if (bt_pf_opp_srv_conn_id != (bt_pf_opp_srv_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF OPP SRV OS READ: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_DEBUG( "BT PF OPP SRV OS READ: Conn: %x", 
                conn_id, 0, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_opp_srv_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_OPP_SRV_READ_REQ;
  event.ev_msg.ev_opp_srv_read_req.conn_id = 
    (bt_pf_opp_srv_conn_id_type)conn_id;
  event.ev_msg.ev_opp_srv_read_req.handle =
    (bt_pf_opp_handle_type)handle;
  event.ev_msg.ev_opp_srv_read_req.max_read =
    max_read;

  bt_pf_opp_srv_read_cfm_fptr = read_cfm_fptr;

  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_objsys_write

DESCRIPTION
  Callback function supplied to the OPP driver. This function is invoked
  by the driver to instruct the application to write to an object.
  
===========================================================================*/
LOCAL OI_STATUS bt_pf_opp_srv_objsys_write(
  OI_OPP_HANDLE          handle,
  const OI_BYTE*         buffer_ptr,
  OI_UINT16              buffer_len,
  OI_OPP_WRITE_CFM       write_cfm_ptr,
  OI_OPP_CONNECTION      conn_id )
{
  bt_ev_msg_type event;

  /* send event to tell app to write to object */
  if (bt_pf_opp_srv_conn_id != (bt_pf_opp_srv_conn_id_type)conn_id)
  {
    BT_ERR( "BT PF OPP SRV OS WRITE: unknown conn: %x", 
            conn_id, 0, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_DEBUG( "BT PF OPP SRV OS WRITE: Conn: %x Len 0x%x", 
                conn_id, buffer_len, 0 ); 

  event.ev_hdr.bt_app_id = bt_pf_opp_srv_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_OPP_SRV_WRITE_REQ;
  event.ev_msg.ev_opp_srv_write_req.conn_id = 
    (bt_pf_opp_srv_conn_id_type)conn_id;
  event.ev_msg.ev_opp_srv_write_req.handle =
    (bt_pf_opp_handle_type)handle;
  event.ev_msg.ev_opp_srv_write_req.buffer_ptr =
    (byte*)buffer_ptr;
  event.ev_msg.ev_opp_srv_write_req.buffer_len =
    buffer_len;

  bt_pf_opp_srv_write_cfm_fptr = write_cfm_ptr;

  bt_ec_send_event( &event );
  
  return(OI_OK);
}

/* Structure storing the pointers to the above callback functions          */
/* This is passed into the driver when bt_pf_opp_srv_cmd_connect is called */
LOCAL OI_OPP_OBJSYS_FUNCTIONS bt_pf_opp_srv_objsys_ops =
{
  bt_pf_opp_srv_objsys_open_read,
  bt_pf_opp_srv_objsys_open_write,
  bt_pf_opp_srv_objsys_close,
  bt_pf_opp_srv_objsys_read,
  bt_pf_opp_srv_objsys_write
};

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_cmd_register

DESCRIPTION
  Perform initialization of object push server and service discovery 
  database registration.

===========================================================================*/
bt_cmd_status_type bt_pf_opp_srv_cmd_register(
  bt_app_id_type                app_id,   
  uint32                        supported_formats,
  char*                         srv_name_ptr )
{
  OI_STATUS                    status = OI_OK;
  uint8                        i;
  bt_sd_uint8_list_type        supported_formats_list;
  uint32                       mask;

  TASKLOCK();

  if ( bt_pf_opp_srv_app_id != BT_PF_APP_ID_NULL )
  {
    status = OI_STATUS_ALREADY_REGISTERED;
  }
  else
  {
    bt_pf_opp_srv_app_id = app_id;
  } 

  TASKFREE();

  if ( status != OI_OK )
  {
    BT_ERR( "BT PF OPP SRV REG: alrdy reg, App: %x, Stat: %x",
            app_id, status, 0 );
    return(status);
  }

  BT_MSG_API( "BT PF OPP SRV REG: supported_formats: %x", supported_formats, 0, 0 ); 

  supported_formats_list.num_val = 0;
  mask = 1;
  for (i = 0; i < BT_PF_NUM_OBEX_OBJ_TYPES; i++)
  {
    if (supported_formats & mask)
    {
      supported_formats_list.val[ supported_formats_list.num_val ] = i + 1;
      supported_formats_list.num_val++;
    }
    mask = mask << 1;
  }

  /* Any other formats (e.g, JPG) */
  if ((supported_formats & OI_OPP_SERVER_OBJ_FORMAT_ANY) == OI_OPP_SERVER_OBJ_FORMAT_ANY)
  {
    supported_formats_list.val[ supported_formats_list.num_val ] = 
      OI_OBEX_OBJ_FORMAT_ANY;
    supported_formats_list.num_val++;
  }

  /* Sanity check */
  if ( supported_formats_list.num_val == 0 )
  {                                     
    BT_ERR( "BT PF OPP SRV REG: invalid format list: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  bt_pf_opp_srv_open_read_cfm_fptr = NULL;
  bt_pf_opp_srv_open_write_cfm_fptr = NULL;
  bt_pf_opp_srv_read_cfm_fptr = NULL;
  bt_pf_opp_srv_write_cfm_fptr = NULL;
  
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */

  status = OI_OPPServer_Register( 
    bt_pf_opp_srv_con_ind_cb, 
    bt_pf_opp_srv_dcn_ind_cb,
    &bt_pf_opp_srv_objsys_ops,
    (OI_OPP_SERVER_OBJECT_FORMATS)supported_formats,
    &dummy_opp_sdp_str,
    &bt_pf_opp_scn );

  if (status != OI_OK)
  {
    BT_ERR( "BT PF OPP SRV REG: OI_OPPServer_Register returns %x", 
                status, 0, 0 );
    bt_pf_opp_srv_app_id = BT_PF_APP_ID_NULL;
    return(status);
  }

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#else /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */

  /* Updates service record with additional profile specific attributes */
  status = bt_cmd_sd_update_obex_profile_service_record(
             app_id,
             BT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH,
             bt_pf_opp_scn,
             srv_name_ptr,
             NULL, /* service ID */
             &supported_formats_list,
             0,    /* supported capabilities */
             0,    /* supported features */
             0,    /* supported functions */
             0 );  /* imaging capacity */
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */
  
  if (status != BT_CS_GN_PENDING)
  {
    BT_ERR( "BT PF OPP SRV REG: update srv rec failed, App %x", 
            app_id, 0, 0 );
    OI_OPPServer_Deregister( bt_pf_opp_scn );
    return(status);
  }

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_cmd_deregister

DESCRIPTION
  Terminates the Object Push server and deregisters it from the SDP database.

===========================================================================*/
bt_cmd_status_type bt_pf_opp_srv_cmd_deregister(
  bt_app_id_type                app_id ) 
{
  OI_STATUS           status;

  if (bt_pf_opp_srv_app_id == BT_PF_APP_ID_NULL)
  {
    BT_ERR( "BT PF OPP SRV DEREG: not reg, App: %x, Stat: %x",
            app_id, OI_STATUS_NOT_REGISTERED, 0 );
    return(OI_STATUS_NOT_REGISTERED);
  }

  if (bt_pf_opp_srv_app_id != app_id)
  {
    BT_ERR( "BT PF OPP SRV DEREG: unknown app: %x, Stat: %x", 
            app_id, OI_STATUS_INVALID_PARAMETERS, 0 );
    return(OI_STATUS_INVALID_PARAMETERS);
  }

  status = OI_OPPServer_Deregister( bt_pf_opp_scn );
  if (status != OI_OK)
  {
    BT_ERR( "BT PF OPP SRV DEREG: OI_OPPServer_Deregister returns %x", 
                status, 0, 0 );
  }
  else
  {
    bt_pf_opp_srv_app_id = BT_PF_APP_ID_NULL;
    bt_pf_opp_srv_open_read_cfm_fptr = NULL;
    bt_pf_opp_srv_open_write_cfm_fptr = NULL;
    bt_pf_opp_srv_read_cfm_fptr = NULL;
    bt_pf_opp_srv_write_cfm_fptr = NULL;
  }

  return(status);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_cmd_open_read_done

DESCRIPTION
  Processes an OPP server open write done command.

===========================================================================*/
bt_cmd_status_type bt_pf_opp_srv_cmd_open_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
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

  if (bt_pf_opp_srv_open_read_cfm_fptr != NULL)
  {
    open_read_cb_fptr = bt_pf_opp_srv_open_read_cfm_fptr;
    bt_pf_opp_srv_open_read_cfm_fptr = NULL;
  }
  else
  {
    BT_ERR( "BT PF OPP SRV OPEN READ DNE: no event sent, App: %x, Stat: %x",
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
    if (conn_id != bt_pf_opp_srv_conn_id)
    { 
      BT_ERR( "BT PF OPP SRV OPEN READ DNE: App: %x, unknown conn: %x", 
              app_id, conn_id, 0 );
      return(OI_OBEX_CONNECTION_NOT_FOUND);
    }                                              
  
    name.len = (uint16)name_len;
    name.str = name_ptr;
  }

  BT_MSG_API( "BT PF OPP SRV OPEN READ DNE: App: %x, Conn: %x, Stat: %x", 
                app_id, conn_id, status );

  open_read_cb_fptr( (OI_OPP_HANDLE)handle, &name, type_ptr, size, 
                     (OI_STATUS)status, (OI_OPP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_cmd_open_write_done

DESCRIPTION
  Processes an OPP server open write done command.

===========================================================================*/
bt_cmd_status_type bt_pf_opp_srv_cmd_open_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status )
{
  OI_OPP_OPEN_WRITE_CFM open_write_cb_fptr = NULL;
  bt_cmd_status_type    ret = OI_OK;

  TASKLOCK();

  if (bt_pf_opp_srv_open_write_cfm_fptr != NULL)
  {
    open_write_cb_fptr = bt_pf_opp_srv_open_write_cfm_fptr;
    bt_pf_opp_srv_open_write_cfm_fptr = NULL;
  }
  else
  {
    BT_ERR( "BT PF OPP SRV OPEN WRITE DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    return( ret );
  }             

  if (conn_id != bt_pf_opp_srv_conn_id)
  {
    BT_ERR( "BT PF OPP SRV OPEN WRITE DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }            
  
  BT_MSG_API( "BT PF OPP SRV OPEN WRITE DNE: App: %x, Conn: %x, Stat: %x", 
              app_id, conn_id, status );

  open_write_cb_fptr( (OI_OPP_HANDLE)handle, (OI_STATUS)status, 
                      (OI_OPP_CONNECTION)conn_id );
                                              
  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_cmd_read_done

DESCRIPTION
  Processes an OPP server read done command.

===========================================================================*/
bt_cmd_status_type bt_pf_opp_srv_cmd_read_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  byte*                         data_ptr,
  uint16                        data_len,
  bt_cmd_status_type            status )
{
  OI_OPP_READ_CFM    read_cb_fptr = NULL;
  bt_cmd_status_type ret = OI_OK;

  TASKLOCK();

  if (bt_pf_opp_srv_read_cfm_fptr != NULL)
  {
    read_cb_fptr = bt_pf_opp_srv_read_cfm_fptr;
    bt_pf_opp_srv_read_cfm_fptr = NULL;
  }
  else
  {
    BT_ERR( "BT PF OPP SRV READ DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    return( ret );
  }             

  if (conn_id != bt_pf_opp_srv_conn_id)
  {
    BT_ERR( "BT PF OPP SRV READ DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }            
  
  BT_MSG_API( "BT PF OPP SRV READ DNE: App: %x, Conn: %x, Stat: %x", 
              app_id, conn_id, status );

  read_cb_fptr( (OI_OPP_HANDLE)handle, data_ptr, data_len, (OI_STATUS)status,
                (OI_OPP_CONNECTION)conn_id );

  return(OI_OK);
}

/*===========================================================================

FUNCTION
  bt_pf_opp_srv_cmd_write_done

DESCRIPTION
  Processes an OPP server write done command.

===========================================================================*/
bt_cmd_status_type bt_pf_opp_srv_cmd_write_done(
  bt_app_id_type                app_id,
  bt_pf_opp_srv_conn_id_type    conn_id,
  bt_pf_opp_handle_type         handle,
  bt_cmd_status_type            status )
{ 
  OI_OPP_WRITE_CFM   write_cb_fptr = NULL;
  bt_cmd_status_type ret = OI_OK;

  TASKLOCK();

  if (bt_pf_opp_srv_write_cfm_fptr != NULL)
  {
    write_cb_fptr = bt_pf_opp_srv_write_cfm_fptr;
    bt_pf_opp_srv_write_cfm_fptr = NULL;
  }
  else
  {
    BT_ERR( "BT PF OPP SRV WRITE DNE: no event sent, App: %x, Stat: %x",
            app_id, OI_STATUS_INTERNAL_ERROR, 0 );
    ret = OI_STATUS_INTERNAL_ERROR;
  }

  TASKFREE();

  if ( ret != OI_OK )
  {
    return( ret );
  }             

  if (conn_id != bt_pf_opp_srv_conn_id)
  {
    BT_ERR( "BT PF OPP SRV WRITE DNE: App: %x, unknown conn: %x", 
            app_id, conn_id, 0 );
    return(OI_OBEX_CONNECTION_NOT_FOUND);
  }
  
  BT_MSG_API( "BT PF OPP SRV WRITE DNE: App: %x, Conn: %x, Stat: %x", 
              app_id, conn_id, status );

  write_cb_fptr( (OI_OPP_HANDLE)handle, (OI_STATUS)status, 
                 (OI_OPP_CONNECTION)conn_id );

  return(OI_OK);
}

#endif /* FEATURE_BT_EXTPF_OPP */
#endif /* FEATURE_BT */
