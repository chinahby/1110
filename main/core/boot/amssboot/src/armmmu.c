/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

						ARM Memory Management Unit

GENERAL DESCRIPTION
		  This module provides some Methods and accessor functions to the
		  ARM MMU. Recomended technical reading includes the ARM926EJ-S
		  Technical Reference Manual.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2009 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

									EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/boot/armmmu.c#16 $

when		  who		 what, where, why
--------	  ---		 ----------------------------------------------------------
05/21/09          dk             To provide cache write through regions for 
                                 debugging purposes.
05/21/09          dk             Reverting changes being done in version 14 due to 
                                 lack of requirement/not being picked in baseline.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/



#include <string.h>
#include "customer.h"
#include "comdef.h"
#include "armmmu.h"
#include "boothw.h"
#include "memory_map_full.h"


/*=============================================================================
*/

/* Include the target specific instructions that are used to populate
 * L1 and L2 page tables
 */

#include "arm_mmu_table.h"

static uint32 pagetable_l2_offset; /* Next Available L2 Pagetable Offset */


/* Memory regions MMU_ACCESS_PRV_RO, 
** MMU_ACCESS_PRV_RW, MMU_ACCESS_PRV_RW, 
** MMU_ACCESS_PRV_RO, MMU_ACCESS_PRV_RO 
** are Cache configured as write through for debug support. 
*/
#if defined(FEATURE_CACHE_WT) 
/* Following global variable is being used for AMSS side which  
** sets the cacheWT if the NV item is enabled. For Boot stage,
** it would be writeback since it is set to 0.
*/
    int cacheWT = 0;
#endif

/*=============================================================================
FUNCTION MMU_INVALIDATE_PAGETABLE

DESCRIPTION
  Zeros out all entries in the mmu page table.  Having an entry of type 0
  raises an exception.

RETURN VALUE
  NONE

=============================================================================*/
static void mmu_invalidate_pagetables(void)
{
   /* Zero = Fault Access, memset() is optimized for speed of operation */
   
   memset(PAGETABLE_L1, 0, sizeof(PAGETABLE_L1));           /* Clear L1 Page Tables */

   memset(PAGETABLE_L2, 0, sizeof(PAGETABLE_L2));           /* Clear L2 Page Tables */
}


/*=============================================================================
FUNCTION MMU_MAP_BLOCK_AS_SECTIONS

DESCRIPTION
  Maps a block descriptor into the page table as sections

RETURN VALUE
  NONE

=============================================================================*/
static void mmu_map_block_as_sections(const mem_block * block)
{
   uint32 *translate_l1, *limit;
   uint32 addr, flags1;

   /* Pointer to L1 Table */

   translate_l1 = mmu_ttbr_get();
   translate_l1 += ((uint32)block->v_base &  MMU_L1_SECT_ADDR_MASK) >> MMU_L1_SECT_ADDR_SHIFT;

   /* Table Values: Physical Address */

   addr = (uint32)block->p_base & MMU_L1_SECT_ADDR_MASK;

   /* Table Values: Flags, Access Permissions, Mapping Type */

   flags1 = (block->access_permission << MMU_L1_SECT_ACCESS_SHIFT)
            | (block->domain << MMU_L1_SECT_DOMAIN_SHIFT)
            | MMU_L1_SECT_SBO
            | (block->cache_attribute << MMU_L1_SECT_CACHE_SHIFT)
            | MMU_MAPPING_SECTION;

   /* Create upper bound */

   limit = translate_l1 + (block->size_in_kbytes / MMU_SIZE_SCALE_1M);
    
   if (block->size_in_kbytes % MMU_SIZE_SCALE_1M != 0)
   {
      limit++;
   }

   /* Populate the L1 Table */

   while (limit > translate_l1)
   {
      *translate_l1 = addr | flags1;

      addr += MMU_L1_SECT_SIZE;

      translate_l1++;
   }
}


