#ifndef CLKRGMI_MODEM_H
#define CLKRGMI_MODEM_H

/*=========================================================================

              M O D E M   C L O C K   D E V I C E   D R I V E R
               
                  I N T E R N A L   D E F I N I T I O N S

GENERAL DESCRIPTION
  This file contains the internal definitions for the Modem Clock Device
  Driver.  The modem clocks part of the Clock Device Driver is responsible for
  controlling the modem core clock blocks, and the mDSP clock blocks.

EXTERNALIZED FUNCTIONS
  See file clkrgm_modem.h.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  See file clkrgm_modem.h.

      Copyright (c) 2004 by QUALCOMM, Inc.  All Rights Reserved.
      Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
      Copyright (c) 2006 by QUALCOMM, Inc.  All Rights Reserved.
      Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
      Copyright (c) 2008 by QUALCOMM, Inc.  All Rights Reserved.

==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
 $Header: //depot/asic/qsc1100/drivers/clkregim/clkrgmi_modem.h#8 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
10/11/07   bc      Initial port from QSC6055v3202.

==========================================================================*/ 

#ifndef _ARM_ASM_

#include "comdef.h"
#include "log.h"

#ifdef FEATURE_CLKREGIM_RM
#include "clkrgmi_rm.h"
#endif


#include CLKRGM_H

/*=========================================================================
      Constants & Macros & Typedefs
==========================================================================*/

#define CLKRGM_MODEM_REG_ALL_MASK          0xffffffff

/*--------------------------------------------------------------------------
  mDSP CLOCK CONFIGURATION
---------------------------------------------------------------------------*/

/*
  clkrgm_modem_mdsp_cfg_type -
  Structure containing the mDSP clock configuration, including
  the clock control registers used for this specific speed.
  An instance of this structure should be used for each available clock speed.
*/
typedef struct
{
  uint32                clk_freq_khz;           /* mDSP clock freq in KHz */
  clkrgm_pll_type       pll;                    /* PLL in use */
  uint32                mdsp_mncntr_md;         /* MDSP_MNCNTR_MD */
  uint32                mdsp_mncntr_ns;         /* MDSP_MNCNTR_NS */
  uint32                mode;                   /* 1x = 1, 2x = 0 */
} clkrgm_modem_mdsp_cfg_type;

/*
  CLKRGM_MDSP_CFG_DATA -
  Values to populate the clkrgm_modem_mdsp_cfg_type structure array.
  This array contains one entry for each available mDSP clock speed.
*/

#if defined FEATURE_PLL_192
#define CLKRGM_MDSP_CFG_DATA                      \
{                                                 \
  {                                               \
    19200,            /* Freq = 19.2 MHz     */  \
    CLKRGM_NOPLL,     /* TCXO                */  \
    0x0,              /* MDSP_MNCNTR_MD      */  \
    0x0,              /* MDSP_MNCNTR_NS      */  \
    1,                                            \
  },                                              \
  {                                               \
    64000,            /* Freq = 64.0 MHz      */  \
    CLKRGM_PLL,       /* PLL                  */  \
    0x0001fffc,       /* MDSP_MNCNTR_MD       */  \
    0x0013fffd,       /* MDSP_MNCNTR_NS       */  \
    1,                                            \
  },                                              \
                                                  \
}
#elif defined FEATURE_PLL_384
#define CLKRGM_MDSP_CFG_DATA                      \
{                                                 \
  {                                               \
    19200,            /* Freq = 19.2 MHz     */  \
    CLKRGM_NOPLL,     /* TCXO                */  \
    0x0,              /* MDSP_MNCNTR_MD      */  \
    0x0,              /* MDSP_MNCNTR_NS      */  \
    1,                                            \
  },                                              \
  {                                               \
    64000,            /* Freq = 64.0 MHz      */  \
    CLKRGM_PLL,       /* PLL                  */  \
    0x0001fff9,       /* MDSP_MNCNTR_MD       */  \
    0x0013fffa,       /* MDSP_MNCNTR_NS       */  \
    1,                                            \
  },                                              \
                                                  \
}
#endif

