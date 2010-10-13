/***********************************************************************
 * fs_hotplug.c
 *
 * EFS2 hotplug manager.
 * Copyright (C) 2005-2009 QUALCOMM, Inc.
 *
 * Auto mount/umount hot-insertable devices in EFS2.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug.c#20 $ $DateTime: 2009/07/06 18:43:01 $ $Author: ebuller $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-07-06   ebb   Converted the ERR_FATAL calls to FS_ERR_FATAL.
2009-06-12   ebb   Moved rex_xxx calls to fs_osal.h abstraction.
2009-06-10   yog   Don't call blockdev_xxx_dev_idle for state DEV_UNUSABLE.
2009-05-15   ebb   Updated blockdev_open call to use NULL for phys partiton.
2009-05-08   ebb   Separate hotplug from block device driver.
2009-04-01   ebb   Added memory protect/unprotect calls around checkfat.
2009-01-26   ebb   Added stubs to remove dependency of fs_diag on hotplug.
2008-12-16   yg    Code change to count for modified info structure.
2008-12-12   rp    Added write_iovec, reliable write functions to hdev.
2008-12-11   rp    Fix compiler error in stub set_idle_device_event_duration.
2008-12-06   ebb   Fix hotplug_checkfat for hfat_getstatus removal.
2008-11-24   yog   Added hotplug_idle_device_dispatch ().
2008-11-06   sh    Update ZAP macros to Babylon macros
2008-11-03   rp    Introduce device abstraction.
2008-09-19   rp    Corrected the featurization of the hotplug_format().
2008-09-19   wek   Featurize hotplug_format().
2008-09-04   ebb   Added HOTPLUG_LOCK_INIT() to hotplug_init.
2008-05-30   ebb   Added callback to hotplug_checkfat.
2008-05-27    yg   Remove call to ftl_cache_init.
2008-05-05   ebb   Added hotplug_checkfat interface.
2008-04-28    np   Added support for ftl interface change.
2008-02-28    yg   Do not return unusable devices during enumeration.
2007-11-28    rp   Corrected the multiple SD slots feature.
2007-11-09    rp   Revert multi-sd card change as 7500-target has issues.
2007-10-09   sch   Added support for multiple SD devices
2007-07-31    sh   Make hotplug_force_poll_now() static.
2007-06-04    sh   Pull notification structures out of HOTPLUG-only region.
2007-04-26    rp   Fixed compiler warning due to hotplug_notify_unregister().
2007-04-08    sh   Validate hdev pointers from external calls.
2007-04-06    sh   Hide unrequested notification events.
2007-04-06    rp   Force a poll whenever we hear anything from USB callback.
2007-04-05    sh   Comment and cleanup.
2007-04-02    rp   Added hotplug event notification.
2006-11-09   sch   Added implementation of hotplug_dev_write_udata
2006-11-09    sh   Removed remote hotplug altogether.
2006-10-17    sh   Always poll from hotplug task, and on media change
2006-10-17    sh   Count hotplug read/write operations for benchmarking.
2006-10-16    sh   Change lock/unlock functions to return int status.
2006-10-03    sh   Fixed some ZPRINT line endings.
2006-09-20    sh   Renamed TYPE_ values to have HOTPLUG prefix.
2006-09-10    sh   Removed prohibited %s from MSG calls.
2006-09-06    sh   Lint cleanup
2006-08-27    sh   Allow HFAT or SFAT calls for formatting.
2006-06-28    sh   Hold FS_GLOBAL lock only for SD's is_present()
2006-06-25    sh   Initialize was_present instead of sd_present
2006-06-12    sh   Fix the return status for fat_format.
2006-06-06    sh   Added NULL device
2006-06-05    sh   Added more feature flag safeguards
2006-06-02    sh   Added reset handler
2006-05-25    sh   Removed redundant nodev mount from fat_mount
2006-05-25    sh   Simplified HOTPLUG vs COLDPLUG switches
2006-05-23   sch   Removed device open from hotplug_unlock_dev
2006-05-23    sh   Added FTL to hotplug.
2006-05-16    sh   Added hotplug_dev_is_mounted()
2006-05-13    sh   Fixed error in debugging print
2006-05-11    sh   Renamed efs_hotplug_... to hotplug_...
2006-05-10    sh   Moved common 'format' up one level.
2006-05-10    sh   Replaced DRV_GEOMETRY_DESC with block size & count
2006-05-10    sh   Added device access functions
2006-05-08    sh   Corrected typo in ZPRINTF debug message
2006-05-08    sh   Leave mountpoint directories present as NODEV
2006-05-05    sh   Allow target override of default poll interval
2006-05-04    sh   Close device on lock_dev, and open it for mount
2006-05-03    sh   Added FTL to Hotplug
2006-05-03    sh   Made lock_card device-specific
2006-04-22    sh   Moved all private definitions into fs_hotplug_i.h
2006-04-18    sh   Poll again immediately if any chages were detected
2006-04-17    sh   Renamed lock/unlock card functions.
2006-04-17   nrs   Moved is_mounted to fs_hotplug_remote
2006-04-14    sh   Added more debug messages.
2006-04-14    sh   Code review feedback implemented.
2006-04-13    sh   Allow compilation without USB Mass Storage
2006-03-31   sch   Modified hotplug_format to ignore leading slash
2006-03-31    sh   USB unmount
2006-03-31    sh   Lint cleanup
2006-03-28   nrs   Fix typo in unmount code
2006-03-28   sch   Use table-based hotplug.
2006-03-10   sch   Added USB mass storage support
2006-03-21   nrs   Add multi-processor support
2006-01-19   nrs   Allocate SFAT memory structures if using MMC
2006-01-10   sh    Fix Compiler warning again
2005-03-10   sch   Added USB mass storage support
2005-11-18   nrs   Fix Compiler warning
2005-10-21   dlb   Support cardreader mode.
2005-09-27   dlb   Mount to hidden dir and remount.
2005-09-20   dlb   Create

===========================================================================*/

#include "customer.h"

#include "msg.h"
#include "fs_osal.h"
#include "assert.h"
#include "task.h"
#include "string.h"
#include "jzap.h"

#include "fs_err.h"
#include "fs_efs2.h"
#include "fs_public.h"
#include "fs_global_lock.h"
#include "fs_benchmark.h"
#include "fs_hotplug.h"
#include "fs_hotplug_i.h"

