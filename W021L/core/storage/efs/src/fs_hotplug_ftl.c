/***********************************************************************
 * fs_blockdev_ftl.c
 *
 * Flash Translation Layer support for Hotplug
 * Copyright (C) 2006 - 2009 QUALCOMM, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_ftl.c#6 $ $DateTime: 2009/05/18 17:26:04 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-15   ebb   Updated open stub signature to take additional void *.
2009-05-08   ebb   Migrated to blockdev from hotplug.
2008-12-12   rp    Added write_iovec, reliable write functions to hdev.
2008-11-24   yog   Added hotplug_success_stub for new timeout function.
2008-05-27   yg    Call ftl_cache_init in reset.
2008-04-28   np    Add support for the FTL interface changes.
2007-12-11   yg    FTL initialization code moved to boot time reset func.
2006-11-09   sch   Added extra write handler to the device table
2006-08-28   yg    Added reset handler
2006-06-02   s h   Added reset handler
2006-05-29   sh    Stub format function
2006-05-03   sh    Create

===========================================================================*/

#include "customer.h"
#include "fs_blockdev_i.h"

#ifdef FS_BLOCKDEV_FTL

#include "fs_blockdev_ftl.h"
#include "fs_ftl_cache.h"
#include "assert.h"
#include "fs_public.h"
#include "fs_ftl.h"
#include "fs_err.h"

#define FTL_DRIVER_VERSION         0x0001

static int
fs_ftl_get_size (struct block_device *bdev, uint32* pblk_cnt, uint16* ppg_size)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;
  if (FTL_CACHED_NO_DEVICE == fdev->handle)
    return -1;

  return ftl_cache_get_size (fdev->handle, pblk_cnt, ppg_size);
}


static int
fs_ftl_is_present (struct block_device *bdev)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;
  return (FTL_CACHED_NO_DEVICE != fdev->handle);
}

/*
 * For Unit test, it's important to be able to 'reboot' the phone
 * and rescan the flash.  This function ensures that when blockdev
 * restarts, the FTL subsystem will also reinspect the flash
 * contents.
 */
static int
fs_ftl_reset (struct block_device *bdev)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;

  /* Initialization is done here since reset is called on bootup before
   * any other blockdev functions is called. If initialization is not
   * done here, but post poned till our first poll (which is 10 secs
   * later), then if some module like USB mass storage wants to lock
   * the device for exclusive access before FTL is initialized it would
   * not work. Also we do not want to initialize FTL in lock function
   * since the task in which lock is done will be unknown task.
   * So to keep the things consistent and simple we do initialize
   * FTL in here which happens on bootup. */

  /* Make sure that the device is not open, if its open, close it. */
  ASSERT (fdev->handle == FTL_CACHED_NO_DEVICE);

  if (ftl_cache_init (fdev->driveno) != 0)
    return -1;

  fdev->handle = ftl_cache_open (fdev->driveno);
  if (fdev->handle == NULL)
    return -1;

  return 0;
}

static int
fs_ftl_read (struct block_device *bdev, uint32 lba, unsigned char *buf,
             uint16 n_to_read)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;

  if (FTL_CACHED_NO_DEVICE == fdev->handle)
    return -1;

  return ftl_cache_read (fdev->handle, lba, buf, n_to_read);
}

static int
fs_ftl_write (struct block_device *bdev, uint32 lba, unsigned char *buf,
              uint16 n_to_write)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;
  if (FTL_CACHED_NO_DEVICE == fdev->handle)
    return -1;

  return ftl_cache_write (fdev->handle, lba, buf, n_to_write);
}

static int
fs_ftl_close (struct block_device *bdev)
{
  ftl_device_data *fdev = (ftl_device_data *)bdev->device_data;

  if (FTL_CACHED_NO_DEVICE == fdev->handle)
    return -1;

  ftl_cache_force_sync_all ();

  return 0;
}

/* Non-implemented stub functions */
static int
fs_ftl_success_stub (struct block_device *bdev)
{
  (void) bdev;
  return 0;
}

static int
fs_ftl_fail_stub (struct block_device *bdev)
{
  (void) bdev;
  return -1;
}

static int
fs_ftl_open_stub (struct block_device *bdev, void *data)
{
  (void) bdev; (void) data;
  return 0;
}

