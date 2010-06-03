/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   S I O R S 2 3 2   D M   L I T E

         SIO Real Device Layer for UARTDM - does not use DataMover

GENERAL DESCRIPTION

  This Module is the real physical device layer of DMSS Serial I/O for an 
  UART hardware.  It takes care of all of the hardware dependent functionalities 
  of Serial I/O.  Only asynchronous mode is supported.

EXTERNALIZED FUNCTIONS

  siors_dml_initialize
    This function initialize the actual driver layer and 
    populate the exported API table so the media driver layer can access 
    the physical driver layer code

PHYSICAL DEVICE LAYER EXPORTED FUNCTIONS
  siors_dml_enable_tx_hardware_fctl
    Enable hardware flow control on transmitter

  siors_dml_disable_tx_hardware_fctl
    Disable hardware flow control on transmitter

  siors_dml_enable_rx_inbound_fctl
    Enable inbound hardware flow control on receiver

  siors_dml_disable_rx_inbound_fctl
    Disable inbound hardware flow control on receiver

  siors_dml_enable_transmit_interrupt
    Enable transmit interrupt
  
  siors_dml_disable_transmit_interrupt
    Disable transmit interrupt

  siors_dml_enable_auto_hrdwr_fctl
    Enable automatic hardware flow control on receiver

  siors_dml_disable_auto_hrdwr_fctl
    Disable automatic hardware flow control on receiver

  siors_dml_set_bit_rate
    Set the current bit rate on the UART

  siors_dml_reset_transmitter
    Reset the transmitter

  siors_dml_enable_transmitter
    Enable the transmitter

  siors_dml_reset_receiver
    Reset the receiver

  siors_dml_enable_receiver
    Enable the receiver

  siors_dml_disable_receiver
    Disable the receiver

  siors_dml_open
    Called when the port is opened for communication

  siors_dml_close
    Called when the port is closed
   
  siors_dml_transmit
    Transmit a packet

  siors_dml_receive
    Receive a packet

  siors_dml_receive_tail_char
    Receive a packet with detection on the tail character
   
  siors_dml_get_current_rts
    Get the current status on the RTS line

  siors_dml_is_tx_fifo_empty
    Check to see if the TX FIFO is empty
   
  siors_dml_check_rx_cable_status
    Check to see if the RX line is in a extended marked state, suggesting
    the cable has been disconnected

  siors_dml_detect_rx_activity
    Enable the detection of RX activity
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  siors_dml_initialize() must be called prior to accessing the exported
  API table.

Copyright (c) 2007-2008 by QUALCOMM Incorporated.  
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors232_dm_lite.c#9 $ $DateTime: 2009/04/02 17:24:01 $ $Author: cmihalik $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/08   cm      Apply workaround for HW bug only on targets that have HW bug
10/01/08   cm      Don't reset transmitter when closing the port
06/24/08   rh      Adjust rx_fifo_state workaround delay duration
06/11/08   rh      Remove the use of SW STALE
06/06/08   rh      Correct some badly named function
                   Add workaround for bad rx_fifo_state value
05/28/08   rh      Add workaround for SNAP register reading 0 after SW stale
04/30/08   rh      Klocwork cleanup
04/16/08   rh      Add software data re-alignment for TX
04/09/08   rh      Not resetting the RX if BREAK state is entered
04/03/08   rh      Adding support for 3.2Mbps
03/24/08   rh      Fix an issue which cause the RX to fail to start
03/20/08   rh      Add support for entering and exiting TX break state
03/04/08   rh      Code cleanup + Add support for 3.2Mbps
02/01/08   rh      Allows the use of higher bitrate by default
09/24/07   rh      Add new quick open option for power up/down event
08/15/07   rh      Make sure RX detection is disabled when port is closed
08/10/07   rh      Add RX activity detection
07/27/07   rh      Remove some unused code
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
#include "siors_lib.h"

//  If there is at least one UART.
#if( defined(FEATURE_FIRST_UART)  || \
     defined(FEATURE_SECOND_UART) || \
     defined(FEATURE_THIRD_UART))                    //{

//  Remove entire file if no DM lite driver in use.
#if((SIORS_FIRST_UART_TYPE  == HW_UARTDM_LT) ||\
    (SIORS_SECOND_UART_TYPE == HW_UARTDM_LT) ||\
    (SIORS_THIRD_UART_TYPE  == HW_UARTDM_LT ))       //{

#include "err.h"                   /* Error services definitions           */
#include "tramp.h"                 /* Trampoline typedefs and prototypes   */
#include "clk.h"                   /* Clock services definitions           */
#include "clkregim.h"              /* Clock Regime Definitions             */
#include "hw.h"                    /* Hardware Utilities                   */
#include "assert.h"                /* ASSERT macro unitilities             */
#include "siorsreg.h"              /* SIO RS232 UART Register Definitions  */
#include "siors232_dm_lite.h"      /* SIO RS232 Device Layer Header File   */
#include "siors232_trace.h"        /* For the debug trace feature          */
#include "uart_compat.h"           /* Compatability layer                  */


/*===========================================================================

                FEATURE FLAG RESOLUTION - Keep the mess here

===========================================================================*/
#ifndef FEATURE_UART_TCXO_CLK_FREQ
#if defined(FEATURE_JCDMA_DS) || defined(FEATURE_JCDMA_SIO)
#define SIORS_HAS_BAUD_76800
#define SIORS_HAS_BAUD_14400
#endif
#endif  /* TCXO CLK FREQUENCY */

#define SIORS_HAS_BAUD_230400
#define SIORS_HAS_BAUD_460800

#if defined(FEATURE_SIO_PNP)
#error code not present
#endif

/* No clock regime on RUMI */
#ifdef T_RUMI
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

/* Default FIFO size over-ride */
#ifndef SIORS_FIRST_UART_FIFO_SIZE
#define SIORS_FIRST_UART_FIFO_SIZE   SIORS_EXT_FIFO_SIZE
#endif
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================
 General constant and variable definitions.
