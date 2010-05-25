#ifndef ARM_MMU_TABLE_H
#define ARM_MMU_TABLE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  ARM9 QSC1100 MMU Table

GENERAL DESCRIPTION
        This module contains memory map for ARM9 QSC1100 MMU table.
        The constants declared within this file are statically included
        into ARMMMU.C

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2009 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/boot/arm_mmu_table.h#15 $

when          who       what, where, why
--------      ---       ----------------------------------------------------------
05/22/09      dk        Fix for proper featurization and secure boot workaround.
05/21/09      dk        To provide cache write through regions for debugging purposes.
04/01/09      ddk       Adding workaround to fix the flash write issue seen in the 
                        NOR area during boot code execution when data cache is 
                        enabled.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Local used data tables, the linker is instructed to place these
** at a specific address; that address is located as a constant in
** the builds description file.
 */

#pragma arm section zidata = "PAGE_TABLES"
static uint32 PAGETABLE_L1[MMU_PT_SIZE];
static uint32 PAGETABLE_L2[MMU_NUM_COARSE_PT][MMU_COARSE_PT_SIZE];
#pragma arm section zidata

#if ((((MMU_PT_SIZE + MMU_NUM_COARSE_PT * MMU_COARSE_PT_SIZE) * 4) % 4096) != 0)
#error allocation of space to page tables must be a multiple of 4KB
#endif

/*=============================================================================
Memory Map Structure
  The memory map structure is an array of mem blocks defined above.  It is used
  my the function create_pagetable to create the mmu pagetable.  By
  modifying the structure one can change the memory map of the mmu page table.
  Each entry represents a memory block and consists of a base address, size in
  megabytes, access type and domain permissions.

=============================================================================*/

/*=============================================================================
    memory map for nor or nand flash only.
=============================================================================*/

static const mem_block memory_memory_map[] =
{
   /* Section Mappings Come First
    */
/*
** Adding workaround to fix the flash write issue seen in the 
** NOR area during boot code execution when data cache is 
** enabled.
*/
#ifdef BUILD_BOOT_CHAIN
        /* EBI1 NOR */
        {
            MMU_MAPPING_SECTION,
            (uint32*)QSC11x0_MEMORY_0_BASE, (uint32*)QSC11x0_MEMORY_0_BASE, QSC11x0_MEMORY_0_SIZE,
            MMU_ACCESS_PRV_RW, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_CONTEXT_DEFAULT
        },
#else
        /* EBI1 NOR */
        {
            MMU_MAPPING_SECTION,
            (uint32*)QSC11x0_MEMORY_0_BASE, (uint32*)QSC11x0_MEMORY_0_BASE, QSC11x0_MEMORY_0_SIZE,
            MMU_ACCESS_PRV_RO, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_CONTEXT_DEFAULT
        },
#endif
	/* EBI1 NOR / EFS */
	{
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_MEMORY_EFS_BASE, (uint32*)QSC11x0_MEMORY_EFS_BASE, QSC11x0_MEMORY_EFS_SIZE,
		MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_EFS
	},

	/* EBI1 PSRAM */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_MEMORY_1_BASE, (uint32*)QSC11x0_MEMORY_1_BASE, QSC11x0_MEMORY_1_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_WRITE_BACK, MMU_DOMAIN_CONTEXT_DEFAULT
    },

	/* IMEM, L1 */
	{
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_IMEM_BASE, (uint32*)QSC11x0_IMEM_BASE, QSC11x0_IMEM_SIZE,
		MMU_ACCESS_PRV_RO, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_CONTEXT_DEFAULT
	},

	/* BOOT ROM */
	{
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_BOOT_ROM_BASE, (uint32*)QSC11x0_BOOT_ROM_BASE, QSC11x0_BOOT_ROM_SIZE,
		MMU_ACCESS_PRV_RO, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_CONTEXT_DEFAULT
	},

#if defined(SHADOW_MODE)

   /* Any SHADOW MODE override to the table should now be performed.
    * The target has been initilaized for the default operation. These
    * definitions can be used to override memory defaults previously
    * defined within this table.
    */

   /* CS2 NOR for EFS on Shadow RAM build */
   {
      MMU_MAPPING_SECTION,
      (uint32*)QSC11x0_MEMORY_2_BASE, (uint32*)QSC11x0_MEMORY_2_BASE, QSC11x0_MEMORY_2_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_CONTEXT_EFS
   },

