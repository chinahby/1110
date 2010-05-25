#ifndef _QUSB_DLOAD_API_H_
#define _QUSB_DLOAD_API_H_
/*=======================================================================*//**
 * @file        qhsusb_al_dload_api.h
 * @author:     amirs
 * @date        14-July-2008
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) AL (Adaptation-Layer) exported API.
 *
 * @details     This header file contains the API to be used by the boot downloader.
 *              A software download application such QPST can communicate with the MSM using the USB.
 *              The host application can send software download commands over the OUT Bulk endpoint,
 *              and receive status information from the MSM over the IN Bulk endpoint.
 *
 * @note        The DLOAD was originally using UART ,
 *              therefore the TX and RX API are using bytes.
 *
 *              Copyright (c) 2008 QUALCOMM Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/

// ===========================================================================
//
//                           EDIT HISTORY FOR FILE
//  This section contains comments describing changes made to the module.
//  Notice that changes are listed in reverse chronological order.
//
//  $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/inc/qhsusb_al_dload_api.h#4 $$DateTime: 2008/12/29 03:57:32 $$Author: vkondrat $
//
// when       who     what, where, why
// --------   ---     ----------------------------------------------------------
// 07/14/08   amirs   First Draft
// 08/08/08   amirs   First working DLOAD process
// 08/11/08   amirs   Add files and functions headers according to coding standard.
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"     // basic types as byte word uint32 etc

//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

/* Error return code from qhsusb_al_dload_receive_byte */
/* This int value must not be any valid unsigned char value. */
#define  USB_NO_CHAR    (-1)

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------
// None

//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------

// ===========================================================================
/**
 * @function    qhsusb_al_dload_drain
 * 
 * @brief       wait until tx complete.
 *
 * @details
 *          This function waits for the last character in the USB's transmit
 *          FIFO to be transmitted.  This allows the caller to be sure that all
 *          characters are transmitted.
 * 
 * @param   None
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qhsusb_al_dload_drain(void);

// ===========================================================================
/**
 * @brief Initialize the USB stack
 * @details 
 * 
 *         This function initializes the USB device controller with no
 *         interrupts enabled but both receiver and transmitter running.
 * @param use_cdcacm Defines USB interface type:
 *                   0 - OBEX (vendor specific), 1 - CDC/ACM
 */
// ===========================================================================
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
void qhsusb_al_dload_init(int use_cdcacm);
#else /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */
void qhsusb_al_dload_init(void);
#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */
// ===========================================================================
/**
 * @function    qhsusb_al_dload_receive_byte
 * 
 * @brief   return a byte received from the host.
 *
 * @details
 *         This function receives an incoming data from the respective USB out fifos and
 *         returns one character at a time to the calling function. Though it receives
 *         a bigger packet at once, it always retuns one character to the calling function.
 *         This approach is choosen to have a consitancy between the UART and USB modules.
 * 
 * @param   None
 * 
 * @return Value/Status
 *         Returns the next character from the receive buffer.
 *         If there is nothing in the receive buffer then it return USB_NO_CHAR (-1).
 * 
 */
// ===========================================================================
int qhsusb_al_dload_receive_byte(void);

// ===========================================================================
/**
 * @function    qhsusb_al_dload_transmit_byte
 * 
 * @brief       Transmit a byte to the host.
 *
 * @details
 *          This function transmits a single byte through the USB.
 *          The byte is actually added to a buffer until the tx buffer is full,
 *          or an "end of message" (0x7E) is detected.
 * 
 *          This function also poll the USB stack for any event,
 *          like previouse rx/tx completed.
 * 
 * @param   chr
 *          one byte character value.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qhsusb_al_dload_transmit_byte (byte  chr);

// ===========================================================================
/**
 * @function    qhsusb_al_dload_shutdown
 * 
 * @brief       This function will cancel any pending transfer.
 *
 * @note        This function is called before the DLOAD jump to ARMPROG.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 * @see     qhsusb_boot_dload_armprg_transition , go_cmd
 * 
 */
// ===========================================================================
void qhsusb_al_dload_shutdown(void);

// ===========================================================================
/**
 * @function    qhsusb_boot_dload_armprg_transition
 * 
 * @brief       This function handle the transition from DLOAD to ARMPROG from the USB point of view.
 *
 * @param   None.
 * 
 * @return  None
 * 
 * @see     qhsusb_al_dload_drain , qhsusb_al_dload_shutdown , spoof_reconnect , go_cmd
 * 
 */
// ===========================================================================
void qhsusb_boot_dload_armprg_transition(void);

// ===========================================================================
/**
 * @brief This function receive a buffer from the host.
 * 
 * Packet get copied from USB internal buffer
 * to one pointed by \a receive_ptr
 * 
 * Copy till the end of packet, maximum *len bytes
 * 
 * @note may be used simultaneously with qhsusb_al_dload_receive_byte
 *
 * @param receive_ptr
 *               Buffer to be filled with incoming packet
 * @param len    - input: buffer size
 *               - output: write actual number of bytes copied here
 * 
 * @return -  0 : OK
 *         - -1 : Rx error
 *         - -2 : no data
 *         - -3 : timeout
 *         - -4 : configuration error
 * @see qhsusb_al_dload_receive_byte
 */
// ===========================================================================
int32 qhsusb_al_dload_receive_pkt(byte* receive_ptr, uint32* len);
#define  QHSUSB_PKT_DRIVER_ALL_OK     (0)
#define  QHSUSB_PKT_DRIVER_RX_ERR    (-1)
#define  QHSUSB_PKT_DRIVER_NO_CHAR   (-2)
#define  QHSUSB_PKT_DRIVER_TIMEOUT   (-3)
#define  QHSUSB_PKT_DRIVER_CONF_ERR  (-4)

// ===========================================================================
/**
 * @function    qhsusb_al_dload_transmit_pkt
 * 
 * @brief       This function transmit a buffer to the host.
 *
 * @note        Not implemented yet.
 * 
 * @param   pkt
 *          pointer to pkt to be transmitted
 * 
 * @param   len
 *          number of bytes to tx
 * 
 * @return  none
 * 
 * @see     qhsusb_al_dload_receive_pkt
 * 
 */
// ===========================================================================
void qhsusb_al_dload_transmit_pkt(byte *pkt, uint32 len);


#endif // _QUSB_DLOAD_API_H_