===========================================================================*/
/* Setting for TX FIFO interrupt.                                          */
#define SIO_FLOW_TXLEV_EMPTY         0

/* RXLEV for data transfers at different baud rates                        */
#define SIO_LOW_RATE_FLOW_RXLEV      31         /* Less than 9600          */
#define SIO_9600_FLOW_RXLEV          30         /* Also used for autobaud  */
#define SIO_14400_FLOW_RXLEV         60
#define SIO_19200_FLOW_RXLEV         56
#define SIO_38400_FLOW_RXLEV         49
#define SIO_57600_FLOW_RXLEV         46
#define SIO_76800_FLOW_RXLEV         40
#define SIO_115200_FLOW_RXLEV        38
#define SIO_230400_FLOW_RXLEV        38  
#define SIO_460800_FLOW_RXLEV        38

/* Set the RXLEV base on the FIFO size */
#if (SIORS_FIRST_UART_FIFO_SIZE == 512)
  #define SIO_3200000_FLOW_RXLEV       60
#else
  #define SIO_3200000_FLOW_RXLEV       384
#endif

/* Size of the FIFO. */
#define SIORS_DEFAULT_FIFO_SIZE      512  
#define SIORS_EXT_FIFO_SIZE          4096

/* RXLEV for data transfers at rates higher than 38400 (e.g. 115.2Kbps) for
   packet calls when using hardware flow control.                          */
#define SIO_PKT_HIGH_RATE_FLOW_RXLEV 32  

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
siors_port_type    siors_dml_ports[UART_NUM_DEVICES];

/* UARTDM constants */
#define  SIORS_UARTDM_DMRX_DEFAULT_VALUE            65535
#define  SIORS_UARTDM_DEFAULT_DMEN                  0x00

/* This value is depending on the device UART talks to, assume BT device delay */
#define  SIORS_UARTDM_INFLOW_STOP_DELAY             200

#ifdef T_QSC1100 //{

  #include "msm_drv.h"

  /* Value returned by VERSION_GET_VERSION_ID macro. */
  #define  PEDRO_1_0_VERSION                          0

#endif //}

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*---------------------------------------------------------------------------
                      Function prototypes 
---------------------------------------------------------------------------*/
static void siors_dml_set_rxlev( siors_port_type * );
static void siors_dml_reset_receiver( void * );
static void siors_dml_reset_transmitter( void * );
static void siors_dml_enable_receiver( void * );
static void siors_dml_set_bit_rate (void *, sio_bitrate_type);
static void siors_dml_enable_transmitter ( void * );
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/


/*===========================================================================

                       FUNCTIONs, ISRs and MACROs

===========================================================================*/
/*===========================================================================

FUNCTION SIORS_DML_ENABLE_HARDWARE_FCTL                           EXPORTED

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

static void siors_dml_enable_tx_hardware_fctl ( void *pHead )
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

FUNCTION SIORS_DML_DISABLE_HARDWARE_FCTL                          EXPORTED

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

static void siors_dml_disable_tx_hardware_fctl ( void *pHead )
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

FUNCTION SIORS_DML_ENABLE_TRANSMIT_INTERRUPT                         EXPORTED

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
static void siors_dml_enable_transmit_interrupt
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

  MSMU_OUT( hwreg->msmu_tfwr, SIO_FLOW_TXLEV_EMPTY);

  SIORS_TRACE_CODE(201, rs_port->device_id);

  rs_port->gv.sio_imr_shadow |= MSMU_IMR_TX_READY;
  MSMU_OUT(hwreg->msmu_imr, rs_port->gv.sio_imr_shadow);
} 


/*===========================================================================

FUNCTION SIORS_DML_DISABLE_TRANSMIT_INTERRUPT                    EXPORTED

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
static void siors_dml_disable_transmit_interrupt ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  SIORS_TRACE_CODE(200, rs_port->device_id);

  rs_port->gv.sio_imr_shadow &= ~MSMU_IMR_TXLEV;
  rs_port->gv.sio_imr_shadow &= ~MSMU_IMR_TX_READY;
  MSMU_OUT(rs_port->hw.msmu_imr, rs_port->gv.sio_imr_shadow);
}


/*===========================================================================

FUNCTION SIORS_DML_ENABLE_AUTO_HRDWR_FCTL                         EXPORTED

DESCRIPTION
  This procedure enables automatic RX hardware flow control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dml_enable_auto_hrdwr_fctl ( void *pHead )
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

FUNCTION SIORS_DML_DISABLE_AUTO_HRDWR_FCTL                        EXPORTED

DESCRIPTION
  This procedure disables automatic RX hardware flow control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_dml_disable_auto_hrdwr_fctl ( void *pHead )
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

FUNCTION SIORS_DML_ENABLE_RX_INBOUND_FCTL                      EXPORTED

DESCRIPTION
  Given RS232 enable in-bound flow control (HW).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dml_enable_rx_inbound_fctl ( void *pHead )
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

FUNCTION SIORS_DML_DISABLE_RX_INBOUND_FCTL                      EXPORTED

DESCRIPTION
  Given RS232, disable in-bound flow using appropriate flow contol (HW)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dml_disable_rx_inbound_fctl ( void *pHead )
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

FUNCTION SIORS_DML_DETECT_RX_ACTIVITY                        EXPORTED

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
  This function can be called when the port is closed
  
===========================================================================*/
static void siors_dml_detect_rx_activity( void *pHead )
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

FUNCTION SIORS_DML_DP_RX_COMMON_ISR                              INTERNAL

DESCRIPTION
  This ISR is triggered when RX activity is detected

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_dml_dp_rx_common_isr ( siors_port_type *rs_port )
{
  /* Make sure we actually want to get this ISR */
  ASSERT(rs_port->gv.is_wakeup_isr_registered);
  tramp_set_isr( rs_port->gv.wakeup_isr_type, NULL);
  rs_port->gv.is_wakeup_isr_registered = FALSE;
  /* Call the upper layer */
  rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_RX_ACTIVITY);
} 



