#ifndef _OI_FTP_SERVER_H
#define _OI_FTP_SERVER_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_ftp_server.h#1 $ 

$Log: $
*
*    #3        18 Oct  2006           PR
*  Merged OI Code v3.0.14rev1revQ
*
===========================================================================*/
/** 
  @file  

  This file provides the API for the server side of the File Transfer Profile.

  The File Transfer Profile provides functions for establishing a connection to
  a remote device that support the File Transfer Profile over RFCOMM and
  functions for putting and getting files. This implementation currently only
  allows one client connection at a time. 
  
  The file system support on the local device is defined by oi_ftp_file.c and is
  implementation-dependent. On server platforms with a native file system,
  oi_ftp_file.c will likely be a thin wrapper on the native file system APIs. On
  other platforms, oi_ftp_file.c might implement a simple in-memory object manager.
 */

/** \addtogroup FTP FTP APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_obexsrv.h"
#include "oi_status.h"
#include "oi_ftp_sys.h"

/**
 * Represents an active connection between the FTP server and a remote FTP
 * client
 */

typedef OI_FTP_CONNECTION OI_FTP_SERVER_CONNECTION_HANDLE;



/**
 * A callback function of this type is called to indicate a connection request.
 * The server application provides a function with this profile to
 * OI_FTPServer_Register()
 *
 * @param clientAddr     The Bluetooth device address of the client requesting
 *                       the connection.
 *
 * @param unauthorized   Indicates if the connection requires authentication. If
 *                       this parameter is TRUE the server application must call
 *                       OI_FTPServer_PasswordRsp() to provide a password.
 *                       After succesful authentication this function will be
 *                       called again wuth unauthorized == FALSE indicating that
 *                       the connection has been established. If authentication
 *                       was attempted but failed this unauthorized parameter
 *                       will still be TRUE.
 *
 * @param userId         The user id  of the user requesting the connection.
 *                       This parameter is only set for authenticated
 *                       connections and may be NULL in any case.
 *                       The application server may use this value to choose a
 *                       root directory or select a previously stored password.
 *
 * @param connectionId   handle representing the connection to an FTP client.
 *
 * @param userIdLen      Length of the user id.
 */

typedef void (*OI_FTP_CONNECTION_IND)(OI_BD_ADDR *clientAddr,
                                      OI_BOOL unauthorized,
                                      OI_BYTE *userId,
                                      OI_UINT8 userIdLen,
                                      OI_FTP_SERVER_CONNECTION_HANDLE connectionId);


/**
 * A callback function of this type is called to indicate a client has
 * disconnected from the FTP server. The server application provides a function
 * with this profile to OI_FTPServer_Register()
 *
 * @param connectionId   handle representing the connection to an FTP client.
 */

typedef void (*OI_FTP_DISCONNECTION_IND)(OI_FTP_SERVER_CONNECTION_HANDLE connectionId);


/**
 * This function is called by the application in response to a connnection
 * indication that requires authentication.
 *
 * @param connectionId   handle representing the connection to an FTP client.
 *
 * @param password       NULL-teminated password
 *
 * @param readOnly       indicates if the client is only being allowed read
 *                       access. A client that only granted read access cannot
 *                       create, put, or delete files and folders.
 *
 * @param allowBrowse    indicates if the client is allowed to browse the file
 *                       system. A client that is not permitted to browse can
 *                       still get and put files from the root folder.
 *
 */

OI_STATUS OI_FTPServer_AuthenticationRsp(OI_FTP_SERVER_CONNECTION_HANDLE connectionId,
                                         const OI_CHAR *password,
                                         OI_BOOL readOnly,
                                         OI_BOOL allowBrowse);


 /**
 * A application calls thsi function to accept or reject an indicated
 * connection.
 *
 * @param connectionId   handle representing the connection to an FTP client.
 *
 * @param accept         TRUE if the connection is being accepted, FALSE
 *                       otherwise. If FALSE the values of the remaining
 *                       parameters is ignored.
 *
 * @param rootFolder     the implementation-dependent root folder for this
 *                       connection.
 *
 */

OI_STATUS OI_FTPServer_AcceptConnection(OI_FTP_SERVER_CONNECTION_HANDLE connectionId,
                                        OI_BOOL accept,
                                        const OI_OBEX_UNICODE *rootFolder);


/**
 * Forcibly severs the connection from an FTP client to the FTP server. This
 * function should be called for all active connections before terminating the
 * FTP server.
 *
 * @param connectionId  handle representing the connection between a remote
 *                      client and the local FTP server.
 *
 * @return              OI_OK if the connection will be terminated.
 */

OI_STATUS OI_FTPServer_ForceDisconnect(OI_FTP_SERVER_CONNECTION_HANDLE connectionId);


/**
 * This function initialzes the FTP server to accept connections and registers
 * it with the SDP database so that the service becomes discoverable.
 *
 * @param authentication This parameter indicates whether connections to this
 *                       server must be authenticated using OBEX authentication.
 *                       If this parameter is FALSE, clients connecting to the
 *                       server may demand authentication. 
 *
 * @param readOnly       Default access for unauthenticated clients. Applies to
 *                       all clients in authenticated == FALSE.
 *
 * @param allowBrowse    Default browse permission for unauthenticated clients. 
 *                       Applies to all clients if authentication == FALSE.
 *
 * @param connectInd     a callback function for indicating a incoming client
 *                       connection to the server application.
 * @param disconnectInd  a callback function for indicating a disconnection
 *
 * @param fileOperations interface to file system operations.
 *
 * @param strings        strings to register in service record
 *
 * @param channelNumber  [OUT] returns the RFCOMM channel number allocate for
 *                       the server. The channel number is required for
 *                       OI_FTPServer_Deregister().
 *
 * @return               OI_OK if the service was successfully registered.
 */

OI_STATUS OI_FTPServer_Register(OI_OBEXSRV_AUTHENTICATION authentication,
                                OI_BOOL readOnly,
                                OI_BOOL allowBrowse,
                                OI_FTP_CONNECTION_IND connectInd,
                                OI_FTP_DISCONNECTION_IND disconnectInd,
                                const OI_FTP_FILESYS_FUNCTIONS *fileOperations,
                                const OI_SDP_STRINGS *strings,
                                OI_UINT8 *channelNumber);


/**
 * Get the service record handle associated with this service.  
 * This can be used with e.g. OI_SDPDB_SetAttributeList to add
 * vendor-specific SDP attributes to the profile.
 *
 * @param handle  return the service record's handle
 */
OI_STATUS OI_FTPServer_GetServiceRecord(OI_UINT32 *handle);

/**
 * Deregisters an FTP server. 
 *
 * @param channelNumber   The RFCOMM channel number returned when the server was
 *                        registered.
 *
 * @return                OI_OK if the service was successfully deregistered.
 *
 */
OI_STATUS OI_FTPServer_Deregister(OI_UINT8 channelNumber);

/**@}*/

#endif /* _OI_FTP_SERVER_H */
