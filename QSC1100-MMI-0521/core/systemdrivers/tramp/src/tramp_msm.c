
/*==========================================================================

                 T R A M P O L I N E    S E R V I C E S
       T A R G E T   S P E C I F I C   I M P L E M E N T A T I O N
                            M S M 6 8 0 0

GENERAL DESCRIPTION
  This file contains the MSM-specific declarations for the MSM6800
  Tramp Services.
  
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

  Once the interrupt is acknowlTRAMP_EDGEd, the corresponding bit in the
  IRQ_CLEAR_0/IRQ_CLEAR_1 is cleared to clear the status bits in the
  associated status register.

  Interrupts are enabled/disabled by setting/clearing the corresponding
  bits in the IRQ_ENABLE_0/IRQ_ENABLE_1 register.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Function tramp_init() should be called before any of the Tramp Services
  can be used.

Copyright (c) 2004 - 2008 by QUALCOMM Incorporated.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or
software is regulated by the U.S. Government. Diversion contrary to
U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //depot/asic/qsc1100/drivers/tramp/tramp_msm.c#18 $  
$DateTime: 2008/08/26 10:17:32 $ $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/07   vph     Create and ported from QSC60X5.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "tramp.h"
#include "trampi.h"

#ifdef TIMETEST
#include "timetest.h"
#endif


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* ------------------------------------------------------------------------
*                       CONSTANT / DEFINE DECLARATIONS
--------------------------------------------------------------------------*/

/* TRAMP_EN access info for sub interrupt enabling */
static tramp_access_type tramp_en_table[TRAMP_EN_MAX] =
{
  { /* TRAMP_RTC_INT_ENABLE */
    HWIO_ADDR(RTC_INT_ENABLE),
    &HWIO_SHDW(RTC_INT_ENABLE),
    HWIO_RMSK(RTC_INT_ENABLE),
    0,
    NULL,
    0,
    NULL
  },
  { /* TRAMP_MOD_TX_SLOT_INT_EN */
    HWIO_ADDR(MOD_TX_SLOT_INT_EN),
    NULL,
    HWIO_RMSK(MOD_TX_SLOT_INT_EN),
    0,
    NULL,
    0,
    NULL
  },
  { /* TRAMP_MOD_TX_FR_INT_EN */
    HWIO_ADDR(MOD_TX_FR_INT_EN),
    NULL,
    HWIO_RMSK(MOD_TX_FR_INT_EN),
    0,
    NULL,
    0,
    NULL
  },
  { /* TRAMP_KEYSENSE_INT_EN */
    HWIO_ADDR(MSM_INT_EN),
    NULL,
    HWIO_RMSK(MSM_INT_EN),
    0,
    NULL,
    0,
    NULL
  }
};
hwio_tramp_table_t hwio_tramp_table=(hwio_tramp_table_t)&tramp_en_table;

/* Table of pointers to the ISRs. Use index retrieved from PIC to access 
 *
 *  There must be a one-to-one correspondence between the entries in 
 *  this table and the values in the tramp_isr_type enumeration for 
 *  this target, upto but not including TRAMP_NUM_PIC_INTS.
 */

