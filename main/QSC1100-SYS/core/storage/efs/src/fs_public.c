/***********************************************************************
 * fs_public.c
 *
 * Public interface for EFS2
 * Copyright (C) 2002-2009 Qualcomm, Inc.
 *
 * This file contains POSIX interface functions using which EFS2
 * can be utilized.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_public.c#12 $ $DateTime: 2009/01/30 16:38:40 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2009-01-29    wek     Made hfat_init run on target, added to fs_extfs_init.
2009-01-28    wek     Updated initialization of stdio and rollback.
2008-12-12    rp      Introducing Managed Nand.
2008-12-02    yg      Fail rename if the destination file is open.
2008-11-19    yg      Move cpu_hog task init to benchmark init.
2008-11-06    sh      Removed the last vestiges of ZAP
2008-11-03    rp      Introduce device abstraction.
2008-10-27    yg      Removing remote EFS related code for CMI.
2008-10-17    yog     Featurized pragma directive to exclude for FS_STANDALONE.
2008-10-06    np      Added efs_shred API.
2008-05-23    rp      Changes for new SCM.
2008-03-19    sh      Improved Babylon message logging for several calls.
2008-01-24    sch     Used getsize before getstat in seek and append
2007-12-05    rp      Moved FS_PARANOIA_CHECKS feature to fs_paranoia.c
2007-12-10    rp      Added FDI support to the efs_get_device_info() API.
2007-11-16    sh      Rename TIMETEST2 calls to Babylon calls
2007-11-02    rp      Move fs_fresh_start variable from fs_pm_super.c file.
2007-10-26    rp      Reset paranoia computed crc on efs_initialize().
2007-10-18    rp      Featurized QEFS2.
2007-10-01    rp      Add FDI support.
2007-09-10    sh      Adjust seek position after write with O_APPEND
2007-08-19    rp      FS_GLOBAL_LOCK() cleanup.
2007-06-14    dlb     Fix vnode leak querying group information.
2007-04-23    umr     Added support for EFS memory protection feature.
2007-05-16    rp      The truncate API's will return EINVAL for -ve args.
2007-05-11    umr     Added stack check on simulator build
2007-04-24    sch     Allowed renaming of directories
2006-12-21    s h     Implement FIFO and Socket file nodes.
2006-11-10    s h     Add support for efs_chmod().
2006-11-02     rp     Added efs_stdio_init() in efs_initialize().
2006-10-26    s h     Removed all references to obsolete fs_testapp
2006-10-05    umr     Added call to initialize deltree lock in efs_initialize
2006-09-20    dlb     Lint cleanup.
2006-08-15    dlb     Initialize upgrade system.
2006-07-19    dlb     Move transaction limits down to each FS.
2006-07-06    dlb     Remove 'size' field from vnode.
2006-07-05    dlb     Move read-end check down to VFS code.
2006-06-27    dlb     Pass stat through readdir.
2006-06-23     sh     Removed conditional FEATURE_FS_BENCHMARK.. stubs now.
2006-06-19    dlb     Enable diag in off-target test.
2006-06-19    nrs     Allow renames on different size items
2006-06-07     sh     Featurized hotplug_init()
2006-06-02     sh     Removed FTL layer initialization. (moved to hotplug)
2006-05-23     yg     Added FTL layer initialization.
2006-05-11     sh     Renamed efs_hotplug_... to hotplug_...
2006-05-09    dlb     Add nodevfs.
2006-03-31     sh     Lint cleanup
2006-03-29     sh     Hotplug init is no longer optional.
2006-01-31    dlb     Correct errno of access call.
2006-01-26     sh     Added TIMETEST2 messages for public API
2006-01-26     sh     Added TIMETEST2 message support
2006-01-11    nrs     Fixed Copyright header
2005-12-20    dlb     Initialize descriptor table.
2005-11-28    nrs     Merge 7500 orphans
2005-11-12     sh     Use rex tcb err_num instead of global efs_errno
2005-11-10    nrs     Merge Fix
2005-11-09    sh      Renamed FS_FEATURE_EXTFS to FEATURE_EFS_EXTFS
2005-10-28    nrs     Changed raw_put and raw_get length from int to fs_size_t
2005-10-26     sh     Lint cleanup
2005-10-20    sh      Initialize fs_benchmark task and cpu_hog task
2005-10-05    nrs     Clean and fix quotas and reservations
2005-09-02    dlb     Don't follow last symlink on symlink creation.
2005-09-02    dlb     Add sfat mounting feature on /mmc1.
2005-08-26    sh      Don't ERR_FATAL on efs_chmod, even unimplemented
2005-08-23    sh      Add support for mknod, device files
2005-08-10    dlb     Umount of opened dirs.
2005-08-10    dlb     Nodev root of mpoint properly.
2005-08-09    dlb     Add nodev support, for cleanup on umount.
2005-08-08    sh      Enforce identical item size for put
2005-08-03    dlb     Add umount support.
2005-07-22    nrs     Added implementation for quotas & reservations
2005-07-15    nrs     Added implementation for efs_chown
2005-07-13    dlb     Mountpoint cleanup.
2005-06-06    dlb     Extensions for SFAT.
2005-05-26    sd      Compilation fixes for L4.
2005-05-10     sh     Lock filesystem before starting superblock reset
2005-04-26    dlb     Add 2K page support.
2005-04-13    dlb     Fix testapp build for remote.
2005-04-06    nrs     Removed fs_diag dependencies from Unit Test
2005-03-28    dlb     Move unref of mkdir vp up one level.
2005-02-17    dlb     Remove transaction mention from mkdir.
2005-01-27    dlb     Allow standalone builds.
2004-11-02    dlb     Remove all mention of transactions here.
2004-10-21    nrs     Modified fs_public_desc_write() to not use transactions
2004-10-13    dlb     Fixed flush outside of xact on unlink.
2004-10-12    dlb     Add remote EFS client support.
2004-10-11    nrs     Modified rename to work with files that currently exist
2004-10-11    nrs     Modified unlink to remove transaction calls
2004-10-07    nrs     Changed efs_put, efs_get to efs_raw_put, efs_raw_get
2004-10-07    dlb     Whitespace cleanup.
2004-07-26    dlb     Support extfs.
2004-07-19    dlb     Restructure vnode interface.
2004-02-20    dlb     Disallow renames of symlinks.
2003-11-18     gr     Restored call to FS_GLOBAL_UNLOCK in efs_rename.
2003-10-22    dlb     Fix error code on write of 0 bytes.
2003-09-09    dlb     Factory image works with bitmap and tree.
2003-08-20    dlb     Remove delayed truncate code.
2003-07-26     gr     Modified efs_lseek to return an error if an attempt to
                      seek before the beginning of the file is made.
2003-07-23     gr     Added check in ftruncate to return an error if the input
                      file was opened in read-only mode.
2003-06-17    jkl     Clean up code.
2003-06-13     gr     Modified efs_unlink and efs_rmdir to return an error if
                      the file or directory being deleted is open.
2003-06-12    adm     Add support for factory start.
2003-06-12     cr     Featurize testapp init.
2003-05-30     gr     Fixed a bug in efs_open. Modified efs_rename to return
                      failure if called on a directory.
2003-05-23     cr     Add initialization of on target test application.
                      The init won't happen unless one takes a breakpoint in
                      debugger and changes state on static variables.
2003-05-16     gr     Fixed a bug in efs_rename.
2003-05-02     gr     Added master reset functionality.
2003-04-15    cr      Moved global lock to fs_efs2.h as part of delayed
                      truncation implementation to fix CR#28410.
2003-03-24     gr     Code modifications to ensure that all writes to flash
                      have transaction wrappers.
2003-03-12     gr     Implemented efs_rename.
2003-03-06    bgc     Moved vnode flush to inside the transaction in
                      fs_public_desc_write().
2003-03-04     gr     Renamed the parameters to efs_rename so that the C++
                      reserved word "new" is not used.
2002-08-20    adm     Created.

===========================================================================*/

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "fs_sys_types.h"
#include "fs_err.h"
#include "fs_fcntl.h"
#include "fs_errno.h"
#include "fs_public.h"
#include "fs_desc.h"
#include "fs_vnode.h"
#include "fs_namei.h"
#include "fs_mount.h"
#include "fs_efs2.h"
#include "fs_romfs.h"
#include "fs_nodev.h"
#include "fs_db.h"
#include "fs_babylon.h"
#include "fs_hotplug.h"
#include "fs_benchmark.h"
#include "fs_upgrade.h"
#include "fs_pathbuff.h"
#include "fs_estdio.h"
#include "fs_privatedir.h"
#include "fs_paranoia.h"

