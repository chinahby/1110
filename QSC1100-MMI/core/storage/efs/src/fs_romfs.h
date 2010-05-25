/**********************************************************************
 * fs_romfs.h
 *
 * EFS2
 * Copyright (C) 2002-2005, 2007, Qualcomm, Inc.
 *
 * ROM filesystem mountpoint handler.
 *
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_romfs.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2007-10-18    rp      Featurized QEFS2.
2005-09-02    dlb     Remove spurious declaration.
2005-07-16    dlb     Mountpoint cleanup.
2004-10-15    dlb     Update copyright header.
2002-10-14    dlb     Created

===========================================================================*/

#ifndef __FS_ROMFS_H__
#define __FS_ROMFS_H__

#include "fs_efs2_cust.h"

#ifdef FEATURE_EFS_QEFS2

#include "comdef.h"
#include "fs_mount.h"
#include "fs_vnode.h"

struct efs_romfs_node {
  const char *name;
  void *data;
  uint32 length;
};

struct fs_mount_romfs {
  /* Fields to set before starting the mountpoint. */
  struct fs_mount       parent;
  struct efs_romfs_node *nodes;
  int                   node_count;

  /* The remainder are for internal use by the romfs. */
  struct fs_vnode      *root;
};

void fs_romfs_init (void);

#endif /* FEATURE_EFS_QEFS2 */

#endif /* not __FS_ROMFS_H__ */
