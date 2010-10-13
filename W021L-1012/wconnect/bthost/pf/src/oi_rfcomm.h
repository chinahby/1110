#ifndef _OI_RFCOMM_H
#define _OI_RFCOMM_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_rfcomm.h#1 $ 

$Log: $

===========================================================================*/
/**
 * @file  
 * 
 * This file provides the RFCOMM application programming interface (API).
 *
 * See the @ref RFCOMM_docpage section of the BLUEmagic 3.0 SDK documentation
 * for more information.
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/

#include "oi_common.h"
#include "oi_bt_stack_config.h"
#include "oi_connect_policy.h"

/* RFCOMM frame size constants */
#define OI_RFCOMM_FRAMESIZE_DEFAULT        127   /**< default RFCOMM frame size in bytes */
#define OI_RFCOMM_FRAMESIZE_MIN            23    /**< minimum RFCOMM frame size in bytes */
#define OI_RFCOMM_FRAMESIZE_MAX            32767 /**< maximum RFCOMM frame size in bytes */

/** type definition for an RFCOMM link handle, by which RFCOMM links are referenced */
typedef OI_UINT16 OI_RFCOMM_LINK_HANDLE;

/** 
 * type definition for a structure to carry serial port parameters to be 
 * passed to the OI_RFCOMM_SetPortParams() function
 */ 
typedef struct {

    /** The bits set in this mask indicate which parameters are to be set; bits
     * set to zero indicate that the parameters in this structure associated
     * with those bits are to be ignored. For example, a mask of 0x0001 in a
     * call to OI_RFCOMM_SetPortParams() will result in the bit rate for the
     * port being set to baudRate and every parameter other than baudRate being
     * ignored.  Instead of using 0x0001 as a mask, the more human-readable name
     * OI_RFCOMM_MASK_BITRATE can be used. Default is OI_RFCOMM_MASK_ALL. */
    OI_INT16    mask;

    /** bit rate; default: OI_RFCOMM_BAUDRATE_9600 */
    OI_BYTE    baudRate;

    /** number of data bits; default: OI_RFCOMM_DATABIT_8 */
    OI_BYTE    dataBits;

    /** number of stop bits; default: OI_RFCOMM_STOPBIT_1 */
    OI_BYTE    stopBits;

    /** parity; default: OI_RFCOMM_NO_PARITY */
    OI_BYTE    parity;

    /** type of parity; default: OI_RFCOMM_PARITY_ODD */
    OI_BYTE    parityType;

    /** type of flow control; default: OI_RFCOMM_FLOW_NONE */
    OI_BYTE    flowControlType;

    /** character to use for XON; default: 0x11 (DC1) */
    OI_BYTE    XON_Char;

    /** character to use for XOFF; default: 0x13 (DC3) */
    OI_BYTE    XOFF_Char;

} OI_RFCOMM_PORTPARAM;

/* OI_RFCOMM_PORTPARAM constant definitions */

/** 
 * @name Mask bits 
 * These values are used for setting the mask parameter in OI_RFCOMM_PORTPARAM.
 */
