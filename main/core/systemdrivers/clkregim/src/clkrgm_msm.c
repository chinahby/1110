/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    Q S C 1 1 0 0    C L O C K    R E G I M E    D E V I C E   D R I V E R


GENERAL DESCRIPTION

  This module contains procedures to support clock regime configuration and
  control functions for the MSM6800 clock regimes.

EXTERNALIZED FUNCTIONS

  clk_regime_init
    Initialize module data structures and hardware registers.

  clk_regime_msm_reset
    Reset specified hardware clock regime.

  clk_regime_msm_is_on
    Test if clock regime is on.

  clk_regime_msm_sel_clk_src
    Select input clock source for specified hardware clock regime.

  clk_regime_msm_sel_clk_div
    Select clock divider for specified hardware clock regime.

  clk_regime_msm_sel_clk_inv
    Select output clock polarity for specified hardware clock regime.

  clk_regime_msm_misc_reset
    Reset specified miscellaneous hardware clock function.

  clk_regime_force_mclk_tcxo
    Switch processor clock to TXCO frequency.

  clk_regime_set_mclk_sleep
    Switch processor clock to SLEEP XTAL frequency.

  clk_regime_switch_mclk
    Switch processor clock to another speed.

  clk_regime_set_dload_mclk
    Switch processor clock to downloader frequency.

  clk_regime_set_mclk_hi_clk
    Switch processor clock to full speed frequency.

  clk_regime_get_i2c_clk_khz()
    Return frequency in KHz of the I2C clock

  clk_regime_icodec_clk_disable
    Disable icodec hardware clock regime.

  clk_regime_sel_icodec_clk
    Select clock frequency for icodec hardware clock regime.

  clk_regime_sel_sdac_clk
    Select clock frequency for sdac hardware clock regime.

  clk_regime_uP_sleep
    Command processor hardware to enter sleep state.

  config_tcxo_off
    Disable TCXO oscillator hardware.

  set_sdram_self_refresh_and_tcxo_off
    Configure SDRAM for self refresh and disable TCXO oscillator hardware.

  turn_off_micro_clk
    Disable microprocessor hardware clock regime.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  All clock regimes are enabled after reset.  The clk_regime_msm_init() function
  should be called prior to enabling or disabling specific clock regimes.


  Copyright (c) 2004-2008     by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/clkregim/clkrgm_msm.c#53 $
$DateTime: 2010/12/28 15:01:23 $
$Author: kmaliped $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/09   km      Fix for CR : 210811
02/24/09   hvo     Removed worked around for VREG MSMA
02/19/09   hvo     Fixed RUIM leakage current
01/12/09    br     Corrected adsp clock change to report DevMan in sleep and ARM Halt.
                   Removed ADSP clock off while switching ADSP clock.
                   Changed the Mode switch logic to occur in steps instead of one go.  							   
12/08/08    hvo    Disabled AHB bus halt because BT, RUIM, Audio,... failed
                   this is the work around as we need more time to investigate
12/02/08    hvo    Increased S2 to 2.3 V to pass cold temperature issue with RXADC
11/19/08    kma    Ground ADSP Clock source which is not in use. CR Fix : 161335
                   Ground GSBI1_CLK source when not in use in clk_regime_init(). CR Fix : 161336.
11/07/08    hvo    Dynamically detect PG1-PG3,Locally enable/disable MSMA
                   because can’t do it from master, remove HWIO_OUT(RXA2D_EN, 0x3e)
                   RF driver handles it
11/07/08    kma    Modified check to enable and Disable ADSP_CLK_EN
                   Modified code to switch ADSP Clock only when requested.
                   clk_regime_switch_qdsp_clk_internal is called from within clk_regime code only.
                   clk_regime_switch_qdsp_clk API remains as external Interface API only. 
10/19/08    hvo    Reduced S1/S2 voltage during XO shutdown, reduced RUIM
                   voltage to 1.35 V for PG2 during sleep, reduced ARM clock
                   to 38.4 MHz DCVS
09/22/08    hvo    Enabled ARM halt using MCR command for now
                   Put VREG_MSMC, MSME1, MSME2, in lower power mode
				   Enable DCVS, still have work around for USB
09/08/08    hvo    Added Handshaking between RF and clock regime during XO
                   shutdown and wakeup.
				   Turned off MSMA, MPLL during sleep.
				   Cleaned up functions to configure pmic during xo shutdown
                   and wakeup.
				   Set VREG_MSMC to 1.3 V when wakeup from xo shutdown.
08/10/08    hvo    KIP A0 has overshoot problem when switch from PWM to PFM
                   mode and vise versa. So during XO shutdown, don't use the
                   automatic mode otherwise it could damage the chip. Need
                   undo this fix when KIP B0 is available and tested.
07/14/08    hvo    Only allow USB to have clock of 48MHz or greater
06/26/08    hvo    Enable sleep with XO disable, AHB halt voting, PWM to PFM
05/15/08    bc     Fixed and enabled clock regime switching of the
                   ARM/AHB/Bus clocks.
04/18/08    bc     Removed clock disable polling loops until they can be 
                   verified.  Set hclk rate for clk_busy_wait accuracy.
10/11/07    bc     Initial port for QSC1100 from QSC6055v3202.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"

#include "clkrgm_modem.h"
#include "clkrgmi_msm.h" /* Clock regime configuration defs     */
#include "clkrgmi_devman.h"
#include "clkregim.h"   /* Clock Regime definitions             */
#include "msm.h"
#include "bsp.h"        /* Board support package                */
#include "boothw.h"     /* Memory wait state macros             */
#include "assert.h"
#include "clk.h"        /* Clock services                       */
#include "msmhwio.h"
#include "tlmm.h"
#include "pmgen.h"
#include "pm66XXvregs.h"/* VREG voting api return status        */ 
#include "trampi.h"
#include "hw.h"                                    /* Hardware header file */

#include "adie_sbi.h"
#include "adie.h"
#include "adie_cdcdefs.h"

#ifdef CUST_EDITION
//关闭睡眠时系统对GPIO配置的控制，否则会引起很多由于睡眠引起的不稳定问题
#undef FEATURE_MANAGE_GPIOS_FOR_SLEEP
#endif

#ifdef BUILD_BOOT_CHAIN_OEMSBL
#undef MSG_ERROR
#define MSG_ERROR(a,b,c,d)
#undef ERR
#define ERR(a,b,c,d)
#endif

extern const clkrgm_modem_mdsp_cfg_type  
             clkrgm_modem_mdsp_cfg[CLKRGM_MDSP_NUM_SPEEDS];
void clk_regime_set_mdsp(clkrgm_mdsp_clk_frequency_type mdsp_speed );

extern void clk_regime_dcvs_log (
  clkrgm_dcvs_clk_log_type reg_dreg,
  uint8 transaction);

#ifndef BUILD_BOOT_CHAIN_OEMSBL
static int clkrgm_verbose=0;
#endif

/* Variables to Check the Status of FM Path */
static boolean clkrgm_fmpath_on;

/* Debug message macro */
#ifdef BUILD_BOOT_CHAIN_OEMSBL
  #define CLKRGM_MSM_MSG(str, a, b, c, d)
#else
  #include "msg.h"
  #define CLKRGM_MSM_MSG(str, a, b, c, d)                         \
    if (clkrgm_verbose)                                     \
    {                                                           \
      MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d);   \
    }
#endif

#ifdef FEATURE_CLKREGIM_RM
  #define RM_CLK_INFO(clk) (&clkrgm.rm_clk_info[(int)clk])
#endif


/*===========================================================================

                     FORWARD FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

void clk_regime_devman_handler
(
  clkrgm_device_type          device,
  clkrgm_device_request_type  request,
  clkrgm_device_param_type   *param
);

extern void set_micro_to_32khz_and_kill_tcxo(void);
extern void mmu_dcache_flush_all(void);
extern void mmu_standby_wfi(void);

/* --------------------------------------------------------------------------
   Microprocessor and memory bus configuration data structures.
-------------------------------------------------------------------------- */

  extern const clkrgm_mclk_clkrgm_type clkrgm_cpu_cfg [CLKRGM_CPU_NUM_SPEEDS];

 /* EBI register value structure array.  Contains EBI register values for
    each memory bus clock frequency. */

  boothw_ebi_regs_type clkrgm_ebi_regs [ CLKRGM_CPU_NUM_SPEEDS ];

/* --------------------------------------------------------------------------
   DSP clock regime configuration data structures.
-------------------------------------------------------------------------- */

  /* ADSP clock regime configuration structure array. */
  extern const clkrgm_adsp_cfg_type clkrgm_adsp_cfg [CLKRGM_ADSP_NUM_SPEEDS];

/* --------------------------------------------------------------------------
   Audio clock regime configuration data structures.
-------------------------------------------------------------------------- */

  /* Internal CODEC clock regime configuration structure array. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_icodec_cfg [ ];

  /* External CODEC clock regime configuration structure. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_ecodec_cfg[ ];

  /* Stereo DAC clock regime configuration structure array. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_sdac_cfg [ ];

/* --------------------------------------------------------------------------
   Peripheral clock regime configuration data structures.
-------------------------------------------------------------------------- */

  /* External camera (GPIO 13) clock regime configuration structure array. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_camclk_cfg[] ;

  boolean clk_regime_calculate_mnd( uint32 freq_in, uint32 freq_out, uint32 field_mask,
                                    uint32 *mn_value, uint16 *d_value);

  /* SDCC clock regime configuration structure array. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_sdcc_cfg [ ] ;

  /* UART_DM clock regime configuration structure array. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_uart_dm_cfg [ ] ;

  extern const clkrgm_msm_type1_cfg_type clkrgm_gsbi0_uart_cfg [ ] ;
  extern const clkrgm_msm_type1_cfg_type clkrgm_gsbi_1_2_uart_cfg [ ] ;

/* --------------------------------------------------------------------------
   Clock regime driver state information structure.
-------------------------------------------------------------------------- */

  /* Clock regime state information structure definition. */

extern   clkrgm_msm_state_type clkrgm;
extern   clkrgm_modem_state_type clkrgm_modem;

