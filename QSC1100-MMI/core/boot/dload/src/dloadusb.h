#ifndef DLOADUSB_H
#define DLOADUSB_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   U S B   D R I V E R  H E A D E R

 GENERAL DESCRIPTION
  This header file contains the definitions of the MSM5.x USB
  hardware registers needed for the boot downloader.

  This file also contains declarations for data structure and functions
  related to the USB software download on to the MSM

  The USB software download function's configuration consists of one Interface
  with two BULK endpoints. A software download application such QPST can
  communicate with the MSM using the end points of the USB software download
  function. The host application can send DMSS software download commands
  over the
  OUT Bulk endpoint and receive status information from the MSM over the IN
  Bulk endpoint.

  Copyright (c) 2003, 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/dload/1_0/dloadusb.h#3 $ $DateTime: 2008/11/14 17:29:10 $ $Author: taaran $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/05   tbg     Increased buffer to support FEATURE_DLOAD_MEM_DEBUG
12/22/04   rp      Added init_rx_dma_ptr to dloadusb_data_type.
07/08/04   rp      Updated to use usbotg_xcvr_type in usbotg.h
06/07/04   rp      Added dloadusb_endpoint_type struct.
05/05/04   rp      Added configured flag, rx_dma_index, and second rx_buffer
                   in dloadusb_data_type.
03/25/04   rp      Added tx_sending_short_pkt flag and xcvr_type to 
                   dloadusb_data_type.
09/19/03   rp      File creation for USB OTG support.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_USB
	#include "usbdc.h"
	#include "usbotg.h"
	#include "usbcdc.h"
	#include "usbdci.h"
#endif	

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Error return code from dloadusb_receive_byte */
/* This int value must not be any valid unsigned char value. */
#define  USB_NO_CHAR    (-1)

typedef enum
{
  INTERFACE_UNKNOWN,
  INTERFACE_UART1,
  INTERFACE_USB
} dloadusb_interface_type;

#ifdef FEATURE_USB

/*---------------------------------------------------------------------------
 The USB Device Endpoint Info
---------------------------------------------------------------------------*/
typedef struct
{
  word            transfer_type;          // transfer type
  int             ep_num;                 // logical endpoint number
  byte            max_pkt_sz;             // max packet size
  word            buffersize;             // Size of X and Y buffers
} dloadusb_endpoint_type;

/* USB data to be stored in upper RAM */
typedef struct
{
  /* Indexes */
  int tx_buf_index;
  int rx_index;
  int rx_dma_index;

  /* Counter indicates how many bytes are read into the rx_buffer. */
  int rx_num_bytes_read;

  /* Buffer to save the information about the line coding. */
  byte line_coding[8];

  /* This points to the transmit buffer on the IN end-point. */
  byte tx_buffer[4096];

  /* To store the data received from the host. 
     2 buffers are allocated so data processing can be done simultaneously
     with a new transfer */
  byte rx_buffer[2][USBDC_BULK_PKT_SIZE*2];

  /* Used to track progress of DMA transfer */
  dword init_rx_dma_ptr;
  
  /* The following two attributes are to save the out and in end points
     used by the diag task to communicate with the host. */
  byte usb_in_endpoint;
  byte usb_out_endpoint;

  /* Host defined device address */
  word address;
  
  /* Setup message flag */
  boolean setup_data_stage_flag;

  /* Sending short packet to host flag */
  boolean tx_sending_short_pkt;

  /* Five wire or OTG xcvr */
  usbotg_xcvr_type xcvr_type; 

  /* Host has configured device */
  boolean configured;
 
} dloadusb_data_type;

#endif  

/*===========================================================================

FUNCTION dloadusb_drain

DESCRIPTION
  This function waits for the last character in the USB's transmit
  FIFO to be transmitted.  This allows the caller to be sure that all
  characters are transmitted.

DEPENDENCIES
  If the transmit FIFO is not empty, the USB must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/

void dloadusb_drain(void);

/*===========================================================================

FUNCTION dloadusb_init

DESCRIPTION
  This function initializes the MSM 5500 USB device controller with no
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
  None

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/

void dloadusb_init(void);

/*===========================================================================

FUNCTION dloadusb_receive_byte

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

int dloadusb_receive_byte(void);

/*===========================================================================

FUNCTION dloadusb_transmit_byte

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

void dloadusb_transmit_byte (byte  chr);

/*===========================================================================

FUNCTION dloadusb_shutdown

DESCRIPTION
  This function will disable and de-configure all
  confgured endpoints.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dloadusb_shutdown
(
  void
);
#endif
