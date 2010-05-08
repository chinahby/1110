/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   S I O R S 2 3 2   D M   P D

                SIO Real Device Layer for UARTDM 

GENERAL DESCRIPTION

  This Module is the real physical device layer of DMSS Serial I/O for an 
  UART hardware.  It takes care of all of the hardware dependent functionalities 
  of Serial I/O.  Only asynchronous mode is supported.

EXTERNALIZED FUNCTIONS

  siors_dm_initialize
    This function initialize the actual driver layer and 
    populate the exported API table so the media driver layer can access 
    the physical driver layer code

PHYSICAL DEVICE LAYER EXPORTED FUNCTIONS
  siors_dm_enable_tx_hardware_fctl
    Enable hardware flow control on transmitter

  siors_dm_disable_tx_hardware_fctl
    Disable hardware flow control on transmitter

  siors_dm_enable_rx_hardware_fctl
    Enable hardware flow control on receiver

  siors_dm_disable_rx_hardware_fctl
    Disable hardware flow control on receiver

  siors_dm_enable_transmit_interrupt
    Enable transmit interrupt
  
  siors_dm_disable_transmit_interrupt
    Disable transmit interrupt

  siors_dm_enable_auto_hrdwr_fctl
    Enable automatic hardware flow control on receiver

  siors_dm_disable_auto_hrdwr_fctl
    Disable automatic hardware flow control on receiver

  siors_dm_powerdown
    Do the necessary steps to turn off power to the UART

  siors_dm_powerup
    Do the necessary steps to turn on power to the UART

  siors_dm_set_bit_rate
    Set the current bit rate on the UART

  siors_dm_reset_transmitter
    Reset the transmitter

  siors_dm_enable_transmitter
    Enable the transmitter

  siors_dm_reset_receiver
    Reset the receiver

  siors_dm_enable_receiver
    Enable the receiver

  siors_dm_disable_receiver
    Disable the receiver

  siors_dm_open
    Called when the port is opened for communication

  siors_dm_close
    Called when the port is closed
    
  siors_dm_transmit
    Transmit a packet

  siors_dm_receive
    Receive a packet

  siors_dm_receive_tail_char
    Receive a packet with detection on the tail character
    
  siors_dm_get_current_rts
    Get the current status on the RTS line

  siors_dm_is_tx_fifo_empty
    Check to see if the TX FIFO is empty
    
  siors_dm_check_rx_cable_status
    Check to see if the RX line is in a extended marked state, suggesting
    the cable has been disconnected

  siors_dm_detect_rx_activity
    Enable the detection of RX activity
  
  siors_dm_set_tx_break_state
    Put the TX into/outof break state

    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  siors_dm_initialize() must be called prior to accessing the exported
  API table.

Copyright (c) 2007-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors232_dm_pd.c#7 $ $DateTime: 2009/04/03 08:54:12 $ $Author: tbailey $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/08   tb      Disable DMOV control while closing.
06/25/08   rh      Add user configurable FIFO SIZE
06/19/08   rh      Adding INTLOCK around mdm disable routine
06/11/08   jmm     Fix Klockwork and compiler warning messages
06/04/08   rh      Add clock frequency switch support for UART2DM
05/22/08   rh      Add DMOV sleep enable/disable call
04/03/08   rh      Fix an compatability issue with UART2DM code
03/12/08   rh      Add support for UART2DM
02/26/08   rh      Optimization for port open and RX
02/20/08   rh      Add error log message when break state is entered during TX
12/17/07   rh      Fix some race condition when DP_RX_ISR fires during DM RX CB
11/21/07   rh      Allow RX line detection to be enabled when port is open
                   Not resetting TX hardware when TX break is activated
10/18/07   rh      Change so BREAK event do not result in a RX FIFO purge
10/05/07   rh      Remove the ASSERT for opening an already opened port
09/24/07   rh      Add supporting APIs for TX break event generation
08/15/07   rh      Make sure RX detection is disabled when port is closed
08/10/07   rh      Add RX activity detection
08/09/07   rh      Fix issues that cause RX to fail
07/19/07   rh      Created file from siors232_dm_lite.c
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
#include "siors_lib.h"


//  If there is at least one UART.
#if( defined(FEATURE_FIRST_UART)  || \
     defined(FEATURE_SECOND_UART) || \
     defined(FEATURE_THIRD_UART))                   //{ 

//  Remove entire file if no DM driver in use.
#if((SIORS_FIRST_UART_TYPE  == HW_UARTDM) || \
    (SIORS_SECOND_UART_TYPE == HW_UARTDM) || \
    (SIORS_THIRD_UART_TYPE  == HW_UARTDM))          //{

#include "err.h"                   /* Error services definitions           */
#include "tramp.h"                 /* Trampoline typedefs and prototypes   */
#include "clk.h"                   /* Clock services definitions           */
#include "clkregim.h"              /* Clock Regime Definitions             */
#include "hw.h"                    /* Hardware Utilities                   */
#include "assert.h"                /* ASSERT macro unitilities             */
#include "siorsreg.h"              /* SIO RS232 UART Register Definitions  */
#include "dmov.h"                  /* Data mover driver definitions        */
#include "siors232_dm_pd.h"        /* SIO RS232 Device Layer Header File   */
#include "siors232_trace.h"        /* For the debug trace feature          */
#include "pmem.h"                  /* Memory allocation                    */
#include "uart_compat.h"           /* Compatability layer                  */




/*===========================================================================

                FEATURE FLAG RESOLUTION - Keep the mess here

===========================================================================*/
#if defined(FEATURE_JCDMA_DS) || defined(FEATURE_JCDMA_SIO)
#define SIORS_HAS_BAUD_14400
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

#ifdef SIORS_HAVE_TXRDY_INTR 
#define SIORS_UARTDM_TX_ISR          MSMU_IMR_TX_READY;
#else
#define SIORS_UARTDM_TX_ISR          MSMU_IMR_TXLEV;
#endif

/* Only enable UART support if it is selected as UARTDM */
#if (SIORS_FIRST_UART_TYPE != HW_UARTDM)
#undef FEATURE_FIRST_UART
#endif

#if (SIORS_SECOND_UART_TYPE != HW_UARTDM)
#undef FEATURE_SECOND_UART
#endif

#define DMOV_CLIENT_VOTE_NOT_IN_USE          0xdead

/* Feature to control if DMOV voting is required or not */
#ifndef SIORS_FIRST_UART_DMOV_ENABLE_VOTE    
#define SIORS_FIRST_UART_DMOV_ENABLE_VOTE    FALSE
#define SIORS_FIRST_UART_DMOV_CLIENT         DMOV_CLIENT_VOTE_NOT_IN_USE
#endif

#ifndef SIORS_FIRST_UART_DMOV_CLIENT
#define SIORS_FIRST_UART_DMOV_CLIENT         DMOV_CLIENT_UART
#endif

#ifndef SIORS_SECOND_UART_DMOV_ENABLE_VOTE    
#define SIORS_SECOND_UART_DMOV_ENABLE_VOTE   FALSE
#define SIORS_SECOND_UART_DMOV_CLIENT        DMOV_CLIENT_VOTE_NOT_IN_USE
#endif

#ifndef SIORS_SECOND_UART_DMOV_CLIENT
#define SIORS_SECOND_UART_DMOV_CLIENT        DMOV_CLIENT_UART
#endif


#if (SIORS_FIRST_UART_DMOV_ENABLE_VOTE == TRUE)||((SIORS_SECOND_UART_DMOV_ENABLE_VOTE == TRUE))
#define DMOV_ENABLE_CLIENT(x)       dmov_enable(x)
#define DMOV_DISABLE_CLIENT(x)      dmov_disable(x)
#else
#define DMOV_ENABLE_CLIENT(x)
#define DMOV_DISABLE_CLIENT(x)
#endif


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================
 General constant and variable definitions.
===========================================================================*/
/* Default RXLEV setting for UARTDM                                        */
#define SIORS_UARTDM_FLOW_RXLEV          16

/* Setting for TX FIFO interrupt.                                          */
/* Usually, for UARTDM, TX interrupt occured when TX-FIFO is empty         */
#define SIO_FLOW_TXLEV_UARTDM        0

/* Size of the FIFO. */
#define SIORS_FIFO_SIZE              512

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
siors_port_type    siors_dm_ports[UART_NUM_DEVICES];

/* UARTDM constants */
#define  SIORS_UARTDM_DMRX_DEFAULT_VALUE            256
#define  SIORS_UARTDM_DEFAULT_BADR                  0x40
#define  SIORS_UARTDM_DEFAULT_DMEN                  0x03
#define  SIORS_UARTDM_TX_FIFO_SIZE                  252
#define  SIORS_UARTDM_BADR_REDUCED_FIFO_DEFAULT     0x380

/* Remap feature flag into MACRO */
#ifdef   SIORS_USES_REDUCED_RX_FIFO_SIZE
#undef   SIORS_USES_REDUCED_RX_FIFO_SIZE
#define  SIORS_USES_REDUCED_RX_FIFO_SIZE    TRUE
#else
#define  SIORS_USES_REDUCED_RX_FIFO_SIZE    FALSE
#endif


/* Default FIFO size over-ride */
#ifndef SIORS_FIRST_UART_FIFO_SIZE
#define SIORS_FIRST_UART_FIFO_SIZE    SIORS_FIFO_SIZE
#endif

#ifndef SIORS_SECOND_UART_FIFO_SIZE
#define SIORS_SECOND_UART_FIFO_SIZE   SIORS_FIFO_SIZE
#endif
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*---------------------------------------------------------------------------
                      Function prototypes 
---------------------------------------------------------------------------*/
static void siors_dm_reset_receiver ( void * );
static void siors_dm_reset_transmitter ( void * );
static void siors_dm_enable_receiver ( void * );
static void siors_dm_set_bit_rate (void *, sio_bitrate_type);
static void siors_dm_enable_transmitter ( void * );
static void siors_dm_set_uart_clock_div_regs ( siors_port_type * , word , word , word );
static void siors_dm_start_rx ( siors_port_type * );
static void siors_dm_tx_cb ( const dmov_result_s_type *, void * );
static void siors_dm_rx_cb ( const dmov_result_s_type *, void * );
static void siors_dm_flush_rx ( siors_port_type * );
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/


