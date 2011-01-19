#ifndef ARMMMU_H
#define ARMMMU_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                    ARM MMU Definitions Header File

DESCRIPTION
    Definitions specific to the ARM Memory Management Unit

Copyright (c) 2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/boot/armmmu.h#8 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "memory_map_full.h"

#define MMU_LAST_ADDRESS				0xffffffff

#define MMU_PT_SIZE                 4096                    /* Number of entries in pagetable */
#define MMU_COARSE_PT_SIZE          256                     /* Number of entries in coarse pagetable */

#define MMU_NUM_COARSE_PT           12                      /* Number of L2 pagetables (max) */

#define MMU_L1_TYPE_MASK				0x00000003
#define MMU_L1_TYPE_SHIFT				0
#define MMU_L1_FAULT_TYPE				0x00000000

#define MMU_L1_SECT_ADDR_MASK			0xfff00000
#define MMU_L1_SECT_ADDR_SHIFT		20
#define MMU_L1_SECT_ACCESS_MASK		0x00000c00
#define MMU_L1_SECT_ACCESS_SHIFT		10
#define MMU_L1_SECT_DOMAIN_MASK		0x000001e0
#define MMU_L1_SECT_DOMAIN_SHIFT		5
#define MMU_L1_SECT_SBO					0x00000010
#define MMU_L1_SECT_CACHE_MASK		0x0000000c
#define MMU_L1_SECT_CACHE_SHIFT		2
#define MMU_L1_SECT_TYPE				0x00000002
#define MMU_L1_SECT_SIZE				(1<<MMU_L1_SECT_ADDR_SHIFT)

#define MMU_L1_COARSE_ADDR_MASK		0xfffffc00
#define MMU_L1_COARSE_ADDR_SHIFT		10
#define MMU_L1_COARSE_DOMAIN_MASK	0x000001e0
#define MMU_L1_COARSE_DOMAIN_SHIFT	5
#define MMU_L1_COARSE_SBO				0x00000010
#define MMU_L1_COARSE_TYPE				0x00000001
#define MMU_L1_COARSE_TYPE_MASK		0x00000003
#define MMU_L1_COARSE_TYPE_SHIFT		0

#define MMU_L2_TYPE_MASK				0x00000003
#define MMU_L2_TYPE_SHIFT				0

#define MMU_L2_LARGE_ADDR_MASK		0xffff0000
#define MMU_L2_LARGE_ADDR_SHIFT		16
#define MMU_L2_LARGE_ACCESS_MASK		0x00000ff0
#define MMU_L2_LARGE_ACCESS_SHIFT	4
#define MMU_L2_LARGE_CACHE_MASK		0x0000000c
#define MMU_L2_LARGE_CACHE_SHIFT		2
#define MMU_L2_LARGE_TYPE				0x00000001
#define MMU_L2_LARGE_TYPE_MASK		0x00000003
#define MMU_L2_LARGE_TYPE_SHIFT		0
#define MMU_L2_LARGE_SIZE				(1<<MMU_L2_LARGE_ADDR_SHIFT)

#define MMU_L2_SMALL_ADDR_MASK		0xfffff000
#define MMU_L2_SMALL_ADDR_SHIFT		12
#define MMU_L2_SMALL_ACCESS_MASK		0x00000ff0
#define MMU_L2_SMALL_ACCESS_SHIFT	4
#define MMU_L2_SMALL_CACHE_MASK		0x0000000c
#define MMU_L2_SMALL_CACHE_SHIFT		2
#define MMU_L2_SMALL_TYPE				0x00000002
#define MMU_L2_SMALL_TYPE_MASK		0x00000003
#define MMU_L2_SMALL_TYPE_SHIFT		0
#define MMU_L2_SMALL_SIZE				(1<<MMU_L2_SMALL_ADDR_SHIFT)

#define MMU_SR_IDCODE_IMPLEMENTOR   0xff000000              /* IDCode Implementor */
#define MMU_SR_IDCODE_VARIANT       0x00f00000              /* IDCode Variant */
#define MMU_SR_IDCODE_ARCH          0x000f0000              /* IDCode Architecture */
#define MMU_SR_IDCODE_PART          0x0000fff0              /* IDCode Part */
#define MMU_SR_IDCODE_REV           0x0000000f              /* IDCode Revision */

