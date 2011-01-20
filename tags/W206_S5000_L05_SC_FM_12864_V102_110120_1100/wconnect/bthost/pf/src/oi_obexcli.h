#ifndef _OI_OBEXCLI_H
#define _OI_OBEXCLI_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_obexcli.h#1 $ 

$Log: $
*
*    #11       01 Mar  2007           MH
*  Modified auth indication to support full access.
*
*    #10       18 Oct  2006           PR
*  Merged OI Code v3.0.14rev1revQ
*
*    #9        19 Sept 2006           MH
*  Added Auth response Callback event.
*
*    #8        12 Aug 2006            BK
* Removed extra #define
*
*    #7        29 Jul 2006            BK
* Modified for obex client initiated authentication.
*
*    #5-6      10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #4        23 Jun 2006            JH
* Added support for optional headers in the Disconnect and SetPath commands, as
*  well as the Connect, Dsconnect, and SetPath responses for JSR82.
*
*    #3        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
===========================================================================*/
/**
  @file  
 API for OBEX client
*/

/** \addtogroup OBEX */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_status.h"
#include "oi_obexspec.h"
#include "oi_bt_profile_config.h"


/**
*  represents an active connection to an OBEX server
*/

typedef OI_UINT16 OI_OBEXCLI_CONNECTION_HANDLE;

/**
 * A callback function of this type is called when an OBEX connection request completes. 
 * The status code indicates if the connection was established.
 *
 * @param connectionId   a unique ID that represents a connection to an OBEX server
 *
 * @param readOnly       if TRUE, indicates that the server is only granting read access
 *                       to the server's resources to this client. Access rights
 *                       are only passed  on authenticated connections so this
 *                       parameter will only ever be TRUE for authenticated
 *                       connections. A FALSE value does not necessarily mean
 *                       that the client has write access to the server. 
 *
 * @param status         OI_OK if successful; error status if the connection attempt failed
 *
 */

typedef void (*OI_OBEXCLI_CONNECT_CFM)(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                       OI_BOOL readOnly,
#ifndef OI_CODE
                                       OI_OBEX_HEADER_LIST *hdrs,
#endif
                                       OI_STATUS status);

/**
 * A callback function of this type is called when an OBEX disconnect request completes, 
 * whether requested by a call to OI_OBEXCLI_Disconnect() or because the connection was dropped.
 *
 * @param connectionId  the (now invalid) connection to an OBEX server
 */

#ifdef OI_CODE
#error code not present
#else
typedef void (*OI_OBEXCLI_DISCONNECT_IND)(OI_OBEXCLI_CONNECTION_HANDLE connectionId, 
                                          OI_OBEX_HEADER_LIST *hdrs);
#endif

/**
 * A callback function of this type is registered by OI_OBEXCLI_Put() and called when OBEX is ready for
 * more data to put.
 *
 * @param connectionId    This parameter is a unique ID that represents an established connection
 *                        to an OBEX server.
 *
 * @param rspHeaders      This parameter is a pointer to a list of OBEX headers received from the
 *                        OBEX server. Storage for rspHeaders is allocated by OBEX and must not be
 *                        freed by the caller. The caller should copy any data that is needed after
 *                        this function returns into a local buffer.
 *
 * @param status          OI_OK if the put transaction is complete (if this is the last chunk of data);
 *                        OI_OBEX_CONTINUE if the application is being asked for more data to put; 
 *                        or an error status indicating that the transaction has been canceled and why
 */

typedef void (*OI_OBEXCLI_PUT_CFM)(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                   OI_OBEX_HEADER_LIST *rspHeaders,
                                   OI_STATUS status);

/**
 * A callback function of this type is called when an OBEX get request returns data. 
 *
 * @param connectionId    This parameter is a unique ID that represents an established connection
 *                        to an OBEX server.
 *
 * @param rspHeaders      This parameter is a pointer to a list of OBEX headers received from the
 *                        OBEX server. Storage for rspHeaders is allocated by OBEX and must not be
 *                        freed by the caller. The caller should copy any data that is needed after
 *                        this function returns into a local buffer.
 *
 * @param status          OI_OK if the put transaction is complete (if this is the last chunk of data);
 *                        OI_OBEX_CONTINUE if the application is being asked for more data to put; 
 *                        or an error status indicating that the transaction has been canceled and why
 */

typedef void (*OI_OBEXCLI_GET_RECV_DATA)(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                         OI_OBEX_HEADER_LIST *rspHeaders,
                                         OI_STATUS status);