/*===========================================================================

                       FUNCTIONs, ISRs and MACROs

===========================================================================*/
/*===========================================================================

FUNCTION SIORS_DM_ENABLE_HARDWARE_FCTL                           EXPORTED

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

static void siors_dm_enable_tx_hardware_fctl ( void *pHead )
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

FUNCTION SIORS_DM_MDM_DISABLE_TIMER_CB                        INTERNAL

DESCRIPTION
  This callback is triggered when there is a need to turn off MDM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

static void siors_dm_mdm_disable_timer_cb
(
  int32 time_ms, 
  timer_cb_data_type data
)
{
  siors_port_type *rs_port = (siors_port_type *)data;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  INTLOCK();
  if( rs_port->gv.is_port_closing )
  {
    DMOV_DISABLE_CLIENT(rs_port->gv.dmov_client_enum);
  }
  INTFREE();
}



/*===========================================================================

FUNCTION SIORS_DM_DISABLE_HARDWARE_FCTL                          EXPORTED

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

static void siors_dm_disable_tx_hardware_fctl ( void *pHead )
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

FUNCTION SIORS_DM_ENABLE_TRANSMIT_INTERRUPT                         EXPORTED

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
static void siors_dm_enable_transmit_interrupt
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
  /* Ignore the enable request if force ignore is on */
  if(rs_port->gv.ignore_tx_isr_enable){
    return;
  }

  SIORS_TRACE_CODE(201, rs_port->device_id);

  hwreg = &(rs_port->hw);

  MSMU_OUT( hwreg->msmu_tfwr, SIO_FLOW_TXLEV_UARTDM);

  rs_port->gv.sio_imr_shadow |= SIORS_UARTDM_TX_ISR;
  MSMU_OUT(hwreg->msmu_imr, rs_port->gv.sio_imr_shadow);
} 


/*===========================================================================

FUNCTION SIORS_DM_DISABLE_TRANSMIT_INTERRUPT                    EXPORTED

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
static void siors_dm_disable_transmit_interrupt ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  SIORS_TRACE_CODE(200, rs_port->device_id);

  rs_port->gv.sio_imr_shadow &= ~SIORS_UARTDM_TX_ISR;
  MSMU_OUT(rs_port->hw.msmu_imr, rs_port->gv.sio_imr_shadow);
}


/*===========================================================================

FUNCTION SIORS_DM_ENABLE_AUTO_HRDWR_FCTL                         EXPORTED

DESCRIPTION
  This procedure enables automatic RX hardware flow control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dm_enable_auto_hrdwr_fctl ( void *pHead )
{
  word uart_flow_rxlev;                   /* Uart RXLEV value              */
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  /* Calculate the RFR level base on FIFO size.  Padding is chosen to be 32 bytes
   * The final figure need to be in dwords */
  rs_port->gv.sio_uart_flow_rx_rfr_lev = (rs_port->gv.rx_fifo_size - 32) / 4;
    
  /* The RFR level in the MR1 register is not continuous, shift the bits
   * to the right position before writing to the register                  */
  uart_flow_rxlev = 
      ((rs_port->gv.sio_uart_flow_rx_rfr_lev) & MSMU_MR1_RFR_LVL_MASK);
  rs_port->gv.sio_mr1_shadow &= ~MSMU_MR1_RFR_LVL_MASK;  

  uart_flow_rxlev |= ((rs_port->gv.sio_uart_flow_rx_rfr_lev << 2) 
          & MSMU_MR1_RFR_LVL_HI_MASK);
  rs_port->gv.sio_mr1_shadow &= ~MSMU_MR1_RFR_LVL_HI_MASK;

  rs_port->gv.sio_mr1_shadow |= ( MSMU_MR1_RFRC | uart_flow_rxlev);
  MSMU_OUT(rs_port->hw.msmu_mr1, rs_port->gv.sio_mr1_shadow);
  rs_port->gv.inbound_auto_hw_flow_enabled = TRUE;
} 


/*===========================================================================

FUNCTION SIORS_DM_DISABLE_AUTO_HRDWR_FCTL                        EXPORTED

DESCRIPTION
  This procedure disables automatic RX hardware flow control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_dm_disable_auto_hrdwr_fctl ( void *pHead )
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

FUNCTION SIORS_DM_INBOUND_FLOW_ENABLE                      EXPORTED

DESCRIPTION
  Given RS232 enable in-bound flow control (HW).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dm_enable_rx_hardware_fctl ( void *pHead )
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

FUNCTION SIORS_DM_INBOUND_FLOW_DISABLE                     EXPORTED

DESCRIPTION
  Given RS232, disable in-bound flow using appropriate flow contol (HW)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dm_disable_rx_hardware_fctl ( void *pHead )
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
      rs_port->gv.sio_mr1_shadow &= ~MSMU_MR1_RFRC;
      MSMU_OUT(rs_port->hw.msmu_mr1, rs_port->gv.sio_mr1_shadow);
      MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_RFR);
      INTFREE();
  }
  rs_port->gv.sio_inbound_flow_enabled = FALSE;
} 


/*===========================================================================

FUNCTION SIORS_DM_DETECT_RX_ACTIVITY                        EXPORTED

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
  
===========================================================================*/
static void siors_dm_detect_rx_activity( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* If detection is already activated, exit right away */
  if(rs_port->gv.is_wakeup_isr_registered){
    return;
  }

  /* Register the wake up interrupt and exit */
  tramp_set_isr(rs_port->gv.wakeup_isr_type, rs_port->gv.wakeup_isr_func_ptr);
  rs_port->gv.is_wakeup_isr_registered = TRUE;
}



/*===========================================================================

FUNCTION SIORS_DM_DP_RX_COMMON_ISR                              INTERNAL

DESCRIPTION
  This ISR is triggered when RX activity is detected

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_dm_dp_rx_common_isr ( siors_port_type *rs_port )
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
static void siors_dm_dp_rx_isr()
{
  siors_dm_dp_rx_common_isr(&(siors_dm_ports[UART_DEVICE_1]));
} 
#endif

#ifdef FEATURE_SECOND_UART
static void siors_dm_dp_rx_isr2()
{
  siors_dm_dp_rx_common_isr(&(siors_dm_ports[UART_DEVICE_2]));
} 
#endif


/*===========================================================================

FUNCTION SIORS_DM_SET_UART_CLOCK_DIV_REGS                          INTERNAL

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
static void siors_dm_set_uart_clock_div_regs
(
    siors_port_type *rs_port,           /* Port                         */
    word  m_val,                        /* Value of m reg to set to     */
    word  n_val,                        /* Value of n reg to set to     */
    word  d_val                         /* Value of d reg to set to     */
)
{
    /* UARTDM always has 16 bit registers */
#ifndef SIORS_USES_EXTERNAL_MND_CNT
    MSMU_OUT( rs_port->hw.msmu_mvr,  m_val);    
    MSMU_OUT( rs_port->hw.msmu_nvr,  n_val);
    MSMU_OUT( rs_port->hw.msmu_dvr,  d_val);
#endif
    
    rs_port->gv.sio_mvr_shadow = m_val;
    rs_port->gv.sio_nvr_shadow = n_val;
    rs_port->gv.sio_dvr_shadow = d_val;
}


#ifdef SIORS_USES_EXTERNAL_MND_CNT
/*===========================================================================

FUNCTION SIORS_SET_UART_CLOCK_FREQ                          INTERNAL

DESCRIPTION
  This function will set the value of the m, n, d, mnd register.  
  The shadow register will also be updated

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void siors_dm_set_uart_clock_freq
(
    siors_port_type *rs_port,           /* Port                         */
    dword freq                          /* Frequency                    */
)
{


  if (freq > (dword)460800)
  {
    switch (freq)
     {
       case (dword)921600:
         rs_port->gv.fp_clk_regime_sel_clk(CLKRGM_UARTDM_SPEED_14_7456_MHZ);
         break;
       case (dword)3200000:
         rs_port->gv.fp_clk_regime_sel_clk(CLKRGM_UARTDM_SPEED_51_2_MHZ);
         break;
       case (dword)3686400:
         rs_port->gv.fp_clk_regime_sel_clk(CLKRGM_UARTDM_SPEED_58_9824_MHZ);
         break;
       case (dword)4000000:
         rs_port->gv.fp_clk_regime_sel_clk(CLKRGM_UARTDM_SPEED_64_MHZ);
         break;
       default:
         ERR_FATAL("Unsupported Baudrate: UART-DM does not supports requested baud rate", 0, 0, 0);
         break;
     }
  } /* if (freq > 460800) */
  else 
  {
     rs_port->gv.fp_clk_regime_sel_clk(CLKRGM_UARTDM_SPEED_7_3728_MHZ);
  }
}
#endif


/*===========================================================================

FUNCTION SIORS_DM_SET_BIT_RATE                              EXPORTED

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

static void siors_dm_set_bit_rate
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
  siors_dm_set_uart_clock_div_regs( rs_port,
                                rs_port->gv.sio_mvr_default,
                                rs_port->gv.sio_nvr_default,
                                rs_port->gv.sio_dvr_default);
#ifdef SIORS_USES_EXTERNAL_MND_CNT
  siors_dm_set_uart_clock_freq(rs_port, (dword)115200);
#endif    
  /*-------------------------------------------------------------------------
   Set UART baud rate according to passed parameter. NOTE: The UART RXLEV
   flow variable is set to provide a max 4ms interrupt latency (before an RX
   overrun would occur).
  -------------------------------------------------------------------------*/
  switch( req_bitrate)
  {
      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_300:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_300_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_LOW_RATE;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_600:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_600_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_LOW_RATE;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_1200:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_1200_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_1200;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_2400:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_2400_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_2400;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_4800:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_4800_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_4800;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_9600:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_9600_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_9600;
      break;

#ifdef SIORS_HAS_BAUD_14400 
      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_14400:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_144K_BPS;
      rs_port->gv.sio_uart_flow_txlev  = SIO_FLOW_TXLEV;
      flow_rxstale                     = SIO_FLOW_RXSTALE_14400;
      break;
#endif

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_19200:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_192K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_19200;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_38400:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_384K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_57600:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_576K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_115200:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_1152K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_230400:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_2304K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_460800:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_4608K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_921600:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_9216K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
#ifdef SIORS_USES_EXTERNAL_MND_CNT
      siors_dm_set_uart_clock_freq(rs_port, (dword)921600);
#else    
      siors_dm_set_uart_clock_div_regs( rs_port,
                                MSMU_MVR_921600,
                                MSMU_NVR_921600,
                                MSMU_DVR_921600);
#endif
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_2900000:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_2900K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;

#ifdef SIORS_USES_EXTERNAL_MND_CNT
      siors_dm_set_uart_clock_freq(rs_port, (dword)2900000);
#else    
      siors_dm_set_uart_clock_div_regs( rs_port,
                                MSMU_MVR_2900000,
                                MSMU_NVR_2900000,
                                MSMU_DVR_2900000);
#endif
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_3200000:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_3200K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
#ifdef SIORS_USES_EXTERNAL_MND_CNT
      siors_dm_set_uart_clock_freq(rs_port, (dword)3200000);
#else
      siors_dm_set_uart_clock_div_regs( rs_port,
                                MSMU_MVR_3200000,
                                MSMU_NVR_3200000,
                                MSMU_DVR_3200000);
#endif
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_3686400:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_36864K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
#ifdef SIORS_USES_EXTERNAL_MND_CNT
      siors_dm_set_uart_clock_freq(rs_port, (dword)3686400);
#else
      siors_dm_set_uart_clock_div_regs( rs_port,
                                MSMU_MVR_3686400,
                                MSMU_NVR_3686400,
                                MSMU_DVR_3686400);
#endif
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_4000000:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_4000K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
#ifdef SIORS_USES_EXTERNAL_MND_CNT
      siors_dm_set_uart_clock_freq(rs_port, (dword)4000000);
#else
      siors_dm_set_uart_clock_div_regs( rs_port,
                                MSMU_MVR_4000000,
                                MSMU_NVR_4000000,
                                MSMU_DVR_4000000);
#endif
      break;

      
    default:
      ERR_FATAL( "Invalid bit-rate", 0, 0, 0);
      break;
  } /* switch */

  /*-------------------------------------------------------------------------
   Reset the hardware to disable receving before setting the bit rate
  -------------------------------------------------------------------------*/
  MSMU_OUT( rs_port->hw.msmu_cr,  MSMU_CR_RESET_RX);
  /* If tx break is in use, do not reset the transmitter, 
   * this is done so TX break condition is not destroyed */
  if(!rs_port->gv.tx_break_state)
  {
    siors_dm_reset_transmitter( rs_port );
  }

  rs_port->gv.sio_csr_shadow = temp_csr_value;

  MSMU_OUT( hwreg->msmu_csr, rs_port->gv.sio_csr_shadow);
  MSMUW_OUT( hwreg->msmu_cr,  MSMU_CR_RESET_ERR);

  /*-----------------------------------------------------------------------
    UARTDM has 9 non-continuous STALE_TIMEOUT bits.
   ------------------------------------------------------------------------*/      
  rs_port->gv.sio_ipr_rxstale_on = flow_rxstale & MSMU_IPR_STALE_LSB;
  rs_port->gv.sio_ipr_rxstale_on |= (flow_rxstale << 2) & MSMU_IPR_STALE_MSB;
  rs_port->gv.sio_ipr_rxstale_on |= MSMU_IPR_NEW_RXSTALE;

  /*------------------------------------------------------------------------
    Preserve SAMPLE_DATA bit of IPR_SHADOW, update the rest bit field.
    Then output sio_ipr_shadw to UART_IPR register 
  ------------------------------------------------------------------------*/    
  rs_port->gv.sio_ipr_shadow &= MSMU_IPR_NEW_SAMPLE_MODE;
  rs_port->gv.sio_ipr_shadow |= rs_port->gv.sio_ipr_rxstale_on;

  MSMU_OUT( hwreg->msmu_ipr, rs_port->gv.sio_ipr_shadow);


  /*-----------------------------------------------------------------------
    Reset and enable the receive and transmit hardware after all the
    configuration has been finished
   ------------------------------------------------------------------------*/      
  siors_dm_reset_receiver( rs_port );
  if(!rs_port->gv.tx_break_state)
  {
    siors_dm_reset_transmitter( rs_port );
  }

  siors_dm_enable_receiver( rs_port );
  siors_dm_enable_transmitter( rs_port );
}



