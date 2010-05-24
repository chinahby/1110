#ifndef UARTDM_H
#define UARTDM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   U A R T D M    D R I V E R    H E A D E R 

 GENERAL DESCRIPTION
  This header file contains the definitions of the MSM2.x UART 
  hardware registers needed for the boot downloader.

  Copyright (c) 2007                by QUALCOMM Incorporated.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/dloaduart.h_v   1.2   18 Mar 2002 13:08:40   rprenove  $
$Header: //depot/asic/qsc1100/services/dload/uartdm.h#1 $ 
$DateTime: 2008/08/29 06:24:58 $ $Author: taaran $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================
MACRO bLoad8D

DESCRIPTION
  
  This function takes a byte pointer and it returns the byte value.

DEPENDENCIES
  None

PARAMETERS
  p        - a Byte  Pointer.
  
RETURN VALUE
  Byte value.

SIDE EFFECTS
  None
===========================================================================*/
#define bLoad8D(p)      (*(uint8*)(p))


/*===========================================================================
MACRO bStore8D

DESCRIPTION
  
  This function takes input value and stores it at the address
  pointed by the input byte pointer.
  
DEPENDENCIES
  None

PARAMETERS
  p        - Byte pointer.
  val      - Data to be stored in memory.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#define bStor8D(p,val) (*(uint8*)(p) = (val))



#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

FUNCTION uartdm_init

DESCRIPTION
  This function initializes the MSM 2.0 UART to 115,200 bps, 8N1, with
  no interrupts enabled but both receiver and transmitter running.

  Actually, at the moment this function implements the clock/2
  workaround for a clocking bug, so it gets 19,200 bps.

DEPENDENCIES
  If the transmit FIFO is not empty, the UART must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/
extern void uartdm_init(void);

/*===========================================================================

FUNCTION uartdm_drain

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
extern void uartdm_drain(void);

/*===========================================================================

FUNCTION uartdm_receive_byte

DESCRIPTION
  This function receives a single byte from the UART by polling.

DEPENDENCIES
  The UART must be initialized and enabled, or else this routine will
  wait forever.

RETURN VALUE
  If a character is received without error, returns the value of the
  character received.  If an error occurs, returns UART_RX_ERR.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/
extern int uartdm_receive_byte(byte* receive_ptr, uint32* len);

/*===========================================================================






FUNCTION uartdm_transmit_byte

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
extern void uartdm_transmit_byte
(
  byte  chr
    /* Character to be transmitted */
);


int uartdm_get_interface_status(void);
	


/*===========================================================================

FUNCTION uartdm_receive_byte

DESCRIPTION
  This function receives a single byte from the UART by polling.

DEPENDENCIES
  The UART must be initialized and enabled, or else this routine will
  wait forever.

RETURN VALUE
  If a character is received without error, returns the value of the
  character received.  If an error occurs, returns UART_RX_ERR.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/
extern void uartdm_init_receiver(void);

#ifdef __cplusplus
}
#endif

#endif /*UARTDM_H*/