#if defined (FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || defined (FEATURE_CLKREGIM_DYNAMIC_VOLTAGE)
  boolean   clk_regime_dyn_clock_on=FALSE;      /* feature control         */
  boolean   clk_regime_dyn_voltage_on=FALSE;    /* feature control         */

  clk_regime_freq_plan_type clk_regime_freq_plans[] =
  {
    //AUDIO MP3
    {
      CLKRGM_CPU_32P0_DIV1, CLKRGM_CPU_32P0_DIV1_HCLK_DIV, 
      CLKRGM_ADSP_64P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //AUDIO AAC-LC
    {
      CLKRGM_CPU_32P0_DIV1, CLKRGM_CPU_32P0_DIV1_HCLK_DIV, 
      CLKRGM_ADSP_64P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //AUDIO AAC-SBR
    {
      CLKRGM_CPU_32P0_DIV1, CLKRGM_CPU_32P0_DIV1_HCLK_DIV, 
      CLKRGM_ADSP_96P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //AUDIO AAC-PS
    {
      CLKRGM_CPU_32P0_DIV1, CLKRGM_CPU_32P0_DIV1_HCLK_DIV, 
      CLKRGM_ADSP_96P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //AUDIO WMA
    {
      CLKRGM_CPU_32P0_DIV1, CLKRGM_CPU_32P0_DIV1_HCLK_DIV, 
      CLKRGM_ADSP_96P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //AUDIO Real Audio
    {
      CLKRGM_CPU_32P0_DIV1, CLKRGM_CPU_32P0_DIV1_HCLK_DIV, 
      CLKRGM_ADSP_64P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //Video Decode
    {
      CLKRGM_CPU_HALF_SPEED, CLKRGM_CPU_HALF_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_48P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //QTV
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_96P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //QCamcorder ViewFinder
    {
      CLKRGM_CPU_32P0_DIV1, CLKRGM_CPU_32P0_DIV1_HCLK_DIV, 
      CLKRGM_ADSP_TCXO_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //QCamcorder Record 1
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_96P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //QCamcorder Record 2
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_96P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //QCamera ViewFinder
    {
      CLKRGM_CPU_32P0_DIV1, CLKRGM_CPU_32P0_DIV1_HCLK_DIV, 
      CLKRGM_ADSP_TCXO_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //QCamera Snap Shot 1
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_48P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //QCamera Snap Shot 2
    {
      CLKRGM_CPU_HALF_SPEED, CLKRGM_CPU_HALF_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_48P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //QCamera Review
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_48P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //Graphics
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_96P0MHZ_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    //1X Voice
    { 
      CLKRGM_CPU_38P4_DIV1, CLKRGM_CPU_38P4_DIV1_HCLK_DIV, 
      CLKRGM_ADSP_38P4MHZ_SPEED, CLKRGM_MDSP_64MHZ_SPEED 
    },
    //EVDO Data, we don't have it, hvo
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_TCXO_SPEED, CLKRGM_MDSP_64MHZ_SPEED 
    },
    //1x Data
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_TCXO_SPEED, CLKRGM_MDSP_64MHZ_SPEED 
    },
    //Hybrid
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_TCXO_SPEED, CLKRGM_MDSP_64MHZ_SPEED 
    },
    //GPS, we don't have it, hvo?
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_TCXO_SPEED, CLKRGM_MDSP_64MHZ_SPEED 
    },
    //BT - Voice
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_TCXO_SPEED, CLKRGM_MDSP_64MHZ_SPEED 
    },
    //Zero PLL config
    {
      CLKRGM_CPU_XO_SPEED, CLKRGM_CPU_XO_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_TCXO_SPEED, CLKRGM_MDSP_19P2MHZ_SPEED 
    },
    // CLKRGM_CPU_CLIENT_MAX
    {
      CLKRGM_CPU_FULL_SPEED, CLKRGM_CPU_FULL_SPEED_HCLK_DIV, 
      CLKRGM_ADSP_96P0MHZ_SPEED, CLKRGM_MDSP_64MHZ
    },
  };

  #define CLKRGM_FREQ_PLAN_CLIENTS 0x00ffffff
  #define CLKRGM_FREQ_PLAN_CLIENTS 0x00ffffff
#endif

void clk_regime_set_mclk(clkrgm_cpu_speed_type  new_mclk)
{
#if !defined(BUILD_BOOT_CHAIN_OEMSBL)
  clkrgm_device_param_type  param;
  if(new_mclk < CLKRGM_CPU_NUM_SPEEDS)
  {
    param.minimum_speed = clkrgm_cpu_cfg[new_mclk].mclk.khz;
    clk_regime_devman_request(
      &clkrgm.devman_client, CLKRGM_DEVICE_MCPU,
      CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);
  }
#else
  (void)new_mclk;
#endif
}

/* --------------------------------------------------------------------------
   USB variables.
-------------------------------------------------------------------------- */

  extern boolean usb_xtal_on;
  boolean usb_needs_hclk = FALSE;

/* --------------------------------------------------------------------------
   DSP variables.
-------------------------------------------------------------------------- */

  clkrgm_dme_mode_type adsp_dme_mode = CLKRGM_DME_NOT_REQUIRED;
  clkrgm_dme_mode_type mdsp_dme_mode = CLKRGM_DME_NOT_REQUIRED;

/* --------------------------------------------------------------------------
   SDIO variables.
-------------------------------------------------------------------------- */

  boolean sdio_active_mode = FALSE;

/* --------------------------------------------------------------------------
   Clock regime maps.

   The following relate the clock regimes available for the clock source
   selection, clock divider selection and clock polarity selection back to
   the clock regimes that can be enabled and disabled.  These maps are used
   to determine if a specified clock regime is enabled prior to changing the
   clock source, clock divider or clock polarity configuration for the
   specified clock regime.
-------------------------------------------------------------------------- */

  /* Clock source selection clock regime map. The order of the items in this
     array must match the order of the items in the clkrgm_msm_src_sel_type  
     enum since the enum value is used as the array index. */

  const clkrgm_msm_clk_type clkrgm_msm_src_sel_regime_map [CLK_RGM_MSM_SRC_SEL_NUM_REGIMES] =
  {
    CLK_RGM_ADSP_M,
    CLK_RGM_GEN_M,
    CLK_RGM_GSBI0_M,
    CLK_RGM_GSBI1_M,
    CLK_RGM_GSBI2_M,
    CLK_RGM_UART_LEGACY_M,
    CLK_RGM_I2C_M,
    CLK_RGM_USB_M,
    CLK_RGM_VIDEO_VFE_M,
    CLK_RGM_CAMIF_PCLK_M
  };

  /* Clock divider selection clock regime map. The order of the items in this
     array must match the order of the items in the clk_regime_msm_div_sel_type
     enum since the enum value is used as the array index. */

#if 0
  const clkrgm_msm_clk_type clkrgm_msm_div_sel_regime_map [CLK_RGM_MSM_DIV_SEL_NUM_REGIMES] =
  {
    CLK_RGM_I2C0_M,           /* CLK_RGM_DIV_SEL_I2C0           */
    CLK_RGM_I2C1_M,           /* CLK_RGM_DIV_SEL_I2C1           */
    CLK_RGM_I2C2_M,           /* CLK_RGM_DIV_SEL_I2C2           */
    CLK_RGM_I2C_LEGACY_M,     /* CLK_RGM_DIV_SEL_I2C_LEGACY     */
    CLK_RGM_MMC_M,            /* CLK_RGM_DIV_SEL_MMCC           */
    CLK_RGM_USB_SCLK_M,       /* CLK_RGM_DIV_SEL_USB_SCLK       */
    CLK_RGM_USB_MCLK_M,       /* CLK_RGM_DIV_SEL_USB_MCLK       */
    CLK_RGM_NOT_SUPPORTED,
    CLK_RGM_NOT_SUPPORTED
  };
#endif

  /* Clock polarity selection clock regime map. The order of the items in this
     array must match the order of the items in the clk_regime_msm_inv_sel_type
     enum since the enum value is used as the array index. */

  const clkrgm_msm_clk_type clkrgm_msm_inv_sel_regime_map [CLK_RGM_MSM_INV_SEL_NUM_REGIMES] =
  {
    CLK_RGM_USB_M,            /* CLK_RGM_INV_SEL_USB            */
    CLK_RGM_NOT_SUPPORTED,    /* CLK_RGM_INV_SEL_BT_PCM         */
    CLK_RGM_ECODEC_M,         /* CLK_RGM_INV_SEL_ECODECIF       */
    CLK_RGM_NOT_SUPPORTED,    /* CLK_RGM_INV_SEL_RPCM           */
    CLK_RGM_ETM_M,            /* CLK_RGM_INV_SEL_ETM            */
    CLK_RGM_ICODEC_M,         /* CLK_RGM_INV_SEL_ADIE_CODEC     */
    CLK_RGM_VIDEO_M,          /* CLK_RGM_INV_SEL_VIDEO          */
    CLK_RGM_UART0_M,          /* CLK_RGM_INV_SEL_UART0          */
    CLK_RGM_UART1_M,          /* CLK_RGM_INV_SEL_UART1          */
    CLK_RGM_UART2_M,          /* CLK_RGM_INV_SEL_UART2          */
    CLK_RGM_UART_LEGACY_M,    /* CLK_RGM_INV_SEL_UART_LEGACY    */
    CLK_RGM_GEN_M,            /* CLK_RGM_INV_SEL_TCXO4          */
    CLK_RGM_NOT_SUPPORTED,    /* CLK_RGM_INV_SEL_MMCDIV         */
    CLK_RGM_VIDEO_VFE_M,      /* CLK_RGM_INV_SEL_VIDEO_VFE      */
    CLK_RGM_I2C0_M,           /* CLK_RGM_INV_SEL_I2C0           */
    CLK_RGM_I2C1_M,           /* CLK_RGM_INV_SEL_I2C1           */
    CLK_RGM_I2C2_M,           /* CLK_RGM_INV_SEL_I2C2           */
    CLK_RGM_I2C_LEGACY_M,     /* CLK_RGM_INV_SEL_I2C_LEGACY     */
    CLK_RGM_SPI0_M,           /* CLK_RGM_INV_SEL_SPI0           */
    CLK_RGM_SPI1_M,           /* CLK_RGM_INV_SEL_SPI1           */
    CLK_RGM_SPI2_M,           /* CLK_RGM_INV_SEL_SPI2           */
    CLK_RGM_ADSP_M,           /* CLK_RGM_INV_SEL_ADSP           */
    CLK_RGM_SBI_M,            /* CLK_RGM_INV_SEL_SBI            */
    CLK_RGM_ICODEC_M,         /* CLK_RGM_INV_SEL_ICODEC         */
    CLK_RGM_SDAC_M,           /* CLK_RGM_INV_SEL_SDAC           */
    CLK_RGM_NOT_SUPPORTED,    /* CLK_RGM_INV_SEL_MMCC           */
    CLK_RGM_NOT_SUPPORTED,    /* CLK_RGM_INV_SEL_GRAPHICS       */
    CLK_RGM_USB_OTG_M,        /* CLK_RGM_INV_SEL_USB_OTG        */
    CLK_RGM_SDCC_HCLK_M,      /* CLK_RGM_INV_SEL_SDCC_HCLK      */
    CLK_RGM_SDCC_MCLK_M,      /* CLK_RGM_INV_SEL_SDCC_MCLK      */
    CLK_RGM_MDP_M,            /* CLK_RGM_INV_SEL_MDP_HCLK       */
    CLK_RGM_CAMIF_PCLK_M      /* CLK_RGM_INV_SEL_CAMIF_PCLK     */
  };

/*===========================================================================
                   Defines to control features
===========================================================================*/
  #define clk_regime_halt_wait 10
  #define clk_regime_do_mclk_sleep 1
  #define clk_regime_do_mclk_hclk_sleep 1
  #define clk_regime_do_vreg_msma_off 1
  #define clk_regime_do_voltage_switch 0
  #define clk_regime_do_vreg_rf_mgt_breakout 1
  #define clk_regime_do_vreg_rf_mgt_lpm 1
  #define clk_regime_rf_lpm_wakeup_delay 800
  #define CLKRGM_LOW_VOLTAGE_MODEM_CLK_HALTA 0xFFFFFC73
  #define CLKRGM_LOW_VOLTAGE_MODEM_CLK_HALTB 0x0003FFFE

  #define clk_regime_do_modem_clk_halt 0
  static uint32 rs_modem_clk_halta=0, rs_modem_clk_haltb=0;


/*===========================================================================

                   FUNCTION DEFINITIONS FOR MODULE

  This section contains local definitions for the functions of this module.

===========================================================================*/

static const clkrgm_adsp_cfg_type * clkrgm_current_adsp_cfg = NULL;

extern void clk_regime_msm_off(clkrgm_msm_clk_type regime);
extern void clk_regime_msm_on(clkrgm_msm_clk_type regime);

extern void clkrgm_clk_off_a_outm( uint32 clk_off_a_mask, uint32 clk_off_a_value );
extern void clkrgm_clk_off_b_outm( uint32 clk_off_b_mask, uint32 clk_off_b_value );

boolean clk_regime_pll_is_on(clkrgm_pll_type pll);
void clk_regime_pll_on (clkrgm_pll_type pll);
void clk_regime_pll_off(clkrgm_pll_type pll);

clkrgm_cpu_speed_type clk_regime_detect_cpu_clk (void);

void clk_regime_set_core_voltage(uint16 oldmv, uint16 newmv);
#ifndef BUILD_BOOT_CHAIN_OEMSBL
static void clk_regime_set_core_voltage_qsc1100(uint16 newmv);

/* --------------------------------------------------------------------------

   Clock regime diag packet user table entry

-------------------------------------------------------------------------- */
PACKED void * clk_regime_diag_set_voltage_per_cpu_speed(PACKED void *req_pkt, uint16 pkt_len);
static const diagpkt_user_table_entry_type clk_regime_diag_tbl[] =
{
  {CLKRGM_SET_VOLTAGE_PER_CPU_SPEED_F, CLKRGM_SET_VOLTAGE_PER_CPU_SPEED_F, clk_regime_diag_set_voltage_per_cpu_speed}
};
void clk_regime_set_voltage_per_cpu_speed(clkrgm_cpu_speed_type clk_frequency,uint16 vreg_mv);
void clk_regime_diag_init (void);


//Following are used for synchronizing VREG Power ON OFF with RF.
static int clk_regime_rf_handshake_power_on=1;
boolean clkregime_power_on_flag_is_set(void)
{
   if(1 == clk_regime_rf_handshake_power_on)
     return TRUE;
   else
     return FALSE;
}

void clkregime_power_on_flag_clear(void)
{
  clk_regime_rf_handshake_power_on = 0;
}


#endif

/*==========================================================================

  FUNCTION      CLK_REGIME_SWITCH_ADSP_HW

  DESCRIPTION   Switch the ADSP to a new clock frequency.

  PARAMETERS    New ADSP speed.

  RETURN VALUE  None.

  SIDE EFFECTS  ADSP clock rate changes.

==========================================================================*/

void clk_regime_switch_adsp_hw(clkrgm_adsp_speed_type s)
{
  const clkrgm_adsp_cfg_type *adsp_cfg;
  int adsp_src_index;
  byte m,n,d;
    
  if(s >= CLKRGM_ADSP_NUM_SPEEDS) 
    return;

  adsp_cfg = &clkrgm_adsp_cfg[s];
  adsp_src_index = HWIO_INF(ADSP_CLK_CTL_REG, ADSP_SRC_SEL);


  if( adsp_src_index == 0 ) 
  {
    /* If the current source is not oscillating, then select it for
       configuration.  Otherwise an attempt at a glitch free 
       switch will fail. */
    if( HWIO_INF(ADSP_CLK_CTL_REG, SRC_SEL0) == 
        HWIO_FVAL(ADSP_CLK_CTL_REG, SRC_SEL0, EXT_CLK) )
    {
      adsp_src_index = 1;
    }
  } else {
    if( HWIO_INF(ADSP_CLK_CTL_REG, SRC_SEL1) == 
        HWIO_FVAL(ADSP_CLK_CTL_REG, SRC_SEL1, EXT_CLK) )
    {
      adsp_src_index = 0;
    }
  }

  /* make sure the PLL is on before switching to it as a source */
  if (adsp_cfg->src == CLKRGM_TYPE1_SRC_IS_PLL )
  {
    if (!clk_regime_pll_is_on(CLKRGM_PLL))
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_CPU_CLIENT,
                              CLKRGM_PLL_ENABLE );
    }
  }

  if(adsp_cfg->div > 1)
  {
    /* If the divider is on, set the source and MND */

    m = 1;
    n = ~(adsp_cfg->div - m);
    d = ~(adsp_cfg->div);

    if(adsp_src_index == 0)
    {
      /* Modify the source we are not using, src 1 */
      HWIO_OUT( ADSP_MND1_REG,
                (m<<HWIO_SHFT(ADSP_MND1_REG, M_VAL))
                | (n<<HWIO_SHFT(ADSP_MND1_REG, N_VAL))
                | (d<<HWIO_SHFT(ADSP_MND1_REG, D_VAL)) );
      
      HWIO_OUTM( ADSP_CLK_CTL_REG,
                 HWIO_FMSK( ADSP_CLK_CTL_REG, DIV_MND_MODE1 )
                 | HWIO_FMSK( ADSP_CLK_CTL_REG, DIV_MND_EN1 )
                 | HWIO_FMSK( ADSP_CLK_CTL_REG, SRC_SEL1 ),
                 CR_FVAL( ADSP_CLK_CTL_REG, DIV_MND_MODE1, DUAL_EDGE_MODE )
                 | CR_FVAL( ADSP_CLK_CTL_REG, DIV_MND_EN1, ENABLED )
                 | adsp_cfg->src << HWIO_SHFT( ADSP_CLK_CTL_REG, SRC_SEL1 )
               );

    } else {
      /* Modify the source we are not using, src 0 */
      HWIO_OUT( ADSP_MND0_REG,
                (m<<HWIO_SHFT(ADSP_MND0_REG, M_VAL))
                | (n<<HWIO_SHFT(ADSP_MND0_REG, N_VAL))
                | (d<<HWIO_SHFT(ADSP_MND0_REG, D_VAL)) );
      
      HWIO_OUTM( ADSP_CLK_CTL_REG,
                 HWIO_FMSK(ADSP_CLK_CTL_REG, DIV_MND_MODE0)
                 | HWIO_FMSK( ADSP_CLK_CTL_REG, DIV_MND_EN0)
                 | HWIO_FMSK( ADSP_CLK_CTL_REG, SRC_SEL0 ),
                 CR_FVAL(ADSP_CLK_CTL_REG, DIV_MND_MODE0, DUAL_EDGE_MODE)
                 | CR_FVAL(ADSP_CLK_CTL_REG, DIV_MND_EN0, ENABLED)
                 | adsp_cfg->src << HWIO_SHFT( ADSP_CLK_CTL_REG, SRC_SEL0 )
               );
    }

  } else {
    /* Divider is 1, disable the MND, set the source */
    if(adsp_src_index == 0)
    {
      HWIO_OUT( ADSP_MND1_REG, 0x0 );
      HWIO_OUTM( ADSP_CLK_CTL_REG,
                 HWIO_FMSK(ADSP_CLK_CTL_REG, DIV_MND_MODE1)
                 | HWIO_FMSK( ADSP_CLK_CTL_REG, DIV_MND_EN1)
                 | HWIO_FMSK( ADSP_CLK_CTL_REG, SRC_SEL1 ),
                 CR_FVAL(ADSP_CLK_CTL_REG, DIV_MND_MODE1, BYPASS)
                 | CR_FVAL(ADSP_CLK_CTL_REG, DIV_MND_EN1, NOT_ENABLED)
                 | adsp_cfg->src << HWIO_SHFT( ADSP_CLK_CTL_REG, SRC_SEL1 )
               );
    } else {
      HWIO_OUT( ADSP_MND0_REG, 0x0 );
      HWIO_OUTM( ADSP_CLK_CTL_REG,
                 HWIO_FMSK(ADSP_CLK_CTL_REG, DIV_MND_MODE0)
                 | HWIO_FMSK( ADSP_CLK_CTL_REG, DIV_MND_EN0)
                 | HWIO_FMSK( ADSP_CLK_CTL_REG, SRC_SEL0 ),
                 CR_FVAL(ADSP_CLK_CTL_REG, DIV_MND_MODE0, BYPASS)
                 | CR_FVAL(ADSP_CLK_CTL_REG, DIV_MND_EN0, NOT_ENABLED)
                 | adsp_cfg->src << HWIO_SHFT( ADSP_CLK_CTL_REG, SRC_SEL0 )
               );
    }
  }


  /* switch the source */
  HWIO_OUTF(ADSP_CLK_CTL_REG, ADSP_SRC_SEL, (1 - adsp_src_index));

  /* After Changing the Clock source, Ground the previous Source */
  if(adsp_src_index)
  {
      HWIO_OUTF( ADSP_CLK_CTL_REG, SRC_SEL1, 0x00);
  }
  else 
  {
      HWIO_OUTF( ADSP_CLK_CTL_REG, SRC_SEL0, 0x00);
  }

  adsp_src_index = 1 - adsp_src_index;
 
  clkrgm_current_adsp_cfg = adsp_cfg;

  clkrgm.adsp_speed = s;
  clkrgm.adsp_pending_speed = s;
}

/*==========================================================================

  FUNCTION      CLK_REGIME_SWITCH_MCLK_QSC1100

  DESCRIPTION   The ARM micro clock is switched to a new frequency.

  PARAMETERS    New MCLK speed.

  RETURN VALUE  None.

  SIDE EFFECTS  The ARM and HCLK frequencies change.

==========================================================================*/
void clk_regime_switch_mclk_qsc1100 (clkrgm_cpu_speed_type s)
{
  /* The active clock source index */
  uint32 micro_src_index;
  /* A flag to indicate whether clock speeds are increasing or not */
  uint32 switch_up_flag;
  /* Prototype of the assembly */
  void clk_regime_switch_asm(uint32 divs, uint32 switch_flag);

  ASSERT( s < CLKRGM_CPU_NUM_SPEEDS );
  ASSERT( clkrgm.cpu_speed < CLKRGM_CPU_NUM_SPEEDS);

  INTLOCK();

  /* Is this a switch to a faster rate, or a slower rate */
  switch_up_flag = ( clkrgm_cpu_cfg[s].global.khz >
                       clkrgm_cpu_cfg[clkrgm.cpu_speed].global.khz ) ? 1 : 0;

  micro_src_index = HWIO_INF(ARM_CFG_REG, ARM_SRC_SEL);

  /* Prepare the other global clock source */
  if( micro_src_index == 0 )
  {
    HWIO_OUTF( ARM_MOD_REG, ARM_DIV_MOD1, clkrgm_cpu_cfg[s].global_cfg.div );
    HWIO_OUTF( ARM_CFG_REG, SRC_SEL1, clkrgm_cpu_cfg[s].global_cfg.src );
  } else {
    HWIO_OUTF( ARM_MOD_REG, ARM_DIV_MOD0, clkrgm_cpu_cfg[s].global_cfg.div );
    HWIO_OUTF( ARM_CFG_REG, SRC_SEL0, clkrgm_cpu_cfg[s].global_cfg.src );
  }

  {
    uint32 t; /* temp register value */
    const clkrgm_mclk_clkrgm_type *cpu_cfg;

    cpu_cfg = &clkrgm_cpu_cfg[s];

    t =  CR_IVAL(ARM_CFG_REG,ARM_RATE_SEL, cpu_cfg->mclk_div);
    t |= CR_IVAL(ARM_CFG_REG,AHB_RATE_SEL, cpu_cfg->hclk_div);
    t |= CR_IVAL(ARM_CFG_REG,EBI2_RATE_SEL, cpu_cfg->ebi2_div);
    t |= CR_IVAL(ARM_CFG_REG,EBI1_RATE_SEL, cpu_cfg->ebi1_div);

    clk_regime_switch_asm( t, switch_up_flag );
  }

  /* Disable the other global clock path for power savings */
  if( micro_src_index == 0 )
  {
    CLKRGM_SET_FVAL_NL( ARM_MOD_REG, ARM_DIV_MOD0, DIV_1 );
    CLKRGM_SET_FVAL_NL( ARM_CFG_REG, SRC_SEL0, EXT_CLK );
  } else {
    CLKRGM_SET_FVAL_NL( ARM_MOD_REG, ARM_DIV_MOD1, DIV_1 );
    CLKRGM_SET_FVAL_NL( ARM_CFG_REG, SRC_SEL1, EXT_CLK );
  }

  clkrgm.cpu_speed = s;
  INTFREE();
}



/*==========================================================================

  FUNCTION      CLK_REGIME_ALL_PLL_ON

  DESCRIPTION   Turn on all the PLLs.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  All the PLLs are turned on.

==========================================================================*/

void clk_regime_all_pll_on (void)
{
  clk_regime_pll_on (CLKRGM_PLL);

} /* clk_regime_all_pll_on */

/*==========================================================================

  FUNCTION      CLK_REGIME_ALL_PLL_OFF

  DESCRIPTION   Turn off all the PLLs, only if they are no longer used.

  PARAMETERS    NOne.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  All the PLLs that are no longer used, are turned off.

==========================================================================*/

void clk_regime_all_pll_off (void)
{
  if (clk_regime_pll_usage_count (CLKRGM_PLL) == 0)
  {
    clk_regime_pll_off (CLKRGM_PLL);
  }
} /* clk_regime_all_pll_off */


/*===========================================================================

FUNCTION CLK_REGIME_MSM_RESET

DESCRIPTION
  Resets the specified clock regime.  This effectively resets all registers
  in that regime (that are attached to a reset) to be set to their default
  powerup values.  Also resets the cicruits to a known state.

  Should be used with caution.

PARAMETERS
  regime - clock regime to be reset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Selected clock regime circuit is reset as if a RESOUT occured.

===========================================================================*/
void clk_regime_msm_reset
(
  clkrgm_msm_clk_type regime
)
{
  /* ------------------------------------------------------------------------
     Verify that clock regime option is supported.
  ------------------------------------------------------------------------ */
  if ( regime >= CLK_RGM_MSM_NUM_REGIMES )
  {
    ERR("Clock regime not supported!",0,0,0);
    return;
  }

  /* ------------------------------------------------------------------------
     Reset specified clock regime.
  ------------------------------------------------------------------------ */
 switch( regime )
 {   
    case CLK_RGM_GEN_M:          /* General TCXO/4                    */
      CLKRGM_PULSE( SW_RESET_REG, XO4_RESET );
      break;

    case CLK_RGM_UART0_M:              /* UART on GSBI0                     */
    case CLK_RGM_SPI0_M:               /* SPI bus on GSBI0                  */
    case CLK_RGM_I2C0_M:               /* I2C bus on GSBI0                  */
    case CLK_RGM_GSBI0_M:              /* GSBI0 (superset of UART0,SPI0,I2C0) */
      CLKRGM_PULSE( SW_RESET_REG, GSBI0_RESET );
      break;

    case CLK_RGM_UART1_M:              /* UART on GSBI1                     */
    case CLK_RGM_SPI1_M:               /* SPI bus on GSBI1                  */
    case CLK_RGM_I2C1_M:               /* I2C bus on GSBI1                  */
    case CLK_RGM_GSBI1_M:              /* GSBI1 (superset of UART1,SPI1,I2C1) */
      CLKRGM_PULSE( SW_RESET_REG, GSBI1_RESET );
      break;

    case CLK_RGM_UART2_M:              /* UART on GSBI2                     */
    case CLK_RGM_I2C2_M:               /* I2C bus on GSBI2                  */
    case CLK_RGM_SPI2_M:               /* SPI bus on GSBI2                  */
    case CLK_RGM_GSBI2_M:              /* GSBI2 (superset of UART2,SPI2,I2C2) */
      CLKRGM_PULSE( SW_RESET_REG, GSBI2_RESET );
      break;

    case CLK_RGM_ECODEC_M:             /* External CODEC                    */
      CLKRGM_PULSE( SW_RESET_REG, ECODECIF_RESET );
      break;

    case CLK_RGM_ADSP_M:               /* Application DSP                   */
      CLKRGM_PULSE( SW_RESET_REG, ADSP_RESET );
      break;

    case CLK_RGM_SBI_M:                /* PMIC SSBI                         */
      CLKRGM_PULSE( SW_RESET_REG, SSBI_PMIC_RESET );
      break;

    case CLK_RGM_ICODEC_M:             /* Internal CODEC                    */
      CLKRGM_PULSE( SW_RESET_REG, ICODEC_TX_RESET );
      CLKRGM_PULSE( SW_RESET_REG, ICODEC_RX_RESET );
      break;

    case CLK_RGM_USB_AHB_CLK_M:        /* USB AHB Bus                       */
    case CLK_RGM_USB_M:                /* Combined USB clock regimes        */
    case CLK_RGM_USB_MCLK_M:
      CLKRGM_PULSE( SW_RESET_REG, USB_RESET );
      break;

    case CLK_RGM_SDAC_M:               /* I2S clock                         */
      CLKRGM_PULSE( SW_RESET_REG, SDAC_RESET );
      break;

    case CLK_RGM_ETM_M:                /* ETM                               */
      CLKRGM_PULSE( SW_RESET_REG, ETM_RESET );
      break;

    case CLK_RGM_I2C_LEGACY_M:         /* I2C bus on GPIO 39,40             */
      CLKRGM_PULSE( SW_RESET_REG, I2C_RESET );
      break;

    case CLK_RGM_CAMCLK_M:             /* Camera clock                      */
      break;

    case CLK_RGM_CAMIF_PCLK_M:         /* Camif pclk                        */
      CLKRGM_PULSE( SW_RESET_REG, CAMIF_PAD_REG_RESET );

    case CLK_RGM_VIDEO_VFE_M:          /* Video Front End                   */
      break;

    case CLK_RGM_VIDEO_M:              /* Video AHB clock                   */
      CLKRGM_PULSE( SW_RESET_REG, VIDEO_RESET );
      break;

    case CLK_RGM_SDCC_HCLK_M:          /* SDCC HCLK (AHB/Bus)               */
    case CLK_RGM_SDCC_MCLK_M:          /* SDCC MCLK (core)                  */
      CLKRGM_PULSE( SW_RESET_REG, SDCC_RESET );
      break;

    case CLK_RGM_MDP_M:                /* MDP                               */
      CLKRGM_PULSE( SW_RESET_REG, MDP_RESET );
      break;

    case CLK_RGM_QMEMBIST_M:           /* QMEMBIST core                     */
      CLKRGM_PULSE( SW_RESET_REG, QMEMBIST_RESET );
      break;

    case CLK_RGM_UART_LEGACY_M:        /* 2-pin UART muxed on GPIO 25,27    */
      CLKRGM_PULSE( SW_RESET_REG, UART_DM_RESET );
      break;

    case CLK_RGM_ICODEC_RX_M:          /* Internal CODEC receive            */
      CLKRGM_PULSE( SW_RESET_REG, ICODEC_TX_RESET );
      break;

    case CLK_RGM_ICODEC_TX_M:          /* Internal CODEC transmit           */
      CLKRGM_PULSE( SW_RESET_REG, ICODEC_TX_RESET );
      break;

    case CLK_RGM_EBI2_CLK_M:           /* EBI2 clock                        */
      CLKRGM_PULSE( SW_RESET_REG, EBI2_RESET );
      break;

    case CLK_RGM_EBI1_CLK_M:           /* EBI1 clock                        */
      CLKRGM_PULSE( SW_RESET_REG, EBI1_RESET );
      break;

    case CLK_RGM_SEC_HCLK_M:   /* security block                    */
      CLKRGM_PULSE( SW_RESET_REG, SEC_RESET );
      break;

    case CLK_RGM_IMEM_CLK_M:           /* IMEM clock                        */
      CLKRGM_PULSE( SW_RESET_REG, IMEM_RESET );
      break;

    case CLK_RGM_MCLK_M:               /* Micro (ARM) clock                 */
    case CLK_RGM_HCLK_M:               /* Bus (AHB) clock                   */
    case CLK_RGM_UXMC_M:               /* UXMC                              */
    case CLK_RGM_AUDIO_M:              /* Audio AHB clock                   */

    default:
      /* Reset not supported for this item. */
      break;
  }

}


/*===========================================================================

FUNCTION  CLK_REGIME_MSM_SEL_CLK_SRC

DESCRIPTION
  Select a clock source for a specified clock regime.

PARAMETERS
  regime - selected clock regime
  source - selected clock source

DEPENDENCIES
  Clock regime input should be stable before enabling clock regime output.

RETURN VALUE
  None.

SIDE EFFECTS
  Clock regime source is switched.

===========================================================================*/
void clk_regime_msm_sel_clk_src
(
  clkrgm_msm_src_sel_type      regime,
  uint32                       source
)
{
  boolean is_on = FALSE;
  uint32  isave;
  boolean set_xo4_div = FALSE;
  uint32 cd_reg = 0, cd_mask = 0;

  /* ------------------------------------------------------------------------
     Verify that clock regime option is supported.
  ------------------------------------------------------------------------ */
  if ( regime >= CLK_RGM_MSM_SRC_SEL_NUM_REGIMES )
  {
    ERR("Clock regime not supported!",0,0,0);
    return;
  }

  /* ------------------------------------------------------------------------
     Lock interrupts around this operation since the corresponding clock
     may be turned off.
  ------------------------------------------------------------------------ */
  INTLOCK_SAV( isave );

  /* ------------------------------------------------------------------------
     If the clock regime is enabled, then disable the clock regime before
     switching the clock source.
  ------------------------------------------------------------------------ */
  is_on = clk_regime_msm_is_on( clkrgm_msm_src_sel_regime_map[ regime ] );

  if ( is_on == TRUE )
  {
    clk_regime_msm_off( clkrgm_msm_src_sel_regime_map[ regime ] );
  }

  
#if 0 /* TBP */
  /* Wait until the clock turns off */
  while( clk_regime_msm_is_on( clkrgm_msm_src_sel_regime_map[ regime ] ) )
  {
    clk_busy_wait(1);
  }
#endif

  /* The XO/4 clock source is not available on GSBI sources and must be 
     set to XO, with an MND divider of 1/4.  A special value indicates XO4
     so that this can be done */

  if( source == CLK_RGM_UART2_SRC_TCXODIV4_V )
  {
    source = CLK_RGM_SRC_SEL_GSBI_XO_V;
    set_xo4_div = TRUE;
  }

  /* ------------------------------------------------------------------------
     Switch the clock source.
  ------------------------------------------------------------------------ */

  switch( regime )
  {
    case CLK_RGM_SRC_SEL_GSBI0:
      if(set_xo4_div == TRUE)
      {
        HWIO_OUT( GSBI0_MN_REG, CLKRGM_GSBI0_MN_REG_XO4 );
        clkrgm.gsbi[0].mn_reg = CLKRGM_GSBI0_MN_REG_XO4;
        cd_reg = CLKRGM_GSBI0_CD_REG_XO4;
        cd_mask =CLKRGM_GSBI0_CD_REG_XO4_MASK;
      }

      cd_mask |= HWIO_FMSK(GSBI0_CD_REG, SRC_SEL);
      cd_reg |= ( source << HWIO_SHFT(GSBI0_CD_REG, SRC_SEL) );
      HWIO_OUTM( GSBI0_CD_REG, cd_mask, cd_reg );

      clkrgm.gsbi[0].cd_reg = HWIO_IN(GSBI0_CD_REG);
      clkrgm.gsbi[0].src = source;

      break;

    case CLK_RGM_SRC_SEL_GSBI1:
      if(set_xo4_div == TRUE)
      {
        HWIO_OUT( GSBI1_MN_REG, CLKRGM_GSBI1_MN_REG_XO4 );
        clkrgm.gsbi[1].mn_reg = CLKRGM_GSBI1_MN_REG_XO4;
        cd_reg = CLKRGM_GSBI1_CD_REG_XO4;
        cd_mask =CLKRGM_GSBI1_CD_REG_XO4_MASK;
      }

      cd_mask |= HWIO_FMSK(GSBI1_CD_REG, SRC_SEL);
      cd_reg |= ( source << HWIO_SHFT(GSBI1_CD_REG, SRC_SEL) );
      HWIO_OUTM( GSBI1_CD_REG, cd_mask, cd_reg );

      clkrgm.gsbi[1].cd_reg = HWIO_IN(GSBI1_CD_REG);
      clkrgm.gsbi[1].src = source;
      break;

    case CLK_RGM_SRC_SEL_GSBI2:
      if(set_xo4_div == TRUE)
      {
        HWIO_OUT( GSBI2_MN_REG, CLKRGM_GSBI2_MN_REG_XO4 );
        clkrgm.gsbi[2].mn_reg = CLKRGM_GSBI2_MN_REG_XO4;
        cd_reg = CLKRGM_GSBI2_CD_REG_XO4;
        cd_mask = CLKRGM_GSBI2_CD_REG_XO4_MASK;
      }

      cd_mask |= HWIO_FMSK(GSBI2_CD_REG, SRC_SEL);
      cd_reg |= ( source << HWIO_SHFT(GSBI2_CD_REG, SRC_SEL) );
      HWIO_OUTM( GSBI2_CD_REG, cd_mask, cd_reg );

      clkrgm.gsbi[2].cd_reg = HWIO_IN(GSBI1_CD_REG);
      clkrgm.gsbi[2].src = source;
      break;

    case CLK_RGM_SRC_SEL_UART_LEGACY:
      HWIO_OUTF( UART_DM_CD_REG, SRC_SEL, source );
      clkrgm.uart_legacy.src = source;
      break;

    case CLK_RGM_SRC_SEL_I2C_LEGACY:
      HWIO_OUTF( MISC_CFG_REG, I2C_SRC_SEL, source );
      clkrgm.i2c_legacy.src = source;
      break;

    case CLK_RGM_SRC_SEL_USB:
      HWIO_OUTF( USB_CD_REG, SRC_SEL, source );
      clkrgm.usb.src = source;
      break;

    case CLK_RGM_SRC_SEL_MDDI_CLIENT_VFE:
      HWIO_OUTF( VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_SRC_SEL, source );
      clkrgm.video_vfe.src = source;
      break;

    case CLK_RGM_SRC_SEL_CAMIF_PCLK:
      HWIO_OUTF( CAMCLK_CD_REG, CAM_PCLK_SRC_SEL, source );
      clkrgm.camclk.src = source;
      break;

    /* Unsupported clock regimes. */
    case CLK_RGM_SRC_SEL_TCXO4:
    case CLK_RGM_SRC_SEL_ADSP:
    default:
      /* Option not supported. */
      break;
  }

  /* ------------------------------------------------------------------------
     If the clock regime was enabled, then re-enable the clock regime after
     switching the source.
  ------------------------------------------------------------------------ */
  if ( is_on == TRUE )
  {
    clk_regime_msm_on( clkrgm_msm_src_sel_regime_map[ regime ] );
  }

  /* ------------------------------------------------------------------------
     Unlock interrupts now that the selected clock is back on.
  ------------------------------------------------------------------------ */
  INTFREE_SAV( isave );
}


/*===========================================================================

FUNCTION  CLK_REGIME_MSM_SEL_CLK_INV

DESCRIPTION
  Select a clock polarity for a specified clock regime.

PARAMETERS
  regime - selected clock regime
  invert - TRUE = invert signal

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Clock signal may be inverted.

===========================================================================*/
void clk_regime_msm_sel_clk_inv
(
  clkrgm_msm_src_inv_type  regime,
  boolean                  invert
)
{
  uint32  isave;
  boolean is_on = FALSE;

  /* ------------------------------------------------------------------------
     Verify that clock regime option is supported.
  ------------------------------------------------------------------------ */
  if ( regime >= CLK_RGM_MSM_INV_SEL_NUM_REGIMES )
  {
    ERR("Clock regime not supported!",0,0,0);
    return;
  }

  /* ------------------------------------------------------------------------
     Lock interrupts around this operation since the corresponding clock
     may be turned off.
  ------------------------------------------------------------------------ */
  INTLOCK_SAV( isave );

  /* ------------------------------------------------------------------------
     If the clock regime is enabled, then disable the clock regime before
     changing the clock divider.
  ------------------------------------------------------------------------ */
  is_on = clk_regime_msm_is_on( clkrgm_msm_inv_sel_regime_map[ regime ] );

  if ( is_on == TRUE )
  {
    clk_regime_msm_off( clkrgm_msm_inv_sel_regime_map[ regime ] );
  }

#if 0 /* TBP */
  /* ------------------------------------------------------------------------
     Insert an unconditional delay since the caller may have disabled the
     clock regime immediately prior to calling this function.
  ------------------------------------------------------------------------ */
  while( clk_regime_msm_is_on( clkrgm_msm_inv_sel_regime_map[ regime ] ) )
  {
    clk_busy_wait(1);
  }
#endif

  /* ------------------------------------------------------------------------
     Switch the clock polarity.
  ------------------------------------------------------------------------ */
  switch( regime )
  {

    case CLK_RGM_INV_SEL_USB:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( USB_CD_REG, USB_FS_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( USB_CD_REG, USB_FS_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_BT_PCM:
      break;

    case CLK_RGM_INV_SEL_ECODECIF:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( ECODEC_CD_REG, ECODECIF_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( ECODEC_CD_REG, ECODECIF_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_RPCM:
      break;

    case CLK_RGM_INV_SEL_ETM:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( ARM_CLK_INV_REG, ETM_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( ARM_CLK_INV_REG, ETM_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_ADIE_CODEC:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_RX_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_RX_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_VIDEO:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_UART0:
    case CLK_RGM_INV_SEL_I2C0:
    case CLK_RGM_INV_SEL_SPI0:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_UART1:
    case CLK_RGM_INV_SEL_I2C1:
    case CLK_RGM_INV_SEL_SPI1:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( GSBI1_CD_REG, GSBI1_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( GSBI1_CD_REG, GSBI1_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_UART2:
    case CLK_RGM_INV_SEL_I2C2:
    case CLK_RGM_INV_SEL_SPI2:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( GSBI2_CD_REG, GSBI2_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( GSBI2_CD_REG, GSBI2_CLK_INV, NOT_INVERTED );
      }
      break;


    case CLK_RGM_INV_SEL_UART_LEGACY:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( UART_DM_CD_REG, UART_DM_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( UART_DM_CD_REG, UART_DM_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_TCXO4:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( MISC_CFG_REG, XO4_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( MISC_CFG_REG, XO4_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_MMCDIV:
      break;

    case CLK_RGM_INV_SEL_VIDEO_VFE:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_I2C_LEGACY:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( MISC_CFG_REG, I2C_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( MISC_CFG_REG, I2C_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_ADSP:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( ADSP_CFG_REG, ADSP_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( ADSP_CFG_REG, ADSP_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_SBI:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( MISC_CFG_REG, SSBI_PMIC_CLK_INV, INVERTED );
      } else { 
        CLKRGM_SET_FVAL_NL( MISC_CFG_REG, SSBI_PMIC_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_ICODEC:
      if ( invert == TRUE )
      {
        CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_RX_CLK_INV, INVERTED );
        CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_2X_RX_CLK_INV, INVERTED );
        CLKRGM_SET_FVAL_NL( ICODEC_TX_CD_REG, ICODEC_TX_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_RX_CLK_INV, NOT_INVERTED );
        CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_2X_RX_CLK_INV, NOT_INVERTED );
        CLKRGM_SET_FVAL_NL( ICODEC_TX_CD_REG, ICODEC_TX_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_SDAC:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( I2S_CD_REG, I2S_MSTR_CLK_INV, INVERTED);
        CLKRGM_SET_FVAL_NL( I2S_CD_REG, I2S_BIT_CLK_INV, INVERTED);
      } else {
        CLKRGM_SET_FVAL_NL( I2S_CD_REG, I2S_MSTR_CLK_INV, NOT_INVERTED);
        CLKRGM_SET_FVAL_NL( I2S_CD_REG, I2S_BIT_CLK_INV, NOT_INVERTED);
      }
      break;

    case CLK_RGM_INV_SEL_MMCC:
      break;

    case CLK_RGM_INV_SEL_GRAPHICS:
      break;

    case CLK_RGM_INV_SEL_USB_OTG:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( ARM_CLK_INV_REG, USB_AHB_CLK_INV, INVERTED);
      } else {
        CLKRGM_SET_FVAL_NL( ARM_CLK_INV_REG, USB_AHB_CLK_INV, NOT_INVERTED);
      }
      break;

    case CLK_RGM_INV_SEL_SDCC_HCLK:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( ARM_CLK_INV_REG, SDCC_AHB_CLK_INV, INVERTED);
      } else {
        CLKRGM_SET_FVAL_NL( ARM_CLK_INV_REG, SDCC_AHB_CLK_INV, NOT_INVERTED);
      }
      break;

    case CLK_RGM_INV_SEL_SDCC_MCLK:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( SDCC_CD_REG, SDCC_CLK_INV, INVERTED );
      } else {
        CLKRGM_SET_FVAL_NL( SDCC_CD_REG, SDCC_CLK_INV, NOT_INVERTED );
      }
      break;

    case CLK_RGM_INV_SEL_MDP_HCLK:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( ARM_CLK_INV_REG, MDP_AHB_CLK_INV, INVERTED);
      } else {
        CLKRGM_SET_FVAL_NL( ARM_CLK_INV_REG, MDP_AHB_CLK_INV, NOT_INVERTED);
      }
      break;

    case CLK_RGM_INV_SEL_CAMIF_PCLK:
      if ( invert == TRUE ) {
        CLKRGM_SET_FVAL_NL( CAMCLK_CD_REG, CAM_PCLK_INVERT, INVERT);
      } else {
        CLKRGM_SET_FVAL_NL( CAMCLK_CD_REG, CAM_PCLK_INVERT, DO_NOT_INVERT);
      }
      break;

    default:
      /* Option not supported. */
      break;
  }

  /* ------------------------------------------------------------------------
     If the clock regime was enabled, then re-enable the clock regime.
  ------------------------------------------------------------------------ */
  if ( is_on == TRUE )
  {
    clk_regime_msm_on( clkrgm_msm_inv_sel_regime_map[ regime ] );
  }

  /* ------------------------------------------------------------------------
     Unlock interrupts now that the selected clock is back on.
  ------------------------------------------------------------------------ */
  INTFREE_SAV( isave );
}


/*===========================================================================

  FUNCTION CLK_REGIME_SWITCH_MCLK

  DESCRIPTION
    Switch the MCLK source to that specified in the argument.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
#ifdef CLKRGM_FREQ_STEP
clkrgm_cpu_speed_type freq_steps[] = 
  {
    CLKRGM_CPU_QUARTER_SPEED, // 24MHz
    CLKRGM_CPU_HALF_SPEED,    // 48MHz
  };
#define CLKRGM_NUM_FREQ_STEPS 2
#endif

void clk_regime_switch_mclk
(
  clkrgm_cpu_speed_type  new_mclk
)
{
  int i=0;

  #ifdef FEATURE_CLKREGIM_DYNAMIC_VOLTAGE
  uint16 old_mv, new_mv;

  old_mv = 0;
  new_mv = 0;
  #endif

  if( (new_mclk >= CLKRGM_CPU_NUM_SPEEDS) 
      || (clkrgm.cpu_speed >= CLKRGM_CPU_NUM_SPEEDS) )
  {
    ERR_FATAL("Illegal MCLK speed", 0,0,0);
  }

  if (clkrgm.cpu_speed == new_mclk)
  {
    return;
  }

  MSM_INTLOCK();

  #ifdef FEATURE_CLKREGIM_DYNAMIC_VOLTAGE
  if (clk_regime_dyn_voltage_on)
  {
    old_mv = clkrgm_cpu_cfg[clkrgm.cpu_speed].pm_vreg_mv;
    new_mv = clkrgm_cpu_cfg[new_mclk].pm_vreg_mv;

    /* Turn up MSMC voltage if necessary */
    if (clkrgm.cpu_mv != new_mv)
    {
      if(old_mv < new_mv)
      {
        clk_regime_set_core_voltage(old_mv, new_mv);
      }
    }
  }
  #endif

  /* ------------------------------------------------------------------------
     If the new configuration requires a different source than the old
     configuration, and if source required by the new configuration is a PLL,
     then request that the PLL required by the new configuration be enabled
     prior to switching the processor clock to the new configuration.
  ------------------------------------------------------------------------ */

  if ( clkrgm_cpu_cfg[ new_mclk ].global_cfg.src != 
       clkrgm_cpu_cfg[ clkrgm.cpu_speed ].global_cfg.src )
  {
    if ( clkrgm_cpu_cfg[ new_mclk ].global_cfg.src == CLKRGM_MCLK_SRC_PLL )
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_CPU_CLIENT, CLKRGM_PLL_ENABLE );
    }
  }

  /* ------------------------------------------------------------------------
     If switching to a faster clock, then first change the memory interface
     programming and then switch the processor to the faster clock.
     Otherwise, first switch the processor to the slower clock and
     then change the memory interface programming.  This procedure ensures
     that there will never be an access on the memory bus that is too short
     for the memory components.
  ------------------------------------------------------------------------ */

  /* If the micro clock is the same, but the HCLK divider changed, then 
     only adjust the memory wait states and HCLK divider. */

  if ( (clkrgm_cpu_cfg[ new_mclk ].mclk.khz ==
         clkrgm_cpu_cfg[ clkrgm.cpu_speed ].mclk.khz) && 
       (clkrgm_cpu_cfg[ new_mclk ].hclk_div != 
        clkrgm_cpu_cfg[ clkrgm.cpu_speed ].hclk_div))
  {
    uint32 m, v; /* register mask and value */
    const clkrgm_mclk_clkrgm_type *cpu_cfg;

    /* faster HCLK */
    if (clkrgm_cpu_cfg[ new_mclk ].hclk_div < 
        clkrgm_cpu_cfg[ clkrgm.cpu_speed ].hclk_div)
      boot_hw_program_ebi_regs( &(clkrgm_ebi_regs[ new_mclk ]) );

    /* Mask containing the RATE_SELs */
    m =  HWIO_FMSK(ARM_CFG_REG,EBI2_RATE_SEL);
    m |= HWIO_FMSK(ARM_CFG_REG,EBI1_RATE_SEL);
    m |= HWIO_FMSK(ARM_CFG_REG,AHB_RATE_SEL);
    m |= HWIO_FMSK(ARM_CFG_REG,ARM_RATE_SEL);

    cpu_cfg = &clkrgm_cpu_cfg[new_mclk];

    /* Values for each of the RATE_SELs shifted and OR'd */
    v  = CR_IVAL(ARM_CFG_REG,EBI2_RATE_SEL, cpu_cfg->ebi2_div);
    v |= CR_IVAL(ARM_CFG_REG,EBI1_RATE_SEL, cpu_cfg->ebi1_div);
    v |= CR_IVAL(ARM_CFG_REG,AHB_RATE_SEL, cpu_cfg->hclk_div);
    v |= CR_IVAL(ARM_CFG_REG,ARM_RATE_SEL, cpu_cfg->mclk_div);

    /* Set the rate select values in one write */
    HWIO_OUTM ( ARM_CFG_REG, m, v );

    /* slower HCLK */
    if (clkrgm_cpu_cfg[ new_mclk ].hclk_div > 
        clkrgm_cpu_cfg[ clkrgm.cpu_speed ].hclk_div)
      boot_hw_program_ebi_regs( &(clkrgm_ebi_regs[ new_mclk ]) );

  }

  /* If switching to a faster clock .. */
  if ( clkrgm_cpu_cfg[ new_mclk ].mclk.khz >
         clkrgm_cpu_cfg[ clkrgm.cpu_speed ].mclk.khz )
  {
    /* Prepare new wait states. */

    boot_hw_program_ebi_regs( &(clkrgm_ebi_regs[ new_mclk ]) );

#ifdef CLKRGM_FREQ_STEP
    for (i=0; i < CLKRGM_NUM_FREQ_STEPS; i++)
    {
      if ((clkrgm_cpu_cfg[ freq_steps[i] ].mclk.khz > 
          clkrgm_cpu_cfg[ clkrgm.cpu_speed ].mclk.khz) &&
          (clkrgm_cpu_cfg[ freq_steps[i] ].mclk.khz <
          clkrgm_cpu_cfg[ new_mclk ].mclk.khz))
      {
        clk_regime_switch_mclk_qsc1100(freq_steps[i]);
      }
    }
#endif
    clk_regime_switch_mclk_qsc1100(new_mclk);

  }
  else /* .. switching to a slower clock. */
  if (clkrgm_cpu_cfg[ new_mclk ].mclk.khz !=
         clkrgm_cpu_cfg[ clkrgm.cpu_speed ].mclk.khz) 
  {
#ifdef CLKRGM_FREQ_STEP
    for (i=CLKRGM_NUM_FREQ_STEPS-1; i >= 0; i--)
    {
      if ((clkrgm_cpu_cfg[ freq_steps[i] ].mclk.khz < 
          clkrgm_cpu_cfg[ clkrgm.cpu_speed ].mclk.khz) &&
          (clkrgm_cpu_cfg[ freq_steps[i] ].mclk.khz >
          clkrgm_cpu_cfg[ new_mclk ].mclk.khz))
      {
        clk_regime_switch_mclk_qsc1100(freq_steps[i]);
      }
    }
#endif
    clk_regime_switch_mclk_qsc1100(new_mclk);

    /* Prepare new wait states. */

    boot_hw_program_ebi_regs( &(clkrgm_ebi_regs[ new_mclk ]) );

    #if defined(FEATURE_CLKREGIM_DYNAMIC_VOLTAGE) && !defined(BUILD_BOOT_CHAIN_OEMSBL)

    if (clk_regime_dyn_voltage_on)
    {
      /* Turn down MSMC voltage if necessary */
      if (clkrgm.cpu_mv != new_mv)
      {
        if(old_mv > new_mv && 
           new_mclk < CLKRGM_CPU_XO_SPEED && 
           clkrgm.adsp_speed <= CLKRGM_ADSP_64P0MHZ_SPEED
          )
        {
          clk_regime_set_core_voltage(old_mv, new_mv);
        }
      }
    }

    #endif
  }

  /* ------------------------------------------------------------------------
     If the new configuration requires a different source than the old
     configuration, and if source required by the old configuration is a PLL,
     then request that the PLL required by the old configuration be disabled
     after switching the processor clock to the new configuration.
  ------------------------------------------------------------------------ */

  if ( clkrgm_cpu_cfg[ new_mclk ].global_cfg.src !=
       clkrgm_cpu_cfg[ clkrgm.cpu_speed ].global_cfg.src )
  {
    if ( clkrgm_cpu_cfg[ clkrgm.cpu_speed ].global_cfg.src == CLKRGM_MCLK_SRC_PLL )
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_CPU_CLIENT, CLKRGM_PLL_DISABLE );
    }
  }

  /* ------------------------------------------------------------------------
     Update the clock services to use the new HCLK frequency for memory bus
     timing related functions, such as busy wait.
  ------------------------------------------------------------------------ */

#ifndef BUILD_BOOT_CHAIN_OEMSBL
  clk_set_mclk_parms( clkrgm_cpu_cfg[ new_mclk ].hclk.mhz );
#endif /* BUILD_BOOT_CHAIN_OEMSBL */
  /* ------------------------------------------------------------------------
     Update the clkrgm state information.
  ------------------------------------------------------------------------ */

  clkrgm.cpu_speed = new_mclk;

  MSM_INTFREE();
}

#ifndef BUILD_BOOT_CHAIN_OEMSBL


/*===========================================================================

  FUNCTION CLK_REGIME_INIT

  DESCRIPTION
    This is the MSM clock regime initialization function.
    It sets up various clock regime and clock control registers, main and
    codec PLLs, and selects clock sources for various MSM blocks.

  PARAMETERS
    None.

  DEPENDENCIES
    TCXO and SLEEP xtal clock sources need to be stable.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The PLL and clock regime hardware is initialized.

===========================================================================*/
void clk_regime_init( void )
{
  uint32  isave;
  uint32  i;


  /* Detect and memorize CPU rate. */
  i = clk_regime_detect_cpu_clk();
  if(i < CLKRGM_CPU_NUM_SPEEDS ) {
    clkrgm.cpu_speed = i;
  } else  {
    ERR_FATAL("Unknown CPU rate at clk_regime_init", 0,0,0);
  }

  /* Boot loaders initalize the CPU speed to FULL_SPEED */
  clk_set_mclk_parms( clkrgm_cpu_cfg[clkrgm.cpu_speed].hclk.mhz );

  clk_regime_client_init();

  /* Take all blocks out of reset */
  HWIO_OUT(SW_RESET_REG, 0x0);

  /* TBP */
  CLKRGM_SET_FVAL( MSM_BRIDGE_CFG, TIMEOUT_EN, DISABLED );

  /* External clock disabled for power savings when sources are off */
  CLKRGM_SET_FVAL( MISC_CFG_REG, EXTERNAL_CLK_EN, NOT_ENABLED);

  /* ------------------------------------------------------------------------
     The full speed clock configuration is established at boot time to
     minimize the boot time, so initialize the local state variable
     to be consistent.
  ------------------------------------------------------------------------ */
  #ifdef FEATURE_CLKREGIM_DYNAMIC_VOLTAGE
    clkrgm.adsp_speed = CLKRGM_ADSP_96P0MHZ_SPEED;
    clkrgm_modem.mdsp_speed = CLKRGM_MDSP_SPEED_HIGH;

    /* Doesn't work since pm_init() hasn't been called yet. 
    clk_regime_set_core_voltage(clkrgm_cpu_cfg[clkrgm.cpu_speed].pm_vreg_mv);
    */
    clkrgm.cpu_mv    = MSMC_VOLTAGE_HIGH_V; // assume default is HIGH_V

  #endif


  clkrgm.cpu_client_mask  = 0;
  clkrgm.cpu_client_usage = 0;

  /* Register a Boot Client with DCVS so that Clock is held at Full speed,
   * and voltage is not lowered, until the end of tmc_init. TMC maintains a 
   * timer that expire and calls deregister(BOOT).
  */
  #if defined (FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)
    clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_BOOT);
  #endif

  /* ------------------------------------------------------------------------
     Compute and save the EBI register values for each microprocessor clock
     configuration.
  ------------------------------------------------------------------------ */

  for ( i = 0 ; i < (uint32)CLKRGM_CPU_NUM_SPEEDS ; i++ )
  {
    boot_hw_compute_ebi_regs( &(clkrgm_ebi_regs[ i ]),
                                clkrgm_cpu_cfg[ i ].hclk.khz );
  }

  /* ------------------------------------------------------------------------
     Lock interrupts.
  ------------------------------------------------------------------------ */

  INTLOCK_SAV( isave );

  /* ------------------------------------------------------------------------
     Switch the CPU to the default TCXO clock prior to PLL configuration.
  ------------------------------------------------------------------------ */


#if 0
  /* A verification of the clock switch code. */
  {
    clkrgm_cpu_speed_type   cpu_clk, cpu_next;

    for( cpu_clk = CLKRGM_CPU_XO_SPEED;
         cpu_clk < CLKRGM_CPU_NUM_SPEEDS;
         cpu_clk++)
    {
      for( cpu_next = CLKRGM_CPU_XO_SPEED;
           cpu_next < CLKRGM_CPU_NUM_SPEEDS;
           cpu_next++)
      {
        clk_regime_switch_mclk_qsc1100( cpu_clk );
        clk_regime_switch_mclk_qsc1100( cpu_next );
      }
    }
  }
#endif
    
  clk_regime_switch_mclk_qsc1100( CLKRGM_CPU_TCXO_SPEED );
  /* ------------------------------------------------------------------------
     Turn on the PLLs so the sources for the following clock regimes are on
     and stable for configuration.
  ------------------------------------------------------------------------ */

  clkrgm.pll_usage = 0;

  /* Enable the PLL. */

  clk_regime_pll_on( CLKRGM_PLL );

  /* ------------------------------------------------------------------------
     Switch the CPU clock to full speed and set the ARM bus clock divider
     accordingly.
  ------------------------------------------------------------------------ */
#if defined  FEATURE_PLL_192
  clk_regime_switch_mclk_qsc1100( CLKRGM_CPU_FULL_SPEED );
#elif defined FEATURE_PLL_384
  clk_regime_switch_mclk_qsc1100( CLKRGM_CPU_128_SPEED);
#endif
 
  /* ------------------------------------------------------------------------
     Initialize the Type 1 clock regimes.
  ------------------------------------------------------------------------ */

  /* ADSP clock regime */
  // should it depend on 1x or 2x mode?
  clk_regime_switch_adsp_hw( CLKRGM_ADSP_96P0MHZ_SPEED );

  /* ICODEC clock regime */

  HWIO_OUT( ICODEC_RX_MN_REG, clkrgm_icodec_cfg[ CLKRGM_CODEC_CLK_2P048_MHZ ].mn_reg );
  HWIO_OUT( ICODEC_RX_CD_REG, clkrgm_icodec_cfg[ CLKRGM_CODEC_CLK_2P048_MHZ ].cd_reg );
  CLKRGM_SET_FVAL( ICODEC_TX_CD_REG, CLK_SEL, ICODEC_RX );
  clkrgm.icodec_speed = CLKRGM_CODEC_CLK_2P048_MHZ;

  /* ECODEC clock regime */

  HWIO_OUT( ECODEC_MN_REG, clkrgm_ecodec_cfg[CLKRGM_ECODEC_CLK_4P096_MHZ].mn_reg );
  HWIO_OUT( ECODEC_CD_REG, clkrgm_ecodec_cfg[CLKRGM_ECODEC_CLK_4P096_MHZ].cd_reg );
  clkrgm.ecodec_speed = CLKRGM_ECODEC_CLK_4P096_MHZ;

  /* I2S clock regime */

  HWIO_OUT( I2S_MN_REG, clkrgm_sdac_cfg[ CLKRGM_SDAC_CLK_256_KHZ ].mn_reg );
  HWIO_OUT( I2S_CD_REG, clkrgm_sdac_cfg[ CLKRGM_SDAC_CLK_256_KHZ ].cd_reg );
  clkrgm.sdac_speed = CLKRGM_SDAC_CLK_256_KHZ;

  /* Camera clock regime, should feature it, as low,mid tiers don't have cam */
  HWIO_OUT( CAMCLK_MN_REG, clkrgm_camclk_cfg[CLKRGM_CAMCLK_24P576_MHZ].mn_reg );
  HWIO_OUT( CAMCLK_CD_REG, clkrgm_camclk_cfg[CLKRGM_CAMCLK_24P576_MHZ].cd_reg );
  clkrgm.camclk = clkrgm_camclk_cfg[CLKRGM_CAMCLK_24P576_MHZ];

  /* Video Front End */
  clkrgm.video_vfe.src = HWIO_FVAL(VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_SRC_SEL, CAMIF_PCLK);
  HWIO_OUT( VIDEO_VFE_CD_REG,
            clkrgm.video_vfe.src << HWIO_SHFT(VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_SRC_SEL ) );

  /* SDCC  clock regime */
  HWIO_OUT( SDCC_MN_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_FAST ].mn_reg );
  HWIO_OUT( SDCC_CD_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_FAST ].cd_reg );
  clkrgm.sdcc = clkrgm_sdcc_cfg[ CLKRGM_SDCC_FAST ];

  /* The UIM specification requires that the UIM clock be in the */
  /* range of 3.25 MHz - 4 MHz and the GSBI clock should be 64 MHz. */
  HWIO_OUT( GSBI0_MN_REG, 
            clkrgm_gsbi0_uart_cfg[CLKRGM_UART_DM_64P0_MHZ].mn_reg);
  HWIO_OUT( GSBI0_CD_REG, 
            clkrgm_gsbi0_uart_cfg[CLKRGM_UART_DM_64P0_MHZ].cd_reg);
  clkrgm.gsbi[0] = clkrgm_gsbi0_uart_cfg[CLKRGM_UART_DM_64P0_MHZ];

  clk_regime_msm_sel_uart_dm_clk(64000000);

  /* Set the SBI clock source to be the TCXO clock. */
  HWIO_OUTM( MISC_CFG_REG, 
             HWIO_FMSK( MISC_CFG_REG, EXTERNAL_CLK_SEL) \
             | HWIO_FMSK( MISC_CFG_REG, SSBI_PMIC_SRC_SEL) \
             | HWIO_FMSK( MISC_CFG_REG, RXADC_SRC_SEL) \
             | HWIO_FMSK( MISC_CFG_REG, RFMS_XO_SRC_SEL) \
             | HWIO_FMSK( MISC_CFG_REG, I2C_SRC_SEL),
             CR_FVAL( MISC_CFG_REG, EXTERNAL_CLK_SEL, EXT_CLK1 ) \
             | CR_FVAL( MISC_CFG_REG, SSBI_PMIC_SRC_SEL, XO ) \
             | CR_FVAL( MISC_CFG_REG, RXADC_SRC_SEL, XO ) \
             | CR_FVAL( MISC_CFG_REG, RFMS_XO_SRC_SEL, XO ) \
             | CR_FVAL( MISC_CFG_REG, I2C_SRC_SEL, XO ) );

  /* USB is PLL 60 MHz */
  clkrgm.usb.mn_reg = CLKRGM_USB_60_MHZ_MN_REG_VAL;
  clkrgm.usb.cd_reg = CLKRGM_USB_60_MHZ_CD_REG_VAL;
  clkrgm.usb.src = HWIO_FVAL( USB_CD_REG, SRC_SEL, PLL_SRC );
  HWIO_OUT( USB_MN_REG, clkrgm.usb.mn_reg );
  HWIO_OUT( USB_CD_REG, clkrgm.usb.cd_reg );

  // I2C clock is tcxo, should feature it
  clkrgm.i2c_legacy.src = HWIO_FVAL( MISC_CFG_REG, I2C_SRC_SEL, XO );
  HWIO_OUTF( MISC_CFG_REG, I2C_SRC_SEL, clkrgm.i2c_legacy.src );

  /* ------------------------------------------------------------------------
     Configure remaining MISC_CLK_CTL options.
  ------------------------------------------------------------------------ */

  /* Enable ahb_wr_clk for for AHB register accesses only. */
  /* Does not exist on QSC1100 */
  /* CLKRGM_W_MASKED( MISC_CLK_CTL, AHB_WR_CLK_EN, 0 ); */


  /*-----------------------------------------------------------------------*/
  /* Initialize the device manager                                         */
  /*-----------------------------------------------------------------------*/

#ifndef BUILD_BOOT_CHAIN_OEMSBL
  clk_regime_devman_init(clk_regime_devman_handler);

  /* Register our internal client */
  clk_regime_devman_register(
    &clkrgm.devman_client,
    CLKRGM_DEVMAN_CLIENT_GROUP_QDSP_SERVICES);
#endif

  /* ------------------------------------------------------------------------
     Enable the start-up default clock regimes.
  ------------------------------------------------------------------------ */
#define T_BRINGUP
#ifndef T_BRINGUP

  /* Disable the MSM clocks */
  clkrgm_clk_off_a_outm( CLKRGM_REG_A_ALL_MASK, CLKRGM_REG_A_ALL_MASK );
  clkrgm_clk_off_b_outm( CLKRGM_REG_B_ALL_MASK, CLKRGM_REG_B_ALL_MASK );

  /* ground the clock source in the mn counter */
  CLKRGM_CD_REG_SET_MND( GSBI1_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );

  /* initialize modem clocks */
  clk_regime_modem_init();

  /* Enable General clock regime. This is for clk_tick_isr() registration. */

  clk_regime_msm_enable_client( CLK_RGM_GEN_M, CLKRGM_CLKREGIM );

  clk_regime_msm_enable_client( CLK_RGM_ADSP_M, CLKRGM_CLKREGIM );

  /* Not enabled anywhere else in the code. */

  clk_regime_msm_enable_client( CLK_RGM_I2C_M, CLKRGM_CLKREGIM );

  clk_regime_msm_enable_client( CLK_RGM_SBI_M, CLKRGM_CLKREGIM );

  clk_regime_msm_enable_client (CLK_RGM_MDP_M, CLKRGM_CLKREGIM);

  /* ------------------------------------------------------------------------
     Disable clock regimes that are excluded from the CLK_RGM_MSM_ALL_M mask.
  ------------------------------------------------------------------------ */

  /* Disable the ETM clock regime if ETM operation is not selected via the
     hardware MODE pins. */

  if ( CLKRGM_GET( CHIP_MODE, MODE0_PIN ) == 0 )
  {
    clk_regime_msm_disable_client( CLK_RGM_ETM_M, CLKRGM_CLKREGIM );
  }

  /* ------------------------------------------------------------------------
     Disable PLLs that are not required yet.
  ------------------------------------------------------------------------ */

  /* Disable the MODEM PLL. */

  if ( clkrgm.pll_usage == 0 )
  {
    clk_regime_pll_off( CLKRGM_PLL );
  }

#else

  /* Enable the MSM clocks */
  //clkrgm_clk_off_a_outm( CLKRGM_REG_A_ALL_MASK, 0x0 );
  HWIO_OUT(ARM_CLK_EN_REG,CLKRGM_REG_A_DEFAULT_BOOT);
  clkrgm_clk_off_b_outm( CLKRGM_REG_B_ALL_MASK, 0x1c5047a0 );
  
  /* ground the clock source for GSBI1 Clock */
  CLKRGM_CD_REG_SET_MND( GSBI1_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );

  /* initialize modem clocks */
  clk_regime_modem_init();

#endif /* !T_BRINGUP */

  /* Register the diag packet function */
#ifdef FEATURE_CLKREGIM_DYNAMIC_VOLTAGE
  clk_regime_diag_init ();
#endif /* FEATURE_CLKREGIM_PMIC_VOLTAGE_CTL */

  /* Allow UXMC and aDSP voting to halt the AHB during ARM halt */
  CLKRGM_SET_FVAL( ARM_CFG_REG, AHB_VOTE_EN, ENABLED );
  
#if defined (FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)
    clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_BOOT);
#endif

  /* ------------------------------------------------------------------------
     Unlock interrupts.
  ------------------------------------------------------------------------ */

  INTFREE_SAV( isave );
}
#endif /* BUILD_BOOT_CHAIN_OEMSBL */


/*===========================================================================

FUNCTION    CLK_REGIME_ICODEC_CLK_DISABLE

DESCRIPTION
  Disables codec PLL for the CLKRGM_PLL_ICODEC_CLKRGM_CL client.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void clk_regime_icodec_clk_disable ( void )
{
  /* No longer needed. */
}


/*===========================================================================

FUNCTION    CLK_REGIME_SEL_ICODEC_CLK

DESCRIPTION
  Selects a clock source for the internal codec.

PARAMETERS
  new_clk - desired internal codec clock frequency.

DEPENDENCIES
  The internal codec clock regime should be disabled.

RETURN VALUE
  None.

SIDE EFFECTS
  Internal codec clock frequency will be changed.

===========================================================================*/

void clk_regime_sel_icodec_clk
(
  clkrgm_codec_clk_type new_clk       /* Desired clock input to the codec */
)
{
  boolean is_on;

  #ifdef CLKRGM_TEST
#error code not present
  #endif


  if (new_clk == clkrgm.icodec_speed)
    return;

  /* ------------------------------------------------------------------------
     If the clock regime is enabled, then disable the clock regime before
     changing the clock regime configuration.
  ------------------------------------------------------------------------ */

  /* Determine if the clock regime is currently on. */

  is_on = clk_regime_msm_is_on( CLK_RGM_ICODEC_M );

  /* If the clock regime is on, then turn it off. */

  if ( is_on == TRUE ) clk_regime_msm_off( CLK_RGM_ICODEC_M );
  /* ------------------------------------------------------------------------
     If the new configuration requires a different source than the old
     configuration, and if source required by the new configuration is a PLL,
     then request that the PLL required by the new configuration be enabled
     prior to switching to the new configuration.
  ------------------------------------------------------------------------ */

  if ( clkrgm_icodec_cfg[ new_clk ].src != clkrgm_icodec_cfg[ clkrgm.icodec_speed].src )
  {
    if ( clkrgm_icodec_cfg[ new_clk ].src == CLKRGM_TYPE1_SRC_IS_PLL )
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_ICODEC_CLIENT,
                              CLKRGM_PLL_ENABLE );
    }
  }

  /* ------------------------------------------------------------------------
     Program the ICODEC clock regime MND counter values.
  ------------------------------------------------------------------------ */

  HWIO_OUT( ICODEC_RX_MN_REG, clkrgm_icodec_cfg[ new_clk ].mn_reg );
  HWIO_OUT( ICODEC_RX_CD_REG, clkrgm_icodec_cfg[ new_clk ].cd_reg );

  /* ------------------------------------------------------------------------
     If the new configuration requires a different source than the old
     configuration, and if source required by the old configuration is a PLL,
     then request that the PLL required by the old configuration be disabled
     after switching to the new configuration.
  ------------------------------------------------------------------------ */

  if ( clkrgm_icodec_cfg[ new_clk ].src != clkrgm_icodec_cfg[ clkrgm.icodec_speed ].src )
  {
    if ( clkrgm_icodec_cfg[ clkrgm.icodec_speed ].src == CLKRGM_TYPE1_SRC_IS_PLL )
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_ICODEC_CLIENT, CLKRGM_PLL_DISABLE );
    }
  }

  /* ------------------------------------------------------------------------
     Update clock regime driver state information.
  ------------------------------------------------------------------------ */

  clkrgm.icodec_speed = new_clk;

  /* ------------------------------------------------------------------------
     If the clock regime was enabled upon entry, then re-enable the clock
     regime before exit.
  ------------------------------------------------------------------------ */

  if ( is_on == TRUE ) clk_regime_msm_on( CLK_RGM_ICODEC_M );
}


