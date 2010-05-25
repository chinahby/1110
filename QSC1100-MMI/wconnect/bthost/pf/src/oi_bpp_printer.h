#ifndef _OI_BPP_PRINTER_H
#define _OI_BPP_PRINTER_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bpp_printer.h#1 $ 

$Log: $
*
*    #3       18 Oct 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
===========================================================================*/
/**
 * @file  
 *
 * This file provides the interface for a Basic Printing Profile printer
 * application.
 */

/** \addtogroup BPP BPP APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/
#include "oi_bpp.h"
#include "oi_obexcli.h"


/* Printer indications */

/** Indicates a connection request by a sender. The application should call
 * OI_OBEXSRV_AcceptConnection if unauthorized is FALSE, or
 * OI_OBEXSRV_AuthenticationResponse if unauthorized is TRUE. 
 *
 * @param handle    handle identifying this connection
 * @param unauthorized   If TRUE, the application should respond with OI_OBEXSRV_AuthenticationResponse
 * @param userId    If unauthorized is TRUE, the client may have specified a
 * userId.
 * @param userIdLen length of the user Id (if present)
 */
typedef OI_STATUS (*OI_BPP_PRINTER_CONNECT_IND)(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                                OI_BOOL unauthorized,
                                                OI_BYTE *userId,
                                                OI_UINT8 userIdLen);

/** Indicates a sender has disconneted from a printer server
 * @param handle    handle identifying the dropped connection
 */
typedef void (*OI_BPP_PRINTER_DISCONNECT_IND)(OI_OBEXSRV_CONNECTION_HANDLE handle);


/** Indicates a file push operation request by a sender.
 *  If the status code is OI_OBEX_CONTINUE, the application should
 *  respond by invoking OI_BPP_PRINTER_FilePushResponse.
 *
 * @param handle    connection handle associated with the request
 * @param type      MIME type of the pushed file
 * @param description   description of the pushed file (may not be present)
 * @param name      name of the pushed file (may not be present)
 * @param jobId     pointer to a jobId (may not be present)
 * @param data      body of the pushed file
 * @param status    OI_OBEX_CONTINUE if more file data exists, OI_OK or an error
 * otherwise.
 * @returns OI_OK if a response is (or will be) invoked, otherwise an error to
 * return to the client.
 */
typedef OI_STATUS (*OI_BPP_PRINTER_FILE_PUSH_IND)(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                                  OI_OBEX_BYTESEQ *type,
                                                  OI_OBEX_UNICODE *description,
                                                  OI_OBEX_UNICODE *name,
                                                  OI_BPP_JOB_ID *jobId,
                                                  OI_OBEX_BYTESEQ *data,
                                                  OI_STATUS status);

/** Responds to a file push indication. 
 *
 * @param handle    connection handle associated with the request
 * @param status    set equal to the status passed by the callback to continue
 * the request, or specify an error to terminate.
 */
OI_STATUS OI_BPP_PRINTER_FilePushResponse(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                          OI_STATUS status);


/** Indicates a reference push operation request by a sender.
 * If the status code is OI_OBEX_CONTINUE, the application
 * should respond with OI_BPP_PRINTER_ReferencePushResponse.
 *
 * @param handle    connection handle associated with the request
 * @param type      an enumeration constant indicating the type of reference
 * @param httpHeaders   http header with access credentials for referenced
 * resource (may not be present)
 * @param jobId     pointer to a jobId (may not be present)
 * @param data      body of the pushed reference.
 * @param status    OI_OBEX_CONTINUE if more file data exists, OI_OK or an error
 * otherwise.
 */
typedef OI_STATUS (*OI_BPP_PRINTER_REF_PUSH_IND)(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                                 OI_BPP_REF_TYPE type,
                                                 OI_OBEX_BYTESEQ *httpHeaders,
                                                 OI_BPP_JOB_ID *jobId,
                                                 OI_OBEX_BYTESEQ *data,
                                                 OI_STATUS status);

