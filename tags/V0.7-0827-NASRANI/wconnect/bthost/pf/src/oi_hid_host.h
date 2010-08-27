#ifndef _OI_HID_HOST_H
#define _OI_HID_HOST_H
/**
 * @file  
 * This file provides the API for the host side of the Human Interface Device (HID) Profile.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/

#include "oi_common.h"
#include "oi_hid.h"

/** \addtogroup HID HID APIs */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif


typedef OI_UINT16 OI_HID_HOST_HANDLE;

/** Indicates an incoming connection request. The host should respond with
 * #OI_HIDHost_Accept.
 * @param addr  Address of the device requesting a connection
 * @param handle    A handle to refer to this connection in subsequent calls
 */
typedef void (*OI_HID_HOST_CONNECT_IND)(OI_BD_ADDR const *addr, OI_HID_HOST_HANDLE handle);

/** Called once a HID connection is complete, successfully or otherwise. A
 * connection attempt is not complete until this callback has been issued.
 * @param handle    The handle associated with this connection
 * @param status    OI_SUCCESS if the connection is established, otherwise an
 * error code.*/
typedef void (*OI_HID_HOST_CONNECT_CFM)(OI_HID_HOST_HANDLE cid, OI_STATUS status);

/** Called to indicate a HID connection is being dropped. This does not imply a
 * HID Virtual Cable Disconnect event.
 * @param handle    The connection being dropped
 * @param reason    The reason for the disconnection
 */
typedef void (*OI_HID_HOST_DISCONNECT_IND)(OI_HID_HOST_HANDLE cid, OI_STATUS status);



/** Called in response to a GET_REPORT request.
 * @param handle The handle associated with the connection
 * @param handshake #OI_HID_HANDSHAKE_SUCCESSFUL or a failure code if the operation
 * failed.
 * @param reportType The requested report type
 * @param data  A pointer to the report data.
 * @param length The length of the report data
 * @param first TRUE if this is the first (and possibly final as well) packet
 * associated with this response
 * @param final TRUE if this is the final packet associated with this response
 */
typedef void (*OI_HID_HOST_GET_REPORT_CFM)(OI_HID_HOST_HANDLE handle,
                                           OI_BYTE handshake,
                                           OI_BYTE reportType,
                                           OI_BYTE const *data,
                                           OI_UINT16 length,
                                           OI_BOOL first,
                                           OI_BOOL final);

/** Called in response to a SET_REPORT request.
 * @param handle The handle associated with the connection
 * @param handshake #OI_HID_HANDSHAKE_SUCCESSFUL or a failure code if the operation
 * failed.
 * @param data  A pointer to the report data passed into the original request
 * @param length The length of the report data passed into the original  request
 * @param status An error response if there was an error in a lower stack
 * layer, OI_OK otherwise.
 */
typedef void (*OI_HID_HOST_SET_REPORT_CFM)(OI_HID_HOST_HANDLE handle,
                                           OI_BYTE handshake,
                                           OI_BYTE const *data,
                                           OI_UINT16 length,
                                           OI_STATUS status);

/** Called in response to a GET_PROTOCOL request.
 * @param handle The handle associated with the connection
 * @param protocol  The device's current protocol (if handshake indicates
 * success)
 * @param handshake #OI_HID_HANDSHAKE_SUCCESSFUL or a failure code if the operation
 * failed.
 * @param status An error response if there was an error in a lower stack
 * layer, OI_OK otherwise.
 */
typedef void (*OI_HID_HOST_GET_PROTOCOL_CFM)(OI_HID_HOST_HANDLE handle,
                                             OI_BYTE protocol,
                                             OI_BYTE handshake,
                                             OI_STATUS status);

/** Called in response to a GET_IDLE request.
 * @param handle The handle associated with the connection
 * @param idleRate  The device's idle reporting rate (if handshake indicates
 * success)
 * @param handshake #OI_HID_HANDSHAKE_SUCCESSFUL or a failure code if the operation
 * failed.
 * @param status An error response if there was an error in a lower stack
 * layer, OI_OK otherwise.
 */
