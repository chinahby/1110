#ifndef SIORS232_PD_H
#define SIORS232_PD_H
/*===========================================================================

              S I O   D E V I C E   L A Y E R   H E A D E R 

DESCRIPTION
   This is the header file for the interface layer of SIO.  Therefore, this 
   header file provides the complete interface to SIO services.

   Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors232_pd.h#2 $ $DateTime: 2009/02/11 17:20:36 $ $Author: cmihalik $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/07   rh      Add RX activity detection
07/26/07   rh      Required Koel change merged
05/02/07   rh      Created file.
===========================================================================*/

/*===========================================================================

                        FEATURE FLAGS RESOLUTION 

===========================================================================*/

#ifndef FEATURE_DUAL_CLK_RGM
#define CLKRGM_MSM_CLK_TYPE_P clk_regime_type
#else
#define CLKRGM_MSM_CLK_TYPE_P clkrgm_msm_clk_type
#endif

/*===========================================================================
 
                        Include Files

===========================================================================*/

#include "sio.h"                             /* SIO Server header file     */
#include "clk.h"                             /* Clock services header file */
#include "customer.h"                        /* Customer feature files     */
#include "tramp.h"                 /* Trampoline typedefs and prototypes   */
#include "uart_compat.h"                     /* UART file */

/*===========================================================================

                        ENUMERATION TYPE

===========================================================================*/


/*===========================================================================

                        DATA STRUCTURES 

===========================================================================*/

typedef struct      /* A structure to record error count                    */
{
    dword  async_or;          /* Overruns when in Async mode                */
    dword  async_pf_err;      /* Async mode parity/framing error            */
    dword  async_crc_err;     /* Async mode CRC error                       */
    dword  async_max_pkt;     /* Async mode packet too long                 */
    dword  async_rx_ovrflw;   /* too many RX bytes                          */
    dword  async_rx_spur;     /* Async spurious RX interrupt                */
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
    dword msmu_mndr;         /* UART M/N/D least significant bits register   */
    dword msmu_irda;         /* UART IRDA register                           */
    
    dword msmu_sr;           /* Status Register of UART                      */
    dword msmu_misr;         /* UART masked interrupt status register        */
    dword msmu_isr;          /* UART interrupt status register               */
   
    dword msmu_int_ena;      /* UART interrupt enable                        */
    dword msmu_tf;           /* TX FIFO Register of UART                     */
    dword msmu_rf;           /* RX FIFO Register of UART                     */
} siors_hw_reg_type;
  

typedef struct                 /* This structure contain UART variables         */
{
    sio_rx_func_ptr_type    sio_rx_func_ptr;
                               /* Pointer to queue from which SIO will dequeue 
                                * packets to TX.                                */
                               /* The Current RX and TX bit rates              */
    sio_bitrate_type        sio_current_rx_bitrate;
    sio_bitrate_type        sio_current_tx_bitrate;
                               /* The variable following records the character 
                                * time for current bit-rate                    */
    dword                   sio_single_char_time;
                            /* Indicates whether serial outbound flow is enabled*/
    boolean                 sio_outbound_flow_disabled;
                            /* In-bound flow control state                     */
    boolean                 sio_inbound_flow_enabled;
    boolean                 inbound_auto_hw_flow_enabled;
                            /* Is the UART in packet optimized mode? */
    boolean                 sio_uart_packet_mode;
    /* --------- Variables used in packet detection. --------------------------*/
                               /* Pointer to function which will be called when 
                                * packet is detected.                          */
    sio_vv_func_ptr_type    sio_packet_func_ptr;
                               /* Boolean indicating, if TRUE, that timer for 
                                * byte following PPP flag is scheduled but has 
                                * not happened yet. FALSE, timer has expired.  */
    boolean                 sio_packet_detection_active;

    /* --------- Control and shadow Variables ---------------------------------*/
    int32                   sio_num_avail_tx_fifo_bytes;
                               /* Shadow Registers.                            */
    word                    sio_uart_flow_rx_rfr_lev;
                               /* TXLEV for data flow.                         */
    word                    sio_uart_flow_txlev;
                               /* Number of bytes that can be written to transmit */
    word                    sio_uart_flow_rxlev;
    word                    sio_imr_shadow;
    byte                    sio_csr_shadow;
    word                    sio_ipr_shadow;
    word                    sio_mr1_shadow;
    uint16                  sio_mvr_shadow;
    uint16                  sio_nvr_shadow;
    uint16                  sio_dvr_shadow;
                               /* Value that needs to be written to IPR in order 
                                * for RXSTALE interrupt to function properly.  */
    word                    sio_ipr_rxstale_on;
                               /* M,N,D register default */
    uint16                  sio_mvr_default;
    uint16                  sio_nvr_default;
    uint16                  sio_dvr_default;
                               /* UART error structure.                        */
    uart_errs_type          sio_errs;
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
    void                   *mdd_head_ptr;
                               /* Main UART ISR, power control ISR */
    tramp_isr_type          isr_type;
    tramp_isr_type          wakeup_isr_type;
                               /* If RX isr is registered, RX stream is open   */
    boolean                 is_isr_registered;
							   /* If the wake up event is activated            */
	boolean					is_wakeup_isr_registered;
    CLKRGM_MSM_CLK_TYPE_P   clk_source;
} siors_variable_type;


typedef struct              /* The port_type structure identify a PORT     */
{
    uart_device_id_type     device_id; /* Device ID                        */
    siors_hw_reg_type       hw;        /* Hardware registers               */
    siors_variable_type     gv;        /* General variables                */
    boolean                 is_ext_speed; /* Have faster than 115200 baud  */
    boolean                 is_ext_fifo;  /* Is the fifo field extended?   */
    boolean                 is_clock_enabled;
} siors_port_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================
 FUNCTION/PROCEDURE prototypes.
===========================================================================*/              

/*===========================================================================

FUNCTION SIORS_INITIALIZE                           EXTERNALIZED FUNCTION

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
    uart_device_id_type  device_id,           /* Device ID                 */
    sio_pdd_api_type    *pFuncTab,            /* API entry point structure */
    void                **pHead,              /* Pointer to head structure */
    void                *pMddHead,            /* Pointer to MDD head       */
    void (*mdd_callback) (void *, siors_mdd_callback_type)
                                              /* MDD call back function    */
);

#endif /* SIORS232_PD_H */