#include "fs_blockdev.h"
#include "fs_hotplug_usbhost.h"

#include "fs_ext_hfat.h"
#include "fs_util.h"

#include "hfat_udefs_f.h"
#include "hfat_api_f.h"

#ifdef FS_UNIT_TEST
#error code not present
#endif

/*
 * The remainder of this file is needed only if Hotplug is enabled
 */
#if defined FEATURE_EFS_HOTPLUG || defined FEATURE_EFS_COLDPLUG

#ifdef FEATURE_EFS_HOTPLUG
/* Hotplug has its own task to asynchronously poll devices */
fs_os_tcb_type fs_hotplug_tcb;
static void fs_hotplug_task (dword arg);

#define FS_HOTPLUG_STACK_SIZE 2048 /* (needs a measured reduction) */
static unsigned char fs_hotplug_stack [FS_HOTPLUG_STACK_SIZE];

/* Timer to trigger periodic poll events */
static fs_os_timer_type fs_hotplug_poll_timer;

/* Timer to trigger periodic idle device check event.*/
static fs_os_timer_type fs_hotplug_idle_device_timer;

/* The interval time for the periodic hotplug polls */
static fs_os_timer_cnt_type hotplug_poll_interval;

/* The interval time for the idle device event generation in millisecond. */
static uint32 hotplug_idle_device_evt_duration;

/* Should hotplug be polling?  This is the external polling enable state.
 * This allows polling to be started and stopped by external control. */
static volatile int polling_enabled;

#endif /* FEATURE_EFS_HOTPLUG */

/* This structure represents one instance of a registered Hotplug
 * Notification client. */
struct hotplug_notify_client
{
  hotplug_dev_type                device_type;
  hotplug_notify_event            event_mask;
  hotplug_notify_event_cb         user_cb;
  void                            *user_data;
};

/* The whole set of registered clients is in this array */
static struct hotplug_notify_client
  hotplug_notify_client_tbl[FS_HOTPLUG_NOTIFY_MAX_CLIENTS];

#ifndef FS_MAX_HOTPLUG_DEVICES
  #define FS_MAX_HOTPLUG_DEVICES 20
#endif

/* List of all devices hotplug is managing */
static struct hotplug_device hotplug_device_list[FS_MAX_HOTPLUG_DEVICES];
static unsigned int hotplug_num_devices = 0;

/*
 * Static prototypes
 */
static int hotplug_force_directory (const char *dirname);
static void hotplug_nodev_mount (struct hotplug_device *hdev);
static int hotplug_umount (struct hotplug_device *hdev);
static void hotplug_poll_live_device (struct hotplug_device *hdev);
static void hotplug_notify_events (struct hotplug_device *hdev,
                                   hotplug_notify_event event_id);

/* We need a Hotplug lock to make sure the hotplug data structures are
 * only accessed by one thread at a time.   Any access to the table
 * should only take place with the lock held.  */
#ifdef FS_STANDALONE
  #define HOTPLUG_LOCK_INIT()
  #define HOTPLUG_LOCK()
  #define HOTPLUG_UNLOCK()
#else
  static fs_os_crit_sect_type hotplug_crit_sect;
  #define HOTPLUG_LOCK_INIT() fs_os_init_crit_sect  (&hotplug_crit_sect)
  #define HOTPLUG_LOCK()      fs_os_enter_crit_sect (&hotplug_crit_sect)
  #define HOTPLUG_UNLOCK()    fs_os_leave_crit_sect (&hotplug_crit_sect)
#endif


/* On-target, the CPRM partition shares a physical device number with
 * the user portion of the SD card, and the CPRM calls are distinct.
 * Off-target, where we use the same calls for both, we use a
 * different devno to tell them apart. */
#ifdef FS_UNIT_TEST
#error code not present
#else
  #define CPRM_DEVNO 0
#endif

/* Return the hotplug device that contains the block device
 * with the associated device_id */
struct hotplug_device *
hotplug_hdev (int16 device_id)
{
  unsigned int i;

  for (i = 0; i < hotplug_num_devices; i++)
  {
    struct hotplug_device *hdev = &hotplug_device_list[i];

    if (blockdev_get_device_id (hdev->bdev) == device_id)
      return hdev;
  }
  return NULL;
}

/* Ensure that a directory of this name exists or is created */
static int
hotplug_force_directory (const char *dirname)
{
  /* Kill any regular file that stands in our way. */
  (void) efs_unlink (dirname);

  /* Try to make the new directory.
   * An existing directory of the same name is okay.
   * We will just mount over it, hiding the contents. */
  if ((efs_mkdir (dirname, 0755) != 0)
      && (efs_errno != EEXIST))
  {
    /* Failed to make the directory.  Admit defeat. */
    ZPRINTF ("Cannot make directory %s\n\r", dirname);
    return -1;
  } else {
    return 0;                   /* Directory good */
  }
}

/* To ensure that no process creates files in EFS2 under the hotplug
 * mountpoints, we mount each of them to the 'nodev' filesystem until
 * a device is available.   This should remain the default state until
 * something useable is mounted.
 *
 * This function must succeed, or something is very wrong. */
static void
hotplug_nodev_mount (struct hotplug_device *hdev)
{
  if (efs_mount ("", blockdev_get_name (hdev->bdev), "nodev", 0, NULL) != 0) {
    ZPRINTF ("Mount of %s to nodev failed!\n\r",
             blockdev_get_name (hdev->bdev));
    FS_ERR_FATAL ("Could not prepare hotplug directory %s",
               blockdev_get_name (hdev->bdev), 0, 0);
  }
}

/*
 * Unmount the NODEV fs that was holding this inactive directory
 * This is necessary so that we can remount a different filesystem
 * there when ready.
 */
static void
hotplug_nodev_umount (struct hotplug_device *hdev)
{
  if (efs_umount (blockdev_get_name (hdev->bdev)))  /* Should not fail */
    FS_ERR_FATAL ("Failed to unmount NODEV from %s!\n\r",
               blockdev_get_name (hdev->bdev),0,0);
}

/*
 * Verify that the hdev pointer is one of our own.
 */