#endif

   /* Coarse Mappings Come Second
    */

	/* EBI1 PSRAM, L2 */
	{
		MMU_MAPPING_COARSE,
		(uint32*)QSC11x0_PMEM_DATA_BASE, (uint32*)QSC11x0_PMEM_DATA_BASE, QSC11x0_PMEM_DATA_SIZE,
		MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_UNUSED
	},

	/* IMEM, L2 */
	{
		MMU_MAPPING_COARSE,
		(uint32*)QSC11x0_IMEM_BASE, (uint32*)QSC11x0_IMEM_BASE, QSC11x0_IMEM_SIZE,
		MMU_ACCESS_PRV_RW, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_UNUSED
	},

	/* PAGETABLES, L2 */
	{
		MMU_MAPPING_COARSE,
		(uint32*)QSC11x0_MEMORY_PT_BASE, (uint32*)QSC11x0_MEMORY_PT_BASE, QSC11x0_MEMORY_PT_SIZE,
		MMU_ACCESS_PRV_RO, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_UNUSED
	},

    /* End of Table */
    { 
		MMU_MAPPING_SECTION,
		(uint32*)MMU_LAST_ADDRESS, (uint32*)MMU_LAST_ADDRESS, 0,
		MMU_ACCESS_UNUSED, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_FAULT
    }
};





#if defined(FEATURE_CACHE_WT)

/* Memory regions MMU_ACCESS_PRV_RO, 
** MMU_ACCESS_PRV_RW, MMU_ACCESS_PRV_RW, 
** MMU_ACCESS_PRV_RO, MMU_ACCESS_PRV_RO 
** are Cache configured as write through for 
** debug support. 
*/

static const mem_block memory_memory_map_wt[] =
{
   /* Section Mappings Come First
    */
/*
** Adding workaround to fix the flash write issue seen in the 
** NOR area during boot code execution when data cache is 
** enabled.
*/
#ifdef BUILD_BOOT_CHAIN
        /* EBI1 NOR */
        {
            MMU_MAPPING_SECTION,
            (uint32*)QSC11x0_MEMORY_0_BASE, (uint32*)QSC11x0_MEMORY_0_BASE, QSC11x0_MEMORY_0_SIZE,
            MMU_ACCESS_PRV_RW, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_CONTEXT_DEFAULT
        },
#else
        /* EBI1 NOR */
        {
            MMU_MAPPING_SECTION,
            (uint32*)QSC11x0_MEMORY_0_BASE, (uint32*)QSC11x0_MEMORY_0_BASE, QSC11x0_MEMORY_0_SIZE,
            MMU_ACCESS_PRV_RO, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_CONTEXT_DEFAULT
        },
#endif

    /* EBI1 NOR / EFS */
    {
        MMU_MAPPING_SECTION,
        (uint32*)QSC11x0_MEMORY_EFS_BASE, (uint32*)QSC11x0_MEMORY_EFS_BASE, QSC11x0_MEMORY_EFS_SIZE,
        MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_EFS
    },

    /* EBI1 PSRAM */
    {
        MMU_MAPPING_SECTION,
        (uint32*)QSC11x0_MEMORY_1_BASE, (uint32*)QSC11x0_MEMORY_1_BASE, QSC11x0_MEMORY_1_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_CONTEXT_DEFAULT
    },

    /* IMEM, L1 */
    {
        MMU_MAPPING_SECTION,
        (uint32*)QSC11x0_IMEM_BASE, (uint32*)QSC11x0_IMEM_BASE, QSC11x0_IMEM_SIZE,
        MMU_ACCESS_PRV_RO, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_CONTEXT_DEFAULT
    },

    /* BOOT ROM */
    {
        MMU_MAPPING_SECTION,
        (uint32*)QSC11x0_BOOT_ROM_BASE, (uint32*)QSC11x0_BOOT_ROM_BASE, QSC11x0_BOOT_ROM_SIZE,
        MMU_ACCESS_PRV_RO, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_CONTEXT_DEFAULT
    },


#if defined(SHADOW_MODE)

   /* Any SHADOW MODE override to the table should now be performed.
    * The target has been initilaized for the default operation. These
    * definitions can be used to override memory defaults previously
    * defined within this table.
    */

   /* CS2 NOR for EFS on Shadow RAM build */
   {
      MMU_MAPPING_SECTION,
      (uint32*)QSC11x0_MEMORY_2_BASE, (uint32*)QSC11x0_MEMORY_2_BASE, QSC11x0_MEMORY_2_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_CONTEXT_EFS
   },

#endif

   /* Coarse Mappings Come Second
    */

    /* EBI1 PSRAM, L2 */
    {
        MMU_MAPPING_COARSE,
        (uint32*)QSC11x0_PMEM_DATA_BASE, (uint32*)QSC11x0_PMEM_DATA_BASE, QSC11x0_PMEM_DATA_SIZE,
        MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_UNUSED
    },

    /* IMEM, L2 */
    {
        MMU_MAPPING_COARSE,
        (uint32*)QSC11x0_IMEM_BASE, (uint32*)QSC11x0_IMEM_BASE, QSC11x0_IMEM_SIZE,
        MMU_ACCESS_PRV_RW, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_UNUSED
    },

    /* PAGETABLES, L2 */
    {
        MMU_MAPPING_COARSE,
        (uint32*)QSC11x0_MEMORY_PT_BASE, (uint32*)QSC11x0_MEMORY_PT_BASE, QSC11x0_MEMORY_PT_SIZE,
        MMU_ACCESS_PRV_RO, MMU_CACHE_WRITE_THRU, MMU_DOMAIN_UNUSED
    },

    /* End of Table */
    { 
        MMU_MAPPING_SECTION,
        (uint32*)MMU_LAST_ADDRESS, (uint32*)MMU_LAST_ADDRESS, 0,
        MMU_ACCESS_UNUSED, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_FAULT
    }
};
#endif


