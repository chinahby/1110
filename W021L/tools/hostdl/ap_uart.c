/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   U A R T   D R I V E R

GENERAL DESCRIPTION
  This module contains a very simple-minded polling UART driver
  for ARMPRG.  This version supports only MSM6250.
  
  This module must be ported to all other MSM architectures.

EXTERNALIZED FUNCTIONS
   uartdm_init
      Initializes the UART for transmit and receive.

   uartdm_active
      Determines if any activity on UART
      
   uartdm_drain
      Drain any data from the UART transmit FIFO
      
   do_uart_check
      Receives incoming data (upto 128 bytes) from the UART.

   uartdm_transmit_byte
      Transmits a single byte to the UART.

   uartdm_receive_bytes
      Receives data bytes from the UART

INITIALIZATION AND SEQUENCING REQUIREMENTS
  All necessary initialization for normal CPU operation must have
  been performed before entering this module.  uart_init() should
  be called before any other functions.

  Copyright (c)  2008 Qualcomm Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================
                          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/tools/hostdl/HOSTDL.14.00/ap_uart.c#3 $ $DateTime: 2008/09/18 14:39:04 $ $Author: c_surven $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/MSM6275/ap_uart.c#2 
  
 when       who     what, where, why
 --------   ---     --------------------------------------------------------
09/17/08   sv     Enable UART DM driver support
05/27/08   sv     Fix RVCT compiler warning
01/03/08   op     Initial version for QSC1100


============================================================================*/

//--------------------------------------------------------------------------
// Include Files
//--------------------------------------------------------------------------

#include "comdef.h"
#include "ap_armprg.h"
#include "ap_msm.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* ----------------------------------------------------------------------- */
/* Setup the M/N:D counters based on TCXO                                  */
/* ----------------------------------------------------------------------- */
#define UARTDM_M_REG_VAL              0xC
#define UARTDM_D_REG_VAL              0xFF82
#define UARTDM_N_REG_VAL              0xFF8e

/* ----------------------------------------------------------------------- */
/* Following defines should be changed to match the target on which        */
/* this driver will be ported on.                                          */
/*                   Target specific defines starts                        */
/* ----------------------------------------------------------------------- */
#define UARTDM_SR                     UART_DM_SR
#define UARTDM_CR                     UART_DM_CR
#define UARTDM_ISR                    UART_DM_ISR
#define UARTDM_MR1                    UART_DM_MR1
#define UARTDM_MR2                    UART_DM_MR2
#define UARTDM_MN                     UART_DM_MN_REG
#define UARTDM_IPR                    UART_DM_IPR
#define UARTDM_DMRX                   UART_DM_DMRX
#define UARTDM_DMEN                   UART_DM_DMEN
#define UARTDM_BADR                   UART_DM_BADR
#define UARTDM_NO_CHARS_FOR_TX        UART_DM_NO_CHARS_FOR_TX
#define UARTDM_RX_TOTAL_SNAP          UART_DM_RX_TOTAL_SNAP
#define UARTDM_CD                     UART_DM_CD_REG
#define UARTDM_IRDA                   UART_DM_IRDA
#define UARTDM_IMR                    UART_DM_IMR
#define UARTDM_RFWR                   UART_DM_RFWR
#define UARTDM_TFWR                   UART_DM_TFWR
#define UARTDM_CSR                    UART_DM_CSR
#define UARTDM_RF                     UART_DM_RF
#define UARTDM_TF                     UART_DM_TF
#define UARTDM_MISR                   UART_DM_MISR
#define UARTDM_HCR                    UART_DM_HCR
/* ----------------------------------------------------------------------- */
/*                   Target specific defines ends                          */
/* ----------------------------------------------------------------------- */

#define UARTDM_CSR_384K_BPS          0xdd      /* 38,400 bps                    */
#define UARTDM_CSR_1152K_BPS         0xff      /* 115,200 bps                   */

/* These definitions are moved here from msmbits.h, which has been
   obseleted on 7200 and 7600. */

/*      8 bits per character          */
#define UARTDM_MR2_8BPC         (0x03 << HWIO_SHFT(UARTDM_MR2,BITS_PER_CHAR))

