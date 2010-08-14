#ifdef FEATURE_BT_EXTPF_AV
#ifndef _OI_AVDTP_H
#define _OI_AVDTP_H
/**
 * @file
 * This file provides the interface for the Audio/Video Distribution Transport Protocol (AVDTP).
 * 
 * BLUEmagic 3.0 software complies with the 1.0 version of the Bluetooth Audio/Video Distribution
 * Transport Protocol specification. 
 *
 * To initialize the AVDTP layer, call OI_AVDTP_Register() to register the callbacks
 * to be used. To register a stream endpoint, call OI_AVDTP_RegisterEndpoint().
 * OI_AVDTP_DeregisterEndpoint() can be used to remove a stream endpoint.
 *
 * A stream handle is used after an endpoint has been configured, whereas an endpoint identifier 
 * is used before configuration.
 *
 * Transaction numbers are used to match responses (sent via callback functions) with their
 * respective requests.
 * 
 * Before sending AVDTP commands, an L2CAP signalling connection must be established by calling 
 * OI_AVDTP_Connect(). When the OI_AVDTP_CONNECT_CFM callback is called with a success 
 * status, the connection is ready to use. To establish an AVDTP media transport channel, call
 * OI_AVDTP_SetConfig(), OI_AVDTP_Open(), and then OI_AVDTP_Start() to begin streaming. 
 * OI_AVDTP_Suspend() can be used to suspend the stream and OI_AVDTP_Reconfigure() can 
 * be used to reconfigure a suspended stream. OI_AVDTP_Discover() and OI_AVDTP_GetCap() may
 * be called at anytime but will usually be called before configuring the connection.
 */

/** \addtogroup AVS */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include <stdarg.h>
#include "oi_l2cap.h"
#include "oi_stddefs.h"
#include "oi_status.h"
#include "oi_bt_stack_config.h"
#include "oi_bt_profile_config.h"


/*****************************************************
 *
 * AVDTP configuration parameters
 *
 *****************************************************/

/* service capability category values */
#define OI_AVDTP_CAP_MEDIA_TRANSPORT    1
#define OI_AVDTP_CAP_REPORTING          2
#define OI_AVDTP_CAP_RECOVERY           3
#define OI_AVDTP_CAP_CONTENT_PROTECTION 4
#define OI_AVDTP_CAP_ROHC               5
#define OI_AVDTP_CAP_MUX                6
#define OI_AVDTP_CAP_CODEC              7

#define OI_AVDTP_CAP_TOTAL              OI_AVDTP_CAP_CODEC

/* endpoint type */
#define OI_AVDTP_SRC  0
#define OI_AVDTP_SNK  1

/* media types */
#define OI_AVDTP_MEDIA_TYPE_AUDIO           0x00
#define OI_AVDTP_MEDIA_TYPE_VIDEO           0x01
#define OI_AVDTP_MEDIA_TYPE_MULTIMEDIA      0x02
#define OI_AVDTP_MEDIA_TYPE_MULTICAST_AUDIO 0x0F

/* codec type */
#define OI_AVDTP_CODEC_SBC              0
#define OI_AVDTP_CODEC_MPEG_AUDIO       BIT0
#define OI_AVDTP_CODEC_MPEG_AAC         BIT1
#define OI_AVDTP_CODEC_ATRAC            BIT2
#define OI_AVDTP_CODEC_NON_A2DP         OI_UINT8_MAX

/* error codes */
#define OI_AVDTP_SUCCESS 0x00 // not defined by specification; used only internally
#define OI_AVDTP_ERR_HEADER_FORMAT          0x01
#define OI_AVDTP_ERR_LENGTH                 0x11
#define OI_AVDTP_ERR_ACP_SEID               0x12
#define OI_AVDTP_ERR_SEP_IN_USE             0x13
#define OI_AVDTP_ERR_SEP_NOT_IN_USE         0x14
#define OI_AVDTP_ERR_SERV_CATEGORY          0x17
#define OI_AVDTP_ERR_PAYLOAD_FORMAT         0x18 
#define OI_AVDTP_ERR_UNSUPPORTED_COMMAND    0x19
#define OI_AVDTP_ERR_INVALID_CAPABILITIES   0x1A
#define OI_AVDTP_ERR_RECOVERY_TYPE          0x22
#define OI_AVDTP_ERR_MEDIA_TRANSPORT_FORMAT 0x23
#define OI_AVDTP_ERR_RECOVERY_FORMAT        0x25
#define OI_AVDTP_ERR_ROHC_FORMAT            0x26
#define OI_AVDTP_ERR_CP_FORMAT              0x27
#define OI_AVDTP_ERR_MUX_FORMAT             0x28
#define OI_AVDTP_ERR_UNSUPPORTED_CONFIG     0x29
#define OI_AVDTP_ERR_STATE                  0x31
/* GAVDP error codes */
#define OI_AVDTP_ERR_BAD_SERVICE            0x80
#define OI_AVDTP_ERR_INSUFFICIENT_RESOURCE  0x81
/* Internaly generated errors */
#define OI_AVDTP_ERR_NO_RESPONSE            0xFF

