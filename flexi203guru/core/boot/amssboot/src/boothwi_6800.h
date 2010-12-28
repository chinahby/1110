#ifndef BOOTHWI_6800_H
#define BOOTHWI_6800_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      B O O T    H A R D W A R E    M S M 6 5 5 0    S P E C I F I C
                I N I T I A L I Z A T I O N    H E A D E R

GENERAL DESCRIPTION
  The access parameters required for the configuration of the external memory
  bus interface for each supported memory and display device are defined in
  this file.  The particular external memory bus configuration is also defined
  in this file based on the compile time features that are defined for a
  particular software build.

  To add support for a new memory device, a new memory device configuration
  structure definitino must be added.  The configuration structure for the
  new memory device must be added to the corresponding external bus interface
  configuration structure definition.

  The following is a summary of the portion of the address map that is used
  for external memory and LCD components.  Note that the EBI1 chip selects
  are mapped to different address ranges depending on which BOOT MODE is
  selected.

  0x48000000   -------------------------------------
               |             EBI2_CS_3             |
  0x40000000   |-----------------------------------|
               |             EBI2_CS_2             |
  0x38000000   |-----------------------------------|
               |             EBI2_CS_1             |
  0x30000000   |-----------------------------------|
               |             EBI2_CS_0             |
  0x28000000   |-----------------------------------|
               |             EBI2_LCD_CS           |
  0x20000000   |-----------------------------------|
               |    EBI1_CS_3    |    EBI1_CS_1    |
  0x18000000   |-----------------|-----------------|
               |    EBI1_CS_2    |    EBI1_CS_0    |
  0x10000000   |-----------------|-----------------|
               |    EBI1_CS_1    |    EBI1_CS_3    |
  0x08000000   |-----------------|-----------------|
               |    EBI1_CS_0    |    EBI1_CS_2    |
  0x00000000   -------------------------------------
                                 |
  BOOT MODE         NOR BOOT     |    NAND BOOT


Copyright (c) 2003, 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msm6800/drivers/boot/2.1/boothwi_6800.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/05   rp     Updated BRE value for PSRAM 32 MB.
01/28/05   rp     Updated 32 bit PSRAM settings.
01/18/05   rp      Changed the AHB priorities so that APP->DMA->SYS is the
                   priority for the SDRAM configuration.
08/13/04   trc     Added support for Slingshot EFFA
08/03/04   jkl     Updated 16 bit BURST PSRAM settings.
07/23/04   jkl     Updated for 16 bit PSRAM.
06/22/04   jkl     Updated APP_LOCK_ENABLE.
06/11/04   jkl     Updated PAD settings.
06/01/04   bmg     Added QVGA support for FFA6550 H2
05/20/04   jkl     Added BURST NOR.
05/05/04   ap      Added support for 18BPP.
05/03/04   bmg     Added Crossbow H1 FFA EBI2 config
04/13/04   jkl     Added Burst PSRAM.
04/08/04   jkl     Ported for MSM6550 from MSM6500.
===========================================================================*/

#include "target.h"
#include "comdef.h"


/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/* --------------------------------------------------------------------------
   ADSP_INTF_CFG register configuration parameters.
-------------------------------------------------------------------------- */

/* ACK signal to be used on a micro WRITE access */

#define ADSP_INTF_CFG_WR_ACK_SEL_VAL                   0x1

/* ACK signal to be used on a micro READ access */

#define ADSP_INTF_CFG_RD_ACK_SEL_VAL                   0x3

/* Assign amba_ahb bus higher priority than dsp_ahb bus. */

#define ADSP_INTF_CFG_PRIORITY_VAL                     0x0

/* --------------------------------------------------------------------------
   MDSP_INTF_CFG register configuration parameters.
-------------------------------------------------------------------------- */

/* ACK signal to be used on a micro WRITE access */

#define MDSP_INTF_CFG_WR_ACK_SEL_VAL                   0x1

/* ACK signal to be used on a micro READ access */

#define MDSP_INTF_CFG_RD_ACK_SEL_VAL                   0x3

/* Assign amba_ahb bus higher priority than dsp_ahb bus. */

#define MDSP_INTF_CFG_PRIORITY_VAL                     0x0

/* --------------------------------------------------------------------------
   EBI1_CFG register configuration parameters.
-------------------------------------------------------------------------- */

#define EBI1_CMD_DELAY_ENA                               1

/* Reserved. Kept 1 for backward compatibility */
#define EBI1_RESERVED_VAL                                0

/* ROM1 Clock State  */
#define EBI1_ROM1_CLK_LO                                 0
#define EBI1_ROM1_CLK_HI                                 1

/* APP Lock Enable  */
#define EBI1_APP_LOCK_VAL                                0

/* Feedback clock polarity. */
#define EBI1_FBCLK_POL_VAL                               0

/* Feedback memory clock delay value. */

#define EBI1_FBCLK_PGM_DY_CTL_VAL_CONVENTIONAL           0
#define EBI1_FBCLK_PGM_DY_CTL_VAL_SYNCHRONOUS            0
#define EBI1_PGM_DY_CTL_VAL_NON_CONVENTIONAL             0

/* Preliminary value related to memory timing, subject to tuning */

#ifdef T_PSRAM_IS_BURST_TYPE
  #ifdef T_PSRAM_32
    #define EBI1_FBCLK_PGM_DY_CTL_VAL_BURST             0x33
  #else
    #define EBI1_FBCLK_PGM_DY_CTL_VAL_BURST             0x32
  #endif
