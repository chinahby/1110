#ifndef _OI_OPP_SERVER_H
#define _OI_OPP_SERVER_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_opp_server.h#1 $ 

$Log: $
*
*    #5       20 Jun  2007           GS
* Updated to re-add changes made in #3
*
*    #4       18 Oct  2006           PR
* Merged OI Code v3.0.14rev1revQ
*
*    #3       14 Jul 2006            GS
* Updated OPP object format mask so that "any" implies any other format
* and not all formats.
*

===========================================================================*/
/** 
  @file  
  This file provides the API for the server side of the Object Push Profile.
  The Object Push Profile provides functions for establishing a connection to a
  remote device that support the Object Push Profile over RFCOMM and
  functions for pushing and pulling objects. This implementation currently only
  allows one connection at a time.
 */

/** \addtogroup OPP OPP APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_status.h"
#include "oi_opp_sys.h"
#include "oi_obexspec.h"
#include "oi_dataelem.h"

/**
 * represents an active connection between the local OPP server and a remote
 * OPP client
 */
typedef OI_OPP_CONNECTION OI_OPP_SERVER_CONNECTION_HANDLE;

/*
 * OBEX object types that might be supported by an OPP server.
 * Bits not specified below are reserved.
 */
#ifdef OI_CODE
#error code not present
#else
#define OI_OPP_SERVER_OBJ_FORMAT_ANY       0xFFFF0000 /**< Any format is supported */
#endif
#define OI_OPP_SERVER_OBJ_FORMAT_VCARD_2_1 BIT0 
#define OI_OPP_SERVER_OBJ_FORMAT_VCARD_3_0 BIT1
#define OI_OPP_SERVER_OBJ_FORMAT_VCAL_1_0  BIT2
#define OI_OPP_SERVER_OBJ_FORMAT_ICAL_2_0  BIT3
#define OI_OPP_SERVER_OBJ_FORMAT_VNOTE     BIT4
#define OI_OPP_SERVER_OBJ_FORMAT_VMESSAGE  BIT5


/**
 * Supported formats are ORed together and passed in to OI_OPPServer_Register().
 */
typedef OI_UINT32 OI_OPP_SERVER_OBJECT_FORMATS;


/**
 * The application must provide a function of this type to OI_OPPServer_Register()
 * to be called when a client attempts to connect to the OPP server. The
 * application must call OI_OPP_AcceptConnect() to accept or reject the
 * connection request.
 *
 * @param clientAddr    The Bluetooth device address of the client that is
 *                      attempting to connect.
 *
 * @param connectionId  handle representing the connection between a remote
 *                      client and the local OPP server.
 *
 */
typedef void (*OI_OPP_SERVER_CONNECT_IND)(OI_BD_ADDR *clientAddr,
                                          OI_OPP_SERVER_CONNECTION_HANDLE connectionId);


/**
 * The application must provide a function of this type to OI_OPPServer_Register()
 * to be called when a client disconnects from the OPP server.
 *
 * @param connectionId  handle representing the connection between a remote
 *                      client and the local OPP server.
 *
 */
typedef void (*OI_OPP_SERVER_DISCONNECT_IND)(OI_OPP_SERVER_CONNECTION_HANDLE connectionId);


/**
 * The OPP application must call this function to accept or reject a connect
 * request from a client.
 *
 * @param connectionId  handle representing the connection between a remote
 *                      client and the local OPP server.
 *
 * @param  allowPush   If TRUE the connection is accepted and the client will
 *                     be allowed to push objects to the server. If allowPush
 *                     and allowPull are both FALSE the connection is rejected.
 *
 * @param  allowPull   If TRUE the connection is accepted and the client will be
 *                     allowed to pull objects from the server. If allowPull and
 *                     allowPush are both FALSE the connection is rejected.
 */

OI_STATUS OI_OPP_AcceptConnect(OI_OPP_SERVER_CONNECTION_HANDLE connectionId,
                               OI_BOOL allowPush,
                               OI_BOOL allowPull);

/**
 * Forcibly severs the connection from an OPP client to the OPP server. This
 * function should be called for all active connections before terminating the
 * OPP server.
 *
 * @param connectionId  handle representing the connection between a remote
 *                      client and the local OPP server.
 *
 * @return              OI_OK if the connection will be terminated.
 */

OI_STATUS OI_OPPServer_ForceDisconnect(OI_OPP_SERVER_CONNECTION_HANDLE connectionId);


/**
 * Initialize the Object Push server and register it in the service discovery
 * database.
 *
 * @param connectInd           This callback function will be called when a client is
 *                             attemping to connect to the OPP server. Can be
 *                             NULL in which case the application accepts all connections.
 *
 * @param disconnectInd        This callback function will be called when a client
 *                             disconnects from the OPP server. Can be NULL if
 *                             and only if connectInd is NULL.
 *
 * @param supportedFormats     OPP object formats supported by this server,
 *                             composed by OR'ing together the various format flags.
 *
 * @param objectOperations     the set of operations that interface to the
 *                             application's object management system.
 *
 * @param strings              name to register in service record
 *
 * @param channelNumber        RFCOMM channel number on which this server operates
 *
 * @return                     OI_OK if the service was initialized and registered succesfully with SDP
 */

OI_STATUS OI_OPPServer_Register(OI_OPP_SERVER_CONNECT_IND connectInd,
                                OI_OPP_SERVER_DISCONNECT_IND disconnectInd,
                                const OI_OPP_OBJSYS_FUNCTIONS *objectOperations,
                                OI_OPP_SERVER_OBJECT_FORMATS supportedFormats,
                                const OI_SDP_STRINGS *strings,
                                OI_UINT8 *channelNumber);


/**
 * Get the service record handle associated with this service.  
 * This can be used with e.g. OI_SDPDB_SetAttributeList to add
 * vendor-specific SDP attributes to the profile.
 *
 * @param handle  return the service record's handle
 */
OI_STATUS OI_OPPServer_GetServiceRecord(OI_UINT32 *handle);

/**
 * This function deregisters the Object Push server.
 */
OI_STATUS OI_OPPServer_Deregister(OI_UINT8 channelNumber);

/**@}*/

#endif /* _OI_OPP_SERVER_H */

