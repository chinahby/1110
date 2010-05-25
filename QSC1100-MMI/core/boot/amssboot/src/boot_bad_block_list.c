/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Bad Block List Creation/Reading

GENERAL DESCRIPTION
  Probes NAND for Device paramters. Reads Boot Information Block.
  Creates the Bad block list and writes it to NAND if not already present.
  If Bad Block list is already present it is read from NAND.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2004,2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/04   rp      Added watch dog kicking in bad block list creation code.
                   Fixed bug in bad block list creation where the while loop
                   never returns if there are bad blocks
09/13/04   ktr     Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "flash_nand.h"
#include "flash_nand_partition.h"
#include "flash.h"
#include "boot_bad_block_list.h"
#include "boot_nand.h"
#include "pboot_genhash.h"
#include "miprogressive.h"
#include "miboot.h"
#include "mibib.h"
#include "mibbtb.h"
#include "miheader.h"
#include "boothw.h"
#include "err.h"
/* This custoemsbl.h has to be in the bottom to ensure we can overwrite the
** macro for OEMSBL
*/
#ifdef BUILD_BOOT_CHAIN
  #include "custoemsbl.h"
#endif/* BUILD_BOOT_CHAIN */

/*===========================================================================
                           GLOBAL DATA
===========================================================================*/


/* Bad Block list structure */
uint32 bad_block_table_buffer[BBTB_BUFFER_SIZE/sizeof(uint32)];

/* Pointer Bad Block list structure */
struct bad_block_table *ptr_bbtb = (struct bad_block_table *) &bad_block_table_buffer;

struct progressive_boot_block *prog_hdr;
uint32 paged_start_index;
uint32 bb_start_index;
#ifdef SEC_BOOT
uint8 prog_digest[PROG_BOOT_DIGEST_SIZE];
uint8 *prog_digest_ptr = (uint8*)&prog_digest;
uint8 *hash_tbl_ptr = (uint8*)MI_NAND_AMSS_HASH_TBL_DEST_ADDR;
boolean prog_auth_enable;
#endif
/* Buffer to read progressive boot info. */
unsigned char prog_buf[512];

/* Number of Bits to shift for mult or div with page_size */
uint32 page_size_shift_bits;

/* Number of Bits to shift for mult or div with pages_in_block */
uint32 pages_in_block_shift_bits;

/* Number of Bits to shift for mult or div with block_size */
uint32 block_size_shift_bits;

uint32 partition_start_page;

/*===========================================================================
                           FUNCTIONS
===========================================================================*/

/*===========================================================================

FUNCTION bit_Set_Position

DESCRIPTION
  Given a unsigned integer that is a power of 2, this function returns the nth bit set.

RETURN VALUE
  N'th Bit Set.

SIDE EFFECTS
  Inline Function. If called many times increases code size.
===========================================================================*/

__inline uint32 bit_set_position(uint32 num){

 uint32 shiftBy = 0;

    while(!(num & 0x1))
    {
            num = num >> 1;
            shiftBy = shiftBy + 1;
    }

  return shiftBy;

} // bit_set_position


uint32 boot_find_mibib ( void )
{
  uint32 new_mibib, old_mibib;

  /* Need to locate active MIBIB in flash */
  flash_nand_find_mibib(nand_device,(int*)&old_mibib,(int*)&new_mibib);

  return new_mibib;

} /* boot_find_mibib() */


boolean boot_progressive_init( void )
{ 
  uint32 active_mibib_page_num;
  uint32 new_mibib, old_mibib;  
  /* Need access to partition starting block -- use this structure to reference private data */
  fsi_device_t priv;
  uint32 orig_block_limit;
  int i;
  
  flash_nand_find_mibib(nand_device,(int*)&old_mibib,(int*)&new_mibib);
  
  active_mibib_page_num = new_mibib << pages_in_block_shift_bits;
  
  if(nand_device->read_page(nand_device,MIBIB_PAGE_PROGRESSIVE_BOOT + active_mibib_page_num, 
                            (uint8 *)prog_buf) != FS_DEVICE_DONE)
  {
    return FALSE;
  }
  
  prog_hdr = (struct progressive_boot_block*)prog_buf;
  
  /* If not valid Progressive Boot Current page, fail the boot. */
  if((prog_hdr->magic1 != MI_PBT_MAGIC1) ||
     (prog_hdr->magic2 != MI_PBT_MAGIC2) ||
     (prog_hdr->version!= MI_PBT_VERSION))
  {
    return FALSE;
  }  
  
  /* We also need to find the starting block of the AMSS partition.  For now, we
   * will use the flash_nand_open_parition function to find the block.  However,
   * since the bad block table is computed based on physical blocks (and not blocks
   * in the AMSS partition), we will base all future computations on physical blocks
   * and pages to save time.  So once we get the partition block, we will reset the
   * nand_device structure back to the original values.
   */
  priv=(fsi_device_t)nand_device;
  orig_block_limit = priv->partition_blk_limit;
  
  if(flash_nand_open_partition(nand_device, MIBIB_AMSS_PARTI_NAME) != FS_DEVICE_OK)
  {
    return FALSE;
  }

  /* Remember the starting position of the new partition */
  partition_start_page = priv->partition_blk_start << pages_in_block_shift_bits;

  /* Find the first block in the bad block list which is after our partition start
   * page.  We don't need to account for those that come before our partition.
   */

  bb_start_index = 0;
  while((bb_start_index < ptr_bbtb->num_entries) &&
    (ptr_bbtb->bad_block[bb_start_index] < priv->partition_blk_start))
  {
    bb_start_index++;
  }
  
  /* We found our partition base, now reset the partition */
  priv->partition_blk_start = 0;  
  priv->partition_blk_limit = orig_block_limit;    
  
  /* Find the first PAGED segment in the progressive header.  This will speed up 
   * calculations later, as typically all of the non paged segments are grouped
   * at the beginning.  If none is found, the "start" index will be set to 0.
   */
  paged_start_index = 0;
  for(i=0; i<prog_hdr->numsegments; i++)
  {
    if(prog_hdr->segment[i].type == MI_PBT_PAGED_SEGMENT)
    {
      paged_start_index = i;
#ifdef SEC_BOOT
      /* Update hash table pointer to start at first paged segment */
      hash_tbl_ptr += (paged_start_index * PROG_BOOT_DIGEST_SIZE);      
#endif      
      break;
    }
  }

  
}

