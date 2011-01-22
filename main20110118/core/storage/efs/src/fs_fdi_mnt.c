/***********************************************************************
 * fs_fdi_mnt.c
 *
 * Mount operations for the FDI file-system.
 * Copyright (C) 2007-2008 QUALCOMM, Inc.
 *
 * Mount operations for the FDI file-system.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_fdi_mnt.c#2 $ $DateTime: 2008/06/03 15:15:30 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-05-23   rp    Changes for new SCM.
2007-11-02   rp    Removed the unused variable fdi_resetting.
2007-10-01   rp    Create

===========================================================================*/

#include "fs_fdi_cust.h"


#ifdef FEATURE_EFS_FDI

#include "fs_fdi_mnt.h"
#include "fs_fdi_i.h"
#include "fs_fdi_vnode_ops.h"

static struct fs_mount_fdi fdi_mounts[FS_MAX_FDI_MOUNTS];

static int fdi_fs_mnt_ops_stop (struct fs_mount *mp);

/* Start the FDI file-system. */
static int
fdi_start (void)
{
  UINT32 fdi_res;

  fdi_res = IFM_Start ();
  if (fdi_res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code (IFM_Errno);
  }

  return 0;
}

/* Stop the FDI file-system. */
static int
fdi_shutdown(void)
{
  UINT32 fdi_res;

  fdi_res = IFM_Shutdown ();
  if (fdi_res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code (IFM_Errno);
  }

  return 0;
}

static struct fs_vnode*
fdi_fs_mnt_ops_create_root(struct fs_mount *mpp)
{
  struct fs_vnode *root_vnode;
  IFM_stat file_stat;
  UINT32 stat_res;

  root_vnode = fs_vnode_alloc ();
  FDI_ASSERT (root_vnode != NULL);

  /* stat the file to get the inode-id */
  stat_res = IFM_Stat("/", &file_stat);
  FDI_ASSERT(stat_res == RTOS_API_SUCCESS);

  root_vnode->mp = mpp;
  root_vnode->dev = mpp->dev;
  root_vnode->dirty = 0;
  root_vnode->inum = (fs_inode_t)file_stat.frag_id;
  root_vnode->p.named.p.fdi.size = file_stat.size;
  root_vnode->p.named.p.fdi.uid = file_stat.owner_id;
  root_vnode->p.named.p.fdi.gid = file_stat.group_id;
  root_vnode->p.named.p.fdi.mtime = file_stat.mtime;
  root_vnode->p.named.p.fdi.ctime = file_stat.ctime;
  root_vnode->p.named.p.fdi.fdi_fd = FDI_INVALID_FD;
  root_vnode->p.named.key[0] = '/';
  root_vnode->p.named.key[1] = '\0';
  root_vnode->p.named.key_length = 1;

  root_vnode->mode = fs_fdi_conv_fdi_to_efs_mode (file_stat.mode);
  FDI_ASSERT (S_ISDIR (root_vnode->mode));
  root_vnode->vops = &fdi_dir_vops;

  return root_vnode;
}

/* Mount the FDI file-system */
static int
fdi_fs_mnt_ops_start (struct fs_mount *mpp, const char *args)
{
  struct fs_mount_fdi *fdi_mount = (struct fs_mount_fdi *)mpp;
  int res;

#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */

  (void)args;
  res = fdi_start ();
  FDI_ASSERT (res == 0);

  fdi_mount->root_dir = fdi_fs_mnt_ops_create_root (mpp);

  return 0;
}

/* Unmount the FDI file-system. */
static int
fdi_fs_mnt_ops_stop (struct fs_mount *mp)
{
  int res;

  (void)mp;
  res = fdi_shutdown ();
  FDI_ASSERT (res == 0);

  return 0;
}

static int
fdi_fs_mnt_ops_get_root (struct fs_mount *mpp, struct fs_vnode **rp)
{
  struct fs_mount_fdi *mp = (struct fs_mount_fdi *) mpp;

  *rp = mp->root_dir;
  fs_vnode_ref (*rp);

  return 0;
}

/* Perform any necessary cleanup.  Called just before freeing a vnode.
 * Only called if not NULL. */
static void
fdi_fs_mnt_ops_cleanup (struct fs_mount *mp, struct fs_vnode *vp)
{
  UINT32 close_res;
  (void)mp;

  if (vp->p.named.p.fdi.fdi_fd != FDI_INVALID_FD) {
      close_res = IFM_Close (vp->p.named.p.fdi.fdi_fd);
      FDI_ASSERT (close_res != RTOS_API_FAILURE);
      vp->p.named.p.fdi.fdi_fd = FDI_INVALID_FD;
  }

  return;
}

static struct fs_mount_ops fdi_mnt_ops = {
  fdi_fs_mnt_ops_start,
  fdi_fs_mnt_ops_stop,
  fdi_fs_mnt_ops_get_root,
  NULL, /* create_inode */
  NULL, /* write_inode */
  NULL, /* read_inode */
  NULL, /* read_named_inode */
  fdi_fs_mnt_ops_cleanup,
};


void
fs_fdi_mnt_init (void)
{
  int i;

  for (i=0; i<FS_MAX_FDI_MOUNTS; ++i) {
    fdi_mounts[i].parent.ops = &fdi_mnt_ops;

    fs_mount_register ("FDI", &fdi_mounts[i].parent);
  }
}

#endif /* FEATURE_EFS_FDI */
