/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          S I O R S 2 3 2   P D

                   SIO Real Device Layer for UART

GENERAL DESCRIPTION

  This Module is the real physical device layer of DMSS Serial I/O for an 
  UART hardware.  It takes care of all of the hardware dependent functionalities 
  of Serial I/O.  Only asynchronous mode is supported.

EXTERNALIZED FUNCTIONS

  siors_pd_initialize
    This function initialize the actual driver layer and 
    populate the exported API table so the media driver layer can access 
    the physical driver layer code

PDD LAYER EXPORTED FUNCTIONS
  siors_pd_enable_tx_hardware_fctl
    Enable hardware flow control on transmitter

  siors_pd_disable_tx_hardware_fctl
    Disable hardware flow control on transmitter

  siors_pd_enable_rx_hardware_fctl
    Enable hardware flow control on receiver

  siors_pd_disable_rx_hardware_fctl
    Disable hardware flow control on receiver

  siors_pd_enable_transmit_interrupt
    Enable transmit interrupt
  
  siors_pd_disable_transmit_interrupt
    Disable transmit interrupt

  siors_pd_enable_auto_hrdwr_fctl
    Enable automatic hardware flow control on receiver

  siors_pd_disable_auto_hrdwr_fctl
    Disable automatic hardware flow control on receiver

  siors_pd_set_bit_rate
    Set the current bit rate on the UART

  siors_pd_reset_transmitter
    Reset the transmitter

  siors_pd_enable_transmitter
    Enable the transmitter

  siors_pd_reset_receiver
    Reset the receiver

  siors_pd_enable_receiver
    Enable the receiver

  siors_pd_disable_receiver
    Disable the receiver

  siors_pd_open
    Called when the port is opened for communication

  siors_pd_close
    Called when the port is closed
   
  siors_pd_transmit
    Transmit a packet

  siors_pd_receive
    Receive a packet

  siors_pd_receive_tail_char
    Receive a packet with detection on the tail character
   
  siors_pd_get_current_rts
    Get the current status on the RTS line

  siors_pd_is_tx_fifo_empty
    Check to see if the TX FIFO is empty
   
  siors_pd_check_rx_cable_status
    Check to see if the RX line is in a extended marked state, suggesting
    the cable has been disconnected

  siors_pd_detect_rx_activity
    Enable the detection of RX activity
  
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  siors_xxx_initialize() must be called prior to accessing the exported
  API table.

Copyright (c) 2007-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors232_pd.c#6 $ $DateTime: 2009/02/11 17:20:36 $ $Author: cmihalik $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/08   rh      FIX: Tail character compare routine is reading the wrong byte
04/03/08   rh      Enable the driver only if the correct HW type is selected
03/18/08   rh      Add include for "msm.h"
12/17/07   rh      Increase the RFR set level so it is above RX_LEV
09/24/07   rh      Add new quick open option for power up/down event
08/15/07   rh      Make sure RX detection is disabled when port is closed
08/10/07   rh      Add RX activity detection
08/09/07   rh      Fix issues that cause RX to fail
07/26/07   rh      Required Koel change merged
05/04/07   rh      Created file from siors232.c#23
===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "target.h"                /* Target specific definitions          */
#include "comdef.h"                /* Definitions for byte, word, etc.     */
#include "customer.h"              /* Customer Specific Definitions        */
#include "processor.h"             /* Definition of CPU registers          */
#include "memory.h"                /* Memory copy/set routine prototypes   */
#include "bsp.h"

#include "rex.h"                   /* REX multitasking package             */
#include "sio.h"                   /* Serial I/O service definitions       */
#include "msm.h"                   /* Required for HAL and TRAMP 2         */

#include "err.h"                   /* Error services definitions           */
#include "tramp.h"                 /* Trampoline typedefs and prototypes   */
#include "clk.h"                   /* Clock services definitions           */
#include "clkregim.h"              /* Clock Regime Definitions             */
#include "hw.h"                    /* Hardware Utilities                   */
#include "assert.h"                /* ASSERT macro unitilities             */
#include "siorsreg.h"              /* SIO RS232 UART Register Definitions  */
#include "siors_lib.h"
#include "siors232_pd.h"           /* SIO RS232 Device Layer Header File   */
#include "siors232_trace.h"        /* For the debug trace feature          */
#include "uart_compat.h"           /* Compatability layer                  */

//  If there is is at least one UART.
#if( defined(FEATURE_FIRST_UART)  || \
     defined(FEATURE_SECOND_UART) || \
     defined(FEATURE_THIRD_UART))              //{

//  Remove entire file if no legacy UART driver in use.
#if((SIORS_FIRST_UART_TYPE  == HW_UART) ||\
    (SIORS_SECOND_UART_TYPE == HW_UART) ||\
    (SIORS_THIRD_UART_TYPE  == HW_UART))       //{

/*===========================================================================

                FEATURE FLAG RESOLUTION - Keep the mess here

===========================================================================*/
#ifndef FEATURE_UART_TCXO_CLK_FREQ
#if defined(FEATURE_JCDMA_DS) || defined(FEATURE_JCDMA_SIO)
#define SIORS_HAS_BAUD_76800
#define SIORS_HAS_BAUD_14400
#endif
#endif  /* TCXO CLK FREQUENCY */

#ifdef FEATURE_UART_TCXO_CLK_FREQ
#define SIORS_HAS_BAUD_230400
#ifdef FEATURE_UART_RATE1
#define SIORS_HAS_BAUD_460800
#endif
#endif

#if defined(FEATURE_SIO_PNP)
#error code not present
#endif
/* Most 6K targets uses a different clock regime APIs */
#ifndef FEATURE_DUAL_CLK_RGM
#define CLK_REGIME_MSM_DISABLE(x)    clk_regime_disable(x)    
#define CLK_REGIME_MSM_ENABLE(x)     clk_regime_enable(x)    
#else
#define CLK_REGIME_MSM_DISABLE(x)    clk_regime_msm_disable(x)
#define CLK_REGIME_MSM_ENABLE(x)     clk_regime_msm_enable(x)
#endif

/* Only enable UART support if it is selected as UART */
#if (SIORS_FIRST_UART_TYPE != HW_UART)
#undef FEATURE_FIRST_UART
#endif

#if (SIORS_SECOND_UART_TYPE != HW_UART)
#undef FEATURE_SECOND_UART
#endif

/* If UART need to vote for UIM VREG, enable power control */
#if (SIORS_SECOND_UART_UIM_PWR_VOTE == TRUE)
#include "pmapp.h"
#define SIORS_PD_ENABLE_PM_UIM_VREG_P2()   \
  {if(rs_port->device_id == UART_DEVICE_2){  \
   pm_vote_vreg_switch(PM_ON_CMD, PM_VREG_RUIM_ID, \
                        PM_VOTE_VREG_RUIM_APP__RUIM);}}

#define SIORS_PD_DISABLE_PM_UIM_VREG_P2() \
  {if(rs_port->device_id == UART_DEVICE_2){  \
   pm_vote_vreg_switch(PM_OFF_CMD, PM_VREG_RUIM_ID, \
                        PM_VOTE_VREG_RUIM_APP__RUIM);}}
#else
#define SIORS_PD_ENABLE_PM_UIM_VREG_P2()   
#define SIORS_PD_DISABLE_PM_UIM_VREG_P2()   
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================
 General constant and variable definitions.
===========================================================================*/
/* Data xfer mode(s) RXLEV - used for auto-baud.                           */
#define SIO_UART_FLOW_RXLEV          14

/* Autodetect mode RXLEV when Autobaud disabled                            */
#define DS_AUTODETECT_RXLEV          1

/* Autodetect mode RXLEV when Discarding is active                         */
#define DS_AD_DISCARD_RXLEV          20

/* Setting for TX FIFO interrupt.                                          */
#define SIO_FLOW_TXLEV               2
#define SIO_FLOW_TXLEV_EMPTY         0

/* RXLEV for data transfers at different baud rates                        */
#define SIO_LOW_RATE_FLOW_RXLEV      31         /* Less than 9600          */
#define SIO_9600_FLOW_RXLEV          30         /* Also used for autobaud  */
#define SIO_14400_FLOW_RXLEV         56
#define SIO_19200_FLOW_RXLEV         56
#define SIO_38400_FLOW_RXLEV         49
#define SIO_57600_FLOW_RXLEV         46
#define SIO_76800_FLOW_RXLEV         40
#define SIO_115200_FLOW_RXLEV        38
#define SIO_230400_FLOW_RXLEV        29  
#define SIO_460800_FLOW_RXLEV        29

/* The FIFO water level marker for auto generation of RFR, reserve 32 bytes */
#define SIO_512BYTE_FIFOS_RFR_LVL    512 - 32
#define SIO_64BYTE_FIFOS_RFR_LVL     64 - 8

/* RXLEV for data transfers at rates higher than 38400 (e.g. 115.2Kbps) for
   packet calls when using hardware flow control.                          */
#ifdef MSMHW_UART1_512BYTE_FIFOS
#if (defined(FEATURE_DS_IS2000) || defined(FEATURE_HDR) || defined(FEATURE_DATA) )   
#define SIO_PKT_HIGH_RATE_FLOW_RXLEV 356  /* OTA frame payload w/ supp chs */
#endif 
#endif /* MSMHW_UART1_512BYTE_FIFOS */

