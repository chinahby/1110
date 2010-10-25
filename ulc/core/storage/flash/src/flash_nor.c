/**********************************************************************
 * flash_nor.c
 *
 * NOR device level operations
 *
 * This file implements wrappers for device specific functions on
 * NOR flash parts.
 *
 * Copyright (c) 2004-2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_nor.c#23 $ $DateTime: 2008/08/28 10:20:58 $ $Author: c_surven $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-08-08   sv      Modified is_page_erased() to use 16bit reads. 
2008-03-05   op      Add a dummy device width entry. 
2008-05-23   sv      Fix RVCT compiler warnings for tools compilation.
2008-05-06   sv      Move HD mode parti attribute setting to Intel flash driver.
2008-03-16   sv      Modify to uniformly use limits for range checking
2008-03-11   sv      Modified for M18 half density mode support
2008-03-05   op      Fix fs_nor_device_read_ptr to return physical page address 
2008-02-25   mh      Fix compile error
2008-01-14   op      Add macro for compile caching during probe
2008-01-14   rt      Add a dummy interface to query partition attributes.
2008-01-05   op      Featurize to compile without partitioning driver
2007-10-30   dp      Featurize function table using FLASH_NOR_PARTI_TBL
2007-09-17   op      Add an API to provide physical limits of current partition
2007-06-09   rt      Add a dummy interface to read pages from OTP area.
2007-05-09   amw     Change RAM probe address to use passed addr if non-zero.
2007-02-28   rt      Add support for NOR partitioning
2006-08-16   dp      Integrating quartz kernal ifdefs
2006-01-27   dp      Added Intel MLC support
2005-12-05   dp      NOR Flash Driver Unification
2004-10-21   drh     Clean up warning message
2004-07-20   drh     Remove MSM6050 conditional.  All MSM platforms support
                     this code now.
2004-02-23   drh     Support partition tables.
                     Move MSM specific device bases table to flash_msm.c.
2004-02-21   drh     Came from services/efs/fs_nor_device.c

===========================================================================*/

/****************************************************************
 * Include Files
 ***************************************************************/

#include "flash.h"
#include "flash_nor.h"
#include "flash_nor_partition.h"
#include "flash_msg.h"
#include "flash_partition.h"
#include "rex.h"
#include "task.h"
#include <string.h>

uint16 fs_dev_max_sleep_counter=100;
uint16 fs_dev_max_sleep_duration=50;

#undef FSND_DEBUG

/*lint -e613 -e668 upper layers already check for null pointers */
/*lint -e526 Don't warn about undefined extern symbols          */

/****************************************************************
 * Function Prototypes
 ***************************************************************/

#ifdef FSND_DEBUG
#error code not present
#endif

/* forward function prototype declarations */
int fs_nor_device_bad_block_check(fs_device_t self, block_id block);
int fs_nor_device_mark_block_bad(fs_device_t self, block_id block);
char * fs_nor_device_name(fs_device_t self);
uint32 fs_nor_device_block_count(fs_device_t self);
uint32 fs_nor_device_block_size(fs_device_t self);
uint32 fs_nor_device_page_size(fs_device_t self);
int fs_nor_device_write_page (fs_device_t self, page_id page, void *data);
int fs_nor_device_write_partial_page (fs_device_t self, page_id page,
      void *data, int offset, int length);
boolean fsi_ram_probe (volatile word *probe_addr);
int fs_nor_device_erase_block (fs_device_t self, block_id block);
void *fs_nor_device_read_ptr (fs_device_t self, page_id page);
int fs_nor_device_read_page (fs_device_t self, page_id page, void *data);
int fs_nor_device_is_page_erased (fs_device_t self, page_id page);
int fs_nor_device_start_erase_block (fs_device_t self, block_id block);
int fs_nor_device_suspend_erase (fs_device_t self);
int fs_nor_device_resume_erase (fs_device_t self);
int fs_nor_device_erase_block_status(fs_device_t self);
int flash_nor_get_partition_phy_limits(fs_device_t self, uint32 *start, 
      uint32 *limit, void *reserved);

void cache_mmu_disable(void);
void cache_mmu_re_enable(void);

 /****************************************************************
 * MACROS
 ***************************************************************/


/* On processors with CPU instruction caching, we must disable
 * caching during probe.  These macros make the code much
 * cleaner due to the conditional nature of the operation.
 */
#if !defined(QUARTZ_BSP)&& !defined(BUILD_JFLASH) && !defined(BUILD_ARMPRG)
#define CACHE_DISABLE()  cache_mmu_disable()
#define CACHE_REENABLE()  cache_mmu_re_enable()
#else /* QUARTZ_BSP */
/* stub until nor supported for quartz */
#define CACHE_DISABLE()
#define CACHE_REENABLE()
#endif


/****************************************************************
 * Global Data
 ***************************************************************/

static fs_device_write_style_t
  fs_nor_device_get_write_style (fs_device_t dev);

