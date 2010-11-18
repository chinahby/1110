/***********************************************************************
 * fs_blockdev_sd.c
 *
 * Blockdev SD device driver.
 * Copyright (C) 2006 - 2009 QUALCOMM, Inc.
 *
 * Contains implementation for the blockdev_ops functions specific to
 * the SD driver.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_blockdev_sd.c#5 $ $DateTime: 2009/07/08 13:11:36 $ $Author: ebuller $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-07-08   ebb   Moved to handle based apis for get_device_info calls.
2009-07-01   ebb   Update was_present flag after successful open.
2009-07-01   ebb   Fix incorrect HAVE_SDCC_HANDLE_SUPPORT flag.
2009-06-12   ebb   Created macro for sdcc_init when external DM not avail.
2009-05-15   ebb   Added sdcc handle support to all APIs.
2009-05-08   ebb   Migrated from hotplug interface.
2008-12-16   yg    Use modifed info structure.
2008-12-12   rp    Fix compiler warning about unreachable statement.
2008-12-12   rp    Added write_iovec, reliable write functions to hdev.
2008-11-24   yog   Added hotplug_sd_idle_device() for new SDCC API call.
2008-11-06   sh    Added Babylon markers for SDCC study
2008-03-11   yg    Added HAVE flag around the new SDCC API call invokation.
2008-03-07   yg    Remove SD LOCK that's common across all slots.
2008-03-06   sch   Make the device unusable if its not a memory device
2008-03-06   sh    Reformat hotplug_sd_get_total_slots().
2008-02-28   yg    Do not close the device if open didn't succeed.
2008-01-14   yg    Corrected the SDCC driver API feature name.
2008-01-14   yg    Corrected the SDCC driver API return type and error code.
2007-11-28   rp    Corrected the multiple SD slots feature.
2007-11-05   rp    Fixed compiler warning in hotplug_sd_reset().
2007-10-09   sch   Added support for multiple SD devices
2007-04-02   rp    Corrected the mmc_send_status() function prototype.
2006-11-09   sch   Implemented common functions b/w CPRM and normal SD
2006-07-12   s h   Replace FS_GLOBAL_LOCK with SDCC critical section
2006-06-25   s h   Replaced sd-specific "SD_STATE" with was_present
2006-06-28   s h   Hold FS_GLOBAL lock only for SD's is_present()
2006-06-06   s h   Do not allow read/write on closed sd card.
2006-06-02   s h   Added reset handler
2006-05-23   s h   Remove redundant INIT call from is_present
2006-05-23   sch   Initialized the SD controller before opening
2006-05-17   s h   Always leave a missing card in CLOSED state
2006-05-17   s h   Careful selection between SDCC/MMC
2006-05-12   s h   Split 7500 SDCC from normal SDCC.
2006-05-12   s h   Added 7500-specific naming for SDCC.
2006-05-10   s h   Replaced DRV_GEOMETRY_DESC with block size & count
2006-05-08   s h   Removed umount from device-specific calls.
2006-05-04   s h   Reworked open/close logic to account for sdcc closing
2006-05-04   s h   Protect against redundant open/close
2006-04-24   s h   Renamed control flag to FS_HOTPLUG_SD
2006-04-22   s h   Moved all private definitions into fs_hotplug_i.h
2006-04-14   s h   Corrected logic bug in SD detect.
2006-04-13   s h   Include assert.h
2006-03-31   sch   Modified hotplug_sd_is_present() to include device open
2006-03-31   s h   Unified MMC & SD through macros
2006-03-23   s h   Created by moving sd-specific functions out of hotplug
===========================================================================*/

#include "customer.h"
#include "fs_blockdev_i.h"

#ifdef FS_BLOCKDEV_SD

#include "fs_blockdev_sd.h"
#include "fs_babylon.h"
#include "fs_err.h"

#include "assert.h"
#include "sdcc_api.h"
#include "msg.h"
#include <string.h>

#ifdef FS_UNIT_TEST
#error code not present
#endif

/* If MSG_HIGH is not available */
#ifndef MSG_HIGH
  #define MSG_HIGH(fmt,a,b,c)
#endif

/* We only want to pass driveno if using external DM */
#ifdef FEATURE_SDCC_EXTERNAL_DM
  #define SDCC_INIT(x) sdcc_init(x)
#else
  #define SDCC_INIT(x) sdcc_init()
