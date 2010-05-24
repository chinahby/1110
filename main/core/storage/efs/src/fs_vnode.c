/***********************************************************************
 * fs_vnode.c
 *
 * Vnode operations for EFS2
 * Copyright (C) 2002--2009, Qualcomm, Inc.
 *
 * This file defines the functions related to vnode operations
 ***********************************************************************/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_vnode.c#5 $ $DateTime: 2009/05/20 16:03:23 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2009-05-18    rp      Removed obsolete feature FS_FEATURE_RMTEFS_SERVER.
2009-04-03    wek     Added utime ops to nodev.
2008-12-17    ebb     Updated fs_vnode_init to zero mp and ref fields.
2008-10-06    np      Added shred functionality to delete obsolete user data.
2008-04-28    np      Updated vnode pool size definition.
2008-01-24    sch     Added invalid stub for getsize vnode op
2007-08-19    rp      Use FS_MAX_DESCRIPTORS from fs_config.h
2007-08-18    rp      struct fs_vnode cleanup.
2007-05-25    umr     Added support for EFS memory protection feature.
2006-11-10    sh      Add chmod support.
2006-09-20    dlb     Lint cleanup.
2006-07-12    dlb     Fix bug in named vnode lookup.
2006-06-27    dlb     Pass stat through readdir.
2006-06-26     yg     Memory reduction effort
2006-05-10    dlb     Add generation count to nodev mpoint.
2006-05-09    dlb     Export nodev.
2006-01-11    nrs     Fixed Copyright header
2005-11-12     sh     Use rex tcb err_num instead of global efs_errno
2005-10-18    nrs     Fixed prototypes for quotas & reservations
2005-08-30    dlb     Fix mode type on mknod.
2005-08-23    sh      Added mknod
2005-08-10    dlb     Count vnodes correctly in utest.
2005-08-10    dlb     Closedir must work on ENODEV vnodes.
2005-08-04    dlb     Support NODEV vnode.
2005-08-01    nrs     Added calls to set and get for quotas and reservations
2005-07-15    nrs     Added implementation for efs_chown
2005-05-26    sd      Compilation fixes for L4.
2005-03-25    dlb     Allow invalidate of multiple ref'd vnodes.
2005-02-07    dlb     Increase size of vnode pool.
2004-10-17    dlb     Add vnode obliteration.
2004-10-15    dlb     Update copyright header.
2004-07-19    dlb     Restructure vnode interface.
2003-10-17     gr     Moved fs_time to fs_util.c. Out of place in this file.
2003-06-17    jkl     Clean up code.
2003-06-13     gr     Added a function to get the ref count of a vnode.
2002-08-27    drh     Changed comparisons of cluster_id types to remove
                      compiler warnings and added TO DO with them to check
                      later on.
2002-08-08    drh     Created by adm.  Added history header.

===========================================================================*/

#include <string.h>

#include "assert.h"
#include "fs_sys_types.h"
#include "fs_err.h"
#include "fs_vnode.h"
#include "fs_mount.h"
#include "fs_protected_memory.h"

/**********************************************************************
 * Vnode pool management.
 *
 * The vnodes can be stored one of two ways.  Most entities will be looked
 * up through the dev and inum fields of the inode.  However, if inum is
 * set to FS_INVALID_INODE_NUMBER, then the vnode is name indexed rather
 * than numerically indexed.  The key and key_length fields of the "named"
 * field of the 'p' structure will be used for this key.
 *
 * Notes for implementing filesystems.  If some kind of unique number can
 * be determined for a particular vnode, it should be used.  Possibilites
 * include offsets into index blocks and other such things.  These unique
 * numbers are used to keep track of the vnode pool, and don't need to have
 * any kind of meaning outside of EFS.  The named index should only be used
 * for entities that are truly stored by name, such as entries in a
 * database.
 */

/* Vnode pool.  Size is statically defined for now.  This should be larger
 * than the maximum number of open file descriptors by the following:
 * - 1 for each mountpoint.
 * - 1 for each open iterator.
 * - 3 needed for temporary use to create directories.
 * - 1 needed for mounting the root.
 */
