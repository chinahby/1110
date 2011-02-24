/***********************************************************************
 * fs_device_efs_mnand.c
 *
 * Device layer abstration for EFS on mNand device.
 * Copyright (C) 2008, 2009 QUALCOMM, Inc.
 *
 * Device layer abstration for EFS on mNand device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_device_efs_mnand.c#5 $ $DateTime: 2009/05/18 17:26:04 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-15   ebb   Updated blockdev_open call with dummy argument.
2009-05-08   ebb   Moved to block_device interface.
2008-12-16   yg    Use get_size function to get the size params of device.
2008-12-15   rp    Remove reference to obsolete flash APIs.
2008-12-12   rp    Create

===========================================================================*/
#include "fs_efs2_cust.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND

#include "fs_device_efs_mnand.h"
#include "fs_mnand_config.h"
#include "assert.h"
#include "fs_err.h"
#include "fs_sys_types.h"
#include "fs_paranoia.h"
#include <string.h>
#include "fs_util.h"
#include "fs_blockdev.h"

#ifdef FS_UNIT_TEST
#error code not present
#endif


#ifdef FS_UNIT_TEST
#error code not present
#else
  #define FS_DEVICE_EFS_MNAND_DEV_PATH      "/mmc1"
#endif


#define FS_DEVICE_EFS_MNAND_MAX_DEVICES            (1)


/* Max MNAND instances to encapsulate the MNAND-device. */
static struct efs_device fs_base_mnand_device[FS_DEVICE_EFS_MNAND_MAX_DEVICES];
static uint32 mnand_device_blk_cnt;
static uint16 mnand_device_blk_size_bytes;


void
fs_device_efs_mnand_init (void)
{
  memset (fs_base_mnand_device, 0, sizeof(fs_base_mnand_device));
  fs_paranoia_register (fs_base_mnand_device, sizeof(fs_base_mnand_device),
                        "mnand_dev");
}

efs_device_t
fs_device_efs_mnand_open (uint32 which_device)
{
  efs_device_t base_dev;
  struct block_device *bdev;
  int result;

  if (which_device >= FS_DEVICE_EFS_MNAND_MAX_DEVICES)
    FS_ERR_FATAL ("Max device limit exceeded...",0,0,0);

  bdev = blockdev_find_by_path (FS_DEVICE_EFS_MNAND_DEV_PATH);
  if (bdev == NULL)
  {
    return NULL;
  }

  /* Remove the device from the pool */
  blockdev_hide_device (bdev);

  result = blockdev_reset (bdev);
  if (result != 0)
  {
    return NULL;
  }

  /* Open block device. Use phys partition 0. */
  result = blockdev_open (bdev, NULL);
  if (result != 0)
  {
    return NULL;
  }

  result = blockdev_get_device_size (bdev, &mnand_device_blk_cnt,
                                     &mnand_device_blk_size_bytes);
  if (result != 0)
  {
    (void) blockdev_close (bdev);
    return NULL;
  }

  base_dev = &fs_base_mnand_device[which_device];
  base_dev->efs_device_type = FS_DEVICE_TYPE_EFS_MNAND;
  base_dev->priv.block_dev = bdev;
  fs_device_efs_mnand_init_ops (base_dev);

  return base_dev;
}

int
fs_device_efs_mnand_close (efs_device_t base_dev)
{
  int result;

  ASSERT (base_dev->priv.block_dev != NULL);

  result = blockdev_close (base_dev->priv.block_dev);
  ASSERT (result == 0);
  base_dev->priv.block_dev = NULL;
  return 0;
}

static int
fs_device_efs_mnand_read_iovec (efs_device_t base_dev, fs_iovec_t iovec,
                                uint32 iovec_cnt)
{
  int result;

  ASSERT (base_dev->priv.block_dev != NULL);
  result = blockdev_read_iovec (base_dev->priv.block_dev,
                                iovec, iovec_cnt);
  if (result != 0)
    FS_ERR_FATAL ("blockdev_read_iovec() failed...",0,0,0);

  return result;
}

