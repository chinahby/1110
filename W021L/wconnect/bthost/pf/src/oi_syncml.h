#ifndef _OI_SYNCML_H
#define _OI_SYNCML_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_syncml.h#1 $ 

$Log: $
*
*    #8        18 Oct  2006           PR
*  Merged OI Code v3.0.14rev1revQ
*
*    #7        28 July 2006           BK
*    Modified to initiate obex authentication from client
*
*    #6        30 Aug 2004            JH
* Modified prototype of OI_SyncML_RegisterServer() to add RFCOMM server channel
*  number as output parameter.
*
===========================================================================*/
/** @file
 * This file includes the API for the SyncML OBEX bindings. These
 * bindings comprise a small number of helper functions layered on top
 * of the OBEX API functions. Much of the OBEX API is still exposed
 * in its raw form and therefore the OBEX APIs in oi_obexsrv.h and
 * oi_obexcli.h should be referred to for more details on the
 * OBEX-specific datatypes, constants, and callbacks.
 */

/** \addtogroup SyncML SyncML APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_dataelem.h"
#include "oi_syncml.h"
#include "oi_obexcli.h"
#include "oi_obexspec.h"
#include "oi_obexsrv.h"

/* SyncML SDP UUIDs - static initializers for OI_UUID128 type. */
#define OI_UUID_SyncMLServer { 0x00000001, { 0, 0, 0x10, 0, 0x80, 0, 0, 0x02, 0xEE, 0, 0, 0x02} }
#define OI_UUID_SyncMLClient { 0x00000002, { 0, 0, 0x10, 0, 0x80, 0, 0, 0x02, 0xEE, 0, 0, 0x02} }

/* SyncML OBEX UUIDs */
#define OI_SYNCML_UUID_SYNC "SYNCML-SYNC"
#define OI_SYNCML_UUID_DM "SYNCML-DM"

/* SyncML MIME types */
#define OI_SYNCML_MIME_XML "application/vnd.syncml+xml"
#define OI_SYNCML_MIME_WBXML "application/vnd.syncml+wbxml"
#define OI_SYNCML_MIME_DM_XML "application/vnd.syncml.dm+xml"
#define OI_SYNCML_MIME_DM_WBXML "application/vnd.syncml.dm+wbxml"


/**
 * Register a SyncML server. This registers a server with OBEX and
 * installs a service record in the SDP database.
 * 
 * @param strings         the name to register for the service (must not be NULL)
 * @param authentication  TRUE to require authentication
 * @param CBList          the list of OBEX callbacks to use
 * @param uuid            the OBEX UUID to be used with this server
 */
#ifdef OI_CODE
#error code not present
#else
OI_STATUS OI_SyncML_RegisterServer(const OI_SDP_STRINGS *strings,
                                   OI_BOOL authentication,
                                   OI_OBEXSRV_CB_LIST *CBList,
                                   OI_CHAR *uuid,
                                   OI_UINT8 *channel);
#endif

/**
 * Get the service record handle associated with this service.  
 * This can be used with e.g. OI_SDPDB_SetAttributeList to add
 * vendor-specific SDP attributes to the profile.
 *
 * @param handle  return the service record's handle
 */
OI_STATUS OI_SyncML_GetServerServiceRecord(OI_UINT32 *handle);

/**
 * Deregister a SyncML server. This deregisters a server with OBEX,
 * removes its service record in the SDP database, and frees memory.
 */
OI_STATUS OI_SyncML_DeregisterServer(void);

#ifdef OI_CODE
#error code not present
#else
/**
 * Register a SyncML client. This registers a server with OBEX and
 * installs a service record in the SDP database.
 *
 * @param strings         the name to register for the service (must not be NULL)
 * @param authentication  TRUE to require authentication
 * @param CBList          the list of OBEX callbacks to use
 * @param uuid            the OBEX UUID to be used with this server
 * @param channel         the unique SYNCML ID of this client channel
 */
