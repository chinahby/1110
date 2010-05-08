/***********************************************************************
 * fs_vnode.h
 *
 * Vnode operations for EFS2
 * Copyright (C) 2002-2008, Qualcomm, Inc.
 *
 * This file includes the declarations for function related to vnode
 * operations
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_vnode.h#3 $ $DateTime: 2008/10/29 17:56:01 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2008-10-06    np      Added shred functionality to delete obsolete user data.
2008-05-23    rp      Move NAMED_VNODE_SIZE to fs_sys_types.h for new SCM.
2008-01-24    sch     Added getsize to vnode operations
2007-10-18    rp      Featurized QEFS2.
2007-10-01    rp      Add FDI support.
2007-09-21    sch     Fix potential empty union in struct named_inode
2007-08-18    rp      struct fs_vnode cleanup.
2006-11-10    sh      Add chmod support.
2006-09-20    dlb     Lint cleanup.
2006-07-06    dlb     Remove 'size' field from vnode.
2006-06-27    dlb     Pass stat through readdir.
2006-06-20    nrs     Add reference count check prototype
2006-05-09    dlb     Export nodev.
2006-01-11    nrs     Fixed Copyright header
2005-11-10    nrs     Merge fix
2005-11-09    sh      Renamed FS_FEATURE_EXTFS to FEATURE_EFS_EXTFS
2005-10-18    nrs     Fixed prototypes for quotas & reservations
2005-08-30    dlb     Fix mode type on mknod.
2005-08-23    sh      Added mknod
2005-08-09    dlb     Support umounting of busy FS.
2005-08-01    nrs     Added entries for quotas and reservations
2005-07-15    nrs     Added entry for efs_chown ()
2005-03-23    dlb     Warning fix.
2004-10-17    dlb     Add vnode obliteration.
2004-10-15    dlb     Update copyright header.
2004-07-19    dlb     Restructure vnode interface.
2003-10-17     gr     Moved a couple of definitions into fs_util.h.
2003-06-17    jkl     Clean up code.
2003-06-13     gr     Added a function to return the ref count.
2002-08-08    drh     Created by adm.  Added history header.

===========================================================================*/

#ifndef __FS_VNODE_H__
#define __FS_VNODE_H__

#include "fs_config.h"
#include "comdef.h"
#include "fs_namei.h"

#ifdef FEATURE_EFS_QEFS2
#include "fs_efs2_inode.h"
#endif

#ifdef FEATURE_EFS_EXTFS
#include "fs_extfs_inode.h"
#endif

#ifdef FEATURE_EFS_FDI
#include "fs_fdi_inode.h"
#endif

/* These headers are recursive.  Define some structures to close the loops. */
struct fs_vnode;
struct nameidata;

typedef struct fs_dirstream FS_DIR;

struct vfs_ops {
  /* Lookup.  Lookup this entry.  Returns 0 for success. */
  int (*lookup) (struct nameidata *ndp);

  /* Create a new regular file.  The file named in the 'next/length' field
   * of ndp will be created in ndp->dvp directory.  The resultant vnode
   * will be placed in ndp->vp.  No reference counts will be changed, other
   * than the ndp->vp having a refcount of 1, if created. */
  int (*create) (struct nameidata *ndp, fs_mode_t mode);

  /* Create a link.  The vp field should be filled in for the desired
   * destination of the link. */
  int (*link) (struct nameidata *ndp);

  /* Remove a link to a file.  If the link count goes to zero, this file
   * will be removed as well. */
  int (*unlink) (struct nameidata *ndp);

  /* Write data to a file.  Returns number of bytes written, or a negative
   * errno value.  If 'pos' is FS_OFFSET_APPEND, the file will be appended
   * to, rather than written at a specified position. */
  fs_ssize_t (*write) (struct fs_vnode *vp, fs_off_t pos,
      const void *buf, fs_size_t count);

  /* Read data from a file.  Returns number of bytes read, or a negative
   * errno value. */
  fs_ssize_t (*read) (struct fs_vnode *vp, fs_off_t pos,
      void *buf, fs_size_t count);

  /* Truncate a file.  This current version only supports truncating to
   * size 0.  For EFS, this also may end and re-open the transaction if too
   * much activity has taken place for a single transaction. */
  int (*truncate) (struct fs_vnode *vp, fs_off_t pos);

  /* Create a new directory of the given name and mode. */
  int (*mkdir) (struct nameidata *ndp, fs_mode_t mode);

  /* Remove a directory. */
  int (*rmdir) (struct nameidata *ndp);

