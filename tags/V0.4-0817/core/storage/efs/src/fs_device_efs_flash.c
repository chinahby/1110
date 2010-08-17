/***********************************************************************
 * fs_device_efs_flash.c
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

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_device_efs_flash.c#4 $ $DateTime: 2008/12/15 14:17:12 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-15   rp    Remove reference to obsolete flash APIs.
2008-12-12   rp    Introducing i/o-vec interface.
2008-11-06   rp    Remove old fashioned flash probing.
2008-11-03   rp    Create

===========================================================================*/

#include "fs_config.h"
#include "fs_device.h"

#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)

#include "fs_device_efs_flash.h"
#include "fs_err.h"
#include "fs_util.h"
#include "fs_paranoia.h"

#include "mibib.h"

/* Make sure we have the flash_open() APIs. */
#ifndef HAVE_FLASH_OPEN_AND_CLOSE
  #error "HAVE_FLASH_OPEN_AND_CLOSE is missing..."
#endif /* HAVE_FLASH_OPEN_AND_CLOSE */



#ifndef EFS_PARTITION_NAME
  /* If this is undefined for some reason use default */
  #ifndef MIBIB_EFS2_MODEM_PARTI_NAME
    #define MIBIB_EFS2_MODEM_PARTI_NAME "0:EFS2"
  #endif

  #ifdef FEATURE_FS_REMOTE_APPS_PROC
    #define EFS_PARTITION_NAME MIBIB_EFS2_APPS_PARTI_NAME
  #else
    #define EFS_PARTITION_NAME MIBIB_EFS2_MODEM_PARTI_NAME
  #endif
#endif  /* EFS_PARTITION_NAME defined */



#define FS_DEVICE_FLASH_MAX_DEVICES        (1)


/* Max Flash instances to encapsulate the flash-device. */
static struct efs_device fs_base_flash_device[FS_DEVICE_FLASH_MAX_DEVICES];


void
fs_device_efs_flash_init (void)
{
  memset (fs_base_flash_device, 0, sizeof(fs_base_flash_device));

  fs_paranoia_register (fs_base_flash_device, sizeof(fs_base_flash_device),
                        "flash_dev");
}


efs_device_t
fs_device_efs_flash_open (uint32 which_device)
{
  fs_device_t flash_dev;
  efs_device_t base_dev;
  fs_device_write_style_t write_style;

  if (which_device >= FS_DEVICE_FLASH_MAX_DEVICES)
    FS_ERR_FATAL ("Max device limit exceeded...",0,0,0);

  base_dev = &fs_base_flash_device[which_device];

  flash_dev = flash_open (EFS_PARTITION_NAME, 0);
  if (flash_dev == FS_NO_DEVICE)
    return NULL;

  write_style = flash_dev->get_write_style (flash_dev);
  switch (write_style)
  {
    case FS_DEVICE_WRITES_SIMPLE:
    case FS_DEVICE_WRITES_PAIRED_BITS:
      base_dev->efs_device_type = FS_DEVICE_TYPE_EFS_NOR;
      break;

    case FS_DEVICE_WRITES_PAGED:
      base_dev->efs_device_type = FS_DEVICE_TYPE_EFS_NAND;
      break;

    default:
      FS_ERR_FATAL ("invalid write-style...",0,0,0);
      break;
  }

  base_dev->priv.flash = flash_dev;
  fs_device_efs_flash_init_ops (base_dev);

  return base_dev;
}


int
fs_device_efs_flash_close (efs_device_t base_dev)
{
  int result = -1;

  result = flash_close (base_dev->priv.flash);
  base_dev->priv.flash = NULL;
  return result;
}


static char*
fs_device_flash_device_name (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->device_name (flash_dev);
}

static int
fs_device_flash_device_maker_id (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->device_maker_id (flash_dev);
}

static int
fs_device_flash_device_id (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->device_id (flash_dev);
}

static uint32
fs_device_flash_block_count (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->block_count (flash_dev);
}

static uint32
fs_device_flash_block_size (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->block_size (flash_dev);
}

static uint32
fs_device_flash_page_size (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->page_size (flash_dev);
}

static uint32
fs_device_flash_total_page_size (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->total_page_size (flash_dev);
}

static uint32
fs_device_flash_device_type (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->device_type (flash_dev);
}

static int
fs_device_flash_bad_block_check (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->bad_block_check (flash_dev, block);
}

static int
fs_device_flash_mark_block_bad (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->mark_block_bad (flash_dev, block);
}

static int
fs_device_flash_write_page (efs_device_t base_dev, fs_page_id page, void *data)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->write_page (flash_dev, page, data);
}

static int
fs_device_flash_erase_block (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->erase_block (flash_dev, block);
}

static void*
fs_device_flash_read_pointer (efs_device_t base_dev, fs_page_id page)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->read_pointer (flash_dev, page);
}

static int
fs_device_flash_read_page (efs_device_t base_dev, fs_page_id page, void *data)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->read_page (flash_dev, page, data);
}

static int
fs_device_flash_is_erased (efs_device_t base_dev, fs_page_id page)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->is_erased (flash_dev, page);
}

static int
fs_device_flash_partial_write (efs_device_t base_dev, fs_page_id page,
            void *data, int offset, int length)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->partial_write (flash_dev, page, data, offset, length);
}

static int
fs_device_flash_begin_erase_block (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->begin_erase_block (flash_dev, block);
}

