#ifndef __FLASH_NOR_UTILS_H__
#define __FLASH_NOR_UTILS_H__

/**********************************************************************
 * flash_nor_utils.h
 *
 * Flash Utilites 
 *
 * This file implements the utility functions for nor flash
 *
 * Copyright (c) 2006, 2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_nor_utils.h#2 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------

2008-03-11   sv      Modified for supporting M18 in half-density mode
2006-12-11   mm      Initial Version

===========================================================================*/

#include "flashi.h"

/*===========================================================================

FUNCTION flash_nor_find_block_start_addr

DESCRIPTION
  This function starts at block 0 and will find the start byte address for,
  the block number provided.  

DEPENDENCIES

RETURN VALUE
  If block found, return block number
  If block not found, return -1

SIDE EFFECTS

===========================================================================*/
uint32 flash_nor_find_block_start_addr(flashi_nor_device *self, 
  int which_block);


/*===========================================================================

FUNCTION flash_nor_find_block_num_for_start_addr

DESCRIPTION
  This function will find the block number starting from 0 for a provided
  start byte address. 

DEPENDENCIES

RETURN VALUE
  If block found, return block number
  If block not found, return -1

SIDE EFFECTS

===========================================================================*/

uint32 flash_nor_find_block_num_for_start_addr(fs_device_t self, 
  dword start_addr);

/*===========================================================================

FUNCTION flash_nor_find_size_for_block_num

DESCRIPTION
  This function will find the block size for a provided block number starting 
  from block number 0. 

DEPENDENCIES

RETURN VALUE
  If block found, return block size
  If illegal block number, return 0

SIDE EFFECTS

===========================================================================*/
uint32 flash_nor_find_size_for_block_num(fs_device_t self, uint32 blk_num);

/*===========================================================================

FUNCTION flash_nor_find_size_for_block_addr

DESCRIPTION
  This function will find the block size of a block whose start address 
  is provided as input

DEPENDENCIES

RETURN VALUE
  If block found, return block size
  If illegal block number, return 0

SIDE EFFECTS

===========================================================================*/
uint32  
flash_nor_find_size_for_block_addr (flashi_nor_device *nor_device, 
  uint32 blk_start_addr);   

#endif /* __FLASH_NOR_PARTITION_H__ */
