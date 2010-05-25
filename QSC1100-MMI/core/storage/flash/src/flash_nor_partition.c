
/**********************************************************************
 * flash_nor_partition.c
 *
 * Flash partition support
 *
 * This file implements functions and data used to access partitions
 * for NOR flash parts
 *
 * Copyright (c) 2007, 2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH_PARTI_MIB.02.00/flash_nor_partition.c#6 $ $DateTime: 2008/06/09 16:50:32 $ $Author: c_surven $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-05-27   sv      Fix RVCT compiler warnings for tools compilation
2008-03-18   sv      remove banned API, fix klocworks issues, align comments 
2008-03-18   sv      Moved get_phy_limits fn to flash_nor.c, modified to 
                     support usage of flash_open in tools in partition mode.
2007-09-17   op      Fix Lint compiler warning "total_byte_size is used before 
                     initialized", code alignment
2007-09-17   op      Add support for 0:ALL to represent the whole device
                     Fix the system partition Table EFS size
                     re-write the functions to have single return
2007-02-11   rt/mm   Initial implementation of NOR Partition
===========================================================================*/

#include "comdef.h"
#include "flash_partition.h"
#include "flash_nor_device.h"
#include "flash_nor_partition.h"
#include "mibib.h"
#include <string.h>

#ifdef BUILD_JFLASH
  #include "jflash_msg.h"
#else
  #define DPRINTF(x)
  #define TPRINTF(level, x)
#endif

/****************************************************************
 * Global Data
 ***************************************************************/

/* Buffers to read in pages of Boot Information Block and
 * partition table.  
 *
 * NOTE:  because the structures that are read into these buffers
 *        are 4 byte multiples, align these buffers on 4 byte
 *        boundaries.  If this is not done, the magic numbers
 *        in the partition table may fail to compare.
 */

/****************************************************************
 * Forward function declarations
 ***************************************************************/


/****************************************************************
 * Functions
 ***************************************************************/


/***********************************************************************
FUNCTION      flash_nor_update_limits

DESCRIPTION   This function uses a pointer to a partition table and
              partition name, and if it finds the partition by name
              it updates the block limits in the device structure
              with the values from the partition table entry.

RETURN VALUE  TRUE             if partition entry found
              FALSE            if not found
**********************************************************************/
int 
flash_nor_update_limits(fs_device_t self, flash_partable_t parti_ptr, 
                        char *parti_name)
{
  fsi_device_t priv = (fsi_device_t) self;
  flash_partentry_t entry_ptr;
  dword no_of_shifts = 1;
  dword page_size;
  dword pages_in_block = 0;
  dword start_blk, end_blk;
  dword start_blk_size;
  dword end_blk_size;
  int status;

  /* We are going to mess with the internal data of the device structure,
   * so store away the previous values and modify them so that we can
   * see the entire device.  If we fail, we will restore the original
   * values.  If we find the partition, we will modify the original
   * values to reflect the new partiton limits.
   */    
  entry_ptr = flash_partition_find_entry(parti_ptr, parti_name);

  if (entry_ptr != (flash_partentry_t)0)
  {
    /* We have found the partition, update the device limits in the 
     * caller's data structure.  
     * Ensure that we did not go beyond the physical limits of the
     * the device.  This is because we allow the last partition
     * defined to grow to the device size by giving it a
     * special value.  This code ensures that we remain
     * within the actual device limits.
     */
    priv->partition_start_baddr = entry_ptr->offset;
    priv->partition_limit_baddr = entry_ptr->offset + entry_ptr->length;
    start_blk = 
    flash_nor_find_block_num_for_start_addr(self, entry_ptr->offset);
    end_blk = flash_nor_find_block_num_for_start_addr(self, 
      priv->partition_limit_baddr);
    if ((start_blk == (uint32) -1) ||(end_blk == (uint32) -1))
    {
      status = FALSE;
    }
    else
    {
      priv->partition_blk_start = start_blk;
      priv->partition_blk_limit = end_blk;
      start_blk_size = flash_nor_find_size_for_block_num(self, start_blk);
      end_blk_size = flash_nor_find_size_for_block_num(self, end_blk);
  
      priv->partition_blk_bsize = start_blk_size;
  
      if (start_blk_size != end_blk_size)
      {
        /* Some code partitions use not uniform block sizes. */
        priv->partition_blk_cnt = end_blk - start_blk;
      }
      else
      {
        priv->partition_blk_cnt = entry_ptr->length/start_blk_size;
      }
  
      if ((end_blk - start_blk) != priv->partition_blk_cnt)
      {
        status = FALSE;
      } 
      else
      {
        priv->which_flash = entry_ptr->which_flash;
    
        page_size = priv->page_bsize;
        pages_in_block = (word)(start_blk_size/page_size);
    
        priv->partition_last_pg_id = ( pages_in_block *
                                       (word)priv->partition_blk_cnt ) - 1;
        while (page_size != 2)
        {
          page_size >>= 1;
          no_of_shifts++;
        }
        priv->page_size_shift = no_of_shifts;
        no_of_shifts = 1;
        while (pages_in_block != 2)
        {
          pages_in_block >>= 1;
          no_of_shifts++;
        }
        priv->block_size_shift = no_of_shifts;
    
        /* Found partition and set limits accordingly */
        status = TRUE;
      }//end else (end_blk - start_blk) != priv->partition_blk_cnt
    }// end else (start_blk == (uint32) -1) ||(end_blk == (uint32) -1)
  }// end if entry!= NULL
  else
  {
    TPRINTF(3, ("fnul:  Did not get a partition entry match for %s\n",
                parti_name));
    status = FALSE;
  }// end else entry == NULL 
  return status;
}