static int
fs_device_efs_mnand_write_iovec (efs_device_t base_dev, fs_iovec_t iovec,
                                 uint32 iovec_cnt)
{
  int result;

  ASSERT (base_dev->priv.block_dev != NULL);
  result = blockdev_write_iovec (base_dev->priv.block_dev, iovec,
                                 iovec_cnt, 1);
  if (result != 0)
    FS_ERR_FATAL ("blockdev_write_iovec() failed...",0,0,0);

  return result;
}

static int
fs_device_efs_mnand_read_sectors (efs_device_t base_dev, fs_sector_id sect,
                                  void *data, uint32 num_sectors)
{
  int result;

  ASSERT (base_dev->priv.block_dev != NULL);
  result = blockdev_read (base_dev->priv.block_dev, sect,
                          data, (uint16)num_sectors);
  if (result != 0)
    FS_ERR_FATAL ("blockdev_read() failed...",0,0,0);

  return result;
}

static int
fs_device_efs_mnand_write_sectors (efs_device_t base_dev, fs_sector_id sect,
                                   void *data, uint32 num_sectors)
{
  int result;

  ASSERT (base_dev->priv.block_dev != NULL);
  result = blockdev_write (base_dev->priv.block_dev, sect,
                           data, (uint16)num_sectors);
  if (result != 0)
    FS_ERR_FATAL ("blockdev_write() failed...",0,0,0);

  return result;
}


static int
fs_device_efs_mnand_reliable_write_sectors (efs_device_t base_dev,
                            fs_sector_id sect, void *data, uint32 num_sectors)
{
  int result;

  ASSERT (base_dev->priv.block_dev != NULL);
  result = blockdev_reliable_write (base_dev->priv.block_dev, sect,
                                    data, num_sectors);
  if (result != 0)
    FS_ERR_FATAL ("blockdev_reliable_write() failed...",0,0,0);

  return result;
}


static int
fs_device_efs_mnand_read_page (efs_device_t base_dev, fs_page_id page,
                               void *data)
{
  int result;

  ASSERT (base_dev->priv.block_dev != NULL);
  result = blockdev_read (base_dev->priv.block_dev, page,
                          data, FS_MNAND_SECTORS_PER_CLUSTER);
  if (result != 0)
    FS_ERR_FATAL ("blockdev_read() failed...",0,0,0);

  return result;
}

static int
fs_device_efs_mnand_write_page (efs_device_t base_dev, fs_page_id page,
                                void *data)
{
  int result;

  ASSERT (base_dev->priv.block_dev != NULL);
  result = blockdev_write (base_dev->priv.block_dev, page,
                           data, FS_MNAND_SECTORS_PER_CLUSTER);
  if (result != 0)
    FS_ERR_FATAL ("blockdev_write() failed...",0,0,0);

  return result;
}

static char*
fs_device_efs_mnand_device_name (efs_device_t base_dev)
{
  (void) base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("_mnand_device_name",0,0,0);
  return NULL;
}

