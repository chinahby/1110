/***********************************************************************
 * custefs.example.h
 *
 * Example configuration for EFS features
 *
 * Copyright (C) 2006-2009 QUALCOMM, Inc.
 *
 * EFS has many configuration options.  This example file documents
 * each configurable option and how it is used.
 *
 * Note that this file is NOT USED in the EFS VU itself -- it is just
 * example code that a target may want to borrow from.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/inc/custefs.example.h#7 $ $DateTime: 2009/04/06 18:47:29 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-04-01   yg    Removed unused config items
2009-04-01   yg    Changed default FTL config on target to 2k page
2009-01-29   wek   Added FS_HFAT_MAXVOLUME_COUNT.
2008-12-15   yg    Added documentation for Bad Block % setting.
2008-12-15   yg    Changed name WR_DATACACHE_SIZE to HFAT_WR_DATACACHE_SIZE.
2008-11-24   yg    Added documentation for WR_DATACACHE_SIZE.
2008-07-31   rp    Show targets how to override max file descriptors.
2007-09-20   s h   Removed RAMDISK reference
2007-07-20   s h   Added Shift-JIS documentation
2007-05-10   yg    Added FTL config definitions which override defaults.
2006-12-05   dlb   Explain large transaction definition.
2006-11-30   yg    Option to override FTL Cache page count for small devices.
2006-11-22   yg    Add FTL partition related option description.
2006-11-08   s h   Removed Hotplug Remote reference.
2006-10-26   s h   Create

===========================================================================*/

#ifndef __CUSTEFS_H__
#define __CUSTEFS_H__

/*
 * All externally-selectable EFS code features begin with FEATURE_FS
 * or FEATURE_EFS.  Flags that do not begin with these are not
 * build-tunable options -- they are internal and should be considered
 * carefully before changing.
 */

/*
 * Define to enable the Embedded file system.  These two features are
 * not really optional, since many files in AMSS require them.  Both
 * must be defined on every AMSS target.
 */
#define FEATURE_EFS
#define FEATURE_EFS2

/*
 * There are optional FS Diag commands to execute certain benchmark
 * tests on-target and report timing measurements from the filesystem
 * and underlying devices.  This is useful to characterize performance
 * of the filesystem and block devices, independent of any data
 * transport.
 *
 * This feature is intended ONLY for engineering/debug builds, since
 * the Benchmark commands allow unsecured file access.  They can also
 * be used to perform destructive or disabling operations.
 *
 * Enable only in Engineering builds.  No produciton build should ever
 * define this.
 */
#undef FEATURE_FS_BENCHMARK

/*
 * Enable support for "CEFS" (Canonical EFS) images, which are
 * factory-programmed into NAND.
 *
 * If this feature is not defined, CEFS images will be ignored.
 *
 * If this feature is defined, EFS will check for a CEFS image at
 * startup and unpack it into an EFS2 filesystem if found.
 *
 * Most targets want this enabled in all builds.
 */
#define FEATURE_EFS_NAND_FACTORY_START

/*
 * EFS Access Validation
 *
 * Provides an interface for a client to register an access validator
 * function.  When registered, FS DIAG will call the function for each
 * of the file system operations given by fs_diag_op_type.  The
 * callback returns a value indicating whether the operation can
 * proceed.  If the call back indicates the operation is not ok, DIAG
 * returns an ERR packet indicating this is a bad command - otherwise
 * the operation proceeds as normal.
 *
 * This Access Validation is required by BREW Serial Port Access
 * Restrictions and should be DEFINED in all builds.  Disabling this
 * feature creates security holes in FS Diag, as all files will be
 * accessible.
 */
#define FEATURE_DIAG_FS_ACCESS_VALIDATION

/*
 * Allow fatal errors to be "handled".  If this feature is defined, the
 * function handle_efs_err_fatal will be called.  Normally, this function
 * should either do nothing, or call efs_reset_nostop() and then return.
 *
 * This should never be defined in any build.
 */
#undef FEATURE_EFS_FATAL_ERROR_HANDLER