/* Size of the FIFO. */
/* This is used for calculating the number of bytes that can be written 
   into TX FIFO when TX interrupt occurs.  Only used for TX. */
#ifdef MSMHW_UART1_512BYTE_FIFOS
#define SIO_FIFO_PORT_MAIN_SIZE     512
#endif
#define SIO_FIFO_SIZE                64

/* Set a upper bound for number of bytes TX_ISR can transit per interrupt. 
   the value is adjust to tx_isr during < 1 ms.  */
#define RS232_TX_UPPER_BOUND        130

/* It is essential that SIO not receive interrupts too often.  For instance, 
 * if this contant is set too low, incoming data stream could be coming in at 
 * such a rate that SIO gets interrupted too often.  Because there is overhead 
 * cost for each interrupt, such a scenario can cause SIO to use enough CPU time 
 * to cause performance problems, such as watch-dog timeouts.  However, if the 
 * same RXSTALE values are kept for all baudrates, incorrect behavior will 
 * result (e.g. false postive packet detection).  That is why the RXSTALE 
 * values are different for different baudrates.                           */
#define SIO_FLOW_RXSTALE_HIGH_BITRATE 31
#define SIO_FLOW_RXSTALE_19200        16
#define SIO_FLOW_RXSTALE_14400        12
#define SIO_FLOW_RXSTALE_9600          8
#define SIO_FLOW_RXSTALE_4800          4
#define SIO_FLOW_RXSTALE_2400          2
#define SIO_FLOW_RXSTALE_1200          1
#define SIO_FLOW_RXSTALE_LOW_RATE      1

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*---------------------------------------------------------------------------
 The following array contains the port structure.  All the required 
 information/variable is stored in this structure. 
---------------------------------------------------------------------------*/
siors_port_type    siors_pdd_ports[UART_NUM_DEVICES];

/* The following constants indicate the amount of time (in milliseconds) 
   it takes for a character to be completely received or transmitted, at
   the given bit-rate.  The figure is meant to be conservatively high.     */
#define  SIO_300BPS_SINGLE_CHAR_TIME                55
#define  SIO_600BPS_SINGLE_CHAR_TIME                30
#define  SIO_1200BPS_SINGLE_CHAR_TIME               17
#define  SIO_MED_BITRATE_SINGLE_CHR_TIME            10

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*---------------------------------------------------------------------------
                      Function prototypes 
---------------------------------------------------------------------------*/
static void siors_pd_set_rxlev( siors_port_type * );
static void siors_pd_reset_receiver( void * );
static void siors_pd_reset_transmitter( void * );
static void siors_pd_enable_receiver( void * );
static void siors_pd_set_bit_rate (void *, sio_bitrate_type);
static void siors_pd_enable_transmitter ( void * );
static void siors_pd_set_uart_clock_div_regs ( siors_port_type * , word , word , word );
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/


/*===========================================================================

                       FUNCTIONs, ISRs and MACROs

===========================================================================*/
/*===========================================================================

FUNCTION SIORS_PD_ENABLE_HARDWARE_FCTL                           EXPORTED

DESCRIPTION
  This procedure enables hardware flow control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Hardware will heed hardware flow control lines and not transmit when
  RFR is disabled.

===========================================================================*/

static void siors_pd_enable_tx_hardware_fctl ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  rs_port->gv.sio_mr1_shadow |= MSMU_MR1_CTSC;
  MSMU_OUT( rs_port->hw.msmu_mr1, rs_port->gv.sio_mr1_shadow);
}


/*===========================================================================

FUNCTION SIORS_PD_DISABLE_HARDWARE_FCTL                          EXPORTED

DESCRIPTION
  This procedure disables hardware flow control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Hardware will no longer heed hardware flow control lines.  Changes in
  state of RFR line will not affect the phone.

===========================================================================*/

static void siors_pd_disable_tx_hardware_fctl ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }
  
  rs_port->gv.sio_mr1_shadow &= ~MSMU_MR1_CTSC;
  MSMU_OUT(rs_port->hw.msmu_mr1, rs_port->gv.sio_mr1_shadow);
} 


/*===========================================================================

FUNCTION SIORS_PD_ENABLE_TRANSMIT_INTERRUPT                         EXPORTED

DESCRIPTION
  This routine enables the transmit interrupt.
  Setting the empty_chk to true if TX interrupt should be fired only
  when the FIFO is truly empty

DEPENDENCIES
  This routine should only be called when there is something to transmit.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_pd_enable_transmit_interrupt
(
  void *pHead,
  boolean            empty_chk            /* TX ISR only if FIFO is empty  */
)
{
  siors_port_type *rs_port = pHead;
  siors_hw_reg_type     *hwreg;           /* UART HW regs                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  hwreg = &(rs_port->hw);

  if(empty_chk)
  {
    MSMU_OUT( hwreg->msmu_tfwr, SIO_FLOW_TXLEV_EMPTY);
  }
  else
  {
    MSMU_OUT( hwreg->msmu_tfwr, SIO_FLOW_TXLEV);
  }

  rs_port->gv.sio_imr_shadow |= MSMU_IMR_TXLEV;
  MSMU_OUT(hwreg->msmu_imr, rs_port->gv.sio_imr_shadow);
} 


/*===========================================================================

FUNCTION SIORS_PD_DISABLE_TRANSMIT_INTERRUPT                    EXPORTED

DESCRIPTION
  This routine disables the transmit interrupt.  Call this routine when the
  last byte in the TX queue is transmitted.  If this routine is not called
  and TX interrupt is not disabled by other means, the phone will hang.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_pd_disable_transmit_interrupt ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  rs_port->gv.sio_imr_shadow &= ~MSMU_IMR_TXLEV;
  MSMU_OUT(rs_port->hw.msmu_imr, rs_port->gv.sio_imr_shadow);
}


/*===========================================================================

FUNCTION SIORS_PD_ENABLE_AUTO_HRDWR_FCTL                         EXPORTED

DESCRIPTION
  This procedure enables automatic RX hardware flow control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_pd_enable_auto_hrdwr_fctl ( void *pHead )
{
  word uart_flow_rxlev;                   /* Uart RXLEV value              */
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  /* Determine the RFR level base on the FIFO size */
  if(rs_port->is_ext_fifo)
  {
      rs_port->gv.sio_uart_flow_rx_rfr_lev = SIO_512BYTE_FIFOS_RFR_LVL;
  }
  else
  {
      rs_port->gv.sio_uart_flow_rx_rfr_lev = SIO_64BYTE_FIFOS_RFR_LVL;
  }
    
  uart_flow_rxlev = 
      ((rs_port->gv.sio_uart_flow_rx_rfr_lev) & MSMU_MR1_RFR_LVL_MASK);
  rs_port->gv.sio_mr1_shadow &= ~MSMU_MR1_RFR_LVL_MASK;  

  if(rs_port->is_ext_fifo)
  {
      uart_flow_rxlev |= ((rs_port->gv.sio_uart_flow_rx_rfr_lev << 2) 
              & MSMU_MR1_RFR_LVL_HI_MASK);
      rs_port->gv.sio_mr1_shadow &= ~MSMU_MR1_RFR_LVL_HI_MASK;
  }

  rs_port->gv.sio_mr1_shadow |= ( MSMU_MR1_RFRC | uart_flow_rxlev);
  MSMU_OUT(rs_port->hw.msmu_mr1, rs_port->gv.sio_mr1_shadow);
  rs_port->gv.inbound_auto_hw_flow_enabled = TRUE;
} 


/*===========================================================================

FUNCTION SIORS_PD_DISABLE_AUTO_HRDWR_FCTL                        EXPORTED

DESCRIPTION
  This procedure disables automatic RX hardware flow control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_pd_disable_auto_hrdwr_fctl ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  rs_port->gv.sio_mr1_shadow &= ~MSMU_MR1_RFRC;
  MSMU_OUT(rs_port->hw.msmu_mr1, rs_port->gv.sio_mr1_shadow);
  rs_port->gv.inbound_auto_hw_flow_enabled = FALSE;
} 


/*===========================================================================

FUNCTION SIORS_PD_INBOUND_FLOW_ENABLE                      EXPORTED

DESCRIPTION
  Given RS232 enable in-bound flow control (HW).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_pd_enable_rx_hardware_fctl ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  if (!rs_port->gv.inbound_auto_hw_flow_enabled )
  {
      /* HW flow control.  Set RFR pin on MSM to Assert CTS. */
      MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_SET_RFR);
  }
  else
  {
      /* Nothing need to be done if auto RFR is used, 
       * it is up to the hardware to take care of the RFR signal          */
      rs_port->gv.sio_mr1_shadow |= MSMU_MR1_RFRC;
      MSMU_OUT(rs_port->hw.msmu_mr1, rs_port->gv.sio_mr1_shadow);
  }
  rs_port->gv.sio_inbound_flow_enabled = TRUE;
} 


