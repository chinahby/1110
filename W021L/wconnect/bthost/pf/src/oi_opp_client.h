#ifndef _OI_OPP_CLIENT_H_
#define  _OI_OPP_CLIENT_H_
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_opp_client.h#1 $ 

$Log: $
*
*    #3       18 Oct  2006           PR
*  Merged OI Code v3.0.14rev1revQ
*
===========================================================================*/
/** 
  @file  

  This file provides the API for the client side of the Object Push Profile.

  The Object Push Profile provides functions for establishing a connection to a
  remote device over RFCOMM and functions for pushing and pulling objects, such
  as vCard (business card) and vCal (calendar entry) objects. A Bluetooth device
  address and an RFCOMM channel number are required for setting up the
  connection. Unless the application already knows the RFCOMM channel number,
  the application will need to perform service discovery to obtain the channel
  number.
  
  After a connection has been established, the application can call
  OI_OPPClient_Push() to send objects to the server and OI_OPPClient_Pull() to
  retrieve the default object from the server. The default object is the owner's
  vCard business card. 
 */

/** \addtogroup OPP OPP APIs */
/**@{*/

 /**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_obexspec.h"
#include "oi_opp_sys.h"


/**
 * Completion or notification events returned to the application via the @ref
 * OI_OPP_CLIENT_EVENT_CB callback function. The event indicates completion, the
 * accompanying status code indicates if the operation succeeded or failed. The
 * completion of a pull operation, OI_OPPClient_Pull(), is not signalled by and
 * event, rather a callback function is called with the pulled object.
 */
typedef enum {
    OI_OPP_CLIENT_CONNECTED,     /**< The client has connected to an OPP server */
    OI_OPP_CLIENT_DISCONNECT,    /**< The client is now disconnected from an OPP server */
    OI_OPP_CLIENT_PUSH_COMPLETE, /**< A push operation is complete */
    OI_OPP_CLIENT_PULL_COMPLETE  /**< A pull operation is complete */
} OI_OPP_CLIENT_EVENT;


/**
 *  Represents an active connection between a OPP client and a remote OPP server
 */
typedef OI_OPP_CONNECTION OI_OPP_CLIENT_CONNECTION_HANDLE;


/**
 * The application must provide a function with this profile to
 * OI_OPPClient_Connect() when starting the object push client. This function
 * is called to signal to the application when various operations completed; see
 * @ref OI_OPP_CLIENT_EVENT.
 *
 * @param connectionId  handle representing the connection to the OPP server.

 * @param event  identifies the completion or notification event that is being signalled.
 *
 * @param status indicates if the operation succeeded, OI_OK, or failed with an
 *               error. 
 */
typedef void (*OI_OPP_CLIENT_EVENT_CB)(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId,
                                       OI_OPP_CLIENT_EVENT event,
                                       OI_STATUS status);

/**
 * Connect to remote OBEX object push profile server.
 *
 * @param addr             the address of a remote Bluetooth device that supports OPP.
 *
 * @param channel          the RFCOMM channel number of the OPP service on the remote
 *                         device. The caller will normally perform service
 *                         discovery on the remote device to obtain the chnanel
 *                         number.
 *
 * @param connectionId    (OUT) pointer to return the OPP connection handle.
 *
 * @param eventCB         a callback function that is called when OPP client
 *                        operations complete.
 *
 * @param objectFunctions A set of functions that provide an interface to an
 *                        object management system that supports opening,
 *                        reading, writing of objects.
 *
 * @returns OI_OK if the connection request was sent. 
 */
OI_STATUS OI_OPPClient_Connect(OI_BD_ADDR *addr,
                               OI_UINT channel,
                               OI_OPP_CLIENT_CONNECTION_HANDLE *connectionId,
                               OI_OPP_CLIENT_EVENT_CB eventCB,
                               const OI_OPP_OBJSYS_FUNCTIONS *objectFunctions);


/**
 * Terminate the current OBEX connection to a remote OPP server.
 *
 * @param connectionId  handle representing the connection to the OPP server.
 */
OI_STATUS OI_OPPClient_Disconnect(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId);


/**
 * Push an object to an OBEX server. The object type must be one of the object
 * types supported by the server. The list of supported object types is obtained
 * from the service record for the server. 
 *
 * @param connectionId  handle representing the connection to the OPP server.
 *
 * @param name          a NULL-terminated unicode name of the object to be pushed.
 *
 * @param type          object type
 *
 * @return OI_OK if the push command was sent, OI_OBEX_NOT_CONNECTED if a
 *               connection has not yet been established.
 */
OI_STATUS OI_OPPClient_Push(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId,
                            const OI_OBEX_UNICODE *name,
                            const OI_CHAR *type);


/**
 * Pull the default object from an OBEX server. The default object is the
 * owner's business card which is an OBEX object of type @ref
 * OI_OBEX_VCARD_TYPE.
 *
 * @param connectionId  handle representing the connection to the OPP server.
 *
 * @return OI_OK if the pull request was sent, OI_OBEX_NOT_CONNECTED if a
 *               connection has not yet been established.
 */
OI_STATUS OI_OPPClient_Pull(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId);




/**
 * Callback invoked when the cancellation of an operation is completed.
 *
 * @param connectionId  handle representing the connection to the OPP server.
 */
typedef void (*OI_OPP_CLIENT_CANCEL_CFM)(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId);

/**
 * Terminate the current push or pull operation.
 *
 * @param connectionId  handle representing the connection to the OPP server.
 *
 * @param cancelCfm     Callback to call when cancellation is complete (may be NULL).
 */
OI_STATUS OI_OPPClient_Cancel(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId,
                              OI_OPP_CLIENT_CANCEL_CFM cancelCfm);

#ifdef OI_DEPRECATED
/**
 * @deprecated
 *
 * Use @ref OI_OPPClient_Cancel() instead.  Calling OI_OPPClient_Cancel(connectionId, NULL)
 * is functionally equivalent to OI_OPPClient_Abort(connectionId).
 */
OI_STATUS OI_OPPClient_Abort(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId);
#endif

/**@}*/

#endif /* _OPP_CLIENT_H */
