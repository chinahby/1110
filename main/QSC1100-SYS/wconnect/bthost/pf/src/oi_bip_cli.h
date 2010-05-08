/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bip_cli.h#1 $ 

$Log: $
*
*    #4        18 Oct 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
*    #3        23 Jul 2004            JH
* Moved to version 3.0.12 revision 1
*
===========================================================================*/
/** @file 
 * This file provides the API for the client side of the Basic Imaging Profile.
 *
 * Most functions in this API involve a parameter block. These blocks are
 * explained by oi_bip_spec.h.
 *
 * A note about object liftimes:
 *
 * In keeping with the usual conventions of BM3, all indirectly referenced data
 * passed to a BIP function must remain valid until the corresponding callback
 * is invoked. This means, for example, that storage for parameters should not
 * be allocated on the stack of the function calling into BIP.
 *
 * There is one specific class of exception to this rule for BIP. An
 * OI_OBEX_UNICODE or OI_OBEX_BYTESEQ entry in a parameter block may be safely
 * allocated on the stack because its entries are copied into an internal data
 * structre by the BIP call. The data pointed to by these structures must obey
 * the usual rules, however. If a programmer chooses to take advantage of this
 * exception, it is very important that the entry NOT be referenced by the
 * callback function because it will still contain a stale pointer to a stack
 * variable that has since gone out of scope. The application must have another
 * reference to the underlying data if it needs to free it or process it
 * further.
 */

/** \addtogroup BIP BIP APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef _OI_BIPCLI_H
#define _OI_BIPCLI_H

#include "oi_bip_spec.h"
#include "oi_obexspec.h"
#include "oi_obexcli.h"

typedef OI_OBEXCLI_CONNECTION_HANDLE  OI_BIPCLI_HANDLE;

/**
 * Indicates an authentication request for a BIP connection
 * @param connection    The connection handle associated with the request. The
 * client should respond with OI_BIPCLI_AuthenticationResponse
 */
typedef void(*OI_BIPCLI_AUTH_IND)(OI_BIPCLI_HANDLE connection);

/**
 * Indicates a connection request has been completed.
 * @param connection    The connection handle
 * @param status    The result of the connection: OI_OK or an error
 */
typedef void(*OI_BIPCLI_CONNECT_CFM)(OI_BIPCLI_HANDLE connection,
                                     OI_STATUS status);

/**
 * Indicates that a connection is no longer in place
 */
typedef void(*OI_BIPCLI_DISCONNECT_IND)(OI_BIPCLI_HANDLE connection);



/* *************
 * ************* GetCapabilities
 * *************
 */


/**
 * Callback indicating the result of a GET_CAPABILITIES operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_GET_CAPABILITIES_CFM)(OI_BIPCLI_HANDLE connection,
                                              OI_BIP_GET_CAPABILITIES_PARAMS *params,
                                              OI_STATUS result);

/**
 * Performs a BIP GET_CAPABILITIES operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_GetCapabilities(OI_BIPCLI_HANDLE connection,
                                    OI_BIPCLI_GET_CAPABILITIES_CFM cb,
                                    OI_BIP_GET_CAPABILITIES_PARAMS *params);

/* *************
 * ************* PutImage
 * *************
 */

/**
 * Callback indicating the result of a PUT_IMAGE operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_PUT_IMAGE_CFM)(OI_BIPCLI_HANDLE connection,
                                       OI_BIP_PUT_IMAGE_PARAMS *params,
                                       OI_STATUS result);
/**
 * Performs a BIP PUT_IMAGE operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */

OI_STATUS OI_BIPCLI_PutImage(OI_BIPCLI_HANDLE connection,
                             OI_BIPCLI_PUT_IMAGE_CFM cb,
                             OI_BIP_PUT_IMAGE_PARAMS *params);

/* *************
 * ************* PutLinkedThumbnail
 * *************
 */

/**
 * Callback indicating the result of a PUT_LINKED_THUMBNAIL operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_PUT_LINKED_THUMBNAIL_CFM)(OI_BIPCLI_HANDLE connection,
                                                  OI_BIP_PUT_LINKED_THUMBNAIL_PARAMS *params,
                                                  OI_STATUS result);

/**
 * Performs a BIP PUT_LINKED_THUMBNAIL operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_PutLinkedThumbnail(OI_BIPCLI_HANDLE connection,
                                       OI_BIPCLI_PUT_LINKED_THUMBNAIL_CFM cb,
                                       OI_BIP_PUT_LINKED_THUMBNAIL_PARAMS *params);

/* *************
 * ************* PutLinkedAttachment
 * *************
 */