#else
  /* Prevent DSP accesses from being interrupted. */
  /* Capture address on rising edge of ADV_N signal. */
  #define EBI1_FBCLK_PGM_DY_CTL_VAL_BURST             0x2C
#endif

/* Support 32 PSRAM 2x16 */
#define EBI1_IGN_HROM1_WAIT_N_VAL                        0

/* For Debug ONLY */
#define EBI1_CGC_DIS_VAL                                 0

/* Sleep Override */
#define EBI1_FBCLK_SLEEP_OVERRIDE                        1

/* Prevent PWRSAVE_CLK_DIS from gating ROM1_CLK signal. */
#define EBI1_PWRSAVE_HANDSHAKE_CTL_VAL                   1

/* Clock Pol */
#define EBI1_CLK_POL_VAL                                 0

/* Burst clock only toggles for accesses to burst memory. */
#define EBI1_PWRSAVE_CLK_DIS_VAL                         1

/* FCRAM / Cellular support */
#define EBI1_IGN_WAIT_FOR_WRITE_VAL                      0

/* Output memory clock delay values. */
#ifdef T_PSRAM_32
  #define EBI1_PGM_DY_CTL_VAL_BURST                   0x24
#else
  #define EBI1_PGM_DY_CTL_VAL_BURST                   0x20
#endif


#define EBI1_BURST_DELTA_CYCLES_VAL                      3

/* --------------------------------------------------------------------------
   EBI1_PSRAM_CRE register configuration parameters.
-------------------------------------------------------------------------- */

/* Output zero on SDRAM1_CLK_EN pin. */

#define EBI1_PSRAM_CRE_VAL                               0
#define EBI1_PSRAM_CRE_VAL_HI                            1
#define EBI1_PSRAM_CRE_VAL_LO                            0

/* Value needed to configure BURST PSRAM to */
#ifdef T_PSRAM_32
  #define EBI1_BURST_PSRAM_BRE_VAL                       0x0021673c
#else
  #define EBI1_BURST_PSRAM_BRE_VAL                       0x0010339e
#endif
#define MICRON_PROGRAM_BURST_VALUE_1                     0x0

/* Default value needed for EBI1_CS(1-3)_CFGn registers */
#define EBI1_CS_CFG0_INIT_VALUE                          0x0033FC00
#define EBI1_CS_CFG1_INIT_VALUE                          0x0

/* --------------------------------------------------------------------------
   Heap on Shadow RAM Values for NOR Builds.
-------------------------------------------------------------------------- */

#define SHADOW_RAM_ADDRESS                               0x18000000
#define SHADOW_RAM_SIZE                                  0x00800000
#define CS_TO_ADDRESS_SHIFT_COUNT                        27


/* --------------------------------------------------------------------------
   PAD HDRIVE SEL Initial values.
-------------------------------------------------------------------------- */

#define BOOTHW_PAD_HDRIVE_SEL0_INIT                      0x000400
#define BOOTHW_PAD_HDRIVE_SEL1_INIT                      0x0


/* --------------------------------------------------------------------------
   AHB Bus Priority register configuration parameters.
-------------------------------------------------------------------------- */

/* Low-->Hi  SYS->APP->DMA = 0x24 */
/* Low-->Hi  SYS->DMA->APP = 0x18 */
/* Low-->Hi  APP->DMA->SYS = 0x12 */
/* Low-->Hi  DMA->APP->SYS = 0x06 */
  #define EBI1_AHB_BUS_PRIORITY_VAL                      0x24


/* --------------------------------------------------------------------------
   EBI2_CFG register configuration parameters.
-------------------------------------------------------------------------- */

/* ARM has higher priority than DSP on EBI2 bus. */

#define EBI2_PRIORITY_VAL                                0

/* --------------------------------------------------------------------------
   EBI chip select register configuration parameters.
-------------------------------------------------------------------------- */

/* Memory device page size. */
#ifdef T_PSRAM_32
  #define   EBI_PAGE_SIZE_0_WORDS                          0
  #define   EBI_PAGE_SIZE_2_WORDS                          1
  #define   EBI_PAGE_SIZE_4_WORDS                          2
  #define   EBI_PAGE_SIZE_8_WORDS                          3
  #define   EBI_PAGE_SIZE_16_WORDS                         4
  #define   EBI_PAGE_SIZE_32_WORDS                         5
  #define   EBI_PAGE_SIZE_64_WORDS                         6
  #define   EBI_PAGE_SIZE_128_WORDS                        7
#else
  #define   EBI_PAGE_SIZE_0_WORDS                          0
  #define   EBI_PAGE_SIZE_4_WORDS                          1
  #define   EBI_PAGE_SIZE_8_WORDS                          2
  #define   EBI_PAGE_SIZE_16_WORDS                         3
  #define   EBI_PAGE_SIZE_32_WORDS                         4
  #define   EBI_PAGE_SIZE_64_WORDS                         5
  #define   EBI_PAGE_SIZE_128_WORDS                        6
  #define   EBI_PAGE_SIZE_256_WORDS                        7
#endif
/* AVD OE Recovery */

#define   EBI_AVD_OE_RECOVERY_VAL                        0x1

/* ADDR_HOLD_ENA */

#define   EBI_ADDR_HOLD_ENA_DISABLE                      0
#define   EBI_ADDR_HOLD_ENA_ENABLE                       1

