/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H   P H O N E B O O K   A C C E S S   P R O F I L E   
                                S E R V E R

GENERAL DESCRIPTION
  This module contains the Bluetooth Phone Book Access Profile server 
  data and code.

Copyright (c) 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/pbap_srv.h#1 $ 

$Log: $
*    #2        04 Dec 2007            MH
* Added support for updating BD Address with connect indication.
*
*    #1        06 Feb 2006            GS
* Initial version
*

===========================================================================*/

#ifndef PBAP_SRV_H
#define PBAP_SRV_H

#include "pbap_spec.h"
#include "oi_obexsrv.h"
#include "oi_obexspec.h"

typedef OI_OBEXSRV_CONNECTION_HANDLE PBAPSRV_HANDLE;


/**
 * Indicates a request for a PBAP connection. The server should return OI_OK to
 * allow the connection, or an error code to reject it. 
 *
 * @param connectionId  Connection handle to be used in conjunction with this
 * connection in future calls.
 */
#ifdef OI_CODE
#error code not present
#else
typedef OI_STATUS (*PBAPSRV_CONNECT_IND)(PBAPSRV_HANDLE connectionId,
                                         OI_BYTE*       userId,
                                         OI_UINT8       userIdLen,
                                         OI_BD_ADDR     *clientAddr);
#endif
/**
 * Indicates that a PBAP client has terminated its connection to the server
 *
 * @param connection Connection handle to the connection which is being dropped.
 */
typedef void (*PBAPSRV_DISCONNECT_IND)(PBAPSRV_HANDLE connectionId);

/**
 * This callback is invoked prior to a connection indication when the server has
 * been configured to require authentication. The server should respond by
 * calling PBAPSRV_AuthenticationResponse() with the user-id and pin to use for this
 * connection.
 * @param connectionId Connection handle to the connection on which authentication 
 * is being performed.
 */
typedef void (*PBAPSRV_AUTH_IND)(PBAPSRV_HANDLE connectionId);

/**
 * The server responds to an authentication request with this function.
 * @param connectionId  Connection handle to the connection on which authentication 
 * is being performed. 
 */
void PBAPSRV_AuthenticationResponse(PBAPSRV_HANDLE connectionId,
                                    OI_CHAR *pin);


/* PullPhoneBook */

/**
  @param connectionId Connection handle to the connection in question.
  @param params Parameters specified by the PBAP client.
  @param state The phase of the response
  @param status Whether the client has more data (deferred result from last response sent)
  @return OI_OK if the operation was successful.
  */
typedef OI_STATUS (*PBAPSRV_PULL_PHONE_BOOK_IND)(PBAPSRV_HANDLE connectionId,
                                                 PBAP_PULL_PHONE_BOOK_PARAMS *params,
                                                 PBAP_REQUEST_STATE state,
                                                 OI_STATUS status);

/**
  @param connectionId Connection handle to the connection in question.
  @param params Parameters specified by the PBAP client.
  @param result Returned by callback function; always OI_OK except in case of catastrophic failure.
  @return OI_OK if the operation was successful.
  */
OI_STATUS PBAPSRV_PullPhoneBookResponse(PBAPSRV_HANDLE connectionId,
                                        PBAP_PULL_PHONE_BOOK_PARAMS *params,
                                        OI_STATUS result);

/* SetPhoneBook */

/**
  @param connectionId Connection handle to the connection in question.
  @param params Parameters specified by the PBAP client.
  @param state The phase of the response
  @param status Whether the client has more data (deferred result from last response sent)
  @return OI_OK if the operation was successful.
  */
typedef OI_STATUS (*PBAPSRV_SET_PHONE_BOOK_IND)(PBAPSRV_HANDLE connectionId,
                                                OI_OBEX_UNICODE* folder,
                                                OI_BOOL level);

/**
  @param connectionId Connection handle to the connection in question.
  @param params Parameters specified by the PBAP client.
  @param result Returned by callback function; always OI_OK except in case of catastrophic failure.
  @return OI_OK if the operation was successful.
  */
OI_STATUS PBAPSRV_SetPhoneBookResponse(PBAPSRV_HANDLE connectionId,
                                       OI_STATUS result);

/* PullvCardListing */

/**
  @param connectionId Connection handle to the connection in question.
  @param params Parameters specified by the PBAP client.
  @param state The phase of the response
  @param status Whether the client has more data (deferred result from last response sent)
  @return OI_OK if the operation was successful.
  */
typedef OI_STATUS (*PBAPSRV_PULL_VCARD_LISTING_IND)(PBAPSRV_HANDLE connectionId,
                                                    PBAP_PULL_VCARD_LISTING_PARAMS *params,
                                                    PBAP_REQUEST_STATE state,
                                                    OI_STATUS status);