/*===========================================================================

FUNCTION    CLK_REGIME_SEL_ECODEC_CLK

DESCRIPTION
  Selects a clock source for the external codec.

PARAMETERS
  new_clk - desired external codec clock frequency.

DEPENDENCIES
  The external codec clock regime should be disabled.

RETURN VALUE
  None.

SIDE EFFECTS
  External codec clock frequency will be changed.

===========================================================================*/
void clk_regime_sel_ecodec_clk
(
  clkrgm_ecodec_clk_type new_clk       /* Desired clock input to the codec */
)
{
  boolean is_on;

  if (new_clk == clkrgm.ecodec_speed)
    return;
  /* ------------------------------------------------------------------------
     If the clock regime is enabled, then disable the clock regime before
     changing the clock regime configuration.
  ------------------------------------------------------------------------ */

  /* Determine if the clock regime is currently on. */

  is_on = clk_regime_msm_is_on( CLK_RGM_ECODEC_M );

  /* If the clock regime is on, then turn it off. */

  if ( is_on == TRUE ) clk_regime_msm_off( CLK_RGM_ECODEC_M );

  /* ------------------------------------------------------------------------
     If the new configuration requires a different source than the old
     configuration, and if source required by the new configuration is a PLL,
     then request that the PLL required by the new configuration be enabled
     prior to switching to the new configuration.
  ------------------------------------------------------------------------ */

  if ( clkrgm_ecodec_cfg[ new_clk ].src != clkrgm_ecodec_cfg[ clkrgm.ecodec_speed ].src )
  {
    if ( clkrgm_ecodec_cfg[ new_clk ].src == CLKRGM_TYPE1_SRC_IS_PLL )
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_ECODEC_CLIENT, CLKRGM_PLL_ENABLE );
    }
  }

  /* ------------------------------------------------------------------------
     Program the ECODEC clock regime MND counter values.
  ------------------------------------------------------------------------ */

  HWIO_OUT( ECODEC_MN_REG, clkrgm_ecodec_cfg[ new_clk ].mn_reg );
  HWIO_OUT( ECODEC_CD_REG, clkrgm_ecodec_cfg[ new_clk ].cd_reg );

  /* ------------------------------------------------------------------------
     If the new configuration requires a different source than the old
     configuration, and if source required by the old configuration is a PLL,
     then request that the PLL required by the old configuration be disabled
     after switching to the new configuration.
  ------------------------------------------------------------------------ */

  if ( clkrgm_ecodec_cfg[ new_clk ].src != clkrgm_ecodec_cfg[ clkrgm.ecodec_speed ].src )
  {
    if ( clkrgm_ecodec_cfg[ clkrgm.ecodec_speed ].src == CLKRGM_TYPE1_SRC_IS_PLL )
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_ECODEC_CLIENT, CLKRGM_PLL_DISABLE );
    }
  }

  /* ------------------------------------------------------------------------
     Update clock regime driver state information.
  ------------------------------------------------------------------------ */

  clkrgm.ecodec_speed = new_clk;

  /* ------------------------------------------------------------------------
     If the clock regime was enabled upon entry, then re-enable the clock
     regime before exit.
  ------------------------------------------------------------------------ */

  if ( is_on == TRUE ) clk_regime_msm_on( CLK_RGM_ECODEC_M );
}