typedef void (*OI_HID_HOST_GET_IDLE_CFM)(OI_HID_HOST_HANDLE handle,
                                         OI_BYTE idleRate,
                                         OI_BYTE handshake,
                                         OI_STATUS status);

/** Called in response to a SET_PROTOCOL or SET_IDLE request
 * @param handle The handle associated with the connection
 * @param handshake #OI_HID_HANDSHAKE_SUCCESSFUL or a failure code if the operation
 * failed.
 * @param status An error response if there was an error in a lower stack
 * layer, OI_OK otherwise.
 */
typedef void (*OI_HID_HOST_SET_CFM)(OI_HID_HOST_HANDLE handle,
                                    OI_BYTE handshake,
                                    OI_STATUS status);

/** Called to confirm the transmission of an output report on the interrupt
 * channel.
 * @param handle The handle associated with the connection
 * @param data  A pointer to the report data passed into the original request
 * @param length The length of the report data passed into the original  request
 * @param status An error response if there was an error in a lower stack
 * layer, OI_OK otherwise.
 */
typedef void (*OI_HID_HOST_SEND_OUTPUT_REPORT_CFM)(OI_HID_HOST_HANDLE handle,
                                                   OI_BYTE const *data,
                                                   OI_UINT16 length,
                                                   OI_STATUS status);


/** Called to indicate that the remote device has requested a virtual cable
 * unplug operation.
 * @param handle The handle associated with the device making the request
 */
typedef void (*OI_HID_HOST_VIRTUAL_CABLE_UNPLUGGED_IND)(OI_HID_HOST_HANDLE handle);


/** Called to indicate that the remote device has sent a report-protocol input
 * report over the interrupt channel. This may be set to NULL to ignore
 * report-mode reports if the host intends to support only boot-mode reports.
 * @param handle The handle associated with the device sending the request
 * @param data  A pointer to the report data. The host must not access this
 * pointer after retuning from the callback
 * @param data The length of the report data
 * @param first TRUE if this is the first (and possibly final as well) packet
 * associated with this response
 * @param final TRUE if this is the final packet associated with this response
 */
typedef void (*OI_HID_HOST_INPUT_REPORT_IND)(OI_HID_HOST_HANDLE handle,
                                             OI_BYTE const *data,
                                             OI_UINT16 length,
                                             OI_BOOL first,
                                             OI_BOOL final);

/** Called to indicate that the remote device has sent a boot-mode mouse report
 * over the interrupt channel. This callback will not be called unless the
 * device has successfully acknowledged a request to enter boot-protocol mode.
 * This callback may be NULL, in which case boot-mode mouse reports will be
 * ignored.
 * @param handle    The handle associated with the device sending the report
 * @param buttons   Bitfield indicating which mouse buttons are pressed.
 * @param deltaX    X-coordinate displacement
 * @param deltaY    Y-coordinate displacement
 */
typedef void (*OI_HID_HOST_BOOT_MOUSE_IND)(OI_HID_HOST_HANDLE handle,
                                           OI_BYTE buttons,
                                           OI_INT8 deltaX,
                                           OI_INT8 deltaY);

/** Called to indicate that the remote device has sent a boot-mode keyboard report
 * over the interrupt channel. This callback will not be called unless the
 * device has successfully acknowledged a request to enter boot-protocol mode.
 * This callback may be NULL, in which case boot-mode keyboard reports will be
 * ignored.
 * @param handle    The handle associated with the device sending the report
 * @param modifiers   Bitfield indicating which modifier keys are pressed.
 * @param keycodes  Array indicating keypress events which have occured. The
 * length of the array is specified by the constant #OI_HIDBOOT_KEYBOARD_KEYCODE_COUNT.
 */
typedef void (*OI_HID_HOST_BOOT_KEYBOARD_IND)(OI_HID_HOST_HANDLE handle,
                                              OI_BYTE modifiers,
                                              OI_BYTE const *keycodes);

