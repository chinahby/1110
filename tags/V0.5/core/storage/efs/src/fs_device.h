#ifndef __FS_DEVICE_H__
#define __FS_DEVICE_H__

/**********************************************************************
 * fs_device.h
 *
 * File System Device interface.
 *
 * This module implements a device-independent view of the low-level file
 * system devices.  This interface hides all of the implementation details
 * and device differences between the diverse device types.
 *
 * Copyright (C) 2002-2009, Qualcomm, Inc.
 *
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_device.h#8 $ $DateTime: 2009/05/18 17:19:57 $ $Author: richardp $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-05-08   ebb     Moved to block_device interface.
2009-01-26   ebb     Moved from flash.h => fs_flash.h
2008-12-15    rp     Remove reference to obsolete flash APIs.
2008-12-12    rp     Introducing i/o-vec and Managed Nand.
2008-11-03    rp     Introduce device abstraction.
2008-09-16   yog     Remove trailing commas from the enum declaration.
2008-05-23    rp     Use bencmark counters from fs_counters.h
2007-10-18    rp     Featurized QEFS2.
2007-02-23   dlb     Move read failures into device wrappers.
2006-11-09   dlb     Add dealloc counter.
2006-09-20   dlb     Lint fixes.
2006-09-12   dlb     Change some queries to unsigned values.
2006-09-11   dlb     Add wrappers for all flash functions.
2006-06-26    yg     Memory reduction effort
2006-01-27   dlb     Add wrappers for write styles on old drivers.
2005-10-21   dlb     Mark unsigned constants as such.
2004-10-15   dlb     Update copyright line.
2004-04-26   drh     Remove support for old non-uniform flash drivers
2004-02-20   drh     Add support for Uniform Flash Drivers
2003-06-15    gr     Added a function pointer for mark_block_bad.
2003-04-04   bgc     Added FS_DEVICE_BUSY and function pointers for
                     begin_erase, suspend, resume, and erase_status.
2002-09-05    gr     Changed signature of is_erased to accept a page id rather
                     than a block id.
2002-08-19   drh     Added is_erased to fs_device_data structure
2002-08-08   drh     Added total_page_size to fs_device_data structure
2002-06-13   dlb     Created

===========================================================================*/

#include "fs_efs2_cust.h"

/* Until we upgrade all the custefs.h files to correctly define the device-type
   as NAND or MNAND or NOR, lets detect the device type the old-fashined way.*/
#if defined (FEATURE_EFS_EFS2_ON_NAND) || \
    defined (FEATURE_EFS_EFS2_ON_NOR)  || \
    defined (FEATURE_EFS_EFS2_ON_MNAND)
  /* Device type is configured, so proceed with this device ... */
#else
  /* The custefs.h is not upgraded yet, so detect NAND or NOR the old way */
  #ifdef FEATURE_EFS_PROBE_NAND
    #define FEATURE_EFS_EFS2_ON_NAND
  #else
    #define FEATURE_EFS_EFS2_ON_NOR
  #endif
#endif

#include "fs_flash.h"
#include "comdef.h"
#include "fs_device_types.h"
#include "fs_counters.h"
#include "fs_iovec.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND
  #include "fs_blockdev.h"
#endif

/* The RAMDISK support was removed a long time ago, so ensure that
 * nobody is enabling or expecting this code to be present. */
#ifdef FEATURE_EFS_RAMDISK
  #error "EFS does not have support for RAMDISK (FEATURE_EFS_RAMDISK)"
#endif
#ifdef FEATURE_EFS_ROOT_RAMDISK
  #error "EFS does not allow root on RAMDISK (FEATURE_EFS_ROOT_RAMDISK)"
#endif

/* Encapsulate the get_write_style feature and types, so that the code will
 * continue to work with older drivers.  We assume that all older drivers
 * (for NOR) are simple devices.  The driver will define
 * HAVE_FLASH_WRITE_STYLE to indicate that the query is present, and this
 * driver might support other write styles. */

#ifdef HAVE_FLASH_WRITE_STYLE

  #define FS_GET_WRITE_STYLE(dev)  \
    ((dev)->get_write_style (dev))

#else /* not HAVE_FLASH_WRITE_STYLE */

  /* Enough of the types to allow things to build. */
  typedef word fs_device_write_style_t;

  /* And the definitions used in this file.  The only value that must match
   * is FS_DEVICE_WRITES_SIMPLE, because that is the one returned by the
   * constant "query".  The others are defined to vary different values to
   * make it clear if they ever show up. */
  #define FS_DEVICE_WRITES_SIMPLE 0
  #define FS_DEVICE_WRITES_PAIRED_BITS   0x18ea928b

  #define FS_GET_WRITE_STYLE(dev)   FS_DEVICE_WRITES_SIMPLE

