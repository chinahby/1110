#ifndef CLKRGMI_QSC_1100_H
#define CLKRGMI_QSC_1100_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     Q S C 1 1 0 0   C L O C K   R E G I M E   D E V I C E   D R I V E R

                  I N T E R N A L   D E F I N I T I O N S


GENERAL DESCRIPTION
  This module contains definitions related to the configuration of the clock
  regime services.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2004-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/clkregim/clkrgmi_msm.h#31 $
$DateTime: 2009/04/02 01:06:49 $
$Author: bamidir $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/09    br     Added support for EFUSE Clock regime.
11/21/08    hvo    Undo the aDSP clock overclocked, the original designed
                   is correct as aDSP will divded down by itself
11/20/08    hvo    Fixed aDSP clock overclocked by twice issue
11/19/08    kma    Modified MND values for GSBI1_CLK at 64MHz.
10/19/08    hvo    Removed EBI2 clock from CLKRGM_REG_A_ALL_MASK
09/22/08    hvo    Added default clock boot regimes to turn on only neccessary
                   clock regimes
06/26/08    hvo    Enabled SSBI clock regime to disable/enable XO buffer during
                   sleep and wake up
05/15/08    bc     Fixed and enabled clock regime switching of the
                   ARM/AHB/Bus clocks.
10/11/07    bc     Initial port from QSC6055v3202.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "customer.h"

#define CLKRGM_HAS_EBI2_CLK_SYNC

#ifndef _ARM_ASM_
  #include "log_codes.h"
  #include "log.h" 
  #include "clkregim.h" 
#endif

#ifdef FEATURE_CLKREGIM_RM
  #include "clkrgmi_rm.h"
#endif


  /* ------------------------------------------------------------------------
     PLL configurations options.  The following are the PLL configurations
     options that can be selected at build time.
  ------------------------------------------------------------------------ */

#if defined   FEATURE_PLL_192
  #define CLKRGM_PLL_192                       1
#elif defined FEATURE_PLL_384
  #define CLKRGM_PLL_384                       1
#endif

  /* ------------------------------------------------------------------------
     PLL configuration option selection.  The following identifies the PLL
     configuration option that has been selected at build time.
  ------------------------------------------------------------------------ */

  #if   defined(FEATURE_PLL_192)
    #define CLKRGM_PLL_CFG   CLKRGM_PLL_192
  #elif defined(FEATURE_PLL_384)
    #define CLKRGM_PLL_CFG   CLKRGM_PLL_384
  #else
    #error Must define a supported PLL config at build time.
  #endif

  #if defined   FEATURE_PLL_192
    #define CLKRGM_PLL_SPEED        192000000
  #elif defined FEATURE_PLL_384
    #define CLKRGM_PLL_SPEED        384000000
  #else
    #error PLL Configuration not supported.
  #endif

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




/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* --------------------------------------------------------------------------
   Definitions for warmup and delay times.
-------------------------------------------------------------------------- */

  /* PLL warmup time in microseconds. */

  #define PLL_WARMUP_TIME_US        50

  /* Clock disable delay in microseconds.  This time needs to be at least
     two clock cycles of the clock prior to switching.  The values chosen
     below are based on the slowest clock rates for each clock */

  #define CLKRGM_XO4_DISABLE_TIME_US 1
  #define CLKRGM_XO_DISABLE_TIME_US 1

  /* Microprocessor clock switch pause time after clock switch before
     continuing normal execution. */

  #define MCLK_SWITCH_PAUSE_US        5

/* --------------------------------------------------------------------------
        ADSP clock control defines 
-------------------------------------------------------------------------- */
#define CLK_RGM_DSP_CNTRL_DMA_CLK_V                 1
#define CLK_RGM_SLEEP_CLK_CNTRL_DSP_DMA_CLK_V       0

/* DSP will run at clock speed provided by clock regime */
#define CLK_RGM_DSP_CLK_1X_MODE                     1

/* DSP will run at half the clock speed provided by clock regime */
#define CLK_RGM_DSP_CLK_2X_MODE                     0

/* --------------------------------------------------------------------------
        Defines for switch ARM clock
-------------------------------------------------------------------------- */
#define CLK_RGM_CLOCK_SWITCH_1_V                   1
#define CLK_RGM_CLOCK_SWITCH_0_V                   0

#ifndef T_MSM6800B
  #define CLK_RGM_MSM_FS_ON_INIT_V              0x7fff
  #define CLK_RGM_MSM_FS_CNT_INIT_V             0x3ffff
#else
  #define CLK_RGM_MSM_FS_ON_INIT_V              0x7ffff
  #define CLK_RGM_MSM_FS_CNT_INIT_V             0x3fffffff
  #define CLK_RGM_MSM_FS_CNT2_INIT_V            0x3f
#endif
#define CLK_RGM_MSM_UP_FS_ON_FORCE_V          1
#define CLK_RGM_MSM_UP_FS_ON_EN_V             0

/* --------------------------------------------------------------------------
        Defines for disabling and enabling M/N:D counters
-------------------------------------------------------------------------- */
#define CLKRGM_MNCNTR_DISABLE_V                 0
#define CLKRGM_MNCNTR_ENABLE_V                  1

#define CLKRGM_MNCNTR_RESET_V                   1
#define CLKRGM_MNCNTR_RESET_DEASSERT_V          0
/* --------------------------------------------------------------------------
   Definitions for MISC_CLK_SEL1
-------------------------------------------------------------------------- */
/* UART1/2/3 clock is TCXO */
#ifdef FEATURE_UART_TCXO_CLK_FREQ
#define MISC_CLK_SEL1_CGF_V         0x00000200

#else
/* UART1/2/3 clock is TCXO/4 */
#define MISC_CLK_SEL1_CGF_V         0x00000249
#endif

/* --------------------------------------------------------------------------
   Definitions for combined clock regime enable/disable register bit masks.
-------------------------------------------------------------------------- */

#define CLKRGM_GET( base, field ) \
   ( HWIO_INM ( base, HWIO_FMSK(base,field)) \
                         >> HWIO_SHFT(base,field) )

#define CLKRGM_SET( base, field ) \
    HWIO_OUTM( base, HWIO_FMSK(base,field), HWIO_FMSK(base,field))

/* No intlock -- external lock required */
#define CLKRGM_SET_NL( base, field ) \
    HWIO_OUT( base, HWIO_IN(base) | (dword)HWIO_FMSK(base,field) )

/* No intlock and noread, so use shadow -- external lock required */
#define CLKRGM_SET_NLS( base, field ) \
    { HWIO_SHDW(base) = (HWIO_SHDW(base) | (dword)HWIO_FMSK(base,field)); \
      HWIO_OUT( base, HWIO_SHDW(base) ); } 

#define CLKRGM_CLEAR( base, field ) \
    HWIO_OUTM( base, HWIO_FMSK(base,field), 0 )

/* No intlock -- external lock required */
#define CLKRGM_CLEAR_NL( base, field ) \
    HWIO_OUT( base, HWIO_IN(base) & (dword) ~HWIO_FMSK(base,field) )

/* No intlock and noread, so use shadow -- external lock required */
#define CLKRGM_CLEAR_NLS( base, field ) \
    { HWIO_SHDW(base) = (HWIO_SHDW(base) & (dword) (~HWIO_FMSK(base,field))); \
      HWIO_OUT( base, HWIO_SHDW(base) ); }

#define CLKRGM_PULSE( base, field ) \
    CLKRGM_SET( base, field ); \
    CLKRGM_CLEAR( base, field )

/* No intlock -- external lock required */
#define CLKRGM_PULSE_NL( base, field ) \
    CLKRGM_SET_NL( base, field ); \
    CLKRGM_CLEAR_NL( base, field )

#define CLKRGM_CLEAR_MASK( base, mask ) \
    HWIO_OUTM( base, mask, 0 )

#define CLKRGM_SET_MASK( base, mask ) \
    HWIO_OUTM( base, mask, mask )

#define CLKRGM_W( base, val ) \
    HWIO_OUT( base, val )

#define CLKRGM_W_MASKED( base, field, val )                                 \
    { /*lint -save -e703 -e701 Shift left of signed quantity intentional */ \
      HWIO_OUTM( base, HWIO_FMSK(base, field),                         \
                 (val) << HWIO_SHFT(base, field));                       \
    } /*lint -restore */

#define CR_MODEM_HALT(maskname) \
        HWIO_FMSK(MODEM_CLK_HALT,##maskname##)

/* Using a named field */
#define CR_FVAL(reg, field, val) \
        ( HWIO_FVAL(reg, field, val) << HWIO_SHFT(reg, field) )

/* Using an integer value, to be shifted and masked */
#define CR_IVAL(reg, field, val) \
        ( ((val) << HWIO_SHFT(reg, field)) & HWIO_FMSK(reg,field) )

#define CLKRGM_SET_FVAL(reg, field, val)    \
        HWIO_OUTM( reg, HWIO_FMSK(reg, field), CR_FVAL(reg, field, val) )

#define CLKRGM_SET_FVAL_NL(reg, field, val)    \
        HWIO_OUT( reg, (HWIO_IN(reg) & ~HWIO_FMSK(reg, field)) | CR_FVAL(reg, field, val) )

#define CLKRGM_CD_REG_SET_MND(reg, src, mode, enable)        \
        HWIO_OUTM( reg, HWIO_FMSK( reg, DIV_MND_EN )         \
                       | HWIO_FMSK( reg, SRC_SEL )           \
                       | HWIO_FMSK( reg, DIV_MND_MODE ),     \
                       CR_FVAL(reg, DIV_MND_EN, enable)      \
                       | CR_FVAL(reg, SRC_SEL, src)          \
                       | CR_FVAL(reg, DIV_MND_MODE, mode) );
    
/* The ETM bit is excluded from the mask so the ETM clock regime can not be
   disabled. The ETM bit should be enabled by clearing the ETM bit in the
   corresponding hardware register from the debugger. */

#ifdef FEATURE_ETM
#define CLKRGM_REG_A_ALL_MASK     (HWIO_RMSK(CLK_OFF_A) & \
                                    ~(HWIO_FMSK(CLK_OFF_A,ETM_CLK) | \
                                      HWIO_FMSK(CLK_OFF_A,EBI1_IO_CLK) | \
                                      HWIO_FMSK(CLK_OFF_A,EBI1_CLK) | \
                                      HWIO_FMSK(CLK_OFF_A,AHB_AHB_CLK) | \
                                      HWIO_FMSK(CLK_OFF_A,UXMC_AHB_CLK) | \
                                      HWIO_FMSK(CLK_OFF_A,ARM_CLK) | \
                                      HWIO_FMSK(CLK_OFF_A,IMEM_CLK_SRC) | \
                                      HWIO_FMSK(CLK_OFF_A,PERIPH_AHB_CLK) \
                                  ))
#else
#define CLKRGM_REG_A_ALL_MASK     (HWIO_RMSK(CLK_OFF_A) & \
                                    ~(HWIO_FMSK(CLK_OFF_A,EBI1_IO_CLK) | \
                                      HWIO_FMSK(CLK_OFF_A,EBI1_CLK) | \
                                      HWIO_FMSK(CLK_OFF_A,AHB_AHB_CLK) | \
                                      HWIO_FMSK(CLK_OFF_A,UXMC_AHB_CLK) | \
                                      HWIO_FMSK(CLK_OFF_A,ARM_CLK) | \
                                      HWIO_FMSK(CLK_OFF_A,IMEM_CLK_SRC) | \
                                      HWIO_FMSK(CLK_OFF_A,PERIPH_AHB_CLK) \
                                  ))
#endif /* FEATURE_ETM */

/* The following clock regimes will enable during bootup
   ARM_CLK_EN, 
   IMEM_CLK_SRC_EN,
   EBI1_CLK_EN,
   EBI1_IO_CLK_EN,
   AHB_AHB_CLK_EN,
   USB_AHB_CLK_EN?,
   PERIPH_AHB_CLK_EN,
   MODEM_AHB_CLK_EN,
   AUDIO_AHB_CLK_EN?
   ADSP_AHB_CLK_EN,
   UXMC_AHB_CLK_EN,
   TLMM_GPIO_AHB_CLK_EN,
   PERPH_WEB_AHB_CLK_EN,
   EBI2_CLK_SRC_EN 
*/
#define CLKRGM_REG_A_DEFAULT_BOOT   0x23f7cf
								  
#define CLKRGM_REG_B_ALL_MASK           HWIO_RMSK(CLK_OFF_B)

#define CLKRGM_REG_A_TCXO_MASK ((CLKRGM_REG_A_ALL_MASK) & \
                                ~(HWIO_MSM_CLK_HALTA_TCXO4_CLK_HALT_BMSK))
#define CLKRGM_REG_B_TCXO_MASK (CLKRGM_REG_B_ALL_MASK)


/* --------------------------------------------------------------------------
   Definitions for EBI1 memory controller modes
-------------------------------------------------------------------------- */
#define EBI1_MEM_CTRL_MODE_XMEMC_ONLY   0x0


/* --------------------------------------------------------------------------
   Definitions for combined clock regime enable/disable register bit masks.
-------------------------------------------------------------------------- */

  /* The ETM bit is excluded from the mask so the ETM clock regime can not be
     disabled. The ETM bit should be enabled by clearing the ETM bit in the
     corresponding hardware register from the debugger. */
  /* If using SDRAM then remove the MPMC memory controller bit from the mask
     so the MPMC memory controller can not be disabled. */
  /* It doesn't look like we can halt the ebi1_io clock either without loosing
   * memory access.
   */

/* --------------------------------------------------------------------------
   PLL CONFIGURATION.

   There are two fractional PLLs available for generating clock frequencies
   than can be selected as inputs to the Type1 clock regimes.  The frequency
   configuration for the PLLs is determined at build time.
-------------------------------------------------------------------------- */

  /* ------------------------------------------------------------------------
     PLL register value definitions.  The following are definitions of values
     that can be written to the PLL hardware registers.
  ------------------------------------------------------------------------ */

  /* PLL_MODE register command bit masks. */


  /* PLL internal register settings. */

  ///////////////////////////////////////////////////////////////////////////
  //  MODE values are ported, but INTERNAL values for powerdown are not.
  ///////////////////////////////////////////////////////////////////////////

  /* ------------------------------------------------------------------------
     PLL related type definitions.  The following types are used for the
     configuration and management of the PLL cicuits.
  ------------------------------------------------------------------------ */

  #ifndef _ARM_ASM_


  /* Enum of PLL states. */

  typedef enum
  {
    CLKRGM_PLL_OFF,
    CLKRGM_PLL_RESET,
    CLKRGM_PLL_WARMUP,
    CLKRGM_PLL_BYPASS,
    CLKRGM_PLL_ON,

    CLKRGM_NUM_PLL_STATES
  } clkrgm_pll_state_type;

  /* Enum of PLL clock input sources. */

  typedef enum
  {
    CLKRGM_PLL_SRC_XO,                /* TCXO clock input                */

    CLK_RGM_NUM_PLL_SRCS
  } clkrgm_pll_src_type;


  /* Enum of PLL clients. The following clients can receive the output of one
     of the PLLs directly.  These include the Type 1 clock regimes. Since the
     items in this type are used as bit positions in a bitmask, there cannot
     be more than 32 items in this enum.

     Additional external clients should be added to the external clients
     section of this enum and should be coordinated with the definition of
     the external clients in the clkrgm_ext_pll_client_type definition. */

  typedef enum
  {
    CLKRGM_PLL_ADSP_CLIENT = 0x0,     /* Application DSP clock regime      */
    CLKRGM_PLL_ICODEC_CLIENT,         /* Internal Codec clock regime       */
    CLKRGM_PLL_ECODEC_CLIENT,         /* External Codec clock regime       */
    CLKGRM_PLL_SDAC_CLIENT,           /* External SDAC clock regime        */
    CLKRGM_PLL_CPU_CLIENT,            /* Microprocessor client             */
    CLKRGM_PLL_CAMCLK_CLIENT,         /* External camera clock (GPIO 13)   */
    CLKRGM_PLL_SDCC_CLIENT,           /* SDCC clock                        */
    CLKRGM_PLL_EXT_BT_CLIENT,         /* BT audio *external* client        */
    CLKRGM_PLL_EXT_CODEC_CLIENT,      /* BT audio *external* client        */

    CLKRGM_PLL_UART_DM_CLIENT,
    CLKRGM_PLL_USB_CLIENT,
    CLKRGM_PLL_MDSP,
    CLKRGM_PLL_VIDEO_VFE_CLIENT,
    CLKRGM_PLL_GSBI0_CLIENT,
    CLKRGM_PLL_GSBI1_CLIENT,
    CLKRGM_PLL_GSBI2_CLIENT,

    CLKRGM_MSM_PLL_NUM_CLIENTS
  } clkrgm_msm_pll_client_type;


  /* Structure for PLL state and register values. */

  typedef struct
  {
    clkrgm_pll_type       pll;            /* PLL identifier                  */
    clkrgm_pll_src_type   src;            /* PLL input source                */
    uint32                l_reg_val;      /* PLL L_VAL register value        */
    uint32                m_reg_val;      /* PLL M_VAL register value        */
    uint32                n_reg_val;      /* PLL N_VAL register value        */
    uint32                test_ctrl_val;  /* PLL TEST_CTL register value     */
    uint32                config_reg_val; /* PLL config register value       */
  } clkrgm_pll_cfg_type;

  /* Enum of ADSP PLL frequencies */
  typedef enum 
  {
    CLKRGM_PLL_192_MHZ,    /* source PLL freq for 1x clock mode */
    CLKRGM_PLL_384_MHZ,    /* source PLL freq for 2x clock mode */
    CLKRGM_PLL_MHZ_LAST
  } clkrgm_pll_freq_type;

  #endif /* _ARM_ASM */

  /* ------------------------------------------------------------------------
     MODEM PLL CONFIGURATION. NT_PLL0

     The following selects the MODEM PLL register values for the specified
     PLL configuration.
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

  #ifndef CLKRGM_PLL_CFG 
  #error "CLKRGM_PLL_CFG must be defined"
  #endif
  #if ( CLKRGM_PLL_CFG  ==  CLKRGM_PLL_192 )
    #define CLKRGM_PLL_SRC                    CLKRGM_PLL_SRC_XO
    #define CLKRGM_PLL_L_VAL_REG_VAL          0xA
    #define CLKRGM_PLL_M_VAL_REG_VAL          0x0
    #define CLKRGM_PLL_N_VAL_REG_VAL          0x01
    #define CLKRGM_PLL_TESTCTL_REG_VAL        0x0
    #define CLKRGM_PLL_CFG_REG_VAL            0x00200182

  #elif ( CLKRGM_PLL_CFG  ==  CLKRGM_PLL_384 )
    #define CLKRGM_PLL_SRC                    CLKRGM_PLL_SRC_XO
    #define CLKRGM_PLL_L_VAL_REG_VAL          0x14
    #define CLKRGM_PLL_M_VAL_REG_VAL          0x0
    #define CLKRGM_PLL_N_VAL_REG_VAL          0x01
    #define CLKRGM_PLL_TESTCTL_REG_VAL        0x0
    #define CLKRGM_PLL_CFG_REG_VAL            0x00201182

  #else
    #error PLL Configuration not supported.
  #endif

  /* Populate register configuration data structure with selected values. */

  #define CLKRGM_PLL_CONFIG             \
  {                                     \
    CLKRGM_PLL,                        \
    CLKRGM_PLL_SRC,                    \
    CLKRGM_PLL_L_VAL_REG_VAL,          \
    CLKRGM_PLL_M_VAL_REG_VAL,          \
    CLKRGM_PLL_N_VAL_REG_VAL,          \
    CLKRGM_PLL_TESTCTL_REG_VAL,        \
    CLKRGM_PLL_CFG_REG_VAL             \
  }
  

#ifndef _ARM_ASM_