/*
 * To facilitate factory NAND programming, Diag can be used to extract
 * a Canonical EFS image (CEFS) from a running phone.  This image
 * contains ALL filesystem data, including protected or hidden data.
 * The image can be programmed into NAND memory during factory
 * production to reproduce an exact filesystem across multiple phones,
 * even with different bad-block maps.
 *
 * Since this feature allows unrestricted access to ALL files via
 * Diag, it presents an ENORMOUS SECURITY RISK and must never be
 * enabled in production/customer builds.  Only the 'reference phone'
 * should have this feature defined in a special build.
 *
 * NOTE: Because CEFS extraction locks the filesystem during the
 * entire process (for consistency), all tasks may be blocked from
 * executing.  This can cause software watchdog resets during the CEFS
 * process.  For this reason, the special build flag
 * NO_DOG
 * must also be defined for CEFS-enabled builds to disable the
 * software watchdog.
 */
#undef FEATURE_EFS_ENABLE_FACTORY_IMAGE_SECURITY_HOLE


/*---------------------------------------------------------------------------
  EFS2 Configuration Overrides

  There are some values in fs_config.h that can be overridden from the
  custefs.h or builds file just by defining a different value.

  Most builds should leave these UNDEFINED to use the default values.

  Please see fs_config.h for an explanation of these values.
  ---------------------------------------------------------------------------*/
/* This sets the maximum number of open directory iterators (returned
 * by opendir). */
//#define FS_MAX_ITERATORS        5

/* Maximum length of a full pathname, not including a trailing '\0'
 * character.  More information is available in fs_config.h. */
//#define FS_PATH_MAX 1024

/* Maximum length of the 'name' component of a file.
 * Documented in fs_config.h */
//#define FS_NAME_MAX 768

/* Defining this enabled large transactions.  Currently, this causes
 * 2k-page devices to limit transactions to 128k instead of 32k.  However,
 * it increases the size of the buffer pool to hold the data of the larger
 * transactions (adding 140K of RAM).  If a specific application has
 * buffers large enough to use this, there is a slight performance gain.
 * It offers no benefits to any existing applications, and therefore should
 * normally be disabled. */
#undef FEATURE_EFS_LARGE_TRANSACTIONS


/* Ideally EFS can work with around 4% blocks going bad. But for very small
 * EFS configurations this setting can have a tolerance of just 1 bad block
 * which is not desirable since sometimes factory will ship with more bad
 * blocks or weak blocks that could get bad in that region. So set this
 * macro to appropriate value to increase the tolerance. But the tolerance
 * increase comes with a penalty of lost usable EFS space.
 * */
#define FS_MAX_BAD_BLOCK_PERCENT_ALLOWED   8


/*---------------------------------------------------------------------------
  EFS1 API Compatibility
---------------------------------------------------------------------------*/
/*
 * Define this feature to enable the EFS1 Compatibility layer.
 *
 * The FS Compat code will approximate EFS1 function calls with EFS2
 * calls so that legacy code can continue to execute on EFS2
 * filesystems.
 *
 * All new code should use the EFS2 API efs_xxx(), but since there is
 * still a large body of code using EFS1 calls fs_xxx(), this feature
 * should be enabled on most builds.  Also, the files in
 * MSMSHARED_FS_COMPAT VU is required for this feature.
 *
 * (It should be disabled on some Iguana Server builds.)
 */
#define FEATURE_EFS_COMPATIBILITY

/*
 * Define to enable the Access Methods handlers for the FS Compat
 * task.  This feature is currently required for EFS1 Compatibility.
 * (It will be rendered obsolete and removed soon.)
 */
#ifdef FEATURE_EFS_COMPATIBILITY
#  define FEATURE_EFS_ACCESS_METHODS
#endif

/*---------------------------------------------------------------------------
  Flash Translation Layer

  Simulate sector-based media using a portion of the NAND memory.
---------------------------------------------------------------------------*/

/*
 * The Flash Translation Layer (FTL) allows a portion of a NAND flash
 * to behave like a sector-based device (disk drive) so that a FAT
 * filesystem can be stored on it. When this option is enabled, the
 * partition option (FEATURE_FTL_USE_PARTITION) may also need to be enabled.
 */
#define FEATURE_EFS_FTL

/*
 * WARNING..!! WARNING..!! WARNING..!!
 * Not turning this flag ON, with FTL enabled, will make FTL use the whole
 * device as the FTL storage, it could result into erasing all the other
 * partitions on the flash device.
 *
 * If this option is enabled, then FTL has its own seperate partition as
 * storage, open the partition and use it if available, do not use the whole
 * device as storage media. If the partition is not available then it is
 * similar to the situation as not having a storage media in the slot, so
 * "/ftl1" will not be mounted.
 */
#define FEATURE_FTL_USE_PARTITION