/*=============================================================================
FUNCTION MMU_MAP_BLOCK_AS_PAGES

DESCRIPTION
  Maps a block descriptor into the page table as pages

RETURN VALUE
  NONE

=============================================================================*/
static void mmu_map_block_as_pages(const mem_block * block)
{
   uint32 *translate_l1, *translate_l2, *limit; /* L1 & L2 TTB Pointers */
   uint32 addr, flags1, flags2, temp;

   /* Pointer to L1 Table */

   translate_l1 = mmu_ttbr_get();
   translate_l1 += ((uint32)block->v_base &  MMU_L1_SECT_ADDR_MASK) >> MMU_L1_SECT_ADDR_SHIFT;

   /* Convert L1 Section Mapping to Equivilent L2 Section Mapping */

   if (MMU_MAPPING_SECTION == (*translate_l1 & MMU_L1_TYPE_MASK))
   {
      /* Pointer to L2 Table */

      translate_l2 = &PAGETABLE_L2[pagetable_l2_offset][0];

      pagetable_l2_offset++;

      /* Table Values: Physical Address */

      addr = *translate_l1 & MMU_L2_SMALL_ADDR_MASK;

      /* Table Values: Page Mapping Type */

      flags1  = (*translate_l1 & MMU_L1_SECT_DOMAIN_MASK);
      flags1 |= MMU_L1_COARSE_SBO | MMU_MAPPING_COARSE;
        
      /* Table Values: Access Permissions */

      temp = (*translate_l1 & MMU_L1_SECT_ACCESS_MASK) >> MMU_L1_SECT_ACCESS_SHIFT;
      flags2  = temp << (0 + MMU_L2_SMALL_ACCESS_SHIFT);
      flags2 |= temp << (2 + MMU_L2_SMALL_ACCESS_SHIFT);
      flags2 |= temp << (4 + MMU_L2_SMALL_ACCESS_SHIFT);
      flags2 |= temp << (6 + MMU_L2_SMALL_ACCESS_SHIFT);
      
      /* Table Values: Cache Attributes */

      temp = (*translate_l1 & MMU_L1_SECT_CACHE_MASK) >> MMU_L1_SECT_CACHE_SHIFT;
      flags2 |= temp << MMU_L2_SMALL_CACHE_SHIFT;

      flags2 |= MMU_MAPPING_COARSE_SMALL;

      *translate_l1 = (uint32)translate_l2 & MMU_L1_COARSE_ADDR_MASK | flags1;

      /* Create upper bound */

      limit = translate_l2 + MMU_COARSE_PT_SIZE;

      /* Populate the L2 Table */

      while (limit > translate_l2)
      {
         *translate_l2 = addr | flags2;

         addr += MMU_L2_SMALL_SIZE;

         translate_l2++;
      }
   }

   /* Modify L2 Mapping to Reflect Passed Parameters */

   if (MMU_MAPPING_COARSE == (*translate_l1 & MMU_L1_TYPE_MASK))
   {
      /* Pointer to L2 Table */

      translate_l2 = (uint32 *)(*translate_l1 & MMU_L1_COARSE_ADDR_MASK);
      translate_l2 += (((uint32)block->v_base & MMU_L2_SMALL_ADDR_MASK) >> MMU_L2_SMALL_ADDR_SHIFT) & 0xff;

      /* Table Values: Physical Address */

      addr = (uint32)block->p_base & MMU_L2_SMALL_ADDR_MASK;

      /* Table Values: Access Permissions */

      flags2  = block->access_permission << (0 + MMU_L2_SMALL_ACCESS_SHIFT);
      flags2 |= block->access_permission << (2 + MMU_L2_SMALL_ACCESS_SHIFT);
      flags2 |= block->access_permission << (4 + MMU_L2_SMALL_ACCESS_SHIFT);
      flags2 |= block->access_permission << (6 + MMU_L2_SMALL_ACCESS_SHIFT);

      /* Table Values: Cache Attributes */

      flags2 |= block->cache_attribute << MMU_L2_SMALL_CACHE_SHIFT;

      /* Table Values: Page Mapping Type */

      flags2 |= MMU_MAPPING_COARSE_SMALL;

      /* Create upper bound */

      limit = translate_l2 + (block->size_in_kbytes / MMU_SIZE_SCALE_4K);

      if (block->size_in_kbytes % MMU_SIZE_SCALE_4K != 0)
      {
         limit++;
      }

      /* Populate the L2 Table */

      while (limit > translate_l2)
      {
         *translate_l2 = addr | flags2;

         addr += MMU_L2_SMALL_SIZE;

         translate_l2++;
      }
   }
}


