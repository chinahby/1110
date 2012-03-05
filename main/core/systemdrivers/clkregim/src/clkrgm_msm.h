#ifndef CLKRGM_QSC_1100_H
#define CLKRGM_QSC_1100_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     Q S C 1 1 0 0   C L O C K   R E G I M E   D E V I C E   D R I V E R

DESCRIPTION

  This header file contains the public interface for the clock regime device
  driver module.

EXTERNALIZED FUNCTIONS

  clk_regime_init
    Initialize module data structures and hardware registers.

  clk_regime_msm_reset
    Reset specified hardware clock regime.

  clk_regime_msm_enable
    Enable specified hardware clock regime.

  clk_regime_msm_disable
    Disable specified hardware clock regime.

  clk_regime_msm_is_on
    Test if clock regime is on.

  clk_regime_msm_sel_clk_src
    Select input clock source for specified hardware clock regime.

  clk_regime_msm_sel_clk_div
    Select clock divider for specified hardware clock regime.

  clk_regime_msm_sel_clk_inv
    Select output clock polarity for specified hardware clock regime.

  clk_regime_set_mclk_sleep
    Switch processor clock to SLEEP XTAL frequency.

  clk_regime_switch_mclk
    Switch processor clock to another speed.

  clk_regime_set_dload_mclk
    Switch processor clock to downloader frequency.

  clk_regime_set_mclk_hi_clk
    Switch processor clock to full speed frequency.

  clk_regime_get_NAND_wait_states
    Return number of wait states used for NAND flash memory controller.

  clk_regime_get_max_hclk_freq_khz
    Return frequency in KHz of the maximum HCLK frequency

  clk_regime_get_i2c_clk_khz
    Return the i2c frequency in KHZ

  clk_regime_icodec_clk_disable
    Disable icodec hardware clock regime.

  clk_regime_sel_icodec_clk
    Select clock frequency for icodec hardware clock regime.

  clk_regime_sel_sdac_clk
    Select clock frequency for sdac hardware clock regime.

  clk_regime_usb_xtal_off
    Disable USB crystal oscillator circuit.

  clk_regime_usb_xtal_on
    Enable USB crystal oscillator circuit.

  clk_regime_uP_sleep
    Command processor hardware to enter sleep state.

  config_tcxo_off
    Disable TCXO oscillator hardware.

  clk_regime_msm_sel_uart_dm_clk
    Configure MNCNTR UART1_DM_FROMMN_CLK

INITIALIZATION AND SEQUENCING REQUIREMENTS

  The clock regime initialization function must be executed prior to calling
  any of the other functions defined.

Copyright (c) 2003-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //depot/asic/qsc1100/drivers/clkregim/clkrgm_msm.h#29 $
$DateTime: 2010/12/28 15:01:23 $
$Author: kmaliped $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/09    br     Added support for EFUSE Clock regime.
12/04/08    hvo    Reduced XO warmup time
11/20/08    hvo    Fixed Array over flow, increased CLK_RGM_MAX_REGIMES
07/11/08    kma    Added prototype for clk_regime_switch_qdsp_clk_internal
08/22/08    hvo    Changed CLKRGM_MCPU_LOW_SPEED from CLKRGM_CPU_32P0_DIV1 to
                   CLKRGM_CPU_38P4_DIV1
04/18/08    bc     Added CLK_RGM_MDDI_CLIENT_VFE_M set to not supported.
10/11/07    bc     Initial port form QSC6055v3202.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "msm.h"
#include "diagpkt.h"

/* For the time being, include the following to export the definition of the
   CLKRGM_HCLK_FREQUENCY_IN_MHZ symbol, which is needed for the clk_busy_wait
   macro that is used by the boot loader program and also the flash download
   tools. */

#ifdef BOOT_LOADER

  #include CLKRGM_H

#endif

#include CLK_RGM_MODEM_H

#define CLKRGM_HAS_SLEEP


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Constant used by SLEEP to determine how much time to shave off the wakeup timer.
 * SLEEP has to wakeup early to account for TCXO warmup and the overhead of TCXO
 * shutdown code.
 */

/* TBP : Port this value */
#define CLKRGM_TCXO_WARMUP_AND_SWITCH_OVERHEAD_MS  ( 10 )
#define CLKRGM_TCXO_WARMUP_AND_SWITCH_OVERHEAD_US  (CLKRGM_TCXO_WARMUP_AND_SWITCH_OVERHEAD_MS * 1000)

/* --------------------------------------------------------------------------
   Clock regimes that can be enabled, disabled and reset.

   The clock regimes that can be explicitly enabled, disabled and reset are
   listed below.
-------------------------------------------------------------------------- */

/* Enum of clock regimes that can be enabled and disabled. This is a subset
   of the complete set of hardware clock regimes. 
   The order of the items in this enum is not important. */

typedef enum
{
  CLK_RGM_GEN_M = 0x0,          /* General TCXO/4                    */

  /* ARM and Bus Clocks */
  CLK_RGM_MCLK_M,               /* Micro (ARM) clock                 */
  CLK_RGM_HCLK_M,               /* Bus (AHB) clock                   */
  CLK_RGM_ETM_M,                /* ETM                               */
  CLK_RGM_UXMC_M,               /* UXMC                              */
  CLK_RGM_EBI1_CLK_M,           /* EBI1 clock                        */
  CLK_RGM_EBI2_CLK_M,           /* EBI2 clock                        */
  CLK_RGM_IMEM_CLK_M,           /* IMEM clock                        */
  
  /* Perpherals */
  CLK_RGM_SBI_M,                /* PMIC SSBI                         */
  CLK_RGM_UART0_M,              /* UART on GSBI0                     */
  CLK_RGM_UART1_M,              /* UART on GSBI1                     */
  CLK_RGM_UART2_M,              /* UART on GSBI2                     */
  CLK_RGM_UART_LEGACY_M,        /* 2-pin UART muxed on GPIO 25,27    */
  
  CLK_RGM_I2C0_M,               /* I2C bus on GSBI0                  */
  CLK_RGM_I2C1_M,               /* I2C bus on GSBI1                  */
  CLK_RGM_I2C2_M,               /* I2C bus on GSBI2                  */
  CLK_RGM_I2C_LEGACY_M,         /* I2C bus on GPIO 39,40             */

  CLK_RGM_SPI0_M,               /* SPI bus on GSBI0                  */
  CLK_RGM_SPI1_M,               /* SPI bus on GSBI1                  */
  CLK_RGM_SPI2_M,               /* SPI bus on GSBI2                  */

  CLK_RGM_GSBI0_M,              /* GSBI0 (superset of UART0,SPI0,I2C0) */
  CLK_RGM_GSBI1_M,              /* GSBI1 (superset of UART1,SPI1,I2C1) */
  CLK_RGM_GSBI2_M,              /* GSBI2 (superset of UART2,SPI2,I2C2) */

  CLK_RGM_USB_MCLK_M,           /* USB                               */
  CLK_RGM_USB_AHB_CLK_M,        /* USB AHB Bus                       */
  CLK_RGM_USB_M,                /* Combined USB clock regimes        */

  CLK_RGM_SDCC_MCLK_M,          /* SDCC MCLK (core)                  */
  CLK_RGM_SDCC_HCLK_M,          /* SDCC HCLK (AHB/Bus)               */

  /* ADSP, Audio, Video */
  CLK_RGM_ADSP_M,               /* Application DSP                   */
  CLK_RGM_ADSP_VIDEO_INTF_M,               /* Application DSP Video Intf  */
  CLK_RGM_ICODEC_M,             /* Internal CODEC                    */
  CLK_RGM_ICODEC_RX_M,          /* Internal CODEC receive            */
  CLK_RGM_ICODEC_TX_M,          /* Internal CODEC transmit           */
  CLK_RGM_ECODEC_M,             /* External CODEC                    */
  CLK_RGM_CAMCLK_M,             /* Camera clock                      */
  CLK_RGM_CAMIF_PCLK_M,         /* Camif pclk                        */
  CLK_RGM_VIDEO_VFE_M,          /* Video Front End                   */
  CLK_RGM_VIDEO_M,              /* Video AHB clock                   */
  CLK_RGM_AUDIO_M,              /* Audio AHB clock                   */
  CLK_RGM_RFMS_XO_CLK_M,
  CLK_RGM_MDP_M,                /* MDP                               */
  CLK_RGM_SDAC_M,               /* I2S clock                         */
  CLK_RGM_I2S_M = CLK_RGM_SDAC_M, /* I2S clock name from QSC6270     */

  /* Other, Misc */
  CLK_RGM_SEC_HCLK_M,           /* security block                    */
  CLK_RGM_QMEMBIST_M,           /* QMEMBIST core                     */
  CLK_RGM_EFUSE_CLK,            /* Efuse Clock                       */
  CLK_RGM_MSM_NUM_REGIMES,      /* Number of items in this enum      */
  #define CLK_RGM_NOT_SUPPORTED 0xff  /* Designator for unsupported items  */
  /* Used a define to satify klocwork, no big enum values now. */
} clkrgm_msm_clk_type;

