/**********************************************************************
 * fs_journal.h
 *
 * File system memory journal.
 * Copyright (C) 2002, 2003, 2004, 2007 Qualcomm, Inc.
 */


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_journal.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2007-10-18    rp      Featurized QEFS2.
2004-10-15    dlb     Update copyright line.
2003-06-15     gr     Deleted a comment that no longer applies.
2003-03-07    dlb     Added fs_journal_compare().
2002-08-08    drh     Created by dlb.  Added history header.

===========================================================================*/


#ifndef __FS_JOURNAL_H__
#define __FS_JOURNAL_H__

#include "fs_efs2_cust.h"

#ifdef FEATURE_EFS_QEFS2

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* As EFS page and rpage table entries are updated, the changes are
 * committed to a log, but not written immediately to flash.  The
 * memory-journal keeps track of these changes.  The ptable code will
 * always consult the memory journal first before the actual tables, always
 * giving it the current view.
 *
 * Each entry in the memory journal has an associated age.  Entries can be
 * purged from the memory journal by age, forcing their changes to be
 * written to the actual tables.
 *
 * The keys are either page_id or cluster_id (depending on the data).  Each
 * key also as associated with it a key_type, which indicates the
 * particular table that is affected by this entry.
 */

/* The primary parameter of this journal (which determines the size of its
 * structure) is the number of entries that can be made.
 */

#define FS_JOURNAL_SIZE         1024

struct fs_journal_data;
typedef struct fs_journal_data *fs_journal_t;

/* Initialize the journal.  The journal will have zero entries. */
void fs_journal_init (fs_journal_t jr);

/* Add a single entry to the journal.  If an entry with the same key
 * exists, the value will simply be replaced. */
void fs_journal_add (fs_journal_t jr,
    uint32 key,
    unsigned key_type,
    uint32 value,
    unsigned age);

/* Lookup a single journal entry.  Returns TRUE if the entry was found, or
 * FALSE if it couldn't be found.  If found, the value, and age fields will
 * be set. */
int fs_journal_lookup (fs_journal_t jr,
    uint32 key,
    unsigned key_type,
    uint32 *value,
    unsigned *age);

/* Iterators:
 * There are two iterators for walking through specified nodes within the
 * tree.  These are implemented such that they can be used simultaneously.
 * It is also permissible to add and entries to the tree while iterating.
 * Deletes should not be performed (not hard, since there is not publicly
 * exported delete call).  Also note that the two iterators will never
 * visit the same node. */

/* Setup the iterator for "age" walking.  This walks through every node of
 * a specified age.  The nodes are visited in order.  Advance must be
 * called before accessing any nodes. */
void fs_journal_setup_age_walk (fs_journal_t jr,
    unsigned age);

/* Advance the age walk iterator.  Returns TRUE if there is a node
 * available.  If delete_previous is TRUE the previous (current) node will
 * be deleted as part of the advance. */
int fs_journal_age_advance (fs_journal_t jr,
    int delete_previous);

/* Get the information from the current age walk iterator.  Only valid
 * after a call to fs_journal_age_advance returns true. */
void fs_journal_age_info (fs_journal_t jr,
    uint32 *key, unsigned *key_type,
    uint32 *value, unsigned *age);

/* Setup the iterator for key range walking.  This walks through all of the
 * nodes of a given key type and fall in a certain age range. */
void fs_journal_setup_key_range_walk (fs_journal_t jr,
    unsigned key_type,
    uint32 key_min, uint32 key_max);

/* Same idea as fs_journal_age_advance. */
int fs_journal_key_range_advance (fs_journal_t jr, int delete_previous);

/* Same idea as fs_journal_age_info. */
void fs_journal_key_range_info (fs_journal_t jr,
    uint32 *key, unsigned *key_type,
    uint32 *value, unsigned *age);

/* Return a count of the number of journal entries that remain free. */
int fs_journal_free_count (fs_journal_t jr);

#ifdef FS_UNIT_TEST
#error code not present
#endif

/**********************************************************************
 *  ____       _            _
 * |  _ \ _ __(_)_   ____ _| |_ ___
 * | |_) | '__| \ \ / / _` | __/ _ \
 * |  __/| |  | |\ V / (_| | ||  __/
 * |_|   |_|  |_| \_/ \__,_|\__\___|
 *
 * All information below should be considered private and not externall
 * accessible.
 **********************************************************************/

/* This implementation uses RED/BLACK trees. */

#define FS_JOURNAL_MAX_ITERATORS        2
# define FS_JOURNAL_ITERATOR_AGE        0
# define FS_JOURNAL_ITERATOR_KEY_RANGE  1

typedef uint16 fs_j_index;

struct fs_journal_data {
  fs_j_index    left[FS_JOURNAL_SIZE];
  fs_j_index    right[FS_JOURNAL_SIZE];
  fs_j_index    parent[FS_JOURNAL_SIZE];
  uint32        key[FS_JOURNAL_SIZE];
  uint32        value[FS_JOURNAL_SIZE];
  uint8         age[FS_JOURNAL_SIZE];
  uint8         key_type[FS_JOURNAL_SIZE];
  uint8         color[(FS_JOURNAL_SIZE+7)/8];

  fs_j_index    nil;
  fs_j_index    root;
  fs_j_index    free_list;

  int used;

  fs_j_index    iterator_nexts[FS_JOURNAL_MAX_ITERATORS];
  fs_j_index    iterators[FS_JOURNAL_MAX_ITERATORS];

  unsigned      age_walk_age;
  unsigned      key_range_walk_key_type;
  uint32        key_range_walk_key_min;
  uint32        key_range_walk_key_max;
};

/* This is an optimization over using j->nil */
#if 1
 #define FS_J_NIL        0
#else
 #define FS_J_NIL       jr->nil
#endif

#ifdef FS_UNIT_TEST
#error code not present
#endif

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_EFS_QEFS2 */

#endif /* __FS_JOURNAL_H__ */