/** @{ */
#define OI_RFCOMM_MASK_BITRATE     BIT0        /**< mask bit definition for setting the bit rate parameter */
#define OI_RFCOMM_MASK_DATA_BITS   BIT1        /**< mask bit definition for setting the number of data bits parameter */
#define OI_RFCOMM_MASK_STOP_BITS   BIT2        /**< mask bit definition for setting the number of stop bits parameter  */
#define OI_RFCOMM_MASK_PARITY      BIT3        /**< mask bit definition for setting the parity parameter  */
#define OI_RFCOMM_MASK_PARITYTYPE  BIT4        /**< mask bit definition for setting the parity type parameter  */
#define OI_RFCOMM_MASK_XON_CHAR    BIT5        /**< mask bit definition for setting the XON character parameter (Bluetooth version 1.1 section F:1.6) */
#define OI_RFCOMM_MASK_XOFF_CHAR   BIT6        /**< mask bit definition for setting the XOFF character parameter (Bluetooth version 1.1 section F:1.6) */
//          bit 7: reserved
#define OI_RFCOMM_MASK_XON_INPUT   BIT8        /**< mask bit definition for setting the XON/XOFF flow control on input (Bluetooth version 1.1 section F:1.6) */
#define OI_RFCOMM_MASK_XON_OUTPUT  BIT9        /**< mask bit definition for setting the XON/XOFF flow control on output (Bluetooth version 1.1 section F:1.6) */
#define OI_RFCOMM_MASK_RTR_INPUT   BIT10       /**< mask bit definition for setting the RTR/RTS flow control on input (Bluetooth version 1.1 section F:1.6) */
#define OI_RFCOMM_MASK_RTR_OUTPUT  BIT11       /**< mask bit definition for setting the RTR/RTS flow control on output (Bluetooth version 1.1 section F:1.6) */
#define OI_RFCOMM_MASK_RTC_INPUT   BIT12       /**< mask bit definition for setting the RTC/CTS flow control on input (Bluetooth version 1.1 section F:1.6) */
#define OI_RFCOMM_MASK_RTC_OUTPUT  BIT13       /**< mask bit definition for setting the RTC/CTS flow control on output */
#define OI_RFCOMM_MASK_ALL         ((OI_UINT16) ~(BIT7 | BIT14 | BIT15))   /**< definition for mask that indicates that all parameters are to be set */
/** @} */

/** @name Bit rate */
/** @{ */
#define OI_RFCOMM_BAUDRATE_2400    0           /**< definition for bit rate parameter setting, indicating 2,400 bits per second */
#define OI_RFCOMM_BAUDRATE_4800    1           /**< definition for bit rate parameter setting, indicating 4,800 bits per second */
#define OI_RFCOMM_BAUDRATE_7200    2           /**< definition for bit rate parameter setting, indicating 7,200 bits per second */
#define OI_RFCOMM_BAUDRATE_9600    3           /**< definition for bit rate parameter setting, indicating 9,600 bits per second */
#define OI_RFCOMM_BAUDRATE_19200   4           /**< definition for bit rate parameter setting, indicating 19,200 bits per second */
#define OI_RFCOMM_BAUDRATE_38400   5           /**< definition for bit rate parameter setting, indicating 38,400 bits per second */
#define OI_RFCOMM_BAUDRATE_57600   6           /**< definition for bit rate parameter setting, indicating 57,600 bits per second */
#define OI_RFCOMM_BAUDRATE_115200  7           /**< definition for bit rate parameter setting, indicating 115,200 bits per second */
#define OI_RFCOMM_BAUDRATE_230400  8           /**< definition for bit rate parameter setting, indicating 230,400 bits per second */
/** @} */

/** @name Number of data bits */
/** @{ */
#define OI_RFCOMM_DATABIT_5    0               /**< definition for number of data bits, indicating 5 data bits */
#define OI_RFCOMM_DATABIT_6    BIT1            /**< definition for number of data bits, indicating 6 data bits */
#define OI_RFCOMM_DATABIT_7    BIT0            /**< definition for number of data bits, indicating 7 data bits */
#define OI_RFCOMM_DATABIT_8    ( BIT0 | BIT1 ) /**< definition for number of data bits, indicating 8 data bits */
/** @} */

/** @name Stop bits */
/** @{ */
#define OI_RFCOMM_STOPBIT_1     0           /**< definition for number of stop bits, indicating that 1 stop bit should be used */
#define OI_RFCOMM_STOPBIT_1_5   BIT2        /**< definition for number of stop bits, indicating that 1.5 stop bits should be used */
/** @} */

/** @name Parity */
/** @{ */
#define OI_RFCOMM_NO_PARITY     0           /**< definition for parity setting, indicating that parity checking is disabled */
#define OI_RFCOMM_PARITY        BIT3        /**< definition for parity setting, indicating that parity checking is enabled */
/** @} */