#define clk_regime_type int

#define CLK_RGM_MAX_REGIMES  0xff

#define CLK_RGM_COD_M             CLK_RGM_ECODEC_M
#define CLK_RGM_COD_CORE_M        CLK_RGM_ICODEC_M
#define CLK_RGM_VOC_M             CLK_RGM_ADSP_M
#define CLK_RGM_RUIM1_M           CLK_RGM_UART0_M

#define CLK_RGM_USB_OTG_M         CLK_RGM_USB_AHB_CLK_M

/* LEGACY CLOCK NAME SUPPORT */

#define CLK_RGM_I2C_M             CLK_RGM_I2C_LEGACY_M

/* Allow, and ignore MDDI clock requests at camera request */
#define CLK_RGM_MDDI_CLIENT_VFE_M CLK_RGM_NOT_SUPPORTED

/* Requested for SIO driver support */
#define CLKRGM_UART1DM_CLK        CLK_RGM_UART1_M

#ifdef FEATURE_GSBI1_UART1
#define CLK_RGM_UART_M CLK_RGM_UART1_M
#else
#define CLK_RGM_UART_M CLK_RGM_UART_LEGACY_M
#endif

/* Miscellaneous wait times after clock enable/disable */
  #define CLKRGM_SPI_PAUSE 1
  #define CLKRGM_GSBI_PAUSE 1


/* --------------------------------------------------------------------------
   Clock regimes with selectable clock input sources.

   The clock regimes that have configurable input sources are listed below
   together with the clock input source options for each clock regime.  The
   desired clock input option and clock regime from the list below must be
   passed as a parameters to the clk_regime_msm_sel_clk_src() function to switch
   the clock input source for a specified clock regime.
-------------------------------------------------------------------------- */

/* Enum of clock regimes with configurable input sources. This is a subset of
   the complete set of hardware clock regimes. The order of the items in this
   enum must be preserved since the items are used as array indices. */

typedef enum
{
  CLK_RGM_SRC_SEL_ADSP = 0x0,
  CLK_RGM_SRC_SEL_TCXO4,
  CLK_RGM_SRC_SEL_GSBI0,
  CLK_RGM_SRC_SEL_G_UART0 = CLK_RGM_SRC_SEL_GSBI0,
  CLK_RGM_SRC_SEL_G_I2C0 = CLK_RGM_SRC_SEL_GSBI0,
  CLK_RGM_SRC_SEL_G_SPI0 = CLK_RGM_SRC_SEL_GSBI0,
  CLK_RGM_SRC_SEL_GSBI1,
  CLK_RGM_SRC_SEL_G_UART1 = CLK_RGM_SRC_SEL_GSBI1,
  CLK_RGM_SRC_SEL_G_I2C1 = CLK_RGM_SRC_SEL_GSBI1,
  CLK_RGM_SRC_SEL_G_SPI1 = CLK_RGM_SRC_SEL_GSBI1,
  CLK_RGM_SRC_SEL_GSBI2,
  CLK_RGM_SRC_SEL_G_UART2 = CLK_RGM_SRC_SEL_GSBI2,
  CLK_RGM_SRC_SEL_G_I2C2 = CLK_RGM_SRC_SEL_GSBI2,
  CLK_RGM_SRC_SEL_G_SPI2 = CLK_RGM_SRC_SEL_GSBI2,
  CLK_RGM_SRC_SEL_UART_LEGACY,
  CLK_RGM_SRC_SEL_I2C_LEGACY,
  CLK_RGM_SRC_SEL_USB,
  CLK_RGM_SRC_SEL_MDDI_CLIENT_VFE,
  CLK_RGM_SRC_SEL_CAMIF_PCLK,
  CLK_RGM_MSM_SRC_SEL_NUM_REGIMES,
  CLK_RGM_SRC_SEL_NOT_SUPPORTED = 0xff
} clkrgm_msm_src_sel_type;

/* Support of previous clock source names */
#define CLK_RGM_SRC_SEL_CAM_PCLK        CLK_RGM_SRC_SEL_CAMIF_PCLK

/* Map the UART1 clock source */
#if defined(FEATURE_GSBI0_UART1)
#define CLK_RGM_SRC_SEL_UART1           CLK_RGM_SRC_SEL_G_UART0
#elif defined(FEATURE_GSBI1_UART1)
#define CLK_RGM_SRC_SEL_UART1           CLK_RGM_SRC_SEL_G_UART1
#elif defined(FEATURE_GSBI2_UART1)
#define CLK_RGM_SRC_SEL_UART1           CLK_RGM_SRC_SEL_G_UART2
#else
#define CLK_RGM_SRC_SEL_UART1           CLK_RGM_SRC_SEL_UART_LEGACY
#endif

/* Map the UART2 clock source */
#if defined(FEATURE_GSBI0_UART2) || defined(FEATURE_GSBI0_RUIM)
#define CLK_RGM_SRC_SEL_UART2           CLK_RGM_SRC_SEL_G_UART0
#elif defined(FEATURE_GSBI1_UART2) || defined(FEATURE_GSBI1_RUIM)
#define CLK_RGM_SRC_SEL_UART2           CLK_RGM_SRC_SEL_G_UART1
#elif defined(FEATURE_GSBI2_UART2) || defined(FEATURE_GSBI2_RUIM)
#define CLK_RGM_SRC_SEL_UART2           CLK_RGM_SRC_SEL_G_UART2
#else
#define CLK_RGM_SRC_SEL_UART2           CLK_RGM_SRC_SEL_UART_LEGACY
#endif

/* Map the UART3 clock source */
#if defined(FEATURE_GSBI0_UART3)
#define CLK_RGM_SRC_SEL_UART3           CLK_RGM_SRC_SEL_G_UART0
#elif defined(FEATURE_GSBI1_UART3)
#define CLK_RGM_SRC_SEL_UART3           CLK_RGM_SRC_SEL_G_UART1
#elif defined(FEATURE_GSBI2_UART3)
#define CLK_RGM_SRC_SEL_UART3           CLK_RGM_SRC_SEL_G_UART2
#else
#define CLK_RGM_SRC_SEL_UART3           CLK_RGM_SRC_SEL_UART_LEGACY
#endif

