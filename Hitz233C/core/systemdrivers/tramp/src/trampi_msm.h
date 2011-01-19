#ifndef _TRAMPI_MSM_H_
#define _TRAMPI_MSM_H_
/*==========================================================================

             T R A M P O L I N E    S E R V I C E S

GENERAL DESCRIPTION
  This file contains the MSM-specific declarations for the QSC11X0
  Application Tramp Services.

  These Trampoline Services use the Vectored Interrupt Controller used
  as a Prioritized Interrupt Controller.  The VIC supports 42
  interrupt sources that can be grouped into 8 priority levels (7
  being the highest priority value and 0 being the least), with each
  priority level containing upto 40 sources.

  The VIC provides registers to program the priority level for each
  interrupt source. It allows for nested interrupts. Interrupts
  programmed at the same priority level are dispatched in a
  round-robin manner.

  The interrupt vector index of the highest priority source is
  returned from the VEC_INDEX_RD register. Reading this register
  causes the hardware to push the ISR currently executing onto a
  hardware stack and the new higher priority interrupt can begin
  execution. The VEC_PEND_INDEX_RD register should be read after an
  ISR is done to detect if there is a pending interrupt. A special
  NON_VECTOR value (0xFFFFFFFF) is returned if there are no more
  interrupts pending.

  Two status registers VICIRQ_STATUS_0 and VICIRQ_STATUS_1 are
  provided to check which interrupt occurred. If any of the bits are
  set in these registers, an interrupt from the corresponding source
  has occurred.  VICIRQ_STATUS_0 has bits reserved for 32 interrupt
  sources and VICIRQ_STATUS_1 has bits reserved for 16 interrupt
  sources.

  Once the interrupt is acknowledged, the corresponding in the
  VICIRQ_CLEAR_0/VICIRQ_CLEAR_1 is cleared to clear the status bits in
  the associated status register.

  Interrupts are enabled/disabled by setting/clearing the
  corresponding bits in the VICIRQ_ENABLE_0/VICIRQ_ENABLE_1 register.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Function tramp_init() should be called before any of the Tramp
  Services can be used.

Copyright (c) 2004 - 2008 by QUALCOMM Incorporated.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or
software is regulated by the U.S. Government. Diversion contrary to
U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //depot/asic/qsc1100/drivers/tramp/trampi_msm.h#9 $
$DateTime: 2008/03/18 16:41:29 $ $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/07   vph     Ported from qsc6055.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Setup check */
#ifndef FEATURE_2ND_LVL_INTR 
#error FEATURE_2ND_LVL_INTR MUST BE DEFINED FOR QSC1100 BUILDS!
#endif /* FEATURE_2ND_LVL_INTR */

/*===========================================================================
** Table of data for setting-up and tearing-down ISRs
===========================================================================*/
extern tramp_isr_tab_type tramp_isr_tbl[TRAMP_NUM_PIC_INTS];
extern tramp_sub_isr_tab_type tramp_sub_isr_tbl[TRAMP_NUM_SUB_INTS];

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* TRAMP_EN access info for sub interrupt enabling */
typedef enum
{
  TRAMP_RTC_INT_ENABLE = 0,
  TRAMP_MOD_TX_SLOT_INT_EN,
  TRAMP_MOD_TX_FR_INT_EN,
  TRAMP_KEYSENSE_INT_EN,
  TRAMP_EN_MAX
} tramp_en_idx_type;