/** @name Parity type */
/** @{ */
#define OI_RFCOMM_PARITY_ODD    0               /**< definition for parity type, indicating that odd parity is used */
#define OI_RFCOMM_PARITY_EVEN   BIT5            /**< definition for parity type, indicating that even parity is used */     
#define OI_RFCOMM_PARITY_MARK   BIT4            /**< definition for parity type, indicating that mark parity is used */
#define OI_RFCOMM_PARITY_SPACE  ( BIT4 | BIT5 ) /**< definition for parity type, indicating that space parity is used */
/** @} */

/** @name Flow control */
/** @{ */
#define OI_RFCOMM_FLOW_NONE        0           /**< definition for flow control type, indicating that no flow control is used */
#define OI_RFCOMM_FLOW_XON_INPUT   BIT0        /**< definition for flow control type, indicating that XON/XOFF flow control is used on input */
#define OI_RFCOMM_FLOW_XON_OUTPUT  BIT1        /**< definition for flow control type, indicating that XON/XOFF flow control is used on output */
#define OI_RFCOMM_FLOW_RTR_INPUT   BIT2        /**< definition for flow control type, indicating that RTR/RTS flow control is used */
#define OI_RFCOMM_FLOW_RTR_OUTPUT  BIT3        /**< definition for flow control type, indicating that RTR/RTS flow control is used */
#define OI_RFCOMM_FLOW_RTC_INPUT   BIT4        /**< definition for flow control type, indicating that RTC/CTS flow control is used */
#define OI_RFCOMM_FLOW_RTC_OUTPUT  BIT5        /**< definition for flow control type, indicating that RTC/CTS flow control is used */
/** @} */

/** 
 * @name Line status codes
 * These line status (error) codes are passed to OI_RFCOMM_SendLineStatus() 
 */
/** @{ */
#define OI_RFCOMM_LINESTATUS_OVERRUN_ERR  (BIT0 | BIT1) /**< line status: overrun error */
#define OI_RFCOMM_LINESTATUS_PARITY_ERR   (BIT0 | BIT2) /**< line status: parity error */
#define OI_RFCOMM_LINESTATUS_FRAMING_ERR  (BIT0 | BIT3) /**< line status: framing error  */
/** @} */

/** 
 * @name Line control bits 
 * These line control bits are passed to OI_RFCOMM_SetModemSignals() 
 */
/** @{ */
#define OI_RFCOMM_LINE_RTC BIT2 /**< RTC (Ready To Communicate) bit; maps to DSR/DTR */
#define OI_RFCOMM_LINE_RTR BIT3 /**< RTR (Ready To Receive) bit; maps to RTS/CTS */
#define OI_RFCOMM_LINE_IC  BIT6 /**< IC (Incoming Call) bit; maps to RI */
#define OI_RFCOMM_LINE_DV  BIT7 /**< DV (Data Valid) bit; maps to DCD */
/** @} */

/************************************************************************

  callback function type definitions

 ************************************************************************/

/**
 * A callback function of this type confirms the establishment of a link to a
 * remote device.
 *
 * @param link       link handle
 *
 * @param frameSize  frame size to be used for the link
 *
 * @param result     OI_OK if successful; failure code otherwise
 *
 */
typedef void (*OI_RFCOMM_CONNECT_CFM)(OI_RFCOMM_LINK_HANDLE link,
                                      OI_UINT16 frameSize,
                                      OI_STATUS result);

/**
 * A callback function of this type indicates that a remote application is
 * attempting to establish a link.
 *
 * @param addr        pointer to the Bluetooth device address of the remote
 *                    device attempting to establish a link
 *
 * @param channel     server channel being connected to
 *
 * @param frameSize   frame size for the incoming connection
 *
 * @param link        link handle for the connection
 */
typedef void (*OI_RFCOMM_CONNECT_IND)(OI_BD_ADDR *addr,
                                      OI_UINT8 channel,
                                      OI_UINT16 frameSize,
                                      OI_RFCOMM_LINK_HANDLE link);

/**
 * A callback function of this type indicates that the specified link has been disconnected.
 *
 * @param linkHandle   handle of the link that was disconnected
 *
 * @param reason       status code indicating the reason for the disconnection
 */
