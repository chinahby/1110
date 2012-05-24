
/**********************************************************************
 * flash_msm.c
 *
 * MSM specific flash support
 *
 * This file implements MSM specific functions and data used by both
 * NAND and NOR device drivers
 *
 * Copyright (c) 2008-2009 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/flash/MSM_FLASH.01.04/flash_msm.c#14 $ $DateTime: 2009/05/11 12:29:01 $ $Author: c_surven $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-01-02   sv      Added 8MB Samsung NOR(K5N6433ABM) support
2008-12-16   sv      Fix memcpy declaration compiler warning
2008-10-27   sv      Fix NOR flash probe order
2008-08-02   sv      Added W18 NOR flash Support
2008-07-30   sv      Remove shadow mode probe address from probe table
2008-07-01   jz      Added Samsung NOR flash support
2008-05-23   sv      Fix RVCT compiler warning for tools compilation
2008-03-12   sv      Add 512Mb M18 device to Intel device table
2008-03-08   sv      Add support for M18 half-density mode
2008-03-05   op      Support shadow mode with EFS2 in NOR 
2008-02-28   op      Remove intel_parts in global_parts[]
2008-01-08   op      Initial version
===========================================================================*/

#include "flash.h"
#ifndef FS_UNIT_TEST
  #include "msm.h"
#endif
#include "flashi.h"
#include "flash_nor_msm.h"
#include "flash_nor_device.h"
#include "flash_msg.h"
#include "flash_msm.h"
#include <string.h>

/****************************************************************
 * Global Data
 ***************************************************************/

/* MSM specific definitions pertinent to NOR flash */

/* GLOBAL FOR AMSS & TOOLS */
 /* List only Spansion parts tested by this target */
const flashi_nor_device *(spansion_parts[]) = {
  &S29WS256N0SB,
  &S29WS512P,
  &S29VS128R_TOP,
  &S29VS128R_BOT,
  &S29VS256R_TOP,
  &S29VS256R_BOT,
  NULL
};

/* List only Samsung parts tested by this target */
const flashi_nor_device *(samsung_parts[]) = {
  &K5N5629ABM,  /* 256 Mbit Samsung NOR */
  &K5N6433ABM,  /* 64 Mbit Samsung NOR  */
  &K5N6433ATM,  /* 64 Mbit Samsung NOR  */
  &K5N2833ATB,  /* 128 Mbit Samsung NOR  */
  &K5N2833ABB,  /* 128 Mbit Samsung NOR  */
  &K5N5629ATC,  /* 256 Mbit Sansung NOR  */
  &K5N5629ABC,  /* 256 Mbit Sansung NOR  */
  &K5N5629AUC,  /* 256 Mbit Sansung NOR  */
  NULL
};

/* List of all Intel parts that are probed similarly. */
const flashi_nor_device *(intel_parts[]) = {
#ifndef FEATURE_USES_LOWMEM
  &Intel_128M18_ADMux,
  &Intel_256M18_ADMux,
  &Intel_512M18_ADMux,
  &Intel_1024M18_ADMux,
  &Intel_64W18_ADMux,
#endif
  &M36W0R5040U6ZS,
  &M58LR128KC,
  NULL
};

const flashi_nor_device **(global_parts[]) = {
  intel_parts,
#ifndef FEATURE_USES_LOWMEM
  samsung_parts,
  spansion_parts,
#endif
  NULL
};

/* Flash geometric data buffer. The blk_region array field of this structure
 * can be used for all flash device data structures in the
 * intel_devices_array */
flash_geometry_info flash_geometry_data;
word cfi_data[CFI_TOTAL_WORD_SIZE];

/* Probe table for NOR flash devices usable by flash driver clients 
 * Do Intel probe first as the samsung/spansion probe command sequence 
 * would put the Intel part in Read Device Info mode.
 */
ProbeTbl flash_nor_probe_table[] = {
  { (volatile word *)       0x0, intel_probe    },
  { (volatile word *)       0x0, samsung_nor_probe },
  { (volatile word *)       0x0, spansion_probe },
  { (volatile word *) FLASH_PROBE_END, 0 }
};

