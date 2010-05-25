#ifndef _OI_DEVMGR_H
#define _OI_DEVMGR_H
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
/**
    @file

    This file provides the interface between the Device Manager and
    other program layers.

    The Device Manager functions exist primarily to provide a higher
    level of abstraction above the HCI layer.

    Not all Device Manager commands are directly accessible through
    this API, some Device Manager APIs are private and only available
    to modules within the core protocol stack.

    Additional information is available in the @ref devMgr_docpage
    section.

    @note It is possible to use OI_HCI_SetEventFilter() to configure
    the local device to always automatically accept connections. The
    Device Manager is not able to read the event filter. The Device
    Manager is responsible for managing roles and this may not be
    possible if event filters are used. For example, the Device
    Manager might be configured to reject connections with role
    switching disabled at the same time that the event filter is
    configured to automatically accept connections. To avoid this
    conflict, it is strongly recommended that the
    OI_HCI_SetEventFilter() command not be used to control the
    automatic acceptance of connections. If the event filter command
    must be used, it is up to the applications to ensure that the
    event filter setting and Device Manager API setting do not
    conflict.

*/

#include    "oi_hci.h"

/** callback for various Device Manager functions

    @return status  OI_OK if the operation was successful.
                    Any other value indicates some error.
*/
typedef void (*OI_DEVMGR_STATUS_CB)(OI_STATUS status);

/**************************************************************************************

    Connection information and configuration

**************************************************************************************/
/**

    This function gets the Bluetooth Device Address (BD_ADDR)
    associated with an HCI connection handle.

    @param connectionHandle   HCI connection handle for which a BD_ADDR is desired.  The handle may be either
                              a SCO or ACL handle.

    @param pBdAddr            pointer to caller's variable where the BD_ADDR will be stored

    @return
                              - OI_OK: The connection exists and BD_ADDR has been placed into the caller's variable.
                              - OI_DEVMGR_NO_CONNECTION: No such HCI connection exists; caller's BdAddr variable is unchanged.
*/
OI_STATUS OI_DEVMGR_GetAddrFromHandle(OI_HCI_CONNECTION_HANDLE connectionHandle,
                        OI_BD_ADDR *pBdAddr);

/**

    This function gets the HCI ACL connection handle associated with a
    Bluetooth Device Address (BD_ADDR).

    @param pBdAddr             pointer to the device address for which a connection handle is desired

    @param pConnectionHandle   pointer to caller's variable where the HCI ACL connection handle will be stored.

    @return
                              - OI_OK: A connection exists; the HCI connection handle has been placed into the caller's variable.
                              - OI_DEVMGR_NO_CONNECTION: No such HCI connection exists for the specified BD_ADDR; the caller's connection handle variable is unchanged.
*/
OI_STATUS OI_DEVMGR_GetHandleFromAddr(OI_BD_ADDR *pBdAddr,
                        OI_HCI_CONNECTION_HANDLE *pConnectionHandle);

/**
    This is the type of the callback function for
    OI_DEVMGR_SetLocalDeviceConnectability() and
    OI_DEVMGR_GetLocalDeviceConnectability().

    @param status  OI_OK if the get/set connectability operation was successful.
                    any other value indicates some error.

    @param connectable  Contains the local device's current connectability.
                            TRUE = device is connectable.
                            FALSE = device is not connectable.
                        If the status parameter is not OI_OK, this parameter value in undefined

*/
typedef void (*OI_DEVMGR_CONNECTABILITY_CB)(OI_STATUS status,
                        OI_BOOL     connectable);


/**
    This function sets the local device to be connectable or not
    connectable.  The implementation modifies the device's scan_enable
    parameter.
    
    This function permits a NULL callback in case the application does
    not require explicit notification that the operation has
    completed.

    @param completeCb   Callback which will be invoked when the operation is complete.  This callback will
                        be invoked only if this function returns OI_OK.

    @param connectable
                        - TRUE requests that the local device be connectable.
                        - FALSE requests that the local device be non-connectable.

    @return
                        - OI_OK: Device manager will attempt to set the local device's connectability as requested.  Upon
                        completion (successful or not), the completeCb callback will be called with the
                        final status of the operation.
                        - OI_DEVMGR_PARAM_IO_ACTIVE: The Device Manager can not attempt to set connectability because a
                        conflicting operation (e.g., OI_DEVMGR_WriteDiscoverability()) is in progress.
                        - Any other value indicates an error and the device's connectability will not have been
                        set as requested.
*/
OI_STATUS OI_DEVMGR_SetLocalDeviceConnectability(OI_DEVMGR_CONNECTABILITY_CB completeCb,
                                                 OI_BOOL connectable);

/**

    This function reads the local device's connectability state.  The
    implementation is to read the device's scan_enable parameter.

    @param completeCb   Callback which will be invoked when the operation is complete.  This callback will
                        be invoked only if this function returns OI_OK.

    @return           - OI_OK: Device manager will attempt to get the local device's connectability.  Upon
                            completion (successful or not), the completeCb callback will be called with the
                            final status of the operation.
                        any other value indicates an error and the callback will not be invoked.
*/
OI_STATUS OI_DEVMGR_GetLocalDeviceConnectability(OI_DEVMGR_CONNECTABILITY_CB completeCb);

/**

    This function returns the specified remote device's classOfDevice setting.  The device class of a
    remote device is available in the following circumstances:
        1. The remote device has been recently discovered and the device information is still in
            the device manager's inquiry cache.
        2. The remote device initiated an ACL connection to the local device and the connection is
            currently in progress or has been established.

    @param pBdAddr      pointer to the device address for which the class of device is requested

    @return             Remote device class if available, zero otherwise.
*/

OI_UINT32 OI_DEVMGR_GetRemoteDeviceClass(OI_BD_ADDR *pBdAddr);

/**
   This function registers a service class with the local device and
   modifies the class of device advertised by the device to reflect
   the added service class.

   @param serviceClass  an OR of one or more of the service classes specified in oi_bt_assigned_nos.h
*/
OI_STATUS OI_DEVMGR_RegisterServiceClass(OI_UINT32 serviceClass);

/**
   This function deregisters a service class with the local device and
   modifies the class of device advertised by the device to reflect
   the change.

   @param serviceClass  an OR of one or more of the service classes specified in oi_bt_assigned_nos.h
*/
OI_STATUS OI_DEVMGR_DeregisterServiceClass(OI_UINT32 serviceClass);

/**
   This function sets the device class portion of the class of device
   field to the specified value.

   The major and minor device class should be specified using the constants defined in @ref oi_bt_assigned_nos.h.
   For example, a laptop computer would use:
        OI_DEVMGR_SetDeviceClass(OI_BT_MAJOR_DEVICE_CLASS_COMPUTER, OI_BT_MINOR_DEVICE_CLASS_LAPTOP)

   @param majorDevClass  the major device class
   @param minorDevClass  the minor device class
*/
OI_STATUS OI_DEVMGR_SetDeviceClass(OI_UINT32 majorDevClass,
                                   OI_UINT32 minorDevClass);


/**************************************************************************************

    LOW-POWER APIs

    These functions put the local device in and out of power-saving
    modes.  There are no callbacks for these functions - instead, a
    'mode change' event occurs whenever there is a mode change or if a
    mode-change command fails.  Applications that are interested in
    mode changes may register with the device manager - see @ref
    OI_DEVMGR_RegisterCB_ModeChange().  Note that some mode changes
    may take some time to occur.  In particular, exit park mode will
    have to wait until the next beacon interval to take effect.

    The device manager will disallow any low-power mode which
    conflicts with the current link policy setting.

    All function parameters are specified as described in the HCI
    specification for the corresponding HCI command.  Users of these
    functions should refer to the Bluetooth HCI specification for
    detailed description of the various parameters.  The BlueMagic
    Device Manager simply passes function parameters to the HCI layer
    as is.

**************************************************************************************/

OI_STATUS OI_DEVMGR_HoldMode(                                   // ref 4.6.1
                    OI_BD_ADDR  *pBdAddr,
                    OI_UINT16   holdModeMaxInterval,
                    OI_UINT16   holdModeMinInterval);

OI_STATUS OI_DEVMGR_SniffMode(                                  // ref 4.6.2
                    OI_BD_ADDR  *pBdAddr,
                    OI_UINT16   sniffMaxInterval,
                    OI_UINT16   sniffMinInterval,
                    OI_UINT16   sniffAttempt,
                    OI_UINT16   sniffTimeout);

OI_STATUS OI_DEVMGR_ExitSniffMode (                             // ref 4.6.3
                    OI_BD_ADDR  *pBdAddr);

OI_STATUS OI_DEVMGR_ParkMode(                                   // ref 4.6.4
                    OI_BD_ADDR  *pBdAddr,
                    OI_UINT16   beaconMaxInterval,
                    OI_UINT16   beaconMinInterval);

OI_STATUS OI_DEVMGR_ExitParkMode(                               // ref 4.6.5
                    OI_BD_ADDR  *pBdAddr);

/**
    Returns current mode on the specified connection.
    @param pBdAddr  device address of remote device
    @param pMode    Pointer to caller's variable where current mode will be stored.  Valid only if the
                    function returns OI_OK

    @return         OI_OK - connection mode has been stored in caller's variable
                    OI_DEVMGR_NO_CONNECTION - there is no connection to the specified bd addr.
*/
OI_STATUS OI_DEVMGR_GetCurrentMode(
                    OI_BD_ADDR  *pBdAddr,
                    OI_UINT8    *pMode);

/**************************************************************************************

    SCO/eSCO API

**************************************************************************************/

/** Structure for setting up and accepting Synchronous (eSCO) connections */
typedef struct{
    OI_UINT32 txBandwidth; /**< Octets per second (setup) / Max octets per second (accept)         */
    OI_UINT32 rxBandwidth; /**< Octets per second (setup) / Max octets per second (accept)         */
    OI_UINT16 maxLatency;  /**< Max sum of interval + retransmission window (in Slots)             */
    OI_UINT16 voiceSetting;/**< Voice Setting mask. See v1.2 Spec E:6.12 for definitions           */
    OI_UINT8  reTxEffort;  /**< 0 - no retry; 1 - opt for pwr; 2 - opt for qual; 0xFF - don't care */
    OI_UINT16 pktType;     /**< Packet Type mask. HV1/HV2/HV3/EV3/EV4/EV5                          */
} OI_DEVMGR_ESCO_PARAMS;



/**

This function enables an eSCO connection instead of a regular SCO
connection for the Hands-Free and Headset profiles without disturbing
SCO functionality.  SCO is the default, so if you do not use this
function to specify eSCO, the use of SCO will be assumed by these
profiles.  The profiles do not understand the difference between SCO
and eSCO connections, but user applications can distinguish between
them.

@param pEScoSetupParams Structure for setting up eSCO connections
@param pEScoAcceptParams Structure for accepting eSCO connections
@param pAddr Bluetooth address for which to establish eSCO connection
*/
OI_STATUS   OI_DEVMGR_SetScoParams(
                            const OI_DEVMGR_ESCO_PARAMS *pEScoSetupParams,
                            const OI_DEVMGR_ESCO_PARAMS *pEScoAcceptParams,
                            OI_BD_ADDR                  *pAddr);


/**
This function disables eSCO by setting the parameters for a given
Bluetooth address (or for the default Bluetooth address, which means
all connected devices) to NULL.

@param pAddr Bluetooth address for which to disable eSCO connection
*/
OI_STATUS   OI_DEVMGR_UnsetScoParams(OI_BD_ADDR *pAddr);



/**

    This callback function indicates the status of the establishment of a new SCO connection.
    There are three ways that a connection might be completed:
        - The local device requested the connection.
        - A remote device requested the connection and it was automatically accepted.
        - A remote device requested the connection, the "connect indication" (@ref OI_DEVMGR_SCO_CONNECT_IND_CB)
        callback function was called, and it returned TRUE.

    Note that the connection has been established only if the status is OI_OK.

    @param status              OI_OK if the connection was successful, otherwise error cause indicated by status according to values in oi_status.h

    @param connectionHandle    connection handle assigned to this SCO connection, which is valid only if status is OI_OK

    @param pBdAddr             pointer to the address of the remote device, which is valid only if status is OI_OK
*/
typedef void (*OI_DEVMGR_SCO_CONNECT_COMPLETE_CB)(OI_STATUS status,
                                           OI_HCI_CONNECTION_HANDLE connectionHandle,
                                           OI_BD_ADDR *pBdAddr);

/**

    This callback function indicates the status of the termination of
    a SCO connection, where the termination was requested by the
    application. If the status is OI_OK, the connection has been
    terminated and the reason for the disconnection is indicated. If
    the status is not OI_OK, the connection is not terminated.

    @param status               OI_OK if the disconnection was successful, otherwise status indicates error cause (oi_status.h)

    @param  connectionHandle    handle for the connection that has been terminated

    @param  reason              reason for the disconnection
*/

typedef void (*OI_DEVMGR_SCO_DISCONNECT_COMPLETE_CB)(OI_STATUS status,
                                              OI_HCI_CONNECTION_HANDLE connectionHandle,
                                              OI_STATUS reason);

/**

    Connect indication: When a remote device wants to create a SCO
    connection, the application should return TRUE through this
    callback function if the application wants to accept the
    connection; the application should return FALSE otherwise. If no
    callback is registered, connection requests are rejected.  If the
    connection is actually completed, there will be a subsequent
    'connect complete' (@ref OI_DEVMGR_SCO_CONNECT_COMPLETE_CB)
    callback.

    Connections may be automatically accepted by the module if so
    configured via OI_HCI_SetEventFilter(), in which case this
    callback function will never be called.  
    @note The use of HCI
    event filters is strongly discouraged because they may interfere
    with the proper operation of the device manager.

    @param  pBdAddr         pointer to the Bluetooth device address of the remote device

    @param  deviceClass     class of the device, as defined at the Bluetooth website 
                            (www.bluetooth.org/assigned-numbers/baseband.htm)
*/
typedef OI_BOOL (*OI_DEVMGR_SCO_CONNECT_IND_CB)(OI_BD_ADDR *pBdAddr,
                                         OI_UINT32       deviceClass);


/**

This callback function indicates that the Bluetooth device's SCO
buffers have overflowed.

*/
typedef void (*OI_DEVMGR_SCO_DATA_BUFFER_OVERFLOW_CB)(void);

/**

    This callback function indicates that the Bluetooth device's
    SCO/eSCO configuration has been established or changed.

    @param  connectionHandle   eSCO connection which is being configured

    @param  txInterval         Time between two consecutive eSCO instants measured in slots

    @param  reTxWindow         The size of the retransmission window measured in slots. Must be zero for SCO links

    @param  rxPacketLength     Length in bytes of the eSCO payload in the receive direction

    @param  txPacketLength     Length in bytes of the eSCO payload in the transmit direction

    @param  airMode            Data format of over-the-air data (uLaw/aLaw/CVSD/Transparent)
*/
typedef void (*OI_DEVMGR_SCO_CONFIGURATION)(OI_HCI_CONNECTION_HANDLE connectionHandle,
                                            OI_UINT8                 txInterval,
                                            OI_UINT8                 reTxWindow,
                                            OI_UINT16                rxPacketLength,
                                            OI_UINT16                txPacketLength,
                                            OI_UINT8                 airMode);



/**

    This callback function passes received SCO data back up to the
    application.  It is needed only if SCO data is passed through the
    HCI protocol layer.

    @param  connectionHandle   connection from which the data was received

    @param  rcvBuf             pointer to the received data

    @param  rcvBufLen          number of bytes that were received
*/
typedef void    (*OI_DEVMGR_SCO_RECEIVE_DATA_CB)(OI_HCI_CONNECTION_HANDLE connectionHandle,
                                          OI_BYTE                 *rcvBuf,
                                          OI_UINT16 rcvBufLen);

/**

    This callback function indicates that the transport layer is ready
    for SCO data.  The transport layer has limited buffering. Each
    time that a SCO data packet is sent, the application should wait
    for this callback before sending the next data packet.  This
    callback function is needed only if SCO data is passed through the
    HCI protocol layer.

*/
typedef void    (*OI_DEVMGR_SCO_TRANSPORT_READY)(void);

/**

    This callback function is called when SCO packets have been sent
    by the Bluetooth module.  This flow control indication could be
    used to drive the sending of subsequent SCO data packets.  This
    callback function is needed only if SCO data is not passed through
    the HCI protocol layer and SCO flow control is enabled. Refer to
    section H:1.5.2.19 for details.

    @param  scoConnection           handle for the connection that completed the transmission of the packets

    @param  numCompletedPackets     number of packets that were transmitted

*/
typedef void (*OI_DEVMGR_SCO_TX_PACKET_COMPLETED)(OI_HCI_CONNECTION_HANDLE scoConnection,
                                           OI_UINT16 numCompletedPackets);


