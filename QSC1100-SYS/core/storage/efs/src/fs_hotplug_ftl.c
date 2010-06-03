/***********************************************************************
 * fs_hotplug_ftl.c
 *
 * Flash Translation Layer support for Hotplug
 * Copyright (C) 2006, 2007, 2008 QUALCOMM, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_ftl.c#4 $ $DateTime: 2008/12/12 11:11:10 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
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
#include "fs_hotplug.h"
#include "fs_hotplug_i.h"
#include "fs_hotplug_ftl.h"

#ifdef FS_HOTPLUG_FTL

#include "fs_ftl_cache.h"
#include "assert.h"
#include "fs_public.h"
#include "fs_ftl.h"

int
hotplug_ftl_get_size (struct hotplug_device *hdev, uint32* pblk_cnt,
                      uint16* ppg_size)
{
  if (FTL_CACHED_NO_DEVICE == hdev->fdev)
    return -1;

  return ftl_cache_get_size (hdev->fdev, pblk_cnt, ppg_size);
}


int
hotplug_ftl_is_present (struct hotplug_device *hdev)
{
  return (FTL_CACHED_NO_DEVICE != hdev->fdev);
}

/*
 * For Unit test, it's important to be able to 'reboot' the phone
 * and rescan the flash.  This function ensures that when hotplug
 * restarts, the FTL subsystem will also reinspect the flash
 * contents.
 */
int
hotplug_ftl_reset (struct hotplug_device *hdev)
{

  /* Initialization is done here since reset is called on bootup before
   * any other hotplug functions is called. If initialization is not
   * done here, but post poned till our first poll (which is 10 secs
   * later), then if some module like USB mass storage wants to lock
   * the device for exclusive access before FTL is initialized it would
   * not work. Also we do not want to initialize FTL in lock function
   * since the task in which lock is done will be unknown task.
   * So to keep the things consistent and simple we do initialize
   * FTL in here which happens on bootup. */

  /* Make sure that the device is not open, if its open, close it. */
  ASSERT (hdev->fdev == FTL_CACHED_NO_DEVICE);

  if (ftl_cache_init (hdev->driveno) != 0)
    return -1;

  hdev->fdev = ftl_cache_open (hdev->driveno);
  if (hdev->fdev == NULL)
    return -1;

  return 0;
}

int
hotplug_ftl_read (struct hotplug_device *hdev,
                  uint32 lba, unsigned char *buf, uint16 n_to_read)
{
  if (FTL_CACHED_NO_DEVICE == hdev->fdev)
    return -1;

  return ftl_cache_read (hdev->fdev, lba, buf, n_to_read);
}

int
hotplug_ftl_write (struct hotplug_device *hdev,
                   uint32 lba, unsigned char *buf, uint16 n_to_write)
{
  if (FTL_CACHED_NO_DEVICE == hdev->fdev)
    return -1;

  return ftl_cache_write (hdev->fdev, lba, buf, n_to_write);
}

int
hotplug_ftl_close (struct hotplug_device *hdev)
{
  (void) hdev;

  if (FTL_CACHED_NO_DEVICE == hdev->fdev)
    return -1;

  ftl_cache_force_sync_all ();

  return 0;
}

struct hotplug_dev_funcs hotplug_ftl_dev = {
  hotplug_fat_mount,
  hotplug_success_stub,         /* open */
  hotplug_ftl_close,            /* close */
  hotplug_ftl_read,
  hotplug_ftl_write,            /* write */
  hotplug_ftl_write,            /* write user data */
  hotplug_no_erase,
  hotplug_ftl_get_size,
  hotplug_ftl_is_present,
  hotplug_success_stub,         /* format_prep */
  hotplug_ftl_reset,
  hotplug_success_stub,         /* Idle Dispatch Timeout */
  hotplug_no_dev_read_iovec,
  hotplug_no_dev_write_iovec,
  hotplug_no_dev_reliable_write,
  hotplug_no_dev_get_device_info
};

#else
extern int __dont_complain_about_empty_file;
#endif /* FS_HOTPLUG_FTL */