tramp_isr_tab_type tramp_isr_tbl[TRAMP_NUM_PIC_INTS] = {
  { /* TRAMP_SSBI_ISR */
    NULL,
    TRAMP_SSBI_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SSBI_ISR
#endif
  },

  { /* TRAMP_SPI1_INPUT_SERVICES_ISR */
    NULL,
    TRAMP_SPI1_INPUT_SERVICES_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SPI1_INPUT_SERVICES_ISR
#endif
  },

  { /* TRAMP_SPI1_OUTPUT_SERVICES_ISR */
    NULL,
    TRAMP_SPI1_OUTPUT_SERVICES_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SPI1_OUTPUT_SERVICES_ISR
#endif
  },

  { /* TRAMP_SPI1_ERROR_ISR */
    NULL,
    TRAMP_SPI1_ERROR_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SPI1_ERROR_ISR
#endif
  },

  { /* TRAMP_SPI0_INPUT_SERVICES_ISR */
    NULL,
    TRAMP_SPI0_INPUT_SERVICES_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SPI0_INPUT_SERVICES_ISR
#endif
  },

  { /* TRAMP_SPI0_OUTPUT_SERVICES_ISR */
    NULL,
    TRAMP_SPI0_OUTPUT_SERVICES_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SPI0_OUTPUT_SERVICES_ISR
#endif
  },

  { /* TRAMP_SPI0_ERROR_ISR */
    NULL,
    TRAMP_SPI0_ERROR_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SPI0_ERROR_ISR
#endif
  },

  { /* TRAMP_APP_SEC_ERR_ISR */
    NULL,
    TRAMP_APP_SEC_ERR_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_APP_SEC_ERR_ISR
#endif
  },

  { /* TRAMP_SYS_SEC_ERR_ISR */
    NULL,
    TRAMP_SYS_SEC_ERR_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SYS_SEC_ERR_ISR
#endif
  },

  { /* TRAMP_SYS_POINTER_WRAP_ISR */
    NULL,
    TRAMP_SYS_POINTER_WRAP_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TRAMP_SYS_POINTER_WRAP_ISR
#endif
  },

  { /* TRAMP_APP_POINTER_WRAP_ISR */
    NULL,
    TRAMP_APP_POINTER_WRAP_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_APP_POINTER_WRAP_ISR
#endif
  },

  { /* TRAMP_BPM_ISR */
    NULL,
    TRAMP_BPM_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_BPM_ISR
#endif
  },

  { /* TRAMP_JD_ISR */
    NULL,
    TRAMP_JD_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_JD_ISR
#endif
  },

  { /* TRAMP_GPIO_GROUP1_ISR */
    NULL,
    TRAMP_GPIO_GROUP1_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_GPIO_GROUP1_ISR
#endif
  },

  { /* TRAMP_GPIO_GROUP0_ISR */
    NULL,
    TRAMP_GPIO_GROUP0_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_GPIO_GROUP0_ISR
#endif
  },

  { /* TRAMP_USB_HS_ISR */
    NULL,
    TRAMP_USB_HS_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_AFTER,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_USB_HS_ISR
#endif
  },

  { /* TRAMP_ADSP_INT2_ISR */
    NULL,
    TRAMP_ADSP_INT2_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_ADSP_INT2_ISR
#endif
  },

  { /* TRAMP_ADSP_INT1_ISR */
    NULL,
    TRAMP_ADSP_INT1_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_ADSP_INT1_ISR
#endif
  },

  { /* TRAMP_ADSP_INT0_ISR */
    NULL,
    TRAMP_ADSP_INT0_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_ADSP_INT0_ISR
#endif
  },

  { /* TRAMP_PM_ISR */
    NULL,
    TRAMP_PM_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_PM_ISR
#endif
  },

  { /* TRAMP_I2CC1_ISR */
    NULL,
    TRAMP_I2CC1_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_I2CC1_ISR
#endif
  },

  { /* TRAMP_I2CC0_ISR */
    NULL,
    TRAMP_I2CC0_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_I2CC0_ISR
#endif
  },

  { /* TRAMP_KEYSENSE_GROUP_ISR */
    NULL,
    TRAMP_KEYSENSE_GROUP_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_AFTER,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_KEY_SENSE_ISR,
    TRAMP_MAX_KEYSENSE_ISR,
    HWIO_FMSK(MSM_INT_EN,KEYSENSE_INT),
    TRAMP_KEYSENSE_INT_EN,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_KEYSENSE_GROUP_ISR
#endif
  },

  { /* TRAMP_UART1_ISR */
    NULL,
    TRAMP_UART1_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_UART1_ISR
#endif
  },

  { /* TRAMP_UART0_ISR */
    NULL,
    TRAMP_UART0_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_UART0_ISR
#endif
  },

  { /* TRAMP_AUX_CODEC_ONES_ISR */
    NULL,
    TRAMP_AUX_CODEC_ONES_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0

#ifdef TIMETEST
   , TIMETEST_TRAMP_AUX_CODEC_ONES_ISR
#endif
  },

  { /* TRAMP_SDCC_INT0_ISR */
    NULL,
    TRAMP_SDCC_INT0_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SDCC_INT0_ISR
#endif
  },

  { /* TRAMP_SDCC_INT1_ISR */
    NULL,
    TRAMP_SDCC_INT1_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SDCC_INT1_ISR
#endif
  },

  { /* TRAMP_TIME_TICK_ISR */
    NULL,
    TRAMP_TIME_TICK_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TRAMP_TIME_TICK_ISR
#endif
  },

  { /* TRAMP_SLEEP_TIME_TICK_ISR */
    NULL,
    TRAMP_SLEEP_TIME_TICK_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SLEEP_TIME_TICK_ISR
#endif
  },

  { /* TRAMP_SLEEP_TIMER_0_ISR */
    NULL,
    TRAMP_SLEEP_TIMER0_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0

#ifdef TIMETEST
   , TIMETEST_TRAMP_SLEEP_TIMER0_ISR
#endif
  },

    { /* TRAMP_SLEEP_TIMER_1_ISR */
    NULL,
    TRAMP_SLEEP_TIMER1_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_1),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_1),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_1),
    (volatile uint32 *)HWIO_ADDR(INT_SET_1),
    NULL,
    0