#ifndef FS_VNODE_POOL_SIZE
  #define FS_VNODE_POOL_SIZE      (FS_MAX_DESCRIPTORS+3+1\
                                  +FS_MAX_ITERATORS+FS_MAX_EXTFS_MOUNTS)
#endif

static struct fs_vnode vnode_pool[FS_VNODE_POOL_SIZE];
static struct fs_vnode *vnode_free = NULL;

struct fs_vnode *
fs_vnode_alloc (void)
{
  struct fs_vnode *here = vnode_free;

  if (here == 0)
    return here;

  vnode_free = here->next;

  memset (here, 0, sizeof(*here));
  here->ref = 1;

  return here;
}

void
fs_vnode_ref (struct fs_vnode *vn)
{
  vn->ref++;
}

void
fs_vnode_unref (struct fs_vnode *vn)
{
  vn->ref--;

  if (vn->ref < 0)
    FS_ERR_FATAL ("Vnode reference dropped below zero", 0, 0, 0);

  if (vn->ref == 0)
  {
    if (vn->dirty)
      FS_ERR_FATAL ("Unreferenced dirty vnode", 0, 0, 0);

    if (vn->mp->ops->cleanup != NULL)
      vn->mp->ops->cleanup (vn->mp, vn);

    vn->mode = 0;
    vn->next = vnode_free;
    vnode_free = vn;
  }
}

/* Upon transaction rollback, cause the cached vnode information to be
 * invalidated.  This also does an implicit unref (which, in the common
 * case allows the vnode to just be discarded). */
void
fs_vnode_invalidate (struct fs_vnode *vn)
{
  int result;

  if (vn->ref == 1) {
    /* If this is the only reference to the vnode, just discard it. */
    vn->dirty = 0;

    if (vn->mp->ops->cleanup != NULL)
      FS_ERR_FATAL ("Cannot have cleanup operation and invalidate", 0, 0, 0);

    vn->dev = FS_INVALID_DEV;
    vn->inum = FS_INVALID_INODE;

    fs_vnode_unref (vn);
  } else {
    /* Otherwise, since something else still references this vnode, re-read
     * the vnode information from the device. */

    if (vn->inum == FS_INVALID_INODE) {
      /* Named lookup. */
      ASSERT (vn->mp->ops->read_named_inode != NULL);

      result = vn->mp->ops->read_named_inode (
          vn->mp, vn, vn->mode, vn->p.named.key, vn->p.named.key_length);
      if (result != 0)
        FS_ERR_FATAL ("Unable to re-read named inode information", 0, 0, 0);
      /* Everything else should still be set correctly. */
    } else {
      /* Numbered lookup. */
      result = vn->mp->ops->read_inode (vn->mp, vn, vn->inum);
      if (result != 0)
        FS_ERR_FATAL ("Unable to re-read inode information", 0, 0, 0);
    }

    /* Now unreference our ref. */
    vn->dirty = 0;
    fs_vnode_unref (vn);
  }
}

int
fs_vnode_getref (struct fs_vnode *vn)
{
  return vn->ref;
}

/* XXX: If the vnode pool gets too large, it might help to hash this
 * information. */
struct fs_vnode *
fs_vnode_lookup (struct fs_mount *mp, fs_dev_t dev, fs_inode_t inode)
{
  int i, result;
  struct fs_vnode *vp;

  /* Not a terribly efficient way to do this. */
  for (i = 0; i < FS_VNODE_POOL_SIZE; i++)
  {
    if (vnode_pool[i].mode != 0 &&
        vnode_pool[i].dev == dev &&
        vnode_pool[i].inum == inode)
    {
      fs_vnode_ref (&vnode_pool[i]);
      return &vnode_pool[i];
    }
  }

  /* Allocate the vnode, and attempt to read in its contents. */
  if (mp->ops->read_inode == NULL)
    return NULL;

  vp = fs_vnode_alloc ();
  if (vp == NULL)
    return NULL;

  vp->dev = dev;
  vp->inum = inode;
  vp->mp = mp;
  vp->dirty = 0;
  result = mp->ops->read_inode (mp, vp, inode);

  if (result != 0) {
    fs_vnode_unref (vp);
    return NULL;
  }

  return vp;
}