/*===========================================================================

FUNCTION SIORS_PD_INBOUND_FLOW_DISABLE                     EXPORTED

DESCRIPTION
  Given RS232, disable in-bound flow using appropriate flow contol (HW)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_pd_disable_rx_hardware_fctl ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  if (!rs_port->gv.inbound_auto_hw_flow_enabled )
  {
      /*--------------------------------------------------------------------
       HW Flow control. First disable automatic hardware flow control, then 
       reset RFR pin on MSM to Deassert CTS
      --------------------------------------------------------------------*/
      MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_RFR);
  }
  else
  {  
      /*-------------------------------------------------------------------
       * If flow control is going to be disabled, make sure the manual flow 
       * is disabled using the command register before switching back to 
       * manual flow control.  This way, when the flow control
       * mode is switched back to manual control, the output on the RFR 
       * pin is deasserted.
       * ---------------------------------------------------------------- */
      INTLOCK();
      MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_RFR);
      siors_pd_disable_auto_hrdwr_fctl(rs_port);
      MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_RFR);
      INTFREE();
  }
  rs_port->gv.sio_inbound_flow_enabled = FALSE;
} 



/*===========================================================================

FUNCTION SIORS_PD_DETECT_RX_ACTIVITY                        EXPORTED

DESCRIPTION
  This function activate the RX activity detection routine.  A callback is
  generated when RX activity is detected (A high on the RX line, leaving
  the marked state, signal RX is going into active state)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function will only generate one callback when RX activity is detected.
  This function can only be called when the port is closed
  
===========================================================================*/
static void siors_pd_detect_rx_activity( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* If detection is already activated, exit right away */
  if(rs_port->gv.is_wakeup_isr_registered){
    return;
  }

  /* Make sure the port is closed */
  ASSERT(rs_port->gv.is_isr_registered == FALSE);

  /* Register the wake up interrupt and exit */
  tramp_set_isr(rs_port->gv.wakeup_isr_type, rs_port->gv.wakeup_isr_func_ptr);
  rs_port->gv.is_wakeup_isr_registered = TRUE;
}



/*===========================================================================

FUNCTION SIORS_PD_DP_RX_COMMON_ISR                              INTERNAL

DESCRIPTION
  This ISR is triggered when RX activity is detected

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_pd_dp_rx_common_isr ( siors_port_type *rs_port )
{
  /* Make sure we actually want to get this ISR */
  ASSERT(rs_port->gv.is_wakeup_isr_registered);
  tramp_set_isr( rs_port->gv.wakeup_isr_type, NULL);
  rs_port->gv.is_wakeup_isr_registered = FALSE;
  /* Call the upper layer */
  rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_RX_ACTIVITY);
} 



/*===========================================================================

FUNCTION SIORS_DM_DP_RX_ISRx                                   INTERNAL

DESCRIPTION
  This ISR is triggered when RX activity is detected

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_FIRST_UART
static void siors_pd_dp_rx_isr1()
{
  siors_pd_dp_rx_common_isr(&(siors_pdd_ports[UART_DEVICE_1]));
} 
#endif

#ifdef FEATURE_SECOND_UART
static void siors_pd_dp_rx_isr2()
{
  siors_pd_dp_rx_common_isr(&(siors_pdd_ports[UART_DEVICE_2]));
} 
#endif

#ifdef FEATURE_THIRD_UART
static void siors_pd_dp_rx_isr3()
{
  siors_pd_dp_rx_common_isr(&(siors_pdd_ports[UART_DEVICE_3]));
} 
#endif



/*===========================================================================

FUNCTION SIORS_PD_SET_UART_CLOCK_DIV_REGS                          INTERNAL

DESCRIPTION
  This function will set the value of the m, n, d, mnd register.  
  The shadow register will not be updated if 
  siors_set_uart_clock_div_regs_no_shadow is used.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_pd_set_uart_clock_div_regs
(
    siors_port_type *rs_port,           /* Port                         */
    word  m_val,                        /* Value of m reg to set to     */
    word  n_val,                        /* Value of n reg to set to     */
    word  d_val                         /* Value of d reg to set to     */
)
{
#ifdef MSMHW_UART_MREG_13BIT_NREG_15BIT_DREG_13BIT
#define SIORS_UART_HAS_16BIT_MND_REG    1
#else
#define SIORS_UART_HAS_16BIT_MND_REG    0
#endif

    if(SIORS_UART_HAS_16BIT_MND_REG)
    {
        MSMU_OUT( rs_port->hw.msmu_mvr,  m_val);    /* UARTDM always has 16 bit registers */
        MSMU_OUT( rs_port->hw.msmu_nvr,  n_val);
        MSMU_OUT( rs_port->hw.msmu_dvr,  d_val);
    }
    else
    {
     /* The M/N/D register is always stored as 16 bit value, generate the 
      * split value at run time */
        MSMU_OUT( rs_port->hw.msmu_mvr,  (m_val >> 1));
        MSMU_OUT( rs_port->hw.msmu_nvr,  (n_val >> 3));
        MSMU_OUT( rs_port->hw.msmu_dvr,  (d_val >> 2));
        if ( rs_port->hw.msmu_mndr )
        {
            MSMU_OUT( rs_port->hw.msmu_mndr, ((m_val & 0x01) << 5) | 
                  ((n_val & 0x07) << 2) | ((d_val & 0x03)));
        }
    }

    rs_port->gv.sio_mvr_shadow = m_val;
    rs_port->gv.sio_nvr_shadow = n_val;
    rs_port->gv.sio_dvr_shadow = d_val;
}


