/**********************************************************************
 * fs_pm.h
 *
 * Copyright (C) 2003-2008 Qualcomm, Inc.
 * Page Manager Header for EFS.
 */


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_pm.h#6 $ $DateTime: 2009/01/29 17:05:27 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
12/24/08      yg      Fix potentially overwriting a page after a bad pwrdown.
12/12/08      rp      Introducing i/o-vec interface.
12/02/08      yg      Move bad block % config to fs_config.h
11/03/08      rp      Introduce device abstraction.
10/06/08      np      Added shred functionality to support efs_shred API.
12/05/07      rp      Moved FS_PARANOIA_CHECKS feature to fs_paranoia.c
12/10/07      yg      Update some comments and rename allocators.
10/18/07      rp      Featurized QEFS2.
10/08/07      yg      Change for dynamic threshold configurations.
08/24/07      yg      Config Soft limit values for GC Overhead writes.
08/10/07      rp      Align buffers passed to flash on a 32-byte boundary.
07/06/07      yg      Added new allocator to allocate meta data.
06/29/07      sch     Passed data tag to page_write
06/27/07      yg      Added struct definition for holding erased blocks.
03/06/07      dlb     Query callback about allocated counts.
11/09/06      dlb     Add pm query of freemap.
10/30/05      sh      Lint cleanup.
04/26/05      dlb     Add 2K page support.
10/15/04      dlb     Update copyright line.
10/07/04      dlb     Whitespace cleanup.
04/04/03      bgc     Added incremental garbage collection data and types.
02/13/03      dlb     Created based on fs_pm_gc.h.

===========================================================================*/

#ifndef __FS_PM_H__
#define __FS_PM_H__

#include "fs_efs2_cust.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2

#include "fs_device.h"
#include "fs_pm_ptable.h"
#include "fs_logr.h"
#include "qw.h"
#include "fs_iovec.h"

struct fs_pm_data;
struct fs_pm_flash_data;
typedef struct fs_pm_data *fs_pm_t;
typedef struct fs_pm_flash_data *fs_pm_flash_t;
typedef struct fs_pm_ops* fs_pm_ops_t;


/* Page manager operations. */
struct fs_pm_ops
{
  /* Write a single page of data. */
  void (*page_write) (fs_pm_t gc, fs_cluster_id cluster,
                     void *buffer, uint16 tag);

  /* Read a single page of data.  If the page has never been written, fill
   * the buffer with all 0xFF. */
  void (*page_read) (fs_pm_t gc, fs_cluster_id cluster, void *buffer);

  /* Store a uint32 for an upper layer.  The number of these allowed is
   * defined by FS_UPPER_DATA_COUNT.  These always have as an initial value
   * (on powerup) of FS_INVALID_PAGE_ID.  This data will be managed in the
   * context of transactions. */
  void (*store_info) (fs_pm_t, unsigned offset, uint32 data);

  /* Retrieve the value previously stored. */
  uint32 (*get_info) (fs_pm_t, unsigned offset);

  /* Start and end transactions.  This should _NOT_ be nested. */
  void (*start_transaction) (fs_pm_t gc);
  void (*end_transaction) (fs_pm_t gc);

  /* Compute the space limit for this particular device. */
  uint32 (*space_limit) (fs_pm_t gc);

  /* Register the freespace query function.  The garbage collector will be
   * much more efficient if it can query for a given cluster as to whether
   * or not it is still in use.  If the user of the page manager registers
   * this function, it will be called periodically by the garbage
   * collector.  This function must not make any calls into the page
   * manager (i.e. it must not read or write any data).  The function
   * should return true if the cluster is still used. */
  void (*register_free_check_cb) (fs_pm_t gc,
      int (*is_used_cb) (void *, fs_cluster_id cluster), void *data);

  /* Register the allocation query function.  The garbage collector may
   * call this to determine the number of clusters that have been
   * allocated. */
  void (*register_alloc_count_cb) (fs_pm_t gc,
      uint32 (*alloc_count_cb) (void *), void *data);

  /* Destroy or erase obsolete information from the device. */
  int (*shred) (fs_pm_t gc);

  void (*read_iovec) (fs_pm_t pm_data, fs_iovec_t iovec, uint32 iovec_cnt);
  void (*write_iovec) (fs_pm_t pm_data, fs_iovec_t iovec, uint32 iovec_cnt);
};