static int
fs_ftl_read_iovec (struct block_device *bdev, fs_iovec_t iovec,
                   uint32 iovec_cnt)
{
  (void) bdev; (void) iovec; (void) iovec_cnt;
  return -1;
}
static int
fs_ftl_write_iovec (struct block_device *bdev, fs_iovec_t iovec,
                    uint32 iovec_cnt, int reliable_write)
{
  (void) bdev; (void) iovec;
  (void) iovec_cnt; (void) reliable_write;
  return -1;
}
static int
fs_ftl_reliable_write (struct block_device *bdev, uint32 lba, uint32 *buf,
                       uint32 num_sectors)
{
  (void) bdev; (void) lba;
  (void) buf; (void) num_sectors;
  return -1;
}
static int
fs_ftl_erase_fail (struct block_device *bdev, uint32 lba, uint16 n_to_erase)
{
  (void) bdev; (void) lba; (void) n_to_erase;
  return -1;
}
static int
fs_ftl_get_device_info (struct block_device *bdev,
                        struct block_device_info *info)
{
  (void) bdev; (void) info;
  return -1;
}



static blockdev_ops ftl_ops =
{
  fs_ftl_open_stub,          /* open */
  fs_ftl_close,              /* close */
  fs_ftl_read,               /* read */
  fs_ftl_write,              /* write */
  fs_ftl_write,              /* uwrite */
  fs_ftl_read_iovec,         /* read_iovec */
  fs_ftl_write_iovec,        /* write_iovec */
  fs_ftl_reliable_write,     /* reliable_write */
  fs_ftl_erase_fail,         /* erase */
  fs_ftl_reset,              /* reset */
  fs_ftl_success_stub,       /* device_format */
  fs_ftl_is_present,         /* is_present */
  fs_ftl_get_size,           /* get_device_size */
  fs_ftl_get_device_info,    /* get_device_info */
  fs_ftl_success_stub,       /* set_device_idle */
  fs_ftl_fail_stub
};

/* Private storage for ftl devices */
static ftl_device_data ftl_dev_data[] =
{
  {0, FTL_CACHED_NO_DEVICE},  /* driveno, FTL handle */
  {1, FTL_CACHED_NO_DEVICE}
};

/* Only one dev.null device neccesary ;) */
static struct block_device devices[] =
{
  {"/ftl1", &ftl_ops, (void *)&ftl_dev_data[0], NULL,
   BLOCKDEV_TYPE_FTL, 0, 0xFF, CANARY},
  {"/ftl2", &ftl_ops, (void *)&ftl_dev_data[1], NULL,
   BLOCKDEV_TYPE_FTL, 0, 0xFF, CANARY}
};

#define NUM_DEVICES (sizeof (devices) / sizeof (devices[0]))

/* This should populate the linked list pointer passed in*/
static void
ftl_init (struct block_device **head, uint16 *dev_id)
{
  int i;

  /* Check to ensure the number of private data entries matches
   * the number of devices. */
  if (NUM_DEVICES != sizeof (ftl_dev_data) / sizeof (ftl_dev_data[0]))
    FS_ERR_FATAL ("Number of block devices MUST match private data!", 0,0,0);

  /* Link the list of devices together */
  for (i = NUM_DEVICES - 1; i >= 0 ; i--)
  {
    struct block_device *dev = &devices[i];
    ftl_device_data *fdev = (ftl_device_data *)dev->device_data;

    /* Add device to beginning of the linked list */
    dev->next = *head;
    dev->device_id = (*dev_id)++;
    fdev->handle = FTL_CACHED_NO_DEVICE;
    *head = dev;
  }
}

static void
fs_ftl_cleanup (void)
{
  /* Do nothing as of now */
}


/* Declare the driver for this module, this will be the only thing
 * exposed to the blockdev layer above. */
blockdev_driver blockdev_ftl_driver =
{
  "BDEV_FTL_DRIVER",  /* char *name */
  BLOCKDEV_TYPE_FTL,  /* block_device_type */
  FTL_DRIVER_VERSION, /* uint16 version */
  ftl_init,           /* init fn */
  fs_ftl_cleanup      /* cleanup fn */
};


#else
extern int __dont_complain_about_empty_file;
#endif /* FS_BLOCKDEV_FTL */