/*===========================================================================

FUNCTION SIORS_PD_SET_BIT_RATE                              EXPORTED

DESCRIPTION
  This function will set the bit rate to the bit rate specified and also
  do the book-keeping and clean-up necessary when changing bit-rates.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void siors_pd_set_bit_rate
(
  void                *pHead,
  sio_bitrate_type     req_bitrate         /* specified baud rate for UART */
)
{
  siors_hw_reg_type      *hwreg;           /* UART HW regs                  */
  /* Clock Select Register value -  initialized to removed compiler warning*/
  byte  temp_csr_value = MSMU_CSR_192K_BPS;             
  /* RXSTALE timer value - initialized to removed compiler warning         */
  word  flow_rxstale = SIO_FLOW_RXSTALE_19200;
  siors_port_type *rs_port = pHead;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hwreg = &(rs_port->hw);

  if (sio_is_baud_valid(req_bitrate))
  {
    rs_port->gv.sio_current_rx_bitrate = req_bitrate;
    rs_port->gv.sio_current_tx_bitrate = req_bitrate;
  }

  if (!rs_port->is_clock_enabled){
    return;
  }
  /*-------------------------------------------------------------------------
   Program M register to default value.  Current bit-rate may have been one
   of the non-standard bit-rates.  All four registers have to be written
   even if one value changes. 
   We also restore shadow registers to their defaults since the previous 
   rate may have been non-standard.
  -------------------------------------------------------------------------*/
  siors_pd_set_uart_clock_div_regs( rs_port,
                                rs_port->gv.sio_mvr_default,
                                rs_port->gv.sio_nvr_default,
                                rs_port->gv.sio_dvr_default);
  /*-------------------------------------------------------------------------
   Set UART baud rate according to passed parameter. NOTE: The UART RXLEV
   flow variable is set to provide a max 4ms interrupt latency (before an RX
   overrun would occur).
  -------------------------------------------------------------------------*/
  switch( req_bitrate)
  {
    case SIO_BITRATE_300:
      /*---------------------------------------------------------------------
       Set the UART to 300 baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_300_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_LOW_RATE;
      rs_port->gv.sio_single_char_time = SIO_300BPS_SINGLE_CHAR_TIME;
      break;

    case SIO_BITRATE_600:
      /*---------------------------------------------------------------------
       Set the UART to 600 baud.
      ---------------------------------------------------------------------*/

      temp_csr_value                   = MSMU_CSR_600_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_LOW_RATE;
      rs_port->gv.sio_single_char_time = SIO_600BPS_SINGLE_CHAR_TIME;
      break;

    case SIO_BITRATE_1200:
      /*---------------------------------------------------------------------
       Set the UART to 1200 baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_1200_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_1200;
      rs_port->gv.sio_single_char_time = SIO_1200BPS_SINGLE_CHAR_TIME;
      break;

    case SIO_BITRATE_2400:
      /*---------------------------------------------------------------------
       Set the UART to 2400 baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_2400_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_2400;
      rs_port->gv.sio_single_char_time = SIO_MED_BITRATE_SINGLE_CHR_TIME;
      break;

    case SIO_BITRATE_4800:
      /*---------------------------------------------------------------------
       Set the UART to 4800 baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_4800_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_4800;
      rs_port->gv.sio_single_char_time = SIO_MED_BITRATE_SINGLE_CHR_TIME;
      break;

    case SIO_BITRATE_9600:
      /*---------------------------------------------------------------------
       Set the UART to 9600 baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_9600_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_9600;
      rs_port->gv.sio_single_char_time = SIO_MED_BITRATE_SINGLE_CHR_TIME;
      break;

#ifdef SIORS_HAS_BAUD_14400 
    case SIO_BITRATE_14400:
      /*---------------------------------------------------------------------
       Set the UART to 14.4K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_144K_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_14400;
      rs_port->gv.sio_single_char_time = SIO_MED_BITRATE_SINGLE_CHR_TIME;
      break;
#endif 

    case SIO_BITRATE_19200:
      /*---------------------------------------------------------------------
       Set the UART to 19.2K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_192K_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_19200;
      rs_port->gv.sio_single_char_time = SIO_MED_BITRATE_SINGLE_CHR_TIME;
      break;

    case SIO_BITRATE_38400:
      /*---------------------------------------------------------------------
       Set the UART to 38.4K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_384K_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      rs_port->gv.sio_single_char_time = SIO_MED_BITRATE_SINGLE_CHR_TIME;
      break;

    case SIO_BITRATE_57600:
      /*---------------------------------------------------------------------
       Set the UART to 57.6K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_576K_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      rs_port->gv.sio_single_char_time = SIO_MED_BITRATE_SINGLE_CHR_TIME;
      break;

#ifdef SIORS_HAS_BAUD_76800 
    case SIO_BITRATE_76800:
      /*---------------------------------------------------------------------
       Set the UART to 76.8K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_768K_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      rs_port->gv.sio_single_char_time = SIO_MED_BITRATE_SINGLE_CHR_TIME;
      siors_set_uart_clock_div_regs( rs_port,
                                MSMU_MVR_DEFAULT_76800,
                                MSMU_NVR_DEFAULT_76800,
                                MSMU_DVR_DEFAULT_76800);
      break;
#endif 

    case SIO_BITRATE_115200:
      /*---------------------------------------------------------------------
       Set the UART to 115.2K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_1152K_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      rs_port->gv.sio_single_char_time = SIO_MED_BITRATE_SINGLE_CHR_TIME;
      break;

#ifdef SIORS_HAS_BAUD_230400
    case SIO_BITRATE_230400:
      if (!rs_port->is_ext_speed)
        ERR_FATAL("Baudrate 230400 not available on this UART", 0, 0, 0);

      /*---------------------------------------------------------------------
       Set the UART to 230.4K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_2304K_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      rs_port->gv.sio_single_char_time = SIO_MED_BITRATE_SINGLE_CHR_TIME;
      break;
#endif

#ifdef SIORS_HAS_BAUD_460800
    case SIO_BITRATE_460800:
      if (!rs_port->is_ext_speed)
        ERR_FATAL("Baudrate 460800 not available on this UART", 0, 0, 0);

      /*---------------------------------------------------------------------
       Set the UART to 460.8K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_4608K_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      rs_port->gv.sio_single_char_time = SIO_MED_BITRATE_SINGLE_CHR_TIME;
      break;
#endif

    default:
      ERR_FATAL( "Invalid bit-rate", 0, 0, 0);
      break;
  } /* switch */

  /*-------------------------------------------------------------------------
   Now set bit rate, enable the receiver and the transmitter, and then reset 
   the IMR. Do a double RESET of both RX & TX for MSM2.2 and later versions
   of MSM2p.  
  -------------------------------------------------------------------------*/
  rs_port->gv.sio_csr_shadow = temp_csr_value;

  MSMU_OUT( hwreg->msmu_csr, rs_port->gv.sio_csr_shadow);
  MSMUW_OUT( hwreg->msmu_cr,  MSMU_CR_RESET_ERR);

  siors_pd_reset_receiver( rs_port );
  siors_pd_reset_transmitter( rs_port );

  MSMUW_OUT( hwreg->msmu_cr,  MSMU_CR_ENA_TX);
  MSMUW_OUT( hwreg->msmu_cr,  MSMU_CR_ENA_RX);

  /*--------------------------------------------------------------
   Update RXLEV after sio_current_rx_bitrate[] is valid.
  --------------------------------------------------------------*/
  siors_pd_set_rxlev(rs_port);
  rs_port->gv.sio_current_tx_bitrate        = req_bitrate;

  /*-------------------------------------------------------------------------
   Calculate the number of bytes that can be written to TX FIFO when TXLEV
   interrupt occurs.
  -------------------------------------------------------------------------*/
  if ( rs_port->is_ext_fifo )
  {
    rs_port->gv.sio_num_avail_tx_fifo_bytes =
          SIO_FIFO_PORT_MAIN_SIZE - rs_port->gv.sio_uart_flow_txlev;
  }
  else
  {
    rs_port->gv.sio_num_avail_tx_fifo_bytes =
                    SIO_FIFO_SIZE - rs_port->gv.sio_uart_flow_txlev;
  }

  /*-----------------------------------------------------------------------
   Set limit to 2 bytes after the incoming flow stops.
   ------------------------------------------------------------------------
    UART1 with 510byte FIFO has 9 non-continuous STALE_TIMEOUT bits,
    Other UART(s) with 6 STALE_TIMEOUT bits, but still not continuous.
   ------------------------------------------------------------------------*/      
  rs_port->gv.sio_ipr_rxstale_on = flow_rxstale & MSMU_IPR_STALE_LSB;
  rs_port->gv.sio_ipr_rxstale_on |= (flow_rxstale << 2) & MSMU_IPR_STALE_MSB;
  rs_port->gv.sio_ipr_rxstale_on |= MSMU_IPR_NEW_RXSTALE;

  if ( !rs_port->is_ext_fifo )
    rs_port->gv.sio_ipr_rxstale_on &= 0xFF;  

  /*------------------------------------------------------------------------
    Preserve SAMPLE_DATA bit of IPR_SHADOW, update the rest bit field.
    Then output sio_ipr_shadw to UART_IPR register 
  ------------------------------------------------------------------------*/    
  rs_port->gv.sio_ipr_shadow &= MSMU_IPR_NEW_SAMPLE_MODE;
  rs_port->gv.sio_ipr_shadow |= rs_port->gv.sio_ipr_rxstale_on;

  MSMU_OUT( hwreg->msmu_ipr, rs_port->gv.sio_ipr_shadow);
}



/*===========================================================================

FUNCTION SIORS_PD_RESET_TRANSMITTER                             EXPORTED

DESCRIPTION
  This procedure resets the receiver.  The reset command also flushes the
  TX FIFO.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_pd_reset_transmitter ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  MSMU_OUT( rs_port->hw.msmu_cr,  MSMU_CR_RESET_TX);
  MSMU_OUT( rs_port->hw.msmu_cr,  MSMU_CR_RESET_TX);
} 


 
/*===========================================================================

FUNCTION SIORS_PD_ENABLE_TRANSMITTER                            EXPORTED

DESCRIPTION
  Enable transmitter but not enable transmit interrupt.

DEPENDENCIES
  Should be called after sio_rs232_chng_bitrate.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_pd_enable_transmitter ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  MSMU_OUT( rs_port->hw.msmu_tfwr, rs_port->gv.sio_uart_flow_txlev);
  MSMU_OUT( rs_port->hw.msmu_cr,   MSMU_CR_ENA_TX);
} 


/*===========================================================================

FUNCTION SIORS_PD_RESET_RECEIVER                            EXPORTED

DESCRIPTION
  This procedure resets the receiver.  The reset command also flushes the
  RX FIFO.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_pd_reset_receiver ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  MSMU_OUT( rs_port->hw.msmu_cr,  MSMU_CR_RESET_RX);
  MSMU_OUT( rs_port->hw.msmu_cr,  MSMU_CR_RESET_RX);

  /*---------------------------------------------------------------
   If we're using RTS/CTS flow control then we re-assert only if 
   the in bound flow is enabled, otherwise it's unconditional.
   We disable interrupts to prevent the state of 
   sio_rx_flow_control_method from changing during this . . .
   Otherwise we could set the value of RFR incorrectly.
  ---------------------------------------------------------------*/
  
  INTLOCK();

  if(rs_port->gv.sio_inbound_flow_enabled)
  {
    MSMU_OUT( rs_port->hw.msmu_cr,  MSMU_CR_SET_RFR);
  }
  INTFREE();
}



/*===========================================================================

FUNCTION SIORS_PD_DISABLE_RECEIVER                          EXPORTED

DESCRIPTION
  This procedure Disables the receiver.  This includes issuing the "RX
  Disable" command and also disabling all of the RX interrupts.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  No RX interrupts will be generated until receiver is enabled again.

===========================================================================*/
static void siors_pd_disable_receiver ( void *pHead )
{
  siors_port_type     *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!rs_port->is_clock_enabled){
    return;
  }

  INTLOCK();

  (rs_port->gv.sio_imr_shadow) &= 
                        ~(MSMU_IMR_RXLEV | MSMU_IMR_RXHUNT | MSMU_IMR_RXSTALE);

  rs_port->gv.sio_ipr_shadow = 0;

  MSMU_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_RX);
  MSMU_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_RX);
  MSMU_OUT( rs_port->hw.msmu_cr, MSMU_CR_DIS_RX);
  MSMU_OUT( rs_port->hw.msmu_imr, rs_port->gv.sio_imr_shadow);

  /*-----------------------------------------------------------------------
   Clear IPR.  All the necessary bits will be reset the next time receiver
   is needed.  When clearing, IPR register has to be written after IMR 
   register is written.
  -----------------------------------------------------------------------*/
  MSMU_OUT( rs_port->hw.msmu_ipr, rs_port->gv.sio_ipr_shadow);

  INTFREE();
}