/*=============================================================================
FUNCTION MMU_POPULATE_PAGETABLE

DESCRIPTION
  Populates a pagetable with the content of a memory map.

RETURN VALUE
  NONE

=============================================================================*/
static void mmu_populate_pagetable(const mem_block * map)
{
   while (MMU_LAST_ADDRESS != (uint32)map->v_base)
   {
      if (MMU_MAPPING_SECTION == map->memory_mapping)
      {
         mmu_map_block_as_sections(map++);
      }
      else if (MMU_MAPPING_COARSE == map->memory_mapping)
      {
         mmu_map_block_as_pages(map++);
      }
   }
}


/*=============================================================================
FUNCTION MMU_CREATE_PAGETABLE

DESCRIPTION
  Populates a pagetable with the content of a memory map.

RETURN VALUE
  NONE

=============================================================================*/
void boot_cache_mmu_init(void)
{
   ARMREG cpsr_save;

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

   mmu_cr_set(mmu_cr_get() | MMU_CR_ICACHE);                /* Set Instruction Cache Active */

   mmu_cr_set(mmu_cr_get() | MMU_CR_AFAULT);                /* Set Alignment Fault Active */

   mmu_dcache_disable();                                    /* Set Data Cache Inactive */

   mmu_disable();                                           /* Set Address Translation Inactive */

   mmu_ttbr_set((uint32 *)PAGETABLE_L1);                    /* Set Translation Table Base */

   mmu_invalidate_pagetables();                             /* Initalize Tables (Invalidate Contents) */

   pagetable_l2_offset = 0;                                 /* Initilaize L2 Offset Counter */

/* Populate Tables With Memory Area Information */
#if defined(FEATURE_CACHE_WT)
    if (cacheWT == 0)
    {
        mmu_populate_pagetable((mem_block*)memory_memory_map);   
    }
    else
    {
        /* Memory regions MMU_ACCESS_PRV_RO, 
        ** MMU_ACCESS_PRV_RW, MMU_ACCESS_PRV_RW, 
        ** MMU_ACCESS_PRV_RO, MMU_ACCESS_PRV_RO 
        ** are Cache configured as write through for debug support. 
        */
        mmu_populate_pagetable((mem_block*)memory_memory_map_wt); 
    }
#else
        mmu_populate_pagetable((mem_block*)memory_memory_map);      
#endif

 mmu_populate_pagetable((mem_block*)device_memory_map);   /* Populate Tables With Device Area Information */

   /* Set System Protection Bits : NOTE, S or R, Not Both */

   mmu_cr_set((mmu_cr_get() & ~(MMU_CR_SPROT | MMU_CR_RPROT)) | MMU_CR_SPROT);

   /* Set Default Paging Access, Default Setting for Domain - Faults on Access */
   /* Those listed below are Client Checked Access */

   mmu_dacr_set(MMU_DACR_ACCESS(MMU_DACR_CLIENT, MMU_DOMAIN_CONTEXT_DEFAULT) |         /* Default Areas */
                MMU_DACR_ACCESS(MMU_DACR_CLIENT, MMU_DOMAIN_CONTEXT_EFS) |             /* EFS Sections (MMU Domain Context) */
                MMU_DACR_ACCESS(MMU_DACR_CLIENT, MMU_DOMAIN_CONTEXT_PERIPHERAL)        /* Peripheral Sections (MMU Domain Context) */
               );

   mmu_far_set(0);                                          /* Clear Fault Address Register */

   mmu_dfsr_set(0);                                         /* Clear Data Fault Status Register */

   mmu_ifsr_set(0);                                         /* Clear Instruction Fault Status Register */

   mmu_fcsepid_set(0);                                      /* Clear FCSE PID Register */

   mmu_contextid_set(0);                                    /* Clear Context ID Register */

   mmu_enable();                                            /* Set Address Translation Active */

   mmu_dcache_enable();                                     /* Set Data Cache Active */

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */
}

/*=============================================================================
FUNCTION MMU_ENABLE

DESCRIPTION

RETURN VALUE

=============================================================================*/

void mmu_enable(void)
{
   ARMREG cpsr_save;

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

   mmu_invalidate_tlb();                                    /* Invalidate TLB Contents */

   mmu_cr_set(mmu_cr_get() | MMU_CR_ENABLE);                /* Set Address Translation Active */

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */
}

/*=============================================================================
FUNCTION MMU_DISABLE

DESCRIPTION

RETURN VALUE

=============================================================================*/

