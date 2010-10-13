/***********************************************************************
 * fs_hotplug_usbhost.c
 *
 * Glue logic between hotplug and usbhost drivers.
 *
 * Copyright (C) 2009 QUALCOMM, Inc.
 *
 * This glue logic contains the USB callbacks and helper functions
 * necessary for USB MS mode. This code requires the FS_BLOCKDEV_USB
 * module be defined.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_usbhost.c#2 $ $DateTime: 2009/07/06 18:30:05 $ $Author: ebuller $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-06-12   ebb   Moved rex_xxx calls to fs_osal.h abstraction.
2009-05-08   ebb   Create

===========================================================================*/

#include "customer.h"
#include "fs_hotplug_i.h"

#if defined(FS_HOTPLUG_USB) && defined(FS_BLOCKDEV_USB)

#include "fs_osal.h"
#include "jzap.h"
#include "msg.h"
#include "usbhost_api.h"
#include "fs_blockdev.h"

/* Needed to peek into internal USB state information
 * and reference unique usb client_id */
#include "fs_blockdev_usb.h"

/* Hotplug supports USB Host Mass Storage.. ie, when the USB Host software
 * controls an external USB device like a thumbdrive, hard drive, or card
 * reader. */

/* Returns the first USB block device which has specified 'state' value. */
static struct block_device *
find_by_state (blockdev_usbhost_state state,
               struct block_device *bdev)
{
  usb_device_data *udev;
  do {
    bdev = blockdev_find_by_type (BLOCKDEV_TYPE_USB_MS, bdev);
    udev = (usb_device_data *)bdev->device_data;
  } while (udev && (udev->usb_state != state));
  return bdev;
}


/* Returns the first USB device in the hotplug_device_list which has
 * the specific instance. */
static struct block_device *
find_by_instance (usbhost_dev_instance_type inst,
                  struct block_device *bdev)
{
  usb_device_data *udev;
  do {
    bdev = blockdev_find_by_type (BLOCKDEV_TYPE_USB_MS, bdev);
    udev = (usb_device_data *)bdev->device_data;
  } while (udev && (udev->usb_instance != inst));
  return bdev;
}


/* This is the callback function hotplug passes to usbhost_register.
 * THIS EXECUTES IN USB CONTEXT!  Don't spend too long here.
 * This can be called multiple times in a row (for multiple devices)
 * When it is called, we are given a valid dev_instance, but not "handle".
 */
void
hotplug_usbhost_callback (usbhost_pdev_type pdev_typ,
                          usbhost_dev_status_type status,
                          usbhost_dev_handle_type dev_instance, void *param)
{
  usb_device_data *udev;
  (void) pdev_typ;
  (void) param;

  if (status == USBHOST_DEV_CONNECTED) {
    struct block_device *bdev;

    /*
     * USB has detected the presence of a mass storage device.  Hurah!
     * Try to find the next available structure in the hotplug table
     * (the first USB device that is in UNCONNECTED state).
     */
    ZAP ("USBHOST_DEV_CONNECTED");
    bdev = find_by_state (USBSTATUS_UNCONNECTED, NULL);
    udev = (usb_device_data *)bdev->device_data;

    if (bdev) {
      if (find_by_instance (dev_instance, NULL) != NULL)
        ZAP ("Adding a SECOND duplicate instance!"); /* Bad */

      udev->usb_state = USBSTATUS_CONNECTED;
      udev->usb_instance = dev_instance;
      fs_os_set_sigs (&fs_hotplug_tcb, HOTPLUG_USBHOST_OPEN_SIG);
    } else {
      ZAP ("Could not find an unconnected device slot!");
      MSG_ERROR ("HOTPLUG: New device, with no available slot!" ,0,0,0);
      /* Drop this one */
    }
  } else if (status == USBHOST_DEV_DISCONNECTED) {
    struct block_device *bdev;
    /*
     * USB has detected a disconnect on a device.
     * Find this device instance, mark it closed, and wake up Hotplug task.
     */
    ZAP ("USBHOST_DEV_DISCONNECTED");
    bdev = find_by_instance (dev_instance, NULL);
    udev = (usb_device_data *)bdev->device_data;

    if (!bdev)
    {
      ZAP ("Got a DISCONNECT Notify with no matching instance?");
    } else {
      udev->usb_state = USBSTATUS_CLOSED;
      fs_os_set_sigs (&fs_hotplug_tcb, HOTPLUG_USBHOST_CLOSE_SIG);
    }
  } else {
    ZAP ("Unknown device status from USB host stack");
    MSG_ERROR ("HOTPLUG: Unexpected device status from USB Host MS!",0,0,0);
  }
}

/*
 * Register with USB host stack.  Tell it what signal to use for its own
 * semaphore, which will execute in our task context.
 * Our Hotplug task reserves a signal for this.
 * We get our client_id from this call.
 */