/* Reliability codes for OI_AVDTP_RECV_DATA_IND */
#define OI_AVDTP_RECV_NO_ERROR 0x0000
#define OI_AVDTP_RECV_FRAME_LOST 0x0001 
#define OI_AVDTP_RECV_PACKET_RECOVERED 0x0002 
#define OI_AVDTP_RECV_LENGTH_MISMATCH 0x0003

/******************************************************************

  types

*****************************************************************/

/** the type for a structure describing the capabilities of a stream endpoint */
typedef struct {
    
    /** the capability type */
    OI_UINT8 type;

    union {
        
        /** the data for a codec capability (type == OI_AVDTP_CAP_CODEC) */
        struct {
            
            /** the media type */
            OI_UINT8 mediaType;

            /** the codec type */
            OI_UINT8 codecType;

            /** codec-specific data */
            const OI_UINT8 *data;

            /** the length of the codec-specific data */
            OI_UINT16 dataLen;
        } codecData;

        struct {

            /** Information Element specific data */
            const OI_UINT8 *data;

            /** the length of the Information Element specific data */
            OI_UINT16 dataLen;
        }genericData;

    } data;

} OI_AVDTP_CAP;


/** the type for a data structure used in a discovery response */
typedef struct {

    /** the type of media supported by this endpoint */
    OI_UINT8 mediaType;

    /** source or sink */
    OI_UINT8 endpointType;

    /** the stream endpoint identifier */
    OI_UINT8 seid;

    OI_UINT8 inUse;
    
} OI_AVDTP_DISCOVER_RSP_INFO;


/******************************************************************

  callbacks

*****************************************************************/

/**
 * A callback function of this type indicates that a remote device is attempting a connection.
 *
 * @param addr  the Bluetooth device address of the remote device
 */
typedef void (*OI_AVDTP_CONNECT_IND)(OI_BD_ADDR *addr);

/**
 * A callback function of this type confirms the establishment of the underlying 
 * L2CAP connection.
 *
 * @param addr    the Bluetooth device address of the remote device
 * @param result  result of the attempt to establish a connection
 */
typedef void (*OI_AVDTP_CONNECT_CFM)(OI_BD_ADDR *addr,
                                     OI_STATUS result);

/**
 * A callback function of this type indicates that the underlying L2CAP connection was
 * disconnected.
 *
 * @param addr    the Bluetooth device address of the remote device
 * @param reason  the reason for the disconnection
 */
typedef void (*OI_AVDTP_DISCONNECT_CB)(OI_BD_ADDR *addr,
                                       OI_STATUS reason);

/**
 * A callback function of this type returns the result of a DISCOVER command.
 *
 * @param transaction  the transaction number
 * @param addr         the Bluetooth device address of the remote device
 * @param records      pointer to the records returned for the remote stream endpoints
 * @param numRecords   the number of records returned
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
typedef void (*OI_AVDTP_DISCOVER_CFM)(OI_UINT8 transaction,
                                      OI_BD_ADDR *addr,
                                      OI_AVDTP_DISCOVER_RSP_INFO *records,
                                      OI_UINT8 numRecords,
                                      OI_UINT8 error);

/**
 * A callback function of this type returns the result of a GET CAPABILITES command.
 *
 * @param transaction  the transaction number
 * @param addr         the Bluetooth device address of the remote device
 * @param remoteSEID   the remote stream endpoint ID
 * @param caps         the capabilities for the remote endpoint, NULL if error
 * @param numCaps      the number of capabilities returned, 0 if error
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
typedef void (*OI_AVDTP_GET_CAPABILITES_CFM)(OI_UINT8 transaction,
                                             OI_BD_ADDR *addr,
                                             OI_UINT8 remoteSEID,
                                             OI_AVDTP_CAP *caps,
                                             OI_UINT8 numCaps,
                                             OI_UINT8 error);

/**
 * A callback function of this type indicates that a remote device
 * wishes to configure the specified local endpoint to communicate
 * with the specified remote endpoint. OI_AVDTP_SetConfigRsp() must
 * be called after receiving this callback. The memory pointed to by
 * *caps is dynamic, and owned by the AVDTP profile. It is only in scope
 * within this function, and must be copied to the applications memory
 * space if the application requires persistent access to it.
 *
 * @param transaction  the transaction number
 * @param addr         the Bluetooth device address of the remote device
 * @param localSEID    the local stream endpoint ID
 * @param remoteSEID   the remote stream endpoint ID
 * @param caps         the array of the capabilities to be configured
 * @param handle       the handle to use for referring to this stream
 * @param numCaps      the number of capabilities
 */
