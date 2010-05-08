/*****************************************************************************
***
***  TITLE
***  MT OBEX API Implementation
***
***
***  DESCRIPTION
***  This contains the Implementation of the Multi Transport AMSS OBEX.
***
***
***  Copyright (c) 2004-2006 QUALCOMM Incorporated.
***  All Rights Reserved. QUALCOMM Proprietary.
***
***  Export of this technology or software is regulated by the U.S.
***  Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
***  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/mt_obex.c#1 $
***
***  when       who     what, where, why
***  --------   ---     ----------------------------------------------------------
***  17/01/08   PG      Resolved Compiler Warnings
***  16/02/07   MH      Added support for disconnect response functionality.
***  29/12/07   MH      Added support for 128-bit UUID OBEX server.
***  10/11/06   MH      Modified mt_obex_client_init_data
***  26/10/06   MH      Modified server confirm set path to support all headers
***  25/09/06   MH      Added new function to get client BD Address from server handle.
***  19/09/06   MH      Server Authentication function added.
***  25/08/06   BK      Fixed compiler issues. 
***  01/08/06   BK      Removed compiler warnings.
***  29/07/06   BK      Modified to support OBEX client initiated authentication
***  10/07/06   JH      Removed extra linefeeds introduced in previous checkin.
***  23/06/06   JH      Modified to support updated GOEP API for JSR82.
***  28/03/06   JH      Modofied to not set transport ID bits for BT transport  
***                     when calling bt_cmd_pf_goep_cli_connect().
***                     Fixed indentation.
***  22/10/05   JH      Replaced FEATURE_PCAA with FEATURE_PCAA_SIRIUS.
***  24/11/05   ABu     Added Interface
***  25/01/05   ABu     Added FEATURE_MT_OBEX flag around all mt_obex code
***  14/02/05   ABu     Added FEATURE_PCAA_SIRIUS for Sirius project, PC Application
***                     Agent is a server to support Sirius Mobile Phone Manager
***                     or MPM running on PC
***  14/04/05   ABu     Added mt_obex_srv_register_secondary_target()
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/



#include "target.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT_EXTPF_GOEP
#ifdef FEATURE_MT_OBEX

#include "mt_obex.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/


/************************ Generic Functions *********************************/

/*===========================================================================

FUNCTION mt_obex_get_transports_supported

DESCRIPTION
  This function can be used by an application to find out what transports are
  supported for OBEX to run on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_tp_support_type mt_obex_get_transports_supported(void)
{
  /* change this code if different transports need to be supported, this
     will only need to change most likely on a project by project basis */
  return (MT_OBEX_BLUETOOTH_TP | MT_OBEX_IRDA_TP | MT_OBEX_USB_TP);
}


/************************ client functions *********************************/



