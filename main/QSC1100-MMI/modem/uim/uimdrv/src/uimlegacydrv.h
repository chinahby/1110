#ifndef UIMLEGACYDRV_H
#define UIMLEGACYDRV_H
/*===========================================================================

                 U I M   D E V I C E   L A Y E R   H E A D E R

DESCRIPTION
   This header file contains definitions and MACROS used in UIM Driver for all
   targets that do not have a target specific driver header file.  All new targets
   should create their own header file instead of using this one.
   UIM Driver.

   Copyright (c) 2008-2009 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/uimdrv.h_v   1.12   30 Jul 2002 11:07:44   gravanas  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimlegacydrv.h#6 $ $DateTime: 2009/03/25 00:48:02 $ $Author: suberoy $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/09   sun     Added support for Virtuim
03/13/09   rm      Remove the calls to obsoleted sleep_require_5ms_rex_ticks()
                   and sleep_relinquish_5ms_rex_ticks() APIs
01/16/09   rm      Fix for UIM RX ISR stuck in an infinite loop leading to 
                   crash
01/13/09   jk      Support for VREG Control with FET
01/09/09   rm      Fix for a possible update record failure with UART_DM 
10/10/08   nb      Lint Fixes
09/18/08   rm      WWT fix for UART_DM
09/15/08   rm      UIM powerup sequence correction for UART_DM
09/03/08   rm      Fix for improper featurization introduced due to power 
                   optimization fix for QSC60X5
08/28/08   rm      Pull low the UIM_PWR_DIS line during powerup to fix the 3V
                   problem
08/01/08   rm      Added macros to read the bytes waiting in FIFO and packing
                   buffer
06/30/08   rm      Fix to not disable the VREG_RUIM line and pull high the 
                   UIM_PWR_DIS line to save power when UIM is powered down. 
07/03/08   jk      Fix to not allow GPIO 10 to be configured for 1100
07/1/08    jk      Fixed MACRO merge from old uimdrv.c file
04/24/08   jk      Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifndef FEATURE_UIM_QTF
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

/* For UART IRDA Register */

#ifndef MSMU_IRDA_DISABLE
  #define MSMU_IRDA_DISABLE    0x00   /* Disable UART IRDA transceiver       */
#endif

#if defined (FEATURE_UIM2)
#error FEATURE_UIM2 not supported on this target
#endif /* FEATURE_UIM2 */
#ifdef T_QSC6270
  #define UIM_SELECT_CLOCK_REGIME_SOURCE_1()                               \
    clk_regime_sel_uart2dm_clk(CLKRGM_UARTDM_SPEED_3P84_MHZ);
  #define UIM_CLOCK_REGIME_ENABLE_1()                                      \
    UIM_SELECT_CLOCK_REGIME_SOURCE_1();                                    \
    clk_regime_enable(CLK_RGM_UART2_DM_M)
  #define UIM_CLOCK_REGIME_DISABLE_1()                                     \
    clk_regime_disable(CLK_RGM_UART2_DM_M)
#elif defined(T_MSM8650A)
  /* The appropriate clock selection would be done by clk_regime at
   * initialization itself
   */
  #define UIM_SELECT_CLOCK_REGIME_SOURCE_1()
  #define UIM_SELECT_CLOCK_REGIME_SOURCE_2()
  #define UIM_CLOCK_REGIME_ENABLE_1()                                          \
    clk_regime_msm_enable(CLK_RGM_UART2_M)
  #define UIM_CLOCK_REGIME_ENABLE_2()                                          \
    clk_regime_resource_enable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM2)
  #define UIM_CLOCK_REGIME_DISABLE_1()                                         \
    clk_regime_msm_disable(CLK_RGM_UART2_M)
  #define UIM_CLOCK_REGIME_DISABLE_2()                                         \
    clk_regime_resource_disable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM2)
#elif defined(T_MSM7600)
  /* The appropriate clock selection would be done by clk_regime at 
   * initialization itself
   */
  #define UIM_SELECT_CLOCK_REGIME_SOURCE_1()
  #define UIM_SELECT_CLOCK_REGIME_SOURCE_2()
  #define UIM_CLOCK_REGIME_ENABLE_1()                                          \
    clk_regime_resource_enable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM1) 
  #define UIM_CLOCK_REGIME_ENABLE_2()                                          \
    clk_regime_resource_enable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM2) 
  #define UIM_CLOCK_REGIME_DISABLE_1()                                         \
    clk_regime_resource_disable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM1) 
  #define UIM_CLOCK_REGIME_DISABLE_2()                                         \
    clk_regime_resource_disable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM2) 
#elif defined(T_MSM7500)
  #define UIM_SELECT_CLOCK_REGIME_SOURCE_1()                                   \
    clk_regime_msm_sel_clk_src(CLKRGM_UART2_CLK, CLKRGM_UART_CLK_SRC_GSM_DIV2)
  #define UIM_SELECT_CLOCK_REGIME_SOURCE_2()                                   \
    clk_regime_msm_sel_clk_src(CLKRGM_UART3_CLK, CLKRGM_UART_CLK_SRC_GSM_DIV2)
  #ifdef FEATURE_CLKREGIM_RM
    #define UIM_CLOCK_REGIME_ENABLE_1()                                          \
      clk_regime_resource_enable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM1) 
    #define UIM_CLOCK_REGIME_ENABLE_2()                                          \
      clk_regime_resource_enable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM2) 
    #define UIM_CLOCK_REGIME_DISABLE_1()                                         \
      clk_regime_resource_disable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM1) 
    #define UIM_CLOCK_REGIME_DISABLE_2()                                         \
      clk_regime_resource_disable(CLKRGM_CLIENT_UIM, CLKRGM_RESOURCE_UIM2) 
  #else
    #define UIM_CLOCK_REGIME_ENABLE_1()                                          \
      clk_regime_enable(CLKRGM_GSM_CLK) 
    #define UIM_CLOCK_REGIME_ENABLE_2()                                          \
      clk_regime_enable(CLKRGM_GSM_CLK) 
    #define UIM_CLOCK_REGIME_DISABLE_1()                                         \
      clk_regime_disable(CLKRGM_GSM_CLK) 
    #define UIM_CLOCK_REGIME_DISABLE_2()                                         \
      clk_regime_disable(CLKRGM_GSM_CLK)
  #endif /* FEATURE_CLKREGIM_RM */
#elif defined(T_MSM6800)
  #ifdef FEATURE_UIM_MSM6800_TCXO_DIV5
    #define UIM_SELECT_CLOCK_REGIME_SOURCE_1()                                   \
      clk_regime_msm_sel_clk_src(CLK_RGM_SRC_SEL_UART2,                          \
                             UIM_MSM6800_CLK_SRCSEL_UART2_TCXO_DIV5)
    #define UIM_SELECT_CLOCK_REGIME_SOURCE_2()                                   \
      clk_regime_msm_sel_clk_src(CLK_RGM_SRC_SEL_UART3,                          \
                             UIM_MSM6800_CLK_SRCSEL_UART3_TCXO_DIV5);  
  #else
    #define UIM_SELECT_CLOCK_REGIME_SOURCE_1()                                   \
      clk_regime_msm_sel_clk_src(CLK_RGM_SRC_SEL_UART2,                          \
                             MSM_CLK_SRCSEL1_WW__UART2_CLK_SRC_SEL_RUIM_CLK_SRC)
    #define UIM_SELECT_CLOCK_REGIME_SOURCE_2()                                   \
      clk_regime_msm_sel_clk_src(CLK_RGM_SRC_SEL_UART3,                          \
                         MSM_CLK_SRCSEL2_WW__UART3_CLK_SRC_SEL_RUIM_CLK_SRC);
  #endif /* !FEATURE_UIM_MSM6800_TCXO_DIV5 */
  #define UIM_CLOCK_REGIME_ENABLE_1()                                          \
    clk_regime_msm_enable(CLK_RGM_RUIM1_M) 
  #define UIM_CLOCK_REGIME_ENABLE_2()                                          \
    clk_regime_msm_enable(CLK_RGM_RUIM2_M)   
  #define UIM_CLOCK_REGIME_DISABLE_1()                                         \
    clk_regime_msm_disable(CLK_RGM_RUIM1_M) 
  #define UIM_CLOCK_REGIME_DISABLE_2()                                         \
    clk_regime_msm_disable(CLK_RGM_RUIM2_M)  
#elif defined(FEATURE_UIM_GSM_CLK_SRC)
  #define UIM_SELECT_CLOCK_REGIME_SOURCE_1()                                   \
    clk_regime_sel_clk_src(CLK_RGM_SRC_SEL_UART2,                          \
                           MSM_CLK_SRCSEL1_WW__UART2_CLK_SRC_SEL_RUIM_CLK_SRC)
  #define UIM_SELECT_CLOCK_REGIME_SOURCE_2()                                   \
    clk_regime_sel_clk_src(CLK_RGM_SRC_SEL_UART3,                          \
                       MSM_CLK_SRCSEL2_WW__UART3_CLK_SRC_SEL_RUIM_CLK_SRC);
  #define UIM_CLOCK_REGIME_ENABLE_1()                                          \
    clk_regime_enable(CLK_RGM_RUIM1_M) 
  #define UIM_CLOCK_REGIME_ENABLE_2()                                          \
    clk_regime_enable(CLK_RGM_RUIM2_M)
  #define UIM_CLOCK_REGIME_DISABLE_1()                                         \
    clk_regime_disable(CLK_RGM_RUIM1_M) 
  #define UIM_CLOCK_REGIME_DISABLE_2()                                         \
    clk_regime_disable(CLK_RGM_RUIM2_M)
