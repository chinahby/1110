#ifndef __FS_DB_H__
#define __FS_DB_H__
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   File System Database Code Header

General Description
  Database Code

Copyright (c) 2002-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_db.h#2 $ $DateTime: 2008/12/12 09:42:18 $ $Author: richardp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/08   rp      Introduce device abstraction.
10/18/07   rp      Featurized QEFS2.
06/14/06   dlb     Make iterators max track EFS max iterators.
04/13/06   dlb     Add 'const' to some pointers.
04/07/06   dlb     Add query for iterator value.
01/11/06   nrs     Fixed copyright header
10/30/05   sh      Lint cleanup.
10/18/05   nrs     Added parameters for UID
08/25/05   nrs     Implent buffer accounting for quotas and reservations
04/26/05   dlb     Add 2K page support.
03/30/05   dlb     Increase max depth of tree to allow for larger flash.
10/15/04   dlb     Update copyright line.
10/07/04   dlb     Whitespace cleanup.
05/01/03   dlb     Improved database algorithm.
02/28/03   jkl     Clean Up code.
===========================================================================*/

#include "fs_efs2_cust.h"

#ifdef FEATURE_EFS_QEFS2

#include "fs_buffer.h"

struct fs_db_data;
typedef struct fs_db_data *fs_db_t;

struct fs_db_iter_data;
typedef struct fs_db_iter_data *fs_db_iter_t;

#define FS_DB_TRUE 0
#define FS_DB_FALSE -1

/* Initialize the database to a given device. */
void fs_db_init (fs_db_t db, fsb_t buf);

/* Add a key/value pair.  Returns 0 if successful, or -1 if the key/value
 * pair was already in the database. */
int fs_db_add (fs_db_t db, const void *key, unsigned key_length,
    const void *value, unsigned value_length, uint32 gid, uint32 uid);

/* Update a key/value pair.  Returns 0 if successful, or -1 if the key is
 * not present in the database. */
int fs_db_update (fs_db_t db, const void *key, unsigned key_length,
    const void *value, unsigned value_length, uint32 gid, uint32 uid);

/* Lookup a key in the database.  Returns 0 if found, and -1 if not found.
 * *value_length must be set to the length of available data that can be
 * stored in value.  It will be set to the length of actual data returned.
 * Returns 0 if found or a negative errno, including, -ENOENT if the key is
 * not found, and -ENAMETOOLONG if the name doesn't fit.  If -ENAMETOOLONG
 * is returned, the name will be filled in up to the available character
 * count. */
int fs_db_lookup (fs_db_t db, const void *key, unsigned key_length,
    void *value, unsigned *value_length);

/* Delete a key from the database.  Returns 0 if successful, or -1 if the
 * key is not present. */
int fs_db_delete (fs_db_t, void *key, unsigned key_length);

/* Get the key from the current iterator.  We must have enough space to
 * store the key, or this will fail.  0 means we got the key.  -1 means
 * that either it didn't fit, or there are no more. */
int fs_db_iter_get_key (fs_db_iter_t iter, void *key, unsigned *key_length);

/* Get the value for the current iterator.  Returns same status as
 * fs_db_iter_get_key(). */
int fs_db_iter_get_value (fs_db_iter_t iter, void *value,
    unsigned *value_length);

/* Advance the iterator to the next value.  Returns 0 for success.
 * (Probably will ERR_FATAL, rather than return failure, since you are
 * supposed to stop after fs_db_iter_get_key returns nonzero). */
int fs_db_iter_next (fs_db_iter_t iter);

/* Allocate a new iterator based on this FS. */
fs_db_iter_t fs_db_iter_start (fs_db_t db, void *key, unsigned key_length);

/* Free up an iterator. */
void fs_db_iter_free (fs_db_iter_t iter);

#ifdef FS_UNIT_TEST
#error code not present
#endif

/**********************************************************************
 * Private data.
 */

/* Maximum key length. */
#define FS_DB_MAX_KEY_LENGTH    128
#define FS_DB_MAX_DATA_LENGTH   128

/* Maximum depth of tree supported.  Determines the size of a few in-memory
 * data structures. */
#define FS_DB_MAX_DEPTH         8

#define FS_DB_CLUSTERS_PER_LEVEL        2

/* Number of clusters that database can have wired at any time.  The code
 * determines the value, it is not configurable. */
#define FS_DB_ACTIVE_CLUSTERS   (FS_DB_CLUSTERS_PER_LEVEL*FS_DB_MAX_DEPTH + 2)

/* Aux clusters for inserts and deletes. */
#define FS_DB_AUX1              ((FS_DB_ACTIVE_CLUSTERS) - 2)
#define FS_DB_AUX2              ((FS_DB_ACTIVE_CLUSTERS) - 1)

/* How many iterators are available, smaller will be more efficient.  This
 * is a configuration option. */
#define FS_DB_ACTIVE_ITERATORS  FS_MAX_ITERATORS

struct node_data;
typedef struct node_data *node_t;

/* Iterator data.  This is a little redundant to refer to itself. */
struct fs_db_iter_data {
  fs_db_t       db;
  int           index;
  fs_cluster_id cluster;
  int           pos;
};

struct fs_db_data {
  /* Device to use. */
  fsb_t         buf;

  /* Current top node in the tree.  When modified, the upper-data field in
   * the superblock also needs to be updated. */
  fs_cluster_id top;

  /* Level of the top of the tree.  0 is the leaf.  Will be filled in by
   * down find. */
  int           top_level;

  /* Which of the clusters below is "active". */
  int           active;

  /* Please see the C file for a description of the active clusters notion. */
  fs_cluster_id clust[FS_DB_ACTIVE_CLUSTERS];
  node_t        node[FS_DB_ACTIVE_CLUSTERS];

  /* Position within the active cluster where the work is to be done. */
  unsigned      pos[FS_DB_MAX_DEPTH];

  /* Did the last scan result in a match. */
  int           matched;

  /* Temporary key used when expanding the tree. */
  uint8         tmp_key[FS_DB_MAX_KEY_LENGTH+1+FS_DB_MAX_DATA_LENGTH+1];

  /* Temporary buffers. */
  uint8         tmp_buf[1][EFS_PAGE_SIZE];

  /* The iterators.  Maintain the count to speedup the normal case of no
   * active iterators. */
  int           iter_count;
  struct fs_db_iter_data   iters[FS_DB_ACTIVE_ITERATORS];
};

#endif /* FEATURE_EFS_QEFS2 */

#endif /* not __FS_DB_H__ */