/*===========================================================================

FUNCTION mt_obex_client_init_data

DESCRIPTION
  Initializes the client authentication data.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/

void mt_obex_client_init_data (uint16 index)
{
    bt_cmd_pf_goep_cli_init_data();
}


/*===========================================================================

FUNCTION mt_obex_client_authenticate

DESCRIPTION
  This command indicates OBEX Client to initiate the authentication procedure.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/

mt_obex_status_type mt_obex_client_authenticate(
                                 mt_obex_tp_conn_struct_type *transport_params,
                                 mt_realm_info *realm_info_ptr,
                                 boolean user_id,
                                 boolean full_access)
{
    return (mt_obex_status_type)bt_cmd_pf_goep_cli_authenticate
           ((bt_bd_addr_type*)(transport_params->transport_conn.bt_conn.bd_addr_ptr),
            transport_params->transport_conn.bt_conn.channel,
            (bt_pf_realm_info_type *)realm_info_ptr,
            user_id,
            full_access
           );
}


/*===========================================================================

FUNCTION mt_obex_server_authenticate

DESCRIPTION
  This command indicates OBEX to authenticate the client, the OBEX authenticates 
  the incoming connection if this command is been issued.  
  
DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/

mt_obex_status_type mt_obex_server_authenticate(
                                 mt_obex_tp_conn_struct_type *transport_params,
                                 uint16 conn_id,
                                 mt_realm_info *realm_info_ptr,
                                 boolean user_id,
                                 boolean full_access)
{
    return (mt_obex_status_type)bt_cmd_pf_goep_srv_authenticate
           ( conn_id,
            (bt_bd_addr_type*)(transport_params->transport_conn.bt_conn.bd_addr_ptr),
            transport_params->transport_conn.bt_conn.channel,
            (bt_pf_realm_info_type *)realm_info_ptr,
            user_id,
            full_access
           );
}

/*===========================================================================

FUNCTION mt_obex_cli_auth_response

DESCRIPTION
  Responds to an authentication indication. This command responds to an
  authentication indication, which was previously indicated to the application
  by the MT_OBEX_EV_CLI_AUTH_IND event. A NULL password indicates that the
  application is rejecting the authentication challenge.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/

mt_obex_status_type mt_obex_cli_auth_response(
  mt_obex_app_id_type                  app_id,
  mt_obex_cli_conn_id_type             conn_id,
  byte*                                user_id_ptr,
  uint8                                user_id_len,
  char*                                password_ptr )
{

  return ( (mt_obex_status_type)bt_cmd_pf_goep_cli_auth_response
            (app_id, conn_id, user_id_ptr, user_id_len, password_ptr) );
}

/*===========================================================================

FUNCTION mt_obex_cli_connect

DESCRIPTION
  This command connects a client to an OBEX server and thus begins an OBEX session.
  This command establishes a connection to an OBEX server. For Bluetooth this
  includes setting up an RFCOMM connection to the OBEX server TBD. For IrDA
  this request potentially initiates device discovery and service discovery
  and establishes all necessary layers of connections (i.e. IrLAP/IrLMP/TinyTP).
  A MT_OBEX_EV_CLI_CON_CFM event will be generated to indicate if the connection
  is successful.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/

mt_obex_status_type mt_obex_cli_connect(
  mt_obex_tp_conn_struct_type          *transport_params,
  mt_obex_app_id_type                  app_id,
  mt_obex_hdr_list_type*               headers_ptr)
{
  uint8 transport_id = 0;
  mt_obex_hdr_list_type headers = { NULL, 0 };

  if ( headers_ptr != NULL )
  {
    headers = *headers_ptr;
  }

  switch(transport_params->transport)
  {
    case MT_OBEX_BLUETOOTH_TRANSPORT:
      /* no need to set the transport ID bits for BT */
      return ( (mt_obex_status_type)bt_cmd_pf_goep_cli_connect(app_id,
                 transport_params->transport_conn.bt_conn.bd_addr_ptr,
                 transport_params->transport_conn.bt_conn.channel,
                 headers,
                 transport_params->transport_conn.bt_conn.uuid,
                 (bt_security_type)transport_params->transport_conn.bt_conn.security) );

    case MT_OBEX_IRDA_TRANSPORT:
      transport_id = MT_OBEX_IRDA_TP_ID;
      return ( (mt_obex_status_type)bt_cmd_pf_goep_cli_connect(app_id, NULL, transport_id,
                 headers, 0, (bt_security_type)0) );

    case MT_OBEX_USB_TRANSPORT:
      transport_id = MT_OBEX_USB_TP_ID;
      return ( (mt_obex_status_type)bt_cmd_pf_goep_cli_connect(app_id, NULL, transport_id,
                 headers, 0, (bt_security_type)0) );

    case MT_OBEX_RS232_TRANSPORT:
      transport_id = MT_OBEX_RS232_TP_ID;
      return ( (mt_obex_status_type)bt_cmd_pf_goep_cli_connect(app_id, NULL, transport_id,
                 headers, 0, (bt_security_type)0) );

    default:
      return ( (mt_obex_status_type)bt_cmd_pf_goep_cli_connect(app_id,
                 transport_params->transport_conn.bt_conn.bd_addr_ptr,
                 transport_params->transport_conn.bt_conn.channel,
                 headers,
                 transport_params->transport_conn.bt_conn.uuid,
                 (bt_security_type)transport_params->transport_conn.bt_conn.security) );
  }
}

