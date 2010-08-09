#ifdef FEATURE_BT_EXTPF_AV
#ifndef _OI_AVCTP_H
#define _OI_AVCTP_H

/**
 * @file  
 *
 * This file provides the interface for the Audio/Video Control Transport Protocol (AVCTP).
 * BLUEmagic 3.0 software implements the draft 1.0 version of the Bluetooth Audio/Video Control 
 * Transport Protocol (AVCTP).
 */

/** \addtogroup AVRC */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_l2cap.h"

/*************************************************************

  constants

*************************************************************/

/* the type of an AVCTP message */
#define OI_AVCTP_TYPE_CMD 0x01
#define OI_AVCTP_TYPE_RSP 0x02

/*************************************************************

  callbacks

*************************************************************/

/**
 * A callback function of this type indicates an incoming connection. OI_AVCTP_Accept() should
 * be called in response.
 *
 * @param addr  the Bluetooth device address of the remote device
 */
typedef void (*OI_AVCTP_CONNECT_IND)(OI_BD_ADDR *addr);

/**
 * A callback function of this type indicates that a disconnect initiated by the local or remote
 * side has completed.
 *
 * @param  addr     the Bluetooth device address of the remote device
 * @param  result   the result
 */
typedef void (*OI_AVCTP_DISCONNECT_IND)(OI_BD_ADDR *addr,
                                        OI_STATUS   result);

/**
 * A callback function of this type indicates the reception of data.
 *
 * @param addr         the Bluetooth device address of the remote device
 * @param pid          the identifier for the profile sending the message
 * @param transaction  the transaction label for the message
 * @param type         the type of the message (OI_AVCTP_TYPE_CMD or OI_AVCTP_TYPE_RSP).
 * @param invalidPID   TRUE indicates that the remote host received data with an invalid target PID. 
 *                     In this case, data will be NULL and dataLen will be 0.
 * @param data         a pointer to message data
 * @param dataLen      the number of bytes of data to be sent
 */
typedef void (*OI_AVCTP_RECV_IND)(OI_BD_ADDR *addr,
                                  OI_UINT8    transaction,
                                  OI_UINT8    type,
                                  OI_BOOL     invalidPID,
                                  OI_UINT8   *data,
                                  OI_UINT16   dataLen);

/**
 * A callback function of this type is used to confirm a send.
 *
 * @param addr         the Bluetooth device address of the remote device
 * @param pid          the identifier for the profile sending the message
 * @param transaction  the transaction label for the message
 * @param type         the type of the message (OI_AVCTP_TYPE_CMD or OI_AVCTP_TYPE_RSP).
 * @param data         a pointer to message data
 * @param dataLen      the number of bytes of data
 * @param result       the result
 */
typedef void (*OI_AVCTP_SEND_CFM)(OI_BD_ADDR *addr,
                                  OI_UINT16   pid,
                                  OI_UINT8    transaction,
                                  OI_UINT8    type,
                                  OI_UINT8   *data,
                                  OI_UINT16   dataLen,
                                  OI_STATUS   result);

/**
 * A callback function of this type is used to confirm the establishment of an AVCTP connection.
 *
 * @param  addr    the Bluetooth device address of the remote device
 * @param  inMTU   the incoming MTU for the underlying L2CAP connection
 * @param  outMTU  the outgoing MTU for the underlying L2CAP connection
 * @param  result  the result
 */
typedef void (*OI_AVCTP_CONNECT_CFM)(OI_BD_ADDR *addr,
                                     OI_UINT16   inMTU,
                                     OI_UINT16   outMTU,
                                     OI_STATUS   result);

/*************************************************************

  functions

*************************************************************/

/**
 * This function registers to use the AVCT Protocol layer.
 *
 * @param pid            the identifier for the profile type
 * @param connectInd     connect indication callback function
 * @param disconnectInd  disconnect indication callback function
 * @param recvInd        indication callback function called to indicate that data has been received
 * @param connectCfm     connect confirmation callback function
 * @param sendCfm        send confirmation callback function
 */
OI_STATUS OI_AVCTP_Register(OI_UINT16               pid,
                            OI_AVCTP_CONNECT_IND    connectInd,
                            OI_AVCTP_DISCONNECT_IND disconnectInd,
                            OI_AVCTP_RECV_IND       recvInd,
                            OI_AVCTP_CONNECT_CFM    connectCfm,
                            OI_AVCTP_SEND_CFM       sendCfm);

/**
 * This function Deregisters to pid from the AVCT Protocol layer
 *
 * @param pid            the identifier for the profile type
 */
OI_STATUS OI_AVCTP_Deregister(OI_UINT16 pid);

/**
 * This function creates a connection to the specified remote device.
 *
 * @param addr     the Bluetooth device address of the remote device
 * @param pid      the identifier for the profile type
 * @param params   the parameters to be used for the underlying L2CAP connection
 *                 (Passing a NULL value results in the default L2CAP parameters being used.)
 */
OI_STATUS OI_AVCTP_Connect(OI_BD_ADDR              *addr,
                           OI_UINT16                pid,
                           OI_L2CAP_CONNECT_PARAMS *params);

/**
 * This function accepts or rejects an incoming connection. This function should be
 * called in response to an OI_AVCTP_CONNECT_IND callback.
 *
 * @param addr   the Bluetooth device address of the remote device
 * @param accept TRUE to accept, FALSE to reject
 */
OI_STATUS OI_AVCTP_Accept(OI_BD_ADDR *addr,
                          OI_BOOL     accept);

/**
 * This function disconnects from the specified device.
 *
 * @param addr  the Bluetooth device address of the remote device
 * @param pid   the identifier for the profile type
 */
OI_STATUS OI_AVCTP_Disconnect(OI_BD_ADDR *addr,
                              OI_UINT16   pid);

/**
 * This function sends a message.
 * 
 * @param addr         the Bluetooth device address of the remote device
 * @param pid          the identifier for the profile sending the message
 * @param transaction  the transaction label for the message
 * @param type         the type of the message (OI_AVCTP_TYPE_CMD or OI_AVCTP_TYPE_RSP).
 * @param data         a pointer to message data
 * @param dataLen      the number of bytes of data
 */
OI_STATUS OI_AVCTP_Send(OI_BD_ADDR *addr,
                        OI_UINT16   pid,
                        OI_UINT8    transaction,
                        OI_UINT8    type,
                        OI_UINT8   *data,
                        OI_UINT16   dataLen);

/**@}*/

#endif /* _OI_AVCTP_H */
#endif /* FEATURE_BT_EXTPF_AV */