/*      1 stop bit                    */
#define UARTDM_MR2_1SB          (0x01 << HWIO_SHFT(UARTDM_MR2,STOP_BIT_LEN))

/*      Break received                */
#define UARTDM_SR_BREAK_RXD     (0x01 << HWIO_SHFT(UARTDM_SR,RX_BREAK))

/*      Parity or Framing error       */
#define UARTDM_SR_PF_ERR        (0x01 << HWIO_SHFT(UARTDM_SR,PAR_FRAME_ERR))

/*      Overrun error                 */
#define UARTDM_SR_OVR_ERR       (0x01 << HWIO_SHFT(UARTDM_SR,UART_OVERRUN))

/*      Transmitter empty             */
#define UARTDM_SR_TXEMT         (0x01 << HWIO_SHFT(UARTDM_SR,TXEMT))

/*      Receiver ready                */
#define UARTDM_SR_RXRDY         (0x01 << HWIO_SHFT(UARTDM_SR,RXRDY))

/*      Reset error status            */
#define UARTDM_CR_RESET_ERR     (0x03 << HWIO_SHFT(UARTDM_CR,CHANNEL_COMMAND_LSB))

/*      Reset transmitter             */
#define UARTDM_CR_RESET_TX      (0x02 << HWIO_SHFT(UARTDM_CR,CHANNEL_COMMAND_LSB))

/*      Reset receiver                */
#define UARTDM_CR_RESET_RX      (0x01 << HWIO_SHFT(UARTDM_CR,CHANNEL_COMMAND_LSB))

/*      Reset Stale Interrupt         */
#define UARTDM_CR_RESET_STALE_INT   (0x08 << HWIO_SHFT(UARTDM_CR,CHANNEL_COMMAND_LSB))

/*      Reset Transmitter ready       */
#define UARTDM_CR_GEN_RESET_TX_RDY  (0x03 << HWIO_SHFT(UARTDM_CR,GENERAL_COMMAND))

/*      Force Stale                    */
#define UARTDM_CR_GEN_FORCE_STALE   (0x04 << HWIO_SHFT(UARTDM_CR,GENERAL_COMMAND))

/*      Enable Stale Event             */
#define UARTDM_CR_GEN_ENABLE_STALE  (0x05 << HWIO_SHFT(UARTDM_CR,GENERAL_COMMAND))

/*      Disable Stale Event            */
#define UARTDM_CR_GEN_DISBALE_STALE (0x06 << HWIO_SHFT(UARTDM_CR,GENERAL_COMMAND))

/* Interrupt Programming Register */
#define UARTDM_ISR_TX_READY          (0x01 << HWIO_SHFT(UARTDM_ISR, TX_READY))
#define UARTDM_ISR_RXSTALE           (0x01 << HWIO_SHFT(UARTDM_ISR, RXSTALE))
#define UARTDM_IPR_NEW_SAMPLE_MODE   0x40  /* New Sample Mode method          */
#define UARTDM_IPR_NEW_RXSTALE       0x20  /* New RXSTALE Interrupt method    */
#define UARTDM_IPR_STALE_LSB         0x1f  /* STALE_TIMEOUT_LSB               */
#define UARTDM_IPR_STALE_MSB         0x380 /* STALE_TIMEOUT_MSB               */

/*      Enable receiver               */
#define UARTDM_CR_RX_ENA        (0x01 << HWIO_SHFT(UARTDM_CR,UART_RX_EN))

/*      Enable transmitter            */
#define UARTDM_CR_TX_ENA        (0x01 << HWIO_SHFT(UARTDM_CR,UART_TX_EN))

/*      Tx FIFO at or below the mark  */
#define UARTDM_ISR_TXLEV        (0x01 << HWIO_SHFT(UARTDM_ISR,TXLEV))

/* UARTS w/512 byte FIFOs have some 16-bit control registers */
#define UARTDM_MSMU_RXWAT_OUT( val )     MSMUW_OUT( UARTDM_RFWR, val )
#define UARTDM_MSMU_TXWAT_OUT( val )     MSMUW_OUT( UARTDM_TFWR, val )
#define UARTDM_MSMU_MR1_OUT( val )       MSMUW_OUT( UARTDM_MR1, val )