/**
 * A callback function of this type is called when a setpath command completes.
 *
 * @param connectionId   This parameter is a unique ID that represents an established connection
 *                       to an OBEX server.
 *
 * @param status         OI_OK if the setpath operation succeeded; otherwise, error status indicating why the operation failed
 */

typedef void (*OI_OBEXCLI_SETPATH_CFM)(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
#ifndef OI_CODE
                                       OI_OBEX_HEADER_LIST *rspHeaders,
#endif
                                       OI_STATUS status);

/**
 * A callback function of this type is called to the application to request a PIN if OBEX
 * authentication is required on connections from this client. If the authChallengeIndCB parameter to
 * OI_OBEXCLI_Connect() is non-NULL, then authentication is required. Not all
 * services are allowed to use OBEX authentication.
 *
 * @param connectionId     a unique ID that represents the in-progress connection
 *
 * @param userIdRequired   indicates that the server requires a user id as well
 *                         as a password. The client can provide a user id even
 *                         if the server does not require one. Whether the
 *                         actually makes use of the user id in this case
 *                         depends entirely on the server implementation.
 */

typedef void (*OI_OBEXCLI_AUTH_CHALLENGE_IND)(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
#ifndef OI_CODE
                                              OI_BOOL fullAccess,
#endif
                                              OI_BOOL userIdRequired );


#ifndef OI_CODE
/**
 * A callback function of this type is called to the application to request a PIN if OBEX
 * authentication is required on connections from this client. If the authChallengeIndCB parameter to
 * OI_OBEXCLI_Connect() is non-NULL, then authentication is required. Not all
 * services are allowed to use OBEX authentication.
 *
 * @param connectionId     a unique ID that represents the in-progress connection
 *
 */

typedef void (*OI_OBEXCLI_AUTH_RESPONSE_IND)(OI_OBEXCLI_CONNECTION_HANDLE connectionId);
#endif

/**
 * A callback function of this type is called when an OBEX abort request completes.
 *
 * @param connectionId  a unique ID that represents the in-progress connection
 */
typedef void (*OI_OBEXCLI_ABORT_CFM)(OI_OBEXCLI_CONNECTION_HANDLE connectionId);



/**
 * This function responds to an authentication indication. A NULL password indicates that
 * the application is rejecting the authentication challenge.
 *
 * @param connectionId    This parameter is a unique ID that represents an established connection
 *                        to an OBEX server.
 *
 * @param userId          A user id must be provided if the server indicated
 *                        that user ids are required in the authentication
 *                        challenge. A client can provide a user id even if the
 *                        server does not required one.
 *
 * @param userIdLen       The actual length of the user id.
 *
 * @param password        a NULL-terminated password string.
 *
 * @return                OI_OK if successful or error status if this function
 *                        failed or was called at the wrong time.
 */

OI_STATUS OI_OBEXCLI_AuthenticationResponse(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                            const OI_BYTE *userId,
                                            OI_UINT8 userIdLen,
                                            const OI_CHAR *password);


/**
 * This function establishes a connection to an OBEX server. This includes setting up an
 * RFCOMM connection to the OBEX server.
 *
 * @param addr            pointer to the Bluetooth device address of the OBEX server device
 *
 * @param channel         RFCOMM channel number of the OBEX server
 *
 * @param cmdHeaders      This is a pointer to a list of optional OBEX headers to be included in the
 *                        connect request. This parameter can be NULL if there are no optional headers.
 *                        The caller must not free any data referenced by these headers until the
 *                        receiving the connect confirmation callback.
 *
 * @param connectCfmCB    callback function called when the connection has been established
 *
 * @param disconnectIndCB callback function called when the OBEX connection is disconnected
 *
 * @param authChallengeIndCB callback function called if a response to an authentication challenge is required
 *
 * @param connectionId    (OUT) pointer to the location to which a handle to the OBEX connection will be written
 *                         once the connection is established
 *
 * @param policy          the connection policy required by the OBEX client
 *
 * @return                OI_OK if successful or error status if the connect request failed
 */