/**
 * Callback indicating the result of a PUT_LINKED_ATTACHMENT operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_PUT_LINKED_ATTACHMENT_CFM)(OI_BIPCLI_HANDLE connection,
                                                   OI_BIP_PUT_LINKED_ATTACHMENT_PARAMS *params,
                                                   OI_STATUS result);

/**
 * Performs a BIP PUT_LINKED_ATTACHMENT operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_PutLinkedAttachment(OI_BIPCLI_HANDLE connection,
                                        OI_BIPCLI_PUT_LINKED_ATTACHMENT_CFM cb,
                                        OI_BIP_PUT_LINKED_ATTACHMENT_PARAMS *params);


/* *************
 * ************* RemoteDisplay
 * *************
 */

/**
 * Callback indicating the result of a REMOTE_DISPLAY operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_REMOTE_DISPLAY_CFM)(OI_BIPCLI_HANDLE connection,
                                            OI_BIP_REMOTE_DISPLAY_PARAMS *params,
                                            OI_STATUS result);


/**
 * Performs a BIP REMOTE_DISPLAY operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_RemoteDisplay(OI_BIPCLI_HANDLE connection,
                                  OI_BIPCLI_REMOTE_DISPLAY_CFM cb,
                                  OI_BIP_REMOTE_DISPLAY_PARAMS *params);


/* *************
 * ************* GetImagesList
 * *************
 */

/**
 * Callback indicating the result of a GET_IMAGES_LIST operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_GET_IMAGES_LIST_CFM)(OI_BIPCLI_HANDLE connection,
                                             OI_BIP_GET_IMAGES_LIST_PARAMS *params,
                                             OI_STATUS result);

/**
 * Performs a BIP GET_IMAGES_LIST operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_GetImagesList(OI_BIPCLI_HANDLE connection,
                                  OI_BIPCLI_GET_IMAGES_LIST_CFM cb,
                                  OI_BIP_GET_IMAGES_LIST_PARAMS *params);

/* *************
 * ************* GetImageProperties
 * *************
 */


/**
 * Callback indicating the result of a GET_IMAGE_PROPERTIES operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_GET_IMAGE_PROPERTIES_CFM)(OI_BIPCLI_HANDLE connection,
                                                  OI_BIP_GET_IMAGE_PROPERTIES_PARAMS *params,
                                                  OI_STATUS result);

/**
 * Performs a BIP GET_IMAGE_PROPERTIES operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_GetImageProperties(OI_BIPCLI_HANDLE connection,
                                       OI_BIPCLI_GET_IMAGE_PROPERTIES_CFM cb,
                                       OI_BIP_GET_IMAGE_PROPERTIES_PARAMS *params);

/* *************
 * ************* GetImage
 * *************
 */

/**
 * Callback indicating the result of a GET_IMAGE operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_GET_IMAGE_CFM)(OI_BIPCLI_HANDLE connection,
                                       OI_BIP_GET_IMAGE_PARAMS *params,
                                       OI_STATUS result);

/**
 * Performs a BIP GET_IMAGE operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_GetImage(OI_BIPCLI_HANDLE connection,
                             OI_BIPCLI_GET_IMAGE_CFM cb,
                             OI_BIP_GET_IMAGE_PARAMS *params);


/* *************
 * ************* GetLinkedThumbnail
 * *************
 */

/**
 * Callback indicating the result of a GET_LINKED_THUMBNAIL operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_GET_LINKED_THUMBNAIL_CFM)(OI_BIPCLI_HANDLE connection,
                                                  OI_BIP_GET_LINKED_THUMBNAIL_PARAMS *params,
                                                  OI_STATUS result);

/**
 * Performs a BIP GET_LINKED_THUMBNAIL operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_GetLinkedThumbnail(OI_BIPCLI_HANDLE connection,
                                       OI_BIPCLI_GET_LINKED_THUMBNAIL_CFM cb,
                                       OI_BIP_GET_LINKED_THUMBNAIL_PARAMS *params);


/* *************
 * ************* GetLinkedAttachment
 * *************
 */