typedef void (*OI_AVDTP_SET_CONFIG_IND)(OI_UINT8 transaction,
                                        OI_BD_ADDR *addr,
                                        OI_UINT8 localSEID,
                                        OI_UINT8 remoteSEID,
                                        OI_UINT16 handle,
                                        OI_AVDTP_CAP *caps,
                                        OI_UINT8 numCaps);

/**
 * A callback function of this type confirms the receipt of a SET CONFIG response.
 *
 * @param transaction  the transaction number
 * @param handle       the stream handle
 * @param category     the service category that was rejected
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
typedef void (*OI_AVDTP_SET_CONFIG_CFM)(OI_UINT8 transaction,
                                        OI_UINT16 handle,
                                        OI_UINT8 category,
                                        OI_UINT8 error);

/**
 * A callback function of this type confirms the receipt of a GET CONFIG response.
 *
 * @param transaction  the transaction number
 * @param handle       the stream handle
 * @param caps         the capabilities
 * @param numCaps      the number of capabilites returned
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
typedef void (*OI_AVDTP_GET_CONFIG_CFM)(OI_UINT8 transaction,
                                        OI_UINT16 handle,
                                        OI_AVDTP_CAP *caps,
                                        OI_UINT8 numCaps,
                                        OI_UINT8 error);

/**
 * A callback function of this type confirms an OPEN command and the
 * establishment of the media transport channel.
 *
 * @param transaction  the transaction number
 * @param handle       the stream handle
 * @param outMTU       the outgoing MTU if the media transport was established, 0 otherwise
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
typedef void (*OI_AVDTP_OPEN_CFM)(OI_UINT8 transaction,
                                  OI_UINT16 handle,
                                  OI_UINT16 outMTU,
                                  OI_UINT8 error);


/**
 * A callback function of this type indicates that the specified stream has been moved to
 * the OPEN state and the media transport channel was established.
 *
 * @param handle  the stream handle
 * @param outMTU  the outgoing MTU for the media transport of this endpoint, 
 *                the maximum size of data that can be passed to OI_AVDTP_SendData()
 */
typedef void (*OI_AVDTP_OPEN_IND)(OI_UINT16 handle,
                                  OI_UINT16 outMTU);

/**
 * A callback function of this type indicates the receipt of a CLOSE
 * command for the specified stream.
 *
 * @param transaction  the transaction number
 * @param handle       the stream handle
 */
typedef void (*OI_AVDTP_CLOSE_IND)(OI_UINT8 transaction,
                                   OI_UINT16 handle);

/**
 * A callback function of this type indicates the receipt of a CLOSE
 * response for the specified stream.
 *
 * @param transaction  the transaction number
 * @param handle       the stream handle
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
typedef void (*OI_AVDTP_CLOSE_CFM)(OI_UINT8 transaction,
                                   OI_UINT16 handle,
                                   OI_UINT8 error);

/**
 * A callback function of this type indicates the receipt of a START command.
 *
 * @param transaction  the transaction number
 * @param handles      a list of stream handles on which to start streaming
 * @param num          the number of stream handles in the list
 */
typedef void (*OI_AVDTP_START_IND)(OI_UINT8 transaction,
                                   OI_UINT16 *handles,
                                   OI_UINT8 num);

/**
 * A callback function of this type indicates the receipt of a START response.
 *
 * @param transaction   the transaction number
 * @param handles       the list of stream handles to which the response applies
 * @param numHandles    the number of stream handles in the list
 * @param errorHandle   the first stream handle that caused an error, or 0 if successful
 * @param error         the error status, or OI_AVDTP_SUCCESS if successful
 */
typedef void (*OI_AVDTP_START_CFM)(OI_UINT8 transaction,
                                   OI_UINT16 *handles,
                                   OI_UINT8 numHandles,
                                   OI_UINT8 errorHandle,
                                   OI_UINT8 error);

/**
 * A callback function of this type indicates the receipt of a SUSPEND command.
 *
 * @param transaction  the transaction number
 * @param handles      a list of stream handles
 * @param numHandles   the number of stream handles in the list
 */
typedef void (*OI_AVDTP_SUSPEND_IND)(OI_UINT8 transaction,
                                     OI_UINT16 *handles,
                                     OI_UINT8 numHandles);

/**
 * A callback function of this type indicates the receipt of a SUSPEND response.
 *
 * @param transaction  the transaction number
 * @param handles      the list of stream handles to which the response applies
 * @param numHandles   the number of stream handles in the list
 * @param errorHandle  the first stream handle that caused an error, or 0 if successful
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
typedef void (*OI_AVDTP_SUSPEND_CFM)(OI_UINT8 transaction,
                                     OI_UINT16 *handles,
                                     OI_UINT8 numHandles,
                                     OI_UINT8 errorHandle,
                                     OI_UINT8 error);

/**
 * A callback function of this type indicates that a remote device wishes to
 * reconfigure the specified stream. AVDTP_ReconfigureRsp() must be called
 * after this callback is received. The memory pointed to by *caps is dynamic,
 * and owned by the AVDTP profile. It is only in scope within this function,
 * and must be copied to the applications memory space if the application
 * requires persistent access to it.
 *
 * @param transaction  the transaction number
 * @param handle       the stream handle
 * @param caps         the array of the capabilities to be configured
 * @param numCaps      the number of capabilities
 */
typedef void (*OI_AVDTP_RECONFIGURE_IND)(OI_UINT8 transaction,
                                         OI_UINT16 handle,
                                         OI_AVDTP_CAP *caps,
                                         OI_UINT8 numCaps);

/**
 * A callback function of this type indicates the receipt of a
 * RECONFIGURE response on the specified stream.
 *
 * @param transaction  the transaction number
 * @param handle       the stream handle
 * @param category     the service category that was rejected
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
typedef void (*OI_AVDTP_RECONFIGURE_CFM)(OI_UINT8 transaction,
                                         OI_UINT16 handle,
                                         OI_UINT8 category,
                                         OI_UINT8 error);


/**
 * A callback function of this type indicates the receipt of an ABORT
 * command for the specified stream.
 *
 * @param transaction  the transaction number
 * @param handle       the stream handle
 */
typedef void (*OI_AVDTP_ABORT_IND)(OI_UINT8 transaction,
                                   OI_UINT16 handle);

/**
 * A callback function of this type indicates the receipt of an ABORT command response
 * and indicates the result of the ABORT command.
 *
 * @param transaction  the transaction number
 * @param handle       the stream handle
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
typedef void (*OI_AVDTP_ABORT_CFM)(OI_UINT8 transaction,
                                   OI_UINT16 handle,
                                   OI_UINT8 error);

/**
 * A callback function of this type indicates the receipt of a security control command.
 *
 * @param transaction  the transaction number
 * @param handle       the stream handle
 * @param data         the content protection scheme dependent data
 * @param dataLen      the length of the data in bytes
 */
typedef void (*OI_AVDTP_SECURITY_IND)(OI_UINT8 transaction,
                                      OI_UINT16 handle,
                                      OI_BYTE *data,
                                      OI_UINT16 dataLen);

/**
 * A callback function of this type indicates the receipt of a security control response.
 *
 * @param transaction  the transaction number
 * @param handle       the stream handle
 * @param data         the content protection scheme dependent data, NULL if error
 * @param dataLen      the length of the data in bytes, 0 if error
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
typedef void (*OI_AVDTP_SECURITY_CFM)(OI_UINT8 transaction,
                                      OI_UINT16 handle,
                                      OI_BYTE *data,
                                      OI_UINT16 dataLen,
                                      OI_UINT8 error);

/**
 * A callback function of this type indicates that media data has been received.
 *
 * @param handle    the stream handle on which the data was received
 * @param seqNum    the sequence number of the frame
 * @param timestamp the timestamp for the frame
 * @param data      the received data
 * @param dataLen   the number of bytes received
 * @param marker    the marker bit
 * @param payloadType  the payload type
 * @param reliability  reliability indicator
 */
typedef void (*OI_AVDTP_RECV_DATA_IND)(OI_UINT16 handle,
                                       OI_UINT16 seqNum,
                                       OI_UINT32 timestamp,
                                       OI_BYTE *data,
                                       OI_UINT16 dataLen,
                                       OI_UINT8 payloadType,
                                       OI_UINT16 marker,
                                       OI_UINT16 reliability);

/**
 * A callback function of this type indicates that media data has been received.
 *
 * @param handle    the stream handle on which the data was received
 * @param seqNum    the sequence number of the frame
 * @param timestamp the timestamp for the frame
 * @param data      the received data
 * @param dataLen   the number of bytes received
 * @param marker    the marker bit
 * @param payloadType  the payload type
 * @param reliability  reliability indicator
 */
typedef void (*OI_AVDTP_RECV_BROADCAST_IND)(OI_UINT16 handle,
                                            OI_UINT8  seqNum,
                                            OI_UINT8  offset,
                                            OI_UINT8  fractional,
                                            OI_BOOL   fec,
                                            OI_BYTE * data,
                                            OI_UINT16 dataLen);