/***********************************************************************
FUNCTION      flash_nor_open_partition

DESCRIPTION   Opens a partition found by a name string passed in.
              Modifies the private data of the device structure to
              limit the "view" of the device by the device driver
              for all subsequent calls to device functions.
              
              FS_DEVICE_OK if successful
              FS_DEVICE_FAIL if any fatal device errors
**********************************************************************/
int
flash_nor_open_partition (fs_device_t self, char *partition_name)
{
  fsi_device_t priv = (fsi_device_t) self;
  flash_partable_t parti_ptr;
  uint32 orig_offset, orig_size;
  uint32 result = FALSE;
  int status;

  TPRINTF(3,("fnop:  Attempting to open %s\n", partition_name));

  if (priv == NULL)
  {
    TPRINTF(3,("fnop: open partition failed for %s\n", partition_name));
    return FS_DEVICE_FAIL;
  }   
  /* We are going to mess with the internal data of the device structure,
   * so store away the previous values and modify them so that we can
   * see the entire device.  If we fail, we will restore the original
   * values.  If we find the partition, we will modify the original
   * values to reflect the new partiton limits.
   */
  orig_offset = priv->partition_start_baddr;
  orig_size = priv->partition_blk_limit - priv->partition_blk_start;
   
  /* Check to make sure that this is not a special system partition which is
   * used to represent the whole part. Since this partition is used by tools
   * there may not be a partition table present on the device. Just rely on
   * the information returned by probe function to find the limits for this
   * partition. Please note that all partition specific params which are 
   * populated for regular user partition may not be populated for this
   * partition
   */
  if (strncmp(partition_name, MIBIB_DEVICE_ALL_PARTI_NAME,
              FLASH_PART_NAME_LENGTH) == 0)
  {
      priv->partition_start_baddr = 0;
      priv->partition_limit_baddr = 
       (1 << ((dword)priv->nor_dev_info.geometry->device_size));
      priv->partition_blk_start = 0;
      priv->partition_blk_limit = \
        priv->nor_dev_info.geometry->num_total_blocks;
      result = TRUE;
  }
  else
  {
    /* The parition table in a known block on the flash. Unlike NAND, NOR    
     * does not have bad blocks. This known block number thus will not change 
     * from flash to flash. The address of the start of this block may change
     * from flash to flash. Calculate the beginning address of this block and
     * read the partition table
     */
    parti_ptr = (flash_partable_t)
                 flash_nor_find_block_start_addr(&(priv->nor_dev_info), 
                                                 MI_BLOCK_PARTI_TBL); 

    if ((parti_ptr->magic1 == FLASH_PART_MAGIC1) &&
       (parti_ptr->magic2 == FLASH_PART_MAGIC2) &&
       (parti_ptr->version == FLASH_PARTITION_VERSION) )
    {
      /* We have a valid partition table */
      TPRINTF(5,("fnop:  found partition table\n"));
      TPRINTF(5,("\tMagic1 is 0x%x\n", parti_ptr->magic1));
      TPRINTF(5,("\tMagic2 is 0x%x\n", parti_ptr->magic2));
      TPRINTF(5,("\tTable version is 0x%x\n", parti_ptr->version));
      TPRINTF(5,("\tNumber of partitions is 0x%x\n", parti_ptr->numparts));

      if ((flash_nor_update_limits(self, parti_ptr, partition_name)) == TRUE)
      {
        /* Found the partition and updated the limits in the private data. */
        result = TRUE;
      }
    }
  }
    
  if (result == TRUE)
  {
    TPRINTF(5, ("fnop:  Partition opened sucessfully\n"));
    status = FS_DEVICE_OK;
  }
  else
  {
    DPRINTF(("fnop:  Partition could not be opened sucessfully\n"));
    priv->partition_start_baddr = orig_offset;
    priv->partition_blk_limit = orig_size + orig_offset;
    status = FS_DEVICE_FAIL;
  }
  return status;
}

#if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)
  /*
   * This structure is used locally for conversion the Nor flash usr partition 
   * table to system partition table
   */

  struct nor_convert_partition_table
  {
    fs_device_t self;
    flash_usr_partable_t parti1;
    flash_partable_t parti2;
    block_descriptor_type *block_map;
    const flash_geometry_info *device;
    uint32 current_bank;
    uint32 current_sector;
    uint32 total_blocks; 
  };

  typedef struct nor_convert_partition_table *nor_conv_partable_t;
  
