/***********************************************************************
 * fs_ext_hfat.h
 *
 * HFAT glue interface
 * Copyright (C) 2006 QUALCOMM, Inc.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_ext_hfat.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2006-09-05   sh    fs_hfat_ops must be extern.  Fixed.
2006-08-27   sh    Add macros to provide INIT/OPS/FORMAT using HFAT
2006-08-09   sch   Create

===========================================================================*/

#ifndef __FS_EXT_HFAT_H__
#define __FS_EXT_HFAT_H__

#ifdef FEATURE_EFS_EXTFS_HFAT

#include "fs_extfs.h"

void fs_hfat_init (void);
int  fs_hfat_format (int drive_index);

extern struct fs_extfs_ops fs_hfat_ops;

#define FS_FAT_INIT fs_hfat_init
#define FS_FAT_OPS  fs_hfat_ops
#define FS_FAT_FORMAT(hdev) fs_hfat_format((hdev)->index)

#endif /* FEATURE_EFS_EXTFS_HFAT */

#endif /* not __FS_EXT_HFAT_H__ */
