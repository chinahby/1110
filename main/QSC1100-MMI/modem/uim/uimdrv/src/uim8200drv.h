#ifndef UIM8200DRV_H
#define UIM8200DRV_H
/*===========================================================================

                 U I M   D E V I C E   L A Y E R   H E A D E R

DESCRIPTION
   This header file contains definitions and MACROS specific to the MDM8200
   UIM Driver.

   Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/uimdrv.h_v   1.12   30 Jul 2002 11:07:44   gravanas  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uim8200drv.h#2 $ $DateTime: 2009/03/17 02:58:52 $ $Author: rmandala $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/09   rm      Remove the calls to obsoleted sleep_require_5ms_rex_ticks()
                   and sleep_relinquish_5ms_rex_ticks() APIs
02/25/09   yk      Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "msm.h"

#include "clk.h"

#include "bio.h"                   /* Bit I/O macros                       */
#include "clkregim.h"              /* Clock Regime Definitions             */
#include "err.h"                   /* Error handling utilities             */
#include "msg.h"                   /* Message utilities                    */
#include "uimdrv.h"                /* UIM driver header file               */
#include "uimi.h"                  /* UIM Internal header file             */
#if defined( FEATURE_UIM_T_1_SUPPORT )
#include "crc.h"
#include "uimt1.h"                 /* T=1 protocol support                 */
#endif /* FEATURE_UIM_T_1_SUPPORT */
#include "hw.h"                    /* Target definitions */

#ifdef FEATURE_UIM_UTIL
#include "uimutil.h"
#endif /* FEATURE_UIM_UTIL */

#include "sleep.h"
#include "pm.h"

/* For UART IRDA Register */
#ifndef MSMU_IRDA_DISABLE
  #define MSMU_IRDA_DISABLE    0x00   /* Disable UART IRDA transceiver       */
#endif

#if defined (FEATURE_UIM2)
#error FEATURE_UIM2 not supported on this target
#endif /* FEATURE_UIM2 */

#if defined( FEATURE_UIM_VOLTAGE_TRANSLATOR )
#error FEATURE_UIM_VOLTAGE_TRANSLATOR not supported on this target
#endif

#if !defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
#error only FEATURE_UIM_USE_ONLY_VREG_ON_PMIC supported on this target
#endif /* FEATURE_UIM_USE_ONLY_VREG_ON_PMIC */

#define UIM_SELECT_CLOCK_REGIME_SOURCE_1()

#define UIM_CLOCK_REGIME_ENABLE_1()                                            \
    clk_regime_resource_enable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM1)

#define UIM_CLOCK_REGIME_DISABLE_1()                                           \
    clk_regime_resource_disable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM1)

#ifdef FEATURE_UIM_TCXO_SLEEP
  #if defined(FEATURE_NEW_SLEEP_API) && defined(FEATURE_UIM_USES_NEW_SLEEP_API)
    #define UIM_SLEEP_ASSERT_OKTS() sleep_assert_okts(uim_sleep_okts_handle)
    #define UIM_SLEEP_NEGATE_OKTS() sleep_negate_okts(uim_sleep_okts_handle)
  #else
    #define UIM_SLEEP_ASSERT_OKTS() SLEEP_ASSERT_OKTS(SLEEP_UIM_OKTS_SIG)
    #define UIM_SLEEP_NEGATE_OKTS() SLEEP_NEGATE_OKTS(SLEEP_UIM_OKTS_SIG)
  #endif /* FEATURE_NEW_SLEEP_API && FEATURE_UIM_USES_NEW_SLEEP_API */

  #ifdef FEATURE_UIM_CONTROL_UIM_UART_CLK_REGIME
    #define UIM_TCXO_MUST_BE_ON()                 \
      INTLOCK();                                  \
      UIM_CLOCK_REGIME_ENABLE_1();                \
      uim_clock_is_on = TRUE;                     \
      INTFREE();                                  \
      UIM_SLEEP_NEGATE_OKTS()

    #define UIM_TCXO_CAN_BE_OFF()                 \
      INTLOCK();                                  \
      UIM_CLOCK_REGIME_DISABLE_1();               \
      uim_clock_is_on = FALSE;                    \
      INTFREE();                                  \
      UIM_SLEEP_ASSERT_OKTS()

  #else
    #define UIM_TCXO_MUST_BE_ON()                   \
      INTLOCK();                                    \
      uim_clock_is_on = TRUE;                       \
      UIM_SLEEP_NEGATE_OKTS();                      \
      INTFREE()

    #define UIM_TCXO_CAN_BE_OFF()                   \
      INTLOCK();                                    \
      uim_clock_is_on = FALSE;                      \
      UIM_SLEEP_ASSERT_OKTS();                      \
      INTFREE()
  #endif /* FEATURE_UIM_CONTROL_UIM_UART_CLK_REGIME */