/*===========================================================================

FUNCTION mt_obex_cli_disconnect

DESCRIPTION
  This command disconnects a client from an OBEX server and thus ends an OBEX
  session. This command attempts to tear down the specified connection with
  a remote OBEX server.  For IrDA this command will also tear down the underlying
  transport connection if this transport connection was established during the
  mt_obex_cli_connect() call otherwise just the OBEX session will be
  disconnected/terminated. A MT_OBEX_EV_CLI_DCN_IND event will be generated to
  indicate completion of the disconnection.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_cli_disconnect(
  mt_obex_app_id_type                  app_id,
  mt_obex_cli_conn_id_type             conn_id,
  mt_obex_hdr_list_type*               headers_ptr )
{
  mt_obex_hdr_list_type headers = { NULL, 0 };

  if ( headers_ptr != NULL )
  {
    headers = *headers_ptr;
  }
  return ( (mt_obex_status_type)bt_cmd_pf_goep_cli_disconnect(app_id, conn_id, headers) );
}

/*===========================================================================

FUNCTION mt_obex_cli_put

DESCRIPTION
  Puts data to an OBEX server. The bt_pf_goep_cli_put_cfm_cb() callback
  indicates that the server has received and acknowledged receipt of the
  data and then the MT_OBEX_EV_CLI_PUT_CFM event will be forwarded to the
  application. Headers must not be freed until acknowledged.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_cli_put(
  mt_obex_app_id_type                  app_id,
  mt_obex_cli_conn_id_type             conn_id,
  mt_obex_hdr_list_type*               headers_ptr,
  mt_obex_status_type                  status )
{
  mt_obex_hdr_list_type headers = { NULL, 0 };

  if ( headers_ptr != NULL )
  {
    headers = *headers_ptr;
  }

  return ( (mt_obex_status_type)bt_cmd_pf_goep_cli_put(app_id, conn_id, headers, (bt_cmd_status_type)status) );
}

/*===========================================================================

FUNCTION mt_obex_cli_get

DESCRIPTION
  Get data from the OBEX server. This will cause one or more packets to be
  retrieved from an OBEX server. Headers must not be freed until acknowledged.
  A MT_OBEX_EV_CLI_GET_RCV_DATA event is returned to the application when an
  OBEX get request returns data.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_cli_get(
  mt_obex_app_id_type                  app_id,
  mt_obex_cli_conn_id_type             conn_id,
  mt_obex_hdr_list_type*               headers_ptr,
  boolean                              final )
{
  mt_obex_hdr_list_type headers = { NULL, 0 };

  if ( headers_ptr != NULL )
  {
    headers = *headers_ptr;
  }

  return ( (mt_obex_status_type)bt_cmd_pf_goep_cli_get(app_id, conn_id, headers, final) );
}

/*===========================================================================

FUNCTION mt_obex_cli_set_path

DESCRIPTION
  This command can be used to set the current folder for getting and putting
  objects and can be used to create new folders. A MT_OBEX_EV_CLI_SET_PATH_CFM
  event will be sent to the application if this request is successful.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_cli_set_path(
  mt_obex_app_id_type                  app_id,
  mt_obex_cli_conn_id_type             conn_id,
  uint16*                              folder_ptr,
  boolean                              up_level,
  boolean                              dont_create,
  mt_obex_hdr_list_type*               headers_ptr )
{
  mt_obex_hdr_list_type headers = { NULL, 0 };

  if ( headers_ptr != NULL )
  {
    headers = *headers_ptr;
  }

  return ( (mt_obex_status_type)bt_cmd_pf_goep_cli_set_path(app_id, conn_id, 
             folder_ptr, up_level, dont_create, headers) );
}

/*===========================================================================

FUNCTION mt_obex_cli_abort_operation

DESCRIPTION
  Abort the current OBEX operation. Terminate the current put or get operation
  on this connection by replacing the next GET or PUT command by an ABORT command.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_cli_abort_operation(
  mt_obex_app_id_type                  app_id,
  mt_obex_cli_conn_id_type             conn_id )
{
  return ( (mt_obex_status_type)bt_cmd_pf_goep_cli_abort_operation(app_id, conn_id) );
}


/************************ server functions *********************************/

