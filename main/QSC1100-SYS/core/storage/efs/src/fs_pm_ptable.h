/**********************************************************************
 * fs_pm_ptable.h
 *
 * Copyright (C) 2002-2008 Qualcomm, Inc.
 * Page table management.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_pm_ptable.h#2 $ $DateTime: 2008/12/12 09:42:18 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2008-11-03     rp     Introduce device abstraction.
2007-10-18     rp     Featurized QEFS2.
2007-08-10     rp     Align buffers passed to flash on a 32-byte boundary.
2007-07-25    umr     Added support for 4k Page size.
2007-07-17    sch     Increased pcache size from 2 to 6
2007-07-06     yg     Added tags to reverse ptable functions.
2006-06-26     yg     Memory reduction effort
2006-01-24    dlb     Add startup state variable.
2006-01-17    dlb     Support multiple NOR write styles.
2005-12-14    dlb     Add midpoint startup callback.
2005-10-12    dlb     Fix some ISO C warnings.
2005-06-29    dlb     Allow NOR flash max to be defined externally.
2005-04-26    dlb     Add 2K page support.
2005-01-04    dlb     Update copyright line.
2004-12-30    dlb     Remove excess infiltration of factory image code.
2004-10-15    dlb     Update copyright header.
2003-08-11    dlb     Cache ptable pages.
2003-06-12    adm     Add support for factory start.
2003-04-22     gr     Renamed from fs_pm_ptable_nor.h.
2003-01-24    bgc     Added log entry type FS_LOG_ENTRY_GC_DEALLOC.
2002-08-08    drh     Created by dlb.  Added history header.

===========================================================================*/


#ifndef __FS_PM_PTABLE_H__
#define __FS_PM_PTABLE_H__

#include "fs_efs2_cust.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)

#include "fs_pm_super.h"
#include "fs_journal.h"
#include "fs_pm_log.h"
#include "fs_pm_gc.h"
#include "fs_pm_types.h"
#include "fs_logr.h"