static int
fs_device_flash_suspend_erase (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->suspend_erase (flash_dev);
}

static int
fs_device_flash_resume_erase (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->resume_erase (flash_dev);
}

static int
fs_device_flash_erase_status (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->erase_status (flash_dev);
}

static int
fs_device_flash_is_block_erased (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->is_block_erased (flash_dev, block);
}

static int
fs_device_flash_read_spare_bytes (efs_device_t base_dev, fs_page_id page,
           uint32 spare_offset, uint32 len, void *data)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->read_spare_bytes (flash_dev, page, spare_offset,
                                      len, data);
}

static int
fs_device_flash_read_mpages (efs_device_t base_dev, fs_page_id start_page,
                             fs_page_id num_pages,
                             void *data, void (*wdog_kick_fn_ptr)(void) )
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->read_mpages (flash_dev, start_page, num_pages, data,
                                 wdog_kick_fn_ptr);
}

static int
fs_device_flash_copy_page (efs_device_t base_dev, fs_page_id src_page,
                           fs_page_id dest_page)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->copy_page (flash_dev, src_page, dest_page);
}

static int
fs_device_flash_lock_ops (efs_device_t base_dev, fs_page_id start_page,
                          fs_page_id end_page, uint32 opcode)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->lock_ops (flash_dev, start_page, end_page, opcode);
}

static int
fs_device_flash_get_spare_corrected_bytes (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->get_spare_corrected_bytes (flash_dev);
}

static int
fs_device_flash_write_page_and_udata (efs_device_t base_dev, fs_page_id page,
           void *page_data, void *spare_udata, uint32 spare_udata_blen)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->write_page_and_udata (flash_dev, page, page_data,
                                          spare_udata, spare_udata_blen);
}

static int
fs_device_flash_read_spare_udata (efs_device_t base_dev, fs_page_id page,
           void *spare_udata, uint32 spare_udata_blen)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->read_spare_udata (flash_dev, page,
                                      spare_udata, spare_udata_blen);
}

static int
fs_device_flash_read_page_and_udata (efs_device_t base_dev, fs_page_id page,
           void *data, void *spare, uint32 spare_udata_blen)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->read_page_and_udata (flash_dev, page, data,
                                         spare, spare_udata_blen);
}

static int
fs_device_flash_open_partition (efs_device_t base_dev, char *partition_name)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->open_partition (flash_dev, partition_name);
}

static void
fs_device_flash_set_ecc_state (efs_device_t base_dev, int state)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  flash_dev->set_ecc_state (flash_dev, state);
}

static fs_device_write_style_t
fs_device_flash_get_write_style (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash;
  return flash_dev->get_write_style (flash_dev);
}


void
fs_device_efs_flash_init_ops (efs_device_t base_dev)
{
  base_dev->device_name = fs_device_flash_device_name;
  base_dev->device_maker_id = fs_device_flash_device_maker_id;
  base_dev->device_id = fs_device_flash_device_id;
  base_dev->block_count = fs_device_flash_block_count;
  base_dev->block_size = fs_device_flash_block_size;
  base_dev->page_size = fs_device_flash_page_size;
  base_dev->total_page_size = fs_device_flash_total_page_size;
  base_dev->device_type = fs_device_flash_device_type;
  base_dev->bad_block_check = fs_device_flash_bad_block_check;
  base_dev->mark_block_bad = fs_device_flash_mark_block_bad;
  base_dev->write_page = fs_device_flash_write_page;
  base_dev->erase_block = fs_device_flash_erase_block;
  base_dev->read_pointer = fs_device_flash_read_pointer;
  base_dev->read_page = fs_device_flash_read_page;
  base_dev->is_erased = fs_device_flash_is_erased;
  base_dev->partial_write = fs_device_flash_partial_write;
  base_dev->begin_erase_block = fs_device_flash_begin_erase_block;
  base_dev->suspend_erase = fs_device_flash_suspend_erase;
  base_dev->resume_erase = fs_device_flash_resume_erase;
  base_dev->erase_status = fs_device_flash_erase_status;
  base_dev->is_block_erased = fs_device_flash_is_block_erased;
  base_dev->read_spare_bytes = fs_device_flash_read_spare_bytes;
  base_dev->read_mpages = fs_device_flash_read_mpages;
  base_dev->copy_page = fs_device_flash_copy_page;
  base_dev->lock_ops = fs_device_flash_lock_ops;
  base_dev->get_spare_corrected_bytes =
                        fs_device_flash_get_spare_corrected_bytes;
  base_dev->write_page_and_udata = fs_device_flash_write_page_and_udata;
  base_dev->read_spare_udata = fs_device_flash_read_spare_udata;
  base_dev->read_page_and_udata = fs_device_flash_read_page_and_udata;
  base_dev->open_partition = fs_device_flash_open_partition;
  base_dev->set_ecc_state = fs_device_flash_set_ecc_state;
  base_dev->get_write_style = fs_device_flash_get_write_style;

  base_dev->read_iovec = NULL;
  base_dev->write_iovec = NULL;
  base_dev->read_sectors = NULL;
  base_dev->write_sectors = NULL;
  base_dev->reliable_write_sectors = NULL;
}

#endif /* (FEATURE_EFS_EFS2_ON_NAND) || FEATURE_EFS_EFS2_ON_NOR) */