/*-------------------------------------------------------------------------*/

/* Selection of which receiver status bits we consider to be errors. */

#define UARTDM_SR_RX_ERROR   ( UARTDM_SR_BREAK_RXD |       \
                               UARTDM_SR_PF_ERR    |       \
                               UARTDM_SR_OVR_ERR )

/* Setting for TX Watermark.  Set this to 30 rather than 31, because
   of the known bug in the UART. */
#define UARTDM_TXWAT_VAL     30

//#define UARTDM_DMRX_VALUE    0x100
#define UARTDM_DMRX_VALUE    0x190
#define UARTDM_BADR_VALUE    0x10

/* area for download packet, etc. = 8K */
#define  PKT_BUF_SIZE           (0x2000)

/* Start of packet buffer */
#define  PKT_BUFFER_BASE      (0xA000000-PKT_BUF_SIZE)


#define  MAX_PACKET_LEN    PKT_BUF_SIZE

#define MAX_DATA_LEN   1024

/* Error return code from uart_receive_byte */
/* This int value must not be any valid unsigned char value. */
#define  UART_RX_ERR    (-1)
#define  UART_NO_CHAR   (-2)
#define  UART_TIMEOUT   (-3)
#define  UART_ALL_OK    (0)

/* macro for 16 bit writes to MSM registers */
#define MSMUW_OUT( reg, val) HWIO_OUT( reg, val)  

#define bLoad8D(p)      (*(uint8*)(p))


typedef struct
{
  uint32      bytes_to_be_read;
  uint32      last_snap_value;
  uint32      rx_enabled;
  uint32      last_rx_buffer;
  uint32      snap_counter;
} uartdm_receive_status_type;

uartdm_receive_status_type uartdm_rx_status;

struct   incoming_packet 
{
  uint32     length;                   /** Number of bytes in buffer.            */
  uint32     start_location;           /** Location where the valid data starts  */
  uint32     end_location;             /** Location where the valid data ends    */
  uint32     step_crc;                 /** CRC on the incoming data         */
  uint32     packet_ready;             /** Is data in the packet ready      */
  byte       buffer[MAX_PACKET_LEN];   /** Data in the buffer.              */
};

struct incoming_packet pack_data;
dword byte_cnt = 0;

extern void
packet_handle_incoming (int ch);


//--------------------------------------------------------------------------
// Extern and Forward Declarations
//--------------------------------------------------------------------------

extern boolean uartdm_init (void);
extern boolean uartdm_active (void);
extern void do_uart_check (void);
extern void uartdm_drain (void);
extern void packet_handle_incoming (int ch);
extern int uartdm_receive_bytes (byte*   receive_ptr, uint32* len);
extern void uartdm_transmit_byte (byte);


//--------------------------------------------------------------------------
// Global Data
//--------------------------------------------------------------------------

/* 
 *  UART function dispatch table
 */ 
DISP uart_dispatch_table= {
   uartdm_init,
   uartdm_active,
   uartdm_drain,
   do_uart_check,
   uartdm_transmit_byte,
};



//--------------------------------------------------------------------------
// Function Declarations
//--------------------------------------------------------------------------

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
void uartdm_drain(void)
{
  /* Wait while the UART's transmitter drains.  This lets the ACK to
     the GO command that started FLASHPRG get back to the PC before
     we reset the UART. */
  while ( ( HWIO_IN(UARTDM_SR) & UARTDM_SR_TXEMT ) == 0)
  {
    KICK_WATCHDOG();     /* Don't let the watchdog expire while we wait  */
  }

}/* uartdm_drain() */


/*===========================================================================

FUNCTION uartdm_active

DESCRIPTION
  This function returns the status of whether the UART is active or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE = UART active
  FALSE = no activity on UART

SIDE EFFECTS
  Clears any receive errors on the UART

===========================================================================*/