#endif

/* Bump this for any code changes in the file */
#define SD_DRIVER_VERSION            0x0001

/* We maintain this only for backwards compatibility, as soon as non
 * handle API is deprecated we can remove this. */
#ifndef HAVE_SDCC_HANDLE_SUPPORT
  extern int mmc_send_status (uint16);
#endif

/* By default we shall assume that there is just 1 SD slot. */
#define FS_DEFAULT_MAX_SD_SLOTS       1

#define FS_MAX_SDCC_IOVEC_ITEMS       FS_MAX_IOVEC_ITEMS_COUNT

#ifdef HAVE_SDCC_IOVEC_SUPPORT
  /*  IOVEC buffers to be passed to SDCC to do the buffer writes in one shot.*/
  static sdcc_memory_iovec_data_desc_type sdcc_iovec[FS_MAX_SDCC_IOVEC_ITEMS];
#endif

/* Maximum number of SD-Slots present in this device. */
static int max_sd_slots; 

/* Device operations */
static int fs_sd_open (struct block_device *, void *);
static int fs_sd_close (struct block_device *);
static int fs_sd_read (struct block_device *, uint32, unsigned char *, uint16);
static int fs_sd_write (struct block_device *, uint32,
                        unsigned char *, uint16);
static int fs_sd_reset (struct block_device *);
static int fs_sd_set_device_idle (struct block_device *);
static int fs_sd_get_device_idle (struct block_device *);
static int fs_sd_success_stub (struct block_device *);
static int fs_sd_read_iovec_fail (struct block_device *, fs_iovec_t , uint32);
static int fs_sd_erase_fail (struct block_device *, uint32, uint16);
static int fs_sd_is_present (struct block_device *);
static int fs_sd_get_device_size (struct block_device *, uint32 *, uint16 *);
static int fs_sd_get_device_info (struct block_device *,
                                  struct block_device_info *);
static int fs_sd_write_iovec (struct block_device *, fs_iovec_t, uint32, int);
static int fs_sd_reliable_write (struct block_device *, uint32, uint32 *,
                                 uint32);

/* Driver functions */
static void fs_sd_init (struct block_device **, uint16 *);
static void fs_sd_cleanup (void);


/*------------------------
 * Block Device Structures
 *------------------------*/

/* List of all device operations */
static blockdev_ops sd_ops =
{
  fs_sd_open,
  fs_sd_close,
  fs_sd_read,
  fs_sd_write,
  fs_sd_write,
  fs_sd_read_iovec_fail,
  fs_sd_write_iovec,
  fs_sd_reliable_write,
  fs_sd_erase_fail,
  fs_sd_reset,
  fs_sd_success_stub,          /* device format */
  fs_sd_is_present,
  fs_sd_get_device_size,
  fs_sd_get_device_info,
  fs_sd_set_device_idle,
  fs_sd_get_device_idle
};


/* Storage for SD specific data */
static sd_device_data sd_dev_data[] =
{
  {0, 0, 0, 0, NULL}, /* driveno, is_open, was_present, device_idle, handle */
  {1, 0, 0, 0, NULL},
  {2, 0, 0, 0, NULL},
  {3, 0, 0, 0, NULL},
#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */
};


/* List of sd block devices to manage */
static struct block_device devices[] =
{
  {"/mmc1", &sd_ops, (void *)&sd_dev_data[0], NULL,
   BLOCKDEV_TYPE_MMC, 0, 0xFF, CANARY},
  {"/mmc2", &sd_ops, (void *)&sd_dev_data[1], NULL,
   BLOCKDEV_TYPE_MMC, 0, 0xFF, CANARY},
  {"/mmc3", &sd_ops, (void *)&sd_dev_data[2], NULL,
   BLOCKDEV_TYPE_MMC, 0, 0xFF, CANARY},
  {"/mmc4", &sd_ops, (void *)&sd_dev_data[3], NULL,
   BLOCKDEV_TYPE_MMC, 0, 0xFF, CANARY},

/* Phony mnand device for on target testing, notice that we are initializing
 * this device to be hidden from the start. This will ensure hotplug and/or
 * other clients don't pick it up when iterating the devices. */
#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */
};

#define NUM_DEVICES   (sizeof (devices) / sizeof (devices[0]))
#define MAX_SD_SLOTS  (int16)(sizeof(devices)/sizeof(devices[0]))