void mmu_disable(void)
{
   ARMREG cpsr_save;

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

   mmu_cr_set(mmu_cr_get() & ~MMU_CR_ENABLE);               /* Set Address Translation Inactive */

   mmu_invalidate_tlb();                                    /* Invalidate TLB Contents */

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */
}

/*=============================================================================
FUNCTION MMU_DCACHE_ENABLE

DESCRIPTION

RETURN VALUE

=============================================================================*/

void mmu_dcache_enable(void)
{
   ARMREG cpsr_save;

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

   mmu_invalidate_tlb();                                    /* Invalidate TLB Contents */

   mmu_cr_set(mmu_cr_get() | MMU_CR_DCACHE);                /* Set Data Cache Active */

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */
}

/*=============================================================================
FUNCTION MMU_DCACHE_DISABLE

DESCRIPTION

RETURN VALUE

=============================================================================*/

void mmu_dcache_disable(void)
{
   ARMREG cpsr_save;

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

   mmu_dcache_flush_all();                                  /* Clean and Invalidate Data Cache */

   mmu_cr_set(mmu_cr_get() & ~MMU_CR_DCACHE);               /* Set Data Cache Inactive */

   mmu_invalidate_tlb();                                    /* Invalidate TLB Contents */

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */
}

/*=============================================================================
FUNCTION MMU_ICACHE_ENABLE

DESCRIPTION

RETURN VALUE

=============================================================================*/

void mmu_icache_enable(void)
{
   ARMREG cpsr_save;

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

   mmu_invalidate_tlb();                                    /* Invalidate TLB Contents */

   mmu_cr_set(mmu_cr_get() | MMU_CR_ICACHE);                /* Set Instruction Cache Active */

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */
}

/*=============================================================================
FUNCTION MMU_ICACHE_DISABLE

DESCRIPTION

RETURN VALUE

=============================================================================*/

void mmu_icache_disable(void)
{
   ARMREG cpsr_save;

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

   mmu_cr_set(mmu_cr_get() & ~MMU_CR_ICACHE);               /* Set Data Cache Inactive */

   mmu_invalidate_tlb();                                    /* Invalidate TLB Contents */

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */
}

/*=============================================================================
FUNCTION MMU_DOMAIN_EFS_CLIENT

DESCRIPTION

RETURN VALUE

=============================================================================*/

void mmu_domain_efs_client(void)                            /* Set EFS Context Client */
{
   ARMREG cpsr_save;

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

   mmu_dacr_set((mmu_dacr_get() & ~MMU_DACR_ACCESS_MASK(MMU_DOMAIN_CONTEXT_EFS)) |
                MMU_DACR_ACCESS(MMU_DACR_CLIENT, MMU_DOMAIN_CONTEXT_EFS));
   
   // mmu_l2_set_access(addr, size, MMU_ACCESS_PRV_RO);

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */
}

/*=============================================================================
FUNCTION MMU_DOMAIN_EFS_MANAGER

DESCRIPTION

RETURN VALUE

=============================================================================*/

void mmu_domain_efs_manager(void)                           /* Set EFS Context Manager */
{
   ARMREG cpsr_save;

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

   mmu_dacr_set((mmu_dacr_get() & ~MMU_DACR_ACCESS_MASK(MMU_DOMAIN_CONTEXT_EFS)) |
                MMU_DACR_ACCESS(MMU_DACR_MANAGER, MMU_DOMAIN_CONTEXT_EFS));
   
   // mmu_l2_set_access(addr, size, MMU_ACCESS_PRV_RW);

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */
}

/*=============================================================================
FUNCTION MMU_L1_GET_CACHE

DESCRIPTION

RETURN VALUE

=============================================================================*/

MMU_CacheAttributeType mmu_l1_get_cache(uint32 *v_addr)
{
   uint32 *translate_l1;
   MMU_CacheAttributeType result;

   /* Pointer to L1 Table */

   translate_l1 = mmu_ttbr_get();
   translate_l1 += ((uint32)v_addr &  MMU_L1_SECT_ADDR_MASK) >> MMU_L1_SECT_ADDR_SHIFT;

   /* Get current cache settings */

   result = (MMU_CacheAttributeType)
            ((*translate_l1 & MMU_L1_SECT_CACHE_MASK) >> MMU_L1_SECT_CACHE_SHIFT);

   return (result);                                         /* Return result */
}

