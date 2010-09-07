#ifndef _OI_L2CAP_H
#define _OI_L2CAP_H
/**
 * @file  
 * This file provides the L2CAP (Logical Link Controller and Adaptation Protocol) API.
 * 
 * See the @ref L2CAP_docpage section of the BLUEmagic 3.0 SDK documentation for more information.
 *
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_common.h"
#include "oi_connect_policy.h"

/** type for a channel identifier (CID) */
typedef OI_UINT16 OI_L2CAP_CID;

/** type for a protocol/service multiplexer (PSM) */
typedef OI_UINT16 OI_L2CAP_PSM;

/******************************************************************************

  Connection parameters

 ******************************************************************************/

/** 
 * A structure of this type is passed as an argument to OI_L2CAP_Connect() or
 * OI_L2CAP_Accept() and carries the connection parameters used when creating 
 * or accepting a connection.
 */
typedef struct {

    /** maximum transmission unit (MTU): greatest acceptable size for
        incoming data packets in bytes (D.6.1).  Because of buffer
        size limitations, it may be advantageous to use the value
        returned by OI_L2CAP_MaxSupportedMTU if the largest
        possible buffer size is required. */
    OI_UINT16 inMTU;
    
    /**
     * flush timeout: number of milliseconds to wait before an L2CAP packet that
     * can not be acknowledged at the physical layer is dropped (D.6.2) */
    OI_UINT16 outFlushTO;
    
    /** link timeout: number of milliseconds to wait before terminating an unresponsive link */
    OI_UINT16 linkTO;

} OI_L2CAP_CONNECT_PARAMS;

// MTU constants

/** Section D.6.1 of the Bluetooth version 1.1 specification requires that the 
    maximum transmission unit (MTU) for an L2CAP connection be at least 48. */
#define OI_L2CAP_MTU_MIN           48

/** the default maximum transmission unit (MTU) for an L2CAP connection */
#define OI_L2CAP_MTU_DEFAULT       672

/** the largest possible value for a maximum transmission unit (MTU)
    for an L2CAP connection as defined by the Bluetooth
    specification. */
#define OI_L2CAP_MTU_MAX           65535


// flush timeout constants

/** This flush timeout value indicates that the L2CAP layer should use the existing flush
    timeout value if it exists; otherwise, the L2CAP layer should use the default value (0xFFFF).
*/
#define OI_L2CAP_FLUSHTO_EXISTING       0x0000 

/** This flush timeout value indicates that the L2CAP layer should perform no retransmissions.
    This value represents a number of milliseconds but, since 1 millisecond is smaller than a 
    baseband timeslot pair, the interpretation is that no retransmissions will occur.
*/
#define OI_L2CAP_FLUSHTO_NO_RETRANSMIT  0x0001

/** This flush timeout value indicates that the L2CAP layer should perform retransmissions until
    the link timeout terminates the channel. This is referred to as 'reliable channel'. Note that
    this is the default value. */
#define OI_L2CAP_FLUSHTO_INFINITE       0xFFFF

/** This flush timeout value indicates that the L2CAP layer should use the default flush timeout value. */
#define OI_L2CAP_FLUSHTO_DEFAULT        OI_L2CAP_FLUSHTO_INFINITE


// link timeout constant

/** This is the default link timeout value, which is also maximum link timeout value (D.7.4). */
#define OI_L2CAP_LINKTO_DEFAULT         0xffff

/******************************************************************************

  indication callback function type definitions

 ******************************************************************************/

/**
 * A callback function of this type indicates that a connection request has been received from a remote device.
 * This request should be responded to by calling OI_L2CAP_Accept().
 *
 * @param psm             protocol/service multiplexer of protocol or service requesting connection
 * @param addr            pointer to the Bluetooth device address of the remote device
 * @param cid             channel identifier (CID) of the connection
 */
typedef void (*OI_L2CAP_CONNECT_IND)(OI_L2CAP_PSM psm,
                                     OI_BD_ADDR *addr, 
                                     OI_L2CAP_CID cid);

