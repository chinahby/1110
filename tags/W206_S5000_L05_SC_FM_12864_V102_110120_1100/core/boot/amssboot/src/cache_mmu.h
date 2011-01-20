#ifndef CACHE_MMU_H
#define CACHE_MMU_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     A R M 9 2 6 E J - S   C A C H E   A N D   M M U   C O N T R O L

DESCRIPTION

  This header file contains the public interface for the ARM926EJ-S cache
  and mmu control functions.

EXTERNALIZED FUNCTIONS

boot_cache_mmu_init
  Initialize the ARM926EJ-S MMU and enable the MMU and caches.

cache_mmu_disable
  Disable the ARM926EJ-S MMU and caches.

cache_mmu_re_enable
  Re-enable the ARM926EJ-S MMU and caches.

mmu_invalidate_data_cache
  Invalidate all ARM926EJ-S data cache lines.

mmu_invalidate_data_cache_lines
  Invalidate ARM926EJ-S cache lines for a specified address range.

mmu_invalidate_instruction_cache
  Invalidate all ARM926EJ-S data cache lines.

mmu_enable_instruction_cache
  Enable the ARM926EJ-S instruction cache.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  None.

Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msm6800/drivers/boot/2.1/cache_mmu.h#4 $    $DateTime: 2005/11/23 16:03:51 $ $Author: rajeevp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/04   rp      Added Progressive boot support
04/15/04   AP      Ported for MSM6550 from MSM6500.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "msm.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/



/*===========================================================================

                       FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

  FUNCTION  BOOT_CACHE_MMU_INIT

  DESCRIPTION
    Initialize and enable the MMU, and enable the I-cache and D-cache.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.
  SIDE EFFECTS
    None.

===========================================================================*/
void boot_cache_mmu_init( void );


/*===========================================================================

  FUNCTION  CACHE_MMU_DISABLE

  DESCRIPTION
    Disable the ARM926EJ-S MMU and caches.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    Instruction and data caches will be disabled.

===========================================================================*/
void cache_mmu_disable( void );


/*===========================================================================

  FUNCTION  CACHE_MMU_RE_ENABLE

  DESCRIPTION
    Re-enable the ARM926EJ-S MMU and caches.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    Instruction and data caches will be enabled.

===========================================================================*/
void cache_mmu_re_enable( void );
  
  
/*===========================================================================

  FUNCTION  MMU_INVALIDATE_DATA_CACHE

  DESCRIPTION
    Invalidate all ARM926EJ-S data cache lines.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    All data cache lines will be invalidated.

===========================================================================*/
void mmu_invalidate_data_cache( void );
  
/*===========================================================================

  FUNCTION  MMU_CLEAN_AND_INVALIDATE_DATA_CACHE

  DESCRIPTION
    Clean and invalidate all ARM926EJ-S data cache lines.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    All data cache lines will be invalidated.

===========================================================================*/
void mmu_clean_and_invalidate_data_cache( void );

 
/*===========================================================================

  FUNCTION  MMU_INVALIDATE_DATA_CACHE_LINES

  DESCRIPTION
    Invalidate ARM926EJ-S cache lines for a specified address range.
      
  PARAMETERS
    start_ptr - base address of data region to invalidate.
    byte_cnt  - number of bytes in data region to invalidate.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    Data cache lines that contain bytes from the specified region will be
    invalidated.

===========================================================================*/
void mmu_invalidate_data_cache_lines
(
  uint32*  start_ptr,
  uint32   byte_cnt
);

 
/*===========================================================================

  FUNCTION  MMU_INVALIDATE_INSTRUCTION_CACHE

  DESCRIPTION
    Invalidate all ARM926EJ-S instruction cache lines.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    All instruction cache lines will be invalidated.

===========================================================================*/
void mmu_invalidate_instruction_cache( void );

  
/*===========================================================================

  FUNCTION  MMU_ENABLE_INSTRUCTION_CACHE

  DESCRIPTION
    Enable the ARM926EJ-S instruction cache.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The ARM926EJ-S instruction cache will be enabled.

===========================================================================*/
void mmu_enable_instruction_cache( void );


/*===========================================================================

  FUNCTION  MMU_INVALIDATE_TLB_SINGLE_ENTRY

  DESCRIPTION
    Invalidates the ARM926EJ-S TLB entry for the specified  address.

  PARAMETERS
    The virtual address for which the TLB entry needs to be invalidated.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The ARM926EJ-S instruction cache will be enabled.

===========================================================================*/

void  mmu_invalidate_tlb_single_entry
(
	uint32 virtual_address
);

  
/*===========================================================================

  FUNCTION  MMU_DISABLE_DATA_CACHE

  DESCRIPTION
    Disable the ARM926EJ-S data cache.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The ARM926EJ-S data cache will be enabled.

===========================================================================*/
void mmu_disable_data_cache( void );
/*===========================================================================

  FUNCTION  MMU_RE_ENABLE_DATA_CACHE

  DESCRIPTION
    Re-enables the ARM926EJ-S data cache.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None

  RETURN VALUE
    None.

  SIDE EFFECTS
    The ARM926EJ-S data cache will be enabled.

===========================================================================*/
void mmu_re_enable_data_cache( void );



/*===========================================================================

  FUNCTION  MMU_ENABLE

  DESCRIPTION
    Enables the ARM926EJ-S MMU by modifying the CPSR15 cotrol register.
	Only the MMU enable bit is modified all other bits remain the same.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    Data caches will be enabled if it was previously enabled.

===========================================================================*/
void mmu_enable( void );


/*===========================================================================

  FUNCTION  MMU_DISABLE

  DESCRIPTION
    Disables the ARM926EJ-S MMU by modifying the CPSR15 cotrol register.
	Only the MMU enable bit is modified all other bits remain the same.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    Data caches will be disabled.

===========================================================================*/
void mmu_disable( void );


#endif /* CACHE_MMU_H */