/*===========================================================================

FUNCTION SIORS_DML_DP_RX_ISR1                                   INTERNAL

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
static void siors_dml_dp_rx_isr1()
{
  siors_dml_dp_rx_common_isr(&(siors_dml_ports[UART_DEVICE_1]));
} 
#endif


/*===========================================================================

FUNCTION SIORS_DML_DP_RX_ISR2                                   INTERNAL

DESCRIPTION
  This ISR is triggered when RX activity is detected

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_SECOND_UART
static void siors_dml_dp_rx_isr2()
{
  siors_dml_dp_rx_common_isr(&(siors_dml_ports[UART_DEVICE_2]));
} 
#endif

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
void siors_dml_set_uart_clock_freq
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
         clk_regime_sel_uart1dm_clk(CLKRGM_UARTDM_SPEED_14_7456_MHZ);
         break;
       case (dword)3200000:
         clk_regime_sel_uart1dm_clk(CLKRGM_UARTDM_SPEED_51_2_MHZ);
         break;
       case (dword)3686400:
         clk_regime_sel_uart1dm_clk(CLKRGM_UARTDM_SPEED_58_9824_MHZ);
         break;
       case (dword)4000000:
         clk_regime_sel_uart1dm_clk(CLKRGM_UARTDM_SPEED_64_MHZ);
         break;
       default:
         ERR_FATAL("Unsupported Baudrate: UART-DM does not supports requested baud rate", 0, 0, 0);
         break;
     }
  } /* if (freq > 460800) */
  else 
  {
     clk_regime_sel_uart1dm_clk(CLKRGM_UARTDM_SPEED_7_3728_MHZ);
  }
}


/*===========================================================================

FUNCTION SIORS_DML_SET_BIT_RATE                              EXPORTED

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

static void siors_dml_set_bit_rate
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
  siors_dml_set_uart_clock_freq(rs_port, (dword)115200);
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
      flow_rxstale                     = SIO_FLOW_RXSTALE_LOW_RATE;
      break;

    case SIO_BITRATE_600:
      /*---------------------------------------------------------------------
       Set the UART to 600 baud.
      ---------------------------------------------------------------------*/

      temp_csr_value                   = MSMU_CSR_600_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_LOW_RATE;
      break;

    case SIO_BITRATE_1200:
      /*---------------------------------------------------------------------
       Set the UART to 1200 baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_1200_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_1200;
      break;

    case SIO_BITRATE_2400:
      /*---------------------------------------------------------------------
       Set the UART to 2400 baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_2400_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_2400;
      break;

    case SIO_BITRATE_4800:
      /*---------------------------------------------------------------------
       Set the UART to 4800 baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_4800_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_4800;
      break;

    case SIO_BITRATE_9600:
      /*---------------------------------------------------------------------
       Set the UART to 9600 baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_9600_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_9600;
      break;

#ifdef SIORS_HAS_BAUD_14400 
    case SIO_BITRATE_14400:
      /*---------------------------------------------------------------------
       Set the UART to 14.4K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_144K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_14400;
      break;
#endif 

    case SIO_BITRATE_19200:
      /*---------------------------------------------------------------------
       Set the UART to 19.2K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_192K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_19200;
      break;

    case SIO_BITRATE_38400:
      /*---------------------------------------------------------------------
       Set the UART to 38.4K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_384K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      break;

    case SIO_BITRATE_57600:
      /*---------------------------------------------------------------------
       Set the UART to 57.6K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_576K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      break;

#ifdef SIORS_HAS_BAUD_76800 
    case SIO_BITRATE_76800:
      /*---------------------------------------------------------------------
       Set the UART to 76.8K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_768K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      siors_dml_set_uart_clock_freq(rs_port, (dword)76800);
      break;
#endif 

    case SIO_BITRATE_115200:
      /*---------------------------------------------------------------------
       Set the UART to 115.2K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_1152K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      break;

    case SIO_BITRATE_230400:
      if (!rs_port->is_ext_speed)
        ERR_FATAL("Baudrate 230400 not available on this UART", 0, 0, 0);

      /*---------------------------------------------------------------------
       Set the UART to 230.4K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_2304K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      break;

    case SIO_BITRATE_460800:
      if (!rs_port->is_ext_speed)
        ERR_FATAL("Baudrate 460800 not available on this UART", 0, 0, 0);

      /*---------------------------------------------------------------------
       Set the UART to 460.8K baud.
      ---------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_4608K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      break;

      /*-------------------------------------------------------------------*/
    case SIO_BITRATE_3200000:
      /*-------------------------------------------------------------------*/
      temp_csr_value                   = MSMU_CSR_3200K_BPS;
      flow_rxstale                     = SIO_FLOW_RXSTALE_HIGH_BITRATE;
      siors_dml_set_uart_clock_freq(rs_port, (dword)3200000);
      break;

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

  siors_dml_reset_receiver( rs_port );
  /* If tx break is in use, do not reset the transmitter, 
   * this is done so TX break condition is not destroyed */
  if(!rs_port->gv.tx_break_state)
  {
    siors_dml_reset_transmitter( rs_port );
  }

  /*--------------------------------------------------------------
   Update RXLEV after sio_current_rx_bitrate[] is valid.
  --------------------------------------------------------------*/
  siors_dml_set_rxlev(rs_port);
  rs_port->gv.sio_current_tx_bitrate        = req_bitrate;

  /*-------------------------------------------------------------------------
   Calculate the number of bytes that can be written to TX FIFO when TXLEV
   interrupt occurs.
  -------------------------------------------------------------------------*/
  rs_port->gv.sio_num_avail_tx_fifo_bytes = rs_port->gv.tx_fifo_size;
                  

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

  siors_dml_reset_receiver( rs_port );
  siors_dml_enable_receiver( rs_port );
  siors_dml_enable_transmitter( rs_port );
}