#else /* ! FEATURE_UIM_GSM_CLK_SRC) */
  #define UIM_SELECT_CLOCK_REGIME_SOURCE_1()                                   \
    clk_regime_sel_clk_src(CLK_RGM_SRC_SEL_UART2,                              \
                         MSM_CLK_SRCSEL1_WW__UART2_CLK_SRC_SEL_TCXODIV4);
  #define UIM_SELECT_CLOCK_REGIME_SOURCE_2()                                   \
    clk_regime_sel_clk_src(CLK_RGM_SRC_SEL_UART3,                              \
                         MSM_CLK_SRCSEL2_WW__UART3_CLK_SRC_SEL_TCXODIV4);
  #define UIM_CLOCK_REGIME_ENABLE_1()                                          \
    clk_regime_enable(CLK_RGM_RUIM1_M) 
  #define UIM_CLOCK_REGIME_ENABLE_2()                                          \
    clk_regime_enable(CLK_RGM_RUIM2_M)
  #define UIM_CLOCK_REGIME_DISABLE_1()                                         \
    clk_regime_disable(CLK_RGM_RUIM1_M) 
  #define UIM_CLOCK_REGIME_DISABLE_2()                                         \
    clk_regime_disable(CLK_RGM_RUIM2_M)
#endif

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

/*===========================================================================

MACRO UIM_TCXO_MUST_BE_ON
MACRO UIM_TCXO_CAN_BE_OFF

DESCRIPTION
  Issue votes for TCXO to be available for the R-UIM interface.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
  #if defined(T_MSM8650A) || defined(T_QSC6270)
    #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
      #define UIM_TCXO_MUST_BE_ON()                 \
        INTLOCK();                                  \
        if (UIM_DRV_SLOT1 == uim_drv_slot) {        \
          uim_tcxo_vote |= UIM_DRV_SLOT1_VOTE;      \
          UIM_CLOCK_REGIME_ENABLE_1();              \
          uim_clock_is_on = TRUE;                   \
        }else{                                      \
          uim_tcxo_vote |= UIM_DRV_SLOT2_VOTE;      \
          UIM_CLOCK_REGIME_ENABLE_2(); \
          uim_clock_is_on_2=TRUE;}                  \
        INTFREE();                                  \
        UIM_SLEEP_NEGATE_OKTS();

      #define UIM_TCXO_CAN_BE_OFF()                 \
        INTLOCK();                                  \
        if (UIM_DRV_SLOT1 == uim_drv_slot) {        \
          uim_tcxo_vote &= ~UIM_DRV_SLOT1_VOTE;     \
          UIM_CLOCK_REGIME_DISABLE_1();             \
          uim_clock_is_on = FALSE;                  \
        }else{                                      \
          uim_tcxo_vote &= ~UIM_DRV_SLOT2_VOTE;     \
          UIM_CLOCK_REGIME_DISABLE_2();             \
          uim_clock_is_on_2 = FALSE;               }\
        INTFREE();                                  \
        if (!uim_tcxo_vote){                        \
          UIM_SLEEP_ASSERT_OKTS();}
              
    #elif !defined( FEATURE_UIM2 )
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
    #else
      #define UIM_TCXO_MUST_BE_ON()                 \
        INTLOCK();                                  \
        UIM_CLOCK_REGIME_ENABLE_2();                \
        uim_clock_is_on_2 = TRUE;                   \
        INTFREE();                                  \
        UIM_SLEEP_NEGATE_OKTS();

      #define UIM_TCXO_CAN_BE_OFF()                 \
        INTLOCK();                                  \
        UIM_CLOCK_REGIME_DISABLE_2();               \
        uim_clock_is_on_2 = FALSE;                  \
        INTFREE();                                  \
        UIM_SLEEP_ASSERT_OKTS();
    #endif
  #elif defined(T_MSM31) && !defined (T_MSM33) /* MSM3100 only */
    #define UIM_TCXO_MUST_BE_ON() \
      (void) rex_clr_sigs( &sleep_tcb, SLEEP_UIM_OKTS_SIG )

    #define UIM_TCXO_CAN_BE_OFF() \
      (void)rex_set_sigs( &sleep_tcb, SLEEP_UIM_OKTS_SIG )
  #else
    #ifdef FEATURE_UIM_CONTROL_UIM_UART_CLK_REGIME
      #ifdef FEATURE_WCDMA
#error code not present
      #else
        #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
          #define UIM_TCXO_MUST_BE_ON()                 \
            INTLOCK();                                  \
            if (UIM_DRV_SLOT1 == uim_drv_slot) {        \
              uim_tcxo_vote |= UIM_DRV_SLOT1_VOTE;      \
              UIM_CLOCK_REGIME_ENABLE_1(); \
              uim_clock_is_on = TRUE;                   \
            }else{                                      \
              uim_tcxo_vote |= UIM_DRV_SLOT2_VOTE;      \
              UIM_CLOCK_REGIME_ENABLE_2(); \
              uim_clock_is_on_2 = TRUE;}                \
            INTFREE();                                  \
            UIM_SLEEP_NEGATE_OKTS()     

          #define UIM_TCXO_CAN_BE_OFF()                 \
            INTLOCK();                                  \
            if (UIM_DRV_SLOT1 == uim_drv_slot) {        \
              uim_tcxo_vote &= ~UIM_DRV_SLOT1_VOTE;     \
              UIM_CLOCK_REGIME_DISABLE_1();\
              uim_clock_is_on = FALSE;                  \
            }else{                                      \
              uim_tcxo_vote &= ~UIM_DRV_SLOT2_VOTE;     \
              UIM_CLOCK_REGIME_DISABLE_2();\
              uim_clock_is_on_2 = FALSE;}               \
            INTFREE();                                  \
            if (!uim_tcxo_vote){                        \
              UIM_SLEEP_ASSERT_OKTS();}
        #elif !defined( FEATURE_UIM2 )
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
          #define UIM_TCXO_MUST_BE_ON()                 \
            INTLOCK();                                  \
            UIM_CLOCK_REGIME_ENABLE_2();                \
            uim_clock_is_on_2 = TRUE;                   \
            INTFREE();                                  \
            UIM_SLEEP_NEGATE_OKTS()

          #define UIM_TCXO_CAN_BE_OFF()                 \
            INTLOCK();                                  \
            UIM_CLOCK_REGIME_DISABLE_2();               \
            uim_clock_is_on_2 = FALSE;                  \
            INTFREE();                                  \
            UIM_SLEEP_ASSERT_OKTS()
        #endif /*FEATURE_WCDMA*/
      #endif
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
  #endif
#else
  #define UIM_TCXO_MUST_BE_ON()

  #define UIM_TCXO_CAN_BE_OFF()

#endif /* FEATURE_UIM_TCXO_SLEEP */


#ifdef UIM_DRIVER_TIMESTAMP
#define HI_PRIORITY   5
#define LO_PRIORITY   2

#ifdef T_MSM6250
#error code not present
#else
  /* stub for getting rid of compilation errors */
  #define UIM_SET_UIM_PRIORITY(PRIORITY)
#endif
#endif


/* This holds the vote of the UARTs for keeping tcxo on */
#define UIM_DRV_SLOT1_VOTE  0x1
#define UIM_DRV_SLOT2_VOTE  0x2

#if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
LOCAL int uim_tcxo_vote = 0;
#endif

/* The BIO bits and registers should be defined in a bio file.  For those
   targets that do not follow the BIO naming convention, those names are
   defined here.  Those targets not selected here are assumed to have the
   proper UIM names defined in a BIO file.
 */
#if defined(FEATURE_SURF3100)  /* MSM3100 SURF */
  #define BIO_UIM_PWR_REG     ENC_GPIO_0

  #define BIO_UIM_RST_REG     ENC_GPIO_0

#elif defined (PRODUCTION_SURF_REV1)
  #if defined (T_MSM5000) /* MSM 5000 SURF */
    #define BIO_UIM_PWR_REG     DMOD_GPIO_INT_OUT
    #define BIO_UIM_PWR_M       BIO_RUIM_PWR_M
    #define BIO_UIM_PWR_OFF_V   BIO_RUIM_PWR_OFF_V
    #define BIO_UIM_PWR_ON_V    BIO_RUIM_PWR_ON_V

    #define BIO_UIM_RST_REG     ENC_GPIO_1_OUT
    #define BIO_UIM_RST_M       BIO_RUIM_RST_M
    #define BIO_UIM_RST_HIGH_V  BIO_RUIM_RST_HIGH_V
    #define BIO_UIM_RST_LOW_V   BIO_RUIM_RST_LOW_V

  #elif defined(T_MSM3) /* MSM3000 SURF */
#error code not present
  #endif /* T_MSM5000 */

#elif defined (FEATURE_SURF_5200) && !defined(T_MSM7200)/* MSM 5200 SURF*/
#error code not present
#endif /* FEATURE_SURF3100 */

/*
  Re-define the constants for programming the UART configuration register
  to support the chips with SIM Glue Logic
*/


