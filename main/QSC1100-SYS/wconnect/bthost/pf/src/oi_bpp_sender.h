#ifndef _OI_BPP_SENDER_H
#define _OI_BPP_SENDER_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bpp_sender.h#1 $ 

$Log: $
*
*    #4        28 Sep 2007            RS
* Made the provision to send end of body message through the callback 
* function OI_BPP_SENDER_GET_CFM
*
*    #3        18 Oct 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
*    #2        15 Apr 2004            JH
* Modified sender register API to return server channel number 
*

===========================================================================*/
/**
 * @file  
 *
 * This file provides the interface for a Basic Printing Profile sender
 * application.
 */

/** \addtogroup BPP BPP APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#include "oi_bpp.h"
#include "oi_obexcli.h"

/** Indicates a connection request by a printer for a referenced objects service.
 * The sender application should check to see if the connecting printer is
 * currently or has recently been the target of a printing operation before
 * deciding to accept the connection.
 *
 * @param handle    Connection handle for this session
 * @param addr      Address of printer requesting access
 * @return  OI_OK to accept the connection, OI_OBEX_ACCESS_DENIED or another
 * error status to refuse
 */
typedef OI_STATUS (*OI_BPP_SENDER_CONNECT_IND)(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                               OI_BD_ADDR *addr);

/** Indicates a printer has disconnected.
 *
 * @param handle    Handle of the dropped connection
 */
typedef void (*OI_BPP_SENDER_DISCONNECT_IND)(OI_OBEXSRV_CONNECTION_HANDLE handle);

/** Indicates a request by a connected printer for an object.
 * @param handle    Connection handle on which the request is being made.
 * @param name  URI of the requested object
 * @param offset    Offset into the requested object
 * @param count     Number of bytes, starting at the offset, being requested
 * @param getFileSize   TRUE if the printer is requesting the total size of the
 * indicated object
 */
typedef OI_STATUS (*OI_BPP_SENDER_GET_REFERENCED_OBJECTS_IND)(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                                              OI_OBEX_UNICODE *name,
                                                              OI_UINT32 offset,
                                                              OI_INT32 count,
                                                              OI_BOOL getFileSize,
                                                              OI_STATUS status);
/** Responds to a get referenced objects request
 * @param handle    Connection handle on which the request is being made.
 * @param data  body of the reply
 * @param fileSize  pointer to filesize if requested, NULL if not.
 * @param status OI_OBEX_CONTINUE if more data follows this response, OI_OK or
 * an error otherwise.
 */
OI_STATUS OI_BPP_SENDER_GetReferencedObjectsResponse(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                                     OI_OBEX_BYTESEQ *data,
                                                     OI_INT32 *fileSize,
                                                     OI_STATUS status);



/** Connection establishment */

/** Confirmation of connection attempt.
 * @param handle    connection handle, as returned by OI_BPP_SENDER_Connect
 * @param status  OI_OK if connection was established, error otherwise
 */
typedef void (*OI_BPP_SENDER_CONNECT_CFM)(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                          OI_STATUS status);

/** Authentication challenge during connection attempt. The application should
 * respond with OI_OBEXCLI_AuthenticationResponse.
 * @param handle    connection handle, as returned by OI_BPP_SENDER_Connect
 * @param userIdRequired    indicates whether the authentication response should
 * include a user ID
 */
typedef void (*OI_BPP_SENDER_AUTH_CHALLENGE_IND)(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                                 OI_BOOL userIdRequired);

/** Connects to a BPP Printer. At most one job-channel connection, one
 * rui-channel connection, and one status connection may be in progress at any given time.
 *
 * @param addr  Address of the BPP Printer.
 *
 * @param channel   RFCOMM channel number of the service to connect to
 *
 * @param target    An enumeration constant specifying the type of service to
 * being connected to.
 *
 * @param connectCfm    Callback invoked indicating the success or failure of
 * the connetion request
 *
 * @param disconnectInd Callback to be invoked when this connection terminates
 *
 * @param authInd   Callback to be invoked to get authentication data. May be
 * NULL if authentication is not to be used.
 *
 * @param handle    Out parameter specifying the connection handle associated
 * with the new connection
 *
 * @return  OI_OK if connection request was successful, error otherwise. The
 * connection is not complete until the connectCfm callback is called with OI_OK.
 */
OI_STATUS OI_BPP_SENDER_Connect(OI_BD_ADDR *addr,
                                OI_UINT8 channel,
                                OI_BPP_TARGET target,
                                OI_BPP_SENDER_CONNECT_CFM connectCfm,
                                OI_BPP_SENDER_DISCONNECT_IND disconnectInd,
                                OI_BPP_SENDER_AUTH_CHALLENGE_IND authInd,
                                OI_OBEXCLI_CONNECTION_HANDLE *handle);