typedef void (*OI_RFCOMM_DISCONNECT_IND)(OI_RFCOMM_LINK_HANDLE link,
                                         OI_STATUS reason);


/** 
 * A callback function of this type confirms the remote port negotiation or request
 * for the remote port parameters.
 *
 * @param link  link handle
 *
 * @param port  pointer to the structure containing the port parameters
 */
typedef void (*OI_RFCOMM_PORT_PARAM_CFM)(OI_RFCOMM_LINK_HANDLE link,
                                         OI_RFCOMM_PORTPARAM *port);

/**
 * A callback function of this type indicates that the remote side of the
 * session is requesting to set the port parameters on the local side of the
 * virtual serial link. The implementation of this function should set the
 * parameters in the structure pointed to by the 'port' parameter to the desired
 * values before returning. Leaving a proposed parameter unchanged indicates
 * acceptance of that proposed parameter.
 *
 * @param link  link handle
 *
 * @param port  pointer to the structure containing the port parameters
 */
typedef void (*OI_RFCOMM_PORT_PARAM_IND)(OI_RFCOMM_LINK_HANDLE link,
                                         OI_RFCOMM_PORTPARAM *port);

/**
 * A callback function of this type indicates the control signals set by the
 * remote device by means of the function OI_RFCOMM_SetModemSignals().
 *
 * @param link          link handle for the link on which the signals were set
 *
 * @param lineControl   state of the line control bits
 *
 * @param breakControl  state of the break control bits
 *
 */
typedef void (*OI_RFCOMM_LINE_CONTROL_IND)(OI_RFCOMM_LINK_HANDLE link,
                                           OI_BYTE lineControl,
                                           OI_BYTE breakControl);

/**
 * A callback function of this type indicates the line status sent by the remote
 * device.
 * 
 * @param link            link handle
 *
 * @param lineStatus      line status
 */
typedef void (*OI_RFCOMM_LINE_STATUS_IND)(OI_RFCOMM_LINK_HANDLE link,
                                          OI_BYTE lineStatus);

/**
 * A callback function of this type confirms that data was written to the
 * specified link.
 *
 * @param link         link handle of the link written to
 *
 * @param dataBuf      pointer to the buffer containing the data written
 *
 * @param dataLen      number of bytes written, always the same as the number 
 *                     of bytes passed to RFCOMM_Write()
 */
typedef void (*OI_RFCOMM_WRITE_CFM)(OI_RFCOMM_LINK_HANDLE link,
                                    OI_UINT8 *dataBuf,
                                    OI_UINT16 dataLen,
                                    OI_STATUS result);

/**
 * A callback function of this type indicates the receipt of data on the
 * specified link.
 *
 * @param link        link handle of the link on which the data was received
 *
 * @param dataBuf     pointer to the buffer containing the received data
 *
 * @param dataLen     number of bytes received
 */
typedef void (*OI_RFCOMM_RECV_DATA_IND)(OI_RFCOMM_LINK_HANDLE link,
                                        OI_BYTE *dataBuf,
                                        OI_UINT16 dataLen);

/**
 * A callback function of this type confirms receipt of a test response from a
 * remote device. 
 *
 * @param addr         pointer to the Bluetooth device address of the remote
 *                     device
 *
 * @param testPattern  pointer to the buffer containing the test pattern data
 *                     received from the remote device
 *
 * @param length       number of bytes in the test pattern
 */
typedef void (*OI_RFCOMM_TEST_CFM)(OI_BD_ADDR *addr,
                                   OI_UINT8 *testPattern,
                                   OI_UINT16 length);

/**
 * A structure of this type groups together the indication callbacks associated
 * with a link and is passed to OI_RFCOMM_Connect() by a client application or 
 * OI_RFCOMM_RegisterServer() by a server application.
 */
