#ifndef SIORS232_DM_PD_H
#define SIORS232_DM_PD_H
/*===========================================================================

              S I O   D E V I C E   L A Y E R   H E A D E R 

DESCRIPTION
   This is the header file for the interface layer of SIO.  Therefore, this 
   header file provides the complete interface to SIO services.

Copyright (c) 2007-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors232_dm_pd.h#2 $ $DateTime: 2009/02/11 17:20:36 $ $Author: cmihalik $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/08   rh      Add user configurable FIFO SIZE
06/04/08   rh      Add clock frequency switch support for UART2DM
05/29/08   rh      Add MDM enable/disable support
03/12/08   rh      Add support for UART2DM
11/21/07   rh      Not resetting TX hardware when TX break is activated
10/18/07   rh      Log the number of BREAK event received
09/24/07   rh      Add supporting APIs for TX break event generation
08/10/07   rh      Add RX activity detection
08/09/07   rh      Fix issues that cause RX to fail
07/19/07   rh      Created file.
===========================================================================*/

/*===========================================================================

                        FEATURE FLAGS RESOLUTION 

===========================================================================*/

#ifndef FEATURE_DUAL_CLK_RGM
#define CLKRGM_MSM_CLK_TYPE_P clk_regime_type
#else
#define CLKRGM_MSM_CLK_TYPE_P clkrgm_msm_clk_type
#endif

#ifdef SIORS_USES_SC2X_CLKRGM
#define CLKRGM_UARTDM_SPEED_TYPE   uint32
#else
#define CLKRGM_UARTDM_SPEED_TYPE   clkrgm_uartdm_speed_type
#endif

/*===========================================================================
 
                        Include Files

===========================================================================*/

#include "sio.h"                             /* SIO Server header file     */
#include "clk.h"                             /* Clock services header file */
#include "customer.h"                        /* Customer feature files     */
#include "tramp.h"                 /* Trampoline typedefs and prototypes   */
#include "uart_compat.h"                     /* UART compat file */

/*===========================================================================

                        ENUMERATION TYPE

===========================================================================*/

/*===========================================================================

                         Constant Defines.
                         
===========================================================================*/

/* Size of receive buffer */
#define SIO_DM_RX_BUF_SIZE 1024

/* Number of ping pong receive commands */
#define SIO_DM_NUM_RX_CMDS 2

/* Number of words required for a box command to DM */
#define SIO_DM_BOX_CMD_SIZE 15

/* DM burst size (in bytes) */
#define SIO_DM_BURST_SIZE 16

/*===========================================================================

                        DATA STRUCTURES 

===========================================================================*/

/* Transmit command structure */
typedef struct 
{
  dmov_cmd_box_s_type    *sio_tx_cmd_ptr;
  boolean                in_use;
  void                  *rs_port;               /* Need to hold the siors_port_type */
} sio_tx_cmd_struct;

/* Receive command and buffer information structure */
typedef struct 
{
  byte                   cmd_id;
  byte*                  sio_rx_buffer;
  uint32                 bytes_received;
  void                  *rs_port;
} sio_rx_cmd_struct;

/* Receive command state */
typedef enum
{
  SIO_RX_CMD_INIT,
  SIO_RX_CMD_FLUSHING,
  SIO_RX_CMD_PENDING,
  SIO_RX_CMD_SENT
} sio_rx_cmd_state_type;

