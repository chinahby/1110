/***********************************************************************
 * fs_blockdev.c
 *
 * Block device abstraction layer.
 * Copyright (C) 2008 - 2009 QUALCOMM, Inc.
 *
 * This file abstracts all block devices using a common interface. If
 * a new block device is to be added a driver must be written to implement
 * these common functions. The driver should then be added to the array
 * blockdev_driver_list.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_blockdev.c#3 $ $DateTime: 2009/05/20 15:11:31 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-15   ebb   Fix type mismatch on return value in get_device_id.
2009-05-15   ebb   Changed blockdev_open signature to take additional void *
2009-05-08   ebb   Create

===========================================================================*/

#include "fs_blockdev.h"
#include "fs_blockdev_i.h"

#include <string.h>
#include "assert.h"
#include "fs_counters.h"


/* Device drivers */
#include "fs_blockdev_sd.h"
#include "fs_blockdev_sd_cprm.h"
#include "fs_blockdev_usb.h"
#include "fs_blockdev_ftl.h"
#include "fs_blockdev_devnull.h"


/* Used to verify if the supplied block device is valid */
#define IS_INVALID_BDEV(dev)  (dev == NULL || dev->canary != CANARY)

/* This Blk dev ID will not match any internal blk dev ID.
 * None of the clients should really be using this Device ID, since
 * the offset could be changing and the clients should never be aware
 * of the offset. */
#define INVALID_BLK_DEV_ID    0xFFFFU;

/* Pointer to the head of all managed block devices */
static struct block_device *blockdev_head;

/* Driver list -
 * This is a list of all drivers currently supported. Each of these
 * will be called to be initialized at startup. */
static blockdev_driver *blockdev_driver_list[] =
{
#ifdef FS_BLOCKDEV_SD
  &blockdev_sd_driver,
#endif

#ifdef FS_BLOCKDEV_SD_CPRM
  &blockdev_sd_cprm_driver,
#endif

#ifdef FS_BLOCKDEV_FTL
  &blockdev_ftl_driver,
#endif

#ifdef FS_BLOCKDEV_USB
  &blockdev_usb_driver,
#endif

#ifdef FS_BLOCKDEV_DEVNULL
  &blockdev_devnull_driver,
#endif

  NULL
};

#define NUM_DRIVERS (int)(sizeof (blockdev_driver_list) / \
                          sizeof(blockdev_driver_list[0]))

void
blockdev_init (void)
{
  int i;
  uint16 dev_id = 0;
  blockdev_head = NULL;

  /* Step through list and initialize all drivers. Each driver will
   * add their device nodes to the linked list passed in. At the end
   * of enumeration this should contain all possible block devices
   * in the system. We walk through the list backwards so that the linked list
   * gets created in same order as the drivers are declared */
  for (i = NUM_DRIVERS - 2; i >= 0; i--)
  {
    blockdev_driver *driver = blockdev_driver_list[i];

    /* Let driver initialize and add to device list */
    driver->init (&blockdev_head, &dev_id);
  }
}


struct block_device *
blockdev_find_by_path (char *path)
{
  struct block_device *dev;

  for (dev = blockdev_head; dev != NULL; dev = dev->next)
  {
    if (IS_INVALID_BDEV (dev))
      return NULL;

    if (strcmp (dev->mount_point, path) == 0)
      return dev;
  }

  /* Couldn't find the device */
  return NULL;
}


/* This function assumes all devices of the same type are contiguous
 * in the linked list pointed to by blockdev_head. Any hidden block
 * devices will be skipped */
struct block_device *
blockdev_find_by_type (block_device_type type, struct block_device *last)
{
  struct block_device *cur;

  /* If last = NULL start from the beginning */
  if (last == NULL)
    cur = blockdev_head;
  else
    cur = last->next;

  /* Iterate through device list */
  while (cur != NULL)
  {
    if (IS_INVALID_BDEV (cur))
      return NULL;

    /* Does type match and not hidden? */
    if ((cur->type == type || type == BLOCKDEV_TYPE_ALL) &&
        !cur->hidden)
      return cur;

    /* Skip to next device */
    cur = cur->next;
  }

  /* At the end of the list */
  return NULL;

}

struct block_device *
blockdev_find_by_device_id (uint16 device_id)
{
  struct block_device *dev;

  for (dev = blockdev_head; dev != NULL; dev = dev->next)
  {
    if (IS_INVALID_BDEV (dev))
      return NULL;

    if (dev->device_id == device_id)
      return dev;
  }

  /* Couldn't find the device */
  return NULL;
}

