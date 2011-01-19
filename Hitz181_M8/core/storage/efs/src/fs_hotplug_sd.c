/***********************************************************************
 * fs_hotplug_sd.c
 *
 * Short description.
 * Copyright (C) 2006 - 2008 QUALCOMM, Inc.
 *
 * Verbose Description
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_sd.c#6 $ $DateTime: 2008/12/16 16:11:39 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
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

#include "fs_hotplug.h"
#include "fs_hotplug_i.h"
#include "fs_hotplug_sd.h"
#include "fs_babylon.h"
#include "fs_config.h"

#ifdef FS_HOTPLUG_SD

#include "assert.h"
#include "sdcc_api.h"

/*
 * SDCC, SDCC_DM, or MMC?
 * The APIs are all the same, but different.
 */
/* This must come first, since 7500 Apps defines both SDCC flags */
#if defined FEATURE_SDCC_EXTERNAL_DM
#  include "sdcc_api.h"
#  define MMC_READ_SERIAL     sdcc_read_serial
#  define MMC_INIT(x)         sdcc_init(x) /* 7500 *does* want the driveno */
#  define MMC_OPEN            sdcc_open
#  define MMC_CLOSE           sdcc_close
#  define MMC_READ            sdcc_read
#  define MMC_WRITE           sdcc_write

   /* XXX: Need to put this function in an SD header file, maybe? */
   extern int mmc_send_status (uint16);
#  define MMC_SEND_STATUS(x)  mmc_send_status(x)

/* SDCC without DM */
#elif defined FEATURE_DRV_SDCC
#  include "sdcc_api.h"      /* This also defines drv_geometry_desc */
#  define MMC_READ_SERIAL     sdcc_read_serial
#  define MMC_INIT(x)         sdcc_init(/* no driveno */)
#  define MMC_OPEN            sdcc_open
#  define MMC_CLOSE           sdcc_close
#  define MMC_READ            sdcc_read
#  define MMC_WRITE           sdcc_write

   extern int mmc_send_status (uint16);
#  define MMC_SEND_STATUS(x)  mmc_send_status(x)

/* MMC */
#else
#  include "mmcapi.h"        /* This also defines drv_geometry_desc */
#  define MMC_READ_SERIAL     mmc_read_serial
#  define MMC_INIT(x)         mmc_init()
#  define MMC_OPEN            mmc_drive_open
#  define MMC_CLOSE           mmc_drive_close
#  define MMC_READ            mmc_read
#  define MMC_WRITE           mmc_write

#ifdef FS_UNIT_TEST
#error code not present
#else
#  define MMC_SEND_STATUS(x)  mmc_send_status(/*void*/)
#endif /* FS_UNIT_TEST */

#endif

/* By default we shall assume that there is just 1 SD slot. */
#define FS_DEFAULT_MAX_SD_SLOTS       1

#define FS_MAX_SDCC_IOVEC_ITEMS                    (FS_MAX_IOVEC_ITEMS_COUNT)

#ifdef HAVE_SDCC_IOVEC_SUPPORT
  /*  IOVEC buffers to be passed to SDCC to do the buffer writes in one shot.*/
  static sdcc_memory_iovec_data_desc_type sdcc_iovec[FS_MAX_SDCC_IOVEC_ITEMS];
#endif

/* Maximum number of SD-Slots present in this device. */
static int max_sd_slots;

int
hotplug_sd_get_size (struct hotplug_device *hdev, uint32 *blocks,
                     uint16 *bytes_per_block)
{
  int result;
  struct drv_geometry_desc fat_geom;
  DRV_GEOMETRY_DESC *desc = (DRV_GEOMETRY_DESC *) &fat_geom;

  /* Make sure that the slot is a valid one. */
  ASSERT (hdev->driveno < max_sd_slots);

  result = MMC_READ_SERIAL (hdev->driveno, desc);

  *blocks = desc->totalLBA;
  *bytes_per_block = desc->dfltBytesPerSect;

  return (result == 0) ? -1 : 0;
}

/* We need to lock access to the SDCC driver while we fetch status,
 * since it issues an SDCC command and would corrupt any read/write in
 * progress */
