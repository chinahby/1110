#ifndef MPMC_H
#define MPMC_H
/*==========================================================================

                 Multi Port Memory Controller Definitions Header File

DESCRIPTION
  constants specific to  Multi Port Memory Controller

Copyright (c) 1997,1998 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 1999-2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //depot/asic/msmshared/drivers/boot/mpmc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/17/2002   rp     Created

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "msm.h"
#include CLKRGMI_H
#define MPMC_CLOCK_FREQUENCY   (CLKRGM_HCLK_FREQUENCY_IN_MHZ +1)  /* FREQUENCY IN MHZ */

#ifdef BOOT_MODE_NAND

  #define SDRAM_BASE                                           0x00000000
#else
  #define SDRAM_BASE                                           0x10000000
#endif


/******************************************************/
/* Contains list of memory parameters that change     */
/* depending on the size of memory detected.          */
/*                                                    */
/* Values for these registers are not found in this   */
/* file, but are located in mpmc.c.                   */
/******************************************************/
#define MPMC_TYPE_DY_TSREX_VALUE                          0
#define MPMC_TYPE_DY_TRFC_VALUE                           1
#define MPMC_TYPE_DY_TXSR_VALUE                           2
#define MPMC_TYPE_DY_CONFIG_WW__ADDRESS_MAPPING_VALUE     3
#define MPMC_TYPE_DY_RASCAS_WW__CAS_LATENCY_VALUE         4
#define MPMC_TYPE_DY_RASCAS_WW__RAS_LATENCY_VALUE         5 
#define MPMC_TYPE_DY_MODE_REGISTER_ADDRESS_VALUE          6
#define MPMC_TYPE_DY_EXTENDED_MODE_REGISTER_ADDRESS_VALUE 7
#define MPMC_TYPE_SIZE                                    8




#define MPMC_CONTROL_RW_OFFSET                                 0x00000000
#define MPMC_CONTROL_WW_OFFSET                                 0x00000000
#define MPMC_CONTROL_WW__DEFAULT_VALUE                         0x00000001
#define MPMC_CONTROL_WW__DRAIN_WRITE_BUFFERS_MASK              0x00000008
#define MPMC_CONTROL_WW__DRAIN_WRITE_BUFFERS_VALUE             0x00000008
#define MPMC_CONTROL_WW__NORMAL_WRITE_BUFFERS_VALUE            0x00000000
#define MPMC_CONTROL_WW__LOW_POWER_MODE_MASK                   0x00000004
#define MPMC_CONTROL_WW__LOW_POWER_MODE_VALUE                  0x00000004
#define MPMC_CONTROL_WW__NORMAL_POWER_MODE_VALUE               0x00000000
#define MPMC_CONTROL_WW__ADDRESS_MIRROR_MASK                   0x00000002
#define MPMC_CONTROL_WW__RESET_MEMORY_MAP_VALUE                0x00000002
#define MPMC_CONTROL_WW__NORMAL_MEMORY_MAP_VALUE               0x00000000
#define MPMC_CONTROL_WW__MPMC_ENABLE_MASK                      0x00000001
#define MPMC_CONTROL_WW__MPMC_ENABLE_VALUE                     0x00000001
#define MPMC_CONTROL_WW__MPMC_DISABLE_VALUE                    0x00000000
                                                            
#define MPMC_STATUS_RW_OFFSET                                  0x00000004
#define MPMC_STATUS_RW__DEFAULT_VALUE                          0x00000000
#define MPMC_STATUS_RW__SELF_REFRESH_ACKNOWLEDGE_MASK          0x00000004
#define MPMC_STATUS_RW__WRITE_BUFFER_STATUS_MASK               0x00000002
#define MPMC_STATUS_RW__BUSY_MASK                              0x00000001
#define MPMC_STATUS_RW__BUFFER_AND_BUSY_MASK                   0x00000003
                                                            
#define MPMC_CONFIG_RW_OFFSET                                  0x00000008
#define MPMC_CONFIG_WW_OFFSET                                  0x00000008
#define MPMC_CONFIG_WW__DEFAULT_VALUE                          0x00000000
#define MPMC_CONFIG_WW__CLOCK_RATIO_MASK                       0x00000100
#define MPMC_CONFIG_WW__CLOCK_RATIO_ONE_TO_ONE_VALUE           0x00000000
#define MPMC_CONFIG_WW__CLOCK_RATIO_TWO_TO_ONE_VALUE           0x00000100
#define MPMC_CONFIG_WW__ENDIAN_MODE_MASK                       0x00000001
#define MPMC_CONFIG_WW__LITTLE_ENDIAN_VALUE                    0x00000000
#define MPMC_CONFIG_WW__BIG_ENDIAN_VALUE                       0x00000001
                                                            
#define MPMC_DY_CNTL_RW_OFFSET                                 0x00000020
#define MPMC_DY_CNTL_WW_OFFSET                                 0x00000020
#define MPMC_DY_CNTL_WW__DEFAULT_VALUE                         0x00000002
#define MPMC_DY_CNTL_WW__SLEEP_MODE_MASK                       0x00002000
#define MPMC_DY_CNTL_WW__SLEEP_MODE_NORMAL_OPERATION_VALUE     0x00000000
#define MPMC_DY_CNTL_WW__DEEP_SLEEP_MODE_VALUE                 0x00002000
#define MPMC_DY_CNTL_WW__SDRAM_INIT_MASK                       0x00000180
#define MPMC_DY_CNTL_WW__SDRAM_INIT_NORMAL_VALUE               0x00000000
#define MPMC_DY_CNTL_WW__SDRAM_INIT_MODE_VALUE                 0x00000080
#define MPMC_DY_CNTL_WW__SDRAM_INIT_PALL_VALUE                 0x00000100
#define MPMC_DY_CNTL_WW__SDRAM_INIT_NOP_VALUE                  0x00000180
#define MPMC_DY_CNTL_WW__REFRESH_MODE_MASK                     0x00000004
#define MPMC_DY_CNTL_WW__SELF_REFRESH_VALUE                    0x00000004
#define MPMC_DY_CNTL_WW__NORMAL_REFRESH_VALUE                  0x00000000
#define MPMC_DY_CNTL_WW__CLOCK_CONTROL_MASK                    0x00000002
#define MPMC_DY_CNTL_WW__STOP_WHEN_IDLE_VALUE                  0x00000000
#define MPMC_DY_CNTL_WW__RUN_CONTINUOUSLY_VALUE                0x00000002
#define MPMC_DY_CNTL_WW__CLOCK_ENABLE_MASK                     0x00000001
#define MPMC_DY_CNTL_WW__DEASSERT_ON_IDLE_VALUE                0x00000000
#define MPMC_DY_CNTL_WW__ALWAYS_DRIVE_HIGH_VALUE               0x00000001
#define MPMC_DY_CNTL_WW__CLOCK_POWER_MODE_MASK                 0x00000003
#define MPMC_DY_CNTL_WW__CLOCK_HIGH_POWER_VALUE                0x00000003
#define MPMC_DY_CNTL_WW__CLOCK_LOW_POWER_VALUE                 0x00000000
#define MPMC_DY_CNTL_WW__CLOCK_INTERMEDIATE_POWER_VALUE        0x00000001
/*  ARM documentation says this is wrong, but ARM says ok */

#define MPMC_DY_REF_RW_OFFSET                                  0x00000024
#define MPMC_DY_REF_WW_OFFSET                                  0x00000024
#define MPMC_DY_REF_WW__DEFAULT_VALUE                          0x00000000
#define MPMC_DY_REF_WW__REFRESH_TIMER_MASK                     0x000007FF
#define MPMC_DY_REF_WW__NUMBER_OF_CYCLES_TCXO_VALUE            0x00000013
#define MPMC_DY_REF_WW__NUMBER_OF_CYCLES_VALUE                 0x00000009
#define MPMC_DY_REF_WW__THIRTY_TWO_CYCLES_VALUE                0x00000002
                                                            
// #ifdef T_MSM6100B
                                                            
#define MPMC_DY_READ_CFG_RW_OFFSET                             0x00000028
#define MPMC_DY_READ_CFG_WW_OFFSET                             0x00000028
#define MPMC_DY_READ_CFG_WW__DEFAULT_VALUE                     0x00000000
#define MPMC_DY_READ_CFG_WW__READ_STRATEGY_MASK                0x00000003
#define MPMC_DY_READ_CFG_WW__CLK_OUT_DELAY_VALUE               0x00000000
#define MPMC_DY_READ_CFG_WW__CMD_DELAY_VALUE                   0x00000001
#define MPMC_DY_READ_CFG_WW__CMD_DELAY_PLUS1_VALUE             0x00000002
#define MPMC_DY_READ_CFG_WW__CMD_DELAY_PLUS2_VALUE             0x00000003
                                                            
// #endif
                                                            