#define CLK_RGM_SRC_SEL_HKADC           CLK_RGM_SRC_SEL_NOT_SUPPORTED
#define CLK_RGM_SRC_SEL_MMCDIV          CLK_RGM_SRC_SEL_NOT_SUPPORTED
#define CLK_RGM_SRC_SEL_SBI             CLK_RGM_SRC_SEL_NOT_SUPPORTED
#define CLK_RGM_SRC_SEL_MMCC            CLK_RGM_SRC_SEL_NOT_SUPPORTED
#define CLK_RGM_SRC_SEL_USBC            CLK_RGM_SRC_SEL_NOT_SUPPORTED

/* The following are the clock input source selections for each clock regime
   listed in the enum type above.  Note that these are compiler pre-processor
   symbols that are defined to the values to be written to the corresponding
   hardware register field, so no type checking can be performed on the
   corresponding argument to the clkrgm_msm_sel_clk_src() function. */


/* GSBI clock source selections. */

#define CLK_RGM_SRC_SEL_GSBI_EXT_CLK_V   HWIO_GSBI0_CD_REG_SRC_SEL_EXT_CLK_FVAL
#define CLK_RGM_SRC_SEL_GSBI_XO_V        HWIO_GSBI0_CD_REG_SRC_SEL_XO_FVAL
#define CLK_RGM_SRC_SEL_GSBI_PLL_V       HWIO_GSBI0_CD_REG_SRC_SEL_PLL_SRC_FVAL
#define CLK_RGM_SRC_SEL_GSBI_SLEEP_CLK_V HWIO_GSBI0_CD_REG_SRC_SEL_SLEEP_CLK_FVAL

/* UART1 clock source selections */
#define CLK_RGM_SRC_SEL_UART1_TCXO_V   HWIO_UART_DM_CD_REG_SRC_SEL_XO_FVAL
#define CLK_RGM_SRC_SEL_UART1_PLL0_V   HWIO_UART_DM_CD_REG_SRC_SEL_PLL_SRC_FVAL
#define CLKRGM_UART_CLK_SRC_TCXO_CORE_IN CLK_RGM_SRC_SEL_UART1_TCXO_V
#define CLKRGM_UART_CLK_SRC_MODEM_PLL    CLK_RGM_SRC_SEL_UART1_PLL0_V

/* UART2 clock source selections. */

#define CLK_RGM_SRC_SEL_UART2_TCXO_V      HWIO_UART_DM_CD_REG_SRC_SEL_XO_FVAL
#define CLK_RGM_SRC_SEL_UART2_PLL0_V      HWIO_UART_DM_CD_REG_SRC_SEL_PLL_SRC_FVAL
#define CLK_RGM_SRC_SEL_UART2_EXTCLK_V    HWIO_UART_DM_CD_REG_SRC_SEL_EXT_CLK_FVAL
#define CLK_RGM_SRC_SEL_UART2_SLEEP_CLK_V HWIO_UART_DM_CD_REG_SRC_SEL_SLEEP_CLK_FVAL
/* Special case handling will be needed for TCXODIV4 because that is not
   a clock source choice any longer.  The MND 1/4 will be needed */
#define CLK_RGM_UART2_SRC_TCXODIV4_V      0x1D4
#define CLK_RGM_SRC_SEL_UART2_TCXODIV4_V  CLK_RGM_UART2_SRC_TCXODIV4_V 


#define CLK_RGM_SRC_SEL_MDDI_CLIENT_VFE_SRC_CAMIF_V      \
    HWIO_FVAL(VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_SRC_SEL, CAMIF_PCLK)

#define CLK_RGM_SRC_SEL_MDDI_CLIENT_VFE_SRC_CAMCLK_V     \
    HWIO_FVAL(VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_SRC_SEL, CAMCLK_PO)

  /* CAM_PCLK_SRC_SEL */
#define CLK_RGM_SRC_SEL_CAMIF_PCLK_SRC_SHADOW_CLOCK       \
    HWIO_CAMCLK_CD_REG_CAM_PCLK_SRC_SEL_TCB_SHADOW_TCK_FVAL
#define CLK_RGM_SRC_SEL_CAMIF_PCLK_SRC_NO_CLOCK           \
    HWIO_CAMCLK_CD_REG_CAM_PCLK_SRC_SEL_NO_CLOCK_FVAL
#define CLK_RGM_SRC_SEL_CAMIF_PCLK_SRC_CC_PCLK            \
    HWIO_CAMCLK_CD_REG_CAM_PCLK_SRC_SEL_VIDEO_VFE_CLK_FVAL
#define CLK_RGM_SRC_SEL_CAMIF_PCLK_SRC_TLMM_CAM_PCLK      \
    HWIO_CAMCLK_CD_REG_CAM_PCLK_SRC_SEL_TLMM_CAM_PCLK_FVAL

#define CLK_RGM_SRC_SEL_CAM_PCLK_GND_V       CLK_RGM_SRC_SEL_CAMIF_PCLK_SRC_NO_CLOCK
#define CLK_RGM_SRC_SEL_CAM_PCLK_VIDEO_VFE_V CLK_RGM_SRC_SEL_CAMIF_PCLK_SRC_CC_PCLK
#define CLK_RGM_SRC_SEL_CAM_PCLK_CAM_PCLK_V  CLK_RGM_SRC_SEL_CAMIF_PCLK_SRC_TLMM_CAM_PCLK

/* USB clock source selections. */

#define CLK_RGM_SRC_SEL_USB_SRC_TCXO_V                   \
        HWIO_USB_CD_REG_SRC_SEL_XO_FVAL
#define CLK_RGM_SRC_SEL_USB_SRC_SLEEP_CLK_V              \
        HWIO_USB_CD_REG_SRC_SEL_SLEEP_CLK_FVAL
#define CLK_RGM_SRC_SEL_USB_SRC_EXT_CLK1_V               \
        HWIO_USB_CD_REG_SRC_SEL_EXT_CLK_FVAL
#define CLK_RGM_SRC_SEL_USB_SRC_PLL0_OUT_V               \
        HWIO_USB_CD_REG_SRC_SEL_PLL_SRC_FVAL

#define CLK_RGM_SRC_SEL_I2C CLK_RGM_SRC_SEL_I2C_LEGACY

#if 0
/* Different source values are needed if I2C is in GSBI or not */
  /* I2C is on a GSBI device */
  #define CLK_RGM_SRC_SEL_I2C_TCXO_V                       \
          HWIO_GSBI0_CD_REG_SRC_SEL_XO_FVAL
  #define CLK_RGM_SRC_SEL_I2C_SLEEP_CLK_V                  \
          HWIO_GSBI0_CD_REG_SRC_SEL_SLEEP_CLK_FVAL
  #define CLK_RGM_SRC_SEL_I2C_TCXODIV4_V                   \
          CLK_RGM_UART2_SRC_TCXODIV4_V      
  #define CLK_RGM_SRC_SEL_I2C_GND_TIE_V                    \
          CLKRGM_UNSUPPORTED_ON_QSC1100
#endif

  /* I2C is on the I2C legacy device */
  #define CLK_RGM_SRC_SEL_I2C_TCXO_V                       \
          HWIO_MISC_CFG_REG_I2C_SRC_SEL_XO_FVAL
  #define CLK_RGM_SRC_SEL_I2C_SLEEP_CLK_V                  \
          HWIO_MISC_CFG_REG_I2C_SRC_SEL_SLEEP_CLK_FVAL
  #define CLK_RGM_SRC_SEL_I2C_TCXODIV4_V                   \
          HWIO_MISC_CFG_REG_I2C_SRC_SEL_XO4_FVAL
  #define CLK_RGM_SRC_SEL_I2C_GND_TIE_V                    \
          CLKRGM_UNSUPPORTED_ON_QSC1100