/* Only when compiled into DMSS or for NOR boot */
#if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)

  /* GLOBAL FOR TOOLS ONLY */
  dword clk_mclk_freq_mhz;

#endif


/* MSM specific functions pertinent to NOR flash */

/***********************************************************************
FUNCTION      flash_nor_msm_init

DESCRIPTION   This function does any MSM specific initialization
              necessary for NOR flash to operate

DEPENDENCIES  None

RETURN VALUE  None
**********************************************************************/
void
flash_nor_msm_init(void)
{
}


/* Only called during AMSS probe. Tools probe does not need this function */
#if !defined(BUILD_JFLASH) && !defined(BUILD_ARMPRG) && \
    !defined(FLASH_NOR_PARTI_TBL)
/*===========================================================================
FUNCTION FLASH_NOR_VALIDATE_STRUCTURE

DESCRIPTION
  Verify the EFS2 info structure and validate it with the flash
  geometry fields

DEPENDENCIES
  Must be done before any operations are carried out on the flash hardware.

RETURN VALUE
  Return if all fields validated and found acceptable. Else FLASH_ERR_FATAL.

SIDE EFFECTS
  None
===========================================================================*/
void
flash_nor_validate_structure (struct fsi_device_data *self,
  flash_ptr_type baseaddr)
{
  uint32 flash_saddr;
  uint32 addr_found;
  int    efs_start_blk_region_index = 0;
  int    efs_start_blk_index = 0;
  int    i = 0, j = 0;
  flashi_nor_device *nor_device = &(self->nor_dev_info);
  uint8 hd_blk_size_shift = 0;  /* Block size shift for half-density mode */
  
  /* Flash Start address is an offset which should always start at 0x0 for
   * validating EFS start and end adresses. This is so by design because EFS
   * start and end addresses are defined as offsets into the device from the
   * base address.
   */   
  flash_saddr = 0x0;
  addr_found = FALSE;

  /* For half-density mode the block size is half so right shift by 1 */ 
  if (FLASH_PART_ATTRIB(self->partition_attributes, FLASH_PART_ATTRIBUTE1) ==
       (uint32)FLASH_PARTITION_SLC_READWRITE)
  {
    hd_blk_size_shift = 1;
  }

  /* Scan for the start address of EFS2 in the flash geometry and validate
   * that it starts on a flash block boundary */
  for (i = 0;
      (addr_found == FALSE) && (i < nor_device->geometry->num_erase_blk_region);
       i++)
  {
    for (j = 0; j < nor_device->geometry->blk_regions[i].numblocks; j++)
    {
      if (flash_saddr ==  self->partition_start_baddr)
      {
        /* Found start address on block boundary. Cache the indices */
        efs_start_blk_region_index = i;
        efs_start_blk_index = j;
        addr_found = TRUE;
        break;
      }
      else
      {
        flash_saddr += ((uint32)nor_device->geometry->blk_regions[i].
          blocksize_bytes >> hd_blk_size_shift);
      }
    }
  }

  if (addr_found == FALSE)
  {
    FLASH_ERR_FATAL("EFS2 start address not scanned in blk layout", 0, 0, 0);
  }

  /* Scan for the end address of EFS2 in the flash geometry and validate
   * that it starts on a flash block boundary */
  addr_found = FALSE;
  for (i = efs_start_blk_region_index; (addr_found == FALSE) &&
      (i < nor_device->geometry->num_erase_blk_region); i++)
  {
    if (i == efs_start_blk_region_index)
    {
      j = efs_start_blk_index;
    }
    else
    {
      j = 0;
    }

    for (; j < nor_device->geometry->blk_regions[i].numblocks; j++)
    {
      /* Validate that all EFS2 block sizes are of the same size */
      if ((uint32)(nor_device->geometry->blk_regions[i].blocksize_bytes >>
           hd_blk_size_shift) != (self->partition_blk_bsize))
      {
        FLASH_ERR_FATAL("EFS2 spans multiple blk sizes blocks", 0, 0, 0);
      }

      /* We need to start checking the 'end' address from the end address of the
       * 'start' block. Increment variable 'flash_saddr' to point to the end of
       * the block before comparison.
       */

      flash_saddr += ((uint32)nor_device->geometry->blk_regions[i].
        blocksize_bytes >> hd_blk_size_shift);

      if (flash_saddr == self->partition_limit_baddr)
      {
        /* Found end address on block boundary */
        addr_found = TRUE;
        break;
      }
    }
  }

  if (addr_found == FALSE)
  {
    FLASH_ERR_FATAL("EFS2 end address not scanned in blk layout!", 0, 0, 0);
  }

  /* Validate block count is accurately defined */
  if ((j - efs_start_blk_index + 1) !=
      (int)(self->partition_blk_cnt))
  {
    FLASH_ERR_FATAL("EFS2 block count mismatch!", 0, 0, 0);
  }

  /* Basic checks PASSED. Return */
}
#endif 
/*!defined(BUILD_JFLASH) && !defined(BUILD_ARMPRG) &&  
  !defined(FLASH_NOR_PARTI_TBL) */

