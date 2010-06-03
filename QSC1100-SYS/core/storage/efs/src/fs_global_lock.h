/***********************************************************************
 * fs_global_lock.h
 *
 * FS_GLOBAL_LOCK critical section
 * Copyright (C) 2007 QUALCOMM, Inc.
 *
 * The FS_GLOBAL_LOCK is used to protect the file system data from
 * concurrent access.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_global_lock.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-08-19   rp    Create

===========================================================================*/

#ifndef __FS_GLOBAL_LOCK_H__
#define __FS_GLOBAL_LOCK_H__

void fs_global_lock_init (void);
void fs_global_lock (void);
void fs_global_unlock (void);
void fs_efs2_unprotect_memory (void);

#define FS_GLOBAL_LOCK_INIT() fs_global_lock_init()
#define FS_GLOBAL_LOCK()     fs_global_lock ()
#define FS_GLOBAL_UNLOCK()   fs_global_unlock ()


#endif /* not __FS_GLOBAL_LOCK_H__ */