/** All SCO Signaling callback functions are passed to the Device Manager for registration through this structure. */
typedef const struct  {
    OI_DEVMGR_SCO_CONNECT_COMPLETE_CB      scoConnectCompleteCb;       /**< callback function of type OI_DEVMGR_SCO_CONNECT_COMPLETE_CB to be registered */
    OI_DEVMGR_SCO_DISCONNECT_COMPLETE_CB   scoDisconnectCompleteCb;    /**< callback function of type OI_DEVMGR_SCO_DISCONNECT_COMPLETE_CB to be registered */
    OI_DEVMGR_SCO_CONNECT_IND_CB           scoConnectIndCb;            /**< callback function of type OI_DEVMGR_SCO_CONNECT_IND_CB to be registered */
}   OI_DEVMGR_SCO_SIGNAL_CALLBACKS;

/** All SCO Data callback functions are passed to the Device Manager for registration through this structure. */
typedef const struct  {
    OI_DEVMGR_SCO_DATA_BUFFER_OVERFLOW_CB  scoDataBufferOverflowCb;    /**< callback function of type OI_DEVMGR_SCO_DATA_BUFFER_OVERFLOW_CB to be registered */
    OI_DEVMGR_SCO_RECEIVE_DATA_CB          scoReceiveDataCb;           /**< callback function of type OI_DEVMGR_SCO_RECEIVE_DATA_CB to be registered */
    OI_DEVMGR_SCO_TRANSPORT_READY          scoTransportReadyCb;        /**< callback function of type OI_DEVMGR_SCO_TRANSPORT_READY to be registered */
    OI_DEVMGR_SCO_TX_PACKET_COMPLETED      scoTxPacketCompletedCb;     /**< callback function of type OI_DEVMGR_SCO_TX_PACKET_COMPLETED to be registered */
    OI_DEVMGR_SCO_CONFIGURATION            scoConfigurationCb;         /**< callback function of type OI_DEVMGR_SCO_CONFIGURATION to be registered */
}   OI_DEVMGR_SCO_DATA_CALLBACKS;

/** If registration of the callback functions is successful, the Device Manager returns SCO parameters
    to the application in this structure. This information is needed only if SCO data is passed through
    the HCI protocol layer. */

typedef struct  {
    OI_UINT8 scoMaxDataPacketSize;       /**< maximum size of a SCO packet in bytes */
    OI_UINT16 scoTotalNumBuffers;         /**< number of SCO buffers in the Bluetooth module */
} OI_DEVMGR_SCO_DATA_PARAMETERS;

/**
    SCO registration: An application must register with the Device
    Manager in order to access the SCO API for a particular remote
    device. Only one application can handle a particular remote
    device.

    @param  pCallbacks               This is a pointer to the structure of application callback functions
                                     for the various SCO events. The Device Manager does not copy this structure;
                                     it just retains a pointer to the structure. The application may use a
                                     const structure for application callback functions.

    @param  pAddr                    pointer to the remote device BdAddr that these SCO callbacks will handle

*/
OI_STATUS OI_DEVMGR_RegisterScoApplication(OI_DEVMGR_SCO_SIGNAL_CALLBACKS          *pCallbacks,
                                           OI_BD_ADDR                              *pAddr);

/**
    SCO deregistration: Removes the current SCO registration, freeing
    SCO to be used by some some other application.  The application
    must disconnect any SCO connections that use these callbacks
    before calling this function. This function removes the callbacks
    for all devices that use these particular callbacks.

    @param  pCallbacks               This is a pointer to the structure of application callback functions which
                                     was originally registered.  This parameter is used to verify that the
                                     deregistering application is the same application that originally registered
                                     to use the SCO api's.

*/
OI_STATUS OI_DEVMGR_DeregisterScoApplication(OI_DEVMGR_SCO_SIGNAL_CALLBACKS  *pCallbacks);

/**

    SCO Data handler registration: An application must register with
    the Device Manager in order to receive SCO Data callbacks. Only
    one application may handle SCO data for a particular remote
    device.  This call must be made after setting up the signalling
    callbacks via OI_DEVMGR_RegisterScoApplication().

    @param  pCallbacks               This is a pointer to the structure of application callback functions
                                     for the various SCO data events. The Device Manager does not copy this structure;
                                     it just retains a pointer to the structure. The application may use a
                                     const structure for application callback functions.

    @param  pAddr                    pointer to the remote device BdAddr that these SCO callbacks will handle

*/
OI_STATUS OI_DEVMGR_RegisterScoDataApplication(OI_DEVMGR_SCO_DATA_CALLBACKS          *pCallbacks,
                                               OI_BD_ADDR                            *pAddr);

/**
    SCO Data handler deregistration: Removes the current SCO Data
    registration, freeing SCO Data to be used by some other
    application.  The application must disconnect any SCO connections
    that use these callbacks before calling this function. This
    function removes the callbacks for all devices that use these
    particular callbacks.

    @param  pCallbacks               This is a pointer to the structure of application callback functions which
                                     was originally registered.  This parameter is used to verify that the
                                     deregistering application is the same application that originally registered
                                     to use the SCO APIs.

*/
OI_STATUS OI_DEVMGR_DeregisterScoDataApplication(OI_DEVMGR_SCO_DATA_CALLBACKS          *pCallbacks);


/**
    SCO connection creation: The Device Manager will create the
    underlying ACL connection if needed. Note that multiple SCO
    connections can be created to the same device. Connection requests
    must be serialized; they cannot be executed in parallel.

    @param  pBdAddr        pointer to the Bluetooth Device Address of the device with which to create a connection

    @return
                           - OI_OK: The connection is being attempted; the application's 'connect complete'
                           callback function will be called when the connect completes.
                           - Any other return value: the connect request is rejected and the 'connect complete'
                           callback will not be called.
*/
OI_STATUS OI_DEVMGR_ScoCreateConnection(OI_BD_ADDR *pBdAddr);

/**

    SCO connection Reconfigure: If the remote device is connected to
    the local device on a SCO/eSCO link, and the local device is a
    v1.2 complient device, this command will attempt to reconfigure
    the SCO/eSCO link to the current eSCO parameters registered for
    this BD Addr.

    @param  handle           connectionHandle of the eSCO connection to reconfigure

    @return
                           - OI_OK: The reconfiguration is being attempted. scoConfigurationCb will be called on completion
                           - Any other return value: the reconfigure request is rejected and the scoConfigurationCb
                             callback will not be called.
*/
OI_STATUS OI_DEVMGR_ScoReconfigure(OI_HCI_CONNECTION_HANDLE  handle);

/**

    SCO connection termination: The application's 'disconnect
    complete' callback will be called when the disconnect completes.

    @param  connectionHandle    handle for the connection that is to be terminated

*/
OI_STATUS OI_DEVMGR_ScoDisconnect(OI_HCI_CONNECTION_HANDLE connectionHandle);

/**

    This function writes data to a SCO connection.

    @param  connectionHandle    handle for the connection to which data is to be written

    @param  txBuf               pointer to the SCO data buffer from which data is to be written (transmitted) over the SCO connection

    @param  txBufLen            length in bytes of the SCO data buffer

*/
OI_STATUS OI_DEVMGR_WriteScoData(OI_HCI_CONNECTION_HANDLE connectionHandle,
                        OI_BYTE                 *txBuf,
                        OI_UINT8 txBufLen);

/**

    If SCO data passes through the HCI protocol layer and SCO flow
    control is enabled, this function can be polled to determine
    whether both the transport layer and Bluetooth module are ready
    for another data packet to be written. This function is an
    alternative to the flow control callback functions.

*/
OI_BOOL     OI_DEVMGR_ReadyToTransmitSco(void);

/**************************************************************************************

    Inquiry

**************************************************************************************/


/**

    This callback function makes the results of a device inquiry
    available to the application through an HCI inquiry result
    record. The fields in the result record map directly to the HCI
    Inquiry Result event as defined in section H:1.5.2.2 of the
    Bluetooth specification and as described in the documentation for
    the type OI_HCI_INQUIRY_RESULT_REC.

    @param pInqRec    pointer to the HCI inquiry result record

*/
typedef void (*OI_DEVMGR_INQUIRY_RESULT_CB)(OI_HCI_INQUIRY_RESULT_REC  *pInqRec);

/**

    This callback function becomes runnable when inquiry has
    completed, whether successful or not.

    @param status  OI_OK if inquiry was successful, otherwise error cause indicated by status according to oi_status.h
*/
typedef void (*OI_DEVMGR_INQUIRY_COMPLETE_CB)(OI_STATUS status);

/**

    This callback function becomes runnable when inquiry cancellation
    has completed, whether successful or not.

    @param status  OI_OK if inquiry cancellation was successful, otherwise error cause indicated by status according to OI_HCIERR_ values in oi_status.h
*/
typedef void (*OI_DEVMGR_CANCEL_COMPLETE_CB)(OI_STATUS status);

