/***********************************************************************
 * fs_hotplug_sd_cprm.c
 *
 * Support for CPRM partition on SD Cards
 * Copyright (C) 2006, 2008 QUALCOMM, Inc.
 *
 * All Secure Digital media cards contain a protected partition that
 * is accessible only through the use of CPRM.  This partition appears
 * as a unique hotplug device.
 *
 ***********************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_sd_cprm.c#3 $ $DateTime: 2008/12/12 11:11:10 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-12   rp    Added write_iovec, reliable write functions to hdev.
2008-11-24   yog   Added hotplug_success_stub for new timeout function.
2008-03-07   yg    Remove SD LOCK that's common across all slots.
2006-11-13   s h   Be careful not to include absent CPRM header files.
2006-11-12   s h   Cleanup
2006-09-28   sep   Created.

===========================================================================*/

#include "fs_hotplug.h"
#include "fs_hotplug_i.h"

#ifdef FS_HOTPLUG_SD_CPRM

#include "fs_hotplug_sd_cprm.h"
#include "sdcc_cprm.h"
#include "CprmInterface.h"
#include "CprmProtectedFileSystem.h"

/* All blocks are 512 bytes, always */
#define SDCC_PROTECTED_BLOCK_SIZE 512

/* Maximum number of blocks to read or write in a single transaction.
   Because the data is being passed over RPC, we need a limit on how
   many blocks are passed in one call. */
#define MAX_READ_BLOCKS  1
#define MAX_WRITE_BLOCKS 1

typedef enum
{
  HOTPLUG_SD_CPRM_MODE_READ_ANY_MKB  = 0,
  HOTPLUG_SD_CPRM_MODE_READ_SAME_MKB = 1
} HOTPLUG_SD_CPRM_MODE;

/*======================================================================
 * FUNCTION     hotplug_sd_cprm_open_device
 *
 * DESCRIPTION  Open the SD-card protected area.  Do nothing if it
 *    is already open.
 *
 * DEPENDENCIES     None
 *
 * PARAMETERS       hdev - Hotplug device object
 *
 * RETURN VALUE     0  - Successful, or device was already open
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Device is opened, and reads and writes will now work
 *
 *======================================================================*/
int
hotplug_sd_cprm_open_device (struct hotplug_device *hdev)
{
  if (! hdev->sd_is_open)
  {
    /* The drive is not open.  Attempt to open it. */

    /* Check if there is an SD card in the drive */
    if (sdcc_cprm_does_sd_card_exist (hdev->driveno))
    {
      /* Attempt to open this card */
      hdev->sd_is_open =
        (CprmInterfaceIsEnabled (hdev->driveno) ==
         CPRMINTERFACE_SUCCESS) ? 1 : 0;
    }
  }

  return hdev->sd_is_open ? 0 : -1;
}

/*======================================================================
 * FUNCTION     hotplug_sd_cprm_close_device
 *
 * DESCRIPTION  Close the SD-card protected area.  Do nothing if it
 *    is already closed.
 *
 * DEPENDENCIES     None
 *
 * PARAMETERS       hdev - Hotplug device object
 *
 * RETURN VALUE     0  - Successful, or device was already closed
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Device is closed, and reads or writes to it will no
 *                  longer work
 *
 *======================================================================*/
int
hotplug_sd_cprm_close_device (struct hotplug_device *hdev)
{
  /* Do nothing, just close */
  hdev->sd_is_open = 0;

  return 0;
}

/*======================================================================
 * FUNCTION     hotplug_sd_cprm_is_present
 *
 * DESCRIPTION  This will return that the protected area of the SD-card
 *  is present only if it has been successfully enabled for CPRM, at
 *  which point, CprmProtectedFileSystemIsEnabled will return TRUE.
 *
 * DEPENDENCIES     None
 *
 * PARAMETERS       hdev - Hotplug device object
 *
 * RETURN VALUE     1 if the SD card exists
 *
 * SIDE EFFECTS     None
 *
 *======================================================================*/
int
hotplug_sd_cprm_is_present (struct hotplug_device *hdev)
{
  int cprm_enabled = CprmProtectedFileSystemIsEnabled (hdev->driveno);

  /* If CPRM is enabled, also make sure that there is an SD card in the slot
     before reporting that the SD card is present. */
  if (cprm_enabled && !sdcc_cprm_does_sd_card_exist (hdev->driveno))
  {
    /* If there was no SD card, disable CPRM so we don't keep checking */
    CprmProtectedFileSystemDisable (hdev->driveno);
    cprm_enabled = 0;
  }

  /* If the card is present, and is not open, attempt to open it now.
     HFAT will make read calls to the card without calling open otherwise */
  if (cprm_enabled && !hdev->sd_is_open)
  {
    if (hotplug_sd_cprm_open_device (hdev) != 0)
    {
      /* Something failed in the open call. */
      cprm_enabled = 0;
    }
  }

  return cprm_enabled ? 1 : 0;
}