int
blockdev_hide_device (struct block_device *dev)
{
  int was_hidden;

  if (IS_INVALID_BDEV (dev))
    return -1;

  was_hidden = dev->hidden;
  dev->hidden = 1;

  return was_hidden;
}

int
blockdev_unhide_device (struct block_device *dev)
{
  int was_hidden;

  if (IS_INVALID_BDEV (dev))
      return -1;

  was_hidden = dev->hidden;
  dev->hidden = 0;
  return was_hidden;
}

const char *
blockdev_get_name (struct block_device *dev)
{
  if (IS_INVALID_BDEV (dev))
    return "Invalid device";

  return dev->mount_point;
}

block_device_type
blockdev_get_type (struct block_device *dev)
{
  if (IS_INVALID_BDEV (dev))
    return BLOCKDEV_TYPE_INVALID;

  return dev->type;
}

/* Call the underlying driver function for anything device related */
int
blockdev_open (struct block_device *bdev, void *data)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->open (bdev, data);
}

int
blockdev_close (struct block_device *bdev)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->close (bdev);
}

int
blockdev_read (struct block_device *bdev, uint32 lba, unsigned char *buf,
               uint16 block_cnt)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  fs_counter.blockdev_read_sector_counter += block_cnt;
  return bdev->ops->read (bdev, lba, buf, block_cnt);
}

int
blockdev_write (struct block_device *bdev, uint32 lba, unsigned char *buf,
                uint16 block_cnt)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  fs_counter.blockdev_write_sector_counter += block_cnt;
  return bdev->ops->write (bdev, lba, buf, block_cnt);
}

int
blockdev_reliable_write (struct block_device *bdev, uint32 lba,
                         uint32 *buf, uint16 block_cnt)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  fs_counter.blockdev_reliable_write_sector_counter += block_cnt;
  return bdev->ops->reliable_write (bdev, lba, buf, block_cnt);
}

int
blockdev_uwrite (struct block_device *bdev, uint32 lba, unsigned char *buf,
                 uint16 block_cnt)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  fs_counter.blockdev_write_sector_counter += block_cnt;
  return bdev->ops->uwrite (bdev, lba, buf, block_cnt);
}

int
blockdev_erase (struct block_device *bdev, uint32 lba, uint16 block_cnt)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->erase (bdev, lba, block_cnt);
}

int
blockdev_reset (struct block_device *bdev)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->reset (bdev);
}

int
blockdev_is_present (struct block_device *bdev)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->is_present (bdev);
}

int
blockdev_get_device_info (struct block_device *bdev,
                          struct block_device_info *dev_info)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->get_device_info (bdev, dev_info);
}

int
blockdev_read_iovec (struct block_device *bdev, fs_iovec_t iovec,
                     uint32 iovec_cnt)
{
  uint32 i;
  if (IS_INVALID_BDEV (bdev))
    return -1;

  for (i = 0; i < iovec_cnt; i++)
    fs_counter.blockdev_read_sector_counter += (EFS_PAGE_SIZE /
                                                FS_SDCC_SECTOR_SIZE_IN_BYTES);
  return bdev->ops->read_iovec (bdev, iovec, iovec_cnt);
}

int
blockdev_write_iovec (struct block_device *bdev, fs_iovec_t iovec,
                      uint32 iovec_cnt, int reliable_write)
{
  uint32 i;
  if (IS_INVALID_BDEV (bdev))
    return -1;

  for (i = 0; i < iovec_cnt; i++)
    fs_counter.blockdev_write_sector_counter += (EFS_PAGE_SIZE /
                                                 FS_SDCC_SECTOR_SIZE_IN_BYTES);

  return bdev->ops->write_iovec (bdev, iovec, iovec_cnt, reliable_write);
}

int
blockdev_device_format (struct block_device *bdev)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->device_format (bdev);
}

int
blockdev_get_device_size (struct block_device *bdev, uint32 *blocks,
                          uint16 *bytes_per_block)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->get_device_size (bdev, blocks, bytes_per_block);
}

int
blockdev_set_device_idle (struct block_device *bdev)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->set_device_idle (bdev);
}

int
blockdev_get_device_idle (struct block_device *bdev)
{
  if (IS_INVALID_BDEV (bdev))
    return -1;

  return bdev->ops->get_device_idle (bdev);
}

uint16
blockdev_get_device_id (struct block_device *bdev)
{
  if (IS_INVALID_BDEV (bdev))
    return INVALID_BLK_DEV_ID;

  return bdev->device_id;
}