/*===========================================================================

FUNCTION    CLK_REGIME_SEL_SDAC_CLK

DESCRIPTION
  Selects a clock source for the external stereo DAC.

PARAMETERS
  new_clk - desired external stereo DAC clock frequency.

DEPENDENCIES
  The external stereo DAC clock regime should be disabled.

RETURN VALUE
  None.

SIDE EFFECTS
  External stereo DAC clock frequency will be changed.

===========================================================================*/
void clk_regime_sel_sdac_clk
(
  clkrgm_sdac_clk_type new_clk          /* Desired clock input to the SDAC */
)
{
  boolean is_on;

  if (new_clk == clkrgm.sdac_speed)
    return;
  /* ------------------------------------------------------------------------
     If the clock regime is enabled, then disable the clock regime before
     changing the clock regime configuration.
  ------------------------------------------------------------------------ */

  /* Determine if the clock regime is currently on. */

  is_on = clk_regime_msm_is_on( CLK_RGM_SDAC_M );

  /* If the clock regime is on, then turn it off. */

  if ( is_on == TRUE ) clk_regime_msm_off( CLK_RGM_SDAC_M );

  /* ------------------------------------------------------------------------
     Program the clock regime MND counter values.
  ------------------------------------------------------------------------ */

  HWIO_OUT( I2S_MN_REG, clkrgm_sdac_cfg[ new_clk ].mn_reg );
  HWIO_OUT( I2S_CD_REG, clkrgm_sdac_cfg[ new_clk ].cd_reg );

  /* ------------------------------------------------------------------------
     Update clock regime driver state information.
  ------------------------------------------------------------------------ */

  clkrgm.sdac_speed = new_clk;

  /* ------------------------------------------------------------------------
     If the clock regime was enabled upon entry, then re-enable the clock
     regime before exit.
  ------------------------------------------------------------------------ */

  if ( is_on == TRUE ) clk_regime_msm_on( CLK_RGM_SDAC_M );
}




#ifndef BUILD_BOOT_CHAIN_OEMSBL
void clk_regime_sdram_self_refresh_and_wdog_reset(void)
{
  /* A stub to make it compile */
}

/*===========================================================================

  FUNCTION CLK_REGIME_SET_MCLK_SLEEP

  DESCRIPTION
    Switch the MCLK to the sleep XTAL, and turn off the MCLK.  Wake up
    on a non-masked interrupt, restore the original MCLK, and then
    return.

  DEPENDENCIES
    Interrupts should be locked by the caller.

  RETURN VALUE
    None

===========================================================================*/

static void clk_regime_config_pm_for_tcxo_shutdown(pm_vreg_mask_type* pm_vreg_mask,
                                                   pm_vreg_mask_type* pm_vreg_status)
{
   /* Setup for TCXO shutdown sleep clk_regime_set_mclk_sleep(). */
   /* XO warmup recommendation is 0ms.  XO is always on to generate sleep xtal */

   pm_vreg_mask_type vreg_mask=0, vreg_status;

   if( qsc11x0_hw_rev.pg == 1)
   {
     pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_MSMC_ID );
   }
   
   if(PM_ERR_FLAG__SUCCESS != pm_config_buck_min_p_on_time(BUCK_SMPS_S2, PM_MIN_P_ON_TIME_307))
   {
     MSG_ERROR("Fail to set Min P_ON Time, TCXO Entry",0,0,0);
   }

   pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_MSME1_ID );
   pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_MSME2_ID );


   pm_config_tcxo_ctrl(PM_POLARITY_ACTIVE_HIGH, 164);
   pm_xo_sel_alt_sleep_clk_src(PM_XO_19_2MHZ_XTAL_OSC);

   // KIP A0 has overshoot problem when switch from PWM to PFM mode. 
   // So during XO shutdown, don't use to the automatic mode.
   // It can be remove when KIP B0 is available and retested
   if( qsc11x0_hw_rev.pmic_hw_version != 1 )
   {
     pm_tcxo_cmd(PM_TCXO_AUTOMATIC_MODE_CMD);
   }

   /* VREG voting api return status for all VREGs */
   vreg_status = (pm_vreg_mask_type) pm_vreg_status_get();

   if (vreg_status & PM_VREG_MSMA_M)
   {
      /* Set the mask bit only if PM_VREG_MSMA_M is ON else donot set the mask bit */
      /* With Tcxo OFF we can OFF PM_VREG_MSMA_M and with Tcxo ON we ON PM_VREG_MSMA_M */
      /* Don't Swith OFF MSMA if FM Path is ON */
      if(clkrgm_fmpath_on == FALSE)
      {
         vreg_mask |= PM_VREG_MSMA_M;
      }
   }
   if( qsc11x0_hw_rev.pg != 2)
   {
     if (vreg_status & PM_VREG_RUIM_M)
     {
       /* Commenting out the below line as it controls the RST Pin for RUIM Card 
        * During Sleep, this Pin should not be touched.
        */
       /* pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_RUIM_ID ); */
	 
	   // could also configure GPIO 15 as input with pull up
	   #ifdef FEATURE_MANAGE_GPIOS_FOR_SLEEP
         gpio_config_gpios_for_uim_sleep();
       #endif
     }
   }
   if (vreg_status & PM_VREG_5V_M)
   {
	  /* PM_VREG_5V_M is on */
	  /* Set the mask bit only if PM_VREG_5V_M is ON else donot set the mask bit */
	  /* With Tcxo OFF we can OFF PM_VREG_5V_M and with Tcxo ON we ON PM_VREG_5V_M */
	  vreg_mask |= PM_VREG_5V_M;
   }

   if (vreg_status & PM_VREG_USB3P3_M)
   {
      /* PM_VREG_USB3P3_M is on */
      /* Set the mask bit only if PM_VREG_USB3P3_M is ON else donot set the mask bit */
      /* With Tcxo OFF we can OFF PM_VREG_USB3P3_M and with Tcxo ON we ON PM_VREG_5V_M */
      vreg_mask |= PM_VREG_USB3P3_M;
   }


   if( qsc11x0_hw_rev.pg == 1)
   {
     pm_vreg_set_level(PM_VREG_RFA1_ID,1075);  // Both PG1 and PG3 set to 1.075 V during sleep
   }

   if(qsc11x0_hw_rev.pg == 1)
   {
     pm_vreg_set_level(PM_VREG_RFA2_ID,1800); // PG1 requires RFA2 to be 1.8 V during sleep
   }

   if(qsc11x0_hw_rev.pg == 2 || qsc11x0_hw_rev.pg == 3)
   { 
     vreg_mask |= PM_VREG_RFA1_M;
     vreg_mask |= PM_VREG_RFA2_M;
   }

  pm_vreg_smps_individual_clock_enable(PM_VREG_SMPS_RF_ID, FALSE);
  pm_vreg_smps_individual_clock_enable(PM_VREG_SMPS_BOOST_ID, FALSE);

  pm_vreg_control(PM_OFF_CMD, vreg_mask);

//  HWIO_OUT(RXA2D_EN, 0x3e);  // should remove
  
  *pm_vreg_mask   = vreg_mask;
  *pm_vreg_status = vreg_status;
} /* clk_regime_config_pm_for_tcxo_shutdown */

