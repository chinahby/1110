/***********************************************************************
 * fs_counters.h
 *
 * This file holds the counters to track flash operations.
 * Copyright (C) 2008 - 2009 QUALCOMM, Inc.
 *
 * This file exposes the counters to track the read/write/erase operations
 * on the flash. The actual instance is provided by the fs_benchmark.
 * So both EFS2 and FDI can update the counters whenever they do
 * flash operations.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_counters.h#2 $ $DateTime: 2009/05/18 17:19:57 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-08   ebb   Renamed hotplug counters to blockdev counters.
2008-12-15   yg    Move the header file to internal src folder from public.
2008-12-12   rp    Move counters from fs_device to this rightful place.
2008-05-23   rp    Create

===========================================================================*/

#ifndef __FS_COUNTERS_H__
#define __FS_COUNTERS_H__


#include "comdef.h"
#include "fs_config.h"


/* Accessing the benchmark counters. */
#define fs_flash_write_page_counter fs_counter.flash_write_page_counter
#define fs_flash_read_page_counter fs_counter.flash_read_page_counter
#define fs_flash_erase_block_counter fs_counter.flash_erase_block_counter



/* Type of flash operations. */
enum fs_flashop
{
  FS_FOP_READ,
  FS_FOP_WRITE,
  FS_FOP_ERASE,
  FS_NUM_FOP_OPS
};

/* Types of flash categorizations. */
enum fs_flashop_category
{
  FS_FOP_SUPER,
  FS_FOP_USER,
  FS_FOP_LOG,
  FS_FOP_DATA_GC_MOVE,
  FS_FOP_DATA_GC_RECOVERY,
  FS_FOP_USER_RECOVERY,
  FS_FOP_PTABLE_GC_MOVE,
  FS_FOP_PTABLE,
  FS_FOP_FACTORY_RECOVERY,
  FS_FOP_UNKNOWN,
  FS_FOP_BIRDVIEW,
  NUM_FS_FLASHOP_CATEGORIES
};

/* Types of buffer operations. */
enum fs_bufop_category {
  FS_BOP_DB,
  FS_BOP_FS,
  FS_BOP_ALLOC_TREE,
  FS_BOP_ALLOC_MAP,
  FS_BOP_UNKNOWN,
  NUM_FS_BUFOP_CATEGORIES
};

/* Read, Write, Erase Counters */
struct fs_benchmark_counters
{
  uint32 flash_write_page_counter;
  uint32 flash_write_partial_counter;
  uint32 flash_read_page_counter;
  uint32 flash_erase_block_counter;
  uint32 flash_erase_suspend_counter;
  uint32 blockdev_read_sector_counter;
  uint32 blockdev_write_sector_counter;
  uint32 blockdev_reliable_write_sector_counter;
};

/* There exists one global counter set that is updated by EFS2 & Drivers. */
extern struct fs_benchmark_counters fs_counter;

extern uint32 fs_flash_pcache_miss_counter;
extern uint32 fs_flash_pcache_hit_counter;
extern uint32 fs_flash_buffer_miss_counter;
extern uint32 fs_flash_buffer_hit_counter;
extern uint32 fs_flash_dealloc_counter;
extern uint32 fs_flash_bird_view_miss_counter;
extern uint32 fs_flash_bird_view_hit_counter;


void fs_counters_update (enum fs_flashop op, enum fs_flashop_category category,
                         uint32 count);

void fs_device_buffer_fill (enum fs_bufop_category category);


static inline void
counters_success_stub (enum fs_flashop_category category, uint32 count)
{
  (void) category; (void) count;
}


#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */





#endif /* not __FS_COUNTERS_H__ */
