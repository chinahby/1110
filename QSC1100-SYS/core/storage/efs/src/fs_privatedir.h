/***********************************************************************
 * fs_privatedir.h
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

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_privatedir.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-11-16    rp   Added a temp-directory in our private folder.
2006-11-02    rp   Create

===========================================================================*/

#ifndef __FS_PRIVATEDIR_H__
#define __FS_PRIVATEDIR_H__

/* This is EFS module's private folder, EFS's module's meta-data is kept here*/
#define FS_EFS_PRIVATE_DIR_PATH "/.efs_private"

/* This is our private directory to store symlinks when we iterate through
   all the items of a given directory in the efs_iter(), This directory will
   get deleted everytime upon boot. */
#define FS_EFS_PRIVATE_DIR_ITER_PATH FS_EFS_PRIVATE_DIR_PATH"/iter"

/*=============================================================================
  FUNCTION      efs_privatedir_init

  DESCRIPTION   Register with Diag to prevent access to our special
                '/.efs_private/ folder by Diag users.

  PARAMETERS    None

  RETURN VALUE  None
=============================================================================*/
void efs_privatedir_init (void);


/*=============================================================================
  FUNCTION      efs_privatedir_set_access

  DESCRIPTION   This function will allow/restrict diag users from accessing the
                EFS's /.efs_private folder

  PARAMETERS
    allow_access -> [IN]
                     0 : means restrict access; 1(!=0) : means allow access.

  RETURN VALUE  None
=============================================================================*/
void efs_privatedir_set_access (int allow_access);

#endif /* not __FS_PRIVATEDIR_H__ */
