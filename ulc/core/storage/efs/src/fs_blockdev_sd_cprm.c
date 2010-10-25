/***********************************************************************
 * fs_blockdev_sd_cprm.c
 *
 * Support for CPRM partition on SD Cards
 * Copyright (C) 2006, 2008 - 2009 QUALCOMM, Inc.
 *
 * All Secure Digital media cards contain a protected partition that
 * is accessible only through the use of CPRM.  This partition appears
 * as a unique block device.
 *
 ***********************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_blockdev_sd_cprm.c#2 $ $DateTime: 2009/05/18 17:26:04 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-15   ebb   Added additional void * arg to fs_sd_cprm_open.
2009-05-08   ebb   Migrated to blockdev from hotplug interface.
2008-12-12   rp    Added write_iovec, reliable write functions to hdev.
2008-11-24   yog   Added hotplug_success_stub for new timeout function.
2008-03-07   yg    Remove SD LOCK that's common across all slots.
2006-11-13   s h   Be careful not to include absent CPRM header files.
2006-11-12   s h   Cleanup
2006-09-28   sep   Created.

===========================================================================*/

#include "customer.h"
#include "fs_blockdev_i.h"

#ifdef FS_BLOCKDEV_SD_CPRM

#include "fs_blockdev_sd_cprm.h"
#include "fs_err.h"

#include "sdcc_cprm.h"
#include "sdcc_api.h"
#include "CprmInterface.h"
#include "CprmProtectedFileSystem.h"

#ifdef FS_UNIT_TEST
#error code not present
#endif

/* Bump this version for any software changes */
#define SD_CPRM_DRIVER_VERSION          0x0001

/* All blocks are 512 bytes, always */
#define SDCC_PROTECTED_BLOCK_SIZE 512

/* Maximum number of blocks to read or write in a single transaction.
   Because the data is being passed over RPC, we need a limit on how
   many blocks are passed in one call. */
#define MAX_READ_BLOCKS  1
#define MAX_WRITE_BLOCKS 1

typedef enum
{
  BLOCKDEV_SD_CPRM_MODE_READ_ANY_MKB  = 0,
  BLOCKDEV_SD_CPRM_MODE_READ_SAME_MKB = 1
} BLOCKDEV_SD_CPRM_MODE;

/* Function prototypes */
static void fs_sd_cprm_init (struct block_device **, uint16 *);
static void fs_sd_cprm_cleanup (void);

/* Device functions */
static int fs_sd_cprm_open (struct block_device *, void *);
static int fs_sd_cprm_close (struct block_device *);
static int fs_sd_cprm_is_present (struct block_device *);
static int fs_sd_cprm_read (struct block_device *, uint32, uint8 *, uint16);
static int fs_sd_cprm_write_with_mode (struct block_device *,
                                       uint32, uint8 *, uint16,
                                       BLOCKDEV_SD_CPRM_MODE);
static int fs_sd_cprm_write (struct block_device *, uint32, uint8 *, uint16);
static int fs_sd_cprm_user_write (struct block_device *, uint32, uint8 *,
                                  uint16);
static int fs_sd_cprm_get_size (struct block_device *, uint32 *, uint16 *);
static int fs_sd_cprm_format (struct block_device *);
static int fs_sd_cprm_reset (struct block_device *);

/* Non-implemented stubs */
static int fs_sd_cprm_read_iovec_fail (struct block_device *, fs_iovec_t,
                                       uint32);
static int fs_sd_cprm_write_iovec_fail (struct block_device *, fs_iovec_t,
                                        uint32, int);
static int fs_sd_cprm_reliable_write_fail (struct block_device *, uint32,
                                           uint32 *, uint32);
static int fs_sd_cprm_erase_fail (struct block_device *, uint32, uint16);
static int fs_sd_cprm_get_device_info_fail (struct block_device *,
                                            struct block_device_info *);
static int fs_sd_cprm_device_idle_fail (struct block_device *);

