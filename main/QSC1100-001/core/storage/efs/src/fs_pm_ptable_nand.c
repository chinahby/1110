/**********************************************************************
 * fs_pm_ptable_nand.c
 *
 * Copyright (C) 2002-2009 QUALCOMM, Inc.
 * Page table management for NAND flash.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_pm_ptable_nand.c#4 $ $DateTime: 2009/02/06 16:05:57 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2009-02-06     yg     Alloc page before entries are removed from journal.
2008-11-03     rp     Introduce device abstraction.
2008-06-02     rp     Removed startup_log_flush_found.
2008-04-28     rp     Fix klocwork warnings.
2008-03-31     yg     Fix Klocwork warnings.
2008-03-24    sch     Added code to handle write failures in pcache_write
2008-01-04     yg     Consider last flush log page to avoid another flush log
2007-12-11     yg     Added some comments.
2007-10-18     rp     Featurized QEFS2.
2007-09-19     yg     Changed NULL Tag name.
2007-08-31     yg     Added some code comments.
2007-08-10     rp     Align buffers passed to flash on a 32-byte boundary.
2007-07-17    sch     Changed logic of pcache_lookup to allow > 2 pages
2007-07-06     yg     Added Tag and dual allocators related code.
2007-06-27     yg     Avoid too many startup log flushes.
2007-05-25    umr     Added support for EFS memory protection feature.
2007-03-30     yg     Added handling for FS_LOG_ENTRY_WRITE_FAIL log entry.
2007-02-23    dlb     Move read failures into device wrappers.
2006-09-11    dlb     Use flash driver wrappers.
2006-06-26     yg     Memory reduction effort
2005-12-14    dlb     Add midpoint startup callback.
2005-10-30    sh      Lint cleanup.
2005-05-26    sd      Compilation fixes for L4.
2005-04-26    dlb     Add 2K page support.
2005-01-04    dlb     Update copyright line.
2004-12-30    dlb     Remove excess infiltration of factory image code.
2004-10-15    dlb     Update copyright header.
2004-10-07    dlb     Whitespace cleanup.
2003-11-13    dlb     Fix journal overflow.
2003-08-11    dlb     Cache ptable pages.
2003-07-24    dlb     Log and process log flushes.
2003-06-17    bgc     Handled new FS_LOG_ENTRY_GARBAGE.
2003-06-17    adm     Featurized factory start code.
2003-06-17    dlb     Fixed erase failure handling code.
2003-06-12    adm     Add support for factory start.
2003-05-13    bgc     Added update of reverse_ptable on log iteration.
2003-05-07    dlb     Fix bug with keyrange log not being grouped.
2003-04-25     gr     Split off NOR and NAND support into separate files.
2003-03-07    dlb     Add support for NAND.
2003-03-05     gr     Put a message on the screen when doing a fresh start.
2003-03-04     gr     Added a call to delay watchdog monitoring of tasks if
                      EFS is doing a fresh start. Initialization takes
                      longer in this case.
2003-01-24    bgc     Added handling of FS_LOG_ENTRY_GC_DEALLOC.
2002-11-15    dlb     Write NOR version.
2002-08-08    drh     Created by dlb.  NAND version.

===========================================================================*/

#include "fs_efs2_cust.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#ifdef FEATURE_EFS_EFS2_ON_NAND

#include <stdio.h>
#include <stdlib.h>

#ifdef FEATURE_IG_EFS_EXT_SERVER
  #include "amssassert.h"
#else
#include "assert.h"
#endif

#include "customer.h"
#include "fs_sys_types.h"
#include "fs_err.h"

#include "fs_pm_ptable.h"
#include "fs_pm_super.h"
#include "fs_device.h"
#include "fs_pm_log.h"
#include "fs_logr.h"

#include "fs_protected_memory.h"

#ifdef FS_UNIT_TEST
#error code not present
#endif

#define BUMP_BLOCK(gc, pvar) \
  do { \
    (pvar) = ((pvar) & (gc)->super->block_mask) + \
      (gc)->super->data->block_size; \
    if ((pvar) >= (gc)->max_page) \
      (pvar) = (gc)->min_page; \
  } while (0)

void fs_ptable_init_nand (
    fs_ptable_t         pt,
    fs_log_t            log,
    int                *fresh_start);

/**********************************************************************
 * Entry points
 *********************************************************************/