#define MMU_SR_CACHE_ISIZE_SHIFT    0
#define MMU_SR_CACHE_ISIZE_MASK     ((0x00000fff)<<MMU_SR_ISIZE_SHIFT)
#define MMU_SR_CACHE_DSIZE_SHIFT    12
#define MMU_SR_CACHE_DSIZE_MASK     ((0x00000fff)<<MMU_SR_DSIZE_SHIFT)
#define MMU_SR_CACHE_UNIFIED        0x01000000              /* Unified ICache & DCache */

#define MMU_SR_CACHE_NSIZE_LEN      0x00000003              /* Length of a Line */
#define MMU_SR_CACHE_NSIZE_M        0x00000004              /* Multiplier (Always 0) */
#define MMU_SR_CACHE_NSIZE_ASSOC    0x00000038              /* Associativity of Cache */
#define MMU_SR_CACHE_NSIZE_SIZE     0x000003c0              /* Size of Cache */

#define MMU_CR_ENABLE               (1<<0)                  /* Address Translation Enable */
#define MMU_CR_AFAULT               (1<<1)                  /* Alignment Fault Enable */
#define MMU_CR_DCACHE               (1<<2)                  /* Data Cache Enable */
#define MMU_CR_BE                   (1<<7)                  /* Big Endian Enable */
#define MMU_CR_SPROT						(1<<8)                  /* System Protection Enable */
#define MMU_CR_RPROT                (1<<9)                  /* ROM Protection Enable */
#define MMU_CR_ICACHE               (1<<12)                 /* Instruction Cache Enable */

#define MMU_DACR_ACCESS(type,domain)	((type)<<((domain)<<1))
#define MMU_DACR_ACCESS_MASK(domain)	(MMU_DACR_ACCESS(MMU_DACR_MANAGER,(domain)))

#define MMU_C15R07_LINE_SIZE        0x0020                  /* Line Size Constant for C15:07 Register Use */
#define MMU_C15R08_LINE_SIZE        0x0400                  /* Line Size Constant for C15:08 Register Use */
#define MMU_C15R07_CLEAN_FLAG       0x40000000              /* D30, Clean Flag on DCache */

#define MMU_C15R07_MVA(val)         ((val)&(~0x0000001f))	/* Bits 4:0 SBZ */
#define MMU_C15R07_MVA_MAX(val)     ((val)& (0x0001ffff))	/* 128KB Maximum Target Size for ARM926EJS */

#define MMU_C15R08_MVA(val)         ((val)&(~0x000003ff))	/* Bits 9:0 SBZ */

#define CPSR_I_F_BITS               0xc0

#define MMU_SIZE_SCALE_1K           1                       /* KB Scale Factor */
#define MMU_SIZE_SCALE_4K           4                       /* KB Scale Factor */
#define MMU_SIZE_SCALE_64K          64                      /* KB Scale Factor */
#define MMU_SIZE_SCALE_1M           1024                    /* MB Scale Factor */

typedef enum
{
  MMU_MAPPING_FAULT                 = MMU_L1_FAULT_TYPE,    /* Region Mapped Data Abort */
  MMU_MAPPING_COARSE                = MMU_L1_COARSE_TYPE,	/* Region Mapped Coarse L2 Tables */
  MMU_MAPPING_SECTION               = MMU_L1_SECT_TYPE,     /* Region Section Mapped (1MB) */
  MMU_MAPPING_COARSE_LARGE          = MMU_L2_LARGE_TYPE,    /* Region Mapped L2 Large Pages (64KB) */
  MMU_MAPPING_COARSE_SMALL          = MMU_L2_SMALL_TYPE,    /* Region Mapped L2 Small Pages (4KB) */
} MMU_MemoryMappingType;

