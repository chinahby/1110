/***********************************************************************
 * fs_fdi_mnt.h
 *
 * Mount operations for the FDI file-system.
 * Copyright (C) 2007 QUALCOMM, Inc.
 *
 * Mount operations for the FDI file-system.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_fdi_mnt.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-10-01   rp    Create

===========================================================================*/

#ifndef __FS_FDI_MNT_H__
#define __FS_FDI_MNT_H__

#include "fs_fdi_cust.h"
#include "fs_mount.h"

struct fs_mount_fdi {
  struct fs_mount   parent;
  struct fs_vnode   *root_dir;
};


void fs_fdi_mnt_init (void);


#endif /* not __FS_FDI_MNT_H__ */