#include "jzap.h"

#ifdef FEATURE_EFS_EXTFS
#include "fs_extfs.h"
#endif

#include "fs_protected_memory.h"
#include "fs_global_lock.h"

#ifdef FEATURE_EFS_FDI
#include "fs_fdi_i.h"
#endif

#ifdef FS_UNIT_TEST
#error code not present
#endif


#ifdef FEATURE_EFS_ROOT_IS_FDI
#define FS_ROOT_FS_NAME "FDI"
#endif

#ifndef FS_ROOT_FS_NAME
#define FS_ROOT_FS_NAME "EFS2"
#endif

/* This global variable when set to non-zero value will blow away the entire
 * file-system data contents, refer fs_public.h file for more information. */
int fs_fresh_start = 0;

/* Exclude pragma directive for both FS_STANDALONE and FS_UNIT_TEST. */
#if !defined (FS_UNIT_TEST) && !defined (FS_STANDALONE)
/* return to default placement */
  #pragma arm section rwdata, rodata, zidata
#endif

#ifdef FS_STANDALONE
int efs_errno = 0;
#endif


struct nameidata nameidata_buffer;
struct nameidata nameidata_buf2;

/* Vops. */

static int fs_public_desc_close (struct fs_descriptor *file);
static fs_ssize_t fs_public_desc_write (struct fs_descriptor *file,
    const void *buf, fs_size_t count);
static fs_ssize_t fs_public_desc_read (struct fs_descriptor *file,
    void *buf, fs_size_t count);

static struct desc_ops public_desc_ops = {
  fs_public_desc_close,
  fs_public_desc_write,
  fs_public_desc_read,
};

/**********************************************************************
 * Iterators.
 *
 * All iterators are allocated and deallocated from a common pool.  We make
 * the assumption that there will not be many iterators in use.
 */

struct fs_dir_data {
  int                   busy;
  void                 *iter;
  struct fs_dirent      dirent;
  struct fs_mount      *mp;
  struct fs_vnode      *vp;
};

static struct fs_dir_data all_iters[FS_MAX_ITERATORS];

#ifdef FEATURE_EFS_COMPATIBILITY
extern void fs_compat_init (void);
#endif /* FEATURE_EFS_COMPATIBILITY */

#ifndef FEATURE_IG_EFS_EXT_SERVER
extern void fs_diag_init (void);
#endif

/* Data for exceptions. */
struct efs_catch_block *_fs_catch = NULL;
struct efs_catch_block _fs_static_catch;

/***********************************************************************
FUNCTION      efs_initialize

DESCRIPTION   This function should be called to mount the
              root file system. It calls the start function specified
              in the fs_mount_ops structure to do the actual initialization
              where both the file system and the dbtree are initialized.

DEPENDENCIES

RETURN VALUE  If the mount was successful 0, else the error no.

SIDE EFFECTS
**********************************************************************/
int
efs_initialize ()
{
  int result;
  int i;

  EFS_BAB_START (EFS_BAB_INITIALIZE);

  /* EFS Lock is not initialized yet. Release protected memory */
  fs_efs2_unprotect_memory ();

  /* Simulator tests just call efs_initialize() to simulate resetting the
   * phone, On these occasions we also have to reset the computed paranoia-crc
   * to 0 to avoid the first time failure. */
  fs_paranoia_init ();

  FS_GLOBAL_LOCK_INIT();
  fs_global_lock ();
  FS_PATH_BUFF_LOCK_INIT();

  _fs_catch = NULL;

#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */

  fs_upgrade_init ();

  fs_desc_init ();
  fs_vnode_init ();

  fs_mount_init ();

#ifdef FEATURE_EFS_QEFS2
  fs_efs2_init ();
#endif

#ifdef FEATURE_EFS_FDI
  fs_fdi_init ();
#endif

#ifdef FEATURE_EFS_QEFS2
  fs_romfs_init ();
#endif

  fs_nodevfs_init ();
#ifdef FEATURE_EFS_EXTFS
  fs_extfs_init ();
#endif

  /* Mount the root filesystem. */
  result = fs_mount_root (FS_ROOT_FS_NAME, "");

  if (result != 0)
    FS_ERR_FATAL ("No flash device found", 0, 0, 0);

  /* All of the iterators are available. */
  for (i = 0; i < FS_MAX_ITERATORS; i++)
    all_iters[i].busy = 0;


#ifdef FEATURE_EFS_COMPATIBILITY
  fs_compat_init ();
#endif /* FEATURE_EFS_COMPATIBILITY */

#ifndef FEATURE_IG_EFS_EXT_SERVER
  fs_diag_init ();
#endif

  fs_benchmark_init();

#ifdef FEATURE_L4
#error code not present
#endif /* FEATURE_L4 */

#if defined FEATURE_EFS_HOTPLUG || defined FEATURE_EFS_COLDPLUG
  hotplug_init ();
#endif

  /* create the efs's private meta-data folder */
  efs_privatedir_init ();

  /* Rollback all files that were not closed in previous boot session */
  efs_rollback_recover ();

  /* Initialize stdio related functions */
  efs_estdio_init ();

  fs_global_unlock ();

  EFS_BAB_END (EFS_BAB_INITIALIZE);
  return result;
}