static void nand_log_iterate (fs_ptable_t pt, fs_log_t log,
    void (*visit) (void *priv, fs_log_code_t code, uint32 *args),
    void *priv);
static fs_page_id nand_get_forward (fs_ptable_t pt, fs_cluster_id cluster);
static fs_cluster_id nand_get_reverse (fs_ptable_t pt, fs_page_id page,
                                    uint32* tagp);
static void nand_set_forward (fs_ptable_t pt, fs_cluster_id cluster,
    fs_page_id page);
static void nand_set_reverse (fs_ptable_t pt, fs_page_id page,
    fs_page_id state, uint32 tag);
static int nand_erase (fs_ptable_t pt, fs_block_id block);
static void nand_begin_erase (fs_ptable_t pt, fs_block_id block);
static void nand_mark_alloc (fs_ptable_t pt, fs_page_id page);
static void nand_flush (fs_ptable_t pt);
static int nand_do_gc (fs_ptable_t pt);
static void nand_post_init (fs_ptable_t pt);
static void nand_super_update (fs_ptable_t pt);

/**********************************************************************
 * Rtable cache operations.  The RTABLE cache is a write cache of changes
 * to the rtable.  These operations cannot be written to flash until the
 * log page describing the change has been written (so that they can be
 * completed).  Also, they cannot be flushed in the middle of a
 * transaction.  This queue is kept sorted to allow fast lookups.
 */

static uint32 internal_lookup (fs_ptable_nand_t npt, uint32 index,
    int table, int level);

/**********************************************************************
 * Public interface.
 */

/**********************************************************************
 * NAND stub implementation.  Track everything in memory. */

/* Within a given table (at a given level), determine the index of a
 * particular value within that table. */
#define NAND_PTABLE_INDEX(npt, id, level) \
    (((id) & (npt)->masks[level]) >> ((npt)->shifts[level]))

#ifdef FS_DEBUG_TABLE_CHECK
#error code not present
#endif

static void init_shifts_and_masks (fs_ptable_nand_t npt);

static fs_page_id pt_get_reverse (void *priv, fs_page_id page);
static void pt_set_reverse (void *priv, fs_page_id page, fs_page_id state);
static int pt_erase (void *priv, fs_block_id block);
static void pt_mark_alloc (void *priv, fs_page_id page);
static void pt_page_move (void *priv, fs_page_id state,
    fs_page_id old_place, fs_page_id new_place);
static uint32 *pcache_lookup (fs_ptable_nand_t npt, fs_page_id page,
    int is_to_write);
static int pcache_write (fs_ptable_nand_t npt, fs_page_id old_page,
    fs_page_id new_page);
static void pcache_update (fs_ptable_nand_t npt, fs_page_id old_page,
    fs_page_id new_page);
static void pcache_init (fs_ptable_nand_t npt);
static void pcache_unstick (fs_ptable_nand_t npt);

void record_table_movement (
    fs_ptable_nand_t    npt,
    fs_page_id          state,
    fs_page_id          old_place,
    fs_page_id          new_place);

void fs_ptable_init_nand (
    fs_ptable_t         pt,
    fs_log_t            log,
    int                *fresh_start)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) pt;

  npt->log = log;

  init_shifts_and_masks (npt);

  pt->get_forward = nand_get_forward;
  pt->get_reverse = nand_get_reverse;
  pt->set_forward = nand_set_forward;
  pt->set_reverse = nand_set_reverse;
  pt->erase = nand_erase;
  pt->begin_erase = nand_begin_erase;
  pt->mark_alloc = nand_mark_alloc;
  pt->flush = nand_flush;
  pt->log_iterate = nand_log_iterate;
  pt->do_gc = nand_do_gc;
  pt->post_init = nand_post_init;
  pt->super_update = nand_super_update;
  pt->mid_init = NULL;

  fs_journal_init (&npt->journal);

  npt->last_level = pt->super.data->u.nand.page_depth - 1;