/**
 * Callback indicating the result of a GET_LINKED_ATTACHMENT operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_GET_LINKED_ATTACHMENT_CFM)(OI_BIPCLI_HANDLE connection,
                                                   OI_BIP_GET_LINKED_ATTACHMENT_PARAMS *params,
                                                   OI_STATUS result);

/**
 * Performs a BIP GET_LINKED_ATTACHMENT operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_GetLinkedAttachment(OI_BIPCLI_HANDLE connection,
                                        OI_BIPCLI_GET_LINKED_ATTACHMENT_CFM cb,
                                        OI_BIP_GET_LINKED_ATTACHMENT_PARAMS *params);

/* *************
 * ************* DeleteImage
 * *************
 */

/**
 * Callback indicating the result of a DELETE_IMAGE operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_DELETE_IMAGE_CFM)(OI_BIPCLI_HANDLE connection,
                                          OI_STATUS result);

/**
 * Performs a BIP DELETE_IMAGE operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_DeleteImage(OI_BIPCLI_HANDLE connection,
                                OI_BIPCLI_DELETE_IMAGE_CFM cb,
                                OI_BIP_DELETE_IMAGE_PARAMS *params);

/* *************
 * ************* StartPrint
 * *************
 */

/**
 * Callback indicating the result of a START_PRINT operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_START_PRINT_CFM)(OI_BIPCLI_HANDLE connection,
                                         OI_BIP_START_PRINT_PARAMS *params,
                                         OI_STATUS result);

/**
 * Performs a BIP START_PRINT operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_StartPrint(OI_BIPCLI_HANDLE connection,
                               OI_BIPCLI_START_PRINT_CFM cb,
                               OI_BIP_START_PRINT_PARAMS *params);

/* *************
 * ************* GetPartialImage
 * *************
 */

/**
 * Callback indicating the result of a GET_PARTIAL_IMAGE operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_GET_PARTIAL_IMAGE_CFM)(OI_BIPCLI_HANDLE connection,
                                               OI_BIP_GET_PARTIAL_IMAGE_PARAMS *params,
                                               OI_STATUS result);

/**
 * Performs a BIP GET_PARTIAL_IMAGE operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_GetPartialImage(OI_BIPCLI_HANDLE connection,
                                    OI_BIPCLI_GET_PARTIAL_IMAGE_CFM cb,
                                    OI_BIP_GET_PARTIAL_IMAGE_PARAMS *params);

/* *************
 * ************* StartArchive
 * *************
 */

/**
 * Callback indicating the result of a START_ARCHIVE operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_START_ARCHIVE_CFM)(OI_BIPCLI_HANDLE connection,
                                           OI_STATUS result);

/**
 * Performs a BIP START_ARCHIVE operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_StartArchive(OI_BIPCLI_HANDLE connection,
                                 OI_BIPCLI_START_ARCHIVE_CFM cb,
                                 OI_BIP_START_ARCHIVE_PARAMS *params);

/* *************
 * ************* GetMonitoringImage
 * *************
 */

/**
 * Callback indicating the result of a GET_MONITORING_IMAGE operation.
 * @param connection    The connection handle for this request
 * @param params    A pointer to the parameter block specified by the
 * application during the request.
 * @param result    The response from the server. If this is anything other than
 * OI_OBEX_CONTINUE then it is safe for the application to deallocate or
 * otherwise destroy the parameter block.
 */
typedef void(*OI_BIPCLI_GET_MONITORING_IMAGE_CFM)(OI_BIPCLI_HANDLE connection,
                                                  OI_BIP_GET_MONITORING_IMAGE_PARAMS *params,
                                                  OI_STATUS result);

/**
 * Performs a BIP GET_MONITORING_IMAGE operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS OI_BIPCLI_GetMonitoringImage(OI_BIPCLI_HANDLE connection,
                                       OI_BIPCLI_GET_MONITORING_IMAGE_CFM cb,
                                       OI_BIP_GET_MONITORING_IMAGE_PARAMS *params);

/* *************
 * ************* GetStatus
 * *************
 */
/**
 * Callback indicating the result of a GET_STATUS operation.
 * @param connection    The connection handle for this request
 * @param result    The status result from the server
 */
typedef void(*OI_BIPCLI_GET_STATUS_CFM)(OI_BIPCLI_HANDLE connection,
                                        OI_STATUS result);
/**
 * Performs a BIP GET_STATUS operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 */