/** Disconnects the specified BPP connection
 * @param handle    The handle of the connection to drop
 */
OI_STATUS OI_BPP_SENDER_Disconnect(OI_OBEXCLI_CONNECTION_HANDLE handle);


/** Callback invoked in response to OI_BPP_SENDER_SendFile. If the status code
 * is OI_OBEX_CONTINUE, the client should respond by invoking
 * OI_BPP_SENDER_SendFile(handle, [callback], NULL, NULL, NULL, NULL, [pdata],
 * [final]);
 *
 * @param handle    Handle to the connection over which the file was sent
 *
 * @param status    OI_OBEX_CONTINUE if the server expects more data, OI_OK if
 * the transaction completed without error, otherwise an error code.
 */
typedef void (*OI_BPP_SENDER_SEND_FILE_CFM)(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                       OI_STATUS status);

/** Function to perform the FilePush or SendDocument BPP operations.
 *
 * @param handle    Handle to the connection over which to send the file
 *
 * @param putCfm    Callback indicating the result of the operation
 *
 * @param type      MIME type of the file being sent
 *
 * @param description   Optional document-type specific information
 *
 * @param name      Optional document name
 *
 * @param jobId     Pointer to Job ID if using job-based data transfer, NULL for
 * simple push operation.
 *
 * @param data      File data to send
 *
 * @param final     TRUE if this is the final block of data in this operation,
 * FALSE otherwise.
 */
OI_STATUS OI_BPP_SENDER_SendFile(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                 OI_BPP_SENDER_SEND_FILE_CFM putCfm,
                                 OI_OBEX_BYTESEQ *type,
                                 OI_OBEX_UNICODE *description,
                                 OI_OBEX_UNICODE *name,
                                 OI_BPP_JOB_ID *jobId,
                                 OI_OBEX_BYTESEQ *data,
                                 OI_BOOL final);

/** Callback invoked in response to OI_BPP_SENDER_SendReference. 
 *
 * @param handle    Handle to the connection over which the reference was sent.
 * @param url   In the event that not all references could be retrieved, url is
 * the first to fail
 * @param httpChallenge if url failed due to an authentication challenge, this
 * paramter includes the authentication challenge HTTP header
 * @param status    OI_OBEX_CONTINUE if the server expects more data, OI_OK if
 * the transaction completed without error, otherwise an error code.
 */
typedef void (*OI_BPP_SENDER_SEND_REFERENCE_CFM)(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                            OI_OBEX_BYTESEQ *url,
                                            OI_OBEX_BYTESEQ *httpChallenge,
                                            OI_STATUS status);

/** Function to perform the SimpleReferencePush or SendReference operation.
 * 
 * @param handle    Handle to the connection over which to send the reference
 * @param cb    Callback receiving the result of the operation
 * @param type  Enumeration value indicating the type of reference (simple,
 * list, or xml)
 * @param httpHeaders authentication credentials (optional)
 * @param jobId Pointer to job ID if using job-based transfer, NULL for simple
 * reference push
 * @param data  Reference data to send
 * @param final TRUE if this is the final block of data for this operation,
 * FALSE otherwise.
 */
OI_STATUS OI_BPP_SENDER_SendReference(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                      OI_BPP_SENDER_SEND_REFERENCE_CFM cb,
                                      OI_BPP_REF_TYPE type,
                                      OI_OBEX_BYTESEQ *httpHeaders,
                                      OI_BPP_JOB_ID *jobId,
                                      OI_OBEX_BYTESEQ *data,
                                      OI_BOOL final);

#ifdef OI_CODE
#error code not present
#else
/** Callback invoked in response to SOAP and RUI requests.
 *
 * @param handle    Handle to the connection over which the request was made.
 * @param reply     Body of the reply
 * @param jobId     If a job id was sent as part of the SOAP response, this
 * value will point to it. Otherwise, it is NULL.
 * @param status    OI_OBEX_CONTINUE if there is more response data, OI_OK if
 * the transaction completed without error, otherwise an error code.
 * @param final     Boolean value set if the end of body has
 *                  arrived.
 */
typedef void (*OI_BPP_SENDER_GET_CFM)(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                      OI_OBEX_BYTESEQ *reply,
                                      OI_BPP_JOB_ID *jobId,
                                      OI_STATUS status,
                                      OI_BOOL final);
#endif
/** Sends a SOAP message to a BPP Printer.
 * @param handle    Handle to the connection over which to send the request.
 * @param cb    callback invoked with the result of the request
 * @param body  body of the request
 * @param final TRUE if this is the final (or only) portion of the request
 */