#if defined (T_QSC6270)
  #define UART2_RXFS                         INST2_UART_DM_RXFS
  #define UART2_TXFS                         INST2_UART_DM_TXFS
  #define UART2_TFWR                         INST2_UART_DM_TFWR
  #define UART2_USIM_CFG                     INST2_UART_DM_SIM_CFG
  #define UART2_IRDA                         INST2_UART_DM_IRDA
  #define UART2_DMEM                         INST2_UART_DM_DMEN
  #define UART2_RX_INIT                      INST2_UART_DM_DMRX
  #define UIM_DISABLE_DM                     0x00
  #define UART_TFWR_TFW_MASK                 HWIO_FMSK(UART2_TFWR, TFW)
  /* SIM CONFIG REGISTER MASKS AND SHIFTS*/
  #define UART2_SIM_CFG__MASK_RX_MASK        HWIO_FMSK(UART2_USIM_CFG, MASK_RX);
  #define UART_DEFAULT_SIM_CFG_MASK_RX_VAL   (1<<HWIO_INST2_UART_DM_SIM_CFG_MASK_RX_SHFT)
  #define UART2_SIM_CFG_WH__SWAP_D_MASK      HWIO_FMSK(UART2_USIM_CFG, SWAP_D)
  #define UART2_SIM_CFG_WH__INV_D_MASK       HWIO_FMSK(UART2_USIM_CFG, INV_D)
  #define UART_SIM_CFG_TX_RX_MODE_MASK      (HWIO_FMSK(UART2_USIM_CFG, UIM_TX_MODE) | \
                                             HWIO_FMSK(UART2_USIM_CFG, UIM_RX_MODE))
  #define UART_SIM_CFG__STOP_BIT_LEN_MASK    HWIO_FMSK(UART2_USIM_CFG,SIM_STOP_BIT_LEN)

  #define UART_SIM_CFG__STOP_BIT_LEN_SHFT    HWIO_INST2_UART_DM_SIM_CFG_SIM_STOP_BIT_LEN_SHFT
  #define UIM_DEFAULT_STOP_BIT_VALUE        (0x2<<UART_SIM_CFG__STOP_BIT_LEN_SHFT)
  #define UART_SIM_CFG__CLK_MASK             HWIO_INST2_UART_DM_SIM_CFG_SIM_CLK_ON_BMSK | \
                                             HWIO_INST2_UART_DM_SIM_CFG_SIM_CLK_TD8_SEL_BMSK | \
                                             HWIO_INST2_UART_DM_SIM_CFG_SIM_CLK_STOP_HIGH_BMSK
  #define UART_SIM_CFG__SIM_CLK_OFF          0
  #define UART_SIM_CFG__SIM_CLK_STOP_HI_MASK HWIO_FMSK(UART2_USIM_CFG,SIM_CLK_STOP_HIGH)
  #define UART_SIM_CFG__SIM_CLK_ON_MASK      HWIO_FMSK(UART2_USIM_CFG,SIM_CLK_ON)
  #define UART_SIM_CFG__SIM_CLK_TD8_SEL_MASK HWIO_FMSK(UART2_USIM_CFG,SIM_CLK_TD8_SEL)
  #define UART_SIM_CFG__SIM_CLK_TD4_SEL_MASK 0
  #define UART_SIM_CFG__MASK_RX              HWIO_FMSK(UART2_USIM_CFG,MASK_RX)
  #define UART_SIM_CFG__MASK                (HWIO_FMSK(UART2_USIM_CFG,MASK_RX) | \
                                             HWIO_FMSK(UART2_USIM_CFG,SWAP_D)  | \
                                             HWIO_FMSK(UART2_USIM_CFG,INV_D)   | \
                                             HWIO_FMSK(UART2_USIM_CFG,SIM_SEL))
  #define UART_SIM_CFG__SIM_SEL_MASK         HWIO_FMSK(UART2_USIM_CFG,SIM_SEL)

  #define UART_DM_RXFS__RXFIFO_STATE_LSB_MASK HWIO_FMSK(UART2_RXFS, RX_FIFO_STATE_LSB)

  #define UART_DM_RXFS__RX_BUFFER_STATE_MASK  HWIO_FMSK(UART2_RXFS, RX_BUFFER_STATE)

  #define UART_DM_RXFS__RXFIFO_STATE_LSB_SHFT HWIO_SHFT(UART2_RXFS, RX_FIFO_STATE_LSB)

  #define UART_DM_RXFS__RX_BUFFER_STATE_SHFT  HWIO_SHFT(UART2_RXFS, RX_BUFFER_STATE)

  /* TXFS REGISTER MASKS AND SHIFTS*/

  #define UART_DM_TXFS__TXFIFO_STATE_LSB_MASK HWIO_FMSK(UART2_TXFS, TX_FIFO_STATE_LSB)

  #define UART_DM_TXFS__TX_BUFFER_STATE_MASK  HWIO_FMSK(UART2_TXFS, TX_BUFFER_STATE)

  #define UART_DM_TXFS__TXFIFO_STATE_LSB_SHFT HWIO_SHFT(UART2_TXFS, TX_FIFO_STATE_LSB)

  #define UART_DM_TXFS__TX_BUFFER_STATE_SHFT  HWIO_SHFT(UART2_TXFS, TX_BUFFER_STATE)


  /* Gets the number of bytes that are yet to be read from RX FIFO and the 
     packing buffer */
  #define UIM_GET_NUM_BYTES_WAITING_IN_RX() HWIO_IN(UART2_RXFS)

  /* Gets the number of bytes that are yet to be sent from TX FIFO and the
     un-packing buffer */
  #define UIM_GET_NUM_BYTES_WAITING_IN_TX() HWIO_IN(UART2_TXFS)

  /* Get the size of data received in the current data transfer.
  Note that SNAP gives the size of data received in the last 
  RX data transfer. */
  #define UIM_GET_DYNAMIC_SIZE_IN_RX() HWIO_IN(UART2_RX_INIT)

  /* Holds the MPP voltage reference on the SIM side */
  LOCAL pm_mpp_dlogic_lvl_type uim_pmic_sim_side_ref;

  #define UIM_SET_POWER_AS_OUTPUT()
  /*
   The USIM aux module must be left on even when the USIM is powered off
   so that the MSM pads can drive the card pads low. If the MSM pads
   aren't powered, they can't drive low.

   The second parameter for pm_vreg_control() is of type pm_vreg_mask_type which is 64 bits
   in newer platforms like LCU.
   */
  #define UIM_POWER_OFF()                   (void)pm_usim_aux_vreg_set(PM_USIM_AUX_CTRL_VREG_MSME);   \
                                            (void)pm_vreg_control( PM_OFF_CMD, PM_VREG_USIM_M );

  #define UIM_POWER_ON() ;

  //#define UIM_DISABLE_IRDA()                 HWIO_OUT( UART2_IRDA, MSMU_IRDA_DISABLE)
  #define UIM_DISABLE_DATA_MOVER()           HWIO_OUT( UART2_DMEM, UIM_DISABLE_DM)

  #define UIM_DM_RX_INIT( rx_chars )        HWIO_OUT(UART2_RX_INIT, rx_chars)


#elif defined (T_MSM8650A)
  #if defined( FEATURE_UIM1 )
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
    
    #define UART2_SIM_CFG_WH__SWAP_D_MASK \
            HWIO_FMSK(UART2_SIM_CFG, SWAP_D)

    #define UART2_SIM_CFG_WH__INV_D_MASK \
            HWIO_FMSK(UART2_SIM_CFG, INV_D)

    #ifdef  FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG
      #define UART_SIM_CFG_TX_RX_MODE_MASK (HWIO_FMSK(UART2_SIM_CFG, UIM_TX_MODE) | \
                                          HWIO_FMSK(UART2_SIM_CFG, UIM_RX_MODE))
    #else
      #define UART_TFWR_TX_RX_MODE_MASK (HWIO_FMSK(UART2_TFWR, UIM_TX_MODE) | \
                                     HWIO_FMSK(UART2_TFWR, UIM_RX_MODE))
    #endif /* FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG */

  #endif /* FEATURE_UIM1 */
#elif defined (T_UIM_SABER_DEFS)
#error code not present
#elif defined (T_MSM6100) || defined (T_MSM6275)
#if defined (T_MSM6500) || defined (T_MSM7200)
#if !defined (T_MSM7200 )
  /* 7200 maps MSM_OUT to HWIO_OUT, so this is not needed */
  #define MSMU2_MVR  HWIO_ADDR(UART2_MREG)
  #define MSMU2_NVR  HWIO_ADDR(UART2_NREG)
  #define MSMU2_DVR  HWIO_ADDR(UART2_DREG)
  #define MSMU2_MNDR HWIO_ADDR(UART2_MNDREG)
  #define MSMU3_MVR  HWIO_ADDR(UART3_MREG)
  #define MSMU3_NVR  HWIO_ADDR(UART3_NREG)
  #define MSMU3_DVR  HWIO_ADDR(UART3_DREG)
  #define MSMU3_MNDR HWIO_ADDR(UART3_MNDREG)
