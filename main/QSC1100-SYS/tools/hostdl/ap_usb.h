#ifndef AP_USB_H
#define AP_USB_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   U S B   D R I V E R  H E A D E R 

 GENERAL DESCRIPTION
  This header file is for MSM6500 USB 

  Copyright (c)  2008 Qualcomm Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================
                          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/tools/hostdl/HOSTDL.14.00/ap_usb.h#1 $ $DateTime: 2008/01/21 21:01:41 $ $Author: opham $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/MSM6275/ap_usb.h#2 
  
 when       who     what, where, why
 --------   ---     --------------------------------------------------------
 01/03/08   op     Initial version for QSC1100 Multi-Image Boot

============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "usbdc.h"
#include "usbcdc.h"
#include "usbdci.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Error return code from dloadusb_receive_byte */
/* This int value must not be any valid unsigned char value. */
#define  USB_NO_CHAR    (-1)


/* Size of the USB receive buffers.  Must be larger than the packet size
 * including encapsulation and must be an even multiple of 64
 */
#define USB_RX_BUFFER_SIZE 1152

/* USB data to be stored in upper RAM */
typedef struct
{
  /* Buffer to save the information about the line coding. */
  byte line_coding[8];

  /* This points to the transmit buffer on the IN end-point. 
   * This must be larger than the transmit buffer back to the
   * host and a multiple of 64 bytes.
   */
  byte tx_buffer[1040];

  /* Ping Pong buffers to store the data received from the host. 
   * We keep one DMA going all the time while processing data
   * from the already received buffer.
   */
  byte rx_buffer1[USB_RX_BUFFER_SIZE];
  byte rx_buffer2[USB_RX_BUFFER_SIZE];

  /* Indexes */
  word tx_buf_index;
  word rx_index;

  /* Counter indicates how many bytes are read into the rx_buffer. */
  word rx_num_bytes_read;

  /* The following two attributes are to save the out and in end points
     used by the diag task to communicate with the host. */
  byte usb_in_endpoint;
  byte usb_out_endpoint;

  /* Host defined device address */
  word address;
  
  /* Setup message flag */
  boolean setup_data_stage_flag;

} usb_data_type;


/*===========================================================================

FUNCTION usb_drain

DESCRIPTION
  This function waits for the last character in the USB's transmit
  FIFO to be transmitted.  This allows the caller to be sure that all
  characters are t
DEPENDENCIES
  If the transmit FIFO is not empty, the USB must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/

void usb_drain(void);

/*===========================================================================

FUNCTION dloadusb_init

DESCRIPTION
  This function initializes the MSM 6500 USB device controller with no
  interrupts enabled but both receiver and transmitter running.

  Before initializing the USB, this function waits for the USB's
  transmit buffer to empty out.  This permits the ACK to the GO
  command that started the program (or any other pending transmission)
  to get back to the host, instead of being lost when the USB reset.

DEPENDENCIES
  If the transmit FIFO is not empty, the USB must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

RETURN VALUE
  True  : The USB is running
  False	: The USB is not running

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/

boolean usb_init(void);

/*===========================================================================

FUNCTION usb_receive_byte

DESCRIPTION
  This function receives an incoming data from the respective usb out fifos and
  returns one character at a time to the calling function. Though it receives
  a bigger packet at once, it always retuns one character to the calling function.
  This approach is choosen to have a consitancy between the UART and USB modules.

DEPENDENCIES
  The USB transmitter must be initialized and enabled, or this routine
  will wait forever.

RETURN VALUE
  Returns the next character from the receive buffer. If there is nothing
  in the receive buffer then it just waits unconditionally for an incoming
  data. No timeout mechanism is implemented as opposed to the UART module.

SIDE EFFECTS
  The watchdog may be reset.

===========================================================================*/

int usb_receive_byte(void);

/*===========================================================================

FUNCTION usb_transmit_byte

DESCRIPTION
  This function transmits a single dword through the USB. If the end of message
  (0x7E) is received from the packet module, then this function will send the
  data without waiting for the remaining bytes to fill the 4 byte buffer.

DEPENDENCIES
  The USB transmitter must be initialized and enabled, or this routine
  will wait forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog may be reset.

===========================================================================*/

void usb_transmit_byte (byte  chr);


#endif // #ifndef AP_USB_H