void
hotplug_usbhost_register (void)
{
  usbhost_task_table_type usb_sigs;

  usb_sigs.sem_wait_sig = HOTPLUG_USBHOST_SEM_WAIT_SIG;
  usb_sigs.wait_sig = 0;        /* Deprecated */
  usb_sigs.dog_rpt_sig = 0;     /* No need for dog reporting */
  usb_sigs.dog_rpt_fn_ptr = NULL;
  client_id =
    usbhost_register (USBHOST_PDEV_TYPE_MASS_STORAGE, "Hotplug", usb_sigs,
                      hotplug_usbhost_callback, NULL);
}


/*
 * Called from USB's context when a device has been opened (by our request)
 * We had an instance from the connection, and
 * we now receive a valid handle to use in future calls.
 */
static usbhost_open_ret_type
hotplug_usbhost_open_notification (usbhost_dev_instance_type inst,
                                   usbhost_dev_handle_type handle,
                                   usbhost_open_status stat,
                                   char *client_desc, void *param)
{
  usbhost_open_ret_type result;
  (void) client_desc;
  (void) param;

  /* Assume failure.  Request a retry?  There is no "ERROR" code possible. */
  result = USBHOST_OPEN_RET_RESCHEDULE;

  if (stat == USBHOST_OPEN_AVAILABLE)
  {
    /*
     * Find the first entry in our hotplug table that is of this USB
     * instance and associate that dev_instance to this handle.
     */
    struct block_device *bdev;
    usb_device_data *udev;
    ZAP ("USBHOST_OPEN_AVAILABLE");
    bdev = find_by_instance (inst, NULL);
    udev = (usb_device_data *)bdev->device_data;

    if (!bdev) {
      ZAP ("Mysterious USB device opened.  No matching instance");
    } else if (udev->usb_state != USBSTATUS_OPEN_REQUESTED) {
      ZAP ("We did not request open on this instance!");
    } else {
      udev->usb_handle = handle; /* Save the handle! */
      udev->usb_state = USBSTATUS_OPEN;
      result = USBHOST_OPEN_RET_UNSCHEDULE; /* Accept this connection */
    }
  }
  else if (stat == USBHOST_OPEN_NOT_AVAILABLE)
  {
    /* Our open request was denied.  Request a retry. */
    ZAP ("USBHOST_OPEN_NOT_AVAILABLE");
  }
  else
  {
    ZMSG ("Got an open_notify with a bizzare status: %d", stat, 0,0);
  }
  return result;
}

/*
 * Called from USB's context when a device is requested for closure.
 * This means some other task would like to use a device that we have
 * open, and we are being asked to release it.
 *
 * Since Hotplug "owns" any device listed in the table, we ignore
 * this request.  It would indicate a real problem.
 */
static void
hotplug_usbhost_close_notification (usbhost_dev_instance_type handle,
                                    char *client_desc,
                                    usbhost_client_id_type from_client_id,
                                    void *param)
{
  (void) handle;
  (void) client_desc;
  (void) from_client_id;
  (void) param;

  ZAP ("Got a usbhost_close_notification, and ignored it.");
}

/* This is called by fs_hotplug_task() after a USB device is connected
 * and the callback gives us the signal to open any new devices */
void
hotplug_usbhost_open (void)
{
  struct block_device *bdev = NULL;
  usb_device_data *udev;
  int serviced = 0;

  /* Some of the USB devices are available for communication.
     (state = USBSTATUS_CONNECTED)
     We should find ALL such devices and open them. */
  ZAP ("hotplug_usbhost_open");
  while (1)
  {
    bdev = find_by_state (USBSTATUS_CONNECTED, bdev);
    udev = (usb_device_data *)bdev->device_data;
    if (!bdev)
      break;
    ZPRINTF ("hotplug_usbhost_open(%s)", bdev->mount_point);
    /* Ask that the device be opened now. */
    udev->usb_state = USBSTATUS_OPEN_REQUESTED;
    usbhost_req_open (client_id,
                      udev->usb_instance,
                      hotplug_usbhost_open_notification,
                      hotplug_usbhost_close_notification);
    serviced++;
  }

  if (serviced == 0)
    ZAP ("hotplug_usbhost_open without any CONNECTED devices!");
}

/* This is called by fs_hotplug_task() after a USB device is disconnected.
 * One or more devices is now in USBSTATUS_CLOSED. */
void
hotplug_usbhost_close (void)
{
  struct block_device *bdev = NULL;
  usb_device_data *udev;
  int serviced = 0;

  /* Some of the devices are no longer available */
  while (1)
  {
    bdev = find_by_state (USBSTATUS_CLOSED, bdev);
    udev = (usb_device_data *)bdev->device_data;
    if (!bdev)
      break;

    /* This device will be unmounted the next time hotplug task
     * polls and finds it unuseable without a handle. */
    udev->usb_instance = USB_NO_INSTANCE;
    udev->usb_handle = USB_NO_HANDLE;
    udev->usb_state = USBSTATUS_UNCONNECTED;

    ZPRINTF ("hotplug_usbhost_close(%s)", bdev->mount_point);
    serviced++;
  }

  if (serviced == 0)
    ZAP ("hotplug_usbhost_close without any CLOSED devices!");
}

#else
  extern int __dont_complain_about_empty_file;
#endif /* FS_HOTPLUG_USB */