static blockdev_ops sd_cprm_ops =
{
  fs_sd_cprm_open,                 /* open */
  fs_sd_cprm_close,                /* close */
  fs_sd_cprm_read,                 /* read */
  fs_sd_cprm_write,                /* write */
  fs_sd_cprm_user_write,           /* uwrite */
  fs_sd_cprm_read_iovec_fail,      /* read_iovec */
  fs_sd_cprm_write_iovec_fail,     /* write_iovec */
  fs_sd_cprm_reliable_write_fail,  /* reliable_write */
  fs_sd_cprm_erase_fail,           /* erase */
  fs_sd_cprm_reset,                /* reset */
  fs_sd_cprm_format,               /* device_format */
  fs_sd_cprm_is_present,           /* is_present */
  fs_sd_cprm_get_size,             /* get_device_size */
  fs_sd_cprm_get_device_info_fail, /* get_device_info */
  fs_sd_cprm_device_idle_fail,     /* set_device_idle */
  fs_sd_cprm_device_idle_fail      /* set_device_idle */
};

/* On-target, the CPRM partition shares a physical device number with
 * the user portion of the SD card, and the CPRM calls are distinct.
 * Off-target, where we use the same calls for both, we use a
 * different devno to tell them apart. */
#ifdef FS_UNIT_TEST
#error code not present
#else
  #define CPRM_DEVNO 0
#endif

/* Storage for SD specific data */
static sd_device_data sd_dev_data[] =
{
  {CPRM_DEVNO, 0, 0, 0, NULL} /* driveno, is_open, was_present, device_idle,
                                 handle */
};

/* List of sd block devices to manage */
static struct block_device devices[] =
{
  /* Protected area of SD card 0, using MKB 0 (See CPRM_DEVNO above) */
  {"/mmc1_p", &sd_cprm_ops, (void *)&sd_dev_data[0], NULL, BLOCKDEV_TYPE_MMC,
   0, 0xFF, CANARY},
};


/* Declare the driver for this module, this will be the only thing
 * exposed to the blockdev layer above. */
blockdev_driver blockdev_sd_cprm_driver =
{
  "BDEV_SD_CPRM_DRIVER",  /* char *name */
  BLOCKDEV_TYPE_MMC,      /* block_device type */
  SD_CPRM_DRIVER_VERSION, /* uint16 version */
  fs_sd_cprm_init,           /* init fn */
  fs_sd_cprm_cleanup,        /* cleanup fn */
};




/*-------------------------
 * SD CPRM Driver functions
 *-------------------------*/

/* This should populate the blockdev linked list */
static void
fs_sd_cprm_init (struct block_device **head, uint16 *dev_id)
{
  int i;

  /* Check to ensure the number of private data entries matches
   * the number of devices. */
  if (sizeof (devices) / sizeof (devices[0]) !=
      sizeof (sd_dev_data) / sizeof (sd_dev_data[0]))
    FS_ERR_FATAL ("Number of block devices MUST match private data!", 0,0,0);

  /* Link the list of devices together */
  for (i = (sizeof (devices) / sizeof (devices[0])) - 1; i >= 0; i--)
  {
    struct block_device *dev = &devices[i];
    sd_device_data *sdev = (sd_device_data *)dev->device_data;

    /* Add device to beginning of the linked list */
    dev->next = *head;
    dev->device_id = (*dev_id)++;
    sdev->is_open = 0;
    sdev->was_present = 0;
    sdev->device_idle_state = FALSE;
    *head = dev;
  }
}

static void
fs_sd_cprm_cleanup (void)
{
  /* Do nothing as of now */
}


/*======================================================================
 * FUNCTION     sd_cprm_open_device
 *
 * DESCRIPTION  Open the SD-card protected area.  Do nothing if it
 *    is already open.
 *
 * DEPENDENCIES     None
 *
 * PARAMETERS       bdev - Block device object
 *
 * RETURN VALUE     0  - Successful, or device was already open
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Device is opened, and reads and writes will now work
 *
 *======================================================================*/
static int
fs_sd_cprm_open (struct block_device *bdev, void *data)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;
  (void) data;

  if (! dev->is_open)
  {
    /* The drive is not open.  Attempt to open it. */

    /* Check if there is an SD card in the drive */
    if (sdcc_cprm_does_sd_card_exist (dev->driveno))
    {
      /* Attempt to open this card */
      dev->is_open =
        (CprmInterfaceIsEnabled (dev->driveno) ==
         CPRMINTERFACE_SUCCESS) ? 1 : 0;
    }
  }

  return dev->is_open ? 0 : -1;
}