OI_STATUS OI_BIPCLI_GetStatus(OI_BIPCLI_HANDLE connection,
                              OI_BIPCLI_GET_STATUS_CFM cb);



/* *************  */


/**
 * Connects to a BIP server.
 *
 * @param addr      The BD_ADDR of the server
 * @param channel   The RFCOMM channel number of the server
 * @param target    The OBEX Target UUID of the server
 * @param authInd   A callback handling server authentication challenges
 * @param connectCfm    A callback indicating completion of the connection
 * attempt
 * @param disconnectInd A callback indicating termination of the connection
 * @param handle    An out parameter receiving the handle for this connection, to
 * be used in subsequent function requests.
 */
OI_STATUS OI_BIPCLI_Connect(OI_BD_ADDR *addr,
                            OI_UINT8 channel,
                            OI_OBEX_BYTESEQ const *target,
                            OI_BIPCLI_AUTH_IND authInd,
                            OI_BIPCLI_CONNECT_CFM connectCfm,
                            OI_BIPCLI_DISCONNECT_IND disconnectInd,
                            OI_BIPCLI_HANDLE *handle);

/**
 * Disconnects from a BIP server
 * @param connection    The handle of the connection to be dropped
 */
OI_STATUS OI_BIPCLI_Disconnect(OI_BIPCLI_HANDLE connection);


/**
 * Responds to an authentication challenge
 * @param connection    The handle of the connection as specified by the
 * authentication challenge callback
 * @param pin       Pointer to a NULL-terminated pin, or NULL to refuse
 * authentication
 */
void OI_BIPCLI_AuthenticationResponse(OI_BIPCLI_HANDLE connection,
                                           const OI_CHAR *pin);


/**
 * A callback function of this type is called when a cancel request completes.
 *
 * @param connection   handle for the BIP connection.
 */
typedef void (*OI_BIPCLI_CANCEL_CFM)(OI_BIPCLI_HANDLE connection);


/**
 * Terminates the current BIP operation on this connection. If there is a
 * command in progress this function returns OI_STATUS_PENDING and the operation
 * will be canceled as soon as possible. If there is no command in progress
 * the operation will be canceled immediately and the cancel confirm callback
 * will be called when the cancel is complete. If there is no current BIP
 * operation this function returns OI_STATUS_INVALID_STATE.
 *
 * @param connection      This parameter is a unique ID that represents an
 *                        established connection to an OBEX server.
 *
 * @param cancelCfm       function that will be invoked when the completes
 *                        This function is only called if the return value is
 *                        OI_OK. If the return value is OI_STATUS_PENDING the
 *                        callback for the current command will be called
 *                        instead.
 *
 * @return                - OI_OK if the operation was aborted.
 *                        - OI_STATUS_PENDING if the abort is deferred
 *                        - OI_STATUS_INVALID_STATE if there is nothing to abort
 */
OI_STATUS OI_BIPCLI_Cancel(OI_BIPCLI_HANDLE connection,
                           OI_BIPCLI_CANCEL_CFM cancelCfm);


/**
 * Associates a caller defined context with an BIP client connection. This
 * context can then be retrieved by calling OI_BIPCLI_GetContext().
 *
 * @param connection     The BIP connection to associate the context with.
 *
 * @param context         A value supplied by the caller.
 *
 * @return                OI_OK if the context was set, OI_STATUS_NOT_FOUND if
 *                        the connection handle is not valid.
 */
OI_STATUS OI_BIPCLI_SetContext(OI_BIPCLI_HANDLE connection,
                               void *context);


/**
 * Gets a caller defined context associate with an BIP client connection. This
 * is a value that was previously set by a call to OI_BIPCLI_SetContext().
 *
 * @param connection      The BIP client connection to get the context from.
 *
 * @return                A context pointer or NULL if the handle is invalid or
 *                        there is no context associated with this connection.
 */
void* OI_BIPCLI_GetContext(OI_BIPCLI_HANDLE connection);


/**@}*/

#ifdef OI_DEPRECATED
/**
 * DEPRECATED: Use OI_BIPCLI_Cancel instead
 *
 * Aborts an operation in progress
 * @param connection    The handle of the connection whose operation to abort
 */
OI_STATUS OI_BIPCLI_Abort(OI_BIPCLI_HANDLE connection);
#endif

#endif /* _OI_BIP_CLI_H */
