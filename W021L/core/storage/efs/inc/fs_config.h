/***********************************************************************
 * fs_config.h
 *
 * EFS2 configuration settings.
 * Copyright (C) 2006-2009 QUALCOMM, Inc.
 *
 * This file contains all of the settable parameters used in EFS2.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/inc/fs_config.h#11 $ $DateTime: 2009/07/06 18:43:01 $ $Author: ebuller $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-07-06   ebb   Added support for boot config FEATURE_EFS_BOOT_ENV.
2009-05-29   ds    Enabled DIAG_FS_ACCESS_VALIDATION & EFS_NAND_FACTORY_START
2009-04-01   yg    Moved some private config items into the private headers
2009-01-20   yg    Allow configured number of bad blocks.
2008-12-15   yg    Rename overriding bad block % configuration setting.
2008-12-12   rp    Introducing i/o-vec interface.
2008-12-02   yg    Added bad block % configuration setting.
2008-10-27   yg    Remove remote EFS related code for CMI.
2008-07-31   rp    Allow targets to override the max file descriptors.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2008-04-28   np    Add support for multiple FTL partitions.
2007-11-28   rp    Corrected the multiple SD slots feature.
2007-11-09   rp    Revert multi-sd card change as 7500-target has issues.
2007-10-19   rp    Corrected FS_MAX_EXTFS_MOUNTS to match the hotplug dvc list
2007-10-01   rp    Added FDI support.
2007-09-13   sch   Defined FS_MAX_PAGES_PER_BLOCK
2007-08-19   rp    Added #define FS_MAX_DESCRIPTORS.
2007-06-09   rp    Doxygen comments markup.
2007-04-05   s h   Comments and cleanup.
2007-04-02   rp    Added HOTPLUG_NOTIFY_MAX_CLIENTS.
2006-12-05   dlb   Max transaction size configurable.
2006-10-26   s h   Allow target to define FS_MAX_ITERATORS
2006-10-26   dlb   Put freemap changes back.
2006-10-11   dlb   Back out freemap changes for now.
2006-10-05   s h   Increase the default path length limits
2006-09-08   dlb   Add configuration parameters for buffer cache.
2006-06-14   dlb   Increase number of iterators.
2006-06-01   s h   More NODEV mountpoints, please
2006-05-22   dlb   Allow path and name max to be specified externally.
2006-06-17   yg    Changed some consts
2006-05-09   s h   Increased the number of nodev mountpoints.
2006-05-09   dlb   Add nodevfs.
2006-05-05   dlb   Return name and path limits to original values.
2006-04-18   dlb   Move hotplug definitions here as well.
2006-04-05   dlb   Create

===========================================================================*/

#ifndef __FS_CONFIG_H__
#define __FS_CONFIG_H__

#include "customer.h"

/* Since some of our customers are using FEATURE_EFS2 to decide between the
 * the EFS1(compat)-API and the EFS2 API, lets avoid this feature FEATURE_EFS2
 * Note: Do not use this feature FEATURE_EFS2 in our EFS-VU, in case if you
 * need to refer the EFS2-API layer then use the feature FEATURE_EFS_API
 * instead. Really FEATURE_EFS2 means FEATURE_EFS_API layer. */
#ifdef FEATURE_EFS2
  #define FEATURE_EFS_API
#endif

/* If FDI is defined, make sure it is also configured as Root. */
#if defined(FEATURE_EFS_FDI) || defined(FEATURE_EFS_ROOT_IS_FDI)
  #if !defined(FEATURE_EFS_FDI)
    #error "Please enable FDI"
  #endif
  #if !defined(FEATURE_EFS_ROOT_IS_FDI)
    #error "Please configure FDI as root"
  #endif

/* FEATURE_EFS_BOOT_ENV should be enabled in efs_boot.min
 * to enable the necessary flags to compile EFS with a
 * minimal footprint. */
