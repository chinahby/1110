/***********************************************************************
 * fs_fdi_vnode_sops.c
 *
 * Symlink vNode-ops.
 * Copyright (C) 2007-2008 QUALCOMM, Inc.
 *
 * This file contains all symlink related vNode ops.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_fdi_vnode_sops.c#3 $ $DateTime: 2008/10/29 17:56:01 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-10-06   np    Added shred functionality to delete obsolete user data.
2008-05-23   rp    Changes for new SCM.
2008-01-24   sch   Added getsize to vnode ops
2007-10-01   rp    Create

===========================================================================*/

#include "fs_fdi_cust.h"

#ifdef FEATURE_EFS_FDI

#include "fs_vnode.h"
#include "fs_fdi_i.h"

/* vNode-op for getstat on a symlink. */
static int
v_fdi_sops_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  buf->st_dev = vp->dev;
  buf->st_ino = vp->inum;
  buf->st_mode  = vp->mode;
  buf->st_mtime = 0;
  buf->st_ctime = 0;
  buf->st_nlink = 0;
  buf->st_size = vp->p.named.p.fdi.size;
  buf->st_blksize = EFS_PAGE_SIZE;
  buf->st_blocks = 0;
  buf->st_atime = 0;

  return 0;
}

/* vNode-op for readlink. */
static int
v_fdi_sops_readlink (struct fs_vnode *vp,
    char *buf, fs_size_t bufsiz)
{
  UINT32 num_bytes;
  UINT32 res;

  num_bytes = (UINT32)bufsiz;

  res = IFM_Readlink (vp->p.named.key, buf, num_bytes);
  if (res == RTOS_API_FAILURE)
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);

  return res;
}


/* symlink vNode-ops. */
const struct vfs_ops fdi_symlink_vops = {
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
  v_fdi_sops_getstat,
  fs_vnode_invalid_getsize,
  fs_vnode_invalid_rename,
  fs_vnode_invalid_symlink,
  v_fdi_sops_readlink,
  v_fdi_dops_getstatvfs,
  fs_vnode_invalid_put_item,
  fs_vnode_invalid_get_item,
  fs_vnode_invalid_mknod,
  fs_vnode_invalid_chmod,
  fs_vnode_invalid_chown,
  fs_vnode_invalid_set_reservation,
  fs_vnode_invalid_set_quota,
  fs_vnode_invalid_get_group_info,
  fs_vnode_invalid_shred,
};

#endif /* FEATURE_EFS_FDI */