#endif /*!defined (T_MSM7200 )*/
  #define UART2_SIM_CFG_WH__SWAP_D_MASK \
          HWIO_FMSK(UART2_SIM_CFG, SWAP_D)
  #define UART2_SIM_CFG_WH__INV_D_MASK \
          HWIO_FMSK(UART2_SIM_CFG, INV_D)
  #define UART3_SIM_CFG_WH__SWAP_D_MASK \
          HWIO_FMSK(UART3_USIM_CFG, SWAP_D)
  #define UART3_SIM_CFG_WH__INV_D_MASK \
          HWIO_FMSK(UART3_USIM_CFG, INV_D)
#elif defined (T_MSM6275)
#error code not present
#elif defined(T_QSC60X0)
#error code not present
#endif /* MSM6500 */

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
#elif defined (T_MSM6050)
#error code not present
#elif defined (T_MSM5105) /* Defined for 5105 or 5100 or 5500 */
#error code not present
#elif defined (T_MSM33)
#error code not present
#elif defined (T_UIM_UART1_USIM_CFG)
#error code not present
#endif /* Define clock control constants for SIM Glue Logic chips */

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

#if !defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
  #ifdef FEATURE_UIM_MULTIPLE_REGULATORS_ON_PMIC
    #define UIM_CONFIG_UIM_LINES()                                        \
      (void)pm_mpp_config_digital_output(PM_MPP_6,                        \
                                         PM_MPP__DLOGIC__LVL_MMC,         \
                                         PM_MPP__DLOGIC_OUT__CTRL_MPP);   \
      (void)pm_mpp_config_digital_output(PM_MPP_10,                       \
                                         PM_MPP__DLOGIC__LVL_RUIM,        \
                                         PM_MPP__DLOGIC_OUT__CTRL_MPP);   \
      (void)pm_mpp_config_digital_inout( PM_MPP_12,                       \
                                         PM_MPP__DLOGIC__LVL_RUIM,        \
                                         PM_MPP__DLOGIC_INOUT__PUP_30K)
  #else /* FEATURE_UIM_MULTIPLE_REGULATORS_ON_PMIC */
    #define UIM_CONFIG_UIM_LINES()                                        \
      (void)pm_mpp_config_digital_output(PM_MPP_6,                        \
                                         uim_pmic_sim_side_ref,           \
                                         PM_MPP__DLOGIC_OUT__CTRL_MPP);   \
      (void)pm_mpp_config_digital_output(PM_MPP_10,                       \
                                         uim_pmic_sim_side_ref,           \
                                         PM_MPP__DLOGIC_OUT__CTRL_MPP);   \
      (void)pm_mpp_config_digital_inout( PM_MPP_12,                       \
                                         uim_pmic_sim_side_ref,           \
                                         PM_MPP__DLOGIC_INOUT__PUP_30K)
  #endif /* FEATURE_UIM_MULTIPLE_REGULATORS_ON_PMIC */
#else
  #define UIM_CONFIG_UIM_LINES()
#endif /* !FEATURE_UIM_USE_ONLY_VREG_ON_PMIC */

#if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
#if defined( FEATURE_UIM_PMIC_ON_UIM1 )
  /* Holds the MPP voltage reference on the SIM side */
  pm_mpp_dlogic_lvl_type uim_pmic_sim_side_ref;
  #if defined( T_MSM8650A )
    #define UIM_SET_POWER_AS_OUTPUT()
  #else
    #define UIM_SET_POWER_AS_OUTPUT()                                     \
      if (UIM_DRV_SLOT1 != uim_drv_slot) {                                \
        BIO_TRISTATE( BIO_UIM2_PWR_REG, BIO_UIM2_PWR_M, BIO_UIM2_PWR_M);}
  #endif
  #define UIM_POWER_OFF()                                               \
    if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
      UIM_DRIVE_LINES_LOW();                                            \
      (void)pm_vreg_control( PM_OFF_CMD, PM_VREG_RUIM_M );              \
    }else{                                                              \
      BIO_OUT( BIO_UIM2_PWR_REG, BIO_UIM2_PWR_M, BIO_UIM2_PWR_OFF_V);}
  #define UIM_POWER_ON()                                                \
    if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
      (void)pm_vreg_control( PM_ON_CMD, PM_VREG_RUIM_M );               \
      UIM_CONFIG_UIM_LINES();                                           \
    }else{                                                              \
      BIO_OUT( BIO_UIM2_PWR_REG, BIO_UIM2_PWR_M, BIO_UIM2_PWR_ON_V);}
#elif defined( FEATURE_UIM_PMIC_ON_UIM2 )
  /* Holds the MPP voltage reference on the SIM side */
  pm_mpp_dlogic_lvl_type uim_pmic_sim_side_ref;
  #if defined( T_MSM8650A )
    #define UIM_SET_POWER_AS_OUTPUT()
  #else
    #define UIM_SET_POWER_AS_OUTPUT()                                     \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
        BIO_TRISTATE( BIO_UIM_PWR_REG, BIO_UIM_PWR_M, BIO_UIM_PWR_M);}
  #endif
  #define UIM_POWER_OFF()                                               \
    if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
      BIO_OUT( BIO_UIM_PWR_REG, BIO_UIM_PWR_M, BIO_UIM_PWR_OFF_V);      \
    }else{                                                              \
      UIM_DRIVE_LINES_LOW();                                            \
      (void)pm_vreg_control( PM_OFF_CMD, (uint32)PM_VREG_RUIM_M );}
  #define UIM_POWER_ON()                                                \
    if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
      BIO_OUT( BIO_UIM_PWR_REG, BIO_UIM_PWR_M, BIO_UIM_PWR_ON_V);       \
    }else{                                                              \
      (void)pm_vreg_control( PM_ON_CMD, PM_VREG_RUIM_M );               \
      UIM_CONFIG_UIM_LINES();                                           \
    }
#else /* No PMIC */
  #define UIM_SET_POWER_AS_OUTPUT()                                     \
    if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
      BIO_TRISTATE( BIO_UIM_PWR_REG, BIO_UIM_PWR_M, BIO_UIM_PWR_M);     \
    }else{                                                              \
      BIO_TRISTATE( BIO_UIM2_PWR_REG, BIO_UIM2_PWR_M, BIO_UIM2_PWR_M);}
  #define UIM_POWER_OFF()                                               \
    if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
      BIO_OUT( BIO_UIM_PWR_REG, BIO_UIM_PWR_M, BIO_UIM_PWR_OFF_V);      \
    }else{                                                              \
      BIO_OUT( BIO_UIM2_PWR_REG, BIO_UIM2_PWR_M, BIO_UIM2_PWR_OFF_V);}
  #define UIM_POWER_ON()                                                \
    if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
      BIO_OUT( BIO_UIM_PWR_REG, BIO_UIM_PWR_M, BIO_UIM_PWR_ON_V);       \
    }else{                                                              \
      BIO_OUT( BIO_UIM2_PWR_REG, BIO_UIM2_PWR_M, BIO_UIM2_PWR_ON_V);}
#endif /* FEATURE_UIM_PMIC_ON_UIM1 */

  /*
   * These macros are used to control the reset line
   * of the UIM.
   */
  #define UIM_SET_RESET_AS_OUTPUT()                                     \
    if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
      BIO_TRISTATE( BIO_UIM_RST_REG, BIO_UIM_RST_M, BIO_UIM_RST_M );    \
    }else{                                                              \
      BIO_TRISTATE( BIO_UIM2_RST_REG, BIO_UIM2_RST_M, BIO_UIM2_RST_M );}
  #define UIM_ASSERT_RESET()                                            \
    if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
      BIO_OUT( BIO_UIM_RST_REG, BIO_UIM_RST_M, BIO_UIM_RST_LOW_V );     \
    }else{                                                              \
      BIO_OUT( BIO_UIM2_RST_REG, BIO_UIM2_RST_M, BIO_UIM2_RST_LOW_V );}
  #define UIM_DEASSERT_RESET()                                          \
    if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
      BIO_OUT( BIO_UIM_RST_REG, BIO_UIM_RST_M, BIO_UIM_RST_HIGH_V );    \
    }else{                                                              \
      BIO_OUT( BIO_UIM2_RST_REG, BIO_UIM2_RST_M, BIO_UIM2_RST_HIGH_V );}
#elif !defined( FEATURE_UIM2 )
  /*
   * These macros are used to control the power enable line
   * of the UIM.
   */
#if defined( FEATURE_UIM_NO_UIM_POWER_CNTL )
  #define UIM_SET_POWER_AS_OUTPUT()
  #define UIM_POWER_OFF()
  #define UIM_POWER_ON()
#elif !defined( FEATURE_UIM_PMIC_ON_UIM1 ) && !defined(FEATURE_UIM_USE_ONLY_VREG_ON_PMIC)
  #define UIM_SET_POWER_AS_OUTPUT()                                     \
    BIO_TRISTATE( BIO_UIM_PWR_REG, BIO_UIM_PWR_M, BIO_UIM_PWR_M)
  #define UIM_POWER_OFF()                                               \
    BIO_OUT( BIO_UIM_PWR_REG, BIO_UIM_PWR_M, BIO_UIM_PWR_OFF_V)
  #define UIM_POWER_ON()                                                \
    BIO_OUT( BIO_UIM_PWR_REG, BIO_UIM_PWR_M, BIO_UIM_PWR_ON_V)