#elif defined (FEATURE_EFS_BOOT_ENV)

  /* EFS2 not needed in boot configuration */
  #undef FEATURE_EFS_QEFS2
  #undef FEATURE_EFS_EFS2_ON_MNAND
  #undef FEATURE_EFS_COMPATIBILITY

  /* No USB/Mass Storage needed */
  #undef FEATURE_USB_HOST
  #undef FEATURE_USB_HOST_MASS_STORAGE

  /* Remove most hotplug componenets */
  #undef FEATURE_EFS_HOTPLUG
  #undef FEATURE_EFS_HOTPLUG_FTL
  #undef FEATURE_EFS_HOTPLUG_SD_CPRM
  #undef FEATURE_EFS_HOTPLUG_USB_HOST_MS

  /* Consumes ~120kB ROM */
  #undef FEATURE_EFS_CONVERT_SHIFT_JIS

  /* Remove debugging facilities */
  #undef FEATURE_FS_BENCHMARK

  /* Stub out all rex calls */
  #define FEATURE_EFS_COMPILE_WITHOUT_REX

  /* Only enable API layer and HFAT */
  #define FEATURE_EFS
  #define FEATURE_EFS_API
  #define FEATURE_EFS_EXTFS
  #define FEATURE_EFS_EXTFS_HFAT

  /* Enable hotplug dev APIs and SD driver */
  #define FEATURE_EFS_COLDPLUG
  #define FEATURE_EFS_HOTPLUG_SD

  /* Tune for RAM usage */
  #undef FS_HFAT_MAXVOLUME_COUNT
  #undef FS_HFAT_MAX_OPEN_FILES
  #undef FS_VNODE_POOL_SIZE
  #define FS_HFAT_MAXVOLUME_COUNT   1
  #define FS_HFAT_MAX_OPEN_FILES    8
  #define FS_VNODE_POOL_SIZE        16

#else
  /* By default, make QEFS as the default file-system. */
  #define FEATURE_EFS_QEFS2
  #define FEATURE_EFS_ROOT_IS_QEFS2
#endif


/**********************************************************************
 * CONFIGURATION ITEMS
 *
 * These parameters can be configured as desired.  Note that changing their
 * values doesn't affect existing files in the filesystem.  If a build is
 * loaded with smaller parameters for these values than earlier builds,
 * files may become inaccessible.
 */

/**
 * Maximum length of a full pathname, not including a trailing '\\0'
 * character.  The filesystem only has a few buffers of this length, so it
 * doesn't increase EFS2's RAM usage much, however, other code (such as
 * fs_compat, and fs_am) us this value to allocate a large number of
 * buffers. */
#ifndef FS_PATH_MAX
  #define FS_PATH_MAX 1024
#endif

/**
 * Maximum length of the 'name' component of a file.  The name is the
 * longest string of characters delimited by beginning of string, the '/'
 * character or the end of the string.  This is the maximum length for a
 * filename or directory name.
 */
#ifndef FS_NAME_MAX
  #define FS_NAME_MAX 768
#endif

/**
 * Maximum number of files that can be kept opened simultaneously.
 * Targets can override this value by this using #define
 * EFS_MAX_FILE_DESCRIPTORS
 */
#ifdef EFS_MAX_FILE_DESCRIPTORS
  #define FS_MAX_DESCRIPTORS      EFS_MAX_FILE_DESCRIPTORS
#else
  #define FS_MAX_DESCRIPTORS      128
#endif

/**
 * Not a meaningful value.  Passed back via dia in the max_dir_entries
 * parameter.  Not used anywhere else in the code. */
#define FS_DIR_ENTRY_SIZE 50

/** Maximum number of EFS2 mountpoints.   Only "1" is supported!  */
#define FS_MAX_EFS_MOUNTS 1

/**
 * Maximum number of FDI mountpoints.   Only "1" is supported!  */
#define FS_MAX_FDI_MOUNTS 1

/** The maximum number of EXTFS mountpoints.  Must match the size
 * of the device table in hotplug.   There must be one for each possible FAT
 * mountpoint: USB + SD/MMC + FTL + NULL device. */
#define FS_MAX_EXTFS_MOUNTS     (8 + 5 + 2 + 1)

/** Maximum number of ROMFS mounts */
#define FS_MAX_ROMFS_MOUNTS     2

/**
 * Maximum number of NODEV mounts.  Needs to be at least the size of the
 * EXTFS mounts.  Can be larger if they are used for something else.
 * Add three for additional mountpoints used by unit testing.
 * */
#define FS_MAX_NODEV_MOUNTS     (FS_MAX_EXTFS_MOUNTS +  \
                                 3)