/* Note that passing in a mode of 0 indicates that any mode will match. */
struct fs_vnode *
fs_vnode_named_lookup (struct fs_mount *mp, fs_dev_t dev,
    fs_mode_t mode,
    const char *key, int key_len)
{
  int i, result;
  struct fs_vnode *vp;

  for (i = 0; i < FS_VNODE_POOL_SIZE; i++) {
    if (vnode_pool[i].mode != 0 &&
        (mode == 0 ||
          (vnode_pool[i].mode & S_IFMT) == (mode & S_IFMT)) &&
        vnode_pool[i].dev == dev &&
        vnode_pool[i].inum == FS_INVALID_INODE &&
        vnode_pool[i].p.named.key_length == (unsigned) key_len &&
        memcmp (key, vnode_pool[i].p.named.key, key_len) == 0)
    {
      fs_vnode_ref (&vnode_pool[i]);
      return &vnode_pool[i];
    }
  }

  /* Allocate the vnode, and attempt to read in its contents. */
  if (mp->ops->read_named_inode == NULL)
    return NULL;

  vp = fs_vnode_alloc ();
  if (vp == NULL)
    return NULL;

  vp->dev = dev;
  vp->inum = FS_INVALID_INODE;
  vp->mp = mp;
  vp->dirty = 0;
  result = mp->ops->read_named_inode (mp, vp, mode, key, key_len);

  if (result != 0) {
    fs_vnode_unref (vp);
    return NULL;
  }

  return vp;
}

void
fs_vnode_init (void)
{
  int i;

  vnode_free = 0;

  for (i = 0; i < FS_VNODE_POOL_SIZE; i++)
  {
    vnode_pool[i].ref = 0;
    vnode_pool[i].mp = 0;
    vnode_pool[i].mode = 0;
    vnode_pool[i].dirty = 0;
    vnode_pool[i].next = vnode_free;
    vnode_free = &vnode_pool[i];
  }
}

void
fs_vnode_flush (struct fs_vnode *vn)
{
  if (vn->dirty) {
    (void) vn->mp->ops->write_inode (vn->mp, vn);

    vn->dirty = 0;
  }
}

/* These are all of the invalid operations, so that various mountpoints
 * don't have to provide private error returning implementations. */
int
fs_vnode_invalid_lookup (struct nameidata *ndp)
{
  (void) ndp;

  return -ENOTDIR;
}

int
fs_vnode_invalid_create (struct nameidata *ndp, fs_mode_t mode)
{
  (void) ndp;
  (void) mode;

  return -ENOTDIR;
}

int
fs_vnode_invalid_link (struct nameidata *ndp)
{
  (void) ndp;

  return -ENOTDIR;
}

int
fs_vnode_invalid_unlink (struct nameidata *ndp)
{
  (void) ndp;

  return -ENOTDIR;
}

fs_ssize_t
fs_vnode_invalid_write (struct fs_vnode *vp, fs_off_t pos,
    const void *buf, fs_size_t count)
{
  (void) vp;
  (void) pos;
  (void) buf;
  (void) count;

  return -EINVAL;
}

fs_ssize_t
fs_vnode_invalid_read (struct fs_vnode *vp, fs_off_t pos,
    void *buf, fs_size_t count)
{
  (void) vp;
  (void) pos;
  (void) buf;
  (void) count;

  return -EINVAL;
}

int
fs_vnode_invalid_truncate (struct fs_vnode *vp, fs_off_t pos)
{
  (void) vp;
  (void) pos;

  return -EINVAL;
}

int
fs_vnode_invalid_mkdir (struct nameidata *ndp, fs_mode_t mode)
{
  (void) ndp;
  (void) mode;

  return -EINVAL;
}