/* ENA_x32_INTERFACE. */

#define   EBI_ENA_X32_INTERFACE_DISABLE                  0
#define   EBI_ENA_X32_INTERFACE_ENABLE                   1

/* AVD signal access state. */

#define   EBI_AVD_N_ACCESS_STATE_LOW                     0
#define   EBI_AVD_N_ACCESS_STATE_HIGH                    1

/* AVD Pulse Enable. */

#define   EBI_AVD_PULSE_ENA_DISABLE                      0
#define   EBI_AVD_PULSE_ENA_ENABLE                       1

/* Write protect enable. */

#define   EBI_WRITE_PROTECT_DISABLED                     0
#define   EBI_WRITE_PROTECT_ENABLED                      1

/* Wrap8 mode enable. */

#define   EBI_WRAP8_MODE_DISABLED                        0
#define   EBI_INCR_MODE_ENABLED                          0
#define   EBI_WRAP8_MODE_ENABLED                         1

/* Burst read enable. */

#define   EBI_BURST_READ_DISABLED                        0
#define   EBI_BURST_READ_ENABLED                         1

/* Burst write enable. */

#define   EBI_BURST_WRITE_DISABLED                       0
#define   EBI_BURST_WRITE_ENABLED                        1

/* Page read enable. */

#define   EBI_PAGE_READ_DISABLED                         0
#define   EBI_PAGE_READ_ENABLED                          1

/* Byte access mode enable. */

#define   EBI_BYTE_MODE_DISABLED                         0
#define   EBI_BYTE_MODE_ENABLED                          1

/* LCD interface type. */

#define   EBI_LCD_INTERFACE_TYPE_INTEL                   0
#define   EBI_LCD_INTERFACE_TYPE_MOTOROLA                1


/*---------------------------------------------------------------------------
   MSM_CLK_SLEEPOSC
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
//      Bit [6:0] RESERVED

/*-------------------------------------------------------------------------*/
//      Bit [7] SLEEP_XTAL_EN, this bit specifies if the sleep controller is
//      to be run off of the SLEEP_XTAL_IN pin or TCXO/4
//DA In Jaguar, bit 7 of MPM_CLK_SLEEPOSC (SLEEP_XTAL_EN) is reserved (RESERVE
#define MPM_CLK_SLEEPOSC_WH__SLEEP_XTAL_EN_DIS              (0U <<  \
        HWIO_SHFT(MPM_CLK_SLEEPOSC,SLEEP_XTAL_EN))

#define MPM_CLK_SLEEPOSC_WH__SLEEP_XTAL_EN_ENA              (1U <<  \
        HWIO_SHFT(MPM_CLK_SLEEPOSC,SLEEP_XTAL_EN))

/*-------------------------------------------------------------------------*/
//      Bit [8] SLEEP_OSC_RD_BYPASS, this bit disables the output
//      resistance of the SLEEP_XTAL_OUT path
#define MPM_CLK_SLEEPOSC_WH__SLEEP_OSC_RD_BYPASS_ENA        (1U <<  \
        HWIO_SHFT(MPM_CLK_SLEEPOSC,SLEEP_OSC_RD_BYPASS))

#define MPM_CLK_SLEEPOSC_WH__SLEEP_OSC_RD_BYPASS_DIS        (0U <<  \
        HWIO_SHFT(MPM_CLK_SLEEPOSC,SLEEP_OSC_RD_BYPASS))

/*-------------------------------------------------------------------------*/
//      Bit [9] SLEEP_OSC_RF_BYPASS, this bit disables the internal
//      resistance on the schmitt trigger path
#define MPM_CLK_SLEEPOSC_WH__SLEEP_OSC_RF_BYPASS_ENA        (1U <<  \
        HWIO_SHFT(MPM_CLK_SLEEPOSC,SLEEP_OSC_RF_BYPASS))

#define MPM_CLK_SLEEPOSC_WH__SLEEP_OSC_RF_BYPASS_DIS        (0U <<  \
        HWIO_SHFT(MPM_CLK_SLEEPOSC,SLEEP_OSC_RF_BYPASS))

/*-------------------------------------------------------------------------*/
//      Bit [11:10] SLEEP_OSC_GAIN, configure gain of internal
//      oscillator circuit
#define MPM_CLK_SLEEPOSC_WH__SLEEP_OSC_GAIN_00             (0U <<  \
        HWIO_SHFT(MPM_CLK_SLEEPOSC,SLEEP_OSC_GAIN))

#define MPM_CLK_SLEEPOSC_WH__SLEEP_OSC_GAIN_01             (1U <<  \
        HWIO_SHFT(MPM_CLK_SLEEPOSC,SLEEP_OSC_GAIN))

#define MPM_CLK_SLEEPOSC_WH__SLEEP_OSC_GAIN_10             (2U <<  \
        HWIO_SHFT(MPM_CLK_SLEEPOSC,SLEEP_OSC_GAIN))

#define MPM_CLK_SLEEPOSC_WH__SLEEP_OSC_GAIN_11             (3U <<  \
        HWIO_SHFT(MPM_CLK_SLEEPOSC,SLEEP_OSC_GAIN))


/* --------------------------------------------------------------------------
   Memory component attribute type definitions.

   The following types are used to specify particular attributes of each
   memory component that is supported. This attribute information is used
   to configure the EBI1 and EBI2 external memory interface hardware.
-------------------------------------------------------------------------- */

