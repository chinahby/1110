/*=============================================================================

               OS  S e c o n d a r y   B o o t   L o a d e r
                         M a i n  C o n t r o l

GENERAL DESCRIPTION
  This file contains the main control for OSBL execution.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007-2009 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/osbl/osbl_mc_target.c#6 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/21/09   dk      Adding support for FEATURE_DLOAD_HW_RESET_DETECT.
03/19/09   dk      Reset logging support for ULC target.
02/03/09   tnk	   Added support for Secure boot testing
10/09/07   ly      Created
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "osbl.h"
#include "osbl_mc_target.h"

#include "osbl_dload.h"
#include "osbl_hw_init.h"
#include "osbl_flash.h"
#include "osbl_flash_target.h"
#include "osbl_loader.h"
#include "osbl_error_handler.h"
#include "osbl_hash.h"
#include "osbl_auth.h"
#include "boot_sec_elf_loader.h"
#include "boot_util.h"
#include "boot_pbl_accessor.h"

#ifdef FEATURE_SHARED_SEGMENT
  #include "boot_shared_seg.h"
  #include "osbl_shared_seg.h"
#endif /* FEATURE_SHARED_SEGMENT */

/* Array to store the reset register data passed from Boot ROM */
uint32 register_log[26];
/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


#ifdef BOOT_USES_DCACHE
/*===========================================================================

FUNCTION  OSBL_BOOT_CACHE_MMU_INIT

DESCRIPTION
  Wrapper function that calls boot_cache_mmu_init().
 
DEPENDENCIES
  None
  
RETURN VALUE
  None                                                               

SIDE EFFECTS
  None

===========================================================================*/
static void osbl_boot_cache_mmu_init( bl_shared_data_type *bl_shared_data )
{
  boot_cache_mmu_init();
} /* osbl_boot_cache_mmu_init() */


/*===========================================================================

FUNCTION  OSBL_BOOT_CACHE_RE_ENABLE

DESCRIPTION
  Wrapper function that calls cache_mmu_re_enable().
 
DEPENDENCIES
  None
  
RETURN VALUE
  None                                                               

SIDE EFFECTS
  None

===========================================================================*/
static void osbl_boot_cache_re_enable( bl_shared_data_type *bl_shared_data )
{
  cache_mmu_re_enable();
} /* osbl_boot_cache_re_enable() */
#endif /* BOOT_USES_DCACHE */

#ifdef FEATURE_SHARED_SEGMENT
/*===========================================================================

FUNCTION  OSBL_BOOT_SHARED_SEG_REINIT

DESCRIPTION
  Wrapper function that calls boot_shared_seg_reinit(TRUE).
 
DEPENDENCIES
  None
  
RETURN VALUE
  None                                                               

SIDE EFFECTS
  None

===========================================================================*/
static void osbl_boot_shared_seg_reinit
(
  bl_shared_data_type *bl_shared_data
)
{
  /* Re-init shared segment after dload check */
  boot_shared_seg_reinit(TRUE);
} /* osbl_boot_shared_seg_reinit() */
#endif /* FEATURE_SHARED_SEGMENT */

