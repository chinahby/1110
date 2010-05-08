/***********************************************************************
 * fs_device.c
 *
 * Flash operation wrappers and counters.
 * Copyright (C) 2006-2008 QUALCOMM, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_device.c#6 $ $DateTime: 2008/12/12 11:11:10 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-12    rp   Introducing i/o-vec interface and Managed Nand.
2008-11-06    sh   Add Babylon markers to flash.
2008-11-03    rp   Introduce device abstraction.
2008-09-09    yg   Added code to do readback verification for writes.
2008-05-23    rp   Use bencmark counters from fs_counters.h
2007-10-18    rp   Featurized QEFS2.
2007-09-10    rp   Fix documentation for fs_flash_partial_write.
2007-08-10    rp   Ensure buffers to flash are aligned on a 32-byte boundary.
2007-04-23   umr   Added support for ARM MMU DAC memory protection feature.
2007-02-23   dlb   Move read failures into device wrappers.
2006-11-09   dlb   Add dealloc counter.
2006-09-22    sh   More comments.
2006-09-22   dlb   Lint fixes.
2006-09-12   dlb   Change some queries to unsigned values.
2006-09-01   dlb   Create

===========================================================================*/

#include "fs_efs2_cust.h"
#include "fs_device.h"
#include "fs_err.h"
#include "assert.h"
#include "fs_protected_memory.h"
#include "fs_sys_types.h"
#include "fs_babylon.h"
#include "fs_device_efs_flash.h"
#include "fs_device_ftl_flash.h"
#include "fs_device_efs_mnand.h"

/* Enable this flag to do readback verification of flash writes. */
#undef EFS_READ_BACK_VERIFY_PAGE

#ifdef EFS_READ_BACK_VERIFY_PAGE
#include <string.h>
static byte verify_buffer[EFS_PAGE_SIZE];
#endif


/* The counters should always be updated, but there might not be any
 * code to reference (read) them.  They are still useful from within a
 * debugger.  We explain it to Lint this way: */
//lint -esym(551, write_color_counters)
//lint -esym(551, read_color_counters)
//lint -esym(551, erase_color_counters)
//lint -esym(551, buffer_fill_counters)

void
fs_device_init (void)
{
  fs_device_efs_flash_init ();
  fs_device_ftl_flash_init ();
  fs_device_efs_mnand_init ();
}

efs_device_t
fs_device_open (enum fs_device_type device_type, uint32 which_device)
{
  efs_device_t base_dev = NULL;

  switch (device_type)
  {
    case FS_DEVICE_TYPE_EFS_NAND:
    case FS_DEVICE_TYPE_EFS_ONE_NAND:
    case FS_DEVICE_TYPE_EFS_NOR:
      base_dev = fs_device_efs_flash_open (which_device);
      break;

    case FS_DEVICE_TYPE_FTL_NAND:
      base_dev = fs_device_ftl_flash_open (which_device);
      break;

    case FS_DEVICE_TYPE_EFS_MNAND:
      base_dev = fs_device_efs_mnand_open (which_device);
      break;

    default:
      FS_ERR_FATAL ("Unknown device type...",0,0,0);
      break;
  }

  if (base_dev == NULL)
    return NULL;

  /* Validate the device-type. */
  switch (base_dev->efs_device_type)
  {
    case FS_DEVICE_TYPE_EFS_NOR:
    case FS_DEVICE_TYPE_EFS_NAND:
    case FS_DEVICE_TYPE_EFS_ONE_NAND:
    case FS_DEVICE_TYPE_FTL_NAND:
    case FS_DEVICE_TYPE_EFS_MNAND:
      break;

    default:
      FS_ERR_FATAL ("invalid device type...",0,0,0);
      break;
  }

  return base_dev;
}

