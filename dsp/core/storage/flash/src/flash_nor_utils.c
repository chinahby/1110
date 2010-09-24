
/**********************************************************************
 * flash_nor_utils.c
 *
 * Flash NOR utils
 *
 * This file implements utility functions for NOR 
 *
 * Copyright (c) 2007, 2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_nor_utils.c#4 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-03-11   sv      Added new function flash_nor_find_size_for_block_addr() 
2007-11-08   op      Fix flash_nor_find_size_for_block_num(), 
                     flash_nor_find_block_num_for_start_addr() to
                     support the Intel device last block
2007-09-21   op      Fix the device size
2007-02-27   mm      Initial implementation of Utils for NOR
===========================================================================*/

#include "flash_nor_utils.h"
/****************************************************************
 * Global Data
 ***************************************************************/

/****************************************************************
 * Forward function declarations
 ***************************************************************/


/****************************************************************
 * Functions
 ***************************************************************/

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
uint32
flash_nor_find_block_start_addr(flashi_nor_device *nor_dev_info, 
  int which_block)
{
  uint32 start_addr = 0;
  uint32 blk_size;
  flash_geometry_info   *geometry = nor_dev_info->geometry;
  int block_count;
  int cum_blk_cnt = 0;
  int num_blks_in_rgn;
  int i;

  /* Make sure that the device has been probed and the geometry structure has
   * been filled up.
   */
  if (geometry == NULL)
  {
    return (uint32)-1;
  }

  block_count = geometry->num_total_blocks;

  /* First, test the simple case of a block out or range completely */
  if ((which_block > block_count) || (which_block < 0))
  {
    return (uint32)-1;
  }


  /* To find start address of a block, find the end address of previous block */

  for (i = 0; i < geometry->num_erase_blk_region; i++)
  {
    num_blks_in_rgn =  geometry->blk_regions[i].numblocks;
    blk_size = (uint32)geometry->blk_regions[i].blocksize_bytes;

    if ((which_block + 1) > (cum_blk_cnt + num_blks_in_rgn))
    {
      cum_blk_cnt += num_blks_in_rgn;
      start_addr += num_blks_in_rgn * blk_size; 
      continue;
    }
    else
    {
      start_addr += (which_block - cum_blk_cnt) * blk_size;
      break;
    }
  }

  return start_addr;
}




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

uint32 
flash_nor_find_block_num_for_start_addr(fs_device_t self, 
  dword start_addr)
{
  fsi_device_t priv = (fsi_device_t) self;
  uint32 blk_size;
  flash_geometry_info   *geometry = priv->nor_dev_info.geometry;
  uint32 cum_blk_cnt = 0;
  uint32 cur_start_addr = 0; 
  uint32 num_blks_in_rgn;
  uint32 i, j;
  uint32 match_found = FALSE;
  uint32 blk_num;
  uint32 device_bsize;
  uint32 num_rgns;
  uint32 end_blk_bsize;
  uint32 last_blk_start;
  uint32 rgn_end_addr;

  /* Make sure that the device has been probed and the geometry structure has
   * been filled up.
   */
  blk_num = 0xFFFFFFFF;
  if (geometry != NULL)
  {

    num_rgns = geometry->num_erase_blk_region;
    device_bsize = 1 << (uint32)geometry->device_size;
    end_blk_bsize = geometry->blk_regions[num_rgns - 1].blocksize_bytes;
    last_blk_start = device_bsize - end_blk_bsize;

    /* test the start address is at end of device */
    if (device_bsize == start_addr)
    {
      /* return the total blocks of device */ 
      blk_num = geometry->num_total_blocks;
    }
    /* Test the case of start address out of range */
    else if (start_addr > last_blk_start)
    {
      blk_num = 0xFFFFFFFF;
    }
    else
    {
      /* Determine which sector this start address is supposed to be in. */

      /* To find start addr of a block, find the end addr of previous block */
      for (i = 0; i < geometry->num_erase_blk_region; i++)
      {
        if (match_found == TRUE)
        {
          break;
        }

        num_blks_in_rgn = geometry->blk_regions[i].numblocks;
        blk_size = (uint32)geometry->blk_regions[i].blocksize_bytes;

        rgn_end_addr = (num_blks_in_rgn) * blk_size + cur_start_addr;

        if ((rgn_end_addr - blk_size) < start_addr)
        {
          cum_blk_cnt += num_blks_in_rgn;
          cur_start_addr += (num_blks_in_rgn * blk_size);
          continue;
        }
        else
        {
          for (j = 0; j < num_blks_in_rgn; j++)
          {
            if (cur_start_addr == start_addr)
            {
              match_found = TRUE;
              break;
            }
            else
            {
              cur_start_addr += blk_size;
              cum_blk_cnt++;
            }
          } /* for j < num_blks_in_rgn*/
        } /* else (rgn_end_addr - blk_size) < start_addr*/
      } /* end for i < geometry->num_erase_blk_region */ 

      if (match_found)
      {
        blk_num = cum_blk_cnt;
      }
      else
      {
        blk_num = 0xFFFFFFFF;
      }
    } /* start_addr <= last_blk_start */
  } /* geometry != NULL */
  return blk_num;
}


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

uint32
flash_nor_find_size_for_block_num(fs_device_t self, uint32 blk_num)
{
  fsi_device_t priv = (fsi_device_t) self;
  uint32 blk_size = 0;

  flash_geometry_info   *geometry = priv->nor_dev_info.geometry;
  uint32 cum_blk_cnt = 0;
  uint32 num_blks_in_rgn;
  uint32 i;
  uint32 total_blks;
  
  /* Make sure that the device has been probed and the geometry structure has
   * been filled up.
   */
  if (geometry != NULL)
  {
    total_blks = geometry->num_total_blocks;
    /* Check size of last block of device , block number must be one less
     * than a device total block count
     */
    if (blk_num == total_blks)
    {
      blk_num --;
    }
    /* block in range */
    if (blk_num < total_blks)
    {
      /* Determine which sector this start address is supposed to be in.
       */

      /* To find start addr of a block, find the end addr of previous block */

      for (i = 0; i < geometry->num_erase_blk_region; i++)
      {
        num_blks_in_rgn =  geometry->blk_regions[i].numblocks;
        cum_blk_cnt += num_blks_in_rgn;
        blk_size = (uint32)geometry->blk_regions[i].blocksize_bytes;

        if (blk_num < cum_blk_cnt)
        {
          break;
        }
        else
        {
          continue;
        }
      } /* end for */
    } /* blk_num < total_blks */
  } /* geometry != NULL */
  return blk_size;
}


/*===========================================================================

FUNCTION flash_nor_find_size_for_block_addr

DESCRIPTION
  This function will find the block size for a block start address provided

DEPENDENCIES

RETURN VALUE
  If block found, return block size
  If illegal block number, return 0

SIDE EFFECTS

===========================================================================*/

uint32  
flash_nor_find_size_for_block_addr (flashi_nor_device *nor_device, 
                                    uint32 blk_start_addr)
{
  uint32 blk_size = 0;
  uint32 i = 0;
  uint32 cum_region_size = 0; /* Cumulative region size */

  if (nor_device->geometry != NULL)
  {
    for(i = 0; i < nor_device->geometry->num_erase_blk_region; i++)
    {
      cum_region_size += (nor_device->geometry->blk_regions[i].numblocks *
        nor_device->geometry->blk_regions[i].blocksize_bytes);
      if (blk_start_addr < cum_region_size)
      {
        /* Store the Size of the Block */
        blk_size = nor_device->geometry->blk_regions[i].blocksize_bytes;
        break;
      }
    }
  }
  return blk_size;
}