/**

    Initiation of inquiry: This function intiates an inquiry by
    putting the device in Inquiry Mode. This function maps directly to
    the HCI_Inquiry command described in section H:1.4.5.1 of the
    Bluetooth specification.

    @param LAP               Lower Address Part from which the inquiry access code should be derived, as defined
                             in H:1.4.5.1 of the Bluetooth specification
                             Use the constants OI_LAP_GIAC (general inquiry) or OI_LAP_LIAC (limited inquiry)


    @param inquiryLength     This 8-bit parameter may take values from 0x01 to 0x30, indicating increments of 1.28 seconds,
                             for a range of 1.28 seconds to 61.44 seconds. This value specifies the maximum duration of
                             inquiry; after the time indicated by inquiryLength has passed, inquiry will halt.

    @param maxNumResponses   This parameter may take values from 0x00 to 0xFF. 0x00 indicates that an unlimited
                             number of responses may be taken to an inquiry. Any other value specifies a count
                             of the maximum number of responses that will be accepted before inquiry is halted.

    @param resultCB          This is the caller's callback function to be called with each inquiry result.
                             This callback function may be called multiple times during the inquiry process.

    @param completeCB        This is the callback function to be called upon the completion of the inquiry process.
*/
OI_STATUS OI_DEVMGR_PerformInquiry(OI_UINT32               LAP,
                    OI_UINT8 inquiryLength,
                    OI_UINT8 maxNumResponses,
                    OI_DEVMGR_INQUIRY_RESULT_CB resultCB,
                    OI_DEVMGR_INQUIRY_COMPLETE_CB completeCB);

/**

    Cancellation of inquiry: This function cancels an inquiry and maps
    directly to the HCI Inquiry_Cancel command described in section
    H:1.4.5.2 of the Bluetooth specification.

    @param completeCB       This is the callback function that will be called upon the completion of the inquiry cancellation command.
*/
OI_STATUS OI_DEVMGR_CancelInquiry(OI_DEVMGR_CANCEL_COMPLETE_CB completeCB);

/**

    Start periodic inquiry mode: This function initiates a periodic
    inquiry by putting the device in Periodic Inquiry Mode.  This
    function maps directly to the HCI Periodic_Inquiry_Mode command
    described in section H:1.4.5.3 of the Bluetooth specification.

    @param maxPeriodLength   This indicates the upper bound for the random time between the beginnings of two consecutive inquiries,
                             in units of 1.28 seconds. The range of valid values is 0x03 to 0xFFFF, indicating 3.84 seconds
                             to 83884.80 seconds.

    @param minPeriodLength   This indicates the lower bound for the random time between the beginnings of two consecutive inquiries,
                             in units of 1.28 seconds. The range of valid values is 0x02 to 0xFFFE, indicating 2.56 seconds
                             to 83883.52 seconds.

    @param LAP               Lower Address Part from which the inquiry access code should be derived, as defined in H:1.4.5.2

    @param inquiryLength     This 8-bit parameter may take values from 0x01 to 0x30, indicating increments of 1.28 seconds,
                             for a range of 1.28 seconds to 61.44 seconds. This value specifies the maximum duration of
                             inquiry; after the time indicated by inquiryLength has passed, inquiry will halt.

    @param maxNumResponses   This parameter may take values from 0x00 to 0xFF. 0x00 indicates that an unlimited
                             number of responses may be taken to an inquiry. Any other value specifies a count
                             of the maximum number of responses that will be accepted before inquiry is halted.

    @param resultCB          This is the caller's callback function to be called with each inquiry result.
                             This callback function may be called multiple times during the inquiry process.

    @param completeCB        This is the callback function to be called upon the completion of the inquiry process.
*/
OI_STATUS OI_DEVMGR_StartPeriodicInquiry(OI_UINT16 maxPeriodLength,
                    OI_UINT16 minPeriodLength,
                    OI_UINT32               LAP,
                    OI_UINT8 inquiryLength,
                    OI_UINT8 maxNumResponses,
                    OI_DEVMGR_INQUIRY_RESULT_CB resultCB,
                    OI_DEVMGR_INQUIRY_COMPLETE_CB completeCB);

/**

    Stop a periodic inquiry: This function maps directly to the HCI
    Exit_Periodic_Inquiry_Mode command described in section H:1.4.5.4
    of the Bluetooth specification.

    @param completeCB       This is the callback function that will be called when the command to stop periodic
                            inquiry has completed.
*/
OI_STATUS OI_DEVMGR_StopPeriodicInquiry(OI_DEVMGR_CANCEL_COMPLETE_CB completeCB);

/**************************************************************************************

    Device Enumeration

**************************************************************************************/



/** This indicates the state of the corresponding device enumeration database record. */
typedef enum  {
    /** There is no record. */
    OI_ST_EMPTY,

    /** This device was seen previously but did not respond during the current inquiry. */
    OI_ST_GONE,

    /** This device responded to the current inquiry. All record fields are valid. */
    OI_ST_PRESENT
} OI_DEVMGR_ENUM_DB_RECORD_STATE;


/** This is the device enumeration record type. The device enumeration database is simply
    an array of such records. */
typedef struct  {
        /** record state: must be set to OI_ST_EMPTY for unused device enmueration records */
    OI_DEVMGR_ENUM_DB_RECORD_STATE    recordState;

        /** Bluetooth device address: valid only if recordState != OI_ST_EMPTY */
    OI_BD_ADDR devAddr;

        /** Device name: This points to a buffer where the device name associated with this record is stored.
            If the pointer is NULL, then there is no name associated with the device and the
            Device Manager will never attempt to read the device name. If the pointer points to
            a buffer containing a NULL string, the Device Manager will attempt to read the device name
            for devices for which recordState is OI_ST_PRESENT. If the pointer points to a buffer containing a
            non-NULL string, the string is assumed to be the device name and the Device Manager will not alter the
            pointer or the buffer.

            The global system configuration parameter deviceNameMaxLen (set in the OI_COMMON_SYSTEM_CONFIGURATION
            in oi_bt_stack_config.h) determines how large the name string buffer must be; the name string buffer must be
            at least deviceNameMaxLen characters long. The Device Manager will truncate longer names so that they are
            no longer than deviceNameMaxLen bytes.
        */
    OI_CHAR             *devName;

        /** device class: a 24-bit number, so bits 24-31 are unused and will be zero always */
    OI_BT_DEVICE_CLASS     deviceClass;

        /** a simple counter that is incremented each time this device is discovered  */
    OI_UINT16 discoveryCount;

} OI_ENUM_DB_RECORD;


/**

    Each time that the underlying inquiry completes, this application callback function is called.

    @param completionStatus     status received from the inquiry complete event

    @param otherError           This indicates an error that has occurred during the enumeration process.
                                For example, the inquiry process may complete successfully, but there may have been
                                more results than will fit into the database. In this case, the otherError parameter
                                would contain the status value OI_DEVMGR_ENUM_DATABASE_FULL.
                                Applications will typically treat these other errors as warnings, rather than hard errors.

    @param pDeviceEnumDb        pointer to the device enumeration database, the same pointer that was
                                passed to OI_DEVMGR_EnumerateDevices()
*/
typedef void (*OI_DEVMGR_ENUMERATE_COMPLETE_CB)(OI_STATUS completionStatus,
                        OI_STATUS otherError,
                        OI_ENUM_DB_RECORD  *pDeviceEnumDb);