boolean
uartdm_active (void)
{
  byte status; // to read and save the UART status register contents.
  int ret = FALSE;

  status = HWIO_IN(UARTDM_SR);
  if ((status & HWIO_FMSK(UARTDM_SR,RXRDY)) != 0)
  {
     /* Make sure that there are no errors logged in the status register.
      * If there are any errors, issue a reset on the error status and 
      * UART receiver. And, reenable the UART receiver.
      */

     if((status & UARTDM_SR_RX_ERROR) != 0)
     {
       HWIO_OUT (UARTDM_CR, UART2_CR_RESET_ERR);  /* Reset error status     */
       HWIO_OUT (UARTDM_CR, UART2_CR_RESET_RX);   /* Reset the receiver     */
       HWIO_OUT (UARTDM_CR, UART2_CR_RESET_RX);   /* Reset the receiver     */
       HWIO_OUT (UARTDM_CR, UART2_CR_RX_ENA);     /* Re-enable the receiver */
       ret = FALSE;
     }
     else
     {
       /*
        * Looks like the host is using the UART interface to communicate  
        * with the ARMPRG.  DO NOT READ anything from UART FIFO at this
        * point.  That will be done in main loop, even though there is
        * currently a character or more waiting.
        */ 
       ret = TRUE;
     }
  }   

  return ret;
}                               /* uartdm_active() */


/*===========================================================================

FUNCTION uartdm_reset_receiver

DESCRIPTION
  This function resets the receiver.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void uartdm_reset_receiver(void)
{
  HWIO_OUT(UARTDM_CR, UARTDM_CR_RESET_RX);    /* Reset the receiver     */
  HWIO_OUT(UARTDM_CR, UARTDM_CR_RESET_RX);    /* Reset the receiver     */
}/* uartdm_reset_receiver() */

/*===========================================================================

FUNCTION uartdm_enable_receiver

DESCRIPTION
  This function enables the receiver.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void uartdm_enable_receiver(void)
{
  HWIO_OUT(UARTDM_CR, UARTDM_CR_RX_ENA);      /* Enable the receiver    */
}/* uartdm_enable_receiver() */


/*===========================================================================

FUNCTION uartdm_reset_error_status

DESCRIPTION
  This function resets the error status.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void uartdm_reset_error_status(void)
{
  HWIO_OUT(UARTDM_CR, UARTDM_CR_RESET_ERR);   /* Reset error status     */
}/* uartdm_reset_error_status() */

/*===========================================================================

FUNCTION uartdm_init_local_receiver_status

DESCRIPTION
  This function initializes the local receiver status variable.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/
static void uartdm_init_local_receiver_status(void)
{
  uartdm_rx_status.bytes_to_be_read = 0;
  uartdm_rx_status.rx_enabled       = TRUE;
  uartdm_rx_status.last_rx_buffer   = 0;
  uartdm_rx_status.last_snap_value  = 0;
  uartdm_rx_status.snap_counter     = 0;
}/* uartdm_init_local_receiver_status() */

/*===========================================================================

FUNCTION uartdm_start_receiving_data

DESCRIPTION
  This function initializes the UARTDM to start receiving data.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/
void uartdm_start_receiving_data(void)
{
  HWIO_OUT(UARTDM_DMRX, UARTDM_DMRX_VALUE);
  HWIO_OUT(UARTDM_CR, UARTDM_CR_RESET_STALE_INT);
  HWIO_OUT(UARTDM_CR, UARTDM_CR_GEN_ENABLE_STALE);
}/* uartdm_start_receiving_data() */

/*===========================================================================

FUNCTION uartdm_init

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
boolean uartdm_init(void)
{
  uint32 chr_32;
  
  /**************************************************/
  /* Step . Issue commands                        */
  /**************************************************/

  

  /* Wait while the UART's reciever drains.  After resetting the UART, we
   * may have a couple of spurrious characters in the RX FIFO.  This can
   * happen especially when the phone is not plugged into a DIP, thus 
   * causing the UART to re-recognize the start of break condtion.
  */
  while ( ( HWIO_IN(UARTDM_SR) & UARTDM_SR_RXRDY ) != 0)
  {
    chr_32 = HWIO_IN(UARTDM_RF);    /* Get the received character */
    if ( chr_32 ) { }             /* Get rid of compiler warning */
    KICK_WATCHDOG();    /* Don't let the watchdog expire while we wait  */
  }

  /* INIT the receive status variable */
  uartdm_init_local_receiver_status();

  return TRUE;
}

