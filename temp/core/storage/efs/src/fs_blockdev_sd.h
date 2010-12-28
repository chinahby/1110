/***********************************************************************
 * fs_blockdev_sd.h
 *
 * Blockdev SD driver header.
 * Copyright (C) 2006, 2008 - 2009 QUALCOMM, Inc.
 *
 * Contains SD device specfic data and a handle to the SD device driver.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_blockdev_sd.h#2 $ $DateTime: 2009/05/18 17:26:04 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-15   ebb   Added handle to the struct sd_device_data.
2009-05-08   ebb   Moved from hotplug to blockdev.
2006-04-22   s h   Simplified fs_hotplug_xxx.h header files
2006-03-23   s h   Created.

===========================================================================*/

#ifndef __FS_BLOCKDEV_SD_H__
#define __FS_BLOCKDEV_SD_H__

#include "fs_blockdev_i.h"

#ifdef FS_BLOCKDEV_SD

#include "sdcc_api.h"

/* SD/MMC device specific data */
typedef struct
{
  int16              driveno;
  unsigned char      is_open;
  int                was_present;
  boolean            device_idle_state;
  struct sdcc_device *handle;
} sd_device_data;

/* Handle to SD device driver */
extern blockdev_driver blockdev_sd_driver;

#else
extern int __dont_complain_about_empty_file;
#endif /* FS_BLOCKDEV_SD */

#endif /* not __FS_BLOCKDEV_SD_H__ */