static int
hotplug_hdev_is_valid (struct hotplug_device *hdev)
{
  unsigned int i;

  for (i = 0; i < hotplug_num_devices; i++)
  {
    if (hdev == hotplug_device_list + i)
      return 1;
  }
  return 0;                     /* No match! */
}

/* When we test the supplied 'hdev' argument to the public API calls,
 * it's debatable whether we should ASSERT on invalid pointer, or
 * handle it by returning an error status.
 *
 * We use ASSERT here because a bad hdev pointer indicates a severely
 * malfunctioning caller -- this needs to be debugged.  Also, these
 * are compiled out of most production code. */
#define HDEV_VALIDATE(x)  ASSERT(hotplug_hdev_is_valid (x)) /* NO semicolon */

int
hotplug_fat_mount_core (struct hotplug_device *hdev)
{
  char drive_letter[3];

  drive_letter[0] = 'a' + blockdev_get_device_id (hdev->bdev);
  drive_letter[1] = ':';
  drive_letter[2] = '\0';

  /* Now mount the FAT filesystem there */
  if (efs_mount ("0", blockdev_get_name (hdev->bdev), "extfs", 0,
                 drive_letter) != 0) {
    ZPRINTF ("Mount of %s to %s failed!\n\r",
             blockdev_get_name (hdev->bdev), drive_letter);
    return -1;
  }
  return 0;
}

/* Mount a path using the FAT conventions.
 * (Call only while holding the HOTPLUG_LOCK)
 */
int
hotplug_fat_mount (struct hotplug_device *hdev)
{
  HDEV_VALIDATE (hdev);
  ASSERT (hdev->dev_state == DEV_UNMOUNTED);
  ZPRINTF ("hotplug_fat_mount(%s)\n\r", blockdev_get_name (hdev->bdev));

  if (hotplug_fat_mount_core (hdev) != 0)
    return -1;

  hdev->dev_state = DEV_MOUNTED;

  /* notify mount event */
  hotplug_notify_events (hdev, HOTPLUG_NOTIFY_EVENT_MOUNTED);

  return 0;
}


/*
 * Unmount the hotplug filesystem and leave a NODEV filesystem in its
 * place.
 */
static int
hotplug_umount (struct hotplug_device *hdev)
{
  int status = 0;

  ASSERT (hdev->dev_state == DEV_MOUNTED);
  ZPRINTF ("hotplug_umount(%s)\n\r", blockdev_get_name (hdev->bdev));

  /* Unmount the filesystem */
  if (efs_umount (blockdev_get_name (hdev->bdev)))  /* Should not fail */
    status = -1;

  /* Remount it to nodev.  If the umount failed above, this might be fatal */
  hotplug_nodev_mount (hdev);

  hdev->dev_state = DEV_UNMOUNTED; /* Regardless... */

  /* notify unmount event */
  hotplug_notify_events (hdev, HOTPLUG_NOTIFY_EVENT_UNMOUNTED);

  return status;
}

/* Resolve internal block_device_type to advertised hotplig_dev_type */
static hotplug_dev_type
blockdev_to_hotplug_type (block_device_type type)
{
  hotplug_dev_type btype = HOTPLUG_TYPE_INVALID;

  switch (type)
  {
    case BLOCKDEV_TYPE_INVALID:
      btype = HOTPLUG_TYPE_INVALID;
      break;
    case BLOCKDEV_TYPE_MMC:
      btype = HOTPLUG_TYPE_MMC;
      break;
    case BLOCKDEV_TYPE_REMOTE:
      btype = HOTPLUG_TYPE_REMOTE;
      break;
    case BLOCKDEV_TYPE_USB_MS:
      btype = HOTPLUG_TYPE_USB_MS;
      break;
    case BLOCKDEV_TYPE_DEVNULL:
      btype = HOTPLUG_TYPE_DEVNULL;
      break;
    case BLOCKDEV_TYPE_FTL:
      btype = HOTPLUG_TYPE_FTL;
      break;
    case BLOCKDEV_TYPE_SD_CPRM:
      btype = HOTPLUG_TYPE_SD_CPRM;
      break;
    case BLOCKDEV_TYPE_ALL:
      btype = HOTPLUG_TYPE_ALL;
      break;

    default:
      FS_ERR_FATAL ("Unexpected blockdev type: %d", type, 0,0);
      break;
  }

  return btype;
}

/* Returns the first device in the hotplug_device_list which has specified
 * 'status' set.  */
struct hotplug_device *
hotplug_find_dev_by_type (hotplug_dev_type type,
                          struct hotplug_device *last)
{
  unsigned int index;

  if (last == NULL)
    index = 0;
  else
    index = last->index + 1;

  while (index < hotplug_num_devices)
  {
    /* Skip if this entry is in unusable state */
    if (hotplug_device_list[index].dev_state != DEV_UNUSABLE)
    {
      block_device_type btype =
        blockdev_get_type (hotplug_device_list[index].bdev);

      if (blockdev_to_hotplug_type (btype) == type)
        return &hotplug_device_list[index];
    }
    index++;
  }
  return NULL;
}

/* Returns the first device in the hotplug_device_list which has the
 * specified path */
struct hotplug_device *
hotplug_find_dev_by_path (const char *path)
{
  unsigned int i;

  if (!path)
    return NULL;

  while (path[0] == '/')        /* Strip leading slash(es) */
    path++;

  if (path[0] == '\0')          /* Name is required */
    return NULL;

  for (i = 0; i < hotplug_num_devices; i++)
  {
    /* Skip if this entry is in unusable state */
    if (hotplug_device_list[i].dev_state != DEV_UNUSABLE)
      if (strcmp (blockdev_get_name (hotplug_device_list[i].bdev) + 1,
                  path) == 0)
        return &hotplug_device_list[i];
  }
  return NULL;
}