OI_STATUS OI_BPP_SENDER_SOAPRequest(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                    OI_BPP_SENDER_GET_CFM cb,
                                    OI_OBEX_BYTESEQ *body,
                                    OI_BOOL final);

/** Sends a Get RUI request to a BPP Printer.
 * @param handle       Handle to the connection over which to send the request.
 * @param cb           callback invoked with the result of the request
 * @param type         MIME type of the request
 * @param name         name
 * @param httpHeaders  HTTP headers
 * @param body         body of the request
 * @param final        TRUE if this is the final (or only) portion of the request
 */
OI_STATUS OI_BPP_SENDER_GetRUI(OI_OBEXCLI_CONNECTION_HANDLE handle,
                               OI_BPP_SENDER_GET_CFM cb,
                               OI_OBEX_BYTESEQ *type,
                               OI_OBEX_UNICODE *name,
                               OI_OBEX_BYTESEQ *httpHeaders,
                               OI_OBEX_BYTESEQ *body,
                               OI_BOOL final);


/** Callback invoked when cancel is complete.
 *
 * @param handle      handle to the connection with the recently canceled operation.
 */
typedef void (*OI_BPP_SENDER_CANCEL_CFM)(OI_OBEXCLI_CONNECTION_HANDLE handle);

/** Cancels the current OBEX BPP operation.
 *
 * @param handle      handle to the connection to cancel
 * @param cancelCfm   callback indicating completion of cancel (may be NULL)
 */
OI_STATUS OI_BPP_SENDER_Cancel(OI_OBEXCLI_CONNECTION_HANDLE handle,
                               OI_BPP_SENDER_CANCEL_CFM cancelCfm);

/*******/

typedef struct {
    OI_BPP_SENDER_CONNECT_IND connectInd;
    OI_BPP_SENDER_DISCONNECT_IND disconnectInd;
    OI_BPP_SENDER_GET_REFERENCED_OBJECTS_IND getObj;
} OI_BPP_SENDER_CALLBACKS;

/** Register the BPP sender, optionally registering referenced objects servers.
 * This call must be made even if no servers are to be registered.
 * 
 * @param strings SDP strings for the service record created if servers are to
 * be registered. This may be NULL.
 * 
 * @param objectCallbacks pointer to server callbacks for referenced objects
 * service. If this pointer is NULL, then no referenced objects service will be
 * registered.
 * 
 * @param objectChannelNumber   Out parameter optionally recieving the RFCOMM
 * channel number of the object service. May be NULL if the application doesn't
 * require this information.
 * 
 * @param ruiRefCallbacks poitner to server callbacks for RUI referenced objects
 * service. If this pointer is NULL, then no RUI referenced objcets service will
 * be registered.
 *
 * @param ruiObjectChannelNumber    Out parameter optionally recieving the
 * RFCOMM channel number of the RUI object service. May be NULL if the
 * application doesn't require this information.
 */
OI_STATUS OI_BPP_SENDER_Register(const OI_SDP_STRINGS *strings,
                                 const OI_BPP_SENDER_CALLBACKS *objectCallbacks,
                                 OI_UINT8 *objectChannelNumber,
                                 const OI_BPP_SENDER_CALLBACKS *ruiRefCallbacks,
                                 OI_UINT8 *ruiObjectChannelNumber);

/** Deregister the BPP sender */
OI_STATUS OI_BPP_SENDER_Deregister(void);

/**
 * This function forcibly severs the connection from a BPP client to the OBEX
 * server. The disconnect indication callback will be called when the
 * disconnect is complete.
 * 
 * A BPP server may need to forcibly terminate a connection during
 * deregistration, since deregistration will fail if a connection is in place.
 * 
 * @param handle     a unique identifier generated by the BIP server that
 *                   identifies the connection.
 *
 * @return           OI_OK if the connectionId is valid and the connection
 *                   will be terminated. 
 */
OI_STATUS OI_BPP_SENDER_ForceDisconnect(OI_OBEXSRV_CONNECTION_HANDLE handle);


#ifdef OI_DEPRECATED
/** 
 * @deprecated
 *
 * Use @ref OI_BPP_SENDER_Cancel() instead.  OI_BPP_SENDER_Cancel(handle,
 * NULL) is equivalent to OI_BPP_SENDER_Abort(handle).
 */
OI_STATUS OI_BPP_SENDER_Abort(OI_OBEXCLI_CONNECTION_HANDLE handle);
#endif

/**@}*/

#endif /* _OI_BPP_SENDER_H */