/*===========================================================================

FUNCTION SIORS_PD_ENABLE_RECEIVER                           EXPORTED

DESCRIPTION
  This routine enables SIO receive operations.  This includes enabling the
  SIO RX interrupts.

DEPENDENCIES
  Should be called after sio_rs232_chng_bitrate.

RETURN VALUE
  None

SIDE EFFECTS
  RX interrupts are enabled.

===========================================================================*/
static void siors_pd_enable_receiver ( void *pHead )
{
  uint32 i_save;                      /* holds PSW for interrupt disabling */
  siors_port_type     *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  INTLOCK_SAV( i_save);
  /*-------------------------------------------------------------------------
    Preserve the SAMPLE_DATA bit of IPR shadow before update the rest fields.
  -------------------------------------------------------------------------*/  
  rs_port->gv.sio_ipr_shadow &= MSMU_IPR_NEW_SAMPLE_MODE;
  (rs_port->gv.sio_ipr_shadow) |= (rs_port->gv.sio_ipr_rxstale_on);

  rs_port->gv.sio_imr_shadow |= (MSMU_IMR_RXSTALE | MSMU_IMR_RXLEV);
  /*------------------------------------------------------------------------
   First we reset the receiver . . . 
  ------------------------------------------------------------------------*/
  siors_pd_reset_receiver( rs_port );

  MSMU_OUT( rs_port->hw.msmu_cr, MSMU_CR_ENA_RX);
  MSMU_OUT( rs_port->hw.msmu_rfwr, rs_port->gv.sio_uart_flow_rxlev);
  MSMU_OUT( rs_port->hw.msmu_ipr, rs_port->gv.sio_ipr_shadow);
  MSMU_OUT( rs_port->hw.msmu_imr, rs_port->gv.sio_imr_shadow);

  INTFREE_SAV( i_save);
}




/*===========================================================================

FUNCTION SIORS_PD_CHECK_RX_ERROR                            INTERNAL

DESCRIPTION
  Check if there is error condition in ISR, if so, take proper action

DEPENDENCIES
  None

RETURN VALUE
  TRUE if error has occurred and RX/TX has been resetted.

SIDE EFFECTS
  None

===========================================================================*/
static boolean siors_pd_check_rx_error ( siors_port_type *rs_port )
{
  byte  status_reg;                       /* UART SR (status reg)          */
  siors_hw_reg_type *hwreg;               /* UART HW regs                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hwreg = &(rs_port->hw);

  /*------------------------------------------------------------------------
   Check for parity, framing and overrun errors.  If there is an error, exit
   without processing any bytes.  Flush the FIFO so that we do not get
   interrupted on the same data again.
  ------------------------------------------------------------------------*/
  if((( status_reg = (byte)inp(hwreg->msmu_sr)) & 
                                   ( MSMU_SR_PF_ERR | MSMU_SR_OVR_ERR)) != 0)
  {
    /*-----------------------------------------------------------------
      Increment appropriate Error variable then Reset error condition
      and prepare for recovery. Return to omit RX character processing.
    -----------------------------------------------------------------*/
    if (status_reg & MSMU_SR_PF_ERR)
    {
      INC_SAT( rs_port->gv.sio_errs.async_pf_err);
      MSG_HIGH("SIO UART PF Error!, device_id: %x", rs_port->device_id, 0, 0);
    }
    if (status_reg & MSMU_SR_OVR_ERR)
    {
      INC_SAT( rs_port->gv.sio_errs.async_or);
      ERR("SIO UART Overrun Error!, device_id: %x", rs_port->device_id, 0, 0);
    }
    else if (status_reg & MSMU_SR_RXFULL)
    {
      INC_SAT( rs_port->gv.sio_errs.async_rx_ovrflw);
      MSG_HIGH("SIO UART RX Full!, device_id: %x", rs_port->device_id, 0, 0);
    }
    MSMUW_OUT( hwreg->msmu_cr, MSMU_CR_RESET_ERR);

    siors_pd_reset_receiver( rs_port );
    siors_pd_enable_receiver( rs_port );
    return TRUE;
  } /* Parity/Framing Error, Overrun, or RX Full Error */
  return FALSE;
}



/*===========================================================================

FUNCTION SIORS_PD_COMMON_ISR                                INTERNAL

DESCRIPTION
  This ISR services all SIO ISRs (TX and RX).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_pd_common_isr ( siors_port_type *rs_port )
{
  byte misr_val;                /* Masked Interrupt Status Register Value */ 
  siors_hw_reg_type *hwreg;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled ){
    return;
  }
  hwreg = &(rs_port->hw);

  /*-------------------------------------------------------------------------
   Read the Masked Interrupt Status register (MISR), mask off undesired bits
   then process all interrupts conditions until MISR indicates no more.
  -------------------------------------------------------------------------*/
  misr_val = (inpw(hwreg->msmu_misr) & rs_port->gv.sio_imr_shadow);

  /* Process RX interrupt */
  if((misr_val & MSMU_ISR_RXLEV) !=0)
  {
    if(!siors_pd_check_rx_error(rs_port))
    {
      rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_RXLEV_INTR);
    }
  }
  else if((misr_val & MSMU_ISR_RXSTALE) !=0)
  {
    if(!siors_pd_check_rx_error(rs_port))
    {
      rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_RXSTALE_INTR);
    }
  }

  /* Process TX interrupt */
  else if(( misr_val & MSMU_ISR_TXLEV) != 0)
  {
    rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_TX_INTR);
  }

  /*-------------------------------------------------------------------------
   Check for RXBREAK interrupt.  This is not currently used, but to be safe
   we'll check it and clear it and disable it if set.
  -------------------------------------------------------------------------*/
  else if ( misr_val & MSMU_ISR_RXBREAK)
  {
    /*-----------------------------------------------------------------------
     RXBREAK interrupt is generated on change in break state (from idle to
     break or from break to idle).
     This shouldn't be enabled, so we'll disable it.
    -----------------------------------------------------------------------*/
    rs_port->gv.sio_imr_shadow &= ~MSMU_IMR_RXBREAK;
    MSMU_OUT(hwreg->msmu_imr, rs_port->gv.sio_imr_shadow);

    /*-----------------------------------------------------------------------
     Now we need to clear bit in interrupt status register.
    -----------------------------------------------------------------------*/
    MSMU_OUT(hwreg->msmu_cr, MSMU_CR_RESET_BRK_INT);
  } /* else if misr_val & MSMU_ISR_RXBREAK */
} 



/*===========================================================================

FUNCTION SIORS_PD_ISRx                                      INTERNAL

DESCRIPTION
  This ISR services all SIO ISRs (TX and RX).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_FIRST_UART
static void siors_pd_isr()
{
  siors_pd_common_isr(&(siors_pdd_ports[UART_DEVICE_1]));
} 
#endif


#ifdef FEATURE_SECOND_UART
static void siors_pd_isr2()
{
  siors_pd_common_isr(&(siors_pdd_ports[UART_DEVICE_2]));
} 
#endif


#ifdef FEATURE_THIRD_UART
static void siors_pd_isr3()
{
  siors_pd_common_isr(&(siors_pdd_ports[UART_DEVICE_3]));
} 
#endif



/*===========================================================================

FUNCTION SIORS_PD_INITIALIZE_COMMON                         INTERNAL

DESCRIPTION
  Initialize variables/hardware that is common to all UARTs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_pd_initialize_common ( siors_port_type *rs_port )
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    rs_port->gv.sio_imr_shadow                  = 0;
    rs_port->gv.sio_csr_shadow                  = 0;
    rs_port->gv.sio_ipr_shadow                  = 0;
    rs_port->gv.sio_mr1_shadow                  = 0;

    rs_port->gv.sio_mvr_shadow                  = MSMU_MVR_DEFAULT;
    rs_port->gv.sio_nvr_shadow                  = MSMU_NVR_DEFAULT;
    rs_port->gv.sio_dvr_shadow                  = MSMU_DVR_DEFAULT;

    rs_port->gv.sio_mvr_default                 = MSMU_MVR_DEFAULT; 
    rs_port->gv.sio_nvr_default                 = MSMU_NVR_DEFAULT;
    rs_port->gv.sio_dvr_default                 = MSMU_DVR_DEFAULT;

    rs_port->gv.sio_current_rx_bitrate          = SIO_BITRATE_19200;
    rs_port->gv.sio_current_tx_bitrate          = SIO_BITRATE_19200;
    
    rs_port->gv.inbound_auto_hw_flow_enabled = FALSE;

  /*-------------------------------------------------------------------------
  Reset error structure.
  -------------------------------------------------------------------------*/
    rs_port->gv.sio_errs.async_or = 0;
    rs_port->gv.sio_errs.async_pf_err = 0;
    rs_port->gv.sio_errs.async_crc_err = 0;
    rs_port->gv.sio_errs.async_max_pkt = 0;
    rs_port->gv.sio_errs.async_rx_ovrflw = 0;
    rs_port->gv.sio_errs.async_rx_spur = 0;
}