/* Array containing fs_device_data structure definitions for all the
 * nor devices currently supported */
struct fs_device_data flash_nor_ops[] = {
  {
    fs_nor_device_name,
    0,                                 /* Maker ID */
    0,                                 /* Device ID */
    fs_nor_device_block_count,
    fs_nor_device_block_size,
    fs_nor_device_page_size,
    0,                                 /* Total page size */
    0,                                 /* Device type */
    0,                                 /* Device width */
    fs_nor_device_bad_block_check,
    fs_nor_device_mark_block_bad,
    fs_nor_device_write_page,
    fs_nor_device_erase_block,
    fs_nor_device_read_ptr,
    fs_nor_device_read_page,
    fs_nor_device_is_page_erased,
    fs_nor_device_write_partial_page,
    fs_nor_device_start_erase_block,
    fs_nor_device_suspend_erase,
    fs_nor_device_resume_erase,
    fs_nor_device_erase_block_status,

    0,                                 /* Is block erased */
    0,                                 /* Read spare bytes */
    0,                                 /* Read multiple pages */
    0,                                 /* Copy page */
    0,                                 /* Lock Unlock Locktight */
    0,                                 /* Get corrected spare bytes count */
    0,                                 /* Write page and spare udata */
    0,                                 /* Read spare udata */
    0,                                 /* Read page and spare udata */
    0,                                 /* Read OTP page and spare data */
  #ifdef FLASH_NOR_PARTI_TBL
    FLASH_COMPILETIME_FN_INIT(flash_nor_open_partition), /* Open partition */   
  #else
    0,
  #endif
    FLASH_COMPILETIME_FN_INIT(flash_nor_get_partition_phy_limits), /*Get lmts*/
    0,                                 /* Get partition attributes */
    0,                                 /* Set ECC generation    */
    fs_nor_device_get_write_style,
  }
};

/* Most components need an address to check status of, or continue an
 * erase.
 */
static dword flashi_erase_location;
static fsi_erase_state_type flashi_erase_state;

volatile word *flashi_probe_addr;


/****************************************************************
 * Functions
 ***************************************************************/


