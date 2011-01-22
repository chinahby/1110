#ifndef DLOADUART_H
#define DLOADUART_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   U A R T   D R I V E R  H E A D E R 

 GENERAL DESCRIPTION
  This header file contains the definitions of the MSM2.x UART 
  hardware registers needed for the boot downloader.

  Copyright (c) 1995,1996,1997,1998,1999 by QUALCOMM Incorporated.
  Copyright (c) 2000,2002                by QUALCOMM Incorporated.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/dloaduart.h_v   1.2   18 Mar 2002 13:08:40   rprenove  $
$Header: //depot/asic/qsc1100/services/dload/dloaduart.h#2 $ $DateTime: 2008/08/29 06:24:58 $ $Author: taaran $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/07   yl/ty   Support UART3 for dload if FEATURE_THIRD_UART defined.
06/24/05   cr      L4 changes.
11/12/02   rp      Added dload_uart_init(), uart_drain(), uart_receive_byte()
                   and uart_transmit_byte().
10/24/02   dbc     Removed incude of "bsp.h" 
03/18/02   rp      Moved most defines to .c file.
11/01/00   rmd     Replaced FEATURE_TXCO19XX (XX = 2, 8, 68) with 
                   (BSP_OSCILLATOR_IN == BSP_TCXO_19PXX_MHZ).
10/25/00   rmd     Moved bit definitions to msm5XXXbits.h.
08/31/00   jcw     Merge of MSM5000 and MSM3300 versions
06/28/00   aks     UART clock values added for TCXO1920 and TCXO1980.
06/05/00   jc      Added support for FEATURE_TCXO1920
08/05/98   hcg     created, taken from flashprg

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Error return code from uart_receive_byte */
/* This int value must not be any valid unsigned char value. */
#define  UART_RX_ERR    (-1)
#define  UART_NO_CHAR   (-2)
#define  UART_TIMEOUT   (-3)
#define  UART_ALL_OK    (0)

/* macro for 16 bit writes to MSM registers */
#define MSMUW_OUT( reg, val) HWIO_OUT( reg, val)             

#ifdef FEATURE_IG_DPRGS
/* The Iguana Downloader Programmer app builds dload files.  dloaduart
** has some sybmols that collide the dprg symbols.  So, use #define
** to previx dloaduart symbols with dload as it should have been to
** begin with.  Note, this will cause a different symbol to be compiled,
** so remember that when debugging.
*/
//#define uart_drain          dload_uart_drain
//#define uart_receive_byte   dload_receive_byte
//#define uart_transmit_byte  dload_uart_transmit_byte
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

FUNCTION dload_uart_init

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
extern void dload_uart_init(void);


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
extern void uart_drain(void);


/*===========================================================================

FUNCTION uart_receive_byte

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
extern int uart_receive_byte(void);


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
extern void uart_transmit_byte
(
  byte  chr
    /* Character to be transmitted */
);

#ifdef FEATURE_L4_KERNEL
#error code not present
#endif

#if defined(FEATURE_THIRD_UART) && (defined(T_MSM7200) || defined(T_MSM7600))

/*===========================================================================

FUNCTION dload_uart3_init

DESCRIPTION
  This function initializes UART3 to 115,200 bps, 8N1, with
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
extern void dload_uart3_init(void);

/*===========================================================================

FUNCTION uart3_drain

DESCRIPTION
  This function waits for the last character in the UART's transmit
  FIFO to be transmitted.  This allows the caller to be sure that all
  characters are transmitted. This is for UART3.

DEPENDENCIES
  If the transmit FIFO is not empty, the UART must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/
extern void uart3_drain(void);

/*===========================================================================

FUNCTION uart3_receive_byte

DESCRIPTION
  This function receives a single byte from UART3 by polling.

DEPENDENCIES
  The UART must be initialized and enabled, or else this routine will
  wait forever.

RETURN VALUE
  If a character is received without error, returns the value of the
  character received.  If an error occurs, returns UART_RX_ERR.  If a
  timeout occurs, returns UART_TIMEOUT.

SIDE EFFECTS

===========================================================================*/
extern int uart3_receive_byte(void);

/*===========================================================================

FUNCTION uart3_transmit_byte

DESCRIPTION
  This function transmits a single byte through the UART3.

DEPENDENCIES
  The UART transmitter must be initialized and enabled, or this routine
  will wait forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog may be reset.

===========================================================================*/
extern void uart3_transmit_byte
(
  byte  chr         /* Character to be transmitted */
);
#endif  /* FEATURE_THIRD_UART && (T_MSM7200 || T_MSM7600) */

#ifdef __cplusplus
}
#endif

#endif /*DLOADUART_H*/

