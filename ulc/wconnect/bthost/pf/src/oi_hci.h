#ifndef _OI_HCI_H
#define _OI_HCI_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_hci.h#1 $ 

$Log: $

===========================================================================*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

/** 
@file  
    This file provides the HCI (Host Controller Interface) command API.

    Not all HCI commands are directly accessible through this API.
*/


#include "oi_status.h"
#include "oi_bt_spec.h"

/** This structure contains the inquiry result information from a remote Bluetooth device. 
    The fields in this structure are as received in the HCI inquiry result event packet as
    defined in section H:1.5.2.2 of the Bluetooth specification. A OI_DEVMGR_INQUIRY_RESULT_CB callback
    function returns a pointer to this type of structure. Refer to the Device Manager @ref device_inquiry
    API for details.
 */
typedef struct  {
    OI_BD_ADDR bd_addr;                /**< Bluetooth device address of the remote device */
    OI_UINT8 pageScanRepetitionMode;   /**< page scan repetition mode for the remote device */
    OI_UINT8 pageScanPeriodMode;       /**< page scan period mode for the remote device */
    OI_UINT8 pageScanMode;             /**< page scan mode for the remote device */
    OI_UINT32 classOfDevice;           /**< device class of the remote device */
    OI_UINT16 clockOffset;             /**< clock offset for the remote device */
} OI_HCI_INQUIRY_RESULT_REC;

/** A structure of this type constitutes a field of a OI_DEVMGR_LOCAL_INFO_PARMS structure, which is used
    to return the results of a call to the function OI_DEVMGR_ReadInformationalParams(). The fields in this
    structure are as defined in section H:1.4.8.1 of the Bluetooth specification. This information is 
    available via the OI_DEVMGR_ReadInformationalParams() Device Manager function. */
typedef struct  {
    OI_UINT8 hciVersion;               /**<  HCI version of local device: 0x00 for v1.0B, 0x01 for v1.1 */
    OI_UINT16 hciRevision;             /**<  HCI revision in Bluetooth hardware of the local device */
    OI_UINT8 lmpVersion;               /**<  LMP version for local device */
    OI_UINT16 manufacturerName;        /**<  name of manufacturer of local device */
    OI_UINT16 lmpSubversion;           /**<  LMP subversion for local device */
} OI_HCI_LOCAL_VERSION_INFORMATION;

#define OI_HCI_LMP_FEATURES_SIZE   8 /**<  The LMP supported feature list is 8 bytes long. */

/** This type of structure is used to return a list of supported LMP features to the application 
    as the result of a call to the OI_DEVMGR_ReadRemoteSupportedFeatures() Device Manager function. 
    A OI_DEVMGR_REMOTE_SUPPORTED_FEATURES_CB callback function returns a pointer to this type structure.
    command, which returns a bitmask list of supported features. 

    The features are contained in an array of bytes representing a bitmask list. The format and semantics
    of this list are described in H:1.4.8.2 and C:5.1.1 of the Bluetooth specification. 
    The ordering of the array is such that features[0] contains feature byte 0.
*/
typedef struct  {
    OI_UINT8 features[OI_HCI_LMP_FEATURES_SIZE];      /**< array of bytes representing bitmask list of supported LMP features */
} OI_HCI_LMP_FEATURES;

/*************************************************************/
/** @name Command completion callback functions
 
    These type definitions are for callback functions that are called when a command has completed
    with the command completion (OI_DHCI_CMND_CMPLT_EVT) HCI event.

    All command completion callback functions get passed a status code as their first parameter. 
    A status code of OI_OK means that the operation was successful; any other value indicates an error.
    All HCI error codes are mapped to @ref OI_STATUS values.
*/

/** @{ */

/**
    common HCI command completion callback function for those commands that result in the return of 
    a status code only 

    @param status    status code
*/
typedef void(*OI_HCI_CMD_COMPLETE_CB)(OI_STATUS status); 

/**
    common HCI command completion callback function that gets passed a connection handle parameter

    @param status                status code
    @param connectionHandle      connection handle
 */
typedef void(*OI_HCI_CMD_COMPLETE_CONNECTION_CB)(OI_STATUS status,
                                          OI_HCI_CONNECTION_HANDLE connectionHandle); 

/**
    HCI command completion callback function that passes the PIN type back, requested by a call to OI_HCI_ReadPINType() 

    @param status                status code
    @param pinType               PIN type: 0x00 for variable PIN, 0x01 for fixed PIN
*/
typedef void(*OI_HCI_READ_PIN_TYPE_CB)(OI_STATUS status,              
                                OI_UINT8 pinType);           
                             
/**
    HCI command completion callback function for OI_HCI_ReadLocalName() 

    @param status                status code
    @param pName                 pointer to the user-friendly name of the local device, 
                                 a UTF-8-encoded 248-byte name as described in section H:1.4.7.12
                                 of the Bluetooth specification.  The name may be slightly modified in
                                 the special case where the name is exactly 248 bytes long.  In this
                                 case, the last byte of the name will be replaced by a null terminator.
*/
typedef void(*OI_HCI_READ_LOCAL_NAME_CB)(OI_STATUS status,              
                                  OI_BYTE *pName);            

/**
    HCI command completion callback for OI_HCI_ReadRSSI() 

    @param status                status code
    @param connectionHandle      connection handle
    @param rssi                  Received Signal Strength Indication(RSSI) in dBm, a value -128 through +127, 
                                 as defined in section H:1.4.9.4 of the Bluetooth specification
*/
typedef void(*OI_HCI_READ_SSI_CB)(OI_STATUS status,              
                           OI_HCI_CONNECTION_HANDLE connectionHandle,    
                           OI_UINT8 rssi);              

/**
    HCI command completion callback for OI_HCI_ReadClassofDevice() 

    @param status                status code
    @param classOfDevice         Class of device as defined in the Bluetooth specification
*/
typedef void (*READ_CLASS_OF_DEVICE_CB)(
                    OI_STATUS               status,
                    OI_UINT32               classOfDevice) ;


/** @} */

/*************************************************************

    HCI Command interface

*************************************************************/

/**
    
    This function changes the packet types that are to be used with the specified connection. 
    This function can be used for SCO or ACL connections. The packet type parameter is as defined
    in section H:1.4.5.14 of the Bluetooth specification. Feedback from this command can be 
    obtained by registering a callback function by means of the Device Manager function OI_DEVMGR_RegisterCB_ConnectionPacketTypeChanged(). 

    @param connectionHandle      connection handle
    @param packetType            Packet type(s) that can be used on  the connection are indicated by a 
                                 bitwise OR of the following:
                                 - 0x0008 ACL DM1
                                 - 0x0010       ACL DH1
                                 - 0x0400       ACL DM3
                                 - 0x0800       ACL DH3
                                 - 0x4000       ACL DM5
                                 - 0x8000       ACL DH5
                                 - 0x0020       SCO HV1
                                 - 0x0040       SCO HV3
                                 - 0x0080       SCO HV5
*/
OI_STATUS OI_HCI_ChangeConnectionPacketType(OI_HCI_CONNECTION_HANDLE connectionHandle,   
                                            OI_UINT16 packetType);       
                                            
/**
    
    This function sets an event filter as described in section H:1.4.7.3 of the Bluetooth specification.
    Event filters allow higher program layers to configure the HCI layer to not send certain events that
    are not of interest to the higher layers. Not all events have associated event filters. 

    It is possible to use an event filter to configure the local device to automatically accept connections.
    Using an event filter in this way is discouraged, since this creates the possibility of a conflict with
    the connection management features of the Device Manager.  The device manager may not be able to properly
    manage master/slave roles if connections are automatically accepted.  If event filters are used, it is up
    to the application programmer to ensure that there is no conflict between the role switching behavior 
    in the Device Manager and the behavior indicated by the event filter settings.

    @param filterType              as described in section H:1.4.7.3 of the Bluetooth specification 
    @param filterConditionType     as described in section H:1.4.7.3 of the Bluetooth specification
    @param pCondition              Rather than create a union of all the possible permutations of the 
                                   condition parameter, the filter condition is simply defined as an array of bytes.
                                   The caller is responsible for building the condition parameters into
                                   an array of bytes.
    @param conditionLength         number of bytes in condition array.
    @param cmdCompleteCB           callback function that will be called upon the command completion event
*/
OI_STATUS OI_HCI_SetEventFilter(OI_HCI_CMD_COMPLETE_CB cmdCompleteCB,             
                                OI_UINT8 filterType,                
                                OI_UINT8 filterConditionType,       
                                OI_UINT8 conditionLength,           
                                OI_UINT8 *pCondition);             

/**
    
    This function requests a read of the type of PIN(personal identification number) used by the 
    local device. Upon the command completion event, the callback function will be invoked
    to return the result. This command operates as described in section H:1.4.7.5 of the Bluetooth specification.

    @param cmdCompleteCB           callback function that will be called upon the command completion event
*/
OI_STATUS OI_HCI_ReadPINType(OI_HCI_READ_PIN_TYPE_CB cmdCompleteCB);

/**
    
    This function requests a write of the type of PIN(personal identification number) used by the 
    local device. Upon the command completion event, the callback function will be invoked. 
    This command operates as described in section H:1.4.7.6 of the Bluetooth specification.

    @param cmdCompleteCB           callback function that will be called upon the command completion event
    @param pinType                 PIN type: 0x00 for variable PIN, 0x01 for fixed PIN
*/
OI_STATUS OI_HCI_WritePINType(OI_HCI_CMD_COMPLETE_CB cmdCompleteCB,             
                              OI_UINT8 pinType);                 

/**
    
    This function requests the creation of a new unit key. If the device uses unit keys, a random seed
    will be generated, from which the new unit key will be derived. The new unit key will be used for
    new connections, but the old unit key will be used for current connections. If the device only uses
    combination keys, this function has no effect. Upon the command completion event, the callback
    function will be invoked and the new unit key can be read using the Security Manager function
    OI_SECMGR_GetAllStoredLinkKeys().
    This command operates as described in section H:1.4.7.7 of the Bluetooth specification.

    @param cmdCompleteCB           callback function that will be called upon the command completion event
*/
OI_STATUS OI_HCI_CreateNewUnitKey(OI_HCI_CMD_COMPLETE_CB cmdCompleteCB);


/**

    This function is used to request a change in the user-friendly descriptive name of the 
    local device. The name is a null-terminated string of length up to 248 bytes, encoded with UTF-8.
    Upon the command completion event, the callback function will be invoked.
    This command operates as described in section H:1.4.7.11 of the Bluetooth specification.

    @param pName            pointer to a variable-length(up to 248 bytes) null-terminated UTF-8 string
    @param cmdCompleteCB    callback function that will be called upon the command completion event
*/
OI_STATUS OI_HCI_ChangeLocalName(OI_HCI_CMD_COMPLETE_CB cmdCompleteCB,             
                                 OI_UINT8 *pName);                  

/**
    
    This function is used to read the user-friendly descriptive name of the local device. The name
    is a null-terminated string of length up to 248 bytes, encoded with UTF-8. Upon the command
    completion event, the callback function will be invoked to return the result.
    This command operates as described in section H:1.4.7.12 of the Bluetooth specification.

    @param cmdCompleteCB           callback function that will be called upon the command completion event
*/
OI_STATUS OI_HCI_ReadLocalName(OI_HCI_READ_LOCAL_NAME_CB cmdCompleteCB);           
             
/**
    This function is used to write the page-scan-activity configuration parameter values.
    Upon the command completion event, the callback function will be invoked.
    This command operates as described in section H:1.4.7.20 of the Bluetooth specification.

    @param cmdCompleteCB        callback function that will be called upon the command completion event
    @param pageScanInterval     as specified in section H:1.4.7.20 of the Bluetooth specification
    @param pageScanWindow       as specified in section H:1.4.7.20 of the Bluetooth specification
*/
OI_STATUS OI_HCI_WritePageScanActivity(OI_HCI_CMD_COMPLETE_CB    cmdCompleteCB,
                                       OI_UINT16                 pageScanInterval,
                                       OI_UINT16                 pageScanWindow) ;
                                          
/**
    This function is used to write the inquiry-scan-activity configuration parameter values.
    Upon the command completion event, the callback function will be invoked.
    This command operates as described in section H:1.4.7.22 of the Bluetooth specification.

    @param cmdCompleteCB        callback function that will be called upon the command completion event
    @param inquiryScanInterval  as specified in section H:1.4.7.22 of the Bluetooth specification
    @param inquiryScanWindow    as specified in section H:1.4.7.22 of the Bluetooth specification
*/
OI_STATUS OI_HCI_WriteInquiryScanActivity (OI_HCI_CMD_COMPLETE_CB  cmdCompleteCB,
                                           OI_UINT16               inquiryScanInterval,
                                           OI_UINT16               inquiryScanWindow) ;
                                          