/***********************************************************************
FUNCTION      flash_nor_device_probe

DESCRIPTION   This functions helps probe and identify the memory device
              to see if it is one of the known nor devices. For every
              nor device defined in the probe table the probe_device 
              function is called until a device is successfully identified.
              If a device is identified successfully then the partition 
              data is initialized with NULL values for partition mode and FS
              partition info for non-partition mode.

DEPENDENCIES  None

RETURN VALUE  fs_device_data structure pointer fs_device_t if the
              nor device was identified successfully else FS_NO_DEVICE
**********************************************************************/
int
flash_nor_device_probe (struct fsi_device_data *self, int whichflash)
{
  ProbeTbl_t prPtr;
  flashi_nor_device *nor_device = NULL;
#if !defined(FLASH_NOR_PARTI_TBL) 
  word pages_in_block;
  word num_of_shifts = 1;
  dword page_size;
  dword blk_size = 0;
#endif 

  /* Caching of flash must be disabled during probe
   * or flash will look like RAM and no flash will
   * be found.
   */
  CACHE_DISABLE();
  
  prPtr = flash_nor_probe_table;
  /* Begin probing the device */
  for (; NULL != prPtr->probe_fcn; prPtr++)
  {
    /* Probe at each of the possible addresses for flash 0
     * using the MSM specific table provided from flash_msm.c  
     */
    flashi_probe_addr = prPtr->base_addr;

    flashi_probe_addr =
      &flashi_probe_addr[(((uint32)FLASH_NON_ZERO_PROBE_ADDR_OFFSET) >> 1)];

    if (fsi_ram_probe(flashi_probe_addr) == FALSE)
    {
      /* Location is not a RAM location. Go ahead and probe for a flash device
       */
      nor_device = prPtr->probe_fcn (flashi_probe_addr);
    }
  #if defined(SHADOW_MODE) && defined(NOR_FLASH_ON_RAM)
    /* Make sure to include flash_nor_ram.c for compilation
     * to simulate a NOR flash on shadow RAM */
    else
    {
      extern const flashi_nor_device RAM_NOR;
      self->nor_dev_info = (flashi_nor_device *)&RAM_NOR;
    }
  #endif
    if (nor_device != NULL)
    {
      /* Initialize the base address field in the device structure */
      nor_device->base_addr = prPtr->base_addr;

      /* We detected a device */
      break;
    }
  }

  if (nor_device == NULL)
  {
    CACHE_REENABLE();
    return (int)FS_DEVICE_FAIL;
  }

  /* Initialize the attribute flag disabling HD mode as a default mode */ 
  self->partition_attributes = 0;

  /* Initialize the page size field to the default size */
  self->page_bsize = FLASH_NOR_DEFAULT_PAGE_SIZE;

  /* Force a structure copy to initialize device specific fields */
  self->nor_dev_info = *nor_device;

  /*  Call device's configure function.  On some flash parts,
   *  this will unlock the sectors for programming, on others
   *  it is merely a stub.
   */
  if ((*nor_device->ops->configure) (self, flashi_probe_addr) !=
       FLASH_SUCCESS)
  {
    CACHE_REENABLE();
    return (int) FS_DEVICE_FAIL;
  }

  self->pub_dev = flash_nor_ops[0];

#if defined(FLASH_NOR_PARTI_TBL)
  /* Initialize to 0 for partition mode.The partition data will be updated
   * with right values by open partition() function.
   */
  self->partition_start_baddr = 0;
  self->partition_limit_baddr = 0;
  self->which_flash = whichflash; 
#else 
  /* If there is no NOR partitioning support fill in the required members in
   * the device data structure which otherwise will be filled in by a call
   * to open_partition() function
   */
   
  blk_size = flash_nor_find_size_for_block_addr (&(self->nor_dev_info), 
    (dword)FLASH_NOR_EFS2_START_BYTE_OFFSET);
  if (blk_size == 0)
  {
    CACHE_REENABLE();
    return (int) FS_DEVICE_FAIL;
  }

  if (FLASH_PART_ATTRIB(self->partition_attributes, FLASH_PART_ATTRIBUTE1) ==
       (uint32)FLASH_PARTITION_SLC_READWRITE)
  {
    /*
     * Initialize partition data for M18 Half Density mode
     * The start address is divided by 2 
     * The limit address is truncated to fall on the end of the nearest block
     * The block size is divided by 2 as only half of the block can be used 
     * in Half Density mode 
     */

    self->partition_start_baddr = \
      (dword) (FLASH_NOR_EFS2_START_BYTE_OFFSET >> 1);
    self->partition_limit_baddr = \
      (dword) ((FLASH_NOR_EFS2_END_BYTE_OFFSET + 1) >> 1);
    self->partition_blk_bsize = (blk_size >> 1);   
  }
  else
  {
    /* Initialize partition data for all other device*/
    self->partition_start_baddr = (dword) (FLASH_NOR_EFS2_START_BYTE_OFFSET);
    self->partition_limit_baddr = (dword) (FLASH_NOR_EFS2_END_BYTE_OFFSET + 1);
    self->partition_blk_bsize = blk_size; 
  }

  /* Calculate block count as size of the partition divided by size of each
   * sector in the partition. It is assumed that partition only has uniformly
   * sized blocks.
   */
  self->partition_blk_cnt = (dword) ( (self->partition_limit_baddr -
                                       self->partition_start_baddr) /
                                    self->partition_blk_bsize );

  pages_in_block = (word) (self->partition_blk_bsize /
                           self->page_bsize);
  self->partition_last_pg_id = ( pages_in_block *
                                 (word) (self->partition_blk_cnt) ) - 1;

  page_size =  self->page_bsize;
  while (page_size != 2)
  {
    page_size >>= 1;
    num_of_shifts++;
  }
  self->page_size_shift = num_of_shifts;
  num_of_shifts = 1;
  while (pages_in_block != 2)
  {
    pages_in_block >>= 1;
    num_of_shifts++;
  }

  self->block_size_shift = num_of_shifts;
  self->which_flash = (uint32)whichflash;

#endif

  CACHE_REENABLE();

  return (int) FS_DEVICE_OK;
} /* End of flash_nor_device_probe */


/***********************************************************************
FUNCTION      fs_nor_device_write_page

DESCRIPTION   This functions writes a page data into the given page
              on the device

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if write was successful else FS_DEVICE_FAIL
**********************************************************************/
int
fs_nor_device_write_page (fs_device_t self, page_id page, void *data)
{
  fsi_device_t priv = (fsi_device_t) self;
  int length = priv->page_bsize; //lint !e713 no loss of precision.

#ifdef FSND_DEBUG
#error code not present
#endif

  return fs_nor_device_write_partial_page (self, page, data, 0, length);

} /* End of fs_nor_device_write_page */