int
fs_device_close (efs_device_t base_dev)
{
  int result = -1;

  switch (base_dev->efs_device_type)
  {
    case FS_DEVICE_TYPE_EFS_NAND:
    case FS_DEVICE_TYPE_EFS_ONE_NAND:
    case FS_DEVICE_TYPE_EFS_NOR:
      result = fs_device_efs_flash_close (base_dev);
      break;

    case FS_DEVICE_TYPE_FTL_NAND:
      result = fs_device_ftl_flash_close (base_dev);
      break;

    case FS_DEVICE_TYPE_EFS_MNAND:
      result = fs_device_efs_mnand_close (base_dev);
      break;

    default:
      FS_ERR_FATAL ("Unknown device type...",0,0,0);
      break;
  }

  if (result != 0)
  {
    FS_ERR_FATAL ("FS could not close device...",0,0,0);
  }

  return result;
}

int
fs_device_is_nand_device (efs_device_t base_dev)
{
  int is_nand = 0;

  switch (base_dev->efs_device_type)
  {
    case FS_DEVICE_TYPE_EFS_NAND:
    case FS_DEVICE_TYPE_EFS_ONE_NAND:
      is_nand = 1;
      break;

    default:
      is_nand = 0;
  }

  return is_nand;
}

int
fs_device_write_page (efs_device_t base_dev, fs_page_id page, void *data,
    enum fs_flashop_category category)
{
  int result;

  EFS_BAB_START (EFS_BAB_FLASH_WRITE_PAGE);
  /* Make sure that the address of the buffer is meeting the alignment
     expectations of the DMA. */
  ASSERT (FS_OKAY_FOR_DM (data));

  fs_counters_update (FS_FOP_WRITE, category, 1);

  result = base_dev->write_page (base_dev, page, data);
#ifdef EFS_READ_BACK_VERIFY_PAGE
  if (result == FS_DEVICE_RESULT_SUCCESS)
  {
    result = base_dev->read_page (base_dev, page, verify_buffer);
    if (result != FS_DEVICE_RESULT_SUCCESS)
      FS_ERR_FATAL ("Unable to read the written page=%d", page, 0, 0);

    if (memcmp (data, verify_buffer, sizeof(verify_buffer)))
      FS_ERR_FATAL ("Read data is different than written page=%d", page, 0, 0);
  }
#endif
  EFS_BAB_END (EFS_BAB_FLASH_WRITE_PAGE);
  return result;
}

int
fs_device_erase_block (efs_device_t base_dev, fs_block_id block,
    enum fs_flashop_category category)
{
  int result;
  EFS_BAB_START (EFS_BAB_FLASH_ERASE_BLOCK);
  fs_counters_update (FS_FOP_ERASE, category, 1);
  result = base_dev->erase_block (base_dev, block);
  EFS_BAB_END (EFS_BAB_FLASH_ERASE_BLOCK);
  return result;
}

int
fs_device_read_page (efs_device_t base_dev, fs_page_id page, void *data,
    enum fs_flashop_category category,
    enum fs_flash_failure failure)
{
  int result;

  EFS_BAB_START (EFS_BAB_FLASH_READ_PAGE);
  /* Make sure that the address of the buffer is meeting the alignment
     expectations of the DMA. */
  ASSERT (FS_OKAY_FOR_DM (data));

  /* Update the counters. */
  fs_counters_update (FS_FOP_READ, category, 1);

  result = base_dev->read_page (base_dev, page, data);
  if (failure == FS_FLASH_MUST_WORK && result != FS_DEVICE_RESULT_SUCCESS)
  {
    /* Read failures in NAND have several causes.  The most common are that
     * the page has not been written to since the last erase, or that power
     * was lost during the last write.  Because of the filesystem log,
     * there are only a few places in the filesystem where these are
     * expected to occur (such as reading the log), and these calls will
     * use FS_FLASH_FAIL_OK to distinguish this.  If we've gotten to this
     * failure, then the failure is due to a different cause that is
     * unexpected.
     *
     * Almost always, this indicates that the flash device driver is
     * malfunctioning.  It is possible to see this failure at end of life
     * for the part, but generally writes or erases fail first.  First line
     * of debugging is to figure out what is likely wrong with the flash
     * driver. */
    FS_ERR_FATAL ("Unexpected device read failure, page=%d", page, 0, 0);
  }
  EFS_BAB_END (EFS_BAB_FLASH_READ_PAGE);
  return result;
}

