/****************************************************************************
 * hfat_interface.c
 *
 * This file has glue functions for HFAT through which it interacts with 
 * Hotplug
 * 
 * Copyright (C) 2006--2009 QUALCOMM, Inc.
 *
 * Verbose Description
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/hfat/main/latest/src/hfat_interface.c#4 $ $DateTime: 2009/01/30 18:21:56 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-01-30   wek   Added ERR_FATAL to increase robustness on device init.
2009-01-29   wek   Dynamic allocation of drivers from a pool.
2008-11-20   ebb   Stub out hfat_getstatus to return 0.
2007-05-25   umr   Added support for EFS memory protection feature.
2006-12-19   sch   Initialized udata multiple sector write calls
2006-11-09   sch   Implemented udata write calls
2006-09-27    sh   Purge all references to C/H/S.
2006-07-13   sch   create

===========================================================================*/
#include "fs_hotplug.h"
#include "hfat_fat.h"
#include "fs_protected_memory.h"
#include "hfat_common.h"
#include "err.h"

/* This data structure is used to construct a table that specifies
 * the number of heads and sectors per track for various card sizes.
 */
typedef struct card_CHS_t
{
  uint16 numberMegBytes;        /* card size in mega-bytes */
  uint8 numberHeads;            /* number of  Heads   */
  uint8 secPerTrack;            /* sectors per track  */
} card_CHS;

static F_DRIVER hfat_driver_table[FN_MAXVOLUME];

int
hfat_readsector (F_DRIVER * driver, void *data, unsigned long sector)
{
  return hotplug_dev_read (driver->user_ptr, sector, data, 1);
}

int
hfat_writesector (F_DRIVER * driver, void *data, unsigned long sector)
{
  return hotplug_dev_write (driver->user_ptr, sector, data, 1);
}

int
hfat_write_udata_sector (F_DRIVER * driver, void *data, unsigned long sector)
{
  return hotplug_dev_write_udata (driver->user_ptr, sector, data, 1);
}

int
hfat_readmultiplesector (F_DRIVER * driver, void *data, unsigned long sector,
                         int count)
{
  return hotplug_dev_read (driver->user_ptr, sector, data, count);
}

int
hfat_writemultiplesector (F_DRIVER * driver, void *data, unsigned long sector,
                          int count)
{
  return hotplug_dev_write (driver->user_ptr, sector, data, count);
}

int
hfat_write_udata_multiplesector (F_DRIVER * driver, void *data,
                                 unsigned long sector, int count)
{
  return hotplug_dev_write_udata (driver->user_ptr, sector, data, count);
}

int
hfat_getphy (F_DRIVER * driver, F_PHY *phy)
{
  uint32 block_cnt;
  uint16 block_size;

  /* Ask hotplug how big this device is */
  if (hotplug_dev_get_size (driver->user_ptr, &block_cnt, &block_size) != 0)
    return -1;

  /* HFAT is only tested with block sizes of exactly 512 bytes */
  if (block_size != 512)
    return -1;

  /* This value is real.  It is the exact (LBA) count of blocks */
  phy->number_of_sectors = block_cnt;

  /* There is no such thing as CHS any more, so we use max values */
  phy->number_of_heads = 255;      /* Bogus */
  phy->number_of_cylinders = 1024; /* Bogus */
  phy->sector_per_track = 63;      /* Bogus */

  return 0;
}

long
hfat_getstatus (F_DRIVER * driver)
{
  (void) driver;
  return 0;
}

/* Device initialization function */
F_DRIVER *
hfat_device_init (unsigned long driver_param)
{
  F_DRIVER *f_driver;
  int8 index;

  index = _f_drivenum_to_index (driver_param);
  
  if (index < 0)
    ERR_FATAL ("HFAT invalid device index",0,0,0);

  f_driver = &hfat_driver_table[index];

  f_driver->readsector                  = hfat_readsector;
  f_driver->readmultiplesector          = hfat_readmultiplesector;
  f_driver->writesector                 = hfat_writesector;
  f_driver->writemultiplesector         = hfat_writemultiplesector;
  f_driver->write_udata_sector          = hfat_write_udata_sector;
  f_driver->write_udata_multiplesector  = hfat_write_udata_multiplesector;
  f_driver->getphy                      = hfat_getphy;
  f_driver->getstatus                   = hfat_getstatus;
  f_driver->release                     = NULL;
  f_driver->user_ptr                    = hotplug_hdev (driver_param);

  return f_driver;
}
