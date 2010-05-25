#ifndef _TRAMP_MSM_H_
#define _TRAMP_MSM_H_

/*==========================================================================

                 I N T E R R U P T   T R A M P O L I N E
             T A R G E T   S P E C I F I C   H E A D E R   F I L E
                              Q S C 1 1 X X
                     
GENERAL DESCRIPTION
  This file contains the MSM-specific declarations for the QSC11XX
  Modem Tramp Services.
  
  These Trampoline Services use the Prioritized Interrupt Controller.
  The PIC supports 42 interrupt sources that can be grouped into 8
  priority levels (0 being the highest priority value and 7 being the
  least), with each priority level containing upto 40 sources.

  The PIC provides registers to program the priority level for each
  interrupt source. It allows for nested interrupts. Interrupts programmed
  at the same priority level are dispatched in a round-robin manner.

  The interrupt vector index of the highest priority source is returned
  from the VEC_INDEX_RD register. Reading this register causes the
  hardware to push the ISR currently executing onto a hardware stack and
  the new higher priority interrupt can begin execution. VEC_INDEX_PEND_RD
  should be read after an ISR is done to detect if there is another
  pending interrupt. A special NON_VECTOR value (0x3f) is returned if
  there are no more interrupts pending.

  Two status registers IRQ_STATUS_0 and IRQ_STATUS_1 are provided to check
  which interrupt occurred. If any of the bits are set in these registers,
  an interrupt from the corresponding source has occurred. IRQ_STATUS_0
  has bits reserved for 32 interrupt sources and IRQ_STATUS_1 has bits
  reserved for 10 interrupt sources.

  Once the interrupt is acknowledged, the corresponding bit in the
  IRQ_CLEAR_0/IRQ_CLEAR_1 is cleared to clear the status bits in the
  associated status register.

  Interrupts are enabled/disabled by setting/clearing the corresponding
  bits in the IRQ_ENABLE_0/IRQ_ENABLE_1 register.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Function tramp_init() should be called before any of the Tramp Services
  can be used.

Copyright (c) 2004, 2005 by QUALCOMM Incorporated.  All Rights Reserved.  
Copyright (c) 2007 - 2008 by QUALCOMM Incorporated.  All Rights Reserved.

QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //depot/asic/qsc1100/drivers/tramp/tramp_msm.h#9 $  
$DateTime: 2008/03/25 14:44:18 $ $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/07   vph     Ported from QSC60x5

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"


/*===========================================================================

                         CONSTANTS

===========================================================================*/

/*===========================================================================

                        TYPE DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                  Definitions for System Interrupts
   The following are the enumerated list of interrupts defined for QSC11XX
---------------------------------------------------------------------------*/
/* TRAMP table to use masked output */
typedef const void * const hwio_tramp_table_t;
extern hwio_tramp_table_t hwio_tramp_table;\

#define HWIO_TRAMP_EN_ADDR(n) \
  (((tramp_access_type*)hwio_tramp_table)[n].out_addr)
#define HWIO_TRAMP_EN_RMSK(n) \
  (((tramp_access_type*)hwio_tramp_table)[n].out_rmsk) 
#define HWIO_TRAMP_EN_SHFT(n) 0x0
#define HWIO_TRAMP_EN_SHDW_ADDR(n) \
  (((tramp_access_type*)hwio_tramp_table)[n].out_shdw)
#define HWIO_TRAMP_EN_SHDW(n) \
  (*(((tramp_access_type*)hwio_tramp_table)[n].out_shdw))
#define HWIO_TRAMP_EN_SHDW_SHFT(n) \
  (((tramp_access_type*)hwio_tramp_table)[n].shdw_shft)
#define HWIO_TRAMP_EN_COMB_SHDW_ADDR(n) \
  (((tramp_access_type*)hwio_tramp_table)[n].comb_shdw)
#define HWIO_TRAMP_EN_COMB_SHDW(n) \
  (*(((tramp_access_type*)hwio_tramp_table)[n].comb_shdw))