#endif /* not HAVE_FLASH_WRITE_STYLE */

/* TODO: These are simple thin wrappers.  Eventually, we want to capture
 * which calls are allowed to fail, and die with a more meaningful message
 * if the others fail. */

/* These calls are wrappers around the device calls.  Most of the time,
 * device failures are fatal, but occasionally they are expected and are
 * handled.  The last argument to these calls indicates whether or not
 * these calls should fail. */
enum fs_flash_failure {
  FS_FLASH_FAIL_OK,   /* It is ok to return a failure. */
  FS_FLASH_MUST_WORK  /* This call should not fail, and a failure return
                         will result in a fatal error. */
};


struct efs_device;
typedef struct efs_device *efs_device_t;
struct efs_device
{

  /* Query operations. */

  /* These will no longer be one each for each device, but
   * will be generic functions that extract data from the
   * private portions of the device structure and return
   * that data.  They will operate the same for all devices.*/
  char * (*device_name) (efs_device_t self);
  int (*device_maker_id) (efs_device_t self);
  int (*device_id) (efs_device_t self);
  uint32 (*block_count) (efs_device_t self);
  uint32 (*block_size) (efs_device_t self);
  uint32 (*page_size) (efs_device_t self);
  uint32 (*total_page_size) (efs_device_t self);
  uint32 (*device_type) (efs_device_t self);

  /* Bad block check for NAND devices */
  int (*bad_block_check) (efs_device_t self, fs_block_id block);

  /* Mark a block as bad for NAND device */
  int (*mark_block_bad) (efs_device_t self, fs_block_id block);

  /* Write operations. */
  int (*write_page) (efs_device_t self,
    fs_page_id page,
    void *data);

  /* A full erase.  Eventually, will implement NOR-style partial erasing. */
  int (*erase_block) (efs_device_t self,
    fs_block_id block);

  /* Get the address of read data.  Returns NULL if this device doesn't
   * support direct mapping of read data. */
  void *(*read_pointer) (efs_device_t self,
    fs_page_id page);

  /* Read the data into a buffer.  Can be used if the above returns NULL. */
  int (*read_page) (efs_device_t self,
    fs_page_id page,
    void *data);

  /* Determine erase status of page */
  int (*is_erased) (efs_device_t self, fs_page_id page);

  /* If this function pointer is non-null, then can be called to perform a
   * partial page write.  It is only valid to change FF's in flash into
   * values, and then to change values into 00's.  The write must be
   * contained entirely within a page, and is only guaranteed to work
   * on 32 bit boundaries. */
  int (*partial_write) (efs_device_t self, fs_page_id page,
    void *data, int offset, int length);

  /* A full erase that returns and leaves the erase pending */
  int (*begin_erase_block) (efs_device_t self,
    fs_block_id block);

  /* Suspend the current erase */
  int (*suspend_erase) (efs_device_t self);

  /* Resume suspended erase */
  int (*resume_erase) (efs_device_t self);

  /* Check if the erase block operation begun with begin_erase_block has
   * completed. */
  int (*erase_status) (efs_device_t self);

  /* Check if given block is erased */
  int (*is_block_erased) (efs_device_t self, fs_block_id block);

  /* Read len bytes from spare_offset in the spare area in given page
   * and copy into given data buffer. Data is read with ECC turned OFF */
  int (*read_spare_bytes) (efs_device_t self, fs_page_id page,
  uint32 spare_offset, uint32 len, void *data);

  /* Read num_pages from start_page and copy them into given data location/
   * address */
  int (*read_mpages) (efs_device_t self, fs_page_id start_page,
    fs_page_id num_pages, void *data, void (*wdog_kick_fn_ptr)(void));

  /* Copy data from source page and write it into dest_page */
  int (*copy_page) (efs_device_t self, fs_page_id src_page,
    fs_page_id dest_page);

  /* Support lock/unlock/locktight of blocks from start_page to end_page */
  int (*lock_ops) (efs_device_t self, fs_page_id start_page,
    fs_page_id end_page, uint32 opcode);

  /* Get corrected spare user data bytes available from controller */
  int (*get_spare_corrected_bytes) (efs_device_t self);

  /* Write page data and spare user data bytes to given page with any
     hardware/software ECC for spare user data bytes if needed */
  int (*write_page_and_udata) (efs_device_t self,
    fs_page_id page,
    void *page_data,
    void *spare_udata,
    uint32 spare_udata_blen);