/*===========================================================================

FUNCTION SIORS_DM_RESET_TRANSMITTER                             EXPORTED

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

static void siors_dm_reset_transmitter ( void *pHead )
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

FUNCTION SIORS_DM_ENABLE_TRANSMITTER                            EXPORTED

DESCRIPTION
  Enable transmitter but not enable transmit interrupt.

DEPENDENCIES
  Should be called after sio_rs232_chng_bitrate.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dm_enable_transmitter ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  rs_port->dmv.tx_started = FALSE;

  MSMU_OUT( rs_port->hw.msmu_tfwr, SIO_FLOW_TXLEV_UARTDM);
  MSMU_OUT( rs_port->hw.msmu_cr,   MSMU_CR_ENA_TX);
} 


/*===========================================================================

FUNCTION SIORS_DM_RESET_RECEIVER                            EXPORTED

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
static void siors_dm_reset_receiver ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  /* FLUSH all the data in the buffer out first */
  siors_dm_flush_rx( rs_port );

  
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

FUNCTION SIORS_DM_DISABLE_RECEIVER                          EXPORTED

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
static void siors_dm_disable_receiver ( void    *pHead )
{
  siors_port_type     *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!rs_port->is_clock_enabled){
    return;
  }

  INTLOCK();

  rs_port->gv.sio_imr_shadow &= 
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

FUNCTION SIORS_DM_ENABLE_RECEIVER                           EXPORTED

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
static void siors_dm_enable_receiver ( void *pHead )
{
  uint32 i_save;                      /* holds PSW for interrupt disabling */
  siors_port_type     *rs_port;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  rs_port = pHead;

  if (!rs_port->is_clock_enabled){
    return;
  }

  INTLOCK_SAV( i_save);
  /*-------------------------------------------------------------------------
    Preserve the SAMPLE_DATA bit of IPR shadow before update the rest fields.
  -------------------------------------------------------------------------*/  
  rs_port->gv.sio_ipr_shadow &= MSMU_IPR_NEW_SAMPLE_MODE;
  (rs_port->gv.sio_ipr_shadow) |= (rs_port->gv.sio_ipr_rxstale_on);

  rs_port->gv.sio_imr_shadow |= (MSMU_IMR_RXSTALE);

  MSMU_OUT( rs_port->hw.msmu_cr, MSMU_CR_ENA_RX);
  //MSMU_OUT( rs_port->hw.msmu_rfwr, rs_port->gv.sio_uart_flow_rxlev);
  MSMU_OUT( rs_port->hw.msmu_ipr, rs_port->gv.sio_ipr_shadow);
  MSMU_OUT( rs_port->hw.msmu_imr, rs_port->gv.sio_imr_shadow);

  siors_dm_start_rx(rs_port);
  INTFREE_SAV( i_save);
}



/*===========================================================================

FUNCTION SIORS_DM_CHECK_RX_ERROR                            INTERNAL

DESCRIPTION
  Check if there is error condition in ISR, if so, take proper action

DEPENDENCIES
  None

RETURN VALUE
  TRUE if error has occurred and RX/TX has been resetted.

SIDE EFFECTS
  None

===========================================================================*/
static boolean siors_dm_check_rx_error ( siors_port_type *rs_port )
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

    /* To prevent the ISR being fired continuously if a RX break event occurs, make
     * sure receiver is not resetted if RX break detected */
    if((status_reg & MSMU_SR_RXBREAK ) == 0)
    {
      INC_SAT( rs_port->gv.sio_errs.async_rx_break);
      siors_dm_reset_receiver( rs_port );
      siors_dm_enable_receiver( rs_port );
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  } /* Parity/Framing Error, Overrun, or RX Full Error */
  return FALSE;
}



/*===========================================================================

FUNCTION SIORS_DM_COMMON_ISR                                INTERNAL

DESCRIPTION
  This ISR services all SIO ISRs (TX and RX).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_dm_common_isr ( siors_port_type *rs_port )
{
  byte misr_val;                /* Masked Interrupt Status Register Value */ 
  siors_hw_reg_type *hwreg;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled ){
    return;
  }
  hwreg = &(rs_port->hw);

  SIORS_TRACE_CODE(99, rs_port->device_id);
  SIORS_TRACE_CODE(inp(hwreg->msmu_isr), rs_port->device_id);
  SIORS_TRACE_CODE(inp(hwreg->msmu_misr), rs_port->device_id);
  SIORS_TRACE_CODE(rs_port->gv.sio_imr_shadow, rs_port->device_id);
  
  /* Need to make sure DM call back do not occur when stale condition
   * is being processed */
  INTLOCK();
  /*-------------------------------------------------------------------------
   Read the Masked Interrupt Status register (MISR), mask off undesired bits
   then process all interrupts conditions until MISR indicates no more.
  -------------------------------------------------------------------------*/
  misr_val = (inpw(hwreg->msmu_misr) & rs_port->gv.sio_imr_shadow);

  /* Process RX interrupt */
  if((misr_val & MSMU_ISR_RXLEV) !=0)
  {
    /*
    if(!siors_dm_check_rx_error(rs_port))
    {
      rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_RXLEV_INTR);
    }*/
  }
  else if((misr_val & MSMU_ISR_RXSTALE) !=0)
  {
    if(!siors_dm_check_rx_error(rs_port))
    {
      /* Stale interrupt fired. Flush the current DM command */
      ASSERT(rs_port->dmv.rx_cmd_state == SIO_RX_CMD_SENT);
      SIORS_TRACE_CODE(11, rs_port->device_id);
      
      rs_port->dmv.rx_cmd_state = SIO_RX_CMD_FLUSHING;
      rs_port->dmv.dm_stale_evt = TRUE;
      
      MSMUW_OUT(hwreg->msmu_cr, MSMU_CR_DIS_STALE_EVT);
      MSMUW_OUT(hwreg->msmu_cr, MSMU_CR_RESET_STALE);
      if( ! dmov_chan_flush( rs_port->dmv.dm_device, rs_port->dmv.dm_channel, FALSE ) )
      {
        ERR_FATAL("DMOV chan flush failed",0,0,0);
      }
    }
  }
  /* Process TX interrupt */
  else if(( misr_val & MSMU_ISR_TXLEV) != 0)
  {
    rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_TX_INTR);
  }
  /* For build that has TX READY enabled */
  else if(( misr_val & MSMU_ISR_TX_READY) != 0)
  {
    rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_TX_INTR);
  }
  /* Check for RXBREAK interrupt. */
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
  INTFREE();
} 



/*===========================================================================

FUNCTION SIORS_DM_ISRx                                      INTERNAL

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
static void siors_dm_isr()
{
  siors_dm_common_isr(&(siors_dm_ports[UART_DEVICE_1]));
} 
#endif

#ifdef FEATURE_SECOND_UART
static void siors_dm_isr2()
{
  siors_dm_common_isr(&(siors_dm_ports[UART_DEVICE_2]));
} 
#endif


/*===========================================================================

FUNCTION SIORS_DM_INITIALIZE_COMMON                         INTERNAL

DESCRIPTION
  Initialize variables/hardware that is common to all UARTs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_dm_initialize_common ( siors_port_type *rs_port )
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    rs_port->gv.sio_imr_shadow                  = 0;
    rs_port->gv.sio_csr_shadow                  = 0;
    rs_port->gv.sio_ipr_shadow                  = 0;
    rs_port->gv.sio_mr1_shadow                  = 0;

#ifdef SIORS_USES_EXTERNAL_MND_CNT
    rs_port->gv.sio_mvr_shadow                  = 0;
    rs_port->gv.sio_nvr_shadow                  = 0;
    rs_port->gv.sio_dvr_shadow                  = 0;

    rs_port->gv.sio_mvr_default                 = 0; 
    rs_port->gv.sio_nvr_default                 = 0;
    rs_port->gv.sio_dvr_default                 = 0;
#else
    rs_port->gv.sio_mvr_shadow                  = MSMU_MVR_DEFAULT_UARTDM;
    rs_port->gv.sio_nvr_shadow                  = MSMU_NVR_DEFAULT_UARTDM;
    rs_port->gv.sio_dvr_shadow                  = MSMU_DVR_DEFAULT_UARTDM;

    rs_port->gv.sio_mvr_default                 = MSMU_MVR_DEFAULT_UARTDM; 
    rs_port->gv.sio_nvr_default                 = MSMU_NVR_DEFAULT_UARTDM;
    rs_port->gv.sio_dvr_default                 = MSMU_DVR_DEFAULT_UARTDM;
#endif

    rs_port->gv.sio_current_rx_bitrate          = SIO_BITRATE_19200;
    rs_port->gv.sio_current_tx_bitrate          = SIO_BITRATE_19200;
    
    rs_port->gv.inbound_auto_hw_flow_enabled    = FALSE;
    rs_port->gv.tx_break_state                  = FALSE;

  /*-------------------------------------------------------------------------
  Reset error structure.
  -------------------------------------------------------------------------*/
    rs_port->gv.sio_errs.async_or = 0;
    rs_port->gv.sio_errs.async_pf_err = 0;
    rs_port->gv.sio_errs.async_crc_err = 0;
    rs_port->gv.sio_errs.async_max_pkt = 0;
    rs_port->gv.sio_errs.async_rx_ovrflw = 0;
    rs_port->gv.sio_errs.async_rx_spur = 0;
    rs_port->gv.sio_errs.async_rx_break = 0;
 
    clk_def( &( rs_port->gv.dmov_disable_cb_item ));
}