/*===========================================================================

FUNCTION  OSBL_INIT_MODULES

DESCRIPTION
  Initializes the OSBL modules and locks the module interfaces.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None                                                               

SIDE EFFECTS
  None

===========================================================================*/
static void osbl_init_modules( bl_shared_data_type *bl_shared_data )
{
  OSBL_VERIFY( bl_shared_data != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Use the OSBL authentication module if the FSBL one doesn't exist */
  if ( ! boot_auth_if_singleton_exists() )
  {
   /*  osbl_auth_init_and_register();  */
  }
  /* Now lock the authentication interface */
  boot_auth_if_lock_singleton();

  /* Use the OSBL hashing module if the FSBL one doesn't exist */
  if ( ! boot_hash_if_singleton_exists() )
  {
    /*  osbl_hash_init_and_register();   */
  }
  /* Now lock the hashing interface */
  boot_hash_if_lock_singleton();

  /* Lock the downloader and FSBL config interfaces */
  boot_dload_if_lock_singleton();
  boot_fsbl_config_if_lock_singleton();

  /* Initialize and lock the elf loader factory */
  boot_elf_loader_factory_init_and_register();
  boot_elf_loader_factory_if_lock_singleton();

  /* Initialize and lock the secboot elf loader factory */
  boot_sec_elf_loader_factory_init_and_register();
  boot_sec_elf_loader_factory_if_lock_singleton();

} /* osbl_init_modules() */

/*===========================================================================

FUNCTION  OSBL_SAVE_RESET_REGISTER_LOG

DESCRIPTION
  Save reset registers to global variable register_log. This is used to 
  debug hardware reset by dumping registers in downloader mode.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None                                                               

SIDE EFFECTS
  None

===========================================================================*/
static void osbl_save_reset_register_log( bl_shared_data_type *bl_shared_data )
{

	/* Backup 26 uint32 reset registers stored by PBL at BOOT_STACK_BASE.
     SP and LR SYS, IRQ, FIQ, ABT, UDF - 10 uint32 registers
	 SVC_R0
	 SVC_R1 
	 ......
	 ......
	 SVC_SP <---- (BOOT_STACK_BAES - 0x8)
	 SVC_LR <---- (BOOT_STACK_BAES - 0x4)
	 SVC_PC <---- (BOOT_STACK_BASE)
    */
	qmemcpy((void *)register_log, 
	        (const void *)(boot_pbl_get_boot_stack_base()- 0x68), 0x68);

}

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/* Main list of OSBL procedures */
osbl_procedure_func_type osbl_main_procs[] =
{
  /*-----------------------------------------------------------------------
   * Save the registers in a variable from a known location 
   *-----------------------------------------------------------------------*/
  osbl_save_reset_register_log,
  /*-----------------------------------------------------------------------
   * Initialize minimal hw for USB charging 
   *-----------------------------------------------------------------------*/
  osbl_hw_init,

#ifdef FEATURE_OSBL_USB_BATT_CHG
  /*-----------------------------------------------------------------------
   * Perform dead battery charging if needed 
   *-----------------------------------------------------------------------*/
  osbl_usb_chg_init,
#endif /* FEATURE_OSBL_USB_BATT_CHG */

  /*-----------------------------------------------------------------------
   * Increase clk speed after charging is done
   *-----------------------------------------------------------------------*/
  osbl_increase_clk_speed,

  /*----------------------------------------------------------------------- 
    Initialize addtional hw after bumping up clk_speed (place holder)
   ------------------------------------------------------------------------*/
#ifdef BOOT_USES_DCACHE
  /*-----------------------------------------------------------------------
    Setup the pagetable and MMU, enable the data cache.
    -----------------------------------------------------------------------*/
  osbl_boot_cache_re_enable,
#endif /* BOOT_USES_DCACHE  */
  /*-----------------------------------------------------------------------
   * Initialize the OSBL modules and lock the interfaces.
   *-----------------------------------------------------------------------*/
  osbl_init_modules,

  /*-----------------------------------------------------------------------
   * Initialize the flash device
   *-----------------------------------------------------------------------*/
  osbl_flash_init,

  /*-----------------------------------------------------------------------
   * Get shared data out of the flash device module
   *-----------------------------------------------------------------------*/
  osbl_share_flash_data,
  
  /*-----------------------------------------------------------------------
   * Initialize the data structures for loading the AMSS image
   *-----------------------------------------------------------------------*/
  osbl_init_amss_image,

  /*-----------------------------------------------------------------------
   * Retrieve the JTAG debug flag from the AMSS image and update
   * the hardware
   *-----------------------------------------------------------------------*/
  osbl_process_amss_debug_flag,
  
  /*----------------------------------------------------------------------- 
    Initialize the shared segment 
    -----------------------------------------------------------------------*/
#ifdef FEATURE_SHARED_SEGMENT
  osbl_init_shared_segment,
#endif /* FEATURE_SHARED_SEGMENT */

  /*-----------------------------------------------------------------------
   * Check to see if DLOAD mode needs to be entered
   *-----------------------------------------------------------------------*/
  osbl_dload_check,

#ifdef FEATURE_SHARED_SEGMENT
  /*-----------------------------------------------------------------------
   * Re-init shared segment after dload check
   *-----------------------------------------------------------------------*/
  osbl_boot_shared_seg_reinit,
#endif /* FEATURE_SHARED_SEGMENT */

  /*-----------------------------------------------------------------------
   * Load the AMSS image 
   *-----------------------------------------------------------------------*/
	osbl_auth_amss_image,
   osbl_load_amss_image,


#ifdef FEATURE_SHARED_SEGMENT
  /*-----------------------------------------------------------------------
   * Save data to the shared segment before jumping to AMSS
   *-----------------------------------------------------------------------*/
   osbl_save_shared_segment_info,
#endif /* FEATURE_SHARED_SETMENT */

  /*-----------------------------------------------------------------------
   * NULL pointer indicates the end of the list
   *-----------------------------------------------------------------------*/
  NULL
};