int
fs_vnode_invalid_rmdir (struct nameidata *ndp)
{
  (void) ndp;

  return -EINVAL;
}

void *
fs_vnode_invalid_opendir (struct fs_vnode *vp)
{
  (void) vp;

  return NULL;
}

int
fs_vnode_invalid_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent)
{
  (void) vp;
  (void) dir;
  (void) dirent;

  return -EINVAL;
}

int
fs_vnode_invalid_closedir (struct fs_vnode *vp, void *dir)
{
  (void) vp;
  (void) dir;

  return -EINVAL;
}

int
fs_vnode_invalid_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  (void) vp;
  (void) buf;

  return -EINVAL;
}

int
fs_vnode_invalid_getsize (struct fs_vnode *vp, uint32 *numbytes)
{
  (void) vp;
  (void) numbytes;

  return -EINVAL;
}

int
fs_vnode_invalid_rename (struct nameidata *ndp, struct nameidata *ndp2)
{
  (void) ndp;
  (void) ndp2;

  return -EINVAL;
}

int
fs_vnode_invalid_symlink (struct nameidata *ndp, const char *dest)
{
  (void) ndp;
  (void) dest;

  return -EINVAL;
}

int
fs_vnode_invalid_readlink (struct fs_vnode *vp,
    char *buf, fs_size_t bufsiz)
{
  (void) vp;
  (void) buf;
  (void) bufsiz;

  return -EINVAL;
}

int
fs_vnode_invalid_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf)
{
  (void) vp;
  (void) buf;

  return -EINVAL;
}

int
fs_vnode_invalid_put_item (struct nameidata *ndp,
    void *data, int length, int mode)
{
  (void) ndp;
  (void) data;
  (void) length;
  (void) mode;

  return -EINVAL;
}

int
fs_vnode_invalid_mknod (struct nameidata *ndp, fs_mode_t mode,
                        fs_devspecial_t dev)
{
  (void) ndp;
  (void) mode;
  (void) dev;

  return -EINVAL;
}

int
fs_vnode_invalid_get_item (struct fs_vnode *dvp, void *data, int length)
{
  (void) dvp;
  (void) data;
  (void) length;

  return -EINVAL;
}

/* The nodev vops. */
static int vnodev_lookup (struct nameidata *ndp);
static int vnodev_create (struct nameidata *ndp, fs_mode_t mode);
static int vnodev_link (struct nameidata *ndp);
static int vnodev_unlink (struct nameidata *ndp);
static fs_ssize_t vnodev_write (struct fs_vnode *vp, fs_off_t pos,
    const void *buf, fs_size_t count);
static fs_ssize_t vnodev_read (struct fs_vnode *vp, fs_off_t pos,
    void *buf, fs_size_t count);
static int vnodev_truncate (struct fs_vnode *vp, fs_off_t pos);
static int vnodev_mkdir (struct nameidata *ndp, fs_mode_t mode);
static int vnodev_rmdir (struct nameidata *ndp);
static void *vnodev_opendir (struct fs_vnode *vp);
static int vnodev_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent);
static int vnodev_closedir (struct fs_vnode *vp, void *dir);
static int vnodev_getstat (struct fs_vnode *vp, struct fs_stat *buf);
static int vnodev_getsize (struct fs_vnode *vp, uint32 *numbytes);
static int vnodev_rename (struct nameidata *ndp, struct nameidata *ndp2);
static int vnodev_symlink (struct nameidata *ndp, const char *dest);
static int vnodev_readlink (struct fs_vnode *vp,
    char *buf, fs_size_t bufsiz);
static int vnodev_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf);
static int vnodev_put_item (struct nameidata *ndp, void *data,
    int length, int mode);
static int vnodev_get_item (struct fs_vnode *vp, void *data, int length);
static int vnodev_mknod (struct nameidata *ndp, fs_mode_t mode,
    fs_devspecial_t dev);
static int vnodev_chmod (struct fs_vnode *vp, fs_mode_t mode);
static int vnodev_chown (struct fs_vnode *vp, int uid_val,
    int gid_val);