#define HWIO_TRAMP_EN_SHRD_SHDW(n) \
  (*(((tramp_access_type*)hwio_tramp_table)[n].shrd_shdw))
#define HWIO_TRAMP_EN_SHRD_SHDW_SHFT(n) \
  (((tramp_access_type*)hwio_tramp_table)[n].shrd_shft)

#define HWIO_TRAMP_EN_READ(n) \
  in_dword_masked( HWIO_TRAMP_EN_ADDR(n), HWIO_TRAMP_EN_RMSK(n) )

#define HWIO_TRAMP_EN_OUTMI(n, m, v) \
  INTLOCK(); \
  if( HWIO_TRAMP_EN_SHDW_ADDR(n) != NULL ) \
   {                                                                 \
    out_dword_masked( HWIO_TRAMP_EN_ADDR(n), m, v, HWIO_TRAMP_EN_SHDW(n) ); \
    if( HWIO_TRAMP_EN_COMB_SHDW_ADDR(n) != NULL ) \
    { \
      HWIO_TRAMP_EN_COMB_SHDW(n) = \
        ( HWIO_TRAMP_EN_SHDW(n) << HWIO_TRAMP_EN_SHDW_SHFT(n) ) | \
        ( HWIO_TRAMP_EN_SHRD_SHDW(n) << HWIO_TRAMP_EN_SHRD_SHDW_SHFT(n) ); \
    } \
  } \
  else \
  { \
    out_dword_masked_ns( HWIO_TRAMP_EN_ADDR(n), m, \
                         v, HWIO_TRAMP_EN_READ(n) ); \
  } \
  INTFREE()

#define HWIO_TRAMP_EN_CHECK(n, m, v) \
  INTLOCK(); \
  if( HWIO_TRAMP_EN_COMB_SHDW_ADDR(n) != NULL ) \
  { \
    v = ( HWIO_TRAMP_EN_COMB_SHDW(n) & m ); \
  } \
  else if( HWIO_TRAMP_EN_SHDW_ADDR(n) != NULL ) \
  { \
    v = ( HWIO_TRAMP_EN_SHDW(n) & m ); \
  }                                                                 \
  else \
  { \
    v = ( HWIO_TRAMP_EN_READ(n) & m ); \
  } \
  INTFREE()