/***********************************************************************
FUNCTION      fsi_nor_device_erase_block

DESCRIPTION   This function erases the contents of the given block

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if erase was successful else FS_DEVICE_FAIL
**********************************************************************/
#if !defined(QUARTZ_BSP) && !defined(FLASH_SINGLE_THREADED)
static int
fsi_nor_device_erase_block (fs_device_t self, uint32 block_addr)
{
  fsi_device_t priv = (fsi_device_t) self;
  flash_status status;
  uint16 sleepcounter=0;
  rex_timer_type  fs_dev_tout_timer;
  rex_tcb_type      * my_tcb;
  dword parti_boffset = priv->partition_start_baddr;
  volatile word * dev_base_addr = priv->nor_dev_info.base_addr;

  /* Determine if we have to check if an erase has terminated or
     not. */
  switch (flashi_erase_state)
  {
  case FSI_READ_MODE:
    break;                                   /* Easy. */

  case FSI_ERASING:
    /* Must check to see if this erase is still running. */
    FLASH_ERR_FATAL ("not yet written 123", 0, 0, 0);
    break;

  case FSI_ERASE_SUSPENDED:
    TPRINTF(3, ("flash_nor: Attempt to start erase while erase is suspended",
             0, 0, 0));
    return (int)FS_FAIL_S;
  }

  /* If we made it here, then the component is now sitting in read
     mode.  Our offset is in bytes from the beginning of EFS, get a
     pointer to the real data. */
  flashi_erase_location =
    (priv->partition_start_baddr >> 1) + (block_addr >> 1);

  status = (* priv->nor_dev_info.ops->erase_start)
    (&dev_base_addr[(parti_boffset >> 1)], (dword)block_addr);

  if (status != FLASH_SUCCESS)
    return FS_DEVICE_FAIL;

  flashi_erase_state = FSI_ERASING;

  do
  {
    sleepcounter++;
    if(sleepcounter > fs_dev_max_sleep_counter)
    {
       my_tcb = rex_self();
       rex_def_timer (&fs_dev_tout_timer, my_tcb, FS_OP_COMPLETE_SIG);
       (void)rex_timed_wait (FS_OP_COMPLETE_SIG,
                             &fs_dev_tout_timer, fs_dev_max_sleep_duration);
       sleepcounter=0;
    }
    status = (* priv->nor_dev_info.ops->erase_status)
               (&priv->nor_dev_info, &dev_base_addr[flashi_erase_location]);
  } while (status != FLASH_SUCCESS);

  flashi_erase_state = FSI_READ_MODE;

  return FS_DEVICE_DONE;
} /* End of fs_nor_device_erase_block */

#else /* QUARTZ_BSP || FLASH_SINGLE_THREADED */
/* stub due to rex until nor supported for quartz */
static int
fsi_nor_device_erase_block (fs_device_t self, uint32 block_addr)
{
    return FS_DEVICE_FAIL;
} /* End of fs_nor_device_erase_block */
#endif /* QUARTZ_BSP || FLASH_SINGLE_THREADED */


/***********************************************************************
FUNCTION      fs_nor_device_erase_block

DESCRIPTION   This function is a wrapper for the actual erase block
              and handles the special case of the last block, which
              is actually several blocks.

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if erase was successful else FS_DEVICE_FAIL
**********************************************************************/
int
fs_nor_device_erase_block (fs_device_t self, block_id block)
{
  fsi_device_t priv = (fsi_device_t) self;
  int status;
  uint32 block_address;
  uint32 block_count = priv->partition_blk_cnt;

  /* bail out early if block is invalid */
  if (block >= block_count)
  {
    return FS_DEVICE_FAIL;
  }

  /* Set the block address */
  block_address = block << (priv->page_size_shift + priv->block_size_shift);

  status = fsi_nor_device_erase_block(self, block_address);

  if (status != FS_DEVICE_DONE)
    return FS_DEVICE_FAIL;

 /* special code for handling of erase of last block presently commented 
  * out to improve EFS2 performance */

  /* Call the internal function that does erase and wait for each
   * block as necessary.  Account for the fact that the last "block"
   * is really several blocks masquerading as one block to the 
   * public interface */
  /*if (block != last_block)
  {
    status = fs_nor_device_erase_blocki(block_address);

    if (status != FS_DEVICE_DONE)
      return FS_DEVICE_FAIL;
  }
  else
  {
    small_block_count = nor_device->info.small_block_count;
    small_block_bytes = nor_device->info.small_block_bytes;
    for (i=0; i<small_block_count; i++)
    {
      status = fs_nor_device_erase_blocki(block_address);

      if (status != FS_DEVICE_DONE)
        return FS_DEVICE_FAIL;

      block_address += small_block_bytes;
    }
  }*/
  /* End of special code for handling of last block */

  return FS_DEVICE_DONE;

} /* End of fs_nor_device_erase_block */

