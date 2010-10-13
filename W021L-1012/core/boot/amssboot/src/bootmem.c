/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  B O O T  M E M O R Y  I N I T I A L I Z A T I O N

GENERAL DESCRIPTION
  This module handles the AMSS Boot block memory initialization.
  The boot memory read/write data segments are also initialized in this module.

EXTERNALIZED FUNCTIONS
  boot_ram_init()
    Zero-Out ZI RAM and copy initialized data (RW-DATA) into RAM
  boot_load_pbl_to_iram()
    Load PBL code into IRAM for NOR flash.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  All necessary initialization for normal CPU operation must have
  been performed before entering this module.

Copyright (c) 2004-2009 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/bootmem.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/09  dk       Removing unwanted code in bootmem.c
04/09/09   dk      Added memset kind of functionality for optimised
                   ZI init timings. 
04/09/09   raja    HSUSB buffer update.
11/30/07   ps      Adding support for SECBOOT 2.0
05/16/07   gdp     Decreased HEAP to 24MB to fit in Physical RAM.
04/24/07   amw     Cleanup lint errors.
07/23/05   wd      L4 Merge.
04/06/05   chs     Renamed target_ram_start_addr to
                   cfg_item_target_ram_start_addr and target_ram_size to
                   cfg_item_target_ram_size.
03/09/05   chs     Read ram_start_addr and ram_size from the Feature
                   config data.
12/01/04   chs     Modified to make it compatible for NOR/NAND boot.
12/01/04   chs     Replace hard-coded addresses with #defines.
10/19/04   tkuo    Ported from MSM6250.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "armasm.h"
#include "boothw.h"
#include "target.h"
#include "customer.h"
#include "bootmem.h"
#include "miboot.h"
#include "miprogressive.h"
#include "err.h"
#ifdef FEATURE_DLOAD_HW_RESET_DETECT
#include "boot_hw_reset.h"
#endif
#include "memory_map_full.h"


#ifndef FEATURE_UI_CORE_REMOVED
#include "OEMFeatures.h"
#include "AEE_OEM.h"
uint32* oem_dynamic_heap_pointer    ;
uint32 oem_dynamic_heap_size;
#endif

#if defined FEATURE_SI_STACK_WM
#error code not present
#endif

#if defined(FEATURE_SHARED_SEGMENT)
  #include "boot_shared_seg.h"
#if defined(FEATURE_PBOOT_BLOCK_SHARED_SEG)
  #include "boot_shared_progressive_boot_block.h"
#endif
#endif

#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_PBOOT_BLOCK_SHARED_SEG)
#include "boot_shared_progressive_boot_block.h"
#endif
/*----------------------------------------------------------------------------
  Static LCD Error Messages
----------------------------------------------------------------------------*/
const char rom_error_msg[] = "ROM FAILED";
#ifdef FEATURE_RELOCATE_RW_DATA
#define RW_DATA_UNINITIALIZED_VALUE 0xaaaaaaaa
#define RW_DATA_INITIALIZED_VALUE   0x55555555

static uint32 rw_data_initialized = RW_DATA_UNINITIALIZED_VALUE;
#endif

#ifdef FEATURE_BREW_3_0
extern void * gpuHeapBytes;
extern uint32 guHeapBytesLen;
#endif

extern void boot_zi_init(void *dest, uint32 length);

/*============================================================================

  FUNCTION BOOT_RAM_INIT

  DESCRIPTION
    Zero-Out RAM and copy initialized data (RW-DATA) into RAM for both
    the bootblock (BB_RAM) and for the application area (APP_RAM).
    Note that this function cannot use any variables stored in RAM.
    In theory we can use the stack for local variables, but we're not
    going to do that just to be safe.  Therefore, we bind our local
    variables to registers 1-3.

    See the scatter loading file (*.scl) for a description of the
    structure of RAM.

  FORMAL PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    Memory not in the initialized image or in the zero-init section is
    not affected by this function -- those locations are left in an
    unknown state.  Also, the watchdog timer is kicked more often than
    is strictly necessary, but that is a cheap (time-wise) operation
    and will prevent future problems (in case we increase our memory usage).

============================================================================*/
__global_reg(1) dword *dst32;
__global_reg(2) dword *src32;
__global_reg(3) dword *stop_point;


void boot_ram_init(void)
{
  uint32  oem_heap_start;
  uint32  ram_start_addr;
  uint32  ram_size;
  uint32  segment_len = 0x0;
  uint8   *ram_offset = 0x0;

  /* Defined for kicking watchdog */
  #define ZI_INIT_CHUNK_SIZE (1024 * 1024)

  /* kick watchdog at least every 426 ms. */
  BOOTHW_KICK_WATCHDOG();

  /* Zero out the zero-init data segment of the boot block
   *
   * This writes zeros into the RAM in the zero-init section of the
   * boot block ram.  This area starts at Image__ZI_REGION__ZI_Base and
   * continues for Image__ZI_REGION__ZI_Length dwords.
   */
  segment_len = (uint32)(Image__ZI_REGION__ZI__Length);
  ram_offset  = (uint8 *)Image__ZI_REGION__ZI__Base;


    do
    {
      BOOTHW_KICK_WATCHDOG(); 
      if(ZI_INIT_CHUNK_SIZE >= segment_len)
      {
        boot_zi_init((void *)ram_offset,segment_len);
      break;
      }
      else
      {
        boot_zi_init((void *)ram_offset,ZI_INIT_CHUNK_SIZE);
        segment_len -= ZI_INIT_CHUNK_SIZE;
        ram_offset += ZI_INIT_CHUNK_SIZE;
      }
    }while(0 < segment_len);

#ifdef FEATURE_WB_CACHE_REGION
  /* Zero out the WB_CACHE region, since some of this data was "relocated"
   * from the zero-init section of the boot block ram.
   *
   * This writes zeros into the RAM in the zero-init section of the
   * boot block ram.  This area starts at T_WBCACHE_BASE and
   * continues for T_WBCACHE_SIZE bytes.
   */
  segment_len = (uint32)(T_WBCACHE_SIZE);
  ram_offset  = (uint8 *)T_WBCACHE_BASE;

    do
    {
      BOOTHW_KICK_WATCHDOG(); 
      if(ZI_INIT_CHUNK_SIZE >= segment_len)
      {
        boot_zi_init((void *)ram_offset,segment_len);
        break;
      }
      else
      {
        boot_zi_init((void *)ram_offset,ZI_INIT_CHUNK_SIZE);
        segment_len -= ZI_INIT_CHUNK_SIZE;
        ram_offset += ZI_INIT_CHUNK_SIZE;
      }
    }while(0 < segment_len);

  /* kick watchdog at least every 426 ms. */
  BOOTHW_KICK_WATCHDOG();
#endif // FEATURE_WB_CACHE_REGION

#if defined FEATURE_SI_STACK_WM
#error code not present
#endif /*  FEATURE_SI_STACK_WM   */

/* Set the values for OEM dynamic heap pointer and size.*/
  oem_heap_start =   (dword) Image__UNINIT_RAM__ZI__Base +
                     (dword) Image__UNINIT_RAM__ZI__Length;

  oem_heap_start += 16;
  oem_heap_start &=  0xfffffff0;

  /*-----------------------------------------------------------------------
   Perform shared segment initialization before error handling code runs
  -----------------------------------------------------------------------*/
  #ifdef FEATURE_SHARED_SEGMENT
  boot_shared_seg_init();
  #endif

  ram_start_addr = T_RAM_BASE;
  ram_size = T_RAM_SIZE;
  #ifndef FEATURE_UI_CORE_REMOVED
  oem_dynamic_heap_pointer = (uint32 *) oem_heap_start;
  oem_dynamic_heap_size    = ram_start_addr + ram_size - oem_heap_start;
#ifdef FEATURE_WB_CACHE_REGION
  /* Actually - Brew heap gets reduced by T_WBCACHE_SIZE */
  oem_dynamic_heap_size -= T_WBCACHE_SIZE;
#endif // FEATURE_WB_CACHE_REGION

  /* Reduce Brew Heap by size of USB buffers */
  /* TBP (to be ported) Needs fixing after we use a 2nd level MMU
     pagetable to reduce the size of the USB buffer region */
  oem_dynamic_heap_size -= (QSC11x0_PMEM_DATA_SIZE*1024);

  #endif /* FEATURE_UI_CORE_REMOVED */

#ifdef FEATURE_BREW_3_0
  gpuHeapBytes   = oem_dynamic_heap_pointer;
  guHeapBytesLen = oem_dynamic_heap_size;
#else
  #ifndef FEATURE_UI_CORE_REMOVED
    OEM_InitHeap(oem_dynamic_heap_pointer, oem_dynamic_heap_size);
  #endif
#endif

  return ;
}

/*============================================================================

FUNCTION BOOT_CREATE_VECTOR_TABLE

DESCRIPTION
  Copy AMSS Vector table from Flash into IMEM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
============================================================================*/
void boot_create_vector_table( void )
{
 register uint32 *dst32;
 register uint32 *src32;
 register uint32 *stop_point;

  /* Copy Vector table to internal RAM to create vector table in IRAM.*/
  stop_point = (uint32 *) ((uint32) Image__AMSS_IMEM_VEC_TBL__Base + 
                           Image__AMSS_IMEM_VEC_TBL__Length);

  /* Start to copy */
  for( src32 = (uint32 *) Load__AMSS_IMEM_VEC_TBL__Base,
       dst32 = (uint32 *) Image__AMSS_IMEM_VEC_TBL__Base; 
       dst32 < stop_point; src32++, dst32++ )
  {
    *dst32 = *src32;
  }
  
} /* boot_create_vector_table() */