/* --------------------------------------------------------------------------
   Clock regimes with configurable input clock dividers.

   The clock regimes that have configurable clock dividers are listed below
   together with the clock divider options for each clock regime.  The
   desired clock divider option and clock regime from the list below must be
   passed as a parameters to the clkrgm_msm_sel_clk_div() function to switch
   the clock divider value for a specified clock regime.
-------------------------------------------------------------------------- */

/* Enum of clock regimes with configurable clock dividers. This is a subset of
   the complete set of hardware clock regimes. The order of the items in this
   enum must be preserved since the items are used as array indices. */

#if 0
  /* This function does not appear to be used on QSC1100, and porting
     will be problematic, so I am removing it */

typedef enum
{
  CLK_RGM_DIV_SEL_I2C0 = 0x0,
  CLK_RGM_DIV_SEL_I2C1,
  CLK_RGM_DIV_SEL_I2C2,
  CLK_RGM_DIV_SEL_I2C_LEGACY,
  CLK_RGM_DIV_SEL_MMCC,
  CLK_RGM_DIV_SEL_USB_SCLK,
  CLK_RGM_DIV_SEL_USB_MCLK,
  CLK_RGM_MSM_DIV_SEL_NUM_REGIMES,
  CLK_RGM_DIV_SEL_NOT_SUPPORTED = 0xff
} clkrgm_msm_src_div_type;
#endif

/* The following are the clock input source selections for each clock regime
   listed in the enum type above.  For clock regimes that have a range of
   divider values that translate directly, the explicit definitions of these
   values are omitted (I2C and MMCC). */

/* USB SCLK divider values. */

#define CLK_RGM_DIV_SEL_USB_SCLK_PLL3_V                  0
#define CLK_RGM_DIV_SEL_USB_SCLK_PLL3DIV2_V              1
#define CLK_RGM_DIV_SEL_USB_SCLK_PLL3DIV3_V              2
#define CLK_RGM_DIV_SEL_USB_SCLK_PLL3DIV4_V              3

/* USB MCLK divider values. */

#define CLK_RGM_DIV_SEL_USB_MCLK_DIV1_V                  0
#define CLK_RGM_DIV_SEL_USB_MCLK_DIV2_V                  1


/* --------------------------------------------------------------------------
   Clock regimes with configurable output clock signal polarities.

   The clock regimes that have configurable output clock inversion are listed
   below.  The desired clock inversion option and clock regime from the list
   below must be passed as a parameters to the clkrgm_msm_sel_clk_inv()
   function to select the polarity for a specified clock regime.
-------------------------------------------------------------------------- */

/* Enum of clock regimes whose output clock polarities can be controlled by
   software. This is a subset of the complete set of hardware clock regimes.
   The order of the items in this enum must be preserved since the items are
   used as array indices. */

typedef enum
{
  CLK_RGM_INV_SEL_USB,
  CLK_RGM_INV_SEL_BT_PCM,
  CLK_RGM_INV_SEL_ECODECIF,
  CLK_RGM_INV_SEL_RPCM,
  CLK_RGM_INV_SEL_ETM,
  CLK_RGM_INV_SEL_ADIE_CODEC,
  CLK_RGM_INV_SEL_VIDEO,
  CLK_RGM_INV_SEL_MDDI_HOST_HCLK = CLK_RGM_INV_SEL_VIDEO,
  CLK_RGM_INV_SEL_UART0,
  CLK_RGM_INV_SEL_UART1,
  CLK_RGM_INV_SEL_UART2,
  CLK_RGM_INV_SEL_UART_LEGACY,
  CLK_RGM_INV_SEL_TCXO4,
  CLK_RGM_INV_SEL_MMCDIV,
  CLK_RGM_INV_SEL_VIDEO_VFE,
  CLK_RGM_INV_SEL_MDDI_CLIENT_VFE = CLK_RGM_INV_SEL_VIDEO_VFE,
  CLK_RGM_INV_SEL_I2C0,
  CLK_RGM_INV_SEL_I2C1,
  CLK_RGM_INV_SEL_I2C2,
  CLK_RGM_INV_SEL_I2C_LEGACY,
  CLK_RGM_INV_SEL_SPI0,
  CLK_RGM_INV_SEL_SPI1,
  CLK_RGM_INV_SEL_SPI2,
  CLK_RGM_INV_SEL_ADSP,
  CLK_RGM_INV_SEL_SBI,
  CLK_RGM_INV_SEL_ICODEC,
  CLK_RGM_INV_SEL_SDAC,
  CLK_RGM_INV_SEL_MMCC,
  CLK_RGM_INV_SEL_GRAPHICS,
  CLK_RGM_INV_SEL_USB_OTG,
  CLK_RGM_INV_SEL_SDCC_HCLK,
  CLK_RGM_INV_SEL_SDCC_MCLK,
  CLK_RGM_INV_SEL_MDP_HCLK,
  CLK_RGM_INV_SEL_CAMIF_PCLK,

  CLK_RGM_MSM_INV_SEL_NUM_REGIMES,
  CLK_RGM_INV_SEL_NOT_SUPPORTED = 0xff

} clkrgm_msm_src_inv_type;

#define CLK_RGM_INV_SEL_CAM_PCLK        CLK_RGM_INV_SEL_CAMIF_PCLK


/* --------------------------------------------------------------------------
   PLL usage parameters.
-------------------------------------------------------------------------- */

  /* Enum of available PLLs. */


typedef enum
{
  CLKRGM_NOPLL,         /* no pll - mdsp/asdsp can source tcxo*/
  CLKRGM_PLL,
  CLKRGM_NUM_PLLS
} clkrgm_pll_type;

#define CLKRGM_NO_PLL CLKRGM_NOPLL

/* Operations that may be performed on a PLL. */

typedef enum
{
  CLKRGM_PLL_DISABLE = 0x0,
  CLKRGM_PLL_ENABLE
} clkrgm_pll_op_type;

/* --------------------------------------------------------------------------
   CPU speed options.
-------------------------------------------------------------------------- */
  /* Enum of clock configurations that are supported. This enum is used
     as an array index for the configuration array and should match the
     order of the set of configurations defined. */

  #ifndef _ARM_ASM_

#if defined  FEATURE_PLL_192
    
  typedef enum
  {
    CLKRGM_CPU_SLPOSC_SPEED=0, // 32.764 kHz
    CLKRGM_CPU_XO_SPEED,       // 19.2 MHz
    CLKRGM_CPU_TCXO_SPEED = CLKRGM_CPU_XO_SPEED,   /* Supporting the old name for it */
    CLKRGM_CPU_QUARTER_SPEED,  // 24 MHz - PLL/8
    CLKRGM_CPU_32P0_DIV1,      // 32 MHz - PLL/6
    CLKRGM_CPU_38P4_DIV1,      // 34.9 MHz - PLL/5
    CLKRGM_CPU_HALF_SPEED,     // 48 MHz - PLL/4
    CLKRGM_CPU_FULL_SPEED,     // 96 MHz - PLL/2

    #define CLKRGM_CPU_NUM_SPEEDS (CLKRGM_CPU_FULL_SPEED + 1) /*to be fixed like CLKRGM_CPU_FULL_SPEED + 1*/
    /* Use define for NUM_SPEEDS to block an illegal value from the enum */
    /* And repair klocwork concerns */

  } clkrgm_cpu_speed_type;
 
#elif defined FEATURE_PLL_384

