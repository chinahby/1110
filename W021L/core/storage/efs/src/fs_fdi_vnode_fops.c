/***********************************************************************
 * fs_fdi_vnode_fops.c
 *
 * File related vNode-ops.
 * Copyright (C) 2007-2009 QUALCOMM, Inc.
 *
 * This file contain all the file related vNode-ops.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_fdi_vnode_fops.c#5 $ $DateTime: 2009/04/06 21:22:51 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-04-03   wek   Added invalid utime ops for FDI.
2008-10-06   np    Added shred functionality to delete obsolete user data.
2008-07-28   rp    Fix RVCT compiler warning.
2008-05-23   rp    Changes for new SCM.
2008-01-24   sch   Added getsize to vnode ops
2007-10-01   rp    Create

===========================================================================*/


#include "fs_fdi_cust.h"

#ifdef FEATURE_EFS_FDI

#include "fs_vnode.h"
#include "fs_fdi_i.h"
#include "fs_fcntl.h"

/* Helper function to create/open a file. */
static int
fdi_fops_open_helper (struct fs_vnode *vp)
{
  UINT32 open_fd;
  UINT32 fdi_oflag;
  UINT16 fdi_mode;

  FDI_DBG_CHECK_VNODE (vp);

  fdi_oflag = fs_fdi_conv_efs_to_fdi_oflag (O_CREAT | O_RDWR);
  fdi_mode = fs_fdi_conv_efs_to_fdi_mode (vp->mode);

  open_fd = IFM_Open(vp->p.named.key, fdi_oflag, fdi_mode);
  if (open_fd == RTOS_API_FAILURE) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  vp->p.named.p.fdi.fdi_fd = open_fd;

  return 0;
}


/* vNode-op to truncate a file. */
static int
v_fdi_fops_truncate (struct fs_vnode *vp, fs_off_t pos)
{
  UINT32 trunc_res;
  UINT32 trunc_len;
  int res;

  trunc_len = (UINT32)pos;

  /* open the file if it is not already open. */
  if (vp->p.named.p.fdi.fdi_fd == FDI_INVALID_FD) {
    res = fdi_fops_open_helper (vp);
    if (res != 0) {
      return res;
    }
  }

  trunc_res = IFM_Ftruncate (vp->p.named.p.fdi.fdi_fd, trunc_len);
  if (trunc_res == RTOS_API_FAILURE)
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);

  /* update the file-size in vnode */
  vp->p.named.p.fdi.size = trunc_len;

  return 0;
}

/* vNode-op to write to a file. */
static fs_ssize_t
v_fdi_fops_write (struct fs_vnode *vp, fs_off_t pos,
    const void *buf, fs_size_t count)
{
  UINT32 fdi_fd;
  UINT32 num_bytes;
  UINT32 write_res;
  fs_ssize_t ret_res;
  int res;

  /* open the file if it is not already open. */
  if (vp->p.named.p.fdi.fdi_fd == FDI_INVALID_FD) {
    res = fdi_fops_open_helper (vp);
    if (res != 0) {
      return res;
    }
  }

  fdi_fd = vp->p.named.p.fdi.fdi_fd;
  num_bytes = (UINT32)count;

  if (pos == FS_OFFSET_APPEND) {
    write_res = IFM_Lseek (fdi_fd, 0, FDI_SEEK_END);
    if (write_res == RTOS_API_FAILURE) {
      return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
    }
  }
  else {
    write_res = IFM_Lseek (fdi_fd, pos, FDI_SEEK_SET);
    if (write_res == RTOS_API_FAILURE) {
      return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
    }
  }

  write_res = IFM_Write (fdi_fd, (VOID_PTR)buf, num_bytes);
  if (write_res == RTOS_API_FAILURE) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  /* update the file-size in vnode */
  if (pos == FS_OFFSET_APPEND) {
    vp->p.named.p.fdi.size += write_res;
  }
  else if ( (pos + write_res) > vp->p.named.p.fdi.size ) {
    vp->p.named.p.fdi.size = pos + write_res;
  }

  ret_res = (fs_size_t)write_res;
  return ret_res;
}

/* vNode-op to read from a file. */
static fs_ssize_t
v_fdi_fops_read (struct fs_vnode *vp, fs_off_t pos,
    void *buf, fs_size_t count)
{
  UINT32 fdi_fd;
  UINT32 num_bytes;
  UINT32 read_res;
  fs_ssize_t ret_res;
  int res;

  (void)pos;

  /* open the file if it is not already open. */
  if (vp->p.named.p.fdi.fdi_fd == FDI_INVALID_FD) {
    res = fdi_fops_open_helper (vp);
    if (res != 0) {
      return res;
    }
  }

  fdi_fd = vp->p.named.p.fdi.fdi_fd;
  num_bytes = (UINT32)count;

  read_res = IFM_Lseek (fdi_fd, pos, FDI_SEEK_SET);
  if (read_res == RTOS_API_FAILURE) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  read_res = IFM_Read (fdi_fd, buf, num_bytes);
  if (read_res == RTOS_API_FAILURE)
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);

  ret_res = (fs_size_t)read_res;
  return ret_res;
}



/* File vNode-ops. */
const struct vfs_ops fdi_file_vops = {
  fs_vnode_invalid_lookup,
  fs_vnode_invalid_create,
  fs_vnode_invalid_link,
  fs_vnode_invalid_unlink,
  v_fdi_fops_write,
  v_fdi_fops_read,
  v_fdi_fops_truncate,
  fs_vnode_invalid_mkdir,
  fs_vnode_invalid_rmdir,
  fs_vnode_invalid_opendir,
  fs_vnode_invalid_readdir,
  fs_vnode_invalid_closedir,
  v_fdi_dops_getstat,
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
  v_fdi_dops_set_reservation,
  v_fdi_dops_set_quota,
  v_fdi_dops_get_group_info,
  fs_vnode_invalid_shred,
  fs_vnode_invalid_utime
};

#endif /* FEATURE_EFS_FDI */