static int vnodev_set_reservation (struct fs_vnode *vp, uint32 gid,
    fs_size_t size);
static int vnodev_set_quota (struct fs_vnode *vp, uint32 gid,
    uint32 size);
static int vnodev_get_group_info (struct fs_vnode *vp, uint32 gid,
    struct fs_group_info *ginfo);
static int vnodev_utime (struct fs_vnode *vp, const struct fs_utimbuf *times);

const struct vfs_ops fs_vnode_nodev_ops = {
  vnodev_lookup,
  vnodev_create,
  vnodev_link,
  vnodev_unlink,
  vnodev_write,
  vnodev_read,
  vnodev_truncate,
  vnodev_mkdir,
  vnodev_rmdir,
  vnodev_opendir,
  vnodev_readdir,
  vnodev_closedir,
  vnodev_getstat,
  vnodev_getsize,
  vnodev_rename,
  vnodev_symlink,
  vnodev_readlink,
  vnodev_getstatvfs,
  vnodev_put_item,
  vnodev_get_item,
  vnodev_mknod,
  vnodev_chmod,
  vnodev_chown,
  vnodev_set_reservation,
  vnodev_set_quota,
  vnodev_get_group_info,
  fs_vnode_invalid_shred,
  vnodev_utime
};

static int
vnodev_lookup (struct nameidata *ndp)
{
  (void) ndp;
  return -ENODEV;
}

static int
vnodev_create (struct nameidata *ndp, fs_mode_t mode)
{
  (void) ndp;
  (void) mode;
  return -ENODEV;
}

static int
vnodev_link (struct nameidata *ndp)
{
  (void) ndp;
  return -ENODEV;
}

static int
vnodev_unlink (struct nameidata *ndp)
{
  (void) ndp;
  return -ENODEV;
}

static fs_ssize_t
vnodev_write (struct fs_vnode *vp, fs_off_t pos,
    const void *buf, fs_size_t count)
{
  (void) vp;
  (void) pos;
  (void) buf;
  (void) count;
  return -ENODEV;
}

static fs_ssize_t
vnodev_read (struct fs_vnode *vp, fs_off_t pos,
    void *buf, fs_size_t count)
{
  (void) vp;
  (void) pos;
  (void) buf;
  (void) count;
  return -ENODEV;
}

static int
vnodev_truncate (struct fs_vnode *vp, fs_off_t pos)
{
  (void) vp;
  (void) pos;
  return -ENODEV;
}

static int
vnodev_mkdir (struct nameidata *ndp, fs_mode_t mode)
{
  (void) ndp;
  (void) mode;
  return -ENODEV;
}

static int
vnodev_rmdir (struct nameidata *ndp)
{
  (void) ndp;
  return -ENODEV;
}

static void *
vnodev_opendir (struct fs_vnode *vp)
{
  (void) vp;
  return NULL;
  /* return -ENODEV; */
}

static int
vnodev_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent)
{
  (void) vp;
  (void) dir;
  (void) dirent;
  return -ENODEV;
}

/* Closedir is a special case, since we allow it to pass.  This method is
 * only invoked through the efs_closedir call, which should have already
 * checked for its existence. */
static int
vnodev_closedir (struct fs_vnode *vp, void *dir)
{
  (void) vp;
  (void) dir;
  return 0;
}

static int
vnodev_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  (void) vp;
  (void) buf;
  return -ENODEV;
}

static int
vnodev_getsize (struct fs_vnode *vp, uint32 *size)
{
  (void) vp;
  (void) size;
  return -ENODEV;
}

static int
vnodev_rename (struct nameidata *ndp, struct nameidata *ndp2)
{
  (void) ndp;
  (void) ndp2;
  return -ENODEV;
}

static int
vnodev_symlink (struct nameidata *ndp, const char *dest)
{
  (void) ndp;
  (void) dest;
  return -ENODEV;
}

