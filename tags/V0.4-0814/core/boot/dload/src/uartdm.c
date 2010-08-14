/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   U A R T D M   D R I V E R

GENERAL DESCRIPTION
  This module contains a very simple-minded polling UART driver
  for the boot downloader facility.  
  
EXTERNALIZED FUNCTIONS
   uartdm_init
      Initializes the UART for transmit and receive.

   uartdm_receive_byte
      Receives a single byte from the UART.

   uartdm_transmit_byte
      Transmits a single byte to the UART.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  All necessary initialization for normal CPU operation must have
  been performed before entering this module.  dload_uart_init() should
  be called before any other functions.

  Copyright (c) 2007, 2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/dloaduart.c_v   1.4   18 Mar 2002 13:09:14   rprenove  $
$Header: //depot/asic/qsc1100/services/dload/uartdm.c#1 $ 
$DateTime: 2008/08/29 06:24:58 $ $Author: taaran $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/08   yli     No need to set enable bit at local LDO, set master bit on
                   global VREG enable register only
02/05/08   yli     Update SSBI code after code review
02/01/08   yli     Read Efuse to decide if VREG_MSME1 or VREG_UIM is used to drive UART2DM.
                   Needs to enable the module to get VREG output to UIM_PAD
01/21/08   yli     Update UART2DM GPIO setting according to config from Yucong Tao
01/15/08   tkuo    Fixed and add new wait cycle function to correct wait time 
                   and delay.
01/07/08   yli     First check-in for LCU boot ROM 2.0 (with UART2DM DL) code review
07/15/07   anb     Implemented UARTDM driver for Boot-ROM
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "boothw.h"
#include "boot_util.h"
#include "dloaduart.h"
#include "dloadarm.h"
#include "uartdm.h"

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



typedef struct
{
  uint32      bytes_to_be_read;
  uint32      last_snap_value;
  uint32      rx_enabled;
  uint32      last_rx_buffer;
  uint32      snap_counter;
} uartdm_receive_status_type;

uartdm_receive_status_type uartdm_rx_status;


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
    BOOTHW_KICK_WATCHDOG();     /* Don't let the watchdog expire while we wait  */
  }

}/* uartdm_drain() */

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

FUNCTION uartdm_reset_transmitter

DESCRIPTION
  This function resets the transmitter.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void uartdm_reset_transmitter(void)
{
  HWIO_OUT(UARTDM_CR, UARTDM_CR_RESET_TX);    /* Reset the transmitter  */
  HWIO_OUT(UARTDM_CR, UARTDM_CR_RESET_TX);    /* Reset the transmitter  */
}/* uartdm_reset_transmitter() */