/*===========================================================================

FUNCTION BOOT_NAND_FLASH_INITIALIZE

DESCRIPTION
  This function initializes nand flash and reads the progressive boot header
  from the MIBIB.

DEPENDENCIES
  None

RETURN VALUE
  If success, return TRUE else FALSE

===========================================================================*/

boolean boot_nand_flash_initialize(void)
{
  if (flash_nand_device_probe (&nand_priv_device, 0) == FS_DEVICE_OK)
  {
    nand_device = (fs_device_t)&nand_priv_device ;
  }
  else
  {
    nand_device = FS_NO_DEVICE;
  }
  
  if (nand_device == FS_NO_DEVICE)
  {
    return FALSE;
  }

  /* get some information about the device */
  block_count = nand_device->block_count(nand_device);
  pages_in_block = nand_device->block_size(nand_device);
  page_size = nand_device->page_size(nand_device);
  flash_name = nand_device->device_name(nand_device);

  /* Get Number of bits to shift */
  page_size_shift_bits = bit_set_position(page_size);
  pages_in_block_shift_bits = bit_set_position(pages_in_block);

  /* calculate size of eraseable unit and max number of pages */
  eu_size = (pages_in_block << page_size_shift_bits);
  max_pages = (block_count << pages_in_block_shift_bits);

  /* Get Number of bits to shift for Block Size*/
  block_size_shift_bits = bit_set_position(eu_size);
  
  return TRUE;
} /* boot_nand_flash_initialize () */



/*===========================================================================

FUNCTION boot_read_bad_block_list

DESCRIPTION
  This function creates the bad block list.

DEPENDENCIES

RETURN VALUE
  If success, return TRUE else returns FALSE
  returns CREATE_BAD_BLOCK_LIST_FAIL or
          CREATE_BAD_BLOCK_LIST_CORRUPT

===========================================================================*/

boolean boot_read_bad_block_list(void)
{
  uint32 total_size = 0;
  uint32 byte_count = 0;
  uint32 bad_block_list_firstpage = 0;
  uint32 good_blocks = 0;
  uint32 current_block = 0;
  int i = 0;

  BOOTHW_KICK_WATCHDOG();

  /* Count from block 0 to find the BBTB_BLOCKth non bad block */
  while((good_blocks <= MI_BLOCK_BBTB) && (current_block < block_count))
  {
    if(nand_device->bad_block_check(nand_device, current_block) != FS_DEVICE_BAD_BLOCK)
    {
      good_blocks++;
    }
    current_block++;
  }
  
  /* Adjust current_block to last checked block */
  current_block--;
  
  if (current_block >= block_count)
  {
    return FALSE;
  }

  BOOTHW_KICK_WATCHDOG();

  /* Point to first page in found block where Bad Block List is */
  bad_block_list_firstpage = current_block << pages_in_block_shift_bits; 
  
  
  /* The Bad Block Table should be programmed by the boot loader. 
   * Read the Bad Block Table and check for the magic numbers */
  
  if(nand_device->read_page(nand_device, bad_block_list_firstpage,(uint8*)(ptr_bbtb)) != FS_DEVICE_DONE)
  {
    return FALSE;
  }
  
  if ((ptr_bbtb->magic1  != BBTB_MAGIC1) ||
      (ptr_bbtb->magic2  != BBTB_MAGIC2) ||
      (ptr_bbtb->version != BBTB_VERSION))
  {
    return FALSE;
  }
  
  total_size = sizeof(uint32) * ptr_bbtb->num_entries + SIZEOF_BBTB_INFO;
  
  if(total_size > BBTB_BUFFER_SIZE)
  {
    ERR_FATAL("Bad block table too large to fit into buffer.",0,0,0);
  }
  
  /* Copy enough pages here for the entire bad block list structure */
  for(i=1, byte_count=page_size; byte_count<total_size; i++, byte_count += page_size)
  {
    BOOTHW_KICK_WATCHDOG();
    
    if(nand_device->read_page(nand_device,(bad_block_list_firstpage + i),(((uint8 *)ptr_bbtb)+(page_size*i))) != FS_DEVICE_DONE)
    {
      return FALSE;
    }
  }
  
  return TRUE;
}