static int
hotplug_sd_status (struct hotplug_device *hdev)
{
  (void) hdev;                  /* Not all macros use hdev->driveno. */

  /* Make sure that the slot is a valid one. */
  ASSERT (hdev->driveno < max_sd_slots);

  return MMC_SEND_STATUS (hdev->driveno);
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

int
hotplug_sd_is_present (struct hotplug_device *hdev)
{
  /* Make sure that the slot is a valid one. */
  ASSERT (hdev->driveno < max_sd_slots);

  if (hdev->sd_was_present)
  {
    /* Check if the card that was there earlier is now gone/changed */
    if (hotplug_sd_status (hdev) == 0) {
      hdev->sd_was_present = FALSE;
      (void) hdev->dev->close (hdev); /* CLOSE it */
    }
  }
  else
  {
    /* If both of these succeed, then the card is now present: */
    if (hdev->dev->open (hdev) == 0)
    {
      if (hotplug_sd_status (hdev) != 0)
        hdev->sd_was_present = TRUE;
      else
        (void) hdev->dev->close (hdev); /* Leave it CLOSED */
    }
  }

  return hdev->sd_was_present;
}

/* Open the MMC/SD device.  Do nothing if it's already open. */
int
hotplug_sd_open_device (struct hotplug_device *hdev)
{
  /* Make sure that the slot is a valid one. */
  ASSERT (hdev->driveno < max_sd_slots);

  if (! hdev->sd_is_open) {
    /* Every sd open() call needs an INIT first, so do it here: */
    MMC_INIT (hdev->driveno);

    /* If MMC_OPEN fails, then it leaves the card "closed" */
    hdev->sd_is_open = !!MMC_OPEN (hdev->driveno);
  }
  return hdev->sd_is_open ? 0 : -1;
}

/* Close the MMC/SD device.  Do nothing if it's already closed. */
int
hotplug_sd_close_device (struct hotplug_device *hdev)
{
  /* Make sure that the slot is a valid one. */
  ASSERT (hdev->driveno < max_sd_slots);

  if (hdev->sd_is_open) {
    hdev->sd_is_open = !MMC_CLOSE (hdev->driveno);
    /* This state needs to be modified here, because this function might
       get called from hotplug_lock_dev() */
    hdev->sd_was_present = FALSE;
  }
  return hdev->sd_is_open ? -1 : 0;
}

/* Do not permit read operations if the device is closed */
int
hotplug_sd_read (struct hotplug_device *hdev,
                 uint32 lba, unsigned char *buf, uint16 n_to_read)
{
  int result;

  /* Make sure that the slot is a valid one. */
  ASSERT (hdev->driveno < max_sd_slots);

  if (hdev->sd_is_open) {
    EFS_BAB_START (EFS_BAB_SDCC_READ);
    EFS_BAB_VALUE (EFS_BAB_SDCC_READ, lba,       TRUE, FALSE, TRUE);
    EFS_BAB_VALUE (EFS_BAB_SDCC_READ, n_to_read, TRUE, FALSE, FALSE);
    result =
      (MMC_READ (hdev->driveno, lba, buf, n_to_read) != 0) ? 0 : -1;
    EFS_BAB_END (EFS_BAB_SDCC_READ);
  }
  else
    result = -1;

  return result;
}


/* Do not permit write operations if the device is closed */
int
hotplug_sd_write (struct hotplug_device *hdev,
                  uint32 lba, unsigned char *buf, uint16 n_to_write)
{
  int result;

  /* Make sure that the slot is a valid one. */
  ASSERT (hdev->driveno < max_sd_slots);

  if (hdev->sd_is_open) {
    EFS_BAB_START (EFS_BAB_SDCC_WRITE);
    EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE, lba,       TRUE, FALSE, TRUE);
    EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE, n_to_write, TRUE, FALSE, FALSE);
    result = (MMC_WRITE (hdev->driveno, lba, buf, n_to_write) != 0) ? 0 : -1;
    EFS_BAB_END (EFS_BAB_SDCC_WRITE);
  }
  else
    result = -1;

  return result;
}