/***********************************************************************
FUNCTION      efs_power_down

DESCRIPTION   This function calls the file system finalize function to
              make sure the shut down is handled correctly and normally.

DEPENDENCIES  None

RETURN VALUE  If the shut down was successful 0.

SIDE EFFECTS  None
**********************************************************************/
int
efs_power_down ()
{
  EFS_BAB_API (EFS_BAB_POWER_DOWN);
  /* XXX: Should unmount everything. */
  return fs_root_mpoint->ops->stop (fs_root_mpoint);
}

/* Open/create a file. */
int
efs_open (const char *path, int oflag, ...)
{
  fs_mode_t             mode = 0;
  struct nameidata *ndp = &nameidata_buffer;
  int result = 0;
  struct fs_descriptor *file = 0;

  /* Posix requires the optional third argument for the mode.  Extract this
   * if we are creating a file.
   * Lint is so very very confused by va_arg.  Suppress everything it finds. */
/*lint -save -e746 -e10 -e64 -e718 -e529 */
  if ((oflag & O_CREAT) != 0)
  {
    va_list arg;
    va_start (arg, oflag);
    mode = va_arg (arg, int);
    va_end (arg);
  }
/*lint -restore */

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_OPEN);

  EFS_BAB_PRINT (EFS_BAB_OPEN, path);
  EFS_BAB_VALUE (EFS_BAB_OPEN, (uint32) oflag, FALSE, FALSE, TRUE);

  /* Allocate a descriptor. */
  file = fs_desc_alloc ();
  if (file == NULL) {
    efs_errno = EMFILE;
    goto error_case;
  }

  result = fs_namei (ndp, path, oflag);

  if (result != 0) {
    goto error_case;
  }

  /* Don't allow open on device special files */
  if (ndp->vp && (S_ISSPECIAL(ndp->vp->mode))) {
    efs_errno = EEXIST;
    fs_vnode_unref (ndp->dvp);
    fs_vnode_unref (ndp->vp);
    goto error_case;
  }

  /* If the name lookup succeeds, return an error if the user is attempting
   * to open a directory with O_WRONLY or O_RDWR specified.
   */
  if ((ndp->vp != NULL) && S_ISDIR (ndp->vp->mode) &&
      ((oflag & O_RDWR) || (oflag & O_WRONLY))) {
    efs_errno = EISDIR;
    fs_vnode_unref (ndp->dvp);
    fs_vnode_unref (ndp->vp);
    goto error_case;
  }

  /* fs_namei already checked the open flags for O_CREAT and O_EXCL, so, if
   * we get this far, we can either create, or open the file. */

  if (ndp->vp == NULL) {
    result = ndp->dvp->vops->create (ndp, mode);
    if (result < 0) {
      fs_vnode_unref (ndp->dvp);
      goto error_case;
    }
  }

  /* Now, ndp->vp points to our file.  We don't need the directory any
   * more, so unreference that. */
  fs_vnode_unref (ndp->dvp);
  ndp->dvp = 0;

  /* If the file should be truncated (and was opened for writing). */
  if ((oflag & O_TRUNC) &&
      (((oflag & O_ACCMODE) + 1 ) & 2) != 0)
  {
    result = ndp->vp->vops->truncate (ndp->vp, 0);
    /* XXX: What do we do if there was a problem truncating the file? */
  }

  /* XXX: Handle different operation types. */
  /* Fill in the descriptor */
  file->state = FS_DESC_STATE_FILE;

  /* Move the referenced vnode into the descriptor. */
  file->vp = ndp->vp;
  ndp->vp = 0;

  file->mode = oflag;
  file->file_pos = 0;

  file->dops = &public_desc_ops;

  EFS_BAB_RETURN (EFS_BAB_OPEN, file->fd);
  EFS_BAB_END (EFS_BAB_OPEN);
  fs_global_unlock ();

  return file->fd;

error_case:
  if (file != NULL)
    fs_desc_free (file);
  if (result < 0)
    efs_errno = -result;
  EFS_BAB_RETURN (EFS_BAB_OPEN, -result);
  EFS_BAB_END (EFS_BAB_OPEN);
  fs_global_unlock ();
  return -1;
}

static fs_ssize_t
_efs_read_core (int filedes, void *buf, fs_size_t nbyte)
{
  struct fs_descriptor *desc_ptr;
  int result;

  if ((desc_ptr = fs_desc_lookup (filedes)) == NULL)
  {
    efs_errno = EBADF;
    return -1;
  }

  result = desc_ptr->dops->read (desc_ptr, buf, nbyte);

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return result;
}

fs_ssize_t
efs_read (int filedes, void *buf, fs_size_t nbyte)
{
  fs_ssize_t result;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_READ);
  EFS_BAB_VALUE (EFS_BAB_READ, (uint16)filedes, TRUE, TRUE, FALSE);
  EFS_BAB_VALUE (EFS_BAB_READ, (uint32)nbyte, TRUE, TRUE, TRUE);
  result = _efs_read_core (filedes, buf, nbyte);
  EFS_BAB_RETURN (EFS_BAB_READ, result);
  EFS_BAB_END (EFS_BAB_READ);
  fs_global_unlock ();

  return result;
}

static fs_ssize_t
fs_public_desc_read (struct fs_descriptor *file,
    void *buf, fs_size_t count)
{
  fs_ssize_t result;

  /* Check permissions, use the same trick described in
   * fs_public_desc_write. */
  if ((((file->mode & O_ACCMODE) + 1) & 1) == 0) {
    return -EBADF;
  }

  /* Do the actual read. */
  if (count > 0)
    result = file->vp->vops->read (file->vp, file->file_pos, buf, count);
  else
    result = 0;

  /* Adjust the file pointer. */
  if (result > 0) {
    file->file_pos += result;
  }

  return result;
}

static fs_ssize_t
_efs_write_core (int filedes, const void *buf, fs_size_t nbyte)
{
  struct fs_descriptor *desc_ptr;
  int result;

  /* Just return if asked to write zero bytes.
   */
  if (nbyte == 0)
    return 0;

  if ((desc_ptr = fs_desc_lookup (filedes)) == NULL)
  {
    efs_errno = EBADF;
    return -1;
  }

  result = desc_ptr->dops->write (desc_ptr, buf, nbyte);

  /* It is not an error to write fewer bytes than requested. However, we
   * consider it an error if not even one byte could be written.
   */
  if (result <= 0) {
    if (result == 0)
      result = -ENOSPC;
    efs_errno = -result;
    return -1;
  } else
    return result;
}