  /* Directory iteration, applied to a directory vnode.  Works like the
   * posix interface of a similar name.  The void* that it returns can be
   * passed to readdir and closedir. */
  void * (*opendir) (struct fs_vnode *vp);

  /* Read a directory entry.  The name is placed in the dirent->d_name.
   * The name is null terminated.  Must also fill in at least
   * d_stat_present field, and the fields of d_stat that are indicated.
   * Returns 0 for success, or -1 for either error or end of file. */
  int (*readdir) (struct fs_vnode *vp, void *dir,
      struct fs_dirent *dirent);

  /* Close a directory iterator.  Return 0 for success, or -errno for an
   * error. */
  int (*closedir) (struct fs_vnode *vp, void *dir);

  /* Query the stat information.  Fills in the "stat" structure.  Returns 0
   * on success. */
  int (*getstat) (struct fs_vnode *vp, struct fs_stat *buf);

  /* Query the size information. Returns the size of the file in bytes.
   * This function is only defined on some file systems. This is added to
   * optimize the seek performance on FAT based file systems.
   * Returns 0 on success */
  int (*getsize) (struct fs_vnode *vp, uint32 *size);

  /* Rename the given entity. */
  int (*rename) (struct nameidata *ndp, struct nameidata *ndp2);

  /* Create a symlink in the given directory with the given name referring
   * to the desgination.  The dest is null terminated. */
  int (*symlink) (struct nameidata *ndp, const char *dest);

  /* Read the contents of a symlink.  Returns a count of bytes, or a -errno
   * on error. */
  int (*readlink) (struct fs_vnode *vp,
      char *buf,
      fs_size_t bufsiz);

  /* Query the filesystem information.  Fills in the "statvfs" structure.
   * Returns 0 on success. */
  int (*getstatvfs) (struct fs_vnode *vp, struct fs_statvfs *buf);

  /* Write new data for an "item".  Overwrites any existing item. */
  int (*put_item) (struct nameidata *ndp, void *data, int length, int mode);

  /* Read the data for an "item". */
  int (*get_item) (struct fs_vnode *dvp, void *data, int length);

  /* Create a new device special file of the given major/minor number. */
  int (*mknod) (struct nameidata *ndp, fs_mode_t mode, fs_devspecial_t dev);

  /* Change the file mode */
  int (*chmod) (struct fs_vnode *vp, fs_mode_t mode);

  /* Change the group or owner ID */
  int (*chown) (struct fs_vnode *vp, int uid_val, int gid_val);

  /* Set a reservation */
  int (*set_reservation) (struct fs_vnode *vp, uint32 gid, fs_size_t size);

  /* Set a quota */
  int (*set_quota) (struct fs_vnode *vp, uint32 gid, fs_size_t size);

  /* Get reservation and quota information */
  int (*get_group_info) (struct fs_vnode *vp,
      uint32 gid,
      struct fs_group_info *ginfo);

  /* Shred obsolete information from the file systems. */
  int (*shred) (struct fs_vnode *vp);
};

/* All file-systems that do not support inodes and rely on filenames should
  become member of this union. */
struct named_inode {
  /* common members: filepath and its length. */
  unsigned  key_length;
  char      key[NAMED_VNODE_SIZE];

  union {
#ifdef FEATURE_EFS_EXTFS
    struct extfs_inode    extfs;
#endif
#ifdef FEATURE_EFS_FDI
    struct fdi_inode      fdi;
#endif
    int                   dummy; /* Fix potential empty union */
  } p;
};

struct fs_vnode {
  struct fs_mount *mp;
  fs_dev_t dev;
  fs_inode_t inum;

  /* This field is cached copies of values from the inode.  It
   * should always be updated with the value in the inode. */
  fs_mode_t mode;

  int dirty;                    /* Is the inode data here dirty. */

  const struct vfs_ops *vops;

  struct fs_vnode *next;           /* Used for chaining. */
  int ref;                      /* Reference count. */

  /* Private data, specific to the type of vnode this is. */
  union {
  #ifdef FEATURE_EFS_QEFS2
    struct efs2_inode   inode;
  #endif
    struct named_inode  named;
  } p;
};


/* Allocate a new vnode.  Should be filled in before being used.  The
 * reference count will initially be set to 1. */
struct fs_vnode *fs_vnode_alloc (void);

/* Add a reference to the vnode.  Increments the reference count. */
void fs_vnode_ref (struct fs_vnode *vn);

