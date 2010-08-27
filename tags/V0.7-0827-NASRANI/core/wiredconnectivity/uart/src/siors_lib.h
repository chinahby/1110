#ifndef SIORS_LIB_H
#define SIORS_LIB_H
/*===========================================================================

         S I O   D E V I C E   L A Y E R   S H A R E D   L I B  

DESCRIPTION
   This is the header file for the common code used by the SIO UART driver. 

   Copyright (c) 2007 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors_lib.h#3 $ $DateTime: 2009/01/30 12:47:02 $ $Author: cmihalik $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/08   rh      Use a common definition for HW type
09/24/07   rh      Add SC2x specific clock regime workaround
09/24/07   rh      Add new quick open option for power up/down event
08/10/07   rh      Add RX activity detection
08/09/07   rh      Added SIORS_TX_PKT_DONE callback 
02/23/07   rh      Created file.
===========================================================================*/

/*===========================================================================

                        FEATURE FLAGS RESOLUTION 

===========================================================================*/

/*===========================================================================
 
                        Include Files

===========================================================================*/

#include "sio.h"                             /* SIO Server header file     */
#include "clk.h"                             /* Clock services header file */
#include "customer.h"                        /* Customer feature files     */
#include "tramp.h"                 /* Trampoline typedefs and prototypes   */

/*===========================================================================

                    ENUMERATION AND CONSTANTS

===========================================================================*/

/* SC2x uses a different way to set the clock frequency, add the
 * translation routine here.  Hopefully, the routine will be moved
 * off to the clock regime int he future                                   */
#ifdef SIORS_USES_SC2X_CLKRGM
#define  CLKRGM_UARTDM_SPEED_7_3728_MHZ                7372800
#define  CLKRGM_UARTDM_SPEED_14_7456_MHZ              14745600
#define  CLKRGM_UARTDM_SPEED_51_2_MHZ                 51200000
#define  CLKRGM_UARTDM_SPEED_58_9824_MHZ              58982400
#define  CLKRGM_UARTDM_SPEED_64_MHZ                   64000000

#define  clk_regime_sel_uart1dm_clk       clk_regime_msm_sel_uart_dm_clk

#ifndef CLKRGM_UART2_CLK
#define CLKRGM_UART2_CLK             CLK_RGM_UART2_M
#endif

#ifdef  CLKRGM_UART1DM_CLK
#undef  CLKRGM_UART1DM_CLK
#define CLKRGM_UART1DM_CLK           CLK_RGM_UART1_M
#endif

#endif  /* SC2X specific clock regime support */

/* Remap SIO configuration to internal MACRO */
#define HW_NONE       0
#define HW_UART       1
#define HW_UARTDM     2
#define HW_UARTDM_LT  3

//  Choice values for tramp versions.
#define TRAMP_VERSION_INVALID   0
#define TRAMP_VERSION_2_0       1 
#define TRAMP_VERSION_1100      2
#define TRAMP_VERSION_6055      3

/*===========================================================================

                        DATA STRUCTURES 

===========================================================================*/

/* This is required information when PDD generate a call back into the MDD
 * layer                                                                   */
typedef enum
{
    SIORS_TX_INTR,                  /* TX Interrupt */
    SIORS_TX_PKT_DONE,              /* TX Current packet finished */
    SIORS_RXLEV_INTR,               /* RX Level interrupt */
    SIORS_RXSTALE_INTR,             /* RX Stale interrupt */
    SIORS_RX_ACTIVITY            /* RX activity detected */
}siors_mdd_callback_type;

/* This sructure contain a set of standard APIs exposed by each of the 
 * device driver                                                           */
typedef struct
{
                                    /* TX Hardware flow control */
    void    (*enable_tx_hw_fctl)    (void *pHead);
    void    (*disable_tx_hw_fctl)   (void *pHead);
    void    (*enable_rx_hw_fctl)    (void *pHead);
    void    (*disable_rx_hw_fctl)   (void *pHead);
    void    (*enable_auto_hw_fctl)  (void *pHead);
    void    (*disable_auto_hw_fctl) (void *pHead);
                                    /* TX Interrupt control */
    void    (*enable_tx_interrupt)  (void *pHead, boolean empty_chk);
    void    (*disable_tx_interrupt) (void *pHead);
                                    /* Power control */
    void    (*detect_rx_activity) (void *pHead);
                                    /* Bit rate setting */
    void    (*set_bit_rate) (void *pHead, 
                            sio_bitrate_type req_bitrate);
                                    /* Reset and Enable routines */
    void    (*reset_tx)   (void *pHead);
    void    (*enable_tx)  (void *pHead);
    void    (*reset_rx)   (void *pHead);
    void    (*disable_rx) (void *pHead);
    void    (*enable_rx)  (void *pHead);
                                    /* Opening and closing port */
    void    (*open)  (void *pHead, boolean );
    void    (*close) (void *pHead);
                                    /* Transmit and receive data */
    void    (*transmit) (void *pHead, byte *tx_ptr, int32 *byte_count);
    void    (*receive)  (void *pHead, byte *rx_ptr, uint32 *byte_count);
    void    (*receive_tail_char)    (void *pHead, byte *rx_ptr, 
                                     uint32 *byte_count, byte tail_char,
                                     boolean *more_data);
    int32   (*transmit_single_char) (void *pHead); /* Not used right now */
                                    /* Getting status */
    void    (*get_current_rts)  (void *pHead, sio_ioctl_param_type *param);
    int32   (*is_tx_fifo_empty) (void *pHead);
                                    /* Check for ok to power down */
    boolean (*check_rx_cable_status) (void *pHead);

	 /* Optional driver APIs, set to NULL if the function is not supported */
	                                 /* TX break and RX break detection 
												 * These function are optional */
    void    (*set_tx_break_state)  (void *pHead, boolean enable_break);
} sio_pdd_api_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================
 FUNCTION/PROCEDURE prototypes.
===========================================================================*/              

#endif /* SIORS_LIB_H */