static void clk_regime_config_pm_for_tcxo_wakeup(pm_vreg_mask_type vreg_mask,
                                                 pm_vreg_mask_type vreg_status)
{

  if (vreg_mask)
  {
     pm_vreg_control(PM_ON_CMD, vreg_mask); 
  }
  
  if( qsc11x0_hw_rev.pg != 2)
  {
     if (vreg_status & PM_VREG_RUIM_M)
     {
       /* Commenting out the below line as it controls the RST Pin for RUIM Card 
        * During Sleep, this Pin should not be touched.
        */
       /* pm_lp_mode_control( PM_OFF_CMD, PM_VREG_LP_RUIM_ID ); */

       #ifdef FEATURE_MANAGE_GPIOS_FOR_SLEEP
          gpio_restore_gpios_after_uim_sleep();
       #endif
     }
  }
  
  if( qsc11x0_hw_rev.pg == 1 && qsc11x0_hw_rev.msm_hw_version == 0 )
   {
	   pm_vreg_set_level(PM_VREG_RFA1_ID,1300);
	   pm_vreg_set_level(PM_VREG_RFA2_ID,2200);
   }
  
  if( qsc11x0_hw_rev.pg == 3 )
   {
	   // For PG3
	   pm_vreg_set_level(PM_VREG_RFA1_ID,1200);
	   // take it out of bypass mode
   }

  clk_regime_rf_handshake_power_on = 1;
  
  pm_vreg_smps_individual_clock_enable(PM_VREG_SMPS_BOOST_ID, TRUE);
  pm_vreg_smps_individual_clock_enable(PM_VREG_SMPS_RF_ID, TRUE);

//  HWIO_OUT(RXA2D_EN, 0x3f);   // Should remove it, rf driver handles it
  
  if(PM_ERR_FLAG__SUCCESS != pm_config_buck_min_p_on_time(BUCK_SMPS_S2, PM_MIN_P_ON_TIME_68))
   {
     MSG_ERROR("Fail to set Min P_ON Time, TCXO Exit",0,0,0);
   }

} /* clk_regime_config_pm_for_tcxo_wakeup */

void clk_regime_set_mclk_sleep( void )
{
  clkrgm_cpu_speed_type cpu_speed;
  clkrgm_qdsp_clk_frequency_type adsp_speed;
  clkrgm_pll_state_type     pll_state;

  pm_vreg_mask_type pm_vreg_mask=0, pm_vreg_status=0;

  dword clk_off_a;
  unsigned char pa_register[3];
  dword clk_off_b;
  dword modem_halt;
  #ifdef T_MSM6800B
  dword modem_halt_2;
  #endif

  #ifdef CLKRGM_TEST
#error code not present
  #endif

  #ifdef FEATURE_MANAGE_GPIOS_FOR_SLEEP
    gpio_config_gpios_for_sleep();
  #endif

  /* Allow the HCLK to be turned off when the ARM is turned off */

#ifdef CLKRGM_TEST
#error code not present
#endif
    CLKRGM_SET_FVAL(ARM_CFG_REG, AHB_SLEEP_EN, ENABLED);

  /* Remeber current MCLK setup */
  cpu_speed = clkrgm.cpu_speed;


  /* Set ADSP to TCXO otherwise clk_regime_pll_off(PLL) may not
   * succeed in turning off the PLL.
   */
  adsp_speed = clkrgm.adsp_speed;

  clk_regime_set_qdsp_clk_frequency(CLKRGM_ADSP_19P2MHZ);
  clk_regime_switch_qdsp_clk(); /* Inform the Devman */
  clk_regime_complete_qdsp_clk_switch();

  /* Read the Adie Registers to Check if FM Path is ON or OFF */
  ADIE_IN(ADIE_RX_PA_ENABLE_REGISTER1_R,&pa_register[0]);
  ADIE_IN(ADIE_RX_PA_ENABLE_REGISTER2_R,&pa_register[1]);
  ADIE_IN(ADIE_RX_PA_CONTROL_REGISTER1_R,&pa_register[2]);

  /* Set fmpath_on static variable to TRUE if FM Path is ON */
  if(((pa_register[0] == 0xF8)&&(pa_register[2] == 0x00))||
     ((pa_register[1] == 0x0F)||(pa_register[1] == 0x0C))||
     ((pa_register[0] == 0xF8)&&(pa_register[2] == 0x01)))
  {
     clkrgm_fmpath_on = TRUE;
  }
  else
  {
     clkrgm_fmpath_on = FALSE;
  }

  clk_regime_config_pm_for_tcxo_shutdown(&pm_vreg_mask, &pm_vreg_status);

  // Allow ARM clock domain to be turned off
  CLKRGM_SET_FVAL( ARM_CFG_REG, AHB_VOTE_EN, NOT_ENABLED );

  /* Set MCLK to TCXO */
  clk_regime_switch_mclk(CLKRGM_CPU_TCXO_SPEED);

  /* Remember PLL states */
  pll_state = clkrgm.pll_state;

  /* Turn off the PLLs */
  clk_regime_pll_off( CLKRGM_PLL );

  /* Now VREG_MPLL can turn off */
  CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_CLK_EN, ENABLED );
  if (qsc11x0_hw_rev.pg == 2 )
  {
     pm_vreg_control(PM_OFF_CMD, PM_VREG_RUIM_M); 
     pm_vreg_set_level(PM_VREG_RUIM_ID, 1350);
  }
  else
  {
     pm_vreg_control(PM_OFF_CMD, PM_VREG_MPLL_M); 
  }

  /* Disable all unnecessary clock regimes before turning off clocks */
  clk_off_a = HWIO_IN(CLK_OFF_A);
  clkrgm_clk_off_a_outm( CLKRGM_REG_A_ALL_MASK, CLKRGM_REG_A_ALL_MASK );

  clk_off_b = HWIO_IN(CLK_OFF_B);
  clkrgm_clk_off_b_outm( CLKRGM_REG_B_ALL_MASK, CLKRGM_REG_B_ALL_MASK ); 

  modem_halt = HWIO_IN(MODEM_CLK_HALT);
  CLKRGM_SET_MASK( MODEM_CLK_HALT, CLKRGM_MODEM_REG_ALL_MASK );
  modem_halt_2 = HWIO_IN(MODEM_CLK_HALT_2);
  CLKRGM_SET_MASK( MODEM_CLK_HALT_2, CLKRGM_MODEM_REG_ALL_MASK);
  
  if (modem_halt == CLKRGM_LOW_VOLTAGE_MODEM_CLK_HALTA && rs_modem_clk_halta != 0)
  {
    modem_halt = rs_modem_clk_halta;
    rs_modem_clk_halta=0x0;
  }
  if (modem_halt_2 == CLKRGM_LOW_VOLTAGE_MODEM_CLK_HALTB && rs_modem_clk_haltb != 0)
  {
    modem_halt_2 = rs_modem_clk_haltb;
    rs_modem_clk_haltb=0x0;
  }

  HWIO_OUT(PAUSE_TIMER, clk_regime_halt_wait); // allow time for halted clocks to shutdown

  /* enable foot switch for MDSP */
  CLKRGM_CLEAR( MODEM_CLK_MDSP_FS_CTL, HM_CLK_UP_FS_ON );


  /* Low voltage required TCXO shutdown. Halt modem clocks before voltage down. */
  CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_CLK_EN, ENABLED ); // don't mask it off yet
  if(qsc11x0_hw_rev.pmic_hw_version == 1)
  {
     if(qsc11x0_hw_rev.pg == 1)
	 {
        pm_vreg_set_level(PM_VREG_MSMC_ID,1075);
	 }
     pm_vreg_smps_config (PM_VREG_SMPS_S1_ID,PM_VREG_SMPS_MODE__PWM  );  //forced PWM all the time
     pm_vreg_smps_config (PM_VREG_SMPS_S2_ID,PM_VREG_SMPS_MODE__PBM  );  //forced PFM
  }
  pm_vreg_set_level(PM_VREG_S1_ID, 1075);
  pm_vreg_set_level(PM_VREG_S2_ID, 2100);
  CLKRGM_SET_FVAL( MISC_CFG_REG,SSBI_PMIC_CLK_EN, NOT_ENABLED);

  set_micro_to_32khz_and_kill_tcxo();

  CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_CLK_EN, ENABLED );

  if(qsc11x0_hw_rev.pmic_hw_version == 1)
  {
     pm_vreg_smps_config (PM_VREG_SMPS_S2_ID, PM_VREG_SMPS_MODE__PWM);
  }


  if(qsc11x0_hw_rev.pg == 3 || qsc11x0_hw_rev.pg == 2 )
  {
#ifdef FEATURE_POWER_TEST_BUILD
     if(qsc11x0_hw_rev.msm_hw_version > 0)
	 {
		 pm_vreg_set_level(PM_VREG_S1_ID, 1200);
	 }
	 else
	 {
		 pm_vreg_set_level(PM_VREG_S1_ID, 1300);
	 }
#else
     pm_vreg_set_level(PM_VREG_S1_ID, 1300);
#endif
     pm_vreg_set_level(PM_VREG_S2_ID, 2300);
  }
  else
  {
     pm_vreg_set_level(PM_VREG_S1_ID, 1500);
     pm_vreg_set_level(PM_VREG_S2_ID, 2400);
  }

  // PG2 uses RUIM to power the PLL
  if(qsc11x0_hw_rev.pg == 2 )
  {
     pm_vreg_set_level(PM_VREG_RUIM_ID, 2700);
     pm_vreg_control(PM_ON_CMD, PM_VREG_RUIM_M);
  }
  else
  {
     pm_vreg_control(PM_ON_CMD, PM_VREG_MPLL_M);
  }


  if(qsc11x0_hw_rev.pg == 1 )
  {
     pm_lp_mode_control( PM_OFF_CMD, PM_VREG_LP_MSMC_ID );
	 if( qsc11x0_hw_rev.msm_hw_version == 0 )
     {
        pm_vreg_set_level(PM_VREG_MSMC_ID, 1300);
     }
	 else
	 {
       pm_vreg_set_level(PM_VREG_MSMC_ID, 1200);
	 }
  }

  pm_lp_mode_control( PM_OFF_CMD, PM_VREG_LP_MSME1_ID );
  pm_lp_mode_control( PM_OFF_CMD, PM_VREG_LP_MSME2_ID );


  /* Turn on main PLL, which sources chipxn clock generator (and perhaps
     the post-sleep MCLK) */
  /* turn on  PLL */
  clk_regime_pll_on( CLKRGM_PLL );

  /* Set our MCLK back to the last configured setting (before this call) */
  clk_regime_switch_mclk( cpu_speed );

  clk_regime_set_qdsp_clk_frequency(adsp_speed);
  clk_regime_switch_qdsp_clk(); /* Inform the DevMan */
  clk_regime_complete_qdsp_clk_switch();

  /* disable foot switch for MDSP */
  CLKRGM_SET( MODEM_CLK_MDSP_FS_CTL, HM_CLK_UP_FS_ON );

  /* Re-enable clock regimes */
  clkrgm_clk_off_a_outm( CLKRGM_REG_A_ALL_MASK, clk_off_a );
  clkrgm_clk_off_b_outm( CLKRGM_REG_B_ALL_MASK, clk_off_b ); 

  HWIO_OUT( MODEM_CLK_HALT, modem_halt);
  HWIO_OUT( MODEM_CLK_HALT_2, modem_halt_2);

  clk_regime_config_pm_for_tcxo_wakeup(pm_vreg_mask, pm_vreg_status);

  #ifdef FEATURE_MANAGE_GPIOS_FOR_SLEEP
    gpio_restore_gpios_after_sleep();
  #endif
  /* I2C comes up in a bad state because of gpio's state being changed in sleep mode. */
  clk_regime_msm_reset(CLK_RGM_I2C_LEGACY_M);

  // Allow ARM clock domain to be turned off
  CLKRGM_SET_FVAL( ARM_CFG_REG, AHB_VOTE_EN, ENABLED );
}


/*===========================================================================

  FUNCTION CLK_REGIME_UP_SLEEP

  DESCRIPTION
    This function powers down the microprocessor by removing MCLK.
    An unmasked interrupt will wake it back up.

  DEPENDENCIES
    XTAL and SLEEP XTAL clock sources need to be stable.
    Interrupts should be locked by the caller.

    Assumes that MICRO_CLK_DIV_WB = MICRO_CLK_OFF_WB + 4.

  RETURN VALUE
    None

===========================================================================*/

#define clk_regime_do_uP_sleep        1
#define clk_regime_do_uP_off          1
#define clk_regime_do_uP_sleep_simple 0
#define clk_regime_do_bus_lock_ddr_hclk_sleep 1
#define clk_regime_do_arm_halt_at_speed 1

#if 0
void  clk_regime_turn_off_micro_clk( void )
{
#ifdef CLKRGM_TEST
#error code not present
#endif
  { 
    /* power down ARM */
    CLKRGM_SET_FVAL(ARM_CFG_REG, ARM_SLEEP_WFI, ENABLED);
    CLKRGM_SET_FVAL(ARM_CFG_REG, ARM_SLEEP_WFI, NOT_ENABLED);
  }
}
#endif
void clk_regime_halt_arm_microprocessor(
                          clkrgm_mclk_sleep_type mclk_sleep,
                          clkrgm_hclk_sleep_type hclk_sleep)
{
  clkrgm_cpu_speed_type saved_cpu_speed = clkrgm.cpu_speed;

#ifdef CLKRGM_TEST
#error code not present
#endif /* CLKRGM_TEST */
    boolean ok_to_halt=FALSE;
    boolean hclk_1_1 = FALSE;
    clkrgm_cpu_speed_type arm_halt_cpu_speed;

    mmu_dcache_flush_all();

    /* ------------------------------------------------------------------------
     If the ADSP requires the HCLK, promote the hclk_sleep state to full speed.
    ------------------------------------------------------------------------ */
    if( adsp_dme_mode == CLKRGM_DME_REQUIRED )
    {
      if( hclk_sleep <= CLKRGM_HCLK_FULL )
      {
        hclk_sleep = CLKRGM_HCLK_FULL;
      }
    }

    /* ------------------------------------------------------------------------
     If the USB requires the HCLK, promote the hclk_sleep state to full speed.
    ------------------------------------------------------------------------ */
    // Huan/Sridhar testing USB, force it so it stays at 48 MHz
//    usb_needs_hclk = 1;  
    if( usb_needs_hclk )
    {
      if( hclk_sleep <= CLKRGM_HCLK_FULL )
      {
        hclk_sleep = CLKRGM_HCLK_FULL;
      }
    }

    /* ------------------------------------------------------------------------
     If the SD card requires the HCLK, promote the hclk_sleep state to full speed.
    ------------------------------------------------------------------------ */
    if( sdio_active_mode )
    {
      if( hclk_sleep <= CLKRGM_HCLK_FULL )
      {
        hclk_sleep = CLKRGM_HCLK_FULL;
      }
    }
    /* ------------------------------------------------------------------------
       Default the HCLK to be left on when the ARM is halted.
    ------------------------------------------------------------------------ */

    CLKRGM_SET_FVAL( ARM_CFG_REG, AHB_SLEEP_EN, NOT_ENABLED );

    arm_halt_cpu_speed = clkrgm.cpu_speed;
//    hclk_1_1 = ( clkrgm_cpu_cfg[ clkrgm.cpu_speed ].hclk_div == CLKRGM_BUS_IS_MCLK_DIV1 );
    hclk_1_1 = TRUE; 

    if ( hclk_sleep <= CLKRGM_HCLK_TCXO)
    {
        arm_halt_cpu_speed = CLKRGM_CPU_TCXO_SPEED;
//        hclk_1_1 = TRUE;
    }
#if 0
    /* Need 1-1 mclk/hclk at arm halt.*/
    else if ( !hclk_1_1 ) 
    {
      arm_halt_cpu_speed = CLKRGM_CPU_HALF_SPEED;    // mclk, hclk = 96, 96MHz
      hclk_1_1 = TRUE;
    }
#endif

    if ( (mclk_sleep == CLKRGM_MCLK_OFF) && 
          hclk_1_1 && (hclk_sleep < CLKRGM_HCLK_NO_SWITCH) )
    {
        if (clkrgm.cpu_speed != arm_halt_cpu_speed)
          clk_regime_switch_mclk( arm_halt_cpu_speed );
        ok_to_halt = TRUE;
    }

    /* If we can't halt, then spin. */
    if (!ok_to_halt)
    {
      dword clk_int0_bits = 0;
      dword clk_int1_bits = 0;

      /* The following loops on the interrupt status registers until an
         unmasked interrupt occurs. This simulates the CPU wakeup in response
         to an interrupt. */

      while( ( clk_int0_bits | clk_int1_bits ) == 0 )
      {
        clk_int0_bits =  HWIO_INM(IRQ_STATUS_0, HWIO_IN(IRQ_EN_0));
        clk_int1_bits =  HWIO_INM(IRQ_STATUS_1, HWIO_IN(IRQ_EN_1));
      }

      return;
    }
#ifdef FEATURE_ENABLE_AHB_CLK_HALT
    if ( hclk_sleep == CLKRGM_HCLK_OFF )
    {
      CLKRGM_SET_FVAL( ARM_CFG_REG, AHB_SLEEP_EN, ENABLED );
//	  HWIO_OUT(AUTOKICK_START,1);
//	  mmu_standby_wfi();
//	  HWIO_OUT(AUTOKICK_START,0);
// Not work	  CLKRGM_SET_FVAL( ARM_CFG_REG, ARM_SLEEP_WFI, ENABLED );
//      clk_regime_turn_off_micro_clk();
    }
#endif
    /* ------------------------------------------------------------------------
       Stop the ARM until the next interrupt.
    ------------------------------------------------------------------------ */

    /* turn_off_micro_clk() is linked to IMEM address, 
     * so has to be copied there now.
     */

    #ifdef CLKRGM_TEST
#error code not present
    #endif

//      clk_regime_turn_off_micro_clk( );
	  mmu_standby_wfi();

    #ifdef CLKRGM_TEST
#error code not present
    #endif


    /* ------------------------------------------------------------------------
       Restore the original CPU configuration before returning.
    ------------------------------------------------------------------------ */
    clk_regime_switch_mclk( saved_cpu_speed );

#ifdef CLKRGM_TEST
#error code not present
#endif /* CLKRGM_TEST */
}

void clk_regime_uP_sleep( clkrgm_mclk_sleep_type mclk_sleep,
                          clkrgm_hclk_sleep_type hclk_sleep)
{
  clk_regime_halt_arm_microprocessor( mclk_sleep, hclk_sleep );
}

#endif /*  BUILD_BOOT_CHAIN_OEMSBL */


/*===========================================================================

FUNCTION CLK_REGIME_SET_QDSP_DME_MODE

DESCRIPTION
  Allows outside users to inform Clock Regime services as to whether DME
  is required or not for ADSP.

DEPENDENCIES
  Interrupts should be locked by the caller

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void clk_regime_set_qdsp_dme_mode
(
  clkrgm_dme_mode_type dme_mode
)
{
  adsp_dme_mode = dme_mode;
}


/*===========================================================================

FUNCTION CLK_REGIME_SET_QDSP_CLK_FREQUENCY

DESCRIPTION
  Allows outside users to inform Clock Regime services as to what will be the
  ADSP clock frequency is going to be. This function is used to prepare the
  data structures for the ADSP clock before changing the clock frequency.This
  function doesn't actually switch the clock source for the ADSP, that will
  be done by clk_regime_switch_adsp_clk().

  This function is provided so as to reduce the time during which the DSP is
  held in wait mode for switching the clock frequency. This way, the selection
  of the frequency can be done any time before the actual switching and the DSP
  can continue to operate while the clock frequency selection is being carried
  out. The DSP needs to be put inot wait mode only when the actual switching of
  the clock frequency is required.

DEPENDENCIES
  Interrupts should be locked by the caller

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void clk_regime_set_qdsp_clk_frequency
(
  clkrgm_qdsp_clk_frequency_type  clk_frequency
)
{
  /* If any rate needs to be mapped to a different speed, it can
     be done with this table. */
  static const clkrgm_adsp_speed_type
    clkrgm_adsp_config_from_freq[CLKRGM_ADSP_CLK_MAX_NUM] =
  {
    CLKRGM_ADSP_TCXO_SPEED,
    CLKRGM_ADSP_38P4MHZ_SPEED,
    CLKRGM_ADSP_48P0MHZ_SPEED,
    CLKRGM_ADSP_64P0MHZ_SPEED,
    CLKRGM_ADSP_96P0MHZ_SPEED
  };

  clkrgm_adsp_speed_type new_speed;
  
  if(clk_frequency < CLKRGM_ADSP_CLK_MAX_NUM)
  {
    /* Get new speed for request (actual speed differs in some cases,
       but only for the unused speeds */
    new_speed = clkrgm_adsp_config_from_freq[clk_frequency];

    if(new_speed != clkrgm.adsp_speed)
    {
      /* Clock Speed change will happen to new_speed, 
       * when clk_regime_switch_qdsp_clk is called 
       */
      clkrgm.adsp_pending_speed = new_speed;
    }
  }
}

void clk_regime_switch_adsp_clk
(
  clkrgm_adsp_speed_type new_speed
)
{

  if(new_speed == clkrgm.adsp_speed)
  {
    /* No change */
    clkrgm.adsp_pending_speed = new_speed;
    return;
  }


  /* Turn on the new clock source, if needed before the switch */
  if(clkrgm_adsp_cfg[new_speed].src !=
     clkrgm_adsp_cfg[clkrgm.adsp_speed].src)
  {
    switch(clkrgm_adsp_cfg[new_speed].src)
    {
      case CLKRGM_TYPE1_SRC_IS_PLL:
        clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_ADSP_CLIENT,
                                CLKRGM_PLL_ENABLE);
        break;

      default:
        break;
    }
  }

  clkrgm.adsp_pending_speed = new_speed;
  clk_regime_switch_qdsp_clk_internal();
  clk_regime_complete_qdsp_clk_switch();
}

/*===========================================================================

FUNCTION CLK_REGIME_SWITCH_QDSP_CLK

DESCRIPTION
  Switches the ADSP clock frequency to a previously defined frequency
  selected thorough clk_regime_set_qdsp_clk_frequency().
  This API will be called by QDSP services.

DEPENDENCIES
  clk_regime_set_qdsp_clk_frequency() should have been called prior to select
  the desired frequency prior to calling this function .

  Interrupts should be locked by the caller

RETURN VALUE
  None

SIDE EFFECTS
  The ADSP clock frequency will be changed to the new value
===========================================================================*/
void clk_regime_switch_qdsp_clk( void )
{
  /* 
   * Here we can safely assume that BUILD_BOOT_CHAIN_OEMSBL is not defined
   * Hence removed the condition for the same.
   */
  clkrgm_device_param_type  param;
    
  if(clkrgm.adsp_pending_speed == clkrgm.adsp_speed)
  {
    /* No change */
    return;
  }

  /* Use DEVMAN to set adsp so that DCVS and QDSP services will
   * stay in sync
   */
  param.minimum_speed =
    clkrgm_adsp_cfg[clkrgm.adsp_pending_speed].clk_freq_khz;

  clk_regime_devman_request(
    &clkrgm.devman_client, CLKRGM_DEVICE_ADSP,
    CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);
}