/* Declare the driver for this module, this will be the only thing
 * exposed to the blockdev layer above. */
blockdev_driver blockdev_sd_driver =
{
  "BDEV_SD_DRIVER",     /* char *name */
  BLOCKDEV_TYPE_MMC,    /* block_device_type */
  SD_DRIVER_VERSION,    /* uint16 version */
  fs_sd_init,           /* init fn */
  fs_sd_cleanup         /* cleanup fn */
};

/*--------------------
 * SD Driver functions
 *--------------------*/

/* This should populate the blockdev linked list */
static void
fs_sd_init (struct block_device **head, uint16 *dev_id)
{
  int i;

  /* Check to ensure the number of private data entries matches
   * the number of devices. */
  if (NUM_DEVICES != sizeof (sd_dev_data) / sizeof (sd_dev_data[0]))
    FS_ERR_FATAL ("Number of block devices MUST match private data!", 0,0,0);

  /* Link the list of devices together */
  MSG_FATAL("fs_sd_init............................",0,0,0);
  for (i = NUM_DEVICES - 1; i >= 0; i--)
  {
    struct block_device *dev = &devices[i];
    sd_device_data *sdev = (sd_device_data *)dev->device_data;

    /* Add device to beginning of the linked list */
    dev->next = *head;
    dev->device_id = (*dev_id)++;
    sdev->is_open = 0;
    sdev->was_present = 0;
    sdev->device_idle_state = FALSE;
    sdev->handle = NULL;
    *head = dev;
  }
}

static void
fs_sd_cleanup (void)
{
  /* Do nothing as of now */
}


/*---------------------
 * SD Device Operations
 *---------------------*/

/* Internal function used to send/recv sd status */
static int
fs_sd_status (struct block_device *bdev)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  /* Make sure that the slot is a valid one. */
  ASSERT (dev->driveno < max_sd_slots);
  dev->device_idle_state = FALSE;

#ifdef HAVE_SDCC_HANDLE_SUPPORT
  ASSERT (dev->handle != NULL);
  return sdcc_handle_is_mem_present (dev->handle);
#else
  return mmc_send_status (dev->driveno);
#endif /* HAVE_SDCC_HANDLE_SUPPORT */
}

/* Get the size of the device */
static int
fs_sd_get_device_size (struct block_device *bdev, uint32 *blocks,
                       uint16 *bytes_per_block)
{
  int rv;
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  /* Make sure that the slot is a valid one. */
  ASSERT (dev->driveno < max_sd_slots);
  dev->device_idle_state = FALSE;

  /* Check if the newer sdcc api is available */
#ifdef HAVE_SDCC_GET_MEMORY_DEVICE_INFO
  {
    sdcc_mem_info_type mem_info;
    SDCC_STATUS result;

/* If using the handle based API  there is a requirement to open
 * the device before calling any handle based API. To stay backwards
 * compatible we do the transparent open here if the device has
 * not been opened yet.
 *
 * XXX: Fix once we have designed in multiple handle support
 */
#  ifdef HAVE_SDCC_HANDLE_SUPPORT

    if (bdev->ops->open (bdev, NULL) != 0)
      MSG_HIGH ("blockdev open failed!",0,0,0);

    ASSERT (dev->handle != NULL);
    result = sdcc_handle_mem_get_device_info (dev->handle, &mem_info);
#  else
    result = sdcc_mem_get_device_info (dev->driveno, &mem_info);
#  endif /* HAVE_SDCC_HANDLE_SUPPORT */

    *blocks = mem_info.card_size_in_sectors;
    *bytes_per_block = mem_info.block_len;

    rv = (result == SDCC_NO_ERROR) ? 0 : -1;
  }
#else /* !HAVE_SDCC_GET_MEMORY_DEVICE_INFO */
  {
    struct drv_geometry_desc fat_geom;
    DRV_GEOMETRY_DESC *desc = (DRV_GEOMETRY_DESC *) &fat_geom;

    rv = sdcc_read_serial (dev->driveno, desc);

    *blocks = desc->totalLBA;
    *bytes_per_block = desc->dfltBytesPerSect;

    rv = (rv == 0) ? -1 : 0;
  }
#endif

  return rv;
}