/*======================================================================
 * FUNCTION     sd_cprm_close_device
 *
 * DESCRIPTION  Close the SD-card protected area.  Do nothing if it
 *    is already closed.
 *
 * DEPENDENCIES     None
 *
 * PARAMETERS       bdev - Block device object
 *
 * RETURN VALUE     0  - Successful, or device was already closed
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Device is closed, and reads or writes to it will no
 *                  longer work
 *
 *======================================================================*/
static int
fs_sd_cprm_close (struct block_device *bdev)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  /* Do nothing, just close */
  dev->is_open = 0;

  return 0;
}

/*======================================================================
 * FUNCTION     sd_cprm_is_present
 *
 * DESCRIPTION  This will return that the protected area of the SD-card
 *  is present only if it has been successfully enabled for CPRM, at
 *  which point, CprmProtectedFileSystemIsEnabled will return TRUE.
 *
 * DEPENDENCIES     None
 *
 * PARAMETERS       bdev - Block device object
 *
 * RETURN VALUE     1 if the SD card exists
 *
 * SIDE EFFECTS     None
 *
 *======================================================================*/
static int
fs_sd_cprm_is_present (struct block_device *bdev)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;
  int cprm_enabled = CprmProtectedFileSystemIsEnabled (dev->driveno);

  /* If CPRM is enabled, also make sure that there is an SD card in the slot
     before reporting that the SD card is present. */
  if (cprm_enabled && !sdcc_cprm_does_sd_card_exist (dev->driveno))
  {
    /* If there was no SD card, disable CPRM so we don't keep checking */
    CprmProtectedFileSystemDisable (dev->driveno);
    cprm_enabled = 0;
  }

  /* If the card is present, and is not open, attempt to open it now.
     HFAT will make read calls to the card without calling open otherwise */
  if (cprm_enabled && !dev->is_open)
  {
    if (fs_sd_cprm_open (bdev, NULL) != 0)
    {
      /* Something failed in the open call. */
      cprm_enabled = 0;
    }
  }

  return cprm_enabled ? 1 : 0;
}

/*======================================================================
 * FUNCTION     fs_sd_cprm_read
 *
 * DESCRIPTION  Read some data from the protected area of the SD-card
 *
 * DEPENDENCIES     The device must have been opened with fs_sd_cprm_open
 *
 * PARAMETERS       bdev - Block device object
 *
 *                  sector_number - Number of the block to read.  The
 *                     first block number is 0.
 *
 *                  p_buffer - Buffer to read the data into
 *
 *                  sector_count - Number of blocks to read.
 *
 * RETURN VALUE     0  - Successful
 *                  -1 - Failed
 *
 * SIDE EFFECTS     p_buffer receives the data read from the SD card
 *
 *======================================================================*/
static int
fs_sd_cprm_read (struct block_device *bdev, uint32 sector_number,
                 uint8 * p_buffer, uint16 sector_count)
{
  int result = -1;
  uint32 block_offset = 0;
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  if (dev->is_open)
  {
    while (sector_count)
    {
      uint32 blocks_to_read = sector_count;

      /* Since this is going over RPC, we don't want to transfer a lot
         of blocks at once.  So limit it to MAX_BLOCKS_TO_WRITE per
         call. */
      if (blocks_to_read > MAX_READ_BLOCKS)
        blocks_to_read = MAX_READ_BLOCKS;

      // Address apparantly requires a block number
      result = CprmInterfaceProtectedRead
        (dev->driveno, // drive number
         sector_number + block_offset, // address
         p_buffer + block_offset * SDCC_PROTECTED_BLOCK_SIZE, // p_data
         blocks_to_read) ? 0 : -1;    // blockCount

      /* Exit early if any read failed */
      if (result == -1)
        break;

      sector_count -= blocks_to_read;
      block_offset += blocks_to_read;
    }

  }

  return result;
}

