/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  BOOT MAIN CONTROL 

 GENERAL DESCRIPTION
   This file contains the necessary functionality which needs to be performed
   in AMSS boot process. Please be aware that the order of functions is 
   very important.


 INITIALIZATION AND SEQUENCING REQUIREMENTS
   Routines within this module automatically obtain control of the processor
   following a boot sequence. Control may also return to this module 
   if main() exits unexpectedly, if an unexpected hardware or software 
   interrupt occurs, or if a fatal error occurs.

 Copyright (c) 2004-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*============================================================================

                       EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $PVCSPath:  $
  $Header: //depot/asic/qsc1100/drivers/boot/boot_mc.c#2 $

 when      who     what, where, why
============================================================================
01/11/06   ty      Added support for logging PMIC power on status
09/14/05   tbg     Added FEATURE_LOG_RESET_REGISTERS.
05/06/05   tbg     Added support for reset detection and logging.
12/08/04   tkuo    Removed boot_hw_tlmm_init.
10/19/04   tkuo    Created for MSM6275
============================================================================*/

/*============================================================================

                             MODULE IMPORTS

============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "cache_mmu.h"
#include BOOTHW_MSM_H
#include "bootmem.h"
#include "boot_stacks.h"
#include "boot_trap.h"
#include "boothw.h"

#ifdef FEATURE_SHARED_SEGMENT
#include "boot_shared_seg.h"
#endif

/*============================================================================

                             MODULE EXPORTS

============================================================================*/

#ifdef FEATURE_ERR_EXTENDED_STORE
#ifdef FEATURE_LOG_RESET_REGISTERS
void boot_err_reset_check (void);
#endif
#endif

/*===========================================================================

FUNCTION  BOOT_MAIN_CTL

DESCRIPTION
   The AMSS BOOT Main Controller performs the following functions:
        
      - Sets up the hardware with the configuration required.
        for AMSS execution.
      - Perform RAM initialization.
      - Reads off-chip configuration data from IRAM into external RAM. 

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE-means succeed. FLASE-means boot failed. 
SIDE EFFECTS
  None

===========================================================================*/
void boot_main_ctl(void) 
{
 /*-----------------------------------------------------------------------
  Copy the AMSS Vector Table into IMEM 
 -----------------------------------------------------------------------*/ 
 boot_create_vector_table();

 /*-----------------------------------------------------------------------
  Enable the instrunction cache.
 -----------------------------------------------------------------------*/ 
 mmu_enable_instruction_cache();
 
 #ifdef FEATURE_ERR_EXTENDED_STORE

 /*-----------------------------------------------------------------------
  Perform shared segment initialization before error handling code runs
 -----------------------------------------------------------------------*/ 
 #ifdef FEATURE_SHARED_SEGMENT
 boot_shared_seg_init();
 #endif

 #ifdef FEATURE_LOG_RESET_REGISTERS
 /* This will restore registers from stack and log registers values that were
  * current when AMSS started execution.
  */
 boot_err_reset_check();
 #else
 /* This bypasses the register restore and simply zeros out register values in
  * the logs.  This is the case if the boot loader has not saved the registers
  * for us -- we zero them out in the log to make it clear they were not saved.
  */
 boot_process_err_reset_check();
 #endif
 #endif
 
 /*-----------------------------------------------------------------------
  Perform RAM initialization.
 -----------------------------------------------------------------------*/ 
 boot_ram_init();  

 /*-----------------------------------------------------------------------
  Perform shared segment initialization after ram is initialized
 -----------------------------------------------------------------------*/ 
 #ifdef FEATURE_SHARED_SEGMENT
 boot_shared_seg_init();
#endif 

 /*-----------------------------------------------------------------------
  Perform Stack initialization for different modes.
 -----------------------------------------------------------------------*/ 
 boot_stack_initialize();

 /*-----------------------------------------------------------------------
  Call TLMM  init
 -----------------------------------------------------------------------*/ 
 boot_hw_tlmm_init();
 /*-----------------------------------------------------------------------
  Enable the Data cache.
 -----------------------------------------------------------------------*/ 
 boot_cache_mmu_init();
 return;
}/* boot_main_ctl() */
