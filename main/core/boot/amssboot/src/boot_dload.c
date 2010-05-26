/*=============================================================================

                         B o o t  D o w n l o a d e r
 
GENERAL DESCRIPTION
  This file contains the code to initiate a reset of the chip and
  transition to the into the downloader.

EXTERNALIZED FUNCTIONS
  boot_dload_transition
  boot_dload_entry

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2009 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/boot/boot_dload.c#5 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/21/09   dk      Adding support for FEATURE_DLOAD_HW_RESET_DETECT.
10/16/08   tnk     Modified to exclude hs_usb feature exclusion while boot compilation
05/08/07   ty      Modify structure saved to IRAM to include serial number
03/26/07   tbg     Added support for FEATURE_DLOAD_HW_RESET_DETECT
01/24/07   ly      Add HS_USB support
10/13/05   sd      Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_dload.h"
#include "msm.h"
#include "cache_mmu.h"



#ifndef BUILD_BOOT_CHAIN
#ifdef FEATURE_HS_USB
  #include "hsu_boot_dload.h"
#endif
#endif

#ifdef FEATURE_DLOAD_HW_RESET_DETECT
#include "boot_hw_reset.h"
#endif
/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Magic numbers for notifying OEMSBL to boot into download mode. */
#define MAGIC_NUM_1     0xE47B337D
#define MAGIC_NUM_2     0xCE14091A

/* Location to store ID in IRAM. */
#define DLOAD_ID_ADDR   0x5801FFF8

/* WDOG timer count until WDOG expires in 32KHz sleep clocks. */
#define WDOG_COUNT  1000


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

#ifndef BUILD_BOOT_CHAIN
extern void dcache_clean_line(uint32 *addr);       



/*===========================================================================

FUNCTION  BOOT_DLOAD_TRANSITION

DESCRIPTION
  This function initiates the reset of the chip and transition to the
  boot downloader in the OEMSBL during boot.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Never returns and causes chip reset to occur.

===========================================================================*/
void boot_dload_transition( void )
{
  dload_id_type *dload_id_ptr = (dload_id_type*) DLOAD_ID_ADDR;
#ifdef FEATURE_HS_USB
  hsu_boot_dload_transition();
#endif  /* FEATURE_HS_USB */
  /* Set watchdog counter to a known value so it expires soon. */
  HWIO_OUT( WDOG_TEST, WDOG_COUNT );
  HWIO_OUT( WDOG_TEST_LOAD,
            HWIO_FMSK( WDOG_TEST_LOAD, LOAD ) );
  
  /* Set download ID. */
  dload_id_ptr->magic_1 = MAGIC_NUM_1;
  dload_id_ptr->magic_2 = MAGIC_NUM_2;

  #ifdef FEATURE_DLOAD_HW_RESET_DETECT
  /* If AMSS code is putting the phone in download mode then make sure
   * that the magic number in the shared memory is initialized to zero.
   * Since this is not an abnormal reset
   */
    boot_hw_reset_shared_info_reset();
  #endif
  
  /* Make sure the cache line is flushed (ID flushed). */
  dcache_clean_line( (uint32*)dload_id_ptr );

 #if 0
  /* Assert reset for all chip clock branches. */
  HWIO_OUT( RESET_ALL,
            HWIO_FMSK( RESET_ALL, GLOBAL ) );
 #endif
  
  /* Wait for watchdog to expire */
  for( ;; ){}
  
} /* boot_dload_transition() */
#endif /* BUILD_BOOT_CHAIN*/

#ifdef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  BOOT_DLOAD_ENTRY

DESCRIPTION
  Determines if identifier is present in IRAM to tell OEMSBL to enter
  the boot downloader, instead on continuing the normal boot process.
  
DEPENDENCIES
  Data in IRAM is retained across a reset.
  
RETURN VALUE
  TRUE indicates downloader should be entered.

SIDE EFFECTS
  None

===========================================================================*/
boolean boot_dload_entry( void )
{
  dload_id_type *dload_id_ptr = (dload_id_type*) DLOAD_ID_ADDR;

  /* Check to see if download ID is present. */
  if ( dload_id_ptr->magic_1 == MAGIC_NUM_1 &&
       dload_id_ptr->magic_2 == MAGIC_NUM_2 )
  {
    /* Clear ID so the downloader is not entered again on the next boot. */
    dload_id_ptr->magic_1 = 0;
    dload_id_ptr->magic_2 = 0;
    
    return TRUE;
  }
  else
  {
#ifdef CUST_EDITION
    return keypad_is_dload_key_pressed();//lucid Modify for support dload key
#else
    return FALSE;
#endif
    }
  
} 
   /* boot_dload_entry() */
#endif /* BUILD_BOOT_CHAIN */