fs_ssize_t
efs_write (int filedes, const void *buf, fs_size_t nbyte)
{
  fs_ssize_t result;

  fs_global_lock ();

  EFS_BAB_START (EFS_BAB_WRITE);
  EFS_BAB_VALUE (EFS_BAB_WRITE, (uint16)filedes, TRUE, TRUE, FALSE);
  EFS_BAB_VALUE (EFS_BAB_WRITE, (uint32)nbyte, TRUE, TRUE, TRUE);
  result = _efs_write_core (filedes, buf, nbyte);
  EFS_BAB_RETURN (EFS_BAB_WRITE, result);
  EFS_BAB_END   (EFS_BAB_WRITE);

  fs_global_unlock ();
  return result;
}

static fs_ssize_t
fs_public_desc_write (struct fs_descriptor *file,
    const void *buf, fs_size_t count)
{
  fs_ssize_t result;
  fs_off_t write_pos;

  /* Check that we have permissions to write this file.  This depends on
   * the specific values of O_RDONLY=0, O_WRONLY=1, and O_RDWR=2 that posix
   * use.  By adding 1, the value is just a pair of bits with the lowest
   * bit indicating read, the the next bit indicating write.  Neat, isn't
   * it. */

  if ((((file->mode & O_ACCMODE) + 1) & 2) == 0) {
    return -EBADF;
  }

  /* In append mode, all writes always seek to the end of the file.
   * In normal writes, use the current position.  */
  write_pos = (file->mode & O_APPEND) ? FS_OFFSET_APPEND : file->file_pos;

  /* Do the actual write. */
  if (count > 0) {
    result = file->vp->vops->write (file->vp, write_pos, buf, count);

    /* Adjust the file pointer appropriately. */
    if (result > 0) {
      if (file->mode & O_APPEND) {
        struct fs_stat sbuf;
        uint32 numbytes;
        if (file->vp->vops->getsize (file->vp, &numbytes) == 0)
          file->file_pos = numbytes;
        else if (file->vp->vops->getstat (file->vp, &sbuf) == 0)
          file->file_pos = sbuf.st_size;
      } else {
        file->file_pos += result;
      }
    }
  }
  else
    result = 0;

  return result;
}

/* efs_mkdir */
int
efs_mkdir (const char *path, fs_mode_t mode)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_MKDIR);
  EFS_BAB_PRINT (EFS_BAB_MKDIR, path);

  result = fs_namei (ndp, path, O_EXCL | O_CREAT);

  if (result < 0)
  {
    goto clean_up;
  }

  result = ndp->dvp->vops->mkdir (ndp, mode);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

clean_up:
  if (result != 0)
    EFS_BAB_RETURN (EFS_BAB_MKDIR, -result);
  EFS_BAB_END (EFS_BAB_MKDIR);
  fs_global_unlock ();
  if (result == 0)
    return 0;
  else {
    efs_errno = -result;
    return -1;
  }
}

int
efs_creat (const char *path, fs_mode_t mode)
{
  EFS_BAB_API (EFS_BAB_CREAT);
  return efs_open (path, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

int
efs_close (int filedes)
{
  struct fs_descriptor *desc_ptr;
  int result = 0;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_CLOSE);
  EFS_BAB_VALUE (EFS_BAB_CLOSE, (uint16)filedes, TRUE, TRUE, FALSE);

  /* Check if file descriptor is valid */
  if ((desc_ptr = fs_desc_lookup (filedes)) == NULL)
  {
    efs_errno = EBADF;
    result = -1;
  } else {
    result = desc_ptr->dops->close (desc_ptr);
    fs_desc_free (desc_ptr);
  }

  EFS_BAB_END (EFS_BAB_CLOSE);
  fs_global_unlock ();

  return result;
}

/* Close handler for filesystem (through vnode) operations. */
static int
fs_public_desc_close (struct fs_descriptor *file)
{
  /* Dereference the vnode.  There should be no need to flush. */
  file->state = 0;
  fs_vnode_unref (file->vp);
  file->vp = 0;

  return 0;
}

fs_off_t
efs_lseek (int filedes, fs_off_t offset, int whence)
{
  struct fs_descriptor *desc_ptr;
  fs_off_t              result = 0;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_LSEEK);
  EFS_BAB_VALUE (EFS_BAB_LSEEK, (uint16)filedes, TRUE, TRUE, FALSE);
  EFS_BAB_VALUE (EFS_BAB_LSEEK, (uint32)offset,  TRUE, TRUE, TRUE);
  EFS_BAB_VALUE (EFS_BAB_LSEEK, (uint16)whence,  TRUE, TRUE, FALSE);

  /* Check if file descriptor is valid */
  if ((desc_ptr = fs_desc_lookup (filedes)) == NULL)
  {
    efs_errno = EBADF;
    result = -1;
    goto error_case;
  }

  /* This is only valid with files. */
  if (desc_ptr->state != FS_DESC_STATE_FILE) {
    efs_errno = ESPIPE;
    result = -1;
    goto error_case;
  }

  switch (whence) {
    case SEEK_SET:
      result = 0;
      break;

    case SEEK_CUR:
      result = desc_ptr->file_pos;
      break;

    case SEEK_END:
      /* The size of the file is first obtained using getsize function. It
       * might fail either if 'getsize' is not defined for the file system
       * or if the call cannot be performed on this vp.
       * In that case, we try getstat() to obtain the size */
      {
        uint32 num_bytes;
        result = desc_ptr->vp->vops->getsize (desc_ptr->vp, &num_bytes);

        if (result == 0)
          result = num_bytes;
        else
        {
          static struct  fs_stat sbuf;
          result = desc_ptr->vp->vops->getstat (desc_ptr->vp, &sbuf);

          if (result == 0)
            result = (fs_off_t) sbuf.st_size;
          else {
            efs_errno = -result;
            result = -1;
            goto error_case;
          }
        }
        break;
      }

    default:
      efs_errno = EINVAL;
      result = -1;
      goto error_case;
  }

  if (result + offset >= 0) {
    desc_ptr->file_pos = result + offset;
    result = desc_ptr->file_pos;
  } else {
    efs_errno = EINVAL;
    result = -1;
  }

error_case:
  EFS_BAB_RETURN (EFS_BAB_LSEEK, result);
  EFS_BAB_END (EFS_BAB_LSEEK);
  fs_global_unlock ();

  return result;
}

fs_off_t
efs_truncate (const char *path, fs_off_t length)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  if (length < 0) {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_TRUNCATE);

  result = fs_namei (ndp, path, 0);

  if (result == 0)
    result = ndp->vp->vops->truncate (ndp->vp, length);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (EFS_BAB_TRUNCATE);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;

}