/*=============================================================================
FUNCTION MMU_L1_SET_CACHE

DESCRIPTION

RETURN VALUE

=============================================================================*/

MMU_CacheAttributeType mmu_l1_set_cache(uint32 *v_addr, uint32 size, MMU_CacheAttributeType type)
{
   uint32 *translate_l1;
   uint32 flags1;
   ARMREG cpsr_save;
   MMU_CacheAttributeType result;

   /* Pointer to L1 Table */

   translate_l1 = mmu_ttbr_get();
   translate_l1 += ((uint32)v_addr &  MMU_L1_SECT_ADDR_MASK) >> MMU_L1_SECT_ADDR_SHIFT;

   /* Get current cache settings */

   result = (MMU_CacheAttributeType)
            ((*translate_l1 & MMU_L1_SECT_CACHE_MASK) >> MMU_L1_SECT_CACHE_SHIFT);

   /* Setup new cache settings for a write */

   flags1  = (*translate_l1 & ~(MMU_CACHE_WRITE_BACK << MMU_L1_SECT_CACHE_SHIFT));
   flags1 |= (type << MMU_L1_SECT_CACHE_SHIFT);

   /* Disable the MMU and write new settings */

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */
    
   mmu_dcache_flush_all();                                  /* Clean and Invalidate Data Cache */

   mmu_disable();                                           /* Disable Address Translation */
    
   *translate_l1 = flags1;                                  /* Set New Cache Method */

   mmu_enable();                                            /* Enable Address Translation */

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */

   return (result);                                         /* Return result */
}

/*=============================================================================

DESCRIPTION
LEGACY FUNCTIONS FROM ASSEMBLY

=============================================================================*/

void mmu_dcache_clean_region(void *addr, uint32 length)
{
   ARMREG cpsr_save;

	char *limit = (char *)addr + MMU_C15R07_MVA_MAX(length) - 1;	/* MVA of Upper Bound */

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

	while (limit >= (char *)addr)										/* Loop and perform operation on each line */
   {
      mmu_dcache_clean_line(limit);                         /* Perform Operation */

      limit -= MMU_C15R07_LINE_SIZE;                        /* Adjust loop counter */
   }

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */   
}

void mmu_dcache_inval_region(void *addr, uint32 length)
{
   ARMREG cpsr_save;

	char *limit = (char *)addr + MMU_C15R07_MVA_MAX(length) - 1;	/* MVA of Upper Bound */

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

	while (limit >= (char *)addr)										/* Loop and perform operation on each line */
   {
      mmu_dcache_inval_line(limit);                         /* Perform Operation */

      limit -= MMU_C15R07_LINE_SIZE;                        /* Adjust loop counter */
   }

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */   
}

void mmu_dcache_flush_region(void *addr, uint32 length)
{
   ARMREG cpsr_save;

	char *limit = (char *)addr + MMU_C15R07_MVA_MAX(length) - 1;	/* MVA of Upper Bound */

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

	while (limit >= (char *)addr)										/* Loop and perform operation on each line */
   {
      mmu_dcache_flush_line(limit);                         /* Perform Operation */

      limit -= MMU_C15R07_LINE_SIZE;                        /* Adjust loop counter */
   }

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */   
}

void mmu_icache_precache_region(void *addr, uint32 length)
{
   ARMREG cpsr_save;

	char *limit = (char *)addr + MMU_C15R07_MVA_MAX(length) - 1;	/* MVA of Upper Bound */

   cpsr_save = arm_cpsr_bis(CPSR_I_F_BITS);                 /* Mask Interrupts */

	while (limit >= (char *)addr)										/* Loop and perform operation on each line */
   {
      mmu_icache_precache_line(limit);                      /* Perform Operation */

      limit -= MMU_C15R07_LINE_SIZE;                        /* Adjust loop counter */
   }

   arm_cpsr_set(cpsr_save);                                 /* Restore Interrupt State */   
}

/*=============================================================================

DESCRIPTION
ARM REGISTER ACCESSOR FUNCTIONS

These functions all fit the same boilerplate in order to provide access
to ARM internal registers. The ARM internal registers are documented in
many ARM architecture reference manuals.

=============================================================================*/

ARMREG arm_cpsr_get(void)                                   /* CURRENT PROCESSOR STATUS REGISTER */
{
	register ARMREG value;                                   /* Register Allocated Value */
   __asm{ mrs value,CPSR }                                  /* Read CPSR */
   return (value);                                          /* Return result */
}

