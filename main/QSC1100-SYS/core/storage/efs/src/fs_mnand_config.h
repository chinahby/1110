/***********************************************************************
 * fs_mnand_config.h
 *
 * Config file for mnand.
 * Copyright (C) 2008 QUALCOMM, Inc.
 *
 * Configuration file for mnand.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_mnand_config.h#1 $ $DateTime: 2008/12/12 11:11:10 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-12   rp    Create

===========================================================================*/

#ifndef __FS_MNAND_CONFIG_H__
#define __FS_MNAND_CONFIG_H__

#include "fs_config.h"
#include "fs_efs2_cust.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND

/** How big is a sector and cluster in Bytes */
#define FS_MNAND_SECTOR_SIZE              (FS_SDCC_SECTOR_SIZE_IN_BYTES)
#ifndef FS_MNAND_CLUSTER_SIZE
  #define FS_MNAND_CLUSTER_SIZE           (EFS_PAGE_SIZE)
#endif

/** How many sectors fit in one cluster */
#define FS_MNAND_SECTORS_PER_CLUSTER      (FS_MNAND_CLUSTER_SIZE / \
                                              FS_MNAND_SECTOR_SIZE)

/** How big is a logical block */
#ifndef FS_MNAND_CLUSTERS_PER_BLOCK
  #define FS_MNAND_CLUSTERS_PER_BLOCK     (64)
#endif

/** How many Page-Table entries fit in one sector */
#define FS_MNAND_PTBL_ENTRIES_PER_SECT    (FS_MNAND_SECTOR_SIZE/4)

/** How many Bird-View entries fit in one sector, 1 BV entry is 2 bits. */
#define FS_MNAND_BIRD_VIEW_ENTRIES_SIZE_IN_BITS  (2)
#define FS_MNAND_BIRD_VIEW_ENTRIES_PER_SECT      ( (FS_MNAND_SECTOR_SIZE*8) \
                                / FS_MNAND_BIRD_VIEW_ENTRIES_SIZE_IN_BITS )

/** Reserved region [ also contains superblock. ] */
#ifndef FS_MNAND_RESERVED_TOTAL_SECTORS
  #define FS_MNAND_RESERVED_TOTAL_SECTORS         (16)
#endif
#define FS_MNAND_SUPER_SECTOR_ID                  (0)
#ifndef FS_MNAND_SUPER_BLOCK_SIZE
  #define FS_MNAND_SUPER_BLOCK_SIZE               (FS_MNAND_SECTOR_SIZE*1)
#endif
#define FS_MNAND_SUPER_BLOCK_SECTORS_PER_PAGE     (FS_MNAND_SUPER_BLOCK_SIZE  \
                                                        /FS_MNAND_SECTOR_SIZE)


/** Configure log region, */
#ifndef FS_MNAND_LOG_TOTAL_SECTORS
  #define FS_MNAND_LOG_TOTAL_SECTORS      (128)
#endif
#ifndef FS_MNAND_LOG_PAGE_SIZE
  #define FS_MNAND_LOG_PAGE_SIZE          (FS_MNAND_SECTOR_SIZE*1)
#endif
#define FS_MNAND_LOG_SECTORS_PER_PAGE     (FS_MNAND_LOG_PAGE_SIZE   \
                                              /FS_MNAND_SECTOR_SIZE)


/** How much space to reserve as PM overhead in data region */
#ifndef FS_MNAND_OVERHEAD_PERCENT
  #define FS_MNAND_OVERHEAD_PERCENT       (3)
#endif

/* Configure Log-Queue. How many max written-logs in the Log-Q */
#define FS_MNAND_LOG_WRITTEN_LOGS         (20)
#define FS_MNAND_LOG_ALLOCATED_LOGS       (10)
#define FS_MNAND_LOG_QUEUE_SIZE           (FS_MNAND_LOG_WRITTEN_LOGS + \
                                           FS_MNAND_LOG_ALLOCATED_LOGS + 10)

#endif /* FEATURE_EFS_EFS2_ON_MNAND */

#endif /* not __FS_MNAND_CONFIG_H__ */