typedef struct {
    OI_HID_HOST_CONNECT_IND connectInd;                     /**< incoming connect request callback (must be set) */
    OI_HID_HOST_CONNECT_CFM connectCfm;                     /**< connect confirmation callback (must be set) */
    OI_HID_HOST_DISCONNECT_IND disconnectInd;               /**< disconnect indication callback (must be set) */

    OI_HID_HOST_VIRTUAL_CABLE_UNPLUGGED_IND unpluggedInd;   /**< unplug indication callback (must be set) */

    OI_HID_HOST_GET_REPORT_CFM getReportCfm;                /**< get report confirmation callback (may be NULL) */
    OI_HID_HOST_SET_REPORT_CFM setReportCfm;                /**< set report confirmation callback (may be NULL) */

    OI_HID_HOST_GET_PROTOCOL_CFM getProtocolCfm;            /**< get protocol confirmation callback (may be NULL) */
    OI_HID_HOST_SET_CFM setProtocolCfm;                     /**< set protocol confirmation callback (may be NULL) */

    OI_HID_HOST_GET_IDLE_CFM getIdleCfm;                    /**< get idle confirmation callback (may be NULL) */
    OI_HID_HOST_SET_CFM setIdleCfm;                         /**< set idle confirmation callback (may be NULL) */

    OI_HID_HOST_INPUT_REPORT_IND inputReportInd;            /**< incoming input report callback (may be NULL) */
    OI_HID_HOST_SEND_OUTPUT_REPORT_CFM outputReportCfm;     /**< output report confirmation callback (may be NULL) */

    OI_HID_HOST_BOOT_KEYBOARD_IND bootKeyboardInd;          /**< incoming boot mode keyboard event callback (may be NULL) */
    OI_HID_HOST_BOOT_MOUSE_IND bootMouseInd;                /**< incoming boot mode mouse event callback (may be NULL) */
} OI_HID_HOST_CALLBACKS;

typedef struct {
    OI_UINT maxConnections;
} OI_HID_HOST_CONFIG;

/** Registers a HID Host. Only one host may be registered.
 * @param config    Host configuration information
 * @param cb        Host event callbacks
 * @return   OI_OK if successful, error code otherwise
 */
OI_STATUS OI_HIDHost_Register(OI_HID_HOST_CONFIG const *config, OI_HID_HOST_CALLBACKS const *cb);

OI_STATUS OI_HIDHost_Deregister(void);

/** Requests a connection to a remote device.
 * @param addr      Address of remote device
 * @param params    HIDP connection parameters. If default values are
 * acceptable, this may be NULL.
 * @param handle    Location of variable into which the handle for this
 * connection will be stored
 * @return   OI_OK if request could be sent, error code otherwise.
 */
#ifndef OI_CODE
OI_STATUS OI_HIDHost_Connect(OI_BD_ADDR *addr, OI_HID_CONNECTION_PARAMS const *params, OI_HID_HOST_HANDLE *handle);
#else
#error code not present
#endif

/** Accepts or refuses an incoming connection request.
 * @param handle    Handle associated with the connection
 * @param params    HIDP connection paramters to be used for this connection. If
 * the connection will be refused, or if default values are acceptable, this may
 * be NULL.
 * @param allow     TRUE to accept the connection, FALSE to reject it.
 * @return   OI_OK if the request was sent, error code otherwise.
 */
OI_STATUS OI_HIDHost_Accept(OI_HID_HOST_HANDLE handle, OI_HID_CONNECTION_PARAMS const *params, OI_BOOL allow);

/** Disconnects a HIDP connection. This does not send an Unplug Virtual Cable
 * event.
 * @param  handle Handle associated with the connection to be dropped
 * @return   OI_OK if the request was sent, error code otherwise.
 */
OI_STATUS OI_HIDHost_Disconnect(OI_HID_HOST_HANDLE handle);

/** Sends an #OI_HID_CONTROL operation to a remote device
 * @param handle The handle to the associated connection
 * @param controlOp The control command to send (see oi_hid_consts.h)
 * @return   OI_OK if the request was sent, error code otherwise.
 */
