/***********************************************************************
 * fs_fdi_inode.h
 *
 * FDI specific members of inode.
 * Copyright (C) 2007-2008 QUALCOMM, Inc.
 *
 * All the FDI specific members of the inode. These members will become part
 * of the vnode.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.w
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_fdi_inode.h#2 $ $DateTime: 2008/07/31 16:37:17 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-07-28   rp    Renamed fs_fdi.h to fs_fdi_i.h for /inc folder.
2007-10-01   rp    Create

===========================================================================*/
#ifndef __FS_FDI_INODE_H__
#define __FS_FDI_INODE_H__

#include "fs_fdi_cust.h"

#ifdef FEATURE_EFS_FDI

/* FDI specific inode members. This structure will become part of the vnod.e */
struct fdi_inode {
  uint32        size;           /* Size of file, in bytes. */
  uint16        uid;
  uint32        gid;

  uint32        mtime;          /* Last mod time of file. */
  uint32        ctime;          /* Posix ctime. */

  uint32        fdi_fd;
};

#endif /* FEATURE_EFS_FDI */

#endif /* not __FS_FDI_INODE_H__ */