ARMREG arm_cpsr_set(ARMREG value)                           /* CURRENT PROCESSOR STATUS REGISTER */
{
   __asm{ msr CPSR_c,value }                                /* Write CPSR */
   return (value);                                          /* Return result */
}

ARMREG arm_cpsr_bic(ARMREG value)                           /* CURRENT PROCESSOR STATUS REGISTER */
{
   register ARMREG original;                                /* Register Allocated Value */   
   __asm{ mrs original,CPSR }                               /* Read CPSR */
   value = original & (~value);                             /* Clear Bits */
   __asm{ msr CPSR_c,value }                                /* Write CPSR */
   return (original);                                       /* Return result */
}

ARMREG arm_cpsr_bis(ARMREG value)                           /* CURRENT PROCESSOR STATUS REGISTER */
{
   register ARMREG original;                                /* Register Allocated Value */   
   __asm{ mrs original,CPSR }                               /* Read CPSR */
   value = original | value;                                /* Set Bits */
   __asm{ msr CPSR_c,value }                                /* Write CPSR */
   return (original);                                       /* Return result */
}

ARMREG arm_spsr_get(void)                                   /* SAVED PROCESSOR STATUS REGISTER */
{
   register ARMREG value;                                   /* Register Allocated Value */
   __asm{ mrs value,SPSR }                                  /* Read SPSR */
   return (value);                                          /* Return result */
}

ARMREG arm_spsr_set(ARMREG value)                           /* SAVED PROCESSOR STATUS REGISTER */
{
   __asm{ msr SPSR_c,value }                                /* Write SPSR */
   return (value);                                          /* Return result */
}

/*=============================================================================

DESCRIPTION
ARM MMU REGISTER ACCESSOR FUNCTIONS

These functions all fit the same boilerplate in order to provide access
to MMU internal registers.

=============================================================================*/

MMUREG mmu_sr_idcode(void)                                  /* STATUS REGISTER C15:00 */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c0,c0,0 }                         /* Read SR */
   return (value);                                          /* Return result */
}

MMUREG mmu_sr_cache(void)                                   /* STATUS REGISTER C15:00 */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c0,c0,1 }                         /* Read SR */
   return (value);                                          /* Return result */
}

MMUREG mmu_sr_tcm(void)                                     /* STATUS REGISTER C15:00 */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c0,c0,2 }                         /* Read SR */
   return (value);                                          /* Return result */
}

MMUREG mmu_cr_get(void)                                     /* CONFIGURATION REGISTER C15:01 */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c1,c0,0 }                         /* Read CR */
   return (value);                                          /* Return result */
}

MMUREG mmu_cr_set(MMUREG value)                             /* CONFIGURATION REGISTER C15:01 */
{
   __asm{ mcr p15,0,value,c1,c0,0 }                         /* Write CR */
   return (value);                                          /* Return result */
}

uint32 *mmu_ttbr_get(void)                                  /* TRANSLATE TABLE BASE REGISTER C15:02 */
{
   uint32 *value;
   __asm{ mrc p15,0,value,c2,c0,0 }                         /* Read TTBR */
   return (value);                                          /* Return result */
}

uint32 *mmu_ttbr_set(uint32 *value)                         /* TRANSLATE TABLE BASE REGISTER C15:02 */
{
   __asm{ mcr p15,0,value,c2,c0,0 }                         /* Write TTBR */
   return (value);                                          /* Return result */
}

MMUREG mmu_dacr_get(void)                                   /* DATA ACCESS CONTROL REGISTER C15:03 */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c3,c0,0 }                         /* Read DACR */
   return (value);                                          /* Return result */
}

MMUREG mmu_dacr_set(MMUREG value)                           /* DATA ACCESS CONTROL REGISTER C15:03 */
{
   __asm{ mcr p15,0,value,c3,c0,0 }                         /* Write DACR */
   return (value);                                          /* Return result */
}

MMUREG mmu_dfsr_get(void)                                   /* DATA FAULT STATUS REGISTER C15:05 */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c5,c0,0 }                         /* Read DFSR */
   return (value);                                          /* Return result */
}

MMUREG mmu_dfsr_set(MMUREG value)                           /* DATA FAULT STATUS REGISTER C15:05 */
{
   __asm{ mcr p15,0,value,c5,c0,0 }                         /* Write DFSR */
   return (value);                                          /* Return result */
}

