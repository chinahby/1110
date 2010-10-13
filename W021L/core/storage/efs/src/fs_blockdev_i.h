/***********************************************************************
 * fs_blockdev_i.h
 *
 * Internal header containing private definitions for the block device
 * module.
 * Copyright (C) 2008 - 2009 QUALCOMM, Inc.
 *
 * This header contains all the private structure definitions for the
 * block device module. Nothing defined here should be accessible to
 * external clients. Also all logic concerning blockdev flags
 * ie: FS_BLOCKDEV_xxx should be in this file.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_blockdev_i.h#2 $ $DateTime: 2009/05/18 17:26:04 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-15   ebb   Updated open signature to take additional void * arg.
2009-05-08   ebb   Create

===========================================================================*/

#ifndef __FS_BLOCKDEV_I_H__
#define __FS_BLOCKDEV_I_H__

#include "customer.h"
#include "comdef.h"
#include "fs_iovec.h"
#include "fs_blockdev.h"

/* All the proper flags are set accoring to hoplug external features */
#include "fs_hotplug_i.h"

/* Ensure SD is supported if using CPRM */
#if defined(FS_BLOCKDEV_SD_CPRM) && !defined(FS_BLOCKDEV_SD)
  #error "Must enable FEATURE_EFS_BLOCKDEV_SD to use CPRM"
#endif


#define CANARY 0x0D00CAFE

/* Operations for each block device */
typedef struct
{
  int (*open)            (struct block_device *, void *);
  int (*close)           (struct block_device *);
  int (*read)            (struct block_device *, uint32, unsigned char *,
                          uint16);
  int (*write)           (struct block_device *, uint32, unsigned char *,
                          uint16);
  int (*uwrite)          (struct block_device *, uint32, unsigned char *,
                          uint16);
  int (*read_iovec)      (struct block_device *, fs_iovec_t, uint32);
  int (*write_iovec)     (struct block_device *, fs_iovec_t, uint32, int);
  int (*reliable_write)  (struct block_device *, uint32, uint32 *, uint32);
  int (*erase)           (struct block_device *, uint32, uint16);
  int (*reset)           (struct block_device *);
  int (*device_format)   (struct block_device *);
  int (*is_present)      (struct block_device *);
  int (*get_device_size) (struct block_device *, uint32 *, uint16 *);
  int (*get_device_info) (struct block_device *, struct block_device_info *);
  int (*set_device_idle) (struct block_device *);
  int (*get_device_idle) (struct block_device *);
} blockdev_ops;

/* Structure containing all device specific data */
struct block_device
{
  const char            *mount_point;   /* Mount point of this device */
  blockdev_ops          *ops;           /* Device operations */
  void                  *device_data;   /* Internal device specific data */
  struct block_device   *next;          /* Pointer to the next device */
  block_device_type      type;          /* Type of this block device */
  int8                   hidden;        /* Skip this device when iterating */
  uint16                 device_id;     /* Unique id to represent dev */
  uint32 canary;                        /* Verify struct integrity */
};

/* This contains driver specific information and functions */
typedef struct
{
  const char               *name;
  block_device_type        type;
  uint16                   version;
  void   (*init)           (struct block_device **, uint16 *dev_id);
  void   (*cleanup)        (void);
} blockdev_driver;


#endif /* not __FS_BLOCKDEV_I_H__ */
