/***********************************************************************
 * fs_device_efs_flash.h
 *
 * Device layer abstration for EFS on flash device.
 * Copyright (C) 2008 QUALCOMM, Inc.
 *
 * Device layer abstration for EFS on flash device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_device_efs_flash.h#1 $ $DateTime: 2008/12/12 09:42:18 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-11-03   rp    Create

===========================================================================*/

#ifndef __FS_DEVICE_EFS_FLASH_H__
#define __FS_DEVICE_EFS_FLASH_H__

#include "fs_config.h"
#include "fs_device.h"
#include "fs_sys_types.h"

#if defined (FEATURE_EFS_EFS2_ON_NAND) ||     \
    defined (FEATURE_EFS_EFS2_ON_NOR)

extern void fs_device_efs_flash_init (void);
extern efs_device_t fs_device_efs_flash_open (uint32 which_device);
extern int fs_device_efs_flash_close (efs_device_t base_dev);
extern void fs_device_efs_flash_init_ops (efs_device_t base_dev);

#else /* (FEATURE_EFS_EFS2_ON_NAND) || FEATURE_EFS_EFS2_ON_NOR) */

static inline void
fs_device_efs_flash_init (void)
{
}

static inline efs_device_t
fs_device_efs_flash_open (uint32 which_device)
{
  (void) which_device;
  return NULL;
}

static inline int
fs_device_efs_flash_close (efs_device_t base_dev)
{
  (void) base_dev;
  return -1;
}

static inline void
fs_device_efs_flash_init_ops (efs_device_t base_dev)
{
  (void) base_dev;
}

#endif /* (FEATURE_EFS_EFS2_ON_NAND) || FEATURE_EFS_EFS2_ON_NOR) */


#endif /* not __FS_DEVICE_EFS_FLASH_H__ */
