/***********************************************************************
 * fs_blockdev_usb.c
 *
 * Blockdev USB device driver.
 * Copyright (C) 2006, 2008 - 2009 QUALCOMM, Inc.
 *
 * Contains implementation for the blockdev_ops functions specific to
 * the USB driver.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_blockdev_usb.c#3 $ $DateTime: 2009/05/20 15:11:31 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-15   ebb   Fixed integer conversion on client_id initialization.
2009-05-15   ebb   Added void * arg to usb open stub.
2009-05-08   ebb   Migrated from hotplug framework
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
#include "fs_blockdev_i.h"

/* Hotplug supports USB Host Mass Storage.. ie, when the MSM controls
 * an external USB device like a thumbdrive, hard drive, or card
 * reader.  */
#ifdef FS_BLOCKDEV_USB

#include "fs_blockdev_usb.h"
#include "fs_err.h"

#include "jzap.h"
#include "msg.h"
#include "usbhost_api.h"

/* Unique ID that USBHOST uses to recognize clients (like Hotplug) */
usbhost_client_id_type client_id = USBHOST_INVALID_CLIENT_ID;

/* Bump this for any code changes */
#define USB_DRIVER_VERSION        0x0001

/* Driver functions */
static void fs_usb_init (struct block_device **, uint16 *);
static void fs_usb_cleanup (void);

/* Device IO functions */
static int fs_usb_read (struct block_device *, uint32, unsigned char *,
                        uint16);
static int fs_usb_write (struct block_device *, uint32, unsigned char *,
                         uint16);
static int fs_usb_get_size (struct block_device *, uint32 *blocks,
                            uint16 *bytes_per_block);
static int fs_usb_is_present (struct block_device *);

/* Stubs */
static int fs_usb_success_stub (struct block_device *);
static int fs_usb_open_stub (struct block_device *, void *);
static int fs_usb_read_iovec_fail (struct block_device *, fs_iovec_t, uint32);
static int fs_usb_write_iovec_fail (struct block_device *, fs_iovec_t,
                                    uint32, int);
static int fs_usb_reliable_write_fail (struct block_device *, uint32,
                                       uint32 *, uint32);
static int fs_usb_erase_fail (struct block_device *, uint32, uint16);
static int fs_usb_get_device_info_fail (struct block_device *,
                                        struct block_device_info *);
static int fs_usb_device_idle_fail (struct block_device *);

/* USB operations */
static blockdev_ops usb_ops =
{
  fs_usb_open_stub,            /* open */
  fs_usb_success_stub,         /* close */
  fs_usb_read,                 /* read */
  fs_usb_write,                /* write */
  fs_usb_write,                /* user write */
  fs_usb_read_iovec_fail,      /* read iovec */
  fs_usb_write_iovec_fail,     /* write iovec */
  fs_usb_reliable_write_fail,  /* reliable write */
  fs_usb_erase_fail,           /* erase */
  fs_usb_success_stub,         /* reset */
  fs_usb_success_stub,         /* device format */
  fs_usb_is_present,           /* device present */
  fs_usb_get_size,             /* device size */
  fs_usb_get_device_info_fail, /* get_device_info */
  fs_usb_device_idle_fail,     /* set_device_idle */
  fs_usb_device_idle_fail      /* set_device_idle */
};

/* Initialize all usb specific data */
static usb_device_data usb_dev_data[] =
{
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
  {USB_NO_INSTANCE, USB_NO_HANDLE, USBSTATUS_UNCONNECTED},
};

/* Devices this driver manages */
static struct block_device devices[] =
{
  {"/usb1", &usb_ops, (void *)&usb_dev_data[0],
   NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, CANARY},
  {"/usb2", &usb_ops, (void *)&usb_dev_data[1],
   NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, CANARY},
  {"/usb3", &usb_ops, (void *)&usb_dev_data[2],
   NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, CANARY},
  {"/usb4", &usb_ops, (void *)&usb_dev_data[3],
   NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, CANARY},
  {"/usb5", &usb_ops, (void *)&usb_dev_data[4],
   NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, CANARY},
  {"/usb6", &usb_ops, (void *)&usb_dev_data[5],
   NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, CANARY},
  {"/usb7", &usb_ops, (void *)&usb_dev_data[6],
   NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, CANARY},
  {"/usb8", &usb_ops, (void *)&usb_dev_data[7],
   NULL, BLOCKDEV_TYPE_USB_MS, 0, 0xFF, CANARY},
};

#define NUM_DEVICES (sizeof (devices) / sizeof (devices[0]))

/* Driver object to be exposed to the upper layer */
blockdev_driver blockdev_usb_driver =
{
  "BDEV_USB_DRIVER",    /* char *name */
  BLOCKDEV_TYPE_USB_MS, /* block_device type */
  USB_DRIVER_VERSION,   /* uint16 version */
  fs_usb_init,          /* init fn */
  fs_usb_cleanup        /* cleanup fn */
};



/*--------------------
 * USB Driver functions
 *--------------------*/