/**
 * A callback function of this type confirms a media data send.
 *
 * @param handle  the stream handle on which the data was sent
 * @param buf     the data buffer
 * @param bufLen  the length of the data buffer in bytes
 * @param result  the result
 */
typedef void (*OI_AVDTP_SEND_CFM)(OI_UINT16 handle,
                                  OI_BYTE *buf,
                                  OI_UINT16 bufLen,
                                  OI_STATUS result);

/** A structure of this type is used to pass a list of callbacks for the AVDTP layer. */
typedef struct OI_AVDTP_CALLBACKS {
    OI_AVDTP_CONNECT_IND connectInd; /* Mandatory */
    OI_AVDTP_CONNECT_CFM connectCfm; /* Mandatory */
    OI_AVDTP_DISCONNECT_CB disconnectCB; /* Mandatory */

    OI_AVDTP_DISCOVER_CFM discoverCfm; /* Mandatory */

    OI_AVDTP_GET_CAPABILITES_CFM getCapsCfm; /* Mandatory */

    OI_AVDTP_SET_CONFIG_IND setConfigInd; /* Mandatory */
    OI_AVDTP_SET_CONFIG_CFM setConfigCfm; /* Mandatory */

    OI_AVDTP_GET_CONFIG_CFM getConfigCfm; /* Mandatory */
    
    OI_AVDTP_OPEN_CFM openCfm; /* Mandatory */
    OI_AVDTP_OPEN_IND openInd; /* Mandatory */

    OI_AVDTP_CLOSE_IND closeInd; /* Mandatory */
    OI_AVDTP_CLOSE_CFM closeCfm; /* Mandatory */

    OI_AVDTP_START_IND startInd; /* Mandatory */
    OI_AVDTP_START_CFM startCfm; /* Mandatory */

    OI_AVDTP_SUSPEND_IND suspendInd; /* Optional, but recomended */
    OI_AVDTP_SUSPEND_CFM suspendCfm; /* Optional, but recomended */

    OI_AVDTP_RECONFIGURE_IND reconfigureInd; /* Optional */
    OI_AVDTP_RECONFIGURE_CFM reconfigureCfm; /* Optional */

    OI_AVDTP_SECURITY_IND securityInd; /* Optional */
    OI_AVDTP_SECURITY_CFM securityCfm; /* Optional */
    
    OI_AVDTP_ABORT_IND abortInd; /* Mandatory */
    OI_AVDTP_ABORT_CFM abortCfm; /* Mandatory */

    OI_AVDTP_RECV_DATA_IND recvDataInd; /* Mandatory */
    OI_AVDTP_SEND_CFM sendCfm; /* Mandatory */

    OI_AVDTP_RECV_BROADCAST_IND recvBroadcastDataInd; /* Optional */

} OI_AVDTP_CALLBACKS;


/** A data structure of this type is used to register a stream endpoint. */
typedef struct {

    /** the type of media supported by this endpoint */
    OI_UINT8 mediaType;

    /** source or sink */
    OI_UINT8 endpointType;
     
    /** the array of capabilities for this endpoint */
    OI_AVDTP_CAP *caps;

    /** the number of capabilities */
    OI_UINT16 numCaps;

} OI_AVDTP_ENDPOINT_REG;


/******************************************************************

  functions

*****************************************************************/

/**
 * This function registers the application callback functions and connection parameters.
 *
 * It cannot be known whether the remote device is well-behaved and
 * will not send data we did not request, so all callbacks must be
 * defined as functions.  It is never safe to define one of the
 * callbacks as NULL, and it is never safe to pass NULL as the entire
 * callback structure.
 *
 * @param c       the callbacks
 * @param params  pointer to the parameters to use when establishing a media transport channel 
 *                (NULL to use defaults)
 */
OI_STATUS OI_AVDTP_Register(const OI_AVDTP_CALLBACKS *c,
                            const OI_L2CAP_CONNECT_PARAMS *params);

/**
 * This function deregisters the the AVDTP profile. All endpoints must be
 * deregistered prior to profile deregistration.
 */
OI_STATUS OI_AVDTP_Deregister( void );

/**
 * This function registers a stream endpoint with the AVDTP layer.
 *
 * @param mediaType     the type of media offered by the endpoint 
 *                      (AVDTP_MEDIA_TYPE_AUDIO, AVDTP_MEDIA_TYPE_VIDEO, AVDTP_MEDIA_TYPE_MULTIMEDIA)
 * @param endpointType  the type of endpoint (OI_AVDTP_SRC or OI_AVDTP_SNK)
 * @param caps          the list of the capabilities offered by the endpoint
 * @param numCaps       the number of capabilities in the list
 * @param seid          [OUT] pointer to the buffer into which the stream endpoint identifier for the 
 *                      registered endpoint will be written
 */