/*===========================================================================

FUNCTION SIORS_DML_RESET_TRANSMITTER                             EXPORTED

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

static void siors_dml_reset_transmitter ( void *pHead )
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

FUNCTION SIORS_DML_ENABLE_TRANSMITTER                            EXPORTED

DESCRIPTION
  Enable transmitter but not enable transmit interrupt.

DEPENDENCIES
  Should be called after sio_rs232_chng_bitrate.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dml_enable_transmitter ( void *pHead )
{
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!rs_port->is_clock_enabled){
    return;
  }

  rs_port->gv.is_tx_first_started             = TRUE;

  MSMU_OUT( rs_port->hw.msmu_tfwr, SIO_FLOW_TXLEV_EMPTY);
  MSMU_OUT( rs_port->hw.msmu_cr,   MSMU_CR_ENA_TX);
} 


/*===========================================================================

FUNCTION SIORS_DML_RESET_RECEIVER                            EXPORTED

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
static void siors_dml_reset_receiver ( void *pHead )
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

FUNCTION SIORS_DML_DISABLE_RECEIVER                          EXPORTED

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
static void siors_dml_disable_receiver ( void    *pHead )
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

FUNCTION SIORS_DML_ENABLE_RECEIVER                           EXPORTED

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
static void siors_dml_enable_receiver ( void *pHead )
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

  rs_port->gv.sio_imr_shadow |= (MSMU_IMR_RXSTALE | MSMU_IMR_RXLEV);
  /*------------------------------------------------------------------------
   First we reset the receiver . . . 
  ------------------------------------------------------------------------*/
  siors_dml_reset_receiver( rs_port );

  MSMU_OUT( rs_port->hw.msmu_cr, MSMU_CR_ENA_RX);
  MSMU_OUT( rs_port->hw.msmu_rfwr, rs_port->gv.sio_uart_flow_rxlev);
  MSMU_OUT( rs_port->hw.msmu_ipr, rs_port->gv.sio_ipr_shadow);
  MSMU_OUT( rs_port->hw.msmu_imr, rs_port->gv.sio_imr_shadow);

  /* Always enable the stale event after a reset */
  MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_STALE);
  MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_ENA_STALE_EVT);
  MSMUW_OUT( rs_port->hw.msmu_dmrx, SIORS_UARTDM_DMRX_DEFAULT_VALUE );

  /* Reset stale state flag */ 
  rs_port->gv.is_ok_to_start_rx = TRUE;

  INTFREE_SAV( i_save);
}



/*===========================================================================

FUNCTION SIORS_DML_CHECK_RX_ERROR                            INTERNAL

DESCRIPTION
  Check if there is error condition in ISR, if so, take proper action

DEPENDENCIES
  None

RETURN VALUE
  TRUE if error has occurred and RX/TX has been resetted.

SIDE EFFECTS
  None

===========================================================================*/
static boolean siors_dml_check_rx_error ( siors_port_type *rs_port )
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
      siors_dml_reset_receiver( rs_port );
      siors_dml_enable_receiver( rs_port );
      return TRUE;
    }
  } /* Parity/Framing Error, Overrun, or RX Full Error */
  return FALSE;
}



/*===========================================================================

FUNCTION SIORS_DML_COMMON_ISR                                INTERNAL

DESCRIPTION
  This ISR services all SIO ISRs (TX and RX).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void siors_dml_common_isr ( siors_port_type *rs_port )
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

  /*-------------------------------------------------------------------------
   Read the Masked Interrupt Status register (MISR), mask off undesired bits
   then process all interrupts conditions until MISR indicates no more.
  -------------------------------------------------------------------------*/
  misr_val = (inpw(hwreg->msmu_misr) & rs_port->gv.sio_imr_shadow);

  /* Process RX interrupt */

  if(((misr_val & MSMU_ISR_RXSTALE) !=0) && ((misr_val & MSMU_ISR_RXLEV) !=0))
  {
    if(!siors_dml_check_rx_error(rs_port))
    {
      rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_RXSTALE_INTR);
    }
  }
  else if((misr_val & MSMU_ISR_RXLEV) !=0)
  {
    rs_port->gv.is_watermark_event = TRUE;
    if(!siors_dml_check_rx_error(rs_port))
    {
      rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_RXLEV_INTR);
    }
  }
  else if((misr_val & MSMU_ISR_RXSTALE) !=0)
  {
    if(!siors_dml_check_rx_error(rs_port))
    {
      rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_RXSTALE_INTR);
    }
  }

  /* Process TX interrupt */
  else if(( misr_val & (MSMU_ISR_TXLEV)) != 0)
  {
    rs_port->gv.mdd_callback_func_ptr (rs_port->gv.mdd_head_ptr, SIORS_TX_INTR);
  }
  else if(( misr_val & (MSMU_ISR_TX_READY)) != 0)
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

FUNCTION SIORS_DML_ISR1                                      INTERNAL

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
static void siors_dml_isr1()
{
  siors_dml_common_isr(&(siors_dml_ports[UART_DEVICE_1]));
} 
#endif

