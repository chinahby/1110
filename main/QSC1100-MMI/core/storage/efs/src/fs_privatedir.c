/***********************************************************************
 * fs_privatedir.c
 *
 * This module reserves a folder only to be available for EFS-Module. Other
 * EFS-Module's could create  their own-specific directories inside this
 * folder.
 *
 * Copyright (C) 2006-2007 QUALCOMM, Inc.
 *
 * EFS-Module will reserve a particular folder for its internal storage, and
 * will prevent diag-users from accessing this folder.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_privatedir.c#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-11-16    rp   Blow away the temp-directory in our private folder on boot.
2006-11-02    rp   Create

===========================================================================*/

#include "fs_privatedir.h"
#include "fs_diag_access.h"
#include "fs_public.h"

/* List of directories that we will prevent diag-users from accessing */
#define FS_PRIVATE_DIR_COUNT      2
static const char *fs_privatedir_list[FS_PRIVATE_DIR_COUNT];

/* This variable if set to 1 will allow diag to access the EFS private dir */
int efs_privatedir_allow_access = 0;


/* This is the callback function that diag will call when diag-users try to
   access our '/.efs_private' folder. We shall return FALSE to prevent-
   access */
static boolean
efs_privatedir_check_diag_access_cb (char *path, uint32 op_type)
{
  (void) path;
  (void) op_type;

  if (efs_privatedir_allow_access) {
    return TRUE;
  }

  /* return FALSE to prevent access */
  return FALSE;
}

void
efs_privatedir_init (void)
{
  /* Remove any file by this name and then try to create this directory */
  (void) efs_unlink (FS_EFS_PRIVATE_DIR_PATH);
  (void) efs_mkdir (FS_EFS_PRIVATE_DIR_PATH, 0755);

  /* Blow away if there are any contents in the iter directory. */
  (void) efs_deltree (FS_EFS_PRIVATE_DIR_ITER_PATH);

  /* Register a callback with diag for any attempt to access to our
     '/.efs_metadata' folder */
  fs_privatedir_list[0] = FS_EFS_PRIVATE_DIR_PATH;
  fs_privatedir_list[1] = FS_EFS_PRIVATE_DIR_ITER_PATH;
  diag_fs_register_access_check (fs_privatedir_list, FS_PRIVATE_DIR_COUNT,
      efs_privatedir_check_diag_access_cb);
}

void
efs_privatedir_set_access (int allow_access)
{
  efs_privatedir_allow_access = allow_access;
}