typedef enum
  {
    CLKRGM_CPU_SLPOSC_SPEED=0, // 32.764 kHz
    CLKRGM_CPU_XO_SPEED,       // 19.2 MHz
    CLKRGM_CPU_TCXO_SPEED = CLKRGM_CPU_XO_SPEED,   /* Supporting the old name for it */
    CLKRGM_CPU_QUARTER_SPEED,  // 24 MHz - PLL/8
    CLKRGM_CPU_32P0_DIV1,      // 32 MHz - PLL/6
    CLKRGM_CPU_38P4_DIV1,      // 34.9 MHz - PLL/5
    CLKRGM_CPU_HALF_SPEED,     // 48 MHz - PLL/4
    CLKRGM_CPU_FULL_SPEED,     // 96 MHz - PLL/2
    CLKRGM_CPU_128_SPEED,      //added to run the cpu at 128MHz Recalculate 

    #define CLKRGM_CPU_NUM_SPEEDS (CLKRGM_CPU_128_SPEED + 1) /*CLKRGM_CPU_NUM_SPEEDS should be CLKRGM_CPU_128_SPEED + 1 hard coding for testing*/
    /* Use define for NUM_SPEEDS to block an illegal value from the enum */
    /* And repair klocwork concerns */

  } clkrgm_cpu_speed_type;

#endif


  #define clkrgm_mcpu_speed_type clkrgm_cpu_speed_type

//   #define CLKRGM_MCPU_LOW_SPEED      CLKRGM_CPU_32P0_DIV1
  #define CLKRGM_MCPU_LOW_SPEED      CLKRGM_CPU_38P4_DIV1
  #define CLKRGM_MCPU_MED_SPEED      CLKRGM_CPU_HALF_SPEED
  #define CLKRGM_MCPU_HIGH_SPEED     CLKRGM_CPU_FULL_SPEED
  #define CLKRGM_MCPU_HIGH_BUS_SPEED CLKRGM_CPU_FULL_SPEED

  #endif // _ARM_ASM_

#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
typedef enum 
{
  /* first 23 client ID used to index clk_regime_freq_plan_type array */
  CLKRGM_CPU_CLIENT_MP3_APP      = 0,
  CLKRGM_CPU_CLIENT_AAC_LC_APP,
  CLKRGM_CPU_CLIENT_AAC_SBR_APP,
  CLKRGM_CPU_CLIENT_AAC_PS_APP,
  CLKRGM_CPU_CLIENT_WMA_APP,
  CLKRGM_CPU_CLIENT_REALAUDIO_APP,
  CLKRGM_CPU_CLIENT_VIDEO_APPS,           //video decode
  CLKRGM_CPU_CLIENT_QTV_APPS,
  CLKRGM_CPU_CLIENT_QCAMCORDER_VIEWFINDER,
  CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1,
  CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD2,
  CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER,
  CLKRGM_CPU_CLIENT_QCAMERA_SNAPSHOT1,
  CLKRGM_CPU_CLIENT_QCAMERA_SNAPSHOT2,
  CLKRGM_CPU_CLIENT_QCAMERA_REVIEW,
  CLKRGM_CPU_CLIENT_GRAPHICS_APPS,
  CLKRGM_CPU_CLIENT_SRCHTC,         //  1x Voice
  CLKRGM_CPU_CLIENT_EVDO_DATA,      //  1x Data
  CLKRGM_CPU_CLIENT_CDMA_DATA_APPS, //  EVDO Data
  CLKRGM_CPU_CLIENT_HDR_VOICE,      //  Hybrid
  CLKRGM_CPU_CLIENT_GPS_SERVICES,
  CLKRGM_CPU_CLIENT_BT_VOICE,
  CLKRGM_CPU_CLIENT_ZERO_PLL,
  CLKRGM_CPU_CLIENT_MAX,            //23

  CLKRGM_CPU_CLIENT_INTERMEDIATE_SPEED,
  CLKRGM_CPU_CLIENT_NON_CDMA_APPS,
  CLKRGM_CPU_CLIENT_MDSP_SERVICES,
  CLKRGM_CPU_CLIENT_ADSP_SERVICES,
  CLKRGM_CPU_CLIENT_QDSP_SERVICES = CLKRGM_CPU_CLIENT_ADSP_SERVICES, /* Needed for legacy drivers */
  CLKRGM_CPU_CLIENT_DEBUG,         //28
  CLKRGM_CPU_CLIENT_SLOTTED_WAKEUP,
  CLKRGM_CPU_CLIENT_SRCHAQ,
  CLKRGM_CPU_CLIENT_SRCHNONTC,     //31
  CLKRGM_CPU_CLIENT_SRCHSLEEP,
  CLKRGM_CPU_CLIENT_HDR_WAKEUP,
  CLKRGM_CPU_CLIENT_JPEG_APPS,
  CLKRGM_CPU_CLIENT_HSDIAG_DISP_CAPTURE_SERVICES, //35
  CLKRGM_CPU_CLIENT_WLAN_SERVICES,
  CLKRGM_CPU_CLIENT_KEYPAD,
  CLKRGM_CPU_CLIENT_MFLO,
  CLKRGM_CPU_CLIENT_BOOT,
#ifdef FEATURE_EXT_USB_P1
  CLKRGM_CPU_CLIENT_EXT_HS_USB,
#endif
  CLKRGM_CPU_CLIENT_HS_USB,
  CLKRGM_CPU_CLIENT_BT_SERVICES,
  CLKRGM_CPU_CLIENT_QCAMCORDER,
  CLKRGM_CPU_CLIENT_HDR_ACTIVATED,

  CLKRGM_CPU_RESOURCE_NUM_CLIENTS  
} clkrgm_cpu_resource_client_type;

#define BITSHFT64                         (uint64)1
#define CLKRGM_CPU_CLIENT(client)         (BITSHFT64 << CLKRGM_CPU_CLIENT_##client)
#define CLKRGM_CPU_CLIENT_IS_SET(client)  \
        (clkrgm.cpu_client_mask & CLKRGM_CPU_CLIENT(client))

/* Processor-specific client masks */
#define CLKRGM_CPU_CLIENT_MASK_MODEM   (0x0000FFFE)
#define CLKRGM_CPU_CLIENT_MASK_APPS    (0xFFFF0000)

/* High modem speed client mask */
#define CLKRGM_CPU_CLIENT_MASK_HIGH_MARM_SPEED \
  ((BITSHFT64 << CLKRGM_CPU_CLIENT_BOOT)           |   \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHAQ)         |   \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_SLOTTED_WAKEUP) |   \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_MDSP_SERVICES)  |   \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_GPS_SERVICES)   |   \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_HDR_WAKEUP)     |   \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_DIAG)           |   \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_TTY)            |   \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_HSDPA)          |   \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_EUL)            |   \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_MAX))

/* High bus speed client mask */
#define CLKRGM_CPU_CLIENT_MASK_HIGH_BUS_SPEED  \
  ((BITSHFT64 << CLKRGM_CPU_CLIENT_MFLO)            |  \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_CDMA_DATA_APPS)  |  \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_TV_OUT)          |  \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_AUDIO)           |  \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA)         |  \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_MAX)             |  \
   (BITSHFT64 << CLKRGM_CPU_CLIENT_APPS_HIGH_SPEED)    \
  )

/* Initial registered clients */
#ifdef FEATURE_DCVS_APPS
  #define CLKRGM_CPU_CLIENT_MASK_INIT          \
    ((BITSHFT64 << CLKRGM_CPU_CLIENT_BOOT)        |    \
     (BITSHFT64 << CLKRGM_CPU_CLIENT_APPS_HIGH_SPEED))
#else
  #define CLKRGM_CPU_CLIENT_MASK_INIT          \
    (BITSHFT64 << CLKRGM_CPU_CLIENT_BOOT)
#endif

#ifdef FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED
/* High mDSP speed client mask */
#define CLKRGM_CPU_CLIENT_MASK_HIGH_MDSP_SPEED (BITSHFT64 << CLKRGM_CPU_CLIENT_HDR_VOICE)
#endif

/* legacy definitions */

#define    CLKRGM_BT_SERVICES               CLKRGM_CPU_CLIENT_BT_SERVICES
#define    CLKRGM_CPU_CLIENT_QCAMERA        CLKRGM_CPU_CLIENT_QCAMERA_REVIEW
#define    CLKRGM_CPU_CLIENT_QCAMERA_MED    CLKRGM_CPU_CLIENT_MAX
#define    CLKRGM_INTERMEDIATE_SPEED        CLKRGM_CPU_CLIENT_INTERMEDIATE_SPEED
#define    CLKRGM_NON_CDMA_APPS             CLKRGM_CPU_CLIENT_NON_CDMA_APPS
#define    CLKRGM_MDSP_SERVICES             CLKRGM_CPU_CLIENT_MDSP_SERVICES
#define    CLKRGM_ADSP_SERVICES             CLKRGM_CPU_CLIENT_ADSP_SERVICES
#define    CLKRGM_QDSP_SERVICES             CLKRGM_CPU_CLIENT_QDSP_SERVICES /* Needed for legacy drivers */
#define    CLKRGM_DEBUG                     CLKRGM_CPU_CLIENT_DEBUG
#define    CLKRGM_SLOTTED_WAKEUP            CLKRGM_CPU_CLIENT_SLOTTED_WAKEUP
#define    CLKRGM_SLOTTED_SLEEP             CLKRGM_CPU_CLIENT_SRCHSLEEP
#define    CLKRGM_SRCHAQ                    CLKRGM_CPU_CLIENT_SRCHAQ
#define    CLKRGM_SRCHNONTC                 CLKRGM_CPU_CLIENT_SRCHNONTC
#define    CLKRGM_SRCHTC                    CLKRGM_CPU_CLIENT_SRCHTC
#define    CLKRGM_CDMA_DATA_APPS            CLKRGM_CPU_CLIENT_CDMA_DATA_APPS
#define    CLKRGM_JPEG_APPS                 CLKRGM_CPU_CLIENT_JPEG_APPS
#define    CLKRGM_HSDIAG_DISP_CAPTURE_SERVICES  CLKRGM_CPU_CLIENT_HSDIAG_DISP_CAPTURE_SERVICES
#define    CLKRGM_GPS_SERVICES              CLKRGM_CPU_CLIENT_GPS_SERVICES
#define    CLKRGM_AUDIO_SERVICES            CLKRGM_CPU_CLIENT_AUDIO_SERVICES
#define    CLKRGM_MP3_SERVICES              CLKRGM_CPU_CLIENT_MP3_SERVICES
#define    CLKRGM_CPU_CLIENT_MP3_SERVICES   CLKRGM_CPU_CLIENT_AUDIO_SERVICES
#define    CLKRGM_CPU_CLIENT_AUDIO_SERVICES CLKRGM_CPU_CLIENT_MP3_APP
#define    CLKRGM_WLAN_SERVICES             CLKRGM_CPU_CLIENT_WLAN_SERVICES
#define    CLKRGM_HDR_WAKEUP                CLKRGM_CPU_CLIENT_HDR_WAKEUP
#define    CLKRGM_HDR_VOICE                 CLKRGM_CPU_CLIENT_HDR_VOICE
#define    CLKRGM_HDR_ACTIVATED             CLKRGM_CPU_CLIENT_HDR_ACTIVATED



typedef enum
{
    CLKRGM_HIGH_FREQUENCY_STATE = 0,
    #ifdef FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING
    CLKRGM_INTERMEDIATE_FREQUENCY_STATE,
    #endif
    #ifdef FEATURE_GSM
#error code not present
    #endif
    CLKRGM_LOW_FREQUENCY_STATE
} clkrgm_cpu_frequency_state;

extern clkrgm_cpu_frequency_state clock_frequency_state;

#endif

  /* ------------------------------------------------------------------------
     PLL configurations options.  The following are the PLL configurations
     options that can be selected at build time.
  ------------------------------------------------------------------------ */
#if 0
  //#define CLKRGM_PLL_192                       0 /*Changed for testing recalculate*/
  #define CLKRGM_PLL_384                       1
#endif
  /* ------------------------------------------------------------------------
     PLL configuration option selection.  The following identifies the PLL
     configuration option that has been selected at build time.
  ------------------------------------------------------------------------ */

  #if defined  FEATURE_PLL_192
    #define CLKRGM_PLL_CFG   CLKRGM_PLL_192
  #elif defined FEATURE_PLL_384
    #define CLKRGM_PLL_CFG   CLKRGM_PLL_384
  #else
    #error Must define a supported PLL config at build time.
  #endif

  #if defined  FEATURE_PLL_192
    #define CLKRGM_PLL_SPEED        192000000
  #elif defined FEATURE_PLL_384
    #define CLKRGM_PLL_SPEED        384000000
  #else
    #error PLL Configuration not supported.
  #endif

  /* For backwards compatibility */
  #define CLKRGM_MPLL_SPEED      (CLKRGM_PLL_SPEED)

  #define CLKRGM_PLL_FREQ_HZ     (CLKRGM_PLL_SPEED)
  #define CLKRGM_PLL_FREQ_KHZ    (CLKRGM_PLL_SPEED/1000)
  #define CLKRGM_PLL_FREQ_MHZ    (CLKRGM_PLL_SPEED/1000000)

  /* ------------------------------------------------------------------------
     XO clock frequency 
  ------------------------------------------------------------------------ */

  #define CLKRGM_XO_FREQ_HZ        19200000
  #define CLKRGM_XO_FREQ_KHZ       (CLKRGM_XO_FREQ_HZ/1000)
  #define CLKRGM_XO_FREQ_MHZ       (CLKRGM_XO_FREQ_HZ/1000000)

  /* ------------------------------------------------------------------------
     Sleep clock frequency 
  ------------------------------------------------------------------------ */
  #define CLKRGM_SLP_FREQ_HZ       32764
  #define CLKRGM_SLP_FREQ_KHZ      (CLKRGM_SLP_FREQ_HZ/1000)
  #define CLKRGM_SLP_FREQ_MHZ      (CLKRGM_SLP_FREQ_HZ/1000000)



/* Frequency options for camera clock */
/* This enum is used as an array index for the camclk configuration
   related arrays, so the ordering must be consistent with those arrays. */

typedef enum 
{
  CLKRGM_CAMCLK_TCXO,
  CLKRGM_CAMCLK_6P144_MHZ,
  CLKRGM_CAMCLK_12P0_MHZ,
  CLKRGM_CAMCLK_12P288_MHZ,
  CLKRGM_CAMCLK_24P576_MHZ,
  CLKRGM_CAMCLK_24P0_MHZ,
  CLKRGM_CAMCLK_48P0_MHZ,
  CLKRGM_CAMCLK_49P152_MHZ,
  CLKRGM_CAMCLK_MISC,

  CLKRGM_CAMCLK_CLK_MAX
} clkrgm_camclk_type;
#define CLKRGM_CAMCLK_PAUSE 1

/* --------------------------------------------------------------------------
   Audio clock usage parameters.
-------------------------------------------------------------------------- */

/* Frequency options for the internal codec input clock. */

/* This enum is used as an array index for the ICODEC configuration
   related arrays, so don't change the following ordering. */

typedef enum
{
  CLKRGM_CODEC_CLK_2P048_MHZ = 0,
  CLKRGM_CODEC_CLK_2P8224_MHZ,
  CLKRGM_CODEC_CLK_3P072_MHZ,
  CLKRGM_CODEC_CLK_4P096_MHZ,
  CLKRGM_CODEC_CLK_5P6448_MHZ,
  CLKRGM_CODEC_CLK_6P144_MHZ,
  CLKRGM_CODEC_CLK_8P192_MHZ,
  CLKRGM_CODEC_CLK_11P264_MHZ,
  CLKRGM_CODEC_CLK_11P2896_MHZ,
  CLKRGM_CODEC_CLK_12P288_MHZ,

  CLKRGM_CODEC_CLK_MAX
} clkrgm_codec_clk_type;