#define MPMC_DY_TRP_RW_OFFSET                                  0x00000030
#define MPMC_DY_TRP_WW_OFFSET                                  0x00000030
#define MPMC_DY_TRP_WW__DEFAULT_VALUE                          0x0000000F
#define MPMC_DY_TRP_VALUE                                      0x0000001e
                                                            
#define MPMC_DY_TRAS_RW_OFFSET                                 0x00000034
#define MPMC_DY_TRAS_WW_OFFSET                                 0x00000034
#define MPMC_DY_TRAS_RW__DEFAULT_VALUE                         0x0000000F
#define MPMC_DY_TRAS_VALUE                                     0x0000003c
                                                            
#define MPMC_DY_TSREX_RW_OFFSET                                0x00000038
#define MPMC_DY_TSREX_WW_OFFSET                                0x00000038
#define MPMC_DY_TSREX_WW__DEFAULT_VALUE                        0x0000000F
                                                            
#define MPMC_DY_TAPR_RW_OFFSET                                 0x0000003C
#define MPMC_DY_TAPR_WW_OFFSET                                 0x0000003C
#define MPMC_DY_TAPR_WW__DEFAULT_VALUE                         0x0000000F
#define MPMC_DY_TAPR__VALUE                                    0x0000005a
                                                           
#define MPMC_DY_TDAL_RW_OFFSET                                 0x00000040
#define MPMC_DY_TDAL_WW_OFFSET                                 0x00000040
#define MPMC_DY_TDAL_WW__DEFAULT_VALUE                         0x0000000F
                                                            
#define MPMC_DY_TWR_RW_OFFSET                                  0x00000044
#define MPMC_DY_TWR_WW_OFFSET                                  0x00000044
#define MPMC_DY_TWR_WW__DEFAULT_VALUE                          0x0000000F
#define MPMC_DY_TWR_VALUE                                      0x00000005
/*    DELAY IS 5ns + 1 Clock Cycle*/
                                                            
#define MPMC_DY_TRC_RW_OFFSET                                  0x00000048
#define MPMC_DY_TRC_WW_OFFSET                                  0x00000048
#define MPMC_DY_TRC_WW__DEFAULT_VALUE                          0x0000001F
#define MPMC_DY_TRC_VALUE                                      0x0000005a
                                                            
#define MPMC_DY_TRFC_RW_OFFSET                                 0x0000004C
#define MPMC_DY_TRFC_WW_OFFSET                                 0x0000004C
#define MPMC_DY_TRFC_WW__DEFAULT_VALUE                         0x0000001F
                                                            
#define MPMC_DY_TXSR_RW_OFFSET                                 0x00000050
#define MPMC_DY_TXSR_WW_OFFSET                                 0x00000050
#define MPMC_DY_TXSR_WW__DEFAULT_VALUE                         0x0000001F
                                                            
#define MPMC_DY_TRRD_RW_OFFSET                                 0x00000054
#define MPMC_DY_TRRD_WW_OFFSET                                 0x00000054
#define MPMC_DY_TRRD_WW__DEFAULT_VALUE                         0x0000000F
#define MPMC_DY_TRRD_VALUE                                     0x00000014
                                                            
#define MPMC_DY_TMRD_RW_OFFSET                                 0x00000058
#define MPMC_DY_TMRD_WW_OFFSET                                 0x00000058
#define MPMC_DY_TMRD_WW__DEFAULT_VALUE                         0x0000000F
#define MPMC_DY_TMRD_VALUE                                     0x00000002 /*    Measured in cycles, not ns */
                                                            