/**
  @param connectionId Connection handle to the connection in question.
  @param params Parameters specified by the PBAP client.
  @param result Returned by callback function; always OI_OK except in case of catastrophic failure.
  @return OI_OK if the operation was successful.
  */
OI_STATUS PBAPSRV_PullvCardListingResponse(PBAPSRV_HANDLE connectionId,
                                           PBAP_PULL_VCARD_LISTING_PARAMS *params,
                                           OI_STATUS result);

/* PullvCardEntry */

/**
  @param connectionId Connection handle to the connection in question.
  @param params Parameters specified by the PBAP client.
  @param state The phase of the response
  @param status Whether the client has more data (deferred result from last response sent)
  @return OI_OK if the operation was successful.
  */
typedef OI_STATUS (*PBAPSRV_PULL_VCARD_ENTRY_IND)(PBAPSRV_HANDLE connectionId,
                                                  PBAP_PULL_VCARD_ENTRY_PARAMS *params,
                                                  PBAP_REQUEST_STATE state,
                                                  OI_STATUS status);

/**
  @param connectionId Connection handle to the connection in question.
  @param params Parameters specified by the PBAP client.
  @param result Returned by callback function; always OI_OK except in case of catastrophic failure.
  @return OI_OK if the operation was successful.
  */
OI_STATUS PBAPSRV_PullvCardEntryResponse(PBAPSRV_HANDLE connectionId,
                                         PBAP_PULL_VCARD_ENTRY_PARAMS *params,
                                         OI_STATUS result);

/**
 * A application calls thsi function to accept or reject an indicated
 * connection.
 *
 * @param connectionId   handle representing the connection to an PBAP client.
 *
 * @param accept         TRUE if the connection is being accepted, FALSE
 *                       otherwise. 
 */

OI_STATUS PBAPSRV_AcceptConnection(PBAPSRV_HANDLE connectionId,
                                   OI_BOOL accept);


typedef struct {
    PBAPSRV_CONNECT_IND connectInd;
    PBAPSRV_DISCONNECT_IND disconnectInd;
    PBAPSRV_AUTH_IND authInd;

    PBAPSRV_PULL_PHONE_BOOK_IND pullPhoneBook;
    PBAPSRV_SET_PHONE_BOOK_IND setPhoneBook;
    PBAPSRV_PULL_VCARD_LISTING_IND pullvCardListing;
    PBAPSRV_PULL_VCARD_ENTRY_IND pullvCardEntry;
} PBAPSRV_CALLBACKS;


typedef struct {
  const OI_CHAR *serviceName; /**< @deprecated Should be NULL. */
  OI_UINT8 supportedRepositories;
} PBAPSRV_INFO;


/**
 * This function registers a PBAP server.
 *
 * @param cb               pointer to a struct specifying callbacks for the functions
 *                         implemented by this server
 * 
 * @param info             pointer to a struct containing information about the server, to
 *                         be published via SDP. The value of
 *                         info->supportedFeatures determines which OBEX targets
 *                         will be registered. At least one of its bits must be
 *                         set, or registration will fail.
 *
 * @param strings          name to register in service record 
 * @param authentication   TRUE if authentication is to be required of clients,
 *                         FALSE otherwise
 * 
 * @param channel          [OUT] parameter assigned the RFCOMM channel number of the
 *                         registered server
 *
 * @return                 OI_OK if registration completed successfully, an error otherwise
 */
OI_STATUS PBAPSRV_Register(const PBAPSRV_CALLBACKS *cb,
                           PBAPSRV_INFO *info,
                           const OI_SDP_STRINGS *strings,
                           OI_OBEXSRV_AUTHENTICATION authentication,
                           OI_UINT8 *channel);

/**
 * Deregisters a previously registered PBAP server.
 *
 * @param channel   The RFCOMM channel number of the server to deregister.
 * 
 * @return  OI_OK if registration completed successfully, an error otherwise.
 */
OI_STATUS PBAPSRV_DeregisterServer(OI_UINT8 channel);

/**
 * This function forcibly severs the connection from a PBAP client to the OBEX
 * server. The disconnect indication callback will be called when the disconnect
 * is complete.
 * 
 * A PBAP server may need to forcibly terminate a connection during
 * deregistration, since deregistration will fail if a connection is in place. 
 * 
 * @param connectionId     a unique identifier generated by the PBAP server that
 *                         identifies the connection.
 *
 * @return                 OI_OK if the connectionId is valid and the connection
 *                         will be terminated. 
 */
OI_STATUS PBAPSRV_ForceDisconnect(PBAPSRV_HANDLE connectionId);


/**@}*/

#endif /* PBAP_SRV_H */