#ifdef FEATURE_FIRST_UART
/*===========================================================================

FUNCTION SIORS_PD_UART1_INIT                             INTERNAL

DESCRIPTION
  Initialize the UART with UART1 parameter.  The port number does not have
  to be hard linked to a physical device.  i.e. UART_AUX can be initialized 
  with UART1 hardware

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_pd_uart1_init( siors_port_type *rs_port )
{
    siors_hw_reg_type *hwreg;               /* Hardware register             */
    hwreg = &(rs_port->hw);

    /* ---- BASIC access structure initialize ---- */
    hwreg->msmu_mndr = 0;
    hwreg->msmu_mr1  = (dword)HWIO_ADDR(UART_MR1);
    hwreg->msmu_mr2  = (dword)HWIO_ADDR(UART_MR2);
    hwreg->msmu_irda = (dword)HWIO_ADDR(UART_IRDA);
    hwreg->msmu_imr  = (dword)HWIO_ADDR(UART_IMR);
    hwreg->msmu_mvr  = (dword)HWIO_ADDR(UART_MREG);
    hwreg->msmu_nvr  = (dword)HWIO_ADDR(UART_NREG);
    hwreg->msmu_dvr  = (dword)HWIO_ADDR(UART_DREG);
    hwreg->msmu_cr   = (dword)HWIO_ADDR(UART_CR);
    hwreg->msmu_sr   = (dword)HWIO_ADDR(UART_SR);
    hwreg->msmu_isr  = (dword)HWIO_ADDR(UART_ISR);
    hwreg->msmu_tf   = (dword)HWIO_ADDR(UART_TF);
    hwreg->msmu_rf   = (dword)HWIO_ADDR(UART_RF);
    hwreg->msmu_rfwr = (dword)HWIO_ADDR(UART_RFWR);
    hwreg->msmu_csr  = (dword)HWIO_ADDR(UART_CSR);
    hwreg->msmu_tfwr = (dword)HWIO_ADDR(UART_TFWR);
    hwreg->msmu_ipr  = (dword)HWIO_ADDR(UART_IPR);
    hwreg->msmu_hcr  = (dword)HWIO_ADDR(UART_HCR);
    hwreg->msmu_misr = (dword)HWIO_ADDR(UART_MISR);
#ifndef MSMHW_UART_MREG_13BIT_NREG_15BIT_DREG_13BIT
    hwreg->msmu_mndr = (dword)HWIO_ADDR(UART_MNDREG);
#endif
      
    rs_port->is_ext_fifo = TRUE;
    rs_port->is_ext_speed = TRUE;

  /*-----------------------------------------------------------------------*/
    siors_pd_initialize_common(rs_port);  

  /*-------------------------------------------------------------------------
   Explicitly initialize array of function pointer to be used for the 
   purposes of calling call-back functions which signal end of transmission.
  -------------------------------------------------------------------------*/
    rs_port->gv.siors_isr_func_ptr = siors_pd_isr;
    rs_port->gv.isr_type = UART1_IRQ;
    rs_port->gv.is_isr_registered = FALSE;
    rs_port->gv.clk_source = CLKRGM_UART1_CLK;
    rs_port->gv.wakeup_isr_type = UART1_RX_IRQ;
    rs_port->gv.wakeup_isr_func_ptr = siors_pd_dp_rx_isr1;
    rs_port->gv.is_wakeup_isr_registered = FALSE;
}
#endif


#ifdef FEATURE_SECOND_UART
/*===========================================================================

FUNCTION SIORS_PD_UART2_INIT                             INTERNAL

DESCRIPTION
  Initialize the UART with UART2 parameter.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_pd_uart2_init( siors_port_type *rs_port )
{
    siors_hw_reg_type *hwreg;               /* Hardware register             */
    hwreg = &(rs_port->hw);

    rs_port->is_ext_fifo = FALSE;
    rs_port->is_ext_speed = FALSE;
    hwreg->msmu_mndr = 0;
    hwreg->msmu_imr  = (dword)HWIO_ADDR(UART2_IMR);
    hwreg->msmu_cr   = (dword)HWIO_ADDR(UART2_CR);
    hwreg->msmu_mvr  = (dword)HWIO_ADDR(UART2_MREG);
    hwreg->msmu_nvr  = (dword)HWIO_ADDR(UART2_NREG);
    hwreg->msmu_dvr  = (dword)HWIO_ADDR(UART2_DREG);
    hwreg->msmu_mr1  = (dword)HWIO_ADDR(UART2_MR1);
    hwreg->msmu_mr2  = (dword)HWIO_ADDR(UART2_MR2);
    hwreg->msmu_irda = (dword)HWIO_ADDR(UART2_IRDA);
    hwreg->msmu_sr   = (dword)HWIO_ADDR(UART2_SR);
    hwreg->msmu_isr  = (dword)HWIO_ADDR(UART2_ISR);
    hwreg->msmu_tf   = (dword)HWIO_ADDR(UART2_TF);
    hwreg->msmu_rf   = (dword)HWIO_ADDR(UART2_RF);
    hwreg->msmu_rfwr = (dword)HWIO_ADDR(UART2_RFWR);
    hwreg->msmu_csr  = (dword)HWIO_ADDR(UART2_CSR);
    hwreg->msmu_tfwr = (dword)HWIO_ADDR(UART2_TFWR);
    hwreg->msmu_ipr  = (dword)HWIO_ADDR(UART2_IPR);
    hwreg->msmu_hcr  = (dword)HWIO_ADDR(UART2_HCR);
    hwreg->msmu_misr = (dword)HWIO_ADDR(UART2_MISR);
#ifndef MSMHW_UART_MREG_13BIT_NREG_15BIT_DREG_13BIT
    hwreg->msmu_mndr = (dword)HWIO_ADDR(UART2_MNDREG);
#endif

    /*-----------------------------------------------------------------------*/
    siors_pd_initialize_common(rs_port);  
   
    /*-------------------------------------------------------------------------
     Explicitly initialize array of function pointer to be used for the 
     purposes of calling call-back functions which signal end of transmission.
    -------------------------------------------------------------------------*/
    rs_port->gv.siors_isr_func_ptr = siors_pd_isr2;
    rs_port->gv.isr_type = UART2_IRQ;
    rs_port->gv.is_isr_registered = FALSE;
    rs_port->gv.clk_source = CLKRGM_UART2_CLK;
    rs_port->gv.wakeup_isr_type = UART2_RX_IRQ;
    rs_port->gv.wakeup_isr_func_ptr = siors_pd_dp_rx_isr2;
    rs_port->gv.is_wakeup_isr_registered = FALSE;
}
#endif


#ifdef FEATURE_THIRD_UART
/*===========================================================================

FUNCTION SIORS_PD_UART3_INIT                             INTERNAL

DESCRIPTION
  Initialize the UART with UART3 parameter.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_pd_uart3_init( siors_port_type *rs_port )
{
    siors_hw_reg_type *hwreg;               /* Hardware register             */
    hwreg = &(rs_port->hw);

    rs_port->is_ext_fifo = FALSE;
    rs_port->is_ext_speed = FALSE;
    hwreg->msmu_mndr = 0;
    hwreg->msmu_imr  = (dword)HWIO_ADDR(UART3_IMR);
    hwreg->msmu_cr   = (dword)HWIO_ADDR(UART3_CR);
    hwreg->msmu_mvr  = (dword)HWIO_ADDR(UART3_MREG);
    hwreg->msmu_nvr  = (dword)HWIO_ADDR(UART3_NREG);
    hwreg->msmu_dvr  = (dword)HWIO_ADDR(UART3_DREG);
    hwreg->msmu_mr1  = (dword)HWIO_ADDR(UART3_MR1);
    hwreg->msmu_mr2  = (dword)HWIO_ADDR(UART3_MR2);
    hwreg->msmu_irda = (dword)HWIO_ADDR(UART3_IRDA);
    hwreg->msmu_sr   = (dword)HWIO_ADDR(UART3_SR);
    hwreg->msmu_isr  = (dword)HWIO_ADDR(UART3_ISR);
    hwreg->msmu_tf   = (dword)HWIO_ADDR(UART3_TF);
    hwreg->msmu_rf   = (dword)HWIO_ADDR(UART3_RF);
    hwreg->msmu_rfwr = (dword)HWIO_ADDR(UART3_RFWR);
    hwreg->msmu_csr  = (dword)HWIO_ADDR(UART3_CSR);
    hwreg->msmu_tfwr = (dword)HWIO_ADDR(UART3_TFWR);
    hwreg->msmu_ipr  = (dword)HWIO_ADDR(UART3_IPR);
    hwreg->msmu_hcr  = (dword)HWIO_ADDR(UART3_HCR);
    hwreg->msmu_misr = (dword)HWIO_ADDR(UART3_MISR);
#ifndef MSMHW_UART_MREG_13BIT_NREG_15BIT_DREG_13BIT
    hwreg->msmu_mndr = (dword)HWIO_ADDR(UART3_MNDREG);
#endif

    /*-----------------------------------------------------------------------*/
    siors_pd_initialize_common(rs_port);  
   
    /*-------------------------------------------------------------------------
     Explicitly initialize array of function pointer to be used for the 
     purposes of calling call-back functions which signal end of transmission.
    -------------------------------------------------------------------------*/
    rs_port->gv.siors_isr_func_ptr = siors_pd_isr3;
    rs_port->gv.isr_type = UART3_IRQ;
    rs_port->gv.is_isr_registered = FALSE;
    rs_port->gv.clk_source = CLKRGM_UART3_CLK;
    rs_port->gv.wakeup_isr_type = UART3_RX_IRQ;
    rs_port->gv.wakeup_isr_func_ptr = siors_pd_dp_rx_isr3;
    rs_port->gv.is_wakeup_isr_registered = FALSE;
}
#endif