/*--------------------------------------------------------------------------
  MODEM_CHIPXN CLOCK CONFIGURATION

  Required Frequency:  39.3216 MHz (chipx32)
---------------------------------------------------------------------------*/
#if defined FEATURE_PLL_192
#ifdef FEATURE_USE_PANDA_CHIPX16
#error code not present
#else
  #if defined(T_QSC6055) && !defined(T_RUMI)
    #define MODEM_CHIPXN_CLK_CTL_V            0x00001A1
    #define MODEM_CHIPXN_MNCNTR_M_V           (128)
    #define MODEM_CHIPXN_MNCNTR_NOT_2D_V      (~625)      /* 0xfffffd8e */
    #define MODEM_CHIPXN_MNCNTR_NOT_N_M_V     (~(625-128)) /* 0xfffffe0e */
    #define MODEM_CHIPXN_CLK_SRC_SEL_VAL      0
  #else
    #define MODEM_CHIPXN_CLK_CTL_V            0x00000005
    #define MODEM_CHIPXN_MNCNTR_M_V           0x0000
    #define MODEM_CHIPXN_MNCNTR_NOT_2D_V      0x0000
    #define MODEM_CHIPXN_MNCNTR_NOT_N_M_V     0x0000
  #endif
#endif
#elif defined FEATURE_PLL_384

/*--------------------------------------------------------------------------
  MODEM_CHIPXN CLOCK CONFIGURATION

  Required Frequency:  39.3216 MHz (chipx32)
---------------------------------------------------------------------------*/

#ifdef FEATURE_USE_PANDA_CHIPX16
#error code not present
#else
  #if defined(T_QSC6055) && !defined(T_RUMI)
    #define MODEM_CHIPXN_CLK_CTL_V            0x00001A1
    #define MODEM_CHIPXN_MNCNTR_M_V           0x40
    #define MODEM_CHIPXN_MNCNTR_NOT_2D_V      0x3ffffd8e  /*(~625) old*/      /* 0xfffffd8e */
    #define MODEM_CHIPXN_MNCNTR_NOT_N_M_V     0x3ffffdce /*(~(625-64)) old*/ /* 0xfffffdce */
    #define MODEM_CHIPXN_CLK_SRC_SEL_VAL      0
  #else
    #define MODEM_CHIPXN_CLK_CTL_V            0x00000005
    #define MODEM_CHIPXN_MNCNTR_M_V           0x0000
    #define MODEM_CHIPXN_MNCNTR_NOT_2D_V      0x0000
    #define MODEM_CHIPXN_MNCNTR_NOT_N_M_V     0x0000
  #endif
#endif

#endif

/*--------------------------------------------------------------------------
  MODEM_OFFLINE CLOCK CONFIGURATION

  Required Frequency:  51.2 MHz (chipx40) M=4, N=15, D=7.5
---------------------------------------------------------------------------*/
#if defined FEATURE_PLL_192
#if defined(T_QSC6055) && !defined(T_RUMI)
  #define MODEM_OFFLINE_CLK_CTL_V            0x1A1
  #define MODEM_OFFLINE_MNCNTR_M_V           (4)
  #define MODEM_OFFLINE_MNCNTR_NOT_2D_V      (~15)     /* 0xfffffff0 */
  #define MODEM_OFFLINE_MNCNTR_NOT_N_M_V     (~(15-4)) /* 0xfffffff4 */
#else
  #define MODEM_OFFLINE_CLK_CTL_V            0x00000000
  #define MODEM_OFFLINE_MNCNTR_M_V           0x0000
  #define MODEM_OFFLINE_MNCNTR_NOT_2D_V      0x0000
  #define MODEM_OFFLINE_MNCNTR_NOT_N_M_V     0x0000
#endif
#elif defined FEATURE_PLL_384
/*--------------------------------------------------------------------------
  MODEM_OFFLINE CLOCK CONFIGURATION

  Required Frequency:  51.2 MHz (chipx40) M=2, N=15, D=7.5
---------------------------------------------------------------------------*/

#if defined(T_QSC6055) && !defined(T_RUMI)
  #define MODEM_OFFLINE_CLK_CTL_V            0x1A1
  #define MODEM_OFFLINE_MNCNTR_M_V           (0x2)
  #define MODEM_OFFLINE_MNCNTR_NOT_2D_V      (0xfffffff0)     /* ~(15)  */
  #define MODEM_OFFLINE_MNCNTR_NOT_N_M_V     (0xfffffff2)     /* ~(15-2) */
#else
  #define MODEM_OFFLINE_CLK_CTL_V            0x00000000
  #define MODEM_OFFLINE_MNCNTR_M_V           0x0000
  #define MODEM_OFFLINE_MNCNTR_NOT_2D_V      0x0000
  #define MODEM_OFFLINE_MNCNTR_NOT_N_M_V     0x0000
