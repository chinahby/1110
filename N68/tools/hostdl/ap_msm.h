#ifndef AP_MSM_H
#define AP_MSM_H

/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
; GENERAL DESCRIPTION
;	This header file contains hardware specific definitions which
;	must be ported to each MSM
;
; EXTERNALIZED FUNCTIONS
;   None. 
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;	None
;
; Copyright (c)  2008 Qualcomm Incorporated. 
; All Rights Reserved.
; Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================
                          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/tools/hostdl/HOSTDL.14.00/ap_msm.h#1 $ $DateTime: 2008/01/21 21:01:41 $ $Author: opham $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/MSM6275/ap_msm.h#2 
  
 when       who     what, where, why
 --------   ---     --------------------------------------------------------
 01/03/08   op      NOR flash driver unification
 ============================================================================*/


#include "msm.h"
#include "bsp.h"

extern void mdelay (uint32);

//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------


// some macros not in msm6100bits.h

/*      Reset Break             */
#define UART2_CR_RESET_BREAK (0x04 << HWIO_SHFT(UART2_CR,CHANNEL_COMMAND))

/*      Stop Break             */
#define UART2_CR_STOP_BREAK  (0x06 << HWIO_SHFT(UART2_CR,CHANNEL_COMMAND))

/*      Set Ready for Receive             */
#define UART2_CR_SET_RFR     (0x0D << HWIO_SHFT(UART2_CR,CHANNEL_COMMAND))





/* defines needed for ap_uart.c */

/*      Break received                */
#define UART2_SR_BREAK_RXD           (0x01 << \
        HWIO_SHFT(UART2_SR,RX_BREAK))

/*      Parity or Framing error       */
#define UART2_SR_PF_ERR              (0x01 << \
        HWIO_SHFT(UART2_SR,PAR_FRAME_ERR))

/*      Overrun error                 */
#define UART2_SR_OVR_ERR             (0x01 << \
        HWIO_SHFT(UART2_SR,UART_OVERRUN))

/*      8 bits per character          */
#define UART2_MR2_8BPC               (0x03 << \
        HWIO_SHFT(UART2_MR2,BITS_PER_CHAR))

/*      1 stop bit                    */
#define UART2_MR2_1SB                (0x01 << \
        HWIO_SHFT(UART2_MR2,STOP_BIT_LEN))

/*      Reset error status            */
#define UART2_CR_RESET_ERR           (0x03 << \
        HWIO_SHFT(UART2_CR,CHANNEL_COMMAND))

/*      Reset transmitter             */
#define UART2_CR_RESET_TX            (0x02 << \
        HWIO_SHFT(UART2_CR,CHANNEL_COMMAND))

/*      Reset receiver                */
#define UART2_CR_RESET_RX            (0x01 << \
        HWIO_SHFT(UART2_CR,CHANNEL_COMMAND))

/*      Enable receiver               */
#define UART2_CR_RX_ENA              (0x01 << \
        HWIO_SHFT(UART2_CR,UART_RX_EN))

/*      Enable transmitter            */
#define UART2_CR_TX_ENA              (0x01 << \
        HWIO_SHFT(UART2_CR,UART_TX_EN))



/* Selection of which receiver status bits we consider to be errors. */

#define  UART2_SR_RX_ERROR  (UART2_SR_BREAK_RXD |       \
                            UART2_SR_PF_ERR    |       \
                            UART2_SR_OVR_ERR )


/*---------------------------------------------------------------------------

                    MACRO KICK_WATCHDOG

This macro resets the watchdog timer circuit.
---------------------------------------------------------------------------*/
#define KICK_WATCHDOG()                                               \
            HWIO_OUT(WDOG_RESET, HWIO_FMSK(WDOG_RESET,WATCH_DOG));


#ifndef ERR_FATAL
#define ERR_FATAL(x,a,b,c)  while (1);
#endif

/*--------------------------------------------------------------------------
 * Function Prototypes
 *------------------------------------------------------------------------*/


extern uint32 clk_regime_get_NAND_wait_states(void);



#endif /* AP_MSM_H */