/***********************************************************************
FUNCTION      flash_nor_convert_tbl_init

DESCRIPTION   This function validates the parameters, and intializes the 
              global tables which are used to convert user partition
              table to system partition table.

              
              
RETURN VALUE  TRUE  if Sanity check that this is a valid parition table
              FALSE  if sannity check fail
************************************************************************/
static int flash_nor_convert_table_init ( fs_device_t self, 
      flash_usr_partable_t parti1, flash_partable_t parti2,
      block_descriptor_type *block_map, nor_conv_partable_t pt)
{
  int statusOk = TRUE;
  flashi_nor_device * nor_device;
  
  /* init the local global pointer to these tables */
  pt->self = self ;
  pt->parti1 = parti1;
  pt->parti2 = parti2;
  pt->block_map = block_map;
  pt->current_bank = 0;
  pt->current_sector = 0;
  pt->total_blocks = 0;
   
  /* Sanity check that this is a valid parition table. This is not absolutely 
   * necessary since we have already checked things before. Because we are
   * writing the system partition table based on the user one, it is good to
   * do this sanity check.
   */
  if (parti1 && self && parti2 && block_map)
  {
    nor_device = &(((fsi_device_t)self)->nor_dev_info);
    pt->total_blocks = nor_device->geometry->num_total_blocks;
    if ((parti1->magic1 == FLASH_USR_PART_MAGIC1) &&
         (parti1->magic2 == FLASH_USR_PART_MAGIC2) &&
         (parti1->version == FLASH_PARTITION_VERSION) &&
         (parti1->numparts <= FLASH_NUM_PART_ENTRIES) &&
         (nor_device->geometry != NULL))
    {
      /* Device is not probed yet. See if any static device info is avaiable */
      {

        pt->device = nor_device->geometry;
        memset ((uint8 *) parti2, 0xFF, NOR_FLASH_PARTI_TBL_LENGTH);

        /* Initialize the global page buffer with 1s
         * Check to make sure that for 16 layouts there will not be any
         * overflow 
         */

        parti2->magic1 = FLASH_PART_MAGIC1;
        parti2->magic2 = FLASH_PART_MAGIC2;
        parti2->version = FLASH_PARTITION_VERSION;
        parti2->numparts = parti1->numparts;
      }
    }
    else
    {
      if (nor_device->geometry == NULL)
      {
        DPRINTF (("fncuptts: Device info not available\n"));
        statusOk = FALSE;
      }
    }
  }
  return statusOk;
}


/***********************************************************************
FUNCTION      flash_nor_setup_system_parti_name

DESCRIPTION   This function set the system partition name and partition
              index based on usr partition table.

              
RETURN VALUE  TRUE  if Sanity check that this is a valid parition table
              FALSE  if sannity check fail
***********************************************************************/
static int flash_nor_setup_system_parti_name(uint32 erase_blk_rgns,   
                                             uint32 *num_top_boot_blks, int i,  
                                             nor_conv_partable_t pt)
{
  int statusOk = TRUE;
  uint32 partition_size = 0;
  static int mipib_parti_indx = -1;
  static int boot_parti_indx = -1;
  static int sim_secure_indx = -1;

  partition_size = pt->parti1->part_entry[i].img_size;

  if (partition_size == 0)
  {
    DPRINTF(("fncuptts: Partition %s is defined with zero size\n",            
             pt->parti1->part_entry[i].name));
    DPRINTF(("          Not a valid definition!\n"));
    statusOk = FALSE;
  }
  else
  {
    if (strncmp(pt->parti1->part_entry[i].name,
                MIBIB_DEVICE_ALL_PARTI_NAME,
                FLASH_PART_NAME_LENGTH) != 0)
    {
      memcpy (pt->parti2->part_entry[i].name, pt->parti1->part_entry[i].name, 
              FLASH_PART_NAME_LENGTH);

      pt->parti2->part_entry[i].which_flash =
      (uint32) pt->parti1->part_entry[i].which_flash;

      TPRINTF (4, ("\n\n"));
      TPRINTF (4, ("Block size is %d KB \n", 
        (&pt->block_map[pt->current_sector])->len>>10));

      if ((boot_parti_indx == -1) ||(mipib_parti_indx == -1) ||
         (sim_secure_indx == -1))
      {
        switch (i)
        {
          /* BOOT partition should always be the first partition */
          case 0: 
            if ( !strncmp(pt->parti2->part_entry[i].name, 
                          MIBIB_NOR_BOOT_PARTI_NAME,
                          FLASH_PART_NAME_LENGTH) )
            {
              TPRINTF(3,
                ("BOOT partition found at index %d in partition table\n", i) );
              boot_parti_indx = i;
            }
            else
            {
              DPRINTF(
                ("fncuptts: BOOT partition is not the first partition!\n") );
              statusOk = FALSE;
            }
            break;

          /* MIPIB partition should always be the second partition */
          case 1:
            if ( !strncmp(pt->parti2->part_entry[i].name, 
                          MIBIB_NOR_PARTI_INFO_PARTI_NAME,
                          FLASH_PART_NAME_LENGTH) )
            {
              TPRINTF(3,                                                              
                ("MIPIB partition found at index %d in partition table\n", i)); 
              mipib_parti_indx = i;
            }
            else
            {
              DPRINTF(
               ("fncuptts: MIPIB partition is not the second partition!\n") );
              statusOk = FALSE;
            }
            break;
            
          default:
          /* If SIM_SECURE partition has not been seen yet, check to see if 
           * this is SIM_SECURE.
           */
            if ( !strncmp(pt->parti2->part_entry[i].name, 
                          MIBIB_SIMSECURE_PARTI_NAME,
                          FLASH_PART_NAME_LENGTH) )
            {
              TPRINTF(3,                                                              
                ("SIM_SECURE parti found at index %d in partition table\n", i)); 

              sim_secure_indx = i;
              /* check to see if we have smaller top boot blocks */
              if ( erase_blk_rgns > 2)
              {
                *num_top_boot_blks = 
                  pt->device->blk_regions[erase_blk_rgns -1].numblocks;
              }
            }
            break;
        } /* end switch i */ 
      } /* end if (boot_parti_indx==-1 ... */
    } /* end if strncmp(pt->parti2->part_entry[i].name ... */
    else
    {
      DPRINTF(("fncuptts: Rsrvd partition name found in usr partiton tbl!\n"));
      statusOk = FALSE;
    }
  }
  
  return statusOk;
}