/*
 * Returns TRUE if an SD card is present in this device.
 *
 * If we had a HARDWARE physical card-presence switch, that would be a
 * great help here.  Then, the logic would first check the physical
 * switch (GPIO?) to see if it should bother looking for a card.  If
 * the physical switch was 'empty' we could return immediately without
 * doing a costly and slow poll.
 *
 * If the switch showed something positively in the slot, then we
 * would do the device poll to confirm a good card was found.
 *
 * Maybe future Surfs will have this switch.  (Customers are
 * encouraged to improve this function to detect a card more quickly
 * on their particular design.)
 *
 *
 * As far as we know, mmc_send_status() returns 1 when a card is
 * present, zero when not present.
 *
 * A newly (re)inserted card must be initialized.  The only time the
 * card status is actually fetched is when sdcc_open() is called, so
 * we must close and open the card each time.  The code attempts to
 * leave an empty slot in the 'closed' state (returns FALSE).
 *
 * A slot with a detected card in it is left in the 'open' state when
 * we return TRUE.
 */

static int
fs_sd_is_present (struct block_device *bdev)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  /* Make sure that the slot is a valid one. */
  ASSERT (dev->driveno < max_sd_slots);
  dev->device_idle_state = FALSE;

  if (dev->was_present)
  {
    /* Check if the card that was there earlier is now gone/changed */
    if (fs_sd_status (bdev) == 0) {
      dev->was_present = FALSE;
      (void) bdev->ops->close (bdev); /* CLOSE it */
    }
  }
  else
  {
    /* If both of these succeed, then the card is now present: */
    if (bdev->ops->open (bdev, NULL) == 0)
    {
      if (fs_sd_status (bdev) != 0)
        dev->was_present = TRUE;
      else
        (void) bdev->ops->close (bdev); /* Leave it CLOSED */
    }
  }

  return dev->was_present;
}

/* Open the MMC/SD device.  Do nothing if it's already open. */
static int
fs_sd_open (struct block_device *bdev, void *data)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;
  MSG_FATAL("fs_sd_open.....................",0,0,0);
  /* Make sure that the slot is a valid one. */
  ASSERT (dev->driveno < max_sd_slots);
  dev->device_idle_state = FALSE;

  if ((! dev->is_open)) {

#ifdef HAVE_SDCC_HANDLE_SUPPORT
    uint32 phy_part_num = (uint32)data;

    dev->handle = sdcc_handle_open (dev->driveno, phy_part_num);
    dev->is_open = dev->handle ? 1 : 0;
    dev->was_present = dev->handle ? TRUE : FALSE;
#else
    (void) data;
    MSG_FATAL("fs_sd_open fist.....................",0,0,0);
    /* Every sd open() call needs an INIT first, so do it here: */
    SDCC_INIT (dev->driveno);

    /* If sdcc_open fails, then it leaves the card "closed" */
    dev->is_open = !!sdcc_open (dev->driveno); 
#endif /* HAVE_SDCC_HANDLE_SUPPORT */
  }
  return dev->is_open ? 0 : -1;
}

/* Close the MMC/SD device.  Do nothing if it's already closed. */
static int
fs_sd_close (struct block_device *bdev)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  /* Make sure that the slot is a valid one. */
  ASSERT (dev->driveno < max_sd_slots);
  dev->device_idle_state = FALSE;

  if (dev->is_open) {
#ifdef HAVE_SDCC_HANDLE_SUPPORT
    SDCC_STATUS rv;

    ASSERT (dev->handle != NULL);
    rv = sdcc_handle_close (dev->handle);
    dev->is_open = (rv == SDCC_NO_ERROR) ? 0 : 1;
    if (!dev->is_open)
      dev->handle = NULL;
#else
    dev->is_open = !sdcc_close (dev->driveno);
#endif /* HAVE_SDCC_HANDLE_SUPPORT */

    dev->was_present = FALSE;
  }
  return dev->is_open ? -1 : 0;
}