/* Remove a reference to the vnode.  If the reference count drops to 0, add
 * the vnode back to the free pool. */
void fs_vnode_unref (struct fs_vnode *vn);

/* Return the reference count of the vnode. */
int fs_vnode_getref (struct fs_vnode *vn);

/* Initialize the vnode pool. */
void fs_vnode_init (void);

/* Flush a given vnode (if it is dirty, call the mountpoint handler to
 * actually write it out). */
void fs_vnode_flush (struct fs_vnode *vn);

/* Record that a given vnode is no longer valid (usually used after rolling
 * back a transaction. */
void fs_vnode_invalidate (struct fs_vnode *vn);

/**********************************************************************
 * For each entry point in vops, we define an error_returning version here.
 * These can be used for operations that make no sense. */

int fs_vnode_invalid_lookup (struct nameidata *ndp);
int fs_vnode_invalid_create (struct nameidata *ndp, fs_mode_t mode);
int fs_vnode_invalid_link (struct nameidata *ndp);
int fs_vnode_invalid_unlink (struct nameidata *ndp);
fs_ssize_t fs_vnode_invalid_write (struct fs_vnode *vp, fs_off_t pos,
    const void *buf, fs_size_t count);
fs_ssize_t fs_vnode_invalid_read (struct fs_vnode *vp, fs_off_t pos,
    void *buf, fs_size_t count);
int fs_vnode_invalid_truncate (struct fs_vnode *vp, fs_off_t pos);
int fs_vnode_invalid_mkdir (struct nameidata *ndp, fs_mode_t mode);
int fs_vnode_invalid_rmdir (struct nameidata *ndp);
void *fs_vnode_invalid_opendir (struct fs_vnode *vp);
int fs_vnode_invalid_readdir (struct fs_vnode *vp, void *dir,
    struct fs_dirent *dirent);
int fs_vnode_invalid_closedir (struct fs_vnode *vp, void *dir);
int fs_vnode_invalid_getstat (struct fs_vnode *vp, struct fs_stat *buf);
int fs_vnode_invalid_getsize (struct fs_vnode *vp, uint32 *size);
int fs_vnode_invalid_rename (struct nameidata *ndp, struct nameidata *ndp2);
int fs_vnode_invalid_symlink (struct nameidata *ndp, const char *dest);
int fs_vnode_invalid_readlink (struct fs_vnode *vp,
    char *buf, fs_size_t bufsiz);
int fs_vnode_invalid_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf);

int fs_vnode_invalid_put_item (struct nameidata *ndp,
    void *data, int length, int mode);
int fs_vnode_invalid_get_item (struct fs_vnode *dvp, void *data, int length);
int fs_vnode_invalid_mknod (struct nameidata *ndp,
    fs_mode_t mode, fs_devspecial_t dev);
int fs_vnode_invalid_chmod (struct fs_vnode *vp, fs_mode_t mode);
int fs_vnode_invalid_chown (struct fs_vnode *vp, int uid_val, int gid_val);
int fs_vnode_invalid_set_reservation (struct fs_vnode *vp, uint32 gid,
    fs_size_t size);
int fs_vnode_invalid_set_quota (struct fs_vnode *vp, uint32 gid, uint32 size);
int fs_vnode_invalid_get_group_info (struct fs_vnode *vp, uint32 gid,
    struct fs_group_info *ginfo);
int fs_vnode_invalid_shred (struct fs_vnode *vp);

/* Vnode ops table for a nodev filesystem. */
extern const struct vfs_ops fs_vnode_nodev_ops;

/* Lookup a vnode by dev/inode.  NULL is returned if there is no vnode with
 * this information.  Otherwise returns the vnode.  NOTE: This does not
 * increment the reference count.  vnode_ref() must be called if this is to
 * be done. */
struct fs_vnode *fs_vnode_lookup (struct fs_mount *mp, fs_dev_t dev,
    fs_inode_t inode);

/* Lookup a vnode by dev/name.  Otherwise the same as fs_vnode_lookup. */
struct fs_vnode *fs_vnode_named_lookup (struct fs_mount *mp, fs_dev_t dev,
    fs_mode_t mode, const char *key, int key_len);

/* Overwrite the given vnode with a replacement vnode that returns ENODEV
 * on every call.  The vnode isn't flushed, and the dirty bit will be
 * discarded.  This is intended to be called if the underlying filesystem
 * becomes unavailable (such as device removal). */
void fs_vnode_make_nodev (struct fs_vnode *vp);

#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */

#endif /* not __FS_VNODE_H__ */