/* --------------------------------------------------------------------------
   TYPE 1 CLOCK REGIME DEFINITIONS.

   The Type 1 clock regimes each have a dedicated source selection mux, clock
   input divider and M/N:D counter that can be programmed to generate the
   desired output clock frequency from the selected input clock source.
-------------------------------------------------------------------------- */
  /* Enum of Type 1 clock regime input clock sources. */

  typedef enum
  {
    /* All the Type 1 clock regimes share these 4 sources as the first 4, so GSBI0_CD_REG was
       selected as being typical of all the Type 1 clock sources */
    CLKRGM_TYPE1_SRC_IS_EXT = HWIO_FVAL(GSBI0_CD_REG, SRC_SEL, EXT_CLK),     /* EXT_CLK     */
    CLKRGM_TYPE1_SRC_IS_XO = HWIO_FVAL(GSBI0_CD_REG, SRC_SEL, XO),           /* XO clock    */
    CLKRGM_TYPE1_SRC_IS_PLL = HWIO_FVAL(GSBI0_CD_REG, SRC_SEL, PLL_SRC),     /* PLL         */ 
    CLKRGM_TYPE1_SRC_IS_SLPOSC = HWIO_FVAL(GSBI0_CD_REG, SRC_SEL, SLEEP_CLK),/* SLEEP clock */

    CLKRGM_NUM_TYPE1_SRCS,
    CLKRGM_TYPE1_SRC_IS_INVALID,
  } clkrgm_msm_type1_src_type;

  /* Structure of Type 1 ARM clock regime configuration items. */

  typedef struct
  {
    clkrgm_msm_type1_src_type  src;          /* Input clock source           */
    uint32                     mn_reg;       /* MN register value            */
    uint32                     cd_reg;       /* cd register value            */
  } clkrgm_msm_type1_cfg_type;


  /* I2C clock rates */
  #define CLKRGM_I2C_TCXO_FREQ_KHZ   19200
  #define CLKRGM_I2C_SLPOSC_FREQ_KHZ 32 
  #define CLKRGM_I2C_TCXO4_KHZ       4800
  #define CLKRGM_I2C_PAUSE 1

  #define CLKRGM_GSBI0_CD_REG_XO     0x00810000
  #define CLKRGM_GSBI1_CD_REG_XO     0x00000100
  #define CLKRGM_GSBI2_CD_REG_XO     0x00000100

  /* Set MND for 1/4 of XO */
  /* M/N = 1/4, M_reg = 1, N_reg = ~(4-1) = (field_mask - 3) */
  #define CLKRGM_GSBI0_MN_REG_XO4 \
     ((1<<HWIO_SHFT(GSBI0_MN_REG, M_VAL)) | (HWIO_FMSK(GSBI0_MN_REG, N_VAL) - (4-1)))
  #define CLKRGM_GSBI1_MN_REG_XO4 \
     ((1<<HWIO_SHFT(GSBI1_MN_REG, M_VAL)) | (HWIO_FMSK(GSBI1_MN_REG, N_VAL) - (4-1)))
  #define CLKRGM_GSBI2_MN_REG_XO4 \
     ((1<<HWIO_SHFT(GSBI2_MN_REG, M_VAL)) | (HWIO_FMSK(GSBI1_MN_REG, N_VAL) - (4-1)))

  /* D = 4/2 = 2.  D_reg = ~(2*D) = ~4 = field_mask - 4; */

  #define CLKRGM_GSBI0_CD_REG_XO4 \
    ( CR_FVAL(GSBI0_CD_REG, DIV_MND_EN, ENABLED) \
      | CR_FVAL(GSBI0_CD_REG, DIV_MND_MODE, DUAL_EDGE_MODE) \
      | (HWIO_FMSK(GSBI0_CD_REG, D_VAL)-4) )
  #define CLKRGM_GSBI0_CD_REG_XO4_MASK \
    ( HWIO_FMSK(GSBI0_CD_REG, DIV_MND_EN) \
      | HWIO_FMSK(GSBI0_CD_REG, DIV_MND_MODE) \
      | HWIO_FMSK(GSBI0_CD_REG, D_VAL) )

  #define CLKRGM_GSBI1_CD_REG_XO4 \
    ( CR_FVAL(GSBI1_CD_REG, DIV_MND_EN, ENABLED) \
      | CR_FVAL(GSBI1_CD_REG, DIV_MND_MODE, DUAL_EDGE_MODE) \
      | (HWIO_FMSK(GSBI1_CD_REG, D_VAL)-4) )
  #define CLKRGM_GSBI1_CD_REG_XO4_MASK \
    ( HWIO_FMSK(GSBI1_CD_REG, DIV_MND_EN) \
      | HWIO_FMSK(GSBI1_CD_REG, DIV_MND_MODE) \
      | HWIO_FMSK(GSBI1_CD_REG, D_VAL) )

  #define CLKRGM_GSBI2_CD_REG_XO4 \
    ( CR_FVAL(GSBI2_CD_REG, DIV_MND_EN, ENABLED) \
      | CR_FVAL(GSBI2_CD_REG, DIV_MND_MODE, DUAL_EDGE_MODE) \
      | (HWIO_FMSK(GSBI2_CD_REG, D_VAL)-4) )
  #define CLKRGM_GSBI2_CD_REG_XO4_MASK \
    ( HWIO_FMSK(GSBI2_CD_REG, DIV_MND_EN) \
      | HWIO_FMSK(GSBI2_CD_REG, DIV_MND_MODE) \
      | HWIO_FMSK(GSBI2_CD_REG, D_VAL) )

  typedef struct
  {
    clkrgm_msm_type1_src_type src;       /* Input clock source              */
    uint16                    div;       /* Divider value                   */
    uint16                    mode;      /* DSP_DMA_CLK_OVR 1X mode value   */
    uint32                    clk_freq_khz;       /* aDSP clock freq in KHz */
  } clkrgm_adsp_cfg_type;


/* --------------------------------------------------------------------------
   MICROPROCESSOR CLOCK REGIME.

   This section provides the type definitions and supported configurations
   for this clock regime.  At compile time, the actual configuration to be
   used at runtime is determined based on the compile time options.
-------------------------------------------------------------------------- */

  /* Microprocessor clock sources */
  typedef enum
  {
    CLKRGM_MCLK_SRC_XO    =  HWIO_FVAL(ARM_CFG_REG, SRC_SEL0, XO),
    CLKRGM_MCLK_SRC_PLL   =  HWIO_FVAL(ARM_CFG_REG, SRC_SEL0, PLL_SRC),
    CLKRGM_MCLK_SRC_SLEEP =  HWIO_FVAL(ARM_CFG_REG, SRC_SEL0, SLEEP_CLK),
    CLKRGM_MCLK_SRC_EXT   =  HWIO_FVAL(ARM_CFG_REG, SRC_SEL0, EXT_CLK),
  } clkrgm_global_src_type;

  /* Microprocessor clock source dividers */
  typedef enum
  {
    CLKRGM_MCLK_DIV1   = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_1 ),
    CLKRGM_MCLK_DIV2   = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_2 ),
    CLKRGM_MCLK_DIV3   = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_3 ),
    CLKRGM_MCLK_DIV4   = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_4 ),
    CLKRGM_MCLK_DIV5   = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_5 ),
    CLKRGM_MCLK_DIV6   = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_6 ),
    CLKRGM_MCLK_DIV7   = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_7 ),
    CLKRGM_MCLK_DIV8   = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_8 ),
    CLKRGM_MCLK_DIV9   = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_9 ),
    CLKRGM_MCLK_DIV10  = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_10),
    CLKRGM_MCLK_DIV11  = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_11),
    CLKRGM_MCLK_DIV12  = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_12),
    CLKRGM_MCLK_DIV13  = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_13),
    CLKRGM_MCLK_DIV14  = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_14),
    CLKRGM_MCLK_DIV15  = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_15),
    CLKRGM_MCLK_DIV16  = HWIO_FVAL(ARM_MOD_REG,ARM_DIV_MOD1,DIV_16),
  } clkrgm_global_div_type;

  typedef enum
  {
    CLKRGM_MCLK_RATESEL_DIV1   = HWIO_FVAL(ARM_CFG_REG,ARM_RATE_SEL,DIV_1),
    CLKRGM_MCLK_RATESEL_DIV2   = HWIO_FVAL(ARM_CFG_REG,ARM_RATE_SEL,DIV_2)
  } clkrgm_arm_ratesel_type;

  /* Microprocessor clock source description */
  typedef struct
  {
    clkrgm_global_src_type  src;       /* NS register input clock source */
    clkrgm_global_div_type  div;       /* Divider value                  */
  } clkrgm_msm_global_cfg_type;

  /* ------------------------------------------------------------------------
     MICROPROCESSOR derived clock regime type definitions.

     The AHB, EBI1, and EBI2 clocks are derived from the micro clock by
     dividers to create syncronous clocks.
  ------------------------------------------------------------------------ */

  /* Memory bus frequency divider options. */
  typedef enum
  {
    CLKRGM_BUS_IS_MCLK_DIV1         = 0,
    CLKRGM_BUS_IS_MCLK_DIV2         = 1,
    CLKRGM_BUS_IS_MCLK_DIV4         = 2,
    CLKRGM_BUS_IS_MCLK_DIV8         = 3,
  } clkrgm_msm_bus_div_type;

  /* Structure for storage of microprocessor and memory bus clock frequencies
     in KHz and MHz. */

  typedef struct
  {
    uint32    khz;  /* Frequency in KHz */
    uint32    mhz;  /* Frequency in MHz */
  } clkrgm_msm_freqs_type;

  /* Structure for microprocessor clock regime configuration information. */

  typedef struct
  {
    /* The global clock that serves as source for all other arm domain clocks */
    clkrgm_msm_global_cfg_type   global_cfg;
    clkrgm_msm_freqs_type         global; 

    clkrgm_arm_ratesel_type  mclk_div;    /* Processor div from global clock */
    clkrgm_msm_freqs_type    mclk;        /* Processor MCLK frequency         */
    clkrgm_msm_bus_div_type  hclk_div;    /* Divider from global clock */
    clkrgm_msm_freqs_type    hclk;        /* Memory bus HCLK frequency        */
    clkrgm_msm_bus_div_type  ebi1_div;    /* Divider from global clock */
    clkrgm_msm_freqs_type    ebi1;        /* Memory bus HCLK frequency        */
    clkrgm_msm_bus_div_type  ebi2_div;    /* Divider from global clock */
    clkrgm_msm_freqs_type    ebi2;         /* Memory bus HCLK frequency        */
    uint16                   pm_vreg_mv;
 
  } clkrgm_mclk_clkrgm_type;

#endif /* _ARM_ASM_ */

/* PMIC voltage for MSMC */

#ifndef FEATURE_CUSTDRIVERS_HAS_MSMC_V

#define MSMC_VOLTAGE_HIGH_V     1200
#define MSMC_VOLTAGE_LOW_V      1200

#endif /* !FEATURE_CUSTDRIVERS_HAS_MSMC_V */


#ifndef _ARM_ASM_

/* Used for logging clocks switch info */
typedef PACKED struct {
  log_hdr_type hdr;
  uint8  cpu_speed;
  uint8  adsp_speed;
  uint8  mdsp_speed;
  uint16 msmc_mv;
  uint64 cpu_client_mask;
} clk_regime_switch_log_type;
  

/* Structure for varying CPU voltage */
typedef PACKED struct {
  diagpkt_subsys_header_type header;
  uint8 cpu_speed;
  uint16 vreg_mv;
} clkrgm_diag_set_voltage_per_cpu_speed_req_type;

/* SUBSYSTEM PACKET CODE DEFINITIONS*/
#define CLKRGM_SET_VOLTAGE_PER_CPU_SPEED_F      0

/* Voltage switching stepping */
#define CLKRGM_VOLTAGE_SWITCHING_STEP_MV              25

/* Delay between each voltage switching step */
#define CLKRGM_VOLTAGE_SWITCHING_DELAY_US             100