typedef struct {

    /** the callback function to be called when a link is released */
    OI_RFCOMM_DISCONNECT_IND disconnectInd;

    /** the callback function to be called when data is received */
    OI_RFCOMM_RECV_DATA_IND recvDataInd;

    /** the callback function to be called to indicate the line status sent by
     * the remote device.  (It is not necessary to register this type of
     * callback function, so this may be NULL.) */
    OI_RFCOMM_LINE_STATUS_IND lineStatusInd;

    /** the callback function to be called to indicate the state of the control
     * lines (It is not necessary to register this type of callback function, so this
     * may be NULL.) */
    OI_RFCOMM_LINE_CONTROL_IND lineControlInd;

    /** the callback function to be called to indicate that a remote device
     * wishes to configure a port.  (It is not necessary to register this type
     * of callback function, so this may be NULL.) */
    OI_RFCOMM_PORT_PARAM_IND portParamInd;

} OI_RFCOMM_LINK_INDICATIONS;


/************************************************************************

  functions

 ************************************************************************/

/**
 * This function registers a server application with the RFCOMM layer and
 * obtains a server channel number.
 *
 * @param callback      the indication callback function used to notify the
 *                      server of a client application attempting to connect
 *                      @param indications pointer to a structure containing
 *                      indication callback functions to be used for any new
 *                      link
 *
 * @param maxFrameSize  This indicates the maximum frame size that can be
 *                      received on links with this server application.  The
 *                      negotiated frame size (which is reported by the
 *                      OI_RFCOMM_CONNECT_IND callback function) may be smaller
 *                      than this.
 *
 * @param bufSize       This indicates the size of buffer to use for flow
 *                      control on a link created with this server.  This
 *                      buffering will be used for storing received data when
 *                      incoming flow is disabled.  To achieve optimal use of
 *                      buffer space, this number should be a multiple of the
 *                      frame size.  If the buffer size is 0, then no buffer
 *                      space will be allocated for the link and the application
 *                      should not invoke OI_RFCOMM_FlowEnable() with the
 *                      Boolean parameter FALSE.
 *
 * @param serverChannel pointer to the location where the server channel number
 *                      assigned to the application will be written.  If the input 
 *                      value is a valid RFCOMM channel number (1 - 30), this 
 *                      channel number will be used in preference to any other.
 * 
 * @param policy        the connection policy required 
 * 
 * @return              OI_OK if successful; OI_STATUS_NO_RESOURCES if no more
 *                      application registrations are allowed
 */
OI_STATUS OI_RFCOMM_RegisterServer(OI_RFCOMM_CONNECT_IND callback,
                                   const OI_RFCOMM_LINK_INDICATIONS *indications,
                                   OI_UINT16 maxFrameSize,
                                   OI_UINT16 bufSize,
                                   OI_UINT8 *serverChannel,
                                   const OI_CONNECT_POLICY *policy);

/**
 * This function deregisters a server application with the RFCOMM layer.
 *
 * @param serverChannel  the server channel number of the server to deregister
 *
 * @return               OI_OK if successful; OI_RFCOMM_INVALID_CHANNEL if the
 *                       server channel number is invalid
 */
OI_STATUS OI_RFCOMM_DeregisterServer(OI_UINT8 serverChannel);

/**
 * This function establishes an RFCOMM link with a remote server.
 *
 * @param callback       the confirmation callback function to be called when
 *                       the link is established
 *
 * @param indications    pointer to a structure containing indication callback
 *                       functions to be used for this link
 *
 * @param addr           pointer to the Bluetooth device address of the remote
 *                       device to connect to
 *
 * @param serverChannel  the server channel to connect to on the remote device
 *
 * @param maxFrameSize   This indicates the maximum frame size that can be
 *                       received on this link. The negotiated frame size (which
 *                       is reported by the OI_RFCOMM_CONNECT_IND callback
 *                       function) may be smaller than this.
 *
 * @param bufSize        This indicates the size of buffer to use for flow
 *                       control on this link.  This buffering will be used for
 *                       storing received data when incoming flow is disabled.
 *                       To achieve optimal use of buffer space, this number
 *                       should be a multiple of the frame size.  If the buffer
 *                       size is 0, then no buffer space will be allocated for
 *                       the link and the application should not invoke
 *                       OI_RFCOMM_FlowEnable() with the Boolean parameter FALSE.
 *
 * @param handle         pointer to the buffer where the link handle will be written.
 *                       This link handle can be used to reference the link once
 *                       a success (OI_OK) status is returned and the
 *                       OI_RFCOMM_CONNECT_CFM confirmation callback function
 *                       has been called.
 * 
 * @param policy         connection policy required for this connection
 *
 * @return               OI_OK if successful
 */