/* All required data used by the driver */
typedef struct
{
    /********************** Transmit Variables ************************/
                            /* Transmit Data mover command */
    dmov_xfer_desc_s_type   tx_xfer;
                            /* Buffer to initialize the transmit DM command */
    uint32                  tx_cmd_buf[SIO_DM_BOX_CMD_SIZE];
                            /* Transmit command information */
    sio_tx_cmd_struct       tx_cmd;
                            /* Pointer to buffer used when transmitting a single byte */
    byte*                   tx_buffer;
                            /* True if TX is not in reset */
    boolean                 tx_started;

    /********************** Receive Variables ************************/
                            /* Receive Data mover command */
    dmov_xfer_desc_s_type   rx_xfer;
                            /* Buffer to initialize the receive DM command */
    uint32                  rx_cmd_buf[SIO_DM_BOX_CMD_SIZE];
                            /* Receive command information. Two ping-pong rx commands used */
    sio_rx_cmd_struct       rx_cmd[SIO_DM_NUM_RX_CMDS];
                            /* ID of receive command in progress */
    byte                    current_rx_cmd;
                            /* Stale event flag */
    boolean                 dm_stale_evt;
                            /* State of receive and its command */
    sio_rx_cmd_state_type   rx_cmd_state;
                            /* True if RX is not in reset */
    boolean                 rx_started;

    /********************** DM channel Variables ************************/
    int                     dm_device;
    int                     dm_channel;
} siors_dm_variable_type;


typedef struct      /* A structure to record error count                    */
{
    dword  async_or;          /* Overruns when in Async mode                */
    dword  async_pf_err;      /* Async mode parity/framing error            */
    dword  async_crc_err;     /* Async mode CRC error                       */
    dword  async_max_pkt;     /* Async mode packet too long                 */
    dword  async_rx_ovrflw;   /* too many RX bytes                          */
    dword  async_rx_spur;     /* Async spurious RX interrupt                */
    dword  async_rx_break;    /* Break event count                          */
} uart_errs_type;

typedef struct      /* A structure that contains address of hardware regs  */
{
    dword msmu_mr1;          /* UART mode register 1                         */
    dword msmu_mr2;          /* UART mode register 2                         */
    dword msmu_csr;          /* UART Clock Select register                   */
    dword msmu_cr;           /* UART command register                        */
    dword msmu_imr;          /* UART interrupt mask register                 */
    dword msmu_ipr;          /* UART interrupt programming register          */
    dword msmu_tfwr;         /* UART TX FIFO Watermark register              */
    dword msmu_rfwr;         /* UART RX FIFO Watermark register              */
    dword msmu_hcr;          /* UART RX hunt character register              */
    dword msmu_mvr;          /* UART M register                              */
    dword msmu_nvr;          /* UART N register                              */
    dword msmu_dvr;          /* UART D register                              */
    dword msmu_irda;         /* UART IRDA register                           */
    
    dword msmu_sr;           /* Status Register of UART                      */
    dword msmu_misr;         /* UART masked interrupt status register        */
    dword msmu_isr;          /* UART interrupt status register               */
   
    dword msmu_int_ena;      /* UART interrupt enable                        */
    dword msmu_tf;           /* TX FIFO Register of UART                     */
    dword msmu_rf;           /* RX FIFO Register of UART                     */
  /* --------  UART_DM only registers  ---------------------------------   */
    dword msmu_snap;         /* UART Rx total snap register                  */
    dword msmu_dmrx;         /* UART Dm receive register                     */
    dword msmu_rxfs;         /* UART RX FIFO status register                 */
    dword msmu_ncft;         /* UART Number of char for TX register          */
    dword msmu_phytf;        /* TX FIFO Register of UART physical Address    */
    dword msmu_phyrf;        /* RX FIFO Register of UART physical address    */
    dword msmu_dmen;         /* DM Enable Register                           */
    dword msmu_badr;         /* FIFO Register base address                   */
} siors_hw_reg_type;
  