/*===========================================================================

FUNCTION uartdm_enable_transmitter

DESCRIPTION
  This function enables the transmitter.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void uartdm_enable_transmitter(void)
{
  HWIO_OUT(UARTDM_CR, UARTDM_CR_TX_ENA);      /* Enable the transmitter */
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
void uartdm_init(void)
{
  uint32 chr_32;
  #ifdef BOOT_UART_SURF_DEBUG
#error code not present
  #endif
  

   HWIO_OUT(ARM_CLK_EN_REG, 0xFFFFFFFF); /* ARM_CLK_EN_REG*/
   HWIO_OUT (SW_RESET_REG, 0xf8000000);  /* SW_RESET_REG */
  /*************************************************************/
  /* Step 0. Setup ULC surf to enable UART_DM port behind SDCC */
  /*************************************************************/
  
#ifdef BOOT_UART_SURF_DEBUG
#error code not present
#endif
  /**************************************************/
  /* Step 1. Configure the GPIO's                   */
  /**************************************************/
  //gpio_tlmm_config(UART_TX_DATA);
  //gpio_tlmm_config(UART_RX_DATA);

  HWIO_OUT(GPIO_PAGE, 27); /* RX Data */
  HWIO_OUT( GPIO_CFG,    0x02 << HWIO_SHFT(GPIO_CFG, FUNC_SEL) |
                         0x01 << HWIO_SHFT(GPIO_CFG, GPIO_PULL)
          );

  HWIO_OUT(GPIO_PAGE, 25);  /* TX Data */
  HWIO_OUT( GPIO_CFG,    0x02 << HWIO_SHFT(GPIO_CFG, FUNC_SEL) |
                         0x01 << HWIO_SHFT(GPIO_CFG, GPIO_PULL)
          );

  HWIO_OUTM ( GPIO_OUT_0, 0x02000000, 0x00000000 );
  HWIO_OUTM ( GPIO_OE_0,  0x02000000, 0x02000000);

  /**************************************************
  * to get TCXO supplied to UARTDM core            *
  * This field selects the output of the clock source MUX.
  * 00 : ext_clk
  * 01 : xo
  * 10 : pll_src
  * 11 : sleep_clk  */
  /**************************************************/
  HWIO_OUTM(  UARTDM_CD, 
              HWIO_FMSK(UARTDM_CD, SRC_SEL),
              (1 << HWIO_SHFT(UARTDM_CD, SRC_SEL))
           );

  /**************************************************/  
  /* Set up the Mode.                               */
  /**************************************************/
  HWIO_OUTM(  UARTDM_CD, 
              HWIO_FMSK(UARTDM_CD, DIV_MND_MODE),
              (2 << HWIO_SHFT(UARTDM_CD, DIV_MND_MODE))
           );

  /**************************************************/
  /* Setup M and N value                            */
  /**************************************************/
  HWIO_OUT(UARTDM_MN, UARTDM_M_REG_VAL << 0x10 | UARTDM_N_REG_VAL );  
  
  /**************************************************/
  /* Setup D value                                  */
  /**************************************************/
  HWIO_OUTM(  UARTDM_CD, 
              HWIO_FMSK(UARTDM_CD, D_VAL),
              ((uint32)UARTDM_D_REG_VAL)
           );
  
  /****************************************************/
  /* Enable the MN Counter                            */ 
  /****************************************************/
  HWIO_OUTM(  UARTDM_CD, 
              HWIO_FMSK(UARTDM_CD, DIV_MND_EN),
              HWIO_FMSK(UARTDM_CD, DIV_MND_EN)
           );

  HWIO_OUTM(  UARTDM_CD, 
              HWIO_FMSK(UARTDM_CD, UART_DM_CLK_EN),
              HWIO_FMSK(UARTDM_CD, UART_DM_CLK_EN)
           );

  /**************************************************/
  /* UARTDM block: Turn on the clock                */
  /**************************************************/

  chr_32 = HWIO_IN (ARM_CLK_EN_REG);
  HWIO_OUT (ARM_CLK_EN_REG , chr_32| 1<<5); /* enable UART DM clk */

  /**************************************************/
  /* Step 4. Disable IRDA                           */
  /**************************************************/
  HWIO_OUT(UARTDM_IRDA, 0x00);

  /**************************************************/
  /* Step 5. Drain the UART                         */
  /**************************************************/
  //uartdm_drain();

  /**************************************************/
  /* Step 7. Setup MR1 & MR2 mode registers         */
  /**************************************************/
  /* Do not enable hardware flow control.  It works, but it causes
   * more trouble than it's worth with improperly-wired cables.
   * There's really no need for it anyway, since the main data flow
   * is from the host to the phone, and the phone (having nothing
   * better to do than service the UART) will always keep up.
   */
  UARTDM_MSMU_MR1_OUT(0);
  HWIO_OUT(UARTDM_MR2, UARTDM_MR2_8BPC | UARTDM_MR2_1SB); /* 8N1 */

  /**************************************************/
  /* Step 8. Mask all interrupts off                */
  /**************************************************/
  HWIO_OUT(UARTDM_IMR, 0);

  /**************************************************/
  /* Step 9. Setup Tx and Rx Watermarks             */
  /**************************************************/
  /* Receive watermark at zero.  This means an interrupt would be generated 
   * whenever any (more than zero) characters are in the Rx FIFO.  Since we 
   * don't use Rx interrupts, this doesn't really matter.
   */
  UARTDM_MSMU_RXWAT_OUT(0);
  
  /* Transmit watermark.  We DO use this, to determine if there is
   * room for one more character in the Tx FIFO.
   */
  UARTDM_MSMU_TXWAT_OUT(UARTDM_TXWAT_VAL);  
  
  /**************************************************/
  /* Step 10. Setup the BaudRate                    */
  /**************************************************/
  HWIO_OUT( UARTDM_CSR, UARTDM_CSR_1152K_BPS);

  HWIO_OUT( UARTDM_IPR, (0x31 & UARTDM_IPR_STALE_LSB) |
                         ((0x31 << 2) & UARTDM_IPR_STALE_MSB) |
                         UARTDM_IPR_NEW_RXSTALE |
                         UARTDM_IPR_NEW_SAMPLE_MODE
          );
  
  /**************************************************/
  /* Step 11. Disable DM                            */
  /**************************************************/
  HWIO_OUT(UARTDM_DMRX, UARTDM_DMRX_VALUE);
  HWIO_OUT(UARTDM_DMEN, 0x0);
  HWIO_OUT(UARTDM_CR, UARTDM_CR_RESET_RX);
  
  /**************************************************/
  /* Step 12. Disable DM                            */
  /**************************************************/  
  HWIO_OUT(UARTDM_BADR, UARTDM_BADR_VALUE);

  /**************************************************/
  /* Step 13. Issue commands                        */
  /**************************************************/
  uartdm_reset_error_status();
  uartdm_reset_receiver();
  uartdm_reset_transmitter();
  uartdm_enable_receiver();
  uartdm_enable_transmitter();

  /* Wait while the UART's reciever drains.  After resetting the UART, we
   * may have a couple of spurrious characters in the RX FIFO.  This can
   * happen especially when the phone is not plugged into a DIP, thus 
   * causing the UART to re-recognize the start of break condtion.
  */
  while ( ( HWIO_IN(UARTDM_SR) & UARTDM_SR_RXRDY ) != 0)
  {
    chr_32 = HWIO_IN(UARTDM_RF);    /* Get the received character */
    if ( chr_32 ) { }             /* Get rid of compiler warning */
    BOOTHW_KICK_WATCHDOG();    /* Don't let the watchdog expire while we wait  */
  }

  /* INIT the receive status variable */
  uartdm_init_local_receiver_status();
  uartdm_start_receiving_data();

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
    BOOTHW_KICK_WATCHDOG();
  }

  data = chr;
  /* Set the output to 1 */
  HWIO_OUT(UARTDM_NO_CHARS_FOR_TX, 0x1);
  /* Put the byte into the Tx FIFO  */
  HWIO_OUT(UARTDM_TF, data);
  while ((HWIO_IN(UARTDM_ISR)&UARTDM_ISR_TX_READY) == 0)
  {
    /* Don't let the watchdog expire while we wait */
    BOOTHW_KICK_WATCHDOG();
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

FUNCTION uartdm_receive_byte

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
int uartdm_receive_byte
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

  while (((status = HWIO_IN(UARTDM_SR))&UARTDM_SR_RXRDY)==1)
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
      BOOTHW_KICK_WATCHDOG();
      bytes_read = bytes_read + bytes_count;

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
} /* uartdm_receive_byte() */