#ifdef FS_DEBUG_TABLE_CHECK
#error code not present
#endif

  if (*fresh_start) {
#ifdef FS_DEBUG_TABLE_CHECK
#error code not present
#endif
  } else {
    /* Extract the GC parameters out of the superblock. */
    npt->ptable_gc.alloc_next = pt->super.data->alloc_next[1];
    npt->ptable_gc.gc_next = pt->super.data->gc_next[1];
  }

  npt->ptable_gc.min_page = pt->super.data->u.nand.regions[0]
    << pt->super.block_shift;
  npt->ptable_gc.max_page = pt->super.data->u.nand.regions[1]
    << pt->super.block_shift;
  npt->ptable_gc.super = &pt->super;
  npt->ptable_gc.priv = (void *) npt;
  npt->ptable_gc.dev = pt->dev;
  npt->ptable_gc.log = npt->log;

  npt->ptable_gc.get_reverse = pt_get_reverse;
  npt->ptable_gc.set_reverse = pt_set_reverse;
  npt->ptable_gc.erase = pt_erase;
  npt->ptable_gc.mark_alloc = pt_mark_alloc;
  npt->ptable_gc.page_move = pt_page_move;

  npt->logging = 0;

  pcache_init (npt);

  if (*fresh_start)
    npt->logging = 1;

  fs_gc_init (&npt->ptable_gc, *fresh_start);
}

static void
init_shifts_and_masks (fs_ptable_nand_t npt)
{
  int level;
  uint8         shift = 0;
  uint32        mask = FS_PTE_PAGE_MASK;
  uint32        mask_off = 0xFFFFFFFF;

  for (level = npt->parent.super.data->u.nand.page_depth - 1;
      level >= 0;
      level--)
  {
    mask_off &= ~mask;

    npt->shifts[level] = shift;
    shift += FS_PTE_SHIFT;
    npt->masks[level] = mask;
    mask <<= FS_PTE_SHIFT;
    npt->mask_off[level] = mask_off;
  }
}

void
fs_ptable_nand_log_visit (fs_ptable_t pt, fs_log_code_t code, uint32 *args)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) pt;
  uint32 tag, clust, tmp;

  ASSERT (!npt->logging);

  switch (code) {
    case LOG_CODE_GROUP_END:
      break;

    case FS_LOG_ENTRY_UPPER_DATA:
      if (args[0] < FS_UPPER_DATA_COUNT)
        npt->parent.super.data->upper_data[args[0]] = args[1];
      break;

    case FS_LOG_ENTRY_ERASE_FINISH:
    case FS_LOG_ENTRY_XACT_START:
    case FS_LOG_ENTRY_XACT_END:
    case FS_LOG_ENTRY_ERASE_FAIL:
    case FS_LOG_ENTRY_WRITE_FAIL:
      break;

    case FS_LOG_ENTRY_PTABLE_MOVE:
      /* printf ("ptable move: 0x%08x, 0x%04x -> 0x%04x\n",
          args[0], args[1], args[2]); */
      record_table_movement (npt,
          args[0],
          args[1],
          args[2]);
      break;

    case FS_LOG_ENTRY_GC_MOVE:
    case FS_LOG_ENTRY_PAGE_MOVE:
      /* args[0] = cluster.
       * args[1] = old page,
       * args[2] = new page */
      /* printf ("page_move 0x%08x 0x%08x 0x%08x\n",
          args[0], args[1], args[2]); */
      tag = (args[0] >> TAG_SHIFT) & TAG_MASK;
      clust = args[0] & CLUSTER_MASK;
      if (args[1] != FS_INVALID_PAGE_ID) {
        nand_set_reverse (&npt->parent, args[1], FS_RMAP_PAGE_GARBAGE,
                          FS_PM_NULL_TAG);
      }

      /* Mark the new location. */
      nand_set_reverse (&npt->parent, args[2], clust, tag);

      /* Set the forward table entry. */
      nand_set_forward (&npt->parent, clust, args[2]);
      break;

    case FS_LOG_ENTRY_NEW_DATA:
      /* args[0] = cluster.
       * args[1] = page. */
      tag = (args[0] >> TAG_SHIFT) & TAG_MASK;
      clust = args[0] & CLUSTER_MASK;
      /* Set the reverse table entry. */
      nand_set_reverse (&npt->parent, args[1], clust, tag);

      /* Set the forward table entry. */
      nand_set_forward (&npt->parent, clust, args[1]);
      break;

    case FS_LOG_ENTRY_KEYRANGE:
      /* args[0] = encoded journal for key_type.
       * args[1] = update_index. */
      /* For keyrange walks that get logged, just go through and wipe out
       * journal entries, just like the code did before. */
      /* printf ("keyrange walk: %d, 0x%04x - 0x%04x\n",
          args[0],
          args[1],
          args[1] | (~npt->mask_off[FS_JOURNAL_LEVELOF (args[0])])); */
      tmp = FS_JOURNAL_LEVELOF (args[0]);
      if (tmp >= FS_MAX_LEVELS)
      {
        FS_ERR_FATAL ("Exceed key range %d...",tmp,0,0);
      }
      fs_journal_setup_key_range_walk (&npt->journal,
          args[0],
          args[1],
          args[1] | (~npt->mask_off[tmp]));
      while (fs_journal_key_range_advance (&npt->journal, TRUE))
        ;
      break;

    case FS_LOG_ENTRY_GC_DEALLOC:
      /* args[0] = cluster
       * args[1] = page. */
      nand_set_forward (&npt->parent, args[0], FS_INVALID_PAGE_ID);
      nand_set_reverse (&npt->parent, args[1], FS_RMAP_PAGE_GARBAGE,
                        FS_PM_NULL_TAG);
      break;

    case FS_LOG_ENTRY_GARBAGE:
      /* args[0] = page. */
      nand_set_reverse (&npt->parent, args[0], FS_RMAP_PAGE_GARBAGE,
                        FS_PM_NULL_TAG);
      break;

    case FS_LOG_ENTRY_LOG_FLUSH:
      /* args[0] = log page to flush. */
      if (fs_log_written_count (npt->log) > 0
          && fs_log_peek_written (npt->log) == args[0])
      {
        (void) fs_log_get_written (npt->log);
      }
      break;

    default:
      FS_ERR_FATAL ("Unknown log code: 0x%02x", code, 0, 0);
      break;
  }

  fs_gc_visit_node (&npt->ptable_gc, code, args);
}