#ifdef TIMETEST
   , TIMETEST_TRAMP_SLEEP_TIMER1_ISR
#endif
  },

  { /* TRAMP_UART2_RX_DATA_ISR */
    NULL,
    TRAMP_UART2_RX_DATA_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_UART2_RX_DATA_ISR
#endif
  },

  { /* TRAMP_UART1_RX_DATA_ISR */
    NULL,
    TRAMP_UART1_RX_DATA_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_UART1_RX_DATA_ISR
#endif
  },

  { /* TRAMP_UART0_RX_DATA_ISR */
    NULL,
    TRAMP_UART0_RX_DATA_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_UART0_RX_DATA_ISR
#endif
  },

  { /* TRAMP_UART_DM_RX_DATA_ISR */
    NULL,
    TRAMP_UART_DM_RX_DATA_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_UART_DM_RX_DATA_ISR
#endif
  },

  { /* TRAMP_UART_DM_ISR */
    NULL,
    TRAMP_UART_DM_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_UART_DM_ISR
#endif
  },

  { /* TRAMP_MDP_ISR */
    NULL,
    TRAMP_MDP_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_MDP_ISR
#endif
  },

  { /* TRAMP_CAMIF_PAD_REG_OVERFLOW_ISR */
    NULL,
    TRAMP_CAMIF_PAD_REG_OVERFLOW_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_CAMIF_PAD_REG_OVERFLOW_ISR
#endif
  },

  { /* TRAMP_I2CC_ISR */
    NULL,
    TRAMP_I2CC_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_I2CC_ISR
#endif
  },

  { /* TRAMP_I2CC2_ISR */
    NULL,
    TRAMP_I2CC2_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_I2CC2_ISR
#endif
  },

  { /* TRAMP_UART2_ISR */
    NULL,
    TRAMP_UART2_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_UART2_ISR
#endif
  },

  { /* TRAMP_SPI2_INPUT_SERVICE_ISR */
    NULL,
    TRAMP_SPI2_INPUT_SERVICE_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SPI2_INPUT_SERVICE_ISR
#endif
  },

  { /* TRAMP_SPI2_OUTPUT_SERVICE_ISR */
    NULL,
    TRAMP_SPI2_OUTPUT_SERVICE_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SPI2_OUTPUT_SERVICE_ISR
#endif
  },

  { /* TRAMP_SPI2_ERROR_ISR */
    NULL,
    TRAMP_SPI2_ERROR_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SPI2_ERROR_ISR
#endif
  },

  { /* TRAMP_SYS_ON_LINE0_ISR */
    NULL,
    TRAMP_SYS_ON_LINE0_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SYS_ON_LINE0_ISR
#endif
  },

  { /* TRAMP_RXF_JAMDET_ISR */
    NULL,
    TRAMP_RXF_JAMDET_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_RXF_JAMDET_ISR
#endif
  },

  { /* TRAMP_SBI_ISR */
    NULL,
    TRAMP_SBI_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_AFTER,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0

#ifdef TIMETEST
   , TIMETEST_TRAMP_SBI_ISR
#endif
  },

  { /* TRAMP_MDSP_ISR */
    NULL,
    TRAMP_MDSP_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_MDSP_ISR
#endif
  },

  { /* TRAMP_SLEEP_FEE_ISR */
    NULL,
    TRAMP_SLEEP_FEE_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SLEEP_FEE_ISR
#endif
  },

  { /* TRAMP_SLEEP_INT0_ISR */
    NULL,
    TRAMP_SLEEP_INT0_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_SLEEP_INT0_ISR
#endif
  },

  { /* TRAMP_RTC_ROLLOVER_ISR */
    NULL,
    TRAMP_RTC_ROLLOVER_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_RTC_ROLLOVER_ISR
#endif
  },

  { /* TRAMP_OFFLINE_DONE_ISR */
    NULL,
    TRAMP_OFFLINE_DONE_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_OFFLINE_DONE_ISR
#endif
  },

  { /* TRAMP_QUICK_PAGING_ISR */
    NULL,
    TRAMP_QUICK_PAGING_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_QUICK_PAGING_ISR
#endif
  },

  { /* TRAMP_DEC_DONE_ISR */
    NULL,
    TRAMP_DEC_DONE_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_AFTER,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_DEC_DONE_ISR
#endif
  },

  { /* TRAMP_MOD_TX_FR_ISR */
    NULL,
    TRAMP_MOD_TX_FR_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_AFTER,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_1X_TX_FR_ISR,
    TRAMP_MAX_MOD_TX_FR_ISR,
    HWIO_FMSK(MOD_TX_FR_INT_EN,MOD_TX_FR_INT_CLR),
    TRAMP_MOD_TX_FR_INT_EN,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_MOD_TX_FR_ISR
#endif
  },

  { /* TRAMP_RTC_1XHDR_ISR */
    NULL,
    TRAMP_RTC_1XHDR_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_AFTER,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_COMP1_INT,
    TRAMP_MAX_RTC_ISR,
    HWIO_FMSK(RTC_INT_ENABLE,PN_ROLLOVER_1X) |
    HWIO_FMSK(RTC_INT_ENABLE,RTC_CMP1) |
    HWIO_FMSK(RTC_INT_ENABLE,RTC_CMP2),
    TRAMP_RTC_INT_ENABLE,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_RTC_1XHDR_ISR
#endif
  },

  { /* TRAMP_MOD_TX_SLOT_ISR */
    NULL,
    TRAMP_MOD_TX_SLOT_ISR_PRIO_VAL,
    TRAMP_LEVEL_DETECT,
    TRAMP_CLR_AFTER,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_125_MS_ISR,
    TRAMP_MAX_TX_SLOT_ISR,
    HWIO_FMSK(MOD_TX_SLOT_INT_EN,MOD_TX_1_25_MS_INT_CLR),
    TRAMP_MOD_TX_SLOT_INT_EN,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_MOD_TX_SLOT_ISR
#endif
  },

  { /* TRAMP_DEM_TIME_INT2_ISR */
    NULL,
    TRAMP_DEM_TIME_INT2_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0
#ifdef TIMETEST
   , TIMETEST_TRAMP_DEM_TIME_INT2_ISR
#endif
  },

  { /* TRAMP_DEM_TIME_INT1_ISR */
    NULL,
    TRAMP_DEM_TIME_INT1_ISR_PRIO_VAL,
    TRAMP_EDGE_DETECT,
    TRAMP_CLR_BEFORE,

#ifdef FEATURE_2ND_LVL_INTR
    TRAMP_NUM_INTS,
    0,
    0,
    NULL,
#endif

    (volatile uint32 *)HWIO_ADDR(IRQ_STATUS_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_EN_0),
    (volatile uint32 *)HWIO_ADDR(IRQ_CLEAR_0),
    (volatile uint32 *)HWIO_ADDR(INT_DET_REG_0),
    (volatile uint32 *)HWIO_ADDR(INT_SET_0),
    NULL,
    0

#ifdef TIMETEST
   , TIMETEST_TRAMP_DEC_TIME_INT1_ISR
#endif
  }
};