/*===========================================================================

FUNCTION SIORS_DML_ISR2                                      INTERNAL

DESCRIPTION
  This ISR services all SIO ISRs (TX and RX).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_SECOND_UART
static void siors_dml_isr2()
{
  siors_dml_common_isr(&(siors_dml_ports[UART_DEVICE_2]));
} 
#endif



/*===========================================================================

FUNCTION SIORS_DML_INITIALIZE_COMMON                         INTERNAL

DESCRIPTION
  Initialize variables/hardware that is common to all UARTs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_dml_initialize_common ( siors_port_type *rs_port )
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
    rs_port->gv.has_fifo_hw_bug = FALSE;

#ifdef T_QSC1100

    //  This hw bug is reported only on the 1100 and only on Pedro 1.0.

    if( PEDRO_1_0_VERSION == VERSION_GET_VERSION_ID())
    {
        rs_port->gv.has_fifo_hw_bug = TRUE;
    }
#endif
}

/*===========================================================================

FUNCTION SIORS_DML_UART1DM_INIT                             INTERNAL

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

#ifdef FEATURE_FIRST_UART //{

static void siors_dml_uart1dm_init( siors_port_type *rs_port )
{
    siors_hw_reg_type *hwreg;               /* Hardware register             */
    hwreg = &(rs_port->hw);

    /* ---- BASIC access structure initialize ---- */
    hwreg->msmu_mr1  = (dword)HWIO_ADDR(UART1DM_MR1);
    hwreg->msmu_mr2  = (dword)HWIO_ADDR(UART1DM_MR2);
    hwreg->msmu_irda = (dword)HWIO_ADDR(UART1DM_IRDA);
    hwreg->msmu_imr  = (dword)HWIO_ADDR(UART1DM_IMR);
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
      
    rs_port->is_ext_fifo = TRUE;
    rs_port->is_ext_speed = TRUE;

    /* Limitation of the FIFO size: it must be a multiple of 16 */
    rs_port->gv.total_fifo_size = SIORS_FIRST_UART_FIFO_SIZE;
    rs_port->gv.rx_fifo_size = rs_port->gv.total_fifo_size / 2;   
    rs_port->gv.tx_fifo_size = rs_port->gv.total_fifo_size / 2;   

  /*-----------------------------------------------------------------------*/
    siors_dml_initialize_common(rs_port);  

  /*-------------------------------------------------------------------------
   Explicitly initialize array of function pointer to be used for the 
   purposes of calling call-back functions which signal end of transmission.
  -------------------------------------------------------------------------*/
    rs_port->gv.siors_isr_func_ptr       = siors_dml_isr1;
    rs_port->gv.isr_type                 = UART1DM_LT_IRQ;
    rs_port->gv.is_isr_registered        = FALSE;
    rs_port->gv.clk_source               = CLKRGM_UART1DM_CLK;
    rs_port->gv.wakeup_isr_type          = UART1DM_LT_RX_IRQ;
    rs_port->gv.wakeup_isr_func_ptr      = siors_dml_dp_rx_isr1;
    rs_port->gv.is_wakeup_isr_registered = FALSE;
}
#endif //}

/*===========================================================================

FUNCTION SIORS_DML_UART2DM_INIT                             INTERNAL

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

#ifdef FEATURE_SECOND_UART //{

static void siors_dml_uart2dm_init( siors_port_type *rs_port )
{
    siors_hw_reg_type *hwreg;               /* Hardware register             */
    hwreg = &(rs_port->hw);

    /* ---- BASIC access structure initialize ---- */
    hwreg->msmu_mr1  = (dword)HWIO_ADDR(UART2DM_MR1);
    hwreg->msmu_mr2  = (dword)HWIO_ADDR(UART2DM_MR2);
    hwreg->msmu_irda = (dword)HWIO_ADDR(UART2DM_IRDA);
    hwreg->msmu_imr  = (dword)HWIO_ADDR(UART2DM_IMR);
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
      
    rs_port->is_ext_fifo  = TRUE;
    rs_port->is_ext_speed = TRUE;

    /* Limitation of the FIFO size: it must be a multiple of 16 */
    rs_port->gv.total_fifo_size = SIORS_SECOND_UART_FIFO_SIZE;
    rs_port->gv.rx_fifo_size    = rs_port->gv.total_fifo_size / 2;   
    rs_port->gv.tx_fifo_size    = rs_port->gv.total_fifo_size / 2;   

  /*-----------------------------------------------------------------------*/
    siors_dml_initialize_common(rs_port);  

  /*-------------------------------------------------------------------------
   Explicitly initialize array of function pointer to be used for the 
   purposes of calling call-back functions which signal end of transmission.
  -------------------------------------------------------------------------*/
    rs_port->gv.siors_isr_func_ptr       = siors_dml_isr2;
    rs_port->gv.isr_type                 = UART2DM_LT_IRQ;
    rs_port->gv.is_isr_registered        = FALSE;
    rs_port->gv.clk_source               = CLKRGM_UART2DM_CLK;
    rs_port->gv.wakeup_isr_type          = UART2DM_LT_RX_IRQ;
    rs_port->gv.wakeup_isr_func_ptr      = siors_dml_dp_rx_isr2;
    rs_port->gv.is_wakeup_isr_registered = FALSE;
}
#endif //}


/*===========================================================================

FUNCTION SIORS_DML_OPEN                                          EXPORTED

DESCRIPTION
  This function opens a port so it can be used
    
DEPENDENCIES
  None
  
RETURN VALUE
  Returns non-zero if the port failed to open.
  
SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_dml_open ( void *pHead, 
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

  /* Turning on the clock when the port is opened */
  CLK_REGIME_MSM_ENABLE(rs_port->gv.clk_source);    

  rs_port->is_clock_enabled = TRUE;

  /* Disable IRDA tranceiver.  No QC product currently uses IRDA. */
  MSMU_OUT( hwreg->msmu_irda, MSMU_IRDA_DISABLE);

  /* Protect against consective commands */
  MSMUW_OUT( hwreg->msmu_cr, MSMU_CR_ENA_CR_PROT);

  rs_port->gv.sio_imr_shadow = 0;
  MSMU_OUT( hwreg->msmu_imr, rs_port->gv.sio_imr_shadow);
  MSMUW_OUT( hwreg->msmu_cr,  MSMU_CR_RESET_ERR);

  siors_dml_reset_receiver( rs_port );
  /* During non-reset device open, and tx break is in use, do not reset 
   * transmitter, this is done so TX break condition is not destroyed */
  if(!(rs_port->gv.tx_break_state && quick))
  {
    siors_dml_reset_transmitter( rs_port );
  }


  /* STALE event will be on constantly unless DMRX is set to >= 16 */
  MSMUW_OUT( rs_port->hw.msmu_dmrx, SIORS_UARTDM_DMRX_DEFAULT_VALUE );
  
  /* Program for eight bits per character, 1 stop bit and block check error
   * mode */
  MSMU_OUT(hwreg->msmu_mr2, (MSMU_MR2_8BPC | MSMU_MR2_1SB | MSMU_MR2_ERRMODE));

  /* Now set Automatic RFR in UART (for outbound flow monitoring). */
  rs_port->gv.sio_mr1_shadow = MSMU_MR1_CTSC;
  MSMU_OUT(hwreg->msmu_mr1, rs_port->gv.sio_mr1_shadow);

  /*  When port is opened, register the ISR  */
  tramp_set_isr( rs_port->gv.isr_type , rs_port->gv.siors_isr_func_ptr);
  rs_port->gv.is_isr_registered = TRUE;

  /*  UARTDM specific setup */
  MSMUW_OUT(hwreg->msmu_badr, rs_port->gv.tx_fifo_size / 4);
  MSMUW_OUT(hwreg->msmu_dmen, SIORS_UARTDM_DEFAULT_DMEN);

  MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_ERR);

  rs_port->gv.packet_byte_cnt = 0;
    
  siors_dml_set_bit_rate(rs_port,
                     rs_port->gv.sio_current_rx_bitrate);
}