#endif /* _ARM_ASM_ */

  /* ------------------------------------------------------------------------
     CPU sleep xtal speed configuration (default after RESET) - 19.20 MHz.
  ------------------------------------------------------------------------ */

  /* CPU clock = 32 khz, sleep clock divided by 1 */
  #define  CLKRGM_CPU_SLP_SPEED_CLK_SRC           CLKRGM_MCLK_SRC_SLEEP 
  #define  CLKRGM_CPU_SLP_SPEED_DIV_VAL           CLKRGM_MCLK_DIV1
  #define  CLKRGM_CPU_SLP_SPEED_GLOBAL_FREQ_KHZ   (CLKRGM_SLP_FREQ_KHZ/1)
  #define  CLKRGM_CPU_SLP_SPEED_RS_VAL            CLKRGM_MCLK_RATESEL_DIV1
  #define  CLKRGM_CPU_SLP_SPEED_MCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_SLP_SPEED_GLOBAL_FREQ_KHZ / 1 )

  /* AHB clock = 32 kHz, MCLK/1 */
  #define  CLKRGM_CPU_SLP_SPEED_HCLK_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_SLP_SPEED_HCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_SLP_SPEED_GLOBAL_FREQ_KHZ/ 1 )

  /* EBI1 clock = 32 kHz, MCLK/1 */
  #define  CLKRGM_CPU_SLP_SPEED_EBI1_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_SLP_SPEED_EBI1_KHZ          \
             ( CLKRGM_CPU_SLP_SPEED_GLOBAL_FREQ_KHZ/ 1 )

  /* EBI2 clock = 32 kHz, MCLK/1 */
  #define  CLKRGM_CPU_SLP_SPEED_EBI2_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_SLP_SPEED_EBI2_KHZ          \
             ( CLKRGM_CPU_SLP_SPEED_GLOBAL_FREQ_KHZ/ 1 )

  /* Minimum voltage needed for this CPU configuration */
  #define  CLKRGM_CPU_SLP_SPEED_MSMC_mV           MSMC_VOLTAGE_LOW_V     


  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CPU_SLP_SPEED_CONFIG                              \
  {                                                                \
    {                                                              \
      CLKRGM_CPU_SLP_SPEED_CLK_SRC,                                \
      CLKRGM_CPU_SLP_SPEED_DIV_VAL,                                \
    },                                                             \
    {                                                              \
      CLKRGM_CPU_SLP_SPEED_GLOBAL_FREQ_KHZ,                        \
      CLKRGM_CPU_SLP_SPEED_GLOBAL_FREQ_KHZ/1000                    \
    },                                                             \
    CLKRGM_CPU_SLP_SPEED_RS_VAL,                                   \
    {                                                              \
      CLKRGM_CPU_SLP_SPEED_MCLK_FREQ_KHZ,                          \
      CLKRGM_CPU_SLP_SPEED_MCLK_FREQ_KHZ/1000                      \
    },                                                             \
    CLKRGM_CPU_SLP_SPEED_HCLK_DIV,                                 \
    {                                                              \
      CLKRGM_CPU_SLP_SPEED_HCLK_FREQ_KHZ,                          \
      CLKRGM_CPU_SLP_SPEED_HCLK_FREQ_KHZ/1000                      \
    },                                                             \
    CLKRGM_CPU_SLP_SPEED_EBI1_DIV,                                 \
    {                                                              \
      CLKRGM_CPU_SLP_SPEED_EBI1_KHZ,                               \
      CLKRGM_CPU_SLP_SPEED_EBI1_KHZ/1000                           \
    },                                                             \
    CLKRGM_CPU_SLP_SPEED_EBI2_DIV,                                 \
    {                                                              \
      CLKRGM_CPU_SLP_SPEED_EBI2_KHZ,                               \
      CLKRGM_CPU_SLP_SPEED_EBI2_KHZ/1000                           \
    },                                                             \
    CLKRGM_CPU_SLP_SPEED_MSMC_mV                                   \
  }



  /* ------------------------------------------------------------------------
     CPU XO speed configuration (default after RESET) - 19.20 MHz.
  ------------------------------------------------------------------------ */

  /* CPU clock = 19.2 MHz, source XO, divided by 1 */
  #define  CLKRGM_CPU_XO_SPEED_CLK_SRC           CLKRGM_MCLK_SRC_XO
  #define  CLKRGM_CPU_XO_SPEED_DIV_VAL           CLKRGM_MCLK_DIV1
  #define  CLKRGM_CPU_XO_SPEED_GLOBAL_FREQ_KHZ   (CLKRGM_XO_FREQ_KHZ/1)
  #define  CLKRGM_CPU_XO_SPEED_RS_VAL            CLKRGM_MCLK_RATESEL_DIV1
  #define  CLKRGM_CPU_XO_SPEED_MCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_XO_SPEED_GLOBAL_FREQ_KHZ / 1 )

  /* AHB clock = 19.2 MHz, MCLK/1 */
  #define  CLKRGM_CPU_XO_SPEED_HCLK_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_XO_SPEED_HCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_XO_SPEED_GLOBAL_FREQ_KHZ / 1 )

  /* EBI1 clock = 19.2 MHz, MCLK/1 */
  #define  CLKRGM_CPU_XO_SPEED_EBI1_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_XO_SPEED_EBI1_KHZ          \
             ( CLKRGM_CPU_XO_SPEED_GLOBAL_FREQ_KHZ / 1 )

  /* EBI2 clock = 19.2 MHz, MCLK/1 */
  #define  CLKRGM_CPU_XO_SPEED_EBI2_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_XO_SPEED_EBI2_KHZ          \
             ( CLKRGM_CPU_XO_SPEED_GLOBAL_FREQ_KHZ / 1 )

  /* Minimum voltage needed for this CPU configuration */
  #define  CLKRGM_CPU_XO_SPEED_MSMC_mV           MSMC_VOLTAGE_HIGH_V     


  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CPU_XO_SPEED_CONFIG                             \
  {                                                                \
    {                                                              \
      CLKRGM_CPU_XO_SPEED_CLK_SRC,                               \
      CLKRGM_CPU_XO_SPEED_DIV_VAL,                               \
    },                                                             \
    {                                                              \
      CLKRGM_CPU_XO_SPEED_GLOBAL_FREQ_KHZ,                         \
      CLKRGM_CPU_XO_SPEED_GLOBAL_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_XO_SPEED_RS_VAL,                                   \
    {                                                              \
      CLKRGM_CPU_XO_SPEED_MCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_XO_SPEED_MCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_XO_SPEED_HCLK_DIV,                                \
    {                                                              \
      CLKRGM_CPU_XO_SPEED_HCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_XO_SPEED_HCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_XO_SPEED_EBI1_DIV,                                \
    {                                                              \
      CLKRGM_CPU_XO_SPEED_EBI1_KHZ,                              \
      CLKRGM_CPU_XO_SPEED_EBI1_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_XO_SPEED_EBI2_DIV,                                \
    {                                                              \
      CLKRGM_CPU_XO_SPEED_EBI2_KHZ,                              \
      CLKRGM_CPU_XO_SPEED_EBI2_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_XO_SPEED_MSMC_mV                                  \
  }

  /* ------------------------------------------------------------------------
     CPU QUARTER speed configuration - 24 MHz
  ------------------------------------------------------------------------ */

#if defined FEATURE_PLL_192
  /* CPU clock = 24 MHz, source PLL, div 8 */
  #define  CLKRGM_CPU_QUARTER_SPEED_CLK_SRC           CLKRGM_MCLK_SRC_PLL
  #define  CLKRGM_CPU_QUARTER_SPEED_DIV_VAL           CLKRGM_MCLK_DIV8
  #define  CLKRGM_CPU_QUARTER_SPEED_GLOBAL_FREQ_KHZ   (CLKRGM_PLL_FREQ_KHZ/8)
  #define  CLKRGM_CPU_QUARTER_SPEED_RS_VAL            CLKRGM_MCLK_RATESEL_DIV1
  #define  CLKRGM_CPU_QUARTER_SPEED_MCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_QUARTER_SPEED_GLOBAL_FREQ_KHZ / 1 )

#elif defined FEATURE_PLL_384
/* CPU clock = 24 MHz, source PLL, div 12 */
  #define  CLKRGM_CPU_QUARTER_SPEED_CLK_SRC           CLKRGM_MCLK_SRC_PLL
  #define  CLKRGM_CPU_QUARTER_SPEED_DIV_VAL           CLKRGM_MCLK_DIV12
  #define  CLKRGM_CPU_QUARTER_SPEED_GLOBAL_FREQ_KHZ   (CLKRGM_PLL_FREQ_KHZ/12)
  #define  CLKRGM_CPU_QUARTER_SPEED_RS_VAL            CLKRGM_MCLK_RATESEL_DIV1
  #define  CLKRGM_CPU_QUARTER_SPEED_MCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_QUARTER_SPEED_GLOBAL_FREQ_KHZ / 1 )
#endif

  /* AHB clock = 24 MHz, MCLK/1 */
  #define  CLKRGM_CPU_QUARTER_SPEED_HCLK_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_QUARTER_SPEED_HCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_QUARTER_SPEED_GLOBAL_FREQ_KHZ/ 1 )

  /* EBI1 clock = 24 MHz, MCLK/1 */
  #define  CLKRGM_CPU_QUARTER_SPEED_EBI1_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_QUARTER_SPEED_EBI1_KHZ          \
             ( CLKRGM_CPU_QUARTER_SPEED_GLOBAL_FREQ_KHZ/ 1 )

  /* EBI2 clock = 24 MHz, MCLK/1 */
  #define  CLKRGM_CPU_QUARTER_SPEED_EBI2_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_QUARTER_SPEED_EBI2_KHZ          \
             ( CLKRGM_CPU_QUARTER_SPEED_GLOBAL_FREQ_KHZ/ 1 )

  /* Minimum voltage needed for this CPU configuration */
  #define  CLKRGM_CPU_QUARTER_SPEED_MSMC_mV           MSMC_VOLTAGE_HIGH_V     


  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CPU_QUARTER_SPEED_CONFIG                             \
  {                                                                \
    {                                                              \
      CLKRGM_CPU_QUARTER_SPEED_CLK_SRC,                               \
      CLKRGM_CPU_QUARTER_SPEED_DIV_VAL,                               \
    },                                                             \
    {                                                              \
      CLKRGM_CPU_QUARTER_SPEED_GLOBAL_FREQ_KHZ,                         \
      CLKRGM_CPU_QUARTER_SPEED_GLOBAL_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_QUARTER_SPEED_RS_VAL,                                   \
    {                                                              \
      CLKRGM_CPU_QUARTER_SPEED_MCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_QUARTER_SPEED_MCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_QUARTER_SPEED_HCLK_DIV,                                \
    {                                                              \
      CLKRGM_CPU_QUARTER_SPEED_HCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_QUARTER_SPEED_HCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_QUARTER_SPEED_EBI1_DIV,                                \
    {                                                              \
      CLKRGM_CPU_QUARTER_SPEED_EBI1_KHZ,                              \
      CLKRGM_CPU_QUARTER_SPEED_EBI1_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_QUARTER_SPEED_EBI2_DIV,                                \
    {                                                              \
      CLKRGM_CPU_QUARTER_SPEED_EBI2_KHZ,                              \
      CLKRGM_CPU_QUARTER_SPEED_EBI2_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_QUARTER_SPEED_MSMC_mV                                  \
  }


  /* ------------------------------------------------------------------------
     CPU 32.0 MHz speed configuration 
  ------------------------------------------------------------------------ */

  /* CPU clock = 32.0 MHz, source PLL, div 6 */
  #define  CLKRGM_CPU_32P0_DIV1_CLK_SRC           CLKRGM_MCLK_SRC_PLL
  #define  CLKRGM_CPU_32P0_DIV1_DIV_VAL           CLKRGM_MCLK_DIV6
  #define  CLKRGM_CPU_32P0_DIV1_GLOBAL_FREQ_KHZ   (CLKRGM_PLL_FREQ_KHZ/6)
  #define  CLKRGM_CPU_32P0_SPEED_RS_VAL           CLKRGM_MCLK_RATESEL_DIV1
  #define  CLKRGM_CPU_32P0_DIV1_MCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_32P0_DIV1_GLOBAL_FREQ_KHZ / 1 )

  /* AHB clock = 32.0 MHz, GLOBAL/1 */
  #define  CLKRGM_CPU_32P0_DIV1_HCLK_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_32P0_DIV1_HCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_32P0_DIV1_GLOBAL_FREQ_KHZ / 1 )

  /* EBI1 clock = 32.0 MHz, GLOBAL/1 */
  #define  CLKRGM_CPU_32P0_DIV1_EBI1_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_32P0_DIV1_EBI1_KHZ          \
             ( CLKRGM_CPU_32P0_DIV1_GLOBAL_FREQ_KHZ / 1 )

  /* EBI2 clock = 32.0 MHz, GLOBAL/1 */
  #define  CLKRGM_CPU_32P0_DIV1_EBI2_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_32P0_DIV1_EBI2_KHZ          \
             ( CLKRGM_CPU_32P0_DIV1_GLOBAL_FREQ_KHZ / 1 )

  /* Minimum voltage needed for this CPU configuration */
  #define  CLKRGM_CPU_32P0_DIV1_MSMC_mV           MSMC_VOLTAGE_HIGH_V     


  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CPU_32P0_DIV1_CONFIG                             \
  {                                                                \
    {                                                              \
      CLKRGM_CPU_32P0_DIV1_CLK_SRC,                               \
      CLKRGM_CPU_32P0_DIV1_DIV_VAL,                               \
    },                                                             \
    {                                                              \
      CLKRGM_CPU_32P0_DIV1_GLOBAL_FREQ_KHZ,                         \
      CLKRGM_CPU_32P0_DIV1_GLOBAL_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_32P0_SPEED_RS_VAL,                                   \
    {                                                              \
      CLKRGM_CPU_32P0_DIV1_MCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_32P0_DIV1_MCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_32P0_DIV1_HCLK_DIV,                                \
    {                                                              \
      CLKRGM_CPU_32P0_DIV1_HCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_32P0_DIV1_HCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_32P0_DIV1_EBI1_DIV,                                \
    {                                                              \
      CLKRGM_CPU_32P0_DIV1_EBI1_KHZ,                              \
      CLKRGM_CPU_32P0_DIV1_EBI1_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_32P0_DIV1_EBI2_DIV,                                \
    {                                                              \
      CLKRGM_CPU_32P0_DIV1_EBI2_KHZ,                              \
      CLKRGM_CPU_32P0_DIV1_EBI2_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_32P0_DIV1_MSMC_mV                                  \
  }

  /* ------------------------------------------------------------------------
     CPU 38.4 MHz speed configuration - PLL/5 = 38.4 MHz
  ------------------------------------------------------------------------ */

  /* CPU clock = 38.4 MHz, source PLL, div 5 */
  #define  CLKRGM_CPU_38P4_DIV1_CLK_SRC           CLKRGM_MCLK_SRC_PLL
  #define  CLKRGM_CPU_38P4_DIV1_DIV_VAL           CLKRGM_MCLK_DIV5
  #define  CLKRGM_CPU_38P4_DIV1_GLOBAL_FREQ_KHZ   (CLKRGM_PLL_FREQ_KHZ/5)
  #define  CLKRGM_CPU_38P4_SPEED_RS_VAL           CLKRGM_MCLK_RATESEL_DIV1
  #define  CLKRGM_CPU_38P4_DIV1_MCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_38P4_DIV1_GLOBAL_FREQ_KHZ / 1 )

  /* AHB clock = 38.4 MHz, MCLK/1 */
  #define  CLKRGM_CPU_38P4_DIV1_HCLK_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_38P4_DIV1_HCLK_FREQ_KHZ     \
             (CLKRGM_CPU_38P4_DIV1_GLOBAL_FREQ_KHZ/ 1  )

  /* EBI1 clock = 38.4 MHz, MCLK/1 */
  #define  CLKRGM_CPU_38P4_DIV1_EBI1_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_38P4_DIV1_EBI1_KHZ          \
             (CLKRGM_CPU_38P4_DIV1_GLOBAL_FREQ_KHZ/ 1  )

  /* EBI2 clock = 19.2 MHz, MCLK/2 */
  #define  CLKRGM_CPU_38P4_DIV1_EBI2_DIV          CLKRGM_BUS_IS_MCLK_DIV2
  #define  CLKRGM_CPU_38P4_DIV1_EBI2_KHZ          \
             (CLKRGM_CPU_38P4_DIV1_GLOBAL_FREQ_KHZ/ 2  )

  /* Minimum voltage needed for this CPU configuration */
  #define  CLKRGM_CPU_38P4_DIV1_MSMC_mV           MSMC_VOLTAGE_HIGH_V     


  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CPU_38P4_DIV1_CONFIG                             \
  {                                                                \
    {                                                              \
      CLKRGM_CPU_38P4_DIV1_CLK_SRC,                               \
      CLKRGM_CPU_38P4_DIV1_DIV_VAL,                               \
    },                                                             \
    {                                                              \
      CLKRGM_CPU_38P4_DIV1_GLOBAL_FREQ_KHZ,                         \
      CLKRGM_CPU_38P4_DIV1_GLOBAL_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_38P4_SPEED_RS_VAL,                                  \
    {                                                              \
      CLKRGM_CPU_38P4_DIV1_MCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_38P4_DIV1_MCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_38P4_DIV1_HCLK_DIV,                                \
    {                                                              \
      CLKRGM_CPU_38P4_DIV1_HCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_38P4_DIV1_HCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_38P4_DIV1_EBI1_DIV,                                \
    {                                                              \
      CLKRGM_CPU_38P4_DIV1_EBI1_KHZ,                              \
      CLKRGM_CPU_38P4_DIV1_EBI1_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_38P4_DIV1_EBI2_DIV,                                \
    {                                                              \
      CLKRGM_CPU_38P4_DIV1_EBI2_KHZ,                              \
      CLKRGM_CPU_38P4_DIV1_EBI2_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_38P4_DIV1_MSMC_mV                                  \
  }

  /* ------------------------------------------------------------------------
     CPU HALF speed configuration - 48 MHz
  ------------------------------------------------------------------------ */

#if defined FEATURE_PLL_192

  /* CPU clock = 48 MHz, source PLL, div 4 */
  #define  CLKRGM_CPU_HALF_SPEED_CLK_SRC           CLKRGM_MCLK_SRC_PLL
  #define  CLKRGM_CPU_HALF_SPEED_DIV_VAL           CLKRGM_MCLK_DIV4
  #define  CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ   (CLKRGM_PLL_FREQ_KHZ/4)
  #define  CLKRGM_CPU_HALF_SPEED_RS_VAL            CLKRGM_MCLK_RATESEL_DIV1
  #define  CLKRGM_CPU_HALF_SPEED_MCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ/1 )
#elif defined FEATURE_PLL_384
  /* CPU clock = 64 MHz, source PLL, div 6 */
  #define  CLKRGM_CPU_HALF_SPEED_CLK_SRC           CLKRGM_MCLK_SRC_PLL
  #define  CLKRGM_CPU_HALF_SPEED_DIV_VAL           CLKRGM_MCLK_DIV6 /*Added to correct the problem of half speed - If needed recalculate*/
  #define  CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ   (CLKRGM_PLL_FREQ_KHZ/6)
  #define  CLKRGM_CPU_HALF_SPEED_RS_VAL            CLKRGM_MCLK_RATESEL_DIV1
  #define  CLKRGM_CPU_HALF_SPEED_MCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ/1 )
#endif

#if defined  FEATURE_PLL_192


  /* AHB clock = 48 MHz, MCLK/2 */
  #define  CLKRGM_CPU_HALF_SPEED_HCLK_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_HALF_SPEED_HCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ/ 1 )

  /* EBI1 clock = 48 MHz, MCLK/1 */
  #define  CLKRGM_CPU_HALF_SPEED_EBI1_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_HALF_SPEED_EBI1_KHZ          \
             ( CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ/ 1 )

  /* EBI2 clock = 24 MHz, MCLK/2 */
  #define  CLKRGM_CPU_HALF_SPEED_EBI2_DIV          CLKRGM_BUS_IS_MCLK_DIV2
  #define  CLKRGM_CPU_HALF_SPEED_EBI2_KHZ          \
             ( CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ/ 2 )

#elif defined FEATURE_PLL_384

  /* AHB clock = 64 MHz, MCLK/1 */
  #define  CLKRGM_CPU_HALF_SPEED_HCLK_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_HALF_SPEED_HCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ/ 1 )

  /* EBI1 clock = 64 MHz, MCLK/1 */
  #define  CLKRGM_CPU_HALF_SPEED_EBI1_DIV          CLKRGM_BUS_IS_MCLK_DIV1
  #define  CLKRGM_CPU_HALF_SPEED_EBI1_KHZ          \
             ( CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ/ 1 )

  /* EBI2 clock = 32 MHz, MCLK/2 */
  #define  CLKRGM_CPU_HALF_SPEED_EBI2_DIV          CLKRGM_BUS_IS_MCLK_DIV2
  #define  CLKRGM_CPU_HALF_SPEED_EBI2_KHZ          \
             ( CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ/ 2 )
#endif


  /* Minimum voltage needed for this CPU configuration */
  #define  CLKRGM_CPU_HALF_SPEED_MSMC_mV           MSMC_VOLTAGE_HIGH_V     


  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CPU_HALF_SPEED_CONFIG                             \
  {                                                                \
    {                                                              \
      CLKRGM_CPU_HALF_SPEED_CLK_SRC,                               \
      CLKRGM_CPU_HALF_SPEED_DIV_VAL,                               \
    },                                                             \
    {                                                              \
      CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ,                       \
      CLKRGM_CPU_HALF_SPEED_GLOBAL_FREQ_KHZ/1000                   \
    },                                                             \
    CLKRGM_CPU_HALF_SPEED_RS_VAL,                                  \
    {                                                              \
      CLKRGM_CPU_HALF_SPEED_MCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_HALF_SPEED_MCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_HALF_SPEED_HCLK_DIV,                                \
    {                                                              \
      CLKRGM_CPU_HALF_SPEED_HCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_HALF_SPEED_HCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_HALF_SPEED_EBI1_DIV,                                \
    {                                                              \
      CLKRGM_CPU_HALF_SPEED_EBI1_KHZ,                              \
      CLKRGM_CPU_HALF_SPEED_EBI1_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_HALF_SPEED_EBI2_DIV,                                \
    {                                                              \
      CLKRGM_CPU_HALF_SPEED_EBI2_KHZ,                              \
      CLKRGM_CPU_HALF_SPEED_EBI2_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_HALF_SPEED_MSMC_mV                                  \
  }



  /* ------------------------------------------------------------------------
     CPU FULL 1X speed configuration - ARM 96 MHz, EBI1 48MHz.
  ------------------------------------------------------------------------ */

  /* CPU clock = 96MHz, source PLL, div 2 */
  #define  CLKRGM_CPU_FULL_1X_SPEED_CLK_SRC           CLKRGM_MCLK_SRC_PLL
  #define  CLKRGM_CPU_FULL_1X_SPEED_DIV_VAL           CLKRGM_MCLK_DIV2
  #define  CLKRGM_CPU_FULL_1X_SPEED_GLOBAL_FREQ_KHZ   (CLKRGM_PLL_FREQ_KHZ/2)
  #define  CLKRGM_CPU_FULL_1X_SPEED_RS_VAL            CLKRGM_MCLK_RATESEL_DIV1
  #define  CLKRGM_CPU_FULL_1X_SPEED_MCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_FULL_1X_SPEED_GLOBAL_FREQ_KHZ / 1)

  /* AHB clock = 48 MHz, MCLK/2 */
  #define  CLKRGM_CPU_FULL_1X_SPEED_HCLK_DIV          CLKRGM_BUS_IS_MCLK_DIV2
  #define  CLKRGM_CPU_FULL_1X_SPEED_HCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_FULL_1X_SPEED_GLOBAL_FREQ_KHZ / 2  )

  /* EBI1 clock = 48 MHz, MCLK/2 */
  #define  CLKRGM_CPU_FULL_1X_SPEED_EBI1_DIV          CLKRGM_BUS_IS_MCLK_DIV2
  #define  CLKRGM_CPU_FULL_1X_SPEED_EBI1_KHZ          \
             ( CLKRGM_CPU_FULL_1X_SPEED_GLOBAL_FREQ_KHZ / 2  )

  /* EBI2 clock = 24 MHz, MCLK/4 */
  #define  CLKRGM_CPU_FULL_1X_SPEED_EBI2_DIV          CLKRGM_BUS_IS_MCLK_DIV4
  #define  CLKRGM_CPU_FULL_1X_SPEED_EBI2_KHZ          \
             ( CLKRGM_CPU_FULL_1X_SPEED_GLOBAL_FREQ_KHZ / 4  )

  /* Minimum voltage needed for this CPU configuration */
  #define  CLKRGM_CPU_FULL_1X_SPEED_MSMC_mV           MSMC_VOLTAGE_HIGH_V     


  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CPU_FULL_1X_SPEED_CONFIG                             \
  {                                                                \
    {                                                              \
      CLKRGM_CPU_FULL_1X_SPEED_CLK_SRC,                               \
      CLKRGM_CPU_FULL_1X_SPEED_DIV_VAL,                               \
    },                                                             \
    {                                                              \
      CLKRGM_CPU_FULL_1X_SPEED_GLOBAL_FREQ_KHZ,                         \
      CLKRGM_CPU_FULL_1X_SPEED_GLOBAL_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_FULL_1X_SPEED_RS_VAL,                                  \
    {                                                              \
      CLKRGM_CPU_FULL_1X_SPEED_MCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_FULL_1X_SPEED_MCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_FULL_1X_SPEED_HCLK_DIV,                                \
    {                                                              \
      CLKRGM_CPU_FULL_1X_SPEED_HCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_FULL_1X_SPEED_HCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_FULL_1X_SPEED_EBI1_DIV,                                \
    {                                                              \
      CLKRGM_CPU_FULL_1X_SPEED_EBI1_KHZ,                              \
      CLKRGM_CPU_FULL_1X_SPEED_EBI1_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_FULL_1X_SPEED_EBI2_DIV,                                \
    {                                                              \
      CLKRGM_CPU_FULL_1X_SPEED_EBI2_KHZ,                              \
      CLKRGM_CPU_FULL_1X_SPEED_EBI2_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_FULL_1X_SPEED_MSMC_mV                                  \
  }


/* ------------------------------------------------------------------------

     CPU FULL 128 speed configuration - ARM 128 MHz, EBI1 64MHz.

  ------------------------------------------------------------------------ */

 

  /* CPU clock = 128MHz, source PLL, div 3 */

  #define  CLKRGM_CPU_128_1X_SPEED_CLK_SRC           CLKRGM_MCLK_SRC_PLL
  #define  CLKRGM_CPU_128_1X_SPEED_DIV_VAL           CLKRGM_MCLK_DIV3
  #define  CLKRGM_CPU_128_1X_SPEED_GLOBAL_FREQ_KHZ   (CLKRGM_PLL_FREQ_KHZ/3)
  #define  CLKRGM_CPU_128_1X_SPEED_RS_VAL            CLKRGM_MCLK_RATESEL_DIV1

  #define  CLKRGM_CPU_128_1X_SPEED_MCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_128_1X_SPEED_GLOBAL_FREQ_KHZ / 1)

  /* AHB clock = 64 MHz, MCLK/2 */
  #define  CLKRGM_CPU_128_1X_SPEED_HCLK_DIV          CLKRGM_BUS_IS_MCLK_DIV2
  #define  CLKRGM_CPU_128_1X_SPEED_HCLK_FREQ_KHZ     \
             ( CLKRGM_CPU_128_1X_SPEED_GLOBAL_FREQ_KHZ / 2  )
  /* EBI1 clock = 64 MHz, MCLK/2 */

  #define  CLKRGM_CPU_128_1X_SPEED_EBI1_DIV          CLKRGM_BUS_IS_MCLK_DIV2
  #define  CLKRGM_CPU_128_1X_SPEED_EBI1_KHZ          \
             ( CLKRGM_CPU_128_1X_SPEED_GLOBAL_FREQ_KHZ / 2  )
  /* EBI2 clock = 32 MHz, MCLK/4 */

  #define  CLKRGM_CPU_128_1X_SPEED_EBI2_DIV          CLKRGM_BUS_IS_MCLK_DIV4
  #define  CLKRGM_CPU_128_1X_SPEED_EBI2_KHZ          \
             ( CLKRGM_CPU_128_1X_SPEED_GLOBAL_FREQ_KHZ / 4  )
  /* Minimum voltage needed for this CPU configuration */

  #define  CLKRGM_CPU_128_1X_SPEED_MSMC_mV           MSMC_VOLTAGE_HIGH_V     
  /* Populate the configuration structure with the selected values. */
 

  #define CLKRGM_CPU_128_1X_SPEED_CONFIG                           \
  {                                                                \
    {                                                              \
      CLKRGM_CPU_128_1X_SPEED_CLK_SRC,                             \
      CLKRGM_CPU_128_1X_SPEED_DIV_VAL,                             \
    },                                                             \
    {                                                              \
      CLKRGM_CPU_128_1X_SPEED_GLOBAL_FREQ_KHZ,                     \
      CLKRGM_CPU_128_1X_SPEED_GLOBAL_FREQ_KHZ/1000                 \
    },                                                             \
    CLKRGM_CPU_128_1X_SPEED_RS_VAL,                                \
    {                                                              \
      CLKRGM_CPU_128_1X_SPEED_MCLK_FREQ_KHZ,                       \
      CLKRGM_CPU_128_1X_SPEED_MCLK_FREQ_KHZ/1000                   \
    },                                                             \
    CLKRGM_CPU_128_1X_SPEED_HCLK_DIV,                              \
    {                                                              \
      CLKRGM_CPU_128_1X_SPEED_HCLK_FREQ_KHZ,                       \
      CLKRGM_CPU_128_1X_SPEED_HCLK_FREQ_KHZ/1000                   \
    },                                                             \
      CLKRGM_CPU_128_1X_SPEED_EBI1_DIV,                            \
    {                                                              \
      CLKRGM_CPU_128_1X_SPEED_EBI1_KHZ,                            \
      CLKRGM_CPU_128_1X_SPEED_EBI1_KHZ/1000                        \
    },                                                             \
    CLKRGM_CPU_128_1X_SPEED_EBI2_DIV,                              \
    {                                                              \
      CLKRGM_CPU_128_1X_SPEED_EBI2_KHZ,                            \
      CLKRGM_CPU_128_1X_SPEED_EBI2_KHZ/1000                        \
    },                                                             \
    CLKRGM_CPU_128_1X_SPEED_MSMC_mV                                \
  }


#ifdef FEATURE_CLKREGIM_2X_MODE 
  /* ------------------------------------------------------------------------
     CPU FULL 2X speed configuration - ARM 96 MHz, EBI1 96MHz, EBI1_IO 192MHz.
  ------------------------------------------------------------------------ */

  /* global = 192,  CPU clock = global / 2 = 96MHz */
  #define  CLKRGM_CPU_FULL_2X_SPEED_CLK_SRC           CLKRGM_MCLK_SRC_PLL
  #define  CLKRGM_CPU_FULL_2X_SPEED_DIV_VAL           CLKRGM_MCLK_DIV1
  #define  CLKRGM_CPU_FULL_2X_SPEED_GLOBAL_FREQ_KHZ   (CLKRGM_PLL_FREQ_KHZ/1)
  #define  CLKRGM_CPU_FULL_2X_SPEED_RS_VAL            CLKRGM_MCLK_RATESEL_DIV2
  #define  CLKRGM_CPU_FULL_2X_SPEED_MCLK_FREQ_KHZ     \
            (CLKRGM_CPU_FULL_2X_SPEED_GLOBAL_FREQ_KHZ / 2)

  /* AHB clock = 48 MHz, global/4 */
  #define  CLKRGM_CPU_FULL_2X_SPEED_HCLK_DIV          CLKRGM_BUS_IS_MCLK_DIV4
  #define  CLKRGM_CPU_FULL_2X_SPEED_HCLK_FREQ_KHZ     \
             (CLKRGM_CPU_FULL_2X_SPEED_GLOBAL_FREQ_KHZ / 4  )

  /* EBI1 clock = 96MHz, global/2 */
  #define  CLKRGM_CPU_FULL_2X_SPEED_EBI1_DIV          CLKRGM_BUS_IS_MCLK_DIV2
  #define  CLKRGM_CPU_FULL_2X_SPEED_EBI1_KHZ          \
             (CLKRGM_CPU_FULL_2X_SPEED_GLOBAL_FREQ_KHZ / 2  )

  /* EBI2 clock = 24 MHz, global/8 */
  #define  CLKRGM_CPU_FULL_2X_SPEED_EBI2_DIV          CLKRGM_BUS_IS_MCLK_DIV8
  #define  CLKRGM_CPU_FULL_2X_SPEED_EBI2_KHZ          \
             (CLKRGM_CPU_FULL_2X_SPEED_GLOBAL_FREQ_KHZ / 8)

  /* Minimum voltage needed for this CPU configuration */
  #define  CLKRGM_CPU_FULL_2X_SPEED_MSMC_mV           MSMC_VOLTAGE_HIGH_V     


  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CPU_FULL_2X_SPEED_CONFIG                             \
  {                                                                \
    {                                                              \
      CLKRGM_CPU_FULL_2X_SPEED_CLK_SRC,                               \
      CLKRGM_CPU_FULL_2X_SPEED_DIV_VAL,                               \
    },                                                             \
    {                                                              \
      CLKRGM_CPU_FULL_2X_SPEED_GLOBAL_FREQ_KHZ,                         \
      CLKRGM_CPU_FULL_2X_SPEED_GLOBAL_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_FULL_2X_SPEED_RS_VAL,                                  \
    {                                                              \
      CLKRGM_CPU_FULL_2X_SPEED_MCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_FULL_2X_SPEED_MCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_FULL_2X_SPEED_HCLK_DIV,                                \
    {                                                              \
      CLKRGM_CPU_FULL_2X_SPEED_HCLK_FREQ_KHZ,                         \
      CLKRGM_CPU_FULL_2X_SPEED_HCLK_FREQ_KHZ/1000                     \
    },                                                             \
    CLKRGM_CPU_FULL_2X_SPEED_EBI1_DIV,                                \
    {                                                              \
      CLKRGM_CPU_FULL_2X_SPEED_EBI1_KHZ,                              \
      CLKRGM_CPU_FULL_2X_SPEED_EBI1_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_FULL_2X_SPEED_EBI2_DIV,                                \
    {                                                              \
      CLKRGM_CPU_FULL_2X_SPEED_EBI2_KHZ,                              \
      CLKRGM_CPU_FULL_2X_SPEED_EBI2_KHZ/1000                          \
    },                                                             \
    CLKRGM_CPU_FULL_2X_SPEED_MSMC_mV                                  \
  }

#endif /* FEATURE_CLKREGIM_2X_MODE */

  /* ------------------------------------------------------------------------
     CPU clock regime configurations.

     The following defines the set of configurations that are available for
     this clock regime.  To add a configuration, add the corresponding item
     to the enum and add the corresponding configuration to the set of
     configurations that is defined below.
  ------------------------------------------------------------------------ */

  /* Set of configurations that are available for this clock regime. The
     following will be instantiated as the array of configurations that are
     available for this clock regime. */

#if defined FEATURE_PLL_192 && defined FEATURE_CLKREGIM_2X_MODE /* For 192 MHz and Higher builds*/
  #define CLKRGM_CPU_CONFIGS                 \
  {                                          \
    CLKRGM_CPU_SLP_SPEED_CONFIG,             \
    CLKRGM_CPU_XO_SPEED_CONFIG,              \
    CLKRGM_CPU_QUARTER_SPEED_CONFIG,         \
    CLKRGM_CPU_32P0_DIV1_CONFIG,             \
    CLKRGM_CPU_38P4_DIV1_CONFIG,             \
    CLKRGM_CPU_HALF_SPEED_CONFIG,            \
    CLKRGM_CPU_FULL_2X_SPEED_CONFIG          \
  }

#elif defined FEATURE_PLL_384 && defined FEATURE_CLKREGIM_1X_MODE /*For 384MHz and higher builds*/
  #define CLKRGM_CPU_CONFIGS                 \
  {                                          \
    CLKRGM_CPU_SLP_SPEED_CONFIG,             \
    CLKRGM_CPU_XO_SPEED_CONFIG,              \
    CLKRGM_CPU_QUARTER_SPEED_CONFIG,         \
    CLKRGM_CPU_32P0_DIV1_CONFIG,             \
    CLKRGM_CPU_38P4_DIV1_CONFIG,             \
    CLKRGM_CPU_HALF_SPEED_CONFIG,            \
    CLKRGM_CPU_FULL_1X_SPEED_CONFIG,         \
    CLKRGM_CPU_128_1X_SPEED_CONFIG           \
  }


#elif defined FEATURE_PLL_192 && defined FEATURE_CLKREGIM_1X_MODE /* For 192 and low builds */
  #define CLKRGM_CPU_CONFIGS                 \
  {                                          \
    CLKRGM_CPU_SLP_SPEED_CONFIG,             \
    CLKRGM_CPU_XO_SPEED_CONFIG,              \
    CLKRGM_CPU_QUARTER_SPEED_CONFIG,         \
    CLKRGM_CPU_32P0_DIV1_CONFIG,             \
    CLKRGM_CPU_38P4_DIV1_CONFIG,             \
    CLKRGM_CPU_HALF_SPEED_CONFIG,            \
    CLKRGM_CPU_FULL_1X_SPEED_CONFIG          \
  }

#endif /* FEATURE_CLKREGIM_2X_MODE  */



  /* ------------------------------------------------------------------------
     The following information is shared with the boot code to enable the
     determination of the external memory access timing parameters required
     for each supported HCLK and memory configuration.
  ------------------------------------------------------------------------ */
#ifdef FEATURE_CLKREGIM_2X_MODE 
  #define CLKRGM_CPU_FULL_SPEED_HCLK_DIV  CLKRGM_CPU_FULL_2X_SPEED_HCLK_DIV
  #define CLKRGM_HCLK_FREQUENCY        CLKRGM_CPU_FULL_2X_SPEED_HCLK_FREQ_KHZ 
#else
  #define CLKRGM_CPU_FULL_SPEED_HCLK_DIV  CLKRGM_CPU_FULL_1X_SPEED_HCLK_DIV
  #define CLKRGM_HCLK_FREQUENCY        CLKRGM_CPU_FULL_1X_SPEED_HCLK_FREQ_KHZ 
#endif
  #define CLKRGM_HCLK_FREQUENCY_IN_MHZ (CLKRGM_HCLK_FREQUENCY/1000)


/* --------------------------------------------------------------------------
   APPLICATION DSP CLOCK REGIME.

   This section provides the type definitions and supported configurations
   for this clock regime.  At compile time, the actual configuration to be
   used at runtime is determined based on the compile time options.  The
   configurations defined below conform to the clkrgm_type1_cfg_type type
   definition.
-------------------------------------------------------------------------- */
  #define CLKRGM_ADSP_MODE_DIV1 1
  #define CLKRGM_ADSP_MODE_DIV2 0

#if defined  FEATURE_PLL_192

      #define CLKRGM_ADSP_XO_CLK_SRC                  CLKRGM_TYPE1_SRC_IS_XO
      #define CLKRGM_ADSP_XO_CLK_DIV_REG_VAL          1
      #define CLKRGM_ADSP_XO_CLK_MODE_VAL             CLKRGM_ADSP_MODE_DIV1
      #define CLKRGM_ADSP_XO_CLK_FREQ_KHZ             (CLKRGM_XO_FREQ_KHZ/1)

      /* PLL / 5, div5 in the MND */
      #define CLKRGM_ADSP_38P4MHZ_CLK_SRC             CLKRGM_TYPE1_SRC_IS_PLL
      #define CLKRGM_ADSP_38P4MHZ_CLK_DIV_REG_VAL     5
      #define CLKRGM_ADSP_38P4MHZ_CLK_MODE_VAL        CLKRGM_ADSP_MODE_DIV1
      #define CLKRGM_ADSP_38P4MHZ_CLK_FREQ_KHZ        (CLKRGM_PLL_FREQ_KHZ/5)
      
      /* PLL / 4, div4 in the MND */
      #define CLKRGM_ADSP_48P0MHZ_CLK_SRC             CLKRGM_TYPE1_SRC_IS_PLL
      #define CLKRGM_ADSP_48P0MHZ_CLK_DIV_REG_VAL     4
      #define CLKRGM_ADSP_48P0MHZ_CLK_MODE_VAL        CLKRGM_ADSP_MODE_DIV1
      #define CLKRGM_ADSP_48P0MHZ_CLK_FREQ_KHZ        (CLKRGM_PLL_FREQ_KHZ/4)
  
      /* PLL / 3, div3 in the MND */
      #define CLKRGM_ADSP_64P0MHZ_CLK_SRC             CLKRGM_TYPE1_SRC_IS_PLL
      #define CLKRGM_ADSP_64P0MHZ_CLK_DIV_REG_VAL     3
      #define CLKRGM_ADSP_64P0MHZ_CLK_MODE_VAL        CLKRGM_ADSP_MODE_DIV1
      #define CLKRGM_ADSP_64P0MHZ_CLK_FREQ_KHZ        (CLKRGM_PLL_FREQ_KHZ/3)
 
      /* PLL / 2, div1 in the MND, div2 in the Mode */
      #define CLKRGM_ADSP_96P0MHZ_CLK_SRC             CLKRGM_TYPE1_SRC_IS_PLL
      #define CLKRGM_ADSP_96P0MHZ_CLK_DIV_REG_VAL     1
      #define CLKRGM_ADSP_96P0MHZ_CLK_MODE_VAL        CLKRGM_ADSP_MODE_DIV2
      #define CLKRGM_ADSP_96P0MHZ_CLK_FREQ_KHZ        (CLKRGM_PLL_FREQ_KHZ/2)

#elif defined FEATURE_PLL_384
     #define CLKRGM_ADSP_XO_CLK_SRC					CLKRGM_TYPE1_SRC_IS_XO
	 #define CLKRGM_ADSP_XO_CLK_DIV_REG_VAL 		 1
	 #define CLKRGM_ADSP_XO_CLK_MODE_VAL			 CLKRGM_ADSP_MODE_DIV1
	 #define CLKRGM_ADSP_XO_CLK_FREQ_KHZ			 (CLKRGM_XO_FREQ_KHZ/1)
  
	   /* PLL / 5, div5 in the MND */
	 #define CLKRGM_ADSP_38P4MHZ_CLK_SRC			 CLKRGM_TYPE1_SRC_IS_PLL
	 #define CLKRGM_ADSP_38P4MHZ_CLK_DIV_REG_VAL	 10
	 #define CLKRGM_ADSP_38P4MHZ_CLK_MODE_VAL		 CLKRGM_ADSP_MODE_DIV1
	 #define CLKRGM_ADSP_38P4MHZ_CLK_FREQ_KHZ		 (CLKRGM_PLL_FREQ_KHZ/5)
	   
	   /* PLL / 4, div4 in the MND */
	 #define CLKRGM_ADSP_48P0MHZ_CLK_SRC			 CLKRGM_TYPE1_SRC_IS_PLL
	 #define CLKRGM_ADSP_48P0MHZ_CLK_DIV_REG_VAL	 8
	 #define CLKRGM_ADSP_48P0MHZ_CLK_MODE_VAL		 CLKRGM_ADSP_MODE_DIV1
	 #define CLKRGM_ADSP_48P0MHZ_CLK_FREQ_KHZ		 (CLKRGM_PLL_FREQ_KHZ/4)
   
	   /* PLL / 3, div3 in the MND */
	 #define CLKRGM_ADSP_64P0MHZ_CLK_SRC			 CLKRGM_TYPE1_SRC_IS_PLL
	 #define CLKRGM_ADSP_64P0MHZ_CLK_DIV_REG_VAL	 6
	 #define CLKRGM_ADSP_64P0MHZ_CLK_MODE_VAL		 CLKRGM_ADSP_MODE_DIV1
	 #define CLKRGM_ADSP_64P0MHZ_CLK_FREQ_KHZ		 (CLKRGM_PLL_FREQ_KHZ/3)
  
	   /* PLL / 2, div1 in the MND, div2 in the Mode */
	 #define CLKRGM_ADSP_96P0MHZ_CLK_SRC			 CLKRGM_TYPE1_SRC_IS_PLL
	 #define CLKRGM_ADSP_96P0MHZ_CLK_DIV_REG_VAL	 2
	 #define CLKRGM_ADSP_96P0MHZ_CLK_MODE_VAL		 CLKRGM_ADSP_MODE_DIV2
	 #define CLKRGM_ADSP_96P0MHZ_CLK_FREQ_KHZ		 (CLKRGM_PLL_FREQ_KHZ/2)
	 
  #else 
    #error Not supported.
  #endif

  

  /* Populate the configuration structure with the selected values. */
  #define CLKRGM_ADSP_XO_CONFIG            \
    {                                      \
      CLKRGM_ADSP_XO_CLK_SRC,              \
      CLKRGM_ADSP_XO_CLK_DIV_REG_VAL,      \
      CLKRGM_ADSP_XO_CLK_MODE_VAL,         \
      CLKRGM_ADSP_XO_CLK_FREQ_KHZ          \
    }
  #define CLKRGM_ADSP_38P4MHZ_CONFIG          \
    {                                         \
      CLKRGM_ADSP_38P4MHZ_CLK_SRC,            \
      CLKRGM_ADSP_38P4MHZ_CLK_DIV_REG_VAL,    \
      CLKRGM_ADSP_38P4MHZ_CLK_MODE_VAL,       \
      CLKRGM_ADSP_38P4MHZ_CLK_FREQ_KHZ        \
    }
      
  #define CLKRGM_ADSP_48P0MHZ_CONFIG          \
    {                                         \
      CLKRGM_ADSP_48P0MHZ_CLK_SRC,            \
      CLKRGM_ADSP_48P0MHZ_CLK_DIV_REG_VAL,    \
      CLKRGM_ADSP_48P0MHZ_CLK_MODE_VAL,       \
      CLKRGM_ADSP_48P0MHZ_CLK_FREQ_KHZ        \
    }
  
  #define CLKRGM_ADSP_64P0MHZ_CONFIG          \
    {                                         \
      CLKRGM_ADSP_64P0MHZ_CLK_SRC,            \
      CLKRGM_ADSP_64P0MHZ_CLK_DIV_REG_VAL,    \
      CLKRGM_ADSP_64P0MHZ_CLK_MODE_VAL,       \
      CLKRGM_ADSP_64P0MHZ_CLK_FREQ_KHZ        \
    }
 
  #define CLKRGM_ADSP_96P0MHZ_CONFIG          \
    {                                         \
      CLKRGM_ADSP_96P0MHZ_CLK_SRC,            \
      CLKRGM_ADSP_96P0MHZ_CLK_DIV_REG_VAL,    \
      CLKRGM_ADSP_96P0MHZ_CLK_MODE_VAL,       \
      CLKRGM_ADSP_96P0MHZ_CLK_FREQ_KHZ        \
    }
 

  /* ------------------------------------------------------------------------
     ADSP clock regime configurations.

     The following defines the set of configurations that are available for
     this clock regime.  To add a configuration, add the corresponding item
     to the enum and add the corresponding configuration to the set of
     configurations that is defined below.
  ------------------------------------------------------------------------ */

  /* Enum of clock configurations that are supported. This enum is used
     as an array index for the configuration array and should match the
     order of the set of configurations defined below. */

  #ifndef _ARM_ASM_

  typedef enum
  {  
    CLKRGM_ADSP_TCXO_SPEED = 0,
    CLKRGM_ADSP_38P4MHZ_SPEED,
    CLKRGM_ADSP_48P0MHZ_SPEED,
    CLKRGM_ADSP_64P0MHZ_SPEED,
    CLKRGM_ADSP_96P0MHZ_SPEED
    /* for klocwork */
    #define CLKRGM_ADSP_NUM_SPEEDS (CLKRGM_ADSP_96P0MHZ_SPEED+1) 
  } clkrgm_adsp_speed_type;

  /* clk_regime_freq_plan_type
   * Specific clock frequencies for the case where only
   * one DCVS client has registered.
  */
  typedef struct 
  {
    clkrgm_cpu_speed_type          cpu_speed;
    int                            hclk_div;
    clkrgm_adsp_speed_type         adsp_speed;
    clkrgm_mdsp_clk_frequency_type mdsp_speed;
  } clk_regime_freq_plan_type;


  #endif // _ARM_ASM_

  #define CLKRGM_ADSP_SPEED_INIT  CLKRGM_ADSP_64P0MHZ_SPEED

  /* Set of configurations that are available for this clock regime. The
     following will be instantiated as the array of configurations that are
     available for this clock regime. */

  #define CLKRGM_ADSP_CONFIGS                \
  {                                          \
    CLKRGM_ADSP_XO_CONFIG,                   \
    CLKRGM_ADSP_38P4MHZ_CONFIG,              \
    CLKRGM_ADSP_48P0MHZ_CONFIG,              \
    CLKRGM_ADSP_64P0MHZ_CONFIG,              \
    CLKRGM_ADSP_96P0MHZ_CONFIG               \
  }
  /* clock enable/disable time is 3 cycles @ 19.2MHz */
  #define CLKRGM_ADSP_PAUSE 1

  #define CLKRGM_ADSP_MAX_SPEED_KHZ  96000

/* ------------------------------------------------------------------------
   USB 60MHz clock source
------------------------------------------------------------------------ */

#if defined   FEATURE_PLL_192

    #define CLKRGM_USB_60_MHZ_MN_REG_VAL   0x000005f4
    #define CLKRGM_USB_60_MHZ_CD_REG_VAL   0x000016ef

#elif defined FEATURE_PLL_384

    #define CLKRGM_USB_60_MHZ_MN_REG_VAL   0x000005e4
    #define CLKRGM_USB_60_MHZ_CD_REG_VAL   0x000016df
#endif


/* --------------------------------------------------------------------------
   INTERNAL CODEC CLOCK REGIME.

   This section provides the type definitions and supported configurations
   for this clock regime.  At compile time, the actual configuration to be
   used at runtime is determined based on the compile time options. The
   configurations defined below conform to the clkrgm_type1_cfg_type type
   definition.
-------------------------------------------------------------------------- */

  /* ------------------------------------------------------------------------
     CODEC 2.048 MHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

#if defined FEATURE_PLL_192
    /* Actual clock frequency of 4.096 MHz needed at the icodec_2x_rx_clk */
    /* icodec_2x_rx_clk = 192 MHz * 8/375 = 4.096 MHz */
    #define CLKRGM_CODEC_2P048_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_2P048_MHZ_MN_REG_VAL   0x0008fe90
    #define CLKRGM_CODEC_2P048_MHZ_CD_REG_VAL   0x0016fe88

#elif defined FEATURE_PLL_384
    /* Actual clock frequency of 4.096 MHz needed at the icodec_2x_rx_clk */
    /* icodec_2x_rx_clk = 384 MHz * 8/375 = 4.096 MHz This maths is not correct*/
    #define CLKRGM_CODEC_2P048_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_2P048_MHZ_MN_REG_VAL   0x0004fe8c
    #define CLKRGM_CODEC_2P048_MHZ_CD_REG_VAL   0x0016fe88
  #else
    #error Not supported.
  #endif

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CODEC_2P048_MHZ_CONFIG      \
  {                                          \
    CLKRGM_CODEC_2P048_MHZ_CLK_SRC,          \
    CLKRGM_CODEC_2P048_MHZ_MN_REG_VAL,       \
    CLKRGM_CODEC_2P048_MHZ_CD_REG_VAL        \
  }

  /* ------------------------------------------------------------------------
     CODEC 2.8224 MHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

#if defined  FEATURE_PLL_192


    /* Actual clock frequency of 4.096 MHz needed at the icodec_2x_rx_clk */
    /* icodec_2x_rx_clk = 192 MHz * 147/5000 = 5.6448 MHz */
    #define CLKRGM_CODEC_2P8224_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_2P8224_MHZ_MN_REG_VAL   0x0093ed0a
    #define CLKRGM_CODEC_2P8224_MHZ_CD_REG_VAL   0x0016ec77
#elif defined FEATURE_PLL_384
  /* Actual clock frequency of 4.096 MHz needed at the icodec_2x_rx_clk */
	  /* icodec_2x_rx_clk = 384 MHz * 147/(2*5000) = 5.6448 MHz This maths is not correct*/
    #define CLKRGM_CODEC_2P8224_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_2P8224_MHZ_MN_REG_VAL   0x0093d982
    #define CLKRGM_CODEC_2P8224_MHZ_CD_REG_VAL   0x0016d8ef

  #else
    #error Not supported.
  #endif

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CODEC_2P8224_MHZ_CONFIG     \
  {                                          \
    CLKRGM_CODEC_2P8224_MHZ_CLK_SRC,         \
    CLKRGM_CODEC_2P8224_MHZ_MN_REG_VAL,      \
    CLKRGM_CODEC_2P8224_MHZ_CD_REG_VAL       \
  }

  /* ------------------------------------------------------------------------
     CODEC 3.072 MHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

#if defined   FEATURE_PLL_192


    /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
    /* icodec_2x_rx_clk = 192 MHz * 4/125 = 6.144 MHz */
    #define CLKRGM_CODEC_3P072_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_3P072_MHZ_MN_REG_VAL   0x0004ff86
    #define CLKRGM_CODEC_3P072_MHZ_CD_REG_VAL   0x0016ff82
#elif defined FEATURE_PLL_384
  /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
	  /* icodec_2x_rx_clk = 384 MHz * 4/(2*125) = 6.144 MHz */
    #define CLKRGM_CODEC_3P072_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_3P072_MHZ_MN_REG_VAL   0x0002ff84
    #define CLKRGM_CODEC_3P072_MHZ_CD_REG_VAL   0x0016ff82

  #else
    #error Not supported.
  #endif

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CODEC_3P072_MHZ_CONFIG      \
  {                                          \
    CLKRGM_CODEC_3P072_MHZ_CLK_SRC,          \
    CLKRGM_CODEC_3P072_MHZ_MN_REG_VAL,       \
    CLKRGM_CODEC_3P072_MHZ_CD_REG_VAL        \
  }

  /* ------------------------------------------------------------------------
     CODEC 4.096 MHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

#if defined FEATURE_PLL_192

    /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
    /* icodec_2x_rx_clk = 192 MHz * 16/375 = 8.192 MHz */
    #define CLKRGM_CODEC_4P096_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_4P096_MHZ_MN_REG_VAL   0x0010fe98
    #define CLKRGM_CODEC_4P096_MHZ_CD_REG_VAL   0x0016fe88

#elif defined FEATURE_PLL_384
  /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
	 /* icodec_2x_rx_clk = 384 MHz * 8/375 = 8.192 MHz */
   #define CLKRGM_CODEC_4P096_MHZ_CLK_SRC	   CLKRGM_TYPE1_SRC_IS_PLL
   #define CLKRGM_CODEC_4P096_MHZ_MN_REG_VAL   0x0008fe90
   #define CLKRGM_CODEC_4P096_MHZ_CD_REG_VAL   0x0016fe88


  #else
    #error Not supported.
  #endif

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CODEC_4P096_MHZ_CONFIG      \
  {                                          \
    CLKRGM_CODEC_4P096_MHZ_CLK_SRC,          \
    CLKRGM_CODEC_4P096_MHZ_MN_REG_VAL,       \
    CLKRGM_CODEC_4P096_MHZ_CD_REG_VAL        \
  }

  /* ------------------------------------------------------------------------
     CODEC 5.6448 MHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

#if defined FEATURE_PLL_192


    /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
    /* icodec_2x_rx_clk = 192 MHz * 147/2500 = 11.2896 MHz */
    #define CLKRGM_CODEC_5P6448_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_5P6448_MHZ_MN_REG_VAL   0x0093f6ce
    #define CLKRGM_CODEC_5P6448_MHZ_CD_REG_VAL   0x0016f63b
#elif defined FEATURE_PLL_384
  
  /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
  /* icodec_2x_rx_clk = 384 MHz * 147/(2*2500) = 11.2896 MHz */
#define CLKRGM_CODEC_5P6448_MHZ_CLK_SRC 	 CLKRGM_TYPE1_SRC_IS_PLL
#define CLKRGM_CODEC_5P6448_MHZ_MN_REG_VAL	 0x0093ed0a
#define CLKRGM_CODEC_5P6448_MHZ_CD_REG_VAL	 0x0016ec77

  #else
    #error Not supported.
  #endif

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CODEC_5P6448_MHZ_CONFIG     \
  {                                          \
    CLKRGM_CODEC_5P6448_MHZ_CLK_SRC,         \
    CLKRGM_CODEC_5P6448_MHZ_MN_REG_VAL,      \
    CLKRGM_CODEC_5P6448_MHZ_CD_REG_VAL       \
  }

  /* ------------------------------------------------------------------------
     CODEC 6.144 MHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

#if defined  FEATURE_PLL_192


    /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
    /* icodec_2x_rx_clk = 192 MHz * 8/125 = 12.288 MHz */
    #define CLKRGM_CODEC_6P144_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_6P144_MHZ_MN_REG_VAL   0x0008ff8a
    #define CLKRGM_CODEC_6P144_MHZ_CD_REG_VAL   0x0016ff82

#elif defined FEATURE_PLL_384
  /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
   /* icodec_2x_rx_clk = 384 MHz * 8/125 = 12.288 MHz */
 #define CLKRGM_CODEC_6P144_MHZ_CLK_SRC 	 CLKRGM_TYPE1_SRC_IS_PLL
 #define CLKRGM_CODEC_6P144_MHZ_MN_REG_VAL	 0x0004ff86
 #define CLKRGM_CODEC_6P144_MHZ_CD_REG_VAL	 0x0016ff82


  #else
    #error Not supported.
  #endif

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CODEC_6P144_MHZ_CONFIG      \
  {                                          \
    CLKRGM_CODEC_6P144_MHZ_CLK_SRC,          \
    CLKRGM_CODEC_6P144_MHZ_MN_REG_VAL,       \
    CLKRGM_CODEC_6P144_MHZ_CD_REG_VAL        \
  }

  /* ------------------------------------------------------------------------
     6.) CODEC 8.192 MHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

#if defined FEATURE_PLL_192

    /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
    /* icodec_2x_rx_clk = 192 MHz * 32/375 = 16.384 MHz */
    #define CLKRGM_CODEC_8P192_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_8P192_MHZ_MN_REG_VAL   0x0020fea8
    #define CLKRGM_CODEC_8P192_MHZ_CD_REG_VAL   0x0016fe88

#elif defined FEATURE_PLL_384
  /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
	  /* icodec_2x_rx_clk = 384 MHz * 32/375 = 16.384 MHz */
    #define CLKRGM_CODEC_8P192_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_8P192_MHZ_MN_REG_VAL   0x0010fe98
    #define CLKRGM_CODEC_8P192_MHZ_CD_REG_VAL   0x0016fe88

  #else
    #error Not supported.
  #endif

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CODEC_8P192_MHZ_CONFIG      \
  {                                          \
    CLKRGM_CODEC_8P192_MHZ_CLK_SRC,          \
    CLKRGM_CODEC_8P192_MHZ_MN_REG_VAL,       \
    CLKRGM_CODEC_8P192_MHZ_CD_REG_VAL        \
  }

  /* ------------------------------------------------------------------------
     CODEC 11.2896 MHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

#if defined  FEATURE_PLL_192
    /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
    /* icodec_2x_rx_clk = 192 MHz * 147/1250 = 22.5792 MHz */
    #define CLKRGM_CODEC_11P2896_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_11P2896_MHZ_MN_REG_VAL   0x0093fbb0
    #define CLKRGM_CODEC_11P2896_MHZ_CD_REG_VAL   0x0016fb1d

    /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
    /* icodec_2x_rx_clk = 192 MHz * 44/375 = 22.528 MHz */
    #define CLKRGM_CODEC_11P264_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_11P264_MHZ_MN_REG_VAL   0x002cfeb4
    #define CLKRGM_CODEC_11P264_MHZ_CD_REG_VAL   0x0016fe88

#elif defined FEATURE_PLL_384
  /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
	 /* icodec_2x_rx_clk = 384 MHz * 147/(2*1250) = 22.5792 MHz */
   #define CLKRGM_CODEC_11P2896_MHZ_CLK_SRC 	 CLKRGM_TYPE1_SRC_IS_PLL
   #define CLKRGM_CODEC_11P2896_MHZ_MN_REG_VAL	 0x0093f6ce
   #define CLKRGM_CODEC_11P2896_MHZ_CD_REG_VAL	 0x0016f63b
  
	 /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
	 /* icodec_2x_rx_clk = 384 MHz * 22/375 = 22.528 MHz */
   #define CLKRGM_CODEC_11P264_MHZ_CLK_SRC		CLKRGM_TYPE1_SRC_IS_PLL
   #define CLKRGM_CODEC_11P264_MHZ_MN_REG_VAL	0x0016fe9e
   #define CLKRGM_CODEC_11P264_MHZ_CD_REG_VAL	0x0016fe88


  #else
    #error Not supported.
  #endif

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CODEC_11P264_MHZ_CONFIG    \
  {                                          \
    CLKRGM_CODEC_11P264_MHZ_CLK_SRC,        \
    CLKRGM_CODEC_11P264_MHZ_MN_REG_VAL,     \
    CLKRGM_CODEC_11P264_MHZ_CD_REG_VAL      \
  }

  #define CLKRGM_CODEC_11P2896_MHZ_CONFIG    \
  {                                          \
    CLKRGM_CODEC_11P2896_MHZ_CLK_SRC,        \
    CLKRGM_CODEC_11P2896_MHZ_MN_REG_VAL,     \
    CLKRGM_CODEC_11P2896_MHZ_CD_REG_VAL      \
  }

  /* ------------------------------------------------------------------------
     CODEC 12.288 MHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

#if defined  FEATURE_PLL_192

    /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
    /* icodec_2x_rx_clk = 192 MHz * 16/125 = 24.576 MHz */
    #define CLKRGM_CODEC_12P288_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CODEC_12P288_MHZ_MN_REG_VAL   0x0010ff92
    #define CLKRGM_CODEC_12P288_MHZ_CD_REG_VAL   0x0016ff82

#elif defined FEATURE_PLL_384
  /* Actual clock frequency needs to be double at the icodec_2x_rx_clk */
	 /* icodec_2x_rx_clk = 384 MHz * 8/125 = 24.576 MHz */
   #define CLKRGM_CODEC_12P288_MHZ_CLK_SRC		CLKRGM_TYPE1_SRC_IS_PLL
   #define CLKRGM_CODEC_12P288_MHZ_MN_REG_VAL	0x0008ff8a
   #define CLKRGM_CODEC_12P288_MHZ_CD_REG_VAL	0x0016ff82

  #else
    #error Not supported.
  #endif

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_CODEC_12P288_MHZ_CONFIG     \
  {                                          \
    CLKRGM_CODEC_12P288_MHZ_CLK_SRC,         \
    CLKRGM_CODEC_12P288_MHZ_MN_REG_VAL,      \
    CLKRGM_CODEC_12P288_MHZ_CD_REG_VAL       \
  }

  /* ------------------------------------------------------------------------
     CODEC clock regime configurations.

     The following defines the set of configurations that are available for
     this clock regime.  To add a configuration, add the corresponding item
     to the enum and add the corresponding configuration to the set of
     configurations that is defined below.
  ------------------------------------------------------------------------ */

  /* Set of configurations that are available for this clock regime. The
     following will be instantiated as the array of configurations that are
     available for this clock regime. */

  /* NOTE:  The following order must match the order of the items in the
            clkrgm_codec_clk_type enum as these are used as array indices. */

  #define CLKRGM_ICODEC_CONFIGS               \
  {                                           \
    CLKRGM_CODEC_2P048_MHZ_CONFIG,           \
    CLKRGM_CODEC_2P8224_MHZ_CONFIG,          \
    CLKRGM_CODEC_3P072_MHZ_CONFIG,           \
    CLKRGM_CODEC_4P096_MHZ_CONFIG,           \
    CLKRGM_CODEC_5P6448_MHZ_CONFIG,          \
    CLKRGM_CODEC_6P144_MHZ_CONFIG,           \
    CLKRGM_CODEC_8P192_MHZ_CONFIG,           \
    CLKRGM_CODEC_11P264_MHZ_CONFIG,          \
    CLKRGM_CODEC_11P2896_MHZ_CONFIG,         \
    CLKRGM_CODEC_12P288_MHZ_CONFIG           \
  }


/* --------------------------------------------------------------------------
   EXTERNAL CODEC CLOCK REGIME.

   This section provides the type definitions and supported configurations
   for this clock regime.  At compile time, the actual configuration to be
   used at runtime is determined based on the compile time options. The
   configurations defined below conform to the clkrgm_type1_cfg_type type
   definition.
-------------------------------------------------------------------------- */


  /* ------------------------------------------------------------------------
     ECODEC configuration - 4.096 MHz.
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

#if defined  FEATURE_PLL_192


    #define CLKRGM_ECODEC_4P096_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    /* MND = 8 / 375 */
    #define CLKRGM_ECODEC_4P096_MHZ_MN_REG_VAL   0x0008fe90
    #define CLKRGM_ECODEC_4P096_MHZ_CD_REG_VAL   0x102afe88

    #define CLKRGM_ECODEC_2P048_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    /* MND = 4 / 375 */
    #define CLKRGM_ECODEC_2P048_MHZ_MN_REG_VAL   0x0004fe8c
    #define CLKRGM_ECODEC_2P048_MHZ_CD_REG_VAL   0x102afe88

    #define CLKRGM_ECODEC_0P256_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    /* MND = 1 / 750 */
    #define CLKRGM_ECODEC_0P256_MHZ_MN_REG_VAL   0x0001fd12
    #define CLKRGM_ECODEC_0P256_MHZ_CD_REG_VAL   0x102afd11

    #define CLKRGM_ECODEC_0P128_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    /* MND = 1 / 1500 */
    #define CLKRGM_ECODEC_0P128_MHZ_MN_REG_VAL   0x0001fa24
    #define CLKRGM_ECODEC_0P128_MHZ_CD_REG_VAL   0x102afa23

#elif defined FEATURE_PLL_384

    #define CLKRGM_ECODEC_4P096_MHZ_CLK_SRC 	 CLKRGM_TYPE1_SRC_IS_PLL
	 /* MND = 8 / 375 */
    #define CLKRGM_ECODEC_4P096_MHZ_MN_REG_VAL	0x0004fe8c
    #define CLKRGM_ECODEC_4P096_MHZ_CD_REG_VAL	0x102afe88
  
    #define CLKRGM_ECODEC_2P048_MHZ_CLK_SRC		CLKRGM_TYPE1_SRC_IS_PLL
	 /* MND = 4 / 375 */
    #define CLKRGM_ECODEC_2P048_MHZ_MN_REG_VAL	0x0002fe8a
    #define CLKRGM_ECODEC_2P048_MHZ_CD_REG_VAL	0x102afe88
  
    #define CLKRGM_ECODEC_0P256_MHZ_CLK_SRC		CLKRGM_TYPE1_SRC_IS_PLL
	 /* MND = 1 / 750 */
    #define CLKRGM_ECODEC_0P256_MHZ_MN_REG_VAL	0x0001fa24
    #define CLKRGM_ECODEC_0P256_MHZ_CD_REG_VAL	0x102afa23
  
    #define CLKRGM_ECODEC_0P128_MHZ_CLK_SRC		CLKRGM_TYPE1_SRC_IS_PLL
	 /* MND = 1 / 1500 */
    #define CLKRGM_ECODEC_0P128_MHZ_MN_REG_VAL	0x0001f448
    #define CLKRGM_ECODEC_0P128_MHZ_CD_REG_VAL	0x102af447


  #else
    #error Not supported.
  #endif

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_ECODEC_4P096_MHZ_CONFIG               \
  {                                          \
    CLKRGM_ECODEC_4P096_MHZ_CLK_SRC,         \
    CLKRGM_ECODEC_4P096_MHZ_MN_REG_VAL,      \
    CLKRGM_ECODEC_4P096_MHZ_CD_REG_VAL       \
  }

  #define CLKRGM_ECODEC_2P048_MHZ_CONFIG               \
  {                                          \
    CLKRGM_ECODEC_2P048_MHZ_CLK_SRC,         \
    CLKRGM_ECODEC_2P048_MHZ_MN_REG_VAL,      \
    CLKRGM_ECODEC_2P048_MHZ_CD_REG_VAL       \
  }

  #define CLKRGM_ECODEC_0P256_MHZ_CONFIG               \
  {                                          \
    CLKRGM_ECODEC_0P256_MHZ_CLK_SRC,         \
    CLKRGM_ECODEC_0P256_MHZ_MN_REG_VAL,      \
    CLKRGM_ECODEC_0P256_MHZ_CD_REG_VAL       \
  }

  #define CLKRGM_ECODEC_0P128_MHZ_CONFIG               \
  {                                          \
    CLKRGM_ECODEC_0P128_MHZ_CLK_SRC,         \
    CLKRGM_ECODEC_0P128_MHZ_MN_REG_VAL,      \
    CLKRGM_ECODEC_0P128_MHZ_CD_REG_VAL       \
  }

  #define CLKRGM_ECODEC_CONFIG \
  { \
    CLKRGM_ECODEC_0P128_MHZ_CONFIG,               \
    CLKRGM_ECODEC_0P256_MHZ_CONFIG,               \
    CLKRGM_ECODEC_2P048_MHZ_CONFIG,               \
    CLKRGM_ECODEC_4P096_MHZ_CONFIG                \
  }



/* --------------------------------------------------------------------------
   STEREO DAC CLOCK REGIME.

   This section provides the type definitions and supported configurations
   for this clock regime.  At compile time, the actual configuration to be
   used at runtime is determined based on the compile time options. The
   configurations defined below conform to the clkrgm_type1_cfg_type type
   definition.
-------------------------------------------------------------------------- */

  /* ------------------------------------------------------------------------
     SDAC 256.0 KHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */
  /* frequency = 19.2 MHz * 640/48000 = 0.256 MHz */
  #define CLKRGM_SDAC_256_KHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDAC_256_KHZ_MN_REG_VAL   0x028046ff
  #define CLKRGM_SDAC_256_KHZ_CD_REG_VAL   0x0029447f

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_SDAC_256_KHZ_CONFIG      \
  {                                       \
    CLKRGM_SDAC_256_KHZ_CLK_SRC,          \
    CLKRGM_SDAC_256_KHZ_MN_REG_VAL,       \
    CLKRGM_SDAC_256_KHZ_CD_REG_VAL        \
  }

  /* ------------------------------------------------------------------------
     SDAC 352.8 KHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */
  /* frequency = 19.2 MHz * 882/48000 = 0.3528 MHz */
  #define CLKRGM_SDAC_352P8_KHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDAC_352P8_KHZ_MN_REG_VAL   0x037247f1
  #define CLKRGM_SDAC_352P8_KHZ_CD_REG_VAL   0x0029447f

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_SDAC_352P8_KHZ_CONFIG     \
  {                                        \
    CLKRGM_SDAC_352P8_KHZ_CLK_SRC,         \
    CLKRGM_SDAC_352P8_KHZ_MN_REG_VAL,      \
    CLKRGM_SDAC_352P8_KHZ_CD_REG_VAL       \
  }

  /* ------------------------------------------------------------------------
     SDAC 384.0 KHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */
  /* frequency = 19.2 MHz * 960/48000 = 0.384 MHz */
  #define CLKRGM_SDAC_384_KHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDAC_384_KHZ_MN_REG_VAL   0x03c0483f
  #define CLKRGM_SDAC_384_KHZ_CD_REG_VAL   0x0029447f

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_SDAC_384_KHZ_CONFIG      \
  {                                       \
    CLKRGM_SDAC_384_KHZ_CLK_SRC,          \
    CLKRGM_SDAC_384_KHZ_MN_REG_VAL,       \
    CLKRGM_SDAC_384_KHZ_CD_REG_VAL        \
  }

  /* ------------------------------------------------------------------------
     SDAC 512.0 KHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */
  /* frequency = 19.2 MHz * 1280/48000 = 0.512 MHz */
  #define CLKRGM_SDAC_512_KHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDAC_512_KHZ_MN_REG_VAL   0x0500497f
  #define CLKRGM_SDAC_512_KHZ_CD_REG_VAL   0x0029447f

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_SDAC_512_KHZ_CONFIG      \
  {                                       \
    CLKRGM_SDAC_512_KHZ_CLK_SRC,          \
    CLKRGM_SDAC_512_KHZ_MN_REG_VAL,       \
    CLKRGM_SDAC_512_KHZ_CD_REG_VAL        \
  }

  /* ------------------------------------------------------------------------
     SDAC 705.6 KHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */
  /* frequency = 19.2 MHz * 1764/48000 = 0.7056 MHz */
  #define CLKRGM_SDAC_705P6_KHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDAC_705P6_KHZ_MN_REG_VAL   0x06e44b63
  #define CLKRGM_SDAC_705P6_KHZ_CD_REG_VAL   0x0029447f

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_SDAC_705P6_KHZ_CONFIG     \
  {                                        \
    CLKRGM_SDAC_705P6_KHZ_CLK_SRC,         \
    CLKRGM_SDAC_705P6_KHZ_MN_REG_VAL,      \
    CLKRGM_SDAC_705P6_KHZ_CD_REG_VAL       \
  }

  /* ------------------------------------------------------------------------
     SDAC 768 KHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */
  /* frequency = 19.2 MHz *  1920/48000 = 0.768 MHz */
  #define CLKRGM_SDAC_768_KHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDAC_768_KHZ_MN_REG_VAL   0x07804bff
  #define CLKRGM_SDAC_768_KHZ_CD_REG_VAL   0x0029447f

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_SDAC_768_KHZ_CONFIG      \
  {                                       \
    CLKRGM_SDAC_768_KHZ_CLK_SRC,          \
    CLKRGM_SDAC_768_KHZ_MN_REG_VAL,       \
    CLKRGM_SDAC_768_KHZ_CD_REG_VAL        \
  }

  /* ------------------------------------------------------------------------
     SDAC 1024 KHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */
  /* frequency = 19.2 MHz *  2560/48000 = 1.024 MHz */
  #define CLKRGM_SDAC_1024_KHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDAC_1024_KHZ_MN_REG_VAL   0x0a004e7f
  #define CLKRGM_SDAC_1024_KHZ_CD_REG_VAL   0x0029447f

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_SDAC_1024_KHZ_CONFIG      \
  {                                        \
    CLKRGM_SDAC_1024_KHZ_CLK_SRC,          \
    CLKRGM_SDAC_1024_KHZ_MN_REG_VAL,       \
    CLKRGM_SDAC_1024_KHZ_CD_REG_VAL        \
  }

  /* ------------------------------------------------------------------------
     SDAC 1411.2 KHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */
  /* frequency = 19.2 MHz *  3528/48000 = 1.4112 MHz */
  #define CLKRGM_SDAC_1411P2_KHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDAC_1411P2_KHZ_MN_REG_VAL   0x0dc85247
  #define CLKRGM_SDAC_1411P2_KHZ_CD_REG_VAL   0x0029447f

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_SDAC_1411P2_KHZ_CONFIG    \
  {                                        \
    CLKRGM_SDAC_1411P2_KHZ_CLK_SRC,        \
    CLKRGM_SDAC_1411P2_KHZ_MN_REG_VAL,     \
    CLKRGM_SDAC_1411P2_KHZ_CD_REG_VAL      \
  }

  /* ------------------------------------------------------------------------
     SDAC 1536.0 KHz configuration
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */
  /* frequency = 19.2 MHz *  3840/48000 = 1.536 MHz */
  #define CLKRGM_SDAC_1536_KHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDAC_1536_KHZ_MN_REG_VAL   0x0f00537f
  #define CLKRGM_SDAC_1536_KHZ_CD_REG_VAL   0x0029447f

  /* Populate the configuration structure with the selected values. */

  #define CLKRGM_SDAC_1536_KHZ_CONFIG     \
  {                                       \
    CLKRGM_SDAC_1536_KHZ_CLK_SRC,         \
    CLKRGM_SDAC_1536_KHZ_MN_REG_VAL,      \
    CLKRGM_SDAC_1536_KHZ_CD_REG_VAL       \
  }

  /* ------------------------------------------------------------------------
     SDAC clock regime configurations.

     The following defines the set of configurations that are available for
     this clock regime.  To add a configuration, add the corresponding item
     to the enum and add the corresponding configuration to the set of
     configurations that is defined below.
  ------------------------------------------------------------------------ */

  /* Set of configurations that are available for this clock regime. The
     following will be instantiated as the array of configurations that are
     available for this clock regime. */

  /* NOTE:  The following order must match the order of the items in the
            clkrgm_sdac_clk_type enum as these are used as array indices. */

  #define CLKRGM_SDAC_CONFIGS             \
  {                                       \
    CLKRGM_SDAC_256_KHZ_CONFIG,           \
    CLKRGM_SDAC_352P8_KHZ_CONFIG,         \
    CLKRGM_SDAC_384_KHZ_CONFIG,           \
    CLKRGM_SDAC_512_KHZ_CONFIG,           \
    CLKRGM_SDAC_705P6_KHZ_CONFIG,         \
    CLKRGM_SDAC_768_KHZ_CONFIG,           \
    CLKRGM_SDAC_1024_KHZ_CONFIG,          \
    CLKRGM_SDAC_1411P2_KHZ_CONFIG,        \
    CLKRGM_SDAC_1536_KHZ_CONFIG           \
  }

  /* ------------------------------------------------------------------------
     UART_DM clock configuration 
  ------------------------------------------------------------------------ */

  #define CLKRGM_UART_DM_XO_CLK_SRC        CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_UART_DM_XO_MN_REG_VAL     0x00000000
  #define CLKRGM_UART_DM_XO_CD_REG_VAL     0x00010000

  #define CLKRGM_UART_DM_XO_CONFIG    \
  {                                   \
    CLKRGM_UART_DM_XO_CLK_SRC,        \
    CLKRGM_UART_DM_XO_MN_REG_VAL,     \
    CLKRGM_UART_DM_XO_CD_REG_VAL      \
  }
  
#if defined  FEATURE_PLL_192

  /* M/N = 24/625 * 192 = 7.3728 MHz */
  #define CLKRGM_UART_DM_7P3728_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_UART_DM_7P3728_MHZ_MN_REG_VAL   0x0018FDA6
  #define CLKRGM_UART_DM_7P3728_MHZ_CD_REG_VAL   0x0016FD8E
  #define CLKRGM_UART_DM_7P3728_MHZ_CONFIG \
  {                  \
      CLKRGM_UART_DM_7P3728_MHZ_CLK_SRC,    \
      CLKRGM_UART_DM_7P3728_MHZ_MN_REG_VAL,  \
      CLKRGM_UART_DM_7P3728_MHZ_CD_REG_VAL  \
  }

  /* M/N = 48/625 * 192 = 14.7456 MHz */
  #define CLKRGM_UART_DM_14P7456_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_UART_DM_14P7456_MHZ_MN_REG_VAL   0x0030fdbe
  #define CLKRGM_UART_DM_14P7456_MHZ_CD_REG_VAL   0x0016fd8e
  #define    CLKRGM_UART_DM_14P7456_MHZ_CONFIG\
  {                  \
      CLKRGM_UART_DM_14P7456_MHZ_CLK_SRC,    \
      CLKRGM_UART_DM_14P7456_MHZ_MN_REG_VAL,  \
      CLKRGM_UART_DM_14P7456_MHZ_CD_REG_VAL  \
  }

  /* M/N = 4/15 * 192 = 51.2 MHz */
  #define CLKRGM_UART_DM_51P20_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_UART_DM_51P20_MHZ_MN_REG_VAL   0x0004fff4
  #define CLKRGM_UART_DM_51P20_MHZ_CD_REG_VAL   0x0016fff0
  #define    CLKRGM_UART_DM_51P20_MHZ_CONFIG  \
  {                  \
      CLKRGM_UART_DM_51P20_MHZ_CLK_SRC,    \
      CLKRGM_UART_DM_51P20_MHZ_MN_REG_VAL,  \
      CLKRGM_UART_DM_51P20_MHZ_CD_REG_VAL  \
  }

  /* M/N = 192/625 * 192 = 58.9824 MHz */
  #define CLKRGM_UART_DM_58P9824_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_UART_DM_58P9824_MHZ_MN_REG_VAL   0x00c0fe4e
  #define CLKRGM_UART_DM_58P9824_MHZ_CD_REG_VAL   0x0016fd8e
  #define    CLKRGM_UART_DM_58P9824_MHZ_CONFIG\
  {                  \
      CLKRGM_UART_DM_58P9824_MHZ_CLK_SRC,    \
      CLKRGM_UART_DM_58P9824_MHZ_MN_REG_VAL,  \
      CLKRGM_UART_DM_58P9824_MHZ_CD_REG_VAL  \
  }
  /* ------------------------------------------------------------------------
     UART_DM clock configuration - 64MHz
  ------------------------------------------------------------------------ */

  /* M/N = 1/3 * 192 = 64 MHz */
  #define CLKRGM_UART_DM_64P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_UART_DM_64P0_MHZ_MN_REG_VAL   0x0001fffd
  #define CLKRGM_UART_DM_64P0_MHZ_CD_REG_VAL   0x0016fffc

#elif defined FEATURE_PLL_384 

/* M/N = 24/625 * 384 = 7.3728 MHz */
  #define CLKRGM_UART_DM_7P3728_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_UART_DM_7P3728_MHZ_MN_REG_VAL   0x000cfd9a /*0x0018FDA6*/
  #define CLKRGM_UART_DM_7P3728_MHZ_CD_REG_VAL   0x0016fd8e /*0x0016FD8E*/
  #define CLKRGM_UART_DM_7P3728_MHZ_CONFIG \
  {                  \
      CLKRGM_UART_DM_7P3728_MHZ_CLK_SRC,    \
      CLKRGM_UART_DM_7P3728_MHZ_MN_REG_VAL,  \
      CLKRGM_UART_DM_7P3728_MHZ_CD_REG_VAL  \
  }

  /* M/N = 48/625 * 384 = 14.7456 MHz */
  #define CLKRGM_UART_DM_14P7456_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_UART_DM_14P7456_MHZ_MN_REG_VAL   0x0018fda6 /*0x0030fdbe*/
  #define CLKRGM_UART_DM_14P7456_MHZ_CD_REG_VAL   0x0016fd8e /*0x0016fd8e*/
  #define    CLKRGM_UART_DM_14P7456_MHZ_CONFIG\
  {                  \
      CLKRGM_UART_DM_14P7456_MHZ_CLK_SRC,    \
      CLKRGM_UART_DM_14P7456_MHZ_MN_REG_VAL,  \
      CLKRGM_UART_DM_14P7456_MHZ_CD_REG_VAL  \
  }

  /* M/N = 4/15 * 384 = 51.2 MHz */
  #define CLKRGM_UART_DM_51P20_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_UART_DM_51P20_MHZ_MN_REG_VAL   0x0002fff2 /*0x0004fff4*/
  #define CLKRGM_UART_DM_51P20_MHZ_CD_REG_VAL   0x0016fff0 /*0x0016fff0*/
  #define    CLKRGM_UART_DM_51P20_MHZ_CONFIG  \
  {                  \
      CLKRGM_UART_DM_51P20_MHZ_CLK_SRC,    \
      CLKRGM_UART_DM_51P20_MHZ_MN_REG_VAL,  \
      CLKRGM_UART_DM_51P20_MHZ_CD_REG_VAL  \
  }

  /* M/N = 192/625 * 384 = 58.9824 MHz */
  #define CLKRGM_UART_DM_58P9824_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_UART_DM_58P9824_MHZ_MN_REG_VAL   0x0060fdee /*0x00c0fe4e*/
  #define CLKRGM_UART_DM_58P9824_MHZ_CD_REG_VAL   0x0016fd8e /*0x0016fd8e*/
  #define    CLKRGM_UART_DM_58P9824_MHZ_CONFIG\
  {                  \
      CLKRGM_UART_DM_58P9824_MHZ_CLK_SRC,    \
      CLKRGM_UART_DM_58P9824_MHZ_MN_REG_VAL,  \
      CLKRGM_UART_DM_58P9824_MHZ_CD_REG_VAL  \
  }
  /* ------------------------------------------------------------------------
     UART_DM clock configuration - 64MHz
  ------------------------------------------------------------------------ */

  /* M/N = 1/3 * 384 = 64 MHz */
  #define CLKRGM_UART_DM_64P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_UART_DM_64P0_MHZ_MN_REG_VAL   0x0001fffa /*0x0001fffd*/
  #define CLKRGM_UART_DM_64P0_MHZ_CD_REG_VAL   0x0196fff9 /*0x0016fffc*/
  
#endif

  #define CLKRGM_UART_DM_64P0_MHZ_CONFIG  \
  {                  \
    CLKRGM_UART_DM_64P0_MHZ_CLK_SRC,    \
    CLKRGM_UART_DM_64P0_MHZ_MN_REG_VAL,  \
    CLKRGM_UART_DM_64P0_MHZ_CD_REG_VAL  \
  }

  #define CLKRGM_UART_DM_CONFIGS       \
  {                                    \
    CLKRGM_UART_DM_XO_CONFIG,          \
    CLKRGM_UART_DM_7P3728_MHZ_CONFIG,  \
    CLKRGM_UART_DM_14P7456_MHZ_CONFIG, \
    CLKRGM_UART_DM_51P20_MHZ_CONFIG,   \
    CLKRGM_UART_DM_58P9824_MHZ_CONFIG, \
    CLKRGM_UART_DM_64P0_MHZ_CONFIG,    \
  }

  /* ------------------------------------------------------------------------
     GSBI0_UART clock configurations
  ------------------------------------------------------------------------ */
  /* ---- XO ---- */
  /* Clock source = XO, SIM_SRC = XO, MND bypassed */
  #define CLKRGM_GSBI0_UART_XO_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_GSBI0_UART_XO_MN_REG_VAL   0x00000000
  #define CLKRGM_GSBI0_UART_XO_CD_REG_VAL   0x04810000

  #define CLKRGM_GSBI0_UART_XO_CONFIG \
  {                                   \
    CLKRGM_GSBI0_UART_XO_CLK_SRC,     \
    CLKRGM_GSBI0_UART_XO_MN_REG_VAL,  \
    CLKRGM_GSBI0_UART_XO_CD_REG_VAL   \
  }

#if defined  FEATURE_PLL_192

  /* ---- 7.3728 MHz ---- */
  /* M/N = 24/625 * 192 = 7.3728 MHz */
  #define CLKRGM_GSBI0_UART_7P3728_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI0_UART_7P3728_MHZ_MN_REG_VAL   0x0018FDA6
  #define CLKRGM_GSBI0_UART_7P3728_MHZ_CD_REG_VAL   0x0116FD8E

  #define CLKRGM_GSBI0_UART_7P3728_MHZ_CONFIG    \
  {                                              \
      CLKRGM_GSBI0_UART_7P3728_MHZ_CLK_SRC,      \
      CLKRGM_GSBI0_UART_7P3728_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI0_UART_7P3728_MHZ_CD_REG_VAL    \
  }

  /* ---- 14.7456 MHz ---- */
  /* M/N = 48/625 * 192 = 14.7456 MHz */
  #define CLKRGM_GSBI0_UART_14P7456_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI0_UART_14P7456_MHZ_MN_REG_VAL   0x0030FDBE
  #define CLKRGM_GSBI0_UART_14P7456_MHZ_CD_REG_VAL   0x0316FD8E

  #define CLKRGM_GSBI0_UART_14P7456_MHZ_CONFIG    \
  {                                              \
      CLKRGM_GSBI0_UART_14P7456_MHZ_CLK_SRC,      \
      CLKRGM_GSBI0_UART_14P7456_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI0_UART_14P7456_MHZ_CD_REG_VAL    \
  }

  /* ---- 51.2 MHz ---- */
  /* M/N = 4/15 * 192 = 51.2 MHz */
  #define CLKRGM_GSBI0_UART_51P20_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI0_UART_51P20_MHZ_MN_REG_VAL   0x0004FFF4
  #define CLKRGM_GSBI0_UART_51P20_MHZ_CD_REG_VAL   0x0f16fff0

  #define CLKRGM_GSBI0_UART_51P20_MHZ_CONFIG    \
  {                                              \
      CLKRGM_GSBI0_UART_51P20_MHZ_CLK_SRC,      \
      CLKRGM_GSBI0_UART_51P20_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI0_UART_51P20_MHZ_CD_REG_VAL    \
  }

  /* ---- 58.9824 MHz ---- */
  /* M/N = 192/625 * 192 = 58.9824 MHz */
  #define CLKRGM_GSBI0_UART_58P9824_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI0_UART_58P9824_MHZ_MN_REG_VAL   0x00C0FE4E
  #define CLKRGM_GSBI0_UART_58P9824_MHZ_CD_REG_VAL   0x0F16FD8E

  #define CLKRGM_GSBI0_UART_58P9824_MHZ_CONFIG    \
  {                                              \
      CLKRGM_GSBI0_UART_58P9824_MHZ_CLK_SRC,      \
      CLKRGM_GSBI0_UART_58P9824_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI0_UART_58P9824_MHZ_CD_REG_VAL    \
  }

  /* ---- 64 MHz ---- */
  /* M/N = 1/3 * 384 = 64 MHz */
  #define CLKRGM_GSBI0_UART_64P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI0_UART_64P0_MHZ_MN_REG_VAL   0x0001FFFD
  #define CLKRGM_GSBI0_UART_64P0_MHZ_CD_REG_VAL   0x0F16FFFC

#elif  defined FEATURE_PLL_384
  /* ---- 7.3728 MHz ---- */
  /* M/N = 24/625 * 384 = 7.3728 MHz */
  #define CLKRGM_GSBI0_UART_7P3728_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI0_UART_7P3728_MHZ_MN_REG_VAL   0x000cfd9a /*0x0018FDA6*/
  #define CLKRGM_GSBI0_UART_7P3728_MHZ_CD_REG_VAL   0x0016fd8e /*0x0116FD8E */

  #define CLKRGM_GSBI0_UART_7P3728_MHZ_CONFIG    \
  {                                              \
      CLKRGM_GSBI0_UART_7P3728_MHZ_CLK_SRC,      \
      CLKRGM_GSBI0_UART_7P3728_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI0_UART_7P3728_MHZ_CD_REG_VAL    \
  }

  /* ---- 14.7456 MHz ---- */
  /* M/N = 48/625 * 384 = 14.7456 MHz */
  #define CLKRGM_GSBI0_UART_14P7456_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI0_UART_14P7456_MHZ_MN_REG_VAL   0x0018fda6 /*0x0030FDBE*/
  #define CLKRGM_GSBI0_UART_14P7456_MHZ_CD_REG_VAL   0x0016fd8e /*0x0316FD8E*/

  #define CLKRGM_GSBI0_UART_14P7456_MHZ_CONFIG    \
  {                                              \
      CLKRGM_GSBI0_UART_14P7456_MHZ_CLK_SRC,      \
      CLKRGM_GSBI0_UART_14P7456_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI0_UART_14P7456_MHZ_CD_REG_VAL    \
  }

  /* ---- 51.2 MHz ---- */
  /* M/N = 4/15 * 384 = 51.2 MHz */
  #define CLKRGM_GSBI0_UART_51P20_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI0_UART_51P20_MHZ_MN_REG_VAL   0x0002fff2 /*0x0004FFF4*/
  #define CLKRGM_GSBI0_UART_51P20_MHZ_CD_REG_VAL   0x0016fff0 /*0x0f16fff0*/

  #define CLKRGM_GSBI0_UART_51P20_MHZ_CONFIG    \
  {                                              \
      CLKRGM_GSBI0_UART_51P20_MHZ_CLK_SRC,      \
      CLKRGM_GSBI0_UART_51P20_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI0_UART_51P20_MHZ_CD_REG_VAL    \
  }

  /* ---- 58.9824 MHz ---- */
  /* M/N = 192/625 * 384 = 58.9824 MHz */
  #define CLKRGM_GSBI0_UART_58P9824_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI0_UART_58P9824_MHZ_MN_REG_VAL   0x0060fdee /*0x00C0FE4E*/
  #define CLKRGM_GSBI0_UART_58P9824_MHZ_CD_REG_VAL   0x0016fd8e /*0x0F16FD8E*/

  #define CLKRGM_GSBI0_UART_58P9824_MHZ_CONFIG    \
  {                                              \
      CLKRGM_GSBI0_UART_58P9824_MHZ_CLK_SRC,      \
      CLKRGM_GSBI0_UART_58P9824_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI0_UART_58P9824_MHZ_CD_REG_VAL    \
  }

  /* ---- 64 MHz ---- */
  /* M/N = 1/3 * 384 = 64 MHz */
  #define CLKRGM_GSBI0_UART_64P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI0_UART_64P0_MHZ_MN_REG_VAL   0x0001fffa /*0x0001FFFD*/
  #define CLKRGM_GSBI0_UART_64P0_MHZ_CD_REG_VAL   0x0196fff9 /*0x0F16FFFC*/

#endif

  #define CLKRGM_GSBI0_UART_64P0_MHZ_CONFIG    \
  {                                              \
      CLKRGM_GSBI0_UART_64P0_MHZ_CLK_SRC,      \
      CLKRGM_GSBI0_UART_64P0_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI0_UART_64P0_MHZ_CD_REG_VAL    \
  }

  #define CLKRGM_GSBI0_UART_CONFIGS      \
  {                                      \
    CLKRGM_GSBI0_UART_XO_CONFIG,         \
    CLKRGM_GSBI0_UART_7P3728_MHZ_CONFIG, \
    CLKRGM_GSBI0_UART_14P7456_MHZ_CONFIG, \
    CLKRGM_GSBI0_UART_51P20_MHZ_CONFIG,   \
    CLKRGM_GSBI0_UART_58P9824_MHZ_CONFIG, \
    CLKRGM_GSBI0_UART_64P0_MHZ_CONFIG    \
  }

  /* ------------------------------------------------------------------------
     GSBI1 OR 2 UART clock configurations
  ------------------------------------------------------------------------ */
  /* ---- XO ---- */
  /* Clock source = XO, SIM_SRC = XO, MND bypassed */
  #define CLKRGM_GSBI_1_2_UART_XO_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_GSBI_1_2_UART_XO_MN_REG_VAL   0x00000000
  #define CLKRGM_GSBI_1_2_UART_XO_CD_REG_VAL   0x00000100

  #define CLKRGM_GSBI_1_2_UART_XO_CONFIG \
  {                                      \
    CLKRGM_GSBI_1_2_UART_XO_CLK_SRC,     \
    CLKRGM_GSBI_1_2_UART_XO_MN_REG_VAL,  \
    CLKRGM_GSBI_1_2_UART_XO_CD_REG_VAL   \
  }

  /* ---- Approximately 7.3728 MHz (7.384615~ actual)  ---- */
  /* With a 8-bit MND, 24/625 is not possible, so this will be 
     M/N = 10/26 * 19.2 = 7.384615 MHz, which is within 0.2% */
  #define CLKRGM_GSBI_1_2_UART_7P3846_MHZ_CLK_SRC    CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_GSBI_1_2_UART_7P3846_MHZ_MN_REG_VAL 0x00000aef
  #define CLKRGM_GSBI_1_2_UART_7P3846_MHZ_CD_REG_VAL 0x000015e5

  #define CLKRGM_GSBI_1_2_UART_7P3846_MHZ_CONFIG    \
  {                                                \
      CLKRGM_GSBI_1_2_UART_7P3846_MHZ_CLK_SRC,      \
      CLKRGM_GSBI_1_2_UART_7P3846_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI_1_2_UART_7P3846_MHZ_CD_REG_VAL    \
  }

  /* ---- Approximately 14.7456 MHz (14.76923~ actual)  ---- */
  /* With a 8-bit MND, 48/625 is not possible, so this will be 
     M/N = 1/13 * 192 = 14.76923 MHz, which is within 0.2% */
  #define CLKRGM_GSBI_1_2_UART_14P7692_MHZ_CLK_SRC    CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI_1_2_UART_14P7692_MHZ_MN_REG_VAL 0x000001f3
  #define CLKRGM_GSBI_1_2_UART_14P7692_MHZ_CD_REG_VAL 0x000016f2

  #define CLKRGM_GSBI_1_2_UART_14P7692_MHZ_CONFIG    \
  {                                                \
      CLKRGM_GSBI_1_2_UART_14P7692_MHZ_CLK_SRC,      \
      CLKRGM_GSBI_1_2_UART_14P7692_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI_1_2_UART_14P7692_MHZ_CD_REG_VAL    \
  }

#if defined  FEATURE_PLL_192

  /* ---- 51.2 MHz ---- */
  /* M/N = 4/15 * 192 = 51.2 MHz */
  #define CLKRGM_GSBI_1_2_UART_51P20_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI_1_2_UART_51P20_MHZ_MN_REG_VAL   0x000004F4
  #define CLKRGM_GSBI_1_2_UART_51P20_MHZ_CD_REG_VAL   0x000016f0

  #define CLKRGM_GSBI_1_2_UART_51P20_MHZ_CONFIG    \
  {                                                \
      CLKRGM_GSBI_1_2_UART_51P20_MHZ_CLK_SRC,      \
      CLKRGM_GSBI_1_2_UART_51P20_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI_1_2_UART_51P20_MHZ_CD_REG_VAL    \
  }

  /* ---- Approximately 58.9824 MHz (59.0769~ actual)  ---- */
  /* With a 8-bit MND, 192/625 is not possible, so this will be 
     M/N = 4/13 * 192 = 59.0769 MHz, which is within 0.2% */
  #define CLKRGM_GSBI_1_2_UART_59P0769_MHZ_CLK_SRC    CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI_1_2_UART_59P0769_MHZ_MN_REG_VAL 0x000004f6
  #define CLKRGM_GSBI_1_2_UART_59P0769_MHZ_CD_REG_VAL 0x000016f2

  #define CLKRGM_GSBI_1_2_UART_59P0769_MHZ_CONFIG    \
  {                                                \
      CLKRGM_GSBI_1_2_UART_59P0769_MHZ_CLK_SRC,      \
      CLKRGM_GSBI_1_2_UART_59P0769_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI_1_2_UART_59P0769_MHZ_CD_REG_VAL    \
  }

  /* ---- 64 MHz ---- */
  /* M/N = 1/3 * 384 = 64 MHz */
  #define CLKRGM_GSBI_1_2_UART_64P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI_1_2_UART_64P0_MHZ_MN_REG_VAL   0x000001FA
  #define CLKRGM_GSBI_1_2_UART_64P0_MHZ_CD_REG_VAL   0x000016F9

#elif defined FEATURE_PLL_384
  /* ---- 51.2 MHz ---- */
  /* M/N = 4/15 * 384 = 51.2 MHz */
  #define CLKRGM_GSBI_1_2_UART_51P20_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI_1_2_UART_51P20_MHZ_MN_REG_VAL   0x0002fff2 /*0x000004F4*/
  #define CLKRGM_GSBI_1_2_UART_51P20_MHZ_CD_REG_VAL   0x0016fff0 /*0x000016f0*/

  #define CLKRGM_GSBI_1_2_UART_51P20_MHZ_CONFIG    \
  {                                                \
      CLKRGM_GSBI_1_2_UART_51P20_MHZ_CLK_SRC,      \
      CLKRGM_GSBI_1_2_UART_51P20_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI_1_2_UART_51P20_MHZ_CD_REG_VAL    \
  }

  /* ---- Approximately 58.9824 MHz (59.0769~ actual)  ---- */
  /* With a 8-bit MND, 192/625 is not possible, so this will be 
     M/N = 4/13 * 384 = 59.0769 MHz, which is within 0.2% */
  #define CLKRGM_GSBI_1_2_UART_59P0769_MHZ_CLK_SRC    CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI_1_2_UART_59P0769_MHZ_MN_REG_VAL 0x0060fdee /*0x000004f6*/
  #define CLKRGM_GSBI_1_2_UART_59P0769_MHZ_CD_REG_VAL 0x0016fd8e /*0x000016f2*/

  #define CLKRGM_GSBI_1_2_UART_59P0769_MHZ_CONFIG    \
  {                                                \
      CLKRGM_GSBI_1_2_UART_59P0769_MHZ_CLK_SRC,      \
      CLKRGM_GSBI_1_2_UART_59P0769_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI_1_2_UART_59P0769_MHZ_CD_REG_VAL    \
  }

  /* ---- 64 MHz ---- */
  /* M/N = 1/3 * 384 = 64 MHz */
  #define CLKRGM_GSBI_1_2_UART_64P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_GSBI_1_2_UART_64P0_MHZ_MN_REG_VAL   0x0001fffa /*0x000001FA*/
  #define CLKRGM_GSBI_1_2_UART_64P0_MHZ_CD_REG_VAL   0x0196fff9 /*0x000016F9*/

  #endif

  #define CLKRGM_GSBI_1_2_UART_64P0_MHZ_CONFIG    \
  {                                               \
      CLKRGM_GSBI_1_2_UART_64P0_MHZ_CLK_SRC,      \
      CLKRGM_GSBI_1_2_UART_64P0_MHZ_MN_REG_VAL,   \
      CLKRGM_GSBI_1_2_UART_64P0_MHZ_CD_REG_VAL    \
  }

  #define CLKRGM_GSBI_1_2_UART_CONFIGS        \
  {                                           \
    CLKRGM_GSBI_1_2_UART_XO_CONFIG,           \
    CLKRGM_GSBI_1_2_UART_7P3846_MHZ_CONFIG,   \
    CLKRGM_GSBI_1_2_UART_14P7692_MHZ_CONFIG,  \
    CLKRGM_GSBI_1_2_UART_51P20_MHZ_CONFIG,    \
    CLKRGM_GSBI_1_2_UART_59P0769_MHZ_CONFIG,  \
    CLKRGM_GSBI_1_2_UART_64P0_MHZ_CONFIG      \
  }


/* --------------------------------------------------------------------------
   CAMERA CLOCK REGIME.

   This section provides the type definitions and supported configurations
   for this clock regime.  At compile time, the actual configuration to be
   used at runtime is determined based on the compile time options.
-------------------------------------------------------------------------- */

  /* ------------------------------------------------------------------------
     Camera clock configuration - TCXO
  ------------------------------------------------------------------------ */

  #define CLKRGM_CAMCLK_XO_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
    #define CLKRGM_CAMCLK_XO_MN_REG_VAL   0x00000000
    #define CLKRGM_CAMCLK_XO_CD_REG_VAL   0x01810000							

  #define CLKRGM_CAMCLK_XO_CONFIG  \
  {                                  \
    CLKRGM_CAMCLK_XO_CLK_SRC,        \
    CLKRGM_CAMCLK_XO_MN_REG_VAL,     \
    CLKRGM_CAMCLK_XO_CD_REG_VAL      \
  }

#if defined  FEATURE_PLL_192

  /* ------------------------------------------------------------------------
     Camera clock configuration - 6.144 MHz
    ------------------------------------------------------------------------ */
  /* 6.144 = 192 * 4/125  */
  #define CLKRGM_CAMCLK_6P144_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_6P144_MHZ_MN_REG_VAL   0x0004ff86
    #define CLKRGM_CAMCLK_6P144_MHZ_CD_REG_VAL   0x0196ff82

  #define CLKRGM_CAMCLK_6P144_MHZ_CONFIG  \
  {                  \
    CLKRGM_CAMCLK_6P144_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_6P144_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_6P144_MHZ_CD_REG_VAL  \
  }

  /* ------------------------------------------------------------------------
     Camera clock configuration - 12.0 MHz
    ------------------------------------------------------------------------ */
  /* 12.0 = 192MHz / 16 */  
  #define CLKRGM_CAMCLK_12P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_CAMCLK_12P0_MHZ_MN_REG_VAL   0x0001fff0
  #define CLKRGM_CAMCLK_12P0_MHZ_CD_REG_VAL   0x0196ffef

  #define CLKRGM_CAMCLK_12P0_MHZ_CONFIG  \
  {                  \
    CLKRGM_CAMCLK_12P0_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_12P0_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_12P0_MHZ_CD_REG_VAL  \
  }
  /* ------------------------------------------------------------------------
     Camera clock configuration - 12.288 MHz
    ------------------------------------------------------------------------ */
  /* 12.288 = 192MHz * 8/125 */  
  #define CLKRGM_CAMCLK_12P288_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_CAMCLK_12P288_MHZ_MN_REG_VAL   0x0008ff8a
  #define CLKRGM_CAMCLK_12P288_MHZ_CD_REG_VAL   0x0196ff82

  #define CLKRGM_CAMCLK_12P288_MHZ_CONFIG  \
  {                  \
    CLKRGM_CAMCLK_12P288_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_12P288_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_12P288_MHZ_CD_REG_VAL  \
  }

  /* ------------------------------------------------------------------------
     Camera clock configuration - 24.576 MHz
  ------------------------------------------------------------------------ */
  /* 24.576 = 192MHz * 16/125 */  

  #define CLKRGM_CAMCLK_24P576_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_24P576_MHZ_MN_REG_VAL   0x0010ff92
    #define CLKRGM_CAMCLK_24P576_MHZ_CD_REG_VAL   0x0196ff82

  #define CLKRGM_CAMCLK_24P576_MHZ_CONFIG  \
  {                                \
    CLKRGM_CAMCLK_24P576_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_24P576_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_24P576_MHZ_CD_REG_VAL  \
  }
  /* ------------------------------------------------------------------------
     Camera clock configuration - 24.000 MHz
  ------------------------------------------------------------------------ */
  /* 24.0 = 192MHz * 1/8 */  

  #define CLKRGM_CAMCLK_24P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_24P0_MHZ_MN_REG_VAL   0x0001fff8
    #define CLKRGM_CAMCLK_24P0_MHZ_CD_REG_VAL   0x0196fff7

  #define CLKRGM_CAMCLK_24P0_MHZ_CONFIG  \
  {                                \
    CLKRGM_CAMCLK_24P0_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_24P0_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_24P0_MHZ_CD_REG_VAL  \
  }
  /* ------------------------------------------------------------------------
     Camera clock configuration - 48.000 MHz
  ------------------------------------------------------------------------ */
  /* 48.0 = 192MHz * 1/4 */  

  #define CLKRGM_CAMCLK_48P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_48P0_MHZ_MN_REG_VAL   0x0001fffc
    #define CLKRGM_CAMCLK_48P0_MHZ_CD_REG_VAL   0x0196fffb

  #define CLKRGM_CAMCLK_48P0_MHZ_CONFIG  \
  {                                \
    CLKRGM_CAMCLK_48P0_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_48P0_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_48P0_MHZ_CD_REG_VAL  \
  }
  /* ------------------------------------------------------------------------
     Camera clock configuration - 49.152 MHz
    ------------------------------------------------------------------------ */
  /* 49.152 = 192MHz * 32/125 */  
  #define CLKRGM_CAMCLK_49P152_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_49P152_MHZ_MN_REG_VAL   0x0020ffa2
    #define CLKRGM_CAMCLK_49P152_MHZ_CD_REG_VAL   0x0196ff82

  #define CLKRGM_CAMCLK_49P152_MHZ_CONFIG  \
  {                  \
    CLKRGM_CAMCLK_49P152_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_49P152_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_49P152_MHZ_CD_REG_VAL  \
  }
  
  /* ------------------------------------------------------------------------
     Camera clock configuration - Misc
    ------------------------------------------------------------------------ */
  /* Camera needs to support arbitrary frequencies, so the md and n values given
  here should be overwritten in the code.  For now we have these placeholders.
  */

  /* Currently, MND enabled, set for 192MHz * 1/4 = 64 MHz */
  /* It sounds as though one expects this to be overwritten elsewhere. */
  #define CLKRGM_CAMCLK_MISC_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_MISC_MN_REG_VAL   0x0001fffc
    #define CLKRGM_CAMCLK_MISC_CD_REG_VAL   0x0196fffb

  #define CLKRGM_CAMCLK_MISC_CONFIG  \
  {                  \
    CLKRGM_CAMCLK_MISC_CLK_SRC,    \
    CLKRGM_CAMCLK_MISC_MN_REG_VAL,  \
    CLKRGM_CAMCLK_MISC_CD_REG_VAL  \
  }
  
#elif defined FEATURE_PLL_384

  /* ------------------------------------------------------------------------
     Camera clock configuration - 6.144 MHz
    ------------------------------------------------------------------------ */
  /* 6.144 = 384 * 4/125  */
  #define CLKRGM_CAMCLK_6P144_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_6P144_MHZ_MN_REG_VAL   0x0002FF84 /*0x0004ff86 old for 192 recalculat*/
    #define CLKRGM_CAMCLK_6P144_MHZ_CD_REG_VAL   0x0016FF82 /*0x0196ff82 old for 192 recalculat*/

  #define CLKRGM_CAMCLK_6P144_MHZ_CONFIG  \
  {                  \
    CLKRGM_CAMCLK_6P144_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_6P144_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_6P144_MHZ_CD_REG_VAL  \
  }

  /* ------------------------------------------------------------------------
     Camera clock configuration - 12.0 MHz
    ------------------------------------------------------------------------ */
  /* 12.0 = 384 / 16 */  
  #define CLKRGM_CAMCLK_12P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_CAMCLK_12P0_MHZ_MN_REG_VAL   0x0001ffe0 /*0x0001fff0 old for 192*/
  #define CLKRGM_CAMCLK_12P0_MHZ_CD_REG_VAL   0x0196ffdf /*0x0196ffef old for 192*/

  #define CLKRGM_CAMCLK_12P0_MHZ_CONFIG  \
  {                  \
    CLKRGM_CAMCLK_12P0_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_12P0_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_12P0_MHZ_CD_REG_VAL  \
  }
  /* ------------------------------------------------------------------------
     Camera clock configuration - 12.288 MHz
    ------------------------------------------------------------------------ */
  /* 12.288 = 384 * 8/125 */  
  #define CLKRGM_CAMCLK_12P288_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
  #define CLKRGM_CAMCLK_12P288_MHZ_MN_REG_VAL   0x0004ff86 /*0x0008ff8a old for 192 recalculat*/
  #define CLKRGM_CAMCLK_12P288_MHZ_CD_REG_VAL   0x0016ff82 /*0x0196ff82 old for 192 recalculat*/

  #define CLKRGM_CAMCLK_12P288_MHZ_CONFIG  \
  {                  \
    CLKRGM_CAMCLK_12P288_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_12P288_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_12P288_MHZ_CD_REG_VAL  \
  }

  /* ------------------------------------------------------------------------
     Camera clock configuration - 24.576 MHz
  ------------------------------------------------------------------------ */
  /* 24.576 = 384 * 16/125 */  

  #define CLKRGM_CAMCLK_24P576_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_24P576_MHZ_MN_REG_VAL 0x0008ff8a  /*0x0010ff92 old for 192 recalculat*/
    #define CLKRGM_CAMCLK_24P576_MHZ_CD_REG_VAL 0x0016ff82  /*0x0196ff82 old for 192 recalculat*/

  #define CLKRGM_CAMCLK_24P576_MHZ_CONFIG  \
  {                                \
    CLKRGM_CAMCLK_24P576_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_24P576_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_24P576_MHZ_CD_REG_VAL  \
  }
  /* ------------------------------------------------------------------------
     Camera clock configuration - 24.000 MHz
  ------------------------------------------------------------------------ */
  /* 24.0 = 384 * 1/8 */  

  #define CLKRGM_CAMCLK_24P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_24P0_MHZ_MN_REG_VAL   0x0001fff0 /*0x0001fff8 old for 192 recalculat*/
    #define CLKRGM_CAMCLK_24P0_MHZ_CD_REG_VAL   0x0196ffef /*0x0196fff7 old for 192 recalculat*/

  #define CLKRGM_CAMCLK_24P0_MHZ_CONFIG  \
  {                                \
    CLKRGM_CAMCLK_24P0_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_24P0_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_24P0_MHZ_CD_REG_VAL  \
  }
  /* ------------------------------------------------------------------------
     Camera clock configuration - 48.000 MHz
  ------------------------------------------------------------------------ */
  /* 48.0 = 384 * 1/4 */  

  #define CLKRGM_CAMCLK_48P0_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_48P0_MHZ_MN_REG_VAL   0x0001fff8 /*0x0001fffc*/
    #define CLKRGM_CAMCLK_48P0_MHZ_CD_REG_VAL   0x0196fff7 /*0x0196fffb*/

  #define CLKRGM_CAMCLK_48P0_MHZ_CONFIG  \
  {                                \
    CLKRGM_CAMCLK_48P0_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_48P0_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_48P0_MHZ_CD_REG_VAL  \
  }
  /* ------------------------------------------------------------------------
     Camera clock configuration - 49.152 MHz
    ------------------------------------------------------------------------ */
  /* 49.152 = 384 * 32/125 */  
  #define CLKRGM_CAMCLK_49P152_MHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_49P152_MHZ_MN_REG_VAL   0x0010ff92 /*0x0020ffa2*/
    #define CLKRGM_CAMCLK_49P152_MHZ_CD_REG_VAL   0x0196ff82 /*0x0196ff82*/

  #define CLKRGM_CAMCLK_49P152_MHZ_CONFIG  \
  {                  \
    CLKRGM_CAMCLK_49P152_MHZ_CLK_SRC,    \
    CLKRGM_CAMCLK_49P152_MHZ_MN_REG_VAL,  \
    CLKRGM_CAMCLK_49P152_MHZ_CD_REG_VAL  \
  }
  
  /* ------------------------------------------------------------------------
     Camera clock configuration - Misc
    ------------------------------------------------------------------------ */
  /* Camera needs to support arbitrary frequencies, so the md and n values given
  here should be overwritten in the code.  For now we have these placeholders.
  */

  /* Currently, MND enabled, set for 192MHz * 1/4 = 64 MHz */
  /* It sounds as though one expects this to be overwritten elsewhere. */
  #define CLKRGM_CAMCLK_MISC_CLK_SRC      CLKRGM_TYPE1_SRC_IS_PLL
    #define CLKRGM_CAMCLK_MISC_MN_REG_VAL   0x0001fffa /*0x0001fffc*/
    #define CLKRGM_CAMCLK_MISC_CD_REG_VAL   0x0196fff9 /*0x0196fffb*/

  #define CLKRGM_CAMCLK_MISC_CONFIG  \
  {                  \
    CLKRGM_CAMCLK_MISC_CLK_SRC,    \
    CLKRGM_CAMCLK_MISC_MN_REG_VAL,  \
    CLKRGM_CAMCLK_MISC_CD_REG_VAL  \
  }
  #endif  
  /* ------------------------------------------------------------------------
     Camera clock configurations
  ------------------------------------------------------------------------ */

  #define CLKRGM_CAMCLK_CONFIGS       \
  {                                   \
    CLKRGM_CAMCLK_XO_CONFIG,          \
    CLKRGM_CAMCLK_6P144_MHZ_CONFIG,   \
    CLKRGM_CAMCLK_12P0_MHZ_CONFIG,    \
    CLKRGM_CAMCLK_12P288_MHZ_CONFIG,  \
    CLKRGM_CAMCLK_24P576_MHZ_CONFIG,  \
    CLKRGM_CAMCLK_24P0_MHZ_CONFIG,    \
    CLKRGM_CAMCLK_48P0_MHZ_CONFIG,    \
    CLKRGM_CAMCLK_49P152_MHZ_CONFIG,  \
    CLKRGM_CAMCLK_MISC_CONFIG         \
  }

/* --------------------------------------------------------------------------
   SDCC CLOCK REGIME.

   This section provides the type definitions and supported configurations
   for this clock regime.  At compile time, the actual configuration to be
   used at runtime is determined based on the compile time options.
-------------------------------------------------------------------------- */

  /* ------------------------------------------------------------------------
     SDCC configuration - TCXO, or 400kHz in slow mode
  ------------------------------------------------------------------------ */

  /* Select required register values based on specified PLL frequencies. */

  #define CLKRGM_SDCC_XO_CLK_SRC          CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDCC_XO_MN_REG_VAL       0x00000000
  #define CLKRGM_SDCC_XO_CD_REG_VAL       0x00010000

  /* M/N = 1/2 */
  #define CLKRGM_SDCC_9600KHZ_CLK_SRC     CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDCC_9600KHZ_MN_REG_VAL  0x0001fffe
  #define CLKRGM_SDCC_9600KHZ_CD_REG_VAL  0x0015fffd
 
  /* M/N = 1/4 */
  #define CLKRGM_SDCC_4800KHZ_CLK_SRC     CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDCC_4800KHZ_MN_REG_VAL  0x0001fffc 
  #define CLKRGM_SDCC_4800KHZ_CD_REG_VAL  0x0015fffb 
 
  /* M/N = 1/48 */
  #define CLKRGM_SDCC_400KHZ_CLK_SRC      CLKRGM_TYPE1_SRC_IS_XO
  #define CLKRGM_SDCC_400KHZ_MN_REG_VAL   0x0001ffd0
  #define CLKRGM_SDCC_400KHZ_CD_REG_VAL   0x0015ffcf


  /* ------------------------------------------------------------------------
     SDCC clock regime configurations.

     The following defines the set of configurations that are available for
     this clock regime.  To add a configuration, add the corresponding item
     to the enum and add the corresponding configuration to the set of
     configurations that is defined below.
  ------------------------------------------------------------------------ */

  /* Set of configurations that are available for this clock regime. The
     following will be instantiated as the array of configurations that are
     available for this clock regime. */

  /* NOTE:  The following order must match the order of the items in the
            clkrgm_sdcc_clk_type enum as these are used as array indices. */

  #define CLKRGM_SDCC_XO_CONFIG       \
  {                                   \
    CLKRGM_SDCC_XO_CLK_SRC,           \
    CLKRGM_SDCC_XO_MN_REG_VAL,        \
    CLKRGM_SDCC_XO_CD_REG_VAL         \
  }

  #define CLKRGM_SDCC_9600KHZ_CONFIG  \
  {                                   \
    CLKRGM_SDCC_9600KHZ_CLK_SRC,      \
    CLKRGM_SDCC_9600KHZ_MN_REG_VAL,   \
    CLKRGM_SDCC_9600KHZ_CD_REG_VAL    \
  }

  #define CLKRGM_SDCC_4800KHZ_CONFIG  \
  {                                   \
    CLKRGM_SDCC_4800KHZ_CLK_SRC,      \
    CLKRGM_SDCC_4800KHZ_MN_REG_VAL,   \
    CLKRGM_SDCC_4800KHZ_CD_REG_VAL    \
  }

  #define CLKRGM_SDCC_400KHZ_CONFIG   \
  {                                   \
    CLKRGM_SDCC_400KHZ_CLK_SRC,       \
    CLKRGM_SDCC_400KHZ_MN_REG_VAL,    \
    CLKRGM_SDCC_400KHZ_CD_REG_VAL     \
  }

  #define CLKRGM_SDCC_CONFIGS         \
  {                                   \
    CLKRGM_SDCC_XO_CONFIG,            \
    CLKRGM_SDCC_400KHZ_CONFIG,        \
    CLKRGM_SDCC_400KHZ_CONFIG,        \
    CLKRGM_SDCC_XO_CONFIG,            \
    CLKRGM_SDCC_9600KHZ_CONFIG,       \
    CLKRGM_SDCC_4800KHZ_CONFIG        \
  }


/* crr -- 10/31/06 need to add the following two configs  */
  /* UART_DM clock regime configuration structure array. 

  const clkrgm_msm_type1_cfg_type clkrgm_uartdm_cfg [ ] = CLKRGM_UARTDM_CONFIGS;
  */

#ifndef _ARM_ASM_
  #include CLK_RGMI_MODEM_H

/* --------------------------------------------------------------------------
   Clock regime driver state information structure.
-------------------------------------------------------------------------- */

  /* Clock regime state information structure type definition. */

  typedef struct
  {
    clkrgm_pll_state_type     pll_state;         /* PLL circuit state.    */
    clkrgm_rxf_state_type     rxf_state;         
    int                       pll_usage;         /* CLKs using PLL        */
    int                       rxf_usage;  /* count clients requiring RXF on/off */
    clkrgm_pll_freq_type      pll_freq;          /* PLL current freq cfg  */
    boolean rxf_clients[CLKRGM_RXF_MAX_CLIENTS];/* RXF clients */
    boolean pll_clients[CLK_RGM_MAX_REGIMES];   /* PLL clients */
    clkrgm_cpu_speed_type     cpu_speed;         /* CPU speed.              */
    uint16                    cpu_mv;            /* Core voltage in mv      */
    clkrgm_adsp_speed_type    adsp_speed;        /* ADSP speed.             */
    clkrgm_adsp_speed_type    adsp_pending_speed;/* ADSP pending speed.     */
    clkrgm_codec_clk_type     icodec_speed;      /* ICODEC frequency.        */
    clkrgm_ecodec_clk_type    ecodec_speed;      /* ECODEC frequency.        */
    clkrgm_sdac_clk_type      sdac_speed;        /* SDAC frequency.         */
    clkrgm_msm_type1_cfg_type gsbi[3],
                              uart_legacy,
                              i2c_legacy,
                              usb,
                              video_vfe,
                              camclk,
                              sdcc;
                           
  #ifdef FEATURE_CLKREGIM_RM
    clkrgm_rm_clk_info_type           rm_clk_info[CLK_RGM_MSM_NUM_REGIMES];
                                      /* Resource manager data */
  #endif
#ifndef BUILD_BOOT_CHAIN_OEMSBL
    uint64                            cpu_client_mask;
                                    /* Mask of active CPU resource clients */
    int                               cpu_client_usage;
                                    /* count registered clients */
    clkrgm_devman_client_type         devman_client;
                                    /* Internal device manager client */
#endif /* BUILD_BOOT_CHAIN_OEMSBL */
  } clkrgm_msm_state_type;

/*--------------------------------------------------------------------------
  Logging record (LOG_CLOCK_C)
---------------------------------------------------------------------------*/

typedef struct
{
  log_hdr_type  log_header;
  uint32        hw_rev_num;
  uint8         clock_domain;
  uint8         transaction_type;
  uint8         clock_num;
  uint8         status_reg_count;
  uint32        status_reg[3];
} clkrgm_msm_log_type;

typedef enum
{
  CLKRGM_MSM_LOG_NONE = 0,
  CLKRGM_MSM_LOG_EBI2_CLK,
  CLKRGM_MSM_LOG_CAMCLK_PO,
  CLKRGM_MSM_LOG_VIDEO_VFE_CLK,
  CLKRGM_MSM_LOG_VIDEO_HCLK,
  CLKRGM_MSM_LOG_EBI1_IO_CLK,
  CLKRGM_MSM_LOG_EBI2_IO_CLK,
  CLKRGM_MSM_LOG_SDRAMC_CLK,
  CLKRGM_MSM_LOG_ETM_CLK,
  CLKRGM_MSM_LOG_I2C_CLK,
  CLKRGM_MSM_LOG_I2S_CLK,
  CLKRGM_MSM_LOG_I2S_MCLK,
  CLKRGM_MSM_LOG_ECODECIF_PO_CLK,
  CLKRGM_MSM_LOG_ECODECIF_CLK,
  CLKRGM_MSM_LOG_ICODEC_2X_CLK,
  CLKRGM_MSM_LOG_ECODEC_CLK,
  CLKRGM_MSM_LOG_USB_MCLK,
  CLKRGM_MSM_LOG_USB_SCLK,
  CLKRGM_MSM_LOG_UART1_DM_CLK,
  CLKRGM_MSM_LOG_UART2_CLK,
  CLKRGM_MSM_LOG_TCXO4_CLK,
  CLKRGM_MSM_LOG_RFMS_XO_CLK,
  CLKRGM_MSM_LOG_ADSP_HM_CLK,
  CLKRGM_MSM_LOG_QMEMBIST_CLK,
  CLKRGM_MSM_LOG_SEC_HCLK,
  CLKRGM_MSM_LOG_MDP_HCLK,
  CLKRGM_MSM_LOG_SDCC_MCLK,
  CLKRGM_MSM_LOG_SDCC_HCLK,
  CLKRGM_MSM_LOG_USB_OTG_HCLK,
  CLKRGM_MSM_LOG_GSBI0_CLK,
  CLKRGM_MSM_LOG_GSBI1_CLK,
  CLKRGM_MSM_LOG_GSBI2_CLK,
  CLKRGM_MSM_LOG_SSBI_PMIC_CLK,
  CLKRGM_MSM_LOG_AUDIO_HCLK,
  CLKRGM_MSM_LOG_EFUSE_CLK,
  CLKRGM_MSM_LOG_IMEM_CLK
} clkrgm_msm_clk_log_type;

#define CLKRGM_MSM_LOG_DOMAIN               0

#define CLKRGM_MSM_LOG_TRANSACTION_NA       0
#define CLKRGM_MSM_LOG_TRANSACTION_ENABLE   1
#define CLKRGM_MSM_LOG_TRANSACTION_DISABLE  2
#define CLKRGM_MSM_LOG_STATUS_REG_COUNT     2

typedef struct
{
  log_hdr_type  log_header;
  uint32        hw_rev_num;
  uint8         clock_domain;
  uint8         transaction_type;
  uint8         clock_num;
  uint8         status_reg_count;
#ifdef FEATURE_CLKREGIM_DYNAMIC_VOLTAGE
  uint32        status_reg[6];
#else
  uint32        status_reg[5];
#endif
} clkrgm_dcvs_log_type;

typedef enum
{
  CLKRGM_DCVS_LOG_NONE = 0,
  CLKRGM_DCVS_LOG_REGISTER,
  CLKRGM_DCVS_LOG_DEREGISTER

} clkrgm_dcvs_clk_log_type;
#define CLKRGM_DCVS_LOG_DOMAIN              2

#define CLKRGM_DCVS_LOG_TRANSACTION_NA      0
#define CLKRGM_DCVS_LOG_TRANS_REGISTER      1
#define CLKRGM_DCVS_LOG_TRANS_DEREGISTER    2
#define CLKRGM_DCVS_LOG_TRANS_REGISTER_BYPASS      3
#define CLKRGM_DCVS_LOG_TRANS_DEREGISTER_BYPASS    4

#ifdef FEATURE_CLKREGIM_DYNAMIC_VOLTAGE
  #define CLKRGM_DCVS_LOG_STATUS_REG_COUNT  6
#else
  #define CLKRGM_DCVS_LOG_STATUS_REG_COUNT  5
#endif

/* Reduce the set of GSBI defines down to 9. */
/* UARTS */
#if defined(FEATURE_GSBI0_UART0) || defined(FEATURE_GSBI0_UART1) || defined(FEATURE_GSBI0_UART2)
  #define CLKRGM_GSBI0_IS_UART
#endif
#if defined(FEATURE_GSBI1_UART0) || defined(FEATURE_GSBI1_UART1) || defined(FEATURE_GSBI1_UART2)
  #define CLKRGM_GSBI1_IS_UART
#endif
#if defined(FEATURE_GSBI2_UART0) || defined(FEATURE_GSBI2_UART1) || defined(FEATURE_GSBI2_UART2)
  #define CLKRGM_GSBI2_IS_UART
#endif

/* I2C */
#if defined(FEATURE_GSBI0_I2C0) || defined(FEATURE_GSBI0_I2C1) || defined(FEATURE_GSBI0_I2C2)
  #define CLKRGM_GSBI0_IS_I2C
#endif
#if defined(FEATURE_GSBI1_I2C0) || defined(FEATURE_GSBI1_I2C1) || defined(FEATURE_GSBI1_I2C2)
  #define CLKRGM_GSBI1_IS_I2C
#endif
#if defined(FEATURE_GSBI2_I2C0) || defined(FEATURE_GSBI2_I2C1) || defined(FEATURE_GSBI2_I2C2)
  #define CLKRGM_GSBI2_IS_I2C
#endif

/* SPI */
#if defined(FEATURE_GSBI0_SPI0) || defined(FEATURE_GSBI0_SPI1) || defined(FEATURE_GSBI0_SPI2)
  #define CLKRGM_GSBI0_IS_SPI
#endif
#if defined(FEATURE_GSBI1_SPI0) || defined(FEATURE_GSBI1_SPI1) || defined(FEATURE_GSBI1_SPI2)
  #define CLKRGM_GSBI1_IS_SPI
#endif
#if defined(FEATURE_GSBI2_SPI0) || defined(FEATURE_GSBI2_SPI1) || defined(FEATURE_GSBI2_SPI2)
  #define CLKRGM_GSBI2_IS_SPI
#endif

#endif /* _ARM_ASM_ */

#endif /* CLKRGMI_QSC_1100_H */





