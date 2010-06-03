#ifndef UIM11X0DRV_H
#define UIM11X0DRV_H
/*===========================================================================

                 U I M   D E V I C E   L A Y E R   H E A D E R

DESCRIPTION
   This header file contains definitions and MACROS specific to the QSC11x0
   UIM Driver.

   Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/uimdrv.h_v   1.12   30 Jul 2002 11:07:44   gravanas  $
$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3/uim/uimdrv/src/uim11x0drv.h#2 $ $DateTime: 2009/06/17 06:12:33 $ $Author: rmandala $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/09   rm      Fixes to correct the PULL settings for UIM_DATA and 
                   UIM_CLK GPIOs
03/13/09   rm      Remove the calls to obsoleted sleep_require_5ms_rex_ticks()
                   and sleep_relinquish_5ms_rex_ticks() APIs
01/19/09   rm      Fix for slow response of RUIM cards with UART DM
01/09/09   rm      Fix for a possible update record failure with UART_DM 
09/18/08   rm      Work waiting time fix for UART_DM
08/18/08   rm      Fix for compiler warning due to a left shift
07/03/08   jk      Fix to not allow GPIO 10 to be configured for 1100
04/24/08   jk      Initial Revision
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
#if defined( FEATURE_UIM_PMIC_ON_UIM1 ) || \
    defined( FEATURE_UIM_PMIC_ON_UIM2 ) || \
    defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
#include "pm.h"
#include "pmgen.h"
#endif

#if defined (FEATURE_UIM2)
#error FEATURE_UIM2 not supported on this target
#endif /* FEATURE_UIM2 */

#define UIM_READ_RX_FIFO()           \
  HWIO_IN( GSBI0_UART_DM_RF )

#define UIM_READ_ISR()               \
  HWIO_IN( GSBI0_UART_DM_ISR )

#define UIM_READ_MISR()              \
  HWIO_IN( GSBI0_UART_DM_MISR )

#define UIM_SEND_TX_FIFO( tx_value )     \
  HWIO_OUT( GSBI0_UART_DM_TF, tx_value )

#define UIM_RESET_RX()                       \
  HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_RESET_RX );    \
  HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_RESET_RX )

#define UIM_RESET_TX()                       \
  HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_RESET_TX );    \
  HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_RESET_TX )

#define UIM_RESET_ERR()                       \
  HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_RESET_ERR )

#define UIM_RESET_BREAK()                         \
  HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_RESET_BRK_INT )

#define UIM_START_BREAK()                   \
  HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_STA_BRK )

#define UIM_STOP_BREAK()                    \
  HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_STO_BRK )

#define UIM_RESET_TX_ERROR()                     \
  HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_CLR_TX_ERROR )

#define UIM_SAMPLE_MODE()                     \
  HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_SAMP_MODE )

#define UIM_DISABLE_UART_TX()               \
  HWIO_OUT( GSBI0_UART_DM_CR,  MSMU_CR_DIS_TX )

#define UIM_ENABLE_UART_TX()                \
  HWIO_OUT( GSBI0_UART_DM_CR,  MSMU_CR_ENA_TX )

#define UIM_DISABLE_UART_RX()               \
  HWIO_OUT( GSBI0_UART_DM_CR,  MSMU_CR_DIS_RX )

#define UIM_ENABLE_UART_RX()                \
  HWIO_OUT( GSBI0_UART_DM_CR,  MSMU_CR_ENA_RX )

#define UIM_RESET_SAMPLE_MODE()                   \
  HWIO_OUT( GSBI0_UART_DM_CR,  MSMU_CR_RESET_SAMPLE )

#define UIM_PRG_UART_CSR( csr_val )     \
  HWIO_OUT( GSBI0_UART_DM_CSR, csr_val )

#define UIM_PRG_MR1( mr1_val )          \
  HWIO_OUT( GSBI0_UART_DM_MR1, mr1_val )

#define UIM_PRG_MR2( mr2_val )          \
  HWIO_OUT( GSBI0_UART_DM_MR2, mr2_val )

#define UIM_PRG_IMR( imr_val )          \
  HWIO_OUT( GSBI0_UART_DM_IMR, imr_val )

#define UIM_PRG_IPR( ipr_val )          \
  HWIO_OUT( GSBI0_UART_DM_IPR, ipr_val )

/* For UART IRDA Register */

#ifndef MSMU_IRDA_DISABLE
  #define MSMU_IRDA_DISABLE    0x00   /* Disable UART IRDA transceiver       */