/**
 * A callback function of this type indicates that the quality of service agreement between the
 * local device and a remote device has been violated. This indication does not require a response.
 * [Since quality of service management is not supported in the present BLUEmagic 3.0 software implementation
 * of L2CAP, this callback function will never be called.]
 *
 * @param addr            pointer to the  Bluetooth device address of the remote device
 */
typedef void (*OI_L2CAP_QOS_VIOLATION_IND)(OI_BD_ADDR *addr);


/**
 * A callback function of this type indicates that data was received on the specified connection.
 *
 * @param cid            channel identifier for channel on which data was received
 * @param buf            pointer to buffer holding the received data
 * @param length         number of bytes received in the buffer
 */
typedef void(*OI_L2CAP_RECV_DATA_IND)(OI_L2CAP_CID cid,
                                      OI_UINT8 *buf,
                                      OI_UINT16 length);

/**
 * A callback function of this type indicates that the specified connection was disconnected.
 *
 * @param cid            local channel identifier for the connection being disconnected
 * @param reason         the reason for the disconnection
 */
typedef void (*OI_L2CAP_DISCONNECT_IND)(OI_L2CAP_CID cid,
                                        OI_STATUS reason);

/******************************************************************************

  confirmation callback function type definitions

 ******************************************************************************/

/**
 * A callback function of this type informs a higher program layer of the success or failure of an 
 * OI_L2CAP_Connect() or OI_L2CAP_Accept() command. A status of OI_OK
 * indicates that a connection corresponding to the specified CID has been created and configured
 * and is therefore in the open state. A callback function of this type will also be
 * called if a reconfiguration of the connection takes place. The fields inMTU and outMTU
 * carry the parameters for maximum transmission unit (MTU) for incoming data and outgoing data.
 *
 * @param cid         local channel identifier for the connection being created
 * @param result      success or failure status code for the connection request
 * @param inMTU       MTU for incoming packets
 * @param outMTU      MTU for outgoing packets 
 */
typedef void(*OI_L2CAP_CONNECT_COMPLETE_CALLBACK)(OI_L2CAP_CID cid,
                                                  OI_UINT16 inMTU,
                                                  OI_UINT16 outMTU,
                                                  OI_STATUS result);

/**
 * A callback function of this type informs a higher program layer of the success or failure of
 * a call to OI_L2CAP_Write().
 *
 * @param cid        local channel identifier for the connection to which the data is to be written
 * @param buf        pointer to the buffer to which data was written
 * @param sendCount  number of bytes written from the send buffer
 * @param result     success or failure status code for the write command
 */
typedef void(*OI_L2CAP_DATA_WRITE_CFM)(OI_L2CAP_CID cid,
                                       OI_UINT8 *buf,
                                       OI_UINT16 sendCount,
                                       OI_STATUS result);

/**
 * A callback function of this type informs a higher program layer of the success or failure of
 * a call to OI_L2CAP_Ping().
 *
 * @param result    success or failure status code for the ping command
 * @param addr      pointer to the Bluetooth device address of the responding device
 * @param echoData  (optional) pointer to the buffer holding data returned by the responding device
 * @param size      number of bytes of data returned by ther responding device
 */
typedef void(*OI_L2CAP_PING_CFM)(OI_STATUS result,
                                 OI_BD_ADDR *addr,
                                 OI_UINT8 *echoData,
                                 OI_UINT16 size);

/**
 * A callback function of this type informs a higher program layer of the success or failure of
 * a call to OI_L2CAP_GetInfo().
 *
 * @param result    success or failure status code for the request for implementation-specific information
 * @param infoType  type of data requested
 * @param infoData  pointer to the buffer holding response data returned by the remote device
 * @param size      number of bytes of response data
 */
typedef void(*OI_L2CAP_GET_INFO_CFM)(OI_STATUS result,
                                     OI_UINT16 infoType,
                                     OI_UINT8 *infoData,
                                     OI_UINT16 size);

/****************************************************************

  Functions

 *****************************************************************/