/* Memory component emum type. */

typedef enum
{
  EBI_MEM_TYPE_NONE,
  EBI_MEM_TYPE_NOR_FLASH,
  EBI_MEM_TYPE_NAND_FLASH,
  EBI_MEM_TYPE_SRAM,
  EBI_MEM_TYPE_SDRAM,
  EBI_MEM_TYPE_PSRAM
} boothw_mem_device_type;

/* Display component enum type. */

typedef enum
{
  EBI_LCD_TYPE_EPSON_176x220,
  EBI_LCD_TYPE_TMD_QVGA_240x320
} boothw_lcd_device_type;

/* Component access enum type. */

typedef enum
{
  EBI_ACCESS_TYPE_ASYNC,
  EBI_ACCESS_TYPE_PAGE,
  EBI_ACCESS_TYPE_BURST,
  EBI_ACCESS_TYPE_SYNC
} boothw_access_type;

/* Component data width enum type. */

typedef enum
{
  EBI_DATA_WIDTH_8_BITS,
  EBI_DATA_WIDTH_16_BITS,
  EBI_DATA_WIDTH_32_BITS
} boothw_data_width_type;

typedef enum
{
  EBI_LCD_DATA_WIDTH_16_BITS,
  EBI_LCD_DATA_WIDTH_18_BITS
} boothw_lcd_data_width_type;



/* --------------------------------------------------------------------------
   Memory component configuration type definitions.

   The information specified in the following types are used to program the
   EBI1 and EBI2 chip select configuration registers.  For more details,
   refer to the MSM6550 software interface specification.

   The memory timing data in the following types are specified in nanoseconds
   and must specify total memory access time in accordance with MSM memory
   access timing specifications.  Note that a value of at least '1' will
   ensure that at least one cycle will be allocated for the corresponding
   attribute, regardless of the CPU clock frequency.
-------------------------------------------------------------------------- */

/* EBI1 memory component configuration type. */

typedef struct
{
  boothw_mem_device_type     device_type;            /* Memory type        */
  boothw_access_type         access_type;            /* Access type        */
  boothw_data_width_type     data_width;             /* Data width         */
  uint8                      page_size;              /* Page size          */
  uint8                      cs_precharge_ns;        /* CS setup time      */
  uint8                      cs_recovery_ns;         /* CS recovery time   */
  uint8                      first_rd_ns;            /* First read time    */
  uint8                      second_rd_ns;           /* Second read time   */
  uint8                      first_wr_ns;            /* First write time   */
  uint8                      second_wr_ns;           /* Second write time  */
  uint8                      write_hold_ns;          /* Write hold time    */
  uint8                      avd_recovery_ns;        /* AVD recovery time  */
  uint8                      avd_state;              /* AVD access state   */
  uint8                      write_prot_ena;         /* Write protect      */
  uint8                      wrap8_mode_ena;         /* Wrap8 mode         */
} boothw_ebi1_memory_type;

/* EBI2 memory component configuration type. */

typedef struct
{
  boothw_mem_device_type     device_type;            /* Memory type        */
  boothw_access_type         access_type;            /* Access type        */
  boothw_data_width_type     data_width;             /* Data width         */
  uint8                      page_size;              /* Page size          */
  uint8                      cs_setup_ns;            /* CS setup time      */
  uint8                      cs_recovery_ns;         /* CS recovery time   */
  uint8                      first_rd_ns;            /* First read time    */
  uint8                      second_rd_ns;           /* Second read time   */
  uint8                      read_hold_ns;           /* Read hold time     */
  uint8                      first_wr_ns;            /* First write time   */
  uint8                      second_wr_ns;           /* Second write time  */
  uint8                      write_hold_ns;          /* Write hold time    */
  uint8                      write_prot_ena;         /* Write protect      */
  uint8                      byte_mode_ena;          /* Byte mode enable   */
} boothw_ebi2_memory_type;

/* EBI2 LCD component configuration type. */

typedef struct
{
  boothw_lcd_device_type     device_type;            /* Display type       */
  boothw_lcd_data_width_type data_width;             /* Data width         */
  uint8                      cs_setup_ns;            /* CS setup time      */
  uint8                      cs_recovery_ns;         /* CS recovery time   */
  uint8                      first_rd_ns;            /* Read access time   */
  uint8                      read_hold_ns;           /* Read hold time     */
  uint8                      first_wr_ns;            /* Write access time  */
  uint8                      write_hold_ns;          /* Write hold time    */
  uint8                      lcde_rd_setup_ns;       /* LCD_E read setup   */
  uint8                      lcde_wr_setup_ns;       /* LCD_E write setup  */
  uint8                      lcde_rd_high_ns;        /* LCD_E read high    */
  uint8                      lcde_wr_high_ns;        /* LCD_E write high   */
  uint8                      write_prot_ena;         /* Write protect      */
  uint8                      byte_mode_ena;          /* Byte mode enable   */
  uint8                      interface_type;         /* LCD interface type */
} boothw_ebi2_lcd_type;


/* --------------------------------------------------------------------------
   Memory component configuration definitions.

   The following define the access characteristics for the memory and LCD
   components that are supported by the SURF and FFA target hardware platforms.

   The memory component configurations defined below conform with the memory
   component configuration types that are defined above.  The following
   memory component definitions can be used as a reference for the definition
   of additional target specific memory components.
-------------------------------------------------------------------------- */

/* EBI1 - Unused chip select. */