#endif

#define UIM_DISABLE_IRDA()                     \
  HWIO_OUT( GSBI0_UART_DM_IRDA, MSMU_IRDA_DISABLE)

#define UIM_RX_WATERLEVEL( rfwr_val )   \
  HWIO_OUT( GSBI0_UART_DM_RFWR, rfwr_val )

#define ENABLE_UART_INTERFACE()

/* Macro to configure the UIM DATA and CLK GPIOs as PULL_DOWN */
#define CONFIGURE_UIM_DATA_AND_CLK_AS_PULL_DOWN()    \
        gpio_tlmm_config( UIM_CLK_PD );              \
        gpio_tlmm_config( UIM_DATA_PD )

/* Macro to configure the UIM DATA and CLK GPIOs as NO_PULL */
#define CONFIGURE_UIM_DATA_AND_CLK_AS_NO_PULL()   \
        gpio_tlmm_config( UIM_CLK_NP );           \
        gpio_tlmm_config( UIM_DATA_NP )

#define UIM_READ_STATUS()                  HWIO_IN( GSBI0_UART_DM_SR )

#define UIM_DISABLE_DM                     0x00

#define UART_TFWR_TFW_MASK                 HWIO_FMSK(GSBI0_UART_DM_TFWR, TFW)

/* SIM CONFIG REGISTER MASKS AND SHIFTS*/
#define UART_DM_SIM_CFG_WH__SWAP_D_MASK    HWIO_FMSK(GSBI0_UART_DM_SIM_CFG, SWAP_D)

#define UART_DM_SIM_CFG_WH__INV_D_MASK     HWIO_FMSK(GSBI0_UART_DM_SIM_CFG, INV_D)

#define UART_DM_SIM_CFG_TX_RX_MODE_MASK     (HWIO_FMSK(GSBI0_UART_DM_SIM_CFG, UIM_TX_MODE) | \
                                             HWIO_FMSK(GSBI0_UART_DM_SIM_CFG, UIM_RX_MODE))

#define UART_SIM_CFG__STOP_BIT_LEN_MASK    HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,SIM_STOP_BIT_LEN)

#define UART_SIM_CFG__STOP_BIT_LEN_SHFT    (uint8)HWIO_GSBI0_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_SHFT

#define UIM_DEFAULT_STOP_BIT_VALUE        (0x2<< ((uint8)(UART_SIM_CFG__STOP_BIT_LEN_SHFT)) )

#define UART_SIM_CFG__CLK_MASK             HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,SIM_CLK_ON) | \
                                         HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,SIM_CLK_TD8_SEL)| \
                                         HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,SIM_CLK_STOP_HIGH)

#define UART_SIM_CFG__SIM_CLK_OFF          0

#define UART_SIM_CFG__SIM_CLK_STOP_HI_MASK HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,SIM_CLK_STOP_HIGH)

#define UART_SIM_CFG__SIM_CLK_ON_MASK      HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,SIM_CLK_ON)

#define UART_SIM_CFG__SIM_CLK_TD8_SEL_MASK HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,SIM_CLK_TD8_SEL)

#define UART_SIM_CFG__SIM_CLK_TD4_SEL_MASK 0

#define UART_SIM_CFG__MASK_RX              HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,MASK_RX)

#define UART_SIM_CFG__MASK                (HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,MASK_RX) | \
                                           HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,SWAP_D)  | \
                                           HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,INV_D)   | \
                                           HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,SIM_SEL))

#define UART_SIM_CFG__SIM_SEL_MASK         HWIO_FMSK(GSBI0_UART_DM_SIM_CFG,SIM_SEL)

/* RXFS REGISTER MASKS AND SHIFTS*/

#define UART_DM_RXFS__RXFIFO_STATE_LSB_MASK HWIO_FMSK(GSBI0_UART_DM_RXFS, RX_FIFO_STATE_LSB)

#define UART_DM_RXFS__RX_BUFFER_STATE_MASK  HWIO_FMSK(GSBI0_UART_DM_RXFS, RX_BUFFER_STATE)

#define UART_DM_RXFS__RXFIFO_STATE_LSB_SHFT HWIO_SHFT(GSBI0_UART_DM_RXFS, RX_FIFO_STATE_LSB)

#define UART_DM_RXFS__RX_BUFFER_STATE_SHFT  HWIO_SHFT(GSBI0_UART_DM_RXFS, RX_BUFFER_STATE)

/* TXFS REGISTER MASKS AND SHIFTS*/