/* Table of pointers to the ISRs for combined interrupts. 
 * Use index retrieved from entry in tramp_isr_func_tbl to access.
 *
 * There must be a one-to-one correspondence between the entries in 
 * this table and the values in the tramp_isr_type enumeration from 
 * TRAMP_NUM_PIC_INTS to TRAMP_NUM_INTS.
 *
 * Within the following table there are some lines that are commented
 * out and then followed by an entry that is either zero (0) or NULL.
 * There are two types of secondary interrupts, those that use
 * interrupt controller registers to do things such as clearing,
 * enabling, etc. and those that use external registers. 
 *
 * Secondary interrupts that are not controlled using interrupt
 * controller registers will require that enabling and clearing be
 * handled by drivers for those specific hardware blocks. 
 *
 * We need to simulate the enable bit because tramp_set_sub_isr uses
 * it to keep track of whether or not any sub-interrupts are still
 * using a particular interrupt vector.  To do this, a distinct bit is
 * used in this field for every interrupt that doesn't explicitly use
 * an enable register.  The requirement is that this bit remain
 * distinct among a group sub-interrupts that use the same
 * consolidated interrupt.
 */
tramp_sub_isr_tab_type tramp_sub_isr_tbl[TRAMP_NUM_INTS - TRAMP_NUM_PIC_INTS] = 
{
  
  { /* TRAMP_RTC_COMPARE_1_ISR */
    NULL,
    TRAMP_CLR_AFTER,
    TRAMP_LEVEL_DETECT,
    TRAMP_NEG_POLARITY,
    HWIO_FMSK(RTC_INT_CLEAR, RTC_CMP1),
    HWIO_FMSK(RTC_INT_STATUS, RTC_CMP1),
    HWIO_FMSK(RTC_INT_ENABLE,RTC_CMP1),
    0,
    0,
    TRAMP_RTC_1XHDR_ISR,
    (volatile uint32 *)HWIO_ADDR(RTC_INT_CLEAR),
    (volatile uint32 *)HWIO_ADDR(RTC_INT_STATUS),
    NULL,
    NULL,
    TRAMP_RTC_INT_ENABLE,
    0,
    TRAMP_MODEM

#ifdef TIMETEST
   , TIMETEST_TRAMP_RTC_COMPARE_1_ISR
#endif
  },
  { /* TRAMP_RTC_COMPARE_2_ISR */
    NULL,
    TRAMP_CLR_AFTER,
    TRAMP_LEVEL_DETECT,
    TRAMP_NEG_POLARITY,
    HWIO_FMSK(RTC_INT_CLEAR, RTC_CMP2),
    HWIO_FMSK(RTC_INT_CLEAR, RTC_CMP2),
    HWIO_FMSK(RTC_INT_ENABLE,RTC_CMP2),
    0,
    0,
    TRAMP_RTC_1XHDR_ISR,
    (volatile uint32 *)HWIO_ADDR(RTC_INT_CLEAR),
    (volatile uint32 *)HWIO_ADDR(RTC_INT_STATUS),
    NULL,
    NULL,
    TRAMP_RTC_INT_ENABLE,
    0,
    TRAMP_MODEM

#ifdef TIMETEST
   , TIMETEST_TRAMP_RTC_COMPARE_2_ISR
#endif
  },
  { /* TRAMP_PN_ROLLOVER_ISR */
    NULL,
    TRAMP_CLR_BEFORE,
    TRAMP_LEVEL_DETECT,
    TRAMP_POS_POLARITY,
    HWIO_FMSK(RTC_INT_CLEAR,PN_ROLLOVER_1X),
    HWIO_FMSK(RTC_INT_STATUS,PN_ROLLOVER_1X),
    HWIO_FMSK(RTC_INT_ENABLE,PN_ROLLOVER_1X),
    0,
    0,
    TRAMP_RTC_1XHDR_ISR,
    (volatile uint32 *)HWIO_ADDR(RTC_INT_CLEAR), 
    (volatile uint32 *)HWIO_ADDR(RTC_INT_STATUS),
    NULL,
    NULL,
    TRAMP_RTC_INT_ENABLE, 
    0,
    TRAMP_MODEM

#ifdef TIMETEST
   , TIMETEST_TRAMP_PN_ROLLOVER_ISR
#endif
  },
  { /* TRAMP_125_MS_ISR */ 
    NULL,
    TRAMP_CLR_AFTER,
    TRAMP_LEVEL_DETECT,
    TRAMP_NEG_POLARITY,
    HWIO_FMSK(MOD_TX_SLOT_INT_CLR,MOD_TX_1_25_MS_INT_CLR),
    HWIO_FMSK(MOD_TX_SLOT_INT_STATUS,MOD_TX_1_25_MS_INT),
    HWIO_FMSK(MOD_TX_SLOT_INT_EN,MOD_TX_1_25_MS_INT_CLR),
    0,
    0,
    TRAMP_MOD_TX_SLOT_ISR,
    (volatile uint32 *)HWIO_ADDR(MOD_TX_SLOT_INT_CLR),
    (volatile uint32 *)HWIO_ADDR(MOD_TX_SLOT_INT_STATUS),
    NULL,
    NULL,
    TRAMP_MOD_TX_SLOT_INT_EN,
    0,
    TRAMP_MODEM

#ifdef TIMETEST
   , TIMETEST_TRAMP_125_MS_ISR
#endif
  },
  { /* TRAMP_1X_TX_FR_ISR */
    NULL,
    TRAMP_CLR_AFTER,
    TRAMP_LEVEL_DETECT,
    TRAMP_NEG_POLARITY,
    HWIO_FMSK(MOD_TX_FR_INT_CLR,MOD_TX_FR_INT_CLR),
    HWIO_FMSK(MOD_TX_FR_INT_STATUS,MOD_TX_FR_INT),
    HWIO_FMSK(MOD_TX_FR_INT_EN,MOD_TX_FR_INT_CLR),
    0,
    0,
    TRAMP_MOD_TX_FR_ISR,
    (volatile uint32 *)HWIO_ADDR(MOD_TX_FR_INT_CLR),
    (volatile uint32 *)HWIO_ADDR(MOD_TX_FR_INT_STATUS),
    NULL,
    NULL,
    TRAMP_MOD_TX_FR_INT_EN,
    0,
    TRAMP_MODEM

#ifdef TIMETEST
   , TIMETEST_TRAMP_1X_TX_FR_ISR
#endif
  },
  { /* TRAMP_KEY_SENSE_ISR */
    NULL,
    TRAMP_CLR_BEFORE,
    TRAMP_EDGE_DETECT,
    TRAMP_POS_POLARITY,
    HWIO_FMSK(MSM_INT_CLEAR,KEYSENSE_INT),
    HWIO_FMSK(MSM_INT_STATUS,KEYSENSE_INT),
    HWIO_FMSK(MSM_INT_EN,KEYSENSE_INT),
    HWIO_FMSK(MSM_DETECT_CTL,KEYSENSE_INT),
    HWIO_FMSK(MSM_INT_POLARITY,KEYSENSE_INT),
    TRAMP_KEYSENSE_GROUP_ISR,
    (volatile uint32 *)HWIO_ADDR(MSM_INT_CLEAR), 
    (volatile uint32 *)HWIO_ADDR(MSM_INT_STATUS),
    (volatile uint32 *)HWIO_ADDR(MSM_DETECT_CTL),
    (volatile uint32 *)HWIO_ADDR(MSM_INT_POLARITY),
    TRAMP_KEYSENSE_INT_EN, 
    0,
    TRAMP_PIC

#ifdef TIMETEST
   , TIMETEST_TRAMP_KEY_SENSE_ISR
#endif
  }
};