#define EBI1_CS_UNUSED                                                      \
{                                                                           \
  EBI_MEM_TYPE_NONE,                            /* Memory type        */    \
  EBI_ACCESS_TYPE_ASYNC,                        /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
  0,                                            /* CS setup time      */    \
  0,                                            /* CS recovery time   */    \
  0,                                            /* First read time    */    \
  0,                                            /* Second read time   */    \
  0,                                            /* First write time   */    \
  0,                                            /* Second write time  */    \
  0,                                            /* Write hold time    */    \
  0,                                            /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                   /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_WRAP8_MODE_DISABLED,                      /* Wrap8 mode         */    \
}

/* EBI2 - Unused chip select. */

#define EBI2_CS_UNUSED                                                      \
{                                                                           \
  EBI_MEM_TYPE_NONE,                            /* Memory type        */    \
  EBI_ACCESS_TYPE_ASYNC,                        /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
  0,                                            /* CS setup time      */    \
  0,                                            /* CS recovery time   */    \
  0,                                            /* First read time    */    \
  0,                                            /* Second read time   */    \
  0,                                            /* Read hold time     */    \
  0,                                            /* First write time   */    \
  0,                                            /* Second write time  */    \
  0,                                            /* Write hold time    */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_BYTE_MODE_DISABLED                        /* Byte mode enable   */    \
}

/* EBI1 - Burst NOR Flash - AMD Am29BDS128G. */

#define EBI1_AMD_AM29BDS128_ASYNC_NOR_FLASH                                 \
{                                                                           \
  EBI_MEM_TYPE_NOR_FLASH,                       /* Memory type        */    \
  EBI_ACCESS_TYPE_ASYNC,                        /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
    0,                                          /* CS setup time      */    \
   40,                                          /* CS recovery time   */    \
  110,                                          /* First read time    */    \
   19,                                          /* Second read time   */    \
  110,                                          /* First write time   */    \
   19,                                          /* Second write time  */    \
    1,                                          /* Write hold time    */    \
    0,                                          /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                   /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_WRAP8_MODE_ENABLED,                       /* Wrap8 mode         */    \
}


/* EBI1 - Shadow PSRAM - Toshiba TC51WKM616. */

#define EBI1_TOSHIBA_TC51WKM616_SHADOW_PSRAM                                \
{                                                                           \
  EBI_MEM_TYPE_PSRAM,                           /* Memory type        */    \
  EBI_ACCESS_TYPE_ASYNC,                        /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
    1,                                          /* CS setup time      */    \
   40,                                          /* CS recovery time   */    \
  120,                                          /* First read time    */    \
   40,                                          /* Second read time   */    \
  120,                                          /* First write time   */    \
   40,                                          /* Second write time  */    \
    1,                                          /* Write hold time    */    \
    0,                                          /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                   /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_WRAP8_MODE_DISABLED,                      /* Wrap8 mode         */    \
}

/* EBI1 - Data PSRAM - Toshiba TC51WKM616. */

#define EBI1_TOSHIBA_TC51WKM616_DATA_PSRAM                                  \
{                                                                           \
  EBI_MEM_TYPE_PSRAM,                           /* Memory type        */    \
  EBI_ACCESS_TYPE_PAGE,                         /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_16_WORDS,                       /* Page size          */    \
    1,                                          /* CS setup time      */    \
   40,                                          /* CS recovery time   */    \
  120,                                          /* First read time    */    \
   40,                                          /* Second read time   */    \
  120,                                          /* First write time   */    \
   40,                                          /* Second write time  */    \
    1,                                          /* Write hold time    */    \
    0,                                          /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                   /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_WRAP8_MODE_DISABLED,                      /* Wrap8 mode         */    \
}

/* EBI1 - Burst PSRAM for SHADOW - Micron MT45W4Mx16BFB. */
#define EBI1_MICRON_MT45W4Mx16BFB_SHADOW_PSRAM                              \
{                                                                           \
  EBI_MEM_TYPE_PSRAM,                           /* Memory type        */    \
  EBI_ACCESS_TYPE_ASYNC,                        /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
    1,                                          /* CS setup time      */    \
   40,                                          /* CS recovery time   */    \
  120,                                          /* First read time    */    \
   40,                                          /* Second read time   */    \
  120,                                          /* First write time   */    \
   40,                                          /* Second write time  */    \
    1,                                          /* Write hold time    */    \
    0,                                          /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                   /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_WRAP8_MODE_DISABLED,                      /* Wrap8 mode         */    \
}

/* EBI1 - Burst PSRAM - Micron MT45W4Mx16BFB. */
#define EBI1_MICRON_MT45W4Mx16BFB_PSRAM                                     \
{                                                                           \
  EBI_MEM_TYPE_PSRAM,                           /* Memory type        */    \
  EBI_ACCESS_TYPE_BURST,                        /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_128_WORDS,                      /* Page size          */    \
    1,                                          /* CS setup time      */    \
   40,                                          /* CS recovery time   */    \
  120,                                          /* First read time    */    \
   10,                                          /* Second read time   */    \
  120,                                          /* First write time   */    \
   40,                                          /* Second write time  */    \
    1,                                          /* Write hold time    */    \
    0,                                          /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                   /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_INCR_MODE_ENABLED,                        /* INCR mode          */    \
}