#define UART_DM_TXFS__TXFIFO_STATE_LSB_MASK HWIO_FMSK(GSBI0_UART_DM_TXFS, TX_FIFO_STATE_LSB)

#define UART_DM_TXFS__TX_BUFFER_STATE_MASK  HWIO_FMSK(GSBI0_UART_DM_TXFS, TX_BUFFER_STATE)

#define UART_DM_TXFS__TXFIFO_STATE_LSB_SHFT HWIO_SHFT(GSBI0_UART_DM_TXFS, TX_FIFO_STATE_LSB)

#define UART_DM_TXFS__TX_BUFFER_STATE_SHFT  HWIO_SHFT(GSBI0_UART_DM_TXFS, TX_BUFFER_STATE)

/* Holds the MPP voltage reference on the SIM side */
LOCAL pm_mpp_dlogic_lvl_type uim_pmic_sim_side_ref;

#define UIM_SET_POWER_AS_OUTPUT()

#define UIM_POWER_OFF()                   (void)pm_vreg_control( PM_OFF_CMD, (uint32)PM_VREG_RUIM_M )

#define UIM_POWER_ON()                    (void)pm_vreg_control( PM_ON_CMD, (uint32)PM_VREG_RUIM_M )

#define UIM_DISABLE_DATA_MOVER()           HWIO_OUT( GSBI0_UART_DM_DMEN, UIM_DISABLE_DM)

#define UIM_DM_RX_INIT( rx_chars )        HWIO_OUT(GSBI0_UART_DM_DMRX, rx_chars)

#ifdef  FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG
  #define UART_SIM_CFG_TX_RX_MODE_MASK (HWIO_FMSK(GSBI0_UART_DM_SIM_CFG, UIM_TX_MODE) | \
                                        HWIO_FMSK(GSBI0_UART_DM_SIM_CFG, UIM_RX_MODE))
#else
  #define UART_TFWR_TX_RX_MODE_MASK (HWIO_FMSK(GSBI0_UART_DM_TFWR, UIM_TX_MODE) | \
                                     HWIO_FMSK(GSBI0_UART_DM_TFWR, UIM_RX_MODE))
#endif /* FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG */

#define UIM_TX_WATERLEVEL( tfwr_val )                                 \
  INTLOCK( );                                                         \
  uim_tfwr_shadow[UIM_DRV_SLOT1] =                                    \
    (uim_tfwr_shadow[UIM_DRV_SLOT1] & (word)~(UART_TFWR_TFW_MASK)) |  \
                         ((word)((tfwr_val) & (UART_TFWR_TFW_MASK))); \
  HWIO_OUT( GSBI0_UART_DM_TFWR, uim_tfwr_shadow[UIM_DRV_SLOT1] );             \
  INTFREE( )

#ifdef FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG

  #define UIM_UART_CNFG( cnfg_mask, cnfg_val )                  \
    INTLOCK( );                                                 \
    uim_uart_shadow[UIM_DRV_SLOT1] =                            \
      (uim_uart_shadow[UIM_DRV_SLOT1] & (dword)~(cnfg_mask)) |  \
                           ((dword)((cnfg_val) & (cnfg_mask))); \
    HWIO_OUT( GSBI0_UART_DM_SIM_CFG, uim_uart_shadow[UIM_DRV_SLOT1] );  \
    INTFREE( )

#else

  #define UIM_UART_CNFG( cnfg_mask, cnfg_val )                  \
    INTLOCK( );                                                 \
    uim_uart_shadow[UIM_DRV_SLOT1] =                            \
      (uim_uart_shadow[UIM_DRV_SLOT1] & (word)~(cnfg_mask)) |   \
                           ((word)((cnfg_val) & (cnfg_mask)));  \
    HWIO_OUT( GSBI0_UART_DM_SIM_CFG, uim_uart_shadow[UIM_DRV_SLOT1] );  \
    INTFREE( )

#endif /* FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG */

#ifndef FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG 

  #define UIM_BLOCK_MODE( )                                                 \
    INTLOCK( );                                                             \
    uim_tfwr_shadow[UIM_DRV_SLOT1] |= (UART_TFWR_TX_RX_MODE_MASK);          \
    HWIO_OUT( GSBI0_UART_DM_TFWR, uim_tfwr_shadow[UIM_DRV_SLOT1] );                 \
    INTFREE( )
#endif /* !FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG */

#define UIM_SET_RESET_AS_OUTPUT()

#define UIM_ASSERT_RESET()             gpio_out(UIM_RESET,GPIO_LOW_VALUE);