/*===========================================================================
 
                      PUBLIC FUNCTION DECLARATIONS
                      
===========================================================================*/

/*===========================================================================

FUNCTION     TRAMP_INIT_PROC_SPEC

DESCRIPTION
  This function disables and clears the FIQ.
  It also disables and clears all sub interrupt sources.  If the sub
  interrupt source has detection or polarity then these are set as
  level and negative as default 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tramp_init_proc_spec( void )
{
  /* Clear the FIQ Status registers */
  HWIO_OUT(FIQ_CLEAR_0, CLEAR_ALL_MASK);
  HWIO_OUT(FIQ_CLEAR_1, CLEAR_ALL_MASK);

  /* Mask all FIQ interrupts. Interrupts are masked until the corresponding
   * bit in the IRQ_ENABLE register is set.
   */
  HWIO_OUT(FIQ_EN_0, DISABLE_ALL_MASK);
  HWIO_OUT(FIQ_EN_1, DISABLE_ALL_MASK);

  /* Select '1' for 1x demod 26ms rollover interrupt */
  HWIO_OUT(RTC_INT_SELECT, 1);

} /* end tramp_init_proc_spec */


#ifdef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION     TRAMP_CLEANUP

DESCRIPTION
  This function masks all interrupts. This is to reset the interrupts to a 
  known state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All interrupts are masked
===========================================================================*/
void tramp_cleanup(void)
{
  
  INTLOCK();
  /* Mask all IRQ interrupts. Interrupts are masked until the corresponding
   * bit in the IRQ_ENABLE register is set.
   */
  HWIO_OUT(IRQ_EN_0, DISABLE_ALL_MASK);
  HWIO_OUT(IRQ_EN_1, DISABLE_ALL_MASK);

  /* Mask all FIQ interrupts. Interrupts are masked until the corresponding
   * bit in the IRQ_ENABLE register is set.
   */
  HWIO_OUT(FIQ_EN_0, DISABLE_ALL_MASK);
  HWIO_OUT(FIQ_EN_1, DISABLE_ALL_MASK);

  /* Clear the IRQ and FIQ Status registers */
  HWIO_OUT(IRQ_CLEAR_0, CLEAR_ALL_MASK);
  HWIO_OUT(IRQ_CLEAR_1, CLEAR_ALL_MASK);
  HWIO_OUT(FIQ_CLEAR_0, CLEAR_ALL_MASK);
  HWIO_OUT(FIQ_CLEAR_1, CLEAR_ALL_MASK);
  INTFREE();
}
#endif