#ifdef FEATURE_FIRST_UART
/*===========================================================================

FUNCTION SIORS_DM_UART1DM_INIT                             INTERNAL

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

static void siors_dm_uart1dm_init( siors_port_type *rs_port )
{
    siors_hw_reg_type *hwreg;               /* Hardware register             */
    byte* rx_buffer;
    dmov_cmd_box_s_type* tx_cmd;
    dmov_cmd_box_s_type* rx_cmd;
    byte cmd_id;
    siors_dm_variable_type *dmvar;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    dmvar = &(rs_port->dmv);
    hwreg = &(rs_port->hw);

    /* ---- BASIC access structure initialize ---- */
    hwreg->msmu_mr1  = (dword)HWIO_ADDR(UART1DM_MR1);
    hwreg->msmu_mr2  = (dword)HWIO_ADDR(UART1DM_MR2);
    hwreg->msmu_irda = (dword)HWIO_ADDR(UART1DM_IRDA);
    hwreg->msmu_imr  = (dword)HWIO_ADDR(UART1DM_IMR);
#ifdef SIORS_USES_EXTERNAL_MND_CNT
    hwreg->msmu_mvr  = 0;
    hwreg->msmu_nvr  = 0;
    hwreg->msmu_dvr  = 0;
#else
    hwreg->msmu_mvr  = (dword)HWIO_ADDR(UART1DM_MREG);
    hwreg->msmu_nvr  = (dword)HWIO_ADDR(UART1DM_NREG);
    hwreg->msmu_dvr  = (dword)HWIO_ADDR(UART1DM_DREG);
#endif
    hwreg->msmu_cr   = (dword)HWIO_ADDR(UART1DM_CR);
    hwreg->msmu_sr   = (dword)HWIO_ADDR(UART1DM_SR);
    hwreg->msmu_isr  = (dword)HWIO_ADDR(UART1DM_ISR);
    hwreg->msmu_tf   = (dword)HWIO_ADDR(UART1DM_TF);
    hwreg->msmu_rf   = (dword)HWIO_ADDR(UART1DM_RF);
    hwreg->msmu_rfwr = (dword)HWIO_ADDR(UART1DM_RFWR);
    hwreg->msmu_csr  = (dword)HWIO_ADDR(UART1DM_CSR);
    hwreg->msmu_tfwr = (dword)HWIO_ADDR(UART1DM_TFWR);
    hwreg->msmu_ipr  = (dword)HWIO_ADDR(UART1DM_IPR);
    hwreg->msmu_hcr  = (dword)HWIO_ADDR(UART1DM_HCR);
    hwreg->msmu_misr = (dword)HWIO_ADDR(UART1DM_MISR);
    hwreg->msmu_snap = (dword)HWIO_ADDR(UART1DM_RX_TOTAL_SNAP);
    hwreg->msmu_dmrx = (dword)HWIO_ADDR(UART1DM_DMRX);
    hwreg->msmu_rxfs = (dword)HWIO_ADDR(UART1DM_RXFS);
    hwreg->msmu_ncft = (dword)HWIO_ADDR(UART1DM_NO_CHARS_FOR_TX);
    hwreg->msmu_dmen = (dword)HWIO_ADDR(UART1DM_DMEN);
    hwreg->msmu_badr = (dword)HWIO_ADDR(UART1DM_BADR);
    hwreg->msmu_phytf = (dword)HWIO_PHYS(UART1DM_TF);
    hwreg->msmu_phyrf = (dword)HWIO_PHYS(UART1DM_RF);
      
  /*-----------------------------------------------------------------------*/
    siors_dm_initialize_common(rs_port);  

  /*-------------------------------------------------------------------------
   Explicitly initialize array of function pointer to be used for the 
   purposes of calling call-back functions which signal end of transmission.
  -------------------------------------------------------------------------*/
    rs_port->gv.siors_isr_func_ptr = siors_dm_isr;
    rs_port->gv.isr_type = UART1DM_IRQ; 
    rs_port->gv.is_isr_registered = FALSE;
    rs_port->gv.clk_source = CLKRGM_UART1DM_CLK;
    rs_port->gv.wakeup_isr_type = UART1DM_RX_IRQ;
    rs_port->gv.wakeup_isr_func_ptr = siors_dm_dp_rx_isr;
    rs_port->gv.is_wakeup_isr_registered = FALSE;

  /*-------------------------------------------------------------------------
   Allocate data structure required to enable DMOV operation
  -------------------------------------------------------------------------*/
    SIORS_TRACE_CODE(30, rs_port->device_id);

    /* Initialize DM variable structure */
    dmvar->current_rx_cmd = 0;
    dmvar->dm_stale_evt = FALSE;
    
    /* Allocate buffer space for Rx and Tx operations */
    rx_buffer = pmem_malloc(
                     (SIO_DM_RX_BUF_SIZE*SIO_DM_NUM_RX_CMDS)+SIO_DM_BURST_SIZE, 
                     PMEM_UNCACHED_ID);

    if ( rx_buffer == NULL )
    {
      ERR_FATAL("Could not allocate memory",0,0,0);
    }

    dmvar->tx_buffer = rx_buffer + (SIO_DM_RX_BUF_SIZE*SIO_DM_NUM_RX_CMDS);

    /* Initialize Transmit structures */
    tx_cmd = dmvar->tx_cmd.sio_tx_cmd_ptr = 
                    (void *)(((uint32)dmvar->tx_cmd_buf + 0x7 ) & ~0x7);
    tx_cmd->command = DMOV_FIELD(CMD_WORD, LC, 1) |
                    DMOV_FIELD(CMD_WORD, DST_CRCI_CTL, DMOV_CRCI_HS_UART_TX) |
                DMOV_FIELD(CMD_WORD, ADDR_MODE, DMOV_ADDR_MODE_BOX);
    tx_cmd->src_row_len    = SIO_DM_BURST_SIZE;
    tx_cmd->src_row_offset = SIO_DM_BURST_SIZE;
    tx_cmd->dst_row_addr   = hwreg->msmu_phytf;
    tx_cmd->dst_row_len    = SIO_DM_BURST_SIZE;
    tx_cmd->dst_row_offset = 0;
    dmvar->tx_cmd.in_use      = FALSE;
    dmvar->tx_cmd.rs_port     = rs_port;

    dmvar->tx_xfer.device       = DMOV_DEVICE_HS_UART;
    dmvar->tx_xfer.chan         = DMOV_CHAN_HS_UART_TX;
    dmvar->tx_xfer.priority     = DMOV_PRI_HS_UART;
    dmvar->tx_xfer.cmd_ptr      = dmvar->tx_cmd.sio_tx_cmd_ptr;
    dmvar->tx_xfer.cmd_type     = DMOV_CMD_LIST;
    dmvar->tx_xfer.callback_ptr = siors_dm_tx_cb;
    dmvar->tx_xfer.user_ptr     = (void *)&dmvar->tx_cmd;
    dmvar->tx_xfer.options      = DMOV_XFER_DEFAULT;

    /* Initialize Receive structures */
    for(cmd_id = 0; cmd_id < SIO_DM_NUM_RX_CMDS; cmd_id++)
    {
      /* Divide buffer and check if it is 32 bytes aligned */
      rx_buffer += (SIO_DM_RX_BUF_SIZE*cmd_id);
      ASSERT(((uint32)rx_buffer&0x1F) == 0);
      dmvar->rx_cmd[cmd_id].sio_rx_buffer = (byte *)rx_buffer;
      dmvar->rx_cmd[cmd_id].cmd_id = cmd_id;
      dmvar->rx_cmd[cmd_id].rs_port = rs_port; 
    }

    rx_cmd = dmvar->rx_xfer.cmd_ptr = 
                    (void *)(((uint32)dmvar->rx_cmd_buf + 0x7) & ~0x7);

    rx_cmd->command = DMOV_FIELD(CMD_WORD, LC, 1) |
                    DMOV_FIELD(CMD_WORD, SRC_CRCI_CTL, DMOV_CRCI_HS_UART_RX) |
                DMOV_FIELD(CMD_WORD, ADDR_MODE, DMOV_ADDR_MODE_BOX);
    rx_cmd->src_row_addr   = hwreg->msmu_phyrf;
    rx_cmd->src_row_len    = SIO_DM_BURST_SIZE;
    rx_cmd->src_row_offset = 0;
    rx_cmd->dst_row_len    = SIO_DM_BURST_SIZE;
    rx_cmd->dst_row_offset = SIO_DM_BURST_SIZE;

    dmvar->rx_xfer.device       = DMOV_DEVICE_HS_UART;
    dmvar->rx_xfer.chan         = DMOV_CHAN_HS_UART_RX;
    dmvar->rx_xfer.priority     = DMOV_PRI_HS_UART;
    dmvar->rx_xfer.cmd_type     = DMOV_CMD_LIST;
    dmvar->rx_xfer.callback_ptr = siors_dm_rx_cb; 
    dmvar->rx_xfer.options      = DMOV_XFER_DEFAULT; 
    
    dmvar->rx_started   = FALSE;
    dmvar->rx_cmd_state = SIO_RX_CMD_INIT;
    dmvar->tx_started   = FALSE;

    /* DM setup information */
    dmvar->dm_device = DMOV_DEVICE_HS_UART;
    dmvar->dm_channel = DMOV_CHAN_HS_UART_RX;

    /* Zero out the current receive byte count */
    rs_port->gv.current_rx_bytes = 0;

    /* Set the DMOV vote if one is required */
    rs_port->gv.dmov_client_enum = SIORS_FIRST_UART_DMOV_CLIENT;

    /* Setting the clock regime clock adjustment routine */
    rs_port->gv.fp_clk_regime_sel_clk = clk_regime_sel_uart1dm_clk;

    /* Limitation of the FIFO size: it must be a multiple of 16 */
    rs_port->gv.total_fifo_size = SIORS_FIRST_UART_FIFO_SIZE;
    rs_port->gv.rx_fifo_size = rs_port->gv.total_fifo_size / 2;   
    rs_port->gv.tx_fifo_size = rs_port->gv.total_fifo_size / 2;   
}
#endif