/*===========================================================================

FUNCTION mt_obex_srv_register

DESCRIPTION
  Register an OBEX server. This command registers an OBEX server to accept put
  and/or get requests from OBEX clients. This command is returned immediately
  and is not placed on the command queue.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_register(
  mt_obex_app_id_type                  app_id,
  mt_obex_byte_seq_type*               target_ptr,
  mt_obex_srv_auth_type                auth,
  mt_obex_tp_config_struct_type        *tp_config_params,
  uint8*                               ch_num )
{
  /* at the moment only BT transport wants to configure settings but this
     implementation leaves it open for other transports to potentially
     configure service records etc in future */
  mt_obex_status_type status = MT_OBEX_SUCCESS;
  if(tp_config_params == NULL)
  {
    return MT_OBEX_UNSUPPORTED_TP;
  }

  if( (tp_config_params->transports & MT_OBEX_USB_TP) )
  {
    /* check whether this transport is supported */
    if( !(mt_obex_get_transports_supported() & MT_OBEX_USB_TP) )
    {
      return MT_OBEX_UNSUPPORTED_TP;
    }
    /* USB only OBEX server */
    if(tp_config_params->transports == MT_OBEX_USB_TP)
    {
      (*ch_num) = MT_OBEX_USB_TP_ID;
    }
  }
  if( (tp_config_params->transports & MT_OBEX_IRDA_TP) )
  {
    /* check whether this transport is supported */
    if( !(mt_obex_get_transports_supported() & MT_OBEX_IRDA_TP) )
    {
      return MT_OBEX_UNSUPPORTED_TP;
    }
    /* IrDA only OBEX server */
    if(tp_config_params->transports == MT_OBEX_IRDA_TP)
    {
      (*ch_num) = MT_OBEX_IRDA_TP_ID;
    }
  }
  if( (tp_config_params->transports & MT_OBEX_RS232_TP) )
  {
    /* check whether this transport is supported */
    if( !(mt_obex_get_transports_supported() & MT_OBEX_RS232_TP) )
    {
      return MT_OBEX_UNSUPPORTED_TP;
    }
    /* RS232 only OBEX server */
    if(tp_config_params->transports == MT_OBEX_RS232_TP)
    {
      (*ch_num) = MT_OBEX_RS232_TP_ID;
    }
  }

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif

  if( (tp_config_params->transports & MT_OBEX_BLUETOOTH_TP) &&
      mt_obex_get_transports_supported() & MT_OBEX_BLUETOOTH_TP )
  {
    /* for bluetooth keep the original first 5 bits of channel num */
    if(tp_config_params->transports == MT_OBEX_BLUETOOTH_TP)
    {
      (*ch_num) = ((*ch_num) | MT_OBEX_BLUETOOTH_TP_ID);
    }
    status =  ( (mt_obex_status_type)bt_cmd_pf_goep_srv_register(app_id, target_ptr, (bt_pf_goep_srv_auth_type)auth,
               tp_config_params->bt_config.uuid,
               (bt_security_type)tp_config_params->bt_config.security, ch_num) );
     /* This is to check if the AUTH req event has to be given to the
      * Java APP
      */
     if(status == MT_OBEX_SUCCESS)
     {
         if(tp_config_params->bt_config.authRequired)
         {
             status = (mt_obex_status_type)bt_cmd_pf_goep_srv_reg_auth_cb
                  (app_id,
                   (*ch_num),
                   tp_config_params->bt_config.authRequired);
         }
  }
     return status;
  }
  else
  {
    return ( (mt_obex_status_type)bt_cmd_pf_goep_srv_register(app_id, target_ptr, (bt_pf_goep_srv_auth_type)auth,
               0, (bt_security_type)MT_OBEX_SEC_NONE, ch_num) );
  }
}