/*===========================================================================

FUNCTION uartdm_init_receiver

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
void uartdm_init_receiver(void)
{
  uartdm_reset_error_status();
  uartdm_reset_receiver();
  uartdm_enable_receiver();
  uartdm_drain();
  uartdm_init_local_receiver_status();
  uartdm_start_receiving_data();
}

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
void uartdm_transmit_byte
(
  byte  chr
    /* Character to be transmitted */
)
{
  dword data;

  /* Wait until the transmit FIFO is not too full */
  while ((HWIO_IN(UARTDM_ISR) & UARTDM_ISR_TXLEV) == 0)
  {
    /* Don't let the watchdog expire while we wait */
    KICK_WATCHDOG();
  }

  data = chr;
  /* Set the output to 1 */
  HWIO_OUT(UARTDM_NO_CHARS_FOR_TX, 0x1);
  /* Put the byte into the Tx FIFO  */
  HWIO_OUT(UARTDM_TF, data);
  while ((HWIO_IN(UARTDM_ISR)&UARTDM_ISR_TX_READY) == 0)
  {
    /* Don't let the watchdog expire while we wait */
    KICK_WATCHDOG();
  }

  /* RESET_TX_RDY */
  HWIO_OUT(UARTDM_CR, UARTDM_CR_GEN_RESET_TX_RDY);      
  
} /* uart_transmit_byte() */


int uartdm_get_interface_status (void)
{
  int   status;
  int chr_32;

  /* Get the UART's status register */
  status = HWIO_IN(UARTDM_SR);

  if ((status & UARTDM_SR_RXRDY) == 0)
  {
    return UART_NO_CHAR;
  }

 if(((status = HWIO_IN(UARTDM_SR))&UARTDM_SR_RXRDY)==1) 
 {
    if ((status & UARTDM_SR_BREAK_RXD) !=0)
    {
      chr_32 = HWIO_IN(UARTDM_RF);     
      uartdm_reset_error_status();
      uartdm_reset_receiver();
      uartdm_enable_receiver();
      uartdm_drain();
      uartdm_init_local_receiver_status();
      uartdm_start_receiving_data();
      return UART_RX_ERR;
    }
    else if ((status & UARTDM_SR_PF_ERR) != 0)
    {
      chr_32 = HWIO_IN(UARTDM_RF);     
      uartdm_reset_error_status();
      uartdm_reset_receiver();
      uartdm_enable_receiver();
      uartdm_drain();
      uartdm_init_local_receiver_status();
      uartdm_start_receiving_data();
      return UART_RX_ERR;
    }
  }

  return status; /* Char Present, No errors */
}