static void
nand_log_iterate (fs_ptable_t pt, fs_log_t log,
    void (*visit) (void *priv, fs_log_code_t code, uint32 *args),
    void *priv)
{
  (void) pt;
  (void) log;
  (void) visit;
  (void) priv;

  FS_ERR_FATAL ("nand_log_iterate should never be called", 0, 0, 0);
}

static void
nand_post_init (fs_ptable_t pt)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) pt;

  fs_gc_init_finish (&npt->ptable_gc);

  npt->logging = 1;

#ifdef FS_DEBUG_JOURNAL_CHECK
#error code not present
#endif
}

static void
nand_super_update (fs_ptable_t pt)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) pt;

  /* Fill in fields in the superblock based on our garbage collectors. */
  pt->super.data->alloc_next[1] = npt->ptable_gc.alloc_next;
  pt->super.data->gc_next[1] = npt->ptable_gc.gc_next;
}

static fs_page_id
nand_get_forward (fs_ptable_t pt, fs_cluster_id cluster)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) pt;

#ifdef FS_DEBUG_TABLE_CHECK
#error code not present
#else
  return internal_lookup (npt, cluster, FS_PTABLE, npt->last_level);
#endif
}

static fs_cluster_id
nand_get_reverse (fs_ptable_t pt, fs_page_id page, uint32 *tagp)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) pt;
  fs_cluster_id clust;

#ifdef FS_DEBUG_TABLE_CHECK
#error code not present
#else
  clust = internal_lookup (npt, page, FS_RTABLE, npt->last_level);

  /* If the status isn't a special status like Garbage or Bad block
   * etc then it would be cluster, in that case extract the tag if
   * the upper layer is interested in it. */
  if (!FS_RMAP_IS_SPECIAL (clust))
  {
    if (tagp)
      *tagp = (clust >> TAG_SHIFT) & TAG_MASK;
    clust = clust & CLUSTER_MASK;
  }

  return clust;
#endif
}

static void
nand_set_forward (fs_ptable_t pt, fs_cluster_id cluster,
    fs_page_id page)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) pt;
  fs_page_id log_page;

#ifdef FS_DEBUG_TABLE_CHECK
#error code not present
#endif

  if (npt->logging)
    log_page = fs_log_peek_page (npt->log);
  else
    log_page = fs_logr_peek_alloc (npt->parent.logr);

  fs_journal_add (&npt->journal,
      cluster, FS_JOURNAL_ENCODE (FS_PTABLE, npt->last_level),
      page,
      log_page & 0xFF);
}