fs_off_t
efs_ftruncate (int fd, fs_off_t length)
{
  struct fs_descriptor *desc;
  int result;

  if (length < 0) {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_FTRUNCATE);

  desc = fs_desc_lookup (fd);
  if (desc == NULL) {
    result = -EBADF;
    goto clean_up;
  }

  /* Check that we have permissions to write this file.  Use the trick
   * described in fs_public_desc_write.
   */
  if ((((desc->mode & O_ACCMODE) + 1) & 2) == 0) {
    result = -EINVAL;
    goto clean_up;
  }

  result = desc->vp->vops->truncate (desc->vp, length);

clean_up:
  EFS_BAB_END (EFS_BAB_FTRUNCATE);
  fs_global_unlock ();
  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return result;
}

int
efs_stat (const char *path, struct fs_stat *buf)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;
  struct fs_vnode *vp;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_STAT);
  EFS_BAB_PRINT (EFS_BAB_STAT, path);

  result = fs_namei (ndp, path, 0);

  if (result == 0) {
    vp = ndp->vp;
    result = vp->vops->getstat (vp, buf);
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN(EFS_BAB_STAT, -result);

  EFS_BAB_END (EFS_BAB_STAT);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
}

int
efs_fstat_core (int fd, struct fs_stat *buf)
{
  struct fs_descriptor *desc_ptr;
  struct fs_vnode      *vp;
  int result;

  /* Check if file descriptor is valid */
  if ((desc_ptr = fs_desc_lookup (fd)) == NULL)
  {
    efs_errno = EBADF;
    return -1;
  }

  /* This is only valid with files. */
  if (desc_ptr->state != FS_DESC_STATE_FILE) {
    efs_errno = ESPIPE;
    return -1;
  }

  vp = desc_ptr->vp;

  result = vp->vops->getstat (vp, buf);

  if (result == 0)
    return 0;
  else {
    efs_errno = -result;
    return -1;
  }
}

int
efs_fstat (int fd, struct fs_stat *buf)
{
  int result;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_FSTAT);
  EFS_BAB_VALUE (EFS_BAB_FSTAT, (uint16)fd, TRUE, TRUE, FALSE);

  result = efs_fstat_core (fd, buf);

  if (result != 0)
    EFS_BAB_RETURN (EFS_BAB_FSTAT, - efs_errno);

  EFS_BAB_END (EFS_BAB_FSTAT);
  fs_global_unlock ();
  return result;
}



int
efs_lstat (const char *path, struct fs_stat *buf)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;
  struct fs_vnode *vp;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_LSTAT);
  EFS_BAB_PRINT (EFS_BAB_LSTAT, path);

  result = fs_namei (ndp, path, O_NOFOLLOW);

  if (result != 0)
    goto error_case;

  vp = ndp->vp;

  result = vp->vops->getstat (vp, buf);

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN (EFS_BAB_LSTAT, -result);
  EFS_BAB_END (EFS_BAB_LSTAT);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
} /* efs_lstat */

int
efs_statvfs (const char *path, struct fs_statvfs *buf)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;
  struct fs_vnode *vp;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_STATVFS);
  EFS_BAB_PRINT (EFS_BAB_STATVFS, path);

  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto error_case;

  vp = ndp->vp;

  result = vp->vops->getstatvfs (vp, buf);

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN(EFS_BAB_STATVFS, -result);

  EFS_BAB_END (EFS_BAB_STATVFS);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }

  return 0;
} /* END efs_statvfs */

int
efs_fstatvfs(int fd, struct fs_statvfs *buf)
{
  struct fs_descriptor * desc_ptr;
  struct fs_vnode * vp;
  int result;

  fs_global_lock();
  EFS_BAB_START (EFS_BAB_FSTATVFS);
  /* Check if file descriptor is valid */
  if ((desc_ptr = fs_desc_lookup (fd)) == NULL)
  {
    result = -EBADF;
    goto error_case;
  }

  /* This is only valid with files. */
  if (desc_ptr->state != FS_DESC_STATE_FILE) {
    result = -ESPIPE;
    goto error_case;
  }

  vp = desc_ptr->vp;

  result = vp->vops->getstatvfs (vp, buf);

error_case:
  EFS_BAB_END (EFS_BAB_FSTATVFS);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  }
  return 0;
}

int
efs_unlink (const char *path)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_UNLINK);
  EFS_BAB_PRINT (EFS_BAB_UNLINK, path);

  result = fs_namei (ndp, path, O_NOFOLLOW);

  if (result != 0) {
    goto error_case;
  }

  if (!S_ISREG (ndp->vp->mode) &&
      !S_ISLNK (ndp->vp->mode) &&
      !S_ISSPECIAL (ndp->vp->mode) &&
      !S_ISITM (ndp->vp->mode)) {
    /* Posix actually says to return EPERM for this case.  EISDIR seems to
     * make more sense, but we'll comply with Posix. */
    result = -EPERM;
    fs_vnode_unref (ndp->vp);
    goto error_case;
  }

  if (S_ISREG (ndp->vp->mode)) {
    /* If the file is already open, return an error.
     * XXX: This is not ideal behavior. At some point, we should handle
     * unlinking open files.
     */
    if (fs_vnode_getref (ndp->vp) > 1) {
      result = -ETXTBSY;
      fs_vnode_unref (ndp->vp);
      goto error_case;
    }
  }

  result = ndp->dvp->vops->unlink (ndp);

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  /* The entry vnode was unreferenced by the unlink operation itself. */

  if (result < 0)
    EFS_BAB_RETURN(EFS_BAB_UNLINK, -result);
  EFS_BAB_END (EFS_BAB_UNLINK);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;

} /* END efs_unlink */

static EFSDIR *
efs_opendir_core (const char *dirname)
{
  int i;
  EFSDIR *iter;
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  for (i = 0; i < FS_MAX_ITERATORS; i++) {
    if (!all_iters[i].busy)
      break;
  }
  if (i == FS_MAX_ITERATORS) {
    efs_errno = EMFILE;
    return NULL;
  }
  iter = &all_iters[i];

  result = fs_namei (ndp, dirname, 0);

  if (result != 0) {
    iter->iter = NULL;
    goto error_case;
  }

  iter->iter = ndp->vp->vops->opendir (ndp->vp);
  if (iter->iter == NULL) {
    fs_vnode_unref (ndp->vp);
    result = -ENOTDIR;
  } else {
    iter->busy = 1;

    /* Copy the reference in, so don't unref it until close. */
    iter->vp = ndp->vp;
    iter->mp = ndp->vp->mp;
  }

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);

  if (result < 0) {
    efs_errno = -result;
    return NULL;
  } else
    return iter;
}

EFSDIR *
efs_opendir (const char *dirname)
{
  EFSDIR *d;
  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_OPENDIR);
  EFS_BAB_PRINT (EFS_BAB_OPENDIR, dirname);
  d = efs_opendir_core (dirname);
  if (d == NULL)
    EFS_BAB_RETURN(EFS_BAB_OPENDIR, - efs_errno); /* neg indicates error */
  EFS_BAB_END (EFS_BAB_OPENDIR);
  fs_global_unlock ();
  return d;
}

