/***********************************************************************
 * fs_blockdev_devnull.c
 *
 * Implement a small no-op device for blockdev
 * Copyright (C) 2006, 2008 - 2009 QUALCOMM, Inc.
 *
 * It is sometimes useful to have a 'sink' device for benchmarking.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_blockdev_devnull.c#2 $ $DateTime: 2009/05/18 17:26:04 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-15   ebb   Updated open stub to take void * as second arg.
2009-05-08   ebb   Moved to blockdev interface.
2008-12-12   rp    Added write_iovec, reliable write functions to hdev.
2008-11-24   yog   Added hotplug_success_stub for new timeout function.
2006-11-09   sch   Added extra write handler to the device table
2006-06-07   sh    Added reset stub
2006-06-06   sh    Create

===========================================================================*/

#include "customer.h"
#include "fs_blockdev.h"
#include "fs_blockdev_i.h"

#ifdef FS_BLOCKDEV_DEVNULL

#include "fs_blockdev_devnull.h"
#define DEVNULL_DRIVER_VERSION      0x0001
#define DEVNULL_DEVICE_SIZE_IN_BLKS (1 * 1024 * 1024 * 1024 / 512) /* 1 GB */

static int
fs_devnull_get_size (struct block_device *bdev,
                     uint32 *blocks,
                     uint16 *bytes_per_block)
{
  (void) bdev;

  /* Good question.. how big is the NULL device?  */
  *bytes_per_block = 512;
  *blocks = DEVNULL_DEVICE_SIZE_IN_BLKS;
  return 0;
}

/*
 * This device is never "present" so that we don't try to mount it.
 */
static int
fs_devnull_is_present (struct block_device *bdev)
{
  (void) bdev;

  return 0;                     /* FALSE */
}

static int
fs_devnull_read (struct block_device *bdev,
                 uint32 lba, unsigned char *buf, uint16 n_to_read)
{
  (void) bdev;
  (void) lba;
  (void) buf;
  (void) n_to_read;

  return 0;                     /* Sure, done */
}


static int
fs_devnull_write (struct block_device *bdev,
                  uint32 lba, unsigned char *buf, uint16 n_to_write)
{
  (void) bdev;
  (void) lba;
  (void) buf;
  (void) n_to_write;

  return 0;                     /* No problem */
}

static int
fs_devnull_read_iovec (struct block_device *bdev,
                       fs_iovec_t iovec, uint32 iovec_cnt)
{
  (void) bdev; (void) iovec; (void) iovec_cnt;
  return -1;
}
static int
fs_devnull_write_iovec (struct block_device *bdev, fs_iovec_t iovec,
                        uint32 iovec_cnt, int reliable_write)
{
  (void) bdev; (void) iovec;
  (void) iovec_cnt; (void) reliable_write;
  return -1;
}
static int
fs_devnull_reliable_write (struct block_device *bdev,
                           uint32 lba, uint32 *buf, uint32 num_sectors)
{
  (void) bdev; (void) lba;
  (void) buf; (void) num_sectors;
  return -1;
}
static int
fs_devnull_erase (struct block_device *bdev, uint32 lba, uint16 n_to_erase)
{
  (void) bdev; (void) lba; (void) n_to_erase;
  return 0;
}
static int
fs_devnull_get_device_info (struct block_device *bdev,
                            struct block_device_info *info)
{
  (void) bdev; (void) info;
  return -1;
}

static int
fs_devnull_success_stub (struct block_device *bdev)
{
  (void) bdev;
  return 0;
}

static int
fs_devnull_open_stub (struct block_device *bdev, void *data)
{
  (void) bdev; (void) data;
  return 0;
}

static int
fs_devnull_fail_stub (struct block_device *bdev)
{
  (void) bdev;
  return -1;
}


static blockdev_ops devnull_ops =
{
  fs_devnull_open_stub,          /* open */
  fs_devnull_success_stub,       /* close */
  fs_devnull_read,               /* read */
  fs_devnull_write,              /* write */
  fs_devnull_write,              /* uwrite */
  fs_devnull_read_iovec,         /* read_iovec */
  fs_devnull_write_iovec,        /* write_iovec */
  fs_devnull_reliable_write,     /* reliable_write */
  fs_devnull_erase,              /* erase */
  fs_devnull_success_stub,       /* reset */
  fs_devnull_success_stub,       /* device_format */
  fs_devnull_is_present,         /* is_present */
  fs_devnull_get_size,           /* get_device_size */
  fs_devnull_get_device_info,    /* get_device_info */
  fs_devnull_success_stub,       /* set_device_idle */
  fs_devnull_fail_stub           /* get_device_idle */
};

/* Only one dev.null device neccesary ;) */
static struct block_device devices[] =
{
  {"/dev.null", &devnull_ops, NULL, NULL, BLOCKDEV_TYPE_DEVNULL, 0, 0xFF,
   CANARY}
};

#define NUM_DEVICES (sizeof (devices) / sizeof (devices[0]))

/* This should populate the linked list pointer passed in*/
static void
fs_devnull_init (struct block_device **head, uint16 *dev_id)
{
  int i;

  /* Link the list of devices together */
  for (i = NUM_DEVICES - 1; i >= 0; i--)
  {
    struct block_device *dev = &devices[i];

    /* Add device to beginning of the linked list */
    dev->next = *head;
    dev->device_id = (*dev_id)++;
    *head = dev;
  }
}

static void
fs_devnull_cleanup (void)
{
  /* Do nothing as of now */
}

/* Declare the driver for this module, this will be the only thing
 * exposed to the blockdev layer above. */
blockdev_driver blockdev_devnull_driver =
{
  "BDEV_DEVNULL_DRIVER",     /* char *name */
  BLOCKDEV_TYPE_DEVNULL,     /* block_device_type */
  DEVNULL_DRIVER_VERSION,    /* uint16 version */
  fs_devnull_init,              /* init fn */
  fs_devnull_cleanup            /* cleanup fn */
};

#else
extern int __dont_complain_about_empty_file;
#endif /* FS_BLOCKDEV_DEVNULL */