/***********************************************************************
FUNCTION      fs_nor_device_is_page_erased

DESCRIPTION   This function checks if every byte of a given page is
              erased

DEPENDENCIES  None

RETURN VALUE  TRUE if page is erased
              FALSE if page is not erased, or any error occurs during
                    checking
**********************************************************************/
int
fs_nor_device_is_page_erased (fs_device_t self, page_id page)
{
  fsi_device_t priv = (fsi_device_t) self;
  dword page_address;
  word *rdptr;
  dword page_size = priv->page_bsize;
  dword i;
  int status = TRUE;

  /* Verify that the requested page is in range. */
  if (page > priv->partition_last_pg_id)
  {
    status = FALSE;
  }
  else /* if (page <= priv->partition_last_pg_id) */
  {
    page_address = page << priv->page_size_shift;

    /* Get the address to the beginning of partition into a dword pointer
     * and then add the page offset to it to come up with the page address.
     *
     * Note that since partition start address is a byte offset, it is
     * converted to a word offset for being used as an index into an array
     * represented by base address which is a word pointer.
     *
     * Same rule is used when calculating physical address to the beginning
     * of the page by adding logical page address, to the partition physical
     * start address.Since page address is used as an index into an array 
     * represented by partition physical start address which is a dword 
     * pointer, logical page address is divided by 4 before being used as an 
     * index into the array. 
     */
    rdptr =
      (word *) &(priv->nor_dev_info.base_addr)[(priv->partition_start_baddr >> 
        1)];
    rdptr = &(rdptr[page_address >> 1]);

  #ifdef FSND_DEBUG
#error code not present
  #endif

    if ((priv->nor_dev_info.ops->is_erased) != NULL)
    {
      /* Use the device specific func call to check if page is erased */
      if ((*priv->nor_dev_info.ops->is_erased)((flash_ptr_type)rdptr, 
                                               page_size) !=  FLASH_SUCCESS)
      {
        status = FALSE;
      }
    }
    else /* if ((priv->nor_dev_info.ops->is_erased) == NULL) */
    {
      for (i = 0; i < (page_size >> 1); i++)
      {
        /* Check if any bit is programmed to 0, if so return error */
        if ((rdptr[i] & 0xFFFF) != 0xFFFF)
        {
          status = FALSE;
          break;
        }
      }
    } /* if ((priv->nor_dev_info.ops->is_erased) != NULL) */
  } /* if (page <= priv->partition_last_pg_id) */

  return status;
} /* END fs_nor_device_is_page_erased */


/***********************************************************************
FUNCTION      fs_nor_device_read_ptr

DESCRIPTION   This function returns an address pointer to a page.

DEPENDENCIES  None

RETURN VALUE  Pointer to the page beginning

SIDE EFFECTS  None
**********************************************************************/
void *
fs_nor_device_read_ptr (fs_device_t self, page_id page)
{
  fsi_device_t priv = (fsi_device_t) self;
  uint32 page_ptr = 0;

  if (page <= priv->partition_last_pg_id)
  {
    page_ptr = page << priv->page_size_shift;

    /* Get a physical page address */
    page_ptr = page_ptr + priv->partition_start_baddr + 
               (uint32)priv->nor_dev_info.base_addr;

  #ifdef FSND_DEBUG
#error code not present
  #endif

    if ((priv->nor_dev_info.ops->read_ptr) != NULL)
    {
      /* Read ptr using device specific function call */
      page_ptr = (uint32) \
        (*priv->nor_dev_info.ops->read_ptr)((flash_ptr_type)page_ptr,
                                             priv->page_bsize);
    }
  }
  return (void *)page_ptr;
}


/***********************************************************************
FUNCTION      fs_nor_device_read_page

DESCRIPTION   This function read page data of the given page into the
              given buffer

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if read was successful else
              FS_DEVICE_FAIL
**********************************************************************/
int
fs_nor_device_read_page (fs_device_t self, page_id page, void *data)
{
  fsi_device_t priv = (fsi_device_t) self;
  uint32 page_address;
  flash_status status;

  /* Put a check here to make sure that page ID is within in partition */

  page_address = page << priv->page_size_shift;

#ifdef FSND_DEBUG
#error code not present
#endif

  status = fsi_nor_device_worded_read (self, (byte *)data, (dword)page_address,
                                       (dword)(priv->page_bsize));

  if (status != FLASH_SUCCESS)
  {
    return FS_DEVICE_FAIL;
  }
  else
  {
    return FS_DEVICE_DONE;
  }

} /* End of fs_nor_device_read_page */


/***********************************************************************
FUNCTION      fs_nor_device_bad_block_check

DESCRIPTION   This function should be called to determine whether
              a block is bad or not

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_OK always, as there is no such thing as a
              bad block in NOR flash.

SIDE EFFECTS  None
**********************************************************************/
int
fs_nor_device_bad_block_check (fs_device_t self, block_id block)
{
  (void) self;
  (void) block;
  return FS_DEVICE_OK;
} /* End of fs_device_nor_bad_block_check */


/***********************************************************************
FUNCTION      fs_nor_device_mark_block_bad

DESCRIPTION   Mark a block as bad

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_FAIL always, as this cannot be done on NOR flash.

SIDE EFFECTS  None
**********************************************************************/
int
fs_nor_device_mark_block_bad (fs_device_t self, block_id block)
{
  (void) self;
  (void) block;
  return FS_DEVICE_FAIL;
} /* End of fs_device_nor_mark_block_bad */