struct fs_dirent *
efs_readdir_core (EFSDIR *dirp)
{
  int result;

  if (dirp == NULL || !dirp->busy) {
    efs_errno = EINVAL;
    return 0;
  }

  result = dirp->vp->vops->readdir (dirp->vp, dirp->iter,
                                    &dirp->dirent);

  if (result == -EEOF) {
    dirp->dirent.d_name[0] = '\0';
    return 0;
  }
  else if (result < 0) {
    efs_errno = -result;
    return 0;
  } else {
    return &dirp->dirent;
  }
}

struct fs_dirent *
efs_readdir (EFSDIR *dirp)
{
  struct fs_dirent *d;
  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_READDIR);
  d = efs_readdir_core (dirp);
  if (d)
    EFS_BAB_PRINT (EFS_BAB_READDIR, d->d_name);
  EFS_BAB_END (EFS_BAB_READDIR);
  fs_global_unlock ();
  return d;
}


static int
efs_closedir_core (EFSDIR *dirp)
{
  int result;

  if (dirp == NULL || !dirp->busy) {
    efs_errno = EINVAL;
    return -1;
  }

  result = dirp->vp->vops->closedir (dirp->vp, dirp->iter);

  dirp->busy = 0;
  fs_vnode_unref (dirp->vp);

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
}

int
efs_closedir (EFSDIR *dirp)
{
  int result;
  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_CLOSEDIR);
  result = efs_closedir_core (dirp);
  if (result < 0)
    EFS_BAB_RETURN (EFS_BAB_CLOSEDIR, - efs_errno);
  EFS_BAB_END (EFS_BAB_CLOSEDIR);
  fs_global_unlock ();
  return result;
}

int
efs_rename (const char *oldpath, const char *newpath)
{
  struct nameidata *ndp1 = &nameidata_buffer;
  struct nameidata *ndp2 = &nameidata_buf2;
  int result;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_RENAME);
  EFS_BAB_PRINT (EFS_BAB_RENAME, oldpath);
  EFS_BAB_PRINT (EFS_BAB_RENAME, newpath);

  ndp1->dvp = ndp1->vp = NULL;
  ndp2->dvp = ndp2->vp = NULL;

  result = fs_namei (ndp1, oldpath, O_NOFOLLOW);

  if (result == 0) {
    result = fs_namei (ndp2, newpath, O_CREAT);
  }

  /* Posix requires rename to the same file name to succeed. So
   * detect that condition since in that case the reference count
   * would show > 1 since the first namei already incremented the
   * reference for that. */
  if (((ndp2->vp != 0) && S_ISREG (ndp2->vp->mode)) &&
      (result == 0) && (ndp1->vp != ndp2->vp)) {
    /* If the destination file is open, return an error.  */
    if (fs_vnode_getref (ndp2->vp) > 1)
      result = -ETXTBSY;
  }

  if (result == 0) {
    result = ndp1->dvp->vops->rename (ndp1, ndp2);
  }

  /* rename call unrefs on error. */
  if (ndp1->dvp != NULL) {
    fs_vnode_unref (ndp1->dvp);
  }
  if (ndp1->vp != NULL) {
    fs_vnode_unref (ndp1->vp);
  }
  if (ndp2->dvp != NULL) {
    fs_vnode_unref (ndp2->dvp);
  }
  if (ndp2->vp != NULL) {
    fs_vnode_unref (ndp2->vp);
  }

  if (result != 0) {
    EFS_BAB_RETURN (EFS_BAB_RENAME, -result);
    efs_errno = -result;
    result = -1;
  }

  EFS_BAB_END (EFS_BAB_RENAME);
  fs_global_unlock ();

  return result;
}

int
efs_rmdir (const char *path)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_RMDIR);
  EFS_BAB_PRINT (EFS_BAB_RMDIR, path);

  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto error_case;

  /* If they try to delete '.', or '..', or trailing slash. */
  if (ndp->vp == ndp->dvp ||
      (ndp->length == 2 && ndp->next[0] == '.' && ndp->next[1] == '.'))
  {
    result = -EINVAL;
    goto error_case;
  }

  result = ndp->dvp->vops->rmdir (ndp);

  if (result != 0)
    fs_vnode_unref (ndp->vp);
  ndp->vp = NULL;

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (EFS_BAB_RMDIR);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;

} /* END efs_rmdir */

int
efs_chmod (const char *path, fs_mode_t mode)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_CHMOD);

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  /* If file exists, change the mode on this vp */
  if (result == 0)
    result = ndp->vp->vops->chmod (ndp->vp, mode);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);
  EFS_BAB_END (EFS_BAB_CHMOD);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;
} /* END efs_chmod */

/**********************************************************************
 * Mountpoints.
 */
int
efs_mount (const char *special, const char *dir,
    const char *fstype, int flags, const void *data)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  (void) flags;
  (void) special;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_MOUNT);
  EFS_BAB_PRINT (EFS_BAB_MOUNT, dir);

  result = fs_namei (ndp, dir, 0);

  if (result == 0 && !S_ISDIR (ndp->vp->mode)) {
    result = -ENOTDIR;
  }

  if (result == 0) {
    result = fs_mount_mount (ndp->vp, fstype, data);
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (EFS_BAB_MOUNT);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
}

int
efs_umount (const char *target)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;
  struct fs_mount *mp = NULL;
  int i;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_UMOUNT);
  EFS_BAB_PRINT (EFS_BAB_UMOUNT, target);

  result = fs_namei (ndp, target, 0);

  if (result == 0) {
    result = fs_mount_umount (ndp->vp);
    mp = ndp->vp->mp;
  }

  if (result == 0 && mp != NULL) {
    fs_desc_make_nodev (mp);

    /* printf ("umount ref = %d\n", ndp->vp->ref); */
    fs_vnode_make_nodev (ndp->vp);

    /* Cleanup any iterators. */
    for (i = 0; i < FS_MAX_ITERATORS; i++) {
      if (all_iters[i].busy &&
          all_iters[i].vp != NULL &&
          all_iters[i].mp == mp)
      {
        /* If this vnode hasn't been obliterated already, clean it up. */
        if (all_iters[i].vp->mp == mp)
          fs_vnode_make_nodev (all_iters[i].vp);
      }
    }
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

#ifdef FS_UNIT_TEST
#error code not present
#endif

  EFS_BAB_END (EFS_BAB_UMOUNT);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
}