#endif
#endif
/*--------------------------------------------------------------------------
  MODEM_GACC CLOCK CONFIGURATION

  Required Frequency:  59.07 MHz
  P = 1, M = 2, N = 11, D = 6.5 
---------------------------------------------------------------------------*/

#if defined(T_QSC6055) & !defined(T_RUMI)
  #define MODEM_GACC_CLK_CTL_V               0x00D1
  #define MODEM_GACC_MNCNTR_M_V              0x0002
  #define MODEM_GACC_MNCNTR_NOT_2D_V         0xfffffff4 /* ~(11) */
  #define MODEM_GACC_MNCNTR_NOT_N_M_V        0xfffffff6 /* ~(11-2) */
#else /*T_RUMI*/
  #define MODEM_GACC_CLK_CTL_V               0x0001
  #define MODEM_GACC_MNCNTR_M_V              0x0001
  #define MODEM_GACC_MNCNTR_NOT_2D_V         0xFFFC
  #define MODEM_GACC_MNCNTR_NOT_N_M_V        0xFFFD
#endif
/*--------------------------------------------------------------------------
  MODEM_GPS CLOCK CONFIGURATION

  Required Frequency:  32.736 MHz (gps_chipx32)
  P = 1, M = 341, N = 4000, D = 2000
---------------------------------------------------------------------------*/
#if defined(T_QSC6055) & !defined(T_RUMI)
  #define MODEM_GPS_CLK_CTL_V                 0x00D6
  #define MODEM_GPS_MNCNTR_M_V                0x0155
  #define MODEM_GPS_MNCNTR_NOT_2D_V           0xfffff05f /* ~(4000) */
  #define MODEM_GPS_MNCNTR_NOT_N_M_V          0xfffff1b4 /* ~(4000-341) */
#else
  #define MODEM_GPS_CLK_CTL_V                 0x00D3
  #define MODEM_GPS_MNCNTR_M_V                0x0155
  #define MODEM_GPS_MNCNTR_NOT_2D_V           0xF7FF
  #define MODEM_GPS_MNCNTR_NOT_N_M_V          0xF954
#endif

/*--------------------------------------------------------------------------
  MODEM_F9 CLOCK CONFIGURATION

  Required Frequency:  19.2 MHz (TCXO)
---------------------------------------------------------------------------*/

#define MODEM_F9_CLK_CTL_V                  0x0000

/*--------------------------------------------------------------------------
  MODEM_EDGE_TX CLOCK CONFIGURATION

  Required Frequency:  52 MHz
  P = 1, M = 1625, N = 6144, D = 3072
---------------------------------------------------------------------------*/
#ifdef T_MSM6800B
  #define MODEM_EDGE_TX_CLK_CTL_V             0x0069
#else
  #define MODEM_EDGE_TX_CLK_CTL_V             0x006A
#endif
#define MODEM_EDGE_TX_MNCNTR_M_V            0x0659
#define MODEM_EDGE_TX_MNCNTR_NOT_2D_V       0xE7FF
#define MODEM_EDGE_TX_MNCNTR_NOT_N_M_V      0xEE58

/*--------------------------------------------------------------------------
  MODEM_EDGE_RX CLOCK CONFIGURATION

  Required Frequency:  45 MHz
  P = 1, M = 1875, N = 8192, D = 4096
---------------------------------------------------------------------------*/

#define MODEM_EDGE_RX_CLK_CTL_V             0x006A
#define MODEM_EDGE_RX_MNCNTR_M_V            0x0753
#define MODEM_EDGE_RX_MNCNTR_NOT_2D_V       0xDFFF
#define MODEM_EDGE_RX_MNCNTR_NOT_N_M_V      0xE752

/*--------------------------------------------------------------------------
  MODEM_GSAC CLOCK CONFIGURATION

  Required Frequency:  65 MHz
  P = 1, M = 8125, N = 24576, D = 12288
---------------------------------------------------------------------------*/

#define MODEM_GSAC_CLK_CTL_V                0x006A
#define MODEM_GSAC_MNCNTR_M_V               0x1FBD
#define MODEM_GSAC_MNCNTR_NOT_2D_V          0x9FFF
#define MODEM_GSAC_MNCNTR_NOT_N_M_V         0xBFBC

/*--------------------------------------------------------------------------
  MODEM_WEB CLOCK CONFIGURATION

  Required Frequency:  19.2 MHz (TCXO)
---------------------------------------------------------------------------*/