/*********************************************************************
FUNCTION      flash_nor_parti_size_in_KB

DESCRIPTION   This function calculates the size of a partition in KB.
              The partition size is stored in parti_size_in_kB. It also 
              updates the system partition offset and length.

              
RETURN VALUE  TRUE  if device has enough banks
              FALSE if device does not have enough banks 
**********************************************************************/

static int flash_nor_parti_size_in_KB( int i, uint32 *parti_size_in_kB, 
                                       uint32 partition_size, 
                                       nor_conv_partable_t pt)
{
  int statusOk = TRUE;
  flashi_nor_device * nor_device;
  nor_device = &(((fsi_device_t)pt->self)->nor_dev_info);

  *parti_size_in_kB = 0;
  pt->parti2->part_entry[i].offset = 
    (&pt->block_map[pt->current_sector])->start;

  while (*parti_size_in_kB < partition_size)
  {
    *parti_size_in_kB += (&pt->block_map[pt->current_sector])->len>>10;
    pt->current_sector++;

    if (pt->current_sector >
        nor_device->geometry->bank_info[pt->current_bank].end_block)
    {
      pt->current_bank++;
      if (pt->current_bank >= nor_device->geometry->num_banks)
      {
        DPRINTF(("fncuptts: Device does not have enough banks!\n"));
        statusOk = FALSE;
        break;
      }
    }
  }

  if (statusOk)
  {
    pt->parti2->part_entry[i].length = 
      (&pt->block_map[pt->current_sector])->start -
      pt->parti2->part_entry[i].offset;

    TPRINTF(3,("fncuptts: Partition start at addr %d\n" ,
               pt->parti2->part_entry[i].offset));
    TPRINTF(3,("fncuptts: Partition length in bytes %d\n" ,
               pt->parti2->part_entry[i].length));
  }

  return statusOk;
}


/*********************************************************************
FUNCTION      flash_nor_count_sectors_in_parti

DESCRIPTION   This function counts number of sectors in a partition.
              It updates the partition size is stored in parti_size_in_kB, 
              current sectors in partition and system partition length
              
              
RETURN VALUE  FALSE If number of sectors in a partition is over the 
                    total sectors in device.
              TRUE otherswise
**********************************************************************/
static int flash_nor_count_sectors_in_parti(  int i, uint32 *parti_size_in_kB,
             uint32 partition_min_size, nor_conv_partable_t pt)
{
  int statusOk = TRUE;

  pt->parti2->part_entry[i].length = 
    (&pt->block_map[pt->current_sector])->start -
    pt->parti2->part_entry[i].offset;

                                 
  *parti_size_in_kB = pt->parti2->part_entry[i].length>>10;

  while(*parti_size_in_kB < partition_min_size)
  {
    pt->current_sector++;

    if (pt->current_sector >= pt->total_blocks)
    {
      DPRINTF(("fncuptts: Device does not have enough blocks!\n"));
      statusOk = FALSE;
      break;
    }

    if (statusOk)
    {
      pt->parti2->part_entry[i].length = 
        (&pt->block_map[pt->current_sector])->start -
          pt->parti2->part_entry[i].offset;

      *parti_size_in_kB = pt->parti2->part_entry[i].length>>10;
    }
  }

  if (statusOk)
  {
    TPRINTF(3,("fncuptts: Partition start at byte addr 0x%x\n" ,
               pt->parti2->part_entry[i].offset));
    TPRINTF(3,("fncuptts: Partition length in bytes: 0x%x\n" ,
               pt->parti2->part_entry[i].length));
  }

  return statusOk;
}


/*********************************************************************
FUNCTION      flash_nor_check_system_parti_bank

DESCRIPTION   This function checks to make sure that this partition does not 
              need to  start from a fresh bank. If it does, make necessary 
              adjustments to current sector and current bank. set 
              start_in_new_bank flag to TRUE if a partition needs to start 
              from a fresh bank.

             
RETURN VALUE  TRUE  if device has enough banks or blocks
              FALSE if device does not have enough banks or blocks
**********************************************************************/
static int flash_nor_check_system_parti_bank(  
               uint32 *start_in_new_bank, int i, nor_conv_partable_t pt)
{
  int statusOk = TRUE;
  flashi_nor_device * nor_device;
  nor_device = &(((fsi_device_t)pt->self)->nor_dev_info);

  if ( !( pt->parti1->part_entry[i].reserved_flag4 & ~(FLASH_PART_ATTR_BASE &
                                           FLASH_PART_NEW_BANK)) ) 
  {
    if (pt->current_sector !=
        nor_device->geometry->bank_info[pt->current_bank].start_block)
    {
      pt->current_sector =
      nor_device->geometry->bank_info[pt->current_bank].end_block + 1;

      if (pt->current_sector >=pt->total_blocks)
      {
        DPRINTF(("fncuptts: Device does not have enough blocks!\n"));
        statusOk = FALSE;
      }
      else
      {
        pt->current_bank += 1;
        if (pt->current_bank >= nor_device->geometry->num_banks)
        {
          DPRINTF(("fncuptts: Device does not have enough banks!\n"));
          statusOk = FALSE;
        }
        else
        {
          /* Extend the previous partition to occupy the whole of previous bank,
           * which otherwise will go waste.
           */

          /* For the first partition expression (i-1) can lead to negative indx.
           * Please note that execution path will not get here because the first
           * partition will start at bank boundary automatically.
           */
          pt->parti2->part_entry[i - 1].length =                                       
          (&pt->block_map[pt->current_sector])->start -
          pt->parti2->part_entry[i - 1].offset;
        }
      }
    }

    if (statusOk)
    {
      *start_in_new_bank = TRUE;
    }
  }

  return statusOk;
}


