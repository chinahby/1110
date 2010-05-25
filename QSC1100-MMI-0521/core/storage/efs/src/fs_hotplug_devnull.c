/***********************************************************************
 * fs_hotplug_null.c
 *
 * Implement a small no-op device for hotplug
 * Copyright (C) 2006, 2008 QUALCOMM, Inc.
 *
 * It is sometimes useful to have a 'sink' device for benchmarking.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_devnull.c#3 $ $DateTime: 2008/12/12 11:11:10 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-12   rp    Added write_iovec, reliable write functions to hdev.
2008-11-24   yog   Added hotplug_success_stub for new timeout function.
2006-11-09   sch   Added extra write handler to the device table
2006-06-07   sh    Added reset stub
2006-06-06   sh    Create

===========================================================================*/

#include "customer.h"
#include "fs_hotplug.h"
#include "fs_hotplug_i.h"
#include "fs_hotplug_devnull.h"

#ifdef FS_HOTPLUG_DEVNULL

int
hotplug_devnull_get_size (struct hotplug_device *hdev,
                       uint32 *blocks,
                       uint16 *bytes_per_block)
{
  (void) hdev;

  /* Good question.. how big is the NULL device?  Let's say 1GB. */

  *bytes_per_block = 512;
  *blocks = 1 * 1024 * 1024 * 1024 / 512;
  return 0;
}

/*
 * This device is never "present" so that we don't try to mount it.
 */
int
hotplug_devnull_is_present (struct hotplug_device *hdev)
{
  (void) hdev;

  return 0;                     /* FALSE */
}

int
hotplug_devnull_read (struct hotplug_device *hdev,
                 uint32 lba, unsigned char *buf, uint16 n_to_read)
{
  (void) hdev;
  (void) lba;
  (void) buf;
  (void) n_to_read;

  return 0;                     /* Sure, done */
}


int
hotplug_devnull_write (struct hotplug_device *hdev,
                  uint32 lba, unsigned char *buf, uint16 n_to_write)
{
  (void) hdev;
  (void) lba;
  (void) buf;
  (void) n_to_write;

  return 0;                     /* No problem */
}

struct hotplug_dev_funcs hotplug_devnull_dev = {
  hotplug_fat_mount,
  hotplug_success_stub,         /* open */
  hotplug_success_stub,         /* close */
  hotplug_devnull_read,
  hotplug_devnull_write,
  hotplug_devnull_write,
  hotplug_no_erase,
  hotplug_devnull_get_size,
  hotplug_devnull_is_present,
  hotplug_success_stub,         /* format */
  hotplug_success_stub,         /* Reset */
  hotplug_success_stub,         /* Idle Dispatch Timeout */
  hotplug_no_dev_read_iovec,
  hotplug_no_dev_write_iovec,
  hotplug_no_dev_reliable_write,
  hotplug_no_dev_get_device_info
};
#else
extern int __dont_complain_about_empty_file;
#endif /* FS_HOTPLUG_DEVNULL */