OI_STATUS OI_RFCOMM_Connect(OI_RFCOMM_CONNECT_CFM callback,
                            const OI_RFCOMM_LINK_INDICATIONS *indications,
                            OI_BD_ADDR *addr,
                            OI_UINT8 serverChannel,
                            OI_UINT16 maxFrameSize,
                            OI_UINT16 bufSize,
                            OI_RFCOMM_LINK_HANDLE *handle,
                            const OI_CONNECT_POLICY *policy);

/**
 * This function is used to respond to an indication that a remote application
 * is attempting to establish a link.  No connection parameters are passed in
 * here, since connection parameters (frame size and buffer size) were already
 * configured in OI_RFCOMM_RegisterServer().
 *
 * @param callback   confirmation callback to call when the connection has been
 *                   established
 *
 * @param link       the link handle for the connection
 *
 * @param accept     TRUE to accept and establish the link; FALSE to reject.
 */
OI_STATUS OI_RFCOMM_Accept( OI_RFCOMM_CONNECT_CFM callback,
                            OI_RFCOMM_LINK_HANDLE link,
                            OI_BOOL accept);

/** 
 * This function terminates the specified RFCOMM link. The disconnect indication
 * callback function passed to OI_RFCOMM_Accept() or OI_RFCOMM_Connect() will be
 * called when the disconnection is complete.
 *
 * @param link       the link handle of the link to be terminated
 * 
 * @return           OI_OK if successful
 */
OI_STATUS OI_RFCOMM_Disconnect(OI_RFCOMM_LINK_HANDLE link);

/** 
 * This function writes data to the specified RFCOMM link. Once this function
 * has been called with a given link handle, it should not be called again with
 * the same link handle before receiving an OI_RFCOMM_WRITE_CFM
 * confirmation callback function for that link. Concurrent writes to the same
 * link handle without an intervening write confirmation callback will result in
 * an error return status from OI_RFCOMM_Write(). 
 * 
 * The argument dataLen must be less than or equal to the negotiated maximum RFCOMM frame
 * size, maxFrameSize. Use the function OI_RFCOMM_WriteSegmented() to send data
 * with a dataLen larger than maxFrameSize.
 *
 * @param callback    the callback function for confirming the write
 *
 * @param link        the link handle of the link to which to write data
 *
 * @param data        pointer to the output buffer containing the data to write
 *
 * @param dataLen     number of bytes to send from the data output buffer 
 *
 * @return            OI_OK if successful
 */
OI_STATUS OI_RFCOMM_Write(OI_RFCOMM_WRITE_CFM callback,
                          OI_RFCOMM_LINK_HANDLE link, 
                          OI_UINT8 *data, 
                          OI_UINT16 dataLen);

/**
 * This function sends an entire buffer of data. Unlike OI_RFCOMM_Write(), 
 * this function accepts a buffer with a length greater than the frame size 
 * for the specified link. If dataLen is greater than the frame size, RFCOMM 
 * will perform segmentation internally, sending multiple frames of data no 
 * more than the frame size in length. The callback will be called once the entire
 * contents of the buffer have been sent.
 *
 * @param callback    the callback function for confirming the write
 *
 * @param link        the link handle of the link to which to write data
 *
 * @param data        pointer to the output buffer containing the data to write
 *
 * @param dataLen     number of bytes to send from the data output buffer 
 */
OI_STATUS OI_RFCOMM_WriteSegmented(OI_RFCOMM_WRITE_CFM callback,
                                   OI_RFCOMM_LINK_HANDLE link, 
                                   OI_UINT8 *data, 
                                   OI_UINT16 dataLen);

