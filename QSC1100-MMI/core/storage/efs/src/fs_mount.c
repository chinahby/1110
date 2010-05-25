/***********************************************************************
 * fs_mount.c
 *
 * EFS2 mountpoint handling.
 * Copyright (C) 2005--2008 QUALCOMM, Inc.
 *
 * Implements handling generally for all mountpoints.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_mount.c#2 $ $DateTime: 2008/10/29 18:12:17 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-10-27   yg    Removing remote EFS related code for CMI.
2007-10-01   rp    Included string.h, Removed fs_db.h inclusion in fs_vnode.h
2007-05-25   umr   Added support for EFS memory protection feature.
2006-06-07   sh    Aegis requires me to touch this file.  No change.
2006-06-02   dlb   Fix partial mount on mount error.
2006-05-10   dlb   Track mountpoint generations.
2006-03-29   nrs   Check for null function pointers in unmount
2006-02-16   dlb   Add mountpoint debugging.
2005-08-03   dlb   Add umount support, properly.
2005-07-13   dlb   Create

===========================================================================*/

#include "msg.h"
#include "fs_err.h"
#include "fs_mount.h"
#include "fs_protected_memory.h"
#include <string.h>

struct fs_mount *fs_root_mpoint;
fs_dev_t         fs_root_dev;
struct fs_vnode *fs_root_vnode;
struct fs_mpoint fs_mpoints[FS_MAX_MOUNTS];

static int mtable_count = 0;

#ifdef FS_DEBUG_MOUNTS

static void
fs_dump_mpoint_table (void)
{
  unsigned i;

  printf ("%s Mountpoint table\n", MSG_RPC_END);
  printf ("  ind  dev ino  rdv rin  kind\n");
  printf ("  ---  --- ---  --- ---  ----\n");
  for (i = 0; i < FS_MAX_MOUNTS; i++) {
    if (fs_mpoints[i].dev != FS_INVALID_DEV) {
      printf ("  [%2u] %3u %3u  %3u %3u  %s",
          i,
          fs_mpoints[i].dev,
          fs_mpoints[i].inode,
          fs_mpoints[i].root_node->dev,
          fs_mpoints[i].root_node->inum,
          fs_mpoints[i].fstype);
      printf ("\n");
    }
  }
}
#else
#define fs_dump_mpoint_table() (void) 0
#endif /* FS_DEBUG_MOUNTS */

void
fs_mount_init (void)
{
  int i;

  /* Clean out the mountpoint table. */
  for (i = 0; i < FS_MAX_MOUNTS; i++) {
    fs_mpoints[i].mount_at_dev = FS_INVALID_DEV;
    fs_mpoints[i].root_node = NULL;
    fs_mpoints[i].fstype = NULL;
    fs_mpoints[i].mp = NULL;
  }
  mtable_count = 0;
}

void
fs_mount_register (const char *fstype, fs_mount_t mp)
{
  if (mtable_count >= FS_MAX_MOUNTS) {
    FS_ERR_FATAL ("To many internal FS types, edit FS_MAX_MOUNTS", 0, 0, 0);
  }

  fs_mpoints[mtable_count].fstype = fstype;
  fs_mpoints[mtable_count].mp = mp;

  /* XXX: If opposite ends of the remote API can be rebooted
   * independently, then this needs to be set to something random. */
  fs_mpoints[mtable_count].mp->mount_generation = 1;

  mtable_count++;
}

static int
fs_mount_common (const char *fstype, const char *arg,
    struct fs_mpoint **mpoint)
{
  int i;
  int found_kind = 0;
  int result = 0;

  for (i = 0; i < mtable_count; i++) {
    if (strcmp (fstype, fs_mpoints[i].fstype) == 0) {
      found_kind = 1;
      if (fs_mpoints[i].root_node == NULL)
        break;
    }
  }

  if (i == mtable_count) {
    if (found_kind) {
      MSG_HIGH ("mountpoint table full", 0, 0, 0);
      result = -EMFILE;
    } else {
      MSG_HIGH ("Invalid filesystem type", 0, 0, 0);
      result = -ENODEV;
    }
  }