#define UIM_DEASSERT_RESET()           gpio_out(UIM_RESET,GPIO_HIGH_VALUE);

#define UIM_PRG_UART_BADR(badr_val)     HWIO_OUT(GSBI0_UART_DM_BADR, badr_val)

#define UIM_GET_RX_WORD()               HWIO_IN( GSBI0_UART_DM_RF )

#define UIM_GET_NUM_BYTES_IN_RX()       HWIO_IN( GSBI0_UART_DM_RX_TOTAL_SNAP)

/* Gets the number of bytes that are yet to be read from RX FIFO and the
   packing buffer */
#define UIM_GET_NUM_BYTES_WAITING_IN_RX() HWIO_IN(GSBI0_UART_DM_RXFS)

/* Gets the number of bytes that are yet to be sent from TX FIFO and the
    un-packing buffer */
#define UIM_GET_NUM_BYTES_WAITING_IN_TX() HWIO_IN(GSBI0_UART_DM_TXFS)

#define UIM_NUM_CHARS_TO_SEND( tx_num ) HWIO_OUT( GSBI0_UART_DM_NO_CHARS_FOR_TX, tx_num)

#define UIM_ENABLE_RX_STALE_INT()       HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_ENA_STALE_EVT)

#define UIM_CLEAR_RX_STALE_INT()        HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_CLEAR_STALE_EVT)

#define UIM_FORCE_RX_STALE_INT()        HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_FORCE_STALE_EVT)

#define UIM_CLEAR_TX_READY_INT()        HWIO_OUT( GSBI0_UART_DM_CR, MSMU_CR_CLR_TX_READY)

/* The appropriate clock selection would be done by clk_regime at 
* initialization itself
*/
#define UIM_SELECT_CLOCK_REGIME_SOURCE_1()

#define UIM_SELECT_CLOCK_REGIME_SOURCE_2()

#define UIM_CLOCK_REGIME_ENABLE_1()                                          \
clk_regime_msm_enable(CLK_RGM_UART0_M)

#define UIM_CLOCK_REGIME_DISABLE_1()                                         \
clk_regime_msm_disable(CLK_RGM_UART0_M) 

/*===========================================================================

MACRO UIM_SLEEP_ASSERT_OKTS
MACRO UIM_SLEEP_NEGATE_OKTS

DESCRIPTION
  Indicates sleep voting

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_UIM_TCXO_SLEEP
#if defined(FEATURE_NEW_SLEEP_API) && defined(FEATURE_UIM_USES_NEW_SLEEP_API)
#define UIM_SLEEP_ASSERT_OKTS() sleep_assert_okts(uim_sleep_okts_handle)

  #define UIM_SLEEP_NEGATE_OKTS() sleep_negate_okts(uim_sleep_okts_handle)
#else
  #define UIM_SLEEP_ASSERT_OKTS() SLEEP_ASSERT_OKTS(SLEEP_UIM_OKTS_SIG)

  #define UIM_SLEEP_NEGATE_OKTS() SLEEP_NEGATE_OKTS(SLEEP_UIM_OKTS_SIG)
#endif /* FEATURE_NEW_SLEEP_API && FEATURE_UIM_USES_NEW_SLEEP_API */
#endif /* FEATURE_UIM_TCXO_SLEEP */

  #define UIM_TCXO_MUST_BE_ON()                 \
    INTLOCK();                                  \
    UIM_CLOCK_REGIME_ENABLE_1();                \
    uim_clock_is_on = TRUE;                     \
    INTFREE();                                  \
    UIM_SLEEP_NEGATE_OKTS();

    #define UIM_TCXO_CAN_BE_OFF()                 \
      INTLOCK();                                  \
      UIM_CLOCK_REGIME_DISABLE_1();               \
      uim_clock_is_on = FALSE;                    \
      INTFREE();                                  \
      UIM_SLEEP_ASSERT_OKTS();

#if defined( FEATURE_UIM_VOLTAGE_TRANSLATOR )
    #define UIM_1_8V_PROG       0x0000
    #define UIM_3V_PROG         0x0001

    #define UIM_PROG_VOLTAGE_CLASS( voltage_ctl ) \
        outp( 0x02010002C, voltage_ctl )

#elif defined( FEATURE_UIM_PMIC_ON_UIM1 ) || \
        defined( FEATURE_UIM_PMIC_ON_UIM2 ) || \
        defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
    /* This value represents the desired voltage in mV */
    #define UIM_1_8V_PROG       1800