/* EBI1 - Burst PSRAM - Micron MT45W4Mx16BFB. */
#define EBI1_MICRON_MT45W4Mx16BFB_32_PSRAM                                     \
{                                                                           \
  EBI_MEM_TYPE_PSRAM,                           /* Memory type        */    \
  EBI_ACCESS_TYPE_BURST,                        /* Access type        */    \
  EBI_DATA_WIDTH_32_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_128_WORDS,                      /* Page size          */    \
    1,                                          /* CS setup time      */    \
   40,                                          /* CS recovery time   */    \
  120,                                          /* First read time    */    \
   10,                                          /* Second read time   */    \
  120,                                          /* First write time   */    \
   40,                                          /* Second write time  */    \
    1,                                          /* Write hold time    */    \
    0,                                          /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                  /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_WRAP8_MODE_DISABLED,                      /* Wrap8 mode         */    \
}

/* EBI1 - 16-bit SDRAM - Elpida EDL1216CA - SURF target. */

#define EBI1_ELPIDA_EDL1216CA_16BIT_SDRAM                                   \
{                                                                           \
  EBI_MEM_TYPE_SDRAM,                           /* Memory type        */    \
  EBI_ACCESS_TYPE_SYNC,                         /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
    0,                                          /* CS setup time      */    \
   40,                                          /* CS recovery time   */    \
    0,                                          /* First read time    */    \
    0,                                          /* Second read time   */    \
    0,                                          /* First write time   */    \
    0,                                          /* Second write time  */    \
    0,                                          /* Write hold time    */    \
    0,                                          /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                   /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_WRAP8_MODE_DISABLED,                      /* Wrap8 mode         */    \
}

/* EBI1 - 32-bit SDRAM - Elpida EDL1216CA - SURF target. */

#define EBI1_ELPIDA_EDL1216CA_32BIT_SDRAM                                   \
{                                                                           \
  EBI_MEM_TYPE_SDRAM,                           /* Memory type        */    \
  EBI_ACCESS_TYPE_SYNC,                         /* Access type        */    \
  EBI_DATA_WIDTH_32_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
    0,                                          /* CS setup time      */    \
   40,                                          /* CS recovery time   */    \
    0,                                          /* First read time    */    \
    0,                                          /* Second read time   */    \
    0,                                          /* First write time   */    \
    0,                                          /* Second write time  */    \
    0,                                          /* Write hold time    */    \
    0,                                          /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                   /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_WRAP8_MODE_DISABLED,                      /* Wrap8 mode         */    \
}

/* EBI1 - QUASAR SRAM */

#define EBI1_QUASAR_SRAM                                                    \
{                                                                           \
  EBI_MEM_TYPE_SRAM,                            /* Memory type        */    \
  EBI_ACCESS_TYPE_ASYNC,                        /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
    0,                                          /* CS setup time      */    \
   20,                                          /* CS recovery time   */    \
   60,                                          /* First read time    */    \
   40,                                          /* Second read time   */    \
   60,                                          /* First write time   */    \
   40,                                          /* Second write time  */    \
    1,                                          /* Write hold time    */    \
    0,                                          /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                   /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_WRAP8_MODE_DISABLED,                      /* Wrap8 mode         */    \
}

/* EBI1 - QUASAR FLASH */

#define EBI1_QUASAR_FLASH                                                   \
{                                     \
  EBI_MEM_TYPE_NOR_FLASH,                       /* Memory type        */    \
  EBI_ACCESS_TYPE_ASYNC,                        /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
    0,                                          /* CS setup time      */    \
   40,                                          /* CS recovery time   */    \
  114,                                          /* First read time    */    \
  114,                                          /* Second read time   */    \
  114,                                          /* First write time   */    \
  114,                                          /* Second write time  */    \
    1,                                          /* Write hold time    */    \
    0,                                          /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                   /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_WRAP8_MODE_DISABLED,                      /* Wrap8 mode         */    \
}

/* EBI2 - 8-bit NAND flash - Samsung K9F5608Q0B - SURF target. */

#define EBI2_SAMSUNG_K9F5608Q0B_NAND_FLASH                                  \
{                                                                           \
  EBI_MEM_TYPE_NAND_FLASH,                      /* Memory type        */    \
  EBI_ACCESS_TYPE_ASYNC,                        /* Access type        */    \
  EBI_DATA_WIDTH_8_BITS,                        /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
    0,                                          /* CS setup time      */    \
    0,                                          /* CS recovery time   */    \
  100,                                          /* First read time    */    \
  100,                                          /* Second read time   */    \
    0,                                          /* Read hold time     */    \
  100,                                          /* First write time   */    \
  100,                                          /* Second write time  */    \
    1,                                          /* Write hold time    */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_BYTE_MODE_DISABLED                        /* Byte mode enable   */    \
}

#define EBI2_CS2_EPSON_TBD_176x220_COLOR_LCD                                \
{                                                                           \
  EBI_MEM_TYPE_PSRAM,                           /* Memory type        */    \
  EBI_ACCESS_TYPE_ASYNC,                        /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
    0,                                          /* CS setup time      */    \
    0,                                          /* CS recovery time   */    \
  100,                                          /* First read time    */    \
  100,                                          /* Second read time   */    \
   35,                                          /* Read hold time     */    \
   35,                                          /* First write time   */    \
   35,                                          /* Second write time  */    \
   25,                                          /* Write hold time    */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_BYTE_MODE_DISABLED                        /* Byte mode enable   */    \
}