int
fs_device_read_iovec (efs_device_t base_dev, struct fs_iovec *iovec,
                      uint32 iovec_cnt, enum fs_flashop_category category,
                      enum fs_flash_failure failure)
{
  int result;

  fs_counters_update (FS_FOP_READ, category, iovec_cnt);

  result = base_dev->read_iovec (base_dev, iovec, iovec_cnt);
  if (failure == FS_FLASH_MUST_WORK && result != FS_DEVICE_RESULT_SUCCESS)
  {
    FS_ERR_FATAL ("Unexpected device read failure", 0, 0, 0);
  }

  return result;
}

int
fs_device_write_iovec (efs_device_t base_dev, struct fs_iovec *iovec,
                       uint32 iovec_cnt, enum fs_flashop_category category)
{
  fs_counters_update (FS_FOP_WRITE, category, iovec_cnt);
  return base_dev->write_iovec (base_dev, iovec, iovec_cnt);
}

int
fs_device_read_sectors (efs_device_t base_dev, uint32 sect, void *data,
                        uint32 num_sectors, enum fs_flashop_category category)
{
  fs_counters_update (FS_FOP_READ, category, num_sectors);
  return base_dev->read_sectors (base_dev, sect, data, num_sectors);
}

int
fs_device_write_sectors (efs_device_t base_dev, uint32 sect, void *data,
                        uint32 num_sectors, enum fs_flashop_category category)
{
  fs_counters_update (FS_FOP_WRITE, category, num_sectors);
  return base_dev->write_sectors (base_dev, sect, data, num_sectors);
}


int
fs_device_reliable_write_sectors (efs_device_t base_dev, uint32 sect,
                                  void *data, uint32 num_sectors,
                                  enum fs_flashop_category category)
{
  fs_counters_update (FS_FOP_WRITE, category, num_sectors);
  return base_dev->reliable_write_sectors (base_dev, sect, data, num_sectors);
}

uint32
fs_device_block_size (efs_device_t base_dev)
{
  return base_dev->block_size (base_dev);
}

uint32
fs_device_page_size (efs_device_t base_dev)
{
  return base_dev->page_size (base_dev);
}

uint32
fs_device_block_count (efs_device_t base_dev)
{
  return base_dev->block_count (base_dev);
}

int
fs_device_bad_block_check (efs_device_t base_dev, fs_block_id block)
{
  return base_dev->bad_block_check (base_dev, block);
}

int
fs_device_is_erased (efs_device_t base_dev, fs_page_id page)
{
  return base_dev->is_erased (base_dev, page);
}

int
fs_device_mark_block_bad (efs_device_t base_dev, fs_block_id block)
{
  return base_dev->mark_block_bad (base_dev, block);
}

int
fs_device_resume_erase (efs_device_t base_dev)
{
  return base_dev->resume_erase (base_dev);
}

int
fs_device_erase_status (efs_device_t base_dev)
{
  return base_dev->erase_status (base_dev);
}

int
fs_device_suspend_erase (efs_device_t base_dev)
{
  return base_dev->suspend_erase (base_dev);
}

int
fs_device_begin_erase_block (efs_device_t base_dev, fs_block_id block)
{
  return base_dev->begin_erase_block (base_dev, block);
}

int
fs_device_partial_write (efs_device_t base_dev, fs_page_id page,
    void *data, int offset, int length)
{
  /* Partial-writes are only possible on NOR-devices. NOR-devices allow
     partial-writes to a page provided if we are changing only 1's -> to 0's.
     Since we can write at any word in a page the buffer-pointer need not be
     aligned on a 32-byte address. */
  return base_dev->partial_write (base_dev, page, data, offset, length);
}

void *
fs_device_read_pointer (efs_device_t base_dev, fs_page_id page)
{
  return base_dev->read_pointer (base_dev, page);
}