/*********************************************************************
FUNCTION      flash_nor_display_partition_info

DESCRIPTION   This function prints a parition entry in user partition
              table.
                                        
RETURN VALUE  TRUE  if partition information is valid
              FALSE if partition information is invalid
**********************************************************************/
static int flash_nor_display_partition_info(uint32 partition_min_size,
                                            int i , uint32 partition_size, 
                                           nor_conv_partable_t pt)
{
  int statusOk = TRUE;
  
  if (partition_min_size != 0)
  {
    DPRINTF(("fncuptts: Non-zero padding defined for partition %s\n", 
                                             pt->parti1->part_entry[i].name));
    DPRINTF(("          Padding not required for NOR partitions\n"));
    DPRINTF(("          Not a valid definition!\n"));
    statusOk = FALSE;
  }
  else
  {
    TPRINTF(3,("fncuptts: Finding bounds for partition %s\n",
                                             &pt->parti2->part_entry[i].name));
    TPRINTF(3, ("fncuptts: Image Size is %d KB\n", partition_size));
    TPRINTF(3, ("fncuptts: Image Pad is %d KB\n", partition_min_size));
  }

  return statusOk;
 }

/*********************************************************************
FUNCTION      flash_nor_setup_system_parti_growOffset

DESCRIPTION   This function setup grow partitions of the system
              partition tables

RETURN VALUE  TRUE  if it can setup the MIBIB and SECURE partitions
              FALSE if it can not setup the MIBIB and SECURE partitions
**********************************************************************/
static int flash_nor_setup_system_parti_growOffset(  
            uint32 *grow_parti_indx, uint32 i, nor_conv_partable_t pt)
{
  int statusOk = TRUE;

  if (*grow_parti_indx != (uint32)-1)
  {
    DPRINTF(("fncuptts: Multiple partitions defined with GROW \n"));
    statusOk = FALSE;
  }
  else
  {
    *grow_parti_indx = i;

    /* Mark the beginning of the partition. The end of this special parition
     * will be marked at the very end, after all other paritions have been
     * processed.
     */
    pt->parti2->part_entry[i].offset =
      (&pt->block_map[pt->current_sector])->start;

    /* This is an ugly hack. We set up the current_* variables to be somewhere
     * towards the before the end of the second last sector.
     */

    TPRINTF(3,("fncuptts: Partition start at addr %d\n" ,
               pt->parti2->part_entry[i].offset));
  }
  return statusOk;
}



/*********************************************************************
FUNCTION      flash_nor_setup_system_parti_growLength

DESCRIPTION   This function setup the length of the system partition grow 

RETURN VALUE  none
**********************************************************************/
static void flash_nor_setup_system_parti_growLength(uint32 erase_blk_rgns,  
              uint32 *grow_parti_indx, uint32 i, nor_conv_partable_t pt)
{
  uint32 last_blk_size = 0;
  uint32 num_top_boot_blks = 0;
  flashi_nor_device * nor_device;
  nor_device = &(((fsi_device_t)pt->self)->nor_dev_info);

  /* Set variable 'current_bank' to the last available bank */
  pt->current_bank = nor_device->geometry->num_banks - 1;
  pt->current_sector = pt->device->bank_info[pt->current_bank].end_block;
  /* Grow attribute is usually defined for file system partition, which 
   * does not use TOP boot blocks if any exist. Check to see if there are
   * any and accordingly adjust the length for this partition.
   */
  if ( erase_blk_rgns > 2)
  {
    num_top_boot_blks = pt->device->blk_regions[erase_blk_rgns -1].numblocks;
  }

  pt->current_sector -= num_top_boot_blks;
  last_blk_size = (num_top_boot_blks) ? 0:
                  (&pt->block_map[pt->current_sector])->len;

  pt->parti2->part_entry[i].length = 
    (&pt->block_map[pt->current_sector])->start +  last_blk_size - 
    pt->parti2->part_entry[i].offset;
  TPRINTF(3,("fncuptts: Partition length in bytes %d\n" ,
    pt->parti2->part_entry[i].length));
  return ;

}                      

  /* Do a feasibility analysis to determine how many banks and sectors will be
   * needed to allocate the rest of the partitions.
   */
  /* If there are some tail partitions that can be allocated in top boot blocks
   * if any, will be allocated in this section.
   */

