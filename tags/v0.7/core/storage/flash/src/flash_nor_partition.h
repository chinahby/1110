#ifndef __FLASH_NOR_PARTITION_H__
#define __FLASH_NOR_PARTITION_H__

/**********************************************************************
 * flash_nor_partition.h
 *
 * Flash partition interface
 *
 * This module implements a device-dependent view of flash
 * partition handling.
 *
 * Copyright (c) 2006-2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH_PARTI_MIB.02.00/flash_nor_partition.h#3 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-03-18   sv      Moved get phy limits function declaration to flash_nor.c
2007-09-17   op      Added API to get Physical boundaries of a partition.
2006-12-11   rt/mm   Spawned from 'flash_nand_parition.h'.

===========================================================================*/

#include "flash.h"
#include "flash_partition.h"
#include "miboot.h"

/*------------------------------------------------------------
 *
 *  Public Interface to NOR Flash Partition Functions
 *
 * ---------------------------------------------------------*/

#if !defined(BOOT_LOADER)
/***********************************************************************
FUNCTION      flash_nor_open_partition

DESCRIPTION   
    Opens a partition found by a name string passed in.
    Modifies the private data of the device structure to
    limit the "view" of the device by the device driver
    for all subsequent calls to device functions.

PARAMETERS:
    OUT : self - pointer to the device structure
    IN  : partition_name - partition name

DEPENDENCIES
    The device should be probed before calling this API. The device 
    descriptor 'self' must be Non-NULL.

RETURN VALUE
    FS_DEVICE_OK if successful
    FS_DEVICE_FAIL if any fatal device errors
    
SIDE EFFECTS
    None
**********************************************************************/
int flash_nor_open_partition (fs_device_t self, char *partition_name);

/***********************************************************************
FUNCTION      flash_nor_update_limits

DESCRIPTION   This function uses a pointer to a partition table and
              partition name, and if it finds the partition by name
              it updates the block limits in the device structure
              with the values from the partition table entry.

PARAMETERS:
    OUT : self - pointer to the device structure
    IN  : parti_ptr - pointer to system partition table
    IN  : partition_name - partition name

DEPENDENCIES
    The device should be probed before calling this API. The device 
    descriptor 'self' must be Non-NULL.

RETURN VALUE  
    FS_DEVICE_OK    if partition entry found
    FS_DEVICE_FAIL  if not found
    
SIDE EFFECTS
    None
**********************************************************************/
int flash_nor_update_limits(fs_device_t self, flash_partable_t parti_ptr, 
  char *parti_name);


/***********************************************************************
FUNCTION      flash_nor_convert_usr_parti_tbl_to_sys

DESCRIPTION   This function accepts user defined partition table as an input
              and generates a system partition table.

PARAMETERS:
    IN  : self - pointer to the device structure
    IN  : parti1 - pointer to usr partition table
    OUT : parti2 - pointer to system partition table
    IN  : block_map - flash block mapping table

DEPENDENCIES
    The device should be probed before calling this API. The device 
    descriptor 'self' must be Non-NULL.

RETURN VALUE  
    Returns TRUE if conversion was successful
    Returns FALSE if conversion failed
    
SIDE EFFECTS
    None
**********************************************************************/
int flash_nor_convert_usr_parti_tbl_to_sys (fs_device_t self, 
  flash_usr_partable_t parti1, flash_partable_t parti2, 
  block_descriptor_type *block_map);


#endif /* !defined(BOOT_LOADER) */

#endif /* __FLASH_NOR_PARTITION_H__ */