/***********************************************************************
FUNCTION      fs_nor_device_write_partial_page

DESCRIPTION   This function writes a partial page of data

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if write was successful else FS_DEVICE_FAIL

SIDE EFFECTS  None
**********************************************************************/
int
fs_nor_device_write_partial_page (fs_device_t self, page_id page,
      void *data, int offset, int length)
{
  fsi_device_t priv = (fsi_device_t) self;
  uint32 page_address;
  flash_status status;
  uint32 write_addr;

  /* If 'length' is 0, there is nothing to be written. Return with a good
   * status.
   */
  if (length == 0)
  {
    return FS_DEVICE_DONE;
  }

  page_address = page << priv->page_size_shift;
  write_addr = page_address + offset;   //lint !e737 offset is always +ve

  /* check to make sure the write does not go off the page */
  if ((offset + length) > (priv->page_bsize))  //lint !e574 !e737
    //sign/unsigned mix rel ok
  {
    return FS_DEVICE_FAIL;
  }

#ifdef FSND_DEBUG
#error code not present
#endif

  /* Verify that the requested data is in range.  The user sees the
     component as starting at zero, so we only need to check the end. */
  if ( (write_addr + (uint32)length) >
       (priv->partition_limit_baddr - priv->partition_start_baddr) )
  {
    TPRINTF(3, ("fs_dev: write request out of range", 0, 0, 0));
    return FS_DEVICE_FAIL;
  }

  /* We cannot write while an erase is happening. */
  if (flashi_erase_state != FSI_READ_MODE
      && flashi_erase_state != FSI_ERASE_SUSPENDED)
  {
    TPRINTF(3, ("fs_dev: attempt to write while device is erasing", 0,
             0, 0));
    return FS_DEVICE_FAIL;
  }

  status = (* priv->nor_dev_info.ops->write) ((byte *)data,
            &(priv->nor_dev_info.base_addr)[(priv->partition_start_baddr >> 1)],
             (dword)write_addr, (dword)length);
  if (status != FLASH_SUCCESS)
  {
    return FS_DEVICE_FAIL;
  }
  else
  {
    return FS_DEVICE_DONE;
  }
}

/*===========================================================================

FUNCTION fs_nor_device_start_erase_block

DESCRIPTION   Starts an erase operation on a block of the flash.

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if erase was successful else FS_DEVICE_FAIL

SIDE EFFECTS   An erase operation is begun upon the flash.  This operation
  must be suspended before writes can occur.

===========================================================================*/
int
fs_nor_device_start_erase_block (fs_device_t self, block_id block)
{
  fsi_device_t priv = (fsi_device_t) self;
  flash_status status;
  uint32 block_address;
  uint32 block_count = priv->partition_blk_cnt;
  boolean fatal_error_occured = FALSE;

  /* bail out early if block is invalid */
  if (block >= block_count)
  {
    return FS_DEVICE_FAIL;
  }

  /* Set the block address */
  block_address = block << (priv->page_size_shift + priv->block_size_shift);

  /* Determine if we have to check if an erase has terminated or
     not. */
  switch (flashi_erase_state)
  {
  case FSI_READ_MODE:
    break;                                   /* Easy. */

  case FSI_ERASING:
    /* Must check to see if this erase is still running. */
    fatal_error_occured = TRUE;
    break;

  case FSI_ERASE_SUSPENDED:
    TPRINTF(3, ("fsi_dev: Attempt to start erase while erase is suspended",
             0, 0, 0));
    return (int)FS_FAIL_S;
  }

  if(fatal_error_occured == TRUE)
  {
    FLASH_ERR_FATAL ("Erase attempted when another erase is in progress", 
      0, 0, 0);
  }
  /* If we made it here, then the component is now sitting in read
   * mode.  Our offset is in bytes from the beginning of EFS, get a
   * pointer to the real data.
   */
  flashi_erase_location = (priv->partition_start_baddr >> 1) +
    (block_address >> 1);

  status = (* priv->nor_dev_info.ops->erase_start)
    ( &(priv->nor_dev_info.base_addr)[(priv->partition_start_baddr>> 1)],
                                      (dword)block_address);

  if (status != FLASH_SUCCESS)
    return FS_DEVICE_FAIL;

  flashi_erase_state = FSI_ERASING;

  return FS_DEVICE_DONE;
} /* End of fs_nor_device_start_erase_block */