#define MODEM_WEB_CLK_CTL_V                 0x0000

/*--------------------------------------------------------------------------
  FOOTSWITCH CONFIGURATION
---------------------------------------------------------------------------*/

#define CLKRGM_MODEM_CLK_MDSP_FS_CTL_V                0x7e

/*--------------------------------------------------------------------------
  MISCELLANEOUS DEFINITIONS
---------------------------------------------------------------------------*/

/* Clock disable delay in microseconds.  This time needs to be at least
   two clock cycles of the clock prior to switching.  The value chosen
   below is conservative based on the slowest clock source that can be
   selected. */
/* 3 sleep clocks = 100 usec */
#define CLKRGM_MODEM_SLEEP_DISABLE_TIME_US        100
/* 3 clocks @ 1MHz = 3 usec */
#define CLKRGM_MODEM_1MHZ_DISABLE_TIME_US         3

/*--------------------------------------------------------------------------
  DRIVER STATE STRUCTURE
---------------------------------------------------------------------------*/

/* The type to hold the mode of each RF chain */
typedef enum
{
  CLKRGM_MODEM_CLK_MODE_CDMA,
  CLKRGM_MODEM_CLK_MODE_NUM_MODES
} clkrgm_modem_clk_mode_type;

/* RXF manager type */
typedef enum
{
  CLKRGM_RXF_OFF,
  CLKRGM_RXF_ON
} clkrgm_rxf_state_type;
  
/*
  clkrgm_modem_debug_type -
  Structure containing debug flags for the modem clock regime
*/
typedef struct
{
  boolean                         clks_always_on;
                                  /* Force all modem clocks to stay on */
  boolean                         disable_dynamic_mdsp_clk;
                                  /* Disabling switching of mdsp clock */
} clkrgm_modem_debug_type;

/*
  clkrgm_modem_state_type -
  Structure containing the general configuration and state of
  the modem clocks.
*/
typedef struct
{
  clkrgm_mdsp_clk_frequency_type  mdsp_speed;
                                  /* mDSP clock speed before switch */
  clkrgm_mdsp_clk_frequency_type  mdsp_pending_speed;                
                                  /* mDSP clock speed after switch */
  boolean                         agc_src_is_on;
                                  /* AGC source flag for GPS operation:
                                   * TRUE = is on; FALSE = is off */
  clkrgm_modem_clk_mode_type      clk_mode[CLKRGM_RX_CHAIN_BOTH];
                                  /* GPS or CMDA mode */
  #ifdef FEATURE_CLKREGIM_RM
  clkrgm_rm_clk_info_type         rm_clk_info[CLKRGM_MODEM_NUM_OF_CLOCKS -
                                              CLKRGM_MODEM_CLK_OFFSET];
                                  /* Resource manager data */
  #endif

  clkrgm_rxf_state_type    rxf_state;
  int                      rxf_usage; /* count clients requiring RXF on/off */
  boolean rxf_clients[CLKRGM_RXF_MAX_CLIENTS];/* RXF clients */

  clkrgm_modem_debug_type         debug;
} clkrgm_modem_state_type;

/*=========================================================================
      Functions
==========================================================================*/
/* None */

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
  uint32        status_reg[2];
} clkrgm_modem_log_type;