#ifdef FEATURE_SECOND_UART
/*===========================================================================

FUNCTION SIORS_DM_UART2DM_INIT                             INTERNAL

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

static void siors_dm_uart2dm_init( siors_port_type *rs_port )
{
    siors_hw_reg_type *hwreg;               /* Hardware register             */
    byte* rx_buffer;
    dmov_cmd_box_s_type* tx_cmd;
    dmov_cmd_box_s_type* rx_cmd;
    byte cmd_id;
    siors_dm_variable_type *dmvar;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    dmvar = &(rs_port->dmv);
    hwreg = &(rs_port->hw);

    /* ---- BASIC access structure initialize ---- */
    hwreg->msmu_mr1  = (dword)HWIO_ADDR(UART2DM_MR1);
    hwreg->msmu_mr2  = (dword)HWIO_ADDR(UART2DM_MR2);
    hwreg->msmu_irda = (dword)HWIO_ADDR(UART2DM_IRDA);
    hwreg->msmu_imr  = (dword)HWIO_ADDR(UART2DM_IMR);
    hwreg->msmu_mvr  = 0;
    hwreg->msmu_nvr  = 0;
    hwreg->msmu_dvr  = 0;
    hwreg->msmu_cr   = (dword)HWIO_ADDR(UART2DM_CR);
    hwreg->msmu_sr   = (dword)HWIO_ADDR(UART2DM_SR);
    hwreg->msmu_isr  = (dword)HWIO_ADDR(UART2DM_ISR);
    hwreg->msmu_tf   = (dword)HWIO_ADDR(UART2DM_TF);
    hwreg->msmu_rf   = (dword)HWIO_ADDR(UART2DM_RF);
    hwreg->msmu_rfwr = (dword)HWIO_ADDR(UART2DM_RFWR);
    hwreg->msmu_csr  = (dword)HWIO_ADDR(UART2DM_CSR);
    hwreg->msmu_tfwr = (dword)HWIO_ADDR(UART2DM_TFWR);
    hwreg->msmu_ipr  = (dword)HWIO_ADDR(UART2DM_IPR);
    hwreg->msmu_hcr  = (dword)HWIO_ADDR(UART2DM_HCR);
    hwreg->msmu_misr = (dword)HWIO_ADDR(UART2DM_MISR);
    hwreg->msmu_snap = (dword)HWIO_ADDR(UART2DM_RX_TOTAL_SNAP);
    hwreg->msmu_dmrx = (dword)HWIO_ADDR(UART2DM_DMRX);
    hwreg->msmu_rxfs = (dword)HWIO_ADDR(UART2DM_RXFS);
    hwreg->msmu_ncft = (dword)HWIO_ADDR(UART2DM_NO_CHARS_FOR_TX);
    hwreg->msmu_dmen = (dword)HWIO_ADDR(UART2DM_DMEN);
    hwreg->msmu_badr = (dword)HWIO_ADDR(UART2DM_BADR);
    hwreg->msmu_phytf = (dword)HWIO_PHYS(UART2DM_TF);
    hwreg->msmu_phyrf = (dword)HWIO_PHYS(UART2DM_RF);
      
  /*-----------------------------------------------------------------------*/
    siors_dm_initialize_common(rs_port);  

  /*-------------------------------------------------------------------------
   Explicitly initialize array of function pointer to be used for the 
   purposes of calling call-back functions which signal end of transmission.
  -------------------------------------------------------------------------*/
    rs_port->gv.siors_isr_func_ptr = siors_dm_isr2;
    rs_port->gv.isr_type = UART2DM_IRQ;
    rs_port->gv.is_isr_registered = FALSE;
    rs_port->gv.clk_source = CLKRGM_UART2DM_CLK;
    rs_port->gv.wakeup_isr_type = UART2DM_RX_IRQ;
    rs_port->gv.wakeup_isr_func_ptr = siors_dm_dp_rx_isr2;
    rs_port->gv.is_wakeup_isr_registered = FALSE;

  /*-------------------------------------------------------------------------
   Allocate data structure required to enable DMOV operation
  -------------------------------------------------------------------------*/
    SIORS_TRACE_CODE(30, rs_port->device_id);

    /* Initialize DM variable structure */
    dmvar->current_rx_cmd = 0;
    dmvar->dm_stale_evt = FALSE;
    
    /* Allocate buffer space for Rx and Tx operations */
    rx_buffer = pmem_malloc(
                     (SIO_DM_RX_BUF_SIZE*SIO_DM_NUM_RX_CMDS)+SIO_DM_BURST_SIZE, 
                     PMEM_UNCACHED_ID);

    if ( NULL == rx_buffer )
    {
      ERR_FATAL("Could not allocate memory",0,0,0);
    }

    dmvar->tx_buffer = rx_buffer + (SIO_DM_RX_BUF_SIZE*SIO_DM_NUM_RX_CMDS);

    /* Initialize Transmit structures */
    tx_cmd = dmvar->tx_cmd.sio_tx_cmd_ptr = 
                    (void *)(((uint32)dmvar->tx_cmd_buf + 0x7 ) & ~0x7);
    tx_cmd->command = DMOV_FIELD(CMD_WORD, LC, 1) |
                    DMOV_FIELD(CMD_WORD, DST_CRCI_CTL, DMOV_CRCI_HS_UART2_TX) |
                DMOV_FIELD(CMD_WORD, ADDR_MODE, DMOV_ADDR_MODE_BOX);
    tx_cmd->src_row_len    = SIO_DM_BURST_SIZE;
    tx_cmd->src_row_offset = SIO_DM_BURST_SIZE;
    tx_cmd->dst_row_addr   = hwreg->msmu_phytf;
    tx_cmd->dst_row_len    = SIO_DM_BURST_SIZE;
    tx_cmd->dst_row_offset = 0;
    dmvar->tx_cmd.in_use      = FALSE;
    dmvar->tx_cmd.rs_port     = rs_port;

    dmvar->tx_xfer.device       = DMOV_DEVICE_HS_UART2;
    dmvar->tx_xfer.chan         = DMOV_CHAN_HS_UART2_TX;
    dmvar->tx_xfer.priority     = DMOV_PRI_HS_UART2;
    dmvar->tx_xfer.cmd_ptr      = dmvar->tx_cmd.sio_tx_cmd_ptr;
    dmvar->tx_xfer.cmd_type     = DMOV_CMD_LIST;
    dmvar->tx_xfer.callback_ptr = siors_dm_tx_cb;
    dmvar->tx_xfer.user_ptr     = (void *)&dmvar->tx_cmd;
    dmvar->tx_xfer.options      = DMOV_XFER_DEFAULT;

    /* Initialize Receive structures */
    for(cmd_id = 0; cmd_id < SIO_DM_NUM_RX_CMDS; cmd_id++)
    {
      /* Divide buffer and check if it is 32 bytes aligned */
      rx_buffer += (SIO_DM_RX_BUF_SIZE*cmd_id);
      ASSERT(((uint32)rx_buffer&0x1F) == 0);
      dmvar->rx_cmd[cmd_id].sio_rx_buffer = (byte *)rx_buffer;
      dmvar->rx_cmd[cmd_id].cmd_id = cmd_id;
      dmvar->rx_cmd[cmd_id].rs_port = rs_port; 
    }

    rx_cmd = dmvar->rx_xfer.cmd_ptr = 
                    (void *)(((uint32)dmvar->rx_cmd_buf + 0x7) & ~0x7);

    rx_cmd->command = DMOV_FIELD(CMD_WORD, LC, 1) |
                    DMOV_FIELD(CMD_WORD, SRC_CRCI_CTL, DMOV_CRCI_HS_UART2_RX) |
                DMOV_FIELD(CMD_WORD, ADDR_MODE, DMOV_ADDR_MODE_BOX);
    rx_cmd->src_row_addr   = hwreg->msmu_phyrf;
    rx_cmd->src_row_len    = SIO_DM_BURST_SIZE;
    rx_cmd->src_row_offset = 0;
    rx_cmd->dst_row_len    = SIO_DM_BURST_SIZE;
    rx_cmd->dst_row_offset = SIO_DM_BURST_SIZE;

    dmvar->rx_xfer.device       = DMOV_DEVICE_HS_UART2;
    dmvar->rx_xfer.chan         = DMOV_CHAN_HS_UART2_RX;
    dmvar->rx_xfer.priority     = DMOV_PRI_HS_UART2;
    dmvar->rx_xfer.cmd_type     = DMOV_CMD_LIST;
    dmvar->rx_xfer.callback_ptr = siors_dm_rx_cb; 
    dmvar->rx_xfer.options      = DMOV_XFER_DEFAULT; 
    
    dmvar->rx_started   = FALSE;
    dmvar->rx_cmd_state = SIO_RX_CMD_INIT;
    dmvar->tx_started   = FALSE;

    /* DM setup information */
    dmvar->dm_device = DMOV_DEVICE_HS_UART2;
    dmvar->dm_channel = DMOV_CHAN_HS_UART2_RX;

    /* Zero out the current receive byte count */
    rs_port->gv.current_rx_bytes = 0;

    /* Set the DMOV vote if one is required */
    rs_port->gv.dmov_client_enum = SIORS_SECOND_UART_DMOV_CLIENT;

    /* Setting the clock regime clock adjustment routine */
    rs_port->gv.fp_clk_regime_sel_clk = clk_regime_sel_uart2dm_clk;

    /* Limitation of the FIFO size: it must be a multiple of 16 */
    rs_port->gv.total_fifo_size = SIORS_FIRST_UART_FIFO_SIZE;
    rs_port->gv.rx_fifo_size = rs_port->gv.total_fifo_size / 2;   
    rs_port->gv.tx_fifo_size = rs_port->gv.total_fifo_size / 2;   
}
#endif


