/***********************************************************************
 * fs_efs2.h
 *
 * Filesystem Mountpoint handler. EFS2
 * Copyright (C) 2002-2008, Qualcomm, Inc.
 *
 * Mountpoint handler for EFS2 flash file system.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_efs2.h#2 $ $DateTime: 2008/12/12 09:42:18 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2008-11-03    rp      Introduce device abstraction.
2007-10-26    rp      Corrected featurization of FS_PARANOIA_CHECKS
2007-10-18    rp      Featurized QEFS2.
2007-08-19    rp      FS_GLOBAL_LOCK() cleanup.
2007-08-18    rp      struct fs_vnode cleanup.
2007-08-10    rp      Align buffers passed to flash on a 32-byte boundary.
2007-05-25    umr     Added support for EFS memory protection feature.
2007-05-11    umr     Added stack check on simulator build
2006-11-06    dlb     Delete recovery support.
2006-10-10    dlb     Partial transaction flush support.
2006-09-29    dlb     Support info block upgrades.
2006-04-13    dlb     Add support for very long filenames.
2006-03-02    sh      Removed FS_FIELD_LIMIT_DIFF
2005-10-30    sh      Lint cleanup.
2005-12-06    sh      Added FS_PARANOIA_CHECKS to detect memory corruption.
2005-07-13    dlb     Mountpoint cleanup.
2005-04-26    dlb     Add 2K page support.
2005-01-27    dlb     Allow standalone builds.
2004-10-15    dlb     Update copyright line.
2003-08-20    dlb     Remove delayed truncate code.
2003-08-20    dlb     Use direct inode addresses.
2003-04-22    cr      Added delayed truncation restart implementation.
2003-04-15    cr      Added delayed truncation implementation to fix CR#28410.
2002-10-08    dlb     Rename to fs_efs2.h.
2002-08-08    drh     Created by gr.  Added history header.

===========================================================================*/

#ifndef __FS_EFS2_H__
#define __FS_EFS2_H__

#include "fs_efs2_cust.h"

#ifdef FEATURE_EFS_QEFS2

#include "comdef.h"
#include "fs_mount.h"
#include "fs_efs2_inode.h"

#include "fs_efs2_names.h"


/* Inode tables. */
#define FS_MOUNT_INODE_TREE_DEPTH       3

struct efs_info_data {
  /* Magic number, and version. */
  uint32        magic;
  uint32        version;

  /* Description of the inode location table.  The table is a tree, with
   * the bottom level consisting of clusters with inodes as contents.  The
   * levels above this are lists of clusters defining where to look for an
   * inode. */
  fs_cluster_id inode_top;

  /* Inode freelist management.  Freed inodes are chainged through the
   * data[0] cluster into the inode_free field.  When a new cluster is
   * allocated to hold inodes, inode_next points to the next available
   * inode in that cluster.  When there is no cluster available, then it is
   * set to FS_INVALID_INODE. */
  fs_inode_t    inode_next;
  fs_inode_t    inode_free;

  fs_inode_t    root;

  /******************************/
  /* First field of version supporting partial delete. */

  /* Indicates that a delete was in progress, and recovery needs to finish
   * this operation.  Zero indicates that there is no partial delete in
   * process.  Larger numbers indicate the leaf level that is currently
   * being deallocated.
   * 'partial_delete_mid' indicates that the current level was only
   * partially completed.  Counts have been adjusted, but all of the nodes
   * haven't been marked in the freemap.
   * 'partial_delete_data' contains the indirect block pointers.  Element
   * '0' is for 1 level of indirection, and so on. */
  uint8         partial_delete;
  uint8         partial_delete_mid;
  fs_gid_t      partial_delete_gid;
  fs_cluster_id partial_delete_data
                  [FS_DIRECTION_LEVELS - 1];
};

/* Private mountpoint data. */
struct fs_mount_efs {
  struct fs_mount       parent;
  fs_dev_t      dev;
  struct fs_vnode *root_dir;
  fsb_t         buf;
  fs_db_t       db;
  unsigned int  cluster_size;
  int           cluster_shift;

  /* How deep are our transactions. */
  int           transaction_depth;

  fs_cluster_id info_cluster;

  /* A cached copy of the info data. */
  struct efs_info_data  info;

  /* Dirty flag for the above. */
  int           info_dirty;

  /* A temporary inode used by the allocator. */
  struct efs2_inode tmp_inode;

  /* Shift and mask for converting fixed inode addresses into
   * cluster/offset pairs. */
  int           inode_shift;
  uint32        inode_low_mask;

  /* The database itself. */
  struct fs_db_data database;
};

/* Initialize the mountpoint. */
void fs_efs2_init (void);
/* Initialize the pointers to the aligned buffers. */
void fs_efs2_init_aligned_buf (void);

/* FS_PARANOIA_CHECKS functions */
extern void fs_efs2_paranoia_init (void);
extern void fs_efs2_paranoia_set_skip_check (uint8 skip_check);
extern void fs_efs2_paranoia_record (void);
extern void fs_efs2_paranoia_check (void);

#endif /* FEATURE_EFS_QEFS2 */

#endif /* not __FS_EFS2_H__ */