OI_STATUS OI_OBEXCLI_Connect(OI_BD_ADDR *addr,
                             OI_UINT8 channel,
                             OI_OBEX_HEADER_LIST const *cmdHeaders,
                             OI_OBEXCLI_CONNECT_CFM connectCfmCB,
                             OI_OBEXCLI_DISCONNECT_IND disconnectIndCB,
                             OI_OBEXCLI_AUTH_CHALLENGE_IND authChallengeIndCB,
#ifndef OI_CODE
                             OI_OBEXCLI_AUTH_RESPONSE_IND authResponseIndCB,
#endif
                             OI_OBEXCLI_CONNECTION_HANDLE *connectionId,
                             const OI_CONNECT_POLICY *policy);


/**
 * This function disconnects from an OBEX server.
 *
 * @param connectionId    This parameter is a unique ID that represents an established connection
 *                        to an OBEX server.
 *
 * @return                OI_OK if the disconnect request could be sent
 */

#ifdef OI_CODE
#error code not present
#else
OI_STATUS OI_OBEXCLI_Disconnect(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                OI_OBEX_HEADER_LIST const *cmdHeaders);
#endif


/**
 * Put data to an OBEX server. The putCfm() callback indicates that the server
 * has received and acknowledged receipt of the data.
 *
 * @param connectionId    This parameter is a unique ID that represents an established connection
 *                        to an OBEX server.
 *
 * @param cmdHeaders      the data being put
 *
 * @param putCfm          callback function called to confirm that the put operation has
 *                        completed and the server has acknowedged receipt of the data
 *
 * @param status          Status code to tell obex what is being put.
 *                        - OI_OK indicates this is the final put,
 *                        - OI_OBEX_CONTINUE if there are more puts to come
 *                        - An error status to terminate the put operation.
 *
 * @return                OI_OK if put complete, OI_OBEX_OPERATION_IN_PROGRESS if the last request has not
 *                        completed, or other errors from lower protocol layers.
*/

OI_STATUS OI_OBEXCLI_Put(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                         OI_OBEX_HEADER_LIST const *cmdHeaders,
                         OI_OBEXCLI_PUT_CFM putCfm,
                         OI_STATUS status);


/**
 * Get data from an OBEX server. This will cause one or more packets to be
 * retrieved from an OBEX server. 
 *
 * @param connectionId    This parameter is a unique ID that represents an established connection
 *                        to an OBEX server.
 *
 * @param cmdHeaders      description of the object being requested.
 *
 * @param getRecvData     function that will be invoked when data is received from the server
 *
 * @param final           TRUE if this is the last packet in the get request,
 *                        FALSE otherwise. Most get requests are only one packet long, and should have
 *                        this parameter set to TRUE.
 * 
 * @return                OI_OK if get successful, OI_OBEX_OPERATION_IN_PROGRESS
 *                        if the last request has not completed, or other errors from lower layers.
*/

OI_STATUS OI_OBEXCLI_Get(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                         OI_OBEX_HEADER_LIST const *cmdHeaders,
                         OI_OBEXCLI_GET_RECV_DATA getRecvData,
                         OI_BOOL final);


/**
 * Terminates the current put or get operation on this connection. If a PUT/GET
 * transaction is in progress this function returns OI_STATUS_PENDING and an
 * OBEX ABORT command is sent when the transaction completes and the appropriate
 * GET/PUT callback is called with status OI_OBEX_CLIENT_ABORTED_COMMAND.  If
 * there is no GET/PUT transaction in progress the OBEX ABORT command is send
 * immediately, this function returns OI_OK and the abort confirm callback will
 * be called when the abort is complete. If there is no current GET/PUT
 * operation, this function returns OI_STATUS_INVALID_STATE.
 *
 * @param connectionId    This parameter is a unique ID that represents an
 *                        established connection to an OBEX server.
 *
 * @param abortCfm        function that will be invoked when the abort
 *                        completes. This function is only called if the return
 *                        value is OI_OK. If the return value is
 *                        OI_STATUS_PENDING the GET/PUT callback will be called
 *                        instead.
 *
 * @return                - OI_OK if the operation was aborted.
 *                        - OI_STATUS_PENDING if the abort is deferred
 *                        - OI_STATUS_INVALID_STATE if there is nothing to abort
 */
OI_STATUS OI_OBEXCLI_Abort(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                           OI_OBEXCLI_ABORT_CFM abortCfm);


/**
 * Set the response timeout for an OBEX connection overriding the internal
 * default timeout. The timeout value indicates how long the OBEX client will
 * wait for a response from an OBEX server before aborting a PUT, GET or other
 * OBEX operation. 
 *
 * @param connectionId    This parameter is a unique ID that represents an
 *                        established connection to an OBEX server.
 *
 * @param timeout         The timeout is specified in 1/10's of seconds. A
 *                        timeout of 0 is not allowed.
 *
 * @return                OI_OK if the connectionId and timeout value was valid.
 */