/* Do not permit read operations if the device is closed */
static int
fs_sd_read (struct block_device *bdev, uint32 lba, unsigned char *buf,
            uint16 n_to_read)
{
  int result;
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  /* Make sure that the slot is a valid one. */
  ASSERT (dev->driveno < max_sd_slots);
  dev->device_idle_state = FALSE;

  if (dev->is_open) {
    EFS_BAB_START (EFS_BAB_SDCC_READ);
    EFS_BAB_VALUE (EFS_BAB_SDCC_READ, lba,       TRUE, FALSE, TRUE);
    EFS_BAB_VALUE (EFS_BAB_SDCC_READ, n_to_read, TRUE, FALSE, FALSE);

#ifdef HAVE_SDCC_HANDLE_SUPPORT
    {
      SDCC_STATUS rv;
      ASSERT (dev->handle != NULL);
      rv = sdcc_handle_read (dev->handle, lba, buf, n_to_read);
      result = (rv == SDCC_NO_ERROR) ? 0 : -1;
    }
#else
    result =
      (sdcc_read (dev->driveno, lba, buf, n_to_read) != 0) ? 0 : -1;
#endif /* HAVE_SDCC_HANDLE_SUPPORT */

    EFS_BAB_END (EFS_BAB_SDCC_READ);
  }
  else
    result = -1;

  return result;
}

/* Do not permit write operations if the device is closed */
static int
fs_sd_write (struct block_device *bdev, uint32 lba, unsigned char *buf,
             uint16 n_to_write)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;
  int result;

  /* Make sure that the slot is a valid one. */
  ASSERT (dev->driveno < max_sd_slots);
  dev->device_idle_state = FALSE;

  if (dev->is_open) {
    EFS_BAB_START (EFS_BAB_SDCC_WRITE);
    EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE, lba, TRUE, FALSE, TRUE);
    EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE, n_to_write, TRUE, FALSE, FALSE);

#ifdef HAVE_SDCC_HANDLE_SUPPORT
    {
      SDCC_STATUS rv;
      ASSERT (dev->handle != NULL);
      rv = sdcc_handle_write (dev->handle, lba, buf, n_to_write);
      result = (rv == SDCC_NO_ERROR) ? 0 : -1;
    }
#else
    result = (sdcc_write (dev->driveno, lba, buf, n_to_write) != 0) ? 0 : -1;
#endif /* HAVE_SDCC_HANDLE_SUPPORT */

    EFS_BAB_END (EFS_BAB_SDCC_WRITE);
  }
  else
    result = -1;

  return result;
}

/* Get the total number of SD-Slots present in this target, If we can't
   figure out this using the SDCC API, then we shall assume that there
   is just 1 SD slot. */
static int
fs_sd_get_total_slots (void)
{
  int num_slots = FS_DEFAULT_MAX_SD_SLOTS;

#ifdef FEATURE_SDCC_HAVE_CONFIGURATION_API
  {
    sdcc_config_info_type sdcc_pconfig;

    /* Find out the total number of SD slots that are present on-target. */
    if (sdcc_get_config_info (&sdcc_pconfig) == SDCC_NO_ERROR)
    {
      num_slots = sdcc_pconfig.num_slots;
    }
  }
#endif

  return num_slots;
}

static int
fs_sd_reset (struct block_device *bdev)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;
  max_sd_slots = fs_sd_get_total_slots ();
  dev->device_idle_state = FALSE;

  if (dev->driveno >= max_sd_slots)
  {
    /* This slot is not present, hence return -1 so that the hotplug-manager
       marks this device as DEV_UNUSABLE and will not poll this device in the
       future. */
    return -1;
  }

#ifdef HAVE_SDCC_SLOT_HAS_SUPPORT
  /* If this slot is enabled, but is not compatible for memory card
   * then also mark this device as not usable. */
  if (sdcc_slot_has_support (dev->driveno, SDCC_SLOT_MEMORY) == FALSE)
    return -1;
#endif

  return 0;
}

/* Used for power saving by notifying hardware that device is not getting
 * used frequently */
static int
fs_sd_set_device_idle (struct block_device *bdev)
{
#ifdef HAVE_SDCC_POWER_COLLAPSE
  sd_device_data *dev = (sd_device_data *)bdev->device_data;
  ASSERT (dev->driveno < max_sd_slots);

  if (dev->device_idle_state)
  {
    /* If the HAVE_SDCC_POWER_COLLAPSE is being defined then call the
     * corresponding timeout_event function of the SD.
     *
     * Ignore the return status of the sdcc_activity_timeout_event () as
     * per specified by the SDCC team as on hotplug side we don't have to care
     * about the return status of this api. */
    (void) sdcc_activity_timeout_event(dev->driveno);
  }
  else
  {
    dev->device_idle_state = TRUE;
  }

  return 0;
#else /* !HAVE_SDCC_POWER_COLLAPSE */
  (void) bdev;

  /* Silently ignore */
  return 0;
#endif
}