typedef struct                 /* This structure contain UART variables         */
{
    sio_rx_func_ptr_type    sio_rx_func_ptr;
                               /* Pointer to queue from which SIO will dequeue 
                                * packets to TX.                                */
                               /* The Current RX and TX bit rates              */
    sio_bitrate_type        sio_current_rx_bitrate;
    sio_bitrate_type        sio_current_tx_bitrate;
                               /* Indicates whether serial outbound flow is enabled*/
    boolean                 sio_outbound_flow_disabled;
                               /* In-bound flow control state                     */
    boolean                 sio_inbound_flow_enabled;
    boolean                 inbound_auto_hw_flow_enabled;
                               /* Is the UART in packet optimized mode? */
    boolean                 sio_uart_packet_mode;
                               /* RX flow control set level */
    uint16                  sio_uart_flow_rx_rfr_lev;
                               /* Data mover clock switching state */
    uint32                  dmov_client_enum;
    clk_cb_type             dmov_disable_cb_item;
    boolean                 is_domv_enabled;
    boolean                 is_port_closing;
                               /* Clock regime API used to set the uart clock */
    void                    (*fp_clk_regime_sel_clk) (CLKRGM_UARTDM_SPEED_TYPE);

    /* --------- Control and shadow Variables ---------------------------------*/
    uint16                  sio_imr_shadow;
    byte                    sio_csr_shadow;
    uint16                  sio_ipr_shadow;
    uint16                  sio_mr1_shadow;
    uint16                  sio_mvr_shadow;
    uint16                  sio_nvr_shadow;
    uint16                  sio_dvr_shadow;
                               /* Value that needs to be written to IPR in order 
                                * for RXSTALE interrupt to function properly.  */
    uint16                  sio_ipr_rxstale_on;
                               /* M,N,D register default */
    uint16                  sio_mvr_default;
    uint16                  sio_nvr_default;
    uint16                  sio_dvr_default;
                               /* UART error structure.                        */
    uart_errs_type          sio_errs;
    boolean                 tx_break_state;

    /* --------- FIFO configuration -------------------------------------------*/
    uint16                  total_fifo_size;
    uint16                  rx_fifo_size;
    uint16                  tx_fifo_size;
    /* --------- Call back function pointers ----------------------------------*/
                               /* Pointer to task context block which is used in 
                                * signalling task when DTR changes state.  DTR/ 
                                * pin allows dedication of GPIO_INT pin to the 
                                * purpose.                                     */
    void                    (*siors_dtr_func_ptr)(void);
                               /* Record what type of ISR is used              */
    void                    (*siors_isr_func_ptr)(void);
                               /* Record what type of wakeup ISR is used       */
    void                    (*wakeup_isr_func_ptr)(void);
                               /* Call back function back to the MDD           */
    void                    (*mdd_callback_func_ptr)(void *, siors_mdd_callback_type);
    void                    *mdd_head_ptr;

    /* --------- ISR control variables ----------------------------------------*/
    tramp_isr_type          isr_type;
    tramp_isr_type          wakeup_isr_type;
                               /* If RX isr is registered, RX stream is open   */
    boolean                 is_isr_registered;
                               /* If the wake up event is activated            */
    boolean                 is_wakeup_isr_registered;
    CLKRGM_MSM_CLK_TYPE_P   clk_source;
                               /* Do not enable the TX_ISR if this bit is true */
    boolean                 ignore_tx_isr_enable;
                               
    /* --------- UARTDM RX variables ------------------------------------------*/
                               /* Current RX buffer being processed by RX func */
    uint8                   *current_rx_buf;
                               /* The number of bytes in the buffer to process */
    uint32                  current_rx_bytes;
} siors_variable_type;


typedef struct              /* The port_type structure identify a PORT     */
{
    uart_device_id_type     device_id; /* Device ID                        */
    siors_hw_reg_type       hw;        /* Hardware registers               */
    siors_variable_type     gv;        /* General variables                */
    siors_dm_variable_type  dmv;       /* Data mover variables             */
    boolean                 is_clock_enabled;
} siors_port_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================
 FUNCTION/PROCEDURE prototypes.
===========================================================================*/              

/*===========================================================================

FUNCTION SIORS_DM_INITIALIZE                        EXTERNALIZED FUNCTION

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
    uart_device_id_type  device_id,            /* Device ID            */
    sio_pdd_api_type    *pFuncTab,            /* API entry point structure */
    void                **pHead,              /* Pointer to head structure */
    void                *pMddHead,            /* Pointer to MDD head       */
    void (*mdd_callback) (void *, siors_mdd_callback_type)
                                              /* MDD call back function    */
);

#endif /* SIORS232_DM_PD_H */