  if (result == 0) {
    /* The device number doesn't have that much meaning here, but it does
     * need to be unique.  Using the table index gives us the uniqueness.
     * Some mpoints may require that the dev be initialized before calling
     * their 'start'. */
    fs_mpoints[i].mp->dev = i;

    result = fs_mpoints[i].mp->ops->start (fs_mpoints[i].mp, arg);
  }

  if (result == 0) {
    result = fs_mpoints[i].mp->ops->get_root (fs_mpoints[i].mp,
        &fs_mpoints[i].root_node);
    /* The root should always be available.  If this doesn't work, the
     * mountpoint is in error. */
    if (result != 0)
      FS_ERR_FATAL ("Unable to get root of file filesystem", 0, 0, 0);
  }

  if (result == 0) {
    *mpoint = &fs_mpoints[i];
    fs_mpoints[i].mp->mount_generation++;
  }

  return result;
}

int
fs_mount_root (const char *fstype, const char *arg)
{
  int result;
  struct fs_mpoint *mpoint;

  result = fs_mount_common (fstype, arg, &mpoint);

  if (result == 0) {
#ifdef FS_DEBUG_MOUNTS
    MSG_HIGH ("root mount: %s, dev=%d, inum=%d",
        fstype, mpoint->dev, mpoint->root_node->inum);
#endif
    fs_root_mpoint = mpoint->mp;
    fs_root_dev = mpoint->mp->dev;
    fs_root_vnode = mpoint->root_node;
  }

  fs_dump_mpoint_table ();

  return result;
}

int
fs_mount_mount (struct fs_vnode *mount_at, const char *fstype, const char *arg)
{
  int result;
  struct fs_mpoint *mpoint;

  result = fs_mount_common (fstype, arg, &mpoint);

  if (result == 0) {
#ifdef FS_DEBUG_MOUNTS
    MSG_HIGH ("sub mount: %s, dev=%d, inum=%d",
        fstype, mount_at->dev, mount_at->inum);
    MSG_HIGH ("    sub dev=%d, inum=%d",
        mpoint->root_node->dev, mpoint->root_node->inum, 0);
#endif
    mpoint->mount_at_dev = mount_at->dev;
    mpoint->mount_at_inode = mount_at->inum;
  }

  fs_dump_mpoint_table ();

  return result;
}

int
fs_mount_umount (struct fs_vnode *mpoint)
{
  int result = 0;
  int i;

  for (i = 0; i < FS_MAX_MOUNTS; i++) {
    if (fs_mpoints[i].root_node == mpoint)
      break;
  }

  if (i == FS_MAX_MOUNTS) {
    MSG_MED ("Attempt to umount non-mountpoint", 0, 0, 0);
    return -EINVAL;
  }

  if (i == 0) {
    MSG_MED ("Attempt to umount root", 0, 0, 0);
    return -EINVAL;
  }

  /* Call the FS specific umount code. */
  /* Check to make sure the function pointer actually exists */
  if (fs_mpoints[i].mp->ops->stop != NULL)
    result = fs_mpoints[i].mp->ops->stop (fs_mpoints[i].mp);

  if (result == 0) {
    fs_vnode_unref (fs_mpoints[i].root_node);

    /* The higher level common code will clean up all open descriptors on
     * this mountpoint, we just have to clean the mountpoint itself up. */
    fs_mpoints[i].mount_at_dev = FS_INVALID_DEV;
    fs_mpoints[i].root_node = NULL;
  }

  return result;
}

int
fs_mount_remount (struct fs_vnode *oldvp, struct fs_vnode *newvp)
{
  int i;

  for (i = 0; i < FS_MAX_MOUNTS; i++) {
    if (fs_mpoints[i].root_node == oldvp)
      break;
  }

  if (i == FS_MAX_MOUNTS) {
    MSG_MED ("Attempt to remount non-mountpoint", 0, 0, 0);
    return -EINVAL;
  }

  if (i == 0) {
    MSG_MED ("Attempt to remount root", 0, 0, 0);
    return -EINVAL;
  }

  /* Replace where the mountpoint occurs, future namei scans will find this
   * mountpoint at the new location. */
  fs_mpoints[i].mount_at_dev = newvp->dev;
  fs_mpoints[i].mount_at_inode = newvp->inum;

  return 0;
}
