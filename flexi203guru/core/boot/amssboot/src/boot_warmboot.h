#ifndef _BOOT_WARMBOOT_H_
#define _BOOT_WARMBOOT_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      B O O T   W A R M B O O T  
      I N I T I A L I Z A T I O N    H E A D E R

GENERAL DESCRIPTION
  This header file contains function definitions specific for warm boot back up
  and restore.

Copyright (c) 2005,2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/boot/boot_warmboot.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/05   tkuo    Created.
===========================================================================*/

#include "boothw.h"
/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
/* Type of which operation to perform the warm boot operation */
typedef enum
{
	WARMBOOT_OPT_BACKUP =0,
	WARMBOOT_OPT_RESTORE,
	WARMBOOT_OPT_MAX=0x7FFFFFFF
}boot_warmboot_opt_type;
typedef  void (*boot_warmboot_func_ptr_type)( void );
/*===========================================================================

FUNCTION  BOOT_WARMBOOT_HANDLER

DESCRIPTION
   This function performs warm boot backup and restore.  It performs memory 
 settings backup and restore. 
 For SDRAM, we put it into self refresh mode to keep the content.
 For PSRAM, it won't get reset after MSM is reset.
 PBL will do minial recovery of memory settings to allow warm boot handler to 
 continue after jumping to SDRAM or PSRAM.
  

DEPENDENCIES
  The warm_boot_func_ptr is always located in RAM.  
  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void boot_warmboot_handler(boot_flash_mem_cfg_type warmboot_mem,
						   boot_warmboot_opt_type warmboot_opt,
                           uint32 *warmboot_storage_ptr,
                           boot_warmboot_func_ptr_type  warmboot_handler_func_ptr
						   );

#endif /*_BOOT_WARMBOOT_H_*/