#define EBI2_LCD_TYPE                                                       \
{                                                                           \
 EBI_LCD_TYPE_TMD_QVGA_240x320,                 /* Display type       */    \
 EBI_LCD_DATA_WIDTH_18_BITS,                    /* Data width         */    \
   0,                                           /* CS setup time      */    \
   0,                                           /* CS recovery time   */    \
 100,                                           /* Read access time   */    \
  35,                                           /* Read hold time     */    \
  35,                                           /* Write access time  */    \
  25,                                           /* Write hold time    */    \
   0,                                           /* LCD_E read setup   */    \
   0,                                           /* LCD_E write setup  */    \
   0,                                           /* LCD_E read high    */    \
   0,                                           /* LCD_E write high   */    \
 EBI_WRITE_PROTECT_DISABLED,                    /* Write protect      */    \
 EBI_BYTE_MODE_DISABLED,                        /* Byte mode enable   */    \
 EBI_LCD_INTERFACE_TYPE_INTEL                   /* LCD interface type */    \
}

/* EBI2 - 176x220 Color LCD - Epson TBD - SURF and FFA targets. */
#define EBI2_EPSON_TBD_176x220_COLOR_LCD                                    \
{                                                                           \
 EBI_LCD_TYPE_EPSON_176x220,                    /* Display type       */    \
 EBI_LCD_DATA_WIDTH_16_BITS,                    /* Data width         */    \
   0,                                           /* CS setup time      */    \
   0,                                           /* CS recovery time   */    \
 100,                                           /* Read access time   */    \
  35,                                           /* Read hold time     */    \
  35,                                           /* Write access time  */    \
  25,                                           /* Write hold time    */    \
   0,                                           /* LCD_E read setup   */    \
   0,                                           /* LCD_E write setup  */    \
   0,                                           /* LCD_E read high    */    \
   0,                                           /* LCD_E write high   */    \
 EBI_WRITE_PROTECT_DISABLED,                    /* Write protect      */    \
 EBI_BYTE_MODE_DISABLED,                        /* Byte mode enable   */    \
 EBI_LCD_INTERFACE_TYPE_INTEL                   /* LCD interface type */    \
}

#define EBI2_SURF_FPGA                                                      \
{                                                                           \
  EBI_MEM_TYPE_NONE,                            /* Memory type        */    \
  EBI_ACCESS_TYPE_ASYNC,                        /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
  0,                                            /* CS setup time      */    \
  0,                                            /* CS recovery time   */    \
  60,                                           /* First read time    */    \
  60,                                           /* Second read time   */    \
  0,                                            /* Read hold time     */    \
  60,                                           /* First write time   */    \
  60,                                           /* Second write time  */    \
  0,                                            /* Write hold time    */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_BYTE_MODE_DISABLED                        /* Byte mode enable   */    \
}

#define EBI1_AMD_AM29BDS128_BURST_NOR_FLASH                                 \
{                                                                           \
  EBI_MEM_TYPE_NOR_FLASH,                       /* Memory type        */    \
  EBI_ACCESS_TYPE_BURST,                        /* Access type        */    \
  EBI_DATA_WIDTH_16_BITS,                       /* Data width         */    \
  EBI_PAGE_SIZE_0_WORDS,                        /* Page size          */    \
    0,                                          /* CS setup time      */    \
   40,                                          /* CS recovery time   */    \
  110,                                          /* First read time    */    \
   19,                                          /* Second read time   */    \
  110,                                          /* First write time   */    \
   19,                                          /* Second write time  */    \
    1,                                          /* Write hold time    */    \
    0,                                          /* AVD recovery time  */    \
  EBI_AVD_N_ACCESS_STATE_LOW,                   /* AVD access state   */    \
  EBI_WRITE_PROTECT_DISABLED,                   /* Write protect      */    \
  EBI_WRAP8_MODE_ENABLED,                       /* Wrap8 mode         */    \
}


/* --------------------------------------------------------------------------
   Memory bus configuration type definitions.

   The information specified in the following types are used to define the
   target EBI1 and EBI2 external memory bus architecture.
-------------------------------------------------------------------------- */

/* EBI1 memory bus - four memory chip selects. */

typedef struct
{
  boothw_ebi1_memory_type  cs0;
  boothw_ebi1_memory_type  cs1;
  boothw_ebi1_memory_type  cs2;
  boothw_ebi1_memory_type  cs3;
} boothw_ebi1_bus_type;

/* EBI2 memory bus - four memory chip selects and one LCD chip select. */

typedef struct
{
  boothw_ebi2_memory_type  cs0;
  boothw_ebi2_memory_type  cs1;
  boothw_ebi2_memory_type  cs2;
  boothw_ebi2_memory_type  cs3;
  boothw_ebi2_lcd_type     lcd;
} boothw_ebi2_bus_type;


/* --------------------------------------------------------------------------
   Memory bus configuration definitions.

   The following selects the specific target EBI1 and EBI2 external memory
   bus architecture based on the compile time feature definitions.

   The memory bus configuration definitions below conform with the memory bus
   configuration types that are defined above.  The following memory bus
   configuration definitions can be used as a reference for the definition of
   additional target specific memory bus configurations.
-------------------------------------------------------------------------- */