/**
    This function is used to read the class of device parameter setting.
    Upon the command completion event, the callback function will be invoked.
    This command operates as described in section H:1.4.7.27 of the Bluetooth specification.

    @param cmdCompleteCB        callback function that will be called upon the command completion event
*/
OI_STATUS    OI_HCI_ReadClassofDevice(READ_CLASS_OF_DEVICE_CB cmdCompleteCB) ;

/**
    This function requests a reset of the failed contact counter, a counter of the number of consecutive
    incidents of the currently transmitted L2CAP being flushed when the master or slave failed to respond
    after the flush timeout had expired.
    This command operates as described in section H:1.4.9.2 of the Bluetooth specification.
    Upon the command completion event, the callback function will be invoked.

    @param cmdCompleteCB           callback function that will be called upon the command completion event
    @param connectionHandle        connection handle for the connection for which the failed contact counter is to be reset
*/
OI_STATUS OI_HCI_ResetFailedContactCounter(OI_HCI_CMD_COMPLETE_CONNECTION_CB cmdCompleteCB,         
                                           OI_HCI_CONNECTION_HANDLE connectionHandle);        

/**
    This function requests a read of the received signal strength indication(RSSI), which 
    is given as a value from -128 through +127, with units of dBm.
    This command operates as described in section H:1.4.9.4 of the Bluetooth specification.
    Upon the command completion event, the callback function will be invoked to return the result.

    @param cmdCompleteCB           callback function that will be called upon the command completion event
    @param connectionHandle        connection handle for the connection for which the RSSI is to be read
*/
OI_STATUS OI_HCI_ReadRSSI(OI_HCI_READ_SSI_CB cmdCompleteCB,             
                          OI_HCI_CONNECTION_HANDLE connectionHandle);        


/**
    This function passes a user-formatted vendor-specific or vendor-private command
    to the Bluetooth hardware module. The only validity check made is that the opcode
    passed must have an OGF(opcode group field, the upper 6 bits of the opcode) of 0x3F, 
    which is reserved for vendor-specific debug commands.
    An error of OI_HCI_INVALID_OPCODE_ERROR is returned for any opcode with an OGF that is not 0x3F.
    This command conforms to section H:1.4.4.1 of the Bluetooth specification.
    As the specification does not define the existence or non-existence of the command completion
    or command status return event from vendor hardware, none is expected. However, all
    event packets from the vendor hardware, including OI_DHCI_VENDOR_SPECIFIC_DEBUG_EVT,
    DHCI_CMD_CMPLT_EVT, and DHCI_CMD_STATUS_EVT that are received as a result of a valid
    OI_HCI_VendorCmd will be delivered to the user via the well-known user-supplied
    function OI_HciEventHandler_VendorDebugEvent(). This allows for the user to perform
    any handling or timing out of event packets they desire.

    @param opcode                  vendor-defined opcode, which must have an OGF(opcode group field) of 0x3F. 
                                   The OCF(opcode command field) is defined by the vendor.
    @param length                  number of 8-bit bytes contained in the data parameter
    @param data                    pointer to vendor-specific data starting with parameter 0, according to H:1.4.4.1
*/
OI_STATUS OI_HCI_VendorCmd(OI_UINT16 opcode,             
                           OI_UINT8 length,
                           OI_UINT8 *data);
                           
/**
    This function allows the local Bluetooth module to enter test mode via LMP test commands.                           
    This command operates as described in section H:1.4.10.3 of the Bluetooth specification.
    Upon the command completion event, the callback function will be invoked to return the result.
*/    
    
OI_STATUS OI_HCI_EnableDeviceUnderTestMode(OI_HCI_CMD_COMPLETE_CB cmdCompleteCB) ;


/*************************************************************

    special, well-known functions called directly from HCI

*************************************************************/

/** Bluetooth Logo Testing event handler function */
extern void OI_HciEventHandler_BtLogoTestEvent(OI_UINT8 *pEventPacket,
                                               OI_UINT8 eventPacketLen);

/** Event handler function for a vendor-specific debug event.

    @param pEventPacket     Pointer to the received HCI packet.  The pointer is pointing at the 
                            first byte in the packet, i.e. the event code.  The event code will 
                            be either the vendor-specific event (0xFF), Command complete or 
                            command status.

    @param eventPacketLen   Total number of bytes in the packet
 */
extern void OI_HciEventHandler_VendorDebugEvent(OI_UINT8 *pEventPacket,
                                                OI_UINT8 eventPacketLen);

/*****************************************************************************/
#endif  /* _OI_HCI_H */