typedef enum
{
  MMU_ACCESS_PRV_RO                 = 0,                    /* S Bit/R Bit = RO, else NONE */
  MMU_ACCESS_PRV_RW                 = 1,                    /* Priv R/W, Usr None */
  MMU_ACCESS_USR_RO                 = 2,                    /* Priv R/W, Usr RO */
  MMU_ACCESS_USR_RW                 = 3,                    /* Priv R/W, Usr RW */
  MMU_ACCESS_UNUSED                 = 4                     /* Mark Unused */
} MMU_AccessPermissionType;

typedef enum
{
  MMU_CACHE_NONE                    = 0,                    /* No Cache, No Buffer */
  MMU_NON_CACHEABLE_BUFFERED        = 1,                    /* No Cache, Buffer */
  MMU_CACHE_WRITE_THRU              = 2,                    /* Cached, No Buffer */
  MMU_CACHE_WRITE_BACK              = 3                     /* Cached, Buffer */
} MMU_CacheAttributeType;

/* Domain context assigned to 1MB sections in L1 table entries; the
   domain is used to index into the access control register to
   determine if access to the section is checked for fault access,
   client (checked) access, or manager (always allowed) access.

Note: Trace32 Command MMU.LIST Presents this information, it makes
classification of the type of memory fault have some granularity.

*/

typedef enum
{
  MMU_DOMAIN_CONTEXT_DEFAULT        = 0,                    /* Default */
  MMU_DOMAIN_1								= 1,
  MMU_DOMAIN_2								= 2,
  MMU_DOMAIN_3								= 3,
  MMU_DOMAIN_4								= 4,
  MMU_DOMAIN_5                      = 5,
  MMU_DOMAIN_6                      = 6,
  MMU_DOMAIN_CONTEXT_EFS            = 7,                    /* EFS Context */
  MMU_DOMAIN_8                      = 8,
  MMU_DOMAIN_9                      = 9,
  MMU_DOMAIN_10                     = 10,
  MMU_DOMAIN_11                     = 11,
  MMU_DOMAIN_12                     = 12,
  MMU_DOMAIN_13                     = 13,
  MMU_DOMAIN_CONTEXT_PERIPHERAL     = 14,                   /* Memory Mapped Peripherals */
  MMU_DOMAIN_CONTEXT_FAULT          = 15,                   /* Faults on Access */
  MMU_DOMAIN_UNUSED                 = 16                    /* Mark Unused */
} MMU_DomainValueType;

typedef enum
{
  MMU_DACR_NONE							= 0,                    /* Access Fault */
  MMU_DACR_CLIENT							= 1,                    /* Client, Access Checked */
  MMU_DACR_MANAGER						= 3                     /* Manager, Access Not Checked */
} MMU_DACRValueType;

typedef struct
{
  MMU_MemoryMappingType      memory_mapping;                /* Type of Mapping: Section, Coarse, Fault */
  uint32*                    p_base;                        /* Physical Base Address */
  uint32*                    v_base;                        /* Virtual Base Address */
  uint32                     size_in_kbytes;                /* Size of Mapping in Kbytes : Multiple of 1MB, 4KB */
  MMU_AccessPermissionType   access_permission;             /* Permissions Applied to Mapping */
  MMU_CacheAttributeType     cache_attribute;               /* Cache Attributes Applied to Mapping */
  MMU_DomainValueType        domain;                        /* Domain Context Applied to Mapping */
} mem_block;

/* Prototypes for assembly exported accessor functions */

typedef unsigned int ARMREG;                                /* ARM Register Type */

ARMREG  arm_cpsr_get(void);                                 /* ARM Register CPSR */
ARMREG  arm_cpsr_set(ARMREG);                               /* ARM Register CPSR */
ARMREG  arm_cpsr_bic(ARMREG);                               /* ARM Register CPSR */
ARMREG  arm_cpsr_bis(ARMREG);                               /* ARM Register CPSR */

ARMREG  arm_spsr_get(void);                                 /* ARM Register SPSR */
ARMREG  arm_spsr_set(ARMREG);                               /* ARM Register SPSR */

typedef unsigned int MMUREG;                                /* MMU Register Type */

MMUREG  mmu_sr_idcode(void);                                /* Status Register C15:00 */
MMUREG  mmu_sr_cache(void);                                 /* Status Register C15:00 */
MMUREG  mmu_sr_tcm(void);                                   /* Status Register C15:00 */