#if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)
void
flash_nor_validate_structure (struct fsi_device_data *self,
  flash_ptr_type baseaddr)
{
}
#endif /* defined(BUILD_JFLASH) || defined(BUILD_ARMPRG) */

/*===========================================================================
FUNCTION FLASH_NOR_GEOMETRY_INIT

DESCRIPTION
  Execute the CFI query command to read out block layout and fill
  corresponding flash flashi_nor_device structure.

DEPENDENCIES
  Must be done before any operations are carried out on the flash hardware.

RETURN VALUE
  FLASH_SUCCESS             - Success
  FLASH_FAILURE             - An error occurred.

SIDE EFFECTS
  None
===========================================================================*/
flash_status
flash_nor_geometry_init (flashi_nor_device *nor_device, flash_ptr_type baseaddr)
{
  volatile word         *cfi_data_ptr = (volatile word *)(cfi_data + 0x2DL);
  word                  saved_int;
  word                  count, index;
  word                  bl_is_inverted = FALSE; /* block layout inverted */
  word                  total_block_count = 0;
  word                  next_bank_start_block = 0;  /* Keeps track of start
                                                      * sector in previous
                                                      * bank */
  volatile word         *ext_cfi_data_ptr;   
  byte                  ext_maj_cfi_revision;                                                      
  byte                  ext_min_cfi_revision;  
  boolean               boot_type_invalid = FALSE;

  if (nor_device->handcoded_geometry != NULL)
  {
    /* Copy over the handcoded geometry info */
    memcpy((void *)&flash_geometry_data, (void *)nor_device->handcoded_geometry,
      sizeof(flash_geometry_info));
  }
  
  /* Fill the fsi_device_info structure appropriately */
  if (nor_device->geometry == INIT_USING_CFI_AT_RUNTIME)
  {
    switch(nor_device->codes[0])
    {
      case 0x0001:
        flash_geometry_data.family_type = FLASH_SPANSION_FAMILY;
        if(nor_device->codes[1] == 0x7E)
        {
            flash_geometry_data.family_type = FLASH_SPANSION_2_FAMILY;
        }
        break;
      case 0x0020:
      case 0x0089:
        flash_geometry_data.family_type = FLASH_INTEL_FAMILY;
        break;
      case 0x00EC:
        flash_geometry_data.family_type = FLASH_SAMSUNG_FAMILY;
        break;
      default:  
        flash_geometry_data.family_type = FLASH_UNKNOWN_FAMILY;
        break;
    }

    /* Perform the ARM specific probe without interrupt */
    INTLOCK_SAV (saved_int);
    flash_read_cfi(baseaddr, &flash_geometry_data, cfi_data);
    INTFREE_SAV (saved_int);

    /* Verify device is CFI compliant */
    if ( ((inp(cfi_data + EXT_CFI_DEV_COMPLIANT0_OFFSET) != 'Q')) ||
         ((inp(cfi_data + EXT_CFI_DEV_COMPLIANT1_OFFSET) != 'R')) ||
         ((inp(cfi_data + EXT_CFI_DEV_COMPLIANT2_OFFSET) != 'Y')) )
    {
      return FLASH_FAILURE;
    }

    /* Fill the flash geometric data */
    flash_geometry_data.device_size = 
      (flash_size_type)inp (cfi_data +EXT_CFI_DEVICE_SIZE_OFFSET);
    flash_geometry_data.write_buf_size =
      (flash_wbuf_size_type) inpw(cfi_data +EXT_CFI_DEVICE_WBUF_SIZE_OFFSET);

    if (flash_geometry_data.family_type == FLASH_SPANSION_FAMILY || flash_geometry_data.family_type == FLASH_SPANSION_2_FAMILY)
    {
      /* Spansion devices encode the Top/Bottom/Dual boot information
       * in the primary vendor specific region of the CFI */
      word boot_type = inp (cfi_data + SPANSION_EXT_CFI_BOOT_TYPE_OFFSET);
      switch (boot_type)
      {
        case 1:
          /* Dual Boot device. So need to change the block layout */
          break;
        case 2:
          /* Bottom boot device.  No need to change block layout */
          break;
        case 3:
          /* Top boot device. Block layout needs to be inverted */
          bl_is_inverted = TRUE;
          break;
        default:
          boot_type_invalid = TRUE;
          break;
      } //end switch (boot_type)
      if(boot_type_invalid == TRUE)
      {
        FLASH_ERR_FATAL("Block layout unknown. Check datasheet!", 0, 0, 0);
      }
    }// end if

    if (flash_geometry_data.family_type == FLASH_SAMSUNG_FAMILY)
    {
      /* Samsung devices encode the Top/Bottom/Dual boot information
       * in the primary vendor specific region of the CFI */
      word boot_type = inp (cfi_data + SAMSUNG_EXT_CFI_BOOT_TYPE_OFFSET);
      switch (boot_type)
      {
        case 2:
          /* Bottom boot device.  No need to change block layout */
          break;
        case 3:
          /* Top boot device. Block layout needs to be inverted */
          bl_is_inverted = TRUE;
          break;
        case 4:
          /* Dual Boot device. So need to change the block layout */
          break;
        default:
          boot_type_invalid = TRUE;
          break;
      } //end switch (boot_type)
      if(boot_type_invalid == TRUE)
      {
        FLASH_ERR_FATAL("Block layout unknown. Check datasheet!", 0, 0, 0);
      }
    }// end if

    flash_geometry_data.num_erase_blk_region  = 
    	 inp (cfi_data +EXT_CFI_DEVICE_ERASE_BLK_INFO_OFFSET);

    if (flash_geometry_data.num_erase_blk_region > MAX_FLASH_REGIONS)
    {
      FLASH_ERR_FATAL("Not enough space to store Erase Block Region info!", 0, 0, 0);
    }

    for (count = 0; count < flash_geometry_data.num_erase_blk_region; count ++)
    {
      if (bl_is_inverted)
      {
        index = (flash_geometry_data.num_erase_blk_region - count) - 1;
      }
      else
      {
        index = count;
      }
      
      /* Check for index overflow */
      if (index >= MAX_FLASH_REGIONS)
      {
      	return FLASH_FAILURE;
      }
      
      flash_geometry_data.blk_regions[index].numblocks =
                                 (inp(cfi_data_ptr + 0x0) |
                                 (inp(cfi_data_ptr + 0x1) << 8)) +
                                 1;

      flash_geometry_data.blk_regions[index].blocksize_bytes =
                                 (inp(cfi_data_ptr + 0x2) |
                                 (inp(cfi_data_ptr + 0x3) << 8)) *
                                 256;

      total_block_count += (word)flash_geometry_data.blk_regions[index].numblocks;

      cfi_data_ptr += 0x4;
    }// end for 

    if (total_block_count > FSD_MAX_SECTORS)
    {
      FLASH_ERR_FATAL("FSD_MAX_SECTORS needs to be increased!", 0, 0, 0);
    }

    flash_geometry_data.num_total_blocks = total_block_count;

    if (flash_geometry_data.family_type == FLASH_SPANSION_FAMILY || flash_geometry_data.family_type == FLASH_SPANSION_2_FAMILY)
    {
      flash_geometry_data.x_iface = 
      	(flash_xiface_type) inpw(cfi_data +EXT_CFI_DEVICE_X_IFACE_OFFSET);
    
     flash_geometry_data.num_banks = 
       inp(cfi_data + SPANSION_EXT_CFI_DEVICE_NUM_BANKS_OFFSET);

     if (flash_geometry_data.num_banks > MAX_FLASH_BANKS)
     {
       FLASH_ERR_FATAL("Not enough space to store Bank info!", 0, 0, 0);
     }// end if 

     for (count = 0; count < flash_geometry_data.num_banks; count++)
     {
       flash_geometry_data.bank_info[count].start_block = next_bank_start_block;

       flash_geometry_data.bank_info[count].num_blocks =
         inp(cfi_data + SPANSION_EXT_CFI_DEVICE_NUM_BLOCKS_OFFSET + count);

       next_bank_start_block = flash_geometry_data.bank_info[count].start_block +
                               flash_geometry_data.bank_info[count].num_blocks;

       flash_geometry_data.bank_info[count].end_block =
         next_bank_start_block - 1;

       flash_geometry_data.bank_info[count].is_read_only = TRUE;
     }// end for count
    }// end if FLASH_SPANSION_FAMILY
    else if (flash_geometry_data.family_type == FLASH_INTEL_FAMILY)      
    {
      flash_geometry_data.x_iface = 
      	(flash_xiface_type) inpw(cfi_data +EXT_CFI_DEVICE_X_IFACE_OFFSET);
      	
      /* point to start of ext query data table */
      ext_cfi_data_ptr = cfi_data + CFI_WORD_SIZE; 

      ext_maj_cfi_revision = inp(ext_cfi_data_ptr + INTEL_EXT_CFI_MAJ_REV_OFFSET); 
      ext_min_cfi_revision = inp(ext_cfi_data_ptr + INTEL_EXT_CFI_MIN_REV_OFFSET);
       
      /* multi-partition support for CFI rev 1.4 */
      if (ext_maj_cfi_revision == '1')
      {
        if (ext_min_cfi_revision == '4')
        {
          if (inp(ext_cfi_data_ptr + INTEL_EXT_CFI_NUM_HWPART_REGION_OFFSET) == 1)
          {
            flash_geometry_data.num_banks = ((word)inp(ext_cfi_data_ptr + INTEL_EXT_CFI_NUM_HWPART_HI)<<8) 
                                               + ((word)inp(ext_cfi_data_ptr + INTEL_EXT_CFI_NUM_HWPART_LO));
           
            if (flash_geometry_data.num_banks > MAX_FLASH_BANKS)
            {
              FLASH_ERR_FATAL("Not enough space to store Bank info!", 0, 0, 0);
            }// end if 

            for (count = 0; count < flash_geometry_data.num_banks; count++)
            {
              flash_geometry_data.bank_info[count].start_block = next_bank_start_block;
     
              flash_geometry_data.bank_info[count].num_blocks =
                     ((word)inp(ext_cfi_data_ptr + INTEL_EXT_CFI_PART_NUM_BLOCKS_HI)<<8) 
                     + ((word)inp(ext_cfi_data_ptr + INTEL_EXT_CFI_PART_NUM_BLOCKS_LO)) 
                     + 1;
     
              next_bank_start_block = flash_geometry_data.bank_info[count].start_block +
                                       flash_geometry_data.bank_info[count].num_blocks;
     
              flash_geometry_data.bank_info[count].end_block =
                   next_bank_start_block - 1;
     
              flash_geometry_data.bank_info[count].is_read_only = TRUE;
            }// end for count
          } // end if inp()
        } // end if (ext_min_cfi_revision == '4')
      } // end if (ext_maj_cfi_revision == '1')
    } // end else if FLASH_INTEL_FAMILY  
    nor_device->geometry= &flash_geometry_data;
  }// end if (nor_device->geometry == INIT_USING_CFI_AT_RUNTIME)

  return FLASH_SUCCESS;
} /* flash_nor_geometry_init */

/*===========================================================================
FUNCTION FLASH_MSM_GET_DEVICE_TYPE

DESCRIPTION
  Returns the device type to be probed.

DEPENDENCIES
  None.

RETURN VALUE
  FLASH_NOR             - Probe a NOR Device

SIDE EFFECTS
  None
===========================================================================*/

uint32 flash_msm_get_device_type(void)
{
  return FLASH_NOR;
}
