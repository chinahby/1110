/***********************************************************************
 * fs_device_types.h
 *
 * Device layer data types.
 * Copyright (C) 2008 QUALCOMM, Inc.
 *
 * Device layer data types.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_device_types.h#2 $ $DateTime: 2008/12/12 11:11:10 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-12   rp    Introducing Managed Nand.
2008-11-03   rp    Create

===========================================================================*/

#ifndef __FS_DEVICE_TYPES_H__
#define __FS_DEVICE_TYPES_H__

#include "fs_efs2_cust.h"
#include "comdef.h"

typedef uint32 fs_page_id;
typedef uint32 fs_block_id;
typedef uint32 fs_cluster_id;
typedef uint32 fs_sector_id;

#define FS_INVALID_PAGE_ID      0xFFFFFFFFU
#define FS_INVALID_BLOCK_ID     0xFFFFFFFFU
#define FS_INVALID_CLUSTER_ID   0xFFFFFFFFU
#define FS_INVALID_SECTOR_ID    0xFFFFFFFFU


enum fs_device_result
{
  FS_DEVICE_RESULT_SUCCESS = 0
};

enum fs_device_type
{
  FS_DEVICE_TYPE_UNKNOWN,
  FS_DEVICE_TYPE_EFS_NAND,
  FS_DEVICE_TYPE_EFS_ONE_NAND,
  FS_DEVICE_TYPE_EFS_NOR,
  FS_DEVICE_TYPE_FTL_NAND,
  FS_DEVICE_TYPE_EFS_MNAND
};



#endif /* not __FS_DEVICE_TYPES_H__ */