/*===========================================================================

FUNCTION SIORS_PD_OPEN                                          EXPORTED

DESCRIPTION
  This function opens a port so it can be used
    
DEPENDENCIES
  None
  
RETURN VALUE
  Returns non-zero if the port failed to open.
  
SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_pd_open ( void *pHead , 
                           /* TRUE if re-initialize is not required */
                             boolean quick )
{
  siors_port_type      *rs_port = pHead;
  siors_hw_reg_type    *hwreg = &(rs_port->hw);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Turning on the clock when the port is opened */
  CLK_REGIME_MSM_ENABLE(rs_port->gv.clk_source);    
  rs_port->is_clock_enabled = TRUE;

  /*-------------------------------------------------------------------------
   Moved part of the initialization from init to open function
  -------------------------------------------------------------------------*/

  /* Disable IRDA tranceiver.  No QC product currently uses IRDA. */
    MSMU_OUT( hwreg->msmu_irda, MSMU_IRDA_DISABLE);

  /* Program the M N and D register with some default rate */
    siors_pd_set_uart_clock_div_regs(rs_port,
                    rs_port->gv.sio_mvr_default,
                    rs_port->gv.sio_nvr_default,
                    rs_port->gv.sio_dvr_default);

    rs_port->gv.sio_imr_shadow = 0;
    MSMU_OUT( hwreg->msmu_imr, rs_port->gv.sio_imr_shadow);
    MSMUW_OUT( hwreg->msmu_cr,  MSMU_CR_RESET_ERR);

    siors_pd_reset_receiver( rs_port );
    siors_pd_reset_transmitter( rs_port );

  /* Program for eight bits per character, 1 stop bit and block check error
   * mode */
    MSMU_OUT(hwreg->msmu_mr2, (MSMU_MR2_8BPC | MSMU_MR2_1SB | MSMU_MR2_ERRMODE));

  /* Now set Automatic RFR in UART (for outbound flow monitoring). */
    rs_port->gv.sio_mr1_shadow = MSMU_MR1_CTSC;
    MSMU_OUT(hwreg->msmu_mr1, rs_port->gv.sio_mr1_shadow);

  /*  When port is opened, register the ISR  */
  tramp_set_isr( rs_port->gv.isr_type , rs_port->gv.siors_isr_func_ptr);
  rs_port->gv.is_isr_registered = TRUE;

  MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_ERR);
    
  siors_pd_set_bit_rate(rs_port,
                     rs_port->gv.sio_current_rx_bitrate);

  /* Turn on UIM VREG */
  SIORS_PD_ENABLE_PM_UIM_VREG_P2();

  siors_pd_enable_receiver( rs_port );
  siors_pd_enable_transmitter( rs_port );
}


/*===========================================================================

FUNCTION SIORS_PD_CLOSE                                     EXPORTED 

DESCRIPTION
  This function close a port in use
    
DEPENDENCIES
  None
  
RETURN VALUE
  None
   
SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_pd_close ( void *pHead )
{
  siors_port_type  *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  tramp_set_isr( rs_port->gv.isr_type , NULL);
  rs_port->gv.is_isr_registered = FALSE;

  siors_pd_reset_receiver( rs_port );
  siors_pd_reset_transmitter( rs_port );

  /* When port is closed, make sure there is no wakeup ISR registered */
  if(rs_port->gv.is_wakeup_isr_registered)
  {
    rs_port->gv.is_wakeup_isr_registered = FALSE;
    tramp_set_isr( rs_port->gv.wakeup_isr_type, NULL);
  }

  /* Turn off UIM VREG */
  SIORS_PD_DISABLE_PM_UIM_VREG_P2();

  rs_port->is_clock_enabled = FALSE;
  /* Turning off the clock as we are done with the port */
  CLK_REGIME_MSM_DISABLE(rs_port->gv.clk_source);    
}


/*===========================================================================

FUNCTION SIORS_PD_TRANSMIT                                  EXPORTED

DESCRIPTION
  This function transmits over MSM's RS-232 UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_pd_transmit
(
  void               *pHead,
  byte               *tx_ptr,             /* Packet for transmission       */
  int32              *byte_count          /* Number of character to sent   */
)
{
  int32    i;                    /* Loop counter                           */
  int32 bytes_to_tx;             /* Temporary for storing the number of
                                    bytes which need to be transmitted.    */
  byte *current_tx_ptr;          /* Temporary for storing current TX ptr   */
  siors_hw_reg_type *hwreg;
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  hwreg = &(rs_port->hw);

  current_tx_ptr = tx_ptr;
  bytes_to_tx = *byte_count;

  if ( ( inp( hwreg->msmu_isr) & MSMU_ISR_TXLEV) && 
           ( bytes_to_tx >= rs_port->gv.sio_num_avail_tx_fifo_bytes ) )
  {
    /*---------------------------------------------------------------------
     If this status bit is set, then we know a priori how many bytes can be 
     stuffed into TX FIFO.
    ---------------------------------------------------------------------*/
    bytes_to_tx -= (rs_port->gv.sio_num_avail_tx_fifo_bytes); /* lint !e734*/ 

    for ( i=0; i < rs_port->gv.sio_num_avail_tx_fifo_bytes ; i++)
    {
       MSMU_OUT( rs_port->hw.msmu_tf, *current_tx_ptr);
       current_tx_ptr++;
    } 
    *byte_count = i;
  } 
  else
  {
    /*---------------------------------------------------------------------
     If there is a packet that needs to be transmitted, transmit it, as 
     long as FIFO is not full.
    ---------------------------------------------------------------------*/
    i = 0;
    while(( inp( hwreg->msmu_sr) & MSMU_SR_TXRDY) != 0)
    {
      if( bytes_to_tx-- <= 0) break;
      MSMU_OUT( rs_port->hw.msmu_tf, *current_tx_ptr);
      current_tx_ptr++;
      i++;
    }
    *byte_count = i;
  }
}


/*===========================================================================

FUNCTION SIORS_PD_RECEIVE                                   EXPORTED

DESCRIPTION
  This function receive data from MSM's RS-232 UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_pd_receive
(
  void               *pHead,
  byte               *rx_ptr,             /* Buffer to store incoming data */
  uint32             *byte_count          /* Number of character to get    */
)
{
  uint32    i;                    /* Loop counter                           */
  uint32 bytes_to_rx;             /* Temporary for storing the number of
                                     bytes which need to be received   .    */
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  bytes_to_rx = *byte_count;

  if (!rs_port->is_clock_enabled){
    *byte_count = 0;
    return;
  }

  for(i=0;i<bytes_to_rx;i++)
  {
    if( ((byte)inp( rs_port->hw.msmu_sr) & MSMU_SR_RXRDY) == 0)
        break;
    *rx_ptr++ = (byte)inp( rs_port->hw.msmu_rf);
  }
  *byte_count = i;
}


/*===========================================================================

FUNCTION SIORS_PD_RECEIVE_TAIL_CHAR                         EXPORTED

DESCRIPTION
  This function receive data from MSM's RS-232 UART.  Tail character is
  used.  When tail character is detected, the function returns with 
  the tail character as the last character in the buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_pd_receive_tail_char
(
  void               *pHead,
  byte               *rx_ptr,             /* Buffer to store incoming data */
  uint32             *byte_count,         /* Number of character to get    */
  byte               tail_char,
  boolean            *more_data
)
{
  uint32    i;                    /* Loop counter                           */
  uint32 bytes_to_rx;             /* Temporary for storing the number of
                                     bytes which need to be received   .    */
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  bytes_to_rx = *byte_count;
  *more_data = TRUE;

  if (!rs_port->is_clock_enabled){
    *byte_count = 0;
    return;
  }
  
  for(i=0;i<bytes_to_rx;i++)
  {
    if( ((byte)inp( rs_port->hw.msmu_sr) & MSMU_SR_RXRDY) == 0)
    {
      *more_data = FALSE;
      break;
    }
    *rx_ptr = (byte)inp( rs_port->hw.msmu_rf);
    if(*rx_ptr == tail_char)
    {
		i++; /* Make sure the byte from the last read is included */
      break;
    }
	 rx_ptr++;
  }
  *byte_count = i;
}


/*===========================================================================

FUNCTION SIORS_PD_GET_CURRENT_CTS                               EXPORTED

DESCRIPTION
  Read the current value of the MSM current CTS pin. This is RFR/RTS of 
  the PC (DTE).

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void siors_pd_get_current_rts
(
  void                      *pHead,
  sio_ioctl_param_type      *param           /* Parameter                  */
)
{
  siors_port_type     *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!rs_port->is_clock_enabled){
    return;
  }

  /*-----------------------------------------------------------------
    Read the current value of RTS/RFR from the PC (DTE) which is
    CTS on the MSM (DCE). Return TRUE if RTS is asserted (High).
  -----------------------------------------------------------------*/
  if (inp(rs_port->hw.msmu_isr) & MSMU_ISR_CUR_CTS)
  {
    *param->rts_asserted = TRUE;
  }
  else
  {
    *param->rts_asserted = FALSE;
  }
}