/*===========================================================================

FUNCTION SIORS_DM_OPEN                                          EXPORTED

DESCRIPTION
  This function opens a port so it can be used
    
DEPENDENCIES
  None
  
RETURN VALUE
  Returns non-zero if the port failed to open.
  
SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_dm_open ( void *pHead, 
                                          /* TRUE if re-initialization is not required */
                                boolean quick )
{
  siors_port_type      *rs_port = pHead;
  siors_hw_reg_type    *hwreg = &(rs_port->hw);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Make sure we did not re-open an opened port */
  if(rs_port->gv.is_isr_registered == TRUE)
  {
    return;
  }
  
  SIORS_TRACE_CODE(55, rs_port->device_id);

  /* Turning on the clock when the port is opened */
  CLK_REGIME_MSM_ENABLE(rs_port->gv.clk_source);    

  rs_port->is_clock_enabled = TRUE;

  /* Disable IRDA tranceiver.  No QC product currently uses IRDA. */
  MSMU_OUT( hwreg->msmu_irda, MSMU_IRDA_DISABLE);

  /* Protect against consective commands */
  MSMUW_OUT( hwreg->msmu_cr, MSMU_CR_ENA_CR_PROT);

  /* Enable RX and TX DM and set base address of FIFO. Currently equal sizes
     are allocated for TX and RX FIFO */
  MSMU_OUT(hwreg->msmu_dmen, MSMU_DMEN_RXTX_DM_ENA );

  /* Check the default BADR value, if it is large FIFO, use the reduced FIFO
   * BADR value if the feature flag is in effect */
  if(SIORS_USES_REDUCED_RX_FIFO_SIZE && (inpdw(hwreg->msmu_badr) > MSMU_BADR_DEFAULT))
  {
    MSMU_OUT(hwreg->msmu_badr, SIORS_UARTDM_BADR_REDUCED_FIFO_DEFAULT );
  } 
  else
  {
    MSMU_OUT(hwreg->msmu_badr, MSMU_BADR_DEFAULT );
  }

  if(rs_port->gv.dmov_client_enum != DMOV_CLIENT_VOTE_NOT_IN_USE)
  {
     DMOV_ENABLE_CLIENT(rs_port->gv.dmov_client_enum);
  }

  /* Program the M N and D register with some default rate */
  siors_dm_set_uart_clock_div_regs(rs_port,
                  rs_port->gv.sio_mvr_default,
                  rs_port->gv.sio_nvr_default,
                  rs_port->gv.sio_dvr_default);

  rs_port->gv.sio_imr_shadow = 0;
  MSMU_OUT( hwreg->msmu_imr, rs_port->gv.sio_imr_shadow);
  MSMUW_OUT( hwreg->msmu_cr,  MSMU_CR_RESET_ERR);

  siors_dm_reset_receiver( rs_port );
  /* During non-reset device open, and tx break is in use, do not reset 
   * transmitter, this is done so TX break condition is not destroyed */
  if(!(rs_port->gv.tx_break_state && quick))
  {
    siors_dm_reset_transmitter( rs_port );
  }

  /* Program for eight bits per character, 1 stop bit and block check error
   * mode */
  MSMU_OUT(hwreg->msmu_mr2, (MSMU_MR2_8BPC | MSMU_MR2_1SB | MSMU_MR2_ERRMODE));

  /* Now set Automatic RFR in UART (for outbound flow monitoring). */
  rs_port->gv.sio_mr1_shadow = MSMU_MR1_CTSC;
  MSMU_OUT(hwreg->msmu_mr1, rs_port->gv.sio_mr1_shadow);

  /* When port is opened, register the ISR  */
  tramp_set_isr( rs_port->gv.isr_type , rs_port->gv.siors_isr_func_ptr);
  rs_port->gv.is_isr_registered = TRUE;

  /*  UARTDM specific setup */
  MSMUW_OUT(hwreg->msmu_badr, SIORS_UARTDM_DEFAULT_BADR);
  MSMUW_OUT(hwreg->msmu_dmen, SIORS_UARTDM_DEFAULT_DMEN);

  MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_ERR);

  /* Set DMRX to some default size first */
  MSMUDW_OUT(hwreg->msmu_dmrx, SIO_DM_RX_BUF_SIZE);
    
  /* Setting the bit rate also enable the TX/RX */
  siors_dm_set_bit_rate(rs_port,
                     rs_port->gv.sio_current_rx_bitrate);

  rs_port->gv.is_port_closing = FALSE;
}


/*===========================================================================

FUNCTION SIORS_DM_CLOSE                                     EXPORTED 

DESCRIPTION
  This function close a port in use
    
DEPENDENCIES
  None
  
RETURN VALUE
  None
   
SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_dm_close ( void *pHead )
{
  siors_port_type  *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Make sure we don't close the port, if the port is not open */
  if(rs_port->gv.is_isr_registered == FALSE)
  {
    return;
  }

  SIORS_TRACE_CODE(56, rs_port->device_id);

  // prevent DMOV error by first disabling DMOV mode.
  MSMU_OUT(rs_port->hw.msmu_dmen, MSMU_DMEN_RXTX_DM_DIS );  

  siors_dm_reset_receiver( rs_port );
  /* TX use to be resetted upon close, but it is not done anymore because when
   * TX is resetted, the break state setting is destroyed */

  tramp_set_isr( rs_port->gv.isr_type , NULL);
  rs_port->gv.is_isr_registered = FALSE;

  rs_port->is_clock_enabled = FALSE;
  /* Turning off the clock as we are done with the port */
  CLK_REGIME_MSM_DISABLE(rs_port->gv.clk_source);    
  /* Mark the port is closing, so when flush callback occurs, disable DMOV*/
  rs_port->gv.is_port_closing = TRUE;
}



/*===========================================================================
 
FUNCTION SIORS_DM_TX_CB

DESCRIPTION
   This function is internal. This is a callback that gets called when 
   transmit completes, allowing queuing up of another tx command to DM.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   Updates sio_bytes_to_tx and possibly sio_current_tx_ptr

===========================================================================*/
static void siors_dm_tx_cb
(
  const dmov_result_s_type *result_ptr,
  void  *user_ptr
)
{
  siors_port_type *rs_port = (((sio_tx_cmd_struct *)user_ptr)->rs_port);

  SIORS_TRACE_CODE(120, rs_port->device_id);
 
  /* Mark the command as free */
  ((sio_tx_cmd_struct *)user_ptr)->in_use = FALSE;
  
  /* Allow enabling TX ISR */
  rs_port->gv.ignore_tx_isr_enable = FALSE;

  /* Tell MDD that current packet is finished */
  rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_TX_PKT_DONE);

  SIORS_TRACE_CODE(121, rs_port->device_id);
}