OI_STATUS OI_SyncML_RegisterClient(const OI_SDP_STRINGS *strings,
                                   OI_BOOL authentication,
                                   OI_OBEXSRV_CB_LIST *CBList,
                                   OI_CHAR *uuid,
                                   OI_UINT8 *channel);
#endif /* OI_CODE */

/**
 * Get the service record handle associated with this service.  
 * This can be used with e.g. OI_SDPDB_SetAttributeList to add
 * vendor-specific SDP attributes to the profile.
 *
 * @param handle  return the service record's handle
 */
OI_STATUS OI_SyncML_GetClientServiceRecord(OI_UINT8 channel, OI_UINT32 *handle);


/**
 * Deregister a SyncML client. This deregisters a server with OBEX,
 * removes its service record in the SDP database, and frees memory.
 * 
 * @param channel  The unique SYNCML ID of this client channel
 */
OI_STATUS OI_SyncML_DeregisterClient(OI_UINT8 channel);

/**
 * Connect to a remote OBEX server for the purpose of using SyncML.
 * 
 * @param addr              the remote device to connect to
 * @param channel           the RFCOMM server channel to connect on
 * @param uuid              the OBEX UUID to connect to
 * @param connectCfm        the OBEX connect callback
 * @param disconnectInd     the OBEX disconnect callback
 * @param authChallengeInd  the OBEX authentication callback
 * @param connectionId      [OUT] parameter returning the connectionId for connection
 */
OI_STATUS OI_SyncML_Connect(OI_BD_ADDR *addr,
                            OI_UINT8 channel,
                            OI_CHAR *uuid,
                            OI_OBEXCLI_CONNECT_CFM connectCfm,
                            OI_OBEXCLI_DISCONNECT_IND disconnectInd,
                            OI_OBEXCLI_AUTH_CHALLENGE_IND authChallengeInd,
                            OI_OBEXCLI_CONNECTION_HANDLE *connectionId);

/**
 * Send a PUT message on the specified connection.
 *
 * @param connectionId  the connection for the PUT operation.
 * @param putCfm        the OBEX PUT callback
 * @param mimeType      a \0 terminated ascii string describing the MIME type of the data
 * @param data          a pointer to the data to be sent
 * @param dataLen       the number of bytes available to be sent
 * @param putLen        [OUT] parameter returning the number of bytes sent
 * @param status        the OBEX status to send
 */
OI_STATUS OI_SyncML_Put(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                        OI_OBEXCLI_PUT_CFM putCfm,
                        OI_CHAR *mimeType,
                        OI_BYTE *data,
                        OI_UINT16 dataLen,
                        OI_UINT16 *putLen,                        
                        OI_STATUS status);

/**
 * Send a GET message on the specified connection.
 *
 * @param connectionId  the connection for the PUT operation
 * @param getRecvData   the OBEX receive callback
 * @param mimeType      a \0 terminated ascii string describing the MIME type of the data
 */
OI_STATUS OI_SyncML_Get(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                        OI_OBEXCLI_GET_RECV_DATA getRecvData,
                        OI_CHAR *mimeType);

/**
 * Send a PUT response on the specified connection.
 *
 * @param connectionId  the connection for the PUT operation
 * @param status        the OBEX status to send
 */
OI_STATUS OI_SyncML_PutResponse(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                OI_STATUS status);

/**
 * Send a GET response on the specified connection.
 *
 * @param connectionId  the connection for the GET operation
 * @param data          a pointer to the data to be sent
 * @param dataLen       the number of bytes available to be sent
 * @param putLen        [OUT] parameter returning the number of bytes sent
 * @param status        the OBEX status to send
 */
OI_STATUS OI_SyncML_GetResponse(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                OI_BYTE *data,
                                OI_UINT16 dataLen,
                                OI_UINT16 *putLen,  
                                OI_STATUS status);

/**
 * Disconnect from an OBEX server.
 *
 * @param connectionId    the connection to disconnect
 */
OI_STATUS OI_SyncML_Disconnect(OI_OBEXCLI_CONNECTION_HANDLE connectionId);

/**@}*/

#endif /* _OI_SYNCML_H */