/* Clock enable/disable time is 3 clocks at 2.048 MHz */
#define CLKRGM_CODEC_PAUSE 2

/* Frequency options for the external stereo dac input clock. */

/* This enum is used as an array index for the ECODEC configuration
   related arrays, so don't change the following ordering. */

typedef enum
{
  CLKRGM_ECODEC_CLK_0P128_MHZ = 0,
  CLKRGM_ECODEC_CLK_0P256_MHZ,
  CLKRGM_ECODEC_CLK_2P048_MHZ,
  CLKRGM_ECODEC_CLK_4P096_MHZ,

  #define CLKRGM_ECODEC_CLK_MAX (CLKRGM_ECODEC_CLK_4P096_MHZ+1) /*klocwork*/
} clkrgm_ecodec_clk_type;
#define CLKRGM_ECODEC_PAUSE 24

/* This enum is used as an array index for the SDAC configuration
   related arrays, so don't change the following ordering. */

typedef enum
{
  CLKRGM_SDAC_CLK_256_KHZ = 0,
  CLKRGM_SDAC_CLK_352P8_KHZ,
  CLKRGM_SDAC_CLK_384_KHZ,
  CLKRGM_SDAC_CLK_512_KHZ,
  CLKRGM_SDAC_CLK_705P6_KHZ,
  CLKRGM_SDAC_CLK_768_KHZ,
  CLKRGM_SDAC_CLK_1024_KHZ,
  CLKRGM_SDAC_CLK_1411P2_KHZ,
  CLKRGM_SDAC_CLK_1536_KHZ,

  #define CLKRGM_SDAC_CLK_MAX (CLKRGM_SDAC_CLK_1536_KHZ+1) /*for klocwork*/
} clkrgm_sdac_clk_type;
#define CLKRGM_SDAC_PAUSE 12

typedef enum
{
  CLKRGM_UART_DM_TCXO = 0,
  CLKRGM_UART_DM_7P3728_MHZ,    // 7372800
  CLKRGM_UART_DM_14P7456_MHZ,   // 14745600,
  CLKRGM_UART_DM_51P20_MHZ,     // 51200000,
  CLKRGM_UART_DM_58P9824_MHZ,   // 58982400,
  CLKRGM_UART_DM_64P0_MHZ,
  CLKRGM_UART_DM_192P0_MHZ,
  CLKRGM_UART_DM_MISC,

  #define CLKRGM_UART_DM_CLK_MAX (CLKRGM_UART_DM_MISC+1) /* klocwork */
} clkrgm_uart_dm_clk_type;


#define CLKRGM_UART_DM_PAUSE 1

#define  CLKRGM_UARTDM_SPEED_7_3728_MHZ                7372800
#define  CLKRGM_UARTDM_SPEED_14_7456_MHZ              14745600
#define  CLKRGM_UARTDM_SPEED_51_2_MHZ                 51200000
#define  CLKRGM_UARTDM_SPEED_58_9824_MHZ              58982400
#define  CLKRGM_UARTDM_SPEED_64_MHZ                   64000000
#define  CLKRGM_UARTDM_SPEED_192_MHZ                 192000000


/* --------------------------------------------------------------------------
   SDCC clock usage parameters.
-------------------------------------------------------------------------- */

/* Frequency options for the SD controler clock. */

/* This enum is used as an array index for the SDCC configuration
   related arrays, so don't change the following ordering. */

typedef enum
{
  CLKRGM_SDCC_FAST = 0,
  CLKRGM_SDCC_SLOW,
  CLKRGM_SDCC_400KHZ,
  CLKRGM_SDCC_19P2MHZ,
  CLKRGM_SDCC_9P6MHZ,
  CLKRGM_SDCC_4P8MHZ,

  CLKRGM_SDCC_CLK_MAX
} clkrgm_sdcc_clk_type;
#define CLKRGM_SDCC_PAUSE 8


/* --------------------------------------------------------------------------
   DSP DME options.
-------------------------------------------------------------------------- */

/* The following options can be selected when configuring the ADSP frequency
   for a specified mode of operation. */

typedef enum
{
  CLKRGM_DME_NOT_REQUIRED = 0,
  CLKRGM_DME_REQUIRED
} clkrgm_dme_mode_type;


/* --------------------------------------------------------------------------
   ADSP clock frequency options.
-------------------------------------------------------------------------- */

/* The following options can be selected when configuring the ADSP frequency
   for a specified mode of operation. */

typedef enum
{
  CLKRGM_ADSP_19P2MHZ = 0,
  CLKRGM_ADSP_38P4MHZ,
  CLKRGM_ADSP_46P0MHZ,
  CLKRGM_ADSP_64P0MHZ,
  CLKRGM_ADSP_96P0MHZ,
  CLKRGM_ADSP_115P2MHZ = CLKRGM_ADSP_96P0MHZ,
  #define CLKRGM_ADSP_CLK_MAX_NUM (CLKRGM_ADSP_96P0MHZ+1) /* for klocwork */
} clkrgm_qdsp_clk_frequency_type;

/* backward compatibility */
#define CLKRGM_ADSP_36P6912MHZ  CLKRGM_ADSP_38P4MHZ
#define CLKRGM_ADSP_49P152MHZ   CLKRGM_ADSP_64P0MHZ
#define CLKRGM_ADSP_67P2MHZ     CLKRGM_ADSP_96P0MHZ
#define CLKRGM_ADSP_73P3824MHZ  CLKRGM_ADSP_96P0MHZ
#define CLKRGM_ADSP_76P8MHZ     CLKRGM_ADSP_96P0MHZ
#define CLKRGM_ADSP_86P4MHZ     CLKRGM_ADSP_96P0MHZ

#define CLKRGM_ADSP_80P00_SPEED CLKRGM_ADSP_64P0MHZ


/* --------------------------------------------------------------------------
   HCLK control enums for clk_regime_uP_sleep()
-------------------------------------------------------------------------- */
typedef enum
{
  CLKRGM_HCLK_OFF,
  CLKRGM_HCLK_TCXO,
  CLKRGM_HCLK_USB = CLKRGM_HCLK_TCXO,
  CLKRGM_HCLK_FULL,
  CLKRGM_HCLK_MDP = CLKRGM_HCLK_FULL,
  CLKRGM_HCLK_MDDI = CLKRGM_HCLK_FULL,
  CLKRGM_HCLK_NO_SWITCH,
  CLKRGM_HCLK_HDR
} clkrgm_hclk_sleep_type;

/* --------------------------------------------------------------------------
   MCLK control enums for clk_regime_uP_sleep()
-------------------------------------------------------------------------- */
typedef enum
{
  CLKRGM_MCLK_OFF,
  CLKRGM_MCLK_ON
} clkrgm_mclk_sleep_type;


/*===========================================================================

                       FUNCTION PROTOTYPES

===========================================================================*/

/* --------------------------------------------------------------------------
   Clock regime driver initialization.
-------------------------------------------------------------------------- */

extern void clk_regime_init(void);

/* --------------------------------------------------------------------------
   Clock regime enable, disable and reset.
-------------------------------------------------------------------------- */

extern void clk_regime_msm_reset(clkrgm_msm_clk_type regime);

extern void clk_regime_msm_enable(clkrgm_msm_clk_type regime);

extern void clk_regime_msm_disable(clkrgm_msm_clk_type regime);

extern boolean clk_regime_msm_is_on(clkrgm_msm_clk_type regime);

/* --------------------------------------------------------------------------
   Clock regime configuration.
-------------------------------------------------------------------------- */