/**
 * This function registers a client protocol or service's PSM (protocol/service multiplexer)
 * and callback functions with the L2CAP layer. This allows the L2CAP layer to 
 * notify the protocol or service when remote hosts attempt to connect to it. If this particular
 * protocol or service will not respond to incoming connection requests, then registration is unnecessary.
 *
 * @param psm            the PSM (protocol/service multiplexer) of the client protocol or service.
 * @param connectInd     the callback used to notify the application of incoming connections
 * @param params         the Config Params (inMTU and outFlushTO) for this PSM
 * 
 * @return               status of registration request: OI_OK, if successful; failure code otherwise
 */
#ifdef OI_CODE
#error code not present
#else
OI_STATUS OI_L2CAP_Register(OI_L2CAP_PSM psm,
                            OI_L2CAP_CONNECT_IND connectInd,
                            const OI_L2CAP_CONNECT_PARAMS *params);
#endif

/** 
 * This function deregisters a protocol registered on a specified PSM.
 *
 * @param psm  the PSM to deregister
 */
OI_STATUS OI_L2CAP_Deregister(OI_L2CAP_PSM psm);

/**
 * This function obtains a dynamically allocated PSM (protocol/service multiplexer) for 
 * an application that does not have a static PSM. Once a PSM has been allocated, the 
 * application should call OI_L2CAP_Register() to register the newly allocated PSM.
 *
 * @param psm            pointer to a PSM to hold the newly allocated PSM
 */
OI_STATUS OI_L2CAP_AllocPSM(OI_L2CAP_PSM *psm);


/**
 * This function requests that a connection be created and configured. The cid parameter is a pointer 
 * to a location where the channel identifier for the connection will be written once the 
 * OI_L2CAP_CONNECT_COMPLETE_CALLBACK callback function is called with a success status code, confirming
 * that the connection has been successfully created. Configuration of the channel will be negotiated
 * with the remote device. If the remote device agrees, then the configuration parameters used will be 
 * those specified by the connectSpec parameter; a null connectSpec pointer specifies the default configuration
 * values. If the remote device does not agree with the configuration parameters specified, then 
 * the parameters requested by the remote side will be used.
 *
 * @param confirmCB       callback function to be called with the result of this connection request
 * @param disconnectInd   callback function to indicate that the connection created by this request was disconnected
 * @param recvDataInd     callback function to indicate that data was received on the connection created by this request
 * @param targetPSM       PSM (protocol/service multiplexer) of the remote protocol to connect to
 * @param addr            pointer to the Blutooth device address of the remote device to connect to
 * @param params          This points to the structure containing the configuration parameters requested by the 
 *                        local device for this connection; a null value for this pointer indicates that the 
 *                        default values are to be used.
 * @param cid             [out] pointer to where the connection's channel identifier will be written once the connection
 *                        has been created
 * @param policy          pointer to the connection policy required on this connection,
 *                        which will be managed and enforced by the Policy Manager
 * 
 * @return                status of connection request: OI_OK, if successful; failure code otherwise
 */
OI_STATUS OI_L2CAP_Connect(OI_L2CAP_CONNECT_COMPLETE_CALLBACK confirmCB,
                           OI_L2CAP_DISCONNECT_IND disconnectInd,
                           OI_L2CAP_RECV_DATA_IND recvDataInd,
                           OI_L2CAP_PSM targetPSM,
                           OI_BD_ADDR *addr,
                           OI_L2CAP_CONNECT_PARAMS *params,
                           OI_L2CAP_CID *cid,
                           const OI_CONNECT_POLICY *policy);

/**
 * This function is used to respond to a connection request, indicating that the connection should be
 * established and configured without further client interaction. This function should be called 
 * in response to an OI_L2CAP_CONNECT_IND indication callback. The OI_L2CAP_CONNECT_COMPLETE_CALLBACK callback
 * function will be called when establishment and configuration of the accepted connection are complete.
 *
 * @param confirmCB      callback function to be called with the result of the attempt to establish and configure
 *                       the connection; may be NULL if the response parameter does not indicate success
 * @param disconnectInd  callback function to indicate that the connection created by this request was disconnected
 * @param recvDataInd    callback function to indicate that data was received on the connection created by this request
 * @param cid            local channel identifier for the connection created by this request
 * @param connectSpec    This points to the structure containing the configuration parameters requested by the 
 *                       local device for this connection; a null value for this pointer indicates that the 
 *                       default values are to be used.
 * @param accept         TRUE to accept the connection, FALSE to reject
 * @param policy         pointer to the connection policy required on this connection,
 *                       which will be managed and enforced by the Policy Manager
 * 
 * @return               status of connection acceptance request: OI_OK, if successful; failure code otherwise
 */