/**
 * The maximum number of mountpoints.  Defines the total number of
 * mountpoints available.  This should be the sum of the maximum for each
 * filesystem type compiled in. */
#define FS_MAX_MOUNTS (FS_MAX_EFS_MOUNTS +    \
                       FS_MAX_FDI_MOUNTS +  \
                       FS_MAX_EXTFS_MOUNTS +  \
                       FS_MAX_ROMFS_MOUNTS +  \
                       FS_MAX_NODEV_MOUNTS)

/**
 * The maximum number of open directory iterators (returned by opendir).
 * These aren't all that expensive.  Although some mountpoint types may
 * allocate a FS_PATH_MAX sized buffer for each one. */
#ifndef FS_MAX_ITERATORS
  #define FS_MAX_ITERATORS        5
#endif

/**
 * The maximum number of flash pages (writeable unit) in one flash
 * block (erase unit).  A 4KB-page device has 128 pages per block.
 * There may be more in 4KB-page MLC devices or future devices, which
 * will require adjustment to this value.
 *
 * This value must be at least as large as the largest block size
 * supported by this build.
 */
#ifndef FS_MAX_PAGES_PER_BLOCK
  #define FS_MAX_PAGES_PER_BLOCK              128
#endif


/**
 * Free bitmap size configuration.  This is the amount of buffer space we
 * set aside to hold the freemap.  The value is a size in bytes for the
 * cache.  It must be large enough to hold the number of clusters needed to
 * hold the freemap (1 bit per data cluster).  It needs to be at least
 *   ceil (total_clusters / 8)
 * rounded up to the nearest EFS_PAGE_SIZE.
 */
#ifndef FS_BUFFER_FREEMAP_SIZE
  #define FS_BUFFER_FREEMAP_SIZE          65536
#endif

/*
 * Set how much % of blocks can go bad. For real small partition this
 * might turn out to be just 1 block. This configuration really depends
 * on how much space can we lose in EFS overhead. Also allowing too many
 * bad blocks in the parition could make EFS very very slow when maximum
 * allowed number of bad blocks have developed and the EFS is full. Because
 * EFS will be trying to freeup enough space by doing GC which it would never
 * be able to attain, since there will be some amount of garbage in the system
 * at any given instance. */
#ifdef FS_MAX_BAD_BLOCK_PERCENT_ALLOWED
  #define MAX_BAD_BLOCK_PERCENT_ALLOWED   FS_MAX_BAD_BLOCK_PERCENT_ALLOWED
#else
  #define MAX_BAD_BLOCK_PERCENT_ALLOWED   4
#endif

/* Configure minimum suported bad blocks in the Data region */
#define FS_MIN_DATA_BAD_BLOCK_ALLOWED          3

/* Configure minimum suported bad blocks in the PTable region */
#define FS_MIN_PTABLE_BAD_BLOCK_ALLOWED          3


#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**********************************************************************
 * The following items shouldn't be changed (and don't need to be changed).
 * They are specifically tuned for characteristics of the B-tree itself.
 * Also, changes to these value would require the filesystem to be wiped.
 * */

/* Maximum length of a EFS item file (efs_raw_get/efs_raw_put).  The entire
 * database entry for the item should be no more than 1/3 of the available
 * space in a database node.  The smallest cluster size supported is 512.
 * This value allows room for the needed overhead. */
#define FS_ITEM_MAX  106

/* Maximum length of a symlink destination.  Should be the same as
 * FS_ITEM_MAX, for the same reasons given in its definition. */
#define FS_MAX_SYMLINK_TARGET  107

/* Size of each piece of a longname stored in the value field of the
 * database. */
#define FS_LONGNAME_PIECE_SIZE  108

/* Maximum 'value' size for a database entry.  Must be able to store the
 * largest item or symlink used by the filesystem.  There is only one
 * buffer of this size. */
#define FS_MAX_DB_VALUE  128

/* Theshold above which filenames are stored differently.  Because the
 * names are stored in a B-Tree, the length of the name stored there is
 * limited.  Names that are longer than this threshold are stored using a
 * hash and long name auxiliary entries. */
#define FS_SHORTNAME_THRESHOLD  128