/*======================================================================
 * FUNCTION     fs_sd_cprm_write_with_mode
 *
 * DESCRIPTION  Write some data from the protected area of the SD-card
 *    This function allows specifying whether to allow reading what
 *    was written with any MKB that can get to the protected area,
 *    or only allowing the same MKB used to write it to read the data
 *    that is being written.
 *
 * DEPENDENCIES     fs_sd_cprm_open should have been called already
 *
 * PARAMETERS       bdev - Block device object
 *
 *                  sector_number - Number of the block to write.  The
 *                     first block number is 0.
 *
 *                  p_buffer - Buffer containing the data to write
 *
 *                  sector_count - Number of blocks to write.
 *
 * RETURN VALUE     0  - Successful
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Data is written to the disk
 *
 *======================================================================*/
static int
fs_sd_cprm_write_with_mode (struct block_device *bdev,
                            uint32 sector_number,
                            uint8 * p_buffer,
                            uint16 sectors_to_write,
                            BLOCKDEV_SD_CPRM_MODE mode)
{
  int result = -1;
  uint32 block_offset = 0;
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  if (dev->is_open)
  {
    while (sectors_to_write)
    {
      uint32 blocks_to_write = sectors_to_write;

      /* Since this is going over RPC, we don't want to transfer a lot
         of blocks at once.  So limit it to MAX_BLOCKS_TO_WRITE per
         call. */
      if (blocks_to_write > MAX_WRITE_BLOCKS)
        blocks_to_write = MAX_WRITE_BLOCKS;

      result = CprmInterfaceProtectedWrite
        (dev->driveno,         //drive number
         sector_number + block_offset, //address
         p_buffer + block_offset * SDCC_PROTECTED_BLOCK_SIZE, //p_data
         blocks_to_write,       //blockCount
         mode) ? 0 : -1;        //mode

      /*Exit early if any write failed */
      if (result == -1)
        break;

      sectors_to_write -= blocks_to_write;
      block_offset += blocks_to_write;
    }
  }

  return result;
}


/*======================================================================
 * FUNCTION     blockdev_fs_sd_cprm_write
 *
 * DESCRIPTION  Write some data from the protected area of the SD-card
 *   This defaults to allowing any MKB to read the data that was written.
 *   This is the correct setting for file system data or directory data,
 *   but writes of data to files should use sd_cprm_write_with_mode
 *   and specify READ_SAME_MKB as the mode.
 *
 * DEPENDENCIES     sd_cprm_open should have been called already
 *
 * PARAMETERS       bdev - Block device object
 *
 *                  sector_number - Number of the block to write.  The
 *                     first block number is 0.
 *
 *                  p_buffer - Buffer containing the data to write
 *
 *                  sector_count - Number of blocks to write.
 *
 * RETURN VALUE     0  - Successful
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Data is written to the disk
 *
 *======================================================================*/
static int
fs_sd_cprm_write (struct block_device *bdev, uint32 sector_number,
                  uint8 * p_buffer, uint16 sectors_to_write)
{
  // By default, do writes that allow read back with any MKB
  return fs_sd_cprm_write_with_mode (bdev,
                                     sector_number,
                                     p_buffer,
                                     sectors_to_write,
                                     BLOCKDEV_SD_CPRM_MODE_READ_ANY_MKB);
}

/*======================================================================
 * FUNCTION     fs_sd_cprm_user_write
 *
 * DESCRIPTION  Write some data from the protected area of the SD-card
 *              This function writes data that can only be read with same MKB
 *
 * DEPENDENCIES     fs_sd_cprm_open should have been called already
 *
 * PARAMETERS       bdev - Block device object
 *
 *                  sector_number - Number of the block to write.  The
 *                     first block number is 0.
 *
 *                  p_buffer - Buffer containing the data to write
 *
 *                  sector_count - Number of blocks to write.
 *
 * RETURN VALUE     0  - Successful
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Data is written to the disk
 *
 *======================================================================*/
static int
fs_sd_cprm_user_write (struct block_device *bdev, uint32 sector_number,
                       uint8 *p_buffer, uint16 sectors_to_write)
{
  // Do writes that allow read back with same MKB
  return fs_sd_cprm_write_with_mode (bdev,
                                     sector_number,
                                     p_buffer,
                                     sectors_to_write,
                                     BLOCKDEV_SD_CPRM_MODE_READ_SAME_MKB);
}