/** OI_DEVMGR_EnumerateDevices() takes quite a few parameters. In order to avoid having a long parameter
    list, the calling parameters are collected together into a single structure that can then be passed by reference.
*/
typedef struct  {
    OI_DEVMGR_ENUMERATE_COMPLETE_CB callback;    /**< application callback function invoked each time enumeration completes */

    OI_ENUM_DB_RECORD *pEnumerationDatabase; /**< pointer to a device enumeration database as an array of device enumeration records */

    OI_UINT16 enumerationDbArraySize;     /**< total size of the device enumeration database array */

    OI_UINT32 LAP;                        /**< HCI Inquiry parameter: Lower Address Part from which the inquiry access code should
                                                be derived, as defined in H:1.4.5.1.
                                                Use the constants OI_LAP_GIAC (general inquiry) or OI_LAP_LIAC (limited inquiry)  */

    OI_UINT8 inquiryLength;               /**< HCI Inquiry parameter: This 8-bit parameter may take values from 0x01 to 0x30,
                                                indicating increments of 1.28 seconds, for a range of 1.28 seconds to 61.44
                                                seconds. This value specifies the maximum duration of inquiry;
                                                after the time indicated by inquiryLength has passed, inquiry will halt. */

    OI_UINT8 maxNumResponses;             /**< HCI Inquiry parameter: This parameter may take values from 0x00 to 0xFF.
                                                0x00 indicates that an unlimited number of responses may be taken
                                                to an inquiry. Any other value specifies a count of the maximum number
                                                of responses that will be accepted before inquiry is halted.  */

    OI_BT_DEVICE_CLASS *pClassFilters;       /**< This is a pointer to a NULL-terminated array of device class values.
                                                Devices will be added to the device enumeration database only if their
                                                Major Device Class or Major Service Class matches one of the class values
                                                in the class filter array. Only bits 8-23 are significant (refer to the Bluetooth
                                                Assigned Numbers document at http://www.bluetooth.org/assigned-numbers/ for more information.
                                                If the pointer is NULL, there is no array and devices will not be filtered by class. */

    OI_UINT16 periodLength;               /**< A value of zero for this parameter indicates that one-time inquiry is to be performed.
                                                Any other value indicates often periodic inquiry is to be performed by specifying how much
                                                time (in units of 1.28 seconds) elapses between the beginnings of subsequent inquiries. The
                                                range of valid values is 0x02 to 0xFFFF, indicating 2.56 seconds to 83884.80 seconds. */

} OI_DEVMGR_ENUMERATION_PARAMS;


/**

    Initiation of device enumeration: The callback passed via @ref OI_DEVMGR_ENUMERATION_PARAMS will be called
    when the enumeration has completed. If a periodic enumeration is requested (see periodLength in
    OI_DEVMGR_ENUMERATION_PARAMS), the callback function will be called each time enumeration completes.

    @param  pParams pointer to a structure containing parameters for this function call
*/
OI_STATUS OI_DEVMGR_EnumerateDevices(OI_DEVMGR_ENUMERATION_PARAMS  *pParams);


/**

    This function cancels the device enumeration process (whether one-time or periodic).
    Note: the enumeration complete callback function will NOT be called. */
OI_STATUS OI_DEVMGR_CancelEnumerateDevices(void);


/**************************************************************************************

    Local device static settings

**************************************************************************************/

/** There are a number of local device settings that are typically set during system startup.
    The Device Manager provides functions to read or write these parameters with a single
    function call. This structure is used to pass these device settings to and from the Device Manager.
    The structure element values correspond with those defined for the HCI protocol in part H:1
    of the Bluetooth specification.

    @note The reads and writes are performed from the caller's structure. The caller must maintain
    the integrity of the structure until the callback function is invoked.

**************************************************************************************/
typedef struct {
    /** This configuration parameter indicates the amount of time between when the device sends a
      connection request event and when the device will automatically reject an incoming connection.
      The time is indicated in units of baseband slots (0.625 milliseconds) and the timeout parameter
      has a valid range of 0x0001 to 0xB540, for a range of 0.625 milliseconds to 29 seconds. This parameter
      and the HCI commands to read and write it are described in section H:1.4.7.13-14 of the Bluetooth specification. */
    OI_UINT16 connectionAcceptTimeout;

    /** This configuration parameter indicates the maximum amount of time the local Link Manager
      will wait for a baseband page reponse from the remote device at a locally initiated connection
      attempt. If the timeout period expires and the remote device has not yet responded to the page
      at the baseband level, the connection attempt will be considered to have failed. The time is
      indicated in units of baseband slots (0.625 milliseconds) and the timeout parameter
      has a valid range of 0x0001 to 0xFFFF, for a range of 0.625 milliseconds to 40.9 seconds. This parameter and
      the HCI commands to read and write it are described in section H:1.4.7.15-16
      of the Bluetooth specification. */
    OI_UINT16 pageTimeout;

    /** This configuration parameter indicates the amount of time between the beginning
      of one page scan and the beginning of the next page scan. The time is given in units of baseband slots
      and the parameter has a valid range of 0x0012 to 0x1000, for a range of 11.25 milliseconds to 2.56 seconds.
      This parameter and the HCI commands to read and write it are described in H:1.4.7.19-20 of the Bluetooth specification.
      This parameter can also be written using the api @ref OI_HCI_WritePageScanActivity() */
    OI_UINT16 pageScanActivityInterval;

    /** This configuration parameter indicates the duration of a page scan. The range for
      this parameter is identical to that for pageScanActivityInterval. The value of pageScanActivityWindow must
      be less than or equal to pageScanActivityInterval. This parameter and the HCI commands to read and write it are
      described in H:1.4.7.19-20 of the Bluetooth specification.
      This parameter can also be written using the api @ref OI_HCI_WritePageScanActivity() */
    OI_UINT16 pageScanActivityWindow;

    /** This configuration parameter indicates the amount of time between the beginning
      of one inquiry scan and the beginning of the next inquiry scan. The time is given in units of baseband slots
      and the parameter has a valid range of 0x0012 to 0x1000, for a range of 11.25 milliseconds to 2.56 seconds.
      This parameter and the HCI commands to read and write it are described in H:1.4.7.21-22 of the Bluetooth specification.
      This parameter can also be written using the api @ref OI_HCI_WriteInquiryScanActivity() */
    OI_UINT16 inquiryScanActivityInterval;

    /** This configuration parameter indicates the duration of an inquiry scan. The range for
      this parameter is identical to that for inquiryScanActivityInterval. The value of inquiryScanActivityWindow must
      be less than or equal to inquiryScanActivityInterval. This parameter and the HCI commands to read and write it are
      described in H:1.4.7.21-22 of the Bluetooth specification.
      This parameter can also be written using the api @ref OI_HCI_WriteInquiryScanActivity() */
    OI_UINT16 inquiryScanActivityWindow;

    /** This configuration parameter indicates how all voice connections will be treated by this device.
      This parameter controls input coding, air coding format, input data format, input sample size, and the linear PCM parameter.
      This parameter and the HCI commands to read and write it are described in H:1.4.7.29-30 of the Bluetooth specification. */
    OI_UINT16 voiceSetting;

    /** A broadcast packet may be sent multiple times. This parameter indicates how many times a broadcast
      packet will be retransmitted (how many times a packet will be sent after the first time). This parameter
      has a valid range of 0x00 to 0xFF. This parameter and the HCI commands to read and write it are described in
      H:1.4.7.33-34 of the Bluetooth specification. */
    OI_UINT8 numBroadcastRetransmissions;

    /** This parameter indicates which activities should be suspended during hold mode. This parameter's value
      is the result of a bitwise OR on values representing individual hold mode settings, as described in section
      H:1.4.7.35-36 of the Bluetooth specification:
          - 0x00    Maintain current power state.
          - 0x01    Suspend page scan.
          - 0x02    Suspend inquiry scan.
          - 0x04    Suspend periodic inquiries.
      */
    OI_UINT8 holdModeActivity;

    /** If this parameter is set to zero, SCO flow control is disabled and the OI_DEVMGR_SCO_TX_PACKET_COMPLETED callback function will
      not be called. If this parameter is set to 1, SCO flow control is enabled and the OI_DEVMGR_SCO_TX_PACKET_COMPLETED callback function
      will be called with an argument indicating the number of completed packets. This parameter and the HCI commands to read and
      write it are described in H:1.4.7.38-39 of the Bluetooth specification. */
    OI_UINT8 scoFlowEnable;

    /** This parameter and the HCI commands to read and write it are described in H:1.4.7.48-49 of the Bluetooth specification. */
    OI_UINT8 pageScanPeriodMode;

    /** This parameter and the HCI commands to read and write it are described in H:1.4.7.50-51 of the Bluetooth specification. */
    OI_UINT8 pageScanMode;

    /** This parameter and the HCI commands to read and write it are described in v1.2 7.3.51-52 of the Bluetooth specification. */
    OI_UINT8 inquiryScanType;

    /** This parameter and the HCI commands to read and write it are described in v1.2 7.3.53-54 of the Bluetooth specification. */
    OI_UINT8 inquiryMode;

    /** This parameter and the HCI commands to read and write it are described in v1.2 7.3.55-56 of the Bluetooth specification. */
    OI_UINT8 pageScanType;

    /** This parameter and the HCI commands to read and write it are described in v1.2 7.3.57-58 of the Bluetooth specification. */
    OI_UINT8 afhChannelAssessmentMode;
} OI_DEVMGR_STATIC_SETTINGS;

/**************************************************************************************/

/** Discoverability: This enumeration gives all possible discoverability combinations.
*/
typedef enum {
    OI_DEVMGR_DISCO_NOT_DISCOVERABLE,  /**< The local device is not discoverable. */
    OI_DEVMGR_DISCO_LIMITED,           /**< The local device is in limited discoverability mode.  */
    OI_DEVMGR_DISCO_GENERAL            /**< The local device is in general discoverability mode. */
} OI_DEVMGR_DISCO_SETTING;