/*======================================================================
 * FUNCTION     hotplug_sd_cprm_read
 *
 * DESCRIPTION  Read some data from the protected area of the SD-card
 *
 * DEPENDENCIES     The device must have been opened with hotplug_sd_cprm_open
 *
 * PARAMETERS       hdev - Hotplug device object
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
int
hotplug_sd_cprm_read (struct hotplug_device *hdev,
                      uint32 sector_number,
                      uint8 * p_buffer, uint16 sector_count)
{
  int result = -1;
  uint32 block_offset = 0;

  if (hdev->sd_is_open)
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
        (hdev->driveno, // drive number
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
 * FUNCTION     hotplug_sd_cprm_write_with_mode
 *
 * DESCRIPTION  Write some data from the protected area of the SD-card
 *    This function allows specifying whether to allow reading what
 *    was written with any MKB that can get to the protected area,
 *    or only allowing the same MKB used to write it to read the data
 *    that is being written.
 *
 * DEPENDENCIES     hotplug_sd_cprm_open should have been called already
 *
 * PARAMETERS       hdev - Hotplug device object
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
int
hotplug_sd_cprm_write_with_mode (struct hotplug_device *hdev,
                                 uint32 sector_number,
                                 uint8 * p_buffer,
                                 uint16 sectors_to_write,
                                 HOTPLUG_SD_CPRM_MODE mode)
{
  int result = -1;
  uint32 block_offset = 0;

  if (hdev->sd_is_open)
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
        (hdev->driveno,         //drive number
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
 * FUNCTION     hotplug_sd_cprm_write
 *
 * DESCRIPTION  Write some data from the protected area of the SD-card
 *   This defaults to allowing any MKB to read the data that was written.
 *   This is the correct setting for file system data or directory data,
 *   but writes of data to files should use hotplug_sd_cprm_write_with_mode
 *   and specify READ_SAME_MKB as the mode.
 *
 * DEPENDENCIES     hotplug_sd_cprm_open should have been called already
 *
 * PARAMETERS       hdev - Hotplug device object
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
int
hotplug_sd_cprm_write (struct hotplug_device *hdev,
                       uint32 sector_number,
                       uint8 * p_buffer, uint16 sectors_to_write)
{
  // By default, do writes that allow read back with any MKB
  return hotplug_sd_cprm_write_with_mode (hdev,
                                          sector_number,
                                          p_buffer,
                                          sectors_to_write,
                                          HOTPLUG_SD_CPRM_MODE_READ_ANY_MKB);
}

/*======================================================================
 * FUNCTION     hotplug_sd_cprm_user_write
 *
 * DESCRIPTION  Write some data from the protected area of the SD-card
 *              This function writes data that can only be read with same MKB
 *
 * DEPENDENCIES     hotplug_sd_cprm_open should have been called already
 *
 * PARAMETERS       hdev - Hotplug device object
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
int
hotplug_sd_cprm_user_write (struct hotplug_device *hdev,
                            uint32 sector_number,
                            uint8 * p_buffer, uint16 sectors_to_write)
{
  // Do writes that allow read back with same MKB
  return hotplug_sd_cprm_write_with_mode (hdev,
                                          sector_number,
                                          p_buffer,
                                          sectors_to_write,
                                          HOTPLUG_SD_CPRM_MODE_READ_SAME_MKB);
}

/*======================================================================
 * FUNCTION     hotplug_sd_cprm_get_size
 *
 * DESCRIPTION  Get the size of the SD-card's protected area.  The size
 *   is returned in the parameters p_blocks and p_bytes_per_block
 *
 * PARAMETERS       hdev - Hotplug device object
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
int
hotplug_sd_cprm_get_size (struct hotplug_device *hdev,
                          uint32 * p_blocks, uint16 * p_bytes_per_block)
{
  int result;
  uint32 protected_area_size;   //in bytes

  /* Fail if either pointer is NULL */
  if (!p_blocks || !p_bytes_per_block)
    return -1;

  /* Fail if the SD card does not exist */
  if (!sdcc_cprm_does_sd_card_exist (hdev->driveno))
    return -1;

  /* Get the size of the protected area */
  result = sdcc_cprm_get_protected_area_size (hdev->driveno,
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
 * FUNCTION     hotplug_sd_cprm_format
 *
 * DESCRIPTION  This callback checks rather or not it is ok to format
 *   the device.  It does not actually format it.  We will always
 *   return failure since we do not want the user to be able to
 *   format the device through this hotplug.
 *
 * PARAMETERS       hdev - Hotplug device object
 *
 * RETURN VALUE     0  - Ok to format
 *                  -1 - Not Ok to format
 *
 *======================================================================*/
int
hotplug_sd_cprm_format (struct hotplug_device *hdev)
{
  (void) hdev;
  /* Do not allow formatting the protected area of the SD card */
  return -1;
}

int
hotplug_sd_cprm_reset (struct hotplug_device *hdev)
{
  (void) hdev;
  return 0;
}


/*======================================================================
  This table contains the functions we expose to the
  file system.
 *======================================================================*/

struct hotplug_dev_funcs hotplug_sd_cprm_functions = {
  hotplug_fat_mount,
  hotplug_sd_cprm_open_device,
  hotplug_sd_cprm_close_device,
  hotplug_sd_cprm_read,
  hotplug_sd_cprm_write,
  hotplug_sd_cprm_user_write,
  hotplug_no_erase,
  hotplug_sd_cprm_get_size,
  hotplug_sd_cprm_is_present,
  hotplug_sd_cprm_format,
  hotplug_sd_cprm_reset,        /* Reset */
  hotplug_success_stub,         /* Idle Dispatch Timeout */
  hotplug_no_dev_read_iovec,
  hotplug_no_dev_write_iovec,
  hotplug_no_dev_reliable_write,
  hotplug_no_dev_get_device_info
};
#else
extern int __dont_complain_about_empty_file;
#endif
