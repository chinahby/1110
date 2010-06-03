/***********************************************************************
 * fs_counters.c
 *
 * This file holds the counters to track flash operations.
 * Copyright (C) 2008 QUALCOMM, Inc.
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

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_counters.c#1 $ $DateTime: 2008/12/12 11:11:10 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-12   rp   Create

===========================================================================*/

#include "fs_counters.h"
#include "assert.h"


#ifdef FEATURE_EFS_EFS2_ON_MNAND
  #define fs_update_mnand_read_counters     update_mnand_read_counters
  #define fs_update_mnand_write_counters    update_mnand_write_counters
#else
  #define fs_update_mnand_read_counters     counters_success_stub
  #define fs_update_mnand_write_counters    counters_success_stub
#endif


uint32 fs_flash_pcache_miss_counter = 0;
uint32 fs_flash_pcache_hit_counter = 0;
uint32 fs_flash_buffer_miss_counter = 0;
uint32 fs_flash_buffer_hit_counter = 0;
uint32 fs_flash_dealloc_counter = 0;
uint32 fs_flash_bird_view_miss_counter = 0;
uint32 fs_flash_bird_view_hit_counter = 0;



/* These variables maintain a count of flash device operations for
 * performance analysis.  The values can be inspected by JTAG or by
 * the optional code below. */
static uint32 read_color_counters[NUM_FS_FLASHOP_CATEGORIES] = { 0 };
static uint32 write_color_counters[NUM_FS_FLASHOP_CATEGORIES] = { 0 };
static uint32 erase_color_counters[NUM_FS_FLASHOP_CATEGORIES] = { 0 };
static uint32 buffer_fill_counters[NUM_FS_BUFOP_CATEGORIES] = { 0 };

/* The counters should always be updated, but there might not be any
 * code to reference (read) them.  They are still useful from within a
 * debugger.  We explain it to Lint this way: */
//lint -esym(551, write_color_counters)
//lint -esym(551, read_color_counters)
//lint -esym(551, erase_color_counters)
//lint -esym(551, buffer_fill_counters)
//lint -esym(551, fs_mnand_counters)



#ifdef FEATURE_EFS_EFS2_ON_MNAND

struct fs_counters_mnand
{
  uint32    mnand_super_read_counter;
  uint32    mnand_super_write_counter;
  uint32    mnand_log_read_counter;
  uint32    mnand_log_write_counter;
  uint32    mnand_pcache_read_counter;
  uint32    mnand_pcache_write_counter;
  uint32    mnand_bird_view_read_counter;
  uint32    mnand_bird_view_write_counter;
  uint32    mnand_data_read_counter;
  uint32    mnand_data_write_counter;
};

static struct fs_counters_mnand fs_mnand_counters = {0,0,0,0,0,0,0,0,0,0};

static void
update_mnand_read_counters (enum fs_flashop_category category, uint32 count)
{
  switch (category)
  {
    case FS_FOP_SUPER:
      fs_mnand_counters.mnand_super_read_counter += count;
      break;
    case FS_FOP_USER:
      fs_mnand_counters.mnand_data_read_counter += count;
      break;
    case FS_FOP_LOG:
      fs_mnand_counters.mnand_log_read_counter += count;
      break;
    case FS_FOP_PTABLE:
      fs_mnand_counters.mnand_pcache_read_counter += count;
      break;
    case FS_FOP_BIRDVIEW:
      fs_mnand_counters.mnand_bird_view_read_counter += count;
      break;
    default:
      break;
  }
}

static void
update_mnand_write_counters (enum fs_flashop_category category, uint32 count)
{
  switch (category)
  {
    case FS_FOP_SUPER:
      fs_mnand_counters.mnand_super_write_counter += count;
      break;
    case FS_FOP_USER:
      fs_mnand_counters.mnand_data_write_counter += count;
      break;
    case FS_FOP_LOG:
      fs_mnand_counters.mnand_log_write_counter += count;
      break;
    case FS_FOP_PTABLE:
      fs_mnand_counters.mnand_pcache_write_counter += count;
      break;
    case FS_FOP_BIRDVIEW:
      fs_mnand_counters.mnand_bird_view_write_counter += count;
      break;
    default:
      break;
  }
}
#endif /* FEATURE_EFS_EFS2_ON_MNAND */





static void
fs_update_read_counters (enum fs_flashop_category category, uint32 count)
{
  fs_flash_read_page_counter += count;
  read_color_counters[category] += count;

  fs_update_mnand_read_counters (category, count);
}

static void
fs_update_write_counters (enum fs_flashop_category category, uint32 count)
{
  fs_flash_write_page_counter += count;
  write_color_counters[category] += count;

  fs_update_mnand_write_counters (category, count);
}

static void
fs_update_erase_counters (enum fs_flashop_category category, uint32 count)
{
  fs_flash_erase_block_counter += count;
  erase_color_counters[category] += count;
}


void
fs_counters_update (enum fs_flashop op, enum fs_flashop_category category,
                    uint32 count)
{
  ASSERT (op < FS_NUM_FOP_OPS);
  ASSERT (category < NUM_FS_FLASHOP_CATEGORIES);

  switch (op)
  {
    case FS_FOP_READ:
      fs_update_read_counters (category, count);
      break;

    case FS_FOP_WRITE:
      fs_update_write_counters (category, count);
      break;

    case FS_FOP_ERASE:
      fs_update_erase_counters (category, count);
      break;

    default:
      ASSERT (0);
      break;
  }
}

void
fs_device_buffer_fill (enum fs_bufop_category category)
{
  ASSERT (category < NUM_FS_BUFOP_CATEGORIES);
  buffer_fill_counters[category]++;
}





#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */

