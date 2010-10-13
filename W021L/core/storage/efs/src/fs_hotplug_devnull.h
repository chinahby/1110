/***********************************************************************
 * fs_blockdev_devnull.h
 *
 * Copyright (C) 2006, 2009 QUALCOMM, Inc.
 *
 * This is not a stand-alone header file.
 * It is used to communicate the exported device functions to hotplug.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_devnull.h#2 $ $DateTime: 2009/05/18 17:19:57 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-08   ebb   Moved from hotplug to blockdev.
2006-06-06   sh    Created.
===========================================================================*/

#ifndef __FS_BLOCKDEV_DEVNULL_H__
#define __FS_BLOCKDEV_DEVNULL_H__

#ifdef FS_BLOCKDEV_DEVNULL
extern blockdev_driver blockdev_devnull_driver;
#else
extern int __dont_complain_about_empty_file;
#endif /* FS_BLOCKDEV_DEVNULL */

#endif /* not __FS_BLOCKDEV_DEVNULL_H__ */