/*********************************************************************
FUNCTION      flash_nor_setup_system_parti_info

DESCRIPTION   This function determine how many banks and sectors will be
              needed to allocate the rest of the partitions.
              If there are some tail partitions that can be allocated in top 
              boot blocks. If any, will be allocated in this section.

                        
RETURN VALUE TRUE  if it can setup the MIBIB and SECURE partitions
             FALSE if it can not setup the MIBIB and SECURE partitions

**********************************************************************/
static int flash_nor_setup_system_parti_info(uint32 *tail_parti_allocated, 
             uint32 *grow_parti_indx, nor_conv_partable_t pt)
{
  uint32 i, j;
  uint32 partition_size = 0;
  uint32 partition_min_size;
  uint32 sector_size;
  static int kilobyte_size_flag = -1;
  uint32 total_byte_size = 0;
  int statusOk = TRUE;
  flashi_nor_device * nor_device;

  nor_device = &(((fsi_device_t)pt->self)->nor_dev_info);
  *tail_parti_allocated = 0;
  pt->current_bank = nor_device->geometry->num_banks - 1;
  pt->current_sector = pt->device->bank_info[pt->current_bank].end_block;

  for (i = (pt->parti1->numparts - 1); i > *grow_parti_indx; i--)
  {

    /* Variable 'current_sector' is pointing to the last sector in last bank 
     * on the flash.
     * If there are tail partitions defined as number of blocks, allocate them
     * on the flash.
     */
    if ( !(pt->parti1->part_entry[i].reserved_flag4 &
           ~(FLASH_PART_ATTR_BASE & FLASH_PART_SIZE_BLK_CNT) ) &
           (kilobyte_size_flag = -1))
    { 
      /* Partition size defined in user partition table is actually number of
       * blocks that are required. Make sure that a minimum partition size is
       * defined.
       */
      partition_size = pt->parti1->part_entry[i].img_size;
      partition_min_size = pt->parti1->part_entry[i].padding;
      total_byte_size = 0;

      for (j = 0; j < partition_size; j++)
      {
        total_byte_size += (&pt->block_map[pt->current_sector - j])->len; 
      }

      if (partition_min_size != 0)
      {
        if (total_byte_size > partition_min_size)
        {
          memcpy (pt->parti2->part_entry[i].name, 
                  pt->parti1->part_entry[i].name, 
                  FLASH_PART_NAME_LENGTH);

          pt->parti2->part_entry[i].which_flash =
            (uint32) pt->parti1->part_entry[i].which_flash;

          pt->parti2->part_entry[i].offset =
            (&pt->block_map[pt->current_sector - partition_size + 1])->start;

          pt->parti2->part_entry[i].length = total_byte_size;

          pt->current_sector -= partition_size;

          if (pt->current_sector < 
            pt->device->bank_info[pt->current_bank].start_block)
          {
              DPRINTF(("fncuptts: Tail partitions defined too big!\n"));
              statusOk = FALSE;
          }
          else
          {
            pt->total_blocks -= partition_size;
            (*tail_parti_allocated)++;

            TPRINTF(3,("fncuptts: Partition start at byte addr 0x%x\n" ,
                         pt->parti2->part_entry[i].offset));
            TPRINTF(3,("fncuptts: Partition length in bytes: 0x%x\n" ,
                         pt->parti2->part_entry[i].length));
          }
        }
      }
      else
      {
        DPRINTF(("fncuptts: Min partition size must be defined\n")); 
        statusOk = FALSE;
      }
    }
    else
    {
      if (kilobyte_size_flag == -1)
      {
        kilobyte_size_flag = i;
        total_byte_size = 0;
        pt->current_sector = 
          pt->device->bank_info[pt->current_bank].start_block;
        sector_size = (&pt->block_map[pt->current_sector])->len>>10; 
      }

      if ( !(pt->parti1->part_entry[i].reserved_flag4 & 
             ~(FLASH_PART_ATTR_BASE & FLASH_PART_SIZE_BLK_CNT)))
      {
        total_byte_size += sector_size * partition_size;
      }
      else
      {
        total_byte_size += partition_size;
      }
    }
  }

  if (statusOk && (total_byte_size <= ((&pt->block_map[1])->start >> 10)))
  {
    /* Set variable 'current_bank' to the last available bank */
    pt->current_bank = nor_device->geometry->num_banks - 2;
    pt->current_sector = pt->device->bank_info[pt->current_bank].end_block;
  }
  else
  {
    DPRINTF(("fncuptts: Tail partitions defined too big!\n"));
    statusOk = FALSE;
  }
  return statusOk;

}


