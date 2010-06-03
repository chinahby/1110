/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   U A R T   D R I V E R

GENERAL DESCRIPTION
  This module contains a very simple-minded polling UART driver
  for the DMSS boot downloader facility.  This version supports
  the MSM 3.0 UART.

EXTERNALIZED FUNCTIONS
   dload_uart_init
      Initializes the UART for transmit and receive.

   uart_receive_byte
      Receives a single byte from the UART.

   uart_transmit_byte
      Transmits a single byte to the UART.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  All necessary initialization for normal CPU operation must have
  been performed before entering this module.  dload_uart_init() should
  be called before any other functions.

  Copyright (c) 1995,1996,1997,1998,1999,2002,2004 by QUALCOMM Incorporated.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/dloaduart.c_v   1.4   18 Mar 2002 13:09:14   rprenove  $
$Header: //depot/asic/qsc1100/services/dload/dloaduart.c#2 $ $DateTime: 2008/08/29 05:17:59 $ $Author: taaran $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "boothw.h"
#include "dloaduart.h"
#include "uartdm.h"

/*===========================================================================

FUNCTION uart_drain

DESCRIPTION
  This function waits for the last character in the UART's transmit
  FIFO to be transmitted.  This allows the caller to be sure that all
  characters are transmitted.

DEPENDENCIES
  If the transmit FIFO is not empty, the UART must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/

void uart_drain(void)
{
uartdm_drain();
}/* uart_drain() */


/*===========================================================================

FUNCTION dload_uart_init

DESCRIPTION
  This function initializes the MSM 3000 UART to 115,200 bps, 8N1, with
  no interrupts enabled but both receiver and transmitter running.

  Before initializing the UART, this function waits for the UART's
  transmit buffer to empty out.  This permits the ACK to the GO
  command that started the program (or any other pending transmission)
  to get back to the host, instead of being lost when the UART is
  reset.

DEPENDENCIES
  If the transmit FIFO is not empty, the UART must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/

void dload_uart_init(void)
{
 uartdm_init();
} /* dload_uart_init() */


/*===========================================================================

FUNCTION uart_receive_byte

DESCRIPTION
  This function receives a single byte from the UART by polling.

DEPENDENCIES
  The UART must be initialized and enabled, or else this routine will
  wait forever.

RETURN VALUE
  If a character is received without error, returns the value of the
  character received.  If an error occurs, returns UART_RX_ERR.  If a
  timeout occurs, returns UART_TIMEOUT.

SIDE EFFECTS

===========================================================================*/

int uart_receive_byte
(
  void
)
{
    return UART_NO_CHAR;
} /* uart_receive_byte() */

/*===========================================================================

FUNCTION uart_transmit_byte

DESCRIPTION
  This function transmits a single byte through the UART.

DEPENDENCIES
  The UART transmitter must be initialized and enabled, or this routine
  will wait forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog may be reset.

===========================================================================*/

void uart_transmit_byte
(
  byte  chr
    /* Character to be transmitted */
)

{
	uartdm_transmit_byte(chr);

} /* uart_transmit_byte() */