/* This should populate the blockdev linked list */
static void
fs_usb_init (struct block_device **head, uint16 *dev_id)
{
  int i;

  /* Check to ensure the number of private data entries matches
   * the number of devices. */
  if (NUM_DEVICES != sizeof (usb_dev_data) / sizeof (usb_dev_data[0]))
    FS_ERR_FATAL ("Number of block devices MUST match private data!", 0,0,0);

  /* Link the list of devices together */
  for (i = NUM_DEVICES - 1; i >= 0; i--)
  {
    struct block_device *dev = &devices[i];
    usb_device_data *udev = (usb_device_data *)dev->device_data;

    /* Add device to beginning of the linked list */
    dev->next = *head;
    dev->device_id = *(dev_id)++;
    udev->usb_instance = USB_NO_INSTANCE;
    udev->usb_handle = USB_NO_HANDLE;
    udev->usb_state = USBSTATUS_UNCONNECTED;
    *head = dev;
  }
}

static void
fs_usb_cleanup (void)
{
  /* Do nothing as of now */
}


/*---------------------
 * USB Device Operations
 *---------------------*/

static int
fs_usb_read (struct block_device *bdev, uint32 lba, unsigned char *buf,
             uint16 n_to_read)
{
  usbhost_IoctlTransfer ms_data;
  usb_device_data *dev = (usb_device_data *)bdev->device_data;

  ms_data.numberBlocks = n_to_read;
  ms_data.startBlock = lba;
  ms_data.buffer = buf;
  ms_data.length = n_to_read * 512;

  if ((dev->usb_handle != USB_NO_HANDLE)
      && (USBHOST_SUCCESS == usbhost_ioctl (client_id, dev->usb_handle,
                                            USBHOST_IOCTL_STOR_BLOCK_READ,
                                            (void *) &ms_data)))
    return 0;                   /* Good! */
  else
    return -1;                  /* Read failed */
}

static int
fs_usb_write (struct block_device *bdev, uint32 lba, unsigned char *buf,
              uint16 n_to_write)
{
  usbhost_IoctlTransfer ms_data;
  usb_device_data *dev = (usb_device_data *)bdev->device_data;

  ms_data.numberBlocks = n_to_write;
  ms_data.startBlock = lba;
  ms_data.buffer = buf;
  ms_data.length = n_to_write * 512;

  if ((dev->usb_handle != USB_NO_HANDLE)
      && (USBHOST_SUCCESS == usbhost_ioctl (client_id, dev->usb_handle,
                                            USBHOST_IOCTL_STOR_BLOCK_WRITE,
                                            (void *) &ms_data)))
    return 0;                   /* Good! */
  else
    return -1;                  /* Write failed */
}

static int
fs_usb_get_size (struct block_device *bdev, uint32 *blocks,
                 uint16 *bytes_per_block)
{
  uint32 count;
  usbhost_status_type status;
  usb_device_data *dev = (usb_device_data *)bdev->device_data;

  if (dev->usb_handle == USB_NO_HANDLE)
    return -1;

  status = usbhost_ioctl (client_id, dev->usb_handle,
                          USBHOST_IOCTL_STOR_BLOCK_SIZE, (void *) &count);
  if (status == USBHOST_ERROR)
    return -1;
  *bytes_per_block = count;

  status = usbhost_ioctl (client_id, dev->usb_handle,
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
fs_usb_is_present (struct block_device *bdev)
{
  usbhost_status_type status;
  int ready;
  usb_device_data *dev = (usb_device_data *)bdev->device_data;

  if (dev->usb_state != USBSTATUS_OPEN)
    return 0;

  /* This will check if the media has changed or become unavailable */
  status = usbhost_ioctl (client_id, dev->usb_handle,
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
    status = usbhost_ioctl (client_id, dev->usb_handle,
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

/***************
 *  Noop Stubs
 **************/

static int
fs_usb_success_stub (struct block_device *bdev)
{
  (void) bdev;
  return 0;
}

static int
fs_usb_open_stub (struct block_device *bdev, void *data)
{
  (void) bdev; (void) data;
  return 0;
}

static int
fs_usb_read_iovec_fail (struct block_device *bdev, fs_iovec_t iovec,
                        uint32 iovec_cnt)
{
  (void) bdev; (void) iovec; (void) iovec_cnt;
  return -1;
}

static int
fs_usb_write_iovec_fail (struct block_device *bdev, fs_iovec_t iovec,
                         uint32 iovec_cnt, int reliable_write)
{
  (void) bdev; (void) iovec; (void) iovec_cnt; (void) reliable_write;
  return -1;
}
static int
fs_usb_reliable_write_fail (struct block_device *bdev, uint32 lba,
                            uint32 *buf, uint32 num_sectors)
{
  (void) bdev; (void) lba; (void) buf; (void) num_sectors;
  return -1;
}
static int
fs_usb_erase_fail (struct block_device *bdev, uint32 lba, uint16 erase_len)
{
  (void) bdev; (void) lba; (void) erase_len;
  return -1;
}

static int
fs_usb_device_idle_fail (struct block_device *bdev)
{
  (void) bdev;
  return -1;
}

static int
fs_usb_get_device_info_fail (struct block_device *bdev,
                             struct block_device_info *dev_info)
{
  (void) bdev; (void) dev_info;
  return -1;
}

#else
extern int __dont_complain_about_empty_file;
#endif /* FS_BLOCKDEV_USB */