int
hotplug_sd_format (struct hotplug_device *hdev)
{
  /* Make sure that the slot is a valid one. */
  ASSERT (hdev->driveno < max_sd_slots);

  /* Make sure a card is present */
  if (!hdev->dev->is_present (hdev))
    return -1;

  ASSERT (hdev->sd_is_open);

  return 0;                     /* OK, ready to format */
}

/* Get the total number of SD-Slots present in this target, If we can't
   figure out this using the SDCC API, then we shall assume that there
   is just 1 SD slot. */
static int
hotplug_sd_get_total_slots (void)
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

int
hotplug_sd_reset (struct hotplug_device *hdev)
{
  /* Get the total number of SD-Slots present in this target. */
  max_sd_slots = hotplug_sd_get_total_slots ();

  if (hdev->driveno >= max_sd_slots)
  {
    /* This slot is not present, hence return -1 so that the hotplug-manager
       marks this device as DEV_UNUSABLE and will not poll this device in the
       future. */
    return -1;
  }

#ifdef HAVE_SDCC_SLOT_HAS_SUPPORT
  /* If this slot is enabled, but is not compatible for memory card
   * then also mark this device as not usable. */
  if (sdcc_slot_has_support (hdev->driveno, SDCC_SLOT_MEMORY) == FALSE)
    return -1;
#endif

  return 0;
}

int
hotplug_sd_idle_device (struct hotplug_device *hdev)
{
  ASSERT (hdev->driveno < max_sd_slots);

#ifdef HAVE_SDCC_POWER_COLLAPSE
  /* If the HAVE_SDCC_POWER_COLLAPSE is being defined then call the
   * corresponding timeout_event function of the SD.
   *
   * Ignore the return status of the sdcc_activity_timeout_event () as
   * per specified by the SDCC team as on hotplug side we don't have to care
   * about the return status of this api. */

  (void) sdcc_activity_timeout_event(hdev->driveno);

#endif

  return 0;
}

#ifdef HAVE_SDCC_IOVEC_SUPPORT
static uint32
hotplug_sd_copy_fs_iovec_to_sdcc_iovec (fs_iovec_t iovec, uint32 iovec_cnt)
{
  uint32 i, total_sect = 0;

  ASSERT (iovec_cnt < FS_MAX_SDCC_IOVEC_ITEMS);

  for (i=0; i<iovec_cnt; ++i)
  {
    sdcc_iovec[i].sector_address = iovec[i].sector_id;
    sdcc_iovec[i].buffer = iovec[i].buff_base;
    sdcc_iovec[i].num_sector = EFS_PAGE_SIZE / FS_SDCC_SECTOR_SIZE_IN_BYTES;
    total_sect += sdcc_iovec[i].num_sector;
  }

  return total_sect;
}

static int
hotplug_sd_write_iovec_i (struct hotplug_device *hdev, fs_iovec_t iovec,
                          uint32 iovec_cnt, int reliable_write)
{
  sdcc_memory_iovec_transfer_type write_type;
  SDCC_STATUS sdcc_res;
  int result;
  uint32 total_sect;

  /* Make sure that the slot is a valid one. */
  ASSERT (hdev->driveno < max_sd_slots);

  /* Do not permit write operations if the device is closed */
  if (!hdev->sd_is_open)
    return -1;

  /* Copy the supplied FS-iovec to our static SDCC-iovec. */
  total_sect = hotplug_sd_copy_fs_iovec_to_sdcc_iovec (iovec, iovec_cnt);

  if (reliable_write)
    write_type = SDCC_MEMORY_IOVEC_RELIABLE_WRITE;
  else
    write_type = SDCC_MEMORY_IOVEC_REGULAR_DATA_XFER;

  EFS_BAB_START (EFS_BAB_SDCC_WRITE_IOVEC);
  EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE_IOVEC, iovec_cnt, TRUE, FALSE, TRUE);
  EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE_IOVEC, total_sect, TRUE, FALSE, FALSE);
  (void) total_sect; /* If Babylon if turned off, then to keep compiler happy*/

  sdcc_res = sdcc_write_iovec (hdev->driveno, sdcc_iovec,
                               iovec_cnt, write_type);

  EFS_BAB_END (EFS_BAB_SDCC_WRITE_IOVEC);

  if (sdcc_res != SDCC_NO_ERROR)
    MSG_HIGH ("sdcc_write_iovec failed...",0,0,0);

  result = (sdcc_res == SDCC_NO_ERROR) ? 0 : -1;
  return result;
}