struct fs_pm_data {
  struct fs_pm_ops      ops;
};


#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)

/* Initialze the Garbage collector.  Calls initialize on all of the page
 * table code. */
void fs_pm_init (fs_pm_flash_t gc, efs_device_t dev);


/* Flush enough that now would be a good time to shutdown. */
void fs_pm_shutdown_flush (fs_pm_t gc);

#ifdef FS_UNIT_TEST
#error code not present
#endif

/* Let Paranoia know the list of internal data-structures that should
   be watched. */
void fs_ptable_register_with_paranoia (void);

#define NUM_REGIONS     4

#define FS_PM_SUPER_CHAIN_SIZE          64

enum gc_state {
  FS_PM_HEAD, FS_PM_MOVING, FS_PM_ERASE
};


enum gc_erase_state {
  FS_PM_ERASE_IDLE,     /* flash device is not doing an erase */
  FS_PM_ERASE_BUSY,     /* flash device was left erasing a block */
  FS_PM_ERASE_SUSPEND   /* flash device has a pending erase to finish */
};


typedef enum {
  SHRED_IDLE,           /* Indicates shred is idle. */
  SHRED_RUNNING         /* Indicates shred is running. */
} shred_state_type;

struct fs_pm_flash_data {
  struct fs_pm_data     parent;

  fs_ptable_t           ptable;
  efs_device_t          dev;
  struct fs_log_data    log;

  /* Liquid data allocator can be used as allocator the data that
   * changes a lot and because of this moves around. Meta data in EFS
   * changes a lot so this allocator will be used to write meta data which
   * is represented using non zero tags */
  fs_page_id               liquid_data_alloc;

  /* Solid data allocator can be used as allocator for the data that
   * doesn't change much once written, and hence will not move much from
   * a place. User data in a file doesn't change much so this allocator
   * will be used to write user data which is represented with tag 0 */
  fs_page_id               solid_data_alloc;

  /* These 2 variables track the last page writes that are logged. This
   * is needed since, when we write last page we assign INVALID_PAGE_ID
   * to the allocator and we lose the info of where was it last. When
   * PM initializes after a powercycle this information is critical since
   * the erased block search algorithm relies on the allocators position
   * to start searching for the erased blocks. If one of the allocators
   * that is near the GC has a valid value and the far allocator has
   * invalid value, then the search algorithm doesn't queue the erased
   * blocks that are behind the nearest allocator. So if a bad powerdown
   * happened and there were some page writes happened using the far
   * allocator then those will cause write failures, since we would be
   * overwriting written pages.
   * Now by having last written page info, we can include all the blocks
   * that are considered erased and mark them appropriately garbage if
   * bad powerdown happened. */
  fs_page_id               known_good_solid_alloc;
  fs_page_id               known_good_liquid_alloc;

  /* Which page will be garbage collected next */
  fs_page_id               gc_next;

  /* Count of how many physical erased pages are there */
  fs_page_id               free_count;

  /* Overhead write count per transaction. We can set this
   * to zero on transaction start and then control GC, so that
   * the page write count will be within some specified config
   * limits. */
  uint32                overhead_write_count;

  /* Variables needed for counting the number of bad blocks in
   * the region. Accumulator will go on accumulating the count
   * when the wrap around happens, then count will hold the
   * bad block count and accumulator is reset. */
  fs_page_id       last_reference;  /* Last reference point of accumulation */
  uint16        bad_blk_count;   /* Bad block counter */
  uint16        bad_block_acc;   /* Bad block accumulator */

  /* Thresholds for GC. Now there will not be any compile time
   * configurations, since they cannot cover various EFS partition
   * sizes. So appropriate thresholds will be determined runtime
   * based on the partition / region size. */
  uint16        max_bad_blocks_allowed;   /* Maximum allowed bad blocks */
  uint16        gc_min_threshold;         /* GC Urgency Minimum threshold */
  uint16        gc_max_threshold;         /* GC Urgency Maximum threshold */

  enum gc_state         state;

  fs_page_id               next_super_page;

  fs_page_id               super_chain[FS_PM_SUPER_CHAIN_SIZE];
  int                      super_head, super_tail;

  /* Callback to query for usage (and associated data). */
  int (*is_used_cb) (void *data, fs_cluster_id cluster);
  void *is_used_data;

  /* Callback to query for total clusters allocated. */
  uint32 (*alloc_count_cb) (void *data);
  void *alloc_count_data;