static const mem_block device_memory_map[] =
{
    /*EBI2-LCD */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_LCD_BASE, (uint32*)QSC11x0_LCD_BASE, QSC11x0_LCD_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_PERIPHERAL
    },

    /*EBI2-FPGA */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_MEMORY_4_BASE, (uint32*)QSC11x0_MEMORY_4_BASE, QSC11x0_MEMORY_4_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_PERIPHERAL
    },

    /* GSBI0 Base */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_GSBI0_BASE, (uint32*)QSC11x0_GSBI0_BASE, QSC11x0_GSBI0_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_PERIPHERAL
    },

    /* GSBI1 Base */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_GSBI1_BASE, (uint32*)QSC11x0_GSBI1_BASE, QSC11x0_GSBI1_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_PERIPHERAL
    },

    /* GSBI2 Base */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_GSBI2_BASE, (uint32*)QSC11x0_GSBI2_BASE, QSC11x0_GSBI2_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_PERIPHERAL
    },

    /* MOT EST */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_MOT_EST_BASE, (uint32*)QSC11x0_MOT_EST_BASE, QSC11x0_MOT_EST_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_PERIPHERAL
    },

    /* ADSP */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_ADSP_BASE, (uint32*)QSC11x0_ADSP_BASE, QSC11x0_ADSP_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_PERIPHERAL
    },

    /* CHIP BUS */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_CHIP_BUS_BASE, (uint32*)QSC11x0_CHIP_BUS_BASE, QSC11x0_CHIP_BUS_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_PERIPHERAL
    },

    /* GPIO2 */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_GPIO2_BASE, (uint32*)QSC11x0_GPIO2_BASE, QSC11x0_GPIO2_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_PERIPHERAL
    },

    /* UARTDM */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_UARTDM_BASE, (uint32*)QSC11x0_UARTDM_BASE, QSC11x0_UARTDM_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_PERIPHERAL
    },

    /* MODEM */
    {
		MMU_MAPPING_SECTION,
		(uint32*)QSC11x0_MODEM_BASE, (uint32*)QSC11x0_MODEM_BASE, QSC11x0_MODEM_SIZE,
      MMU_ACCESS_PRV_RW, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_PERIPHERAL
    },

    /* End of Table */
    {
		MMU_MAPPING_SECTION,
		(uint32*)MMU_LAST_ADDRESS, (uint32*)MMU_LAST_ADDRESS, 0,
      MMU_ACCESS_UNUSED, MMU_CACHE_NONE, MMU_DOMAIN_CONTEXT_FAULT
    }

};

#endif  /* _ARM_MMU_TABLE_H */