/*===========================================================================

FUNCTION SIORS_DML_CLOSE                                     EXPORTED 

DESCRIPTION
  This function close a port in use
    
DEPENDENCIES
  None
  
RETURN VALUE
  None
   
SIDE EFFECTS
  None
  
===========================================================================*/

static void siors_dml_close ( void *pHead )
{
  siors_port_type  *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Make sure we don't close the port, if the port is not open */
  if(rs_port->gv.is_isr_registered == FALSE)
  {
    return;
  }

  tramp_set_isr( rs_port->gv.isr_type , NULL);
  rs_port->gv.is_isr_registered = FALSE;

  siors_dml_reset_receiver( rs_port );
  /* TX use to be resetted upon close, but it is not done anymore because when
   * TX is resetted, the break state setting is destroyed */

  rs_port->is_clock_enabled = FALSE;
  /* Turning off the clock as we are done with the port */
  CLK_REGIME_MSM_DISABLE(rs_port->gv.clk_source);    
}


/*===========================================================================

FUNCTION SIORS_DML_TRANSMIT                                  EXPORTED

DESCRIPTION
  This function transmits over MSM's RS-232 UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dml_transmit
(
  void               *pHead,
  byte               *tx_ptr,             /* Packet for transmission       */
  int32              *byte_count          /* Number of character to sent   */
)
{
  int32 i;                        /* Loop counter                           */
  int32 bytes_to_tx;              /* Temporary for storing the number of
                                     bytes which need to be transmitted.    */
  int32 xfer_cnt;                 /* Transfer counter                       */
  siors_hw_reg_type *hwreg;
  siors_port_type *rs_port;
  uint32 *data_pt;
  uint32 assemble;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  rs_port = pHead;
  hwreg = &(rs_port->hw);

  bytes_to_tx = *byte_count;

  if (!rs_port->is_clock_enabled){
    *byte_count = 0;
    return;
  }

  SIORS_TRACE_CODE(130, rs_port->device_id);

  if(rs_port->gv.tx_break_state)
  {
    ERR( "Transmitting while in BREAK",0,0,0);
  }

  /* If TX is not ready to take more data, return right away */
  /* Skip the test when TX first started, as interrupt will not be on */
  if ( !rs_port->gv.is_tx_first_started)
  {
    if ( (inp(hwreg->msmu_isr) & MSMU_ISR_TX_READY) == 0 )
    {
      *byte_count = 0;
      return;
    }
  }
  else
  {
    rs_port->gv.is_tx_first_started = FALSE;
  }

  /* If there are too many bytes to sent, limit number of transfer */
  if(bytes_to_tx > rs_port->gv.sio_num_avail_tx_fifo_bytes)
  {
    bytes_to_tx = rs_port->gv.sio_num_avail_tx_fifo_bytes;
  }

  /* 32 bit transfer */
  xfer_cnt = bytes_to_tx / 4;
  data_pt = (uint32 *)tx_ptr;

  /* Is there any leftover bytes? If so, send them 
   * This will read more bytes then the size of the tx_ptr, but extra junk is fine */
  if((bytes_to_tx % 4) != 0)
  {
      xfer_cnt++;
  }
  
  /* Write number of character to transmit, can fill up to the size of the FIFO */
  MSMUDW_OUT( rs_port->hw.msmu_ncft, bytes_to_tx);
  MSMU_OUT(hwreg->msmu_cr, MSMU_CR_RESET_TX_RDY);

  if(((uint32)tx_ptr & 0x03) == 0)
  {
    for( i=0 ; i<xfer_cnt ; i++)
    {
      MSMUDW_OUT( rs_port->hw.msmu_tf, *data_pt++);
    }
  }
  else
  {
    MSG_ERROR( "Non-aligned data, doing byte by byte transfer",0,0,0);
    /* Data not aligned, manually process the data */
    for( i=0 ; i<xfer_cnt ; i++)
    {
      assemble  = (*tx_ptr++) & 0x000000ff;
      assemble |= (*tx_ptr++) << 8;
      assemble |= (*tx_ptr++) << 16;
      assemble |= ((*tx_ptr++) << 24);
      MSMUDW_OUT( rs_port->hw.msmu_tf, assemble);
    }
  }
  
  /* This is the number of bytes we actually sent */
  *byte_count = bytes_to_tx;
}


