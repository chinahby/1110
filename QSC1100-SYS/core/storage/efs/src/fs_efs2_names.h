/***********************************************************************
 * fs_efs2_names.h
 *
 * Short description
 * Copyright (C) 2006-2007 QUALCOMM, Inc.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_efs2_names.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-10-18   rp    Featurized QEFS2.
2006-06-27   dlb   Pass stat through readdir.
2006-06-01   dlb   Comment fix.
2006-05-31   dlb   Fix problem with multiple iterators.
2006-04-13   dlb   Create

===========================================================================*/

#ifndef __FS_EFS2_NAMES_H__
#define __FS_EFS2_NAMES_H__

#include "fs_efs2_cust.h"

#ifdef FEATURE_EFS_QEFS2

#include "comdef.h"
#include "fs_sys_types.h"
#include "fs_db.h"

/* Each database entry has a single-character prefix indicating its type.
 * Currently, the only key type stored indicates directories. */
#define FS_KEY_DIR              'd'

/* For keys of type FS_KEY_DIR, the first character of the value indicates
 * the type of entity. */
#define FS_VAL_FILE             'i'
#define FS_VAL_SYMLINK          's'
#define FS_VAL_ITEM             'n'
#define FS_VAL_LONGNAME         'L'

/* Add a single directory entry to 'parent_dir' with the given name (length
 * name_len).  This entry will contain 'value'.  Returns 0 for success, or
 * a negative errno for an error. */
int fs_efs2_name_add (
    fs_db_t db,
    const char *name,
    unsigned name_len,
    fs_inode_t parent_dir,
    const void *value,
    unsigned value_len,
    fs_gid_t gid,
    int allow_overwrite);

/* Remove a single entry, previously added with fs_efs2_name_add.  Returns
 * '0' for success or a negative error code. */
int fs_efs2_name_remove (
    fs_db_t db,
    const char *name,
    unsigned name_len,
    fs_inode_t parent_dir);

/**********************************************************************
 * Name lookup.
 */

/* Lookup a single entry in the database, returns '0' for success, or a
 * negative error if the entry was not found.  The result can be queried
 * with the following functions. */
int fs_efs2_name_lookup (
    fs_db_t db,
    const char *name,
    unsigned name_len,
    fs_inode_t parent_dir);

/* Return the db "key" for the last lookup.  This is a pointer to a static
 * buffer shared across calls, so the data must be called before routines
 * in this module are called again.  This key can be used by
 * fs_efs2_name_keyed_lookup to find this entry again. */
void fs_efs2_name_last_key (const uint8 **key, unsigned *key_len);

/* Return the last "value" for the last lookup.  This is a pointer to a
 * static buffer that will be overwritten by subsequent calls to lookup. */
void fs_efs2_name_last_value (const uint8 **value, unsigned *value_len);

/* Perform a lookup by a key returned previously by
 * fs_efs2_name_last_key().  Can return an error if the key is no longer in
 * the database. */
int fs_efs2_name_key_lookup (fs_db_t db, const uint8 *key, unsigned key_len);

/**********************************************************************
 * Name iteration.
 */

/* Begin directory iteration of the specified directory.  Returns a
 * non-null pointer (the database iterator) upon success. */
void *fs_efs2_iter_begin (
    fs_db_t db,
    fs_inode_t dir_inum);

/* Read the next directory entry.  The name is filled in (if possible), and
 * the iterator is advanced.  Returns 0 on success, -EEOF if there are no
 * more, or another negative error code to indicate a failure.  The dirent
 * structure will be filled in (d_name set to the null terminated name, and
 * the stats filled in as much as we know). */
int fs_efs2_iter_next (void *iter, fs_inode_t dir_inum,
    struct fs_dirent *dirent);

/* Close the directory iterator.  Returns 0 for success. */
int fs_efs2_iter_close (void *iter);

#endif /* FEATURE_EFS_QEFS2 */

#endif /* not __FS_EFS2_NAMES_H__ */