/*---------------------------------------------------------------------------
  FTL Configuration Overrides

  There are some values in fs_ftl_cache_i.h that can be overridden from the
  custefs.h or builds file just by defining a different value.

  Most builds should leave these UNDEFINED to use the default values.

  Please see fs_ftl_cache_i.h or FTL design document for an explanation of
  these values.
  ---------------------------------------------------------------------------*/

/*
 * FS_FTL_FLASH_MAX_PAGE_SIZE defines what is the largest supported Flash
 * device page size. This is one of the parameters that decides how big FTL
 * cache will be. By modifying this config, bigger part can be supported or
 * RAM can be saved by supporting only small page device. Defaulted to 2k
 * if this is not defined.
 */
#define FS_FTL_FLASH_MAX_PAGE_SIZE     4096

/*
 * FS_FTL_FLASH_MAX_BLOCK_SIZE defines how many physical pages are there in
 * the biggest supported flash device's block. Typically this is one of the
 * config that decides how big FTL cache will be. By modifying this config,
 * either smaller or bigger device than the default config can be supported,
 * either to save RAM or just to support bigger device.
 * Note that this configuration really depends on the flash device used,
 * so refer to datasheets of the biggest device that is being supported to
 * configure this value properly. If undefined, this value defaults to 64.
 *
 * For 512 page device value may be 32
 * For 2k  page device value may be 64
 * for 2k  page MLC NAND device value may be 128
 * */
#define FS_FTL_FLASH_MAX_BLOCK_SIZE        128



/*---------------------------------------------------------------------------
  Removable (FAT) Media and Hotplug

  Windows(tm)-compatible media is detected by the Hotplug task, and uses
  a FAT filesystem.
---------------------------------------------------------------------------*/

/*
 * FEATURE_EFS_HOTPLUG_USB_HOST_MS
 * USB Host Mass Storage devices need Hotplug management.
 *
 * This feature will allow the Hotplug task to poll each possible USB
 * slot for media.  Examples are Thumbdrives, USB hard disk drives,
 * and USB Card Readers.
 *
 * If this feature is not defined, then USB Host Mass Storage devices
 * will be ignored by hotplug and unuseable for file storage.
 *
 * Targets that have FEATURE_USB_HOST_MASS_STORAGE should enable this.
 */
#ifdef FEATURE_USB_HOST_MASS_STORAGE
#  define FEATURE_EFS_HOTPLUG_USB_HOST_MS
#endif

/*
 * FEATURE_EFS_HOTPLUG_SD
 * SD/MMC slots needs Hotplug management.
 *
 * Defining this feature will instruct Hotplug to poll each available
 * SD or MMC slot for media.
 *
 * If this feature is not defined, then MMC/SD slots will be ignored
 * by hotplug and not useable for file storage.
 *
 * Targets that support removable cards with an MSM controller should
 * always define this feature.
 */
#if defined FEATURE_MMC
 || defined FEATURE_DRV_SDCC \
 || defined FEATURE_SDCC_EXTERNAL_DM
#  define FEATURE_EFS_HOTPLUG_SD
#endif

/*
 * If the Flash Translation Layer (FTL) code is enabled, then allow
 * hotplug to detect and mount it.  Strictly speaking, it's not
 * removable media -- but it's handled the same way.
 */
#ifdef FEATURE_EFS_FTL
#  define FEATURE_EFS_HOTPLUG_FTL
#endif

/*
 * This enables a virtual "null device" for Hotplug testing.  It
 * should only be enabled in engineering builds, as it serves no
 * useful purpose outside of test.
 */
#undef FEATURE_EFS_HOTPLUG_DEVNULL

/*
 * For all removable media, the target has two options:
 * COLDPLUG and HOTPLUG.  These are mutually exclusive.
 *
 * FEATURE_EFS_HOTPLUG enables a task that will poll the available
 * media slots in the background and mount any FAT media that is
 * inserted.  This is required for any kind of removable media that
 * needs dynamic detection.  Most targets want FEATURE_EFS_HOTPLUG.
 *
 * FEATURE_EFS_COLDPLUG performs this scan only once at startup, mount
 * any found media, and does not execute a background task to monitor
 * media.  It is expected that the media is static and will not be
 * inserted or removed.  Targets that have only FTL (and no removable
 * devices) may use this feature to save memory.
 */
#define FEATURE_EFS_HOTPLUG
#undef  FEATURE_EFS_COLDPLUG