/*********************************************************************
FUNCTION      flash_nor_setup_system_parti_grow_parti

DESCRIPTION   This function (set up) allocate all the partitions before 
              the one defined with GROW attribute 

                       
RETURN VALUE  TRUE  if it can setup the partitions
              FALSE if it can not setup partitions
**********************************************************************/
static int flash_nor_setup_system_parti_grow_parti(  
    uint32 tail_parti_allocated, uint32 *grow_parti_indx, 
    uint32 *start_in_new_bank, uint32 *parti_size_in_kB, nor_conv_partable_t pt)
{
  uint32 i;
  uint32 partition_size = 0;
  uint16 partition_min_size;
  int statusOk = TRUE;
 
  /* All the the partitions before the one defined with GROW attribute have been
   * allocated. Here we will allocate rest of them going backwards. Please note
   * that if partition with GROW attribute was the last partition, this loop 
   * will not be entered.
   */
  for (i = (*grow_parti_indx + 1); 
       i < (pt->parti1->numparts-tail_parti_allocated) && (statusOk); i++)
  {
    *start_in_new_bank = FALSE;
    partition_size = pt->parti1->part_entry[i].img_size;
    partition_min_size = pt->parti1->part_entry[i].padding;

    if (partition_size == 0)
    {
      DPRINTF(("fncuptts: Partition %s is defined with zero size\n",            
        pt->parti1->part_entry[i].name));
      DPRINTF(("          Not a valid definition!\n"));
      statusOk = FALSE;
    }
    else
    {
      memcpy (pt->parti2->part_entry[i].name, pt->parti1->part_entry[i].name, 
        FLASH_PART_NAME_LENGTH);

      pt->parti2->part_entry[i].which_flash =
        (uint32) pt->parti1->part_entry[i].which_flash;


      TPRINTF (4, ("\n\n"));
      TPRINTF (4, ("Block size is %d KB \n", 
        (&pt->block_map[pt->current_sector])->len>>10));

      /* Check to make sure that this partition does not need to start from
       * a fresh bank. If it does, make necessary adjustments to current_*
       * variables.
       */
      statusOk = flash_nor_check_system_parti_bank( 
        (uint32 *)&start_in_new_bank, i, pt);
      
      if (!statusOk)
      {
        break;
      }
      /* Since there is no assigned image for some partitions, partition size
       * will not be known readily. To make it easier for the user, few
       * partitions can be specified in terms of number of blocks.
       */

      /* For now this is only allowed for MIBIB and SIM_SECURE partitions.*/
      if ( !( pt->parti1->part_entry[i].reserved_flag4 & 
              ~(FLASH_PART_ATTR_BASE & FLASH_PART_SIZE_BLK_CNT) ) )
      {
        /* Partition size defined in user partition table is actually number of
         * blocks that are required. Make sure that a minimum partition size is
         * defined.
         */

        if (partition_min_size != 0)
        {
          pt->parti2->part_entry[i].offset = 
            (&pt->block_map[pt->current_sector])->start;

          pt->current_sector += partition_size;

          if (pt->current_sector >= pt->total_blocks)
          {
            DPRINTF(("fncuptts: Device does not have enough blocks!\n"));
            statusOk = FALSE;
            break;
          }

          statusOk = flash_nor_count_sectors_in_parti(i, 
            (uint32 *)&parti_size_in_kB, partition_min_size, pt);
          continue; 
        }
        else
        {
          DPRINTF(("fncuptts: Min partition size must be defined\n")); 
          statusOk = FALSE;
          break;
        }
      }

      if (statusOk)
      {
        statusOk = flash_nor_display_partition_info(  partition_min_size,
          i , partition_size, pt);
        if (statusOk)
        {  
          if (partition_size == FLASH_PARTITION_GROW)
          {
            if (*grow_parti_indx != (uint32)-1)
            {
              DPRINTF(("fncuptts: Multiple partitions defined with GROW \n"));
              statusOk = FALSE;
              break;
            }
          }
          else
          {
            statusOk = 
              flash_nor_parti_size_in_KB(i, (uint32 *)&parti_size_in_kB, 
                                         partition_size, pt);
          }
        }
      }
    }
  }

  return statusOk;
}