static void
nand_set_reverse (fs_ptable_t pt, fs_page_id page,
    fs_page_id state, uint32 tag)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) pt;
  fs_page_id log_page;

  /* If the state is not special state then add tag to the state */
  if (!FS_RMAP_IS_SPECIAL (state))
    state = (state & ~(TAG_MASK << TAG_SHIFT)) |
            ((tag & TAG_MASK) << TAG_SHIFT);
#ifdef FS_UNIT_TEST
#error code not present
#endif

#ifdef FS_DEBUG_TABLE_CHECK
#error code not present
#endif

  if (npt->logging)
    log_page = fs_log_peek_page (npt->log);
  else
    log_page = fs_logr_peek_alloc (npt->parent.logr);

  fs_journal_add (&npt->journal,
      page,
      FS_JOURNAL_ENCODE (FS_RTABLE, npt->last_level),
      state, log_page & 0xFF);
}

static void nand_begin_erase (fs_ptable_t pt, fs_block_id block)
{
  (void) pt;
  (void) block;
  /* Nothing to be done for NAND.  It has hardware CRC checks for
     page validity */
}

static int
nand_erase (fs_ptable_t pt, fs_block_id block)
{
#ifdef FS_UNIT_TEST
#error code not present
#endif

  nand_set_reverse (pt, block << pt->super.block_shift,
      FS_RMAP_BLOCK_ERASED, FS_PM_NULL_TAG);

  return pt->super.data->block_size;
}

static void
nand_mark_alloc (fs_ptable_t pt, fs_page_id page)
{
  /* If this is not the last page of a block, then advance the erase
   * marker. */
  if (((page + 1) & ~pt->super.block_mask) != 0) {
    nand_set_reverse (pt, page + 1, FS_RMAP_BLOCK_ERASED, FS_PM_NULL_TAG);
  }
}

/* This does nothing, at least yet. */
static void
nand_flush (fs_ptable_t pt)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) pt;

  fs_log_flush (npt->log);
}

static void
fs_gc_handle_write_fail (fs_ptable_t pt, uint32 page)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) pt;
  uint32 args[3];
  uint32 tmp;
  /* Handle write failure in ptable region */

  /* Make sure to flush the logs before marking the page as bad */
  args[0] = page;
  fs_log_add (npt->log, FS_LOG_ENTRY_WRITE_FAIL, args);
  fs_log_flush (npt->log);

  npt->ptable_gc.set_reverse (npt->ptable_gc.priv, page,
      FS_RMAP_BLOCK_BAD);

  /* If its the first page in the block then we have to mark it
   * as bad here since it will not be handled later if the state
   * is head (which is true if its first page in the block). */
  if ((page & ~pt->super.block_mask) == 0)
     (void) fs_device_mark_block_bad (pt->dev,
                                     page >> pt->super.block_shift);

  /* Increment bad block count now. We will consider this
   * bad block after we moved all good pages out of this block
   * and mark this as bad in flash and reverse page table. Do
   * err_fatal that time. */
  npt->ptable_gc.bad_blk_count++;

  /* Find the page offset in the block */
  tmp = (page & ~pt->super.block_mask);
  tmp = pt->super.data->block_size - tmp;
  /* Since the count was already reduced in fs_gc_alloc(), adjust
     it here */
  --tmp;

  ASSERT (tmp <= npt->ptable_gc.free_count);
  npt->ptable_gc.free_count -= tmp;

  /* Bump the allocator to next block */
  BUMP_BLOCK (&npt->ptable_gc, npt->ptable_gc.alloc_next);
}