OI_STATUS OI_AVDTP_RegisterEndpoint(OI_UINT8 mediaType,
                                    OI_UINT8 endpointType,
                                    const OI_AVDTP_CAP *caps,
                                    OI_UINT8 numCaps,
                                    OI_UINT8 *seid);

#ifdef OI_DEPRECATED
/**
 * This function registers a stream endpoint with the AVDTP layer.
 *
 * @deprecated - Just register OI_AVDTP_MEDIA_TYPE_MULTICAST_AUDIO 
 * as the endpoint mediaType
 *
 * @param seid          Stream endpoint to configure for Broadcast
 * @param RecvDataInd   Function pointer to receive Broadcast Data
 */
OI_STATUS OI_AVDTP_SetBroadcast(OI_UINT8                    seid,
                                OI_AVDTP_RECV_BROADCAST_IND RecvDataInd);
#endif

/**
 * This function deregisters the specified stream endpoint.
 *
 * @param  seid  the stream endpoint identifier to deregister
 */
OI_STATUS OI_AVDTP_DeregisterEndpoint(OI_UINT8 seid);

/**
 * This function creates the L2CAP connection to use for signalling. This must be done 
 * (and the OI_AVDTP_CONNECT_CFM callback must be called) before any commands can be sent.
 *
 * @param addr   the Bluetooth device address of the remote device
 */
OI_STATUS OI_AVDTP_Connect(OI_BD_ADDR *addr);

/**
 * This function accepts a connection indicated by a OI_AVDTP_CONNECT_IND callback.
 *
 * @param addr  the address of the remote device
 * @param accept  TRUE to accept, FALSE to reject
 */
OI_STATUS OI_AVDTP_Accept(OI_BD_ADDR *addr,
                          OI_BOOL accept);

/**
 * Terminate an existing connection to a remote device.  When the
 * disconnection is complete, the @ref OI_AVDTP_DISCONNECT_CB will be
 * called.
 *
 * @param addr   the Bluetooth device address of the remote device
 */
OI_STATUS OI_AVDTP_Disconnect(OI_BD_ADDR *addr);

/**
 * This function sends a DISCOVER command to discover the endpoints exposed by the specified device.
 * The result is returned by a callback function of type @ref OI_AVDTP_DISCOVER_CFM.
 *
 * @param addr   the Bluetooth device address of the remote device
 * @param transaction  [OUT] parameter indicating the transaction number (May be NULL.)
 */
OI_STATUS OI_AVDTP_Discover(OI_BD_ADDR *addr,
                            OI_UINT8 *transaction);

/**
 * This function gets the capabilities of the specified remote endpoint.
 * The result is returned by a callback function of type @ref OI_AVDTP_GET_CAPABILITES_CFM.
 *
 * @param addr         the Bluetooth device address of the remote device
 * @param remoteSEID   the remote stream endpoint ID
 * @param transaction  [OUT] parameter indicating the transaction number (May be NULL.)
 */
OI_STATUS OI_AVDTP_GetCap(OI_BD_ADDR *addr,
                          OI_UINT8 remoteSEID,
                          OI_UINT8 *transaction);

/**
 * This function configures the specified remote endpoint.
 *
 * @param addr       the Bluetooth device address of the remote device
 * @param remoteSEID the remote stream endpoint ID
 * @param localSEID  the local stream endpoint ID
 * @param caps       the list of the capabilities to configure
 * @param numCaps    the number of capabilites in the list
 * @param transaction  [OUT] parameter indicating the transaction number (May be NULL.)
 * @param handle     [OUT] parameter indicating the stream handle associated with the configured stream.
 */
OI_STATUS OI_AVDTP_SetConfig(OI_BD_ADDR *addr,
                             OI_UINT8 remoteSEID,
                             OI_UINT8 localSEID,
                             const OI_AVDTP_CAP *caps,
                             OI_UINT8 numCaps,
                             OI_UINT8 *transaction,
                             OI_UINT16 *handle);

/**
 * This function sends a GET CONFIG command. 
 *
 * @param handle       the stream handle
 * @param transaction  the transaction number
 */
OI_STATUS OI_AVDTP_GetConfig(OI_UINT16 handle,
                             OI_UINT8 *transaction);