/*
 * Device goes into idle state only when there is no activity for that
 * device. We would set device_idle_state as TRUE in the first call of this
 * function and on subsequent call if the value of device_idle_state remains
 * TRUE then we would call the corresponding hotplug device dispatch
 * function to go to sleep mode.
 *
 * We don't require LOCK protection here, because hotplug task is running in
 * lower priority just above sleep. Hotplug task will be preempted if any other
 * task need to write. By checking the device_idle_state here, if write is
 * preempted we still get correct behavior of waiting for some more time
 * before we generate the timeout dispatch to driver. On the other hand,
 * we should not take global hotplug lock here, since read/write don't take
 * hotplug global locks. Even having per device lock will make read/writes
 * slower. When its in contention, raising the priority of hotplug to generate
 * the timeout dispatch to driver to power down and later doing another
 * initialization, which consumes a lot of time.
 *
 * Device goes into idle state only if there is no activity on that
 * particular slot between 2 consecutive polls. For e.g. if the
 * hotplug_idle_device_evt_duration is being set as 30 mins then the device
 * goes to sleep stage in between 30 mins (minimum) or 60 mins (maximum),
 * only if there is no activity going on device.
 */

void
hotplug_idle_device_dispatch (void)
{
  unsigned int i;
  struct hotplug_device *hdev;

  for (i = 0; i < hotplug_num_devices; i++)
  {
    hdev = &hotplug_device_list[i];

    /* If dev_state is DEV_UNUSABLE then don't call
     * blockdev_xxx_device_idle (). */
    if (hdev->dev_state != DEV_UNUSABLE)
    {
      /* If device_idle_state is TRUE this implies there has been no activity
       * for the current device from the last 2 time polls, thus we would call
       * the corresponding device to go to the idle state.
       */
      if (blockdev_get_device_idle(hdev->bdev) == TRUE)
      {
        /* If task switch happens here, then the next activity would going to
         * suffer till the next hotplug_poll () call. */

        /* notify device_idle event. */
        hotplug_notify_events (hdev, HOTPLUG_NOTIFY_EVENT_DEVICE_IDLE);

        /* This should put the force the device to go into a low-power state.
         */
        (void) blockdev_set_device_idle (hdev->bdev);
      }
      else
      {
        /* This call will put the device into the preliminary idle state.
         * This will not actually cause a powerdown until
         * blockdev_set_device_idle has been called twice.
         */
        blockdev_set_device_idle (hdev->bdev);
      }
    }
  }
}

/*
 * This is where we actually check each device for media changes.
 * This is a private function that should be called from hotplug's task only,
 * since it can take significant time.
 */
void
hotplug_poll (void)
{
  struct hotplug_device *hdev;

  /* Nobody else can manipulate our table during the whole poll */
  HOTPLUG_LOCK();

  ZAP ("Polling for devices...");

  /* Walk through each device in the hotplug device table... */
  hdev = &hotplug_device_list[0];
  while (hdev < &hotplug_device_list[hotplug_num_devices])
  {
    hotplug_dev_state prior_state = hdev->dev_state;

    ZPRINTF ("Polling %s.. ", blockdev_get_name (hdev->bdev));

    switch (hdev->dev_state)
    {
      /* Skip any devices we aren't supposed to look at */
      case DEV_UNUSABLE:
        ZAPN ("UNUSABLE ");
        break;

      case DEV_LOCKED:
        ZAPN ("LOCKED ");
        break;

      case DEV_FORMATTING:
        ZAPN ("FORMATTING ");
        break;

      case DEV_UNMOUNTED:
      case DEV_MOUNTED:
        hotplug_poll_live_device (hdev);
        break;

      default:
        ZAPN ("Invalid device status! ");
        break;
    }

    /* If this device was unchanged, advance to the next device.
     * Otherwise, this device will be polled again immediately
     * until the state settles */
    if (prior_state == hdev->dev_state) {
      hdev++;
    }
  }

  ZAP ("Polling complete.");

  HOTPLUG_UNLOCK();
}

/* Poll a single device that is available for mounting/unmounting.
 * (Helper function for main poll loop, not to be called from
 * anywhere else!) */
static void
hotplug_poll_live_device (struct hotplug_device *hdev)
{
  int present;

  /* Find out if the device is present Right Now. */
  present = blockdev_is_present (hdev->bdev);

  /*
   * Send any necessary Insert or Remove events, based on last-known state.
   */
  if (hdev->was_present && !present) {
    ZAPN ("Removed. ");
    hotplug_notify_events (hdev, HOTPLUG_NOTIFY_EVENT_CARD_REMOVED);
    hdev->was_present = FALSE;
  } else if (!hdev->was_present && present) {
    ZAPN ("Inserted. ");
    hotplug_notify_events (hdev, HOTPLUG_NOTIFY_EVENT_CARD_INSERTED);
    hdev->was_present = TRUE;
  }

  /*
   * An Unmounted device may now be present, or a Mounted device may
   * have been yanked.  Verify.
   */
  if ((hdev->dev_state == DEV_UNMOUNTED) && present) {
    /* Media is available right now, but not mounted. */
    /* Release from the clutches of NODEV and mount the device. */
    ZAPN ("Mounting. ");

    hotplug_nodev_umount (hdev);

    if (hotplug_fat_mount (hdev) != 0) {
      ZPRINTF ("Unable to mount device: %s ", blockdev_get_name (hdev->bdev));
      hotplug_nodev_mount (hdev); /* Oops.  Put back NODEV */
    }
  }

  if ((hdev->dev_state == DEV_MOUNTED) && !present) {
    /* Media has vanished.  Unmount the filesystem. */
    ZAPN ("Unmounting. ");

    if (hotplug_umount (hdev) != 0) {
      ZPRINTF ("Unable to umount: %s ", blockdev_get_name (hdev->bdev));
    }
  }
}

/*
 * Cause the Hotplug poll timer to expire and trigger an immediate
 * poll.
 *
 * Note that this is an internal hotplug function.  External apps can
 * accomplish the same by using hotplug_set_polling_interval().
 */
static void
hotplug_force_poll_now (void)
{
 /* If COLDPLUG is requested, then no task will poll and the polling
  * must be done right now, and in the caller's context. */
#if defined FEATURE_EFS_COLDPLUG || defined FS_UNIT_TEST
  hotplug_poll ();
#else
  /* Otherwise, pretend the timer just expired.  Hotplug Task will put
   * the timer back if polling is still enabled */
  fs_os_clr_timer (&fs_hotplug_poll_timer);
  (void) fs_os_set_sigs (&fs_hotplug_tcb, HOTPLUG_POLL_SIG);
#endif
}