static int
fs_sd_get_device_idle (struct block_device *bdev)
{
#ifdef HAVE_SDCC_POWER_COLLAPSE
  sd_device_data *dev = (sd_device_data *)bdev->device_data;
  ASSERT (dev->driveno < max_sd_slots);

  return dev->device_idle_state;
#else /* !HAVE_SDCC_POWER_COLLAPSE */
  (void) bdev;

  /* Not supported */
  return -1;
#endif
}

#ifdef HAVE_SDCC_IOVEC_SUPPORT
static uint32
iovec_local_memcpy (fs_iovec_t iovec, uint32 iovec_cnt)
{
  uint32 i, total_sect = 0;

  ASSERT (iovec_cnt < FS_MAX_SDCC_IOVEC_ITEMS);

  for (i = 0; i < iovec_cnt; i++)
  {
    sdcc_iovec[i].sector_address = iovec[i].sector_id;
    sdcc_iovec[i].buffer = iovec[i].buff_base;
    sdcc_iovec[i].num_sector = EFS_PAGE_SIZE / FS_SDCC_SECTOR_SIZE_IN_BYTES;
    total_sect += sdcc_iovec[i].num_sector;
  }

  return total_sect;
}
#endif

static int
fs_sd_write_iovec (struct block_device *bdev, fs_iovec_t iovec,
                   uint32 iovec_cnt, int reliable_write)
{
#ifdef HAVE_SDCC_IOVEC_SUPPORT
  sdcc_memory_iovec_transfer_type write_type;
  SDCC_STATUS sdcc_res;
  int result;
  uint32 total_sect;
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  /* Make sure that the slot is a valid one. */
  ASSERT (dev->driveno < max_sd_slots);
  dev->device_idle_state = FALSE;

  /* Do not permit write operations if the device is closed */
  if (!dev->is_open)
    return -1;

  /* Copy the supplied FS-iovec to our static SDCC-iovec. */
  total_sect = iovec_local_memcpy (iovec, iovec_cnt);

  if (reliable_write)
    write_type = SDCC_MEMORY_IOVEC_RELIABLE_WRITE;
  else
    write_type = SDCC_MEMORY_IOVEC_REGULAR_DATA_XFER;

  EFS_BAB_START (EFS_BAB_SDCC_WRITE_IOVEC);
  EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE_IOVEC, iovec_cnt, TRUE, FALSE, TRUE);
  EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE_IOVEC, total_sect, TRUE, FALSE, FALSE);
  (void) total_sect; /* If Babylon if turned off, then to keep compiler happy*/

#ifdef HAVE_SDCC_HANDLE_SUPPORT
  ASSERT (dev->handle != NULL);
  sdcc_res = sdcc_handle_write_iovec (dev->handle, sdcc_iovec, iovec_cnt,
                                      write_type);
#else
  sdcc_res = sdcc_write_iovec (dev->driveno, sdcc_iovec,
                               iovec_cnt, write_type);
#endif /* HAVE_SDCC_HANDLE_SUPPORT */

  EFS_BAB_END (EFS_BAB_SDCC_WRITE_IOVEC);

  if (sdcc_res != SDCC_NO_ERROR)
    MSG_HIGH ("sdcc_write_iovec failed...",0,0,0);

  result = (sdcc_res == SDCC_NO_ERROR) ? 0 : -1;
  return result;

#else /* !HAVE_SDCC_IOVEC_SUPPORT */
  (void) bdev; (void) iovec;
  (void) iovec_cnt; (void) reliable_write;

  return -1;
#endif
}

static int
fs_sd_reliable_write (struct block_device *bdev, uint32 lba,
                      uint32 *buf, uint32 num_sectors)
{
#ifdef HAVE_SDCC_IOVEC_SUPPORT
  SDCC_STATUS sdcc_res;
  int result;
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  /* Make sure that the slot is a valid one. */
  ASSERT (dev->driveno < max_sd_slots);
  dev->device_idle_state = FALSE;

  /* Do not permit write operations if the device is closed */
  if (!dev->is_open)
    return -1;

  sdcc_iovec[0].sector_address = lba;
  sdcc_iovec[0].buffer = buf;
  sdcc_iovec[0].num_sector = num_sectors;

  EFS_BAB_START (EFS_BAB_SDCC_WRITE_REL);
  EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE_REL, lba, TRUE, FALSE, TRUE);
  EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE_REL, num_sectors, TRUE, FALSE, FALSE);

