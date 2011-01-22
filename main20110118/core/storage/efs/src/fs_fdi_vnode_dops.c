/***********************************************************************
 * fs_fdi_vnode_dops.c
 *
 * FDI vNode-Layer Directory ops.
 * Copyright (C) 2007-2009 QUALCOMM, Inc.
 *
 * This file contains all the Directory related vNode ops.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_fdi_vnode_dops.c#5 $ $DateTime: 2009/04/06 21:22:51 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-04-03   wek   Added invalid utime ops for FDI.
2008-10-06   np    Added shred functionality to delete obsolete user data.
2008-07-28   rp    Corrected f_balloc in statvfs.
2008-05-23   rp    Changes for new SCM.
2008-01-24   sch   Added getsize to vnode ops
2007-12-07   rp    Enhancements to the statvfs structure.
2007-10-01   rp    Create

===========================================================================*/

#include "fs_fdi_cust.h"


#ifdef FEATURE_EFS_FDI

#include "fs_fdi_i.h"
#include "fs_namei.h"
#include "ffs_cmn.h"
#include "fs_fcntl.h"
#include "fs_sys_types.h"
#include "fs_err.h"
#include "fs_util.h"
#include "mfm_cust.h"
#include "fs_fdi_mnt.h"
#include "fs_fdi_vnode_ops.h"

#ifdef _lint
  /* For lint which does not see fdi/fdi_task.c where this variable exists. */
  TCHAR  fs_fdi_volume_name[] = "mfm1";
#endif


/* vNode-op to lookup for a given filepath and fetch the corresponding
 * vNode if the file exists. */