/**
 * This function sends a SET CONFIG response. The function should be
 * called in response to a @ref OI_AVDTP_SET_CONFIG_IND callback.
 *
 * @param handle       the stream handle
 * @param transaction  the transaction number
 * @param category     the first configuration category that failed, or 0 if successful
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
OI_STATUS OI_AVDTP_SetConfigRsp(OI_UINT16 handle,
                                OI_UINT8 transaction,
                                OI_UINT8 category,
                                OI_UINT8 error);

/**
 * This function sends the OPEN command. Upon successfully exchanging
 * OPEN messages, the media transport channel will be
 * established. This command can only be sent after an endpoint has
 * been configured.
 *
 * @param handle  the stream handle to open
 * @param transaction  [OUT] parameter indicating the transaction number (May be NULL.)
 */
OI_STATUS OI_AVDTP_Open(OI_UINT16 handle,
                        OI_UINT8 *transaction);

/**
 * This function sends the START command. This command can only be
 * sent after an endpoint has been configured.
 *
 * @param handles  the list of stream handles to start
 * @param num      the number of stream handles in the list
 * @param transaction  [OUT] parameter indicating the transaction number (May be NULL.)
 */
OI_STATUS OI_AVDTP_Start(OI_UINT16 *handles,
                         OI_UINT8 num,
                         OI_UINT8 *transaction);

/** 
 * This function closes a stream.
 *
 * @param handle  the stream handle to close
 * @param transaction  [OUT] parameter indicating the transaction number (May be NULL.)
 */
OI_STATUS OI_AVDTP_Close(OI_UINT16 handle,
                         OI_UINT8 *transaction);

/** 
 * This function aborts a stream.
 *
 * @param handle  the stream handle to abort
 * @param transaction  [OUT] parameter indicating the transaction number (May be NULL.)
 */
OI_STATUS OI_AVDTP_Abort(OI_UINT16 handle,
                         OI_UINT8 *transaction);

/**
 * This function sends the SUSPEND command. This command can only be
 * sent after an endpoint has been configured.
 *
 * @param handles  the list of the stream handles to suspend
 * @param num      the number of handles in the list
 * @param transaction  [OUT] parameter indicating the transaction number (May be NULL.)
 */
OI_STATUS OI_AVDTP_Suspend(OI_UINT16 *handles,
                           OI_UINT8 num,
                           OI_UINT8 *transaction);

/**
 * This function sends the RECONFIGURE command. The endpoint must have
 * been previously suspended.
 *
 * @param handle      the stream handle
 * @param caps        the list of capabilities to configure
 * @param numCaps     the number of capabilities in the list
 * @param transaction  [OUT] parameter indicating the transaction number (May be NULL.)
 */
OI_STATUS OI_AVDTP_Reconfigure(OI_UINT16 handle,
                               OI_AVDTP_CAP *caps,
                               OI_UINT8 numCaps,
                               OI_UINT8 *transaction);

/**
 * This function sends a RECONFIGURE response on the specified
 * stream. This function should be called in response to an OI_AVDTP_RECONFIGURE_IND.
 *
 * @param handle       the stream handle
 * @param transaction  the transaction number
 * @param category     the first configuration category that failed, or 0 if successful
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
OI_STATUS OI_AVDTP_ReconfigureRsp(OI_UINT16 handle,
                                  OI_UINT8 transaction,
                                  OI_UINT8 category,
                                  OI_UINT8 error);

/**
 * This macro sends media data from the specified endpoint.
 *
 * @param handle      the stream handle
 * @param seqNum      the sequence number of the frame
 * @param timestamp   the timestamp for the frame
 * @param data        the data to send
 * @param dataLen     the number of bytes to send
 * @param payloadType the payload type
 * @param marker      the marker bit
 */
#define OI_AVDTP_SendData(handle, seqNum, timestamp, data, dataLen, payloadType, marker) \
    OI_AVDTP_SendFragmented(handle, seqNum, timestamp, payloadType, marker, 1, dataLen, data)

/**
 * This function sends Broadcast media data from the specified endpoint.
 *
 * @param handle     the stream handle
 * @param seqNum     the sequence number of the frame
 * @param offset     offset into data packet of first whole media frame
 * @param fractional byte count of fraction media frame at end of data packet
 * @param fec        Boolean indicating if data is a FEC packet
 * @param data       the data to send
 * @param dataLen    the number of bytes to send
 */
OI_STATUS OI_AVDTP_BroadcastData(OI_UINT16 handle,
                                 OI_UINT8  seqNum,
                                 OI_UINT8  offset,
                                 OI_UINT8  fractional,
                                 OI_BOOL   fec,
                                 OI_BYTE * data,
                                 OI_UINT16 dataLen);