MMUREG mmu_ifsr_get(void)                                   /* INSTRUCTION FAULT STATUS REGISTER C15:05 */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c5,c0,1 }                         /* Read IFSR */
   return (value);                                          /* Return result */
}

MMUREG mmu_ifsr_set(MMUREG value)                           /* INSTRUCTION FAULT STATUS REGISTER C15:05 */
{
   __asm{ mcr p15,0,value,c5,c0,1 }                         /* Write IFSR */
   return (value);                                          /* Return result */
}

MMUREG mmu_far_get(void)                                    /* FAULT ADDRESS REGISTER C15:06 */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c6,c0,0 }                         /* Read FAR */
   return (value);                                          /* Return result */
}

MMUREG mmu_far_set(MMUREG value)                            /* FAULT ADDRESS REGISTER C15:06 */
{
   __asm{ mcr p15,0,value,c6,c0,0 }                         /* Write FAR */
   return (value);                                          /* Return result */
}

MMUREG mmu_clr_icache_get(void)                             /* CACHE LOCKDOWN REGISTER ICACHE C15:09 */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c9,c0,1 }                         /* Read CLR ICache */
   return (value);                                          /* Return result */
}

MMUREG mmu_clr_icache_set(MMUREG value)                     /* CACHE LOCKDOWN REGISTER ICACHE C15:09 */
{
   __asm{ mcr p15,0,value,c9,c0,1 }                         /* Write CLR ICache */
   return (value);                                          /* Return result */
}

MMUREG mmu_clr_dcache_get(void)                             /* CACHE LOCKDOWN REGISTER DCACHE C15:09 */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c9,c0,0 }                         /* Read CLR DCache */
   return (value);                                          /* Return result */
}

MMUREG mmu_clr_dcache_set(MMUREG value)                     /* CACHE LOCKDOWN REGISTER DCACHE C15:09 */
{
   __asm{ mcr p15,0,value,c9,c0,0 }                         /* Write CLR DCache */
   return (value);                                          /* Return result */
}

MMUREG mmu_tlblr_get(void)                                  /* TLB LOCKDOWN REGISTER C15:0A */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c10,c0,0 }                        /* Read TLBLR */
   return (value);                                          /* Return result */
}

MMUREG mmu_tlblr_set(MMUREG value)                          /* TLB LOCKDOWN REGISTER C15:0A */
{
   __asm{ mcr p15,0,value,c10,c0,0 }                        /* Write TLBLR */
   return (value);                                          /* Return result */
}

MMUREG mmu_fcsepid_get(void)                                /* FAST CONTEXT SWITCH PID REGISTER C15:0D */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c13,c0,0 }                        /* Read FCSEPID */
   return (value);                                          /* Return result */
}

MMUREG mmu_fcsepid_set(MMUREG value)                        /* FAST CONTEXT SWITCH PID REGISTER C15:0D */
{
   __asm{ mcr p15,0,value,c13,c0,0 }                        /* Write FCSEPID */
   return (value);                                          /* Return result */
}

MMUREG mmu_contextid_get(void)                              /* CONTEXT ID REGISTER C15:0D */
{
   register MMUREG value;                                   /* Register Allocated Value */
   __asm{ mrc p15,0,value,c13,c0,1 }                        /* Read FCSEPID */
   return (value);                                          /* Return result */
}

MMUREG mmu_contextid_set(MMUREG value)                      /* CONTEXT ID REGISTER C15:0D */
{
   __asm{ mcr p15,0,value,c13,c0,1 }                        /* Write FCSEPID */
   return (value);                                          /* Return result */
}

/*=============================================================================

DESCRIPTION
LEGACY FUNCTIONS FROM ASSEMBLY

=============================================================================*/

void mmu_invalidate_tlb(void)
{
   register uint32 value = 0;                               /* Register Allocated Value */
   __asm{ mcr p15,0,value,c8,c7,0 }                         /* Invalidate TLB */
   __asm{ mcr p15,0,value,c8,c5,0 }                         /* Invalidate DTLB */
   __asm{ mcr p15,0,value,c8,c6,0 }                         /* Invalidate ITLB */
}

void mmu_invalidate_tlb_single_entry(uint32 value)
{
   value = MMU_C15R08_MVA(value);                           /* Create Masked MVA for Operation */
   __asm{ mcr p15,0,value,c8,c7,1 }                         /* Invalidate TLB using MVA */
}

