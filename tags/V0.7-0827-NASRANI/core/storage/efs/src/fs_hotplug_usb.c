/***********************************************************************
 * fs_hotplug_usb.c
 *
 * USB Mass Storage support for Hotplug.
 * Copyright (C) 2006, 2008 QUALCOMM, Inc.
 *
 * Hotplug device I/O and hotplug support functions for USB Mass Storage
 * devices.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_usb.c#3 $ $DateTime: 2008/12/12 11:11:10 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-12   rp    Added write_iovec, reliable write functions to hdev.
2008-11-24   yog   Added hotplug_success_stub for new timeout function.
2006-11-09   sch   Added extra write handler to the device table
2006-11-06   s h   Ignore devices that have a block size other tha 512 bytes.
2006-10-05   s h   Quiet some excessive messages.
2006-09-20   s h   Renamed TYPE_ values to have HOTPLUG prefix.
2006-09-12   s h   Fix usbhost_open_ret_type default value (was warning).
2006-04-24   s h   Renamed control flag to FS_HOTPLUG_USB
2006-06-02   s h   Added reset handler
2006-05-15   s h   Renamed hotplug_null to hotplug_success_stub
2006-05-07   s h   Replaced DRV_GEOMETRY_DESC with block size & count
2006-05-08   s h   Removed umount from device-specific calls.
2006-04-24   s h   Renamed control flag to FS_HOTPLUG_USB_MS
2006-04-22   s h   Moved all private definitions into fs_hotplug_i.h
2006-04-21   s h   Code commenting and cleanup
2006-04-20   sch   Moved the updation of USB state variables to usbhost_close
2006-04-20   sch   Removed ASSERTs to allow operations already in progress
2006-04-20   sch   USB API change: usbhost_ioctl() instance is now handle
2006-04-20   sch   Pass in handle instead of instance in usbhost_ioctl()
2006-04-14   s h   More debugging prints.
2006-04-13   s h   Code review feedback implemented.
2006-04-13   s h   Allow compilation without USB
2006-03-31   sch   Fixed syntax error in hotplug_usbhost_open_notification
2006-03-31   s h   Added USB-specific unmount
2006-03-24   s h   Brought in all USB-specific functions from hotplug
2006-03-15   sch   Create

===========================================================================*/

#include "customer.h"
#include "fs_hotplug.h"
#include "fs_hotplug_i.h"
#include "fs_hotplug_usb.h"

/* Hotplug supports USB Host Mass Storage.. ie, when the MSM controls
 * an external USB device like a thumbdrive, hard drive, or card
 * reader.  */
#ifdef FS_HOTPLUG_USB

#include "jzap.h"
#include "msg.h"
#include "usbhost_api.h"

/* Unique ID that USBHOST uses to recognize clients (like Hotplug) */
static usbhost_client_id_type client_id;

/* These USB-specific handlers are used in the Hotplug device table
 * for USB devices. */
static int hotplug_usb_read (struct hotplug_device *, uint32 lba,
                             unsigned char *buf, uint16 n_to_read);
static int hotplug_usb_write (struct hotplug_device *,
                              uint32 lba, unsigned char *buf,
                              uint16 n_to_write);
static int hotplug_usb_get_size (struct hotplug_device *,
                                 uint32 *blocks, uint16 *bytes_per_block);
static int hotplug_usb_is_present (struct hotplug_device *);

/* Returns the first USB device in the hotplug_device_list which has
 * specified 'state' value.  */
static struct hotplug_device *
find_by_state (hotplug_usbhost_state state,
               struct hotplug_device *hdev)
{
  do {
    hdev = hotplug_find_dev_by_type (HOTPLUG_TYPE_USB_MS, hdev);
  } while (hdev && (hdev->usb_state != state));
  return hdev;
}

/* Returns the first USB device in the hotplug_device_list which has
 * the specific instance. */
static struct hotplug_device *
find_by_instance (usbhost_dev_instance_type inst,
                  struct hotplug_device *hdev)
{
  do {
    hdev = hotplug_find_dev_by_type (HOTPLUG_TYPE_USB_MS, hdev);
  } while (hdev && (hdev->usb_instance != inst));
  return hdev;
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
  (void) pdev_typ;
  (void) param;