/** Responds to a reference push indication.
 *
 * @param handle    connection handle associated with the request
 * @param url       Optional parameter. Should be NULL except possibly on the
 * final invocation of this function during a given request. If present, it indicates a URL which was unable to
 * be retrieved.
 * @param httpHeaders   Optional paramter. Should be NULL except possibly on the
 * final invocation of this function during a given request.  If present, it
 * contains an authentication challenge which the sender must answer in order to
 * access the referenced resource named in the url parameter.
 * @param status    set equal to the status passed by the callback to continue
 * the request, or specify an error to terminate.
 */
OI_STATUS OI_BPP_PRINTER_ReferencePushResponse(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                               OI_OBEX_BYTESEQ *url,
                                               OI_OBEX_BYTESEQ *httpHeaders,
                                               OI_STATUS status);

/** Indicates a reflected user interface operation request by a sender.
 * If the status code is OI_OBEX_CONTINUE, the application
 * should respond with OI_BPP_PRINTER_GetRUI_Response.
 *
 * @param handle    connection handle associated with the request
 * @param type      MIME type of the RUI request.
 * @param name      Depends on the type of request. Might not be present.
 * @param httpHeaders   Depends on the type of request. Might not be present.
 * @param body  Depends on the type of request. Might not be present.
 * @param status    OI_OBEXSRV_INCOMPLETE_GET indicates that more request data
 * follows; the application should respond with
 * OI_BPP_PRINTER_GetRUIResponse(handle, NULL, OI_OBEX_CONTINUE).
 * OI_OBEX_CONTINUE inciates that the client is ready for more response data.
 * OI_OK or an error indicates the end of the transaction.
 */
typedef OI_STATUS (*OI_BPP_PRINTER_GET_RUI_IND)(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                                OI_OBEX_BYTESEQ *type,
                                                OI_OBEX_UNICODE *name,
                                                OI_OBEX_BYTESEQ *httpHeaders,
                                                OI_OBEX_BYTESEQ *body,
                                                OI_STATUS status);

/** Responds to a reflected user interface operation.
 *
 * @param handle    connection handle associated with the request
 * @param body      content of the response
 * @param status    OI_OBEX_CONTINUE if there is more data to be sent after this
 * response, OI_OK or an error to finish the response.
 */
 OI_STATUS OI_BPP_PRINTER_GetRUIResponse(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                        OI_OBEX_BYTESEQ *body,
                                        OI_STATUS status);

/** Indicates a SOAP request.
 * 
 * @param handle    connection handle associated with the request
 * @param soapRequest   body of the request
 * @param status    OI_OBEXSRV_INCOMPLETE_GET indicates that more request data
 * follows; the application should respond with
 * @ref OI_BPP_PRINTER_SOAPResponse(handle, NULL, NULL, OI_OBEX_CONTINUE).
 * OI_OBEX_CONTINUE indicates that the client is ready for more response data.
 * OI_OK or an error indicates the end of the transaction.
 */
typedef OI_STATUS (*OI_BPP_PRINTER_SOAP_IND)(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                             OI_OBEX_BYTESEQ *soapRequest,
                                             OI_STATUS status);

/** Responds to a status-channel SOAP request.
 *
 * @param handle    connection handle associated with the request
 * @param body      body of the response
 * @param status    OI_OBEX_CONTINUE if there is more data to be sent after this
 * response, OI_OK or an error to finish the response
 */
OI_STATUS OI_BPP_PRINTER_StatusSOAPResponse(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                            OI_OBEX_BYTESEQ *body,
                                            OI_STATUS status);

/** Responds to a job-channel SOAP request.
 *
 * @param handle    connection handle associated with the request
 * @param optionalJobId Pointer to a job id value if this soap response results
 * in the allocation of a job id, NULL otherwise.
 * @param body      body of the response
 * @param status    OI_OBEX_CONTINUE if there is more data to be sent after this
 * response, OI_OK or an error to finish the response
 */
