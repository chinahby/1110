/***********************************************************************
 * fs_fdi_vnode_devops.c
 *
 * Device-file vNode ops.
 * Copyright (C) 2007-2008 QUALCOMM, Inc.
 *
 * This file contains all the Device-file related vNode ops.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_fdi_vnode_devops.c#3 $ $DateTime: 2008/10/29 17:56:01 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-10-06   np    Added shred functionality to delete obsolete user data.
2008-05-23   rp    Changes for new SCM.
2008-01-24   sch   Added getsize to vnode ops
2007-01-01   rp    Create

===========================================================================*/

#include "fs_fdi_cust.h"

#ifdef FEATURE_EFS_FDI

#include "fs_vnode.h"
#include "fs_fdi_i.h"

/* vNode-op for getstat on a device-file. */
static int
v_fdi_devspecial_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  UINT32 fdi_res;

  buf->st_dev = vp->dev;
  buf->st_ino = vp->inum;
  buf->st_mode  = vp->mode;
  buf->st_atime = 0;
  buf->st_mtime = vp->p.named.p.fdi.mtime;
  buf->st_ctime = vp->p.named.p.fdi.ctime;
  buf->st_nlink = 1;
  buf->st_blksize = EFS_PAGE_SIZE;
  buf->st_blocks = 0;
  buf->st_size = 0;
  buf->st_rdev = 0;

  /* Device Major/Minor number */
  fdi_res = IFM_Get (vp->p.named.key, &buf->st_rdev, sizeof(buf->st_rdev));
  if (fdi_res == RTOS_API_FAILURE) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }
  if (fdi_res != sizeof(buf->st_rdev)) {
    FDI_ASSERT (fdi_res == sizeof(buf->st_rdev));
    return -1;
  }

  return 0;
}


/* Device-file vNode-ops. */
const struct vfs_ops fdi_devspecial_vops = {
  fs_vnode_invalid_lookup,
  fs_vnode_invalid_create,
  fs_vnode_invalid_link,
  fs_vnode_invalid_unlink,
  fs_vnode_invalid_write,
  fs_vnode_invalid_read,
  fs_vnode_invalid_truncate,
  fs_vnode_invalid_mkdir,
  fs_vnode_invalid_rmdir,
  fs_vnode_invalid_opendir,
  fs_vnode_invalid_readdir,
  fs_vnode_invalid_closedir,
  v_fdi_devspecial_getstat,
  fs_vnode_invalid_getsize,
  fs_vnode_invalid_rename,
  fs_vnode_invalid_symlink,
  fs_vnode_invalid_readlink,
  v_fdi_dops_getstatvfs,
  fs_vnode_invalid_put_item,
  fs_vnode_invalid_get_item,
  fs_vnode_invalid_mknod,
  v_fdi_dops_chmod,
  v_fdi_dops_chown,
  fs_vnode_invalid_set_reservation,
  fs_vnode_invalid_set_quota,
  v_fdi_dops_get_group_info,
  fs_vnode_invalid_shred
};

#endif /* FEATURE_EFS_FDI */