/***********************************************************************
FUNCTION     flash_nor_convert_usr_parti_tbl_to_sys

DESCRIPTION  This function accepts user defined partition table as an input
             and generates a system partition table.

RETURN VALUE Returns TRUE if conversion was successful
             Returns FALSE if conversion failed
**********************************************************************/
int 
flash_nor_convert_usr_parti_tbl_to_sys (fs_device_t self,
  flash_usr_partable_t parti1, flash_partable_t parti2,
  block_descriptor_type *block_map)
{
  uint32 i;
  uint32 partition_size = 0;
  uint16 partition_min_size;
  uint32 start_in_new_bank = FALSE;
  int grow_parti_indx = -1;
  const flash_geometry_info *device;
  uint32 parti_size_in_kB;
  uint32 erase_blk_rgns;
  uint32 num_top_boot_blks = 0;
  uint32 tail_parti_allocated = 0;
  int statusOk = TRUE;
  struct nor_convert_partition_table tbl;
  nor_conv_partable_t pt = &tbl;
  flashi_nor_device * nor_device;
  
  /* validate the parameters and init table */
  statusOk = flash_nor_convert_table_init (self, parti1, parti2, block_map, pt);
   
  if ( (statusOk == TRUE) &&
       (pt->parti1->numparts <= FLASH_NUM_PART_ENTRIES) && 
       (pt->parti2->numparts <= FLASH_NUM_PART_ENTRIES) )
  {
    nor_device = &(((fsi_device_t)self)->nor_dev_info);
    device = nor_device->geometry;
    erase_blk_rgns = device->num_erase_blk_region;
    pt->current_sector = 0;

    /* check to see if we have smaller top boot blocks */
    if ( erase_blk_rgns > 2)
    {
      num_top_boot_blks = pt->device->blk_regions[erase_blk_rgns -1].numblocks;
    }

    /* Find BOOT and SIM_SECURE paritions first. */
    /* Scan through the user parition table to see if any of the paritions are
     * defined with fixed address. If there are some allocate them first.
     */
    for (i=0; (i< pt->parti1->numparts) && statusOk; i++)
    {

      start_in_new_bank = FALSE;

      partition_size = pt->parti1->part_entry[i].img_size;
      partition_min_size = pt->parti1->part_entry[i].padding;

      statusOk = flash_nor_setup_system_parti_name(  
                         erase_blk_rgns, &num_top_boot_blks, i, pt);

      
      /* Check to make sure that this partition does not need to start from
       * a fresh bank. If it does, make necessary adjustments to current_*
       * variables.
       */
      if (statusOk == TRUE)
      {
        statusOk = flash_nor_check_system_parti_bank( 
          (uint32 *)&start_in_new_bank, i, pt );
      }
      
      /* Since there is no assigned image for some partitions, partition size
       * will not be known readily. To make it easier for the user, few
       * partitions can be specified in terms of number of blocks.
       */

      /* For now this is only allowed for MIBIB and SIM_SECURE partitions.*/
      if ( !( parti1->part_entry[i].reserved_flag4 & ~(FLASH_PART_ATTR_BASE &
                                               FLASH_PART_SIZE_BLK_CNT)) ) 
      {
        /* Partition size defined in user partition table is actually number of
         * blocks that are required. Make sure that a minimum partition size is
         * defined.
         */

        if (partition_min_size != 0)
        {
          pt->parti2->part_entry[i].offset = 
            (&pt->block_map[pt->current_sector])->start;

          pt->current_sector += partition_size;

          if (pt->current_sector >= pt->total_blocks)
          {
            DPRINTF(("fncuptts: Device does not have enough blocks!\n"));
            statusOk = FALSE;
          }

          if (statusOk == TRUE)
          {
            statusOk = flash_nor_count_sectors_in_parti(i, 
              (uint32 *)&parti_size_in_kB, partition_min_size, pt);
            if (statusOk == TRUE)
            {
              continue;
            }
          }
        }
        else
        {
          DPRINTF(("fncuptts: Min partition size must be defined\n")); 
          statusOk = FALSE;
        }
      }

      if (statusOk )
      {
        statusOk = flash_nor_display_partition_info(  partition_min_size,
          i , partition_size, pt);
      }

      if ((partition_size == FLASH_PARTITION_GROW) && statusOk)
      {
        statusOk = flash_nor_setup_system_parti_growOffset(  
          (uint32 *)&grow_parti_indx, i, pt);

        /* If there are more partitions left to be processed, break out of this
         * loop and process them first, before determining the length for the
         * grow partition. Else set the length for the grow partition.
         */
        if ( grow_parti_indx < (pt->parti1->numparts - 1))
        {
          TPRINTF(3,("fncuptts: Setting partition length is deferred\n"));

          /* If next partition needs to start from a new bank, move the 
           * current_* variable to point to beginning of next bank
           */

          if ( !( pt->parti1->part_entry[grow_parti_indx + 1].reserved_flag4 & 
                  ~(FLASH_PART_ATTR_BASE & FLASH_PART_NEW_BANK)) )
          {
            pt->current_bank += 1;

            if (pt->current_bank >= pt->device->num_banks)
            {
              DPRINTF(("fncuptts: Device does not have enough banks!\n"));
              statusOk = FALSE;
              break;
            }

            pt->current_sector = 
              pt->device->bank_info[pt->current_bank].start_block;

            if (pt->current_sector >= pt->total_blocks)
            {
              DPRINTF(("fncuptts: Device does not have enough blocks!\n"));
              statusOk = FALSE;
              break;
            }
          }
          break;
        }
        else
        {
          /* Since there other no other partitions to be allocated, allow this
           * parititon to hog-on to the rest of storage left on the device.
           */
          if (statusOk == TRUE)
          {
            flash_nor_setup_system_parti_growLength(  
              erase_blk_rgns, (uint32 *)&grow_parti_indx, i, pt);
          }
        }
      }
      else
      {
        if (statusOk == TRUE)
        {
          statusOk = flash_nor_parti_size_in_KB( i, (uint32 *)&parti_size_in_kB,
                                                 partition_size, pt );
        }
      }
    }

    /* Do a feasibility analysis to determine how many banks and sectors will be
     * needed to allocate the rest of the partitions.
     */
    /* If there are some tail partitions that can be allocated in top boot blk
     * if any, will be allocated in this section.
     */
    if (statusOk == TRUE)
    {
      statusOk = flash_nor_setup_system_parti_info(  
        (uint32 *)&tail_parti_allocated, (uint32 *)&grow_parti_indx, pt );
    }
    
    /* All the the partitions before the one defined with GROW attr have been
     * allocated. Here we will allocate rest of them going bkwards. Please note
     * that if partition with GROW attribute was the last partition, this loop 
     * will not be entered.
     */

    if (statusOk == TRUE)
    {
      statusOk = flash_nor_setup_system_parti_grow_parti(  
        tail_parti_allocated, (uint32 *)&grow_parti_indx, 
        (uint32 *)&start_in_new_bank, (uint32 *)&parti_size_in_kB, pt);
    }

    /* If the last one has a grow attribute then the length is 
     * already adjusted. 
     */
    if((grow_parti_indx != (parti1->numparts -1)) && statusOk)
    {
      /* Adjust the last allocated partition to occupy the the remaining space
       * on flash.
       */
      parti2->part_entry[(parti1->numparts - tail_parti_allocated - 1)].length =
        ((&block_map[pt->total_blocks-1])->start +
        (&block_map[pt->total_blocks-1])->len) -  
        parti2->part_entry[(parti1->numparts - 
        tail_parti_allocated - 1)].offset;
    }
  }
  else
  {
    statusOk = FALSE;
  } 
  
  return statusOk;
}

#endif