#ifdef HAVE_SDCC_HANDLE_SUPPORT
  ASSERT (dev->handle != NULL);
  sdcc_res = sdcc_handle_write_iovec (dev->handle, sdcc_iovec, 1,
                                      SDCC_MEMORY_IOVEC_RELIABLE_WRITE);
#else
  sdcc_res = sdcc_write_iovec (dev->driveno, sdcc_iovec, 1,
                               SDCC_MEMORY_IOVEC_RELIABLE_WRITE);
#endif /* HAVE_SDCC_HANDLE_SUPPORT */

  EFS_BAB_END (EFS_BAB_SDCC_WRITE_REL);

  if (sdcc_res != SDCC_NO_ERROR)
    MSG_HIGH ("sdcc_write_iovec failed...",0,0,0);

  result = (sdcc_res == SDCC_NO_ERROR) ? 0 : -1;

  return result;
#else
  (void) bdev; (void) lba; (void) buf; (void) num_sectors;

  return -1;
#endif
}


static int
fs_sd_get_device_info (struct block_device *bdev,
                       struct block_device_info *dev_info)
{
#ifdef HAVE_SDCC_GET_MEMORY_DEVICE_INFO
  sdcc_mem_info_type sdcc_info;
  SDCC_STATUS sdcc_res;
  int result;
  sd_device_data *dev = (sd_device_data *)bdev->device_data;
  dev->device_idle_state = FALSE;

/* If using the handle based API  there is a requirement to open
 * the device before calling any handle based API. To stay backwards
 * compatible we do the transparent open here if the device has
 * not been opened yet.
 *
 * XXX: Fix once we have designed in multiple handle support
 */
#ifdef HAVE_SDCC_HANDLE_SUPPORT

  if (bdev->ops->open (bdev, NULL) != 0)
    MSG_HIGH ("blockdev open failed!",0,0,0);

  ASSERT (dev->handle != NULL);
  sdcc_res = sdcc_handle_mem_get_device_info (dev->handle, &sdcc_info);
#else
  sdcc_res = sdcc_mem_get_device_info (dev->driveno, &sdcc_info);
#endif /* HAVE_SDCC_HANDLE_SUPPORT */

  if (sdcc_res != SDCC_NO_ERROR)
    MSG_HIGH ("sdcc_mem_get_device_info failed...",0,0,0);
  result = (sdcc_res == SDCC_NO_ERROR) ? 0 : -1;

  if (sdcc_res == SDCC_NO_ERROR)
  {
    memset (dev_info, 0, sizeof (*dev_info));
    dev_info->manufacturer_id = sdcc_info.mfr_id;
    dev_info->oem_id = sdcc_info.oem_id;
    dev_info->prod_rev = sdcc_info.prod_rev;
    dev_info->prod_serial_num = sdcc_info.prod_serial_num;
    memcpy (dev_info->product_name, sdcc_info.prod_name,
            MIN (sizeof (dev_info->product_name),
                 sizeof (sdcc_info.prod_name)));
    memcpy (dev_info->manufactured_date, sdcc_info.mfr_date,
            MIN (sizeof (dev_info->manufactured_date),
                 sizeof (sdcc_info.mfr_date)));
    }

  return result;
#else /* !HAVE_SDCC_GET_MEMORY_DEVICE_INFO */
  (void) bdev; (void) dev_info;

  return -1;
#endif
}


/*---------------------------
 * Stubs for unavailable APIs
 *---------------------------*/
static int
fs_sd_success_stub (struct block_device *bdev)
{
  (void) bdev;
  return 0;
}

static int
fs_sd_read_iovec_fail (struct block_device *bdev, fs_iovec_t iovec,
                       uint32 iovec_cnt)
{
  (void) bdev; (void) iovec; (void) iovec_cnt;
  return -1;
}

static int
fs_sd_erase_fail (struct block_device *bdev, uint32 lba, uint16 erase_len)
{
  (void) bdev; (void) lba; (void) erase_len;
  return -1;                    /* Unsupported */
}

#else
extern int __dont_complain_about_empty_file;
#endif /* FS_BLOCKDEV_SD */