/*===========================================================================

FUNCTION clk_regime_switch_qdsp_clk_internal

DESCRIPTION
  Switches the ADSP clock frequency to a previously defined frequency
  selected thorough clk_regime_set_qdsp_clk_frequency().

DEPENDENCIES
  clk_regime_set_qdsp_clk_frequency() should have been called prior to select
  the desired frequency prior to calling this function .

  Interrupts should be locked by the caller

RETURN VALUE
  None

SIDE EFFECTS
  The ADSP clock frequency will be changed to the new value
===========================================================================*/
void clk_regime_switch_qdsp_clk_internal( void )
{
//  dword adsp_clk_state; /*Removing the unused variable fixes compiler worning*/
  clkrgm_adsp_speed_type adsp_speed_tbs;

  if(clkrgm.adsp_pending_speed == clkrgm.adsp_speed)
  {
    /* No change */
    return;
  }
 
   /* No Need to Turn Off the ADSP Clock for switching ADSP
      clock as it has the capability to change its own clock
      just like ARM
    */

  /* Set new clock source, divider, and mode */

  /* If the current mode and the mode requested are same, then change the clock directly */
  if(clkrgm_adsp_cfg[clkrgm.adsp_pending_speed].mode == clkrgm_adsp_cfg[clkrgm.adsp_speed].mode)
  {
    clk_regime_switch_adsp_hw(clkrgm.adsp_pending_speed);
  }
  else
  {
    /* If current mode and requested mode are not same then switch in steps */
    /* 1. First switch to 64MHz */
    adsp_speed_tbs = clkrgm.adsp_pending_speed;
    clkrgm.adsp_pending_speed = CLKRGM_ADSP_64P0MHZ_SPEED;
    clk_regime_switch_adsp_hw(clkrgm.adsp_pending_speed);
    clkrgm.adsp_pending_speed = adsp_speed_tbs;
    /* 2. Switch the Mode Now */
    HWIO_OUTF( ADSP_MODE_REG, ADSPCLK_1X_MODE, clkrgm_adsp_cfg[clkrgm.adsp_pending_speed].mode);
    /* 3. Switch to the required frequency */
    clk_regime_switch_adsp_hw(clkrgm.adsp_pending_speed);
  }
  
  MSG_HIGH("ADSP Clock being switched to %d ", clkrgm.adsp_pending_speed, 0, 0);
}


/*===========================================================================

FUNCTION CLK_REGIME_COMPLETE_QDSP_CLK_SWITCH

DESCRIPTION
  Completes the process of switching adsp clock.This function is provided to
  complete any operations by the ARM after switching the ADSP clock.

DEPENDENCIES
  clk_regime_switch_adsp_clk() should have been called prior to switch the
  ADSP clock to the desired frequency prior to calling this function .

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void clk_regime_complete_qdsp_clk_switch( void )
{

  /* Finish clock switch by turning off the previous clock source */

  if(clkrgm.adsp_pending_speed == clkrgm.adsp_speed)
  {
    /* No change */
    return;
  }

  if(clkrgm_adsp_cfg[clkrgm.adsp_pending_speed].src !=
     clkrgm_adsp_cfg[clkrgm.adsp_speed].src)
  {
    switch(clkrgm_adsp_cfg[clkrgm.adsp_speed].src)
    {
      case CLKRGM_TYPE1_SRC_IS_PLL:
        clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_ADSP_CLIENT,
                                CLKRGM_PLL_DISABLE);
        break;

      default:
        break;
    }
  }
  clkrgm.adsp_speed = clkrgm.adsp_pending_speed;
}


/*=========================================================================
FUNCTION CLK_REGIME_GET_MAX_ADSP_CLK_KHZ

DESCRIPTION
  This function returns the maximum clock frequency in Khz for the ADSP

DEPENDENCIES
  None.

RETURN VALUE
  The maximum clk frequency in KHZ for the ADSP

SIDE EFFECTS
  None.
=============================================================================*/
uint32 clk_regime_get_max_adsp_clk_khz( void )
{
   return CLKRGM_ADSP_MAX_SPEED_KHZ;
}

/*=========================================================================
FUNCTION CLK_REGIME_SET_SDCC_CLOCK

DESCRIPTION
  This function sets the SDCC MCLK, for card identification.

DEPENDENCIES
  Do not call this function while in the middle of an SD card transaction.

RETURN VALUE
  None.

SIDE EFFECTS
  If the SDCC MCLK is running when this function is called, it is halted
  for a brief period while setting the divider.
=============================================================================*/

void clk_regime_set_sdcc_clk_frequency ( clkrgm_sdcc_clk_type sdcc_freq)
{
  int sdcc_clock_on_before = clk_regime_msm_is_on(CLK_RGM_SDCC_MCLK_M);

  if (sdcc_clock_on_before == TRUE)
  {
    clk_regime_msm_off(CLK_RGM_SDCC_MCLK_M);
  }

  switch(sdcc_freq)
  {
  case CLKRGM_SDCC_SLOW:
    CLKRGM_W( SDCC_MN_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_SLOW ].mn_reg );
    CLKRGM_W( SDCC_CD_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_SLOW ].cd_reg );
    clkrgm.sdcc = clkrgm_sdcc_cfg[ CLKRGM_SDCC_SLOW ];
    break;

  case CLKRGM_SDCC_FAST:
    CLKRGM_W( SDCC_MN_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_FAST ].mn_reg );
    CLKRGM_W( SDCC_CD_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_FAST ].cd_reg );
    clkrgm.sdcc = clkrgm_sdcc_cfg[ CLKRGM_SDCC_FAST ];
    break;

  case CLKRGM_SDCC_9P6MHZ:
    CLKRGM_W( SDCC_MN_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_9P6MHZ ].mn_reg );
    CLKRGM_W( SDCC_CD_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_9P6MHZ ].cd_reg );
    clkrgm.sdcc = clkrgm_sdcc_cfg[ CLKRGM_SDCC_9P6MHZ ];
    break;

  case CLKRGM_SDCC_4P8MHZ:
    CLKRGM_W( SDCC_MN_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_4P8MHZ ].mn_reg );
    CLKRGM_W( SDCC_CD_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_4P8MHZ ].cd_reg );
    clkrgm.sdcc = clkrgm_sdcc_cfg[ CLKRGM_SDCC_4P8MHZ ];
    break;

  case CLKRGM_SDCC_400KHZ:
    CLKRGM_W( SDCC_MN_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_400KHZ ].mn_reg );
    CLKRGM_W( SDCC_CD_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_400KHZ ].cd_reg );
    clkrgm.sdcc = clkrgm_sdcc_cfg[ CLKRGM_SDCC_400KHZ ];
    break;

  case CLKRGM_SDCC_19P2MHZ:
    CLKRGM_W( SDCC_MN_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_19P2MHZ ].mn_reg );
    CLKRGM_W( SDCC_CD_REG, clkrgm_sdcc_cfg[ CLKRGM_SDCC_19P2MHZ ].cd_reg );
    clkrgm.sdcc = clkrgm_sdcc_cfg[ CLKRGM_SDCC_19P2MHZ ];
    break;

  default:
    break;
  }

  if (sdcc_clock_on_before == TRUE)
  {
    clk_regime_msm_on(CLK_RGM_SDCC_MCLK_M);
  }
} /* clk_regime_set_sdcc_clock */
/*=========================================================================
FUNCTION CLK_REGIME_SET_SDCC_SLOW_CLOCK

DESCRIPTION
  This function sets the SDCC MCLK to 400kHz, for card identification.

DEPENDENCIES
  Do not call this function while in the middle of an SD card transaction.

RETURN VALUE
  None.

SIDE EFFECTS
  If the SDCC MCLK is running when this function is called, it is halted
  for a brief period while setting the divider.
=============================================================================*/

void clk_regime_set_sdcc_slow_clock ( void )
{
  clk_regime_set_sdcc_clk_frequency ( CLKRGM_SDCC_SLOW );
} /* clk_regime_set_sdcc_slow_clock */

/*=========================================================================
FUNCTION CLK_REGIME_SET_SDCC_FAST_CLOCK

DESCRIPTION
  This function sets the SDCC MCLK to TCXO.

DEPENDENCIES
  Do not call this function while in the middle of an SD card transaction.

RETURN VALUE
  None.

SIDE EFFECTS
  If the SDCC MCLK is running when this function is called, it is halted
  for a brief period while setting the divider.
=============================================================================*/

void clk_regime_set_sdcc_fast_clock ( void )
{
  clk_regime_set_sdcc_clk_frequency ( CLKRGM_SDCC_FAST );
} /* clk_regime_set_sdcc_slow_clock */

/*=========================================================================
FUNCTION CLK_REGIME_GET_I2C_FREQUENCY_KHZ

DESCRIPTION
  This function returns the clock frequency in Khz for the i2c
DEPENDENCIES
  Assumes that clkrgm.i2c_freq_khz is updated whenever either mclk is
  changed or the I2C divder is changed.

RETURN VALUE
  The clk frequency in KHZ for the i2c

SIDE EFFECTS
  None.
=============================================================================*/
uint32 clk_regime_get_i2c_clk_khz (void)
{
   uint32 f=1; /* avoid div by 0 in i2c driver */
   clkrgm_msm_type1_src_type s;
   uint32 cd_reg = 0;

   switch(CLK_RGM_I2C_M)
   {
     case CLK_RGM_I2C0_M:
       cd_reg = HWIO_IN(GSBI0_CD_REG);
       s = (cd_reg & HWIO_FMSK(GSBI0_CD_REG, SRC_SEL)) >> HWIO_SHFT(GSBI0_CD_REG, SRC_SEL);
       break;
     case CLK_RGM_I2C1_M:
       cd_reg = HWIO_IN(GSBI1_CD_REG);
       s = (cd_reg & HWIO_FMSK(GSBI1_CD_REG, SRC_SEL)) >> HWIO_SHFT(GSBI1_CD_REG, SRC_SEL);
       break;
     case CLK_RGM_I2C2_M:
       cd_reg = HWIO_IN(GSBI2_CD_REG);
       s = (cd_reg & HWIO_FMSK(GSBI2_CD_REG, SRC_SEL)) >> HWIO_SHFT(GSBI2_CD_REG, SRC_SEL);
       break;
     case CLK_RGM_I2C_LEGACY_M:
       s = HWIO_INF(MISC_CFG_REG, I2C_SRC_SEL);
       break;
   }

   /* The extra I2C port uses different sources than the
      typical GSBI type 1 sources */
   if(CLK_RGM_I2C_M == CLK_RGM_I2C_LEGACY_M)
   {
     switch (s)
     {
        case HWIO_FVAL(MISC_CFG_REG, I2C_SRC_SEL, XO):
          f=CLKRGM_I2C_TCXO_FREQ_KHZ;
          break;

        case HWIO_FVAL(MISC_CFG_REG, I2C_SRC_SEL, SLEEP_CLK):
          f=CLKRGM_I2C_SLPOSC_FREQ_KHZ;
          break;

        case HWIO_FVAL(MISC_CFG_REG, I2C_SRC_SEL, XO4):
          f=CLKRGM_I2C_TCXO4_KHZ;
          break;

        case HWIO_FVAL(MISC_CFG_REG, I2C_SRC_SEL, EXT_CLK):
          break;

        default:
          break;
     }
   } else {
     switch (s)
     {
        case HWIO_FVAL(GSBI0_CD_REG, SRC_SEL, XO):
          f=CLKRGM_I2C_TCXO_FREQ_KHZ;

          /* Also might be XO/4.  Check for MND = 1/4 */
          if(~(cd_reg & HWIO_FMSK(GSBI1_CD_REG, D_VAL)) == 4) {
            f=CLKRGM_I2C_TCXO4_KHZ;
          }
          break;

        case HWIO_FVAL(GSBI0_CD_REG, SRC_SEL, SLEEP_CLK):
          f=CLKRGM_I2C_SLPOSC_FREQ_KHZ;
          break;

        case HWIO_FVAL(GSBI0_CD_REG, SRC_SEL, PLL_SRC):
          /* No I2C configs use PLL at this time. */
          break;
        case HWIO_FVAL(GSBI0_CD_REG, SRC_SEL, EXT_CLK):
          break;
        default:
          break;
     }
   }
   return f;
} /* clk_regime_get_i2c_clk_khz */

/*===========================================================================

FUNCTION    CLK_REGIME_MSM_SEL_CAMCLK

DESCRIPTION
  Selects a clock source for the camera.

PARAMETERS
  new_clk - desired camera clock frequency in hertz

DEPENDENCIES
  The camclk regime should be disabled.

RETURN VALUE
  None.

SIDE EFFECTS
   Camclk frequency will be changed.

===========================================================================*/
void clk_regime_msm_sel_camclk
(
  uint32 new_freq /* Desired clock input to the camera */
)
{

  boolean is_on;
  clkrgm_camclk_type new_clk;
  uint32 mn_value;
  uint32 cd_value;
  uint32 c_value;
  uint16 d_value;
  /* ------------------------------------------------------------------------
     If the clock regime is enabled, then disable the clock regime before
     changing the clock regime configuration.
  ------------------------------------------------------------------------ */

  /* Determine if the clock regime is currently on. */

  is_on = clk_regime_msm_is_on( CLK_RGM_CAMCLK_M );

  /* If the clock regime is on, then turn it off. */

  if ( is_on == TRUE ) clk_regime_msm_off( CLK_RGM_CAMCLK_M );

  /* ------------------------------------------------------------------------
   If we have a configuration structure we'll use it.  Otherwise use the
   misc structure
  ------------------------------------------------------------------------ */

  switch (new_freq)
  {
  case 6144000: /* 6.144MHz */
    new_clk = CLKRGM_CAMCLK_6P144_MHZ;
    break;
  case 12000000: /* 12.000MHz  Jitter Free */
    new_clk = CLKRGM_CAMCLK_12P0_MHZ;
    break;
  case 12288000: /* 12.288MHz */
    new_clk = CLKRGM_CAMCLK_12P288_MHZ;
    break;
  case 19200000: /* XO */
    new_clk = CLKRGM_CAMCLK_TCXO;
    break;
  case 24576000: /* 24.576MHz */
    new_clk = CLKRGM_CAMCLK_24P576_MHZ;
    break;
  case 24000000: /* 24.0MHz Jitter Free */
    new_clk = CLKRGM_CAMCLK_24P0_MHZ;
    break;
  case 48000000: /* 48.0MHz Jitter Free */
    new_clk = CLKRGM_CAMCLK_48P0_MHZ;
    break;
  case 49152000: /* 49.152MHz */
    new_clk = CLKRGM_CAMCLK_49P152_MHZ;
    break;
  default: /* We don't have a configuration for requested freq */
    new_clk = CLKRGM_CAMCLK_MISC;
    break;
  }


  /* ------------------------------------------------------------------------
     If the new configuration requires a different source than the old
     configuration, and if source required by the new configuration is a PLL,
     then request that the PLL required by the new configuration be enabled
     prior to switching to the new configuration.
  ------------------------------------------------------------------------ */

  if ( clkrgm_camclk_cfg[ new_clk ].src != clkrgm.camclk.src )
  {
    if ( clkrgm_camclk_cfg[ new_clk ].src == CLKRGM_TYPE1_SRC_IS_PLL )
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_CAMCLK_CLIENT, CLKRGM_PLL_ENABLE );
    }
  }

  /* ------------------------------------------------------------------------
     Program the camera clock regime MND counter values.
  ------------------------------------------------------------------------ */

  if (new_clk == CLKRGM_CAMCLK_MISC)
  {
    clk_regime_calculate_mnd(CLKRGM_PLL_FREQ_HZ, new_freq, 0xffff, &mn_value, &d_value);


    c_value = 
      CR_FVAL(CAMCLK_CD_REG, CAMCLK_PO_SRC_SEL, PLL_SRC) |
      CR_FVAL(CAMCLK_CD_REG, DIV_MND_EN, ENABLED) |
      CR_FVAL(CAMCLK_CD_REG, DIV_MND_MODE, DUAL_EDGE_MODE) |
      CR_FVAL(CAMCLK_CD_REG, CAM_PCLK_SRC_SEL, TLMM_CAM_PCLK);

    cd_value = c_value | d_value;

  } else {
    mn_value = clkrgm_camclk_cfg[ new_clk ].mn_reg;
    cd_value = clkrgm_camclk_cfg[ new_clk ].cd_reg;
  }

  HWIO_OUT( CAMCLK_MN_REG, mn_value );
  HWIO_OUT( CAMCLK_CD_REG, cd_value );
  clkrgm.camclk.mn_reg = mn_value;
  clkrgm.camclk.cd_reg = cd_value;

  /* ------------------------------------------------------------------------
     If the new configuration requires a different source than the old
     configuration, and if source required by the old configuration is a PLL,
     then request that the PLL required by the old configuration be disabled
     after switching to the new configuration.
  ------------------------------------------------------------------------ */

  if ( clkrgm_camclk_cfg[ new_clk ].src != clkrgm.camclk.src )
  {
    if ( clkrgm.camclk.src == CLKRGM_TYPE1_SRC_IS_PLL )
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_CAMCLK_CLIENT, CLKRGM_PLL_DISABLE );
    }
  }

  /* ------------------------------------------------------------------------
     If the clock regime was enabled upon entry, then re-enable the clock
     regime before exit.
  ------------------------------------------------------------------------ */

  if ( is_on == TRUE ) clk_regime_msm_on( CLK_RGM_CAMCLK_M );
  
  clkrgm.camclk.src = clkrgm_camclk_cfg[ new_clk ].src;
}

/*===========================================================================

FUNCTION    CLK_REGIME_MSM_SEL_UART_DM_CLK

DESCRIPTION
  Selects a clock source for the GSBI1.

PARAMETERS
  new_clk - desired GSBI1 uart clock frequency in hertz

DEPENDENCIES
  The GSBI1 regime should be disabled.

RETURN VALUE
  None.

SIDE EFFECTS
   GSBI1 frequency will be changed.

===========================================================================*/
void clk_regime_msm_sel_uart_dm_clk
(
  uint32 new_freq /* Desired clock input to the uart_dm */
)
{
  /* These variables allow this function to be somewhat flexible between the
     4 possible UARTS. */
  const clkrgm_msm_clk_type regime = CLK_RGM_UART1_M;
  const clkrgm_msm_type1_cfg_type *uart_clk_table = clkrgm_gsbi_1_2_uart_cfg;
  clkrgm_msm_type1_cfg_type *clkrgm_uart = &clkrgm.gsbi[1];
  const uint16 mnd_mask = 0xff;
  const uint32 *uart_mn_reg = (uint32 *)HWIO_ADDR(GSBI1_MN_REG);
  const uint32 *uart_cd_reg = (uint32 *)HWIO_ADDR(GSBI1_CD_REG);
  const uint16 c_value =
    CR_FVAL(GSBI1_CD_REG, SRC_SEL, PLL_SRC) |
    CR_FVAL(GSBI1_CD_REG, DIV_MND_EN, ENABLED) |
    CR_FVAL(GSBI1_CD_REG, DIV_MND_MODE, DUAL_EDGE_MODE);

  boolean is_on;
  clkrgm_uart_dm_clk_type new_clk;
  uint32 cd_value;
  uint32 mn_value;
  uint16 d_value;

  /* ------------------------------------------------------------------------
     If the clock regime is enabled, then disable the clock regime before
     changing the clock regime configuration.
  ------------------------------------------------------------------------ */
  /* Determine if the clock regime is currently on. */

  is_on = clk_regime_msm_is_on( regime );

  /* If the clock regime is on, then turn it off. */

  if ( is_on == TRUE ) clk_regime_msm_off( regime );


  /* ------------------------------------------------------------------------
   If we have a configuration structure we'll use it.  Otherwise use the
   misc structure
  ------------------------------------------------------------------------ */

  switch (new_freq)
  {
    case CLKRGM_UARTDM_SPEED_7_3728_MHZ:
      new_clk = CLKRGM_UART_DM_7P3728_MHZ;    // 7372800
      break;
    case CLKRGM_UARTDM_SPEED_14_7456_MHZ:
      new_clk = CLKRGM_UART_DM_14P7456_MHZ;   // 14745600,
      break;
    case CLKRGM_UARTDM_SPEED_51_2_MHZ:
      new_clk = CLKRGM_UART_DM_51P20_MHZ;     // 51200000,
      break;
    case CLKRGM_UARTDM_SPEED_58_9824_MHZ:
      new_clk = CLKRGM_UART_DM_58P9824_MHZ;   // 58982400,
      break;
    case CLKRGM_UARTDM_SPEED_64_MHZ:         /* 64 MHz */
      new_clk = CLKRGM_UART_DM_64P0_MHZ;
      break;
    case CLKRGM_UARTDM_SPEED_192_MHZ:        /* 192.0MHz */
      new_clk = CLKRGM_UART_DM_192P0_MHZ;
      break;
    default:
      new_clk = CLKRGM_UART_DM_MISC;
      break;
  }


  /* ------------------------------------------------------------------------
     If the new configuration requires a different source than the old
     configuration, and if source required by the new configuration is a PLL,
     then request that the PLL required by the new configuration be enabled
     prior to switching to the new configuration.
  ------------------------------------------------------------------------ */

  if ( uart_clk_table[ new_clk ].src != clkrgm_uart->src )
  {
    if ( uart_clk_table[ new_clk ].src == CLKRGM_TYPE1_SRC_IS_PLL )
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_UART_DM_CLIENT,
                              CLKRGM_PLL_ENABLE );
    }
  }

  /* ------------------------------------------------------------------------
     Program the camera clock regime MND counter values.
  ------------------------------------------------------------------------ */

  if (new_clk == CLKRGM_UART_DM_MISC)
  {
    clk_regime_calculate_mnd(CLKRGM_PLL_FREQ_HZ, new_freq, mnd_mask, &mn_value, &d_value);
    cd_value = c_value | d_value;
  } else {
    mn_value = uart_clk_table[ new_clk ].mn_reg;
    cd_value = uart_clk_table[ new_clk ].cd_reg;
  }

  outpdw(uart_mn_reg, mn_value );
  outpdw(uart_cd_reg, cd_value );
  clkrgm_uart->cd_reg = cd_value;
  clkrgm_uart->mn_reg = mn_value;



  /* ------------------------------------------------------------------------
     If the new configuration requires a different source than the old
     configuration, and if source required by the old configuration is a PLL,
     then request that the PLL required by the old configuration be disabled
     after switching to the new configuration.
  ------------------------------------------------------------------------ */

  if ( uart_clk_table[ new_clk ].src != clkrgm_uart->src )
  {
    if ( clkrgm_uart->src == CLKRGM_TYPE1_SRC_IS_PLL )
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_UART_DM_CLIENT,
                              CLKRGM_PLL_DISABLE );
    }
  }

  /* ------------------------------------------------------------------------
     Update clock regime driver state information.
  ------------------------------------------------------------------------ */

  clkrgm_uart->src = uart_clk_table[ new_clk ].src;

  /* ------------------------------------------------------------------------
     If the clock regime was enabled upon entry, then re-enable the clock
     regime before exit.
  ------------------------------------------------------------------------ */

  if ( is_on == TRUE ) clk_regime_msm_on( regime );
}