typedef enum
{
  /*                    !!!!!   NOTE:   !!!!!
   *   The following enumerations from the first upto
   *   TRAMP_NUM_PIC_INTS is the list of interrupts which are handled
   *   directly by the PIC. These interrupts are in the order in which
   *   they occur in the PIC registers from MSB of INT_ENABLE_1 to LSB
   *   of INT_ENABLE_0.  The order of these enums MUST be reflected in
   *   the order of the entries in tramp_isr_tbl in trampi_release.c
   */

  TRAMP_SSBI_ISR,                     /* SSBI_INT                 */
  TRAMP_SPI1_INPUT_SERVICES_ISR,      /* SPI1_INPUT_SERVICE_IRQ   */
  TRAMP_SPI1_OUTPUT_SERVICES_ISR,     /* SPI1_OUTPUT_SERVICE_IRQ  */
  TRAMP_SPI1_ERROR_ISR,               /* SPI1_ERROR_IRQ           */
  TRAMP_SPI0_INPUT_SERVICES_ISR,      /* SPI0_INPUT_SERVICE_IRQ   */
  TRAMP_SPI0_OUTPUT_SERVICES_ISR,     /* SPI0_OUTPUT_SERVICE_IRQ  */
  TRAMP_SPI0_ERROR_ISR,               /* SPI0_ERROR_IRQ           */
  TRAMP_APP_SEC_ERR_ISR,              /* APP_SEC_ERR              */
  TRAMP_SYS_SEC_ERR_ISR,              /* SYS_SEC_ERR              */
  TRAMP_SYS_POINTER_WRAP_ISR,         /* SYS_POINTER_WRAP         */
  TRAMP_APP_POINTER_WRAP_ISR,         /* APP_POINTER_WRAP         */
  TRAMP_BPM_ISR,                      /* BPM_INT                  */
  TRAMP_JD_ISR,                       /* JD_INT                   */
  TRAMP_GPIO_GROUP1_ISR,              /* GPIO2_GROUP_INT          */
  TRAMP_GPIO_GROUP0_ISR,              /* GPIO1_GROUP_INT          */
  TRAMP_USB_ISR,                      /* USB_HS_IRQ               */
  TRAMP_ADSP_INT2_ISR,                /* ADSP_INT_2               */
  TRAMP_ADSP_INT1_ISR,                /* ADSP_INT_1               */
  TRAMP_ADSP_INT0_ISR,                /* ADSP_INT_0               */
  TRAMP_PM_ISR,                       /* PM_INT_N                 */
  TRAMP_I2CC1_ISR,                    /* I2CC1_INT                */
  TRAMP_I2CC0_ISR,                    /* I2CC0_INT                */
  TRAMP_KEYSENSE_GROUP_ISR,           /* KEYSENSE_GROUP_INT       */
  TRAMP_UART1_ISR,                    /* UART1_INT                */
  TRAMP_UART0_ISR,                    /* UART0_INT                */
  TRAMP_AUX_CODEC_ONES_ISR,           /* AUX_CODEC_ONES_INT       */
  TRAMP_SDCC_0_ISR,                   /* SDCC_INT_0               */
  TRAMP_SDCC_1_ISR,                   /* SDCC_INT_1               */
  TRAMP_TICK_ISR,                     /* TIME_TICK_INT            */
  TRAMP_SLEEP_TIME_TICK_ISR,          /* SLEEP_TIME_TICK_INT      */
  TRAMP_SLEEP_TIMER0_ISR,             /* SLEEP_TIMER_0_INT        */
  TRAMP_SLEEP_TIMER1_ISR,             /* SLEEP_TIMER_1_INT        */
  TRAMP_UART2_RX_DATA_ISR,            /* UART2_RX_DATA_INT        */
  TRAMP_UART1_RX_DATA_ISR,            /* UART1_RX_DATA_INT        */
  TRAMP_UART0_RX_DATA_ISR,            /* UART0_RX_DATA_INT        */
  TRAMP_UART_DM_RX_DATA_ISR,          /* UART_DM_RX_DATA_INT      */
  TRAMP_UART_DM_ISR,                  /* UART_DM_INT              */
  TRAMP_MDP_ISR,                      /* MDP_INT                  */
  TRAMP_CAMIF_PAD_REG_OVERFLOW_ISR,   /* CAMIF_PAD_REG_OVERFLOW   */
  TRAMP_I2CC_ISR,                     /* I2CC_INT                 */
  TRAMP_I2CC2_ISR,                    /* I2CC2_INT                */
  TRAMP_UART2_ISR,                    /* UART2_INT                */
  TRAMP_SPI2_INPUT_SERVICE_ISR,       /* SPI2_INPUT_SERVICE_IRQ   */
  TRAMP_SPI2_OUTPUT_SERVICE_ISR,      /* SPI2_OUTPUT_SERVICE_IRQ  */
  TRAMP_SPI2_ERROR_ISR,               /* SPI2_ERROR_IRQ           */
  TRAMP_SYS_ON_LINE0_ISR,             /* SYS_ON_LINE_0            */
  TRAMP_RXF_JAMDET_ISR,               /* RXF_JAMDET_IRQ           */
  TRAMP_SBI_ISR,                      /* SBI_INT                  */
  TRAMP_MDSP_INT_0_ISR,               /* MDSP_INT                 */
  TRAMP_SLEEP_FEE_ISR,                /* SLEEP_FEE_INT            */
  TRAMP_SLEEP_INT0_ISR,               /* SLEEP_INT_0              */
  TRAMP_RTC_ROLLOVER_ISR,             /* RTC_ROLLOVER_INT         */
  TRAMP_OFFLINE_DONE_ISR,             /* OFFLINE_DONE_INT         */
  TRAMP_QUICK_PAGING_ISR,             /* QUICK_PAGING_INT         */
  TRAMP_DEC_DONE_ISR,                 /* DEC_DONE_INT             */
  TRAMP_MOD_TX_FR_ISR,                /* MOD_TX_FR_INT            */
  TRAMP_RTC_1XHDR_ISR,                /* RTC_1XHDR_INT            */
  TRAMP_MOD_TX_SLOT_ISR,              /* MOD_TX_SLOT_INT          */
  TRAMP_DEM_TIME_INT2_ISR,            /* DEM_TIME_INT_2           */
  TRAMP_DEM_TIME_INT1_ISR,            /* DEM_TIME_INT_1           */
  TRAMP_NUM_PIC_INTS,
  
  TRAMP_COMP1_INT = TRAMP_NUM_PIC_INTS,
  TRAMP_COMP2_INT,
  TRAMP_PNROLL_ISR,
  TRAMP_125_MS_ISR,                    /* 1.25 ms interrupt for 1x */
  TRAMP_1X_TX_FR_ISR,                  /* 1x TX frame interrupt              */
  TRAMP_KEY_SENSE_ISR,
  
  TRAMP_NUM_INTS,

  /* The following enumeration is deliberately put here to catch
   * unsupported interrupts which were used in older targets.  There
   * might be client codes that still use these interrupts.  If those
   * interrupts are used it will generate an error fatal forcing the
   * clients to fix their code. A run time detection is used against
   * compile time detection to facilitate chip bring up where most of
   * the client code is still old and is expected not to have new code
   * supporting the new interrupts.
   */
  TRAMP_UN_SUPPORTED_ISR = TRAMP_NUM_INTS,

  TRAMP_NO_INTS = 0x3F          /* Value returned by vector reg
                                 * if no interrupts are pending
                                 */

} tramp_isr_type;