static int
hotplug_sd_reliable_write_i (struct hotplug_device *hdev, uint32 lba,
                             uint32 *buf, uint32 num_sectors)
{
  SDCC_STATUS sdcc_res;
  int result;

  /* Make sure that the slot is a valid one. */
  ASSERT (hdev->driveno < max_sd_slots);

  /* Do not permit write operations if the device is closed */
  if (!hdev->sd_is_open)
    return -1;

  sdcc_iovec[0].sector_address = lba;
  sdcc_iovec[0].buffer = buf;
  sdcc_iovec[0].num_sector = num_sectors;

  EFS_BAB_START (EFS_BAB_SDCC_WRITE_REL);
  EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE_REL, lba, TRUE, FALSE, TRUE);
  EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE_REL, num_sectors, TRUE, FALSE, FALSE);

  sdcc_res = sdcc_write_iovec (hdev->driveno, sdcc_iovec, 1,
                               SDCC_MEMORY_IOVEC_RELIABLE_WRITE);

  EFS_BAB_END (EFS_BAB_SDCC_WRITE_REL);

  if (sdcc_res != SDCC_NO_ERROR)
    MSG_HIGH ("sdcc_write_iovec failed...",0,0,0);

  result = (sdcc_res == SDCC_NO_ERROR) ? 0 : -1;

  return result;
}
#endif /* HAVE_SDCC_IOVEC_SUPPORT */

#ifdef HAVE_SDCC_GET_MEMORY_DEVICE_INFO
static int
hotplug_sd_get_device_info_i (struct hotplug_device *hdev,
                              struct hotplug_device_info *dev_info)
{
  sdcc_mem_info_type sdcc_info;
  SDCC_STATUS sdcc_res;
  int result;

  sdcc_res = sdcc_mem_get_device_info( hdev->driveno, &sdcc_info);
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
}
#endif  /* HAVE_SDCC_GET_MEMORY_DEVICE_INFO */

int
hotplug_sd_write_iovec (struct hotplug_device *hdev, fs_iovec_t iovec,
                        uint32 iovec_cnt, int reliable_write)
{
#ifdef HAVE_SDCC_IOVEC_SUPPORT
  return hotplug_sd_write_iovec_i (hdev, iovec, iovec_cnt, reliable_write);
#else
  return hotplug_no_dev_write_iovec (hdev, iovec, iovec_cnt, reliable_write);
#endif
}

int
hotplug_sd_reliable_write (struct hotplug_device *hdev, uint32 lba,
                           uint32 *buf, uint32 num_sectors)
{
#ifdef HAVE_SDCC_IOVEC_SUPPORT
  return hotplug_sd_reliable_write_i (hdev, lba, buf, num_sectors);
#else
  return hotplug_no_dev_reliable_write (hdev, lba, buf, num_sectors);
#endif
}

int
hotplug_sd_get_device_info (struct hotplug_device *hdev,
                            struct hotplug_device_info *dev_info)
{
#ifdef HAVE_SDCC_GET_MEMORY_DEVICE_INFO
  return hotplug_sd_get_device_info_i (hdev, dev_info);
#else
  return hotplug_no_dev_get_device_info (hdev, dev_info);
#endif
}

struct hotplug_dev_funcs hotplug_sd_dev = {
  hotplug_fat_mount,
  hotplug_sd_open_device,
  hotplug_sd_close_device,
  hotplug_sd_read,
  hotplug_sd_write,
  hotplug_sd_write,
  hotplug_no_erase,
  hotplug_sd_get_size,
  hotplug_sd_is_present,
  hotplug_sd_format,
  hotplug_sd_reset,             /* Reset */
  hotplug_sd_idle_device,       /* Idle Dispatch Timeout */
  hotplug_no_dev_read_iovec,
  hotplug_sd_write_iovec,
  hotplug_sd_reliable_write,
  hotplug_sd_get_device_info,
};
#else
extern int __dont_complain_about_empty_file;
#endif /* FS_HOTPLUG_SD */