/*===========================================================================

FUNCTION SIORS_DM_TRANSMIT                                  EXPORTED

DESCRIPTION
  This function transmits over MSM's RS-232 UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dm_transmit
(
  void               *pHead,
  byte               *tx_ptr,             /* Packet for transmission       */
  int32              *byte_count          /* Number of character to sent   */
)
{
  int32 bytes_to_tx;              /* Temporary for storing the number of
                                     bytes which need to be transmitted.    */
  siors_port_type *rs_port = pHead;
  siors_hw_reg_type *hwreg;
  siors_dm_variable_type *dmvar;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  hwreg = &(rs_port->hw);
  dmvar = &(rs_port->dmv);

  bytes_to_tx = *byte_count;

  if (!rs_port->is_clock_enabled){
    *byte_count = 0;
    return;
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SIORS_TRACE_CODE(130, rs_port->device_id);

  if(rs_port->gv.tx_break_state)
  {
    ERR( "Transmitting while in BREAK",0,0,0);
  }

  /* Check if TX_READY is set. This check should not be done if the UART core
   * is coming out of a reset because TX_READY will not be set. It gets set
   * only after the first Tx tranfer is complete */
  if( dmvar->tx_started )
  {
    if ( (inp(hwreg->msmu_isr) & MSMU_ISR_TX_READY) == 0 )
    {
      /* If TX is not ready, return right away with no byte sent */
      /* Have to wait for TX ISR to come, when TX is ready       */
      *byte_count = 0;
      rs_port->gv.ignore_tx_isr_enable = FALSE;
      SIORS_TRACE_CODE(131, rs_port->device_id);
      return;
    }
  }
  else
  {
    dmvar->tx_started = TRUE;
  }

  /* Assert that TX command is free */
  ASSERT(dmvar->tx_cmd.in_use == FALSE);

  /* Assert that sio_bytes_to_tx is not 0  and less than dmov max of 65535 */
  ASSERT(bytes_to_tx != 0);
  ASSERT(bytes_to_tx < 65535);

  SIORS_TRACE_TX_CHAR_COUNT(bytes_to_tx, rs_port->device_id);
  SIORS_TRACE_TXED_CHARS(tx_ptr, bytes_to_tx, rs_port->device_id);

  INTLOCK();

  /* Clear TX_READY bit */
  MSMU_OUT(hwreg->msmu_cr, MSMU_CR_RESET_TX_RDY);

  /* Write NO_CHARS_FOR_TX register */
  MSMUDW_OUT(hwreg->msmu_ncft, bytes_to_tx);

  /* Fill up command structure */
  dmvar->tx_cmd.sio_tx_cmd_ptr->src_row_addr = (uint32)tx_ptr;

  /* Convert bytes to number of SIO_DM_BURST_SIZE bursts */
  dmvar->tx_cmd.sio_tx_cmd_ptr->src_row_num  = (bytes_to_tx+15)>>4;
  dmvar->tx_cmd.sio_tx_cmd_ptr->dst_row_num  = (bytes_to_tx+15)>>4;
  
  /* Send command to DM and mark command as busy */
  if ( dmov_transfer(&dmvar->tx_xfer) != TRUE )
  {
    ERR_FATAL("Call to dmov_transfer failed",0,0,0);
  }
  dmvar->tx_cmd.in_use = TRUE;

  /* Always disable the TX interrupt after queueing an TX packet */

  siors_dm_disable_transmit_interrupt ( rs_port );

  INTFREE();

  /* Even though the transmit started, we still mark the packet as being
   * transmitted, wait until a TX callback event before release the packet */
  /* Also keep the TX disabled until TX callback event */
  *byte_count = 0;
  rs_port->gv.ignore_tx_isr_enable = TRUE;
  SIORS_TRACE_CODE(132, rs_port->device_id);
}


/*===========================================================================
 
FUNCTION SIORS_DM_FLUSH_RX                              INTERNAL

DESCRIPTION
   This function calls a DM driver function to interrupt the DM command and
   flush the receive buffers.

DEPENDENCIES
   Should be called when a stale interrupt is detected or when driver is being
   stopped

RETURN VALUE
   None

SIDE EFFECTS
   Flushes the DM UART receive channel

===========================================================================*/
static void siors_dm_flush_rx
(
  siors_port_type *rs_port                   /* Port                       */
)
{

  siors_dm_variable_type *dmvar;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dmvar = &(rs_port->dmv);

  /* RX stopped after a flush */
  dmvar->rx_started = FALSE;

  SIORS_TRACE_CODE(40, rs_port->device_id);
 
  /* If RX command has not been sent or if command is already being flushed
   * then return without doing anything
   */
  if( (dmvar->rx_cmd_state == SIO_RX_CMD_FLUSHING) ||
      (dmvar->rx_cmd_state == SIO_RX_CMD_INIT)
    )
  {
    SIORS_TRACE_CODE(41, rs_port->device_id);
    return;
  }

  if( dmvar->rx_cmd_state == SIO_RX_CMD_PENDING )
  {
    SIORS_TRACE_CODE(44, rs_port->device_id);
    return;
  }

  /* If RX command has been queued to DM then flush the command */
  if( dmvar->rx_cmd_state == SIO_RX_CMD_SENT )
  {
    SIORS_TRACE_CODE(42, rs_port->device_id);
    dmvar->rx_cmd_state = SIO_RX_CMD_FLUSHING;
    if( ! dmov_chan_flush( dmvar->dm_device, dmvar->dm_channel, FALSE ) )
    {
      ERR_FATAL("DMOV chan flush failed",0,0,0);
    }

    return;
  }

  SIORS_TRACE_CODE(43, rs_port->device_id);
  dmvar->rx_cmd_state = SIO_RX_CMD_INIT;
}


/*===========================================================================
 
FUNCTION SIORS_DM_START_RX

DESCRIPTION
   This function starts the RX process by queueing a receive command to the DM

DEPENDENCIES
   Should be called only from siors_enable_receiver

RETURN VALUE
   None

SIDE EFFECTS
   Queues a DM command to wait for receive characters.

===========================================================================*/
static void siors_dm_start_rx
(
  siors_port_type *rs_port                   /* Port                       */
)
{
  dmov_cmd_box_s_type* rx_cmd;        /* Receive command */
  siors_hw_reg_type *hwreg;               /* UART HW regs                  */
  siors_dm_variable_type *dmvar;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dmvar = &(rs_port->dmv);
  hwreg = &(rs_port->hw);
  
  SIORS_TRACE_CODE(50, rs_port->device_id);

  /* Check the state of the drivers for consistency and initialize receive
   * state 
   */
  ASSERT(dmvar->rx_cmd_state != SIO_RX_CMD_SENT);
  ASSERT(dmvar->dm_stale_evt == FALSE);

  dmvar->rx_started = TRUE;

  /* If the state is pending when start_rx is called, keep the state at 
   * pending, since start_rx must have been called twice, the second call
   * is usually harmless, and can wait until flush is complete. */
  if( dmvar->rx_cmd_state == SIO_RX_CMD_PENDING )
  {    
    SIORS_TRACE_CODE(53, rs_port->device_id);
    return;
  }
  
  /* If Receive command is being flushed then switch state to Pending. When
   * the flushing is complete and the callback is called, then the command will
   * be queued to DM */
  if( dmvar->rx_cmd_state == SIO_RX_CMD_FLUSHING )
  {    
    SIORS_TRACE_CODE(51, rs_port->device_id);

    dmvar->rx_cmd_state = SIO_RX_CMD_PENDING;
    return;
  }

  ASSERT(dmvar->rx_cmd_state == SIO_RX_CMD_INIT );

  /* Initialize current_rx_bytes variable before starting receive */
  rs_port->gv.current_rx_bytes = 0;

  dmvar->current_rx_cmd = 0;

  /* Queue the receive command to DM */
  dmvar->rx_cmd[dmvar->current_rx_cmd].bytes_received = 0;
  dmvar->rx_xfer.user_ptr = (void *)&dmvar->rx_cmd[dmvar->current_rx_cmd]; 
  rx_cmd               = dmvar->rx_xfer.cmd_ptr;
  rx_cmd->dst_row_addr = (uint32)(dmvar->rx_cmd[dmvar->current_rx_cmd].sio_rx_buffer);
 
  /* Software workaround for missing stale event. In this case a 16byte receive
   * transfer is initiated first and we keep track of whether this transfer
   * has been completed or not
   */
  rx_cmd->src_row_num  = SIO_DM_RX_BUF_SIZE/SIO_DM_BURST_SIZE;
  rx_cmd->dst_row_num  = SIO_DM_RX_BUF_SIZE/SIO_DM_BURST_SIZE;

  if( dmov_transfer(&dmvar->rx_xfer) == FALSE )
  {
    ERR_FATAL("dmov_transfer failed",0,0,0);
  }

  dmvar->rx_cmd_state = SIO_RX_CMD_SENT;

  /* Enable stale event */
  MSMU_OUT(hwreg->msmu_cr, MSMU_CR_ENA_STALE_EVT);

  /* Write to DMRX register to do a new transfer initialization */
  MSMUDW_OUT(hwreg->msmu_dmrx, SIO_DM_RX_BUF_SIZE);

  SIORS_TRACE_CODE(52, rs_port->device_id);
}



/*===========================================================================
 
FUNCTION SIORS_DM_RX_CB

DESCRIPTION
   This function is a callback that gets called when either the RX command
   is flushed or when the receive buffer is full.
   TODO: For some reason, as soon as STALE event is enabled using the command
   register, the STALE interrupt is triggered, even though there is no data
   in the RX FIFO.  Current workaround is to filter out STALE event that has
   zero byte in size.  However, this issue is not observed in the old
   UARTDM driver.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   Another Rx DM command is queued

===========================================================================*/
void siors_dm_rx_cb
(
  const dmov_result_s_type *result_ptr,
  void  *user_ptr
)
{
  volatile byte misr_val;  /* Masked Interrupt Status Register Value       */
  sio_rx_cmd_struct* rx_cmd_ptr = user_ptr;
  byte current_id, new_id;
  dmov_cmd_box_s_type* rx_cmd;
  siors_hw_reg_type *hwreg;               /* UART HW regs                  */
  siors_port_type *rs_port; 
  siors_dm_variable_type *dmvar;
  uint32 i_save;                      /* holds PSW for interrupt disabling */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rs_port = (rx_cmd_ptr->rs_port); 
  hwreg = &(rs_port->hw); 
  dmvar = &(rs_port->dmv);

  /* Blanket the entire DM PD function with INTLOCK, this is because there
   * is a chance that wake up ISR can cause the port to re-open while pending 
   * for a callback, causing cmd_state to be corrupted */

  INTLOCK_SAV( i_save);
  ASSERT(dmvar->rx_cmd_state != SIO_RX_CMD_INIT);

  SIORS_TRACE_CODE(60, rs_port->device_id);

  /* If receive state is NOT STARTED then it means that receive operation 
   * needs to be stopped. Hence do not queue any commands 
   */
  if ( dmvar->rx_started == FALSE )
  {
    SIORS_TRACE_CODE(61, rs_port->device_id);
    ASSERT(dmvar->rx_cmd_state == SIO_RX_CMD_FLUSHING);
    dmvar->rx_cmd_state = SIO_RX_CMD_INIT;

    /* Don't need DMOV anymore */
    if((rs_port->gv.is_port_closing) &&
       (rs_port->gv.dmov_client_enum != DMOV_CLIENT_VOTE_NOT_IN_USE))
    {
       timer_reg(&(rs_port->gv.dmov_disable_cb_item),
                  (timer_t2_cb_type)siors_dm_mdm_disable_timer_cb,
                  (timer_cb_data_type)rs_port,
                  (int4)CLK_MS_PER_TICK,
                  (int4)0);
    }
    INTFREE_SAV( i_save);
    return;
  }

  /* If receive command state is pending this means that a command needs to be
   * queued to the DM for receive transfer 
   */
  if ( dmvar->rx_cmd_state == SIO_RX_CMD_PENDING )
  {
    SIORS_TRACE_CODE(62, rs_port->device_id);

    /* Initialize current_rx_bytes variable before starting receive */
    rs_port->gv.current_rx_bytes = 0;

    /* Calculate next command id for use */
    dmvar->current_rx_cmd = (dmvar->current_rx_cmd+1)%(SIO_DM_NUM_RX_CMDS);

    /* Queue a receive command to the DM */
    dmvar->rx_cmd[dmvar->current_rx_cmd].bytes_received = 0;
    dmvar->rx_xfer.user_ptr = (void *)&dmvar->rx_cmd[dmvar->current_rx_cmd]; 
    rx_cmd               = dmvar->rx_xfer.cmd_ptr;

    /* Setup for the next DM transfer */
    rx_cmd->src_row_num  = SIO_DM_RX_BUF_SIZE/SIO_DM_BURST_SIZE;
    rx_cmd->dst_row_num  = SIO_DM_RX_BUF_SIZE/SIO_DM_BURST_SIZE;
    rx_cmd->dst_row_addr = (uint32)(dmvar->rx_cmd[dmvar->current_rx_cmd].sio_rx_buffer);

    if( dmov_transfer(&dmvar->rx_xfer) == FALSE )
    {
      ERR_FATAL("dmov_transfer failed",0,0,0);
    }
    /* Possible race condition, move it up here */
    dmvar->rx_cmd_state = SIO_RX_CMD_SENT;
  
    INTFREE_SAV( i_save);
  
    /* Enable stale event */
    MSMU_OUT(hwreg->msmu_cr, MSMU_CR_ENA_STALE_EVT);
  
    /* Write to DMRX register to do a new transfer initialization */
    MSMUDW_OUT(hwreg->msmu_dmrx, SIO_DM_RX_BUF_SIZE);

    return;
  }

  /*  Check for error condition here */
  if ( !DMOV_RESULT_OK(result_ptr) ) 
  {
    ASSERT((dmvar->rx_cmd_state==SIO_RX_CMD_FLUSHING)&& (dmvar->dm_stale_evt));
  }

  /* Read MISR and verify if it matches one stored in siors_isr */
  misr_val = ( inp(hwreg->msmu_misr) & rs_port->gv.sio_imr_shadow);

  new_id = current_id = rx_cmd_ptr->cmd_id;

  ASSERT(current_id == dmvar->current_rx_cmd);
  ASSERT(current_id < SIO_DM_NUM_RX_CMDS);

  /* If execution reached here then it means that a stale event fired or a
   * transfer was completed. Check for the conditions and calculate number of
   * bytes received.
   */
  if ( dmvar->dm_stale_evt )
  {
    SIORS_TRACE_CODE(64, rs_port->device_id);
    ASSERT(dmvar->rx_cmd_state == SIO_RX_CMD_FLUSHING);
    dmvar->rx_cmd[current_id].bytes_received = inpdw(hwreg->msmu_snap);
    dmvar->dm_stale_evt = FALSE;

    /* For some reason, the hardware will report a SNAP value of 0 byte when
     * the hardware is first initialized (Not observed in the old driver)  
     * If this is the case, record the event                 */
    if(dmvar->rx_cmd[current_id].bytes_received == 0)
    {
      SIORS_TRACE_CODE(68, rs_port->device_id);
    }
  }
  else
  {
    SIORS_TRACE_CODE(65, rs_port->device_id);
    ASSERT(dmvar->rx_cmd_state == SIO_RX_CMD_SENT);
    /* Check to see if STALE event has occurred, since DMOV has higher ISR
     * priority than UART, it is possible STALE has occurred but DMOV
     * callback happened first. */
    if((misr_val & (MSMU_ISR_RXSTALE)) != 0)
    {
      /* Treat this case as a normal stale event */
      SIORS_TRACE_CODE(66, rs_port->device_id);
      MSMUW_OUT(hwreg->msmu_cr, MSMU_CR_DIS_STALE_EVT);
      MSMUW_OUT(hwreg->msmu_cr, MSMU_CR_RESET_STALE);
      dmvar->rx_cmd_state = SIO_RX_CMD_FLUSHING;
      dmvar->rx_cmd[current_id].bytes_received = inpdw(hwreg->msmu_snap);

      /* Again, if SNAP report a value of 0 byte, record the event */
      if(dmvar->rx_cmd[current_id].bytes_received == 0)
      {
        SIORS_TRACE_CODE(68, rs_port->device_id);
      }
    }
    else
    {
      dmvar->rx_cmd[current_id].bytes_received = SIO_DM_RX_BUF_SIZE;
    }
  }

  /* Calculate next command id */
  new_id = (current_id+1)%(SIO_DM_NUM_RX_CMDS);

  /* Queue another RX command to the DM */
  dmvar->rx_cmd[new_id].bytes_received = 0;
  dmvar->rx_xfer.user_ptr = (void *)&dmvar->rx_cmd[new_id]; 
  rx_cmd               = dmvar->rx_xfer.cmd_ptr;
  rx_cmd->src_row_num  = SIO_DM_RX_BUF_SIZE/SIO_DM_BURST_SIZE;
  rx_cmd->dst_row_num  = SIO_DM_RX_BUF_SIZE/SIO_DM_BURST_SIZE;
  rx_cmd->dst_row_addr = (uint32)(dmvar->rx_cmd[new_id].sio_rx_buffer);

  dmvar->rx_cmd_state = SIO_RX_CMD_SENT;

  if( dmov_transfer(&dmvar->rx_xfer) == FALSE )
  {
    ERR_FATAL("dmov_transfer failed",0,0,0);
  }
  
  /* Enable stale event */
  MSMU_OUT(hwreg->msmu_cr, MSMU_CR_ENA_STALE_EVT);
  
  /* Write to DMRX register to do a new transfer initialization */
  MSMUDW_OUT(hwreg->msmu_dmrx, SIO_DM_RX_BUF_SIZE);

  SIORS_TRACE_CHAR_COUNT_RCVD(dmvar->rx_cmd[current_id].bytes_received, 
                              rs_port->device_id);

  /* Before processing the received data, copy the pointer and count into
   * general variable structure */
  ASSERT(rs_port->gv.current_rx_bytes == 0);
  rs_port->gv.current_rx_buf = dmvar->rx_cmd[current_id].sio_rx_buffer;
  rs_port->gv.current_rx_bytes = dmvar->rx_cmd[current_id].bytes_received;
  dmvar->current_rx_cmd = new_id;
  /* Call MDD layer to process the received characters, if there is sometime
   * to process */
  if(dmvar->rx_cmd[current_id].bytes_received != 0)
  {
    rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_RXSTALE_INTR);
  }

  INTFREE_SAV( i_save);
  SIORS_TRACE_CODE(67, rs_port->device_id);
}