/*===========================================================================

FUNCTION clk_regime_calculate_mnd

DESCRIPTION
  Calculates the m,n,d values to generate a desired frequency.

PARAMETERS
  source_freq, the frequency of the source in Hz
  target_freq, the frequency we're trying to generate in Hz
  field_mask,  a mask for the maximum M,N,D value (usually 0xff, or 0xffff)
  md_value, the 32 bit value of the mnd register will be set here
  n_value,  the 16 bit value of the  n part of the register will be set here


DEPENDENCIES
  None.

RETURN VALUE
  false if the calculation could not succeed, true otherwise

SIDE EFFECTS
  md_value and n_value will be modified to the values to program into the MD register, and N
  half of the NS register, respectively

===========================================================================*/
boolean clk_regime_calculate_mnd(
  uint32 freq_in, /* frequency of clock source (usually pll speed) */
  uint32 freq_out, /* frequency we're trying to generate */
  uint32 field_mask,   /* maximum value for n (usually 255 or 65535 */
  uint32 *mn_value,    /* value of md register */
  uint16 *d_value)     /* n register value */
{

  /* the code below was ported from 6550 camsensor */
   uint32 Mval,Nval,Nreg,Mreg,Dreg;
   uint32 GoalRatio, OutFreq;
   uint32 Nmax = field_mask;
   uint32 Mlimit = field_mask;

   if( (freq_in / freq_out) > Nmax )
   {
     return FALSE;
   }

   /* Expand GoalRatio resolution by 1 digit for integer math */
   GoalRatio = ( (freq_in * 10)  / freq_out );
   /* Round up by 0.5 before scaling back to normal */
   GoalRatio = (GoalRatio + 5) / 10;


   Mval = 1;
   Nval = GoalRatio;
   OutFreq = freq_out/GoalRatio;
   /* Now find largest useable values for best duty cycle control */
   while( (Mval < Mlimit) && (Nval < Nmax) )
   {
     Mval <<= 1;
     Nval <<= 1;
   }
   /* Prepare the data for return to caller */
   Mval >>= 1;
   Nval >>= 1;

   Mreg = Mval & field_mask;             /* trimmed to bit width */
   Nreg = (~(Nval - Mval)& field_mask);  /* ~(N-M) trimmed to bit width */
   Dreg = (~(Nval)) & field_mask;        /* Dreg is ~(2*(N/2)) */

   if(field_mask == 0xff) {
     *mn_value = (Mreg << 8) | Nreg;
   } else {
     *mn_value = (Mreg << 16) | Nreg;
   }
   *d_value = (uint16)Dreg;

   return TRUE;
}

#ifndef BUILD_BOOT_CHAIN_OEMSBL
/*===========================================================================

FUNCTION clk_regime_set_core_voltage

DESCRIPTION
  Changes the processor voltage. 

  For SC2X,
  If lower voltage requested, only lower voltage if modem (1X, GPS, HDR) is 
  inactive and USB is not active. 
  When lowering the voltage, slam modem clocks off on because modem is
  confirmed to be inactive by DCVS registrations. Restore save modem clocks
  when raising voltage.

PARAMETERS
  oldmv, the current voltage in millivolts !! IGNORED in this version !! -erwinv
  newmv, the desired voltage in millivolts


DEPENDENCIES
  None.

RETURN VALUE
  NOne

SIDE EFFECTS
  The processor voltage is changed.

===========================================================================*/
static void clk_regime_set_core_voltage_qsc1100
(
  uint16 newmv
)
{
  uint32 sbi_clk_halt = 
    HWIO_INM(CLK_OFF_B, HWIO_FMSK(CLK_OFF_B, SSBI_PMIC_CLK));

  if (!clk_regime_do_voltage_switch)
    return;

#ifdef FEATURE_CLKREGIM_DYNAMIC_VOLTAGE
  if (newmv != clkrgm.cpu_mv)
  {
#endif
    if (sbi_clk_halt)
    {
      /* clock is halted, so enable it */
      CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_CLK_EN, ENABLED );
    }

    if(pm_vreg_set_level(PM_VREG_MSMC1_ID, newmv) != PM_ERR_FLAG__SUCCESS)
    {
      MSG_ERROR("Fail to set CPU voltage",0,0,0);
      return;
    }
    else
    {
      clk_busy_wait(CLKRGM_VOLTAGE_SWITCHING_DELAY_US);
    }

    if (sbi_clk_halt)
    {
      /* If clock was halted, turn it back off */
      CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_CLK_EN, NOT_ENABLED );
    }

#ifdef FEATURE_CLKREGIM_DYNAMIC_VOLTAGE
    clkrgm.cpu_mv = newmv;
  }
#endif
}
#endif /*BUILD_BOOT_CHAIN_OEMSBL*/

void clk_regime_set_core_voltage
(
  uint16 oldmv, uint16 newmv
)
{
#if !defined(BUILD_BOOT_CHAIN_OEMSBL)
  boolean is_srchnontc;
  boolean is_srchtc;
  boolean is_srchsleep;
  boolean is_srchslotted;
  boolean is_srchaq;
  boolean is_nosrch;

  uint32 sbi_clk_halt_m;
  uint32 sbi_clk_halt_n;
  uint32 sbi_clk_halt;

  uint16  vreg_level=0;
  boolean ok_to_lower=FALSE;

  if (!clk_regime_do_voltage_switch)
    return;
#endif

#ifdef FEATURE_CLKREGIM_DYNAMIC_VOLTAGE
  if (newmv == clkrgm.cpu_mv)
      return;
#endif

#if !defined(BUILD_BOOT_CHAIN_OEMSBL)
  /* ------------------------------------------------------------------------
     If boot client is still active then skip for now.  This is to avoid
     the case where cpu requests arrive before the above NV item has been
     read, which results in an unwanted voltage switch.
  ------------------------------------------------------------------------ */
  if (CLKRGM_CPU_CLIENT_IS_SET(BOOT))
  {
    return;
  }

  sbi_clk_halt_m = HWIO_FMSK(MODEM_CLK_HALT, MODEM_SBI_CLK_HALT);
  sbi_clk_halt_n = ~sbi_clk_halt_m;
  sbi_clk_halt   = HWIO_INM(MODEM_CLK_HALT, sbi_clk_halt_m);

  if (sbi_clk_halt)
  {
     HWIO_OUTM(MODEM_CLK_HALT, sbi_clk_halt_m, sbi_clk_halt_n);
  }
  if (pm_vreg_get_level ( PM_VREG_MSMC1_ID, &vreg_level  ))
  {
     MSG_ERROR("Fail to get CPU voltage",0,0,0);
     return;
  }

  /* Per HW Design team,
   * don't lower voltage if any of the following conditions exist.
   *
   * In a nutshell, 1) modem inactive for all technologies, 2)
   *  no USB.
   *
   */
  is_srchnontc     =CLKRGM_CPU_CLIENT_IS_SET(SRCHNONTC)==0?FALSE:TRUE;
  is_srchtc        =CLKRGM_CPU_CLIENT_IS_SET(SRCHTC)==0?FALSE:TRUE;
  is_srchsleep     =CLKRGM_CPU_CLIENT_IS_SET(SRCHSLEEP)==0?FALSE:TRUE;
  is_srchslotted   =CLKRGM_CPU_CLIENT_IS_SET(SLOTTED_WAKEUP)==0?FALSE:TRUE;
  is_srchaq        =CLKRGM_CPU_CLIENT_IS_SET(SRCHAQ)==0?FALSE:TRUE;
  is_nosrch        = !(is_srchnontc   ||
                       is_srchtc      ||
                       is_srchsleep   ||
                       is_srchslotted ||
                       is_srchaq);

  if ( (is_nosrch || is_srchsleep || !(is_srchnontc || is_srchtc))
      &&
        !(CLKRGM_CPU_CLIENT_IS_SET(GPS_SERVICES))
      &&
        !(CLKRGM_CPU_CLIENT_IS_SET(HDR_WAKEUP))
      &&
        !usb_xtal_on )
  {
    ok_to_lower=TRUE;
  }

  if (ok_to_lower == FALSE)
  {
    /* don't lower voltage if modem is active */
    if (newmv < vreg_level)
    {
      if ((clk_regime_do_modem_clk_halt == 1) && (rs_modem_clk_halta != 0x0))
      { // Restore saved modem clocks here before raising voltage.
        HWIO_OUT(MODEM_CLK_HALT, rs_modem_clk_halta);
        HWIO_OUT(MODEM_CLK_HALT_2, rs_modem_clk_haltb);
        rs_modem_clk_halta = 0x0;
        rs_modem_clk_haltb = 0x0;
      }
      /* if voltage can't be lowered, make sure it is not at low */
      clk_regime_set_core_voltage_qsc1100(MSMC_VOLTAGE_HIGH_V);
      return;
    }
  }

  if (newmv != vreg_level)
  {
    if (clk_regime_do_modem_clk_halt == 1)
    {
      if (newmv < vreg_level)
      { // Turn off modem clocks here before lowering voltage.
        rs_modem_clk_halta = HWIO_IN(MODEM_CLK_HALT);
        rs_modem_clk_haltb = HWIO_IN(MODEM_CLK_HALT_2);
        HWIO_OUTM(MODEM_CLK_HALT, 
              ~(HWIO_FMSK(MODEM_CLK_HALT, FB_ANT0_CLK_HALT) | 
                HWIO_FMSK(MODEM_CLK_HALT, FB_ANT1_CLK_HALT) | 
                HWIO_FMSK(MODEM_CLK_HALT, RXF_SAMP_CLK_HALT)), 
              CLKRGM_LOW_VOLTAGE_MODEM_CLK_HALTA);
        HWIO_OUTM(MODEM_CLK_HALT_2, ~HWIO_FMSK(MODEM_CLK_HALT_2, RXF_SAMP_REF_CLK_HALT), 
              CLKRGM_LOW_VOLTAGE_MODEM_CLK_HALTB);
      }
      else if (rs_modem_clk_halta != 0x0)
      { // Restore saved modem clocks here before raising voltage.
        HWIO_OUT(MODEM_CLK_HALT, rs_modem_clk_halta);
        HWIO_OUT(MODEM_CLK_HALT_2, rs_modem_clk_haltb);
        rs_modem_clk_halta = 0x0;
        rs_modem_clk_haltb = 0x0;
      }
    }
    clk_regime_set_core_voltage_qsc1100(newmv);
  }
  #ifdef FEATURE_CLKREGIM_DYNAMIC_VOLTAGE
  else
  {
    clkrgm.cpu_mv = newmv;
  }
  #endif
#endif
}

#if defined(FEATURE_CLKREGIM_DYNAMIC_VOLTAGE) && !defined(BUILD_BOOT_CHAIN_OEMSBL)

/*===========================================================================

FUNCTION clk_regime_set_voltage_per_cpu_speed

DESCRIPTION
  Changes the processor voltage.

PARAMETERS
  clk_frequency, the desired speed of the processor !! IGNORED in this version --erwinv
  vreg_mv, the desired cpu voltage.

DEPENDENCIES
  None.

RETURN VALUE
  NOne

SIDE EFFECTS
  The processor voltage is changed.  State variable clkrgm.cpu_mv is updated.

===========================================================================*/
void clk_regime_set_voltage_per_cpu_speed
(
  clkrgm_cpu_speed_type clk_frequency,
  uint16 vreg_mv
)
{
 /* I hate to lock interrupts for this, but this is how this mclk frequency
  variable is protected, so I am left with no choice. */

  INTLOCK();
  clk_regime_set_core_voltage(clkrgm.cpu_mv, vreg_mv);
  INTFREE();

} /* clk_regime_set_voltage_per_cpu_speed */


/*===========================================================================

FUNCTION clk_regime_diag_init

DESCRIPTION
  Registers the clock regime diag packet handlers.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  NOne

SIDE EFFECTS
  The diag dispatch table is updated.

===========================================================================*/
void clk_regime_diag_init (void) 
{
  DIAGPKT_DISPATCH_TABLE_REGISTER ((uint16)DIAG_SUBSYS_CLKRGM, clk_regime_diag_tbl);
  return;
}

/*===========================================================================

FUNCTION clk_regime_diag_set_voltage_per_cpu_speed

DESCRIPTION
  Responds to a diag request packet to change cpu voltage

PARAMETERS
  req_pkt, the request packet
  pkt_len, the length of the request packet

DEPENDENCIES
  None.

RETURN VALUE
  A pointer to the response packet.

SIDE EFFECTS
  The processor voltage is changed.

===========================================================================*/
PACKED void * clk_regime_diag_set_voltage_per_cpu_speed
(
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  clkrgm_diag_set_voltage_per_cpu_speed_req_type * req_ptr =
    (clkrgm_diag_set_voltage_per_cpu_speed_req_type *) req_pkt;
  clkrgm_diag_set_voltage_per_cpu_speed_req_type * rsp_ptr;
  const int rsp_len = sizeof (clkrgm_diag_set_voltage_per_cpu_speed_req_type);

 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 clk_regime_set_voltage_per_cpu_speed((clkrgm_cpu_speed_type)req_ptr->cpu_speed, req_ptr->vreg_mv);

 /* Allocate, initialize and return a response packet that is a copy of the
  request packet that was received. */

 rsp_ptr = (clkrgm_diag_set_voltage_per_cpu_speed_req_type *)
   diagpkt_subsys_alloc( (uint16)DIAG_SUBSYS_PM, CLKRGM_SET_VOLTAGE_PER_CPU_SPEED_F, rsp_len );

 if( rsp_ptr != NULL )
 {
   rsp_ptr->cpu_speed = req_ptr->cpu_speed;
   rsp_ptr->vreg_mv = req_ptr->vreg_mv;
 }

 /* Suppress compiler warning. */

 (void) pkt_len;

 return ( rsp_ptr );

}

#endif

/*===========================================================================

FUNCTION CLK_REGIME_SET_MDSP_DME_MODE

DESCRIPTION
  Allows outside users to inform Clock Regime services as to whether DME
  is required or not for RXDSP.

DEPENDENCIES
  Interrupts should be locked by the caller

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void clk_regime_set_mdsp_dme_mode
(
   clkrgm_dme_mode_type dme_mode
)
{
    mdsp_dme_mode  =  dme_mode;
} /* clk_regime_set_mdsp_dme_mode */
/*===========================================================================

FUNCTION CLK_REGIME_SET_SDIO_MODE

DESCRIPTION
  Allows outside users to inform Clock Regime services as to whether SD card
  is active so bus clock will not get switched to low.

DEPENDENCIES
  Interrupts should be locked by the caller

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void clk_regime_set_sdio_mode
(
   boolean sdio_mode
)
{
    sdio_active_mode  =  sdio_mode;
} /* clk_regime_set_sdio_mode */


/*===========================================================================

FUNCTION  clk_regime_msm_get_clk_freq_khz

DESCRIPTION
  Interface to allow queries of the speeds of different clocks

PARAMETERS
   The clock to querry.

DEPENDENCIES
  None.

RETURN VALUE
  the frequency of clk in khz

SIDE EFFECTS
  None.

===========================================================================*/
uint32 clk_regime_msm_get_clk_freq_khz 
(
  clk_regime_type regime
)
{
  uint32 ret_val;

  switch (regime)
  {
    case CLK_RGM_MCLK_M:
      ret_val = clkrgm_cpu_cfg[ clkrgm.cpu_speed ].mclk.khz;
      break;

    case CLK_RGM_HCLK_M:
      ret_val = clkrgm_cpu_cfg[ clkrgm.cpu_speed ].hclk.khz;
      break;

    default:
      ret_val = 0;
      break;
  }

  return ret_val;

} /* clk_regime_msm_get_clk_freq_khz */

#ifndef BUILD_BOOT_CHAIN_OEMSBL

/*===========================================================================

  FUNCTION       CLK_REGIME_PROCESS_CPU_CLIENTS
  
  DESCRIPTION    This function is invoked by a client to inform clock
                 regime it is active.  Depending on the current registered
                 clients, the ARM frequency may be increased to meet the
                 demand for CPU. The ARM clock will be switched to high
                 performance level and remain at such till all the high cpu
                 clients deregister themselves.
  
                 This function, along with
                 clk_regime_deregister_for_cpu_resource()
                 is used to dynamically change the ARM voltage and clock
                 speed based on the applications.
  
  PARAMETERS     client - application needs high cpu resource.
  
  DEPENDENCIES   None
  
  RETURN VALUE   None
  
  SIDE EFFECTS   If the clock speed changes it will also change the AXI
                 bus speed.

===========================================================================*/

static void clk_regime_process_cpu_clients (void)
{
  #if defined(FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED) ||   \
      defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)
  clkrgm_device_param_type  param;

  /* ------------------------------------------------------------------------
     Check if dynamic clock switching has been disabled
  ------------------------------------------------------------------------ */

  if (!clk_regime_dyn_clock_on)
  {
    return;
  }
  #endif

  #ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING

  /* ------------------------------------------------------------------------
     If boot client is still active then skip for now.  This is to avoid
     the case where cpu requests arrive before the above NV item has been
     read, which results in an unwanted clock switch.
  ------------------------------------------------------------------------ */

  if (clkrgm.cpu_client_mask & (BITSHFT64 << CLKRGM_CPU_CLIENT_BOOT))
  {
    return;
  }

  /* test mode to force MED_POWER */
  if (clk_regime_dyn_clock_on == 2)
  {
    param.minimum_speed =
      clkrgm_cpu_cfg[CLKRGM_MCPU_MED_SPEED].mclk.khz;
  }
  else
  {
    /* For now, mdsp speed is always == speed high
    if (clkrgm_modem.mdsp_speed >= CLKRGM_MDSP_SPEED_HIGH ||
               clkrgm.adsp_speed > CLKRGM_ADSP_64P0MHZ_SPEED || 
              (clkrgm.cpu_client_mask & (BITSHFT64 << CLKRGM_CPU_CLIENT_MAX))) 
    */
    /*
    if (clkrgm.adsp_speed > CLKRGM_ADSP_64P0MHZ_SPEED || 
        (clkrgm.cpu_client_mask & (BITSHFT64 << CLKRGM_CPU_CLIENT_MAX)) ) 
    */

    if (clkrgm.cpu_client_mask & (BITSHFT64 << CLKRGM_CPU_CLIENT_MAX))
		param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_HIGH_SPEED].mclk.khz;
//#ifdef CLKRGM_1X_VOICE_DCVS
    /* 1X Voice */
    else if (CLKRGM_CPU_CLIENT_IS_SET(SRCHTC) && 
        ((clkrgm.cpu_client_mask & ~CLKRGM_CPU_CLIENT(SRCHTC))==0))
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_CPU_38P4_DIV1].mclk.khz;
//#endif /*CLKRGM_1X_VOICE_DCVS*/
    /* MP3*/
    else if (clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_MP3_SERVICES) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_MP3_SERVICES) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_MP3_SERVICES) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_MP3_SERVICES) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_MP3_SERVICES) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_MP3_SERVICES) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_MP3_SERVICES) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP)) ||

             clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_MP3_SERVICES) )
    {
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_LOW_SPEED].mclk.khz;
    }

    /* Audio AAC spec says 38.4 MHz  */
    else if (clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_LC_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_LC_APP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_LC_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_LC_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_LC_APP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_LC_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_LC_APP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP)) ||

             clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_LC_APP) )
    {
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_LOW_SPEED].mclk.khz;
    }

    /* Audio AAC spec says 38.4 MHz  */
    else if (clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_SBR_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_SBR_APP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_SBR_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_SBR_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_SBR_APP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_SBR_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC)) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_SBR_APP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP)) ||

             clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_SBR_APP) )
    {
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_LOW_SPEED].mclk.khz;
    }

    /* Audio AAC spec says 38.4 MHz  */
    else if (clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_PS_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_PS_APP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_PS_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_PS_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_PS_APP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_PS_APP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_PS_APP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP)) ||

             clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_CPU_CLIENT_AAC_PS_APP) )
    {
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_LOW_SPEED].mclk.khz;
    }


    /* QCAMCORDER VIEW FINDER, spec says 38.4 Mhz */
    else if (clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP)) ||

             clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_VIEWFINDER) )
    {
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_LOW_SPEED].mclk.khz;
		
    }

    /* QCAMCORDER RECORD, spec says 96 Mhz */
    else if (clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP)) ||

             clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1) )
    {
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_HIGH_SPEED].mclk.khz;
		
    }

    /* QCAMERA REVIEW, spec says 75.3 Mhz */
    else if (clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_REVIEW) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_REVIEW) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_REVIEW) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_REVIEW) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_REVIEW) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_REVIEW) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_REVIEW) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP)) ||

             clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_REVIEW) )
    {
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_HIGH_SPEED].mclk.khz;
		
    }

    /* QCAMERA Snap shot, spec says 75.3 Mhz */
    else if (clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_SNAPSHOT1) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_SNAPSHOT1) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_SNAPSHOT1) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_SNAPSHOT1) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_SNAPSHOT1) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_SNAPSHOT1) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_SNAPSHOT1) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP)) ||

             clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_SNAPSHOT1) )
    {
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_HIGH_SPEED].mclk.khz;
		
    }

    /* QCAMERA VIEW FINDER */
    else if (clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP)) ||

             clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER) )
    {
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_LOW_SPEED].mclk.khz;
		
    }

    /* BT, it can be featurized as only availabe on high tier, hvo */
    else if (clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_BT_SERVICES) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS)) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_BT_SERVICES) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS)) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_BT_SERVICES) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_BT_SERVICES) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC) ) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_BT_SERVICES) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_NON_CDMA_APPS)) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_BT_SERVICES) |
                                         (BITSHFT64 << CLKRGM_SRCHNONTC)) ||

             clkrgm.cpu_client_mask == ( (BITSHFT64 << CLKRGM_BT_SERVICES) |
                                         (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP)) ||

             clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_BT_SERVICES)) 
    {
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_LOW_SPEED].mclk.khz;
    }
    else
        param.minimum_speed =
          clkrgm_cpu_cfg[CLKRGM_MCPU_HIGH_SPEED].mclk.khz;
  }

  /* ------------------------------------------------------------------------
     Send request to device manager
  ------------------------------------------------------------------------ */

  clk_regime_devman_request(
    &clkrgm.devman_client, CLKRGM_DEVICE_MCPU,
    CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);

  #endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */

  #ifdef FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED

  /* ------------------------------------------------------------------------
     Check if we need to go to high MDSP speed
  ------------------------------------------------------------------------ */
  if (clkrgm.cpu_client_mask & CLKRGM_CPU_CLIENT_MASK_HIGH_MDSP_SPEED)
  {
    param.minimum_speed =
      clkrgm_modem_mdsp_cfg[CLKRGM_MDSP_SPEED_HIGH].clk_freq_khz;
  }
  else
  {
    param.minimum_speed =
      clkrgm_modem_mdsp_cfg[CLKRGM_MDSP_SPEED_MID].clk_freq_khz;
  }

  /* ------------------------------------------------------------------------
     Set new minimum speed
  ------------------------------------------------------------------------ */

  clk_regime_devman_request(
    &clkrgm.devman_client, CLKRGM_DEVICE_MDSP,
    CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);

  #endif /* FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED */

  #if defined (FEATURE_CLKREGIM_DYNAMIC_VOLTAGE)
  if (clk_regime_dyn_voltage_on)
    clk_regime_set_core_voltage(0, clkrgm_cpu_cfg[clkrgm.cpu_speed].pm_vreg_mv);
  #endif

} /* clk_regime_process_cpu_clients */

/*===========================================================================

  FUNCTION       CLK_REGIME_PROCESS_SINGLE_CPU_CLIENTS
  
  DESCRIPTION    Set clock frequency plan when only a single DCVS client 
                 has called CLKREGIM or DEVMAN for frequency change.
  
  PARAMETERS     client - application requesting clock setting change.
  
  DEPENDENCIES   None
  
  RETURN VALUE   None
  
  SIDE EFFECTS   If the clock speed changes it will also change the hclk
                 bus speed.

===========================================================================*/