#define MPMC_DY_CONFIG0_RW_OFFSET                              0x00000100
#define MPMC_DY_CONFIG0_WW_OFFSET                              0x00000100
#define MPMC_DY_CONFIG1_RW_OFFSET                              0x00000120
#define MPMC_DY_CONFIG1_WW_OFFSET                              0x00000120
#define MPMC_DY_CONFIG2_RW_OFFSET                              0x00000140
#define MPMC_DY_CONFIG2_WW_OFFSET                              0x00000140
#define MPMC_DY_CONFIG3_RW_OFFSET                              0x00000160
#define MPMC_DY_CONFIG3_WW_OFFSET                              0x00000160
#define MPMC_DY_CONFIG_WW__DEFAULT_VALUE                       0x00000000
#define MPMC_DY_CONFIG_WW__ROW_WIDTH_MASK                      0x30000000
#ifdef T_QUASAR
#error code not present
#else
/*    Sets row width to 12-bits */
#define MPMC_DY_CONFIG_WW__ROW_WIDTH_VALUE                     0x10000000
#endif
/*    Sets number of banks to 4  */
#define MPMC_DY_CONFIG_WW__NUMBER_OF_BANKS_MASK                0x04000000
#define MPMC_DY_CONFIG_WW__NUMBER_OF_BANKS_VALUE               0x04000000
#define MPMC_DY_CONFIG_WW__COLUMN_WIDTH_MASK                   0x01C00000
#ifdef T_QUASAR
#error code not present
#else
/*    Sets column width to 9-bits */
#define MPMC_DY_CONFIG_WW__COLUMN_WIDTH_VALUE                  0x00C00000
#endif
#define MPMC_DY_CONFIG_WW__WRITE_PROTECT_MASK                  0x00100000
#define MPMC_DY_CONFIG_WW__ENABLE_WRITE_PROTECT_VALUE          0x00100000
#define MPMC_DY_CONFIG_WW__DISABLE_WRITE_PROTECT_VALUE         0x00000000
#define MPMC_DY_CONFIG_WW__BUFFER_ENABLE_MASK                  0x00080000
#define MPMC_DY_CONFIG_WW__BUFFER_ENABLE_VALUE                 0x00080000
#define MPMC_DY_CONFIG_WW__BUFFER_DISABLE_VALUE                0x00000000
#define MPMC_DY_CONFIG_WW__ADDRESS_MAPPING_MASK                0x00001F80

#define MPMC_DY_CONFIG_WW__MEMORY_DEVICE_MASK                  0x00000018
/*    Sets ID to Low Power SDRAM */
#define MPMC_DY_CONFIG_WW__MEMORY_DEVICE_ID_VALUE              0x00000008
                                                            
#define MPMC_DY_RASCAS0_RW_OFFSET                              0x00000104
#define MPMC_DY_RASCAS0_WW_OFFSET                              0x00000104
#define MPMC_DY_RASCAS1_RW_OFFSET                              0x00000124
#define MPMC_DY_RASCAS1_WW_OFFSET                              0x00000124
#define MPMC_DY_RASCAS2_RW_OFFSET                              0x00000144
#define MPMC_DY_RASCAS2_WW_OFFSET                              0x00000144
#define MPMC_DY_RASCAS3_RW_OFFSET                              0x00000164
#define MPMC_DY_RASCAS3_WW_OFFSET                              0x00000164
#define MPMC_DY_RASCAS_WW__DEFAULT_VALUE                       0x00000303
#define MPMC_DY_RASCAS_WW__CAS_LATENCY_MASK                    0x00000300
#define MPMC_DY_RASCAS_WW__RAS_LATENCY_MASK                    0x00000003

#define MPMC_ST_CONFIG_WW__MEMORY_WIDTH_MASK                   0x00000003
#define MPMC_ST_CONFIG_WW__MEMORY_WIDTH_8_VALUE                0x00000000
#define MPMC_ST_CONFIG_WW__MEMORY_WIDTH_16_VALUE               0x00000001
#define MPMC_ST_CONFIG_WW__MEMORY_WIDTH_32_VALUE               0x00000002
#define MPMC_ST_CONFIG_WW__BYTE_LANE_STATE_MASK                0x00000080
#define MPMC_ST_CONFIG_WW__BYTE_LANE_8_BIT_VALUE               0x00000000
#define MPMC_ST_CONFIG_WW__BYTE_LANE_16_BIT_VALUE              0x00000080
#define MPMC_ST_CONFIG_WW__PAGE_MODE_MASK                      0x00000008
#define MPMC_ST_CONFIG_WW__PAGE_MODE_ENABLED_VALUE             0x00000008
#define MPMC_ST_CONFIG_WW__PAGE_MODE_DISABLED_VALUE            0x00000000
                                                            
#define MPMC_ST_W_TWEN_WW__WAIT_WRITE_ENABLE_MASK              0x0000000F
#define MPMC_ST_W_TWEN_WW__WAIT_WRITE_ONE_HCLK_VALUE           0x00000000
                                                            
#define MPMC_ST_W_OWEN_WW__WAIT_OUTPUT_ENABLE_MASK             0x0000000F
#define MPMC_ST_W_OWEN_WW__WAIT_OUTPUT_ONE_HCLK_VALUE          0x00000000

#ifdef FEATURE_SDRAM_RUNTIME_DETECTION
  #define MPMC_TEST_ADDRESS 0x0
  #ifdef T_SDRAM_IS_32_BIT
    #define MPMC_TEST_ADDRESS_ALIAS 0x800000
  #else
    #define MPMC_TEST_ADDRESS_ALIAS 0x400000
  #endif
#endif

#endif  /* MPMC_H */

