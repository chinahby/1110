/***********************************************************************
 * fs_hotplug_sd_cprm.h
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

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_blockdev_sd_cprm.h#1 $ $DateTime: 2009/05/18 17:19:57 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-08   ebb   Moved under block device module.
2006-11-12   s h   Cleanup
2006-09-28   sep   Created.

===========================================================================*/

#ifndef __FS_HOTPLUG_SD_CPRM_H__
#define __FS_HOTPLUG_SD_CPRM_H__


#include "fs_blockdev_i.h"

#ifdef FS_BLOCKDEV_SD_CPRM

/* Include basic SD driver to get sd_device_data type*/
#include "fs_blockdev_sd.h"

/* This is the only external structure we export */
extern blockdev_driver blockdev_sd_cprm_driver;

#else
extern int __dont_complain_about_empty_file;
#endif /* FS_BLOCKDEV_SD_CPRM */

#endif  /* __FS_HOTPLUG_SD_CPRM_H__ */