/*----------------------------------------------------------------------------
                        Priorities of Interrupts

   Each Interrupt has it's own priority level register. A programmed
   value of 0b000 represents the lowest priority and 0b111 the highest
   priority level.
-----------------------------------------------------------------------------*/
#define TRAMP_SSBI_ISR_PRIO_VAL                   6
#define TRAMP_SPI1_INPUT_SERVICES_ISR_PRIO_VAL    2
#define TRAMP_SPI1_OUTPUT_SERVICES_ISR_PRIO_VAL   2
#define TRAMP_SPI1_ERROR_ISR_PRIO_VAL             2
#define TRAMP_SPI0_INPUT_SERVICES_ISR_PRIO_VAL    2
#define TRAMP_SPI0_OUTPUT_SERVICES_ISR_PRIO_VAL   2
#define TRAMP_SPI0_ERROR_ISR_PRIO_VAL             2
#define TRAMP_APP_SEC_ERR_ISR_PRIO_VAL            2
#define TRAMP_SYS_SEC_ERR_ISR_PRIO_VAL            2
#define TRAMP_SYS_POINTER_WRAP_ISR_PRIO_VAL       4
#define TRAMP_APP_POINTER_WRAP_ISR_PRIO_VAL       4
#define TRAMP_BPM_ISR_PRIO_VAL                    2
#define TRAMP_JD_ISR_PRIO_VAL                     2
#define TRAMP_GPIO_GROUP1_ISR_PRIO_VAL            4
#define TRAMP_GPIO_GROUP0_ISR_PRIO_VAL            4
#define TRAMP_USB_HS_ISR_PRIO_VAL                 5
#define TRAMP_ADSP_INT2_ISR_PRIO_VAL              6
#define TRAMP_ADSP_INT1_ISR_PRIO_VAL              6
#define TRAMP_ADSP_INT0_ISR_PRIO_VAL              6
#define TRAMP_PM_ISR_PRIO_VAL                     4
#define TRAMP_I2CC1_ISR_PRIO_VAL                  6
#define TRAMP_I2CC0_ISR_PRIO_VAL                  6
#define TRAMP_KEYSENSE_GROUP_ISR_PRIO_VAL         6
#define TRAMP_UART1_ISR_PRIO_VAL                  5
#define TRAMP_UART0_ISR_PRIO_VAL                  5
#define TRAMP_AUX_CODEC_ONES_ISR_PRIO_VAL         2
#define TRAMP_SDCC_INT1_ISR_PRIO_VAL              4
#define TRAMP_SDCC_INT0_ISR_PRIO_VAL              4
#define TRAMP_TIME_TICK_ISR_PRIO_VAL              4
#define TRAMP_SLEEP_TIME_TICK_ISR_PRIO_VAL        6
#define TRAMP_SLEEP_TIMER0_ISR_PRIO_VAL           6
#define TRAMP_SLEEP_TIMER1_ISR_PRIO_VAL           6
#define TRAMP_UART2_RX_DATA_ISR_PRIO_VAL          5
#define TRAMP_UART1_RX_DATA_ISR_PRIO_VAL          5
#define TRAMP_UART0_RX_DATA_ISR_PRIO_VAL          5
#define TRAMP_UART_DM_RX_DATA_ISR_PRIO_VAL        5
#define TRAMP_UART_DM_ISR_PRIO_VAL                5
#define TRAMP_MDP_ISR_PRIO_VAL                    6
#define TRAMP_CAMIF_PAD_REG_OVERFLOW_ISR_PRIO_VAL 6
#define TRAMP_I2CC_ISR_PRIO_VAL                   6
#define TRAMP_I2CC2_ISR_PRIO_VAL                  6
#define TRAMP_UART2_ISR_PRIO_VAL                  5
#define TRAMP_SPI2_INPUT_SERVICE_ISR_PRIO_VAL     2
#define TRAMP_SPI2_OUTPUT_SERVICE_ISR_PRIO_VAL    2
#define TRAMP_SPI2_ERROR_ISR_PRIO_VAL             2
#define TRAMP_SYS_ON_LINE0_ISR_PRIO_VAL           6
#define TRAMP_RXF_JAMDET_ISR_PRIO_VAL             2
#define TRAMP_SBI_ISR_PRIO_VAL                    6
#define TRAMP_MDSP_ISR_PRIO_VAL                   6
#define TRAMP_SLEEP_FEE_ISR_PRIO_VAL              6
#define TRAMP_SLEEP_INT0_ISR_PRIO_VAL             6
#define TRAMP_RTC_ROLLOVER_ISR_PRIO_VAL           4
#define TRAMP_OFFLINE_DONE_ISR_PRIO_VAL           5
#define TRAMP_QUICK_PAGING_ISR_PRIO_VAL           5
#define TRAMP_DEC_DONE_ISR_PRIO_VAL               2
#define TRAMP_MOD_TX_FR_ISR_PRIO_VAL              6
#define TRAMP_RTC_1XHDR_ISR_PRIO_VAL              6
#define TRAMP_MOD_TX_SLOT_ISR_PRIO_VAL            6
#define TRAMP_DEM_TIME_INT2_ISR_PRIO_VAL          6
#define TRAMP_DEM_TIME_INT1_ISR_PRIO_VAL          6
#define TRAMP_UNUSED_ISR_PRIO_VAL                 1

/* Number of sub interrupts in the each consolidated interrupt */
#define TRAMP_MAX_RTC_ISR                     0x3
#define TRAMP_MAX_TX_SLOT_ISR                 0x1
#define TRAMP_MAX_MOD_TX_FR_ISR               0x1
#define TRAMP_MAX_KEYSENSE_ISR                0x1

/* Number of bits corresponding to interrupts in the enable registers */
#define NUM_INT_BITS_IN_REG 28

/* Special value returned by the IRQ_VEC_INDEX_RD register
 * if there are no more interrupts pending.
 */
#define TR_NON_VECTOR 0x3F

/* The following defines map the Application VIC registers to common
 * symbols to be used by the common TRAMP code.
 */
#define PRIORITY_n(n)            HWIO_ADDRI(INT_n_PRIORITY, n)

#endif /* _TRAMPI_MSM_H_ */