/*
 * All removable devices use a Windows(tm)-Compatible FAT filesystem.
 * AMSS currently includes two FAT implementation libraries, Sandisk
 * SFAT and "HFAT".
 *
 * SFAT supports only FAT16 devices, for up to 2GB of storage.
 * HFAT supports both FAT16 and FAT32 for larger devices.
 *
 * One (and only one) of these features is required in builds that
 * support FAT media.   HFAT is the preferred choice in all builds.
 */
#define FEATURE_EFS_EXTFS_HFAT
#undef  FEATURE_EFS_EXTFS_SFAT

/*
 * Configure the HFAT's data write cache size to max capacity of
 * SDCC write. Older controllers could not write 128 sectors on
 * single invokation. So if this is not defined it will be configured
 * as 64 sectors, but if the target is new SDCC controller, then
 * those targets can define this as 128 sectors to take advantage
 * of better throughputs with higer number.
 *
 * SDCC 1.x (older controller) and/or SDCC's 3.x VU's:
 *    Do not define. So it defaults to 64 sectors.
 *
 * SDCC 2.x (newer controllers) and/or SDCC's 4.x VU's:
 *    Set to 128 sectors.
 *
 * */
#define HFAT_WR_DATACACHE_SIZE   128

/*
 * Configure the maximum number of volumes or drives HFAT can allocate.
 * Targets have a fix number of storage devices that can be connected
 * and mounted. Some storage devices can be FTL, SD cards and USB drives.
 *
 * This number should be set as low to reduce HFAT RAM footprint and
 * high enough to allow all possible storage devices to be connected
 * at the same time.
 *
 */
#define FS_HFAT_MAXVOLUME_COUNT    3

/*
 * By default, the EXTFS/HFAT code assumes that DOS short filenames
 * are written using Windows OEM Codepage 437, and Unicode conversion
 * tables are included for that codepage only.
 *
 * The conversion routines are selected by defining
 * EFS_DEFAULT_UNICODE_CONVERSION.  If left undefined, the conversion
 * code will default to UTF8 and Codepage 437, which is appropriate
 * for most targets.
 */
//#define EFS_DEFAULT_UNICODE_CONVERSION fs_convert_utf8

/*
 * Defining this feature will also include conversion tables for
 * translating Japanese Shift-JIS Codepage 932 into UTF-8.
 * ('fs_convert_utf8_sjis').
 *
 * Note that the S-JIS conversion tables are not insignificant in ROM
 * size.
 *
 * When this translation is used, certain Codepage 437 (Western)
 * characters in short filenames may be misrepresented or translated
 * as invalid, since they would be processed as Shift-JIS.
 *
 * This feature should only be enabled if Japanese FAT media is to be
 * supported.  Note that AMSS will never create Shift-JIS filenames,
 * since long (Unicode) filenames are preferred.  This conversion is
 * used when reading pre-existing short filename media only.
 *
 * By default, enabling this feature will also change the default
 * conversion calls to use CP932 instead of CP437 by defining
 * EFS_DEFAULT_UNICODE_CONVERSION to be 'fs_convert_utf8_sjis'.  (The
 * target may choose to override this default.)
 */
#define FEATURE_EFS_CONVERT_SHIFT_JIS
//#define EFS_DEFAULT_UNICODE_CONVERSION fs_convert_utf8_sjis

/*
 * The FAT libraries are handled through the EFS2 "external filesystem
 * (extfs)" support layer.  If either FAT library is enabled, then
 * extfs must also be enabled
 */
#if defined FEATURE_EXT_EXTFS_HFAT || defined FEATURE_EXT_EXTFS_SFAT
#  define FEATURE_EFS_EXTFS
#endif


/*
 * This #define can be used to configure the maximum number of files
 * that can be kept opened simultaneously. Please note that this number
 * is shared by all the files irrespective of their file-system type.
 * For those targets that doesn't define this count, a default value
 * of 128 is assumed.
 */
#define EFS_MAX_FILE_DESCRIPTORS      64
#undef EFS_MAX_FILE_DESCRIPTORS


/* These obsolete features must never be defined */
#undef FEATURE_EFS_SFAT


/*
These features still need documentation:

FEATURE_EFS_EFS2_ON_NAND
FEATURE_EFS_EFS2_ON_NOR
FEATURE_EFS_PROBE_NAND
FEATURE_EFS_ROOT_RAMDISK
FEATURE_EFS_SUPPORT_LARGE_NOR
FEATURE_FS_INTEGRATED_DRIVERS
FEATURE_FS_REMOTE_APPS_PROC

*/

#endif /* not __CUSTEFS_H__ */