#if defined ( T_QUASAR )
#error code not present
#elif defined ( T_SURF )

  #if defined ( BOOT_MODE_NOR )

    #if defined ( SHADOW_MODE )

      /* --------------------------------------------------------------------
         SURF6550 Target: Shadow PSRAM + Data PSRAM Configuration.

         EBI1 Shadow PSRAM + EBI1 Data PSRAM + EBI1 BURST NOR Flash.
      -------------------------------------------------------------------- */

      #define  EBI1_MEMORY_BUS_CFG               \
      {                                          \
        EBI1_MICRON_MT45W4Mx16BFB_SHADOW_PSRAM,  \
        EBI1_MICRON_MT45W4Mx16BFB_32_PSRAM,      \
        EBI1_CS_UNUSED,      \
        EBI1_CS_UNUSED       \
      }

      #define  EBI2_MEMORY_BUS_CFG               \
      {                                          \
        EBI2_SAMSUNG_K9F5608Q0B_NAND_FLASH,      \
        EBI2_SURF_FPGA,                          \
        EBI2_CS_UNUSED,                          \
        EBI2_CS2_EPSON_TBD_176x220_COLOR_LCD,    \
        EBI2_LCD_TYPE                            \
      }

    #else /* ! SHADOW_MODE */

      /* --------------------------------------------------------------------
         SURF Target: Burst NOR + Data PSRAM Configuration.

         EBI1 Burst NOR Flash + EBI1 Data PSRAM + EBI1 Burst NOR Flash.
      -------------------------------------------------------------------- */
      #ifdef T_PSRAM_32
        #define  EBI1_MEMORY_BUS_CFG             \
        {                                        \
          EBI1_AMD_AM29BDS128_BURST_NOR_FLASH,   \
          EBI1_MICRON_MT45W4Mx16BFB_32_PSRAM,    \
          EBI1_AMD_AM29BDS128_BURST_NOR_FLASH,   \
          EBI1_CS_UNUSED                         \
        }
      #else

        #ifdef FEATURE_SHADOW_RAM_HEAP
          #define  EBI1_MEMORY_BUS_CFG             \
          {                                        \
            EBI1_AMD_AM29BDS128_BURST_NOR_FLASH,   \
            EBI1_MICRON_MT45W4Mx16BFB_PSRAM,       \
            EBI1_AMD_AM29BDS128_BURST_NOR_FLASH,   \
            EBI1_MICRON_MT45W4Mx16BFB_PSRAM        \
          }
        #else
        #define  EBI1_MEMORY_BUS_CFG             \
        {                                        \
          EBI1_AMD_AM29BDS128_BURST_NOR_FLASH,   \
          EBI1_MICRON_MT45W4Mx16BFB_PSRAM,       \
          EBI1_AMD_AM29BDS128_BURST_NOR_FLASH,   \
          EBI1_CS_UNUSED                         \
        }
      #endif
      #endif

      #define  EBI2_MEMORY_BUS_CFG               \
      {                                          \
        EBI2_SAMSUNG_K9F5608Q0B_NAND_FLASH,      \
        EBI2_SURF_FPGA,                          \
        EBI2_CS_UNUSED,          \
        EBI2_CS2_EPSON_TBD_176x220_COLOR_LCD,    \
        EBI2_LCD_TYPE         \
      }
    #endif /* SHADOW_MODE */

  #elif defined ( BOOT_MODE_NAND )

    #if defined ( T_SDRAM_IS_32_BIT )

      /* --------------------------------------------------------------------
         SURF Target: 32-bit SDRAM + NAND Flash Configuration.

         EBI1 32-bit SDRAM + EBI2 NAND Flash.
      -------------------------------------------------------------------- */

      #define  EBI1_MEMORY_BUS_CFG               \
      {                                          \
        EBI1_CS_UNUSED,                          \
        EBI1_CS_UNUSED,                          \
        EBI1_ELPIDA_EDL1216CA_32BIT_SDRAM,       \
        EBI1_CS_UNUSED                           \
      }

      #define  EBI2_MEMORY_BUS_CFG               \
      {                                          \
        EBI2_SAMSUNG_K9F5608Q0B_NAND_FLASH,      \
        EBI2_SURF_FPGA,                          \
        EBI2_CS_UNUSED,                          \
        EBI2_CS2_EPSON_TBD_176x220_COLOR_LCD,    \
        EBI2_LCD_TYPE                            \
      }
    #else /* ! T_SDRAM_IS_32_BIT */

      /* --------------------------------------------------------------------
         SURF Target: 16-bit SDRAM + NAND Flash Configuration.

         EBI1 16-bit SDRAM + EBI2 NAND Flash.
      -------------------------------------------------------------------- */

      #define  EBI1_MEMORY_BUS_CFG               \
      {                                          \
        EBI1_CS_UNUSED,                          \
        EBI1_CS_UNUSED,                          \
        EBI1_ELPIDA_EDL1216CA_16BIT_SDRAM,       \
        EBI1_CS_UNUSED                           \
      }

      #define  EBI2_MEMORY_BUS_CFG               \
      {                                          \
        EBI2_SAMSUNG_K9F5608Q0B_NAND_FLASH,      \
        EBI2_SURF_FPGA,                          \
        EBI2_CS_UNUSED,                          \
        EBI2_CS2_EPSON_TBD_176x220_COLOR_LCD,    \
        EBI2_LCD_TYPE                            \
      }
    #endif /* T_SDRAM_IS_32_BIT */

  #else
    #error  Must specify boot mode for SURF target.
  #endif

#elif defined ( T_FFA )
#error code not present
#else
  #error  Must specify a target.
#endif

#endif /*BOOTHWI_6800_H*/