#else /* FEATURE_UIM_PMIC_ON_UIM1 && !FEATURE_UIM_USE_ONLY_VREG_ON_PMIC */
  #if !defined(FEATURE_UIM_USE_ONLY_VREG_ON_PMIC)
    /* Holds the MPP voltage reference on the SIM side */
    LOCAL pm_mpp_dlogic_lvl_type uim_pmic_sim_side_ref;
  #endif /* !FEATURE_UIM_USE_ONLY_VREG_ON_PMIC */
  #define UIM_SET_POWER_AS_OUTPUT()
#if defined(T_QSC60X5) && !defined(T_QSC6270)
    /* For QSC60X5, when UIM is powered down, the recommendation from the
       power management team is to not disable the VREG_RUIM and pull high
       the UIM_PWR_DIS (GPIO_OUTPUT_10) line to save power. */
  #define UIM_POWER_OFF()                                               \
    UIM_DRIVE_LINES_LOW();                                              \
    (void)gpio_out(GPIO_OUTPUT_10, GPIO_HIGH_VALUE)
    /* For QSC60X5, when UIM is powered up, pull low the UIM_PWR_DIS line
       (GPIO_OUTPUT_10) connected to gate of the power enable FET */
  #define UIM_POWER_ON()                                                \
    (void)gpio_out(GPIO_OUTPUT_10, GPIO_LOW_VALUE);                     \
    (void)pm_vreg_control( PM_ON_CMD, PM_VREG_RUIM_M );                 \
    UIM_CONFIG_UIM_LINES();
#elif !defined(T_QSC6270)
#ifdef FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM
#error code not present
#else
  #define UIM_POWER_OFF()                                               \
    UIM_DRIVE_LINES_LOW();                                              \
    (void)pm_vreg_control( PM_OFF_CMD, PM_VREG_RUIM_M )
  #define UIM_POWER_ON()                                                \
    (void)pm_vreg_control( PM_ON_CMD, PM_VREG_RUIM_M );                 \
    UIM_CONFIG_UIM_LINES();
#endif /* FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM */
#endif /* T_QSC60X5 and !T_QSC6270 */
#endif

  /*
   * These macros are used to control the reset line
   * of the UIM.
   */

#ifdef T_QSC6270
  #define UIM_RESET GPIO_USIM_RESET
  #define UIM_SET_RESET_AS_OUTPUT()
  #define UIM_ASSERT_RESET()             gpio_out(UIM_RESET,GPIO_LOW_VALUE);
  #define UIM_DEASSERT_RESET()           gpio_out(UIM_RESET,GPIO_HIGH_VALUE);
#elif defined( T_QSC60X5 ) || defined( T_MSM8650A )
  #define UIM_SET_RESET_AS_OUTPUT()                                     \
    gpio_tlmm_config(UIM_RESET);
  #define UIM_ASSERT_RESET()                                            \
    gpio_out(UIM_RESET,GPIO_LOW_VALUE);
  #define UIM_DEASSERT_RESET()                                          \
    gpio_out(UIM_RESET,GPIO_HIGH_VALUE);
#else
#error code not present
#endif /* T_QSC6270 */
#else /* FEATURE_UIM2 */
  /*
   * These macros are used to control the power enable line
   * of the UIM.
   */
#if !defined( FEATURE_UIM_PMIC_ON_UIM2 )
  #define UIM_SET_POWER_AS_OUTPUT()                                     \
    BIO_TRISTATE( BIO_UIM2_PWR_REG, BIO_UIM2_PWR_M, BIO_UIM2_PWR_M)
  #define UIM_POWER_OFF()                                               \
    BIO_OUT( BIO_UIM2_PWR_REG, BIO_UIM2_PWR_M, BIO_UIM2_PWR_OFF_V)
  #define UIM_POWER_ON()                                                \
    BIO_OUT( BIO_UIM2_PWR_REG, BIO_UIM2_PWR_M, BIO_UIM2_PWR_ON_V)
#else /* FEATURE_UIM_PMIC_ON_UIM2 */
  /* Holds the MPP voltage reference on the SIM side */
  pm_mpp_dlogic_lvl_type uim_pmic_sim_side_ref;
  #define UIM_SET_POWER_AS_OUTPUT()
  #define UIM_POWER_OFF()                                               \
    UIM_DRIVE_LINES_LOW();                                              \
    (void)pm_vreg_control( PM_OFF_CMD, PM_VREG_RUIM_M )
  #define UIM_POWER_ON()                                                \
    (void)pm_vreg_control( PM_ON_CMD, PM_VREG_RUIM_M );                 \
    UIM_CONFIG_UIM_LINES();
#endif
  /*
   * These macros are used to control the reset line
   * of the UIM.
   */
  #define UIM_SET_RESET_AS_OUTPUT()                                     \
    BIO_TRISTATE( BIO_UIM2_RST_REG, BIO_UIM2_RST_M, BIO_UIM2_RST_M )
  #define UIM_ASSERT_RESET()                                            \
    BIO_OUT( BIO_UIM2_RST_REG, BIO_UIM2_RST_M, BIO_UIM2_RST_LOW_V )
  #define UIM_DEASSERT_RESET()                                          \
    BIO_OUT( BIO_UIM2_RST_REG, BIO_UIM2_RST_M, BIO_UIM2_RST_HIGH_V )
#endif

/*Macro to read the status of the UART register */
#ifdef T_QSC6270
  #define UART2_SR              INST2_UART_DM_SR
  #define UIM_READ_STATUS()     HWIO_IN( UART2_SR )
#elif defined (T_MSM6100) || defined (T_UIM_SABER_DEFS) || defined (T_MSM6275) || defined (T_MSM8650A)
  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_READ_STATUS()            \
       ((UIM_DRV_SLOT1 == uim_drv_slot) ? \
       HWIO_IN( UART2_SR )             : \
       HWIO_IN( UART3_SR ))
  #elif !defined( FEATURE_UIM2 )
    #define UIM_READ_STATUS()            \
      HWIO_IN( UART2_SR )
  #else
    #define UIM_READ_STATUS()            \
      HWIO_IN( UART3_SR )
  #endif
#elif defined(T_MSM3) || defined(T_MSM5000)
#error code not present
#elif defined(T_UIM_UART1_USIM_CFG)
#error code not present
#endif /* T_QSC6270 */

  
/*
 * These macros control the UART interface lines.
 */
#define MSMU_ENA             0xffff /* Value to be used in enabling.
                                       Should be used with masks.          */
#define MSMU_DIS             0x0000 /* Value to be used in disabling.
                                       Should be used with masks.          */

#ifdef T_QSC6270

  /* Configuration for 6270 is handled by the BIO Functions*/
  #define ENABLE_UART_INTERFACE()

  #define UART2_CR                      INST2_UART_DM_CR
  #define UART2_IPR                     INST2_UART_DM_IPR
  #define UART2_IMR                     INST2_UART_DM_IMR
  #define UART2_MR1                     INST2_UART_DM_MR1
  #define UART2_MR2                     INST2_UART_DM_MR2
  #define UART2_MISR                    INST2_UART_DM_MISR
  #define UART2_CSR                     INST2_UART_DM_CSR
  #define UART2_BADR                    INST2_UART_DM_BADR
  #define UART2_RF                      INST2_UART_DM_RF
  #define UART2_TF                      INST2_UART_DM_TF
  #define UART2_NUM_TX_CHARS            INST2_UART_DM_NO_CHARS_FOR_TX
  #define UART2_RX_TOTAL_SNAP           INST2_UART_DM_RX_TOTAL_SNAP
  #define UART2_RFWR                    INST2_UART_DM_RFWR
  #define UART2_SIM_CFG                 UART2_USIM_CFG

  #define UIM_PRG_UART_BADR(badr_val)     HWIO_OUT(UART2_BADR, badr_val)
  #define UIM_GET_RX_WORD()               HWIO_IN( UART2_RF )
  #define UIM_GET_NUM_BYTES_IN_RX()       HWIO_IN( UART2_RX_TOTAL_SNAP)
  #define UIM_NUM_CHARS_TO_SEND( tx_num ) HWIO_OUT( UART2_NUM_TX_CHARS, tx_num)
  #define UIM_ENABLE_RX_STALE_INT()       HWIO_OUT( UART2_CR, MSMU_CR_ENA_STALE_EVT)
  #define UIM_CLEAR_RX_STALE_INT()        HWIO_OUT( UART2_CR, MSMU_CR_CLEAR_STALE_EVT)
  #define UIM_FORCE_RX_STALE_INT()        HWIO_OUT( UART2_CR, MSMU_CR_FORCE_STALE_EVT)
  #define UIM_CLEAR_TX_READY_INT()        HWIO_OUT( UART2_CR, MSMU_CR_CLR_TX_READY)
#endif /* T_QSC6270 */