static void clk_regime_process_single_cpu_client ( clkrgm_cpu_resource_client_type client )
{
  #if defined(FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED) ||   \
      defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)
  clkrgm_device_param_type  param;
  clk_regime_freq_plan_type * freq_plan=NULL;

  /* ------------------------------------------------------------------------
     Check if dynamic clock switching has been disabled
  ------------------------------------------------------------------------ */

  if (!clk_regime_dyn_clock_on)
  {
    return;
  }
  #endif

  #ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING

  /* ------------------------------------------------------------------------
     If boot client is still active then skip for now.  This is to avoid
     the case where cpu requests arrive before the above NV item has been
     read, which results in an unwanted clock switch.
  ------------------------------------------------------------------------ */

  if (clkrgm.cpu_client_mask & (BITSHFT64 << CLKRGM_CPU_CLIENT_BOOT))
  {
    return;
  }

  if (client > CLKRGM_CPU_CLIENT_MAX || !(clkrgm.cpu_client_mask & CLKRGM_FREQ_PLAN_CLIENTS))
  {
    clk_regime_process_cpu_clients();
    return;
  }

  freq_plan = &(clk_regime_freq_plans[client]);
 
  ASSERT( freq_plan->cpu_speed < CLKRGM_CPU_NUM_SPEEDS );

  /* setting mclk/hclk based on minimum required hclk */
  param.minimum_speed =
    clkrgm_cpu_cfg[freq_plan->cpu_speed].mclk.khz;

  clk_regime_devman_request(
    &clkrgm.devman_client, CLKRGM_DEVICE_MCPU,
    CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);

  /* set adsp */
  param.minimum_speed =
    clkrgm_adsp_cfg[freq_plan->adsp_speed].clk_freq_khz;

  clk_regime_devman_request(
    &clkrgm.devman_client, CLKRGM_DEVICE_ADSP,
    CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);

  #endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */

  #ifdef FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED
  /* set mdsp */
  param.minimum_speed =
    clkrgm_modem_mdsp_cfg[freq_plan->mdsp_speed].clk_freq_khz;

  clk_regime_devman_request(
    &clkrgm.devman_client, CLKRGM_DEVICE_MDSP,
    CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);

  #endif /* FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED */
  
  #if defined (FEATURE_CLKREGIM_DYNAMIC_VOLTAGE)
  if (clk_regime_dyn_voltage_on)
    clk_regime_set_core_voltage(0, clkrgm_cpu_cfg[freq_plan->cpu_speed].pm_vreg_mv);
  #endif

} /* clk_regime_process_single_cpu_client */

/*===========================================================================

  FUNCTION       CLK_REGIME_REGISTER_FOR_CPU_RESOURCE
  
  DESCRIPTION    This function is invoked by a client to inform clock
                 regime it is active.  Depending on the current registered
                 clients, the ARM frequency may be increased to meet the
                 demand for CPU. The ARM clock will be switched to high
                 performance level and remain at such till all the high cpu
                 clients deregister themselves.
  
                 This function, along with
                 clk_regime_deregister_for_cpu_resource()
                 is used to dynamically change the ARM voltage and clock
                 speed based on the applications.
  
  PARAMETERS     client - application needs high cpu resource.
  
  DEPENDENCIES   None
  
  RETURN VALUE   None
  
  SIDE EFFECTS   If the clock speed changes it will also change the AXI
                 bus speed.

===========================================================================*/

#if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)
/* SRCHSLEEP and SRCHTC are introduced so that CLKREGIM may know when MODEM is 
 * inactive so that CLKREGIM can scale voltage down.
 * When none of SRCHAQ, SRCHNONTC, SRCHTC are registered, then it is safe
 * for CLKREGIM to lower MSMC voltage.
 */
boolean clk_regime_single_client(uint64 client)
{
  if ( 
      (clkrgm.cpu_client_usage==1 && (clkrgm.cpu_client_mask & CLKRGM_FREQ_PLAN_CLIENTS))

      ||

      (clkrgm.cpu_client_usage==2 && client == CLKRGM_CPU_CLIENT_SRCHTC &&  // 1X Voice
            CLKRGM_CPU_CLIENT_IS_SET(SRCHNONTC))
      ||

      /* If one of modem apps is registering and no SRCH client is registered,
       * then we're on the traffic channel for the other modem app, so can
       * program that other modem app dcvs client clock plan.
      */
     (!(clkrgm.cpu_client_mask & ((BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHSLEEP) |
                                  (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHAQ) |
                                  (BITSHFT64 << CLKRGM_CPU_CLIENT_SRCHNONTC) |
                                  (BITSHFT64 << CLKRGM_CPU_CLIENT_SLOTTED_WAKEUP))) && 
      (client == CLKRGM_CPU_CLIENT_CDMA_DATA_APPS ||
       client == CLKRGM_CPU_CLIENT_BT_VOICE ) )

     ||

     ( (client == CLKRGM_CPU_CLIENT_SRCHSLEEP  ||
        client == CLKRGM_CPU_CLIENT_SRCHAQ     ||
        client ==  CLKRGM_CPU_CLIENT_SRCHNONTC ||
        client == CLKRGM_CPU_CLIENT_SLOTTED_WAKEUP) 
       && 
      ( (clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_CPU_CLIENT_CDMA_DATA_APPS)) ||
        (clkrgm.cpu_client_mask == (BITSHFT64 << CLKRGM_CPU_CLIENT_BT_VOICE)) ) )
     )
      return TRUE;
  return FALSE;
}


void clk_regime_register_for_cpu_resource
(
  clkrgm_cpu_resource_client_type client 
)
{
  boolean single_client=FALSE;

  if (!clk_regime_dyn_clock_on && client != CLKRGM_CPU_CLIENT_BOOT)
  {
    clk_regime_dcvs_log(CLKRGM_DCVS_LOG_REGISTER,
                       CLKRGM_DCVS_LOG_TRANS_REGISTER_BYPASS);
    return;
  }
  /* ------------------------------------------------------------------------
     Validate the client
  ------------------------------------------------------------------------ */

  if (client >= CLKRGM_CPU_RESOURCE_NUM_CLIENTS)
  {
    MSG_ERROR("Invalid CPU client: %d", client, 0, 0);
    return;
  }

  /* ------------------------------------------------------------------------
     Update the client mask
  ------------------------------------------------------------------------ */

  INTLOCK();

#if 0
  // Remove if USB deregister it
  if( clkrgm.cpu_client_mask && (BITSHFT64 << CLKRGM_CPU_CLIENT_HS_USB) )
  {
	 // Degregister USB also go thru all the switching as register, mask it off is better for hacking
     clkrgm.cpu_client_mask &= ~(BITSHFT64 << CLKRGM_CPU_CLIENT_HS_USB);
  }
#endif 
  if (!(clkrgm.cpu_client_mask & (BITSHFT64 << client)))
      clkrgm.cpu_client_usage++;

  clkrgm.cpu_client_mask |= (BITSHFT64 << client);

  single_client = clk_regime_single_client(client);

  INTFREE();

  CLKRGM_MSM_MSG("CPU client %d registered", client, 0, 0, 0); 

  /* ------------------------------------------------------------------------
     Process new client mask
  ------------------------------------------------------------------------ */

  if (single_client)
    clk_regime_process_single_cpu_client(client);
  else 
    clk_regime_process_cpu_clients();

  clk_regime_dcvs_log(CLKRGM_DCVS_LOG_REGISTER,
                     CLKRGM_DCVS_LOG_TRANS_REGISTER);

} /* clk_regime_register_for_cpu_resource */


/* The default and 1x Voice are the same clock frequency settings.
*/
void clk_regime_default_clock_plan(void)
{
  #if defined(FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED) ||   \
      defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)
  clkrgm_device_param_type  param;
  clk_regime_freq_plan_type  freq_plan=
  {
      CLKRGM_CPU_38P4_DIV1, CLKRGM_CPU_38P4_DIV1_HCLK_DIV, 
      CLKRGM_ADSP_38P4MHZ_SPEED, CLKRGM_MDSP_64MHZ_SPEED 
  };
  /* ------------------------------------------------------------------------
     Check if dynamic clock switching has been disabled
  ------------------------------------------------------------------------ */

  if (!clk_regime_dyn_clock_on)
  {
    return;
  }
  #endif

  #ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING

  /* setting mclk/hclk based on minimum required hclk */
  param.minimum_speed =
    clkrgm_cpu_cfg[freq_plan.cpu_speed].mclk.khz;

  clk_regime_devman_request(
    &clkrgm.devman_client, CLKRGM_DEVICE_MCPU,
    CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);

  #endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */

  #ifdef FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED
  /* set mdsp */
  param.minimum_speed =
    clkrgm_modem_mdsp_cfg[freq_plan.mdsp_speed].clk_freq_khz;

  clk_regime_devman_request(
    &clkrgm.devman_client, CLKRGM_DEVICE_MDSP,
    CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);

  #endif /* FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED */

  #if defined (FEATURE_CLKREGIM_DYNAMIC_VOLTAGE)
  if (clk_regime_dyn_voltage_on)
    clk_regime_set_core_voltage(0, clkrgm_cpu_cfg[freq_plan.cpu_speed].pm_vreg_mv);
  #endif

}
/*===========================================================================

  FUNCTION       CLK_REGIME_DEREGISTER_FOR_CPU_RESOURCE
  
  DESCRIPTION    This function is invoked by a client to inform clock
                 regime it is no longer active.  Depending on the current
                 registered clients, the ARM frequency may be decreased.
  
                 This function, along with
                 clk_regime_register_for_cpu_resource()
                 is used to dynamically change the ARM voltage and clock
                 speed based on the applications.
  
  PARAMETERS     client - application needs high cpu resource.
  
  DEPENDENCIES   None
  
  RETURN VALUE   None
  
  SIDE EFFECTS   If the clock speed changes it will also change the AXI
                 bus speed.

===========================================================================*/

void clk_regime_deregister_for_cpu_resource
(
  clkrgm_cpu_resource_client_type client 
)
{
  boolean single_client = FALSE;

  /* ------------------------------------------------------------------------
     Validate the client
  ------------------------------------------------------------------------ */

  if (!clk_regime_dyn_clock_on && client != CLKRGM_CPU_CLIENT_BOOT)
  {
    clk_regime_dcvs_log(CLKRGM_DCVS_LOG_DEREGISTER,
                       CLKRGM_DCVS_LOG_TRANS_DEREGISTER_BYPASS);
    return;
  }

  if (client >= CLKRGM_CPU_RESOURCE_NUM_CLIENTS)
  {
    MSG_ERROR("Invalid CPU client: %d", client, 0, 0);
    return;
  }

  /* ------------------------------------------------------------------------
     Update the client mask
  ------------------------------------------------------------------------ */

  INTLOCK();

  if (clkrgm.cpu_client_mask & (BITSHFT64 << client))
    clkrgm.cpu_client_usage--;

  clkrgm.cpu_client_mask &= ~(BITSHFT64 << client);

  single_client = clk_regime_single_client(client);


  INTFREE();

  MSG_HIGH("CPU client %d deregistered", client, 0, 0);

  if (single_client)
    clk_regime_process_single_cpu_client(client);
  else 
    clk_regime_process_cpu_clients();

  clk_regime_dcvs_log(CLKRGM_DCVS_LOG_DEREGISTER,
                     CLKRGM_DCVS_LOG_TRANS_DEREGISTER);
}
#endif /* defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) */

/*===========================================================================

  FUNCTION       CLK_REGIME_DEVMAN_HANDLER_SET_MINIMUM_SPEED
  
  DESCRIPTION    This function handles request to set the minimum speed
                 from the device manager.
  
  PARAMETERS     device        - the device being configured
                 minimum_speed - the minimum speed
  
  DEPENDENCIES   None
  
  RETURN VALUE   None
  
  SIDE EFFECTS   None.

===========================================================================*/

void clk_regime_devman_handler_set_minimum_speed
(
  clkrgm_device_type                device,
  clkrgm_device_minimum_speed_type  minimum_speed_khz
)
{

  switch (device)
  {
    /* ----------------------------------------------------------------------
       MCPU/HBUS/PBUS
    ---------------------------------------------------------------------- */

    case CLKRGM_DEVICE_MCPU:
    {
      #ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING

      clkrgm_mcpu_speed_type   mcpu_speed;
      clkrgm_device_param_type param;
      uint32                   min_speed_mcpu;

      /* Ignore if switching is not allowed */
      if (!clk_regime_dyn_clock_on ||
          (clkrgm.cpu_client_mask & (BITSHFT64 << CLKRGM_CPU_CLIENT_BOOT)))
      {
        break;
      }

      /* Get all relevant minimum speeds */
      clk_regime_devman_param(
        CLKRGM_DEVICE_MCPU, CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);
      min_speed_mcpu = param.minimum_speed;

      /* The loop below will find DIV1 configurations first, thus
       * giving by default configurations with higher bus speeds.
      */
      for (mcpu_speed = CLKRGM_CPU_NUM_SPEEDS-1;
           mcpu_speed >= CLKRGM_CPU_SLPOSC_SPEED &&
           clkrgm_cpu_cfg[mcpu_speed].mclk.khz > min_speed_mcpu;
           mcpu_speed--);

      if (mcpu_speed == CLKRGM_CPU_NUM_SPEEDS)
      {
        MSG_ERROR("Unable to run %d at %d KHz", device, minimum_speed_khz, 0);
        mcpu_speed = CLKRGM_CPU_FULL_SPEED;
      }

      /* Switch speeds */
      clk_regime_switch_mclk(mcpu_speed);

      #endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */

      break;
    }
    case CLKRGM_DEVICE_HBUS:
    {
      #ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING

      clkrgm_mcpu_speed_type   mcpu_speed;
      clkrgm_device_param_type param;
      uint32                   min_speed_hbus;

      /* Ignore if switching is not allowed */
      if (!clk_regime_dyn_clock_on ||
          (clkrgm.cpu_client_mask & (BITSHFT64 << CLKRGM_CPU_CLIENT_BOOT)))
      {
        break;
      }

      clk_regime_devman_param(
        CLKRGM_DEVICE_HBUS, CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);
      min_speed_hbus = param.minimum_speed;

      /* The loop below will find DIV1 configurations first, thus
       * giving by default configurations with higher bus speeds.
      */
      for (mcpu_speed = CLKRGM_CPU_NUM_SPEEDS-1;
           mcpu_speed >= CLKRGM_CPU_SLPOSC_SPEED &&
           clkrgm_cpu_cfg[mcpu_speed].hclk.khz > min_speed_hbus;
           mcpu_speed--);

      if (mcpu_speed == CLKRGM_CPU_NUM_SPEEDS)
      {
        MSG_ERROR("Unable to run %d at %d KHz", device, minimum_speed_khz, 0);
        mcpu_speed = CLKRGM_CPU_FULL_SPEED;
      }

      /* Switch speeds */
      clk_regime_switch_mclk(mcpu_speed);

      #endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */
      break;
    }

    /* ----------------------------------------------------------------------
       MDSP
    ---------------------------------------------------------------------- */

    case CLKRGM_DEVICE_MDSP:
    {
      #ifdef FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED

      clkrgm_mdsp_clk_frequency_type mdsp_speed;

      if (minimum_speed_khz == CLKRGM_DEVICE_SPEED_MAX)
      {
        mdsp_speed = CLKRGM_MDSP_SPEED_MAX;
      }
      else
      {
         for (mdsp_speed = CLKRGM_MDSP_SPEED_NOMINAL;
             mdsp_speed < CLKRGM_MDSP_NUM_SPEEDS &&
               clkrgm_modem_mdsp_cfg[mdsp_speed].clk_freq_khz <
               minimum_speed_khz;
             mdsp_speed++);
        
        if (mdsp_speed == CLKRGM_MDSP_NUM_SPEEDS)
        {
          MSG_ERROR("Unable to run mDSP at %d KHz", minimum_speed_khz, 0, 0);
          mdsp_speed = CLKRGM_MDSP_SPEED_MAX;
        }
      }

      /* Switch speeds if necessary */
      if (mdsp_speed != clk_regime_get_mdsp_clk_frequency())
      {
        clk_regime_set_mdsp(mdsp_speed);
        clk_regime_switch_mdsp_clk();
        clk_regime_complete_mdsp_clk_switch();
      }

      #endif /* FEATURE_CLKREGIM_DYNAMIC_MDSP_SPEED */

      break;
    }

    /* ----------------------------------------------------------------------
       ADSP
    ---------------------------------------------------------------------- */

    case CLKRGM_DEVICE_ADSP:
    {
      clkrgm_adsp_speed_type adsp_speed;

      if (minimum_speed_khz == CLKRGM_DEVICE_SPEED_MAX)
      {
        adsp_speed = CLKRGM_ADSP_96P0MHZ_SPEED;
      }
      else
      {
        /* Find lowest speed that satisfies the minimum */
        for (adsp_speed = CLKRGM_ADSP_TCXO_SPEED;
             adsp_speed < CLKRGM_ADSP_NUM_SPEEDS &&
               clkrgm_adsp_cfg[adsp_speed].clk_freq_khz < minimum_speed_khz;
             adsp_speed++);
        
        if (adsp_speed == CLKRGM_ADSP_NUM_SPEEDS)
        {
          MSG_ERROR("Unable to run aDSP at %d KHz", minimum_speed_khz, 0, 0);
          adsp_speed = CLKRGM_ADSP_96P0MHZ_SPEED;
        }
      }

      /* Switch speeds if necessary */
      if (adsp_speed != clkrgm.adsp_speed)
      {
        clk_regime_switch_adsp_clk(adsp_speed);
      }

      break;
    }

    /* ----------------------------------------------------------------------
       VDC
    ---------------------------------------------------------------------- */

    case CLKRGM_DEVICE_VDC:
    {
#ifdef CLKRGM_HAS_VDC
      clkrgm_vdc_speed_type vdc_speed;

      if (minimum_speed_khz == CLKRGM_DEVICE_SPEED_MAX)
      {
        vdc_speed = CLKRGM_VDC_SPEED_MAX;
      }
      else
      {
        /* Find lowest speed that satisfies the minimum */
        for (vdc_speed = CLKRGM_VDC_SPEED_INIT;
             vdc_speed < CLKRGM_VDC_NUM_SPEEDS &&
               clkrgm_vdc_cfg[vdc_speed].clk_freq_khz < minimum_speed_khz;
             vdc_speed++);
        
        if (vdc_speed == CLKRGM_VDC_NUM_SPEEDS)
        {
          MSG_ERROR("Unable to run VDC at %d KHz", minimum_speed_khz, 0, 0);
          vdc_speed = CLKRGM_VDC_SPEED_MAX;
        }
      }

      /* Switch speeds if necessary */
      if (vdc_speed != clkrgm.vdc_speed)
      {
        clk_regime_sel_vdc_clk(vdc_speed);
      }
#endif /* CLKRGM_HAS_VDC */
      break;
    }

    /* ----------------------------------------------------------------------
       MDH
    ---------------------------------------------------------------------- */

    case CLKRGM_DEVICE_PMDH:
    {
#ifdef CLKRGM_HAS_PMDH
      /* Recalculate the optimum MDH clock */
      clk_regime_calc_mdh_clk(
        CLKRGM_MDH_PRIMARY, minimum_speed_khz,
        clkrgm.pmdh_params.max_freq_khz);

#endif /* CLKRGM_HAS_PMDH */
      break;
    }

    case CLKRGM_DEVICE_EMDH:
    {
#ifdef CKLRGM_HAS_EMDH
      /* Recalculate the optimum MDH clock */
      clk_regime_calc_mdh_clk(
        CLKRGM_MDH_EXTERNAL, minimum_speed_khz,
        clkrgm.emdh_params.max_freq_khz);

#endif /* CKLRGM_HAS_EMDH */
      break;
    }

    default:
    {
      MSG_ERROR(
        "Unsupported device (%d) for set_minimum_speed", device, 0, 0);
      break;
    }
  }

} /* clk_regime_devman_handler_set_minimum_speed */

/*===========================================================================

  FUNCTION       CLK_REGIME_DEVMAN_HANDLER
  
  DESCRIPTION    This function is invoked by the device manager when
                 the configured parameter for a device changes.
  
  PARAMETERS     device  - the device being configured
                 request - the request
                 param   - the parameter for the request
  
  DEPENDENCIES   None
  
  RETURN VALUE   None
  
  SIDE EFFECTS   None.

===========================================================================*/

void clk_regime_devman_handler
(
  clkrgm_device_type          device,
  clkrgm_device_request_type  request,
  clkrgm_device_param_type   *param
)
{
  /* Currently we only support a subset of requests simply because they are
     the only ones being used. */
  switch (request)
  {
    case CLKRGM_DEVICE_REQUEST_SPEED_CHANGE_NOTIFICATION:
    {
      /* Nothing to do */
      break;
    }

    case CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED:
    {
      clk_regime_devman_handler_set_minimum_speed(
        device, param->minimum_speed);
      break;
    }

    default:
    {
      MSG_ERROR(
        "Unsupported devman request: device=%d, request=%d", device,
        request, 0);
      break;
    }
  }

} /* clk_regime_devman_handler */

void clk_regime_set_chipxn_frequency(int32 ppb)
{
  uint32 q_M;
  uint64 q_N;
  clk_regime_chipxn_mnd_cntrl_type mnd_owner;

  /* QSC1100 PLL is either 192MHz or 384 MHz. Nominal N,M values
     for 384MHz frequency are N=100000000 and M=10240000.   M will be
     double that on 192MHz.

     M = 10240000
     N = (10^8)+((PPT * 32 * 100 + 2^14)/(2^15))
     q_N = 100000000 + (ppt*3200+16384)/32768;
     Adding the 2^14 is follow the algorithm of rounding at 15th 
  */

  #if defined  FEATURE_PLL_192
    q_M = 20480000;
  #elif defined FEATURE_PLL_384
    q_M = 10240000;
  #else
    #error Unknown PLL config in Chipx32 computation
  #endif

  q_N = (100000000+(ppb*3200+16384)/32768);

  /* Limit to uint32 */
  if ( q_N > 4294967295ll ) 
  {
    q_N = 4294967295ll;
  }

  /* Remember the MND owner and set to SW control */
  mnd_owner = clk_regime_get_chipxn_mnd_ctrl();
  clk_regime_set_chipxn_mnd_ctrl(CLKRGM_CHIPXN_SW_CNTRL);

  /* Adjust N to account for offset */
  HWIO_OUT(MODEM_CDMA_CHIPXN_MNCNTR_M, q_M);
  HWIO_OUT(MODEM_CDMA_CHIPXN_MNCNTR_NOT_2D, (~((uint32)q_N)) );
  HWIO_OUT(MODEM_CDMA_CHIPXN_MNCNTR_NOT_N_M, (~((uint32)q_N-q_M)) );
  HWIO_OUT(MODEM_CDMA_CHIPXN_MNCTNR_UPDATE, 1);

  /* Restore the original owner */
  clk_regime_set_chipxn_mnd_ctrl(mnd_owner);

}

void clk_regime_set_chipxn_mnd_ctrl(clk_regime_chipxn_mnd_cntrl_type cntrl)
{
  HWIO_OUTM(MODEM_CHIPXN_CLK_CTL, HWIO_FMSK(MODEM_CHIPXN_CLK_CTL, SEL_MDSP), 
            cntrl << HWIO_SHFT(MODEM_CHIPXN_CLK_CTL, SEL_MDSP));
}

clk_regime_chipxn_mnd_cntrl_type clk_regime_get_chipxn_mnd_ctrl(void)
{
  return HWIO_INM(MODEM_CHIPXN_CLK_CTL, HWIO_FMSK(MODEM_CHIPXN_CLK_CTL, SEL_MDSP)) >> 
    HWIO_SHFT(MODEM_CHIPXN_CLK_CTL, SEL_MDSP);
}

#endif /* !BUILD_BOOT_CHAIN_OEMSBL */