OI_STATUS OI_BPP_PRINTER_SOAPResponse(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                      OI_BPP_JOB_ID *optionalJobId,
                                      OI_OBEX_BYTESEQ *body,
                                      OI_STATUS status);

/* Printer GetReferencedObjects operation */

/** Indicates the completion of a connection attempt to a sender referenced
 * objects server
 *
 * @param handle connection handle specified by OI_BPP_PRINTER_Connect
 * @param status  OI_OK if the connection succeeded, an error otherwise.
 */
typedef void (*OI_BPP_PRINTER_CONNECT_CFM)(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                                     OI_STATUS status);

/** Requests a connection with a BPP sender's referenced objects service.
 * @param senderAddr    pointer to sender's address
 * @param channel   RFCOMM channel number of sender's referenced objects service
 * @param connectCfm    connection confirmation callback
 * @param disconnectInd disconnection indication callback
 * @param handle    out parameter specifying the handle associated with this
 * connection request.
 */
OI_STATUS OI_BPP_PRINTER_Connect(OI_BD_ADDR *senderAddr,
                                 OI_UINT8 channel,
                                 OI_BPP_PRINTER_CONNECT_CFM connectCfm,
                                 OI_OBEXCLI_DISCONNECT_IND disconnectInd,
                                 OI_OBEXCLI_CONNECTION_HANDLE *handle);
/** Disconnects from a referenced objects service.
 * @param handle    handle to the connection to drop.
 */
OI_STATUS OI_BPP_PRINTER_Disconnect(OI_OBEXCLI_CONNECTION_HANDLE handle);

/** Cancels the current OBEX BPP operation.
 *
 * @param handle      handle to the connection to cancel
 * @param cancelCfm   callback indicating completion of cancel (may be NULL)
 */
OI_STATUS OI_BPP_PRINTER_Cancel(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                OI_OBEXCLI_ABORT_CFM cancelCfm);

/** Indicates the result of a call to OI_BPP_PRINTER_GetReferencedObjects.
 *  If the status code is OI_OBEX_CONTINUE, the application should
 *  respond by invoking OI_BPP_PRINTER_GetReferencedObjects again.
 *
 * @param handle    connection handle associated with the request
 * @param body      body of the requested data, if available
 * @param fileSize  total size of the requested file, or -1 if not requested or
 *                  not known
 * @param status    OI_OBEX_CONTINUE if more data follows this response; OI_OK
 *                  or error otherwise
 */
typedef void (*OI_BPP_PRINTER_GET_REFERENCED_OBJECTS_CFM)(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                                               OI_OBEX_BYTESEQ *body,
                                                               OI_INT32 fileSize,
                                                               OI_STATUS status);

/** Performs a get referenced objects operation.
 * @param handle  handle to the connection to use for the request
 * @param cb    confirmation callback function
 * @param URI   URI of the object to request
 * @param offset    offset into the object to start retreiving data
 * @param count     maximum amount of data the sender should return
 * @param requestFileSize   TRUE to request that the sender return the total
 * size of the requested object, FALSE otherwise.
 */
OI_STATUS OI_BPP_PRINTER_GetReferencedObjects(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                              OI_BPP_PRINTER_GET_REFERENCED_OBJECTS_CFM cb,
                                              OI_OBEX_UNICODE *URI,
                                              OI_UINT32 offset,
                                              OI_INT32 count,
                                              OI_BOOL requestFileSize);


typedef struct {
    OI_BPP_PRINTER_CONNECT_IND connect;
    OI_BPP_PRINTER_DISCONNECT_IND disconnect;
    OI_BPP_PRINTER_FILE_PUSH_IND filePush;
    OI_BPP_PRINTER_REF_PUSH_IND refPush;
    OI_BPP_PRINTER_GET_RUI_IND getRui;
    OI_BPP_PRINTER_SOAP_IND soap;
    OI_BPP_PRINTER_SOAP_IND statusSoap;
} OI_BPP_PRINTER_CALLBACKS;