#ifdef __cplusplus
extern "C" {
#endif

struct fs_ptable_data;
typedef struct fs_ptable_data *fs_ptable_t;

struct fs_ptable_data {
  /* Lookups into the table. */
  fs_page_id (*get_forward) (fs_ptable_t pt, fs_cluster_id cluster);
  fs_cluster_id (*get_reverse) (fs_ptable_t pt, fs_page_id page,
      uint32 *tag_ptr);

  /* Setting values in the table. */
  void (*set_forward) (fs_ptable_t pt, fs_cluster_id cluster,
      fs_page_id page);
  void (*set_reverse) (fs_ptable_t pt, fs_page_id page,
                       fs_page_id state, uint32 tag);

  /* Mark the given block as having been erased, returns the number of data
   * pages made available by this erase.  This is called by the garbage
   * collector after erasing each block. */
  int (*erase) (fs_ptable_t pt, fs_block_id block);
  /* Mark a block as garbage until the erase completes */
  void (*begin_erase) (fs_ptable_t pt, fs_block_id block);

  /* Mark that the given page has been allocated. */
  void (*mark_alloc) (fs_ptable_t pt, fs_page_id page);

  /* Flush out the ptable.  Only part of NOR tabler. */
  void (*flush) (fs_ptable_t pt);

  /* Do one iteration of GC check at the ptable level.  Returns non-zero if
   * some amount of work was done. */
  int (*do_gc) (fs_ptable_t pt);

  /* Iterate through the known log pages that we found. */
  void (*log_iterate) (fs_ptable_t pt, fs_log_t log,
      void (*visit) (void *priv, fs_log_code_t code, uint32 *args),
      void *priv);

  /* Called after logs have been processed, and before scanning for erased
   * spans. */
  void (*mid_init) (fs_ptable_t pt);

  /* Called after the logs have been processed to finish up any
   * initialization. */
  void (*post_init) (fs_ptable_t pt);

  /* Update the values in the superblock as necessary. */
  void (*super_update) (fs_ptable_t pt);

  /* Data shared by all ptables implementations. */
  struct fs_super_data  super;
  efs_device_t          dev;
  fs_logr_t             logr;
};

/* Initialize.  This will be started appropriately as below. */
void
fs_ptable_init (
    fs_ptable_t         pt,
    efs_device_t        dev,
    fs_log_t            log,
    int                *fresh_start,
    fs_logr_t           logr);

/* NAND specific iteration function. */
void
fs_ptable_nand_log_visit (fs_ptable_t pt, fs_log_code_t code, uint32 *args);

#ifdef FS_UNIT_TEST
#error code not present
#endif

/**********************************************************************
 * NOR Specific code. */

/* Maximum number of reverse table entries kept between flushes. */
#if EFS_PAGE_SIZE==512
  #define FS_MAX_PTABLE_RCACHE    328
#elif EFS_PAGE_SIZE==2048
  #define FS_MAX_PTABLE_RCACHE    1312
#elif EFS_PAGE_SIZE==4096
  #define FS_MAX_PTABLE_RCACHE    2624
#else
  #error "Unsupported page size"
#endif

/* Maximum number of pages supported.  This constant determines the largest
 * flash device supported by the NOR support.  The code requires 4 bytes of
 * RAM for each page of flash.  This amounts to 64K for an 8MB flash.  If
 * the flash is larger than this, it is probably best to use the NAND
 * manager (even on large NOR flash). */
#ifndef FS_MAX_NOR_PAGES
  #ifdef FEATURE_EFS_SUPPORT_LARGE_NOR
    #define FS_MAX_NOR_PAGES       32768
  #else
    #define FS_MAX_NOR_PAGES       16384
  #endif
#endif

struct fs_ptable_rcache_entry {
  fs_page_id       page;
  fs_page_id       state;
};

struct fs_ptable_rcache {
  int           count;
  struct fs_ptable_rcache_entry
                data[FS_MAX_PTABLE_RCACHE];
};

struct fs_ptable_data_nor {
  struct fs_ptable_data parent;

#ifdef FEATURE_EFS_EFS2_ON_NOR
  struct fs_ptable_rcache rcache;
  fs_page_id              ptable[FS_MAX_NOR_PAGES];
#endif

  /* Computed speedups for this particular flash device. */
  fs_page_id               minor_mask;
  fs_page_id               major_shift;
  fs_page_id               reserved_offset;

  /* Specific write function. */
  void (*ptable_write_entry) (struct fs_ptable_data_nor *npt, fs_page_id page,
      fs_page_id state, int ignore_error);

  /* Indicates we are still in startup. */
  unsigned              in_startup_scan;
};
typedef struct fs_ptable_data_nor *fs_ptable_nor_t;

/**********************************************************************
 * NAND Specific code. */

#ifdef FS_UNIT_TEST
#error code not present
#endif

/* Maximum number of page table levels.  5 is sufficient for 512 byte pages
 * and 32 bit page_ids. */
#define FS_MAX_LEVELS   5

/* Total number of page-table pages cached in memory. */
#define FS_MAX_CACHED_PTABLE_PAGES 6

struct fs_ptable_data_nand {
  struct fs_ptable_data parent;

  fs_log_t      log;

#ifdef FS_DEBUG_TABLE_CHECK
#error code not present
#endif

  /* Journal for the forward tables.  Perhaps this will change to include
   * other journal things later... */
  struct fs_journal_data journal;

  /* The last level.  Just super.data.page_depth-1, but we use it a lot. */
  int           last_level;

  /* Shifts and masks for each page table level. */
  uint8         shifts[FS_MAX_LEVELS];
  uint32        masks[FS_MAX_LEVELS];
  uint32        mask_off[FS_MAX_LEVELS];

  /* Garbage collector for ptable. */
  struct fs_gc_data ptable_gc;

  /* Ptable cache. */
  uint32        *pcache_data[FS_MAX_CACHED_PTABLE_PAGES];
  int16         pcache_recent;
  int16         pcache_sticky;
  fs_page_id    pcache_page[FS_MAX_CACHED_PTABLE_PAGES];
  fs_page_id    pcache_alias_page;

  /* Has logging started? */
  int           logging;

#ifdef FS_DEBUG_JOURNAL_CHECK
#error code not present
#endif
};
typedef struct fs_ptable_data_nand *fs_ptable_nand_t;

/* This union combines both of the above as a simple way to combine
 * both types. */
union fs_ptable_combined_data {
  struct fs_ptable_data_nor    nor;
  struct fs_ptable_data_nand   nand;
};

#ifdef __cplusplus
}
#endif

#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) */
#endif /* FEATURE_EFS_QEFS2 */

#endif /* __FS_PM_PTABLE_H__ */
