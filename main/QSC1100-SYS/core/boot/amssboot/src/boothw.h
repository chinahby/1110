#ifndef BOOTHW_H
#define BOOTHW_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      B O O T  H A R D W A R E  I N I T I A L I Z A T I O N   H E A D E R

GENERAL DESCRIPTION
  This header file contains the definitions of the MSM2.x memory and peripheral
  control hardware registers and reserved memory locations.

Copyright (c) 2004, 2005,2006 by QUALCOMM Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/boothw.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/05/06   ty      Changes to read flash information from config data
05/12/06   rjain   Added interface functions to cfgdata-clkregim APIs
12/09/05   tkuo    Add memory configuration type.
10/18/05   rp      Moved boothw_boot_mode_enum_type  to this file from 
                   boothw_6800.h
12/01/04   tkuo    Ported from MSM6250.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#if !defined(BUILD_BOOT_CHAIN) || defined(FSBL) || defined (OSBL)
#include  "msm.h"
#include  BOOTHW_MSM_H
#endif

#define AMSS_BOOT_MODE_NAND 0x00000001UL /* Boot from NAND */
#define AMSS_BOOT_MODE_NOR  0x00000000UL /* Boot from NOR  */


/*************************************************/
/* Enum for bus frequencies that we can move to  */
/* If this is modified hclk_freq_translation_khz */
/* must be modified also                         */
/*************************************************/
typedef enum hclk_freq_enum
{
  BOOT_HCLK_19P2,
  BOOT_HCLK_41,
  BOOT_HCLK_49,
  BOOT_HCLK_61,
  BOOT_HCLK_73,
  BOOT_HCLK_90,
  BOOT_HCLK_LAST_FREQ
} hclk_freq_enum;


typedef enum
{
    BOOTMODE_NOR_NON_TRUSTED,
    BOOTMODE_NAND_NON_TRUSTED,
    BOOTMODE_NOR_TRUSTED,
    BOOTMODE_NAND_TRUSTED,

}boothw_boot_mode_enum_type;

/* Type of which FLASH and RAM configuration */
typedef enum 
{
  BOOT_NAND_SDRAM_16_PRESENT =0,
  BOOT_NAND_512_SDRAM_16_PRESENT = BOOT_NAND_SDRAM_16_PRESENT,
  BOOT_NAND_SDRAM_32_PRESENT,
  BOOT_NAND_512_SDRAM_32_PRESENT = BOOT_NAND_SDRAM_32_PRESENT,
  BOOT_NOR_16_PSRAM_16_PRESENT,
  BOOT_NOR_16_PSRAM_32_PRESENT,
  BOOT_NOR_16_SDRAM_16_PRESENT,
  BOOT_NOR_16_SDRAM_32_PRESENT,
  BOOT_NAND_2048_SDRAM_16_PRESENT,
  BOOT_NAND_2048_SDRAM_32_PRESENT,
  BOOT_ONENAND_2048_SDRAM_16_PRESENT,
  BOOT_ONENAND_2048_SDRAM_32_PRESENT,
  BOOT_MAX_PRESENT=0x7FFFFFFF
}boot_flash_mem_cfg_type;
/* For compatability with non-compliant naming used in 6250 i2c.c */
#define BOOTHW_BootModeType boothw_boot_mode_enum_type


/*===========================================================================
                      FUNCTION DECLARATIONS

=============================================================================*/


/*===========================================================================

  FUNCTION boot_hw_get_bootmode()

  DESCRIPTION
   Returns the bootmode
  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    Bootmode.

  SIDE EFFECTS

===========================================================================*/
boothw_boot_mode_enum_type boot_hw_get_bootmode(void );


#ifndef BUILD_BOOT_CHAIN

/*===========================================================================

  FUNCTION BOOT_HW_CALCULATE_WAIT_STATES

  DESCRIPTION
    Stores memory wait states so we don't have to calculate them later
    Need to be called after RAM is initialized
    
  PARAMETERS
    Clock frequency in khz.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void boot_hw_calculate_wait_states(void);


/*===========================================================================

  FUNCTION boot_hw_use_calculated_wait_states

  DESCRIPTION
    Set up memory wait states using wait states that were calculated before
    
  PARAMETERS
    hclk_freq_enum hclk_freq_index - frequencies to switch to
    
  DEPENDENCIES
    boot_hw_calculate_wait_states must be called before this function is used

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void boot_hw_use_calculated_wait_states
(
   hclk_freq_enum hclk_freq_index                   
);

/*===========================================================================

  FUNCTION boot_hw_ctrl_init()

  DESCRIPTION
    Initializes MSM control registers to enable access to the entire
    target ROM and RAM address space.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The MSM chip is initialized.
===========================================================================*/
extern void boot_hw_ctrl_init(void);

/*===========================================================================

  FUNCTION BOOT_HW_CONFIGURE_WAIT_STATES

  DESCRIPTION
    Setup memory wait states.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void boot_hw_configure_wait_states
(
   uint32                    clock_frequency
);

/*===========================================================================

  FUNCTION boot_hw_tlmm_init()

  DESCRIPTION
    Initializes GPIO

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The MSM chip is initialized.
===========================================================================*/
uint32 boot_hw_tlmm_init(void);

#endif /* BUILD_BOOT_CHAIN */
#endif /*BOOTHW_H*/