/********************/

/**
  The callback function is called with the same settings structure pointer passed to OI_DEVMGR_WriteStaticSettings().

  @param status     status of request to write static settings

  @param pSettings  pointer to a OI_DEVMGR_STATIC_SETTINGS structure containing settings that were written
  */
typedef void (*OI_DEVMGR_WRITE_STATIC_SETTINGS_CB)(OI_STATUS status,
                                                   const OI_DEVMGR_STATIC_SETTINGS *pSettings);

/**
  This function writes settings from the caller's structure to the local device.

  @param cb         callback function

  @param pSettings  pointer to a OI_DEVMGR_STATIC_SETTINGS structure containing settings to be written
  */
OI_STATUS OI_DEVMGR_WriteStaticSettings(OI_DEVMGR_WRITE_STATIC_SETTINGS_CB cb,
                                        const OI_DEVMGR_STATIC_SETTINGS *pSettings);

/********************/

/**
  A callback function of this type is used to pass back a pointer to a structure containing
  a device's static settings that have just been read.

  @param status     status of the static settings read request

  @param pSettings  pointer to a OI_DEVMGR_STATIC_SETTINGS structure to contain settings once they are read

*/
typedef void (*OI_DEVMGR_READ_STATIC_SETTINGS_CB)(OI_STATUS status,
                                           OI_DEVMGR_STATIC_SETTINGS *pSettings);

/**

  This function reads settings from a device into a caller's structure.

  @param cb         The callback function    will be invoked when all settings have been read and will
  pass the settings up to the application through the structure pointer passed to the 'read' function.

  @param pSettings  pointer to a OI_DEVMGR_STATIC_SETTINGS structure to contain settings once they are read

*/
OI_STATUS OI_DEVMGR_ReadStaticSettings(OI_DEVMGR_READ_STATIC_SETTINGS_CB cb,
                                       OI_DEVMGR_STATIC_SETTINGS *pSettings);

/********************/

/**

  A callback function of this type passes back the results of a request to read discoverability settings
  of a device.

  @param status          status of the static settings read request

  @param discoSetting    discoverability settings

*/
typedef void (*OI_DEVMGR_READ_DISCO_CB)(OI_STATUS status,
                                 OI_DEVMGR_DISCO_SETTING discoSetting);

/**
  This is the type of the callback function for OI_DEVMGR_WriteDiscoverability().

  @param status          status of the request to write the discoverability setting

*/
typedef void (*OI_DEVMGR_WRITE_DISCO_CB)(OI_STATUS status);

/**

  This function reads the current discoverability settings. The underlying
  implementation consists of the HCI commands READ_SCAN_ENABLE and READ_CURRENT_IAC_LAP.

  @param  cb              Discoverability settings are passed up to the application through this
  callback function.

*/
OI_STATUS OI_DEVMGR_ReadDiscoverability(OI_DEVMGR_READ_DISCO_CB cb);

/**

  This function writes the current discoverability settings.  The
  underlying implementation consists of the HCI commands
  WRITE_SCAN_ENABLE, WRITE_CURRENT_IAC_LAP, and WRITE_CLASS_OF_DEVICE.

  Limited discoverability: The GAP profile requires that a device
  remain in limited discovery mode for no more than 1 minute.  The
  Device Manager will automatically switch from LIMITED to GENERAL
  discoverability mode after one minute in LIMITED discovery mode. The
  Device Manager will also manage bit 13 of the 'class of device'
  setting as described in the GAP profile.
  
  This function permits a NULL callback in case the application does
  not require explicit notification that the operation has completed.

  @param   cb             callback function

  @param   discoSetting   desired discoverability setting

*/
OI_STATUS OI_DEVMGR_WriteDiscoverability(OI_DEVMGR_WRITE_DISCO_CB cb,
                                         OI_DEVMGR_DISCO_SETTING discoSetting);


/**************************************************************************************

  Local device informational parameters

 **************************************************************************************/

/** These parameters are defined in H:1.4.8 of the Bluetooth specification, except for numSupportedIacs,
  which is described in H:1.4.7.45. The Device Manager provides a helper function that simply collects
  all of these parameters into a convenient structure.
  */
typedef struct {
    OI_HCI_LOCAL_VERSION_INFORMATION localVersionInformation; /**< Bluetooth hardware version information for the local device */
    OI_HCI_LMP_FEATURES lmpFeatures;                          /**< Link Manager Protocol features supported by the local device */
    OI_UINT16 hcAclDataPacketLength;                       /**< ACL data packet length supported by the local device */
    OI_UINT8 hcScoDataPacketLength;                        /**< SCO data packet length supported by the local device */
    OI_UINT16 hcTotalNumAclBuffers;                        /**< number of ACL buffers provided by the local device */
    OI_UINT16 hcTotalNumScoBuffers;                        /**< number of SCO buffers provided by the local device */
    OI_UINT8 countryCode;                                  /**< 0x01 indicates France. 0x00 indicates North America, Japan, and the remainder of Europe. Other values are reserved. */
    OI_UINT8 numSupportedIacs;                             /**< the number of Inquiry Access Codes that the local device can listen for simultaneously */
    OI_BD_ADDR bdAddr;                                     /**< Bluetooth device address for the local device */
} OI_DEVMGR_LOCAL_INFO_PARMS;

/********************/

/**
A callback function of this type is used to return the status of a request to read the informational parameters
and a pointer to a structure containing the parameters if the read was successful.

  @param status    status of request to read informational parameters

  @param pParms    pointer to a structure that contains the informational parameters that have been read

*/
typedef void (*OI_DEVMGR_READ_INFO_PARMS_CB)(OI_STATUS status,
                                      OI_DEVMGR_LOCAL_INFO_PARMS *pParms);

/**

  Informational parameters will be read and placed into the caller's structure.
  The callback function will be invoked when all parameters have been read.
  The callback function is called with the same parameters structure pointer passed to the this function.

  @param cb        callback function to pass the informational parameters up to the application once they are read

  @param pParms    pointer to a structure that contains the informational parameters that have been read
  */
OI_STATUS OI_DEVMGR_ReadInformationalParams(OI_DEVMGR_READ_INFO_PARMS_CB cb,
                                            OI_DEVMGR_LOCAL_INFO_PARMS *pParms);

/**
A callback function of this type is used to return the status of a request to read the local device's Bluetooth device address
and a pointer to a buffer containing the Bluetooth device address if the read was successful.

  @param status    status of request to get the local Bluetooth device address

  @param pAddr     pointer to the local device's Bluetooth device address

*/
typedef void (*OI_DEVMGR_GET_LOCAL_BDADDR_CB)(OI_STATUS status,
                                       OI_BD_ADDR *pAddr);

/**

  This function returns the local Bluetooth device address (BD_ADDR) to a location specified by the pointer
  argument in the function call. The local Bluetooth device address is read and cached as a global variable in
  the Device Manager during HCI intialization. Therefore, this function must not be called
  before stack initialization has been completed. This function always returns a success status code.

  @param pAddr     pointer to the location where the local device address will be stored

*/
OI_STATUS OI_DEVMGR_GetCachedLocalBdAddr(OI_BD_ADDR *pAddr);

/**************************************************************************************

  Link information

 **************************************************************************************/

/** These parameters are defined in H:1.4.8 of the Bluetooth specification, except for numSupportedIacs,
  which is described in H:1.4.7.45. The Device Manager provides a helper function that simply collects
  all of these parameters into a convenient structure.
  */
typedef struct {
    OI_HCI_CONNECTION_HANDLE handle;         /**< HCI connection handle */
    OI_BD_ADDR  bdAddr;                      /**< Bluetooth device address of remote device */
    OI_UINT8    role;                        /**< role (master versus slave) */
    OI_UINT16   failedContactCounter;        /**< failed contact counter */
    OI_UINT8    linkQuality;                 /**< link quality */
    OI_UINT8    rssi;                        /**< received signal strength indication (RSSI) */
    OI_UINT16   automaticflushTimeout;       /**< automatic flush timeout */
    OI_UINT8    currentTransmitPowerLevel;   /**< current transmit power level */
    OI_UINT8    maximumTransmitPowerLevel;   /**< maximum transmit power level */
    OI_UINT16   linkSupervisionTimeout;      /**< link supervision timeout */
    OI_UINT16   linkPolicy;
} OI_DEVMGR_LINK_INFO;