typedef enum
{
  CLKRGM_MODEM_LOG_NONE = 0,
  CLKRGM_MODEM_LOG_CHIPXN_CLK,
  CLKRGM_MODEM_LOG_CHIPXN_DIV2_CLK,
  CLKRGM_MODEM_LOG_CHIPXN_DIV4_CLK,
  CLKRGM_MODEM_LOG_RXF0_SAMP_CLK,
  CLKRGM_MODEM_LOG_RXF1_SAMP_CLK,
  CLKRGM_MODEM_LOG_RXF0_FRONT_CLK,
  CLKRGM_MODEM_LOG_RXF1_FRONT_CLK,
  CLKRGM_MODEM_LOG_RXF0_SAMPCTRL_CLK,
  CLKRGM_MODEM_LOG_RXF1_SAMPCTRL_CLK,
  CLKRGM_MODEM_LOG_EDGE_CLK,
  CLKRGM_MODEM_LOG_EDGE_CLK_DIV2_CLK,
  CLKRGM_MODEM_LOG_GSM_CLK,
  CLKRGM_MODEM_LOG_FB_ANT0_CLK,
  CLKRGM_MODEM_LOG_FB_ANT1_CLK,
  CLKRGM_MODEM_LOG_TX_PDM_CLK,
  CLKRGM_MODEM_LOG_TCXO_PDM_CLK,
  CLKRGM_MODEM_LOG_MDSP_CLK,
  CLKRGM_MODEM_LOG_SLPFAST_CHIPXN_REF1_CLK,
  CLKRGM_MODEM_LOG_SLPFAST_CHIPXN_REF2_CLK,
  CLKRGM_MODEM_LOG_SLPFAST_GSM_CLK,
  CLKRGM_MODEM_LOG_SLPFAST_FEE_CLK,
  CLKRGM_MODEM_LOG_BT_CLK,
  CLKRGM_MODEM_LOG_MODEM_SBI_CLK,
  CLKRGM_MODEM_LOG_MODEM_WEB_CLK,
  CLKRGM_MODEM_LOG_OFFLINE_CLK,
  CLKRGM_MODEM_LOG_GACC_CLK,
  CLKRGM_MODEM_LOG_GPS_CHIPX8_CLK,
  CLKRGM_MODEM_LOG_F9_CLK,
  CLKRGM_MODEM_LOG_MMGPS_CLK,
  CLKRGM_MODEM_LOG_MMGPS_ARM_CLK,
  CLKRGM_MODEM_LOG_TX_DAC_CLK,
  CLKRGM_MODEM_LOG_PC_DAC_CLK,
  CLKRGM_MODEM_LOG_BT_SBI_CLK,
  CLKRGM_MODEM_LOG_MDM_CLK,
  CLKRGM_MODEM_LOG_RXF_SAMP_CLK,
  CLKRGM_MODEM_LOG_RXF_SAMP_REF_CLK,
  CLKRGM_MODEM_LOG_GPS_CHIPX16_CLK,
  CLKRGM_MODEM_LOG_GPS_CHIPX32_CLK,
  CLKRGM_MODEM_LOG_SAMPLE_FIFO0_CLK,
  CLKRGM_MODEM_LOG_SAMPLE_FIFO1_CLK,
  CLKRGM_MODEM_LOG_TX_RAM_CLK,
  CLKRGM_MODEM_LOG_SSRV_OFFLINE0_CLK,
  CLKRGM_MODEM_LOG_SSRV_OFFLINE1_CLK,
  CLKRGM_MODEM_LOG_SYMBOL_BUFFER_CLK,
  CLKRGM_MODEM_LOG_CC_RAM_CLK,
  CLKRGM_MODEM_LOG_DEM1X_OFFLINE_CLK,      
  CLKRGM_MODEM_LOG_DEM1X_FFE_RESULT_CLK,   
  CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,   
  CLKRGM_MODEM_LOG_DEM1X_FFE_CONFIG_CLK,   
  CLKRGM_MODEM_LOG_DEM1X_CX8_CLK,          
  CLKRGM_MODEM_LOG_DEINT_MICRO_CLK,        
  CLKRGM_MODEM_LOG_DEINT_MICRO_TASK_RD_CLK,
  CLKRGM_MODEM_LOG_DEINT_MICRO_TD_RD_CLK,  
  CLKRGM_MODEM_LOG_DEINT_MICRO_VD_RD_CLK,  
  CLKRGM_MODEM_LOG_FL_SYM_BUFF_CLK,        
  CLKRGM_MODEM_LOG_MOD_CX16_CLK,           
  CLKRGM_MODEM_LOG_MOD_CX8_CLK,            
  CLKRGM_MODEM_LOG_MOD_PDM_CLK,            
  CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,        
  CLKRGM_MODEM_LOG_SAMPSRV_CLK,            
  CLKRGM_MODEM_LOG_TD_MICRO_CLK,           
  CLKRGM_MODEM_LOG_VD_MICRO_CLK,           
  CLKRGM_MODEM_LOG_GACC_MICRO_SW_CLK,
  CLKRGM_MODEM_LOG_GACC_MICRO_CLK
} clkrgm_modem_clk_log_type;

#define CLKRGM_MODEM_LOG_DOMAIN               1

#define CLKRGM_MODEM_LOG_TRANSACTION_NA       0
#define CLKRGM_MODEM_LOG_TRANSACTION_ENABLE   1
#define CLKRGM_MODEM_LOG_TRANSACTION_DISABLE  2

#define CLKRGM_MODEM_LOG_STATUS_REG_COUNT     2

#endif  /* _ARM_ASM_ */

#endif  /* CLKRGMI_MODEM_H */ 

