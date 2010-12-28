/***********************************************************************
 * fs_fdi_vnode_ops.h
 *
 * FDI vNode-Layer ops.
 * Copyright (C) 2007 QUALCOMM, Inc.
 *
 * This file contains all the vNode ops structures.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_fdi_vnode_ops.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-10-01   rp    Create

===========================================================================*/

#ifndef __FS_FDI_VNODE_OPS_H__
#define __FS_FDI_VNODE_OPS_H__

#include "fs_fdi_cust.h"
#include "fs_vnode.h"

#ifdef FEATURE_EFS_FDI

/* The vNode ops for all file-types. */
extern const struct vfs_ops fdi_dir_vops;
extern const struct vfs_ops fdi_file_vops;
extern const struct vfs_ops fdi_item_vops;
extern const struct vfs_ops fdi_symlink_vops;
extern const struct vfs_ops fdi_devspecial_vops;

#endif /* FEATURE_EFS_FDI */



#endif /* not __FS_FDI_VNODE_OPS_H__ */