/*===========================================================================

FUNCTION SIORS_PD_IS_TX_FIFO_EMPTY                              EXPORTED

DESCRIPTION
  Returns 1 if TX FIFO is empty, otherwise returns 0

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int32 siors_pd_is_tx_fifo_empty ( void *pHead )
{
  siors_port_type     *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!rs_port->is_clock_enabled){
    return 1;
  }

  if ((inp(rs_port->hw.msmu_sr) & MSMU_SR_TXEMT) != 0)
  {
    return 1;
  }
  return 0;
}


/*===========================================================================

FUNCTION SIORS_PD_CHECK_RX_CABLE_STATUS                     EXPORTED

DESCRIPTION
  Check for a constant mark condition on the RX line, which is used
  to indicate a cable disconnect condition.  Returns TRUE if RX line 
  is active.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean siors_pd_check_rx_cable_status
(
  void                      *pHead
)
{
  siors_port_type     *rs_port = pHead;
  boolean              ret;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check the status and clear it afterward for the next checking period */
  ret = tramp_check_raw_interrupt(rs_port->gv.wakeup_isr_type);
  tramp_clear_interrupt(rs_port->gv.wakeup_isr_type);
  return ret;
}

  
/*===========================================================================

FUNCTION SIORS_PD_SET_RXLEV                                 INTERNAL

DESCRIPTION
  This function sets the rxlev based on the current bitrate.
  Needed when we switch modes, but we don't want to change
  bitrates.  This is a side effect of the new autodetect/autobaud
  handling, where we set the rxlev lower when in that mode.

DEPENDENCIES
  UART is running and enabled.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void siors_pd_set_rxlev ( siors_port_type *rs_port )
{
  switch ( rs_port->gv.sio_current_rx_bitrate )
  {
    case SIO_BITRATE_300:
      rs_port->gv.sio_uart_flow_rxlev  = SIO_LOW_RATE_FLOW_RXLEV;
      break;

    case SIO_BITRATE_600:
      rs_port->gv.sio_uart_flow_rxlev  = SIO_LOW_RATE_FLOW_RXLEV;
      break;

    case SIO_BITRATE_1200:
      rs_port->gv.sio_uart_flow_rxlev  = SIO_LOW_RATE_FLOW_RXLEV;
      break;

    case SIO_BITRATE_2400:
      rs_port->gv.sio_uart_flow_rxlev  = SIO_LOW_RATE_FLOW_RXLEV;
      break;

    case SIO_BITRATE_4800:
      rs_port->gv.sio_uart_flow_rxlev  = SIO_LOW_RATE_FLOW_RXLEV;
      break;

    case SIO_BITRATE_9600:
      rs_port->gv.sio_uart_flow_rxlev  = SIO_9600_FLOW_RXLEV;
      break;

#ifdef SIORS_HAS_BAUD_14400
    case SIO_BITRATE_14400:
      rs_port->gv.sio_uart_flow_rxlev  = SIO_14400_FLOW_RXLEV;
      break;
#endif 

    case SIO_BITRATE_19200:
      rs_port->gv.sio_uart_flow_rxlev  = SIO_19200_FLOW_RXLEV;
      break;

    case SIO_BITRATE_38400:
      rs_port->gv.sio_uart_flow_rxlev  = SIO_38400_FLOW_RXLEV;
      break;

    case SIO_BITRATE_57600:
      rs_port->gv.sio_uart_flow_rxlev  = SIO_57600_FLOW_RXLEV;
      break;

#ifdef SIORS_HAS_BAUD_76800
    case SIO_BITRATE_76800:
      rs_port->gv.sio_uart_flow_rxlev  = SIO_76800_FLOW_RXLEV;
      break;
#endif 

    case SIO_BITRATE_115200:
      if (( rs_port->gv.sio_uart_packet_mode ) &&
          ( rs_port->is_ext_fifo))
      {
        /*-------------------------------------------------------------------
         This means that we are attempting high speed data.
        -------------------------------------------------------------------*/
        rs_port->gv.sio_uart_flow_rxlev = SIO_PKT_HIGH_RATE_FLOW_RXLEV;
      }
      else
      {
        rs_port->gv.sio_uart_flow_rxlev = SIO_115200_FLOW_RXLEV;
      }
      break;

#ifdef SIORS_HAS_BAUD_230400
    case SIO_BITRATE_230400:
      if (( rs_port->gv.sio_uart_packet_mode ) &&
          ( rs_port->is_ext_fifo))
      {
        /*-------------------------------------------------------------------
         This means that we are attempting high speed data.
        -------------------------------------------------------------------*/
        rs_port->gv.sio_uart_flow_rxlev = SIO_PKT_HIGH_RATE_FLOW_RXLEV;
      }
      else
      {
        rs_port->gv.sio_uart_flow_rxlev = SIO_230400_FLOW_RXLEV;
      }
      break;
#endif /* SIORS_HAS_BAUD_230400 */

#ifdef SIORS_HAS_BAUD_460800
    case SIO_BITRATE_460800:
      if (( rs_port->gv.sio_uart_packet_mode ) &&
          ( rs_port->is_ext_fifo))
      {
        /*-------------------------------------------------------------------
         This means that we are attempting high speed data.
        -------------------------------------------------------------------*/
        rs_port->gv.sio_uart_flow_rxlev = SIO_PKT_HIGH_RATE_FLOW_RXLEV;
      }
      else
      {
        rs_port->gv.sio_uart_flow_rxlev = SIO_460800_FLOW_RXLEV;
      }
      break;
#endif /* SIORS_HAS_BAUD_460800 */

    default:
      ERR_FATAL( "Invalid bit-rate, cat't set RXLEV", 0, 0, 0);
  }

  if (!rs_port->is_clock_enabled){
    return;
  }
  /*-------------------------------------------------------------------
    The current rxlev is stored in sio_uart_flow_rxlev[].  
    Now write it out to the MSM.
  -------------------------------------------------------------------*/
  MSMU_OUT( rs_port->hw.msmu_rfwr, rs_port->gv.sio_uart_flow_rxlev);
}


/*===========================================================================

FUNCTION SIORS_PD_INITIALIZE                            EXTERNALIZED FUNCTION

DESCRIPTION
  This is the only externalized function which the MDD layer can call.
  The function initialize the exported API table so the MDD layer can 
  access the UART driver

DEPENDENCIES
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void siors_pd_initialize
(
    uart_device_id_type  device_id,     /* Which device to open */
    sio_pdd_api_type    *pFuncTab,      /* Table of exported APIs, to MDD */
    void                **pHead,        /* Pointer to head structure, to MDD */
    void                *pMddHead,      /* MDD head structure, from MDD */
    void (*mdd_callback) (void *, siors_mdd_callback_type)  /* MDD callback entry */
)
{
    sio_pdd_api_type    *rs_func;
    siors_port_type     *rs_port;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* Allocate a structure from the global variable table */
    ASSERT((UART_DEVICE_FIRST <= device_id) && (device_id < UART_DEVICE_LAST));
    rs_port = &(siors_pdd_ports[device_id]);

    /* Record the MDD callback function */
    ASSERT(mdd_callback);
    rs_port->gv.mdd_callback_func_ptr = mdd_callback;
    
    /* Sending these back to MDD */
    rs_func = pFuncTab;
    ASSERT(rs_func);
    (*pHead) = rs_port;
    rs_port->gv.mdd_head_ptr = pMddHead;
    
    /* Initialize the API table */
    rs_func->enable_tx_hw_fctl = siors_pd_enable_tx_hardware_fctl;
    rs_func->disable_tx_hw_fctl = siors_pd_disable_tx_hardware_fctl;
    rs_func->enable_rx_hw_fctl = siors_pd_enable_rx_hardware_fctl;
    rs_func->disable_rx_hw_fctl = siors_pd_disable_rx_hardware_fctl;
    rs_func->enable_tx_interrupt = siors_pd_enable_transmit_interrupt;
    rs_func->disable_tx_interrupt = siors_pd_disable_transmit_interrupt;
    rs_func->enable_auto_hw_fctl = siors_pd_enable_auto_hrdwr_fctl;
    rs_func->disable_auto_hw_fctl = siors_pd_disable_auto_hrdwr_fctl;
    rs_func->detect_rx_activity = siors_pd_detect_rx_activity;
    rs_func->set_bit_rate = siors_pd_set_bit_rate;
    rs_func->reset_tx = siors_pd_reset_transmitter;
    rs_func->enable_tx = siors_pd_enable_transmitter;
    rs_func->reset_rx = siors_pd_reset_receiver;
    rs_func->enable_rx = siors_pd_enable_receiver;
    rs_func->disable_rx = siors_pd_disable_receiver;
    rs_func->open = siors_pd_open;
    rs_func->close = siors_pd_close;
    rs_func->transmit = siors_pd_transmit;
    rs_func->receive = siors_pd_receive;
    rs_func->receive_tail_char = siors_pd_receive_tail_char;
    rs_func->transmit_single_char = NULL;
    rs_func->get_current_rts = siors_pd_get_current_rts;
    rs_func->is_tx_fifo_empty = siors_pd_is_tx_fifo_empty;
    rs_func->check_rx_cable_status = siors_pd_check_rx_cable_status;
    rs_func->set_tx_break_state = NULL;
    
    rs_port->device_id = device_id;
        
    switch (device_id)
    {
#ifdef FEATURE_FIRST_UART
      case UART_DEVICE_1:
        siors_pd_uart1_init( rs_port );
        break;
#endif

#ifdef FEATURE_SECOND_UART
      case UART_DEVICE_2:
        siors_pd_uart2_init( rs_port );
        break;
#endif 

#ifdef FEATURE_THIRD_UART
      case UART_DEVICE_3:
        siors_pd_uart3_init( rs_port );
        break;
#endif 

    default:
      ERR_FATAL( "Invalid Port ID",0,0,0);
      break;
  } 

  /* Disable the packet mode by default for now */
  rs_port->gv.sio_uart_packet_mode = FALSE;
}

#endif //}
#endif //}

