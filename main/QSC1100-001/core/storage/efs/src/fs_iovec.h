/***********************************************************************
 * fs_iovec.h
 *
 * I/O-Vec data type.
 * Copyright (C) 2008 QUALCOMM, Inc.
 *
 * I/O-Vec data type.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_iovec.h#1 $ $DateTime: 2008/12/12 11:11:10 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-12   rp    Create

===========================================================================*/

#ifndef __FS_IOVEC_H__
#define __FS_IOVEC_H__

#include "fs_efs2_cust.h"

#include "comdef.h"
#include "fs_device_types.h"

/* same as FS_DATA_TAG_DATA */
#define FS_IOVEC_DATA_TYPE_PERMENANT_DATA           (0)

struct fs_iovec
{
  uint32                      *buff_base;
  fs_cluster_id               cluster_id;
  fs_sector_id                sector_id;
  uint16                      data_type;
};
typedef struct fs_iovec* fs_iovec_t;



#endif /* not __FS_IOVEC_H__ */
