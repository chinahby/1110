/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H   P H O N E B O O K   A C C E S S   P R O F I L E   
                                C L I E N T

GENERAL DESCRIPTION
  This module contains the Bluetooth Phone Book Access Profile client 
  data and code.

Copyright (c) 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/inc/pbap_cli.h#1 $ 

$Log: $
*
*    #1        06 Feb 2006            GS
* Initial version
*

===========================================================================*/

#ifndef PBAP_CLI_H
#define PBAP_CLI_H

#include "pbap_spec.h"
#include "pbap_private.h"
#include "oi_obexspec.h"
#include "oi_obexcli.h"

/**
 * This value controls whether a set path request is setting the folder path to a specific
 * folder, to the parent of the current folder, or to the root folder as defined
 * by the remote server.
 */
typedef enum {
    PBAP_CLIENT_SETPATH_TO_FOLDER,
    PBAP_CLIENT_SETPATH_TO_PARENT,
    PBAP_CLIENT_SETPATH_TO_ROOT
} PBAP_CLIENT_SETPATH_CONTROL;


typedef OI_OBEXCLI_CONNECTION_HANDLE  PBAPCLI_HANDLE;

/**
 * Indicates an authentication request for a PBAP connection
 * @param connection    The connection handle associated with the request. The
 * client should respond with PBAPCLI_AuthenticationResponse
 */
typedef void(*PBAPCLI_AUTH_IND)(PBAPCLI_HANDLE connection, boolean userIdRequired);

/**
 * Indicates a connection request has been completed.
 * @param connection    The connection handle
 * @param status    The result of the connection: OI_OK or an error
 */
typedef void(*PBAPCLI_CONNECT_CFM)(PBAPCLI_HANDLE connection,
                                   OI_STATUS status);

/**
 * Indicates that a connection is no longer in place
 */
typedef void(*PBAPCLI_DISCONNECT_IND)(PBAPCLI_HANDLE connection);

/**
 * Callback indicating the result of a PULL_PHONE_BOOK operation.
 */
typedef void(*PBAPCLI_PULL_PHONE_BOOK_CFM)(PBAPCLI_HANDLE connection,
                                           PBAP_PULL_PHONE_BOOK_PARAMS *params,
                                           OI_STATUS result);

/**
 * Performs a PBAP PullPhoneBook operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */
OI_STATUS PBAPCLI_PullPhoneBook(PBAPCLI_HANDLE connection,
                                PBAPCLI_PULL_PHONE_BOOK_CFM cb,
                                PBAP_PULL_PHONE_BOOK_PARAMS *params);

/**
 * Callback indicating the result of a SET_PHONE_BOOK operation.
 */
typedef void(*PBAPCLI_SET_PHONE_BOOK_CFM)(PBAPCLI_HANDLE connection,
                                          OI_STATUS result);

/**
 * This function sets the current folder on the remote server for subsequent put and get
 * operations.
 */
OI_STATUS PBAPCLI_SetPhoneBook(PBAPCLI_HANDLE connection,
                               PBAPCLI_SET_PHONE_BOOK_CFM cb,
                               OI_OBEX_UNICODE *folder,
                               PBAP_CLIENT_SETPATH_CONTROL setPathCtrl);

/**
 * Callback indicating the result of a PULL_VCARD_LISTING operation.
 */
typedef void(*PBAPCLI_PULL_VCARD_LISTING_CFM)(PBAPCLI_HANDLE connection,
                                              PBAP_PULL_VCARD_LISTING_PARAMS *params,
                                              OI_STATUS result);

/**
 * Performs a PBAP PullvCardListing operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */

OI_STATUS PBAPCLI_PullvCardListing(PBAPCLI_HANDLE connection,
                                   PBAPCLI_PULL_VCARD_LISTING_CFM cb,
                                   PBAP_PULL_VCARD_LISTING_PARAMS *params);
                                  

/**
 * Callback indicating the result of a PULL_VCARD_ENTRY operation.
 */
typedef void(*PBAPCLI_PULL_VCARD_ENTRY_CFM)(PBAPCLI_HANDLE connection,
                                            PBAP_PULL_VCARD_ENTRY_PARAMS *params,
                                            OI_STATUS result);

/**
 * Performs a PBAP PullvCardEntry operation.
 * @param connection    The connection handle for this request
 * @param cb    The completion callback
 * @param params    A pointer to a parameter block, allocated by the connection,
 * to be used for any requests and responses. This must remain valid until the
 * final confirmation callback.
 */

OI_STATUS PBAPCLI_PullvCardEntry(PBAPCLI_HANDLE connection,
                                 PBAPCLI_PULL_VCARD_ENTRY_CFM cb,
                                 PBAP_PULL_VCARD_ENTRY_PARAMS *params);
                                

/**
 * Connects to a PBAP server.
 *
 * @param addr      The BD_ADDR of the server
 * @param channel   The RFCOMM channel number of the server
 * @param authInd   A callback handling server authentication challenges
 * @param connectCfm    A callback indicating completion of the connection
 * attempt
 * @param disconnectInd A callback indicating termination of the connection
 * @param handle    An out parameter receiving the handle for this connection, to
 * be used in subsequent function requests.
 */
OI_STATUS PBAPCLI_Connect(OI_BD_ADDR *addr,
                             OI_UINT8 channel,
                             PBAPCLI_AUTH_IND authInd,
                             PBAPCLI_CONNECT_CFM connectCfm,
                             PBAPCLI_DISCONNECT_IND disconnectInd,
                             PBAPCLI_HANDLE *handle);

/**
 * Disconnects from a PBAP server
 * @param connection    The handle of the connection to be dropped
 */
OI_STATUS PBAPCLI_Disconnect(PBAPCLI_HANDLE connection);

/**
 * Aborts an operation in progress
 * @param connection    The handle of the connection whose operation to abort
 */
OI_STATUS PBAPCLI_Abort(PBAPCLI_HANDLE connection);

/**
 * Responds to an authentication challenge
 * @param connection    The handle of the connection as specified by the
 * authentication challenge callback
 * @param pin       Pointer to a NULL-terminated pin, or NULL to refuse
 * authentication
 */
OI_STATUS PBAPCLI_AuthenticationResponse(PBAPCLI_HANDLE connection,
                                         const OI_BYTE *userId,
                                         OI_UINT8 userIdLen,
                                         const OI_CHAR *pin);

#endif /* PBAP_CLI_H */
