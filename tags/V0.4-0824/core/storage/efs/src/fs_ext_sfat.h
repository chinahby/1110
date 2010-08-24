/***********************************************************************
 * fs_ext_sfat.h
 *
 * Exports from SFAT to EXTFS
 * Copyright (C) 2006 QUALCOMM, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_ext_sfat.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2006-08-11   s h   Create

===========================================================================*/

#ifndef __FS_EXT_SFAT_H__
#define __FS_EXT_SFAT_H__

#ifdef FEATURE_EFS_EXTFS_SFAT

#include "fs_extfs.h"
#include "fsfat_sdapi.h"      /* For fsfat_pc_format() */

void fs_sfat_init (void);

extern struct fs_extfs_ops fs_sfat_ops;

#define FS_FAT_INIT fs_sfat_init
#define FS_FAT_OPS  fs_sfat_ops
#define FS_FAT_FORMAT(hdev) (fsfat_pc_format((hdev)->index) == 0)

#endif /* FEATURE_EFS_EXTFS_SFAT */
#endif /* not __FS_EXT_SFAT_H__ */