  /* Buffer to hold the space to reallocate pages.
   * Note: Must be aligned for DM use. */
  uint8                 *realloc_buffer;

  /* Indicates we are inside of a transaction, and shouldn't be performing
   * GC. */
  int                   inside_xact;

  /* Boundaries of each of the regions. */
  fs_page_id            min_page;
  fs_page_id            max_page;

  /* Log region handling.  (NAND) */
  struct fs_logr_data   logr;

  /* Shred state to indicate whether shred is running or idle. */
  shred_state_type shred_state;

  /* When gc->gc_next reaches shred_stop, shred is complete. When
   * no shredding is done, shred_stop is set to FS_INVALID_PAGE_ID. */
  fs_page_id shred_stop;

  /* Incremental garbage collector state variables *
   *                                               */
  /* free count stored at a transaction start */
  fs_page_id trans_start_free_count;
  /* The current state of the flash regarding erases */
  enum gc_erase_state erase_state;
  /* current erase block only valid during an erase */
  fs_block_id gc_current_erase_block;
  /* unfinished work holds incremental work left to be done */
  int unfinished_work;
  /* erase_factor is a variable used to tune the amount
   * of work a page erase is compared to a page move.
   * It is fixed point *10, so erase_factor=125 means
   * 12.5 page moves = 1 page erase. */
  int erase_factor;
  /* erase_time is the best guess time of a block erase.
   * erase_countdown is used to measure erases as they
   * progress. */
  int erase_time;  /* in millisec */
  int erase_countdown;  /* in millisecs */
  qword erase_start; /* in time of day millisecs */
};

/* This circular array holds the erased blocks, so the searching
 * can be avoided while allocating. */
#define MAX_ERASED_BLOCK_ARRAY_SIZE   16

typedef struct
{
  uint32 erased_blk[MAX_ERASED_BLOCK_ARRAY_SIZE]; /* Erased block pages */
  uint16 enqueue_pos;
  uint16 dequeue_pos;
  uint16 item_count;
}
erased_blk_array_type;

/* These configuration settings play an important role in determining
 * the throughput response of the EFS. So cautions should be taken
 * while modifying the value. */

/* Optimal setting for FS_PM_GC_OVERHEAD_WRITES_PER_TXN is 20. This
 * config defines how many page writes of overhead a transaction can
 * have. But this setting is *NOT* a hard limit since if there is not
 * enough free space in EFS and GC is overloaded to move huge amount
 * of data because of fragmentation, this limit is ignored as far as
 * creating enough free space so that EFS does not encounter the fatal
 * error. So configuring this number to a smaller value would worsen
 * the situation than helping. Since if this number is small, then a
 * lot of dynamic data and old generation static data (even in dynamic
 * region, old generation data would not change much) would mixup
 * leading to a situation of need to GC the old dynamic data over and
 * over since full block worth of non changing data will not come together
 * which would help since that block need not be GC'd because it doesn't
 * contain any garbage at all. So the GC would do more GC if this setting
 * is less than block size, compared to if equal to block size. But in
 * that case the overhead per transaction would increase.*/
#define FS_PM_GC_OVERHEAD_WRITES_PER_TXN   20


/*
 * Configuration settings for GC in PTable region.
 *
 * FS_PT_GC_OVERHEAD_WRITES_PER_TXN determines how many overhead page writes
 * can be there in Ptable region GC. Since PTable region can sustain less
 * free space because of journal, critical free space can be quite low in
 * terms of pages. But the normal preferred free space would be few blocks.
 *
 * Again the same restrictions as data region GC settings also apply here,
 * that, too lower overhead would cause more GC operation because of
 * interleaving of older generation pages with new generation pages which
 * might change again. Since if all older generation pages endup in single
 * block, GC can be avoided on that block altogether, not needing to move
 * those pages from GC.
 */
#define FS_PT_GC_OVERHEAD_WRITES_PER_TXN       4

/* This is *NOT* customer configurable. Do not change this.
 * We need minimum of 3 blocks since we have 2 pointers in PM
 * data allocator and meta data allocator, we need an extra
 * block if any block goes bad we should still be able to do GC to get
 * back more free space if needed. */
#define FS_PM_MINIMUM_FREE_BLKS_FOR_GC     3

#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) */
#endif /* FEATURE_EFS_QEFS2 */

#endif /* not __FS_PM_H__ */
