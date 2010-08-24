/**********************************************************************

 fs_pm_gc.h

 Garbage collector.

 Copyright (c) 2003-2005, 2007, 2008 by QUALCOMM Incorporated.
 All Rights Reserved.

***********************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_pm_gc.h#2 $ $DateTime: 2008/12/12 09:42:18 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2008-11-03    rp      Introduce device abstraction.
2008-03-24    sch     Added max_bad_blocks_allowed in fs_gc_data
2007-12-11    yg      Added some comments.
2007-10-18    rp      Featurized QEFS2.
2007-10-08    yg      Add bad block tracking variables.
2007-08-24    yg      Add variable to track GC overhead writes per txn.
2007-08-10    rp      Align buffers passed to flash on a 32-byte boundary.
2005-04-26    dlb     Add 2K page support.
2004-10-15    dlb     Update copyright line.
2004-10-07    dlb     Whitespace cleanup.
2003-03-07    dlb     Created file.
===========================================================================*/

#ifndef __FS_PM_GC_H__
#define __FS_PM_GC_H__

#include "fs_efs2_cust.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)

#include "fs_device.h"
#include "fs_pm_super.h"
#include "fs_pm_log.h"

struct fs_gc_data;
typedef struct fs_gc_data *fs_gc_t;

/* Initialize the garbage collector.  The following fields in 'gc' should
 * already be filled in: ...
 *   min_page
 *   max_page - The bounds of the region (max_page is 1 past the end).
 */
void
fs_gc_init (
    fs_gc_t             gc,
    int                 fresh_start
    );

/* Called after initialization is finished. */
void fs_gc_init_finish (fs_gc_t gc);

/* Do any urgent GC.  Returns true if anything needed to be done. */
int fs_gc_do_urgent (fs_gc_t gc);

/* Allocate a single page with this gc. */
fs_page_id fs_gc_alloc (fs_gc_t gc);

/* Visit a log node. */
void fs_gc_visit_node (fs_gc_t gc, fs_log_code_t code, uint32 *args);

enum fs_gc_state {
  FS_GC_HEAD, FS_GC_MOVING, FS_GC_ERASE
};

struct fs_gc_data {
  /* These fields must be set before calling fs_gc_init. */
  efs_device_t  dev;
  fs_page_id     min_page;
  fs_page_id     max_page;
  fs_super_t    super;
  fs_log_t      log;
  void         *priv;
  fs_cluster_id (*get_reverse) (void *priv, fs_page_id page);
  void (*set_reverse) (void *priv, fs_page_id page, fs_page_id state);
  int (*erase) (void *priv, fs_block_id block);
  void (*mark_alloc) (void *priv, fs_page_id page);
  void (*page_move) (void *priv, fs_page_id state, fs_page_id old_place,
      fs_page_id new_place);

  fs_page_id       alloc_next;
  fs_page_id       gc_next;
  fs_page_id       free_count;

  /* Overhead write count per transaction. We can set this
   * to zero on transaction start and then control GC, so that
   * the page write count will be within some specified config
   * limits. */
  uint32                overhead_write_count;

  /* Variables needed for counting the number of bad blocks in
   * the region. Accumulator will go on accumulating the count
   * when the wrap around happens, then count will hold the
   * bad block count and accumulator is reset. */
  fs_page_id    last_reference;  /* Last reference point of accumulation */
  uint16        bad_blk_count;   /* Bad block counter */
  uint16        bad_block_acc;   /* Bad block accumulator */
  uint16        max_bad_blocks_allowed;   /* Maximum allowed bad blocks */

  enum fs_gc_state state;

  uint8         *realloc_buffer;
};

#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) */
#endif /* FEATURE_EFS_QEFS2 */

#endif /* not __FS_PM_GC_H__ */