/** 
 * This function requests that the port parameters for the remote side of the
 * virtual serial link specified by the OI_RFCOMM_LINK_HANDLE parameter be
 * set to the values indicated by the OI_RFCOMM_PORTPARAM structure. A mask in
 * this structure indicates which parameters are to be set and which are to be
 * ignored. The result of the request will be reported in the callback.
 *
 * @param callback    the confirmation callback function to be called when the
 *                    parameter setting function has completed
 *
 * @param link        the link handle associated with the remote serial port for
 *                    which the configuration is being set
 *
 * @param port        pointer to the structure containing the port parameters
 *
 */
OI_STATUS OI_RFCOMM_SetPortParams(OI_RFCOMM_PORT_PARAM_CFM callback,
                                  OI_RFCOMM_LINK_HANDLE link,
                                  OI_RFCOMM_PORTPARAM *port);

/** 
 * This function requests a read of the port parameters for the remote side of
 * the virtual serial link specified by the OI_RFCOMM_LINK_HANDLE.
 *
 * @param callback    confirmation callback function
 *
 * @param handle      the link handle associated with the serial port for which
 *                    the remote configuration is being read
 */
OI_STATUS OI_RFCOMM_GetPortParams(OI_RFCOMM_PORT_PARAM_CFM callback,
                                  OI_RFCOMM_LINK_HANDLE handle);

/** 
 * This function sets the modem control signals for the specified
 * link. This function should be called to notify the remote side 
 * of the status whenever the signals change.
 *
 * @param link         the link handle of the link for which to set the signals
 *
 * @param lineControl  This parameter carries the line control bits. This should be
 *                     the logical OR of any of: 
 *                     - OI_RFCOMM_LINE_RTC
 *                     - OI_RFCOMM_LINE_RTR
 *                     - OI_RFCOMM_LINE_IC 
 *                     - OI_RFCOMM_LINE_DV
 *
 * @param breakControl  the break control bits
 */
OI_STATUS OI_RFCOMM_SetModemSignals(OI_RFCOMM_LINK_HANDLE link,
                                    OI_INT8 lineControl, 
                                    OI_INT8 breakControl);

/**
 * This function sends the specified line status code to the remote device.
 *
 * @param link        the link handle of the link on which to send the status
 *                    code
 *
 * @param lineStatus  the line status code to send
 */
OI_STATUS OI_RFCOMM_SendLineStatus(OI_RFCOMM_LINK_HANDLE link,
                                   OI_BYTE lineStatus);


/**
 * This function tests the data link to the specified device by sending a test
 * data pattern. The remote device should write the same test pattern back. The
 * callback function confirms that the response has been received.
 * 
 * @param callback     the confirmation callback function to be called when the
 *                     test response is received
 *
 * @param addr         pointer to the Bluetooth device address of the remote
 *                     device
 *
 * @param testPattern  pointer to the buffer containing the test pattern data to
 *                     be sent to the remote device
 *
 * @param length       the number of bytes in the test pattern, which must not
 *                     be greater than 255
 */
OI_STATUS OI_RFCOMM_Test(OI_RFCOMM_TEST_CFM callback,
                         OI_BD_ADDR *addr,
                         OI_BYTE *testPattern,
                         OI_UINT8 length);

/**
 * This function is used to enable or disable flow on incoming data on the
 * specified link. By default, flow is enabled. Flow in this context refers to
 * the application reading data directly without flow control buffering. In
 * other words, flow being enabled means that flow control is off; flow being
 * disabled means that flow control is on. An application that can not receive
 * more data would call OI_RFCOMM_FlowEnable() with the Boolean parameter FALSE;
 * this causes received data to be buffered by the RFCOMM layer. The actual flow
 * control mechanism is not visible to the application.
 *
 * @param handle  link handle for the link on which to enable or disable flow
 *
 * @param enable  TRUE to enable flow, FALSE to disable flow
 */
OI_STATUS OI_RFCOMM_FlowEnable(OI_RFCOMM_LINK_HANDLE handle,
                               OI_BOOL enable );


#endif /* _OI_RFCOMM_H */
