/***********************************************************************
 * fs_blockdev_ftl.h
 *
 * Copyright (C) 2006, 2009 QUALCOMM, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_ftl.h#2 $ $DateTime: 2009/05/18 17:19:57 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-08   ebb   Moved to block device layer.
2006-05-03   sh    Created

===========================================================================*/

#ifndef __FS_BLOCKDEV_FTL_H__
#define __FS_BLOCKDEV_FTL_H__

#ifdef FS_BLOCKDEV_FTL
#include "fs_ftl_cache.h"

typedef struct
{
  int16               driveno;
  ftl_cached_device_t handle;
} ftl_device_data;

/* Expose this driver externally */
extern blockdev_driver blockdev_ftl_driver;

#else /* !FS_BLOCKDEV_FTL */
extern int __dont_complain_about_empty_file;
#endif /* FS_BLOCKDEV_FTL */

#endif /* not __FS_BLOCKDEV_FTL_H__ */