OI_STATUS OI_HIDHost_SendHidControl(OI_HID_HOST_HANDLE handle,
                                    OI_BYTE controlOp);

/** Sends a GET_REPORT operation to a remote device
 * @param handle The handle to the associated connection
 * @param reportType The type of report  (e.g. an input report). See
 * oi_hid_consts.h 
 * @param reportId  The ID of the report
 * being requested, or the constant #OI_HID_REPORT_ID_DEFAULT if no report ID was
 * specified by the device's report descriptors.
 * @param maxBytes  Instructs the device to truncate its report to be at most
 * maxBytes in length. The constant #OI_HID_REPORT_MAX_LENGTH may be specified to
 * disable truncation
 * @return   OI_OK if the request was sent, error code otherwise.
 */
OI_STATUS OI_HIDHost_GetReport(OI_HID_HOST_HANDLE handle,
                               OI_BYTE reportType,
                               OI_INT reportId,
                               OI_UINT16 maxBytes);

/** Sends a SET_REPORT operation to a remote device
 * @param handle    The handle to the associated connection
 * @param reportType    The type of report being sent (e.g. an output report).
 * See oi_hid_consts.h
 * @param data  A pointer to the report data. This must remain accessable to the
 * stack until the #OI_HID_HOST_SET_REPORT_CFM callback is called.
 * @param length Length of the report data
 * @return   OI_OK if the request was sent, error code otherwise.
 */
OI_STATUS OI_HIDHost_SetReport(OI_HID_HOST_HANDLE handle,
                               OI_BYTE reportType,
                               OI_BYTE const *data,
                               OI_UINT16 length);

/** Sends a GET_PROTOCOL request to a remote device
 * @param handle    The handle to the associated connection
 * @return   OI_OK if the request was sent, error code otherwise.
 */
OI_STATUS OI_HIDHost_GetProtocol(OI_HID_HOST_HANDLE handle);

/** Sends a SET_PROTOCOL request to a remote device
 * @param handle    The handle to the associated connection
 * @param protocol  The new protocol to use (see oi_hid_consts.h)
 * @return   OI_OK if the request was sent, error code otherwise.
 */
OI_STATUS OI_HIDHost_SetProtocol(OI_HID_HOST_HANDLE handle,
                                 OI_BYTE protocol);
/** Sends a GET_IDLE request to a remote device
 * @param handle    The handle to the associated connection
 * @return   OI_OK if the request was sent, error code otherwise.
 */
OI_STATUS OI_HIDHost_GetIdle(OI_HID_HOST_HANDLE handle);

/** Sends a SET_IDLE request to a remote device
 * @param handle    The handle to the associated connection
 * @param idleRate  The new idle reporting rate to use
 * @return   OI_OK if the request was sent, error code otherwise.
 */
OI_STATUS OI_HIDHost_SetIdle(OI_HID_HOST_HANDLE handle,
                             OI_BYTE idleRate);

/** Sends an interrupt-channel report to a remote device
 * @param handle    The handle to the associated connection
 * @param reportType    The type of report being sent (e.g. an output report).
 * See oi_hid_consts.h
 * @param data  A pointer to the report data. This must remain accessable to the
 * stack until the #OI_HID_HOST_SEND_OUTPUT_REPORT_CFM callback is called.
 * @param length Length of the report data
 * @return   OI_OK if the request was sent, error code otherwise.
 */
OI_STATUS OI_HIDHost_SendOutputReport(OI_HID_HOST_HANDLE handle,
                                      OI_BYTE reportType,
                                      OI_BYTE const *data,
                                      OI_UINT16 length);

/** Sets keyboard leds.  This function is a wrapper for OI_HIDHost_SendOutputReport, the
 *  semantics of OI_HIDHost_SendOutputReport apply to this function.
 * @param handle    The handle to the associated connection
 * @param leds      Value of led settings
 * @return   OI_OK if the request was sent, error code otherwise.
 */
OI_STATUS OI_HIDHost_SetKeyboardLEDs(OI_HID_HOST_HANDLE handle,
                                     OI_BYTE leds);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* _OI_HID_HOST_H */