/*===========================================================================

FUNCTION uartdm_receive_bytes

DESCRIPTION
  This function receives a 4 bytes from the UART by polling.

DEPENDENCIES
  The UART must be initialized and enabled, or else this routine will
  wait forever.

RETURN VALUE
  If a character is received without error, returns the value of the
  character received.  If an error occurs, returns UART_RX_ERR.  If a
  timeout occurs, returns UART_TIMEOUT.

SIDE EFFECTS

===========================================================================*/
int uartdm_receive_bytes
(
  byte*   receive_ptr, 
  uint32* len
)
{
  int   status;
  /* Return value from the UART's status register */
  uint32 chr_32=0;
  /* The character received from the UART */
  uint32 snap_value;
  uint32 bytes_read;  
  uint32 total_bytes_read;
  byte  *current_rx_ptr;
  uint32 i;
  uint32 dmrx_value;
  uint32  bytes_count;
  boolean handled_prev_read;
  uint32  loop_counter=0;
    
  *len = 0;

  
  /* Get the UART's status register */
  status = HWIO_IN(UARTDM_SR);
      
  if ((status & UARTDM_SR_RXRDY) == 0)
  {
    return UART_NO_CHAR;
  }

  if (uartdm_rx_status.rx_enabled == TRUE)
    HWIO_OUT( UARTDM_DMRX, UARTDM_DMRX_VALUE );

  current_rx_ptr = receive_ptr;
  bytes_read = 0;
  snap_value = 0;
  dmrx_value = 0;
  total_bytes_read = 0;
  loop_counter = 0;
  handled_prev_read = FALSE;


  while  (((status = HWIO_IN(UARTDM_SR))&UARTDM_SR_RXRDY)==1) 

  {
 
    if ((status & UARTDM_SR_BREAK_RXD) !=0)
    {
      chr_32 = HWIO_IN(UARTDM_RF);     
      uartdm_reset_error_status();
      uartdm_reset_receiver();
      uartdm_enable_receiver();
      uartdm_drain();
      uartdm_init_local_receiver_status();
      uartdm_start_receiving_data();
      return UART_RX_ERR;
    }
    else if ((status & UARTDM_SR_PF_ERR) != 0)
    {
      chr_32 = HWIO_IN(UARTDM_RF);     
      uartdm_reset_error_status();
      uartdm_reset_receiver();
      uartdm_enable_receiver();
      uartdm_drain();
      uartdm_init_local_receiver_status();
      uartdm_start_receiving_data();
      return UART_RX_ERR;
    }
	/* The following is added to take care of the wiered behaviour of hardware
	after receiving certain amount of data */
	
	if (byte_cnt >= 12 * MAX_PACKET_LEN) 
	{
	  uartdm_reset_error_status();
      uartdm_reset_receiver();
      uartdm_enable_receiver();
      uartdm_drain();
      uartdm_init_local_receiver_status();
      uartdm_start_receiving_data();
	  byte_cnt = 0;
      return UART_RX_ERR;
	}
    else
    {
      chr_32 = HWIO_IN(UARTDM_RF);

      if ( (uartdm_rx_status.rx_enabled == FALSE) && (handled_prev_read == FALSE))
      {
        if (uartdm_rx_status.bytes_to_be_read > 4)
        {
          bytes_count = 4;
          uartdm_rx_status.bytes_to_be_read = uartdm_rx_status.bytes_to_be_read - 4;
        }
        else
        {
          handled_prev_read = TRUE;
          bytes_count = uartdm_rx_status.bytes_to_be_read;
        }
      }
      else
      {
        dmrx_value = HWIO_IN(UARTDM_DMRX);
        bytes_count = dmrx_value;
        if ((dmrx_value == 0) || (dmrx_value >= 4))
        {
          bytes_count = 4;
        }
      }
     
      for (i=0; i<bytes_count; i++)
      {
        switch (i)
        {
          case 0:
           /* bStor8D((uint8*)current_rx_ptr++, (byte)((chr_32) & 0xff)); */      
		   *(uint8*)(current_rx_ptr++) = (byte)((chr_32) & 0xff);
            break;
          case 1:
           /* bStor8D((uint8*)current_rx_ptr++, (byte)((chr_32 >> 8) & 0xff));*/
		   *(uint8*)(current_rx_ptr++) = (byte)((chr_32 >> 8) & 0xff);
            break;
          case 2:
            /*bStor8D((uint8*)current_rx_ptr++, (byte)((chr_32 >> 16) & 0xff));      */
			 *(uint8*)(current_rx_ptr++) = (byte)((chr_32 >> 16) & 0xff); 
            break;
          case 3:
           /* bStor8D((uint8*)current_rx_ptr++, (byte)((chr_32 >> 24) & 0xff));*/
		   *(uint8*)(current_rx_ptr++) =  (byte)((chr_32 >> 24) & 0xff);
            break;
        }
      }

      /* Don't let the watchdog expire while we wait */
      KICK_WATCHDOG();
      bytes_read = bytes_read + bytes_count;
      byte_cnt = byte_cnt + bytes_count;

      /****************************************************** 
      ** If we already read the max number of data update 
      ** the DMRX register again
      *******************************************************/
      if (bytes_read >= (UARTDM_DMRX_VALUE))
      {
        HWIO_OUT( UARTDM_DMRX, UARTDM_DMRX_VALUE );
      }    

      //if (uartdm_rx_status.rx_enabled == FALSE)
      if (handled_prev_read == TRUE)
        break;
      
      loop_counter++;
    }       
  }
  
  uartdm_rx_status.last_rx_buffer   = chr_32;
  uartdm_rx_status.snap_counter++;
  
  if (uartdm_rx_status.rx_enabled == TRUE)
  {
    //snap_value = HWIO_IN (UARTDM_RX_TOTAL_SNAP); 
    //if ( (snap_value == 0) || ((loop_counter==1) && ((dmrx_value==0) || (dmrx_value>4))) )
    {
      HWIO_OUT(UARTDM_CR, UARTDM_CR_GEN_FORCE_STALE);
      snap_value = HWIO_IN (UARTDM_RX_TOTAL_SNAP);  
    }
    uartdm_rx_status.last_snap_value  = snap_value;

    if ( ((loop_counter==1) && ((dmrx_value==0) || (dmrx_value>4))) && (snap_value == 0))
    {
      /* It is difficult to know what is the correct number of bytes that are read
      ** in
      */
      bytes_read = 4;
    }
  }
  
  if (bytes_read < snap_value)
  {
    uartdm_rx_status.bytes_to_be_read = snap_value-bytes_read;
    uartdm_rx_status.rx_enabled       = FALSE;    
    total_bytes_read = bytes_read;
  }
  else
  {
    uartdm_rx_status.bytes_to_be_read = 0;
    uartdm_rx_status.rx_enabled       = TRUE;
    total_bytes_read = bytes_read;
  }

  if (uartdm_rx_status.rx_enabled == TRUE)
  {
    HWIO_OUT( UARTDM_CR, UARTDM_CR_RESET_STALE_INT);
    HWIO_OUT( UARTDM_CR, UARTDM_CR_GEN_ENABLE_STALE);
  }
  else if (snap_value >= UARTDM_DMRX_VALUE)
    HWIO_OUT( UARTDM_DMRX, UARTDM_DMRX_VALUE );

  *len = total_bytes_read; 
  return UART_ALL_OK;                 /* All OK, return the received character */
} /* uartdm_receive_bytes() */