void
hotplug_init (void)
{
  struct block_device *bdev;
  unsigned int i = 0;
  static int init_complete = 0;

#ifndef FS_UNIT_TEST
  ASSERT (!init_complete);
#endif

  /* Initialize hotplug critical section */
  HOTPLUG_LOCK_INIT ();

  HOTPLUG_LOCK ();

  /* Iterate through all block devices */
  for (bdev = blockdev_find_by_type (BLOCKDEV_TYPE_ALL, NULL);
       bdev != NULL;
       bdev = blockdev_find_by_type (BLOCKDEV_TYPE_ALL, bdev))
  {
    struct hotplug_device *hdev = &hotplug_device_list[i];

    /* Check that we have not filled up device table */
    if (i >= FS_MAX_HOTPLUG_DEVICES)
      FS_ERR_FATAL ("hotplug_device_list full!", 0,0,0);

    /* Initialize table entry */
    hdev->was_present = 0;
    hdev->dev_state = DEV_UNUSABLE;
    hdev->index = i;
    hdev->bdev  = bdev;

    /* Create a directory for each mountpoint and mount it as nodev
     * to prevent unintended access */
    if (hotplug_force_directory (blockdev_get_name (hdev->bdev)) != 0)
      FS_ERR_FATAL ("Could not mkdir hotplug mountpoint #%d",
                 blockdev_get_device_id (hdev->bdev), 0, 0);

    hotplug_nodev_mount (hdev);

    /* Allow the device to do any device-specific reset */
    if (blockdev_reset (bdev) == 0)
    {
      hdev->dev_state = DEV_UNMOUNTED; /* Ready */
    }
    else
    {
      hdev->dev_state = DEV_UNUSABLE;      /* Do not use */
    }

    /* Increment hotplug_device_list index */
    i++;
  }

  hotplug_num_devices = i;

  HOTPLUG_UNLOCK ();

#ifdef FEATURE_EFS_HOTPLUG

#ifdef TIMETEST_FS_HOTPLUG_TASK_ID
  /* Set the task ID for TIMETEST */
  fs_hotplug_tcb.leds = TIMETEST_FS_HOTPLUG_TASK_ID;
#endif

  /* Start the hotplug management task. */
  fs_os_def_task_ext (&fs_hotplug_tcb,
                      fs_hotplug_stack,
                      FS_HOTPLUG_STACK_SIZE,
                      FS_HOTPLUG_PRI,
                      fs_hotplug_task,
                      0,
                      "FS Hotplug Task",
                      FALSE);

#endif /* FEATURE_EFS_HOTPLUG */

  init_complete = 1;

#ifdef FEATURE_EFS_COLDPLUG
  /* This is the one and only poll we will do automatically for COLDPLUG. */
  hotplug_poll();
#endif

}


#ifdef FEATURE_EFS_HOTPLUG      /* Hotplug gets a task, Coldplug gets none. */

static void
fs_hotplug_task (dword arg)
{
  fs_os_sigs_type sigs;
  int timer_running = 0;

  (void) arg;                   /* unused */

  /* Create our timer for the poll cycle */
  fs_os_def_timer (&fs_hotplug_poll_timer, &fs_hotplug_tcb,
                   HOTPLUG_POLL_SIG);

  /* Create our timer for timeout event generation. */
  fs_os_def_timer (&fs_hotplug_idle_device_timer, &fs_hotplug_tcb,
                   HOTPLUG_IDLE_DEVICE_SIG);

  /* Set our default poll interval and clear the timer */
  hotplug_set_polling_interval (FS_HOTPLUG_DEFAULT_POLL_INTERVAL_MS);

  /* Set our default idle device event generation interval. */
  hotplug_set_idle_device_event_duration
                                (FS_HOTPLUG_TIMEOUT_EVT_INTERVAL_MINUTES);

  /* Register with USB for notifications */
  hotplug_usbhost_register();

  /* Set the timer for first time for idle device event generation. */
  fs_os_set_timer (&fs_hotplug_idle_device_timer,
                   hotplug_idle_device_evt_duration);

  while (1)
  {
    /*
     * This has to be protected, because we are checking multiple
     * variables.
     */
    HOTPLUG_LOCK();
    if (polling_enabled && !timer_running)
    {
      fs_os_set_timer (&fs_hotplug_poll_timer, hotplug_poll_interval);
      timer_running = 1;
    }
    else if (!polling_enabled && timer_running)
    {
      fs_os_clr_timer (&fs_hotplug_poll_timer);
      timer_running = 0;
    }
    HOTPLUG_UNLOCK ();

    sigs = fs_os_wait (HOTPLUG_POLL_SIG
                     | HOTPLUG_USBHOST_OPEN_SIG
                     | HOTPLUG_USBHOST_CLOSE_SIG
                     | HOTPLUG_IDLE_DEVICE_SIG);

    /* Did we lose a connected device?   Handle this FIRST */
    if (sigs & HOTPLUG_USBHOST_CLOSE_SIG)
    {
      (void) fs_os_clr_sigs (&fs_hotplug_tcb, HOTPLUG_USBHOST_CLOSE_SIG);
      hotplug_usbhost_close ();
      hotplug_force_poll_now ();
    }

    /* Has hotplug_usbhost_callback signalled that a device is ready? */
    if (sigs & HOTPLUG_USBHOST_OPEN_SIG)
    {
      (void) fs_os_clr_sigs (&fs_hotplug_tcb, HOTPLUG_USBHOST_OPEN_SIG);
      hotplug_usbhost_open ();
      hotplug_force_poll_now ();
    }

    /* Has our timer expired?  Then it's time to visit our devices */
    if (sigs & HOTPLUG_POLL_SIG)
    {
      timer_running = 0;
      if (polling_enabled)
        hotplug_poll ();
    }

    /* Has our timer expired?
     * Then it's the time to visit for timeout event check. */
    if (sigs & HOTPLUG_IDLE_DEVICE_SIG)
    {
      (void) fs_os_clr_sigs (&fs_hotplug_tcb, HOTPLUG_IDLE_DEVICE_SIG);

      if (hotplug_idle_device_evt_duration)
      {
        hotplug_idle_device_dispatch ();
        fs_os_set_timer (&fs_hotplug_idle_device_timer,
                         hotplug_idle_device_evt_duration);
      }
      else
        /* hotplug_set_idle_device_evt_duration is 0 this impiles that user
         * want to stop the idle_device_dispatch () polling entirely. */
        fs_os_clr_timer (&fs_hotplug_idle_device_timer);
    }

    (void) fs_os_clr_sigs (&fs_hotplug_tcb, sigs);
  }
}
#endif /* FEATURE_EFS_HOTPLUG */

/* Set the frequency at which Hotplug task would check for any idle
 * device present on all the slots. */
void
hotplug_set_idle_device_event_duration (uint8 minutes)
#ifdef FEATURE_EFS_HOTPLUG
{
  /* Check the value of the input variable minutes to make sure that
   * value of minutes would not get overflow the limit of uint8. */

  if (minutes <= 254)
    hotplug_idle_device_evt_duration = minutes * 60 * 1000;
  else
    ZPRINTF ("Out of bound duration value.\n");
}
#else  /* function is meaningless on COLDPLUG.  Do nothing. */
{
  (void) minutes;
}
#endif


/* Allow an external task to enable/disable hotplug polling and set
 * the frequency (in milliseconds).  Polling is disabled with a value
 * of zero milliseconds.
 *
 * This also causes an immediate poll and restarts the timer any time
 * the interval is changed.
 *
 * Returns the former polling interval value in milliseconds.
 */
unsigned int
hotplug_set_polling_interval (unsigned int milliseconds)
#ifdef FEATURE_EFS_HOTPLUG
{
  unsigned int old_value;

  old_value = (unsigned int) hotplug_poll_interval;

  /* Do nothing if time isn't actually changed.  This ignores
   * redundant calls without resetting the timer and polling. */
  if (old_value != milliseconds) {
    hotplug_poll_interval = (fs_os_timer_cnt_type) milliseconds;
    polling_enabled = (milliseconds != 0) ? 1 : 0;

    /* Cause an immediate poll event to restart timer with new value.
     * If polling is no longer enabled, then this will terminate the
     * current timer.  */
    hotplug_force_poll_now();
  }

  return old_value;
}
#else  /* function is meaningless on COLDPLUG.  Do nothing. */
{
  (void) milliseconds;
  return 0;                     /* this is a clue */
}
#endif

/* Request that Hotplug cease mount operations on a device.  Will
 * forcibly unmount the device and disable polling.  The device
 * remains useable through direct read/write calls, but Hotplug itself
 * will not attempt to (FAT) mount it.
 *
 * Media that is not locked should never be accessed directly with the
 * hotplug_xxx_dev() functions.
 *
 * Returns 0 on success, or -1 if the media could not be locked.
 * Attempting to lock already-locked media returns -1.
 */
int
hotplug_lock_dev (struct hotplug_device *hdev)
{
  int status = 0;

  HDEV_VALIDATE (hdev);
  HOTPLUG_LOCK ();

  /* Fail if this device is already locked. */
  if (hdev->dev_state == DEV_LOCKED) {
    status = -1;                /* fail */
  } else {
    /* First, unmount the device if it is mounted. */
    if (hdev->dev_state == DEV_MOUNTED)
      status = hotplug_umount (hdev);

    /* Close the device, since someone else intends to open it. */
    if (status == 0)
      status = blockdev_close (hdev->bdev);

    hdev->dev_state = DEV_LOCKED; /* Now avoid this device */

    /* Notify the interested clients */
    hotplug_notify_events (hdev, HOTPLUG_NOTIFY_EVENT_CARD_LOCKED);
  }

  HOTPLUG_UNLOCK ();

  return status;
}

/* Called to indicate that the device is again available for hotplug
 * (FAT) use.  Merely allows the hotplug manager to begin polling and
 * mounting found media in this device.
 *
 * Returns 0 for success, or -1 if the device wasn't
 * locked. (harmless)
 */
int
hotplug_unlock_dev (struct hotplug_device *hdev)
{
  int status = 0;

  HDEV_VALIDATE (hdev);
  HOTPLUG_LOCK ();

  if (hdev->dev_state != DEV_LOCKED) {
    status = -1;                /* It wasn't locked? */
  } else {
    hdev->dev_state = DEV_UNMOUNTED; /* It's ours once again */

    /* Send the card-unlocked event */
    hotplug_notify_events (hdev, HOTPLUG_NOTIFY_EVENT_CARD_UNLOCKED);
  }

  HOTPLUG_UNLOCK ();

  /* Force a poll immediately, since new media is now available */
  hotplug_force_poll_now();

  return status;
}

/* Lock all media of a specific type */
static void
hotplug_lock_by_type (hotplug_dev_type type)
{
  struct hotplug_device *hdev;

  /* Lock ALL the devices of this type */
  hdev = hotplug_find_dev_by_type (type, NULL);
  while (hdev != NULL) {
    hotplug_lock_dev (hdev);
    hdev = hotplug_find_dev_by_type (type, hdev);
  }
}

/* Unlock all media of a specific type */
static void
hotplug_unlock_by_type (hotplug_dev_type type)
{
  struct hotplug_device *hdev;

  /* Unlock ALL the devices of this type */
  hdev = hotplug_find_dev_by_type (type, NULL);
  while (hdev != NULL) {
    hotplug_unlock_dev (hdev);
    hdev = hotplug_find_dev_by_type (type, hdev);
  }
}

/* These are the old functions to lock all of SD/MMC
 * DEPRECATED!  Do not use in new code. */
void
fs_sfat_lock_card (void)
{
  hotplug_lock_by_type (HOTPLUG_TYPE_MMC);
}

/* Unlock all SD/MMC cards (DEPRECATED)*/
void
fs_sfat_unlock_card (void)
{
  hotplug_unlock_by_type (HOTPLUG_TYPE_MMC);
}

/* Format by pathname */
int
hotplug_format (const char *path)
{
  struct hotplug_device *hdev;

  hdev = hotplug_find_dev_by_path (path);
  if (hdev)                     /* Found a name match */
    return hotplug_format_dev (hdev);
  else
    return -1;
}


/* Format device */
int
hotplug_format_dev (struct hotplug_device *hdev)
{
  int status = 0;

  HDEV_VALIDATE (hdev);
  HOTPLUG_LOCK ();

  /* Unmount it, if needed */
  if (hdev->dev_state == DEV_MOUNTED) {
    if (hotplug_umount (hdev) == 0) {
      ZPRINTF ("format unmounted the device %s\n\r",
               blockdev_get_name (hdev->bdev));
    }
    else {
      ZPRINTF ("format got bad status from umount: %s\n\r",
               blockdev_get_name (hdev->bdev));
      status = -1;
    }
  }

  /* Is it in a state that doesn't permit formatting, like UNUSABLE? */
  if (hdev->dev_state != DEV_UNMOUNTED) {
    ZPRINTF ("Can not format a device in state %d\n\r", hdev->dev_state);
    status = -1;
  }

  /* Make sure the device is present (and open) */
  if (! blockdev_is_present (hdev->bdev)) {
    ZPRINTF ("Format requires a device is_present\n\r");
    status = -1;
  }

  if (status == 0) {
    /* Move to FORMATTING state so we don't attempt to mount it */
    hdev->dev_state = DEV_FORMATTING;

    /* notify format-start event */
    hotplug_notify_events (hdev, HOTPLUG_NOTIFY_EVENT_FORMAT_START);

    HOTPLUG_UNLOCK ();          /* Don't hold lock during format */

    /* Prepare the device for format (warn the device layer) */
    status = blockdev_device_format (hdev->bdev);

    /* Now do the filesystem-level format */
    if (status == 0)
      /* Must return 0 for success */
      status = fs_hfat_format (blockdev_get_device_id (hdev->bdev));

    HOTPLUG_LOCK ();         /* Grab the lock again to change state */

    /* notify format-complete event */
    hotplug_notify_events (hdev, HOTPLUG_NOTIFY_EVENT_FORMAT_COMPLETE);

    hdev->dev_state = DEV_UNMOUNTED;
  }

  HOTPLUG_UNLOCK ();            /* We're finished */

  hotplug_force_poll_now();     /* Might have new media, how exciting! */

  return status;
}


/*
 * Return TRUE if the named path is currently mounted by hotplug.
 *
 * (Dual-processor specific)
 * This executes on the Apps (SD-controlling) processor.
 * The modem calls this (via RPC) to find out if the device has been
 * mounted on Apps and is ready for remote mounting.
 */
int
hotplug_is_mounted (const char *name)
{
  return (hotplug_dev_is_mounted (hotplug_find_dev_by_path (name)));
}


/*
 * These are transparent pass-through device access functions to allow
 * us to keep 'struct hotplug_device' anonymous.
 */

/* Return TRUE if the device is currently mounted by hotplug. */
int
hotplug_dev_is_mounted (struct hotplug_device *hdev)
{
  HDEV_VALIDATE (hdev);
#if 0
  ZPRINTF ("hotplug_dev_is_mounted(%s) %s\n\r", blockdev_get_name (hdev->bdev),
           (hdev->dev_state == DEV_MOUNTED) ? "YES" : "no ");
#endif
  if (hdev->dev_state == DEV_MOUNTED)
    return 1;
  else
    return 0;
}

/* Return TRUE if media is believed to be present */
int
hotplug_dev_is_present (struct hotplug_device *hdev)
{
  HDEV_VALIDATE (hdev);
  return blockdev_is_present (hdev->bdev);
}

int
hotplug_dev_open (struct hotplug_device *hdev)
{
  HDEV_VALIDATE (hdev);

  /* Use default physical partition 0 */
  return blockdev_open (hdev->bdev, NULL);
}

int
hotplug_dev_close (struct hotplug_device *hdev)
{
  HDEV_VALIDATE (hdev);

  return blockdev_close (hdev->bdev);
}

int
hotplug_dev_read  (struct hotplug_device *hdev,
                   uint32 lba,
                   unsigned char *buf,
                   uint16 n_to_read)
{
  HDEV_VALIDATE (hdev);

  return blockdev_read (hdev->bdev, lba, buf, n_to_read);
}

int
hotplug_dev_write (struct hotplug_device *hdev,
                   uint32 lba,
                   unsigned char *buf,
                   uint16 n_to_write)
{
  HDEV_VALIDATE (hdev);

  return blockdev_write (hdev->bdev, lba, buf, n_to_write);
}

/*
  This device write is called when the write is specifically know to
  be 'file' or payload data, rather than to directories or other file
  system metadata.

  In the case of CPRM, we need to be able to specify a special mode
  for user data so that it is written using the correct MKB.  The MKB
  is different for metadata, which should use the standard write call.

  For all non-CPRM devices, this write call performs identically to
  the standard write call.
*/
int
hotplug_dev_write_udata(struct hotplug_device *hdev,
                   uint32 lba,
                   unsigned char *buf,
                   uint16 n_to_write)
{
  HDEV_VALIDATE (hdev);

  return blockdev_uwrite (hdev->bdev, lba, buf, n_to_write);
}


int
hotplug_dev_erase (struct hotplug_device *hdev,
                   uint32 lba,
                   uint16 n_to_erase)
{
  HDEV_VALIDATE (hdev);

  return blockdev_erase (hdev->bdev, lba, n_to_erase);
}

int
hotplug_dev_get_size (struct hotplug_device *hdev,
                      uint32 *blocks,
                      uint16 *bytes_per_block)
{
  HDEV_VALIDATE (hdev);

  return blockdev_get_device_size (hdev->bdev, blocks, bytes_per_block);
}


const char *
hotplug_dev_get_path (struct hotplug_device *hdev)
{
  HDEV_VALIDATE (hdev);
  return blockdev_get_name (hdev->bdev);
}

int
hotplug_dev_get_state (struct hotplug_device *hdev,
                       struct hdev_state *state)
{
  /* make sure that the args are ok. */
  HDEV_VALIDATE (hdev);
  if (state == NULL)
    return -1;                  /* fail */

  /* did we find the device present in our last poll? */
  state->is_present = (hdev->was_present == 1);

  /* Assume false to start, then check if we have positive info about
   * this device. */
  state->is_mounted = 0;
  state->is_formatting = 0;
  state->is_locked = 0;

  switch (hdev->dev_state)
  {
    case DEV_MOUNTED:
      state->is_mounted = 1;
      break;

    case DEV_FORMATTING:
      state->is_formatting = 1;
      break;

    case DEV_LOCKED:
      state->is_locked = 1;
      break;

    case DEV_UNMOUNTED:
    case DEV_UNUSABLE:
      break;

    default:
      /* we should never get here. */
      ASSERT (FALSE);
      break;
  }

  return 0;                     /* success */
}


hotplug_notify_handle
hotplug_notify_register (hotplug_dev_type device_type,
                         hotplug_notify_event event_mask,
                         hotplug_notify_event_cb user_cb,
                         void *user_data)
{
  hotplug_notify_handle ret_val = HOTPLUG_NOTIFY_INVALID_HANDLE;
  unsigned int i;

  /* validate the input parameters */
  if (user_cb == NULL) {
    return ret_val;
  }

  HOTPLUG_LOCK ();

  /* find an empty slot in the client table */
  for (i = 0; i < FS_HOTPLUG_NOTIFY_MAX_CLIENTS; ++i) {
    if (hotplug_notify_client_tbl[i].user_cb == NULL)
      break;
  }

  /* If a slots is available to add this client... */
  if (i < FS_HOTPLUG_NOTIFY_MAX_CLIENTS) {

    /* allocate this empty slot to this client. */
    hotplug_notify_client_tbl[i].device_type = device_type;
    hotplug_notify_client_tbl[i].event_mask = event_mask;
    hotplug_notify_client_tbl[i].user_cb = user_cb;
    hotplug_notify_client_tbl[i].user_data = user_data;

    /* this slot index (base 1) is the registration-handle for this client. */
    ret_val = (hotplug_notify_handle)(i + 1);
  }

  HOTPLUG_UNLOCK ();
  return ret_val;
}


int
hotplug_notify_unregister (hotplug_notify_handle reg_hdl)
{
  int ret_val = -1;

  /* sanity check on the registration handle */
  if ((reg_hdl != HOTPLUG_NOTIFY_INVALID_HANDLE) &&
      (reg_hdl <= FS_HOTPLUG_NOTIFY_MAX_CLIENTS)) {

    /* For us the registration handle is 0-based array index. */
    reg_hdl--;

    HOTPLUG_LOCK ();

    /* Make sure we are not attempting to free an already-empty slot */
    if (hotplug_notify_client_tbl[reg_hdl].user_cb != NULL)
    {
      /* mark this slot as free by zeroing out all fields. */
      memset (&hotplug_notify_client_tbl[reg_hdl], 0,
              sizeof (hotplug_notify_client_tbl[0]));

      ret_val = 0;              /* Success */
    }

    HOTPLUG_UNLOCK ();
  }

  return ret_val;
}


static void
hotplug_notify_events (struct hotplug_device *hdev,
                       hotplug_notify_event event_id)
{
  unsigned int i;

  HOTPLUG_LOCK ();

  /* loop through each slot and check if we have to do a callback. */
  for (i = 0; i < FS_HOTPLUG_NOTIFY_MAX_CLIENTS; ++i) {
    hotplug_notify_event interesting_events;

    /* skip empty slots */
    if (hotplug_notify_client_tbl[i].user_cb == 0) {
      continue;
    }

    /* Mask off any unrequested events, leaving only the ones of interest */
    interesting_events = event_id & hotplug_notify_client_tbl[i].event_mask;

    /* check if these events are registered for. */
    if (interesting_events == 0) {
      continue;
    }

    /* check if this device is registered for. */
    if ((hotplug_notify_client_tbl[i].device_type != HOTPLUG_TYPE_ALL) &&
        (hotplug_notify_client_tbl[i].device_type !=
         blockdev_to_hotplug_type (blockdev_get_type (hdev->bdev)))) {
      continue;
    }

    /* if we make it till here, then this client has registered for this
       device-type and events, so invoke the user's callback */
    (hotplug_notify_client_tbl[i].user_cb) (hdev, interesting_events, i+1,
       hotplug_notify_client_tbl[i].user_data);
  }

  HOTPLUG_UNLOCK ();
}

int
hotplug_checkfat (const char *path, int mode, int *errors,
                  void (*callback) (void *), void *param)
{
  struct hotplug_device *hdev;
  int ret;

  /* Get hotplug device */
  hdev = hotplug_find_dev_by_path (path);

  /* Couldn't find the device */
  if (hdev == NULL)
    return -1;

  /* Bounds check path */
  if (strlen (path) < 5)
    return -1;

  /* Check to make sure path is FAT partition */
  if ((strncmp (path, "/mmc", 4) != 0) &&
      (strncmp (path, "/ftl", 4) != 0))
    return -1;

  /* Poll devices to ensure anything currently plugged in is mounted */
  hotplug_poll ();

  /* Only continue if we have a mounted device */
  if (hdev->dev_state != DEV_MOUNTED)
    return -1;

  /* Lock the device.  We just set the device to the locked state
   * because we don't actually want it unmounted, or the side effects
   * that come with unmounting it */
  hdev->dev_state = DEV_LOCKED;

  /* Un-protect EFS memory partition
   * Due to the time taken to run checkfat we do not grab the global lock
   * which allows other threads to operate concurrently if using flash.
   * In lieu of global lock we will unlock it here and relock when we
   * return. */
  fs_efs2_unprotect_memory ();

  /* Run checkfat */
  ret = f_checkfat (blockdev_get_device_id (hdev->bdev),
                    mode, errors, callback, param);

  /* Lock memory segment again */
  fs_efs2_protect_memory ();

  /* Unlock the device for use */
  hdev->dev_state = DEV_MOUNTED;

  /* Poll new data immediately */
  hotplug_poll ();

  return ret;
}

int
hotplug_dev_get_device_info (struct hotplug_device *hdev,
                             struct hotplug_device_info *dev_info)
{
  int rv;
  struct block_device_info bdev_info;
  HDEV_VALIDATE (hdev);

  /* Populate the device specific info. */
  rv = blockdev_get_device_info (hdev->bdev, &bdev_info);
  if (rv)
    return rv;

  /* Copy device info into hotplug info struct */
  dev_info->manufacturer_id = bdev_info.manufacturer_id;
  dev_info->oem_id = bdev_info.oem_id;
  dev_info->prod_rev = bdev_info.prod_rev;
  dev_info->prod_serial_num = bdev_info.prod_serial_num;
  fs_strlcpy ((char *)dev_info->product_name, bdev_info.product_name,
              sizeof (dev_info->product_name));
  fs_strlcpy ((char *)dev_info->manufactured_date, bdev_info.manufactured_date,
              sizeof (dev_info->manufactured_date));

  return 0;
}

#endif