#if defined (T_MSM6100) || defined (T_UIM_SABER_DEFS) || defined (T_MSM6275) || defined(T_MSM7500) || defined(T_MSM7200) || defined(T_MSM8650A)
  #if defined ( T_MSM7500) || defined( T_MSM8650A )
    #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
      #define ENABLE_UART_INTERFACE()      \
        gpio_tlmm_config( UIM1_CLK );      \
        gpio_tlmm_config( UIM1_DATA_OUT ); \
        gpio_tlmm_config( UIM2_CLK );      \
        gpio_tlmm_config( UIM2_DATA_OUT )
    #elif !defined( FEATURE_UIM2 )
      #define ENABLE_UART_INTERFACE()      \
        gpio_tlmm_config( UIM1_CLK );      \
        gpio_tlmm_config( UIM1_DATA_OUT )
    #else
      #define ENABLE_UART_INTERFACE()     \
        gpio_tlmm_config( UIM2_CLK );     \
        gpio_tlmm_config( UIM2_DATA_OUT )
    #endif  
  #elif defined( T_MSM6100 )
    #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
      #define ENABLE_UART_INTERFACE()     \
        gpio_tlmm_config( UIM_CLK );      \
        gpio_tlmm_config( UIM_DATA );     \
        gpio_tlmm_config( UIM2_CLK );     \
        gpio_tlmm_config( UIM2_DATA )
    #elif !defined( FEATURE_UIM2 )
      #define ENABLE_UART_INTERFACE()     \
        gpio_tlmm_config( UIM_CLK );      \
        gpio_tlmm_config( UIM_DATA )
    #else
      #define ENABLE_UART_INTERFACE()     \
        gpio_tlmm_config( UIM2_CLK );     \
        gpio_tlmm_config( UIM2_DATA )
    #endif
  #elif defined (T_MSM6275) && !defined (T_QSC6270)
#error code not present
  #elif defined (T_UIM_SABER_DEFS)
#error code not present
  #else