/*===========================================================================

FUNCTION SIORS_DML_RECEIVE                                   EXPORTED

DESCRIPTION
  This function receive data from MSM's RS-232 UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void siors_dml_receive
(
  void               *pHead,
  byte               *rx_ptr,             /* Buffer to store incoming data */
  uint32             *byte_count          /* Number of character to get    */
)
{
  uint32 i;                       /* Loop counter                           */
  uint32 bytes_to_rx;             /* Temporary for storing the number of
                                     bytes which need to be received   .    */
  uint32 assemble_buf;
  uint32 snap_value;
  byte  *current_rx_ptr;
  siors_port_type *rs_port = pHead;
  uint32 *dword_rx_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  bytes_to_rx = *byte_count;

  if (!rs_port->is_clock_enabled){
    *byte_count = 0;
    return;
  }

  SIORS_TRACE_CODE(300, rs_port->device_id);

  /* Check if the last transfer completed the packet, if so, restart transfer */
  if(!(rs_port->gv.is_ok_to_start_rx || rs_port->gv.last_snap_value != 0))
  {
    if(rs_port->gv.is_watermark_event)
    {
      rs_port->gv.is_watermark_event = FALSE;
    }
    else
    {
      rs_port->gv.is_ok_to_start_rx = TRUE;
      MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_STALE);
      MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_ENA_STALE_EVT);
      MSMUW_OUT( rs_port->hw.msmu_dmrx, SIORS_UARTDM_DMRX_DEFAULT_VALUE );
      *byte_count = 0;
      rs_port->gv.packet_byte_cnt = 0;
    }
    return;
  }

  if(rs_port->gv.has_fifo_hw_bug)
  {
    /* Workaround for data incoming while reading from RX FIFO bug */
    /* Disable incoming flow control, wait for a while */
    siors_dml_disable_rx_inbound_fctl(rs_port);

    {
      int i;
      for(i=0;i<SIORS_UARTDM_INFLOW_STOP_DELAY;i++)
      {
        MSMUW_OUT( rs_port->hw.msmu_cr, 0 );
      }
    }
  }

  /* If a watermark event has occurred, read size of watermark bytes out */
  if (rs_port->gv.is_watermark_event && rs_port->gv.is_ok_to_start_rx)
  {
    snap_value = rs_port->gv.sio_uart_flow_rxlev * 4;
    rs_port->gv.packet_byte_cnt += snap_value;
  }
  else if(rs_port->gv.is_ok_to_start_rx)
  {
    SIORS_TRACE_CODE(301, rs_port->device_id);
    snap_value = inpw ( rs_port->hw.msmu_snap );
    /* Make sure the hardware is still alive */
    ASSERT(snap_value != 0);
    snap_value -= rs_port->gv.packet_byte_cnt;
  }
  else
  {
    snap_value = rs_port->gv.last_snap_value;
  }

  /* Check to see if there is enough room to hold all the data */ 
  if(snap_value >= bytes_to_rx)
  {
    /* Not enough, do not start another transfer next time */
    rs_port->gv.is_ok_to_start_rx = FALSE;
  }
  else
  {
    bytes_to_rx = snap_value;
    rs_port->gv.is_ok_to_start_rx = TRUE;
  }
  
  /* Check for buffer alignment, 
   * if the buffer is aligned, optimized the transfer */
  current_rx_ptr = rx_ptr;
  if(((uint32)current_rx_ptr & 0x0003) != 0) 
  {
     for(i=0;i<bytes_to_rx;)
     {
        if ((inp(rs_port->hw.msmu_sr) & MSMU_SR_RXRDY) != 0)
        {
            assemble_buf = inpdw( rs_port->hw.msmu_rf);
            *current_rx_ptr++ = (byte)((assemble_buf) & 0xff);
            *current_rx_ptr++ = (byte)((assemble_buf >> 8) & 0xff);
            *current_rx_ptr++ = (byte)((assemble_buf >> 16) & 0xff);
            *current_rx_ptr++ = (byte)((assemble_buf >> 24) & 0xff);
            i += 4;
        }
        else
        {
           i = bytes_to_rx;
        }
     }
  }
  else
  {
     dword_rx_ptr = (uint32 *)current_rx_ptr;
     for(i=0;i<bytes_to_rx;)
     {
         if ((inp(rs_port->hw.msmu_sr) & MSMU_SR_RXRDY) != 0)
         {
            *dword_rx_ptr++ = inpdw( rs_port->hw.msmu_rf);
            i += 4;
         }
         else
         {
            i = bytes_to_rx;
         }
     }
  }
  
  rs_port->gv.last_snap_value = snap_value - i;
  *byte_count = bytes_to_rx;

  if(rs_port->gv.is_ok_to_start_rx && rs_port->gv.is_watermark_event)
  {
    rs_port->gv.is_watermark_event = FALSE;
  }
  else if(rs_port->gv.is_ok_to_start_rx && !rs_port->gv.is_watermark_event)
  {
    MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_STALE);
    MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_ENA_STALE_EVT);
    MSMUW_OUT( rs_port->hw.msmu_dmrx, SIORS_UARTDM_DMRX_DEFAULT_VALUE );
    rs_port->gv.packet_byte_cnt = 0;
  }

  if(rs_port->gv.has_fifo_hw_bug)
  {
    siors_dml_enable_rx_inbound_fctl(rs_port);
  }
}