/*======================================================================
 * FUNCTION     sd_cprm_get_size
 *
 * DESCRIPTION  Get the size of the SD-card's protected area.  The size
 *   is returned in the parameters p_blocks and p_bytes_per_block
 *
 * PARAMETERS       bdev - Block device object
 *
 *                  p_blocks - Gets the number of blocks in the protected area
 *
 *                  p_bytes_per_block - Gets the block size in bytes
 *
 * RETURN VALUE     0  - Successful
 *                  -1 - Failed
 *
 *                  Fails if there is a problem reading the protected area
 *                  size, or if the size is not a multiple of the block size.
 *
 * SIDE EFFECTS     p_blocks and p_bytes_per_block recieve the size information
 *
 *======================================================================*/
static int
fs_sd_cprm_get_size (struct block_device *bdev, uint32 * p_blocks,
                     uint16 * p_bytes_per_block)
{
  int result;
  uint32 protected_area_size;   //in bytes
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  /* Fail if either pointer is NULL */
  if (!p_blocks || !p_bytes_per_block)
    return -1;

  /* Fail if the SD card does not exist */
  if (!sdcc_cprm_does_sd_card_exist (dev->driveno))
    return -1;

  /* Get the size of the protected area */
  result = sdcc_cprm_get_protected_area_size (dev->driveno,
                                              &protected_area_size);

  /* Check that the result is valid and divisible by the block size */
  if (!result || ((protected_area_size % SDCC_PROTECTED_BLOCK_SIZE) != 0))
    return -1;

  /* Protected area blocks are always 512 bytes */
  *p_bytes_per_block = SDCC_PROTECTED_BLOCK_SIZE;

  /* The size was returned in bytes */
  *p_blocks = protected_area_size / SDCC_PROTECTED_BLOCK_SIZE;

  /* Successful */
  return 0;
}

/*======================================================================
 * FUNCTION     fs_sd_cprm_format
 *
 * DESCRIPTION  This callback checks rather or not it is ok to format
 *   the device.  It does not actually format it.  We will always
 *   return failure since we do not want the user to be able to
 *   format the device.
 *
 * PARAMETERS       bdev - Block device object
 *
 * RETURN VALUE     0  - Ok to format
 *                  -1 - Not Ok to format
 *
 *======================================================================*/
static int
fs_sd_cprm_format (struct block_device *bdev)
{
  (void) bdev;
  /* Do not allow formatting the protected area of the SD card */
  return -1;
}

static int
fs_sd_cprm_reset (struct block_device *bdev)
{
  (void) bdev;
  return 0;
}

static int fs_sd_cprm_read_iovec_fail (struct block_device *bdev,
                                       fs_iovec_t iovec, uint32 iovec_cnt)
{
  (void) bdev; (void) iovec; (void) iovec_cnt;
  return -1;
}
static int fs_sd_cprm_write_iovec_fail (struct block_device *bdev,
                                        fs_iovec_t iovec, uint32 iovec_cnt,
                                        int reliable_write)
{
  (void) bdev; (void) iovec;
  (void) iovec_cnt; (void) reliable_write;
  return -1;
}
static int fs_sd_cprm_reliable_write_fail (struct block_device *bdev,
                                           uint32 lba, uint32 *buf,
                                           uint32 num_sectors)
{
  (void) bdev; (void) lba;
  (void) buf; (void) num_sectors;
  return -1;
}
static int fs_sd_cprm_erase_fail (struct block_device *bdev, uint32 lba,
                                  uint16 n_to_erase)
{
  (void) bdev; (void) lba; (void) n_to_erase;
  return -1;
}
static int fs_sd_cprm_get_device_info_fail (struct block_device *bdev,
                                            struct block_device_info *info)
{
  (void) bdev; (void) info;
  return -1;
}

static int fs_sd_cprm_device_idle_fail (struct block_device *bdev)
{
  (void) bdev;
  return -1;
}

#else
extern int __dont_complain_about_empty_file;
#endif /* FS_BLOCKDEV_SD_CPRM */