/*===========================================================================

FUNCTION SIORS_DM_RECEIVE                                   EXPORTED

DESCRIPTION
  This function receive data from MSM's RS-232 UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dm_receive
(
  void               *pHead,
  byte               *rx_ptr,             /* Buffer to store incoming data */
  uint32             *byte_count          /* Number of character to get    */
)
{
  uint32 bytes_to_rx;             /* Temporary for storing the number of
                                     bytes which need to be received   .    */
  uint32 rx_data_len;
  byte  *rx_buffer_ptr;
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  bytes_to_rx = *byte_count;

  if (!rs_port->is_clock_enabled){
    *byte_count = 0;
    return;
  }

  /* Read the amout of data to be transfered */
  rx_data_len = rs_port->gv.current_rx_bytes;
  rx_buffer_ptr = rs_port->gv.current_rx_buf;

  /* Check to see if there is enough room to hold all the data */ 
  if(rx_data_len <= bytes_to_rx)
  {
    bytes_to_rx = rx_data_len;
  }
  
  /* Copy the data into the user buffer */
  memcpy(rx_ptr, rx_buffer_ptr, bytes_to_rx);
  
  rs_port->gv.current_rx_bytes -= bytes_to_rx;
  rs_port->gv.current_rx_buf += bytes_to_rx;

  *byte_count = bytes_to_rx;
}


/*===========================================================================

FUNCTION SIORS_DM_RECEIVE_TAIL_CHAR                         EXPORTED

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

static void siors_dm_receive_tail_char
(
  void               *pHead,
  byte               *rx_ptr,             /* Buffer to store incoming data */
  uint32             *byte_count,         /* Number of character to get    */
  byte               tail_char,
  boolean            *more_data
)
{
  uint32 i;                       /* Loop counter                           */
  uint32 bytes_to_rx;             /* Temporary for storing the number of
                                     bytes which need to be received   .    */
  uint32 rx_data_len;
  byte  *rx_buffer_ptr;
  byte  *current_rx_ptr;
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  bytes_to_rx = *byte_count;

  if (!rs_port->is_clock_enabled){
    *byte_count = 0;
    return;
  }

  /* Read the amout of data to be transfered */
  rx_data_len = rs_port->gv.current_rx_bytes;
  rx_buffer_ptr = rs_port->gv.current_rx_buf;

  /* Check to see if there is enough room to hold all the data */ 
  if(rx_data_len <= bytes_to_rx)
  {
    bytes_to_rx = rx_data_len;
  }
  
  current_rx_ptr = rx_ptr;
  for(i=0;i<bytes_to_rx;i++)
  {
    if(*rx_buffer_ptr == tail_char)
    {
      *current_rx_ptr++ = *rx_buffer_ptr++;
      i++;
      break;
    }
    else
    {
      *current_rx_ptr++ = *rx_buffer_ptr++;
    }
  }

  rs_port->gv.current_rx_bytes -= i;
  rs_port->gv.current_rx_buf = rx_buffer_ptr;

  *byte_count = i;
  if((rx_data_len - i) == 0)
  {
      *more_data = FALSE;
  }
  else
  {
      *more_data = TRUE;
  }
}


/*===========================================================================

FUNCTION SIORS_DM_GET_CURRENT_CTS                               EXPORTED

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
static void siors_dm_get_current_rts
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

FUNCTION SIORS_DM_IS_TX_FIFO_EMPTY                              EXPORTED

DESCRIPTION
  Returns 1 if TX FIFO is empty, otherwise returns 0

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int32 siors_dm_is_tx_fifo_empty ( void *pHead )
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

FUNCTION SIORS_DM_CHECK_RX_CABLE_STATUS                     EXPORTED

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
static boolean siors_dm_check_rx_cable_status
(
  void                      *pHead
)
{
  siors_port_type     *rs_port = pHead;
  boolean              ret;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ret = tramp_check_raw_interrupt(rs_port->gv.wakeup_isr_type);
  tramp_clear_interrupt(rs_port->gv.wakeup_isr_type);
  return ret;
}


/*===========================================================================

FUNCTION SIORS_DM_SET_TX_BREAK_STATE                          EXPORTED

DESCRIPTION
  Set the break state on the TX line.  If input is TRUE, start the 
  break state.  If input is FALSE, end the break state.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void siors_dm_set_tx_break_state ( void *pHead, boolean break_enabled )
{
  siors_port_type     *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!rs_port->is_clock_enabled){
    /* Turning on the clock so the port register can be accessed */
    CLK_REGIME_MSM_ENABLE(rs_port->gv.clk_source);    
  }

  if (break_enabled)
  {
    MSMU_OUT( rs_port->hw.msmu_cr, MSMU_CR_STA_BRK);
    if ((inp(rs_port->hw.msmu_sr) & MSMU_SR_TXEMT) == 0)
    {
      ERR( "BREAK during transmitting",0,0,0);
    }
  }
  else
  {
    MSMU_OUT( rs_port->hw.msmu_cr, MSMU_CR_STO_BRK);
  }
  rs_port->gv.tx_break_state = break_enabled;

  if (!rs_port->is_clock_enabled){
    /* Turning off the clock if the UART is not enabled before */
    CLK_REGIME_MSM_ENABLE(rs_port->gv.clk_source);    
  }
}


  

/*===========================================================================

FUNCTION SIORS_DM_INITIALIZE                            EXTERNALIZED FUNCTION

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
void siors_dm_initialize
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
    rs_port = &(siors_dm_ports[device_id]);

    /* Record the MDD callback function */
    ASSERT(mdd_callback);
    rs_port->gv.mdd_callback_func_ptr = mdd_callback;
    
    /* Sending these back to MDD */
    rs_func = pFuncTab;
    ASSERT(rs_func);
    (*pHead) = rs_port;
    rs_port->gv.mdd_head_ptr = pMddHead;
    
    /* Initialize the API table */
    rs_func->enable_tx_hw_fctl = siors_dm_enable_tx_hardware_fctl;
    rs_func->disable_tx_hw_fctl = siors_dm_disable_tx_hardware_fctl;
    rs_func->enable_rx_hw_fctl = siors_dm_enable_rx_hardware_fctl;
    rs_func->disable_rx_hw_fctl = siors_dm_disable_rx_hardware_fctl;
    rs_func->enable_tx_interrupt = siors_dm_enable_transmit_interrupt;
    rs_func->disable_tx_interrupt = siors_dm_disable_transmit_interrupt;
    rs_func->enable_auto_hw_fctl = siors_dm_enable_auto_hrdwr_fctl;
    rs_func->disable_auto_hw_fctl = siors_dm_disable_auto_hrdwr_fctl;
    rs_func->detect_rx_activity = siors_dm_detect_rx_activity;
    rs_func->set_bit_rate = siors_dm_set_bit_rate;
    rs_func->reset_tx = siors_dm_reset_transmitter;
    rs_func->enable_tx = siors_dm_enable_transmitter;
    rs_func->reset_rx = siors_dm_reset_receiver;
    rs_func->enable_rx = siors_dm_enable_receiver;
    rs_func->disable_rx = siors_dm_disable_receiver;
    rs_func->open = siors_dm_open;
    rs_func->close = siors_dm_close;
    rs_func->transmit = siors_dm_transmit;
    rs_func->receive = siors_dm_receive;
    rs_func->receive_tail_char = siors_dm_receive_tail_char;
    rs_func->transmit_single_char = NULL;
    rs_func->get_current_rts = siors_dm_get_current_rts;
    rs_func->is_tx_fifo_empty = siors_dm_is_tx_fifo_empty;
    rs_func->check_rx_cable_status = siors_dm_check_rx_cable_status;
    rs_func->set_tx_break_state = siors_dm_set_tx_break_state;

    
    rs_port->device_id = device_id;
        
    switch (device_id)
    {
#ifdef FEATURE_FIRST_UART
      case UART_DEVICE_1:
        siors_dm_uart1dm_init( rs_port );
        break;
#endif

#ifdef FEATURE_SECOND_UART
      case UART_DEVICE_2:
        siors_dm_uart2dm_init( rs_port );
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