  /* Read only the user data in spare location of given page with ECC
     correction/detection */
  int (*read_spare_udata) (efs_device_t self,
    fs_page_id page,
    void *spare_udata,
    uint32 spare_udata_blen);

 /* Read the data and Spare bytes into buffers. */
  int (*read_page_and_udata) (efs_device_t self,
    fs_page_id page,
    void *data,
    void *spare,
    uint32 spare_udata_blen);

  /* Open a particular partition.
   *     Caller passes device structure filled in during probe,
   *     this function call may modify it if successful. */
  int (*open_partition) (efs_device_t self, char *partition_name);

  /* Change the default state of ECC calculation
   *     This is used mainly when reading back a raw file system
   *     using flash tools. */
  void (*set_ecc_state) (efs_device_t self, int state);

  /* Get the device write style. */
  fs_device_write_style_t
    (*get_write_style) (efs_device_t self);

  /* Read/Write iovec_cnt items onto the iovec array. */
  int (*read_iovec) (efs_device_t self, struct fs_iovec *iovec,
                     uint32 iovec_cnt);
  int (*write_iovec) (efs_device_t self, struct fs_iovec *iovec,
                      uint32 iovec_cnt);

  /* Read/Write multiple sectors.. */
  int (*read_sectors) (efs_device_t self, uint32 sect, void *data,
                       uint32 num_sectors);
  int (*write_sectors) (efs_device_t self, uint32 sect, void *data,
                        uint32 num_sectors);
  /* Reliably write multiple sectors. */
  int (*reliable_write_sectors) (efs_device_t self, uint32 sect, void *data,
                                 uint32 num_sectors);

  enum fs_device_type             efs_device_type;

  union
  {
    #if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
        defined (FEATURE_EFS_EFS2_ON_NOR)
      fs_device_t                 flash;
    #endif

    #if defined (FEATURE_EFS_FTL)
      fs_device_t                 ftl_flash;
    #endif

    #ifdef FEATURE_EFS_EFS2_ON_MNAND
      struct block_device        *block_dev;
    #endif

    int                           dummy; /* Fix potential empty union */
  }priv;

};


void fs_device_init (void);
efs_device_t fs_device_open (enum fs_device_type device_type,
                             uint32 which_device);
int fs_device_close (efs_device_t base_dev);
/* Return 1 if it is NAND device and 0 if it is not. */
int fs_device_is_nand_device (efs_device_t base_dev);

int fs_device_write_page (efs_device_t dev, fs_page_id page, void *data,
    enum fs_flashop_category category);
int fs_device_erase_block (efs_device_t dev, fs_block_id block,
    enum fs_flashop_category category);
int fs_device_read_page (efs_device_t dev, fs_page_id page, void *data,
    enum fs_flashop_category category,
    enum fs_flash_failure failure);

int fs_device_read_iovec (efs_device_t base_dev, struct fs_iovec *iovec,
                          uint32 iovec_cnt, enum fs_flashop_category category,
                          enum fs_flash_failure failure);
int fs_device_write_iovec (efs_device_t base_dev, struct fs_iovec *iovec,
                          uint32 iovec_cnt, enum fs_flashop_category category);
int fs_device_read_sectors (efs_device_t base_dev, uint32 sect, void *data,
                        uint32 num_sectors, enum fs_flashop_category category);
int fs_device_write_sectors (efs_device_t base_dev, uint32 sect, void *data,
                        uint32 num_sectors, enum fs_flashop_category category);
int fs_device_reliable_write_sectors (efs_device_t base_dev, uint32 sect,
                                  void *data, uint32 num_sectors,
                                  enum fs_flashop_category category);

uint32 fs_device_block_size (efs_device_t dev);
uint32 fs_device_page_size (efs_device_t dev);
uint32 fs_device_block_count (efs_device_t dev);
int fs_device_bad_block_check (efs_device_t dev, fs_block_id block);
int fs_device_is_erased (efs_device_t dev, fs_page_id page);
int fs_device_mark_block_bad (efs_device_t dev, fs_block_id block);

int fs_device_resume_erase (efs_device_t dev);
int fs_device_erase_status (efs_device_t dev);
int fs_device_suspend_erase (efs_device_t dev);
int fs_device_begin_erase_block (efs_device_t dev, fs_block_id block);
int fs_device_partial_write (efs_device_t dev, fs_page_id page,
    void *data, int offset, int length);
void *fs_device_read_pointer (efs_device_t dev, fs_page_id page);


#endif /* __FS_DEVICE_H__ */