extern void clk_regime_msm_sel_clk_src(clkrgm_msm_src_sel_type regime, uint32 source  );

#if 0
  /* This function does not appear to be used on QSC1100, and porting
     will be problematic, so I am removing it */
extern void clk_regime_msm_sel_clk_div(clkrgm_msm_src_div_type regime, uint32 divider );
#endif

extern void clk_regime_msm_sel_clk_inv(clkrgm_msm_src_inv_type regime, boolean invert );

/* --------------------------------------------------------------------------
   Processor frequency and memory bus frequency switching.
-------------------------------------------------------------------------- */

extern void clk_regime_switch_mclk(clkrgm_cpu_speed_type new_mclk);

extern void clk_regime_switch_mclk_max (void);
#ifdef CLKRGM_HAS_SLEEP
extern void clk_regime_set_mclk_sleep(void);
#else
#define clk_regime_set_mclk_sleep()
#endif

extern void clk_regime_set_dload_mclk(void);

#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
#ifndef FEATURE_L4_KERNEL
extern void clk_regime_deregister_for_cpu_resource(clkrgm_cpu_resource_client_type client /* Requesting client */);
extern void clk_regime_register_for_cpu_resource(clkrgm_cpu_resource_client_type client /* Requesting client */);
#endif
#endif
/* --------------------------------------------------------------------------
   DSP frequency and memory bus frequency switching.
-------------------------------------------------------------------------- */

extern void clk_regime_set_mdsp_dme_mode(clkrgm_dme_mode_type dme_mode);
extern void clk_regime_set_qdsp_dme_mode(clkrgm_dme_mode_type dme_mode);

extern void clk_regime_set_qdsp_clk_frequency(clkrgm_qdsp_clk_frequency_type clk_frequency);

extern void clk_regime_switch_qdsp_clk(void);

extern void clk_regime_switch_qdsp_clk_internal(void);

extern void clk_regime_complete_qdsp_clk_switch(void);

extern uint32 clk_regime_get_max_adsp_clk_khz(void);

extern uint32 clk_regime_get_i2c_clk_khz (void);

extern uint32 clk_regime_get_max_hclk_freq_khz(void);


/* --------------------------------------------------------------------------
   PLL control.
-------------------------------------------------------------------------- */

extern void clk_regime_pll_request (clkrgm_pll_type pll, int client, clkrgm_pll_op_type request);

extern int clk_regime_pll_usage_count (clkrgm_pll_type pll);
extern void clk_regime_all_pll_off (void);
extern void clk_regime_all_pll_on (void);

/* --------------------------------------------------------------------------
   RXF control.
-------------------------------------------------------------------------- */

/* Clock regime RXF clients */
  typedef enum 
  {
    CLKRGM_RXF_RF_0,
    CLKRGM_RXF_RF_1,
    CLKRGM_RXF_HDR,
    CLKRGM_RXF_GPS,
    CLKRGM_RXF_SRCH,
    CLKRGM_RXF_MDSP,
    CLKRGM_RXF_ADSP,
    CLKRGM_RXF_USB,
    CLKRGM_RXF_CLKREGIM,
    CLKRGM_RXF_MAX_CLIENTS
  } clkrgm_rxf_client_type;

  typedef enum
  {
    CLKRGM_RXF_DISABLE = 0x0,
    CLKRGM_RXF_ENABLE
  } clkrgm_rxf_op_type;

extern void clk_regime_rxf_request (
  clkrgm_rxf_client_type  client,
  clkrgm_rxf_op_type      request);

/* --------------------------------------------------------------------------
   Audio clock control.
-------------------------------------------------------------------------- */

extern void clk_regime_icodec_clk_disable(void);

extern void clk_regime_sel_icodec_clk(clkrgm_codec_clk_type clk_freq);
extern void clk_regime_sel_ecodec_clk(clkrgm_ecodec_clk_type clk_freq);

extern void clk_regime_sel_sdac_clk(clkrgm_sdac_clk_type clk_freq);


/* --------------------------------------------------------------------------
   Camera clock control.
-------------------------------------------------------------------------- */

extern void clk_regime_msm_sel_camclk(uint32 new_freq);

/* --------------------------------------------------------------------------
   UART_DM(UART1DM) clock control.
-------------------------------------------------------------------------- */
extern void clk_regime_msm_sel_uart_dm_clk(uint32 new_freq);

/* --------------------------------------------------------------------------
   USB clock control.
-------------------------------------------------------------------------- */

extern void clk_regime_usb_xtal_off ( void );

extern void clk_regime_usb_xtal_on ( void );

/* --------------------------------------------------------------------------
   SD card clock control.
-------------------------------------------------------------------------- */

extern void clk_regime_set_sdcc_slow_clock ( void );

extern void clk_regime_set_sdcc_fast_clock ( void );

extern void clk_regime_set_sdcc_clk_frequency (clkrgm_sdcc_clk_type sdcc_freq );

/* --------------------------------------------------------------------------
   Sleep related clock control.
-------------------------------------------------------------------------- */
/*
  1)There is a requirement when the ARM clock is disabled, 
    the ARM / HCLK frequency ratio must be 1:1.
  2)The EBI1_IO_CLK must be 2x HCLK in DDR mode.
  3)Currently, the ARM clock has a divider on the path  but
    it is not controllable. It is hardwired to div-1. 
    It is there only for balancing purposes.
  4)To satisfy #2, the HCLK divider (C in the diagram) must be set to div-2.
  5)To satisfy #1, the HCLK divider must be set to div-1. 
  Hence, #4 & #5 obviously cannot be simultaneously satisfied.
*/
#if defined(CLKRGM_HAS_SLEEP)
extern void clk_regime_uP_sleep( clkrgm_mclk_sleep_type mclk_sleep,
                          clkrgm_hclk_sleep_type hclk_sleep);
#else
   #define clk_regime_uP_sleep(ms, hs)
#endif /* CLKRGM_HAS_SLEEP */

extern void config_tcxo_off
            (
               uint32 tcxo_en_addr,
               uint32 tcxo_on_val,
               uint32 tcxo_off_val
            );

#ifdef FEATURE_POWER_COLLAPSE
#error code not present
#endif


extern void switch_micro_clk_high_delayed
            (
               uint32  hclk_div_reg_val,
               uint32  pause_timer_reg_val
            );

extern void switch_micro_clk_low_delayed
            (
              uint32  hclk_div_reg_val,
              uint32  pause_timer_reg_val
            );

extern void clk_regime_set_gps_mode(boolean gps_mode);

extern void clk_regime_set_sdio_mode(boolean sdio_mode);
extern uint32 clk_regime_msm_get_clk_freq_khz (clk_regime_type regime);


/* Boolean to specify if USB needs the hclk */

extern boolean usb_needs_hclk;

/* Boolean to specify if HDR needs the mclk. */

extern boolean hdr_needs_mclk;

/* switches for dynamic clock/voltage control */
#if defined (FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || defined (FEATURE_CLKREGIM_DYNAMIC_VOLTAGE)
extern boolean                  clk_regime_dyn_clock_on;      /* feature control         */
extern boolean                  clk_regime_dyn_voltage_on;    /* feature control         */
#endif

extern void clk_regime_sbi_enable (void);

extern void clk_regime_set_chipxn_frequency(int32 ppt);
typedef enum {
   CLKRGM_CHIPXN_SW_CNTRL,
   CLKRGM_CHIPXN_FW_CNTRL
} clk_regime_chipxn_mnd_cntrl_type;
extern void clk_regime_set_chipxn_mnd_ctrl(clk_regime_chipxn_mnd_cntrl_type cntrl);
extern clk_regime_chipxn_mnd_cntrl_type clk_regime_get_chipxn_mnd_ctrl(void);


#endif /* CLKRGM_QSC_1100_H */