MMUREG  mmu_cr_get(void);                                   /* MMU Register CR C15:01 */
MMUREG  mmu_cr_set(MMUREG);                                 /* MMU Register CR C15:01 */

uint32* mmu_ttbr_get(void);                                 /* MMU Register TTBR C15:02 */
uint32* mmu_ttbr_set(uint32*);                              /* MMU Register TTBR C15:02 */

MMUREG  mmu_dacr_get(void);                                 /* MMU Register DACR C15:03 */
MMUREG  mmu_dacr_set(MMUREG);                               /* MMU Register DACR C15:03 */

MMUREG  mmu_dfsr_get(void);                                 /* MMU Register DSFR C15:05 */
MMUREG  mmu_dfsr_set(MMUREG);                               /* MMU Register DSFR C15:05 */

MMUREG  mmu_ifsr_get(void);                                 /* MMU Register IFSR C15:05 */
MMUREG  mmu_ifsr_set(MMUREG);                               /* MMU Register IFSR C15:05 */

MMUREG  mmu_far_get(void);                                  /* MMU Register FAR C15:06 */
MMUREG  mmu_far_set(MMUREG);                                /* MMU Register FAR C15:06 */

MMUREG  mmu_clr_icache_get(void);                           /* MMU Register CLR ICache C15:09 */
MMUREG  mmu_clr_icache_set(MMUREG value);                   /* MMU Register CLR ICache C15:09 */

MMUREG  mmu_clr_dcache_get(void);                           /* MMU Register CLR DCache C15:09 */
MMUREG  mmu_clr_dcache_set(MMUREG value);                   /* MMU Register CLR DCache C15:09 */

MMUREG  mmu_tlblr_get(void);                                /* MMU Register TLBLR C15:0A */
MMUREG  mmu_tlblr_set(MMUREG value);                        /* MMU Register TLBLR C15:0A */

MMUREG  mmu_fcsepid_get(void);                              /* MMU Register FCSEPID C15:0D */
MMUREG  mmu_fcsepid_set(MMUREG value);                      /* MMU Register FCSEPID C15:0D */

MMUREG  mmu_contextid_get(void);                            /* MMU Register CONTEXTID C15:0D */
MMUREG  mmu_contextid_set(MMUREG value);                    /* MMU Register CONTEXTID C15:0D */

/* Exported MMU Accessor Functions
 */

void mmu_enable(void);                                      /* Address Translation Enable */
void mmu_disable(void);                                     /* Address Translation Disable */

void mmu_icache_enable(void);                               /* Set Instruction Cache Active */
void mmu_icache_disable(void);                              /* Set Instruction Cache Inactive */

void mmu_dcache_enable(void);                               /* Set Data Cache Active */
void mmu_dcache_disable(void);                              /* Set Data Cache Inactive */

void mmu_dcache_clean_line(void *addr);                   /* DCache Clean */
void mmu_dcache_inval_line(void *addr);                   /* DCache Invalidate */
void mmu_dcache_flush_line(void *addr);                   /* DCache Clean & Invalidate */

void mmu_dcache_clean_region(void *addr, uint32 length);	/* DCache Clean */
void mmu_dcache_inval_region(void *addr, uint32 length);	/* DCache Invalidate */
void mmu_dcache_flush_region(void *addr, uint32 length);	/* DCache Clean & Invalidate */

void mmu_dcache_clean_all(void);                            /* DCache Clean */
void mmu_dcache_inval_all(void);                            /* DCache Invalidate */
void mmu_dcache_flush_all(void);                            /* DCache Clean & Invalidate */

void mmu_icache_precache_line(void *addr);
void mmu_icache_precache_region(void *addr, uint32 length);

void mmu_standby_wfi(void);
void mmu_drain_writes(void);

void mmu_invalidate_tlb(void);                              /* Invalidate Complete TLB */

void mmu_domain_efs_client(void);                           /* Set EFS Context Client */
void mmu_domain_efs_manager(void);                          /* Set EFS Context Manager */

#endif /* ARMMMU_H */