static int
v_fdi_dops_lookup (struct nameidata *ndp)
{
  struct fs_mount_fdi *mp = (struct fs_mount_fdi *) ndp->dvp->mp;
  IFM_stat file_stat;
  UINT32 stat_res;
  struct fs_vnode *vnode;
  fs_inode_t inode_id;
  char temp_ch;
  char *nameend = (char *) (ndp->next + ndp->length);
  size_t res;

  /* terminate the path-string, till the place where we should look */
  temp_ch = *nameend;
  *nameend = '\0';

  /* stat the file */
  stat_res = IFM_Stat(ndp->path, &file_stat);

  /* restore the path-string to its original state */
  *nameend = temp_ch;

  /* Was stat ok? */
  if (stat_res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  /* This frag_id is our inode number. */
  inode_id = (fs_inode_t)file_stat.frag_id;

  /* see if this file's vnode is in vnode-pool. */
  ndp->vp = fs_vnode_lookup (&mp->parent, mp->parent.dev, inode_id);
  if (ndp->vp != NULL) {
    FDI_DBG_ASSERT (ndp->vp->p.named.p.fdi.size == file_stat.size);
    return 0;
  }

  vnode = fs_vnode_alloc ();
  FDI_ASSERT (vnode != NULL);

  vnode->mp = ndp->dvp->mp;
  vnode->dev = mp->parent.dev;
  vnode->dirty = 0;
  vnode->inum = inode_id;
  vnode->mode = fs_fdi_conv_fdi_to_efs_mode (file_stat.mode);

  /* store the file-path in the vnode. */
  temp_ch = *nameend;
  *nameend = '\0';
  vnode->p.named.key_length = strlen (ndp->path);
  res = fs_strlcpy (vnode->p.named.key, ndp->path,sizeof (vnode->p.named.key));
  FDI_ASSERT (res == (fs_size_t)vnode->p.named.key_length);
  *nameend = temp_ch;

  vnode->p.named.p.fdi.size = file_stat.size;
  vnode->p.named.p.fdi.uid = file_stat.owner_id;
  vnode->p.named.p.fdi.gid = file_stat.group_id;
  vnode->p.named.p.fdi.mtime = file_stat.mtime;
  vnode->p.named.p.fdi.ctime = file_stat.ctime;
  vnode->p.named.p.fdi.fdi_fd = FDI_INVALID_FD;

  if (S_ISDIR (vnode->mode))
    vnode->vops = &fdi_dir_vops;
  else if (S_ISREG (vnode->mode))
    vnode->vops = &fdi_file_vops;
  else if (S_ISSPECIAL(vnode->mode))
    vnode->vops = &fdi_devspecial_vops;
  else if (S_ISLNK(vnode->mode))
    vnode->vops = &fdi_symlink_vops;
  else if (S_ISITM(vnode->mode))
    vnode->vops = &fdi_item_vops;
  else
    FS_ERR_FATAL ("Unknown file type", 0, 0, 0);

  ndp->vp = vnode;

  return 0;
}


/* vNode-op to create a file. */
int
v_fdi_dops_create (struct nameidata *ndp, fs_mode_t efs_mode)
{
  UINT32 open_fd;
  UINT32 fdi_oflag;
  UINT16 fdi_mode;
  int lookup_res;
  UINT32 fdi_res;

  fdi_oflag = fs_fdi_conv_efs_to_fdi_oflag (O_CREAT | O_RDWR | O_TRUNC);

  /* Force only files to be created. */
  efs_mode = S_IFREG | (efs_mode & 07777);
  fdi_mode = fs_fdi_conv_efs_to_fdi_mode (efs_mode);

  open_fd = IFM_Open(ndp->path, fdi_oflag, fdi_mode);
  if (open_fd == RTOS_API_FAILURE) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  /*--------------------------------------------------------------------------
    FDI commits the newly created files to the flash, only when the file is
    closed. Ex: In FDI if we create a file, keep writing to it, phone resets
    before we close the file, then upon reboot this new file will not exist.
    So we shall close this new file and open it again.
  --------------------------------------------------------------------------*/
  fdi_res = IFM_Close (open_fd);
  if (fdi_res != RTOS_API_SUCCESS)  {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }
  fdi_oflag = fs_fdi_conv_efs_to_fdi_oflag (O_RDWR);
  open_fd = IFM_Open(ndp->path, fdi_oflag, fdi_mode);
  if (open_fd == RTOS_API_FAILURE) {
    FDI_ASSERT (FALSE);
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  /* Now, lookup for this file and get its vnode. */
  ndp->next = ndp->path;
  ndp->length = strlen (ndp->path);
  lookup_res = v_fdi_dops_lookup (ndp);
  FDI_ASSERT (lookup_res == 0);
  FDI_ASSERT (ndp->vp != NULL);

  ndp->vp->p.named.p.fdi.fdi_fd = open_fd;

  return 0;
}

/* vNode-op for mkdir. */
static int
v_fdi_dops_mkdir (struct nameidata *ndp, fs_mode_t efs_mode)
{
  UINT16 fdi_mode;
  UINT32 mkdir_res;
  int lookup_res;

  fdi_mode = fs_fdi_conv_efs_to_fdi_mode (efs_mode);

  mkdir_res = IFM_Mkdir (ndp->path, fdi_mode);
  if (mkdir_res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  /* Ensure that the dir was created and get its vnode */
  ndp->next = ndp->path;
  ndp->length = strlen (ndp->path);
  lookup_res = v_fdi_dops_lookup (ndp);
  FDI_ASSERT (lookup_res == 0);
  FDI_ASSERT (ndp->vp != NULL);

  return 0;

}

/* vNode-op for unlink. */
static int
v_fdi_dops_unlink (struct nameidata *ndp)
{
  UINT32 unlink_res;

  unlink_res = IFM_Unlink (ndp->path);
  if (unlink_res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  FDI_ASSERT (ndp->vp != NULL);
  fs_vnode_unref (ndp->vp);

  return 0;
}

/* vNode-op for rmdir. */
static int
v_fdi_dops_rmdir (struct nameidata *ndp)
{
  UINT32 rmdir_res;

  rmdir_res = IFM_Rmdir (ndp->path);
  if (rmdir_res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  FDI_DBG_ASSERT (ndp->vp != NULL);
  fs_vnode_unref (ndp->vp);

  return 0;
}

/* vNode-op for opendir */
static void *
v_fdi_dops_opendir (struct fs_vnode *vp)
{
  DIR *fdi_iter;

  FDI_DBG_CHECK_VNODE (vp);
  fdi_iter = IFM_Opendir (vp->p.named.key);
  return fdi_iter;
}

/* vNode-op for readdir */
static int
v_fdi_dops_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent)
{
  DIR *fdi_iter = (DIR *)dir;
  struct Dirent *fdi_dirent;

  (void)vp;

  FDI_DBG_ASSERT (fdi_iter != NULL);

  fdi_dirent = IFM_Readdir (fdi_iter);
  if (fdi_dirent == NULL) {
    if (IFM_Errno == IFM_ENOENT)
      return -EEOF;
    else
      return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  (void)fs_strlcpy (dirent->d_name, fdi_dirent->path_name,
          sizeof (dirent->d_name));
  dirent->d_stat.st_mode = fs_fdi_conv_fdi_to_efs_mode (fdi_dirent->mode);
  dirent->d_stat.st_size = fdi_dirent->file_size;
  dirent->d_stat.st_atime = fdi_dirent->a_time;
  dirent->d_stat.st_mtime = fdi_dirent->m_time;
  dirent->d_stat.st_ctime = fdi_dirent->c_time;

  dirent->d_stats_present = (FS_DIRENT_HAS_ST_MODE | FS_DIRENT_HAS_ST_SIZE | \
    FS_DIRENT_HAS_ST_ATIME | FS_DIRENT_HAS_ST_MTIME | FS_DIRENT_HAS_ST_CTIME);

  return 0;
}

/* vNode-op for closedir */
static int
v_fdi_dops_closedir (struct fs_vnode *vp, void *dir)
{
  DIR *fdi_iter = (DIR *)dir;
  UINT32 fdi_res;

  (void)vp;
  FDI_DBG_ASSERT (fdi_iter != NULL);

  fdi_res = IFM_Closedir (fdi_iter);
  if (fdi_res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  return 0;
}

/* vNode-op for getstat. */
int
v_fdi_dops_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  struct fs_mount_fdi *mp = (struct fs_mount_fdi *) vp->mp;

  FDI_DBG_CHECK_VNODE (vp);
  FDI_DBG_ASSERT (mp != NULL);

  /* We have aleady stat'ed the file upon lookup, so just copy the
   * values from the vNode. */
  buf->st_atime   = 0;
  buf->st_ctime   = vp->p.named.p.fdi.ctime;
  buf->st_mtime   = vp->p.named.p.fdi.mtime;
  buf->st_size    = vp->p.named.p.fdi.size;
  buf->st_mode    = vp->mode;
  buf->st_dev     = mp->parent.dev;
  buf->st_ino     = vp->inum;
  buf->st_nlink   = 1;
  buf->st_blksize = EFS_PAGE_SIZE;
  buf->st_blocks  = 1; /* FDI does not tell us how many blks, so hardcode 1 */
  buf->st_uid     = vp->p.named.p.fdi.uid;
  buf->st_gid     = vp->p.named.p.fdi.gid;

  return 0;
}

/* vNode-op for rename. */
static int
v_fdi_dops_rename (struct nameidata *ndp, struct nameidata *ndp2)
{
  UINT32 fdi_res;

  FDI_DBG_ASSERT (ndp != NULL);
  FDI_DBG_ASSERT (ndp2 != NULL);

  /* Check to see if renaming to same name, if so return
   * without doing anything. */
  if (ndp->vp == ndp2->vp)
    return 0;

  /* old path and new path should be on the same file system */
  if (ndp->dvp->mp != ndp2->dvp->mp) {
    return -EXDEV;
  }

  /* Rename is only allowed for regular files, directories ,item files
     and symlinks */
  if ((ndp->vp != NULL) &&
     ((ndp->vp->mode & S_IFMT) != S_IFREG) &&
     ((ndp->vp->mode & S_IFMT) != S_IFDIR) &&
     ((ndp->vp->mode & S_IFMT) != S_IFITM) &&
     ((ndp->vp->mode & S_IFMT) != S_IFLNK)) {
    return -EINVAL;
  }

  fdi_res = IFM_Rename(ndp->path, ndp2->path);
  if (fdi_res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code (IFM_Errno);
  }

  return 0;
}


/* vNode-op for getstatvfs. */
int
v_fdi_dops_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf)
{
  FDI_BOOLEAN ioctl_res;
  UINT32 statistics_buf[5];
  UINT32 bytes_returned;
  extern TCHAR fs_fdi_volume_name[];

  (void)vp;

  ioctl_res = IFM_Ioctl( (UINT8_PTR)fs_fdi_volume_name,
                       mOS_StrLen(fs_fdi_volume_name),
                       (UINT8_PTR)statistics_buf,
                       sizeof(statistics_buf),
                       &bytes_returned,
                       QC_STATISTICS );
  if(!ioctl_res)
  {
     return fs_fdi_conv_fdi_to_efs_err_code (IFM_Errno);
  }

  buf->f_bsize   = EFS_PAGE_SIZE;
  buf->f_blocks  = (statistics_buf[0] + statistics_buf[2])/buf->f_bsize;
  buf->f_bfree   = (statistics_buf[0] + statistics_buf[1])/buf->f_bsize;
  buf->f_bavail  = buf->f_bfree;
  buf->f_files   = 0;
  /* f_free and f_avail are just made up. These limits don't really exist.
   */
  buf->f_ffree  = 0x01000000;
  buf->f_favail = 0x01000000;
  buf->f_fsid    = 0x2;
  buf->f_flag    = 0;
  buf->f_namemax = FS_NAME_MAX;
  buf->f_maxwrite = 1024; // todo: is this compulsory?
  buf->f_balloc = statistics_buf[3]/buf->f_bsize; // use custom Ioctl
  buf->f_hardalloc = statistics_buf[4]/buf->f_bsize;

  buf->f_pathmax = FS_PATH_MAX;
  buf->f_is_case_sensitive = 1; /* Path is case-sensitive. */
  buf->f_is_case_preserving = 1; /* Path is case-preserved. */
  buf->f_max_file_size = FS_MAX_FILE_SIZE;
  buf->f_max_file_size_unit = 1;
  buf->f_max_open_files = FS_MAX_DESCRIPTORS;
  buf->f_name_rule = FS_FILENAME_RULE_FDI;
  buf->f_name_encoding = FS_FILENAME_ENCODING_8BIT;

  return 0;
}

/* vNode-op for chmod. */
int
v_fdi_dops_chmod (struct fs_vnode *vp, fs_mode_t new_mode)
{
  UINT16 fdi_mode;
  UINT32 fdi_res;
  fs_mode_t old_mode;

  FDI_DBG_CHECK_VNODE (vp);

  /* Only the permission/suid/sticky bits can be set */
  if (new_mode & ~07777)
    return -EINVAL;

  /* Get the existing mode bits */
  old_mode = vp->mode;
  vp->mode = (old_mode & ~07777) | (new_mode & 07777);

  fdi_mode = fs_fdi_conv_efs_to_fdi_mode (vp->mode);

  fdi_res = IFM_Chmod (vp->p.named.key, fdi_mode);
  if (fdi_res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code (IFM_Errno);
  }

  return 0;
}

/* vNode-op for chown. */
int
v_fdi_dops_chown (struct fs_vnode *vp, int uid_val, int gid_val)
{
  UINT32 fdi_res;
  UINT8 fdi_owner;
  UINT32 fdi_group;

  FDI_DBG_CHECK_VNODE (vp);

  fdi_owner = (UINT8)uid_val;
  fdi_group = (UINT32)gid_val;

  /* Set all non-Q&R gid's to 0 */
  if (!FS_IS_QUOTA_RESERVED_GROUP (gid_val))
    fdi_group = 0;

  fdi_res = IFM_Chown (vp->p.named.key, fdi_owner);
  if (fdi_res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code (IFM_Errno);
  }

  fdi_res = IFM_Chgrp(vp->p.named.key, fdi_group);
  if (fdi_res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code (IFM_Errno);
  }

  return 0;
}

/* vNode-op for put-item. */
static int
v_fdi_dops_put_item (struct nameidata *ndp,
    void *data, int length, int efs_mode)
{
  UINT32 fdi_oflag;
  UINT16 fdi_mode;
  UINT32 write_res;

  if (length > FS_ITEM_MAX)
    return -ENAMETOOLONG;

  efs_mode |= S_IFITM;

  fdi_oflag = fs_fdi_conv_efs_to_fdi_oflag (O_CREAT | O_WRONLY);
  fdi_mode = fs_fdi_conv_efs_to_fdi_mode (efs_mode);

  write_res = IFM_Put (ndp->path, data, length, fdi_oflag, fdi_mode);
  if (write_res == RTOS_API_FAILURE) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }
  if (write_res != (UINT32)length) {
    return -1;
  }

  return 0;
}

/* vNode-op for symlink. */
static int
v_fdi_dops_symlink (struct nameidata *ndp, const char *dest)
{
  int vlen;
  UINT32 res;

  vlen = strlen (dest);
  if (vlen > FS_MAX_SYMLINK_TARGET)
    return -ENAMETOOLONG;

  res = IFM_Symlink (dest, ndp->path);
  if (res != RTOS_API_SUCCESS)
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);

  return 0;
}

/* vNode-op for mknod. */
int
v_fdi_dops_mknod (struct nameidata *ndp, fs_mode_t efs_mode,
                        fs_devspecial_t dev)
{
  UINT32 fdi_oflag;
  UINT16 fdi_mode;
  UINT32 write_res;
  int res;

  /* One of S_IFCHR or S_IFBLK, etc must be selected for mknod */
  if (!S_ISSPECIAL(efs_mode))
    return -EINVAL;

  efs_mode &= (S_IFCHR | S_IFBLK | S_IFIFO | S_IFSOCK | 07777);

  fdi_oflag = fs_fdi_conv_efs_to_fdi_oflag (O_CREAT | O_WRONLY);
  fdi_mode = fs_fdi_conv_efs_to_fdi_mode (efs_mode);

  write_res = IFM_Put (ndp->path, &dev, sizeof(dev), fdi_oflag, fdi_mode);
  if (write_res == RTOS_API_FAILURE) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }
  if (write_res != sizeof(dev)) {
    FDI_ASSERT (write_res == sizeof(dev));
    return -1;
  }

  /* Now, lookup for this file and get its vnode. */
  ndp->next = ndp->path;
  ndp->length = strlen (ndp->path);
  res = v_fdi_dops_lookup (ndp);
  FDI_ASSERT (res == 0);
  FDI_ASSERT (ndp->vp != NULL);

  return 0;
}

/* vNode-op for set-reservation. */
int
v_fdi_dops_set_reservation (struct fs_vnode *vp, uint32 fdi_gid, uint32 size)
{
  UINT32 res;

  if (!FS_IS_QUOTA_RESERVED_GROUP (fdi_gid)) {
    return -EPERM;
  }

  res = IFM_Set_Reservation (vp->p.named.key, fdi_gid, size);
  if (res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  return 0;
}

/* vNode-op for set-quota. */
int
v_fdi_dops_set_quota (struct fs_vnode *vp, uint32 fdi_gid, uint32 size)
{
  UINT32 res;

  if (!FS_IS_QUOTA_RESERVED_GROUP (fdi_gid)) {
    return -EPERM;
  }

  res = IFM_Set_Quota (vp->p.named.key, fdi_gid, size);
  if (res != RTOS_API_SUCCESS) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  return 0;
}

/* vNode-op for get-group-info. */
int
v_fdi_dops_get_group_info (struct fs_vnode *vp, uint32 fdi_gid,
    struct fs_group_info *ginfo)
{
  UINT32 res;
  IFM_GroupInfo fdi_group_info = {0,0,0,0};

  if (!FS_IS_QUOTA_RESERVED_GROUP (fdi_gid)) {
    return -EPERM;
  }

  res = IFM_Get_Group_Info (vp->p.named.key, fdi_gid, &fdi_group_info);
  if ( (res != RTOS_API_SUCCESS) && (IFM_Errno != IFM_ENOENT) ) {
    return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  ginfo->quota_size = fdi_group_info.quota_size;
  ginfo->reservation_size = fdi_group_info.resrv_size;
  ginfo->space_used = fdi_group_info.space_used;

  return 0;
}

/* Directory vNode-ops. */
const struct vfs_ops fdi_dir_vops = {
  v_fdi_dops_lookup,
  v_fdi_dops_create,
  fs_vnode_invalid_link,
  v_fdi_dops_unlink,
  fs_vnode_invalid_write,
  fs_vnode_invalid_read,
  fs_vnode_invalid_truncate,
  v_fdi_dops_mkdir,
  v_fdi_dops_rmdir,
  v_fdi_dops_opendir,
  v_fdi_dops_readdir,
  v_fdi_dops_closedir,
  v_fdi_dops_getstat,
  fs_vnode_invalid_getsize,
  v_fdi_dops_rename,
  v_fdi_dops_symlink,
  fs_vnode_invalid_readlink,
  v_fdi_dops_getstatvfs,
  v_fdi_dops_put_item,
  fs_vnode_invalid_get_item,
  v_fdi_dops_mknod,
  v_fdi_dops_chmod,
  v_fdi_dops_chown,
  v_fdi_dops_set_reservation,
  v_fdi_dops_set_quota,
  v_fdi_dops_get_group_info,
  fs_vnode_invalid_shred,
  fs_vnode_invalid_utime
};

#endif /* FEATURE_EFS_FDI */