/**
  callback from OI_DEVMGR_ReadLinkInfo().

  @param status    result of request to read link information.


  @param handle    connection handle for link of interest

  @param pParms    pointer to caller's structure which contains the link information.
                   If status is other than OI_OK, contents of the structure are undefined.
*/
typedef void (*OI_DEVMGR_READ_LINK_INFO_CB)(OI_STATUS                status,
                                            OI_DEVMGR_LINK_INFO      *pInfo);

/**
  Informational parameters will be read and placed into the caller's structure.
  The callback function will be invoked when all parameters have been read.
  The callback function is called with the same parameters structure pointer passed to the this function.

  @param cb         callback function to pass the link information back to the application once it has been read.

  @param handle     connection handle for link of interest

  @param pInfo      pointer to a structure that will be filled with the link information
  */
OI_STATUS OI_DEVMGR_ReadLinkInfo(OI_DEVMGR_READ_LINK_INFO_CB cb,
                                 OI_HCI_CONNECTION_HANDLE    handle,
                                 OI_DEVMGR_LINK_INFO         *pInfo);


/**
    Get master/slave role on the indicated connection. This role is returned from the Device
    Manager's internal data structures; the role is not actually read from the device with
    HCI commands.

    @param pBdaddr   address for link of interest

    @return     TRUE if local device is Master, FALSE otherwise
                If connection does not exist, returns FALSE.
*/
OI_BOOL OI_DEVMGR_LocalDeviceIsMaster(OI_BD_ADDR *pBdaddr);
/**************************************************************************************

  remote device informational parameters

 **************************************************************************************/

/**
  This callback function type passes the results of a request to read remote supported features
  back to the calling function.

  @param status              status of request to read supported features of the remote device

  @param pBdaddr             pointer to the remote device's Bluetooth device address

  @param pLmpFeatures        pointer to a structure containing features of the remote device
  */
typedef void (*OI_DEVMGR_REMOTE_SUPPORTED_FEATURES_CB)(OI_STATUS     status,
                                                OI_BD_ADDR          *pBdaddr,
                                                OI_HCI_LMP_FEATURES *pLmpFeatures);

/**
  This function reads a remote device's supported features.

  @param cb                  callback function to pass supported features up to the application once they are read

  @param pBdaddr             pointer to the remote device's Bluetooth device address

*/
OI_STATUS OI_DEVMGR_ReadRemoteSupportedFeatures(OI_DEVMGR_REMOTE_SUPPORTED_FEATURES_CB cb,
                                                OI_BD_ADDR                            *pBdaddr);

/**
  This callback function type passes the results of a request to read remote extended features
  back to the calling function.

  @param status              status of request to read extedned features of the remote device

  @param pBdaddr             pointer to the remote device's Bluetooth device address

  @param pageNumber          Page number of features returned. 0 == same feature returned in OI_DEVMGR_REMOTE_SUPPORTED_FEATURES_CB

  @param maxPageNumber       Maximum page number supported by remote device

  @param pLmpFeatures        pointer to a structure containing features of the remote device
  */
typedef void (*OI_DEVMGR_REMOTE_EXTENDED_FEATURES_CB)(OI_STATUS            status,
                                                      OI_BD_ADDR          *pBdaddr,
                                                      OI_UINT8             pageNumber,
                                                      OI_UINT8             maxPageNumber,
                                                      OI_HCI_LMP_FEATURES *pLmpFeatures);

/**
  This function reads a remote device's supported features.

  @param cb                  callback function to pass extended features up to the application once they are read

  @param pBdaddr             pointer to the remote device's Bluetooth device address

  @param pageNumber          Page number of features requested. 0 == same feature returned by OI_DEVMGR_ReadRemoteSupportedFeatures

*/
OI_STATUS OI_DEVMGR_ReadRemoteExtendedFeatures(OI_DEVMGR_REMOTE_EXTENDED_FEATURES_CB cb,
                                               OI_BD_ADDR                           *pBdaddr,
                                               OI_UINT8                              pageNumber);

/**
  This callback function type passes the results of a request to read remote device Bluetooth hardware version
  back to the calling function.

  @param status              status of request to read supported remote device hardware version

  @param pBdaddr             pointer to the remote device's Bluetooth device address

  @param LMPVersion          remote device Link Manager Protocol version

  @param mfgrName            remote device manufacturer name

  @param LMPSubVersion       remote device Link Manager Protocol subversion

*/
typedef void (*OI_DEVMGR_REMOTE_VERSION_INFORMATION_CB)(OI_STATUS status,
                                                 OI_BD_ADDR *pBdaddr,
                                                 OI_UINT8   LMPVersion,
                                                 OI_UINT16  mfgrName,
                                                 OI_UINT16  LMPSubVersion);

/**
  This function reads a remote device's Bluetooth hardware version.
  See H:1.5.2.12 of the Bluetooth specification for details of Bluetooth hardware version.

  @param cb                  callback function to pass the results of a request to read remote device Bluetooth hardware version
  back to the calling function

  @param pBdaddr             pointer to the remote device's Bluetooth device address

*/
OI_STATUS OI_DEVMGR_ReadRemoteVersionInformation(OI_DEVMGR_REMOTE_VERSION_INFORMATION_CB cb,
                                                 OI_BD_ADDR   *pBdaddr);

/**
  This callback function type passes the results of a request to read a remote device's name
  back to the calling function.

  @param status              status of request to read supported remote device name

  @param pBdaddr             pointer to the remote device's Bluetooth device address

  @param pRemoteName         pointer to the remote device's name

*/
typedef void (*OI_DEVMGR_REMOTE_NAME_REQUEST_CB)(OI_STATUS status,
                                          OI_BD_ADDR *pBdaddr,
                                          OI_UINT8 *pRemoteName);

/**
  This function reads the name of a remote device.

  @param cb                  callback function to pass the results of a request to read remote device's name
  back to the calling function

  @param pBdaddr             pointer to the Bluetooth device address of the remote device

*/
OI_STATUS OI_DEVMGR_RemoteNameRequest(OI_DEVMGR_REMOTE_NAME_REQUEST_CB cb,
                                      OI_BD_ADDR *pBdaddr);

/**
  This callback function type passes the results of a request to read a remote device's clock offset
  back to the calling function.

  @param status              status of request to read supported remote device clock offset

  @param pBdaddr             pointer to the remote device's Bluetooth device address

  @param clockOffset         clock offset

*/
typedef void (*OI_DEVMGR_READ_CLOCK_OFFSET_CB)(OI_STATUS status,
                                        OI_BD_ADDR *pBdaddr,
                                        OI_UINT16 clockOffset);

/**
  This function reads the clock offset of a remote device.

  @param cb                  callback function to pass the results of a request to read the remote device's
  clock offset back to the calling function

  @param pBdaddr             pointer to the remote device's Bluetooth device address

*/
OI_STATUS OI_DEVMGR_ReadClockOffset (OI_DEVMGR_READ_CLOCK_OFFSET_CB cb,
                                     OI_BD_ADDR *pBdaddr);

/**************************************************************************************

  HCI EVENT CALLBACKS

 **************************************************************************************/


/**
  callback function type for the reporting of Bt Logo Test events

  @param pCmdPacket    Pointer to buffer containing event
  @param cmdPacketLen  Byte Count

*/
typedef void (*OI_DEVMGR_EV_CB_BT_LOGO_TEST)(OI_UINT8 *pCmdPacket, OI_UINT8  cmdPacketLen);

/**
  This function registers a callback function for the reporting of Bt Logo Test events.
  See H:1.5.1 of the Bluetooth specification for details.
  @param cb    callback function to be registered
  */
OI_STATUS OI_DEVMGR_RegisterCB_BtLogo(OI_DEVMGR_EV_CB_BT_LOGO_TEST cb);

/**
  This function unregisters a callback function for the reporting of Bt Logo Test events.
  @param cb    callback function to be unregistered
  */
OI_STATUS OI_DEVMGR_UnRegisterCB_BtLogo(OI_DEVMGR_EV_CB_BT_LOGO_TEST cb);
/********************************************************/


/**
  callback function type for the reporting of command loopbacks

  @param pCmdPacket    Pointer to buffer containing command
  @param cmdPacketLen  Byte Count

*/
typedef void (*OI_DEVMGR_EV_CB_LOOPBACK_COMMAND)(OI_UINT8 *pCmdPacket, OI_UINT8  cmdPacketLen);

/**
  This function registers a callback function for the reporting of Command Loopbacks.
  See H:1.5.2.25 of the Bluetooth specification for details.
  @param cb    callback function to be registered
  */
OI_STATUS OI_DEVMGR_RegisterCB_Loopback(OI_DEVMGR_EV_CB_LOOPBACK_COMMAND cb);

/**
  This function unregisters a callback function for the reporting of Command Loopbacks.
  @param cb    callback function to be unregistered
  */