static int
nand_do_gc (fs_ptable_t pt)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) pt;
  fs_page_id log_page;
  unsigned age, key_type;
  uint32 key, value;
  int table, level, update_table, update_level;
  uint32 masked_index, update_index;
  fs_page_id new_location, old_location;
  uint32        log_args[3];
  int did_something = 0;
  int advancing;
  uint32        *update_buffer;

  if (fs_journal_free_count (&npt->journal) < 50)
    FS_ERR_FATAL ("Journal needs to be dealt with", 0, 0, 0);

  /* Do some GC if necessary. */
  fs_gc_do_urgent (&npt->ptable_gc);

  if (fs_log_written_count (npt->log) > 20
      || fs_journal_free_count (&npt->journal) < (FS_JOURNAL_SIZE / 2))
  {
    /* printf ("[lrc]<<<"); fflush (stdout); */
    log_page = fs_log_get_written (npt->log);
    age = log_page & 0xFF;

    /* Setup an age walk. */
    fs_journal_setup_age_walk (&npt->journal, age);

    /* XXXX: This will perform "chunkier" than necessary, since lots of
     * iteration must occur.  This should be made into a state and
     * performed incrementally as necessary (scheduled with the GC). */
    advancing = fs_journal_age_advance (&npt->journal, TRUE);
    while (advancing) {
      fs_journal_age_info (&npt->journal, &key, &key_type, &value, &age);
      table = FS_JOURNAL_TABLEOF (key_type);
      level = FS_JOURNAL_LEVELOF (key_type);

      ASSERT (level != 0);
      ASSERT (level < FS_MAX_LEVELS);
      masked_index = key & npt->mask_off[level];

      /* If this table entry was previously written, we have to read in the
       * old contents. */
      old_location = internal_lookup (npt, key, table, level-1);
      update_buffer = pcache_lookup (npt, old_location, 1);

      update_index = masked_index;
      update_table = table;
      update_level = level;

      /* Update this entry. */
      update_buffer[NAND_PTABLE_INDEX (npt, key, level)] = value;

      /* NOTE: It is important that this allocation not invoke the garbage
       * collector. Also important that this allocation happen before the
       * entries go out of journal. Since the entries that are removed from
       * journal in the following loop will not be available for lookup
       * after this point until pcache write and record_table_movement
       * happens. This allocation might need the data that just cannot
       * be lookup. */
      new_location = fs_gc_alloc (&npt->ptable_gc);

      /* key range walk. */
      fs_journal_setup_key_range_walk (&npt->journal,
          FS_JOURNAL_ENCODE (update_table, update_level),
          update_index,
          update_index | (~npt->mask_off[update_level]));
      while (fs_journal_key_range_advance (&npt->journal, TRUE)) {
        fs_journal_key_range_info (&npt->journal, &key, &key_type,
            &value, &age);

        table = FS_JOURNAL_TABLEOF (key_type);
        level = FS_JOURNAL_LEVELOF (key_type);
        ASSERT (level != 0);
        ASSERT (level < FS_MAX_LEVELS);
        masked_index = key & npt->mask_off[level];

        ASSERT (update_index == masked_index);
        ASSERT (update_level == level);
        ASSERT (update_table == table);

        update_buffer[NAND_PTABLE_INDEX (npt, key, level)] = value;
      }

      /* Write out the newly updated page table. */
      while (pcache_write (npt, old_location, new_location))
      {
        fs_gc_handle_write_fail (pt, new_location);
        new_location = fs_gc_alloc (&npt->ptable_gc);
      }

      /* The keyrange log and the table movement must both be recorded in
       * the log atomically.  record_table_movement uses either
       * FS_LOG_ENTRY_PAGE_MOVE or FS_LOG_ENTRY_PTABLE_MOVE to record the
       * movement.  They are both the same size, so it doesn't matter which
       * we use. */

      fs_log_group (npt->log, FS_LOG_ENTRY_KEYRANGE,
          FS_LOG_ENTRY_PTABLE_MOVE);

      /* Log the keyrange walk. */
      log_args[0] = FS_JOURNAL_ENCODE (update_table, update_level);
      log_args[1] = update_index;
      fs_log_add (npt->log, FS_LOG_ENTRY_KEYRANGE, log_args);

      /* Record the new page table movement. */
      record_table_movement (npt,
          FS_RMAP_PAGE_TABLE (update_table, update_level,
            update_index),
          old_location, new_location);

      pcache_unstick (npt);

      advancing = fs_journal_age_advance (&npt->journal, TRUE);
    }

    /* Log that this log page is no longer needed. */
    log_args[0] = log_page;
    fs_log_add (npt->log, FS_LOG_ENTRY_LOG_FLUSH, log_args);
    did_something = 1;
  }

  return did_something;
}

/* This function is the universal lookup.  Index is either a page or a
 * cluster id (depends on the table).  [table] should be either FS_PTABLE
 * or FS_RTABLE.  The level is which level of the page tree to look that
 * value up in.  0 is the top level (looks up in the superblock).  This
 * always checks the journal first, and also knows how to look at the
 * currently updated page table entry. */