/*===========================================================================
FUNCTION      fs_nor_device_suspend_erase

DESCRIPTION   This function suspends the current erase.

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if the erase has completed early.
              FS_DEVICE_BUSY if the erase is suspended.
              FS_DEVICE_FAIL if the suspend failed.
===========================================================================*/
int
fs_nor_device_suspend_erase (fs_device_t self)
{
  fsi_device_t priv = (fsi_device_t) self;
  flash_status status;

  status = (* priv->nor_dev_info.ops->suspend_erase)
    (&(priv->nor_dev_info.base_addr)[flashi_erase_location]);

  if(status == FLASH_SUCCESS)
  {
    /* Erase complete */
    flashi_erase_state = FSI_READ_MODE;
    return FS_DEVICE_DONE;
  }
  else if (status == FLASH_OP_NOT_COMPLETE)
  {
    /* Suspended */
    flashi_erase_state = FSI_ERASE_SUSPENDED;
    return FS_DEVICE_BUSY;
  }
  return FS_DEVICE_FAIL;
}
/*===========================================================================
FUNCTION      fs_nor_device_resume_erase

DESCRIPTION   This function resumes the suspended erase.

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_BUSY if the erase is running again.
              FS_DEVICE_FAIL if the resume failed.
===========================================================================*/
int
fs_nor_device_resume_erase (fs_device_t self)
{
  fsi_device_t priv = (fsi_device_t) self;
  flash_status status;

  status = (* priv->nor_dev_info.ops->resume_erase)
    (&(priv->nor_dev_info.base_addr)[flashi_erase_location]);
  if(status != FLASH_SUCCESS)
  {
    return FS_DEVICE_FAIL;
  }

  flashi_erase_state = FSI_ERASING;
  return FS_DEVICE_BUSY;
}
/*===========================================================================
FUNCTION      fs_nor_device_erase_block_status

DESCRIPTION   This function checks and returns the status of the
              flash device.

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if the erase has completed.
              FS_DEVICE_BUSY if the erase is still running.
===========================================================================*/
int
fs_nor_device_erase_block_status(fs_device_t self)
{
  fsi_device_t priv = (fsi_device_t) self;
  flash_status status;

  if(flashi_erase_state == FSI_ERASING ||
     flashi_erase_state == FSI_ERASE_SUSPENDED)
  {
    status = (* priv->nor_dev_info.ops->erase_status) ( &priv->nor_dev_info,
                    &(priv->nor_dev_info.base_addr)[flashi_erase_location] );

    if (status != FLASH_SUCCESS)
    {
      return FS_DEVICE_BUSY;
    }
    else
    {
      flashi_erase_state = FSI_READ_MODE;
    }

  }

  return FS_DEVICE_DONE;
}

/*===========================================================================

FUNCTION fsi_ram_probe

DESCRIPTION
  Probes the given address location to see if it is a writable RAM space.

DEPENDENCIES
  None

RETURN VALUE
  True if a writable area is found, False otherwise.

SIDE EFFECTS
  None

===========================================================================*/

#ifndef QUARTZ_BSP
boolean fsi_ram_probe (volatile word *probe_addr)
{
#ifdef FS_DEV_UNIT_TEST
#error code not present
#else
  volatile word *base;
  word saved_val;

  /* Ensure probe address is not NULL. */
  if ( probe_addr == NULL )
  {
    /* Location to probe, use stamp_md5_hash */
  #if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)
    base = probe_addr;
  #else
  {
    extern word stamp_md5_hash;
    base = (word *)((int)&stamp_md5_hash/2);
  }
  #endif
  }
  else
  {
    base = probe_addr;
  }
  saved_val = base[0];

  base[0] = 0xAA;
  if ( (base[0] & 0x00FF) != 0xAA)
  {
    return( FALSE );
  }

  base[0] = 0x55;
  if ( (base[0] & 0x00FF) != 0x55)
  {
    return( FALSE );
  }

  base[0] = 0xFF;
  if ( (base[0] & 0x00FF) != 0xFF )
  {
    return( FALSE );
  }

  base[0] = saved_val;
  return( TRUE );
#endif
}
#else /* QUARTZ BSP */
/* stub until nor supported for quartz */
boolean fsi_ram_probe (volatile word *probe_addr)
{
  (void) *probe_addr;
  return( FALSE );
}
#endif