OI_STATUS OI_OBEXCLI_SetResponseTimeout(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                        OI_INTERVAL timeout);

/**
 * Setpath can be used to set the current folder for getting and putting
 * objects and can be used to create new folders.
 *
 * @param connectionId    This parameter is a unique ID that represents an established connection
 *                        to an OBEX server.
 *
 * @param folder          This parameter is a unicode name specifying the new folder. 
 *                        This must be NULL if upLevel is TRUE. A NULL folder
 *                        (and upLevel == FALSE) sets the folder to the FTP
 *                        server's root folder.
 *
 * @param dontCreate      This parameter indicates whether a new folder should be created if it does not already exist.
 *                        A value of TRUE indicates that the specified folder should not be created if it does not already exist.
 *
 * @param upLevel         This parameter indicates that the path is to be set to the parent folder of
 *                        the current folder. If upLevel is TRUE, folder must be NULL. 
 *
 * @param                 setpathCfm is the callback function called with the server's response to the setpath command.
 *
 * @return                OI_OK if successful; error if the command could not be sent
 */

OI_STATUS OI_OBEXCLI_SetPath(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                             OI_OBEX_UNICODE const *folder,
                             OI_BOOL dontCreate,
                             OI_BOOL upLevel,
#ifndef OI_CODE
                             OI_OBEX_HEADER_LIST const *cmdHeaders,
#endif
                             OI_OBEXCLI_SETPATH_CFM setpathCfm);

/**
 * This function returns the optimal size for a body header for a connection.
 * Sending body headers that are smaller than this size or not an integer
 * multiple of this size will result in slower data transfer rates.
 *
 * @param connectionId    This parameter is a unique ID that represents an established connection
 *                        to an OBEX server.
 *
 * @returns               best body payload size or 0 if there is no connection
 */

OI_UINT16 OI_OBEXCLI_OptimalBodyHeaderSize(OI_OBEXCLI_CONNECTION_HANDLE connectionId);

/**
 * Associates a caller defined context with an OBEX client connection. This
 * context can then be retrieved by calling OI_OBEXCLI_GetConnectionContext().
 *
 * @param connectionId   The connection to associate the context with.
 *
 * @param context         A value supplied by the caller.
 *
 * @return                OI_OK if the context was set, OI_STATUS_NOT_FOUND if
 *                        the connection id is not valid.
 */
OI_STATUS OI_OBEXCLI_SetConnectionContext(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                          void *context);


/**
 * Gets a caller defined context associate with an OBEX client connection. This is a value
 * that we previously set by a call to OI_OBEXCLI_SetConnectionContext().
 *
 * @param connectionId    The OBEX client connection to get the context from.
 *
 * @return                A context pointer or NULL if the handle is invalid or
 *                        there is no context associated with this connection.
 */
void* OI_OBEXCLI_GetConnectionContext(OI_OBEXCLI_CONNECTION_HANDLE connectionId);

#ifndef OI_CODE
/**
 * Initialises the client data
 *
 * @param all_loc : 
 *
 * @return none
 */
void OI_OBEXCLI_InitData(OI_UINT16 index);

/**
 * Stores the information
 *
 * @param all_loc : based on the value of all_loc either the entire array /
 *                  a single location will be initialised.
 *
 * @return none
 */
OI_STATUS OI_OBEXCLI_Authenticate(OI_BD_ADDR *pAddr,
                                OI_UINT8 channel,
                                OI_REALM_INFO *realmInfo,
                                OI_BOOL userId,
                                OI_BOOL accessControl);

#endif /* OI_CODE */

/**
 * @deprecated
 *
 * Terminate the current put or get operation on this connection by replacing
 * the next GET or PUT command by an ABORT command. To send an immediate ABORT
 * call OI_OBEXCLI_Get() or OI_OBEXCLI_Put() as appropriate immediately after
 * calling this function.
 *
 * @param connectionId    This parameter is a unique ID that represents an established connection
 *                        to an OBEX server.
 *
 * @return                OI_OK if the operation could be terminated
 */

OI_STATUS OI_OBEXCLI_AbortOperation(OI_OBEXCLI_CONNECTION_HANDLE connectionId);

/*****************************************************************************/
/**@}*/

#endif /* _OI_OBEXCLI_H */