static uint32
internal_lookup (fs_ptable_nand_t npt, uint32 index,
    int table, int level)
{
  int result;
  uint32 value;
  unsigned age;
  static uint32 *buf;
  uint32 masked_index;

  ASSERT (level >= 0);

  /* Top level is always in the superblock. */
  if (level == 0) {
    return npt->parent.super.data->u.nand.tables[table]
      [NAND_PTABLE_INDEX (npt, index, 0)];
  }

  masked_index = index;
  if (level < npt->last_level) {
    masked_index &= npt->mask_off[level+1];
  }

  /* Check for a journal entry. */
  result = fs_journal_lookup (&npt->journal,
      masked_index, FS_JOURNAL_ENCODE (table, level), &value, &age);
  if (result) {
    return value;
  }

  /* Do we have a table to lookup in? */
  value = internal_lookup (npt, index, table, level-1);

  if (value == FS_INVALID_PAGE_ID) {
    return value;
  }

  /* Read in the page and get the value.
   * XXX: caching some of these pages might be useful. */
  buf = pcache_lookup (npt, value, 0);

  return buf[NAND_PTABLE_INDEX (npt, index, level)];
}

/* Called by the gc/other things to record movement of a page table.  There
 * must be room in the journal for the entries. */
void
record_table_movement (
    fs_ptable_nand_t    npt,
    fs_page_id          state,
    fs_page_id          old_place,
    fs_page_id          new_place)
{
  fs_page_id real_index;
  uint32     log_args[3];
  fs_page_id log_page;

  /* The old one is garbage. */
  if (old_place != FS_INVALID_PAGE_ID) {
    nand_set_reverse (&npt->parent, old_place, FS_RMAP_PAGE_GARBAGE,
                      FS_PM_NULL_TAG);
  }

  nand_set_reverse (&npt->parent, new_place, state, FS_PM_NULL_TAG);

  if (npt->logging)
    log_page = fs_log_peek_page (npt->log);
  else
    log_page = fs_logr_peek_alloc (npt->parent.logr);

  /* Update where this table is. */
  real_index = FS_RMAP_GET_INDEX (state);
  if (FS_RMAP_GET_LEVEL (state) > 1) {
    /* XXX: can nand_set_forward be made generic enough to do this? */
    fs_journal_add (&npt->journal,
        real_index,
        FS_JOURNAL_ENCODE (FS_RMAP_GET_TABLE (state),
          FS_RMAP_GET_LEVEL (state) - 1),
        new_place,
        log_page & 0xFF);
  } else {
    /* level 0 is in the superblock. */
    npt->parent.super.data->u.nand.tables
      [FS_RMAP_GET_TABLE (state)]
      [NAND_PTABLE_INDEX (npt, real_index, 0)]
      = new_place;
  }

  /* Log the change. */
  if (npt->logging) {
    log_args[0] = state;
    log_args[1] = old_place;
    log_args[2] = new_place;
    fs_log_add (npt->log, FS_LOG_ENTRY_PTABLE_MOVE, log_args);
    /* printf ("log move: %x %x %x \n",
        log_args[0],
        log_args[1],
        log_args[2]); */
  }
}

/* Entry points for the GC interface. */

static void
pt_page_move (void *priv,
    fs_page_id     state,
    fs_page_id     old_page,
    fs_page_id     new_page)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) priv;

  record_table_movement (npt, state, old_page, new_page);
  /* Also update our pcache layer about this page move, later
   * we would be searching using new page number. So if pcache
   * is referring to this ptable page to old physical page then
   * that also need to get updated. */
  pcache_update (npt, old_page, new_page);
}

static fs_page_id
pt_get_reverse (void *priv, fs_page_id page)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) priv;

  return nand_get_reverse (&npt->parent, page, NULL);
}

static void
pt_set_reverse (void *priv, fs_page_id page, fs_page_id state)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) priv;

  nand_set_reverse (&npt->parent, page, state, FS_PM_NULL_TAG);
}

static int
pt_erase (void *priv, fs_block_id block)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) priv;

  return nand_erase (&npt->parent, block);
}

static void
pt_mark_alloc (void *priv, fs_page_id page)
{
  fs_ptable_nand_t npt = (fs_ptable_nand_t) priv;

  nand_mark_alloc (&npt->parent, page);
}

#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */

/**********************************************************************
 * Page table cache.  These routines buffer, and cache lookups to page
 * table entries.  [is_to_write] is used to indicate if this page will
 * soon be written to another page (it should be marked sticky).
 *
 * Currently, this is rather hard-coded to two pages, but the API shouldn't
 * change if we use more pages. */

