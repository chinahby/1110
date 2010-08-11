/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              M E M O R Y   D E B U G   I N F O R M A T I O N 

GENERAL DESCRIPTION
  This module defines the target specific memory debug information to be sent
  to host.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005-2009 by QUALCOMM Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/services/dload/dload_debug_qsc1100.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/21/09   dk      Adding support for FEATURE_DLOAD_HW_RESET_DETECT.
09/18/08   vph     Ported for QSC11x0.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "dload_debug.h"
#include <stdio.h>
#ifdef FEATURE_DLOAD_HW_RESET_DETECT 
#include "boot_hw_reset.h"
#endif

#ifdef FEATURE_DLOAD_MEM_DEBUG
/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_TARGET_INIT

DESCRIPTION
  This function initializes and sets up the memory regions to be used.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

                                                                            
void dload_mem_debug_target_init(void)
{  

#ifdef BOOT_MODE_NOR /* 
  /* Full PSRAM dump */
  dload_debug_add_region(MANDATORY, T_RAM_BASE, T_RAM_SIZE, 
                         "Full PSRAM", "psram_dump_0x1.bin");  

#ifndef BUILD_BOOT_CHAIN

#ifdef FEATURE_DLOAD_HW_RESET_DETECT 
/* if AMSS code is putting the phone in download mode then make sure
* that the magic number in the shared memory is initialized to zero.
* Since this is not an abnormal reset
*/
boot_hw_reset_shared_info_reset();
#endif

#endif /* BUILD_BOOT_CHAIN */

  /* Full NOR dump */
  dload_debug_add_region(MANDATORY, 0, FLASH_NOR_EFS2_START_BYTE_OFFSET, 
                         "NOR Code", "nor_dump_0x0.bin");  
#endif /* BOOT_MODE_NOR */
  
 return;
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_SUPPORT

DESCRIPTION
  This function returns if memory debug is supported.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_supported(void)
{
  return TRUE;
}
#endif /* FEATURE_DLOAD_MEM_DEBUG */