OI_STATUS OI_DEVMGR_UnRegisterCB_Loopback(OI_DEVMGR_EV_CB_LOOPBACK_COMMAND cb);
/********************************************************/


/**
  callback function type for the reporting of Vendor Debug events

  @param pEvtPacket    Pointer to buffer containing event
  @param evtPacketLen  Byte Count

*/
typedef void (*OI_DEVMGR_EV_CB_VENDOR_EVENT)(OI_UINT8 *pEvtPacket, OI_UINT16  evtPacketLen);

/**
  This function registers a callback function for the reporting of Vendor Debug Events.
  See H:1.5.1 of the Bluetooth specification for details.
  @param cb    callback function to be registered
  */
OI_STATUS OI_DEVMGR_RegisterCB_Vendor(OI_DEVMGR_EV_CB_VENDOR_EVENT cb);

/**
  This function unregisters a callback function for the reporting of Vendor Debug Events.
  @param cb    callback function to be unregistered
  */
OI_STATUS OI_DEVMGR_UnRegisterCB_Vendor(OI_DEVMGR_EV_CB_VENDOR_EVENT cb);
/********************************************************/


/**
  callback function type for the reporting of hardware errors

  @param hardwareCode    hardware error code

*/
typedef void (*OI_DEVMGR_EV_CB_HARDWARE_ERROR)(OI_UINT8 hardwareCode);

/**
  This function registers a callback function for the reporting of hardware errors.
  See H:1.5.2.16 of the Bluetooth specification for details.
  @param cb    callback function to be registered
  */
OI_STATUS OI_DEVMGR_RegisterCB_HardwareError(OI_DEVMGR_EV_CB_HARDWARE_ERROR cb);

/**
  This function unregisters a callback function for the reporting of hardware errors.
  @param cb    callback function to be unregistered
  */
OI_STATUS OI_DEVMGR_UnRegisterCB_HardwareError(OI_DEVMGR_EV_CB_HARDWARE_ERROR cb);
/********************************************************/

/**
  @deprecated      This callback and the associated register/unregister functions are all deprecated.
                   Applications may discover the local device role on a connection with the
                   function OI_DEVMGR_LocalDeviceIsMaster();
  */
typedef void (*OI_DEVMGR_EV_CB_ROLE_CHANGE)(OI_STATUS status,
                                     OI_BD_ADDR *pBdaddr,
                                     OI_UINT8 newRole);

/********************************************************/

/**
  callback function type for the reporting of a mode change (changes between active, hold, sniff, and park)

  @param status             status

  @param pBdaddr             pointer to the remote device's Bluetooth device address

  @param newMode            new mode after mode change

  @param interval           time interval as relevant to each mode

*/
typedef void (*OI_DEVMGR_EV_CB_MODE_CHANGE)(OI_STATUS   status,
                                     OI_BD_ADDR         *pBdaddr,
                                     OI_UINT8           newMode,
                                     OI_UINT16          interval);

/**
  This function registers a callback function for the reporting of a mode change (changes between active, hold, sniff, and park).
  See H:1.5.2.20 of the Bluetooth specification for details.
  @param cb    callback function to be registered
  */

OI_STATUS OI_DEVMGR_RegisterCB_ModeChange(OI_DEVMGR_EV_CB_MODE_CHANGE cb);
/**
  This function unregisters a callback function for the reporting of a mode change.
  @param cb    callback function to be unregistered
  */
OI_STATUS OI_DEVMGR_UnRegisterCB_ModeChange(OI_DEVMGR_EV_CB_MODE_CHANGE cb);

/********************************************************/

/**
  callback function type for the reporting of a change in the LMP
  parameter that determines the maximum number of slots per transmitted baseband packet

  @param pBdaddr             pointer to the remote device's Bluetooth device address

  @param LmpMaxSlots        maximum number of slots per transmitted baseband packet

*/
typedef void (*OI_DEVMGR_EV_CB_MAX_SLOTS_CHANGE)(OI_BD_ADDR         *pBdaddr,
                                          OI_UINT8 LmpMaxSlots);

/**
  This function registers a callback function for the reporting of a change in the LMP
  parameter that determines the maximum number of slots per transmitted baseband packet.
  See H:1.5.2.27 of the Bluetooth specification for details.
  @param cb    callback function to be registered
  */
OI_STATUS OI_DEVMGR_RegisterCB_MaxSlotsChange(OI_DEVMGR_EV_CB_MAX_SLOTS_CHANGE cb);
/**
  This function unregisters a callback function for the reporting of a change in the LMP
  parameter that determines the maximum number of slots per transmitted baseband packet.
  @param cb    callback function to be unregistered
  */
OI_STATUS OI_DEVMGR_UnRegisterCB_MaxSlotsChange(OI_DEVMGR_EV_CB_MAX_SLOTS_CHANGE cb);

/********************************************************/

/**
  callback function type for the reporting of a change by the Link Manager
  in the type of packets that can be used on a particular connection

  @param status             status

  @param pBdaddr            pointer to the remote device's Bluetooth device address

  @param packetType         type of packets that can be used on a particular connection

*/
typedef void (*OI_DEVMGR_EV_CB_CONNECTION_PACKET_TYPE_CHANGED)(OI_STATUS status,
                                                        OI_BD_ADDR  *pBdaddr,
                                                        OI_UINT16   packetType);

/**
  This function registers a callback function for the reporting of a change by the Link Manager
  in the type of packets that can be used on a particular connection.
  See H:1.5.2.29 of the Bluetooth specification for details.
  @param cb    callback function to be registered
  */
OI_STATUS OI_DEVMGR_RegisterCB_ConnectionPacketTypeChanged(OI_DEVMGR_EV_CB_CONNECTION_PACKET_TYPE_CHANGED cb);

/**
  This function unregisters a callback function for the reporting of a change by the Link Manager
  in the type of packets that can be used on a particular connection.
  @param cb    callback function to be unregistered
  */
OI_STATUS OI_DEVMGR_UnRegisterCB_ConnectionPacketTypeChanged(OI_DEVMGR_EV_CB_CONNECTION_PACKET_TYPE_CHANGED cb);

/********************************************************/

/**
  callback function type for the reporting of change in page scan mode

  @param pBdaddr        pointer to Bluetooth device address of remote device

  @param pageScanMode   new page scan mode after change
  */
typedef void (*OI_DEVMGR_EV_CB_PAGE_SCAN_MODE_CHANGE)(OI_BD_ADDR *pBdaddr,
                                               OI_UINT8 pageScanMode);

/**
  This function registers a callback function for the reporting of change in page scan mode.
  See H:1.5.2.31 of the Bluetooth specification for details.
  @param cb    callback function to be registered
  */
OI_STATUS OI_DEVMGR_RegisterCB_ScanModeChange(OI_DEVMGR_EV_CB_PAGE_SCAN_MODE_CHANGE cb);

/**
  This function unregisters a callback function for the reporting of change in page scan mode.
  @param cb    callback function to be unregistered
  */
OI_STATUS OI_DEVMGR_UnRegisterCB_ScanModeChange(OI_DEVMGR_EV_CB_PAGE_SCAN_MODE_CHANGE cb);

/********************************************************/

/**
  callback function for the reporting of change in page scan repetition mode

  @param pBdaddr                  pointer to Bluetooth device address of remote device

  @param pageScanRepetitionMode   new page scan repetition mode after change
  */
typedef void (*OI_DEVMGR_EV_CB_PAGE_SCAN_REPETITION_MODE_CHANGE)(OI_BD_ADDR *pBdaddr,
                                                          OI_UINT8 pageScanRepetitionMode);

/**
  This function registers a callback function for the reporting of change in page scan repetition mode.
  See H:1.5.2.32 of the Bluetooth specification for details.
  @param cb    callback function to be registered
  */
OI_STATUS OI_DEVMGR_RegisterCB_PageScanRepetitionModeChange(OI_DEVMGR_EV_CB_PAGE_SCAN_REPETITION_MODE_CHANGE cb);
/**
  This function unregisters a callback function for the reporting of change in page scan repetition mode.
  @param cb    callback function to be unregistered
  */
OI_STATUS OI_DEVMGR_UnRegisterCB_PageScanRepetitionModeChange(OI_DEVMGR_EV_CB_PAGE_SCAN_REPETITION_MODE_CHANGE cb);

/**
  This function forces the ACL connection disconnect to the indicated device
  @param addr    BDAddr of the remote device
  */
OI_STATUS OI_DEVMGR_ForceDisconnect( OI_BD_ADDR *addr );

/*****************************************************************************/
#endif /* _OI_DEVMGR_H */