static int
vnodev_readlink (struct fs_vnode *vp,
    char *buf, fs_size_t bufsiz)
{
  (void) vp;
  (void) buf;
  (void) bufsiz;
  return -ENODEV;
}

static int
vnodev_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf)
{
  (void) vp;
  (void) buf;
  return -ENODEV;
}

static int
vnodev_put_item (struct nameidata *ndp, void *data,
    int length, int mode)
{
  (void) ndp;
  (void) data;
  (void) length;
  (void) mode;
  return -ENODEV;
}

static int
vnodev_get_item (struct fs_vnode *vp, void *data, int length)
{
  (void) vp;
  (void) data;
  (void) length;
  return -ENODEV;
}

static int
vnodev_mknod (struct nameidata *ndp, fs_mode_t mode,
    fs_devspecial_t dev)
{
  (void) ndp;
  (void) mode;
  (void) dev;
  return -ENODEV;
}

static int
vnodev_chmod (struct fs_vnode *vp, fs_mode_t mode)
{
  (void) vp;
  (void) mode;

  return -ENODEV;
}

static int
vnodev_chown (struct fs_vnode *vp, int uid_val, int gid_val)
{
  (void) vp;
  (void) uid_val;
  (void) gid_val;

  return -ENODEV;
}

static int
vnodev_utime (struct fs_vnode *vp, const struct fs_utimbuf *times)
{
  (void) vp;
  (void) times;

  return -ENODEV;
}

static int
vnodev_set_reservation (struct fs_vnode *vp, uint32 gid,
    fs_size_t size)
{
  (void) vp;
  (void) gid;
  (void) size;

  return -ENODEV;
}

static int
vnodev_set_quota (struct fs_vnode *vp, uint32 gid, uint32 size)
{
  (void) vp;
  (void) gid;
  (void) size;

  return -ENODEV;
}

static int
vnodev_get_group_info (struct fs_vnode *vp, uint32 gid,
    struct fs_group_info *ginfo)
{
  (void) vp;
  (void) gid;
  (void) ginfo;

  return -ENODEV;
}


static const struct fs_mount_ops mnodev_ops = {
  0, 0, 0, 0, 0, 0, 0, 0
};

static const struct fs_mount mnodev_mpoint = {
  &mnodev_ops,
  FS_INVALID_DEV,
  0,
};

void
fs_vnode_make_nodev (struct fs_vnode *vp)
{
  vp->mp = (struct fs_mount *) &mnodev_mpoint;
  vp->dev = FS_INVALID_DEV;
  vp->inum = FS_INVALID_INODE;

  vp->dirty = 0;
  vp->vops = &fs_vnode_nodev_ops;
}

int
fs_vnode_invalid_chmod (struct fs_vnode *vp, fs_mode_t mode)
{
  (void) vp;
  (void) mode;

  return -EINVAL;
}

int
fs_vnode_invalid_chown (struct fs_vnode *vp, int uid_val, int gid_val)
{
  (void) vp;
  (void) uid_val;
  (void) gid_val;

  return -EINVAL;
}

int fs_vnode_invalid_utime (struct fs_vnode *vp,
                            const struct fs_utimbuf *times)
{
  (void) vp;
  (void) times;

  return -EINVAL;
}

int
fs_vnode_invalid_set_reservation (struct fs_vnode *vp, uint32 gid,
    fs_size_t size)
{
  (void) vp;
  (void) gid;
  (void) size;

  return -EINVAL;
}

int
fs_vnode_invalid_set_quota (struct fs_vnode *vp, uint32 gid, uint32 size)
{
  (void) vp;
  (void) gid;
  (void) size;

  return -EINVAL;
}

int
fs_vnode_invalid_get_group_info (struct fs_vnode *vp, uint32 gid,
    struct fs_group_info *ginfo)
{
  (void) vp;
  (void) gid;
  (void) ginfo;

  return -EINVAL;
}


int
fs_vnode_invalid_shred (struct fs_vnode *vp)
{
  (void) vp;

  return -EPERM;
}

#ifdef FS_UNIT_TEST
#error code not present
#endif

#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */
