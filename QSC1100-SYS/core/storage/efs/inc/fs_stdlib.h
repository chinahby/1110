/**********************************************************************
 * Filesystem standard library.
 *
 * Copyright (C) 2004,2007-2008 Qualcomm, Inc.
 */

/*======================================================================
 *
 *                             EDIT HISTORY
 *
 * $Header: //source/qcom/qct/core/storage/efs/main/latest/inc/fs_stdlib.h#2 $ $DateTime: 2008/06/03 15:15:30 $ $Author: richardp $
 *
 * when        who   what, where why
 * ----------  ---   -----------------------------------------------------
 * 2008/05/23  rp    Introducing /inc folder as per new SCM.
 * 2007/10/01  rp    Added efs_normalize_path_string().
 * 2004/03/29  dlb   Created.
 *======================================================================*/

#ifndef __FS_STDLIB_H__
#define __FS_STDLIB_H__

/** Expand symlinks and resolve path to an absolute path.  'resolved_path'
 * must have at least FS_PATH_MAX bytes available.  Returns a pointer to
 * resolved_path, if there is no error, otherwise returns a NULL pointer
 * and sets efs_errno to the error value. */
char *efs_realpath (const char *path, char *resolved_path);

/** Similar to above, but allows the maximum buffer to be specified.
 * Setting 'resolve_last_link' to TRUE will resolve all links.  False will
 * cause the last component to not be resolved, if it is a symlink.  Return
 * result is the same as efs_realpath. */
char *efs_normalize_path (const char *path, char *resolved_path,
    int rpath_length, int resolve_last_link);

/** Similar to efs_normalize_path(), but this is a light version, it will not
 * stat filepath, will not acquire global-lock, treats the file-path as a
 * string and gets rid of '.' and takes care of '..' in the path. Return
 * result is the same as efs_realpath.
 * Note: The intermediary directory-names, symlinks etc are not checked
 * for existence. */
char *efs_normalize_path_string (const char *path, char *resolved_path,
    int rpath_length);

#endif /* not __FS_STDLIB_H__ */