#error code not present
  #endif


  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_READ_RX_FIFO()           \
       ( (UIM_DRV_SLOT1 == uim_drv_slot) ? \
       HWIO_IN( UART2_RF )             : \
       HWIO_IN( UART3_RF ) )
  #elif !defined( FEATURE_UIM2 )
    #define UIM_READ_RX_FIFO()           \
      HWIO_IN( UART2_RF )
  #else
    #define UIM_READ_RX_FIFO()           \
      HWIO_IN( UART3_RF )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_READ_ISR()               \
       (UIM_DRV_SLOT1 == uim_drv_slot) ? \
       HWIO_IN( UART2_ISR )            : \
       HWIO_IN( UART3_ISR )
  #elif !defined( FEATURE_UIM2 )
    #define UIM_READ_ISR()               \
      HWIO_IN( UART2_ISR )
  #else
    #define UIM_READ_ISR()               \
      HWIO_IN( UART3_ISR )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_READ_MISR()              \
       (UIM_DRV_SLOT1 == uim_drv_slot) ? \
       HWIO_IN( UART2_MISR )           : \
       HWIO_IN( UART3_MISR )
  #elif !defined( FEATURE_UIM2 )
    #define UIM_READ_MISR()              \
      HWIO_IN( UART2_MISR )
  #else
    #define UIM_READ_MISR()              \
      HWIO_IN( UART3_MISR )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_SEND_TX_FIFO( tx_value )     \
       if (UIM_DRV_SLOT1 == uim_drv_slot) {  \
         HWIO_OUT( UART2_TF, tx_value );     \
       }else{                                \
         HWIO_OUT( UART3_TF, tx_value );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_SEND_TX_FIFO( tx_value )     \
      HWIO_OUT( UART2_TF, tx_value )
  #else
    #define UIM_SEND_TX_FIFO( tx_value )     \
      HWIO_OUT( UART3_TF, tx_value )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_RESET_RX()                       \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {       \
        HWIO_OUT( UART2_CR, MSMU_CR_RESET_RX );  \
        HWIO_OUT( UART2_CR, MSMU_CR_RESET_RX );  \
      }else{                                     \
        HWIO_OUT( UART3_CR, MSMU_CR_RESET_RX );  \
        HWIO_OUT( UART3_CR, MSMU_CR_RESET_RX );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_RESET_RX()                       \
      HWIO_OUT( UART2_CR, MSMU_CR_RESET_RX );    \
      HWIO_OUT( UART2_CR, MSMU_CR_RESET_RX )
  #else
    #define UIM_RESET_RX()                       \
      HWIO_OUT( UART3_CR, MSMU_CR_RESET_RX );    \
      HWIO_OUT( UART3_CR, MSMU_CR_RESET_RX )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_RESET_TX()                       \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {       \
        HWIO_OUT( UART2_CR, MSMU_CR_RESET_TX );  \
        HWIO_OUT( UART2_CR, MSMU_CR_RESET_TX );  \
      }else{                                     \
        HWIO_OUT( UART3_CR, MSMU_CR_RESET_TX );  \
        HWIO_OUT( UART3_CR, MSMU_CR_RESET_TX );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_RESET_TX()                       \
      HWIO_OUT( UART2_CR, MSMU_CR_RESET_TX );    \
      HWIO_OUT( UART2_CR, MSMU_CR_RESET_TX )
  #else
    #define UIM_RESET_TX()                       \
      HWIO_OUT( UART3_CR, MSMU_CR_RESET_TX );    \
      HWIO_OUT( UART3_CR, MSMU_CR_RESET_TX )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_RESET_ERR()                       \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {        \
        HWIO_OUT( UART2_CR, MSMU_CR_RESET_ERR );  \
      }else{                                      \
        HWIO_OUT( UART3_CR, MSMU_CR_RESET_ERR );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_RESET_ERR()                       \
      HWIO_OUT( UART2_CR, MSMU_CR_RESET_ERR )
  #else
    #define UIM_RESET_ERR()                       \
      HWIO_OUT( UART3_CR, MSMU_CR_RESET_ERR )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_RESET_BREAK()                         \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {            \
        HWIO_OUT( UART2_CR, MSMU_CR_RESET_BRK_INT );  \
      }else{                                          \
        HWIO_OUT( UART3_CR, MSMU_CR_RESET_BRK_INT );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_RESET_BREAK()                         \
      HWIO_OUT( UART2_CR, MSMU_CR_RESET_BRK_INT )
  #else
    #define UIM_RESET_BREAK()                         \
      HWIO_OUT( UART3_CR, MSMU_CR_RESET_BRK_INT )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_START_BREAK()                   \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {      \
        HWIO_OUT( UART2_CR, MSMU_CR_STA_BRK );  \
      }else{                                    \
        HWIO_OUT( UART3_CR, MSMU_CR_STA_BRK );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_START_BREAK()                   \
      HWIO_OUT( UART2_CR, MSMU_CR_STA_BRK )
  #else
    #define UIM_START_BREAK()                   \
      HWIO_OUT( UART3_CR, MSMU_CR_STA_BRK )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_STOP_BREAK()                    \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {      \
        HWIO_OUT( UART2_CR, MSMU_CR_STO_BRK );  \
      }else{                                    \
        HWIO_OUT( UART3_CR, MSMU_CR_STO_BRK );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_STOP_BREAK()                    \
      HWIO_OUT( UART2_CR, MSMU_CR_STO_BRK )
  #else
    #define UIM_STOP_BREAK()                    \
      HWIO_OUT( UART3_CR, MSMU_CR_STO_BRK )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_RESET_TX_ERROR()                     \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {           \
        HWIO_OUT( UART2_CR, MSMU_CR_CLR_TX_ERROR );  \
      }else{                                         \
        HWIO_OUT( UART3_CR, MSMU_CR_CLR_TX_ERROR );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_RESET_TX_ERROR()                     \
      HWIO_OUT( UART2_CR, MSMU_CR_CLR_TX_ERROR )
  #else
    #define UIM_RESET_TX_ERROR()                     \
      HWIO_OUT( UART3_CR, MSMU_CR_CLR_TX_ERROR )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_SAMPLE_MODE()                     \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {        \
        HWIO_OUT( UART2_CR, MSMU_CR_SAMP_MODE );  \
      }else{                                      \
        HWIO_OUT( UART3_CR, MSMU_CR_SAMP_MODE );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_SAMPLE_MODE()                     \
      HWIO_OUT( UART2_CR, MSMU_CR_SAMP_MODE )
  #else
    #define UIM_SAMPLE_MODE()                     \
      HWIO_OUT( UART3_CR, MSMU_CR_SAMP_MODE )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_DISABLE_UART_TX()               \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {      \
        HWIO_OUT( UART2_CR,  MSMU_CR_DIS_TX );  \
      }else{                                    \
        HWIO_OUT( UART3_CR,  MSMU_CR_DIS_TX );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_DISABLE_UART_TX()               \
      HWIO_OUT( UART2_CR,  MSMU_CR_DIS_TX )
  #else
    #define UIM_DISABLE_UART_TX()               \
      HWIO_OUT( UART3_CR,  MSMU_CR_DIS_TX )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_ENABLE_UART_TX()                \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {      \
        HWIO_OUT( UART2_CR,  MSMU_CR_ENA_TX );  \
      }else{                                    \
        HWIO_OUT( UART3_CR,  MSMU_CR_ENA_TX );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_ENABLE_UART_TX()                \
      HWIO_OUT( UART2_CR,  MSMU_CR_ENA_TX )
  #else
    #define UIM_ENABLE_UART_TX()                \
      HWIO_OUT( UART3_CR,  MSMU_CR_ENA_TX )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_DISABLE_UART_RX()               \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {      \
        HWIO_OUT( UART2_CR,  MSMU_CR_DIS_RX );  \
      }else{                                    \
        HWIO_OUT( UART3_CR,  MSMU_CR_DIS_RX );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_DISABLE_UART_RX()               \
      HWIO_OUT( UART2_CR,  MSMU_CR_DIS_RX )
  #else
    #define UIM_DISABLE_UART_RX()               \
      HWIO_OUT( UART3_CR,  MSMU_CR_DIS_RX )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_ENABLE_UART_RX()                \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {      \
        HWIO_OUT( UART2_CR,  MSMU_CR_ENA_RX );  \
      }else{                                    \
        HWIO_OUT( UART3_CR,  MSMU_CR_ENA_RX );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_ENABLE_UART_RX()                \
      HWIO_OUT( UART2_CR,  MSMU_CR_ENA_RX )
  #else
    #define UIM_ENABLE_UART_RX()                \
      HWIO_OUT( UART3_CR,  MSMU_CR_ENA_RX )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_RESET_SAMPLE_MODE()                   \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {            \
        HWIO_OUT( UART2_CR,  MSMU_CR_RESET_SAMPLE );  \
      }else{                                          \
        HWIO_OUT( UART3_CR,  MSMU_CR_RESET_SAMPLE );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_RESET_SAMPLE_MODE()                   \
      HWIO_OUT( UART2_CR,  MSMU_CR_RESET_SAMPLE )
  #else
    #define UIM_RESET_SAMPLE_MODE()                   \
      HWIO_OUT( UART3_CR,  MSMU_CR_RESET_SAMPLE )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_PRG_UART_CSR( csr_val )     \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {  \
        HWIO_OUT( UART2_CSR, csr_val );     \
      }else{                                \
        HWIO_OUT( UART3_CSR, csr_val );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_PRG_UART_CSR( csr_val )     \
      HWIO_OUT( UART2_CSR, csr_val )
  #else
    #define UIM_PRG_UART_CSR( csr_val )     \
      HWIO_OUT( UART3_CSR, csr_val )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_PRG_MR1( mr1_val )          \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {  \
        HWIO_OUT( UART2_MR1, mr1_val );     \
      }else{                                \
        HWIO_OUT( UART3_MR1, mr1_val );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_PRG_MR1( mr1_val )          \
      HWIO_OUT( UART2_MR1, mr1_val )
  #else
    #define UIM_PRG_MR1( mr1_val )          \
      HWIO_OUT( UART3_MR1, mr1_val )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_PRG_MR2( mr2_val )          \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {  \
        HWIO_OUT( UART2_MR2, mr2_val );     \
      }else{                                \
        HWIO_OUT( UART3_MR2, mr2_val );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_PRG_MR2( mr2_val )          \
      HWIO_OUT( UART2_MR2, mr2_val )
  #else
    #define UIM_PRG_MR2( mr2_val )          \
      HWIO_OUT( UART3_MR2, mr2_val )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_PRG_IMR( imr_val )          \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {  \
        HWIO_OUT( UART2_IMR, imr_val );     \
      }else{                                \
        HWIO_OUT( UART3_IMR, imr_val );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_PRG_IMR( imr_val )          \
      HWIO_OUT( UART2_IMR, imr_val )
  #else
    #define UIM_PRG_IMR( imr_val )          \
      HWIO_OUT( UART3_IMR, imr_val )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_PRG_IPR( ipr_val )          \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {  \
        HWIO_OUT( UART2_IPR, ipr_val );     \
      }else{                                \
        HWIO_OUT( UART3_IPR, ipr_val );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_PRG_IPR( ipr_val )          \
      HWIO_OUT( UART2_IPR, ipr_val )
  #else
    #define UIM_PRG_IPR( ipr_val )          \
      HWIO_OUT( UART3_IPR, ipr_val )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_DISABLE_IRDA()                     \
        HWIO_OUT( UART2_IRDA, MSMU_IRDA_DISABLE);  \
        HWIO_OUT( UART3_IRDA, MSMU_IRDA_DISABLE)
  #elif !defined( FEATURE_UIM2 )
    #define UIM_DISABLE_IRDA()                     \
        HWIO_OUT( UART2_IRDA, MSMU_IRDA_DISABLE)
  #else
    #define UIM_DISABLE_IRDA()                     \
        HWIO_OUT( UART3_IRDA, MSMU_IRDA_DISABLE)
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_RX_WATERLEVEL( rfwr_val )   \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {  \
        HWIO_OUT( UART2_RFWR, rfwr_val );   \
      }else{                                \
        HWIO_OUT( UART3_RFWR, rfwr_val );}
  #elif !defined( FEATURE_UIM2 )
    #define UIM_RX_WATERLEVEL( rfwr_val )   \
      HWIO_OUT( UART2_RFWR, rfwr_val )
  #else
    #define UIM_RX_WATERLEVEL( rfwr_val )   \
      HWIO_OUT( UART3_RFWR, rfwr_val )
  #endif

  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_TX_WATERLEVEL( tfwr_val )                                 \
      INTLOCK( );                                                         \
      uim_tfwr_shadow[uim_drv_slot] =                                     \
        (uim_tfwr_shadow[uim_drv_slot] & (word)~(UART_TFWR_TFW_MASK)) |   \
                             ((word)((tfwr_val) & (UART_TFWR_TFW_MASK))); \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
        HWIO_OUT( UART2_TFWR, uim_tfwr_shadow[uim_drv_slot] );            \
      }else{                                                              \
        HWIO_OUT( UART3_TFWR, uim_tfwr_shadow[uim_drv_slot] );}           \
      INTFREE( )
  #elif !defined( FEATURE_UIM2 )
    #define UIM_TX_WATERLEVEL( tfwr_val )                                 \
      INTLOCK( );                                                         \
      uim_tfwr_shadow[UIM_DRV_SLOT1] =                                    \
        (uim_tfwr_shadow[UIM_DRV_SLOT1] & (word)~(UART_TFWR_TFW_MASK)) |  \
                             ((word)((tfwr_val) & (UART_TFWR_TFW_MASK))); \
      HWIO_OUT( UART2_TFWR, uim_tfwr_shadow[UIM_DRV_SLOT1] );             \
      INTFREE( )
  #else
    #define UIM_TX_WATERLEVEL( tfwr_val )                                 \
      INTLOCK( );                                                         \
      uim_tfwr_shadow[UIM_DRV_SLOT2] =                                    \
        (uim_tfwr_shadow[UIM_DRV_SLOT2] & (word)~(UART_TFWR_TFW_MASK)) |  \
                             ((word)((tfwr_val) & (UART_TFWR_TFW_MASK))); \
      HWIO_OUT( UART3_TFWR, uim_tfwr_shadow[UIM_DRV_SLOT2] );             \
      INTFREE( )
  #endif
  #ifdef FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG
    #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
      #define UIM_UART_CNFG( cnfg_mask, cnfg_val )                  \
        INTLOCK( );                                                 \
        uim_uart_shadow[uim_drv_slot] =                             \
          (uim_uart_shadow[uim_drv_slot] & (dword)~(cnfg_mask)) |   \
                             ((dword)((cnfg_val) & (cnfg_mask)));   \
        if (UIM_DRV_SLOT1 == uim_drv_slot) {                        \
          HWIO_OUT( UART2_SIM_CFG, uim_uart_shadow[uim_drv_slot] ); \
        }else{                                                      \
          HWIO_OUT( UART3_SIM_CFG, uim_uart_shadow[uim_drv_slot] );}\
          INTFREE( )
    #elif !defined( FEATURE_UIM2 )
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
        uim_uart_shadow[UIM_DRV_SLOT2] =                            \
          (uim_uart_shadow[UIM_DRV_SLOT2] & (dword)~(cnfg_mask)) |  \
                               ((dword)((cnfg_val) & (cnfg_mask))); \
        HWIO_OUT( UART3_SIM_CFG, uim_uart_shadow[UIM_DRV_SLOT2] );  \
        INTFREE( )
    #endif  
  #else
    #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
      #define UIM_UART_CNFG( cnfg_mask, cnfg_val )                  \
        INTLOCK( );                                                 \
        uim_uart_shadow[uim_drv_slot] =                             \
          (uim_uart_shadow[uim_drv_slot] & (word)~(cnfg_mask)) |    \
                               ((word)((cnfg_val) & (cnfg_mask)));  \
        if (UIM_DRV_SLOT1 == uim_drv_slot) {                        \
          HWIO_OUT( UART2_SIM_CFG, uim_uart_shadow[uim_drv_slot] ); \
        }else{                                                      \
          HWIO_OUT( UART3_SIM_CFG, uim_uart_shadow[uim_drv_slot] );}\
        INTFREE( )
    #elif !defined( FEATURE_UIM2 )
      #define UIM_UART_CNFG( cnfg_mask, cnfg_val )                  \
        INTLOCK( );                                                 \
        uim_uart_shadow[UIM_DRV_SLOT1] =                            \
          (uim_uart_shadow[UIM_DRV_SLOT1] & (word)~(cnfg_mask)) |   \
                               ((word)((cnfg_val) & (cnfg_mask)));  \
        HWIO_OUT( UART2_SIM_CFG, uim_uart_shadow[UIM_DRV_SLOT1] );  \
        INTFREE( )
    #else
      #define UIM_UART_CNFG( cnfg_mask, cnfg_val )                  \
        INTLOCK( );                                                 \
        uim_uart_shadow[UIM_DRV_SLOT2] =                            \
          (uim_uart_shadow[UIM_DRV_SLOT2] & (word)~(cnfg_mask)) |   \
                               ((word)((cnfg_val) & (cnfg_mask)));  \
        HWIO_OUT( UART3_SIM_CFG, uim_uart_shadow[UIM_DRV_SLOT2] );  \
        INTFREE( )
    #endif
  #endif /* FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG */
  
  #ifndef FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG 
    #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
      #define UIM_BLOCK_MODE( )                                                 \
        INTLOCK( );                                                             \
        uim_tfwr_shadow[uim_drv_slot] |= (UART_TFWR_TX_RX_MODE_MASK);           \
        if (UIM_DRV_SLOT1 == uim_drv_slot) {                                    \
          HWIO_OUT( UART2_TFWR, uim_tfwr_shadow[uim_drv_slot] );                \
        }else{                                                                  \
          HWIO_OUT( UART3_TFWR, uim_tfwr_shadow[uim_drv_slot] );                \
        }                                                                       \
        INTFREE( )
    #elif !defined( FEATURE_UIM2 )
      #define UIM_BLOCK_MODE( )                                                 \
        INTLOCK( );                                                             \
        uim_tfwr_shadow[UIM_DRV_SLOT1] |= (UART_TFWR_TX_RX_MODE_MASK);          \
        HWIO_OUT( UART2_TFWR, uim_tfwr_shadow[UIM_DRV_SLOT1] );                 \
        INTFREE( )
    #else
      #define UIM_BLOCK_MODE( )                                                 \
        INTLOCK( );                                                             \
        uim_tfwr_shadow[UIM_DRV_SLOT2] |=  (UART_TFWR_TX_RX_MODE_MASK);         \
        HWIO_OUT( UART3_TFWR, uim_tfwr_shadow[UIM_DRV_SLOT2] );                 \
        INTFREE( )
    #endif
  #endif /* !FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG */

  #if defined( FEATURE_UIM_VOLTAGE_TRANSLATOR )
    #define UIM_1_8V_PROG       0x0000
    #define UIM_3V_PROG         0x0001

    #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
      #define UIM_PROG_VOLTAGE_CLASS( voltage_ctl ) \
        (UIM_DRV_SLOT1 == uim_drv_slot)           ? \
        outp( 0x02010002C, voltage_ctl )          : \
        outp( 0x02010002C, voltage_ctl )
    #elif !defined( FEATURE_UIM2 )
      #define UIM_PROG_VOLTAGE_CLASS( voltage_ctl ) \
        outp( 0x02010002C, voltage_ctl )
    #else
      #define UIM_PROG_VOLTAGE_CLASS( voltage_ctl ) \
        outp( 0x02010002C, voltage_ctl )
    #endif

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

    /* Assumes that any dual slot would have PIMIC on UIM1
     * and a voltage translator on UIM2
     */
    #if defined(FEATURE_UIM1) && defined(FEATURE_UIM2)
      #define UIM_PROG_VOLTAGE_CLASS( voltage_ctl )                       \
      if (UIM_DRV_SLOT1 == uim_drv_slot) {                                \
        (void)pm_vreg_set_level( PM_VREG_RUIM_ID, voltage_ctl );          \
      }else{                                                              \
        BIO_OUT( BIO_UIM_PWR_REG, BIO_UIM_PWR_M,                          \
        ((voltage_ctl==UIM_3V_PROG)?BIO_UIM_PWR_OFF_V:BIO_UIM_PWR_ON_V));}
    #else

      #ifdef T_QSC6270
         /* 1. Please refer to 80-VB831-13 Rev. A, USIM HW/SW Document for LCU.
            The problem the MSM has with using the RUIM regulator for its 
            pads is that with no, or an unstable supply voltage, the output 
            driver cannot guarantee a low during power-up. The MSM cannot 
            run with the power at 0V, but the USIM must see 0V before it 
            boots up. The USIM signals need to remain low through system 
            power-on and off. The proposed solution decouples the pad and 
            USIM power without adding an extra LDO.

            2. AUX module is that module that connects MSM_VDD_PAD to 
            different voltage sources (VREG_MSME1, VREG_RUIM in our case).

            3. Power up steps:
                (Before all these below, the PMIC might have enabled 
                 VREG_MSME1 in its power-on sequence. PMIC power-on sequence
                 should be happening much before the UIM power-on)
             a. Enable the AUX module 
             b. Set VREG_RUIM to the desired voltage (1.8V / 3.0V)
             c. Busy wait for 100us (some settling time)
             d. Switch VREG_RUIM_PAD to VREG_RUIM
             e. Busy wait for 100us (some settling time)
             f. Connect MSM_VDD_PAD to VREG_RUIM (PM_USIM_AUX_CTRL_VREG_RUIM) */
            
         #define UIM_PROG_VOLTAGE_CLASS(voltage_ctl)                     \
          (void)pm_usim_aux_enable(PM_ON_CMD);                          \
          (void)pm_vreg_set_level( PM_VREG_USIM_ID, voltage_ctl );      \
           uim_clk_busy_wait(100);                                        \
          (void)pm_vreg_control( PM_ON_CMD, PM_VREG_USIM_M );           \
           uim_clk_busy_wait(100);                                        \
          (void)pm_usim_aux_vreg_set(PM_USIM_AUX_CTRL_VREG_RUIM)        
      #else
         #define UIM_PROG_VOLTAGE_CLASS( voltage_ctl ) \
          (void)pm_vreg_set_level( PM_VREG_RUIM_ID, voltage_ctl )
      #endif /* T_QSC6270 */
    #endif /* defined(FEATURE_UIM1) && defined(FEATURE_UIM2) */
  #endif /* FEATURE_UIM_VOLTAGE_TRANSLATOR */

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

#elif defined(T_MSM3) || defined(T_MSM5000)
#error code not present
#elif defined(T_UIM_UART1_USIM_CFG)
#error code not present
#else
#error code not present
#endif

#if defined(T_MSM33) || defined(T_UIM_SIM_GLUE_LOGIC) || defined(T_UIM_SABER_DEFS) || defined(T_MSM6275) || defined(T_MSM8650A)
  #undef UIM_NO_SIM_GLUE_LOGIC
  #define UIM_PROCESS_PARITY_ERROR_RETURN(  ) \
     return
  #define UIM_PROCESS_PARITY_ERROR_CONT(  ) \
     continue
#else /* MSM3000, MSM3100, MSM5000 */
  #define UIM_NO_SIM_GLUE_LOGIC
  #define UIM_PROCESS_PARITY_ERROR_RETURN(  ) \
     uim_reset_receiver ();                   \
     uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE; \
     return
  #define UIM_PROCESS_PARITY_ERROR_CONT(  ) \
     uim_reset_receiver ();                 \
     uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE; \
     return
#endif /* T_MSM33 */

#if defined(UIM_NO_SIM_GLUE_LOGIC) &&  \
          defined( FEATURE_UIM_SUPPORT_INVERSE_CONVENTION )

  /* Set even parity */
  #define UIM_PROGRAM_DIRECT_CONVENTION(  )                     \
    UIM_PRG_MR2 ( MSMU_MR2_8BPC | MSMU_MR2_2SB | MSMU_MR2_EPAR);\
    uim_convention[uim_drv_slot] = UIM_DIRECT_CONVENTION

  /* Set odd parity */
  #define UIM_PROGRAM_INVERSE_CONVENTION(  )                    \
    UIM_PRG_MR2 ( MSMU_MR2_8BPC | MSMU_MR2_2SB | MSMU_MR2_OPAR);\
    uim_convention[uim_drv_slot] = UIM_INVERSE_CONVENTION

  /* Decide if need to convert the received byte */
  #define UIM_GET_RX_BYTE()                                    \
    ((UIM_INVERSE_CONVENTION == uim_convention[uim_drv_slot]) ? \
        change_convention[ UIM_READ_RX_FIFO()] : UIM_READ_RX_FIFO())

#else

  #define UIM_GET_RX_BYTE()  ( UIM_READ_RX_FIFO() & 0xFF )

#endif /* defined(UIM_NO_SIM_GLUE_LOGIC) &&
          defined( FEATURE_UIM_SUPPORT_INVERSE_CONVENTION ) */

#ifdef T_QSC6270
  #define UIM_TRAMP_SET_ISR() tramp_set_isr( TRAMP_UART2DM_IRQ, uim_rx_isr_1);
#else
  #define UIM_TRAMP_SET_ISR() tramp_set_isr( TRAMP_UART2_ISR, uim_rx_isr_1);
#endif /* T_QSC6270 */

#if defined(T_QSC60X5) && !defined(T_QSC6270)
  /* This PWR Disable line should only be configured for SC-2X as a special HW Requirement */
  #define UIM_UIMDRV_PWR_DIS() gpio_tlmm_config(GPIO_OUTPUT_10);
#else
  #define UIM_UIMDRV_PWR_DIS()
#endif /* defined(T_QSC60X5) && !defined(T_QSC6270) */

#ifdef T_QSC6270
/*===========================================================================

FUNCTION uim_bytes_waiting_in_txfifo

DESCRIPTION
  This procedure determines how many bytes are waiting to be sent in the
  TX FIFO by querying the TXFS register.

DEPENDENCIES
  None

RETURN VALUE
  Number of byte present in the TX FIFO which are yet to be read.

SIDE EFFECTS
  None

===========================================================================*/
extern int32 uim_bytes_waiting_in_txfifo(void);
#endif /* T_QSC6270 */

#endif /* FEATURE_UIM_QTF */

#endif /* UIMLEGACYDRV_H */