/**
 * This macro sends media data from the specified endpoint
 * including an application-level header.  This can be used to easily
 * add A2DP header information.
 *
 * @param handle      the stream handle
 * @param seqNum      the sequence number of the frame
 * @param timestamp   the timestamp for the frame
 * @param payloadType the payload type
 * @param marker      the marker bit
 * @param data        the data to send
 * @param dataLen     the number of bytes to send
 * @param header      the header data to send (may be NULL)
 * @param headerLen   the number of bytes of header data to send
 */
#define OI_AVDTP_SendWithHeader(handle, seqNum, timestamp, payloadType, marker, data, dataLen, header, headerLen) \
    OI_AVDTP_SendFragmented(handle, seqNum, timestamp, payloadType, marker, 2, headerLen, header, dataLen, data)

/**
 * This function sends segmented media data from the specified endpoint.
 * As many segments as desired can be sent, and may include both
 * enveloping data (such as A2DP Application header) and multi-
 * segment media data. Segment count may be Zero to send AVDTP
 * header only frames. Data Lengths and Pointers are all variable
 * arguments.
 *
 * @param handle      the stream handle
 * @param seqNum      the sequence number of the frame
 * @param timestamp   the timestamp for the frame
 * @param payloadType the payload type
 * @param marker      the marker bit
 * @param segments    the number segments to send in this media frame
 * @param d1_len      - (optional) number of bytes to send from segment 1
 * @param d1_ptr      - (optional) segment 1 data pointer
 * @param d2_len      - (optional) number of bytes to send from segment 2
 * @param d2_ptr      - (optional) segment 2 data pointer
 *          .         - (etc)
 *          .
 *          .
 */
OI_STATUS OI_AVDTP_SendFragmented(OI_UINT16 handle,
                                  OI_UINT16 seqNum,
                                  OI_UINT32 timestamp,
                                  OI_UINT8  payloadType,
                                  OI_UINT16 marker,
                                  OI_INT    segments,
                                  ...);

/**
 * This function is equivalent to OI_AVDTP_SendFragmented, except that the
 * varaible argument list must already have been processed by the caller.
 * 
 * @param handle      the stream handle
 * @param seqNum      the sequence number of the frame
 * @param timestamp   the timestamp for the frame
 * @param payloadType the payload type
 * @param marker      the marker bit
 * @param segments    the number segments to send in this media frame
 * @param args        argument list, as per \OI_AVDTP_SendFragmented
 */
OI_STATUS OI_AVDTP_VSendFragmented(OI_UINT16 handle,
                                   OI_UINT16 seqNum,
                                   OI_UINT32 timestamp,
                                   OI_UINT8  payloadType,
                                   OI_UINT16 marker,
                                   OI_INT    segments,
                                   va_list args);

/**
 * This function sends security data on the specified endpoint. 
 * The endpoint must have been configured for streaming.
 *
 * @param handle   the stream handle
 * @param data     the data to send
 * @param dataLen  the number of bytes to send
 * @param transaction  [OUT] parameter indicating the transaction number (May be NULL.)
 */
OI_STATUS OI_AVDTP_SecurityControl(OI_UINT16 handle,
                                   OI_BYTE *data,
                                   OI_UINT16 dataLen,
                                   OI_UINT8 *transaction);

/**
 * This function sends response security data on the specified endpoint.
 *
 * @param handle       the stream handle
 * @param transaction  the transaction number
 * @param data         the data to send
 * @param dataLen      the number of bytes to send
 * @param error        the error status, or OI_AVDTP_SUCCESS if successful
 */
OI_STATUS OI_AVDTP_SecurityControlRsp(OI_UINT16 handle,
                                      OI_UINT8 transaction,
                                      OI_BYTE *data,
                                      OI_UINT16 dataLen,
                                      OI_UINT8 error);

/**
 * This function Creates a dynamic memory copy of a capabilities struct.
 * It must be freed at some point by OI_AVDTP_FreeCaps.
 *
 * @param caps            Capability struct to copy
 * @param numCaps         Number of capabilities in struct
 * @param newCaps         Location to set to new capability struct pointer
 * @param newNumCaps      Location to set to number of capabilities
 */

OI_STATUS OI_AVDTP_CopyCaps(const OI_AVDTP_CAP  *caps,
                            OI_UINT8             numCaps,
                            OI_AVDTP_CAP       **newCaps,
                            OI_UINT8            *newNumCaps);

/**
 * This function Frees a dynamic memory copy of a capabilities struct,
 * which was allocated by OI_AVDTP_CopyCaps.
 *
 * @param caps            Capability struct to free
 * @param numCaps         Number of capabilities in struct
 */

void OI_AVDTP_FreeCaps(OI_AVDTP_CAP **caps,
                       OI_UINT8       numCaps);


/**@}*/

#endif
#endif /* FEATURE_BT_EXTPF_AV */