#else
  #define UIM_TCXO_MUST_BE_ON()

  #define UIM_TCXO_CAN_BE_OFF()

#endif /* FEATURE_UIM_TCXO_SLEEP */

/*
  Re-define the constants for programming the UART configuration register
  to support the chips with SIM Glue Logic
*/
#define UART2_SIM_CFG_WH__SWAP_D_MASK \
        HWIO_FMSK(UART2_SIM_CFG, SWAP_D)
#define UART2_SIM_CFG_WH__INV_D_MASK \
        HWIO_FMSK(UART2_SIM_CFG, INV_D)
#define UART3_SIM_CFG_WH__SWAP_D_MASK \
        HWIO_FMSK(UART3_USIM_CFG, SWAP_D)
#define UART3_SIM_CFG_WH__INV_D_MASK \
        HWIO_FMSK(UART3_USIM_CFG, INV_D)

#ifdef  FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG
  #define UART_SIM_CFG_TX_RX_MODE_MASK (HWIO_FMSK(UART2_SIM_CFG, UIM_TX_MODE) | \
                                        HWIO_FMSK(UART2_SIM_CFG, UIM_RX_MODE))
#else
  #define UART_TFWR_TX_RX_MODE_MASK (HWIO_FMSK(UART2_TFWR, UIM_TX_MODE) | \
                                   HWIO_FMSK(UART2_TFWR, UIM_RX_MODE))
#endif /* FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG */

#define UART_TFWR_TFW_MASK HWIO_FMSK(UART2_TFWR, TFW)

#define UART_SIM_CFG__STOP_BIT_LEN_MASK \
        HWIO_FMSK(UART2_SIM_CFG,SIM_STOP_BIT_LEN)
#define UART_SIM_CFG__STOP_BIT_LEN_SHFT \
        HWIO_SHFT(UART2_SIM_CFG,SIM_STOP_BIT_LEN)
#define UIM_DEFAULT_STOP_BIT_VALUE (2<<UART_SIM_CFG__STOP_BIT_LEN_SHFT)

#define UART_SIM_CFG__CLK_MASK (HWIO_FMSK(UART2_SIM_CFG,SIM_CLK_ON)        | \
                                HWIO_FMSK(UART2_SIM_CFG,SIM_CLK_TD8_SEL)   | \
                                HWIO_FMSK(UART2_SIM_CFG,SIM_CLK_STOP_HIGH))

#define UART_SIM_CFG__SIM_CLK_OFF 0
#define UART_SIM_CFG__SIM_CLK_STOP_HI_MASK \
        HWIO_FMSK(UART2_SIM_CFG,SIM_CLK_STOP_HIGH)
#define UART_SIM_CFG__SIM_CLK_ON_MASK \
        HWIO_FMSK(UART2_SIM_CFG,SIM_CLK_ON)
#define UART_SIM_CFG__SIM_CLK_TD8_SEL_MASK \
        HWIO_FMSK(UART2_SIM_CFG,SIM_CLK_TD8_SEL)
#define UART_SIM_CFG__SIM_CLK_TD4_SEL_MASK \
        0
#define UART_SIM_CFG__MASK_RX HWIO_FMSK(UART2_SIM_CFG,MASK_RX)

#define UART_SIM_CFG__MASK (HWIO_FMSK(UART2_SIM_CFG,MASK_RX) | \
                            HWIO_FMSK(UART2_SIM_CFG,SWAP_D)  | \
                            HWIO_FMSK(UART2_SIM_CFG,INV_D)   | \
                            HWIO_FMSK(UART2_SIM_CFG,SIM_SEL))
#define UART_SIM_CFG__SIM_SEL_MASK HWIO_FMSK(UART2_SIM_CFG,SIM_SEL)
/* Define clock control constants for SIM Glue Logic chips */

#define UIM_DRIVE_LINES_LOW()

#define UIM_CONFIG_UIM_LINES()

/*
 * These macros are used to control the power enable line
 * of the UIM.
 */
#define UIM_SET_POWER_AS_OUTPUT() \
  gpio_tlmm_config_remote(GPIO_OUT_37);
#define UIM_POWER_OFF()                                               \
  UIM_DRIVE_LINES_LOW();                                              \
  gpio_out(GPIO_OUT_37, GPIO_LOW_VALUE);                              \
  (void)pm_vreg_control( PM_OFF_CMD, PM_VREG_GP3_M )
#define UIM_POWER_ON()                                                \
  (void)pm_vreg_control( PM_ON_CMD, PM_VREG_RUIM_M);                  \
  (void)pm_vreg_control( PM_ON_CMD, PM_VREG_GP3_M );                  \
  gpio_out(GPIO_OUT_37, GPIO_HIGH_VALUE);                             \
  UIM_CONFIG_UIM_LINES();

/*
 * These macros are used to control the reset line
 * of the UIM.
 */
#define UIM_SET_RESET_AS_OUTPUT()                                     \
  gpio_tlmm_config_remote(UIM_RESET);
#define UIM_ASSERT_RESET()                                            \
  gpio_out(UIM_RESET,GPIO_LOW_VALUE);
#define UIM_DEASSERT_RESET()                                          \
  gpio_out(UIM_RESET,GPIO_HIGH_VALUE);

/*Macro to read the status of the UART register */
#define UIM_READ_STATUS()            \
  HWIO_IN( UART2_SR )

/*
 * These macros control the UART interface lines.
 */
#define MSMU_ENA             0xffff /* Value to be used in enabling.
                                       Should be used with masks.          */
#define MSMU_DIS             0x0000 /* Value to be used in disabling.
                                       Should be used with masks.          */

#define ENABLE_UART_INTERFACE()      \
  gpio_tlmm_config_remote( UART2_UIM1_CLK );      \
  gpio_tlmm_config_remote( UART2_UIM1_DATA )

#define UIM_READ_RX_FIFO()           \
  HWIO_IN( UART2_RF )

#define UIM_READ_ISR()               \
  HWIO_IN( UART2_ISR )

#define UIM_READ_MISR()              \
  HWIO_IN( UART2_MISR )

#define UIM_SEND_TX_FIFO( tx_value )     \
  HWIO_OUT( UART2_TF, tx_value )

#define UIM_RESET_RX()                       \
  HWIO_OUT( UART2_CR, MSMU_CR_RESET_RX );    \
  HWIO_OUT( UART2_CR, MSMU_CR_RESET_RX )

#define UIM_RESET_TX()                       \
  HWIO_OUT( UART2_CR, MSMU_CR_RESET_TX );    \
  HWIO_OUT( UART2_CR, MSMU_CR_RESET_TX )

#define UIM_RESET_ERR()                       \
  HWIO_OUT( UART2_CR, MSMU_CR_RESET_ERR )

#define UIM_RESET_BREAK()                         \
  HWIO_OUT( UART2_CR, MSMU_CR_RESET_BRK_INT )

#define UIM_START_BREAK()                   \
  HWIO_OUT( UART2_CR, MSMU_CR_STA_BRK )

#define UIM_STOP_BREAK()                    \
  HWIO_OUT( UART2_CR, MSMU_CR_STO_BRK )

#define UIM_RESET_TX_ERROR()                     \
  HWIO_OUT( UART2_CR, MSMU_CR_CLR_TX_ERROR )

#define UIM_SAMPLE_MODE()                     \
  HWIO_OUT( UART2_CR, MSMU_CR_SAMP_MODE )

#define UIM_DISABLE_UART_TX()               \
  HWIO_OUT( UART2_CR,  MSMU_CR_DIS_TX )

#define UIM_ENABLE_UART_TX()                \
  HWIO_OUT( UART2_CR,  MSMU_CR_ENA_TX )

#define UIM_DISABLE_UART_RX()               \
  HWIO_OUT( UART2_CR,  MSMU_CR_DIS_RX )

#define UIM_ENABLE_UART_RX()                \
  HWIO_OUT( UART2_CR,  MSMU_CR_ENA_RX )

#define UIM_RESET_SAMPLE_MODE()                   \
  HWIO_OUT( UART2_CR,  MSMU_CR_RESET_SAMPLE )

#define UIM_PRG_UART_CSR( csr_val )     \
  HWIO_OUT( UART2_CSR, csr_val )

#define UIM_PRG_MR1( mr1_val )          \
  HWIO_OUT( UART2_MR1, mr1_val )

#define UIM_PRG_MR2( mr2_val )          \
  HWIO_OUT( UART2_MR2, mr2_val )

#define UIM_PRG_IMR( imr_val )          \
  HWIO_OUT( UART2_IMR, imr_val )

#define UIM_PRG_IPR( ipr_val )          \
  HWIO_OUT( UART2_IPR, ipr_val )

#define UIM_DISABLE_IRDA()                     \
  HWIO_OUT( UART2_IRDA, MSMU_IRDA_DISABLE)

#define UIM_RX_WATERLEVEL( rfwr_val )   \
  HWIO_OUT( UART2_RFWR, rfwr_val )