#define TRAMP_NUM_SUB_INTS (TRAMP_NUM_INTS - TRAMP_NUM_PIC_INTS)

#define tramp_check_raw_interrupt(a)    tramp_is_interrupt_pending(a)

#define TRAMP_UART_ISR                  TRAMP_UART1_ISR
#define TRAMP_TICK2_ISR                 TRAMP_SLEEP_TIMER1_ISR
#define TRAMP_RX_QP_ISR                 TRAMP_QUICK_PAGING_ISR
#define TRAMP_RX_ISR                    TRAMP_DEC_DONE_ISR
#define TRAMP_TX_ISR                    TRAMP_1X_TX_FR_ISR
#define TRAMP_SYS_TIME_INT2             TRAMP_DEM_TIME_INT2_ISR
#define TRAMP_SYS_TIME_INT1             TRAMP_DEM_TIME_INT1_ISR
#define TRAMP_SLEEP_INT_1X              TRAMP_SLEEP_INT0_ISR
#define TRAMP_SLP_TICK_ISR              TRAMP_SLEEP_TIME_TICK_ISR
#define TRAMP_DM_UART_ISR               TRAMP_UART_DM_ISR
#define TRAMP_ISR_CNT                   TRAMP_NUM_INTS

#define TRAMP_UART1DM_ISR               TRAMP_UART1_ISR
#define TRAMP_UART1DM_DP_RX_DATA_ISR    TRAMP_UART1_RX_DATA_ISR  

/* NEED TO CHECK */
#define TRAMP_DM_DP_RX_DATA_ISR         TRAMP_UN_SUPPORTED_ISR
#define TRAMP_MDM_MARM_ISR              TRAMP_UN_SUPPORTED_ISR

#define TRAMP_DP_RX_DATA_ISR            TRAMP_UN_SUPPORTED_ISR
#define TRAMP_SLEEP_INT_HDR             TRAMP_UN_SUPPORTED_ISR
#define TRAMP_PER_SLOT_ISR              TRAMP_UN_SUPPORTED_ISR
#define TRAMP_HMOD_SLOT_ISR             TRAMP_UN_SUPPORTED_ISR
#define TRAMP_ARBITER_ISR               TRAMP_UN_SUPPORTED_ISR
#endif /* _TRAMP_MSM_H_ */
  