/*===========================================================================

FUNCTION uartdm_packet_init

DESCRIPTION
  This function Initialize the packet variables.

DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None.

===========================================================================*/

void uart_packet_init (void)
{
  pack_data.length         = 0;
  pack_data.end_location   = 0;
  pack_data.start_location = 0;
  pack_data.step_crc       = 0xFFFF;
  pack_data.packet_ready   = FALSE;
}

/*===========================================================================

FUNCTION uart_get_data_from_device

DESCRIPTION
 This function stores a character received over the serial link in the
 receive buffer. It handles the escape character appropriately. 
 
DEPENDENCIES
  None.

RETURN VALUE
  Return length of received bytes

SIDE EFFECTS
  None.

===========================================================================*/
uint32 uart_get_data_from_device()
{
  int    status = UART_TIMEOUT;
  uint32 len = 0;
  /* pointer to buffer for receiving a packet */
  byte   *pkt_buf;

  pkt_buf = (byte *)&pack_data.buffer[pack_data.length];  
 
  KICK_WATCHDOG();     /* Don't let the watchdog expire */    
  status = uartdm_receive_bytes(pkt_buf, &len);
  return len;
}


/*===========================================================================

FUNCTION process_packets_uart

DESCRIPTION
  This function is the main loop implementing the DMSS Async Download
  Protocol.  It loops forever, processing packets as they arrive.

DEPENDENCIES
  All necessary initialization for normal CPU operation must have
  been performed, and the UART must have been initialized, before
  entering this function.

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None.

===========================================================================*/

void process_packets_uart(void)

{
  uint32 len;  /* Number of bytes received by the driver */
  uint32 i, len_inc;
  
   
  KICK_WATCHDOG();
  /* Init the packet to store data */
  uart_packet_init();
  /* Get data from the device */
  len = uart_get_data_from_device ();
    
  for (i=0, len_inc=pack_data.length; i<len; i++, len_inc++)
  {
    KICK_WATCHDOG();
	packet_handle_incoming (bLoad8D((uint8*)&pack_data.buffer[len_inc]));
  }

}

/*===========================================================================

FUNCTION do_uart_check
DESCRIPTION
  This function is checks if there is any data received by UART 

DEPENDENCIES
  

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None.

===========================================================================*/


void do_uart_check (void)
{
 process_packets_uart();
}