/*===========================================================================

FUNCTION mt_obex_srv_register_secondary_target

DESCRIPTION
  Register a second target with an already registered OBEX server. Then this
  server will handle OBEX client connects for both of these targets.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_register_secondary_target(
  mt_obex_app_id_type                  app_id,
  mt_obex_byte_seq_type*               target_ptr,
  mt_obex_srv_auth_type                auth,
  uint8                                ch_num,
  uint8*                               target_id )
{
  return ( (mt_obex_status_type)bt_cmd_pf_goep_srv_register_secondary_target(app_id, target_ptr,
             (bt_pf_goep_srv_auth_type)auth, ch_num, target_id) );
}

/*===========================================================================

FUNCTION mt_obex_srv_accept_connect

DESCRIPTION
  Accept or reject a connect request. Accept or reject a connection from an
  OBEX client when a MT_OBEX_EV_SRV_CON_IND event is received from OBEX.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_accept_connect(
  mt_obex_app_id_type                  app_id,
  uint16                               conn_id,
  boolean                              accept,
  mt_obex_hdr_list_type*               headers_ptr,
  mt_obex_status_type                  status )
{
  mt_obex_hdr_list_type headers = { NULL, 0 };

  if ( headers_ptr != NULL )
  {
    headers = *headers_ptr;
  }

  return ( (mt_obex_status_type)bt_cmd_pf_goep_srv_accept_connect(app_id, conn_id, accept, 
             headers, (bt_cmd_status_type)status) );
}

/*===========================================================================

FUNCTION mt_obex_srv_confirm_set_path

DESCRIPTION
  Respond to a SETPATH command. Send SETPATH response to OBEX client when a
  MT_OBEX_EV_SRV_SET_PATH_IND event is received from OBEX.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_confirm_set_path(
  mt_obex_app_id_type                  app_id,
  uint16                               conn_id,
  mt_obex_hdr_list_type*               headers_ptr,
  mt_obex_status_type                  status )
{
  mt_obex_hdr_list_type headers = { NULL, 0 };

  if ( headers_ptr != NULL )
  {
    headers = *headers_ptr;
  }
  return ( (mt_obex_status_type)bt_cmd_pf_goep_srv_confirm_set_path(app_id, conn_id, headers, (bt_cmd_status_type)status) );
}

/*===========================================================================

FUNCTION mt_obex_srv_auth_response

DESCRIPTION
  Response to a connect indication which reported that the connection
  was unauthorized. If the password is correct and authentication
  completes another connect indication will be issued.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_auth_response(
  mt_obex_app_id_type                  app_id,
  uint16                               conn_id,
  const char*                          password_ptr,
  boolean                              read_only )
{
  return ( (mt_obex_status_type)bt_cmd_pf_goep_srv_auth_response(app_id, conn_id, 
             password_ptr, read_only) );
}

/*===========================================================================

FUNCTION mt_obex_srv_get_response

DESCRIPTION
  Respond to a GET command. When application receives a MT_OBEX_EV_SRV_GET_IND
  event this function will respond to a GET command by sending data to a
  connected OBEX client.


DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_get_response(
  mt_obex_app_id_type                  app_id,
  uint16                               conn_id,
  mt_obex_hdr_list_type*               headers_ptr,
  mt_obex_status_type                  status )
{
  mt_obex_hdr_list_type headers = { NULL, 0 };

  if ( headers_ptr != NULL )
  {
    headers = *headers_ptr;
  }

  return ( (mt_obex_status_type)bt_cmd_pf_goep_srv_get_response (app_id, conn_id, headers, (bt_cmd_status_type)status) );
}

/*===========================================================================

FUNCTION mt_obex_srv_put_response

DESCRIPTION
  Respond to a PUT command. When application receives a MT_OBEX_EV_SRV_PUT_IND
  event this function will acknowledge a PUT command, optionally sending response
  headers to a connected OBEX client.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_put_response(
  mt_obex_app_id_type                  app_id,
  uint16                               conn_id,
  mt_obex_hdr_list_type*               headers_ptr,
  mt_obex_status_type                  status )
{
  mt_obex_hdr_list_type headers = { NULL, 0 };

  if ( headers_ptr != NULL )
  {
    headers = *headers_ptr;
  }

  return ( (mt_obex_status_type)bt_cmd_pf_goep_srv_put_response (app_id, conn_id, headers, (bt_cmd_status_type)status) );
}

/*===========================================================================

FUNCTION mt_obex_srv_disconnect_response

DESCRIPTION
  Respond to a Disconnect command. When application receives a MT_OBEX_EV_SRV_DCN_IND
  event this function will acknowledge a Disconenct command, optionally sending response
  headers to a OBEX client.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_disconnect_response(
  mt_obex_app_id_type                  app_id,
  uint16                               conn_id,
  mt_obex_hdr_list_type*               headers_ptr,
  mt_obex_status_type                  status )
{
  mt_obex_hdr_list_type headers = { NULL, 0 };

  if ( headers_ptr != NULL )
  {
    headers = *headers_ptr;
  }

  return ( (mt_obex_status_type)bt_cmd_pf_goep_srv_disconnect_response (app_id, conn_id, headers, (bt_cmd_status_type)status) );
}

/*===========================================================================

FUNCTION mt_obex_srv_force_disconnect

DESCRIPTION
  Force a disconnect from the server. This command forcibly severs the
  connection from an OBEX client to the OBEX server. An OBEX server may need
  to forcibly terminate a connection during deregistration, since
  deregistration will fail if a connection is in place.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_force_disconnect(
  mt_obex_app_id_type                  app_id,
  uint16                               conn_id )
{
  return ( (mt_obex_status_type)bt_cmd_pf_goep_srv_force_disconnect(app_id, conn_id) );
}

/*===========================================================================

FUNCTION mt_obex_srv_deregister_server

DESCRIPTION
  De-register an OBEX server. This function deregisters an OBEX server so that
  it will no longer accept put and/or get requests from OBEX clients for the
  specified target. This will also deregister any secondary targets associated
  with this server. To deregister a server that has active connections,
  terminate the connections using mt_obex_srv_force_disconnect() first, then
  deregister the server.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_deregister_server(
  uint16                               app_id,
  uint8                                ch_num )
{
  return ( (mt_obex_status_type)bt_cmd_pf_goep_srv_deregister_server(app_id, ch_num) );
}

/*===========================================================================

FUNCTION mt_obex_srv_get_client_addr

DESCRIPTION
  This function gets the Bluetooth Device Address (BD_ADDR) associated with 
  an OBEX server connection handle.
  
DEPENDENCIES
  None
    
RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_srv_get_client_addr(
  mt_obex_app_id_type                  app_id,
  uint16                               conn_id,
  bt_bd_addr_type*                     bd_addr_ptr )
{
    return ((mt_obex_status_type)bt_pf_goep_srv_get_client_addr(app_id, conn_id, bd_addr_ptr));
}

/*===========================================================================

FUNCTION 
    mt_obex_status_type mt_obex_sd_register_custom_service

DESCRIPTION
  This function registers custom 128bit address with SD.
  
DEPENDENCIES
  None
    
RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/

mt_obex_status_type	mt_obex_sd_register_custom_service(
            mt_obex_app_id_type             app_id,
            mt_obex_uuid128_type			*uuid128,
            uint8                        	scn,
            char*                        	service_name_str_ptr)
{
    return ((mt_obex_status_type)bt_pf_goep_sd_register_custom_service(
                               app_id,
                                (bt_sd_uuid128_type*) uuid128,
                                scn,
                                service_name_str_ptr));
}

#endif /* FEATURE_MT_OBEX */
#endif /* FEATURE_BT_EXTPF_GOEP */