void mmu_icache_precache_line(void *addr)
{
   register uint32 value = MMU_C15R07_MVA((uint32)addr);    /* Create Masked MVA for Operation */
   __asm{ mcr p15,0,value,c7,c13,1 }                        /* Precache line using MVA */
}

void mmu_dcache_clean_line(void *addr)
{
   register uint32 value = MMU_C15R07_MVA((uint32)addr);    /* Create Masked MVA for Operation */
   __asm{ mcr p15,0,value,c7,c10,1 }                        /* Clean cache line using MVA */
}

void mmu_dcache_inval_line(void *addr)
{
   register uint32 value = MMU_C15R07_MVA((uint32)addr);    /* Create Masked MVA for Operation */
   __asm{ mcr p15,0,value,c7,c6,1 }                         /* Invalidate cache line using MVA */
}

void mmu_dcache_flush_line(void *addr)
{
   register uint32 value = MMU_C15R07_MVA((uint32)addr);    /* Create Masked MVA for Operation */
   __asm{ mcr p15,0,value,c7,c14,1 }                        /* Clean and invalidate cache line using MVA */
}

void mmu_dcache_clean_all(void)
{
   register uint32 value = 0;                               /* Register Allocated Value */
   while (0 == (value & MMU_C15R07_CLEAN_FLAG))             /* Bit 30 - DCache Clean Status */
   {
      __asm{ mrc p15,0,value,c7,c10,3 }                     /* Clean/Test */
   }
}

void mmu_dcache_inval_all(void)
{
   register uint32 value = 0;                               /* Register Allocated Value */
   while (0 == (value & MMU_C15R07_CLEAN_FLAG))             /* Bit 30 - DCache Clean Status */
   {
      __asm{ mrc p15,0,value,c7,c14,3 }                     /* Clean/Invalidate/Test DCache */
   }
}

void mmu_dcache_flush_all(void)
{
   register uint32 value = 0;                               /* Register Allocated Value */
   while (0 == (value & MMU_C15R07_CLEAN_FLAG))             /* Bit 30 - DCache Clean Status */
   {
      __asm{ mrc p15,0,value,c7,c14,3 }                     /* Clean/Invalidate/Test DCache */
   }
}

void mmu_standby_wfi(void)
{
   register uint32 value = 0;                               /* Register Allocated Value */
   __asm{ mcr p15,0,value,c7,c0,4 }                         /* Wait for Interrupt */
}

void mmu_drain_writes(void)
{
   register uint32 value = 0;                               /* Register Allocated Value */
   __asm{ mcr p15,0,value,c7,c10,4 }                        /* Drain write buffer */
}

/*=============================================================================

DESCRIPTION
 LEGACY FUNCTIONS FROM ASSEMBLY : WRAPPERS TO REDUCE DUPLICATED
 FUNCTIONALITY, THESE SHOULD BE DEPRECATED AND AVOIDED.

=============================================================================*/

void cache_mmu_disable(void)                                /* DEPRECATED */
{
   mmu_dcache_disable();
}

void cache_mmu_re_enable(void)                              /* DEPRECATED */
{
   mmu_dcache_enable();
}

void mmu_invalidate_instruction_cache(void)                 /* DEPRECATED */
{
   /* NOP */                     
}

void mmu_enable_instruction_cache(void)                     /* DEPRECATED */
{
   mmu_icache_enable();
}

void mmu_disable_data_cache(void)                           /* DEPRECATED */
{
   mmu_dcache_disable();
}

void mmu_re_enable_data_cache(void)                         /* DEPRECATED */
{
   mmu_dcache_enable();
}

void mmu_invalidate_data_cache(void)                        /* DEPRECATED */
{
   mmu_dcache_flush_all();                                  /* Clean and Invalidate Data Cache */
}

void mmu_clean_and_invalidate_data_cache(void)              /* DEPRECATED */
{
   mmu_dcache_flush_all();                                  /* Clean and Invalidate Data Cache */
}

void mmu_invalidate_data_cache_lines(uint32 *addr, uint32 length) /* DEPRECATED */
{
   mmu_dcache_flush_region(addr, length);
}

void dcache_clean_line(uint32 *addr)                        /* DEPRECATED */
{
   mmu_dcache_clean_line(addr);
}