#define UIM_TX_WATERLEVEL( tfwr_val )                                 \
  INTLOCK( );                                                         \
  uim_tfwr_shadow[UIM_DRV_SLOT1] =                                    \
    (uim_tfwr_shadow[UIM_DRV_SLOT1] & (word)~(UART_TFWR_TFW_MASK)) |  \
                         ((word)((tfwr_val) & (UART_TFWR_TFW_MASK))); \
  HWIO_OUT( UART2_TFWR, uim_tfwr_shadow[UIM_DRV_SLOT1] );             \
  INTFREE( )

#ifdef FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG
  #define UIM_UART_CNFG( cnfg_mask, cnfg_val )                  \
    INTLOCK( );                                                 \
    uim_uart_shadow[UIM_DRV_SLOT1] =                            \
      (uim_uart_shadow[UIM_DRV_SLOT1] & (dword)~(cnfg_mask)) |  \
                           ((dword)((cnfg_val) & (cnfg_mask))); \
    HWIO_OUT( UART2_SIM_CFG, uim_uart_shadow[UIM_DRV_SLOT1] );  \
    INTFREE( )
#else
  #define UIM_UART_CNFG( cnfg_mask, cnfg_val )                  \
    INTLOCK( );                                                 \
    uim_uart_shadow[UIM_DRV_SLOT1] =                            \
      (uim_uart_shadow[UIM_DRV_SLOT1] & (word)~(cnfg_mask)) |   \
                           ((word)((cnfg_val) & (cnfg_mask)));  \
    HWIO_OUT( UART2_SIM_CFG, uim_uart_shadow[UIM_DRV_SLOT1] );  \
    INTFREE( )
  #define UIM_BLOCK_MODE( )                                                 \
    INTLOCK( );                                                             \
    uim_tfwr_shadow[UIM_DRV_SLOT1] |= (UART_TFWR_TX_RX_MODE_MASK);          \
    HWIO_OUT( UART2_TFWR, uim_tfwr_shadow[UIM_DRV_SLOT1] );                 \
    INTFREE( )
#endif /* FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG */


/* This value represents the desired voltage in mV */
#define UIM_1_8V_PROG       1800

#if defined( FEATURE_UIM_DRIVE_MAX_PMIC_VOLTAGE )
  #define UIM_3V_PROG         3050
#else
  #define UIM_3V_PROG         2850
#endif /* FEATURE_UIM_DRIVE_MAX_PMIC_VOLTAGE */

#define UIM_PROG_VOLTAGE_CLASS( voltage_ctl )                    \
            (void) pm_vreg_set_level( PM_VREG_GP3_ID,  voltage_ctl );     \
            (void) pm_vreg_set_level( PM_VREG_RUIM_ID, voltage_ctl )

#if defined( FEATURE_UIM_SUPPORT_INVERSE_CONVENTION )

  #define UIM_PROGRAM_INVERSE_CONVENTION(  )                    \
    UIM_UART_CNFG( UART2_SIM_CFG_WH__SWAP_D_MASK |              \
                   UART2_SIM_CFG_WH__INV_D_MASK,                \
                   UART2_SIM_CFG_WH__SWAP_D_MASK |              \
                   UART2_SIM_CFG_WH__INV_D_MASK );              \
    UIM_PRG_MR2 ( MSMU_MR2_8BPC | MSMU_MR2_2SB | MSMU_MR2_OPAR);\
    uim_convention[uim_drv_slot] = UIM_INVERSE_CONVENTION

  #define UIM_PROGRAM_DIRECT_CONVENTION(  )                     \
    UIM_UART_CNFG( UART2_SIM_CFG_WH__SWAP_D_MASK |              \
                   UART2_SIM_CFG_WH__INV_D_MASK,                \
                   UART_SIM_CFG__SIM_CLK_OFF );                 \
    UIM_PRG_MR2 ( MSMU_MR2_8BPC | MSMU_MR2_2SB | MSMU_MR2_EPAR);\
    uim_convention[uim_drv_slot] = UIM_DIRECT_CONVENTION
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION */

#undef UIM_NO_SIM_GLUE_LOGIC

#define UIM_PROCESS_PARITY_ERROR_RETURN(  ) return

#define UIM_PROCESS_PARITY_ERROR_CONT(  ) continue

#define UIM_GET_RX_BYTE()  ( UIM_READ_RX_FIFO() & 0xFF )

#define UIM_TRAMP_SET_ISR() tramp_set_isr( TRAMP_UART2_ISR, uim_rx_isr_1);

#define UIM_UIMDRV_PWR_DIS()

#endif /* UIM8200DRV_H */