#if defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
#if defined( FEATURE_UIM_DRIVE_MAX_PMIC_VOLTAGE ) 
	#define UIM_3V_PROG         3050
#else 
    #define UIM_3V_PROG         2850
#endif /* FEATURE_UIM_DRIVE_MAX_PMIC_VOLTAGE */
#else
#if defined( FEATURE_UIM_DRIVE_MAX_PMIC_VOLTAGE ) 
	#define UIM_3V_PROG         3050
#else 
    #define UIM_3V_PROG         3000
#endif /* FEATURE_UIM_DRIVE_MAX_PMIC_VOLTAGE */
#endif /* FEATURE_UIM_USE_ONLY_VREG_ON_PMIC  */

         #define UIM_PROG_VOLTAGE_CLASS( voltage_ctl ) \
          (void)pm_vreg_set_level( PM_VREG_RUIM_ID, voltage_ctl )

#endif /* FEATURE_UIM_VOLTAGE_TRANSLATOR */

#if defined( FEATURE_UIM_SUPPORT_INVERSE_CONVENTION )

  #define UIM_PROGRAM_INVERSE_CONVENTION(  )                    \
    UIM_UART_CNFG( UART_DM_SIM_CFG_WH__SWAP_D_MASK |              \
                   UART_DM_SIM_CFG_WH__INV_D_MASK,                \
                   UART_DM_SIM_CFG_WH__SWAP_D_MASK |              \
                   UART_DM_SIM_CFG_WH__INV_D_MASK );              \
    UIM_PRG_MR2 ( MSMU_MR2_8BPC | MSMU_MR2_2SB | MSMU_MR2_OPAR);\
    uim_convention[uim_drv_slot] = UIM_INVERSE_CONVENTION

  #define UIM_PROGRAM_DIRECT_CONVENTION(  )                     \
    UIM_UART_CNFG( UART_DM_SIM_CFG_WH__SWAP_D_MASK |              \
                   UART_DM_SIM_CFG_WH__INV_D_MASK,                \
                   UART_SIM_CFG__SIM_CLK_OFF );                 \
    UIM_PRG_MR2 ( MSMU_MR2_8BPC | MSMU_MR2_2SB | MSMU_MR2_EPAR);\
    uim_convention[uim_drv_slot] = UIM_DIRECT_CONVENTION
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION */

#define CR_FVAL(reg, field, val) \
        ( HWIO_FVAL(reg, field, val) << HWIO_SHFT(reg, field) )

#undef UIM_NO_SIM_GLUE_LOGIC

#define UIM_PROCESS_PARITY_ERROR_RETURN() return

#define UIM_PROCESS_PARITY_ERROR_CONT() continue

#define UIM_GET_RX_BYTE()  ( UIM_READ_RX_FIFO() & 0xFF )

#if !defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
  #define UIM_DRIVE_LINES_LOW()                                         \
    (void)pm_mpp_config_digital_output(PM_MPP_6,                        \
                                       PM_MPP__DLOGIC__LVL_MSMP,        \
                                       PM_MPP__DLOGIC_OUT__CTRL_LOW);   \
    (void)pm_mpp_config_digital_output(PM_MPP_10,                       \
                                       PM_MPP__DLOGIC__LVL_MSMP,        \
                                       PM_MPP__DLOGIC_OUT__CTRL_LOW);   \
    (void)pm_mpp_config_digital_output(PM_MPP_12,                       \
                                       PM_MPP__DLOGIC__LVL_MSMP,        \
                                       PM_MPP__DLOGIC_OUT__CTRL_LOW)
#else
  #define UIM_DRIVE_LINES_LOW()
#endif /* !FEATURE_UIM_USE_ONLY_VREG_ON_PMIC */

#define UIM_TRAMP_SET_ISR()   tramp_set_isr( TRAMP_UART0_ISR, uim_rx_isr_1)

/* There is no PWR Disable line on QSC11X0 */
#define UIM_UIMDRV_PWR_DIS()

/*===========================================================================

FUNCTION uim_bytes_waiting_in_txfifo

DESCRIPTION
  This procedure determines how many bytes are waiting to be sent in the
  TX FIFO by querying the TXFS register.

DEPENDENCIES
  None

RETURN VALUE
  Number of byte present in the RX FIFO which are yet to be read.

SIDE EFFECTS
  None

===========================================================================*/

extern int32 uim_bytes_waiting_in_txfifo(void);

#endif /* UIM11X0DRV_H */