static int
fs_device_efs_mnand_device_maker_id (efs_device_t base_dev)
{
  (void) base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand_device_maker_id",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_device_id (efs_device_t base_dev)
{
  (void) base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand_device_id",0,0,0);
  return -1;
}

static uint32
fs_device_efs_mnand_block_count (efs_device_t base_dev)
{
  (void) base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  return mnand_device_blk_cnt;
}

static uint32
fs_device_efs_mnand_block_size (efs_device_t base_dev)
{
  (void) base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  return mnand_device_blk_size_bytes;
}

static uint32
fs_device_efs_mnand_page_size (efs_device_t base_dev)
{
  (void) base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  return FS_MNAND_SECTOR_SIZE;
}

static uint32
fs_device_efs_mnand_total_page_size (efs_device_t base_dev)
{
  (void) base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand total_page_size",0,0,0);
  return 0;
}

static uint32
fs_device_efs_mnand_device_type (efs_device_t base_dev)
{
  (void) base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand device_type",0,0,0);
  return 0;
}

static int
fs_device_efs_mnand_bad_block_check (efs_device_t base_dev, fs_block_id block)
{
  (void )base_dev; (void )block;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand bad_blk_chk",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_mark_block_bad (efs_device_t base_dev, fs_block_id block)
{
  (void )base_dev; (void )block;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand mard_block_bad",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_erase_block (efs_device_t base_dev, fs_block_id block)
{
  (void )base_dev; (void )block;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand erase_blk",0,0,0);
  return -1;
}

static void*
fs_device_efs_mnand_read_pointer (efs_device_t base_dev, fs_page_id page)
{
  (void )base_dev; (void )page;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand read_pointer",0,0,0);
  return NULL;
}

static int
fs_device_efs_mnand_is_erased (efs_device_t base_dev, fs_page_id page)
{
  (void )base_dev; (void )page;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand is_erased",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_partial_write (efs_device_t base_dev, fs_page_id page,
            void *data, int offset, int length)
{
  (void )base_dev; (void )page; (void )data; (void )offset; (void )length;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand partial_write",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_begin_erase_block (efs_device_t base_dev,
                                       fs_block_id block)
{
  (void )base_dev; (void )block;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand erase_block",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_suspend_erase (efs_device_t base_dev)
{
  (void )base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand suspend_erase",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_resume_erase (efs_device_t base_dev)
{
  (void )base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand resume_erase",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_erase_status (efs_device_t base_dev)
{
  (void )base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand erase_status",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_is_block_erased (efs_device_t base_dev, fs_block_id block)
{
  (void )base_dev; (void )block;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand is_block_erased",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_read_spare_bytes (efs_device_t base_dev, fs_page_id page,
           uint32 spare_offset, uint32 len, void *data)
{
  (void )base_dev; (void )page; (void )spare_offset; (void )len; (void )data;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand read_spare_bytes",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_read_mpages (efs_device_t base_dev, fs_page_id start_page,
                             fs_page_id num_pages,
                             void *data, void (*wdog_kick_fn_ptr)(void) )
{
  (void )base_dev; (void )start_page; (void )num_pages; (void )data;
  (void )wdog_kick_fn_ptr;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand read_mpages",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_copy_page (efs_device_t base_dev, fs_page_id src_page,
                           fs_page_id dest_page)
{
  (void )base_dev; (void )src_page; (void )dest_page;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand copy_page",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_lock_ops (efs_device_t base_dev, fs_page_id start_page,
                          fs_page_id end_page, uint32 opcode)
{
  (void )base_dev; (void )start_page; (void )end_page; (void )opcode;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand lock_ops",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_get_spare_corrected_bytes (efs_device_t base_dev)
{
  (void )base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand get_spare_corrected_bytes",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_write_page_and_udata (efs_device_t base_dev,
          fs_page_id page, void *page_data, void *spare_udata,
          uint32 spare_udata_blen)
{
  (void )base_dev; (void )page; (void )page_data; (void )spare_udata;
  (void )spare_udata_blen;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand write_page_and_udata",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_read_spare_udata (efs_device_t base_dev, fs_page_id page,
           void *spare_udata, uint32 spare_udata_blen)
{
  (void )base_dev; (void )page; (void )spare_udata; (void )spare_udata_blen;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand read_spare_udata",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_read_page_and_udata (efs_device_t base_dev,
           fs_page_id page, void *data, void *spare, uint32 spare_udata_blen)
{
  (void )base_dev; (void )page; (void )data; (void )spare;
  (void )spare_udata_blen;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand read_page_and_udata",0,0,0);
  return -1;
}

static int
fs_device_efs_mnand_open_partition (efs_device_t base_dev,
                                    char *partition_name)
{
  (void )base_dev; (void )partition_name;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand open_partition",0,0,0);
  return -1;
}

static void
fs_device_efs_mnand_set_ecc_state (efs_device_t base_dev, int state)
{
  (void )base_dev; (void )state;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand set_ecc_state",0,0,0);
}

static fs_device_write_style_t
fs_device_efs_mnand_get_write_style (efs_device_t base_dev)
{
  (void )base_dev;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand get_write_style",0,0,0);
  return (fs_device_write_style_t)0;
}


void
fs_device_efs_mnand_init_ops (efs_device_t base_dev)
{
  base_dev->device_name = fs_device_efs_mnand_device_name;
  base_dev->device_maker_id = fs_device_efs_mnand_device_maker_id;
  base_dev->device_id = fs_device_efs_mnand_device_id;
  base_dev->block_count = fs_device_efs_mnand_block_count;
  base_dev->block_size = fs_device_efs_mnand_block_size;
  base_dev->page_size = fs_device_efs_mnand_page_size;
  base_dev->total_page_size = fs_device_efs_mnand_total_page_size;
  base_dev->device_type = fs_device_efs_mnand_device_type;
  base_dev->bad_block_check = fs_device_efs_mnand_bad_block_check;
  base_dev->mark_block_bad = fs_device_efs_mnand_mark_block_bad;
  base_dev->write_page = fs_device_efs_mnand_write_page;
  base_dev->erase_block = fs_device_efs_mnand_erase_block;
  base_dev->read_pointer = fs_device_efs_mnand_read_pointer;
  base_dev->read_page = fs_device_efs_mnand_read_page;
  base_dev->is_erased = fs_device_efs_mnand_is_erased;
  base_dev->partial_write = fs_device_efs_mnand_partial_write;
  base_dev->begin_erase_block = fs_device_efs_mnand_begin_erase_block;
  base_dev->suspend_erase = fs_device_efs_mnand_suspend_erase;
  base_dev->resume_erase = fs_device_efs_mnand_resume_erase;
  base_dev->erase_status = fs_device_efs_mnand_erase_status;
  base_dev->is_block_erased = fs_device_efs_mnand_is_block_erased;
  base_dev->read_spare_bytes = fs_device_efs_mnand_read_spare_bytes;
  base_dev->read_mpages = fs_device_efs_mnand_read_mpages;
  base_dev->copy_page = fs_device_efs_mnand_copy_page;
  base_dev->lock_ops = fs_device_efs_mnand_lock_ops;
  base_dev->get_spare_corrected_bytes =
                        fs_device_efs_mnand_get_spare_corrected_bytes;
  base_dev->write_page_and_udata = fs_device_efs_mnand_write_page_and_udata;
  base_dev->read_spare_udata = fs_device_efs_mnand_read_spare_udata;
  base_dev->read_page_and_udata = fs_device_efs_mnand_read_page_and_udata;
  base_dev->open_partition = fs_device_efs_mnand_open_partition;
  base_dev->set_ecc_state = fs_device_efs_mnand_set_ecc_state;
  base_dev->get_write_style = fs_device_efs_mnand_get_write_style;

  base_dev->read_iovec = fs_device_efs_mnand_read_iovec;
  base_dev->write_iovec = fs_device_efs_mnand_write_iovec;
  base_dev->read_sectors = fs_device_efs_mnand_read_sectors;
  base_dev->write_sectors = fs_device_efs_mnand_write_sectors;
  base_dev->reliable_write_sectors =fs_device_efs_mnand_reliable_write_sectors;
}


int
fs_device_efs_mnand_get_attr (efs_device_t base_dev,
                          struct fs_device_attr *dev_attr)
{
  (void )base_dev; (void )dev_attr;
  ASSERT (base_dev->priv.block_dev != NULL);
  FS_ERR_FATAL ("mnand get_attr",0,0,0);
  return -1;
}

#endif /* FEATURE_EFS_EFS2_ON_MNAND */