OI_STATUS OI_L2CAP_Accept(OI_L2CAP_CONNECT_COMPLETE_CALLBACK confirmCB,
                          OI_L2CAP_DISCONNECT_IND disconnectInd,
                          OI_L2CAP_RECV_DATA_IND recvDataInd,
                          OI_UINT16 cid, 
                          OI_L2CAP_CONNECT_PARAMS *connectSpec,
                          OI_BOOL accept,
                          const OI_CONNECT_POLICY *policy);

/** 
 * This function commands the termination of a connection associated with a particular channel identifier.
 * The disconnect callback function passed to OI_L2CAP_Accept() or OI_L2CAP_Connect() will be called when the
 * disconnection is complete.
 *
 * @param  cid       the local channel identifier (CID) of the connection to terminate
 */
OI_STATUS OI_L2CAP_Disconnect(OI_L2CAP_CID cid);

/** 
 * This function calls for data to be written from the output buffer associated with a particular connection.
 * Once this function has been called to write data to a particular connection, it should not be called again
 * to write to that same connection until a callback of type OI_L2CAP_DATA_WRITE_CFM has been received for that
 * connection, confirming that data has been written from the output buffer. Calling this function twice with
 * the same CID parameter without an intervening OI_L2CAP_DATA_WRITE_CFM confirmation for that CID will result
 * in an error return status from OI_L2CAP_Write(). Concurrent writes to different channels are permitted,
 * since each channel has its own output buffer.
 *
 * @param callback       callback to be called for confirmation that the data has been written to the connection
 *                       from the output buffer
 * @param cid            local channel identifier (CID) of the connection to write to
 * @param outBuffer      pointer to the output buffer containing the data to be written
 * @param length         number of bytes to write from the buffer
 */
OI_STATUS OI_L2CAP_Write(OI_L2CAP_DATA_WRITE_CFM callback,
                         OI_L2CAP_CID cid, 
                         OI_UINT8 *outBuffer,
                         OI_UINT16 length);

/**
 * This function pings a remote device. The current implementation does not
 * support the sending of data with a ping; this functionality will be implemented
 * in a future version of BLUEmagic software.
 *
 * @param callback        callback function to be called for confirmation that the ping has succeeded and
 *                        optionally carrying additional response data
 * @param addr            pointer to the Bluetooth device address of the device to be pinged
 * @param echoData        pointer to the buffer containing the optional data to send
 * @param length          length of the optional data in bytes
 */
OI_STATUS OI_L2CAP_Ping(OI_L2CAP_PING_CFM callback, 
                        OI_BD_ADDR *addr, 
                        OI_UINT8 *echoData, 
                        OI_UINT16 length);

/**
 * This function requests implementation-specific information from a remote device. The only type of 
 * information specified in section D.5.10 of the Bluetooth specification is 'connectionless MTU', for
 * which infoType is 0x0001.
 * 
 * @param callback        callback function to be called for confirmation that the request has succeeded and
 *                        to carry the response
 * @param addr            pointer to the Bluetooth device address of the target device
 * @param infoType        the type of information to get
 */
OI_STATUS OI_L2CAP_GetInfo(OI_L2CAP_GET_INFO_CFM callback, 
                           OI_BD_ADDR *addr, 
                           OI_UINT16 infoType);

/**
 * This function returns the maximum value of the MTU currently be supported by L2CAP.
 */
OI_UINT16 OI_L2CAP_MaxSupportedMTU(void);

/*****************************************************************************/
#endif /* _OI_L2CAP_H */