int
efs_remount (const char *oldtarget, const char *newtarget)
{
  int result;
  struct nameidata *ndp1 = &nameidata_buffer;
  struct nameidata *ndp2 = &nameidata_buf2;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_REMOUNT);
  EFS_BAB_PRINT (EFS_BAB_REMOUNT, oldtarget);
  EFS_BAB_PRINT (EFS_BAB_REMOUNT, newtarget);
  ndp1->dvp = ndp1->vp = NULL;
  ndp2->dvp = ndp2->vp = NULL;

  result = fs_namei (ndp1, oldtarget, 0);

  if (result == 0 && !S_ISDIR (ndp1->vp->mode)) {
    result = -EINVAL;
  }

  if (result == 0) {
    result = fs_namei (ndp2, newtarget, 0);
  }

  if (result == 0 && !S_ISDIR (ndp2->vp->mode)) {
    result = -ENOTDIR;
  }

  if (result == 0) {
    result = fs_mount_remount (ndp1->vp, ndp2->vp);
  }

  if (ndp1->dvp != NULL) {
    fs_vnode_unref (ndp1->dvp);
  }
  if (ndp1->vp != NULL) {
    fs_vnode_unref (ndp1->vp);
  }
  if (ndp2->dvp != NULL) {
    fs_vnode_unref (ndp2->dvp);
  }
  if (ndp2->vp != NULL) {
    fs_vnode_unref (ndp2->vp);
  }

  if (result != 0) {
    efs_errno = -result;
    result = -1;
  }

  EFS_BAB_END (EFS_BAB_REMOUNT);
  fs_global_unlock ();

  return result;
}

int
efs_symlink (const char *oldpath, const char *newpath)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_SYMLINK);
  EFS_BAB_PRINT (EFS_BAB_SYMLINK, oldpath);
  EFS_BAB_PRINT (EFS_BAB_SYMLINK, newpath);

  result = fs_namei (ndp, newpath, O_EXCL | O_CREAT | O_NOFOLLOW);
  if (result < 0) {
    goto error_case;
  }

  result = ndp->dvp->vops->symlink (ndp, oldpath);

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);

  EFS_BAB_END (EFS_BAB_SYMLINK);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
}

int
efs_readlink (const char *path, char *buf, fs_size_t bufsiz)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_READLINK);
  EFS_BAB_PRINT (EFS_BAB_READLINK, path);

  result = fs_namei (ndp, path, O_NOFOLLOW);
  if (result == 0) {
    result = ndp->vp->vops->readlink (ndp->vp, buf, bufsiz);
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (EFS_BAB_READLINK);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return result;
}


int
efs_access (const char *path, int amode)
{
  (void) path;
  (void) amode;

  /* efs_access is not implemented in EFS2.  At this point, there is no
   * intent on doing so.  It is part of a security model not well suited to
   * mobile devices. */
  EFS_BAB_API (EFS_BAB_ACCESS);
  efs_errno = EINVAL;
  return -1;
}

int
efs_mknod (const char *path, int mode, fs_devspecial_t dev)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result = 0;

  fs_global_lock();
  EFS_BAB_START (EFS_BAB_MKNOD);
  EFS_BAB_PRINT (EFS_BAB_MKNOD, path);

  result = fs_namei (ndp, path, O_EXCL | O_CREAT);

  /* If the name lookup succeeds, the file already exists */
  if (ndp->vp != NULL)
    result = -EEXIST;

  /* Just attempt to create the device file (XXX ndp->dvp safe???)*/
  if (result == 0)
    result = ndp->dvp->vops->mknod (ndp, mode, dev);

  /* Done.  */
  if (ndp->dvp)
    fs_vnode_unref (ndp->dvp);

  if (ndp->vp)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (EFS_BAB_MKNOD);
  fs_global_unlock ();

  if (result != 0) {
    efs_errno = -result;
    result = -1;
  }

  return result;
}

int
efs_mkfifo (const char *path, int mode)
{
  return efs_mknod (path, (mode & ~S_IFMT) | S_IFIFO, 0);
}


/**********************************************************************
 * FUNCTION      efs_raw_put
 *
 * Store a value in a special item file.
 */
int
efs_raw_put (const char *path, void *data, fs_size_t length,
    int oflag, int mode)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result = 0;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_RAW_PUT);
  EFS_BAB_PRINT (EFS_BAB_RAW_PUT, path);

  if (result == 0)
    result = fs_namei (ndp, path, oflag);

  if (result == 0 && ndp->vp) {
    /* Verify that if we did recover a 'node' that it is really an item
     * type. */
    if (!S_ISITM (ndp->vp->mode))
      result = -EEXIST;
  }

  if (result == 0) {
    result = ndp->dvp->vops->put_item (ndp, data, length, mode);
  }

  if (ndp->dvp) {
    fs_vnode_unref (ndp->dvp);
  }
  if (ndp->vp)
    fs_vnode_unref (ndp->vp);

  if (result != 0) {
    efs_errno = -result;
    result = -1;
  }

  EFS_BAB_END (EFS_BAB_RAW_PUT);
  fs_global_unlock ();

  return result;
}

/* Read contents of item. */
int
efs_raw_get (const char *path, void *data, fs_size_t length)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;
  struct fs_vnode *vp;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_RAW_GET);
  EFS_BAB_PRINT (EFS_BAB_RAW_GET, path);

  result = fs_namei (ndp, path, 0);

  if (result == 0) {
    vp = ndp->vp;
    result = vp->vops->get_item (vp, data, length);
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (EFS_BAB_RAW_GET);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return result;
}

/***********************************************************************
 * FUNCTION      efs_wait_for_xact
 *
 * DESCRIPTION   This function waits for a transaction to finish.
 *
 ************************************************************************/
void
efs_wait_for_xact (void)
{
  EFS_BAB_API (EFS_BAB_WAIT_FOR_XACT);
  fs_global_lock();
} /* efs_wait_for_xact */

/**********************************************************************
 * FUNCTION      efs_reset
 *
 * DESCRIPTION
 *             Resets the filesystem and stops the system. The mobile must be
 *             rebooted before it can be used again.
 ************************************************************************/
void
efs_reset (void)
{
  /* Invalidate all superblocks and halt the system. This will force a
   * fresh start on the next reboot.
   *
   * Make sure we prevent all filesystem operations until we reboot, so we
   * don't trip on the inconsistent state while the erase is running.
   */
  fs_global_lock();
  EFS_BAB_API (EFS_BAB_RESET);

  efs_reset_nostop ();

  FS_ERR_FATAL ("filesystem reset. reboot needed", 0, 0, 0);
} /* efs_reset */

/***********************************************************************
 * FUNCTION    efs_reset_nostop (void)
 *
 * DESCRIPTION
 *   Same as efs_reset, except that it returns.  Should only be used from
 *   within the FS_ERR_FATAL handler.
 */
void
efs_reset_nostop (void)
{
  EFS_BAB_API (EFS_BAB_RESET_NOSTOP);

#ifdef FEATURE_EFS_QEFS2
  #if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
      defined (FEATURE_EFS_EFS2_ON_NOR)
  {
    fs_pm_super_invalidate_superblocks ();
  }
  #elif defined (FEATURE_EFS_EFS2_ON_MNAND)
  {
    extern void fs_mnand_pm_erase_efs (void);
    fs_mnand_pm_erase_efs ();
  }
  #else
  {
    #error "Provide reset function"
  }
  #endif
#endif
}