/* Initialize the pcache. */
static void
pcache_init (fs_ptable_nand_t npt)
{
  int i, j;

  npt->pcache_recent = -1;
  npt->pcache_sticky = -1;

  for (i = 0; i < FS_MAX_CACHED_PTABLE_PAGES; i++)
    npt->pcache_page[i] = FS_INVALID_PAGE_ID;

  npt->pcache_alias_page = FS_INVALID_PAGE_ID;

  for (i = 0; i < FS_MAX_CACHED_PTABLE_PAGES; i++)
    for (j = 0; j < FS_PTE_PER_PAGE; j++)
      npt->pcache_data[i][j] = FS_INVALID_PAGE_ID;
}

/* Lookup a page table page (by physical address) and return a buffer to
 * it.  The data is read in if necessary.  If [is_to_write] is true, then
 * it is valid for [page] to be FS_INVALID_PAGE_ID, in which case the page is
 * simply filled with all FF.
 */
static uint32 *
pcache_lookup (fs_ptable_nand_t npt, fs_page_id page, int is_to_write)
{
  int use = -1;
  int i;
  int doread = 0;

  /* It is not permissible to stick more than one page. */
  ASSERT (!is_to_write || npt->pcache_sticky < 0);

  /* Determine if we already have the data in question. */
  for (i = 0; i < FS_MAX_CACHED_PTABLE_PAGES; i++)
  {
    if ((page != FS_INVALID_PAGE_ID) && (npt->pcache_page[i] == page))
    {
      use = i;
      break;
    }
  }

  if (use < 0)
  {
    if ((page != FS_INVALID_PAGE_ID) && npt->pcache_alias_page == page)
    {
      use = npt->pcache_sticky;
    }
    else
    {
      use = (npt->pcache_recent + 1) % FS_MAX_CACHED_PTABLE_PAGES;

      if (npt->pcache_sticky >= 0 && use == npt->pcache_sticky)
        use = (use + 1) % FS_MAX_CACHED_PTABLE_PAGES;

      npt->pcache_recent = use;
      doread = 1;
    }
  }

  if (doread)
    fs_flash_pcache_miss_counter++;
  else if (use)
    fs_flash_pcache_hit_counter++;

  npt->pcache_page[use] = page;
  if (is_to_write) {
    npt->pcache_sticky = use;
  }

  if (doread) {
    if (page == FS_INVALID_PAGE_ID) {
      for (i = 0; i < FS_PTE_PER_PAGE; i++)
        npt->pcache_data[use][i] = FS_INVALID_PAGE_ID;
    } else {
      fs_device_read_page (npt->parent.dev, page,
          npt->pcache_data[use],
          FS_FOP_PTABLE, FS_FLASH_MUST_WORK);
    }
  }

  return npt->pcache_data[use];
}

static void
pcache_update (fs_ptable_nand_t npt, fs_page_id old_page, fs_page_id new_page)
{
  int i;

  /* There is nothing to update if old_page is invalid */
  if (old_page != FS_INVALID_PAGE_ID)
  {
    /* By this time, GC has already moved the old_page to new_page. Pcache
       needs updation to ensure that there are no stale copies */
    for (i = 0; i < FS_MAX_CACHED_PTABLE_PAGES; i++)
      if ((old_page == npt->pcache_page[i]))
      {
        npt->pcache_page[i] = new_page;
        break;
      }
  }
}

static int
pcache_write (fs_ptable_nand_t npt, fs_page_id old_page, fs_page_id new_page)
{
  int use;
  int result;

  use = npt->pcache_sticky;
  ASSERT (use >= 0);
  ASSERT (old_page == npt->pcache_page[use]);

  result = fs_device_write_page (npt->parent.dev, new_page,
      npt->pcache_data[use], FS_FOP_PTABLE);

  if (result != FS_DEVICE_OK)
    return -1;

  npt->pcache_page[use] = new_page;
  npt->pcache_alias_page = old_page;
  return 0;
}

static void
pcache_unstick (fs_ptable_nand_t npt)
{
  npt->pcache_sticky = -1;
  npt->pcache_alias_page = FS_INVALID_PAGE_ID;
}

#endif /* FEATURE_EFS_EFS2_ON_NAND */
#endif /* FEATURE_EFS_QEFS2 */