/*===========================================================================

FUNCTION fsi_nor_device_worded_read

DESCRIPTION
  Read a sequence of bytes from a word oriented device.  This is
  generic to any word sized device.
  Flash tools are not supposed to use this function. If a call is made, this 
  function will always return fail.

DEPENDENCIES
  None

RETURN VALUE
  FLASH_FAILURE if the read couldn't be done.
  FLASH_SUCCESS if it succeeded.

SIDE EFFECTS
  None

===========================================================================*/
#if !defined(BUILD_JFLASH) && !defined(BUILD_ARMPRG)
flash_status
fsi_nor_device_worded_read (fs_device_t self, byte *buffer,
  dword offset, dword count)
{
  fsi_device_t priv = (fsi_device_t) self;
  volatile word * dev_base_addr = priv->nor_dev_info.base_addr;
  dword parti_boffset = priv->partition_start_baddr;
  dword parti_bsize = (priv->partition_limit_baddr - parti_boffset);

#ifndef DO_READ_WITH_MEMMOVE
  volatile word *wptr;
  volatile byte *bptr;
#endif /* not DO_READ_WITH_MEMMOVE */

  /* Verify that the requested data is in range.  The user sees the
     component as starting at zero, so we only need to check the end. */
  if ((offset + count) > (parti_bsize))
  {
    TPRINTF(3, ("fs_dev: read request out of range", 0, 0, 0));
    return (flash_status) FLASH_FAILURE;
  }

  /* We cannot read while an erase is happening. */
  if (flashi_erase_state == FSI_ERASING)
  {
    TPRINTF(3, ("fs_dev: attempt to read while device is erasing", 0,
             0, 0));
    return (flash_status) FLASH_FAILURE;
  }

  /* Quickly handle the 0 byte read case. */
  /* This function is always called with count = page_size. It is never
   *  going to be '0'. Should this check be gotten rid of.
   */
  if (count == 0)
  {
    return FLASH_SUCCESS;
  }

  if ((priv->nor_dev_info.ops->read_dev) != NULL)
  {
    /* Read using Device Specific function call*/
    return ((*priv->nor_dev_info.ops->read_dev)
            ((flash_ptr_type)((dword)dev_base_addr + parti_boffset + offset),
            buffer, count));
  }

#ifdef DO_READ_WITH_MEMMOVE
  (void)memmove (buffer, ((byte *) (dev_base_addr +
                 (parti_boffset >> 1)) + offset),
                 count);
#else /* not DO_READ_WITH_MEMMOVE */

  /* The ARM processor doesn't handle misaligned data accesses, so we
     must account for that here. */

  /* The easy case is if the alignment of the two is the same. */
  if (((dword) buffer & 1) == (offset & 1))
  {
    /* Is there a "byte" at the beginning? */
    if ((offset & 1) == 1)
    {
      *buffer = *((volatile byte *) (dev_base_addr + (parti_boffset >> 1))
                  + offset);
      buffer++;
      offset++;
      count--;
    }

    /* Copy over any appropriately aligned data. */
    wptr = &dev_base_addr[(parti_boffset >> 1) + (offset >> 1)];
    while (count > 1)
    {
      *((word *) buffer) = *wptr;  /*lint !e826*/
      buffer += 2;
      wptr   += 1;
      count  -= 2;
    }

    /* There may be one additional byte to copy. */
    if (count)
    {
      *buffer = *((byte *) wptr);   //lint !e794
    }
  }
  /* Otherwise, the two sections of data are misaligned.  There are
     two options, either copy the data in byte mode, or perform word
     operations and scramble the bytes around manually.  Because both
     RAM and the component are 16 bits wide, we gain much by doing the
     scrambling. */
  else
  {
    /* Simple byte copy. */
    bptr = ((byte *) &dev_base_addr[(parti_boffset >> 1)]) + offset;
    while (count-- > 0)
    {
      *buffer++ = *bptr++;
    }
  }

#endif /* not DO_READ_WITH_MEMMOVE */

  return FLASH_SUCCESS;
}
#else
flash_status
fsi_nor_device_worded_read (fs_device_t self, byte *buffer,
  dword offset, dword count)
{
  /* This function is a stub and is not supposed to be called from tools.
   * Return FLASH_FAILURE of it was ever called.
   */
  return (flash_status) FLASH_FAILURE;
}
#endif

/***********************************************************************
  Functions that return device parameters
**********************************************************************/
char *
fs_nor_device_name (fs_device_t self)
{
  fsi_device_t priv = (fsi_device_t) self;
  return (char *)priv->nor_dev_info.name;
}

uint32
fs_nor_device_block_count (fs_device_t self)
{
  fsi_device_t priv = (fsi_device_t) self;
  return priv->partition_blk_cnt;
}

uint32
fs_nor_device_block_size (fs_device_t self)
{
  fsi_device_t priv = (fsi_device_t) self;
  return (priv->partition_blk_bsize / priv->page_bsize);
}

uint32
fs_nor_device_page_size (fs_device_t self)
{
  fsi_device_t priv = (fsi_device_t) self;
  return (priv->page_bsize);
}

fs_device_write_style_t
fs_nor_device_get_write_style (fs_device_t self)
{
  fsi_device_t priv = (fsi_device_t) self;
  return priv->nor_dev_info.write_style;
}


/*===========================================================================
FUNCTION      flash_nor_get_partition_phy_limits

DESCRIPTION   This function uses information in device's private data
              structure to calculate and return physical limits of the device. 
PARAMETERS:
    IN  : self - pointer to the device structure
    OUT : start - partition start byte address
    OUT : limit - partition end byte address
    IN  : reserved - not used

DEPENDENCIES
    The device should be probed before calling this API. The device 
    descriptor 'self' must be Non-NULL.
    
RETURN VALUE  
    FS_DEVICE_OK    if partition entry found
    FS_DEVICE_FAIL  if not found
===========================================================================*/

int 
flash_nor_get_partition_phy_limits(fs_device_t self, uint32 *start, 
                                   uint32 *limit, void *reserved)
{
  int status;
  fsi_device_t priv = (fsi_device_t) self;
  
  if (priv != NULL)
  {  
    uint32 flash_dev_base_addr = (uint32)priv->nor_dev_info.base_addr;
    *start = priv->partition_start_baddr + flash_dev_base_addr;
    *limit = priv->partition_limit_baddr + flash_dev_base_addr;
    status = FS_DEVICE_OK;
  }
  else
  {
    TPRINTF(3, ("fngppl:  Could not get physical limits.\n"));
    status = FS_DEVICE_FAIL;
  }
  return status;
}