/***********************************************************************
FUNCTION      efs_get_fs_data

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  If successful, returns 0.
              Otherwise, returns -1, and sets errno to indicate the error.

SIDE EFFECTS  None
**********************************************************************/
#ifdef FEATURE_EFS_QEFS2
extern int fs_get_fs_data(struct fs_factimage_read_info * read_info_ptr,
                          byte * page_buffer);
#endif

int
efs_get_fs_data (struct fs_factimage_read_info * image_info_ptr,
                 byte* page_buffer)
{
  int result;

  EFS_BAB_API (EFS_BAB_GET_FS_DATA);

  if (image_info_ptr == NULL) {
    efs_errno = EINVAL;
    return -1;
  }

  if (image_info_ptr->stream_state < 0) {
    efs_errno = EINVAL;
    return -1;
  }

#ifdef FEATURE_EFS_QEFS2
  fs_global_lock ();
  result = fs_get_fs_data (image_info_ptr, page_buffer);
  fs_global_unlock ();
#else
  (void) page_buffer;
  result = -EPERM;
#endif

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else {
    return 0;
  }

} /* end of efs_get_fs_data */

/***********************************************************************
 * FUNCTION      efs_get_device_info
 *
 * DESCRIPTION   This function will get the attributes of the flash device
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  If successful efs_access will return zero else -1.
 *
 * SIDE EFFECTS  None
 ***********************************************************************/
#ifdef FEATURE_EFS_QEFS2
extern int fs_get_device_attr(struct fs_device_attr * dev_attr);
#endif

int
efs_get_device_info (struct fs_device_attr * dev_attr)
{
  int result;

  EFS_BAB_START (EFS_BAB_GET_DEVICE_INFO);

#if defined (FEATURE_EFS_QEFS2)
  result = fs_get_device_attr(dev_attr);
#elif defined (FEATURE_EFS_FDI)
  result = fs_fdi_get_device_attr (dev_attr);
#else
  #error "Please provide implementation for efs_get_device_info()"
#endif

  EFS_BAB_END (EFS_BAB_GET_DEVICE_INFO);
  return result;
} /* efs_get_device_info */

/***********************************************************************
 * FUNCTION
 *   efs_image_prepare
 *
 * DESCRIPTION
 *   Make progress toward preparing this image for a factory dump.  This
 *   does a small amount of work, of whatever kind is needed to prepare to
 *   make an exportable image of this filesystem.  This function should be
 *   called repeatedly until it returns that there is no work left to do.
 *
 * RETURN VALUE
 *   Returns a count indicating any additional work.  This number should
 *   only be compared against zero.  Positive indicates that there is work
 *   to do, and negative indicates an error has occured.
 ***********************************************************************/
/* XXX: To support multiple filesystems, this function needs to take an
 * argument to indicate which FS to use. */
int
efs_image_prepare (void)
{
#if defined(FEATURE_EFS_QEFS2)
  {
    extern int fs_efs2_image_prepare (void);
    EFS_BAB_API (EFS_BAB_IMAGE_PREPARE);
    return fs_efs2_image_prepare ();
  }
#elif defined(FEATURE_EFS_FDI)
  return 0;
#else
  #error "Provide efs_image_prepare() for this file system"
#endif
}


/*************************************************************************
 *
 * efs_chown
 *
 * change the owner and/or group id
 *
 */

int
efs_chown (const char *path, int uid_val, int gid_val)
{

  int result;
  struct nameidata *ndp = &nameidata_buffer;

  result = 0;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_CHOWN);
  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto cleanup;

  /* Everything checks out so let us change the group id */

  result = ndp->vp->vops->chown(ndp->vp, uid_val, gid_val);

cleanup:
  if (ndp->dvp != NULL)
    fs_vnode_unref(ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref(ndp->vp);
  EFS_BAB_END (EFS_BAB_CHOWN);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;
}

/*************************************************************************
 *
 * efs_set_reservation
 *
 * sets the amount of space for a reservation
 *
 */

int
efs_set_reservation(const char *path, uint32 groupid, uint32 size)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  result = 0;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_SET_RESERVATION);

  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto cleanup;

  /* Everything checks out so let us set the reservation
   */

  result = ndp->vp->vops->set_reservation(ndp->vp, groupid, size);

cleanup:
  if (ndp->dvp != NULL)
    fs_vnode_unref(ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref(ndp->vp);
  EFS_BAB_END (EFS_BAB_SET_RESERVATION);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;

}

/*************************************************************************
 *
 * efs_set_quota
 *
 * sets the quota limit
 *
 */

int
efs_set_quota(const char *path, uint32 groupid, uint32 size)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  result = 0;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_SET_QUOTA);

  /* We need a buffer to pass in updated values to the superblock so just
   * get a buffer for where we are
   */
  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto cleanup;

  /* Everything checks out so let us set the quota
   */

  result = ndp->vp->vops->set_quota(ndp->vp, groupid, size);

cleanup:
  if (ndp->dvp != NULL)
    fs_vnode_unref(ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref(ndp->vp);
  EFS_BAB_END (EFS_BAB_SET_QUOTA);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;

}
/*************************************************************************
 *
 * efs_get_group_info
 *
 * gets the information for quotas and reservations for a specific group
 * id
 *
 */

int
efs_get_group_info(const char *path, uint32 groupid,
    struct fs_group_info * ginfo)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  result = 0;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_GET_GROUP_INFO);

  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto cleanup;

  /* Everything checks out so let us get the reservation
   */

  result = ndp->vp->vops->get_group_info(ndp->vp, groupid, ginfo);

cleanup:
  if (ndp->dvp != NULL)
    fs_vnode_unref(ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref(ndp->vp);
  EFS_BAB_END (EFS_BAB_GET_GROUP_INFO);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;

}

/*************************************************************************
 *
 * FUNCTION      efs_shred

 * DESCRIPTION   Deletes the old invalid (obsolete) data from the device.
 *
 * PARAMETER     The parameter path is used to determine which filesystem
 *               need to be shredded.
 *
 * RETURN VALUE
 *        0   :  if all the obsolete data, as of when the first time shred
 *               is called, is deleted from the device.
 *        1   :  if the shred is incomplete, another efs_shred call should
 *               be issued until 0 is returned.
 *        -1  :  if the shred is not possible.
 * *
 */
int
efs_shred (const char *path)
{
  struct nameidata *ndp = &nameidata_buffer;
  struct fs_vnode *vp;
  int result = -1;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_SHRED);
  EFS_BAB_PRINT (EFS_BAB_SHRED, path);

  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto error_case;

  vp = ndp->vp;

  result = vp->vops->shred (vp);

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN(EFS_BAB_SHRED, -result);

  EFS_BAB_END (EFS_BAB_SHRED);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  return result;
}