  if (status == USBHOST_DEV_CONNECTED) {
    struct hotplug_device *hdev;
    /*
     * USB has detected the presence of a mass storage device.  Hurah!
     * Try to find the next available structure in the hotplug table
     * (the first USB device that is in UNCONNECTED state).
     */
    ZAP ("USBHOST_DEV_CONNECTED");
    hdev = find_by_state (USBSTATUS_UNCONNECTED, NULL);
    if (hdev) {
      if (find_by_instance (dev_instance, NULL) != NULL)
        ZAP ("Adding a SECOND duplicate instance!"); /* Bad */

      hdev->usb_state = USBSTATUS_CONNECTED;
      hdev->usb_instance = dev_instance;
      rex_set_sigs (&fs_hotplug_tcb, HOTPLUG_USBHOST_OPEN_SIG);
    } else {
      ZAP ("Could not find an unconnected device slot!");
      MSG_ERROR ("HOTPLUG: New device, with no available slot!" ,0,0,0);
      /* Drop this one */
    }
  } else if (status == USBHOST_DEV_DISCONNECTED) {
    struct hotplug_device *hdev;
    /*
     * USB has detected a disconnect on a device.
     * Find this device instance, mark it closed, and wake up Hotplug task.
     */
    ZAP ("USBHOST_DEV_DISCONNECTED");
    hdev = find_by_instance (dev_instance, NULL);
    if (!hdev)
    {
      ZAP ("Got a DISCONNECT Notify with no matching instance?");
    } else {
      hdev->usb_state = USBSTATUS_CLOSED;
      rex_set_sigs (&fs_hotplug_tcb, HOTPLUG_USBHOST_CLOSE_SIG);
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
    struct hotplug_device *hdev;
    ZAP ("USBHOST_OPEN_AVAILABLE");
    hdev = find_by_instance (inst, NULL);
    if (!hdev) {
      ZAP ("Mysterious USB device opened.  No matching instance");
    } else if (hdev->usb_state != USBSTATUS_OPEN_REQUESTED) {
      ZAP ("We did not request open on this instance!");
    } else {
      hdev->usb_handle = handle; /* Save the handle! */
      hdev->usb_state = USBSTATUS_OPEN;
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

/* -------------- Hotplug Context -------------- */

/* This is called by fs_hotplug_task() after a USB device is connected
 * and the callback gives us the signal to open any new devices */
void
hotplug_usbhost_open (void)
{
  struct hotplug_device *hdev = NULL;
  int serviced = 0;

  /* Some of the USB devices are available for communication.
     (state = USBSTATUS_CONNECTED)
     We should find ALL such devices and open them. */
  ZAP ("hotplug_usbhost_open");
  while (1)
  {
    hdev = find_by_state (USBSTATUS_CONNECTED, hdev);
    if (!hdev)
      break;
    ZPRINTF ("hotplug_usbhost_open(%s)", hdev->name);
    /* Ask that the device be opened now. */
    hdev->usb_state = USBSTATUS_OPEN_REQUESTED;
    usbhost_req_open (client_id,
                      hdev->usb_instance,
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
  struct hotplug_device *hdev = NULL;
  int serviced = 0;

  /* Some of the devices are no longer available */
  while (1)
  {
    hdev = find_by_state (USBSTATUS_CLOSED, hdev);
    if (!hdev)
      break;

    /* This device will be unmounted the next time hotplug task
     * polls and finds it unuseable without a handle. */
    hdev->usb_instance = HOTPLUG_NO_INSTANCE;
    hdev->usb_handle = HOTPLUG_NO_HANDLE;
    hdev->usb_state = USBSTATUS_UNCONNECTED;

    ZPRINTF ("hotplug_usbhost_close(%s)", hdev->name);
    serviced++;
  }

  if (serviced == 0)
    ZAP ("hotplug_usbhost_close without any CLOSED devices!");
}

/* --------- Device IO Handlers ----------- */

static int
hotplug_usb_read (struct hotplug_device *hdev, uint32 lba,
                  unsigned char *buf, uint16 n_to_read)
{
  usbhost_IoctlTransfer ms_data;

  ms_data.numberBlocks = n_to_read;
  ms_data.startBlock = lba;
  ms_data.buffer = buf;
  ms_data.length = n_to_read * 512;

  if ((hdev->usb_handle != HOTPLUG_NO_HANDLE)
      && (USBHOST_SUCCESS == usbhost_ioctl (client_id, hdev->usb_handle,
                                            USBHOST_IOCTL_STOR_BLOCK_READ,
                                            (void *) &ms_data)))
    return 0;                   /* Good! */
  else
    return -1;                  /* Read failed */
}

static int
hotplug_usb_write (struct hotplug_device *hdev, uint32 lba,
                   unsigned char *buf, uint16 n_to_write)
{
  usbhost_IoctlTransfer ms_data;

  ms_data.numberBlocks = n_to_write;
  ms_data.startBlock = lba;
  ms_data.buffer = buf;
  ms_data.length = n_to_write * 512;

  if ((hdev->usb_handle != HOTPLUG_NO_HANDLE)
      && (USBHOST_SUCCESS == usbhost_ioctl (client_id, hdev->usb_handle,
                                            USBHOST_IOCTL_STOR_BLOCK_WRITE,
                                            (void *) &ms_data)))
    return 0;                   /* Good! */
  else
    return -1;                  /* Write failed */
}

static int
hotplug_usb_get_size (struct hotplug_device *hdev,
                      uint32 *blocks, uint16 *bytes_per_block)
{
  uint32 count;
  usbhost_status_type status;

  if (hdev->usb_handle == HOTPLUG_NO_HANDLE)
    return -1;

  status = usbhost_ioctl (client_id, hdev->usb_handle,
                          USBHOST_IOCTL_STOR_BLOCK_SIZE, (void *) &count);
  if (status == USBHOST_ERROR)
    return -1;
  *bytes_per_block = count;

  status = usbhost_ioctl (client_id, hdev->usb_handle,
                          USBHOST_IOCTL_STOR_NUMBER_OF_BLOCKS,
                          (void *) &count);
  if (status == USBHOST_ERROR)
    return -1;
  *blocks = count;

  return 0;
}

/* This checks if a particular USB device is ready for use.
 * In the case of card readers, this includes having useable media inserted.
 * (A connected device without media is considered not-present.)
 *
 * NOTE that this must be callable even when the device is mounted and active!
 * This means re-entrancy is required in the USB stack.
 */
static int
hotplug_usb_is_present (struct hotplug_device *hdev)
{
  usbhost_status_type status;
  int ready;

  if (hdev->usb_state != USBSTATUS_OPEN)
    return 0;

  /* This will check if the media has changed or become unavailable */
  status = usbhost_ioctl (client_id, hdev->usb_handle,
                          USBHOST_IOCTL_STOR_CHECK_MEDIA, NULL);

  ready = 0;                    /* Assume the worst */
  switch (status)
  {
    case USBHOST_SUCCESS:
      ready = 1;                /* Success! */
      break;
    case USBHOST_ERROR:
      ZAPN (" USBHOST_ERROR ");
      break;
    case USBHOST_ERROR_MEDIA_CHANGED:
      ZAPN (" USBHOST_ERROR_MEDIA_CHANGED ");
      break;
    case USBHOST_ERROR_MEDIA_NOT_PRESENT:
      ZAPN (" USBHOST_ERROR_MEDIA_NOT_PRESENT ");
      break;
    default:
      ZPRINTF ("Unexpected CHECK_MEDIA return status: %d!", status);
      break;
  }

  /* Also make sure this is a 512-byte block device, so we don't
   * accidentally talk to a CD-ROM device (like U3 thumbdrives) and
   * crash the USB stack. */
  if (ready)
  {
    uint32 count;
    status = usbhost_ioctl (client_id, hdev->usb_handle,
                            USBHOST_IOCTL_STOR_BLOCK_SIZE, (void *) &count);
    if (status != USBHOST_SUCCESS) {
      ZAPN (" STOR_BLOCK_SIZE failed ");
      ready = 0;
    } else if (count != 512) {
      ZPRINTF (" Media has %d block size, != 512.  Ignored!", count);
      ready = 0;
    }
  }

  return ready;
}

/* These are the entry point functions shared with Hotplug for use by any
 * USB device */
struct hotplug_dev_funcs hotplug_usb_dev = {
  hotplug_fat_mount,            /* Mount */
  hotplug_success_stub,         /* Open */
  hotplug_success_stub,         /* Close */
  hotplug_usb_read,             /* Read */
  hotplug_usb_write,            /* Write */
  hotplug_usb_write,            /* Write */
  hotplug_no_erase,             /* Erase */
  hotplug_usb_get_size,         /* Get Size */
  hotplug_usb_is_present,       /* Is Present */
  hotplug_success_stub,         /* Format prep */
  hotplug_success_stub,         /* Reset */
  hotplug_success_stub,         /* Idle Dispatch Timeout */
  hotplug_no_dev_read_iovec,
  hotplug_no_dev_write_iovec,
  hotplug_no_dev_reliable_write,
  hotplug_no_dev_get_device_info
};

#else  /* !FS_HOTPLUG_USB */
/* Stub functions for builds where there is no USB MS support. */
void hotplug_usbhost_register(void) {}
void hotplug_usbhost_open (void) {}
void hotplug_usbhost_close (void) {}
#endif