/*===========================================================================

FUNCTION SIORS_DML_RECEIVE_TAIL_CHAR                         EXPORTED

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

static void siors_dml_receive_tail_char
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
  uint32 assemble_buf;
  uint32 snap_value;
  uint32 current_byte;
  byte  *current_rx_ptr;
  siors_port_type *rs_port = pHead;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  bytes_to_rx = *byte_count;

  if (!rs_port->is_clock_enabled){
    *byte_count = 0;
    return;
  }

  /* If a watermark event has occurred, issue a stale command */
  if (rs_port->gv.is_watermark_event)
  {
    MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_FORCE_STALE );
    rs_port->gv.is_watermark_event = FALSE;
  }

  /* Restore register value incase have to restart */
  current_byte = rs_port->gv.last_byte_position;
  assemble_buf = rs_port->gv.last_rx_buffer;

  if(rs_port->gv.is_ok_to_start_rx)
  {
    snap_value = inpw ( rs_port->hw.msmu_snap );
    current_byte = 0;
    /* Make sure the hardware is still alive */
    ASSERT(snap_value != 0);
  }
  else
  {
    snap_value = rs_port->gv.last_snap_value;
  }

  /* Check to see if there is enough room to hold all the data */ 
  if(snap_value > bytes_to_rx)
  {
    /* Not enough, do not start another transfer next time */
    rs_port->gv.is_ok_to_start_rx = FALSE;
  }
  else
  {
    bytes_to_rx = snap_value;
    rs_port->gv.is_ok_to_start_rx = TRUE;
  }
  
  current_rx_ptr = rx_ptr;
  for(i=0;i<bytes_to_rx;)
  {
    if(current_byte == 0)
    {
      if ((inp(rs_port->hw.msmu_sr) & MSMU_SR_RXRDY) != 0)
      {
         assemble_buf = inpdw( rs_port->hw.msmu_rf);
      }
      else
      {
         i = bytes_to_rx;
      }
    }

    switch(current_byte)
    {
      case 0:
        *current_rx_ptr = (byte)((assemble_buf) & 0xff);
        break;
      case 1:
        *current_rx_ptr = (byte)((assemble_buf >> 8) & 0xff);
        break;
      case 2:
        *current_rx_ptr = (byte)((assemble_buf >> 16) & 0xff);
        break;
      case 3:
        *current_rx_ptr = (byte)((assemble_buf >> 24) & 0xff);
        break;
      default:
        current_byte = 0;   
    }

    current_byte++;
    i++;
    current_byte &= 0x03;
    /* If tail character is found, it is a early termination, not to restart next time 
     * However, not true if the tail character is the last character */
    if((*current_rx_ptr == tail_char) && (i < bytes_to_rx))
    {
        rs_port->gv.is_ok_to_start_rx = FALSE;
        break;
    }
    current_rx_ptr++;
  }
  
  /* Make sure we don't have an empty packet */
  ASSERT(i != 0);
  
  /* Remember the value incase we have to restart */
  rs_port->gv.last_rx_buffer = assemble_buf;
  rs_port->gv.last_snap_value = snap_value - i;
  rs_port->gv.last_byte_position = current_byte;
  *byte_count = i;

  *more_data = !rs_port->gv.is_ok_to_start_rx;
  
  if(rs_port->gv.is_ok_to_start_rx)
  {
    MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_RESET_STALE);
    MSMUW_OUT( rs_port->hw.msmu_cr, MSMU_CR_ENA_STALE_EVT);
    MSMUW_OUT( rs_port->hw.msmu_dmrx, SIORS_UARTDM_DMRX_DEFAULT_VALUE );
  }
}


/*===========================================================================

FUNCTION SIORS_DML_GET_CURRENT_CTS                               EXPORTED

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
static void siors_dml_get_current_rts
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

FUNCTION SIORS_DML_IS_TX_FIFO_EMPTY                              EXPORTED

DESCRIPTION
  Returns 1 if TX FIFO is empty, otherwise returns 0

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int32 siors_dml_is_tx_fifo_empty ( void *pHead )
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

FUNCTION SIORS_DML_CHECK_RX_CABLE_STATUS                     EXPORTED

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
static boolean siors_dml_check_rx_cable_status
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

FUNCTION SIORS_DML_SET_TX_BREAK_STATE                          EXPORTED

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
static void siors_dml_set_tx_break_state ( void *pHead, boolean break_enabled )
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

FUNCTION SIORS_DML_SET_RXLEV                                 INTERNAL

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
static void siors_dml_set_rxlev ( siors_port_type *rs_port )
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
    case SIO_BITRATE_3200000:           
      rs_port->gv.sio_uart_flow_rxlev = SIO_3200000_FLOW_RXLEV;
      break;

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
  ASSERT(rs_port->gv.sio_uart_flow_rxlev < (rs_port->gv.rx_fifo_size / 4));
  MSMUW_OUT( rs_port->hw.msmu_rfwr, rs_port->gv.sio_uart_flow_rxlev);
}


/*===========================================================================

FUNCTION SIORS_DML_INITIALIZE                            EXTERNALIZED FUNCTION

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
void siors_dml_initialize
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
    rs_port = &(siors_dml_ports[device_id]);

    /* Record the MDD callback function */
    ASSERT(mdd_callback);
    rs_port->gv.mdd_callback_func_ptr = mdd_callback;
    
    /* Sending these back to MDD */
    rs_func = pFuncTab;
    ASSERT(rs_func);
    (*pHead) = rs_port;
    rs_port->gv.mdd_head_ptr = pMddHead;
    
    /* Initialize the API table */
    rs_func->enable_tx_hw_fctl = siors_dml_enable_tx_hardware_fctl;
    rs_func->disable_tx_hw_fctl = siors_dml_disable_tx_hardware_fctl;
    rs_func->enable_rx_hw_fctl = siors_dml_enable_rx_inbound_fctl;
    rs_func->disable_rx_hw_fctl = siors_dml_disable_rx_inbound_fctl;
    rs_func->enable_tx_interrupt = siors_dml_enable_transmit_interrupt;
    rs_func->disable_tx_interrupt = siors_dml_disable_transmit_interrupt;
    rs_func->enable_auto_hw_fctl = siors_dml_enable_auto_hrdwr_fctl;
    rs_func->disable_auto_hw_fctl = siors_dml_disable_auto_hrdwr_fctl;
    rs_func->detect_rx_activity = siors_dml_detect_rx_activity;
    rs_func->set_bit_rate = siors_dml_set_bit_rate;
    rs_func->reset_tx = siors_dml_reset_transmitter;
    rs_func->enable_tx = siors_dml_enable_transmitter;
    rs_func->reset_rx = siors_dml_reset_receiver;
    rs_func->enable_rx = siors_dml_enable_receiver;
    rs_func->disable_rx = siors_dml_disable_receiver;
    rs_func->open = siors_dml_open;
    rs_func->close = siors_dml_close;
    rs_func->transmit = siors_dml_transmit;
    rs_func->receive = siors_dml_receive;
    rs_func->receive_tail_char = siors_dml_receive_tail_char;
    rs_func->transmit_single_char = NULL;
    rs_func->get_current_rts = siors_dml_get_current_rts;
    rs_func->is_tx_fifo_empty = siors_dml_is_tx_fifo_empty;
    rs_func->check_rx_cable_status = siors_dml_check_rx_cable_status;
    rs_func->set_tx_break_state = siors_dml_set_tx_break_state;

    
    rs_port->device_id = device_id;
        
    switch (device_id)
    {
#ifdef FEATURE_FIRST_UART
      case UART_DEVICE_1:
        siors_dml_uart1dm_init( rs_port );
        break;
#endif

#ifdef FEATURE_SECOND_UART
      case UART_DEVICE_2:
        siors_dml_uart2dm_init( rs_port );
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