/* This is the number of characters from the full filename that are stored
 * in the hash entry.  Prefixing the shortname with characters from the
 * long name causes the names to still generally sort correctly. */
#define FS_SHORTNAME_PREFIX_LEN 8

/* How many bytes are in 1 SDCC sector. */
#define FS_SDCC_SECTOR_SIZE_IN_BYTES        (512)

/**********************************************************************
 * These parameters should not be adjusted, but are computed based on the
 * above values, and other customer defined values. */

/**
 * Buffer cache configuration.  This is the number of EFS_PAGE_SIZE buffers
 * used by the buffering code (fs_buffer.c).  It needs to be at least as
 * large as the largest number of clusters that will be dirtied by a single
 * transaction.  It can be made larger, which will reduce some reads, but
 * keep in mind that there is no indexing on the cache, so cache misses
 * will cause a scan through the entire pool.
 */
#define FS_BUFFER_NUMBER_BUFFERS        30

/* Maximum number of data pages in a transaction.  This needs to be small
 * enough so that all of the transaction data can be processed in a single
 * log page. */
#define FS_MAX_TRANSACTION_DATA_PAGES 16


/* Maximum IO-Vec items in one transacton. */
#ifndef FS_MAX_IOVEC_ITEMS_COUNT
  /* In one transaction utmost, how many freemap pages we will have to write.*/
  #define FS_MAX_IOVEC_ITEMS_FREEMAP_OVERHEAD_COUNT     \
                      (FS_MAX_TRANSACTION_DATA_PAGES)
  #define FS_MAX_IOVEC_ITEMS_COUNT            (FS_BUFFER_NUMBER_BUFFERS +   \
                                   FS_MAX_IOVEC_ITEMS_FREEMAP_OVERHEAD_COUNT)
#endif



/* The old value was always 8k.  Both of the above numbers have been
 * tested. */
// #define FS_MAX_TRANSACTION_DATA_PAGES (8192 / EFS_PAGE_SIZE)

/* Padding to allow for user pages as well.  Used by the page manager and
 * buffer code to have enough padding to write a full transaction. */
#define FS_TRANSACTION_PAD \
  (FS_MAX_TRANSACTION_DATA_PAGES + (FS_MAX_TRANSACTION_DATA_PAGES >> 2))

/* Maximum transaction size in bytes. */
#define FS_MAX_TRANSACTION (FS_MAX_TRANSACTION_DATA_PAGES * EFS_PAGE_SIZE)

/* Maximum FAT transaction size.  The HFAT filesystem doesn't have a limit,
 * but we set one so that a given request doesn't block the filesystem for
 * too long.  There are also some versions of underlying drivers that have
 * problems over 64k. */
#ifndef FS_MAX_FAT_TRANSACTION
  #define FS_MAX_FAT_TRANSACTION  65536
#endif

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/** Hotplug allows applications to register for notification upon media
 * events.  This define sets the maximum number of specific callbacks
 * that can be concurrently registered with hotplug. */
#ifndef FS_HOTPLUG_NOTIFY_MAX_CLIENTS
  #define FS_HOTPLUG_NOTIFY_MAX_CLIENTS 10
#endif

/** FTL supports multiple partitions. Maximum number of FTL partitions
 *  can be chosen with the following definition.
 */
#ifndef MAX_FTL_PARTITIONS
  #if defined(FS_UNIT_TEST)
    #define MAX_FTL_PARTITIONS                    2
  #else
    #define MAX_FTL_PARTITIONS                    1
  #endif
#endif

/* ------------------------------------------------------------------------
 * Define EFS Access Validation
 * Provides an interface for a client to register an access validation
 * function. When registered, DIAG will call the function for each of
 * the file system operations given by fs_diag_op_type.  The callback
 * returns a value indicating whether the operation can proceed. If
 * the call back indicates the operation is not ok, DIAG returns an
 * ERR packet indicating this is a bad command - otherwise the operation
 * proceeds as normal.
 * ------------------------------------------------------------------------*/
#define FEATURE_DIAG_FS_ACCESS_VALIDATION

/* If no active EFS superblocks are found, then search for the factory
 *  image if this feature is turned on
 */
#define FEATURE_EFS_NAND_FACTORY_START

#endif /* not __FS_CONFIG_H__ */