typedef struct {
    OI_CHAR *documentFormats;           /*<< Supported document formats. Mandatory */
    OI_UINT128 characterRepertoires;    /*<< Supported character repertoires. Mandatory */
    OI_CHAR *xhtmlPrintImageFormats;    /*<< Supported image formats for XHTML-Print. Mandatory */
    OI_BOOL fullColor;                  /*<< TRUE if full color is supported, FALSE otherwise */
    OI_CHAR *id1284;                    /*<< IEEE-1284 ID string. Mandatory */
    OI_CHAR *printerName;               /*<< User-friendly printer name string. Optional (specify NULL to omit). */
    OI_CHAR *printerLocation;           /*<< User-friendly printer location string. Optional (specify NULL to omit). */
    OI_BOOL duplex;                     /*<< TRUE if duplex printing is supported, FALSE otherwise */
    OI_CHAR *mediaTypes;                /*<< Supported print media types. Optional (specify NULL to omit). */
    OI_UINT16 maxMediaWidth;            /*<< Maximum print media width, in millimeters. Optional (specify 0 to omit). */
    OI_UINT16 maxMediaLength;           /*<< Maximum print media length, in millimeters. Optional (specify 0 to omit). */
    OI_BOOL enhancedLayout;             /*<< TRUE if enhanced layout is supported, FALSE otherwise */
    OI_CHAR *ruiFormats;                /*<< Supported RUI formats, in order of preference. Optional (specify NULL to omit). */
    OI_CHAR *referenceTopUrl;           /*<< Top URL for reference printing RUI if supported, NULL otherwise. */
    OI_CHAR *directTopUrl;              /*<< Top URL for direct printing RUI if supported, NULL otherwise. */
} OI_BPP_PRINTER_CAPS;

/**
 * Registers a BPP printer.
 *
 * @param caps  Pointer to printer cabailities struct. This must remain live as
 * long as the printer is registered
 * @param cb    Pointer to printer server callbacks
 * @param sdpStrings    Pointer to localized service record strings
 * @param authentication    enumeration value describing the type of
 * authentication, if any, the printer requires
 * @param jobChannel    out parameter indicating the rfcomm channel number of
 * the job channel
 * @param statusChannel    out parameter indicating the rfcomm channel number of
 * the status channel
 */

OI_STATUS OI_BPP_PRINTER_Register(const OI_BPP_PRINTER_CAPS *caps,
                                  const OI_BPP_PRINTER_CALLBACKS *cb,
                                  const OI_SDP_STRINGS *sdpStrings,
                                  OI_OBEXSRV_AUTHENTICATION authentication,
                                  OI_UINT8 *jobChannel,
                                  OI_UINT8 *statusChannel);

/**
 * Deregisters a BPP printer.
 * @returns     OI_OK if deregistration was successful, an error otherwise.
 */
OI_STATUS OI_BPP_PRINTER_Deregister(void);

/**
 * This function forcibly severs either the JOB or STATUS connection from a
 * BPP client to the OBEX server. The disconnect indication callback will be
 * called when the disconnect is complete.
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
OI_STATUS OI_BPP_PRINTER_ForceDisconnect(OI_OBEXSRV_CONNECTION_HANDLE handle);

#ifdef OI_DEPRECATED
/**
 * @deprecated
 *
 * Use @ref OI_BPP_PRINTER_Cancel() instead.  OI_BPP_PRINTER_Cancel(handle,
 * NULL) is equivalent to OI_BPP_PRINTER_Abort(handle).
 */
OI_STATUS OI_BPP_PRINTER_Abort(OI_OBEXCLI_CONNECTION_HANDLE handle);
#endif

/**@}*/

#endif /* _OI_BPP_PRINTER_H */
